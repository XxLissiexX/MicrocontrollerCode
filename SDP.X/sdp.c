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

// #include "xc.h"
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
 *  A_TRIG_ALL = RD0
 *  A_1_ECHO = RE0
 *  A_2_ECHO = RE1
 */
void _10us_delay(int);
void ms_delay(int);
void Receive_from_Sensor(void);

int i;                      //Loop Counter
int trigdsensor[2] = {0, 0};        //Variable set to determine what sensor is triggered

int main(void) {
    T3CON = 0x8010;        //Timer 3 initialization (used in Receive function)
    TRISA = 0x000C;        //LEDs FOR EXPLORER LED TESTING REMOVE LATERZ
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
        Receive_from_Sensor();  
        // Below code compares between 5 and 10 Inches
              
        if ((trigdsensor[0] == 0) && (trigdsensor[1] == 0)){
            PORTA = 0;  //0
        }
        else if ((trigdsensor[0] == 0) && (trigdsensor[1] == 1)){
            PORTA = 2;  //2
        }
        else if ((trigdsensor[0] == 1) && (trigdsensor[1] == 0)){
            PORTA = 1;  //1
        }
        // Filtering (Ex. Speed Bump)
        else if ((trigdsensor[0] == 1) && (trigdsensor[1] == 1)){
            PORTA = 0;  //0
        }
        while (TMR3 < 60000); //Wait till end of 30mS cycle
        ms_delay(20);       //Delay between pulses to get reading back. SHORTEN!
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


//    time = TMR3 * 8 / 16000000; //Time(sec) = TimerVal * prescaler / Fcyc
//    distance = (int) (time * 10^6) / 148;  //INCHES = Time(uS) / 148


void Receive_from_Sensor(void) {
    int prev0 = 0; //Previous Sensor 0 Value
    int prev1 = 0; //Previous Sensor 1 Value
    
    int tstart0;   //Sensor 0 Start Time
    int tstart1;   //Sensor 1 Start Time
    
    int tend0 = 0;  //Sensor 0 End Time
    int tend1 = 0;  //Sensor 1 End Time
    
//    int distance0;
//    int distance1;
//    
//    int time0;
//    int time1;
    
    TMR3 = 0;
    while (TMR3 < 10000) {  // While Time < 5mS
        if (PORTAbits.RA2 == 1 && prev0 == 0) {
            prev0 = 1;
            tstart0 = TMR3;
        }
        if (PORTAbits.RA3 == 1 && prev1 == 0) {
            prev1 = 1;
            tstart1 = TMR3;
        }
        if (PORTAbits.RA2 == 0 && prev0 == 1) {
            prev0 = 0;
            tend0 = TMR3;
        }
        if (PORTAbits.RA3 == 0 && prev1 == 1) {
            prev1 = 0;
            tend1 = TMR3;
        }
    }
    if (tend0 == 0) {
        tend0 = TMR3;   //If sensor times out, don't want to trigger
    }
    if (tend1 == 0) {
        tend1 = TMR3;   //If sensor times out, don't want to trigger
    }
    
//    time0 = (tend0 - tstart0) * 8 / 16000000; //Time(sec) = TimerVal * prescaler / Fcyc
//    time1 = (tend1 - tstart1) * 8 / 16000000; //Time(sec) = TimerVal * prescaler / Fcyc
//    distance0 = (int) (time0 * 10^6 / 148);
//    distance1 = (int) (time1 * 10^6 / 148);
//    
//    if ((5 < distance0) && (distance0 < 10)) { //2960 ~= 10 inches, 1480 ~= 5 inches
//        trigdsensor[0] = 1;
//    }
//    if ((5 < distance1) && (distance1 < 10)) { //2960 ~= 10 inches, 1480 ~= 5 inches
//        trigdsensor[1] = 1;
//    }

    if ((1480 < (tend0-tstart0)) && ((tend0 - tstart0) < 2960)) { //2960 ~= 10 inches, 1480 ~= 5 inches
    trigdsensor[0] = 1;
    }
    if ((1480 < (tend1 - tstart1)) && ((tend1 - tstart1) < 2960)) { //2960 ~= 10 inches, 1480 ~= 5 inches
        trigdsensor[1] = 1;
    }
}
