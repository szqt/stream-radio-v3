################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include stdio/subdir.mk
-include src/subdir.mk
-include lwip-1.4.0/src/netif/subdir.mk
-include lwip-1.4.0/src/netif/ppp/subdir.mk
-include lwip-1.4.0/src/core/subdir.mk
-include lwip-1.4.0/src/core/snmp/subdir.mk
-include lwip-1.4.0/src/core/ipv4/subdir.mk
-include lwip-1.4.0/src/api/subdir.mk
-include lwip-1.4.0/port/FreeRTOS/ethernetif/LPC17xx/subdir.mk
-include lwip-1.4.0/port/FreeRTOS/arch/subdir.mk
-include VS1053/subdir.mk
-include LPC1768_lib/source/subdir.mk
-include FreeRTOS/subdir.mk
-include FreeRTOS/portable/MemMang/subdir.mk
-include FreeRTOS/portable/GCC/ARM_CM3/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: TCPv3.axf

dependents:
	-cd D:/MyDesigns/LPCXpresso/CMSISv2p00_LPC17xx/Release && $(MAKE) all

# Tool invocations
TCPv3.axf: $(OBJS) $(USER_OBJS) D:/MyDesigns/LPCXpresso/CMSISv2p00_LPC17xx/Release/libCMSISv2p00_LPC17xx.a
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"D:\MyDesigns\LPCXpresso\CMSISv2p00_LPC17xx\Release" -Xlinker -Map="TCPv3.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "TCP_Debug.ld" -o"TCPv3.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) TCPv3.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "TCPv3.axf"; # arm-none-eabi-objcopy -O binary "TCPv3.axf" "TCPv3.bin" ; checksum -p LPC1768 -d "TCPv3.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build
D:/MyDesigns/LPCXpresso/CMSISv2p00_LPC17xx/Release/libCMSISv2p00_LPC17xx.a:

-include ../makefile.targets
