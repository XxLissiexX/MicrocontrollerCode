/*
 * File:   sdp.c
 * Author: Brieoné Slimmors
 * Ally-sah MacGuile-MacGhoul
 * SwÆn Queer-éh 
 * 
 *
 * Created on November 17, 2015, 6:52 PM
 */

#include "xc.h"
#include "config.h"

void _10us_delay(int);
void ms_delay(int);
int Receive_from_Sensor(void);

int distance = 0;           //Sensor Distance value (inches))
int avgdist = 0;            //Running average of valid sensor distance values
int TIMER = 0;
int initial = 0;            //Used in setting average distance

int main(void) {
    T3CON = 0x8010;         //Timer 3 initialization (used in Receive function)
    TRISA = 0x0000;         //FOR EXPLORER LED TESTING REMOVE LATERZ
    TRISD = 0x0010;         //Configure all PORTD pins to output EXCEPT RD4
    PORTD = 0x0000;         //Initialize all PORTD pins as OFF
    PORTA = 0x0000;         //LEDs for Explorer board, remove later!!!
    
	while (1) {  
        PORTDbits.RD0 = 1;     //Begin Pulse TO Sensor on RD0
        _10us_delay(1);        //Not exact, but close enough! (~N+1 uS)
        PORTDbits.RD0 = 0;     //Stop Pulse TO Sensor on RD0
        
        distance = Receive_from_Sensor();  //Return Distance value (inches)
        
        //Below code compares to running average
        if (initial == 0) {
            avgdist = distance;
            initial = 1;
        }
        else if (distance < avgdist / 2) {
            PORTAbits.RA7 = 1;      //FAKEPOTHOLE (Furthest from Breakout)
        }
        else {
            PORTAbits.RA7 = 0;      //NOFAKEPOTHOLE (Furthest from Breakout)
            avgdist = (int) (avgdist + distance) / 2;
        }

        // Below code compares with 11 inches
        if (distance < 11) {     
            PORTAbits.RA1 = 1;  //FAKEPOTHOLE (Closer to Breakout)
        }
        if (distance > 11) {   //11 inches
            PORTAbits.RA1 = 0;  //NOFAKEPOTHOLE (Ctgloser to breakout)
        }
        while (TMR3 < 60000); //Wait till end of 30mS cycle
        ms_delay(5);       //Delay between pulses to get reading back. SHORTEN!
    }
    return 0;
}

void _10us_delay(int N) {
    T1CON = 0x8000;
    TIMER = (int) 160 * N;
    TMR1 = 0;
    while (TMR1 < TIMER) {}
}

void ms_delay(int N) {
	T2CON = 0x8030;// TMR2 enabled, prescalar 256, use CPU clock source
	if ( N > 0 && N <= 1048 ) {
		TIMER = (int) 62.5 * N;
		TMR2 = 0;//Reset timer
		while ( TMR2 < TIMER ) {}
	}
	else;
	return;
}

int Receive_from_Sensor(void) {
    int time = 0;
    while (PORTD == 0x0000);   //wait till echo goes high
    if (PORTDbits.RD4 == 1) {  //once the echo pin goes high (there's a delay)
        TMR3 = 0;              //set timer 3 to 0
        
        //while (PORTD == 0x0010 && TMR3 < 10000); //Echo High and EchoTime < 5mS
                                          //(ignore first 5mS, won't be pothole)
        while (TMR3 < 20000) {  //Listen for only 10mS
            if (PORTDbits.RD4 == 0) {         
                time = TMR3 * 8 / 16000000;
                distance = (int) (time / 2) / 148;  //INCHES
            }
        }
    }
    return distance;
}

