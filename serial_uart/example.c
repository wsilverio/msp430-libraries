#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>

#include "serial_uart.h"

int main(){
    WDTCTL = WDTPW + WDTHOLD;

    if (0xFF == CALBC1_16MHZ){
        while(1);
    }

    DCOCTL = 0;
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;

    serial_config(16000000, 9600);

    serial_print_string("ASCII TABLE:\n");

    char strVal[4];
    for(char i = ' '; i < 127; i++){
        itoa(i, strVal, 10);
        serial_print_string(strVal);
        serial_print_string(": ");
        serial_print_byte((char)i);
        serial_print_byte('\n');
    }

    serial_print_byte('\n');
    
    __enable_interrupt();
    __bis_SR_register(LPM4_bits);

    return 0;
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void serial_receive(){
    char data = UCA0RXBUF;
    serial_print_string("Received: ");
    serial_print_byte(data);
    serial_print_byte('\n');
}
