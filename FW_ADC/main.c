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
    
    uint8_t buffer_uart[10];
    uint8_t buffer_i2c[2];
    uint16_t voltage;
    
    while (1) 
    {
        PORTB ^= (1<<5);
        if (i2c_start(ADC_ADDRESS + I2C_READ))
        {
            i2c_stop();
            uart_puts("error\n");
        }
        else
        {
            buffer_i2c[0] = i2c_readAck(); // MSB voltage
            buffer_i2c[1] = i2c_readNak(); // LSB voltage
            voltage = (buffer_i2c[1] >> 2) + ((uint16_t)(buffer_i2c[0] << 6));
            
            i2c_stop();
            sprintf_P(buffer_uart, PSTR("%d\n"),voltage);
            uart_puts(buffer);
        }
        _delay_ms(1000);
    }
}

