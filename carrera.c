/*
 * File:   carrera.c
 * Author: Lourdes Ruiz
 *
 * Created on Jan 17, 2022, 1:40 PM
 * 
 * Descripcion: 
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
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <pic16f887.h>

// Define la frecuencia para poder usar los delays
#define _XTAL_FREQ 2000000

/*
 * Constantes
 */

#define _tmr0_value 60

/*
 * Variables 
 */

//Array para "tabla" de displays
unsigned char displays[10] =
{
    0b00111111,   
    0b00000110,   
    0b01011011,   
    0b01001111,   
    0b01100110,   
    0b01101101,   
    0b01111101,   
    0b00000111,  
    0b01111111,   
    0b01101111        
};

/*Array para contador de décadas (por medio de un contador se "navega" cada
 * elemento del array. 
*/
unsigned char counter[10] = {
    0X00, 0X01, 0X02, 0X04, 0X08, 0X10, 0X20, 0X40, 0X80    
};

uint8_t contador;
uint8_t semaforo;
uint8_t countdown;
uint8_t start_flag;
uint8_t antirrebote1;
uint8_t antirrebote2;
uint8_t antirrebote3;
uint8_t cont_decada;
uint8_t cont_decada2;
uint8_t player1;
uint8_t player2;
uint8_t flag;
        
/*
 * Prototipos de funciones
 */
void setup(void);

/*
 * Interrupcion 
 */

void __interrupt() isr (void)
{
    if(T0IF){ 
        
        contador++;   //contador de segundos
        
        if (contador == 10){
            if (countdown < 4 && countdown > 0){ //si countdown es menor a 4 y mayor a 0, empieza cuenta regresiva
                countdown --;
                   
            }
            contador = 0;
        }
        else if (countdown == 0 && start_flag == 1){ //si countdown ya es 0 y bandera de inicio está setteada
                start_flag = 0;  //se apaga la bandera de inicio
                flag = 1;        //se prende la bandera que habilita la "carrera"
  
            }
        }
        
        TMR0 = _tmr0_value;
        T0IF = 0;    
    }


/*
 * Codigo Principal
 */
void main (void)
{
    setup();
    PORTC = displays[0];  //display empieza en 0
    countdown = 0;        //valores iniciales
    antirrebote1 = 0;
    antirrebote2 = 0;
    antirrebote3 = 0;
    start_flag = 0;
    flag = 0;
    player1 = 0;
    while(1)
    {
        // loop principal
        
        // Botón de inicio
        if (PORTBbits.RB0 == 0){ //Lógica negada
            antirrebote1 = 1;    
        }   
        
        if (PORTBbits.RB0 == 1 && antirrebote1 == 1 && flag == 0){
            countdown = 3;   //empieza la cuenta regresiva
            start_flag = 1;
            antirrebote1 = 0;
        }
        
        PORTC = displays[countdown];   //display enseña dicha cuenta
        
        //Sub rutina de semáforo
        if (countdown == 3){
            PORTBbits.RB3 = 1;
            PORTBbits.RB4 = 0;
            PORTBbits.RB5 = 0;
            flag = 0;
        }
        
        else if (countdown < 3 && countdown > 0){
            PORTBbits.RB3 = 0;
            PORTBbits.RB4 = 1;
            PORTBbits.RB5 = 0;
        }
        
        else if (countdown == 0 && start_flag == 1){
            PORTBbits.RB3 = 0;
            PORTBbits.RB4 = 0;
            PORTBbits.RB5 = 1;
            flag = 1;
            
        }
        
        //Botón de Jugador 1
        if (PORTBbits.RB1 == 0){
                antirrebote2 = 1;    
            }   

        if (PORTBbits.RB1 == 1 && antirrebote2 == 1){
            if (flag == 1){
                cont_decada++;
            } 
                antirrebote2 = 0;
            
        }
        
        //Botón de Jugador 2
        if (PORTBbits.RB2 == 0){
                antirrebote3 = 1;    
            }   

        if (PORTBbits.RB2 == 1 && antirrebote3 == 1){
                if (flag == 1){
                    cont_decada2++;  
                } 
                
                antirrebote3 = 0;
                       
        }
        
        //Sub rutina de Ganadores
        if (cont_decada == 9 || cont_decada2 == 9 && flag == 1){
                    if (cont_decada > cont_decada2){
                        player1 = 1;
                        PORTC = displays[player1];
                        PORTBbits.RB6 = 1;
                        __delay_ms(2000);
                        player1 = 0;
                        cont_decada = 0;       //se reinician los contadores
                        cont_decada2 = 0;
                        PORTBbits.RB6 = 0;
                    }
                    else if (cont_decada2 > cont_decada){
                        player2 = 2;
                        PORTC = displays[player2];
                        PORTBbits.RB7 = 1;
                        __delay_ms(2000);
                        player2 = 0;
                        cont_decada = 0;
                        cont_decada2 = 0;
                        PORTBbits.RB7 = 0;
                    }
                    flag = 0;  // flag en 0 para que el juego se reinicie
                }   
        PORTA = counter[cont_decada];  //muestra el contador de décadas de cada jugador
        PORTD = counter[cont_decada2];
    }
    
}

/*
 * Funciones
 */
void setup(void) 
{
    // Configuraciones de entradas y salidas 
    ANSEL = 0;  //RA0, RA1 y RA2 RA3 como entradas analógicas para ADC 
    ANSELH = 0;
    
    TRISA = 0;
    TRISB = 0b00000111;        //3 botones: 
    TRISC = 0;
    TRISD = 0;
    
    //Weak Pull-ups
    OPTION_REGbits.nRBPU = 0; //entrada negada
    WPUB = 0b0111;
    
    //valores iniciales
    PORTB = 0;
    PORTA = 0;
    PORTC = 0;
    PORTD = 0;
    
    //Configuracion de oscilador
    OSCCONbits.IRCF = 0b0101; //4MHz
    OSCCONbits.SCS = 1; //ocsilador interno
    
    //Configuracion de Timer0
    //Timer0 Registers Prescaler= 256 - TMR0 Preset = 60 - Freq = 9.96 Hz - Period = 0.100352 seconds
    OPTION_REGbits.T0CS = 0;  // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock (CLKO) 1 = Transition on T0CKI pin
    //OPTION_REGbits.T0SE = 0;  // bit 4 TMR0 Source Edge Select bit 0 = low/high 1 = high/low
    OPTION_REGbits.PSA = 0;   
    OPTION_REGbits.PS2 = 1;   // bits 2-0  PS2:PS0: Prescaler Rate Select bits (256)
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;
    
    TMR0 = _tmr0_value;             // 0.1ms
    T0IF = 0;
   
    //Configuracion de las interrupciones
    
    INTCONbits.T0IF = 0;
    INTCONbits.T0IE = 1;
    INTCONbits.PEIE = 1; //interrupciones perifericas
    INTCONbits.GIE  = 1;
   
    return;
    }



