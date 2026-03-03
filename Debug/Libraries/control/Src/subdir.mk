################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/control/Src/pi.c 

OBJS += \
./Libraries/control/Src/pi.o 

C_DEPS += \
./Libraries/control/Src/pi.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/control/Src/%.o Libraries/control/Src/%.su Libraries/control/Src/%.cyclo: ../Libraries/control/Src/%.c Libraries/control/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Libraries/app/Inc -I../Libraries/driver/Inc -I../Libraries/control/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Libraries-2f-control-2f-Src

clean-Libraries-2f-control-2f-Src:
	-$(RM) ./Libraries/control/Src/pi.cyclo ./Libraries/control/Src/pi.d ./Libraries/control/Src/pi.o ./Libraries/control/Src/pi.su

.PHONY: clean-Libraries-2f-control-2f-Src

