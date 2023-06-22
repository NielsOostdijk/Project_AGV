#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#define motorL_PWM PH3
#define motorL_1 PE4
#define motorL_2 PE5
#define motorR_1 PE1
#define motorR_2 PG5
#define motorR_PWM PH4

/*
//vooruit
        h_bridgeL(100);
        h_bridgeR(100);
        PORTE |= (1 << motorL_1);
        PORTE &= ~(1 << motorL_2);
        PORTE &= ~(1 << motorR_1);
        PORTG |= (1 << motorR_2);
//achteruit
        PORTE |= (1 << motorL_1);
        PORTE &= ~(1 << motorL_2);
        PORTG &= ~(1 << motorR_1);
        PORTE |= (1 << motorR_2);
//rechts
        PORTE &= ~(1 << motorL_2);
        PORTE |= (1 << motorL_1);
        PORTE &= ~(1 << motorR_2);
        PORTG |= (1 << motorR_1);
//links
        h_bridgeL(0);
        h_bridgeR(100);
        PORTE |= (1 << motorL_1);
        PORTE &= ~(1 << motorL_2);
        PORTE &= ~(1 << motorR_1);
        PORTG |= (1 << motorR_2);
*/
void init_h_brug (void)
{
    //timer PWM motoren
    TCCR4A = (1<<COM4A1) | (1<<COM4A0) | (1<<COM4B1) | (1<<COM4B0) | (1<<COM4C1) | (1<<COM4C0) | (1<<WGM41) | (0<<WGM40); //COM1xn is de PWM instelling voor de pins
    TCCR4B = (1<<WGM43) | (1<<WGM42) | (0<<CS42) | (1<<CS41) | (0<<CS40); // CS1n is de prescaler 8 WGM1n is de PWM instelling voor timer 1 (fastPWM)
    OCR4A = 0;
    OCR4B = 0;
    ICR4 = 2000;
}
void h_bridgeL(int PercentKracht)
{
	if ((PercentKracht >= 0) && (PercentKracht <= 100))
        {
            OCR4A = ((ICR4/100)*(PercentKracht));

        }
}

void h_bridgeR(int PercentKracht)
{
	if ((PercentKracht >= 0) && (PercentKracht <= 100))
        {
            OCR4B = ((ICR4/100)*(PercentKracht));
        }
}

int main(void)
{
    init_h_brug();
    DDRF &= ~(1<<PF7);
    //pinout h-brug
    DDRH |= (1 << motorL_PWM);
    DDRE |= (1 << motorL_1);
    DDRE |= (1 << motorL_2);
    DDRE |= (1 << motorR_1);
    DDRG |= (1 << motorR_2);
    DDRH |= (1 << motorR_PWM);

    //H-brug pinnen LOW
    PORTH &= ~(1 << motorL_PWM);
	PORTE &= ~(1 << motorL_1);
	PORTE &= ~(1 << motorL_2);
	PORTE &= ~(1 << motorR_1);
	PORTG &= ~(1 << motorR_2);
	PORTH &= ~(1 << motorR_PWM);

    while(1)
    {

        h_bridgeL(0);
        h_bridgeR(100);
        PORTE |= (1 << motorL_1);
        PORTE &= ~(1 << motorL_2);
        PORTE &= ~(1 << motorR_1);
        PORTG |= (1 << motorR_2);

    }

    return 0;
}
