/*************************************************************************
 *  Digital Clock - A simple digital clock project to help me learn PIC  *
 *  Created by mikemadealarms on January 6, 2016 at 4:38 PM              *
 * --------------------------------------------------------------------- *
 *  Last modified by mikemadealarms on January 6, at 4:38 PM             *
 *  Last modification made was: Add a few of the required functions      *
 *************************************************************************/

#include <xc.h>

/***********************
 *  MCU Configuration  *
 ***********************/

//CONFIG1 Register
#pragma config FOSC = INTOSC   //Configure the MCU to use the internal oscillator and disable the CLKOUT function
#pragma config WDTE = OFF      //Disable the Watchdog Timer as it is not needed
#pragma config PWRTE = OFF     //Disable the Power-up Timer as it is not needed
#pragma config MCLRE = ON      //Enable the use of the MCLRE function on Pin 1 of the MCU
#pragma config CP = OFF        //Disable Code Protection as it is completely unecessary
#pragma config CPD = OFF       //Disable Data Code Protection as it is completely unecessary
#pragma config BOREN = ON      //Enable the Brown-out Reset function
#pragma config CLKOUTEN = OFF  //Disable the CLKOUT feature that is linked to Pin 10 of the MCU
#pragma config IESO = ON       //Allow the use of Internal/External Clock Switch Over feature
#pragma config FCMEN = ON      //Enable the Fail-Safe Clock Monitor as this is a time critical application

//CONFIG2 Register
#pragma config WRT = OFF     //Disable the Flash-memory Re-Write Protection feature as it is completely unecessary
#pragma config VCAPEN = OFF  //Disable the capacitor used by the internal voltage regulator as it is un necessary
#pragma config PLLEN = OFF   //Disable the PLL used for the internal oscillator as a high speed clock is not needed
#pragma config STVREN = ON   //Enable the Stack Overflow/Underflow Reset function to ensure the MCU doesn't jam up
#pragma config BORV = HI     //Set the Brown-Out Reset to occur when the high tripping point is present
#pragma config LVP = OFF     //Disable the Low-Voltage Programming feature of the MCU as it is not needed

/*******************
 *  Configuration  *
 *******************/

//Clock Configuration
#define CLOCK_SET_SPEED 150  //The speed at which the time frame that is being set increments, provides a delay to make use of the clock easier

//Display Mapping
unsigned char DISPLAY_MAPPING[] = {
                                   0xB7,  //Digit mapping of 0
                                   0x24,  //Digit mapping of 1
                                   0x73,  //Digit mapping of 2
                                   0x76,  //Digit mapping of 3
                                   0xE4,  //Digit mapping of 4
                                   0xD6,  //Digit mapping of 5
                                   0xD7,  //Digit mapping of 6
                                   0x34,  //Digit mapping of 7
                                   0xF7,  //Digit mapping of 8
                                   0xF6,  //Digit mapping of 9
                                   0x08   //Digit mapping of the colon
};

/***************
 *  Variables  *
 ***************/

//Time Tracking Variables
unsigned char timeSeconds = 0;  //Tracks the current half seconds of a minute and resets when a full second is completed
unsigned char timeMinutes = 0;  //Tracks the current minute of the hour and resets when a full hour has passed
unsigned char timeHours = 0;    //Tracks the current hour of the day and then resets when a day has passed

//Display Multiplexing Variables
unsigned char displayDigit = 0;                                 //Represents the current digit in the display that is being multiplexed
unsigned char displayValue[] = {0x00, 0x00, 0x00, 0x00, 0x00};  //Holds the values that get written to the IO ports controlling the segment for each display that represent a number

//Clock Variables
unsigned int setTimer = 0;  //Used as a counter to provide a delay in between time frame increments while setting the time of the clock

/***********
 *  Setup  *
 ***********/

//Setup Function, configures MCU modules, initializes variables, and set IO registers
void setup() {
    //Timing Related Registers
    OSCCON = 0x68;      //Set the internal oscillator to run at 125kHz
    OPTION_REG = 0x00;  //Enable the internal weak pull-ups on PORTB and set the Timer0 module to run with no pre-scaler from the internal oscillator
    TMR1H = 0xFF;       //Set the highest byte of the Timer1 module counter
    TMR1L = 0xFF;       //Set the lowest byte of the Timer1 module counter
    T1CON = 0x8D;       //Enable the Timer1 module and set it to use the internal low frequency oscillator
    
    //Interrupt Related Registers
    INTCON = 0xE0;  //Enable interrupts, along with peripheral, Timer0 module overflow, and PORTB change interrupts
    PIE1 = 0x01;    //Enable the Timer1 module overflow interrupt
    
    //IO Port Related Registers
    TRISA = 0x00;   //Set all of PORTA to be outputs
    ANSELA = 0x00;  //Set all of PORTA to digital IO
    WPUA = 0x00;    //Disable all the internal weak pull-up resistors on PORTA
    PORTA = 0x00;   //Set all of PORTA to logic LOW
    
    TRISB = 0xF0;   //Set PORTB0 to PORTB3 to outputs and leave the rest as inputs
    ANSELB = 0x00;  //Set all of PORTB to digital IO
    WPUB = 0x30;    //Enable the internal weak pull-ups on PORTB4 and PORTB5
    LATB = 0x00;    //Set all of PORTB to logic LOW
    
    //Initialize Variables
    timeSeconds = 0;
    timeMinutes = 0;
    timeHours = 0;
    
    setTimer = CLOCK_SET_SPEED - 1;  //Set the counter used to add a delay between increments of a time frame when setting the clocks time to the end of the cycle to start immediately
}

