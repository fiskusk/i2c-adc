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
#include "settings.h"

typedef enum {
    ENABLE  = 1,
    DISABLE = !ENABLE,
} state_t;

state_t state =  DISABLE;
uint8_t instrukce[10];
uint8_t flag = 0;
uint8_t flag_one_second = 0;
uint8_t count = 0;


char* ui_one_decimal(uint16_t milinumber)
{
    uint8_t int_part;                       // integer part
    uint16_t dec_part;                      // decimal part
    static char buffer[20];
    
    //if ( milinumber <= 99 && milinumber >= 50 )
    //milinumber += 10;
    if ( (milinumber - (milinumber/10) * 10) >= 5)
    milinumber += 10;
    int_part = milinumber/1000;
    dec_part = (milinumber%1000)/10;
    
    // correct form message to print of LCD
    if (dec_part >=10)
        sprintf_P(buffer, PSTR("%1d,%2d V, %x, %x\n"),int_part,dec_part,instrukce[0], instrukce[1]);
    else
        sprintf_P(buffer, PSTR("%1d,0%1d V, %x, %x\n"),int_part,dec_part,instrukce[0], instrukce[1]);
       
    return buffer;
    
    } /* ui_one_decimal */

void adc_en(state_t state)
{
    if (state == ENABLE)
    {
        ADC_ENABLE_DDR |= (1<<ADC_ENABLE_PIN);
        ADC_ENABLE_PORT &= ~(1<<ADC_ENABLE_PIN);
    }
    else
    {
        ADC_ENABLE_DDR &= ~(1<<ADC_ENABLE_PIN);
        ADC_ENABLE_PORT &= ~(1<<ADC_ENABLE_PIN);
    }
    
}

void setup(void)
{
    TEST_LED_DDR |= (1<<TEST_LED_PIN);     // test LED on arduino
    TEST_LED_PORT |= (1<<TEST_LED_PIN);
    
    PWR_LED_DDR |= (1<<PWR_LED_PIN);     // pwr LED
    //PORTC |= (1<<0);
    ADC_ENABLE_DDR &= ~(1<<ADC_ENABLE_PIN);     // adc EN
    
    /* TIMER2 CS20 div 1; CS21 div 8; fast PWM on OCR2A */
	TCCR2A = (1<<COM2A1) | (1<<WGM20);
	TCCR2B |= (1<<CS21);
    
    TCCR0B = (1<<CS00) | (1<<CS02);     // prescaler 1024
    TIMSK0 |= (1<<TOIE0);               // interrupt from TC0 EN
    
    
    OCR2A = 0;
    
    uart_init();
    i2c_init();
    
    sei();
    
}

int main(void)
{
    setup();
    
    uint8_t buffer_i2c[2];
    uint16_t voltage;
    float voltage_float;
    
    while (1) 
    {
        while (flag_one_second)
        {
            TEST_LED_PORT ^= (1<<TEST_LED_PIN);
            //PORTC ^= (1<<0);
            adc_en(ENABLE);
            _delay_ms(15);
            if (i2c_start(ADC_ADDRESS + I2C_READ))
            {
                uart_puts("error\n");
            }
            else
            {
                buffer_i2c[0] = i2c_readAck(); // MSB voltage
                buffer_i2c[1] = i2c_readNak(); // LSB voltage
                voltage = (buffer_i2c[1] >> 2) + ((uint16_t)(buffer_i2c[0] << 6));
                voltage_float = voltage *(3326/1024.0) / (100.0/133);
                //voltage_float = 3.325;
                
                //sprintf_P((char *)buffer_uart, PSTR("%.2f V\n"),voltage_float);
                uart_puts(ui_one_decimal(voltage_float));
            }
            i2c_stop();
            adc_en(DISABLE);
            flag_one_second = 0;
            //instr = uart_getc();
        }
        if (flag)
        {
            flag = 0;
            if (instrukce[0] == 0x0E)
            {
                //uart_puts(instrukce[1]);
                OCR2A = instrukce[1];
            }
        }
    }
}

ISR(USART_RX_vect)
{
    //uart_puts("preruseni");
    char znak = UDR0;
    if ((znak != 0x0A) && (znak != 0x0D) && ( znak == 0x0E))
    {
        instrukce[0] = znak;
        //uart_puts("instrukce");
    }
    else if ((znak != 0x0A) && (znak != 0x0D) && ( znak >= 0x10))
    {
        instrukce[1] = znak;
        flag = 1;
        //uart_puts("data");
    }
    //else
    //{
        //instrukce[2] = '\0';
        ////uart_puts("null");
        //flag = 1;
    //}    
}

ISR(TIMER0_OVF_vect)
{
     if (++count == 61)
    {
        flag_one_second = 1;
        count = 0;
    }
}




