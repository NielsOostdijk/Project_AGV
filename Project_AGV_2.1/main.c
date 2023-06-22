#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "clock.h"
#include "agv_ultrasoon.h"

#define IR_L PL4 //D45
#define IR_R PL3 //D46
#define LED PB1 //D52
#define knop PF7 //A7
//US_LV A9
//US_LA A8
//US_VOOR A10

#define motorL_PWM PH3 //D6
#define motorL_1 PE4 //D2
#define motorL_2 PE5 //D3
#define motorR_1 PE1 //D1
#define motorR_2 PG5 //D4
#define motorR_PWM PH4 //D7

/*
//vooruit
        h_bridgeL(100);
        h_bridgeR(100);
        PORTE |= (1 << motorL_1);
        PORTE &= ~(1 << motorL_2);
        PORTE &= ~(1 << motorR_1);
        PORTG |= (1 << motorR_2);
//links
        h_bridgeL(0);
        h_bridgeR(100);
        PORTE |= (1 << motorL_1);
        PORTE &= ~(1 << motorL_2);
        PORTE &= ~(1 << motorR_1);
        PORTG |= (1 << motorR_2);
//stop
        h_bridgeL(0);
        h_bridgeR(0);
        PORTE |= (1 << motorL_1);
        PORTE &= ~(1 << motorL_2);
        PORTE &= ~(1 << motorR_1);
        PORTG |= (1 << motorR_2);
*/

long duration;
int lane = 0;
int US_sens = 0;
int rijden = 0;
float overdracht_L;
float overdracht_R;
unsigned long long int boom_timer = 0;

void PIN_setup ()
{
    DDRB |= (1<<LED);
    PORTB |= (1<<LED);
    DDRF |= (1<<IR_L);
    DDRF |= (1<<IR_R);
}

void init_h_brug (void)
{
    //timer PWM motoren
    TCCR4A = (1<<COM4A1) | (1<<COM4A0) | (1<<COM4B1) | (1<<COM4B0) | (1<<COM4C1) | (1<<COM4C0) | (1<<WGM41) | (0<<WGM40); //COM1xn is de PWM instelling voor de pins
    TCCR4B = (1<<WGM43) | (1<<WGM42) | (0<<CS42) | (1<<CS41) | (0<<CS40); // CS1n is de prescaler 8 WGM1n is de PWM instelling voor timer 1 (fastPWM)
    OCR4A = 0;
    OCR4B = 0;
    ICR4 = 1260;

    //pinout h-brug
    DDRH |= (1 << motorL_PWM);
    DDRE |= (1 << motorL_1);
    DDRE |= (1 << motorL_2);
    DDRG |= (1 << motorR_1);
    DDRE |= (1 << motorR_2);
    DDRH |= (1 << motorR_PWM);

    //H-brug pinnen LOW
    PORTH &= ~(1 << motorL_PWM);
	PORTE &= ~(1 << motorL_1);
	PORTE &= ~(1 << motorL_2);
	PORTG &= ~(1 << motorR_1);
	PORTE &= ~(1 << motorR_2);
	PORTH &= ~(1 << motorR_PWM);
}

void h_bridgeR(int PercentKracht)
{
	if ((PercentKracht >= 0) && (PercentKracht <= 100))
        {
            OCR4A = ((ICR4/100)*PercentKracht);
        }
}

void h_bridgeL(int PercentKracht)
{
	if ((PercentKracht >= 0) && (PercentKracht <= 100))
        {
            OCR4B = ((ICR4/100)*PercentKracht);
        }
}

float PID (overdracht_L, overdracht_R)
{
    if (agv_ultrasoon_LV > agv_ultrasoon_LA)
    {
        overdracht_L = agv_ultrasoon_LV / agv_ultrasoon_LA;
    }
    if (agv_ultrasoon_LV < agv_ultrasoon_LA)
    {
        overdracht_R = agv_ultrasoon_LA / agv_ultrasoon_LV;
    }
}
void draaien()
{
    if ((agv_ultrasoon_LV >= 100) && (agv_ultrasoon_LA >= 100))
    {
        h_bridgeL(0);
        h_bridgeR(30*overdracht_R);
        PORTE |= (1 << motorL_1);
        PORTE &= ~(1 << motorL_2);
        PORTE &= ~(1 << motorR_1);
        PORTG |= (1 << motorR_2);
    }

    //draai 90* naar links
}

