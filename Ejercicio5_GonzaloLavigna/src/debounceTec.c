#include "sapi.h"								// <= Biblioteca sAPI
#include "debounceTec.h"
DEBUG_PRINT_ENABLE

typedef enum {BUTTON_UP, BUTTON_FALLING, BUTTON_DOWN, BUTTON_RAISING} fsmDebounce_t;
typedef struct {
	gpioMap_t tecla;
	fsmDebounce_t state;
	delay_t delay;
}fsmDebounceData_t;

static fsmDebounceData_t dataTecla1,dataTecla2,dataTecla3,dataTecla4;
static uint8_t button_status[] = {TECLA_NO_PULSADA,TECLA_NO_PULSADA,TECLA_NO_PULSADA,TECLA_NO_PULSADA};

#define TECLAS_VALIDAS sizeof(button_status)/sizeof(uint8_t) - 1

static void debounce_tecla_1(void);
static void debounce_tecla_2(void);
static void debounce_tecla_3(void);
static void debounce_tecla_4(void);
static void fsmDebounceInit(fsmDebounceData_t * dataStruct, gpioMap_t dbnTec);
static void fsmDebounceUpdate(fsmDebounceData_t * ptrDataStruct);
static void buttonPressed(gpioMap_t tecla);
static void buttonReleased(gpioMap_t tecla);
static void updateTeclaStatus (buttonGpioStatus_t estado_tecla, buttonGpioIndex_t tecla_index);

void debounce_tecla_1(void){
	fsmDebounceUpdate(&dataTecla1);
}
void debounce_tecla_2(void){
	fsmDebounceUpdate(&dataTecla2);
}
void debounce_tecla_3(void){
	fsmDebounceUpdate(&dataTecla3);
}
void debounce_tecla_4(void){
	fsmDebounceUpdate(&dataTecla4);
}

void debounceInit (void){
	fsmDebounceInit(&dataTecla1,TEC1);
	fsmDebounceInit(&dataTecla2,TEC2);
	fsmDebounceInit(&dataTecla3,TEC3);
	fsmDebounceInit(&dataTecla4,TEC4);

}

void debounceUpdate (void){
	fsmDebounceUpdate(&dataTecla1);
	fsmDebounceUpdate(&dataTecla2);
	fsmDebounceUpdate(&dataTecla3);
	fsmDebounceUpdate(&dataTecla4);
}

void fsmDebounceInit(fsmDebounceData_t * dataStruct, gpioMap_t dbnTec){
	dataStruct->tecla = dbnTec;
	dataStruct->state = BUTTON_DOWN;
	return;
}

void fsmDebounceUpdate(fsmDebounceData_t * ptrDataStruct){
	switch(ptrDataStruct->state){
	case BUTTON_UP:
		if(!gpioRead(ptrDataStruct->tecla))
			ptrDataStruct->state=BUTTON_FALLING;
		break;
	case BUTTON_FALLING:
		if(!gpioRead(ptrDataStruct->tecla)){
			buttonPressed(ptrDataStruct->tecla);
			ptrDataStruct->state=BUTTON_DOWN;
		}
		else
			ptrDataStruct->state = BUTTON_UP;
		break;
	case BUTTON_DOWN:
		if(gpioRead(ptrDataStruct->tecla))
			ptrDataStruct->state = BUTTON_RAISING;
		break;
	case BUTTON_RAISING:
		if(gpioRead(ptrDataStruct->tecla)){
			buttonReleased(ptrDataStruct->tecla);
			ptrDataStruct->state=BUTTON_UP;
		}
		break;
	default:
		break;
	}
}

void buttonPressed(gpioMap_t tecla){
	switch(tecla){
	case TEC1:updateTeclaStatus(TECLA_PULSADA,TEC1_INDICE); break;
	case TEC2:updateTeclaStatus(TECLA_PULSADA,TEC2_INDICE); break;
	case TEC3:updateTeclaStatus(TECLA_PULSADA,TEC3_INDICE); break;
	case TEC4:updateTeclaStatus(TECLA_PULSADA,TEC4_INDICE); break;
	default: break;
	}
}

void buttonReleased(gpioMap_t tecla){

switch(tecla){
	case TEC1:updateTeclaStatus(TECLA_NO_PULSADA,TEC1_INDICE); break;
	case TEC2:updateTeclaStatus(TECLA_NO_PULSADA,TEC2_INDICE); break;
	case TEC3:updateTeclaStatus(TECLA_NO_PULSADA,TEC3_INDICE); break;
	case TEC4:updateTeclaStatus(TECLA_NO_PULSADA,TEC4_INDICE); break;
	default: break;
	}
}

void updateTeclaStatus (buttonGpioStatus_t estado_tecla, buttonGpioIndex_t tecla_index){
	button_status[tecla_index]=estado_tecla;
}

buttonGpioStatus_t getTeclaStatus (buttonGpioIndex_t tecla_index){
	return button_status[tecla_index];
}
