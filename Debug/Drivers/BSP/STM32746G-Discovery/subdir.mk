################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.c \
../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.c \
../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sdram.c \
../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.c 

OBJS += \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.o \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.o \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sdram.o \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.o 

C_DEPS += \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.d \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.d \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sdram.d \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32746G-Discovery/%.o: ../Drivers/BSP/STM32746G-Discovery/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F746xx -I"/home/raphael/stm32/projects/magnetometer2/Inc" -I"/home/raphael/stm32/projects/magnetometer2/Drivers/STM32F7xx_HAL_Driver/Inc" -I"/home/raphael/stm32/projects/magnetometer2/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"/home/raphael/stm32/projects/magnetometer2/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"/home/raphael/stm32/projects/magnetometer2/Drivers/CMSIS/Include" -I"/home/raphael/stm32/projects/magnetometer2/Drivers/BSP" -I"/home/raphael/stm32/projects/magnetometer2/Drivers/BSP/Components" -I"/home/raphael/stm32/projects/magnetometer2/Drivers/BSP/STM32746G-Discovery" -I"/home/raphael/stm32/projects/magnetometer2/Drivers/BSP/Components/Common" -I"/home/raphael/stm32/projects/magnetometer2/Drivers/BSP/Components/rk043fn48h" -I"/home/raphael/stm32/projects/magnetometer2/Drivers/BSP/Components/ft5336"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


