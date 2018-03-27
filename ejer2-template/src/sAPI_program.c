/*============================================================================
 * Autor:
 * Licencia:
 * Fecha:
 *===========================================================================*/

/*==================[inclusiones]============================================*/

#include "sapi.h"        // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

typedef enum {LED_ON, LED_OFF} fsmBlinky_t;
typedef enum {BUTTON_UP,BUTTON_FALLING,BUTTON_DOWN,BUTTON_RISING} fsmDebouncer_t;

/*==================[definiciones de datos internos]=========================*/

#define DELAY_DURATION 1000

fsmBlinky_t state = LED_ON;
fsmDebouncer_t state_t1 = BUTTON_DOWN;
fsmDebouncer_t state_t2 = BUTTON_DOWN;

delay_t blinkyDelay;
delay_t tec1DelayDebouncer;
delay_t tec2DdelayDebouncer;

/*==================[Tipos LED y Tipos de tiempo]=========================*/
#define T_1 50
#define T_2 100
#define T_3 1000

const uint16_t tiempoEncendido[] = {T_1,T_2,T_3};
const gpioMap_t leds[] = {LEDR,  LEDG,  LEDB, LED1 , LED2, LED3};

#define TIEMPOS_VALIDOS sizeof(tiempoEncendido)/sizeof(uint16_t) - 1
#define LEDS_VALIDOS sizeof(leds)/sizeof(gpioMap_t) - 1

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
void fsmBlinky(void);

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void ){

	// ---------- CONFIGURACIONES ------------------------------

	// Inicializar y configurar la plataforma
	boardConfig();

	// Crear varias variables del tipo booleano
	bool_t tec1Value = OFF;
	bool_t tec2Value = OFF;
	bool_t tec3Value = OFF;
	bool_t tec4Value = OFF;

	// ---------- REPETIR POR SIEMPRE --------------------------
	while( TRUE )
	{
		fsmBlinky();




	}

	// NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
	// directamenteno sobre un microcontroladore y no es llamado por ningun
	// Sistema Operativo, como en el caso de un programa para PC.
	return 0;
}



/*==================[definiciones de funciones internas]=====================*/

void fsmBlinky(void){

	switch(state){

	case LED_ON:
		/* Si el delay no está corriendo, lo configuro y cambio el estado del led*/
		if (!blinkyDelay.running){
			delayConfig(&blinkyDelay, DELAY_DURATION);
			/* para que el delay arranque hay que leerlo*/
			delayRead(&blinkyDelay);
			gpioToggle( LEDB );
		}
		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
		if (delayRead(&blinkyDelay))
			state = LED_OFF;
		break;

	case LED_OFF:
		/* Si el delay no está corriendo, lo configuro y cambio el estado del led*/
		if (!blinkyDelay.running){
			delayConfig(&blinkyDelay, DELAY_DURATION);
			/* para que el delay arranque hay que leerlo*/
			delayRead(&blinkyDelay);
			gpioToggle( LEDB );
		}
		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
		if (delayRead(&blinkyDelay))
			state = LED_ON;
		break;

	/* Condición de error, reinicio la máquina de estados*/
	default:
		state = LED_ON;
		break;


	}

	return;
}
/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
