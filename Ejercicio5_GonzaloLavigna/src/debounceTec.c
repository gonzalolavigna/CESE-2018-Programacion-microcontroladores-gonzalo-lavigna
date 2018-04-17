#include "sapi.h"								// <= Biblioteca sAPI
#include "debounceTec.h"
DEBUG_PRINT_ENABLE





typedef struct {
	gpioMap_t tecla;
	fsmDebounce_t state;
	bool_t pressed_pending_event;
	bool_t released_pending_event;
}fsmDebounceData_t;

gpioMap_t teclas[] = {TEC1,TEC2,TEC3,TEC4};
#define TECLAS_VALIDAS sizeof(teclas)/sizeof(gpioMap_t)

static fsmDebounceData_t teclaArray[TECLAS_VALIDAS];


void fsmDebounceInit(void){
	uint32_t i= 0;
	for(i=0; i < TECLAS_VALIDAS; i++){
		teclaArray[i].state = BUTTON_UP;
		teclaArray[i].tecla = teclas[i];
        teclaArray[i].pressed_pending_event = FALSE;
        teclaArray[i].released_pending_event = FALSE;
	}
}

void fsmDebounceUpdate(void){
	uint32_t i = 0;
	for(i=0; i < TECLAS_VALIDAS ; i++){
		switch(teclaArray[i].state){
		case BUTTON_UP:
			if(!gpioRead(teclaArray[i].tecla))
				teclaArray[i].state=BUTTON_FALLING;
			break;
		case BUTTON_FALLING:
			if(!gpioRead(teclaArray[i].tecla)){
				teclaArray[i].pressed_pending_event = TRUE;
				teclaArray[i].state=BUTTON_DOWN;
			}
			else
				teclaArray[i].state = BUTTON_UP;
			break;
		case BUTTON_DOWN:
			if(gpioRead(teclaArray[i].tecla))
				teclaArray[i].state = BUTTON_RAISING;
			break;
		case BUTTON_RAISING:
			if(gpioRead(teclaArray[i].tecla)){
				teclaArray[i].released_pending_event = TRUE;
				teclaArray[i].state =BUTTON_UP;
			}
			break;
		default:
			break;
		}
	}
}

fsmDebounce_t getTeclaState (buttonGpioIndex_t tecla_index){
	return teclaArray[tecla_index].state;
}


bool_t getTeclaEventPressedWithClear (buttonGpioIndex_t tecla_index){
	if (teclaArray[tecla_index].pressed_pending_event == TRUE){
		teclaArray[tecla_index].pressed_pending_event = FALSE;
		return TRUE;
	}
	else return FALSE;

}
bool_t getTeclaEventReleasedWithClear (buttonGpioIndex_t tecla_index){
	if (teclaArray[tecla_index].released_pending_event == TRUE){
		teclaArray[tecla_index].released_pending_event = FALSE;
		return TRUE;
	}
	else return FALSE;
}


uint32_t getTeclaNumber (void){
	return TECLAS_VALIDAS;
}
