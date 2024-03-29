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
INCLUDES = -I"C:\Users\iwate\Documents\ROBOS21\." -I"C:\Users\iwate\Documents\ROBOS21\APPLICATION" -I"C:\Users\iwate\Documents\ROBOS21\PERIPHERAL" -I"C:\Users\iwate\Documents\ROBOS21\PERIPHERAL\LED" -I"C:\Users\iwate\Documents\ROBOS21\PERIPHERAL\MU2" -I"C:\Users\iwate\Documents\ROBOS21\PERIPHERAL\REMOTE_CONTROL" -I"C:\Users\iwate\Documents\ROBOS21\ROBOS\COMMON" -I"C:\Users\iwate\Documents\ROBOS21\ROBOS\I2C" -I"C:\Users\iwate\Documents\ROBOS21\ROBOS\KERNEL" -I"C:\Users\iwate\Documents\ROBOS21\ROBOS\SERIAL" -I"C:\Users\iwate\Documents\ROBOS21\PERIPHERAL\ACTUATER" -I"C:\Users\iwate\Documents\ROBOS21\PERIPHERAL\SWITCH" 

## Objects that must be built in order to link
OBJECTS = main.o UserTask.o remote_control_rx.o mu2.o uart.o ringbuffer.o kernel.o i2c.o led.o wait.o actuater.o switch.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) ROBOS2.hex ROBOS2.eep ROBOS2.lss size

## Compile
main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

UserTask.o: ../APPLICATION/UserTask.c
	$(CC) $(INCLUDES) $(CFLAGS) -c -O0 $<

remote_control_rx.o: ../PERIPHERAL/REMOTE_CONTROL/remote_control_rx.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mu2.o: ../PERIPHERAL/MU2/mu2.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart.o: ../ROBOS/SERIAL/uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ringbuffer.o: ../ROBOS/SERIAL/ringbuffer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

kernel.o: ../ROBOS/KERNEL/kernel.c
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
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

