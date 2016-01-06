/*************************************************************************
 *  Digital Clock - A simple digital clock project to help me learn PIC  *
 *  Created by mikemadealarms on January 6, 2016 at 4:38 PM              *
 * --------------------------------------------------------------------- *
 *  Last modified by mikemadealarms on January 6, at 4:38 PM             *
 *  Last modification made was: Add a few of the required functions      *
 *************************************************************************/

#include <xc.h>

/*******************
 *  Configuration  *
 *******************/

//Clock Configuration
#define CLOCK_SET_SPEED 150  //The speed at which the time frame that is being set increments, provides a delay to make use of the clock easier

/***************
 *  Variables  *
 ***************/

//Time Tracking Variables
unsigned char timeSeconds;  //Tracks the current half seconds of a minute and resets when a full second is completed
unsigned char timeMinutes;  //Tracks the current minute of the hour and resets when a full hour has passed
unsigned char timeHours;    //Tracks the current hour of the day and then resets when a day has passed

//Display Multiplexing Variables
unsigned char displayDigit;    //Represents the current digit in the display that is being multiplexed
unsigned char displayValue[];  //Holds the values that get written to the IO ports controlling the segment for each display that represent a number

//Clock Variables
unsigned int setTimer;  //Used as a counter to provide a delay in between time frame increments while setting the time of the clock

/***********
 *  Setup  *
 ***********/

//Setup Function, configures MCU modules, initializes variables, and set IO registers
void setup() {
    //Initialize Variables
    timeSeconds = 0;                                  //Initialize the variable the tracks the seconds and set its value to 0
    timeMinutes = 0;                                  //Initialize the variable the tracks the minutes and set its value to 0
    timeHours = 0;                                    //Initialize the variable the tracks the hours and set its value to 0
    
    displayDigit = 0;                                 //Initialize the variable that determines the current display that is being multiplexed and set it to 0
    displayValue[] = {0x00, 0x00, 0x00, 0x00, 0x00};  //Initialize the array that holds the values to be written to the IO ports linked to the display segments and turn off each display
    
    setTimer = CLOCK_SET_SPEED - 1;                  //Set the counter used to add a delay between increments of a time frame when setting the clocks time to the end of the cycle to start immediately
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
    
}

//Multiplex Display Function, multiplexes the next digit in the clocks display to display the current time
void multiplexDisplay() {
    
}

//Update Display Function, updates the data that is displayed on the display to be the same as the current time
void updateDisplay() {
    
}

/****************
 *  Interrupts  *
 ****************/

//Interrupt Function, called upon an interrupt of a module or PORTB interrupt within the MCU
void interrupt onInterrupt() {
    
}