################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Manager.cpp \
../Packet.cpp \
../Rfm12b.cpp \
../rfm_edf_ecomanager.cpp \
../spi.cpp 

OBJS += \
./Manager.o \
./Packet.o \
./Rfm12b.o \
./rfm_edf_ecomanager.o \
./spi.o 

CPP_DEPS += \
./Manager.d \
./Packet.d \
./Rfm12b.d \
./rfm_edf_ecomanager.d \
./spi.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I/usr/share/arduino/hardware/arduino/cores/arduino -I/usr/share/arduino/hardware/arduino/variants/standard/ -I"/home/jack/workspaces/avr/rfm_edf_ecomanager" -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


