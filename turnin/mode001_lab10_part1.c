/*              Name & E-mail: Michael O'Dea, mode001@ucr.edu
 *              Lab Section: 025
 *              Assignment: Lab #10  Exercise #1 
 *              Exercise Description: Connect LEDs to PB0, PB1, PB2, and PB3. In one 
 *              state machine (ThreeLEDsSM), output to a shared variable (threeLEDs) 
 *              the following behavior: set only bit 0 to 1, then only bit 1, then 
 *              only bit 2 in sequence for 1 second each. In a second state machine 
 *              (BlinkingLEDSM), output to a shared variable (blinkingLED) the 
 *              following behavior: set bit 3 to 1 for 1 second, then 0 for 1 second. 
 *              In a third state machine (CombineLEDsSM), combine both shared variables
 *              and output to the PORTB. Note: only one SM is writing to outputs. Do 
 *              this for the rest of the quarter.
 *
 *              I acknowledge all content contained herein, excluding 
 *              template or example code, is my own original work.
 */

//DEMO: https://drive.google.com/open?id=1DR7F9sHmE0N60czLelOpx77o_gBHYK-4

#ifdef _SIMULATE_
#include <avr/io.h>
#include <avr/interrupt.h>
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){

	TCCR1B = 0x0B;

	OCR1A = 125;

	TIMSK1 = 0x02;

	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;
}

void TimerOff(){

	TCCR1B = 0x00;

}

void TimerISR(){

	TimerFlag = 1;

}

ISR(TIMER1_COMPA_vect){

	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}

}

void TimerSet(unsigned long M){

	_avr_timer_M = M;

	_avr_timer_cntcurr = _avr_timer_M;

}

enum threeStates{BEGIN, PBZero, PBOne, PBTwo} threeState;

unsigned short i = 0x00;
unsigned char allLEDs = 0x00;

void Tick_ThreeLeds(){
	switch(threeState){
		case BEGIN:
		threeState = PBZero;
		break;
		
		case PBZero:
		if(i < 1000){
			i++;
			threeState = PBZero;
		}
		else{
			
			i = 0;
			threeState = PBOne;
		}
		break;
		
		case PBOne:
		if(i < 1000){
			i++;
			threeState = PBOne;
		}
		else{
			i = 0;
			threeState = PBTwo;
		}
		break;
		
		case PBTwo:
		if(i < 1000){
			i++;
			threeState = PBTwo;
		}
		else{
			i = 0;
			threeState = PBZero;
		}
		break;
		
		default:
		break;
	}

	switch(threeState){
		case BEGIN:
		break;
		
		case PBZero:
		allLEDs = 0x01;
		break;
		
		case PBOne:
		allLEDs = 0x02;
		break;
		
		case PBTwo:
		allLEDs = 0x04;
		break;

		default:
		break;
	}
}

enum blinkingLED{START, ON, OFF} blinkingState;

unsigned short j = 0x00;
unsigned char tick = 0x00;

void Tick_Blinking(){
	switch(blinkingState){
		case START:
		blinkingState = ON;
		break;
		
		case ON:
		if(j < 1000){
			j++;
			blinkingState = ON;
		}
		else{
			j = 0;
			blinkingState = OFF;
		}
		break;

		case OFF:
                if(j < 1000){
			j++;
                        blinkingState = OFF;
                }
                else{
                        j = 0;
			blinkingState = ON;
                }
                break;
		
		default:
		break;
	}

	switch(blinkingState){
		case START:
		break;

		case OFF:
		tick = 0x00;
		break;

		case ON:
		tick = 0x08;
		break;
		
		default:
		break;
	}
}

enum combinedStates{COMBINE, PB} combined;

unsigned char output = 0x00;

void Tick_Combined(){
	switch(combined){
		case COMBINE:
		combined = PB;
		break;
		
		case PB:
		break;

		default:
		break;
	}

	switch(combined){
		case COMBINE:
		break;

		case PB:
		output = tick | allLEDs;
		PORTB = output;
		break;
		
		default:
		break;
	}
}

int main(void){
	DDRB = 0xFF; PORTB = 0x00;
	threeState = BEGIN;
	blinkingState = START;
	combined = COMBINE;
	TimerSet(1);
	TimerOn();

    	while(1){
		Tick_ThreeLeds();
		Tick_Blinking();
		Tick_Combined();
        	while (!TimerFlag);
        	TimerFlag = 0;
    	}

return 0;

}





