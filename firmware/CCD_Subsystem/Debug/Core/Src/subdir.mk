################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/CCD_Module.c \
../Core/Src/Temperature.c \
../Core/Src/W25Qxx.c \
../Core/Src/bsp_timer.c \
../Core/Src/command_handler.c \
../Core/Src/data_integrity.c \
../Core/Src/main.c \
../Core/Src/stm32l0xx_hal_msp.c \
../Core/Src/stm32l0xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_controller.c \
../Core/Src/system_stm32l0xx.c \
../Core/Src/w5500_port.c 

OBJS += \
./Core/Src/CCD_Module.o \
./Core/Src/Temperature.o \
./Core/Src/W25Qxx.o \
./Core/Src/bsp_timer.o \
./Core/Src/command_handler.o \
./Core/Src/data_integrity.o \
./Core/Src/main.o \
./Core/Src/stm32l0xx_hal_msp.o \
./Core/Src/stm32l0xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_controller.o \
./Core/Src/system_stm32l0xx.o \
./Core/Src/w5500_port.o 

C_DEPS += \
./Core/Src/CCD_Module.d \
./Core/Src/Temperature.d \
./Core/Src/W25Qxx.d \
./Core/Src/bsp_timer.d \
./Core/Src/command_handler.d \
./Core/Src/data_integrity.d \
./Core/Src/main.d \
./Core/Src/stm32l0xx_hal_msp.d \
./Core/Src/stm32l0xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_controller.d \
./Core/Src/system_stm32l0xx.d \
./Core/Src/w5500_port.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L073xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet" -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet/W5500" -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Application" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/CCD_Module.cyclo ./Core/Src/CCD_Module.d ./Core/Src/CCD_Module.o ./Core/Src/CCD_Module.su ./Core/Src/Temperature.cyclo ./Core/Src/Temperature.d ./Core/Src/Temperature.o ./Core/Src/Temperature.su ./Core/Src/W25Qxx.cyclo ./Core/Src/W25Qxx.d ./Core/Src/W25Qxx.o ./Core/Src/W25Qxx.su ./Core/Src/bsp_timer.cyclo ./Core/Src/bsp_timer.d ./Core/Src/bsp_timer.o ./Core/Src/bsp_timer.su ./Core/Src/command_handler.cyclo ./Core/Src/command_handler.d ./Core/Src/command_handler.o ./Core/Src/command_handler.su ./Core/Src/data_integrity.cyclo ./Core/Src/data_integrity.d ./Core/Src/data_integrity.o ./Core/Src/data_integrity.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32l0xx_hal_msp.cyclo ./Core/Src/stm32l0xx_hal_msp.d ./Core/Src/stm32l0xx_hal_msp.o ./Core/Src/stm32l0xx_hal_msp.su ./Core/Src/stm32l0xx_it.cyclo ./Core/Src/stm32l0xx_it.d ./Core/Src/stm32l0xx_it.o ./Core/Src/stm32l0xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_controller.cyclo ./Core/Src/system_controller.d ./Core/Src/system_controller.o ./Core/Src/system_controller.su ./Core/Src/system_stm32l0xx.cyclo ./Core/Src/system_stm32l0xx.d ./Core/Src/system_stm32l0xx.o ./Core/Src/system_stm32l0xx.su ./Core/Src/w5500_port.cyclo ./Core/Src/w5500_port.d ./Core/Src/w5500_port.o ./Core/Src/w5500_port.su

.PHONY: clean-Core-2f-Src

