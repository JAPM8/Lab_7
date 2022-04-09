/*
 * File:   main_Prelab.c
 * Author: Javier Alejandro Pérez Marín
 * 
 * Contador en port A con botones de decremento e incremento en port B,
 * 
 * Created on 5 de abril de 2022, 11:55 PM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>

/*
 * PROTOTIPO DE FUNCIÓN
 */
void setup(void);

void __interrupt() isr(void){
    //Se revisa interrupción PORTB
    if (INTCONbits.RBIF){
        if (!PORTBbits.RB0){
            while(!RB0); //Antirebote
            PORTA ++;
            INTCONbits.RBIF = 0; //Limpieza de bandera 
        } else if (!PORTBbits.RB1){
            while(!RB1); //Antirebote
            PORTA --;
            INTCONbits.RBIF = 0; //Limpieza de banderas
        }
    }
    return;
}

void main(void) {  
   
    setup(); // Se pasa a configurar PIC
        
    while(1)
    {
       
    }
}

void setup(void){
    ANSEL = 0;
    ANSELH = 0;     //I/O DIGITALES
    
    OSCCONbits.IRCF = 0b0110;   //Oscilador interno de 4 MHz
    OSCCONbits.SCS = 1;         //Oscilador interno
    
    TRISA = 0;      //PORTA OUTPUT
    TRISBbits.TRISB0 = 1;      //RB0 & RB1 COMO INPUT
    TRISBbits.TRISB1 = 1;
    
    PORTA = 0x00;      //CLEAR DE PUERTO A  
    PORTB = 0x00;      //CLEAR DE PUERTO B
    
    OPTION_REGbits.nRBPU = 0; // Se habilitan pull-up de PORTB
    WPUBbits.WPUB0 = 1; //Se habilita pull de RB0
    WPUBbits.WPUB1 = 1; //Se habilita pull de RB1
    
    INTCONbits.GIE = 1; //Se habilitan interrupciones globales
    INTCONbits.RBIE = 1; //Se habilitan interrupciones del PORTB
    IOCBbits.IOCB0 = 1; //Se habilitan interrupción por cambio de estado de RB0
    IOCBbits.IOCB1 = 1; //Se habilitan interrupción por cambio de estado de RB1
    INTCONbits.RBIF = 0; //Limpieza de bandera de interrupción de IOCB
 }