/****************
 *  Processing  *
 ****************/

//Main Function, called upon reset of the MCU
void main() {
    setup();  //Run setup script to ensure proper operation and functionality of the MCU
    
    //Run in a continuous loop to ensure the MCU doesn't go into sleep mode
    while (1) {
        //Do absolutely nothing
    }
}

//Update Clock Function, updates the clock to ensure the time is kept up to date
void updateClock() {
    timeSeconds++;  //Increment the seconds counter by 1 to indicate that half a second has passed
    //Check to see if a minute has passed
    if (timeSeconds == 120) {
        timeSeconds = 0;  //Reset the seconds counter back to 0 and begin counting for a minute again
        
        timeMinutes++;  //Increment the minutes counter by 1 to indicate that a minute has passed
        //Check to see if an hour has passed
        if (timeMinutes == 60) {
            timeMinutes = 0;  //Reset the minutes counter back to 0 and begin counting for an hour again
            
            timeHours++;  //Increment the hours counter by 1 to indicate that an hour has passed
            //Check to see if a day has passed
            if (timeHours == 24) {
                timeHours = 0;  //Reset the hours counter back to 0 and begin counting for a day again
            }
        }
    }
}

//Multiplex Display Function, multiplexes the next digit in the clocks display to display the current time
void multiplexDisplay() {
    displayDigit++;  //Increment the digit counter up by 1 to indicate what digit is to be multiplexed
    
    //Check to see if the digit actually exists
    if (displayDigit > 4) {
        displayDigit = 0;  //Reset the digit counter back to 0 and start the multiplexing process over again
        LATB = 0x00;       //Clear PORTB so that none of the digits on the display are enabled.
    } else {
        LATB = 1 << displayDigit - 1;  //Enable the display digit that currently being multiplexed to allow the segments to be written to it
    }
    
    LATA = ~displayValue[displayDigit];  //Write the segment values to the current digit being multiplexed
}

//Update Display Function, updates the data that is displayed on the display to be the same as the current time
void updateDisplay() {
    //Toggle the colon
    if (timeSeconds % 2) {
        displayValue[0] = DISPLAY_MAPPING[10];  //Turn on the colon if the seconds value is even
    } else {
        displayValue[0] = 0x00;  //Turn off the colon if the seconds value is odd
    }
    
    //Set the values of the digits on the display to be the current time
    displayValue[1] = DISPLAY_MAPPING[timeHours % 100 / 10];
    displayValue[2] = DISPLAY_MAPPING[timeHours % 10];
    displayValue[3] = DISPLAY_MAPPING[timeMinutes % 100 / 10];
    displayValue[4] = DISPLAY_MAPPING[timeMinutes % 10];
}

/****************
 *  Interrupts  *
 ****************/

//Interrupt Function, called upon an interrupt of a module or PORTB interrupt within the MCU
void interrupt onInterrupt() {
    //Proceed only if the Timer1 module overflow flag has been thrown
    if (TMR1IF) {
        TMR1IF = 0x00;  //Clear the Timer1 module overflow interrupt flag to prevent false interrupt
        
        //Set the Timer1 module to interrupt after half a second has passed in real time
        TMR1H = 0xEF;   //Reset the highest byte of the Timer1 module counter
        TMR1L = 0xFF;   //Reset the lowest byte of the Timer1 module counter
        
        //Update the real time clock and update the display to display the current time
        updateClock();    //Update the current time on the clock to be accurate with world time
        updateDisplay();  //Update the display digits to represent the current time being tracked by the clock
    }
    
    //Proceed only if the Timer0 module overflow flag has been thrown
    if (TMR0IF) {
        TMR0IF = 0x00;  //Clear the Timer0 module overflow interrupt flag to prevent false interrupts
        
        //Multiplex the next digit of the display
        multiplexDisplay();  //Multiplex the next digit of the display
    }
}