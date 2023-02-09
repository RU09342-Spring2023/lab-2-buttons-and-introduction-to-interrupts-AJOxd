/*
 * OccupancyDetector.c
 *
 *  Created on: Feb 9th, 2023
 *      Author: Andrew O'Donnell
 */
#include <msp430.h>

#define ARMED_STATE 0
#define WARNING_STATE 1
#define ALERT_STATE 2
char ToggleEnable = 0x00;                       // Global Variable to track if the LED should be on or off

// Put some initialization here
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    // Configure GPIO. Red LED stuff
       P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
       P1DIR |= BIT0;                          // Set P1.0 to output direction
    // green led configuration
       P6OUT &= ~BIT6;                         // Clear P6.0 output latch for a defined power-on state
       P6DIR |= BIT6;                          // Set P6.0 to output direction

char state = ARMED_STATE;

int i = 0;

// setting up 4.1 button
P4OUT |= BIT1;                          // Configure P4.1 as pulled-up
P4REN |= BIT1;                          // P4.1 pull-up register enable
P4IES &= ~BIT1;                         // P4.1 Low --> High edge
P4IE |= BIT1;                           // P4.1 interrupt enabled

__bis_SR_register(GIE);                 // Enter LPM3 w/interrupt
P4IFG &= ~BIT1;                         // P4.1 IFG cleared

// Disable the GPIO power-on default high-impedance mode
// to activate previously configured port settings
PM5CTL0 &= ~LOCKLPM5;


while(1)
{
switch (state) {
    case ARMED_STATE:
    {
            // Do something in the ARMED state
            //P6OUT &= ~BIT6;
            P6OUT &= ~BIT6;
            __delay_cycles(100000);
            P6OUT ^= BIT6;
            __delay_cycles(3000000);
            i = 0;
            // If something happens, you can move into the WARNING_STATE
            // state = WARNING_STATE
            if (ToggleEnable){
                state = WARNING_STATE;
            }
            break;
        }
        case WARNING_STATE:
        {
            // Do something in the WARNING_STATE
            // green led off
            P6OUT &= ~BIT6;
            // red led set to on
            P1OUT ^= BIT0;
            // 0.5 second delay
            __delay_cycles(500000);
            // red led off
            P1OUT &= ~ BIT0;
            i = i + 1;
            ToggleEnable = 0x00;
            // set ToggleEnable to zero so it resets

            // if OD goes back to 0 move to alert state
            // if button goes to 0?
            if (i >= 500000000){
            P1OUT ^= BIT0;
            state = ALERT_STATE;
            }
            else if(ToggleEnable){
            state = ARMED_STATE;
            }
            break;
        }
        case ALERT_STATE:
        {
            ToggleEnable = 0x00;
            // code for when the P4.1 button is pressed state = ARMED_STATE
            if (ToggleEnable){
                state = ARMED_STATE;
            }
            break;

    }
  }
}
}

// Port 4 interrupt service routine
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;                         // Clear P4.1 IFG
    ToggleEnable ^= 0x01;                   // Enable if the toggle should be active
}
