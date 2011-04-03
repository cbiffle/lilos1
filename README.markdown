LILOS
=====

LILOS is a little operating system I built over Spring Break.  It provides
multitasking and basic hardware abstraction for the Atmel AVR (currently, the
ATmega328p).


The Example Program
-------------------

There's a working example in `main.cc`.  It assumes an Arduino-style
configuration where an LED is connected to Port B 5.  It starts three user tasks
and demonstrates inter-task messaging and use of the USART.

It currently uses 2848 bytes of Flash and 660 bytes of RAM.  I hope to reduce
both numbers now that everything works.
