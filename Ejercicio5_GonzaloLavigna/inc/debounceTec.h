#define BUTTONS_TIME_PERIOD 40

typedef enum {
	TEC1_INDICE = 0,
	TEC2_INDICE,
	TEC3_INDICE,
	TEC4_INDICE,
} buttonGpioIndex_t;


typedef enum {
	BUTTON_UP = 0,
	BUTTON_FALLING,
	BUTTON_DOWN,
	BUTTON_RAISING
} fsmDebounce_t;

void fsmDebounceInit(void);
void fsmDebounceUpdate(void);
fsmDebounce_t getTeclaStatus (buttonGpioIndex_t tecla_index);
bool_t getTeclaEventPressedWithClear (buttonGpioIndex_t tecla_index);
bool_t getTeclaEventReleasedWithClear (buttonGpioIndex_t tecla_index);

uint32_t getTeclaNumber (void);






