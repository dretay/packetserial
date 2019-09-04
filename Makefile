ifeq ($(OS),Windows_NT)
  ifeq ($(shell uname -s),) # not in a bash-like shell
	CLEANUP = del /F /Q
	MKDIR = mkdir
  else # in a bash-like shell, like msys
	CLEANUP = rm -f
	MKDIR = mkdir -p
  endif
	TARGET_EXTENSION=.exe
else
	CLEANUP = rm -f
	MKDIR = mkdir -p
	TARGET_EXTENSION=out
endif

#directories
BUILD_DIR = build/
PROFILING_RESULTS_DIR = $(BUILD_DIR)profiling_results/
CPPCHECK_RESULTS_DIR = $(BUILD_DIR)cppcheck_results/
TEST_RESULTS_DIR = $(BUILD_DIR)test_results/
TEST_OUTPUT = $(BUILD_DIR)test/
CONFIG_DIR = conf/
SRC_DIRS = src/
LIB_DIRS = lib/
TEST_DIRS = test/
TEST_RUNNERS = $(TEST_DIRS)test_runners/

#protobuf files
SRCPB = $(wildcard $(SRC_DIRS)*.proto)
PBMODELS = $(patsubst $(SRC_DIRS)%.proto,$(SRC_DIRS)%.pb.c,$(SRCPB) )
PROTOC = /home/drew/src/nanopb-0.3.9.3/generator-bin/protoc
PB_OBJS = $(patsubst $(SRC_DIRS)%.proto,$(BUILD_DIR)$(SRC_DIRS)%.pb.c.o,$(SRCPB) )

#unity testing files
SRCT = $(wildcard $(TEST_DIRS)*.c)
RUNNERS = $(patsubst $(TEST_DIRS)%.c,$(TEST_RUNNERS)%.c,$(SRCT) )
TEST_RESULTS = $(patsubst $(TEST_DIRS)Test%.c,$(TEST_RESULTS_DIR)Test%.txt,$(SRCT) )
PROFILING_RESULTS = $(patsubst $(TEST_DIRS)Test%.c,$(PROFILING_RESULTS_DIR)Test%.out,$(SRCT) )
TEST_OBJS = $(SRCT:%=$(BUILD_DIR)%.o)
UNITY_ROOT=/home/drew/src/Unity

#valgrind stuff
VALGRIND = /usr/bin/valgrind
VALGRIND_SUPPS = $(CONFIG_DIR)valgrind.memcheck.supp

#project source files
SRCS := $(shell find $(LIB_DIRS) $(SRC_DIRS) -maxdepth 2 \( -iname "*.c" ! -iname "*.pb.c" \))
OBJS = $(SRCS:%=$(BUILD_DIR)%.o) $(PB_OBJS)
INC_DIRS := $(shell find $(LIB_DIRS) -maxdepth 1 -type d)

#cppcheck
CPPCHECK = cppcheck
CPPCHECK_FILES := $(shell find $(SRC_DIRS) -maxdepth 2 \( -iname "*.c" ! -iname "*.pb.c" \))
CPPCHECK_FLAGS = -q --enable=all --inconclusive --suppress=missingIncludeSystem
CPPCHECK_RESULTS = $(CPPCHECK_FILES:%=$(CPPCHECK_RESULTS_DIR)%.txt)

#misc variables
DIRECTIVES = -DPB_FIELD_16BIT -DLOG_USE_COLOR -DUNITY_OUTPUT_COLOR
FLAGS = -fPIC
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) -I$(UNITY_ROOT)/src -I./src
CURRENT_DIR = $(notdir $(shell pwd))

#various build flags
DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS = $(INC_FLAGS) $(FLAGS) $(DIRECTIVES) -g3
else
	CFLAGS = $(INC_FLAGS) $(FLAGS) $(DIRECTIVES) -g0
endif

PLATFORM ?= LINUX
ifeq ($(PLATFORM), ARM)
	CC = arm-none-eabi-gcc
	LD = arm-none-eabi-gcc
else
	CC = gcc
	LD = gcc
endif

.PHONY: all
.PHONY: sharedobject
.PHONY: test
.PHONY: profile
.PHONY: jupyter
.PHONY: pythondeps
.PHONY: clean
.PHONY: cppcheck

all: $(PBMODELS) $(RUNNERS) $(OBJS) cppcheck
sharedobject: all $(BUILD_DIR)$(CURRENT_DIR).so

