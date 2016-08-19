//--------------------------------------------------------------------------
// bibliotecas
#include <msp430.h>
#include <stdint.h>
#include "serial_uart.h"

//==========================================================================
// SERIAL CONFIG
// funcao: configura a comunicacao serial UART
// retorno: nenhum
// parametros: nenhum
// constantes:
//      SERIAL_DBG: se definida, habilita msgs de debug
//      SERIAL_SMCLK: sub-main clock (Hz)
//      SERIAL_BAUD: taxa de comunicacao (bps)
//      SERIALRXPIN: pino de recepcao
//      SERIALTXPIN: pino de transmissao
//==========================================================================
void serial_config(){
    // software reset enabled
    UCA0CTL1 |= UCSWRST;
    // clock source select: SMCLK
    UCA0CTL1 |= UCSSEL_2;
    
    uint16_t ucbr;
    uint8_t ucbrs;
    uint8_t ucbrf;

#ifndef SERIAL_SMCLK
#error SERIAL_SMCLK is not defined
#endif

#ifndef SERIAL_BAUD
#error SERIAL_BAUD is not defined
#endif

#if SERIAL_SMCLK == 16000000 // 16MHz
#if SERIAL_BAUD == 9600
    ucbr = 1666;
    ucbrs = 6;
#else
#error Baudrate not implemented
#endif
#else
#error SMCLK not implemented
#endif

    UCA0BR0 = (ucbr&0x00FF);
    UCA0BR1 = (ucbr&0xFF00)>>8;
    UCA0MCTL |= (ucbrs<<1);    

    // Clear interrupt flag
    UCA0IFG &= ~(UCRXIFG+UCTXIFG);
    // function select registers
    P3SEL  |= (SERIALRXPIN|SERIALTXPIN);
    // software reset disabled
    UCA0CTL1 &= ~UCSWRST;
    // receive interrupt enabled
    UCA0IE |= UCRXIE;
}

//==========================================================================
// SERIAL PRINT BYTE
// funcao: envia um byte pela porta serial UART
// retorno: nenhum
// parametros: caractere ASCII (const int8_t)
// constantes: nenhuma
//==========================================================================
void serial_print_byte(const char data){
    while(!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = data;
}

//==========================================================================
// SERIAL PRINT STRING
// funcao: envia uma c_string pela porta serial UART
// retorno: nenhum
// parametros: string ASCII (const char*)
// constantes: nenhuma
//==========================================================================
void serial_print_string(const char* data){
    while(*data){
        serial_print_byte(*data);
        data++;
    }
}
