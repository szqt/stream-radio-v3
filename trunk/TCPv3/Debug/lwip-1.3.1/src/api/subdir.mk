################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lwip-1.3.1/src/api/api_lib.c \
../lwip-1.3.1/src/api/api_msg.c \
../lwip-1.3.1/src/api/err.c \
../lwip-1.3.1/src/api/netbuf.c \
../lwip-1.3.1/src/api/netdb.c \
../lwip-1.3.1/src/api/netifapi.c \
../lwip-1.3.1/src/api/sockets.c \
../lwip-1.3.1/src/api/tcpip.c 

OBJS += \
./lwip-1.3.1/src/api/api_lib.o \
./lwip-1.3.1/src/api/api_msg.o \
./lwip-1.3.1/src/api/err.o \
./lwip-1.3.1/src/api/netbuf.o \
./lwip-1.3.1/src/api/netdb.o \
./lwip-1.3.1/src/api/netifapi.o \
./lwip-1.3.1/src/api/sockets.o \
./lwip-1.3.1/src/api/tcpip.o 

C_DEPS += \
./lwip-1.3.1/src/api/api_lib.d \
./lwip-1.3.1/src/api/api_msg.d \
./lwip-1.3.1/src/api/err.d \
./lwip-1.3.1/src/api/netbuf.d \
./lwip-1.3.1/src/api/netdb.d \
./lwip-1.3.1/src/api/netifapi.d \
./lwip-1.3.1/src/api/sockets.d \
./lwip-1.3.1/src/api/tcpip.d 


# Each subdirectory must supply rules for building sources it contributes
lwip-1.3.1/src/api/%.o: ../lwip-1.3.1/src/api/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -D__REDLIB__ -I"D:\MyDesigns\LPCXpresso\CMSISv2p00_LPC17xx\inc" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\src\include" -I"D:\MyDesigns\LPCXpresso\TCPv2\VS1053" -I"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\portable" -I"D:\MyDesigns\LPCXpresso\TCPv2\lpc17xx_lib\include" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\port\CoOS\ethernetif\LPC17xx" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\port\CoOS\arch" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\src\include\ipv4" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\src\include\ipv4\lwip" -I"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\kernel" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.3.1\port\CoOS" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


