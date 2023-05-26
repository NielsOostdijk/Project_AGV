/*
    uses counter0 8bit
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "clock.h"

void init_time(){
    cli();
    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0 = 6;
    TCCR0A |= (1 << WGM01);
    TCCR0B |= (1 << CS01) | (1 << CS00);//prescaler 64
    TIMSK0 |= (1 << TOIE0);
    sei();
    //clock 0 defineren
    //interupt setpoint vaststellen
}

int time_current_ms(int update){//0 = return, 1 = time++
    static unsigned long long int time_ms = 0;
    if (update == 1){
        time_ms++;
    }
    return(time_ms);
}

ISR(TIMER0_OVF_vect){
    TCNT0 = 6;
    time_current_ms(1);
}
