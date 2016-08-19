#include <msp430.h>
#include <stdlib.h> // itoa

#include "serial_uart.h"

void clock_config();
void SetVcoreUp (unsigned int level);

int main(){
    WDTCTL = WDTPW | WDTHOLD;

    clock_config();

    serial_config();

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

// #pragma vector = USCIAB0RX_VECTOR
// __interrupt void serial_receive(){
//     char data = UCA0RXBUF;
//     serial_print_string("Received: ");
//     serial_print_byte(data);
//     serial_print_byte('\n');
// }

void clock_config(){
    // Increase Vcore setting to level2 to support fsystem=20MHz
    // NOTE: Change core voltage one level at a time..
    SetVcoreUp (0x01);
    SetVcoreUp (0x02);
    UCSCTL3 |= SELREF_2;                        // Set DCO FLL reference = REFO
    UCSCTL4 |= SELA_2;                          // Set ACLK = REFO

    __bis_SR_register(SCG0);                    // Disable the FLL control loop
    UCSCTL0 = 0x0000;                           // Set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_6;                        // Select DCO range 32MHz operation
    UCSCTL2 = FLLD_1 + 487;                     // Set DCO Multiplier for 16MHz
                                                // (N + 1) * FLLRef = Fdco
                                                // (487 + 1) * 32768 = 16MHz
                                                // Set FLL Div = fDCOCLK/2
    __bic_SR_register(SCG0);                    // Enable the FLL control loop
    // Worst-case settling time for the DCO when the DCO range bits have been
    // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
    // UG for optimization.
    // 32 x 32 x 16 MHz / 32,768 Hz = 500000 = MCLK cycles for DCO to settle
    __delay_cycles(500000);
    // Loop until XT1,XT2 & DCO fault flag is cleared
    do
    {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                                // Clear XT2,XT1,DCO fault flags
        SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    }while (SFRIFG1&OFIFG);                     // Test oscillator fault flag
}

void SetVcoreUp (unsigned int level){
    PMMCTL0_H = PMMPW_H;                            // Open PMM registers for write
    SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;    // Set SVS/SVM high side new level
    SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;   // Set SVM low side to new level
    while ((PMMIFG & SVSMLDLYIFG) == 0);            // Wait till SVM is settled
    PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);              // Clear already set flags
    PMMCTL0_L = PMMCOREV0 * level;                  // Wait till new level reached
    if ((PMMIFG & SVMLIFG))
        while ((PMMIFG & SVMLVLRIFG) == 0);
    SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;// Set SVS/SVM low side to new level
    PMMCTL0_H = 0x00;                               // Lock PMM registers for write access
}
