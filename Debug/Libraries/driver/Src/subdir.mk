################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/driver/Src/adc.c \
../Libraries/driver/Src/gpio.c \
../Libraries/driver/Src/i2c.c \
../Libraries/driver/Src/pwm.c \
../Libraries/driver/Src/ssd1306.c 

OBJS += \
./Libraries/driver/Src/adc.o \
./Libraries/driver/Src/gpio.o \
./Libraries/driver/Src/i2c.o \
./Libraries/driver/Src/pwm.o \
./Libraries/driver/Src/ssd1306.o 

C_DEPS += \
./Libraries/driver/Src/adc.d \
./Libraries/driver/Src/gpio.d \
./Libraries/driver/Src/i2c.d \
./Libraries/driver/Src/pwm.d \
./Libraries/driver/Src/ssd1306.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/driver/Src/%.o Libraries/driver/Src/%.su Libraries/driver/Src/%.cyclo: ../Libraries/driver/Src/%.c Libraries/driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Libraries/app/Inc -I../Libraries/driver/Inc -I../Libraries/control/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Libraries-2f-driver-2f-Src

clean-Libraries-2f-driver-2f-Src:
	-$(RM) ./Libraries/driver/Src/adc.cyclo ./Libraries/driver/Src/adc.d ./Libraries/driver/Src/adc.o ./Libraries/driver/Src/adc.su ./Libraries/driver/Src/gpio.cyclo ./Libraries/driver/Src/gpio.d ./Libraries/driver/Src/gpio.o ./Libraries/driver/Src/gpio.su ./Libraries/driver/Src/i2c.cyclo ./Libraries/driver/Src/i2c.d ./Libraries/driver/Src/i2c.o ./Libraries/driver/Src/i2c.su ./Libraries/driver/Src/pwm.cyclo ./Libraries/driver/Src/pwm.d ./Libraries/driver/Src/pwm.o ./Libraries/driver/Src/pwm.su ./Libraries/driver/Src/ssd1306.cyclo ./Libraries/driver/Src/ssd1306.d ./Libraries/driver/Src/ssd1306.o ./Libraries/driver/Src/ssd1306.su

.PHONY: clean-Libraries-2f-driver-2f-Src

