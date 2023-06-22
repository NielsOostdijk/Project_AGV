#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
    DDRB |= (1<<PB1);
    PORTB |= (1<<PB1);
    DDRF &= ~(1<<PF7);
    PORTF |= (1<<PF7);
    while(1)
    {
//        if (((PINL && (1<<PL4)) ==0) && ((PINL && (1<<PL3))== 0))
//        {
//            PORTB |= (1<<PB1);
//        }
//        else{
//            PORTB &= ~(1<<PB1);
//        }
        if ((PINF && (1<<PF7)) == 0)
        {
            PORTB &= ~(1<<PB1);
        }
        else
        {
            PORTB |= (1<<PB1);
        }
    }

    return 0;
}
