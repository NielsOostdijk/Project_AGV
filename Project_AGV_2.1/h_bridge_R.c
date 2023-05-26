/*
 * h_bridge.c - XvR 2020
 *
 * Use 8-bit timer. Uses interrupts in order to be able
 * to use the pins on the multifunction shield
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "h_bridge_L.h"

ISR(TIMER4_OVF_vect)
{
	if (OCR4A == 0 && OCR4B == 0)
	{
		PORT_RPWM &= ~(1<<PIN_RPWM);
		PORT_LPWM &= ~(1<<PIN_LPWM);
	}
	else if (OCR4A != 0)
	{
		PORT_LPWM &= ~(1<<PIN_LPWM);
		PORT_RPWM |= (1<<PIN_RPWM);
	}
	else if (OCR4B != 0)
	{
		PORT_RPWM &= ~(1<<PIN_RPWM);
		PORT_LPWM |= (1<<PIN_LPWM);
	}
}

ISR(TIMER4_COMPA_vect)
{
	if (OCR4A != 255)
	{
		PORT_RPWM &= ~(1<<PIN_RPWM);
	}
}

ISR(TIMER4_COMPB_vect)
{
	if (OCR4B != 255)
	{
		PORT_LPWM &= ~(1<<PIN_LPWM);
	}
}

void init_h_bridge_R(void)
{
	// Config pins as output
	DDR_RPWM |= (1<<PIN_RPWM);
	DDR_LPWM |= (1<<PIN_LPWM);

	// Output low
	PORT_RPWM &= ~(1<<PIN_RPWM);
	PORT_LPWM &= ~(1<<PIN_LPWM);

	// Use mode 0, clkdiv = 64
	TCCR4A = 0;
	TCCR4B = (0<<CS42) | (1<<CS41) | (1<<CS40);

	// Disable PWM output
	OCR4A = 0;
	OCR4B = 0;

	// Interrupts on OCA, OCB and OVF
	TIMSK4 = (1<<OCIE4B) | (1<<OCIE4A) | (1<<TOIE4);

	sei();
}

void h_bridge_set_percentage_R(signed char percentage)
{
	if (percentage >= -100 && percentage <= 100)
	{
		if (percentage >= 0)
		{
			// Disable LPWM, calculate RPWM
			OCR4B = 0;
			OCR4A = (255*percentage)/100;
		}
		else // percentage < 0
		{
			// Disable RPWM, calculate LPWM
			OCR4A = 0;
			OCR4B = (255*percentage)/-100;
		}
	}
}
