################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/ioLibrary_Driver/Ethernet/W5500/w5500.c 

OBJS += \
./Drivers/ioLibrary_Driver/Ethernet/W5500/w5500.o 

C_DEPS += \
./Drivers/ioLibrary_Driver/Ethernet/W5500/w5500.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ioLibrary_Driver/Ethernet/W5500/%.o Drivers/ioLibrary_Driver/Ethernet/W5500/%.su Drivers/ioLibrary_Driver/Ethernet/W5500/%.cyclo: ../Drivers/ioLibrary_Driver/Ethernet/W5500/%.c Drivers/ioLibrary_Driver/Ethernet/W5500/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -D_WIZCHIP_=5500 -DUSE_HAL_DRIVER -DSTM32L073xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Shrek/OneDrive/Documents/Fall_2025/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet" -I"C:/Users/Shrek/OneDrive/Documents/Fall_2025/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet/W5500" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-ioLibrary_Driver-2f-Ethernet-2f-W5500

clean-Drivers-2f-ioLibrary_Driver-2f-Ethernet-2f-W5500:
	-$(RM) ./Drivers/ioLibrary_Driver/Ethernet/W5500/w5500.cyclo ./Drivers/ioLibrary_Driver/Ethernet/W5500/w5500.d ./Drivers/ioLibrary_Driver/Ethernet/W5500/w5500.o ./Drivers/ioLibrary_Driver/Ethernet/W5500/w5500.su

.PHONY: clean-Drivers-2f-ioLibrary_Driver-2f-Ethernet-2f-W5500

