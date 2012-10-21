compile = g++ -Wall -g -c -O0 -D TESTING -I/home/jack/workspace/avr/rfm_edf_ecomanager
link = g++ -Wall -g -O0 -D TESTING 

#-fno-elide-constructors
# -I/usr/lib/avr/include/ -D UBRRH -I/usr/lib/avr/include/avr/pgmspace.h -I/usr/share/arduino/hardware/arduino/cores/arduino/ -I/usr/share/arduino/hardware/arduino/variants/standard/

all: CcArray_test RollingAv_test Packet_test

Packet_test: Packet_test.o Packet.o FakeArduino.o
	$(link) Packet_test.o Packet.o FakeArduino.o -lboost_unit_test_framework -o Packet_test && ./Packet_test

RollingAv_test: RollingAv.o RollingAv_test.o
	$(link) RollingAv.o RollingAv_test.o -lboost_unit_test_framework -o RollingAv_test && ./RollingAv_test

CcArray_test: CcTx.o CcArray_test.o Packet.o FakeArduino.o RollingAv.o
	$(link) CcTx.o CcArray_test.o Packet.o FakeArduino.o RollingAv.o -lboost_unit_test_framework -o CcArray_test && ./CcArray_test
	
CcTx.o: ../CcTx.cpp ../CcTx.h ../consts.h ../DynamicArray.h ../RollingAv.h ../RollingAv.cpp
	 $(compile) ../CcTx.cpp
	
CcArray_test.o: CcArray_test.cpp ../CcTx.h ../DynamicArray.h ../RollingAv.h ../RollingAv.cpp
	$(compile) CcArray_test.cpp
	
Packet.o: ../Packet.h ../Packet.cpp
	$(compile) ../Packet.cpp
	
FakeArduino.o: FakeArduino.cpp FakeArduino.h
	$(compile) FakeArduino.cpp
	
RollingAv.o: ../RollingAv.h ../RollingAv.cpp
	$(compile) ../RollingAv.cpp	

RollingAv_test.o: RollingAv_test.cpp ../RollingAv.h ../RollingAv.cpp
	$(compile) RollingAv_test.cpp	

Packet_test.o: Packet_test.cpp ../Packet.h ../Packet.cpp
	$(compile) Packet_test.cpp
	
clean:
	rm -rf *.o CcArray_test