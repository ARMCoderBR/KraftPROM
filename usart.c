#include <xc.h>
#include "usart.h"

#define USARTRXSIZE 48
char buf_usartrx[USARTRXSIZE];
uint8_t usart_head = 0;
uint8_t usart_tail = 0;
uint8_t usart_qty = 0;

/******************************************************************************
 * Function:        void USART_Initialize(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine initializes the UART to the value set by Host Terminal Application
 *
 * Note:
 *
 *****************************************************************************/

void USART_Initialize()
{
    unsigned char c;

    di();

    usart_head = 0;
    usart_tail = 0;
    usart_qty = 0;
    //ANSELHbits.ANS11 = 0;    // Make RB5 pin digital
    RCSTAbits.SPEN = 1;
    UART_TRISRx=1;				// RX
    //ANSELCbits.ANSC7 = 0;       // Habilita input buffer.
    UART_TRISTx=0;				// TX
    TXSTA = 0x24;       	// TX enable BRGH=1
    RCSTA = 0x90;       	// Single Character RX

    int dwBaud = ((GetSystemClock()/16) / 19200) - 1;
    SPBRG = (uint8_t) dwBaud;
        
//        SPBRG = 0x71;
//        SPBRGH = 0x02;      	// 0x0271 for 48MHz -> 19200 baud
//    BAUDCON = 0x08;     	// BRG16 = 1
    c = RCREG;				// read
    PIE1bits.RCIE = 1;
//    IPR1bits.RCIP = 1;  // High prio.

    ei();

}//end USART_InitializeUSART

/******************************************************************************
 * Function:        void putcUSART(char c)
 *
 * PreCondition:    None
 *
 * Input:           char c - character to print to the UART
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Print the input character to the UART
 *
 * Note:
 *
 *****************************************************************************/
void USART_putcUSART(char c)
{
    TXREG = c;
}

/******************************************************************************
 * Function:        void USART_getcUSART(char c)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          unsigned char c - character to received on the UART
 *
 * Side Effects:    None
 *
 * Overview:        Print the input character to the UART
 *
 * Note:
 *
 *****************************************************************************/
unsigned char __USART_getcUSART ()
{
	char  c;

	if (RCSTAbits.OERR)  // in case of overrun error
	{                    // we should never see an overrun error, but if we do,
		RCSTAbits.CREN = 0;  // reset the port
		c = RCREG;
		RCSTAbits.CREN = 1;  // and keep going.
	}
	else
    {
		c = RCREG;
    }
// not necessary.  EUSART auto clears the flag when RCREG is cleared
//	PIR1bits.RCIF = 0;    // clear Flag
  
	return c;
}

void USART_close(){
    
    PIE1bits.RCIE = 0;
    RCSTAbits.SPEN = 0;
    UART_TRISRx=1;				// RX
    //ANSELCbits.ANSC7 = 1;       // Desabilita input buffer.
    UART_TRISTx=1;				// TX
}

void usart_isr(void){
    
    if (usart_qty == USARTRXSIZE){
        __USART_getcUSART();
        return;
    }
    
    usart_qty++;
    buf_usartrx[usart_head++] = __USART_getcUSART();
    if (usart_head == USARTRXSIZE)
        usart_head = 0;
}

char usart_has_char(void){
    
    return usart_qty;
}

char usart_getch(void){
    
    if (!usart_qty)
        return (0);
    
    di();
    usart_qty--;
    char c = buf_usartrx[usart_tail++];
    ei();
    
    if (usart_tail == USARTRXSIZE)
        usart_tail = 0;
    return c;
}
