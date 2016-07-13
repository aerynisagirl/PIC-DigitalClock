/*************************************************************************
 *  Digital Clock - A simple digital clock project to help me learn PIC  *
 *  Created by mikemadealarms on January 6, 2016 at 4:38 PM              *
 * --------------------------------------------------------------------- *
 *  Last modified by mikemadealarms on June 13, 2016 at 2:31 PM          *
 *  Last modification made was: Minor cleaning and test of 12 Hour       *
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

//User Interface Configuration
#define TIME_SET_DELAY 0x7F;  //The lower the value, the faster the numbers will increment while using the buttons to set the time

//Display Mapping
const unsigned char DISPLAY_MAPPING[] = {
                                   0xFD,  //Digit mapping of 0
                                   0x30,  //Digit mapping of 1
                                   0x5E,  //Digit mapping of 2
                                   0x3E,  //Digit mapping of 3
                                   0x33,  //Digit mapping of 4
                                   0x2F,  //Digit mapping of 5
                                   0x6F,  //Digit mapping of 6
                                   0x34,  //Digit mapping of 7
                                   0x7F,  //Digit mapping of 8
                                   0x3F,  //Digit mapping of 9
                                   0x80   //Digit mapping of the colon
};

/***************
 *  Variables  *
 ***************/

//Time Tracking Variables
unsigned char timeSeconds = 0x00;  //Tracks the current half seconds of a minute and resets when a full second is completed
unsigned char timeMinutes = 0x00;  //Tracks the current minute of the hour and resets when a full hour has passed
unsigned char timeHours = 0x00;    //Tracks the current hour of the day and then resets when a day has passed

//Display Multiplexing Variables
unsigned char displayDigit = 0;                                 //Represents the current digit in the display that is being multiplexed
unsigned char displayValue[] = {0x00, 0x00, 0x00, 0x00, 0x00};  //Holds the values that get written to the IO ports controlling the segment for each display that represent a number

/***********
 *  Setup  *
 ***********/

//Setup Function, configures MCU modules, initializes variables, and set IO registers
void setup() {
    //Timing Related Registers
    OSCCON = 0x28;         //Set the internal oscillator to run at 125kHz
    OPTION_REG = 0x5F;     //Enable the internal weak pull-ups on PORTB and set the Timer0 module to run with no pre-scaler from the internal oscillator
    TMR1H = 0xFF;          //Set the highest byte of the Timer1 module counter
    TMR1L = 0xFF;          //Set the lowest byte of the Timer1 module counter
    T1CON = 0x8D;          //Enable the Timer1 module and set it to use an external low frequency crystal oscillator
    T2CON = 0x7F;          //Enable the Timer2 module and set the pre-scaler to count on every 64th clock cycle
    PR2 = TIME_SET_DELAY;  //Set the period register of Timer2 used to determine when an interrupt should occur to the value of the constant TIME_SET_DELAY 
    
    //Interrupt Related Registers
    INTCON = 0xE0;  //Enable interrupts, along with peripheral and Timer0 module overflow
    PIR1 = 0x00;    //Clear all the interrupt flags to prevent any false interrupts
    PIE1 = 0x03;    //Enable the Timer1 module overflow interrupt and the Timer2 module match interrupt
    
    //IO Port Related Registers
    TRISA = 0x00;   //Set all of PORTA to be outputs
    ANSELA = 0x00;  //Set all of PORTA to digital IO
    WPUA = 0x00;    //Disable all the internal weak pull-up resistors on PORTA
    LATA = 0x00;    //Set all of PORTA to logic LOW
    
    TRISB = 0xE0;   //Set PORTB0 to PORTB4 to outputs and leave the rest as inputs
    ANSELB = 0x00;  //Set all of PORTB to digital IO
    WPUB = 0xE0;    //Enable the internal weak pull-ups on PORTB5, PORTB6, and PORTB7
    LATB = 0x00;    //Set all of PORTB to logic LOW
    
    //Initialize Variables
    timeSeconds = 0x00;  //Clear the variable used for tracking seconds so it starts at the beginning
    timeMinutes = 0x00;  //Clear the variable used for tracking minutes so it starts at the beginning
    timeHours = 0x00;    //Clear the variable used for tracking hours so it starts at the beginning
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
    
    if (timeSeconds == 0x78) {
        timeSeconds = 0x00;  //Reset the seconds counter back to 0 and begin counting for a minute again
        
        timeMinutes++;  //Increment the minutes counter by 1 to indicate that a minute has passed
        
        //Check to see if an hour has passed
        if (timeMinutes == 0x3C) {
            timeMinutes = 0x00;  //Reset the minutes counter back to 0 and begin counting for an hour again
            
            timeHours++;  //Increment the hours counter by 1 to indicate that an hour has passed
            
            //Check to see if a day has passed
            if (timeHours == 0x18) {
                timeHours = 0x00;  //Reset the hours counter back to 0 and begin counting for a day again
            }
        }
    }
}

