/*
 * FW_ADC.c
 *
 * Created: 13.6.2018 18:32:23
 * Author : fkla
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"
#include "i2c.h"

#define ADC_ADDRESS 0b1001101 

void setup(void)
{
    DDRB |= (1<<5);
    PORTB |= (1<<5);
    
    uart_init();
    i2c_init();
    
}

int main(void)
{
    /* Replace with your application code */
    
    setup();
    
    unsigned char ret;
    
    while (1) 
    {
        PORTB ^= (1<<5);
        //uart_puts("pokus\n");
        ret = i2c_start(0x48 + I2C_WRITE);
        i2c_stop();
        uart_puts("pokus\n");
        if ( ret )
        {
            uart_puts("error\n");
        }
        else
        {
            uart_puts("started OK\n");
        }
        _delay_ms(1000);
        
        
        uart_puts("pokus\n");
    }
}

