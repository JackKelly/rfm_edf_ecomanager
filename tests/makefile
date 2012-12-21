nanode_rf_utils_dir = /home/jack/workspace/avr/nanode_rf_utils
compile = g++ -Wall -MMD -g -c -O0 -D TESTING -I/home/jack/workspace/avr/rfm_edf_ecomanager -I$(nanode_rf_utils_dir) $<
link = g++ -Wall -g -O0 -D TESTING $^ -lboost_unit_test_framework -o $@ && ./$@ 

#-fno-elide-constructors
# -I/usr/lib/avr/include/ -D UBRRH -I/usr/lib/avr/include/avr/pgmspace.h -I/usr/share/arduino/hardware/arduino/cores/arduino/ -I/usr/share/arduino/hardware/arduino/variants/standard/

all: CcArray_test RollingAv_test RxPacketFromSensor_test

RollingAv_test: RollingAv.o RollingAv_test.o
	$(link)

CcArray_test: CcTx.o CcArray_test.o $(nanode_rf_utils_dir)/tests/FakeArduino.o RollingAv.o
	$(link)
	
RxPacketFromSensor_test: RxPacketFromSensor.o RxPacketFromSensor_test.o $(nanode_rf_utils_dir)/tests/FakeArduino.o
	$(link)
	
CcTx.o: ../CcTx.cpp ../CcTx.h ../consts.h ../DynamicArray.h ../RollingAv.h ../RollingAv.cpp
	 $(compile)
	
CcArray_test.o: CcArray_test.cpp ../CcTx.h ../DynamicArray.h ../RollingAv.h ../RollingAv.cpp
	$(compile)
	
RollingAv.o: ../RollingAv.cpp ../RollingAv.h
	$(compile)

RollingAv_test.o: RollingAv_test.cpp ../RollingAv.h ../RollingAv.cpp
	$(compile)
	
RxPacketFromSensor.o: ../RxPacketFromSensor.cpp ../RxPacketFromSensor.h
	$(compile)

RxPacketFromSensor_test.o: RxPacketFromSensor_test.cpp ../RxPacketFromSensor.cpp ../RxPacketFromSensor.h
	$(compile)
	
clean:
	rm -rf *.o *_test *.d