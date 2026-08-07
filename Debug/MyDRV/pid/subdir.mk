################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MyDRV/pid/pid.c 

OBJS += \
./MyDRV/pid/pid.o 

C_DEPS += \
./MyDRV/pid/pid.d 


# Each subdirectory must supply rules for building sources it contributes
MyDRV/pid/%.o MyDRV/pid/%.su MyDRV/pid/%.cyclo: ../MyDRV/pid/%.c MyDRV/pid/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DSTM32F405xx -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -c -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/scheduler" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/device_drv/ADS1115" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/App/database" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/App" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/App/auto_pulsing_task" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/device_drv/TC1047AVNBTR" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/device_drv/BMP390" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/App/cmd_line_task" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/App/fsp_line_task" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/App/cap_controller_task" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/App/sensor_task" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/fsp" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/device_drv" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/device_drv/H3LIS331DL" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/device_drv/LSM6DSOX" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/spi" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/pid" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/i2c" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/pwm" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/stm_header" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVC_FIRM_V2.1/MyDRV/uart" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MyDRV-2f-pid

clean-MyDRV-2f-pid:
	-$(RM) ./MyDRV/pid/pid.cyclo ./MyDRV/pid/pid.d ./MyDRV/pid/pid.o ./MyDRV/pid/pid.su

.PHONY: clean-MyDRV-2f-pid

