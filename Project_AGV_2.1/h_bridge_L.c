

#include <avr/io.h>
#include <avr/interrupt.h>
#include "h_bridge_R.h"

ISR(TIMER3_OVF_vect)
{
	if (OCR3A == 0 && OCR3B == 0)
	{
		PORT_RPWM &= ~(1<<PIN_RPWM);
		PORT_LPWM &= ~(1<<PIN_LPWM);
	}
	else if (OCR3A != 0)
	{
		PORT_LPWM &= ~(1<<PIN_LPWM);
		PORT_RPWM |= (1<<PIN_RPWM);
	}
	else if (OCR3B != 0)
	{
		PORT_RPWM &= ~(1<<PIN_RPWM);
		PORT_LPWM |= (1<<PIN_LPWM);
	}
}

ISR(TIMER3_COMPA_vect)
{
	if (OCR3A != 255)
	{
		PORT_RPWM &= ~(1<<PIN_RPWM);
	}
}

ISR(TIMER3_COMPB_vect)
{
	if (OCR3B != 255)
	{
		PORT_LPWM &= ~(1<<PIN_LPWM);
	}
}

void init_h_bridge_L(void)
{
	// Config pins as output
	DDR_RPWM |= (1<<PIN_RPWM);
	DDR_LPWM |= (1<<PIN_LPWM);

	// Output low
	PORT_RPWM &= ~(1<<PIN_RPWM);
	PORT_LPWM &= ~(1<<PIN_LPWM);

	// Use mode 0, clkdiv = 64
	TCCR3A = 0;
	TCCR3B = (0<<CS32) | (1<<CS31) | (1<<CS30);

	// Disable PWM output
	OCR3A = 0;
	OCR3B = 0;

	// Interrupts on OCA, OCB and OVF
	TIMSK3 = (1<<OCIE3B) | (1<<OCIE3A) | (1<<TOIE3);

	sei();
}

void h_bridge_set_percentage_L(signed char percentage)
{
	if (percentage >= -100 && percentage <= 100)
	{
		if (percentage >= 0)
		{
			// Disable LPWM, calculate RPWM
			OCR3B = 0;
			OCR3A = (255*percentage)/100;
		}
		else // percentage < 0
		{
			// Disable RPWM, calculate LPWM
			OCR3A = 0;
			OCR3B = (255*percentage)/-100;
		}
	}
}
