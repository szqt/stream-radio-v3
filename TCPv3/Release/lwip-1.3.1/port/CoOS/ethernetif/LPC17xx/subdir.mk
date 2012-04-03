################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lwip-1.3.1/port/CoOS/ethernetif/LPC17xx/EMAC.c \
../lwip-1.3.1/port/CoOS/ethernetif/LPC17xx/ethernetif.c 

OBJS += \
./lwip-1.3.1/port/CoOS/ethernetif/LPC17xx/EMAC.o \
./lwip-1.3.1/port/CoOS/ethernetif/LPC17xx/ethernetif.o 

C_DEPS += \
./lwip-1.3.1/port/CoOS/ethernetif/LPC17xx/EMAC.d \
./lwip-1.3.1/port/CoOS/ethernetif/LPC17xx/ethernetif.d 


# Each subdirectory must supply rules for building sources it contributes
lwip-1.3.1/port/CoOS/ethernetif/LPC17xx/%.o: ../lwip-1.3.1/port/CoOS/ethernetif/LPC17xx/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DNDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"D:\MyDesigns\LPCXpresso\CMSISv2p00_LPC17xx\inc" -I"D:\MyDesigns\LPCXpresso\TCP\CoOS\kernel" -I"D:\MyDesigns\LPCXpresso\TCP\CoOS\portable" -I"D:\MyDesigns\LPCXpresso\TCP\CoOS\portable\GCC" -I"D:\MyDesigns\LPCXpresso\TCP\lpc17xx_lib\include" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\port\CoOS\arch" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\port\CoOS\ethernetif\LPC17xx" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include\ipv4\lwip" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include\ipv6\lwip" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include\lwip" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include\netif" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\netif\ppp" -I"D:\MyDesigns\LPCXpresso\TCP\VS1053" -I"D:\MyDesigns\LPCXpresso\TCP\src" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\port\CoOS" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include\ipv4" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include\ipv6" -Os -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


