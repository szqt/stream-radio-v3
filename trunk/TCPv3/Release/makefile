################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include lwip-1.4.0/src/netif/subdir.mk
-include lwip-1.4.0/src/netif/ppp/subdir.mk
-include lwip-1.4.0/src/core/subdir.mk
-include lwip-1.4.0/src/core/snmp/subdir.mk
-include lwip-1.4.0/src/core/ipv4/subdir.mk
-include lwip-1.4.0/src/api/subdir.mk
-include lwip-1.4.0/port/CoOS/ethernetif/LPC17xx/subdir.mk
-include lwip-1.4.0/port/CoOS/arch/subdir.mk
-include lpc17xx_lib/source/subdir.mk
-include VS1053/subdir.mk
-include CoOS/portable/subdir.mk
-include CoOS/portable/GCC/subdir.mk
-include CoOS/kernel/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: TCPv2.axf

# Tool invocations
TCPv2.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"D:\MyDesigns\LPCXpresso\CMSISv2p00_LPC17xx\Release" -L"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\kernel" -L"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\portable" -L"D:\MyDesigns\LPCXpresso\TCPv2\CoOS\portable\GCC" -L"D:\MyDesigns\LPCXpresso\TCPv2\lpc17xx_lib\include" -L"D:\MyDesigns\LPCXpresso\TCPv2\lpc17xx_lib\source" -L"D:\MyDesigns\LPCXpresso\TCPv2\src" -L"D:\MyDesigns\LPCXpresso\TCPv2\VS1053" -L"D:\MyDesigns\LPCXpresso\TCPv2\stdio" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\port\CoOS" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\port\CoOS\arch" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\port\CoOS\ethernetif" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\api" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\core" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\core\ipv4" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\core\snmp" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\ipv4" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\ipv4\lwip" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\ipv6" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\ipv6\lwip" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\include\lwip" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\netif" -L"D:\MyDesigns\LPCXpresso\TCPv2\lwip-1.4.0\src\netif\ppp" -Xlinker -Map="TCPv2.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "TCP_Release.ld" -o"TCPv2.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) TCPv2.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "TCPv2.axf"; # arm-none-eabi-objcopy -O binary "TCPv2.axf" "TCPv2.bin" ; checksum -p LPC1768 -d "TCPv2.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
