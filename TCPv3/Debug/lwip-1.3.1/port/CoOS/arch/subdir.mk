################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lwip-1.3.1/port/CoOS/arch/sys_arch.c 

OBJS += \
./lwip-1.3.1/port/CoOS/arch/sys_arch.o 

C_DEPS += \
./lwip-1.3.1/port/CoOS/arch/sys_arch.d 


# Each subdirectory must supply rules for building sources it contributes
lwip-1.3.1/port/CoOS/arch/%.o: ../lwip-1.3.1/port/CoOS/arch/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -D__REDLIB__ -I"D:\MyDesigns\LPCXpresso\CMSISv2p00_LPC17xx\inc" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\src\include" -I"D:\MyDesigns\LPCXpresso\TCPv2\VS1053" -I"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\portable" -I"D:\MyDesigns\LPCXpresso\TCPv2\lpc17xx_lib\include" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\port\CoOS\ethernetif\LPC17xx" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\port\CoOS\arch" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\src\include\ipv4" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\src\include\ipv4\lwip" -I"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\kernel" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\port\CoOS" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


