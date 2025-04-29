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

#define PGCLK_TRIS TRISAbits.TRISA1
#define PGDAT_TRIS TRISAbits.TRISA0
#define PGCLK      PORTAbits.RA1
#define PGDAT      PORTAbits.RA0

#define EECE_TRIS  TRISAbits.TRISA7
#define EEOE_TRIS  TRISAbits.TRISA6
#define EEWE_TRIS  TRISBbits.TRISB7
#define EECE       PORTAbits.RA7
#define EEOE       PORTAbits.RA6
#define EEWE       PORTBbits.RB7

#define VERCLK_TRIS TRISBbits.TRISB3
#define VERLOAD_TRIS TRISBbits.TRISB0
#define VERREAD_TRIS TRISAbits.TRISA4
#define VERCLK     PORTBbits.RB3
#define VERLOAD    PORTBbits.RB0
#define VERREAD    PORTAbits.RA4

////////////////////////////////////////////////////////////////////////////////
void io_init(){
    
    PGCLK = 0;
    PGDAT = 0;
    PGCLK_TRIS = 0;
    PGDAT_TRIS = 0;

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
    __delay_us(20);
    EECE = 0;
    EEOE = 0;
    __delay_us(20);
    VERLOAD = 0;
    __delay_us(2);
    VERLOAD = 1;
    __delay_us(2);
    EEOE = 1;
    EECE = 1;
    
    int i;
    for (i = 0; i < 8; i++){
        
        if (VERREAD)
            val |= msk;
        
        msk >>= 1;
        VERCLK = 1;
        VERCLK = 0;
        __delay_us(2);
    }
    
    return val;
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

    crlf();
    print_hex8(addr >> 8);
    print_hex8(addr & 0xff);
    USART_putcUSART(':');
    
    print_hex8(read_ee_data(addr));
    crlf();
}

////////////////////////////////////////////////////////////////////////////////
void main(void) {
    
    USART_Initialize();

    INTCONbits.PEIE = 1;
    
    USART_putstr("CPS Tecnologia\r\n");

    ei();
    
    io_init();
    
    uint16_t i;
    for (i = 0; i < 128; i++)
        dump_ee_data(i);
    
    USART_putstr("==================\r\n");
    dump_ee_data(0x51);
    
    send_addr_data(0x51,0);
    __delay_us(2);
    EECE = 0;
    EEOE = 0;

    for (;;){
        
        if (usart_has_char()){
            
            uint8_t c = usart_getch();
            USART_putcUSART(c);

            //dump_ee_data(0x51);

            
            __delay_us(100);
        }
    }


    return;
}
