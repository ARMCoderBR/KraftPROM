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
    //__delay_us(1);
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
        //__delay_us(1);
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
#define POLLED 1
int write_ee_data(uint16_t addr, uint8_t data){
    
    send_addr_data(addr, data);

    PGDATEN = 0;
    __delay_us(1);
    EECE = 0;
    EEWE = 0;
    __delay_us(1);      // Pulse width Twp
    EEWE = 1;
    EECE = 1;
    __delay_us(1);      // Hold Tdh
    PGDATEN = 1;

#if POLLED
    __delay_us(1);
    EECE = 0;
    EEOE = 0;
    __delay_us(1);
    VERLOAD = 0;
    VERLOAD = 1;
    EEOE = 1;
    EECE = 1;
    data >>= 7;
    uint16_t timeout = 5000;
    //__delay_us(10);
    while (VERREAD != data){
        EECE = 0;
        EEOE = 0;
        __delay_us(1);
        VERLOAD = 0;
        VERLOAD = 1;
        EEOE = 1;
        EECE = 1;
        __delay_us(100);
        --timeout;
        if (!timeout) return -1;
    }
#else
    __delay_us(5000);   //Twc
#endif
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
uint8_t getnibble(char c){

    if ((c >='0') && (c <= '9')) return c - '0';
    if ((c >='a') && (c <= 'f')) return c - 'a' + 10;
    if ((c >='A') && (c <= 'F')) return c - 'A' + 10;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
uint8_t bufprog[64];
uint8_t nbytes = 0;
uint8_t ctbytes = 0;
uint16_t paddr = 0;
uint8_t state = 0;
uint8_t type = 0;
uint8_t cksum = 0;
uint8_t cksum2 = 0;

void main(void) {
    
    USART_Initialize();

    INTCONbits.PEIE = 1;
    
    USART_putstr("EEPROM Programmer by Milton (ARMCoder)\r\n");
//    __delay_ms(1000);
//    USART_putstr("1\r\n");
//    __delay_ms(1000);
//    USART_putstr("2\r\n");

    ei();
    
    io_init();
    
    uint16_t i;

    read_ee_data(0);    // Força limpeza dos SHRs
    
    for (;;){
        
        if (usart_has_char()){
            
            uint8_t c = usart_getch();
            //USART_putcUSART(c);

            if ((c == 'x') /*&& (state >= 99)*/){
                state = 0;
                USART_putstr("\r\nReset\r\n");
            }
            
            if ((c == ':') && (state != 99)){
                
                nbytes = 0; state = 0; paddr = 0; type = 0; ctbytes = 0; cksum = 0; cksum2 = 0; continue;
            }

            if ((c == '<') && (state != 99)){
                
                nbytes = 0; state = 20; paddr = 0; type = 0; ctbytes = 0; cksum = 0; cksum2 = 0; continue;
            }
            
            switch(state){
                
                ////////////////////////////////////////////////////////////////
                // Programming from INTEL HEX Line
                case 0:
                    nbytes = getnibble(c);
                    break;
                case 1:
                    nbytes <<= 4;
                    nbytes |= getnibble(c);
                    cksum2 += nbytes;
                    break;

                case 2:
                    paddr = getnibble(c);
                    break;
                case 3:
                    paddr <<= 4;
                    paddr |= getnibble(c);
                    cksum2 += paddr;
                    break;
                case 4:
                    paddr <<= 4;
                    paddr |= getnibble(c);
                    break;
                case 5:
                    paddr <<= 4;
                    paddr |= getnibble(c);
                    cksum2 += paddr;
                    break;
            
                case 6:
                    type |= getnibble(c);
                    break;
                case 7:
                    type <<= 4;
                    type |= getnibble(c);
                    cksum2 += type;
                    break;
            
                case 8:
                    bufprog[ctbytes] = getnibble(c);
                    break;
                case 9:
                    bufprog[ctbytes] <<=4;
                    bufprog[ctbytes] |= getnibble(c);
                    cksum2 += bufprog[ctbytes];
                    ctbytes++;
                    break;

                case 10:
                    cksum = getnibble(c);
                    break;
                case 11:
                    cksum <<=4;
                    cksum |= getnibble(c);
                    if (!((cksum + cksum2)&0xFF)){
                        USART_putstr("\r\nChecksum OK\r\n");
                        USART_putstr("ADDR:");
                        print_hex8(paddr>>8);
                        print_hex8(paddr);
                        USART_putstr("  BS:");
                        print_hex8(nbytes);
                        crlf();
                        for (i = 0; i < nbytes; i++){
                            if (write_ee_data(paddr+i,bufprog[i]) < 0){
                                USART_putstr("Timeout\r\n");
                                state = 99;
                                break;
                            };
                        }
                        for (i = 0; i < nbytes; i++){
                            if (read_ee_data(paddr+i) != bufprog[i]){//dump_ee_data(paddr+i);
                                USART_putstr("VERIFY ERROR\r\n"); 
                                state = 99;
                                break;
                            }
                        }
                        if (state != 99)
                            USART_putstr("Verify OK.\r\n"); 
                    }
                    else{
                        USART_putstr("\r\nHEX Checksum ERR:");
                        print_hex8(cksum);
                        print_hex8(cksum2);
                        crlf();
                        state = 99;
                    }
                    break;
            
                ////////////////////////////////////////////////////////////////
                // Reading and outputting INTEL HEX Line
                case 20:
                    paddr = getnibble(c);
                    break;
                case 21:
                    paddr <<= 4;
                    paddr |= getnibble(c);
                    cksum = paddr;
                    break;
                case 22:
                    paddr <<= 4;
                    paddr |= getnibble(c);
                    break;
                case 23:
                    paddr <<= 4;
                    paddr |= getnibble(c);
                    cksum += paddr;
                    break;
                case 24:
                    nbytes = getnibble(c);
                    break;
                case 25:
                    nbytes <<= 4;
                    nbytes |= getnibble(c);
                    cksum += nbytes;

                    USART_putcUSART(':');
                    print_hex8(nbytes);
                    print_hex8(paddr>>8);
                    print_hex8(paddr&0xFF);
                    for (i = 0; i < nbytes; i++){
                        uint8_t c = read_ee_data(paddr+i);
                        print_hex8(c);
                        cksum += c;
                    }
                    print_hex8(1+(cksum^0xFF));
                    crlf();
                    break;

                    
                    
                    
                    
                    
            
            }

            switch(state){

                case 7:
                    if (nbytes)
                        state++;
                    else
                        state = 10;
                case 9:
                    if (ctbytes < nbytes)
                        state = 8;
                    else
                        state = 10;
                    break;
                case 99:
                    USART_putstr("\r\nPress 'x' to restart process...\r\n");
                    state++;
                    break;
                default:
                    state++;
            }
            
            //__delay_us(100);
        }
    }


    return;
}