//Multiplex Display Function, multiplexes the next digit in the clocks display to display the current time
void multiplexDisplay() {
    displayDigit++;  //Increment the digit counter up by 1 to indicate what digit is to be multiplexed
    
    //Check to see if the digit actually exists
    if (displayDigit > 0x05) {
        displayDigit = 0x00;  //Reset the digit counter back to 0 and start the multiplexing process over again
    }
    
    //Clear the display to prevent any ghosting of other digits on the display
    LATA = 0x00;
    LATB = 0x00;
    
    //Write the next digit to the display
    LATA = displayValue[displayDigit];  //Write the segment values to the current digit being multiplexed
    LATB = 0x01 << displayDigit;       //Enable the display digit that currently being multiplexed to allow the segments to be written to it
}

//Update Display Function, updates the data that is displayed on the display to be the same as the current time
void updateDisplay() {
    //Toggle the colon
    if (timeSeconds % 0x02) {
        displayValue[0x04] = DISPLAY_MAPPING[0x0A];  //Turn on the colon if the seconds value is even
    } else {
        displayValue[0x04] = 0x00;  //Turn off the colon if the seconds value is odd
    }
    
    //Set the values of the digits on the display to be the current time
    displayValue[0x03] = DISPLAY_MAPPING[timeHours % 0x64 / 0x0A / 2];    //Set the first digit of the display to display the tens place value of the current hour
    displayValue[0x02] = DISPLAY_MAPPING[timeHours % 0x0A / 2];           //Set the second digit of the display to display the ones place value of the current hour
    displayValue[0x01] = DISPLAY_MAPPING[timeMinutes % 0x64 / 0x0A / 2];  //Set the third digit of the display to display the tens place value of the current minute
    displayValue[0x00] = DISPLAY_MAPPING[timeMinutes % 0x0A / 2];         //Set the fourth digit of the display to display the ones place value of the current minute
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
        TMR1H = 0xBF;   //Reset the highest byte of the Timer1 module counter
        TMR1L = 0xFD;   //Reset the lowest byte of the Timer1 module counter
        
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
    
    //Proceed only if the Timer1 module match flag has been thrown
    if (TMR2IF) {
        TMR2IF = 0x00;  //Clear the Timer2 module match interrupt flag to prevent false interrupts
        
        //Check to see if the minutes button is being held down
        if (RB6 && !RB7) {
            timeMinutes++;  //Increment the minutes counter by 1
            
            //Check to see if the minutes value is over an hour
            if (timeMinutes == 0x3C) {
                timeMinutes = 0x00;  //Reset the minutes counter back to 0
            }
            
            updateDisplay();  //Update the values on the display to reflect the newly set time
        }
        
        //Check to see if the hours button is being held down
        if (!RB6 && RB7) {
            timeHours++;  //Increment the hours counter by 1
            
            //Check to see if the hours value is over a day
            if (timeHours == 0x18) {
                timeHours = 0x00;  //Reset the hours counter back to 0
            }
            
            updateDisplay();  //Update the values on the display to reflect the newly set time
        }
    }
}