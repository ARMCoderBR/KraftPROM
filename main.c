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

#define PGCLK_TRIS   TRISAbits.TRISA1
#define PGDAT_TRIS   TRISAbits.TRISA0
#define PGDATEN_TRIS TRISBbits.TRISB4
#define PGCLK        PORTAbits.RA1
#define PGDAT        PORTAbits.RA0
#define PGDATEN      PORTBbits.RB4

#define EECE_TRIS    TRISAbits.TRISA7
#define EEOE_TRIS    TRISAbits.TRISA6
#define EEWE_TRIS    TRISBbits.TRISB5
#define EECE         PORTAbits.RA7
#define EEOE         PORTAbits.RA6
#define EEWE         PORTBbits.RB5

#define VERCLK_TRIS  TRISBbits.TRISB3
#define VERLOAD_TRIS TRISBbits.TRISB0
#define VERREAD_TRIS TRISAbits.TRISA4
#define VERCLK       PORTBbits.RB3
#define VERLOAD      PORTBbits.RB0
#define VERREAD      PORTAbits.RA4

////////////////////////////////////////////////////////////////////////////////
void io_init(){
    
    PGCLK = 0;
    PGDAT = 0;
    PGCLK_TRIS = 0;
    PGDAT_TRIS = 0;
    PGDATEN = 1;
    PGDATEN_TRIS = 0;
    
    EECE = 1;
    EEOE = 1;
    EEWE = 1;
    EECE_TRIS = 0;
    EEOE_TRIS = 0;
    EEWE_TRIS = 0;

    VERCLK = 0;
    VERLOAD = 1;
    VERCLK_TRIS = 0;
    VERLOAD_TRIS = 0;
    VERREAD_TRIS = 1;
}

////////////////////////////////////////////////////////////////////////////////
void crlf(){
    
    USART_putcUSART(13);
    USART_putcUSART(10);
}

////////////////////////////////////////////////////////////////////////////////
void send_addr_data(uint16_t addr, uint8_t data){
    
    int i;

    for (i = 0; i < 8; i++){
        
        if (data & 0x80)
            PGDAT=1;
        else
            PGDAT=0;
        PGCLK=1;
        PGCLK=0;
        data <<= 1;
    }

    for (i = 0; i < 16; i++){
        
        if (addr & 0x8000)
            PGDAT=1;
        else
            PGDAT=0;
        PGCLK=1;
        PGCLK=0;
        addr <<= 1;
    }
}

////////////////////////////////////////////////////////////////////////////////
uint8_t read_ee_data(uint16_t addr){
    
    uint8_t val = 0;
    uint8_t msk = 0x80;
    
    send_addr_data(addr,0);
    __delay_us(1);
    EECE = 0;
    EEOE = 0;
    __delay_us(1);
    VERLOAD = 0;
    VERLOAD = 1;
    EEOE = 1;
    EECE = 1;
    
    int i;
    for (i = 0; i < 8; i++){
        
        if (VERREAD)
            val |= msk;
        
        msk >>= 1;
        VERCLK = 1;
        VERCLK = 0;
        __delay_us(1);
    }
    
    return val;
}

const char dighex[] = "0123456789ABCDEF";
////////////////////////////////////////////////////////////////////////////////
void print_hex8(uint8_t c){

    uint8_t ch = c >> 4;
    c &= 0x0f;
    
    USART_putcUSART(dighex[ch]);
    USART_putcUSART(dighex[c]);
}

////////////////////////////////////////////////////////////////////////////////
void dump_ee_data(uint16_t addr){

    print_hex8(addr >> 8);
    print_hex8(addr & 0xff);
    USART_putcUSART(':');
    
    print_hex8(read_ee_data(addr));
    crlf();
}

////////////////////////////////////////////////////////////////////////////////
void write_ee_data(uint16_t addr, uint8_t data){
    
    send_addr_data(addr, data);

    PGDATEN = 0;
    
    EECE = 0;
    __delay_us(1);
    EEWE = 0;
    __delay_us(1);
    EEWE = 1;
    __delay_us(1);
    PGDATEN = 1;
    __delay_us(1000);

#if 0
    __delay_us(1);
    EEOE = 0;
    __delay_us(1);
    VERLOAD = 0;
    VERLOAD = 1;
    EEOE = 1;

    data >>= 7;
    while (VERREAD != data){
        
        EEOE = 0;
        __delay_us(1);
        VERLOAD = 0;
        VERLOAD = 1;
        EEOE = 1;
        __delay_us(1);
    }
#endif
    
    EECE = 1;
}

////////////////////////////////////////////////////////////////////////////////
void main(void) {
    
    USART_Initialize();

    INTCONbits.PEIE = 1;
    
    USART_putstr("CPS Tecnologia\r\n");

    ei();
    
    io_init();
    
    uint16_t i;

    for (;;){
        
        if (usart_has_char()){
            
            uint8_t c = usart_getch();
            USART_putcUSART(c);

            switch(c){
                
                case 'a':
                    send_addr_data(0x51,0);
                    __delay_us(2);
                    EECE = 0;
                    EEOE = 0;
                    break;
                
                case 'b':
                    EEOE = 1;
                    EECE = 1;
                    break;

                case 'c':
                    VERLOAD = 0;
                    __delay_us(2);
                    VERLOAD = 1;
                    break;

                case 'd':
                    VERCLK = 1;
                    VERCLK = 0;
                    break;

                case 'm':
                    crlf();
                    for (i = 0; i < 16; i++)
                        dump_ee_data(i);
                    break;

                case 'k':
                    write_ee_data(0x00,0x83);
                    write_ee_data(0x01,0x84);
                    break;
            }
            
            __delay_us(100);
        }
    }


    return;
}
