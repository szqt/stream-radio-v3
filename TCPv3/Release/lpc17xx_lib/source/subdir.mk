################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lpc17xx_lib/source/adc.c \
../lpc17xx_lib/source/gpio.c \
../lpc17xx_lib/source/lpc17xx_clkpwr.c \
../lpc17xx_lib/source/timer.c \
../lpc17xx_lib/source/uart.c 

OBJS += \
./lpc17xx_lib/source/adc.o \
./lpc17xx_lib/source/gpio.o \
./lpc17xx_lib/source/lpc17xx_clkpwr.o \
./lpc17xx_lib/source/timer.o \
./lpc17xx_lib/source/uart.o 

C_DEPS += \
./lpc17xx_lib/source/adc.d \
./lpc17xx_lib/source/gpio.d \
./lpc17xx_lib/source/lpc17xx_clkpwr.d \
./lpc17xx_lib/source/timer.d \
./lpc17xx_lib/source/uart.d 


# Each subdirectory must supply rules for building sources it contributes
lpc17xx_lib/source/%.o: ../lpc17xx_lib/source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DNDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"D:\MyDesigns\LPCXpresso\CMSISv2p00_LPC17xx\inc" -I"D:\MyDesigns\LPCXpresso\TCPv2\lpc17xx_lib\include" -I"D:\MyDesigns\LPCXpresso\TCPv2\VS1053" -I"D:\MyDesigns\LPCXpresso\TCPv2\src" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\port\CoOS\arch" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\port\CoOS\ethernetif\LPC17xx" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\ipv4\lwip" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\ipv6\lwip" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\lwip" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\netif" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\netif\ppp" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\port\CoOS" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\ipv4" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\ipv6" -I"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\kernel" -I"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\portable" -Os -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


