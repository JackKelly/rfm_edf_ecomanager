all: DynamicArray_test

DynamicArray_test: ../DynamicArray.h DynamicArray_test.cpp
	g++ -Wall -g -O0 DynamicArray_test.cpp FakeArduino.cpp -lboost_unit_test_framework -o DynamicArray_test