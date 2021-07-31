################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_gpio.c \
../drivers/fsl_lptmr.c \
../drivers/fsl_lpuart.c \
../drivers/fsl_pit.c \
../drivers/fsl_rtc.c \
../drivers/fsl_smc.c 

OBJS += \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_gpio.o \
./drivers/fsl_lptmr.o \
./drivers/fsl_lpuart.o \
./drivers/fsl_pit.o \
./drivers/fsl_rtc.o \
./drivers/fsl_smc.o 

C_DEPS += \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_gpio.d \
./drivers/fsl_lptmr.d \
./drivers/fsl_lpuart.d \
./drivers/fsl_pit.d \
./drivers/fsl_rtc.d \
./drivers/fsl_smc.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_MKL82Z128VLH7 -DCPU_MKL82Z128VLH7_cm0plus -DCPU_MKL82Z128VLK7 -DFRDM_KL82Z -DFREEDOM -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I../board -I../source -I../ -I../drivers -I../device -I../CMSIS -I../utilities -I../component/serial_manager -I../component/uart -I../component/lists -O0 -fno-common -g3 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


