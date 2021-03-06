/*
 * File:   main_Lab.c
 * 
 * Author: Javier Alejandro P?rez Mar?n
 * Contador en port A con botones de decremento e incremento en port B,
 * contador con TMR0 en PORTC
 * 
 * Created on 6 de abril de 2022, 01:13 PM
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
#define _tmr0_value 217 //TMR0 a 10 ms

uint8_t cont_tmr0;

/*
 * PROTOTIPO DE FUNCI?N
 */
void setup(void);

void __interrupt() isr(void){
    //Se revisa interrupci?n PORTB
    if (INTCONbits.RBIF){
        if (!PORTBbits.RB0){
            PORTA ++;
        } else if (!PORTBbits.RB1){
            PORTA --;
        }       
        INTCONbits.RBIF = 0; //Limpieza de bandera
    }
    //Revisi?n de interrupci?n TMR0
    if (INTCONbits.T0IF){ 
       cont_tmr0++; //Contador de 10 ms
       if (cont_tmr0 == 10) { //Cuando se llegue a 100 ms se incrementa puerto C y se reinicia contador de 10 ms
           PORTC++;
           cont_tmr0 = 0;

       }
       INTCONbits.T0IF = 0; //Limpieza de bandera
       TMR0 = _tmr0_value;  //Reinicio de TMR0
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
    TRISC = 0;      //PORTC OUTPUT
    
    PORTA = 0x00;      //CLEAR DE PUERTO A  
    PORTB = 0x00;      //CLEAR DE PUERTO B
    PORTC = 0x00;      //CLEAR DE PUERTO C
    
    OPTION_REGbits.nRBPU = 0; // Se habilitan pull-up de PORTB
    WPUBbits.WPUB0 = 1; //Se habilita pull de RB0
    WPUBbits.WPUB1 = 1; //Se habilita pull de RB1
    
    //Timer0 Registers Prescaler= 256 - TMR0 Preset = 217 - Freq = 100.16 Hz - Period = 0.009984 seconds
    OPTION_REGbits.T0CS = 0;  // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock (CLKO) 1 = Transition on T0CKI pin
    OPTION_REGbits.PSA = 0;   // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    OPTION_REGbits.PS2 = 1;   // bits 2-0  PS2:PS0: Prescaler Rate Select bits
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;
    TMR0 = _tmr0_value;       // preset for timer register a 10 ms
    
    INTCONbits.GIE = 1;  //Se habilitan interrupciones globales
    INTCONbits.RBIE = 0; //Se habilitan interrupciones del PORTB
    INTCONbits.T0IE = 1; // Se habilita interrupciones del TMR0
    IOCBbits.IOCB0 = 1;  //Se habilitan interrupci?n por cambio de estado de RB0
    IOCBbits.IOCB1 = 1;  //Se habilitan interrupci?n por cambio de estado de RB1
    INTCONbits.RBIF = 0; //Limpieza de bandera de interrupci?n de IOCB
    INTCONbits.T0IF = 0; // Limpieza de bandera

}  