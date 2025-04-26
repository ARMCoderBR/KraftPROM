/*
 * File:   main.c
 * Author: milton
 *
 * Created on 24 de Abril de 2025, 23:30
 */


#include <xc.h>
#include <pic.h>
#include <stdint.h>
#include "system.h"
#include "usart.h"


void main(void) {
    
    USART_Initialize();

    INTCONbits.PEIE = 1;
    
    USART_putstr("CPS Tecnologia\r\n");

    ei();
    
    for (;;){
        
        if (usart_has_char()){
            
            uint8_t c = usart_getch();
            USART_putcUSART(c);
            
            __delay_us(100);
        }
    }


    return;
}
