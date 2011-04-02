LD=avr-ld
GXX=avr-g++
AR=avr-ar
OBJCOPY=avr-objcopy
PART=atmega328p
DUDE=avrdude
PORT=/dev/tty.usbserial-FTE597U5

CFLAGS= -Iinclude \
        -std=gnu++98 \
        -Os \
        -DF_CPU=8000000 \
        -mmcu=$(PART) \
        -fno-threadsafe-statics \
        -fdata-sections -ffunction-sections \
        -fpack-struct \
        -fshort-enums \
        -unsigned-char \
        -ffreestanding

LDFLAGS= -mmcu=$(PART) \
         -L. \
         -Wl,--gc-sections,--relax \
         -Wl,-Map,main.map

all: main.hex

clean:
	-rm -f main.elf main.hex main.map
	-rm -f *.o
	-rm -rf build/
	-rm -f liblilos.a

build:
	mkdir -p build/


liblilos.a: build/task.o build/usart.o build/time.o build/debug.o
	$(AR) rcs $@ $^

build/%.o: src/%.cc build
	$(GXX) $(CFLAGS) -c -o $@ $<


main.elf: main.o liblilos.a
	$(GXX) $(LDFLAGS) -o $@ $^ -llilos

%.o: %.cc
	$(GXX) $(CFLAGS) -c -o $@ $^

main.hex: main.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

reset:
	stty -f $(PORT) hupcl

program: main.hex reset
	$(DUDE) -p m328p -P $(PORT) -c stk500v1 -b 57600 \
	  -U flash:w:$<
