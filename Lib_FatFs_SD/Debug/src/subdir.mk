################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ff.c \
../src/mmc.c 

OBJS += \
./src/ff.o \
./src/mmc.o 

C_DEPS += \
./src/ff.d \
./src/mmc.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__REDLIB__ -I"C:\Users\rodri\Documents\LPCXpresso_8.2.2_650\projeto2\CMSISv2p00_LPC17xx\inc" -I"C:\Users\rodri\Documents\LPCXpresso_8.2.2_650\projeto2\Lib_FatFs_SD\inc" -I"C:\Users\rodri\Documents\LPCXpresso_8.2.2_650\projeto2\Lib_MCU\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


