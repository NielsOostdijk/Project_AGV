#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "h_bridge.h"
//#include "clock.h"
#define IR_L PF3
#define IR_R PF4
#define USL_TRIGGER PB4
#define USL_ECHO PB5
#define USR_TRIGGER PB6
#define USR_ECHO PB7
#define USV_TRIGGER PL3
#define USV_ECHO PL4
#define SOUND 34300
#define LED PB1
#define DRAAI_DIST 7
long duration;
long distance_V;
long distance_R;
long distance_L;
unsigned long long int boom_timer = 0;

void LED_setup ()
{
    DDRB |= (1<<LED);
    PORTB |= (1<<LED);
}
void us_setup()
{
    DDRL |= (1 << USV_TRIGGER);
    DDRL &= ~(1 << USV_ECHO);
    DDRB |= (1 << USR_TRIGGER);
    DDRB &= ~(1 << USR_ECHO);
    DDRB |= (1 << USL_TRIGGER);
    DDRB &= ~(1 << USL_ECHO);
}

void usv_loop()
{
    PORTD |= (1 << USV_TRIGGER);
    _delay_us(10);
    PORTD &= ~(1 << USV_ECHO);
    // Wachten op de echo-puls
    while (!(PIND & (1 << USV_ECHO)));
    // Starttijd van de echo-puls vastleggen
    duration = 0;
    while (PIND & (1 << USV_ECHO))
    {
        _delay_us(1);
        duration++;
    }
    distance_V = duration * 0.034 / 2;
    _delay_ms(10);
}

void usr_loop()
{
    PORTD |= (1 << USR_TRIGGER);
    _delay_us(10);
    PORTD &= ~(1 << USR_ECHO);
    // Wachten op de echo-puls
    while (!(PIND & (1 << USR_ECHO)));
    // Starttijd van de echo-puls vastleggen
    duration = 0;
    while (PIND & (1 << USR_ECHO))
    {
        _delay_us(1);
        duration++;
    }
    distance_R = duration * 0.034 / 2;
    _delay_ms(10);
}

void usl_loop()
{
    PORTD |= (1 << USL_TRIGGER);
    _delay_us(10);
    PORTD &= ~(1 << USL_ECHO);
    // Wachten op de echo-puls
    while (!(PIND & (1 << USL_ECHO)));
    // Starttijd van de echo-puls vastleggen
    duration = 0;
    while (PIND & (1 << USL_ECHO))
    {
        _delay_us(1);
        duration++;
    }
    distance_L = duration * 0.034 / 2;
    _delay_ms(10);
}

void draaien()
{
    if ((distance_R > DRAAI_DIST) && (distance_R > DRAAI_DIST))
    {
        //rij 2 seconden verder
        //draai motor_L achteruit voor 0,5 sec
        //draai motor_R vooruit voor 0,5 sec
    }

    //draai 90* naar links
}

void boom_det ()
{
    if ((!(PINF & (1<<IR_R))) || (!(PINF & (1<<IR_L))))
    {
        boom_timer = time_current_ms(0);
        if (time_current_ms(0) - boom_timer > 1500)
        {
            h_bridge_set_percentage_R(0);
            h_bridge_set_percentage_L(0);
            PORTB &= ~(1<<LED);
        }
    }
    //als ir sensor iets detecteert
        // alle motoren stoppen
        //led gaat aan
}

void obstakel_det ()
{
    //als distance us sensor_voor kleiner is dan 10 cm
        //motoren gaan uit
    //anders
        //motoren draaien door
}

int main(void)
{
    usr_loop();
    usl_loop();
    usv_loop();
    us_setup();
    boom_det();
    LED_setup();

    while(1)
    {

    }

    return 0;
}
