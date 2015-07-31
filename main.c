/* 
 * File:   Charliemain.c
 * Author: bjt
 *
 * Created on 27 de junio de 2015, 09:10 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#define _XTAL_FREQ 4000000      //Cristal de 4Mhz

// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config CP = OFF         // FLASH Program Memory Code Protection bits (Code protection off)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low Voltage In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection (Code Protection off)
#pragma config WRT = ON         // FLASH Program Memory Write Enable (Unprotected program memory may be written to by EECON control)

int lanzar=0;                   //bandera para lanzar el dado
int dado[2];                    //arreglo que me presenta los dos dados
int ciclo=0;                    //variable de tiempo para los ciclos de charlieplexing

void interrupt isr(void){ 
    
    if(RBIF){                       //¿es la interrupvion del puerto B?
        __delay_ms(160);            //espero bastante tiempo para que se establice la respuesta
        RBIF=0;                     //limpio la bandera de interrupcion
    
        if((!RB4)&&(lanzar==0)){    //la interrupcion se debió al pin 4
            RB0=1;                  //prendo led par
        }
    
        if(!RB5){                   //la interrupcion se debió al pin 4
            lanzar=1;               //bandera lanzar=1
        }
    }
    
    if(T0IF){                      //¿es la interrupcion del timer0?
        T0IF=0;                    //limipo la bandera de interrupcion
        ciclo++;                   //incrremento ciclo
    } 
    
}

void lanzamiento(void){         
    int x=0, y=0;
    srand(TMR1);                 //tomo la semilla de la función rand()
    for(y=0; y<2; y++){          //ciclo para que me guarde solo dos valores del dado
        x=(rand() % 6) +1;       //Me aseguro que los vlaores esten entre 1 y 6
        dado[y]=x;               //asigno el valor random al dado, dado es variable global
    }
}

void par_no_par(void){           //funcion para saber si la suma es par o impar.
    int z=0,residuo=0,a=0;
    z=dado[0]+dado[1];           //sumo el valor de los dos dados.
    residuo=z%2;                 //calculo el residuo si residuo es cero par sino impar.
    if((residuo==0)&&(RB0==1)){  //Es par y se selecionó par, ganastes.
        RB2=1;                   //Prendo led ganador.           
    }
    if((residuo==0)&&(RB0==0)){  //Es par y se selecionó impar, persites.
        RB1=1;                   //Prendo led perdedor
    }  
    if((residuo==1)&&(RB0==0)){  //Es impar y se selecionó impar, ganastes.
        RB2=1;                   //Prendo led ganador.
    }  
    if((residuo==1)&&(RB0==1)){  //Es impar y se selecionó par, persites.
        RB1=1;                   //Prendo led perdedor.
    }          
}

void dado0_1(void){             //multiplexing dado0 es 1   
    TRISC=0B00111111;
    PORTC=0B01000001;
    __delay_ms(1);
    PORTC=0B00000000;
}

void dado0_2(void){            //multiplexing dado0 es 2       
    TRISC=0B11111100;
    PORTC=0B00000010;
    __delay_ms(1);
    PORTC=0B00000000;
    TRISC=0B11110011;
    PORTC=0B00000100;
    __delay_ms(1);
    PORTC=0B00000000;
}

void dado0_3(void){             
    dado0_2();
    dado0_1();
}

void dado0_4(void){           
    dado0_2();
    TRISC=0B11111100;
    PORTC=0B00000001;
    __delay_ms(1);
    PORTC=0B00000000;
    TRISC=0B11110011;
    PORTC=0B00001000;
    __delay_ms(1);
    PORTC=0B00000000;
}

void dado0_5(void){           
    dado0_4();
    dado0_1();
}

void dado0_6(void){           
    dado0_4();
    TRISC=0B11111001;
    PORTC=0B00000010;
    __delay_ms(1);
    PORTC=0B00000000;
    PORTC=0B00000100;
    __delay_ms(1);
    PORTC=0B00000000; 
}

void dado1_1(void){           
    TRISC=0B00111111;
    PORTC=0B10000000;
    __delay_ms(1);
    PORTC=0B00000000;
}

void dado1_2(void){           
    TRISC=0B10011111;
    PORTC=0B00100000;
    __delay_ms(1);
    PORTC=0B00000000;
    TRISC=0B11100111;
    PORTC=0B00010000;
    __delay_ms(1);
    PORTC=0B00000000;
}

void dado1_3(void){           
    dado1_1();
    dado1_2();
}

void dado1_4(void){           
    dado1_2();
    TRISC=0B11100111;
    PORTC=0B00001000;
    __delay_ms(1);
    PORTC=0B00000000;
    TRISC=0B10011111;
    PORTC=0B01000000;
    __delay_ms(1);
    PORTC=0B00000000;
}

void dado1_5(void){           
    dado1_1();
    dado1_4();
}

void dado1_6(void){           
    dado1_4();
    TRISC=0B11001111;
    PORTC=0B00100000;
    __delay_ms(1);
    PORTC=0B00010000;
    __delay_ms(1);
    PORTC=0B00000000;
}

void plex(void){
    if((dado[0]==1)&&(dado[1]==1)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_1();
            dado1_1();
        }
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==1)&&(dado[1]==2)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_1();
            dado1_2();
        }
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==1)&&(dado[1]==3)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_1();
            dado1_3();
        }        
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==1)&&(dado[1]==4)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_1();
            dado1_4();
        }  
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==1)&&(dado[1]==5)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_1();
            dado1_5();
        }
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==1)&&(dado[1]==6)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_1();
            dado1_6();
        }  
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }

    ciclo=0;
    
    if((dado[0]==2)&&(dado[1]==1)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_2();
            dado1_1();
        }  
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
     
    if((dado[0]==2)&&(dado[1]==2)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_2();
            dado1_2();
        }  
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==2)&&(dado[1]==3)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_2();
            dado1_3();
        }  
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==2)&&(dado[1]==4)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_2();
            dado1_4();
        }  
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==2)&&(dado[1]==5)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_2();
            dado1_5();
        }  
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==2)&&(dado[1]==6)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_2();
            dado1_6();
        }  
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==3)&&(dado[1]==1)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_3();
            dado1_1();
        }  
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==3)&&(dado[1]==2)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_3();
            dado1_2();
        }  
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==3)&&(dado[1]==3)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_3();
            dado1_3();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==3)&&(dado[1]==4)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_3();
            dado1_4();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==3)&&(dado[1]==5)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_3();
            dado1_5();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==3)&&(dado[1]==6)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_3();
            dado1_6();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==4)&&(dado[1]==1)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_4();
            dado1_1();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==4)&&(dado[1]==2)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_4();
            dado1_2();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==4)&&(dado[1]==3)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_4();
            dado1_3();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==4)&&(dado[1]==4)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_4();
            dado1_4();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==4)&&(dado[1]==5)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_4();
            dado1_5();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==4)&&(dado[1]==6)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_4();
            dado1_6();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==5)&&(dado[1]==1)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_5();
            dado1_1();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==5)&&(dado[1]==2)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_5();
            dado1_2();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==5)&&(dado[1]==3)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_5();
            dado1_3();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==5)&&(dado[1]==4)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_5();
            dado1_4();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==5)&&(dado[1]==5)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_5();
            dado1_5();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==5)&&(dado[1]==6)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_5();
            dado1_6();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==6)&&(dado[1]==1)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_6();
            dado1_1();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==6)&&(dado[1]==2)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //46 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_6();
            dado1_2();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==6)&&(dado[1]==3)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_6();
            dado1_3();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==6)&&(dado[1]==4)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_6();
            dado1_4();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==6)&&(dado[1]==5)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_6();
            dado1_5();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
    
    ciclo=0;
    
    if((dado[0]==6)&&(dado[1]==6)){
        OPTION_REGbits.T0CS=0;     //selecciono la fuente del timer como interna, comienza a correr
        while(ciclo<92){           //92 ciclos son 6 seg de multiplexado para ver los resultados
            dado0_6();
            dado1_6();
        } 
        OPTION_REGbits.T0CS=1;     //selecciono la fuente del timer como externa evitar que corra
    }
}

int main() {
    //Configuración
    ADCON0bits.ADON=0;          //apago ADC
    TRISA=0B00000000;           //El puerto A como salida
    TRISB=0B00110000;           //El puerto B PIN 4 y 5 cmo entrada
    TRISC=0B00000000;           //El puerto C como salida
    PORTA=0B00000000;           //Valor incial del puerto A
    OPTION_REGbits.nRBPU=0;     //Habilito las weak pull-up del puerto B
    PORTB=0B00110000;           //Valor incial del puerto B
    PORTC=0B00000000;           //Valor incial del puerto C
    
    //habilitar interrupciones
    INTCONbits.RBIE=1;          //hablito la interrupcion del puertoB del 4 al 7
    INTCONbits.T0IE=1;          //habilito interrupción del timer0
    ei();                       //habilito interrupciones globales
    
    //configuracion del timer0
    OPTION_REGbits.T0CS=1;      //selecciono la fuente del timer como externa evitar que corra
    OPTION_REGbits.PSA=0;       //asigno prescaler al timer0
    OPTION_REGbits.PS=0b111;    //configuro prescaler como 256-> un tick cada 256us
                                //desborde cada 1us*256*256=65.536ms
    TMR1ON=1;                   // enciendo el timer1
    TMR1CS=0;                   //selecciono la fuente del timer como interna, comienza a correr
                        
    
    while(1){
        if(lanzar==1){
            lanzamiento();      //realizo el lanzamiento de los dados
            while(!RB5){};      //me quedo aquí hasta que se suelte el botón
            par_no_par();       //verifico si es par o no y doy el resultado de ganador
            plex();             //realizo el charlieplexing
            lanzar=0;           //inicializo otro lanzamiento 
            PORTC=0B00000000,   //apago todos los leds
            RB0=0;
            RB1=0;
            RB2=0;
        }  
    }
    return (0);                //devuelvo cero
}
