/*
 * File:   sdp.c
 * Author: Brieoné Slimmors
 * Ally-sah MacGuile-MacGhoul
 * SwÆn Queer-éh 
 * Eh-lizoo-bthu 2HÆmé-wheéllé
 * 
 *
 * Created on November 17, 2015, 6:52 PM
 */

#include "xc.h"
#include "config.h"

/*
 * Sensor Pins (For Final PCB) :
 *  A_TRIG_ALL = 
 *  B_TRIG_ALL =
 *  C_TRIG_ALL =
 *  A_PWR_ALL =
 *  B_PWR_ALL =
 *  C_PWR_ALL =
 * 
 * Currently Used for Testing :
 *  A_1_TRIG = RD0
 *  A_2_TRIG = RD1
 *  A_1_ECHO = RD4
 *  A_2_ECHO = RD5
 */
void _10us_delay(int);
void ms_delay(int);
int Receive_from_Sensor(void);

int i;                      //Loop Counter
int distance = 0;           //Sensor Distance value (inches))
int avgdist = 0;            //Running average of valid sensor distance values

int initial = 0;            //Used in setting average distance
int trigdsensor[2] = {0, 0};        //Variable set to determine what sensor is triggered

int main(void) {
    T3CON = 0x8010;        //Timer 3 initialization (used in Receive function)
    TRISA = 0x0000;        //LEDs FOR EXPLORER LED TESTING REMOVE LATERZ
    TRISD = 0x0000;        //Configure all PORTD pins to output
    TRISE = 0x0003;        //Configure all PORTE pins to output EXCEPT RE0, RE1
    PORTE = 0x0000;
    PORTD = 0x0000;        //Initialize all PORTD pins as OFF
    PORTA = 0x0000;        //LEDs FOR EXPLORER LED TESTING REMOVE LATERZ
    
	while (1) {  
        trigdsensor[0] = 0;       //Reset trigger values
        trigdsensor[1] = 0;
        PORTDbits.RD0 = 1;     //Begin Pulse TO Sensor on RD0 (Array 1)
        _10us_delay(1);        //Not exact, but close enough! (~N+1 uS) WRONG
        PORTDbits.RD0 = 0;     //Stop Pulse TO Sensor on RD0 (Array 1)
        
        distance = Receive_from_Sensor();  //Return Distance value (inches)

        // Below code compares with 10 Inches     
              //FAKEPOTHOLE (Closer to Breakout)
        if (trigdsensor[0] == 1){
            PORTAbits.RA1 = 1;
        }
        else if (trigdsensor[0] == 0){
            PORTAbits.RA1 = 0;  //NOFAKEPOTHOLE (Closer to breakout)
        }
        if (trigdsensor[1] == 1){
            PORTAbits.RA2 = 1;
        }
        else if (trigdsensor[1] == 0){
            PORTAbits.RA2 = 0;
        }
        while (TMR3 < 60000); //Wait till end of 30mS cycle
        ms_delay(10);       //Delay between pulses to get reading back. SHORTEN!
    }
    return 0;
}

void _10us_delay(int N) {
    int TIMER = 0;              //Used in Delay Functions
    T1CON = 0x8010;
    TIMER = (int) 160 * N;
    TMR1 = 0;
    while (TMR1 < TIMER) {}
}

void ms_delay(int N) {
    int TIMER = 0;              //Used in Delay Functions
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
    while (PORTE == 0x0000);
        TMR3 = 0;              //set timer 3 to 0
    while (TMR3 < 2960) {  //  2960 = ~10 inches
        if (PORTEbits.RE0 == 0) {
           trigdsensor[0] = 1;
        }
        if (PORTEbits.RE1 == 0) {
            trigdsensor[1] = 1;
        }
    }
    time = TMR3 * 8 / 16000000; //Time(sec) = TimerVal * prescaler / Fcyc
    distance = (int) (time * 10^6) / 148;  //INCHES = Time(uS) / 148
                                         
    return distance;
}

