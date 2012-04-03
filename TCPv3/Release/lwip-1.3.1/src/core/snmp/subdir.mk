################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lwip-1.3.1/src/core/snmp/asn1_dec.c \
../lwip-1.3.1/src/core/snmp/asn1_enc.c \
../lwip-1.3.1/src/core/snmp/mib2.c \
../lwip-1.3.1/src/core/snmp/mib_structs.c \
../lwip-1.3.1/src/core/snmp/msg_in.c \
../lwip-1.3.1/src/core/snmp/msg_out.c 

OBJS += \
./lwip-1.3.1/src/core/snmp/asn1_dec.o \
./lwip-1.3.1/src/core/snmp/asn1_enc.o \
./lwip-1.3.1/src/core/snmp/mib2.o \
./lwip-1.3.1/src/core/snmp/mib_structs.o \
./lwip-1.3.1/src/core/snmp/msg_in.o \
./lwip-1.3.1/src/core/snmp/msg_out.o 

C_DEPS += \
./lwip-1.3.1/src/core/snmp/asn1_dec.d \
./lwip-1.3.1/src/core/snmp/asn1_enc.d \
./lwip-1.3.1/src/core/snmp/mib2.d \
./lwip-1.3.1/src/core/snmp/mib_structs.d \
./lwip-1.3.1/src/core/snmp/msg_in.d \
./lwip-1.3.1/src/core/snmp/msg_out.d 


# Each subdirectory must supply rules for building sources it contributes
lwip-1.3.1/src/core/snmp/%.o: ../lwip-1.3.1/src/core/snmp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DNDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"D:\MyDesigns\LPCXpresso\CMSISv2p00_LPC17xx\inc" -I"D:\MyDesigns\LPCXpresso\TCP\CoOS\kernel" -I"D:\MyDesigns\LPCXpresso\TCP\CoOS\portable" -I"D:\MyDesigns\LPCXpresso\TCP\CoOS\portable\GCC" -I"D:\MyDesigns\LPCXpresso\TCP\lpc17xx_lib\include" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\port\CoOS\arch" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\port\CoOS\ethernetif\LPC17xx" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include\ipv4\lwip" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include\ipv6\lwip" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include\lwip" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include\netif" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\netif\ppp" -I"D:\MyDesigns\LPCXpresso\TCP\VS1053" -I"D:\MyDesigns\LPCXpresso\TCP\src" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\port\CoOS" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include\ipv4" -I"D:\MyDesigns\LPCXpresso\TCP\lwip-1.3.1\src\include\ipv6" -Os -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


