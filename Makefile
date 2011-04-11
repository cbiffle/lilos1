LD=avr-ld
GXX=avr-g++
AR=avr-ar
OBJCOPY=avr-objcopy
DUDE=avrdude

include board/$(BOARD)/Makefile.board

PORT=/dev/tty.usbserial-FTE597U5

.SECONDARY: # No seriously make stop doing that

include board/$(BOARD)/Makefile.board
include mcu/$(MMCU)/Makefile.mcu

CFLAGS= -Iinclude \
        -Iboard/$(BOARD)/include \
        -Imcu/$(MMCU)/include \
        -D__STDC_LIMIT_MACROS \
        -std=gnu++98 \
        -Os \
        -DF_CPU=$(F_CPU) \
        -mmcu=$(MMCU) \
        -fno-threadsafe-statics \
        -fdata-sections -ffunction-sections \
        -fpack-struct \
        -fshort-enums \
        -unsigned-char \
        -ffreestanding

LDFLAGS= -mmcu=$(MMCU) \
         -L. \
         -Wl,--gc-sections,--relax \
         -Wl,-Map,main_$(BOARD).map

all: main_$(BOARD).hex

clean:
	-rm -f main_*.elf main_*.hex main_*.map
	-rm -f *.o
	-rm -rf build/
	-rm -rf mcu/*/build/
	-rm -f liblilos_*.a

build:
	mkdir -p build/

%/build:
	mkdir -p $@


liblilos_$(BOARD).a: build/task.o build/usart.o build/time.o build/debug.o \
                     $(MCU_OBJS) $(BOARD_OBJS)
	$(AR) rcs $@ $^

build/%.o: src/%.cc build
	$(GXX) $(CFLAGS) -c -o $@ $<

mcu/$(MMCU)/build/%.o: mcu/$(MMCU)/src/%.cc mcu/$(MMCU)/build
	$(GXX) $(CFLAGS) -c -o $@ $<


main_$(BOARD).elf: main.o liblilos_$(BOARD).a
	$(GXX) $(LDFLAGS) -o $@ $^ -llilos_$(BOARD)

%.o: %.cc
	$(GXX) $(CFLAGS) -c -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

reset:
	stty -f $(PORT) hupcl

program: main_$(BOARD).hex reset
	$(DUDE) -p $(MMCU) -P $(PORT) -c stk500v1 -b 57600 \
	  -U flash:w:$<