void boom_det (void)
{
    if ((!(PINF & (1<<IR_R))) || (!(PINF & (1<<IR_L))))
    {
        boom_timer = time_current_ms(0);
        if (time_current_ms(0) - boom_timer > 1500)
        {
            //stop motoren
            h_bridgeL(0);
            h_bridgeR(0);
            PORTE |= (1 << motorL_1);
            PORTE &= ~(1 << motorL_2);
            PORTE &= ~(1 << motorR_1);
            PORTG |= (1 << motorR_2);
            PORTB &= ~(1<<LED);
        }
    }
    //als ir sensor iets detecteert
        // alle motoren stoppen
        //led gaat aan
}

void obstakel_det (void)
{
    if (agv_ultrasoon_voor <= 50)
    {
        //motoren uit
        h_bridgeL(0);
        h_bridgeR(0);
        PORTE |= (1 << motorL_1);
        PORTE &= ~(1 << motorL_2);
        PORTE &= ~(1 << motorR_1);
        PORTG |= (1 << motorR_2);
    }
    else if ((agv_ultrasoon_voor >= 50) && (agv_ultrasoon_voor <=100))
    {
        //motoren op 20% snelheid
        h_bridgeL(30*overdracht_L);
        h_bridgeR(30*overdracht_R);
        PORTE |= (1 << motorL_1);
        PORTE &= ~(1 << motorL_2);
        PORTG &= ~(1 << motorR_1);
        PORTE |= (1 << motorR_2);
    }
    else
    {
        //motoren op normale snelheid
        h_bridgeL(60*overdracht_L);
        h_bridgeR(60*overdracht_R);
        PORTE |= (1 << motorL_1);
        PORTE &= ~(1 << motorL_2);
        PORTG &= ~(1 << motorR_1);
        PORTE |= (1 << motorR_2);
    }
}
void draaien_2 (void)
{
    if (agv_ultrasoon_LA <= 7)
    {
        if (US_sens == 0)
        {
           lane++;
           US_sens = 1;
        }
    }
    else
    {
        US_sens = 0;
    }
    if (lane == 3)
    {
        //naar links bewegen
        h_bridgeL(0);
        h_bridgeR(30*overdracht_R);
        PORTE |= (1 << motorL_1);
        PORTE &= ~(1 << motorL_2);
        PORTE &= ~(1 << motorR_1);
        PORTG |= (1 << motorR_2);
    }

}

int main(void)
{
    boom_det();
    PIN_setup();
    draaien();
    draaien_2();
    agv_ultrasoon_init();
    PID();
    init_h_brug();

    while(1)
    {
        switch (rijden)
        {
        case 0:
            if((PINF && (1<<knop)) != 0)
            {
                rijden = 1;
            }
        case 1:
            //beide motoren aan
            h_bridgeL(60*overdracht_L);
            h_bridgeR(60*overdracht_R);
            PORTE |= (1 << motorL_1);
            PORTE &= ~(1 << motorL_2);
            PORTG &= ~(1 << motorR_1);
            PORTE |= (1 << motorR_2);
            boom_det();
            rijden = 2;
            break;
        case 2:
            draaien();
            rijden = 3;
            break;
        case 3:
            draaien_2();
            rijden = 4;
            break;
        case 4:
            //bijde motoren weer aan
            h_bridgeL(60*overdracht_L);
            h_bridgeR(60*overdracht_R);
            PORTE |= (1 << motorL_1);
            PORTE &= ~(1 << motorL_2);
            PORTG &= ~(1 << motorR_1);
            PORTE |= (1 << motorR_2);
            boom_det();
            rijden = 5;
            break;
        case 5:
            //beide motoren aan
            boom_det();
            rijden = 6;
            break;
        case 6:
            draaien();
            rijden = 7;
            break;
        case 7:
            draaien_2();
            rijden = 0;
            break;
        }
    }

    return 0;
}
