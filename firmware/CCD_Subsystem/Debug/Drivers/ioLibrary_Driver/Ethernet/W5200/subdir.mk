################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.c 

OBJS += \
./Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.o 

C_DEPS += \
./Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ioLibrary_Driver/Ethernet/W5200/%.o Drivers/ioLibrary_Driver/Ethernet/W5200/%.su Drivers/ioLibrary_Driver/Ethernet/W5200/%.cyclo: ../Drivers/ioLibrary_Driver/Ethernet/W5200/%.c Drivers/ioLibrary_Driver/Ethernet/W5200/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L073xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Shrek/OneDrive/Documents/Spring_2025/ECE 4805 (Senior Design)/CCD/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet" -I"C:/Users/Shrek/OneDrive/Documents/Spring_2025/ECE 4805 (Senior Design)/CCD/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet/W5500" -I"C:/Users/Shrek/OneDrive/Documents/Spring_2025/ECE 4805 (Senior Design)/CCD/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Application" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-ioLibrary_Driver-2f-Ethernet-2f-W5200

clean-Drivers-2f-ioLibrary_Driver-2f-Ethernet-2f-W5200:
	-$(RM) ./Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.cyclo ./Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.d ./Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.o ./Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.su

.PHONY: clean-Drivers-2f-ioLibrary_Driver-2f-Ethernet-2f-W5200

