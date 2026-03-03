################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/app/Src/display.c \
../Libraries/app/Src/fault.c \
../Libraries/app/Src/mode.c \
../Libraries/app/Src/motor.c 

OBJS += \
./Libraries/app/Src/display.o \
./Libraries/app/Src/fault.o \
./Libraries/app/Src/mode.o \
./Libraries/app/Src/motor.o 

C_DEPS += \
./Libraries/app/Src/display.d \
./Libraries/app/Src/fault.d \
./Libraries/app/Src/mode.d \
./Libraries/app/Src/motor.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/app/Src/%.o Libraries/app/Src/%.su Libraries/app/Src/%.cyclo: ../Libraries/app/Src/%.c Libraries/app/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Libraries/app/Inc -I../Libraries/driver/Inc -I../Libraries/control/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Libraries-2f-app-2f-Src

clean-Libraries-2f-app-2f-Src:
	-$(RM) ./Libraries/app/Src/display.cyclo ./Libraries/app/Src/display.d ./Libraries/app/Src/display.o ./Libraries/app/Src/display.su ./Libraries/app/Src/fault.cyclo ./Libraries/app/Src/fault.d ./Libraries/app/Src/fault.o ./Libraries/app/Src/fault.su ./Libraries/app/Src/mode.cyclo ./Libraries/app/Src/mode.d ./Libraries/app/Src/mode.o ./Libraries/app/Src/mode.su ./Libraries/app/Src/motor.cyclo ./Libraries/app/Src/motor.d ./Libraries/app/Src/motor.o ./Libraries/app/Src/motor.su

.PHONY: clean-Libraries-2f-app-2f-Src

