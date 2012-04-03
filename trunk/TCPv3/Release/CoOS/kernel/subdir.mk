################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CoOS/kernel/core.c \
../CoOS/kernel/event.c \
../CoOS/kernel/flag.c \
../CoOS/kernel/hook.c \
../CoOS/kernel/kernelHeap.c \
../CoOS/kernel/mbox.c \
../CoOS/kernel/mm.c \
../CoOS/kernel/mutex.c \
../CoOS/kernel/queue.c \
../CoOS/kernel/sem.c \
../CoOS/kernel/serviceReq.c \
../CoOS/kernel/task.c \
../CoOS/kernel/time.c \
../CoOS/kernel/timer.c \
../CoOS/kernel/utility.c 

OBJS += \
./CoOS/kernel/core.o \
./CoOS/kernel/event.o \
./CoOS/kernel/flag.o \
./CoOS/kernel/hook.o \
./CoOS/kernel/kernelHeap.o \
./CoOS/kernel/mbox.o \
./CoOS/kernel/mm.o \
./CoOS/kernel/mutex.o \
./CoOS/kernel/queue.o \
./CoOS/kernel/sem.o \
./CoOS/kernel/serviceReq.o \
./CoOS/kernel/task.o \
./CoOS/kernel/time.o \
./CoOS/kernel/timer.o \
./CoOS/kernel/utility.o 

C_DEPS += \
./CoOS/kernel/core.d \
./CoOS/kernel/event.d \
./CoOS/kernel/flag.d \
./CoOS/kernel/hook.d \
./CoOS/kernel/kernelHeap.d \
./CoOS/kernel/mbox.d \
./CoOS/kernel/mm.d \
./CoOS/kernel/mutex.d \
./CoOS/kernel/queue.d \
./CoOS/kernel/sem.d \
./CoOS/kernel/serviceReq.d \
./CoOS/kernel/task.d \
./CoOS/kernel/time.d \
./CoOS/kernel/timer.d \
./CoOS/kernel/utility.d 


# Each subdirectory must supply rules for building sources it contributes
CoOS/kernel/%.o: ../CoOS/kernel/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DNDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"D:\MyDesigns\LPCXpresso\CMSISv2p00_LPC17xx\inc" -I"D:\MyDesigns\LPCXpresso\TCPv2\lpc17xx_lib\include" -I"D:\MyDesigns\LPCXpresso\TCPv2\VS1053" -I"D:\MyDesigns\LPCXpresso\TCPv2\src" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\port\CoOS\arch" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\port\CoOS\ethernetif\LPC17xx" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\ipv4\lwip" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\ipv6\lwip" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\lwip" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\netif" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\netif\ppp" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\port\CoOS" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\ipv4" -I"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\ipv6" -I"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\kernel" -I"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\portable" -Os -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


