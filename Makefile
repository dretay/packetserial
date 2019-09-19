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
RELEASE_DIR = release/
PROFILING_RESULTS_DIR = $(BUILD_DIR)profiling_results/
CPPCHECK_RESULTS_DIR = $(BUILD_DIR)cppcheck_results/
TEST_RESULTS_DIR = $(BUILD_DIR)test_results/
TEST_OUTPUT = $(BUILD_DIR)test/
CONFIG_DIR = conf/
SRC_DIRS = src/
LIB_DIRS = lib/
TEST_DIRS = test/
TEST_RUNNERS = $(TEST_DIRS)test_runners/
INCLUDE_DEST = $(RELEASE_DIR)include/
SRC_DEST = $(RELEASE_DIR)src/

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
HEADERS = $(shell find $(LIB_DIRS) $(SRC_DIRS) -maxdepth 2 \( -iname "*.h" \))
OBJS = $(SRCS:%=$(BUILD_DIR)%.o) $(PB_OBJS)
INC_DIRS := $(shell find $(LIB_DIRS) -maxdepth 1 -type d)

#cppcheck
CPPCHECK = cppcheck
CPPCHECK_FILES := $(shell find $(SRC_DIRS) -maxdepth 2 \( -iname "*.c" ! -iname "*.pb.c" \))
CPPCHECK_FLAGS = -q --enable=all --inconclusive --suppress=missingIncludeSystem
CPPCHECK_RESULTS = $(CPPCHECK_FILES:%=$(CPPCHECK_RESULTS_DIR)%.txt)

#swig
SWIG = swig
SWIG_DIR = $(BUILD_DIR)swig/
SWIG_SRC = $(wildcard $(SWIG_DIR)*.c)
SRCSWIG = $(wildcard $(SWIG_DIR)*.i)
SWIGWRAPPERS = $(patsubst $(SWIG_DIR)%.i,$(SWIG_DIR)%_wrap.c,$(SRCSWIG) )
SWIG_OBJS = $(patsubst $(SWIG_DIR)%.i,$(BUILD_DIR)$(SWIG_DIR)%_wrap.c.o,$(SRCSWIG) ) build/swig/swig_stuff.c.o
SWIG_FLAGS = -python -I$(SRC_DIRS)

#misc variables
DIRECTIVES = -DPB_FIELD_16BIT -DLOG_USE_COLOR -DUNITY_OUTPUT_COLOR
FLAGS = -fPIC
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) -I$(UNITY_ROOT)/src -I./src
CURRENT_DIR = $(notdir $(shell pwd))
CP = cp
CFLAGS = $(INC_FLAGS) $(FLAGS) $(DIRECTIVES) --std=gnu99
#various build flags

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS +=   -O0 -g3
	LDFLAGS = -shared
else
	CFLAGS +=  -Os
	LDFLAGS = -shared
# 	ARFLAGS = rcs
endif

PLATFORM ?= LINUX
ifeq ($(PLATFORM),$(filter $(PLATFORM),NRF51 STM32F103))
	#set instruction set to thumb... debatably arm might be valid too i guess but I'm too lazy and this code
	#doesn't really have zero wait to worry about...
	CFLAGS += -mthumb
	# set the ABI to aapcs (current standard, needs to be set consistently or linker will explode?)
	CFLAGS += -mabi=aapcs
	# keep every function in separate section. This will allow linker to dump unused functions
#  	CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
	# don't use functions built-into gcc
	CFLAGS += -fno-builtin

	ARFLAGS += --target elf32-littlearm
	CC = arm-none-eabi-gcc
	LD = arm-none-eabi-ld
	AR = arm-none-eabi-ar
endif
ifeq ($(PLATFORM), NRF51)
	#setting this wrong (like m4) means weird stuff like hard faults when an int goes negative...
	CPU	= cortex-m0
	CFLAGS += -mcpu=$(CPU)
	#software floating point
	CFLAGS +=-mfloat-abi=soft
else
	DIRECTIVES += -DLINUX
	CC = gcc
	LD = ld
	AR = ar
endif

.PHONY: all
.PHONY: release
.PHONY: sharedobject
.PHONY: test
.PHONY: profile
.PHONY: jupyter
.PHONY: pythondeps
.PHONY: clean
.PHONY: cppcheck
.PHONY: includes
.PHONY: swig
.PHONY: cheat

# so i've kinda given up on making this one mega static library. it works for individual functions
# but chaining calls in between different objects breaks in a way i don't understand. For now I'm just
# going to copy this over to a folder on my computer and move on with my life... maybe
# the "right" thing to do is to have smaller static libraries that are built up in a larger main project?
cheat:
	$(CLEANUP)r /mnt/windows/include/packetserial
	$(MKDIR) /mnt/windows/include/packetserial
	cp $(HEADERS) /mnt/windows/include/packetserial
	cp $(SRCS) $(PBMODELS) /mnt/windows/include/packetserial

all: $(PBMODELS) $(RUNNERS) $(OBJS) cppcheck

includes: $(PBMODELS)
	$(MKDIR) $(INCLUDE_DEST)
	cp $(HEADERS) $(INCLUDE_DEST)
	$(MKDIR) $(SRC_DEST)
	cp $(SRCS) $(PBMODELS) $(SRC_DEST)

release: all includes $(RELEASE_DIR)lib$(CURRENT_DIR).a
sharedobject: all includes $(RELEASE_DIR)lib$(CURRENT_DIR).so

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

$(RELEASE_DIR)lib$(CURRENT_DIR).a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#link objects into an so to be included elsewhere
$(RELEASE_DIR)lib$(CURRENT_DIR).so: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -shared -o $@

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

#swig function library
$(BUILD_DIR)%.c.o: %.c
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

#swig wrappers
$(SWIG_DIR)%_wrap.c:: $(SWIG_DIR)%.i
	$(SWIG) $(SWIG_FLAGS) $<


swig: sharedobject $(SWIG_OBJS)
	$(LD) -shared $(SWIG_OBJS) $(OBJS) -o $(RELEASE_DIR)$@_wrapper.so

jupyter: swig pythondeps
	( \
		cd ./python_venv; \
		. ./bin/activate; \
		ipython kernel install --user --name=$(CURRENT_DIR); \
		jupyter notebook; \
	)

pythondeps:
	( \
		. ./python_venv/bin/activate; \
		pip install -r ./python_venv/requirements.txt; \
	)

clean:
	$(CLEANUP) $(SRC_DIRS)*.pb.*
	$(CLEANUP)r $(BUILD_DIR)
	$(CLEANUP)r $(RELEASE_DIR)

.PRECIOUS: $(TEST_RESULTS_DIR)%.txt
.PRECIOUS: $(PROFILING_RESULTS_DIR)%.txt
.PRECIOUS: $(BUILD_DIR)%.c.o.out
.PRECIOUS: $(PBMODELS)