################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/ioLibrary_Driver/Internet/MQTT/MQTTClient.c \
../Drivers/ioLibrary_Driver/Internet/MQTT/mqtt_interface.c 

OBJS += \
./Drivers/ioLibrary_Driver/Internet/MQTT/MQTTClient.o \
./Drivers/ioLibrary_Driver/Internet/MQTT/mqtt_interface.o 

C_DEPS += \
./Drivers/ioLibrary_Driver/Internet/MQTT/MQTTClient.d \
./Drivers/ioLibrary_Driver/Internet/MQTT/mqtt_interface.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ioLibrary_Driver/Internet/MQTT/%.o Drivers/ioLibrary_Driver/Internet/MQTT/%.su Drivers/ioLibrary_Driver/Internet/MQTT/%.cyclo: ../Drivers/ioLibrary_Driver/Internet/MQTT/%.c Drivers/ioLibrary_Driver/Internet/MQTT/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L073xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet" -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet/W5500" -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Application" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-ioLibrary_Driver-2f-Internet-2f-MQTT

clean-Drivers-2f-ioLibrary_Driver-2f-Internet-2f-MQTT:
	-$(RM) ./Drivers/ioLibrary_Driver/Internet/MQTT/MQTTClient.cyclo ./Drivers/ioLibrary_Driver/Internet/MQTT/MQTTClient.d ./Drivers/ioLibrary_Driver/Internet/MQTT/MQTTClient.o ./Drivers/ioLibrary_Driver/Internet/MQTT/MQTTClient.su ./Drivers/ioLibrary_Driver/Internet/MQTT/mqtt_interface.cyclo ./Drivers/ioLibrary_Driver/Internet/MQTT/mqtt_interface.d ./Drivers/ioLibrary_Driver/Internet/MQTT/mqtt_interface.o ./Drivers/ioLibrary_Driver/Internet/MQTT/mqtt_interface.su

.PHONY: clean-Drivers-2f-ioLibrary_Driver-2f-Internet-2f-MQTT

