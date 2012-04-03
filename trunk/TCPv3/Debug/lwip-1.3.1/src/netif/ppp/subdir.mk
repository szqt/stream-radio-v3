################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lwip-1.3.1/src/netif/ppp/auth.c \
../lwip-1.3.1/src/netif/ppp/chap.c \
../lwip-1.3.1/src/netif/ppp/chpms.c \
../lwip-1.3.1/src/netif/ppp/fsm.c \
../lwip-1.3.1/src/netif/ppp/ipcp.c \
../lwip-1.3.1/src/netif/ppp/lcp.c \
../lwip-1.3.1/src/netif/ppp/magic.c \
../lwip-1.3.1/src/netif/ppp/md5.c \
../lwip-1.3.1/src/netif/ppp/pap.c \
../lwip-1.3.1/src/netif/ppp/ppp.c \
../lwip-1.3.1/src/netif/ppp/ppp_oe.c \
../lwip-1.3.1/src/netif/ppp/randm.c \
../lwip-1.3.1/src/netif/ppp/vj.c 

OBJS += \
./lwip-1.3.1/src/netif/ppp/auth.o \
./lwip-1.3.1/src/netif/ppp/chap.o \
./lwip-1.3.1/src/netif/ppp/chpms.o \
./lwip-1.3.1/src/netif/ppp/fsm.o \
./lwip-1.3.1/src/netif/ppp/ipcp.o \
./lwip-1.3.1/src/netif/ppp/lcp.o \
./lwip-1.3.1/src/netif/ppp/magic.o \
./lwip-1.3.1/src/netif/ppp/md5.o \
./lwip-1.3.1/src/netif/ppp/pap.o \
./lwip-1.3.1/src/netif/ppp/ppp.o \
./lwip-1.3.1/src/netif/ppp/ppp_oe.o \
./lwip-1.3.1/src/netif/ppp/randm.o \
./lwip-1.3.1/src/netif/ppp/vj.o 

C_DEPS += \
./lwip-1.3.1/src/netif/ppp/auth.d \
./lwip-1.3.1/src/netif/ppp/chap.d \
./lwip-1.3.1/src/netif/ppp/chpms.d \
./lwip-1.3.1/src/netif/ppp/fsm.d \
./lwip-1.3.1/src/netif/ppp/ipcp.d \
./lwip-1.3.1/src/netif/ppp/lcp.d \
./lwip-1.3.1/src/netif/ppp/magic.d \
./lwip-1.3.1/src/netif/ppp/md5.d \
./lwip-1.3.1/src/netif/ppp/pap.d \
./lwip-1.3.1/src/netif/ppp/ppp.d \
./lwip-1.3.1/src/netif/ppp/ppp_oe.d \
./lwip-1.3.1/src/netif/ppp/randm.d \
./lwip-1.3.1/src/netif/ppp/vj.d 


# Each subdirectory must supply rules for building sources it contributes
lwip-1.3.1/src/netif/ppp/%.o: ../lwip-1.3.1/src/netif/ppp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -D__REDLIB__ -I"D:\MyDesigns\LPCXpresso\CMSISv2p00_LPC17xx\inc" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\src\include" -I"D:\MyDesigns\LPCXpresso\TCPv2\VS1053" -I"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\portable" -I"D:\MyDesigns\LPCXpresso\TCPv2\lpc17xx_lib\include" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\port\CoOS\ethernetif\LPC17xx" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\port\CoOS\arch" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\src\include\ipv4" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\src\include\ipv4\lwip" -I"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\kernel" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\port\CoOS" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


