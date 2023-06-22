#include <avr/interrupt.h>
#include <avr/io.h>

volatile uint16_t agv_ultrasoon_LA = 0;
volatile uint16_t agv_ultrasoon_LV = 0;
volatile uint16_t agv_ultrasoon_voor = 0;
volatile uint8_t agv_ultrasoon_current_sensor = 1;

void agv_ultrasoon_init()
{
    cli();
    //PORTA = 0b11111111;
    //triger configuratie
    TCCR3A |= (1<<COM3A1) | (1<<WGM31);
    TCCR3B |= (1<<WGM33) | (1<<CS31) | (1<<WGM32);//wgm33 wgm32 wgm31
    ICR3 = 32768;
    OCR3A = 20;//10microsec
    OCR3B = 4500;// (10microsec + 480microsec+ marge) *2
    DDRE |= (1<<PE3);
    TIMSK3 |= (1<<ICIE3) | (1<<OCIE3B);
    //pin change interrupt:
    PCICR |= (1<<PCIE2);//enables pci 16 tot 23
    sei();
}


ISR(TIMER3_COMPB_vect)
{
    agv_ultrasoon_current_sensor = (agv_ultrasoon_current_sensor<<1);
    if (agv_ultrasoon_current_sensor == (1<<3))//0b00100000
    {
        agv_ultrasoon_current_sensor = 1;
    }
    PCMSK2 = agv_ultrasoon_current_sensor; //zet pin change aan na rising edge voor falling edge
    //PORTA = agv_ultrasoon_current_sensor;
    TIMSK3 |= (1<<OCIE3B); //zet timer max aan voor out of range detectie
}

ISR(TIMER3_CAPT_vect)
{
    if(agv_ultrasoon_current_sensor == 0b00000001)
    {
        agv_ultrasoon_LA = ((ICR3 - 4454)/4)*0.0343;
    }
    else if(agv_ultrasoon_current_sensor == 0b00000010)
    {
        agv_ultrasoon_LV = ((ICR3 - 4454)/4)*0.0343;
    }
    else if(agv_ultrasoon_current_sensor == 0b00000100)
    {
        agv_ultrasoon_voor = ((ICR3 - 4454)/4)*0.0343;
    }
    PCMSK2 = 0; //zet pin change uit voor rising edge
}

ISR(PCINT2_vect)
{
    if(agv_ultrasoon_current_sensor == 0b00000001)
    {
        agv_ultrasoon_LA = ((TCNT3 - 4454)/4)*0.0343;
    }
    else if(agv_ultrasoon_current_sensor == 0b00000010)
    {
        agv_ultrasoon_LV = ((TCNT3 - 4454)/4)*0.0343;
     }
    else if(agv_ultrasoon_current_sensor == 0b00000100)
    {
        agv_ultrasoon_voor = ((TCNT3 - 4454)/4)*0.0343;
    }
    TIMSK3 &= ~(1<<ICIE3); //zet timer3 overflow flag uit
    PCMSK2 = 0; //zet pin change uit voor rising edge
}
