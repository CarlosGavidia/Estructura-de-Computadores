################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../D8Led.c \
../gpio.c \
../intcontroller.c \
../leds.c \
../main.c \
../timer.c \
../utils.c 

ASM_SRCS += \
../init.asm 

OBJS += \
./D8Led.o \
./gpio.o \
./init.o \
./intcontroller.o \
./leds.o \
./main.o \
./timer.o \
./utils.o 

C_DEPS += \
./D8Led.d \
./gpio.d \
./intcontroller.d \
./leds.d \
./main.d \
./timer.d \
./utils.d 

ASM_DEPS += \
./init.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -Os -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mapcs-frame -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC Assembler'
	arm-none-eabi-gcc -x assembler-with-cpp -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


