#define BUTTONS_TIME_PERIOD 40

typedef enum {
	TEC1_INDICE = 0,
	TEC2_INDICE,
	TEC3_INDICE,
	TEC4_INDICE,
} buttonGpioIndex_t;

typedef enum {
	TECLA_PULSADA = 0,
	TECLA_NO_PULSADA,
} buttonGpioStatus_t;

void debounceInit (void);
void debounceUpdate (void);
uint8_t getTeclaStatus (buttonGpioIndex_t tecla_index);

void debounce_tecla_1(void);
void debounce_tecla_2(void);
void debounce_tecla_3(void);
void debounce_tecla_4(void);




