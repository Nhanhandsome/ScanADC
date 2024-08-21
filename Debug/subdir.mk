################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ADS1256.c \
../crc.c \
../gpio_if.c \
../ringbuffer.c \
../spi_if.c 

C_DEPS += \
./ADS1256.d \
./crc.d \
./gpio_if.d \
./ringbuffer.d \
./spi_if.d 

OBJS += \
./ADS1256.o \
./crc.o \
./gpio_if.o \
./ringbuffer.o \
./spi_if.o 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"C:\Users\MinhNhan\Desktop\ADCScaner" -Os -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean--2e-

clean--2e-:
	-$(RM) ./ADS1256.d ./ADS1256.o ./crc.d ./crc.o ./gpio_if.d ./gpio_if.o ./ringbuffer.d ./ringbuffer.o ./spi_if.d ./spi_if.o

.PHONY: clean--2e-

