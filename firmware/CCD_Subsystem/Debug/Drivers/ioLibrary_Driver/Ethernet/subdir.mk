################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/ioLibrary_Driver/Ethernet/socket.c \
../Drivers/ioLibrary_Driver/Ethernet/wizchip_conf.c 

OBJS += \
./Drivers/ioLibrary_Driver/Ethernet/socket.o \
./Drivers/ioLibrary_Driver/Ethernet/wizchip_conf.o 

C_DEPS += \
./Drivers/ioLibrary_Driver/Ethernet/socket.d \
./Drivers/ioLibrary_Driver/Ethernet/wizchip_conf.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ioLibrary_Driver/Ethernet/%.o Drivers/ioLibrary_Driver/Ethernet/%.su Drivers/ioLibrary_Driver/Ethernet/%.cyclo: ../Drivers/ioLibrary_Driver/Ethernet/%.c Drivers/ioLibrary_Driver/Ethernet/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L073xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet" -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet/W5500" -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Application" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-ioLibrary_Driver-2f-Ethernet

clean-Drivers-2f-ioLibrary_Driver-2f-Ethernet:
	-$(RM) ./Drivers/ioLibrary_Driver/Ethernet/socket.cyclo ./Drivers/ioLibrary_Driver/Ethernet/socket.d ./Drivers/ioLibrary_Driver/Ethernet/socket.o ./Drivers/ioLibrary_Driver/Ethernet/socket.su ./Drivers/ioLibrary_Driver/Ethernet/wizchip_conf.cyclo ./Drivers/ioLibrary_Driver/Ethernet/wizchip_conf.d ./Drivers/ioLibrary_Driver/Ethernet/wizchip_conf.o ./Drivers/ioLibrary_Driver/Ethernet/wizchip_conf.su

.PHONY: clean-Drivers-2f-ioLibrary_Driver-2f-Ethernet

