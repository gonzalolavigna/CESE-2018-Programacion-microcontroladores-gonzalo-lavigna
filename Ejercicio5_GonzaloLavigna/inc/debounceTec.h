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
buttonGpioStatus_t getTeclaStatus (buttonGpioIndex_t tecla_index);






