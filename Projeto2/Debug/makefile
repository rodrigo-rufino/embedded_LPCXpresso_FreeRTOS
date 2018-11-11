################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
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
all: Example07.axf

# Tool invocations
Example07.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\rodri\Documents\LPCXpresso_8.2.2_650\projeto2\CMSISv2p00_LPC17xx\Debug" -L"C:\Users\rodri\Documents\LPCXpresso_8.2.2_650\projeto2\FreeRTOS_Library\Debug" -L"C:\Users\rodri\Documents\LPCXpresso_8.2.2_650\projeto2\Lib_EaBaseBoard\Debug" -L"C:\Users\rodri\Documents\LPCXpresso_8.2.2_650\projeto2\Lib_MCU\Debug" -Xlinker --gc-sections -Xlinker -Map=Example07.map -mcpu=cortex-m3 -mthumb -T "Projeto2_Debug.ld" -o "Example07.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Example07.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size Example07.axf; # arm-none-eabi-objcopy -O ihex Example07.axf Example07.hex ;
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
