/*
This code is for the main function of the program.
It will keep the buzzer going until the user presses the button. (on/off switch)
Also, it will sent out a pulse to the sensor and then wait for the echo to come back.
it will then calculate the distance and change the pitch of the buzzer accordingly.
*/

#define F_CPU 10e6 // 8 MHz clock speed

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "segment_display.h"

#define TRIG PC5
#define ECHO PC4
#define BUZZ PA4

uint16_t watchdog;
uint16_t duration;

ISR(TIMER1_COMPA_vect)
{
	writeLedDisplay(duration);
}

void init_timer1(){
	TCCR1A = 0x00; // set timer 1 a to
	TCCR1B = (1 << CS12) | (1 << WGM12); //Compare mode and 256 prescaler
	TIMSK |= (1<< OCIE1A); //Init timer 1 a interupt
	OCR1A = 39062 / 10;
}

void buzz_ms(int duration)
{
	PORTA |= (1<<BUZZ);
	wait(duration);
	PORTA &= ~(1<<BUZZ);
	wait(duration);
}

// Main function
int main(void)
{
	setup_segment_display();
	watchdog = 0;
	duration = 0;

	DDRC |= (1 << TRIG);
	DDRC &= ~(0 << ECHO);
	DDRA |= (1 << BUZZ);
	
	init_timer1();
	
	sei(); //init interupts
	
	while (1)
	{
		PORTC |= (1 << TRIG); //TRIGGER HIGH
		_delay_us(30);
		PORTC &= ~(1 << TRIG); //IF TRIGGER HIGH, TRIGGER LOW
		_delay_us(30);
	
		while (PINC & (1 << ECHO))
		{
			watchdog++;
			duration++;
			
			if (watchdog > 20000)
			{
				break;
			}
			_delay_us(100);
		}

		if (watchdog < 20000)
		{
			
			buzz_ms(duration * 5);
			
		}
		duration = 0;
	}
}