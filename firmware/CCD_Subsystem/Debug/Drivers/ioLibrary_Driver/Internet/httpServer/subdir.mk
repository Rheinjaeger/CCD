################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/ioLibrary_Driver/Internet/httpServer/httpParser.c \
../Drivers/ioLibrary_Driver/Internet/httpServer/httpServer.c \
../Drivers/ioLibrary_Driver/Internet/httpServer/httpUtil.c 

OBJS += \
./Drivers/ioLibrary_Driver/Internet/httpServer/httpParser.o \
./Drivers/ioLibrary_Driver/Internet/httpServer/httpServer.o \
./Drivers/ioLibrary_Driver/Internet/httpServer/httpUtil.o 

C_DEPS += \
./Drivers/ioLibrary_Driver/Internet/httpServer/httpParser.d \
./Drivers/ioLibrary_Driver/Internet/httpServer/httpServer.d \
./Drivers/ioLibrary_Driver/Internet/httpServer/httpUtil.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ioLibrary_Driver/Internet/httpServer/%.o Drivers/ioLibrary_Driver/Internet/httpServer/%.su Drivers/ioLibrary_Driver/Internet/httpServer/%.cyclo: ../Drivers/ioLibrary_Driver/Internet/httpServer/%.c Drivers/ioLibrary_Driver/Internet/httpServer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L073xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet" -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Ethernet/W5500" -I"C:/Users/torre/Documents/Spring_2025/real_ccd/CCD/firmware/CCD_Subsystem/Drivers/ioLibrary_Driver/Application" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-ioLibrary_Driver-2f-Internet-2f-httpServer

clean-Drivers-2f-ioLibrary_Driver-2f-Internet-2f-httpServer:
	-$(RM) ./Drivers/ioLibrary_Driver/Internet/httpServer/httpParser.cyclo ./Drivers/ioLibrary_Driver/Internet/httpServer/httpParser.d ./Drivers/ioLibrary_Driver/Internet/httpServer/httpParser.o ./Drivers/ioLibrary_Driver/Internet/httpServer/httpParser.su ./Drivers/ioLibrary_Driver/Internet/httpServer/httpServer.cyclo ./Drivers/ioLibrary_Driver/Internet/httpServer/httpServer.d ./Drivers/ioLibrary_Driver/Internet/httpServer/httpServer.o ./Drivers/ioLibrary_Driver/Internet/httpServer/httpServer.su ./Drivers/ioLibrary_Driver/Internet/httpServer/httpUtil.cyclo ./Drivers/ioLibrary_Driver/Internet/httpServer/httpUtil.d ./Drivers/ioLibrary_Driver/Internet/httpServer/httpUtil.o ./Drivers/ioLibrary_Driver/Internet/httpServer/httpUtil.su

.PHONY: clean-Drivers-2f-ioLibrary_Driver-2f-Internet-2f-httpServer

