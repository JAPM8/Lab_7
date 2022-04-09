/*
 * File:   main_Postlab.c
 * 
 * Author: Javier Alejandro Pérez Marín
 * 
 * Contador en port A con botones de decremento e incremento en port B,
 * contador de port A en display de 7 segmentos en port D y contador con TMR0 
 * en PORTC.
 * 
 * Created on 8 de abril de 2022, 02:41 PM
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

uint8_t cont_tmr0, //Contador de 10 ms
        banderas; //Bandera para selector
uint8_t val_div[3] = {0,0,0}; //Arreglo que almacenará unidades, decenas y centenas del contador de botones
uint8_t val_disp[3] = {0,0,0}; //Arreglo que almacenará unidades, decenas y centenas traducidas al display de 7 segmentos

//Arreglo para configuración 7 segmentos, donde la posición en el arreglo es el número que representa
uint8_t tabla_7seg [10] = {0b00111111, 0b00000110,0b01011011,0b01001111,0b01100110,0b01101101,0b01111101,0b00000111,0b01111111,0b01101111};

/*
 * PROTOTIPO DE FUNCIÓN
 */
void setup(void);
void division(uint8_t CONTEO); //FUNCIÓN PARA DIVISIÓN EN UNIDADES, DECENAS Y CENTENAS
void set_displays(uint8_t CONTEO_U, uint8_t CONTEO_D, uint8_t CONTEO_C); //TRADUCCIÓN DE CONTEO A 7 SEGMENTOS

void __interrupt() isr(void){
    //Se revisa interrupción PORTB
    if (INTCONbits.RBIF){
        if (!PORTBbits.RB0){
            PORTA ++;
        } else if (!PORTBbits.RB1){
            PORTA --;
        }       
        INTCONbits.RBIF = 0; //Limpieza de bandera
    }
    //Revisión de interrupción TMR0
    if (INTCONbits.T0IF){ 
       cont_tmr0++; //Contador de 10 ms
       if (cont_tmr0 == 10) { //Cuando se llegue a 100 ms se incrementa puerto C y se reinicia contador de 10 ms
           PORTC++; //Lab contador de 100 ms
           cont_tmr0 = 0;
       }
       
       PORTE = 0; //Limpieza selector
       if(banderas == 0){        
           PORTD = val_disp[0];  // Seteo de valor de unidades traducido 
           PORTE = 0b00000001;   // 3er selector
           banderas = 1; 
       }
       else if(banderas == 1){        
           PORTD = val_disp[1];    // Seteo de valor de decenas traducido
           PORTE = 0b00000010;  // 2do selector
           banderas = 2; 
       }
       else if(banderas == 2){        
           PORTD = val_disp[2];    // Seteo de valor de centenas traducido
           PORTE = 0b00000100;  // 3er selector
           banderas = 0; 
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
        division(PORTA); //Se pasa el valor de PORTA a la función de división
        set_displays(val_div[0],val_div[1],val_div[2]); //Traducción unidades, decenas y centenas
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
    TRISD = 0;      //PORTD OUTPUT
    TRISEbits.TRISE0 = 0; //PORTE RE0-RE2 COMO OUTPUT
    TRISEbits.TRISE1 = 0;
    TRISEbits.TRISE2 = 0;
    
    PORTA = 0x00;      //CLEAR DE PUERTO A  
    PORTB = 0x00;      //CLEAR DE PUERTO B
    PORTC = 0x00;      //CLEAR DE PUERTO C
    PORTD = 0x00;      //CLEAR DE PUERTO D
    PORTE = 0x00;      //CLEAR DE PUERTO E
    
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
    INTCONbits.RBIE = 0; //Se deshabilitan interrupciones del PORTB para asimilar un antirebote
    INTCONbits.T0IE = 1; // Se habilita interrupciones del TMR0
    IOCBbits.IOCB0 = 1;  //Se habilitan interrupción por cambio de estado de RB0
    IOCBbits.IOCB1 = 1;  //Se habilitan interrupción por cambio de estado de RB1
    INTCONbits.RBIF = 0; //Limpieza de bandera de interrupción de IOCB
    INTCONbits.T0IF = 0; // Limpieza de bandera

} 

void division(uint8_t CONTEO){
    val_div[2] = CONTEO/100; // División para obtención de Centenas
    val_div[1] = (CONTEO-val_div[2]*100)/10; // Operación para obtención de Decenas
    val_div[0] = CONTEO-val_div[2]*100-val_div[1]*10; // Operación para obtención de Unidades
    return;
}

void set_displays(uint8_t CONTEO_U, uint8_t CONTEO_D, uint8_t CONTEO_C){
    val_disp[0] = tabla_7seg[CONTEO_U]; //Traducción de unidades
    val_disp[1] = tabla_7seg[CONTEO_D]; //Traducción de decenas
    val_disp[2] = tabla_7seg[CONTEO_C]; //Traducción de centenas
    return;
}
