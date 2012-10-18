compile = g++ -Wall -g -c -O0 -D TESTING -I/home/jack/workspace/avr/rfm_edf_ecomanager
link = g++ -Wall -g -O0 -D TESTING 

#-fno-elide-constructors
# -I/usr/lib/avr/include/ -D UBRRH -I/usr/lib/avr/include/avr/pgmspace.h -I/usr/share/arduino/hardware/arduino/cores/arduino/ -I/usr/share/arduino/hardware/arduino/variants/standard/

all: CcArray_test

DynamicArray_test: ../DynamicArray.h DynamicArray_test.cpp
	$(link) DynamicArray_test.cpp FakeArduino.cpp -lboost_unit_test_framework -o DynamicArray_test
	
CcArray_test: CcTx.o CcArray_test.o Packet.o FakeArduino.o
	$(link) CcTx.o CcArray_test.o Packet.o FakeArduino.o -lboost_unit_test_framework -o CcArray_test && ./CcArray_test
	
CcTx.o: ../CcTx.cpp ../CcTx.h ../consts.h ../DynamicArray.h
	 $(compile) ../CcTx.cpp
	
CcArray_test.o: CcArray_test.cpp ../CcTx.h
	$(compile) CcArray_test.cpp
	
Packet.o: ../Packet.h ../Packet.cpp
	$(compile) ../Packet.cpp
	
FakeArduino.o: FakeArduino.cpp FakeArduino.h
	$(compile) FakeArduino.cpp
	
clean:
	rm -rf *.o CcArray_test