/*
 * File:   main.c
 * Author: milton
 *
 * Created on 24 de Abril de 2025, 23:30
 */


#include <xc.h>
#include "system.h"

void main(void) {
    
    TRISAbits.TRISA0 = 1;
    __delay_us(10);
    return;
}