test: all $(TEST_OBJS) $(TEST_RESULTS) $(CPPCHECK_RESULTS)
	@echo ""
	@echo "-----------------------ANALYSIS AND TESTING SUMMARY-----------------------"
	@echo `find $(TEST_RESULTS_DIR) -type f -exec grep IGNORE {} \;|wc -l` "tests ignored"
	@echo "`find $(TEST_RESULTS_DIR) -type f -exec grep IGNORE {} \;`"
	@echo `find $(TEST_RESULTS_DIR) -type f -exec grep FAIL {} \;|wc -l` "tests failed"
	@echo "`find $(TEST_RESULTS_DIR) -type f -exec grep FAIL {} \;`"
	@echo `find $(TEST_RESULTS_DIR) -type f -exec grep PASS {} \;|wc -l` "tests passed"
	@echo ""
	@echo "`grep -Poh 'ERROR SUMMARY:\K ([0-9]+)' $(TEST_RESULTS_DIR)*| awk '{ SUM += $$1} END { print SUM }'` memory leak(s) detected"
	@echo ""
	@echo `find $(CPPCHECK_RESULTS_DIR) -type f -exec grep warning {} \;|wc -l` "code warnings"
	@echo `find $(CPPCHECK_RESULTS_DIR) -type f -exec grep warning {} \;`
	@echo `find $(CPPCHECK_RESULTS_DIR) -type f -exec grep error {} \;|wc -l` "code errors"
	@echo "`find $(CPPCHECK_RESULTS_DIR) -type f -exec grep error {} \;`"

profile: all $(PROFILING_RESULTS)

#link objects into an so to be included elsewhere
$(BUILD_DIR)$(CURRENT_DIR).so: $(OBJS)
	$(LD) $(OBJS) -shared -o $@

#generate profiling data
$(PROFILING_RESULTS_DIR)%.out: $(BUILD_DIR)%.c.o.$(TARGET_EXTENSION)
	$(MKDIR) $(dir $@)
	-$(VALGRIND) --tool=callgrind --callgrind-out-file=$@  $< > /dev/null 2>&1

#execute tests
$(TEST_RESULTS_DIR)%.txt: $(BUILD_DIR)%.c.o.$(TARGET_EXTENSION)
	$(MKDIR) $(dir $@)
	-$(VALGRIND) --suppressions=$(VALGRIND_SUPPS) --gen-suppressions=all --tool=memcheck --leak-check=full $< > $@ 2>&1

#build the test runners
$(BUILD_DIR)%.c.o.$(TARGET_EXTENSION): $(TEST_OUTPUT)%.c.o
	$(CC) -g -o $@ $^ $(CFLAGS) $(OBJS) $(UNITY_ROOT)/src/unity.c $(TEST_RUNNERS)$(basename $(notdir $<))

# assembly
$(BUILD_DIR)%.s.o: %.s
	$(MKDIR) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

#execute cppcheck
$(CPPCHECK_RESULTS_DIR)%.c.txt: %.c
	$(MKDIR) $(dir $@)
	$(CPPCHECK) $(INC_FLAGS) $(DIRECTIVES) $(CPPCHECK_FLAGS) $< > $@ 2>&1

# c source
$(BUILD_DIR)%.c.o: %.c
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# protocol buffer models
$(SRC_DIRS)%.pb.c:: $(SRC_DIRS)%.proto
	$(PROTOC) --nanopb_out=. $<

#unity test runners
$(TEST_RUNNERS)%.c:: $(TEST_DIRS)%.c
	$(MKDIR) $(dir $@)
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb $< $@

jupyter: all pythondeps
	( \
		cd ./python_venv; \
		jupyter notebook; \
	)

pythondeps:
	( \
		. ./python_venv/bin/activate; \
		pip install -r ./python_venv/requirements.txt; \
	)

clean:
	$(CLEANUP) $(OBJS) $(TEST_OBJS)	$(TEST_RESULTS) $(CPPCHECK_RESULTS) $(BUILD_DIR)*.out $(SRC_DIRS)*.pb.*

.PRECIOUS: $(TEST_RESULTS_DIR)%.txt
.PRECIOUS: $(PROFILING_RESULTS_DIR)%.txt
.PRECIOUS: $(BUILD_DIR)%.c.o.out
.PRECIOUS: $(PBMODELS)