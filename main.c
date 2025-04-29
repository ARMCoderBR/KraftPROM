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

#define PGCLK0 PORTAbits.RA1 = 0
#define PGDAT0 PORTAbits.RA0 = 0
#define PGCLK1 PORTAbits.RA1 = 1
#define PGDAT1 PORTAbits.RA0 = 1

////////////////////////////////////////////////////////////////////////////////
void send_addr_data(uint16_t addr, uint8_t data){
    
    int i;

    for (i = 0; i < 8; i++){
        
        if (data & 0x80)
            PGDAT1;
        else
            PGDAT0;
        PGCLK1;
        PGCLK0;
        data <<= 1;
    }

    for (i = 0; i < 16; i++){
        
        if (addr & 0x8000)
            PGDAT1;
        else
            PGDAT0;
        PGCLK1;
        PGCLK0;
        addr <<= 1;
    }
}

////////////////////////////////////////////////////////////////////////////////
void proc_addr(char c){
    
    switch(c){
        
        case '0':
            send_addr_data(0x0001,0x00);
            break;
        case '1':
            send_addr_data(0x0002,0x11);
            break;
        case '2':
            send_addr_data(0x0004,0x22);
            break;
        case '3':
            send_addr_data(0x0008,0x33);
            break;
        case '4':
            send_addr_data(0x0010,0x44);
            break;
        case '5':
            send_addr_data(0x0020,0x55);
            break;
        case '6':
            send_addr_data(0x0040,0x66);
            break;
        case '7':
            send_addr_data(0x0080,0x77);
            break;
        case '8':
            send_addr_data(0x0100,0x88);
            break;
        case '9':
            send_addr_data(0x0200,0x99);
            break;
        case 'a':
            send_addr_data(0x0400,0xaa);
            break;
        case 'b':
            send_addr_data(0x0800,0xbb);
            break;
        case 'c':
            send_addr_data(0x1000,0xcc);
            break;
        case 'd':
            send_addr_data(0x2000,0xdd);
            break;
        case 'e':
            send_addr_data(0x4000,0xee);
            break;
        case 'f':
            send_addr_data(0x8000,0xff);
            break;
        case 'x':
            send_addr_data(0x0000,0x00);
            break;
        case 'y':
            send_addr_data(0xffff,0xff);
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
void main(void) {
    
    USART_Initialize();

    INTCONbits.PEIE = 1;
    
    USART_putstr("CPS Tecnologia\r\n");

    ei();
    
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 0;
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    
    for (;;){
        
        if (usart_has_char()){
            
            uint8_t c = usart_getch();
            USART_putcUSART(c);

            proc_addr(c);
            
            __delay_us(100);
        }
    }


    return;
}
