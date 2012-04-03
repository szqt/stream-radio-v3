################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../syscalls/syscalls.c 

OBJS += \
./syscalls/syscalls.o 

C_DEPS += \
./syscalls/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
syscalls/%.o: ../syscalls/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"D:\MyDesigns\LPCXpresso\CMSISv2p00_LPC17xx" -I"D:\MyDesigns\LPCXpresso\TCPv3\FreeRTOS" -I"D:\MyDesigns\LPCXpresso\TCPv3\FreeRTOS\include" -I"D:\MyDesigns\LPCXpresso\TCPv3\FreeRTOS\portable" -I"D:\MyDesigns\LPCXpresso\TCPv3\FreeRTOS\portable\GCC" -I"D:\MyDesigns\LPCXpresso\TCPv3\FreeRTOS\portable\GCC\ARM_CM3" -I"D:\MyDesigns\LPCXpresso\TCPv3\FreeRTOS\portable\MemMang" -I"D:\MyDesigns\LPCXpresso\TCPv3\src" -I"D:\MyDesigns\LPCXpresso\CMSISv2p00_LPC17xx\inc" -I"D:\MyDesigns\LPCXpresso\TCPv3\LPC1768_lib\include" -I"D:\MyDesigns\LPCXpresso\TCPv3\LPC1768_lib\source" -I"D:\MyDesigns\LPCXpresso\TCPv3\LPC1768_lib" -I"D:\MyDesigns\LPCXpresso\TCPv3\lwip-1.4.0" -I"D:\MyDesigns\LPCXpresso\TCPv3\lwip-1.4.0\port" -I"D:\MyDesigns\LPCXpresso\TCPv3\lwip-1.4.0\port\FreeRTOS" -I"D:\MyDesigns\LPCXpresso\TCPv3\lwip-1.4.0\port\FreeRTOS\arch" -I"D:\MyDesigns\LPCXpresso\TCPv3\lwip-1.4.0\port\FreeRTOS\ethernetif\LPC17xx" -I"D:\MyDesigns\LPCXpresso\TCPv3\lwip-1.4.0\src\include" -I"D:\MyDesigns\LPCXpresso\TCPv3\lwip-1.4.0\src\include\ipv4" -I"D:\MyDesigns\LPCXpresso\TCPv3\lwip-1.4.0\src\include\lwip" -I"D:\MyDesigns\LPCXpresso\TCPv3\lwip-1.4.0\src" -I"D:\MyDesigns\LPCXpresso\TCPv3\lwip-1.4.0\src\include\ipv6" -I"D:\MyDesigns\LPCXpresso\TCPv3\VS1053" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


