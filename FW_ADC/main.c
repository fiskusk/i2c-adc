/*
 * FW_ADC.c
 *
 * Created: 13.6.2018 18:32:23
 * Author : fkla
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "uart.h"
#include "i2c.h"


#define ADC_ADDRESS 0b10010000 

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
    
    char buffer[10];
    unsigned char ret;
    uint8_t buffer_i2c[2];
    
    while (1) 
    {
        PORTB ^= (1<<5);
        //uart_puts("pokus\n");
        ret = i2c_start(ADC_ADDRESS + I2C_READ);
        //uart_puts("pokus\n");
        if ( ret )
        {
            i2c_stop();
            uart_puts("error\n");
        }
        else
        {
            msb_voltage = i2c_readAck();
            lsb_voltage = i2c_readNak();
            voltage = ((lsb_voltage>>2) + (msb_voltage>>6));
            i2c_stop();
            sprintf_P(buffer, PSTR("%d\n"),voltage);
            uart_puts(buffer);
        }
        _delay_ms(1000);
    }
}

