#define LEDS_TIME_PERIOD 10

typedef enum {
	LED_RED_INDICE = 0,
	LED_GREEN_INDICE,
	LED_BLUE_INDICE,
	LED_1_INDICE,
	LED_2_INDICE,
	LED_3_INDICE,
} ledGpioIndex_t;

typedef enum {
	LED_BLINK = 0,
	LED_BI_LEVEL_HIGH,
	LED_BI_LEVEL_LOW,
}ledGpioConfiguration_t;


void ledInit (void);
void ledUpdate (void);
void setLedConfiguration (ledGpioIndex_t led_index, ledGpioConfiguration_t led_configuration,uint32_t blink_period);
uint32_t getLedNumber (void);
