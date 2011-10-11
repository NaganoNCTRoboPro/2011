###############################################################################
# Makefile for the project MU2-Wii_NunchukVer
###############################################################################

## General Flags
PROJECT = MU2-Wii_NunchukVer
MCU = atmega88
TARGET = MU2-Wii_NunchukVer.elf
CC = avr-gcc

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -Os -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=MU2-Wii_NunchukVer.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = mu2_common.o mu2_control.o mu2_data.o uart.o nunchuk.o nunchuk_main.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) MU2-Wii_NunchukVer.hex MU2-Wii_NunchukVer.eep MU2-Wii_NunchukVer.lss size

## Compile
mu2_common.o: ../mu2/mu2_common.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mu2_control.o: ../mu2/mu2_control.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mu2_data.o: ../mu2/mu2_data.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart.o: ../uart/uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

nunchuk.o: ../nunchuk/nunchuk.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

nunchuk_main.o: ../nunchuk_main.c
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
	-rm -rf $(OBJECTS) MU2-Wii_NunchukVer.elf dep/* MU2-Wii_NunchukVer.hex MU2-Wii_NunchukVer.eep MU2-Wii_NunchukVer.lss MU2-Wii_NunchukVer.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

