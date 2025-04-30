/* 
 * File:   usart.h
 * Author: milton
 *
 * Created on 24 de Abril de 2025, 23:46
 */

#ifndef USART_H
#define	USART_H

#include <xc.h>
#include <pic.h>

#define CLOCK_FREQ 4000000
#define GetSystemClock() CLOCK_FREQ

#define UART_ENABLE   RCSTAbits.SPEN

#define UART_TRISTx   TRISBbits.TRISB2
#define UART_TRISRx   TRISBbits.TRISB1
#define UART_Tx       PORTBbits.RB2
#define UART_Rx       PORTBbits.RB1

// Use following only for Hardware Flow Control

#define RTS_TRIS     TRISAbits.TRISA3
#define RTS          PORTAbits.RA3

//#define mDataRdyUSART() PIR1bits.RCIF
#define mTxRdyUSART()   TXSTAbits.TRMT

/*********************************************************************
* Function: void USART_Initialize(void);
*
* Overview: Initializes USART (RS-232 port)
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void USART_Initialize();

void USART_close(void);

void usart_isr(void);

char usart_has_char(void);

char usart_getch(void);

void USART_putcUSART(char c);

void USART_putstr(char *s);

#endif	/* USART_H */

