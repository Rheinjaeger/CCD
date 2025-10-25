################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/ioLibrary_Driver/Internet/SNTP/sntp.c 

OBJS += \
./Drivers/ioLibrary_Driver/Internet/SNTP/sntp.o 

C_DEPS += \
./Drivers/ioLibrary_Driver/Internet/SNTP/sntp.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ioLibrary_Driver/Internet/SNTP/%.o Drivers/ioLibrary_Driver/Internet/SNTP/%.su Drivers/ioLibrary_Driver/Internet/SNTP/%.cyclo: ../Drivers/ioLibrary_Driver/Internet/SNTP/%.c Drivers/ioLibrary_Driver/Internet/SNTP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L073xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet" -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet/W5500" -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Application" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-ioLibrary_Driver-2f-Internet-2f-SNTP

clean-Drivers-2f-ioLibrary_Driver-2f-Internet-2f-SNTP:
	-$(RM) ./Drivers/ioLibrary_Driver/Internet/SNTP/sntp.cyclo ./Drivers/ioLibrary_Driver/Internet/SNTP/sntp.d ./Drivers/ioLibrary_Driver/Internet/SNTP/sntp.o ./Drivers/ioLibrary_Driver/Internet/SNTP/sntp.su

.PHONY: clean-Drivers-2f-ioLibrary_Driver-2f-Internet-2f-SNTP

