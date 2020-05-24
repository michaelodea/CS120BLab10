/*              Name & E-mail: Michael O'Dea, mode001@ucr.edu
 *              Lab Section: 025
 *              Assignment: Lab #10  Exercise #3
 *              Exercise Description: To the previous exercise's implementation
 *              , connect your speaker's red wire to PB4 and black wire to 
 *              ground. Add a third task that toggles PB4 on for 2 ms and off 
 *              for 2 ms as long as a switch on PA2 is in the on position. 
 *              Don’t use the PWM for this task.
 *
 *              I acknowledge all content contained herein, excluding 
 *              template or example code, is my own original work.
 */

//DEMO: https://drive.google.com/open?id=1DyZkGmSU2TwBshe0m0a_Ko6xMd4v6qz0

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
		if(i < 300){
			i++;
			threeState = PBZero;
		}
		else{
			
			i = 0;
			threeState = PBOne;
		}
		break;
		
		case PBOne:
		if(i < 300){
			i++;
			threeState = PBOne;
		}
		else{
			i = 0;
			threeState = PBTwo;
		}
		break;
		
		case PBTwo:
		if(i < 300){
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

enum speakerStates { INIT, SPEAKEROFF, SPEAKERON, SPEAKEROFFTWO} speakerState;

unsigned char speak = 0x00;

void Tick_Speaker(){
	switch(speakerState){
		case INIT:
		speakerState = SPEAKEROFF;
		break;

		case SPEAKEROFF:
		speakerState = (~PINA & 0x04) ? SPEAKERON : SPEAKEROFF;
		break;

		case SPEAKERON:
		speakerState = (~PINA & 0x04) ? SPEAKEROFFTWO : SPEAKEROFF;
                break;

		case SPEAKEROFFTWO:
		speakerState = (~PINA & 0x04) ? SPEAKERON : SPEAKEROFF;
                break;

		default:
		speakerState = INIT;
		break;
	}

	switch(speakerState){
		case INIT:
		speakerState = SPEAKEROFF;
		break;
		
		case SPEAKEROFF:
		speak = 0x00;
		break;
		
		case SPEAKERON:
		speak = 0x10;
		break;
	
		case SPEAKEROFFTWO:
		speak = 0x00;
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
		output = (tick | allLEDs | speak);
		PORTB = output;
		break;
		
		default:
		break;
	}
}

const unsigned long speakerP = 2;
unsigned long speakerT = 0;
const unsigned long timerP = 1;

int main(void){
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	threeState = BEGIN;
	blinkingState = START;
	speakerState = INIT;
	combined = COMBINE;
	TimerSet(timerP);
	TimerOn();

    	while(1){
		Tick_ThreeLeds();
		Tick_Blinking();
		if(speakerT >= speakerP){
			Tick_Speaker();
			speakerT = 0;
		}
		Tick_Combined();
        	while (!TimerFlag);
        	TimerFlag = 0;
		speakerT = speakerT + timerP;
    }

return 0;

}





