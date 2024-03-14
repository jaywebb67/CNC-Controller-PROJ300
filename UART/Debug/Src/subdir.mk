################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/FPU.c \
../Src/Gcode.c \
../Src/UART.c \
../Src/clock_config.c \
../Src/main.c \
../Src/steppers.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/systick_delay.c 

OBJS += \
./Src/FPU.o \
./Src/Gcode.o \
./Src/UART.o \
./Src/clock_config.o \
./Src/main.o \
./Src/steppers.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/systick_delay.o 

C_DEPS += \
./Src/FPU.d \
./Src/Gcode.d \
./Src/UART.d \
./Src/clock_config.d \
./Src/main.d \
./Src/steppers.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/systick_delay.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F429ZITx -DSTM32F4 -DNUCLEO_F429ZI -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/FPU.cyclo ./Src/FPU.d ./Src/FPU.o ./Src/FPU.su ./Src/Gcode.cyclo ./Src/Gcode.d ./Src/Gcode.o ./Src/Gcode.su ./Src/UART.cyclo ./Src/UART.d ./Src/UART.o ./Src/UART.su ./Src/clock_config.cyclo ./Src/clock_config.d ./Src/clock_config.o ./Src/clock_config.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/steppers.cyclo ./Src/steppers.d ./Src/steppers.o ./Src/steppers.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/systick_delay.cyclo ./Src/systick_delay.d ./Src/systick_delay.o ./Src/systick_delay.su

.PHONY: clean-Src

