LD=avr-ld
GXX=avr-g++
OBJCOPY=avr-objcopy
PART=atmega328p
DUDE=avrdude
PORT=/dev/tty.usbserial-FTE597U5

CFLAGS= -O2 \
        -DF_CPU=8000000 \
        -mmcu=$(PART) \
        -fdata-sections -ffunction-sections \
        -fpack-struct \
        -fshort-enums \
        -unsigned-char \
        -ffreestanding

LDFLAGS= -mmcu=$(PART) \
         -Wl,--gc-sections,--relax \
         -Wl,-Map,main.map

all: main.hex

clean:
	-rm -f main.elf main.hex
	-rm -f *.o

main.elf: main.o task.o usart.o time.o
	$(GXX) $(LDFLAGS) -o $@ $^

%.o: %.cc
	$(GXX) $(CFLAGS) -c -o $@ $<

main.hex: main.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

program: main.hex
	stty -f $(PORT) hupcl
	$(DUDE) -p m328p -P $(PORT) -c stk500v1 -b 57600 \
	  -U flash:w:$<
