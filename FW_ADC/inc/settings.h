/*
 * settings.h
 *
 * Created: 16.6.2018 8:05:30
 *  Author: fkla
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_

#define ADC_ADDRESS 0b10010000 

#define ADC_ENABLE_DDR      DDRC
#define TEST_LED_DDR        DDRB
#define PWR_LED_DDR         DDRB

#define ADC_ENABLE_PORT     PORTC
#define TEST_LED_PORT       PORTB
#define PWR_LED_PORT        PORTB

#define ADC_ENABLE_PIN      1
#define TEST_LED_PIN        5
#define PWR_LED_PIN         3




#endif /* SETTINGS_H_ */