################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/packet_manager/subdir.mk
-include source/Supervisor/subdir.mk
-include source/Serial_Manager/subdir.mk
-include source/RadioManager/subdir.mk
-include source/MCU_Drivers/uart/subdir.mk
-include source/MCU_Drivers/spi/subdir.mk
-include source/MCU_Drivers/pit_timer/subdir.mk
-include source/MCU_Drivers/lptmr1_timer/subdir.mk
-include source/MCU_Drivers/adc/subdir.mk
-include source/Health_Manager/subdir.mk
-include source/FlashManager/FlashDataRWInterface/subdir.mk
-include source/External_HW_Drivers/s2lp/subdir.mk
-include source/External_HW_Drivers/mx25l128/subdir.mk
-include source/Configuration_Manager/subdir.mk
-include source/CRC_tools/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include component/serial_manager/subdir.mk
-include component/lists/subdir.mk
-include board/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: MKL82Z128xxx7_Project.axf

# Tool invocations
MKL82Z128xxx7_Project.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="MKL82Z128xxx7_Project.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -mcpu=cortex-m0plus -mthumb -T MKL82Z128xxx7_Project_Debug.ld -o "MKL82Z128xxx7_Project.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) MKL82Z128xxx7_Project.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "MKL82Z128xxx7_Project.axf"; # arm-none-eabi-objcopy -v -O binary "MKL82Z128xxx7_Project.axf" "MKL82Z128xxx7_Project.bin" ; # checksum -p MKL82Z128xxx7 -d "MKL82Z128xxx7_Project.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
