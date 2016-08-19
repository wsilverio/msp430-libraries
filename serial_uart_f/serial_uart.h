#ifndef _SERIAL_UART_H_
#define _SERIAL_UART_H_

// #define SERIAL_DBG

#define SERIAL_SMCLK 16000000
#define SERIAL_BAUD 9600

#define SERIALRXPIN BIT4 // P3.4 RXD USB
#define SERIALTXPIN BIT3 // P3.3 TXD USB

void serial_config();
void serial_print_byte(const char data);
void serial_print_string(const char* data);

#endif
