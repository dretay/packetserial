obj = PacketDAO_wrap.c _PacketDAO.so

.PHONY: all
all : $(obj)

clean:
	rm -f *.o
	rm -f *.so
	rm -f *wrap*.c*
	rm -Rf build

PacketDAO_wrap.c : PacketDAO.i
	swig -python PacketDAO.i

_PacketDAO.so: PacketDAO.i
	python setup.py build_ext --inplace

test:
	python -c "from _PacketDAO import hello; hello()"
