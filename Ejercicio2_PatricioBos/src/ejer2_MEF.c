/*============================================================================
 * Autor: Patricio Bos
 * Licencia:
 * Fecha: 25/03/2018
 *===========================================================================*/

/*==================[inclusiones]============================================*/

#include "sapi.h"        // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

typedef enum {LED_ON, LED_OFF} fsmBlinky_t;
typedef enum {BUTTON_UP, BUTTON_FALLING, BUTTON_DOWN, BUTTON_RAISING} fsmDebounce_t;

/* Estructura de datos necesarios para la MEF antirebote*/
typedef struct{
	gpioMap_t tecla;
	fsmDebounce_t state;
	delay_t delay;
}debounceData_t;
/*==================[definiciones de datos internos]=========================*/

#define DEBOUNCE_TIME 40

/* Arreglo con los tiempo de encendido posibles, se cambian con TEC1*/
static uint16_t tiempoEncendido[] = {50,250,1000};
/* Arreglo con los leds posibles, se cambian con TEC2*/
static gpioMap_t leds[] = {LEDR,  LEDG,  LEDB, LED1, LED2, LED3};

/* Indices para recorrer los arreglos de tiempos y de leds*/
static uint8_t indiceTiempo = 0;
static uint8_t indiceLed = 0;

/* Tamaño del arreglo dividido el tamaño de un elemento individual.
 * Se resta uno porque el primer índice es cero*/
#define TIEMPOS_VALIDOS sizeof(tiempoEncendido)/sizeof(uint16_t) - 1
#define LEDS_VALIDOS sizeof(leds)/sizeof(gpioMap_t) - 1

/* variable de estado de la MEF que hace parpadear un led*/
fsmBlinky_t blinkyState = LED_ON;

debounceData_t dataTecla1,dataTecla2;

delay_t blinkyDelay;



/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
void fsmBlinky(void);
void fsmDebounceUpdate(debounceData_t *);
void fsmDebounceInit(debounceData_t *, gpioMap_t);
void buttonPressed(gpioMap_t);
void buttonReleased(gpioMap_t);

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void ){

	// ---------- CONFIGURACIONES ------------------------------

	// Inicializar y configurar la plataforma
	boardConfig();

	fsmDebounceInit(&dataTecla1, TEC1);
	fsmDebounceInit(&dataTecla2, TEC2);

	// ---------- REPETIR POR SIEMPRE --------------------------
	while( TRUE )
	{
		fsmBlinky();
		fsmDebounceUpdate(&dataTecla1);
		fsmDebounceUpdate(&dataTecla2);

	}

	// NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
	// directamenteno sobre un microcontroladore y no es llamado por ningun
	// Sistema Operativo, como en el caso de un programa para PC.
	return 0;
}



/*==================[definiciones de funciones internas]=====================*/

void fsmBlinky(void){

	switch(blinkyState){

	case LED_ON:
		/* Si el delay no está corriendo, lo configuro y cambio el estado del led*/
		if (!blinkyDelay.running){
			delayConfig(&blinkyDelay, tiempoEncendido[indiceTiempo]);
			/* para que el delay arranque hay que leerlo*/
			delayRead(&blinkyDelay);
			gpioToggle(leds[indiceLed]);
		}
		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
		if (delayRead(&blinkyDelay))
			blinkyState = LED_OFF;
		break;

	case LED_OFF:
		/* Si el delay no está corriendo, lo configuro y cambio el estado del led*/
		if (!blinkyDelay.running){
			delayConfig(&blinkyDelay, tiempoEncendido[indiceTiempo]);
			/* para que el delay arranque hay que leerlo*/
			delayRead(&blinkyDelay);
			gpioToggle(leds[indiceLed]);
		}
		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
		if (delayRead(&blinkyDelay))
			blinkyState = LED_ON;
		break;

		/* Condición de error, reinicio la máquina de estados*/
	default:
		blinkyState = LED_ON;
		break;


	}

	return;
}

void fsmDebounceInit(debounceData_t * dataStruct, gpioMap_t dbnTec){
	dataStruct->tecla = dbnTec;
	dataStruct->state = BUTTON_UP;
	return;
}

void fsmDebounceUpdate(debounceData_t * ptrDataStruct){
	switch(ptrDataStruct->state){
	case BUTTON_UP:
		if(!gpioRead(ptrDataStruct->tecla))
			ptrDataStruct->state = BUTTON_FALLING;
		break;

	case BUTTON_FALLING:
		if (!ptrDataStruct->delay.running){
			delayConfig(&ptrDataStruct->delay, DEBOUNCE_TIME);
			/* para que el delay arranque hay que leerlo*/
			delayRead(&ptrDataStruct->delay);
		}
		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
		if (delayRead(&ptrDataStruct->delay)) {
			if(!gpioRead(ptrDataStruct->tecla)) {
				buttonPressed(ptrDataStruct->tecla);
				ptrDataStruct->state = BUTTON_DOWN;
			}
			else
				ptrDataStruct->state = BUTTON_UP;
		}
		break;

	case BUTTON_DOWN:
		if(gpioRead(ptrDataStruct->tecla))
			ptrDataStruct->state = BUTTON_RAISING;
		break;

	case BUTTON_RAISING:
		if (!ptrDataStruct->delay.running){
			delayConfig(&ptrDataStruct->delay, DEBOUNCE_TIME);
			/* para que el delay arranque hay que leerlo*/
			delayRead(&ptrDataStruct->delay);
		}
		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
		if (delayRead(&ptrDataStruct->delay)) {
			if(gpioRead(ptrDataStruct->tecla)) {
				buttonReleased(ptrDataStruct->tecla);
				ptrDataStruct->state = BUTTON_UP;
			}
			else
				ptrDataStruct->state = BUTTON_DOWN;
		}
		break;

	default:
		break;
	}

	return;
}

void buttonPressed(gpioMap_t tecla){
	switch(tecla){
	case TEC1:
		break;
	case TEC2:
		break;
	default:
		break;
	}
	return;
}

void buttonReleased(gpioMap_t tecla){
	switch(tecla){
	case TEC1:
		if( indiceTiempo >= TIEMPOS_VALIDOS)
			/* El índice estaba en el valor máximo y debe reiniciarse */
			indiceTiempo = 0;
		else
			/* El índice era menor al valor máximo y debe incrementarse*/
			indiceTiempo++;
		break;
	case TEC2:
		/* Apago el led que estaba parpadeando*/
		gpioWrite(leds[indiceLed], OFF);


		if( indiceLed >= LEDS_VALIDOS) {
			/* El índice estaba en el valor máximo y debe reiniciarse */
			indiceLed = 0;
		}
		else
			/* El índice era menor al valor máximo*/
			indiceLed++;
		break;
	default:
		break;
	}
	return;
}
/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
