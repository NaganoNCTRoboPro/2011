###############################################################################
# Makefile for the project ROBOS2
###############################################################################

## General Flags
PROJECT = ROBOS2
MCU = atmega1284p
TARGET = ROBOS2.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -O0 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=ROBOS2.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
<<<<<<< HEAD
INCLUDES = -I"D:\tmp\2011\NotOSX_AOF\." -I"D:\tmp\2011\NotOSX_AOF\APPLICATION" -I"D:\tmp\2011\NotOSX_AOF\PERIPHERAL" -I"D:\tmp\2011\NotOSX_AOF\PERIPHERAL\LED" -I"D:\tmp\2011\NotOSX_AOF\PERIPHERAL\MU2" -I"D:\tmp\2011\NotOSX_AOF\PERIPHERAL\REMOTE_CONTROL" -I"D:\tmp\2011\NotOSX_AOF\ROBOS\COMMON" -I"D:\tmp\2011\NotOSX_AOF\ROBOS\I2C" -I"D:\tmp\2011\NotOSX_AOF\ROBOS\KERNEL" -I"D:\tmp\2011\NotOSX_AOF\ROBOS\SERIAL" -I"D:\tmp\2011\NotOSX_AOF\PERIPHERAL\ACTUATER" -I"D:\tmp\2011\NotOSX_AOF\PERIPHERAL\SWITCH" -I"D:\tmp\2011\NotOSX_AOF\PERIPHERAL\BEEP" -I"D:\tmp\2011\NotOSX_AOF\PERIPHERAL\EMERGENCY" 
=======
INCLUDES = -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\." -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\APPLICATION" -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\PERIPHERAL" -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\PERIPHERAL\LED" -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\PERIPHERAL\MU2" -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\PERIPHERAL\REMOTE_CONTROL" -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\ROBOS\COMMON" -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\ROBOS\I2C" -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\ROBOS\KERNEL" -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\ROBOS\SERIAL" -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\PERIPHERAL\ACTUATER" -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\PERIPHERAL\SWITCH" -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\PERIPHERAL\BEEP" -I"C:\Users\BillGates\Desktop\natinal\2011\NotOSX_AOF\PERIPHERAL\EMERGENCY" 
>>>>>>> 317eb8aadb7b758495f27da2e2f8e6f397e1e755

## Objects that must be built in order to link
OBJECTS = main.o remote_control_rx.o mu2.o uart.o ringbuffer.o i2c.o led.o wait.o actuater.o switch.o beep.o emergency.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) ROBOS2.hex ROBOS2.eep ROBOS2.lss size

## Compile
main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

remote_control_rx.o: ../PERIPHERAL/REMOTE_CONTROL/remote_control_rx.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mu2.o: ../PERIPHERAL/MU2/mu2.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart.o: ../ROBOS/SERIAL/uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ringbuffer.o: ../ROBOS/SERIAL/ringbuffer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

i2c.o: ../ROBOS/I2C/i2c.c
	$(CC) $(INCLUDES) $(CFLAGS) -c -O0 $<

led.o: ../PERIPHERAL/LED/led.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

wait.o: ../ROBOS/COMMON/wait.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

actuater.o: ../PERIPHERAL/ACTUATER/actuater.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

switch.o: ../PERIPHERAL/SWITCH/switch.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

beep.o: ../PERIPHERAL/BEEP/beep.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

emergency.o: ../PERIPHERAL/EMERGENCY/emergency.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) ROBOS2.elf dep/* ROBOS2.hex ROBOS2.eep ROBOS2.lss ROBOS2.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

