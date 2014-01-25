
#define F_CPU (1000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "api.h"
#include "uart.h"

#define maxDataLength 8

//DemoNode Id
NODE_ID = NODE_demoNode2;

// Set up external interrupts for INT0 for any logical change
int initButton() {
    EICRA = _BV(ISC00) | _BV(ISC10) | _BV(ISC30);
    EIMSK = _BV(INT0) | _BV(INT1) | _BV(INT3);

    return(0);
}

void buttonScript(int val, uint8_t x, uint8_t y){
char cSREG = SREG; //Store SREG
    uint8_t* msg = (uint8_t*)malloc(1*sizeof(uint8_t));
    if (val) {
        *msg = x; // turn top LED on
    } else {
        *msg = y; // turn top LED off
    }
    sendCANmsg(NODE_demoNode1, MSG_demoMsg, msg, 1);
    free(msg);
SREG = cSREG;
}
ISR(INT0_vect) {
    buttonScript(PIND & _BV(PD6), 0b01, 0b00);
}

ISR(INT1_vect) {
buttonScript(PINB & _BV(PB2), 0b11, 0b10);
}

ISR(INT3_vect) {
buttonScript(PINC & _BV(PC0), 0b11, 0b10);
}

// TODO: change this method for each of the demo nodes
void handleCANmsg(uint8_t destID, uint8_t msgID, uint8_t* msg, uint8_t msgLen) {
    uint8_t cmd = *msg;
    //Turn both off first
    PORTB &= ~(_BV(PB4));
    PORTB &= ~(_BV(PB6));
    int ledOn = cmd & 0b01;
    int whichLED = cmd & 0b10;
    if (whichLED) {
        if (ledOn) {
            PORTB |= _BV(PB4);
        } else {
            PORTB &= ~(_BV(PB4));
        }
    } else {
        if (ledOn) {
            PORTB |= _BV(PB6);
        } else {
            PORTB &= ~(_BV(PB6));
        }
    }
}

int main (void) {
    DDRB |= 0xFF; // set all PORTB pins for output
    DDRB &= ~(_BV(PB2)); // set pin 16 for input
    DDRC &= ~(_BV(PC0)); // set pin 30 for input
    DDRD &= ~(_BV(PD6)); // set pin 14 for input

    sei(); // enable global interrupts    
    initCAN(NODE_ID); // initialize CAN bus
    initButton(); // intitialize button interrupts

    for (;;) {
        // listen for button presses forever
    }
}

