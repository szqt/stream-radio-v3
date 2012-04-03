################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lwip-1.3.1/src/core/def.c \
../lwip-1.3.1/src/core/dhcp.c \
../lwip-1.3.1/src/core/dns.c \
../lwip-1.3.1/src/core/init.c \
../lwip-1.3.1/src/core/mem.c \
../lwip-1.3.1/src/core/memp.c \
../lwip-1.3.1/src/core/netif.c \
../lwip-1.3.1/src/core/pbuf.c \
../lwip-1.3.1/src/core/raw.c \
../lwip-1.3.1/src/core/stats.c \
../lwip-1.3.1/src/core/sys.c \
../lwip-1.3.1/src/core/tcp.c \
../lwip-1.3.1/src/core/tcp_in.c \
../lwip-1.3.1/src/core/tcp_out.c \
../lwip-1.3.1/src/core/timers.c \
../lwip-1.3.1/src/core/udp.c 

OBJS += \
./lwip-1.3.1/src/core/def.o \
./lwip-1.3.1/src/core/dhcp.o \
./lwip-1.3.1/src/core/dns.o \
./lwip-1.3.1/src/core/init.o \
./lwip-1.3.1/src/core/mem.o \
./lwip-1.3.1/src/core/memp.o \
./lwip-1.3.1/src/core/netif.o \
./lwip-1.3.1/src/core/pbuf.o \
./lwip-1.3.1/src/core/raw.o \
./lwip-1.3.1/src/core/stats.o \
./lwip-1.3.1/src/core/sys.o \
./lwip-1.3.1/src/core/tcp.o \
./lwip-1.3.1/src/core/tcp_in.o \
./lwip-1.3.1/src/core/tcp_out.o \
./lwip-1.3.1/src/core/timers.o \
./lwip-1.3.1/src/core/udp.o 

C_DEPS += \
./lwip-1.3.1/src/core/def.d \
./lwip-1.3.1/src/core/dhcp.d \
./lwip-1.3.1/src/core/dns.d \
./lwip-1.3.1/src/core/init.d \
./lwip-1.3.1/src/core/mem.d \
./lwip-1.3.1/src/core/memp.d \
./lwip-1.3.1/src/core/netif.d \
./lwip-1.3.1/src/core/pbuf.d \
./lwip-1.3.1/src/core/raw.d \
./lwip-1.3.1/src/core/stats.d \
./lwip-1.3.1/src/core/sys.d \
./lwip-1.3.1/src/core/tcp.d \
./lwip-1.3.1/src/core/tcp_in.d \
./lwip-1.3.1/src/core/tcp_out.d \
./lwip-1.3.1/src/core/timers.d \
./lwip-1.3.1/src/core/udp.d 


# Each subdirectory must supply rules for building sources it contributes
lwip-1.3.1/src/core/%.o: ../lwip-1.3.1/src/core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -D__REDLIB__ -I"D:\MyDesigns\LPCXpresso\CMSISv2p00_LPC17xx\inc" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\src\include" -I"D:\MyDesigns\LPCXpresso\TCPv2\VS1053" -I"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\portable" -I"D:\MyDesigns\LPCXpresso\TCPv2\lpc17xx_lib\include" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\port\CoOS\ethernetif\LPC17xx" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\port\CoOS\arch" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\src\include\ipv4" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\src\include\ipv4\lwip" -I"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\kernel" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\port\CoOS" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


