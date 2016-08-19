CC=msp430-gcc
CFLAGS=-Wall -pedantic -std=c99 -mmcu=msp430f5529
SOURCES=./example.c ./serial_uart.c ./serial_uart.h
OUTFILE=firmware.elf

firmware:
	@echo $(OUTFILE)
	@$(CC) $(CFLAGS) -o $(OUTFILE) $(SOURCES)

flash: firmware
	@mspdebug tilib --force-reset "prog $(OUTFILE)"
	
clean:
	@if [ -e $(OUTFILE) ]; then rm $(OUTFILE); fi
