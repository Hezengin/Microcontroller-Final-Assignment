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

#define TRIG PC5
#define ECHO PC4
#define BUZZ PA4

uint16_t watchdog;
uint16_t duration;
volatile uint8_t buzzer_state = 0;

void wait(int ms)
{
	for (int i = 0; i < ms; i++)
	{
		_delay_ms(1);
	}
}

ISR(TIMER1_OVF_vect)
{
	buzzer_state = !buzzer_state;
	PORTA = (buzzer_state << BUZZ); 
}

ISR(TIMER3_OVF_vect)
{
	PORTB ^= (1 << PB2);
}

void set_timer1_interupt(int ms){
	TCNT1 = 39062 * ms/1000;
}

void init_timer1(){
	TCCR1A = 0x00;
	TCCR1B = (1 << 2); //256 prescaler
	TIMSK |= (1 << TOIE1); //Init timer 1 interupt
}

void init_timer3(){
	TCCR3A = 0x00;
	TCCR3B = (1 << 2); //256 prescaler
	ETIMSK |= (1 << TOIE3); //Init timer 3 interupt
}

void buzz_ms(int duration)
{
	PORTB |= (1<<BUZZ);
	wait(duration);
	PORTB &= ~(1<<BUZZ);
	wait(duration);
}

// Main function
int main(void)
{
	watchdog = 0;
	duration = 0;

	DDRC |= (1 << TRIG);
	DDRC &= ~(0 << ECHO);
	DDRA |= (1 << BUZZ);
	DDRB |= (1 << BUZZ);
	DDRB |= (1 << PB2);
	
	init_timer1();
	init_timer3();
	
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
			buzz_ms(duration * 10);
			//set_timer1_interupt(duration * 5);
		}
		duration = 0;
	}
}