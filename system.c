#include "xc.h"
#include "system.h"
#include "usart.h"

////////////////////////////////////////////////////////////////////////////////
void interrupt SYS_Interrupt(void)
{
    if (INTCONbits.T0IF){
//        timer0_int_handler();
    }

    if (PIR1bits.TMR1IF){

//        timer1_int_handler();
    }

    if (PIR1bits.RCIF)
        usart_isr();
}

