#include "sapi.h"								// <= Biblioteca sAPI
#include "leds.h"
DEBUG_PRINT_ENABLE

#define BLINK_PERIOD_DEFAULT 10
#define BLINK_PERIOD_MAX 1000
#define BLINK_PERIOD_MIN 10

gpioMap_t leds[] = {LEDR,LEDG,LEDB,LED1,LED2,LED3};
#define LEDS_VALIDOS sizeof(leds)/sizeof(gpioMap_t)

typedef enum {BILEVEL_OFF = 0 , BILEVEL_ON} bilevelLedConfiguration_t;
typedef enum {BLINK_LED, BILEVEL_LED} fsmLed_t;
typedef struct {
	gpioMap_t led_position;
	fsmLed_t state;
	bilevelLedConfiguration_t bilevelConfiguracion;
	uint32_t tick_limit;
	uint32_t tick_counter;
}ledData_t;

ledData_t ledArray[LEDS_VALIDOS];

void ledInit (void){
	uint32_t i;
	for (i=0; i< LEDS_VALIDOS; i++){
		ledArray[i].led_position = leds[i];
		ledArray[i].state = BILEVEL_LED;
		ledArray[i].bilevelConfiguracion = BILEVEL_OFF;
		ledArray[i].tick_counter = 0;
		ledArray[i].tick_limit = BLINK_PERIOD_DEFAULT;
	}
}

void ledUpdate (void){
	uint32_t i = 0;
	for(i=0;i<LEDS_VALIDOS;i++){
		switch (ledArray[i].state) {
			case BLINK_LED:
				if(ledArray[i].tick_counter >= ledArray[i].tick_limit){
					ledArray[i].tick_counter = 0;
					gpioToggle(ledArray[i].led_position);
				}
				else {
					ledArray[i].tick_counter++ ;
				}
				break;
			case BILEVEL_LED:
				gpioWrite(ledArray[i].led_position,ledArray[i].bilevelConfiguracion);
				break;
			default:
				gpioWrite(ledArray[i].led_position,BILEVEL_OFF);
				ledArray[i].state = BILEVEL_LED;
				break;
		}
	}
}


void setLedConfiguration (ledGpioIndex_t led_index, ledGpioConfiguration_t led_configuration,uint32_t blink_period){
	if(led_index > LED_3_INDICE || led_index < LED_RED_INDICE){
		debugPrintlnString("LED MODULE:Indice de LED Incorrecto");
		return;
	}
	if (led_configuration == LED_BLINK && (blink_period > BLINK_PERIOD_MAX || blink_period < BLINK_PERIOD_MIN)){
		debugPrintlnString("LED MODULE:Blink Perior fuera de rango");
		return;
	}

	if(led_configuration == LED_BLINK){
		ledArray[led_index].state = BLINK_LED;
		ledArray[led_index].tick_limit = blink_period/LEDS_TIME_PERIOD;
		ledArray[led_index].tick_counter = 0;
		ledArray[led_index].bilevelConfiguracion = BILEVEL_OFF;
	}
	else if (led_configuration == LED_BI_LEVEL_HIGH || led_configuration == LED_BI_LEVEL_LOW){
		ledArray[led_index].state = BILEVEL_LED;
		ledArray[led_index].bilevelConfiguracion = (led_configuration == LED_BI_LEVEL_HIGH) ? BILEVEL_ON : BILEVEL_OFF;
		ledArray[led_index].tick_counter = 0;
		ledArray[led_index].tick_limit = BLINK_PERIOD_DEFAULT;
	}
	else {
		debugPrintlnString("LED MODULE:Configuracion de LED Incorrecta");
	}
}

uint32_t getLedNumber (void){
	return LEDS_VALIDOS;
}
