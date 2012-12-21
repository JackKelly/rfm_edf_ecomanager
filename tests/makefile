# DIRECTORIES
nanode_rf_utils_dir = /home/jack/workspace/avr/nanode_rf_utils
rfm_edf_ecomanager_dir = /home/jack/workspace/avr/rfm_edf_ecomanager

# COMPILATION AND LINKING VARIABLES
CXX = g++
CXXFLAGS := -Wall -MMD -g -O0 -D TESTING -I$(rfm_edf_ecomanager_dir) -I$(nanode_rf_utils_dir)

# TARGETS
EXECS = RollingAv_test CcArray_test RxPacketFromSensor_test

# RULES FOR all
all: $(EXECS)

# DEPENDENCIES FOR LINKING STEP
RollingAv_test: ../RollingAv.o RollingAv_test.o
CcArray_test: ../CcTx.o CcArray_test.o $(nanode_rf_utils_dir)/tests/FakeArduino.o ../RollingAv.o
RxPacketFromSensor_test: ../RxPacketFromSensor.o RxPacketFromSensor_test.o $(nanode_rf_utils_dir)/tests/FakeArduino.o

# LINKING STEP:
$(EXECS):
	${CXX} $^ -lboost_unit_test_framework -o $@ && ./$@

# INCLUDE COMPILATION DEPENDENCIES
-include *.d
-include ../*.d

# Clean
clean:
	rm -rf *.o *_test *.d ../*.o ../*.d