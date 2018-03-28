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
typedef struct {
	bool_t tecPresseed;
	bool_t tecReleased;
}tecEvents_t;

/*==================[definiciones de datos internos]=========================*/

#define DELAY_DURATION 1000

fsmBlinky_t state = LED_ON;
fsmDebouncer_t state_t1 = BUTTON_DOWN;
fsmDebouncer_t state_t2 = BUTTON_DOWN;

//Definicion de delays para el delay y tambien para los correspondientes debouncer
delay_t blinkyDelay;
delay_t tec1DelayDebouncer;
delay_t tec2DelayDebouncer;
//Definicion de eventos para las correspondientes teclas.
tecEvents_t tec1Events;
tecEvents_t tec2Events;

/*==================[Tipos LED y Tipos de tiempo]=========================*/
#define T_1 50
#define T_2 100
#define T_3 1000
#define T_Debouncer 40

const uint16_t tiempoEncendido[] = {T_1,T_2,T_3};
const gpioMap_t leds[] = {LEDR,  LEDG,  LEDB, LED1 , LED2, LED3};

#define TIEMPOS_VALIDOS sizeof(tiempoEncendido)/sizeof(uint16_t) - 1
#define LEDS_VALIDOS sizeof(leds)/sizeof(gpioMap_t) - 1

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
void fsmBlinky(uint8_t indiceLed, uint8_t indiceTiempo);
void fsmTec1Debouncer (void);
void fsmTec2Debouncer (void);

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void ){
	// ---------- CONFIGURACIONES ------------------------------
	// Inicializar y configurar la plataforma
	boardConfig();
	uint8_t indiceTiempo = 0;
	uint8_t indiceLed = 0;
	// Crear varias variables del tipo booleano
	tec1Events.tecPresseed = FALSE;
	tec1Events.tecReleased = FALSE;
	tec2Events.tecPresseed = FALSE;
	tec2Events.tecReleased = FALSE;


	// ---------- REPETIR POR SIEMPRE --------------------------
	while( TRUE )
	{
		fsmBlinky(indiceLed,indiceTiempo);
		fsmTec1Debouncer();
		fsmTec2Debouncer();
		if(tec1Events.tecPresseed == TRUE){
			if(indiceTiempo >= TIEMPOS_VALIDOS)
				indiceTiempo = 0;
			else
				indiceTiempo ++;
		}
		if(tec2Events.tecPresseed == TRUE){
			if(indiceLed >= LEDS_VALIDOS)
				indiceLed = 0;
			else
				indiceLed ++;
		}

	}

	// NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
	// directamenteno sobre un microcontroladore y no es llamado por ningun
	// Sistema Operativo, como en el caso de un programa para PC.
	return 0;
}



/*==================[definiciones de funciones internas]=====================*/

void fsmBlinky(uint8_t indiceLed, uint8_t indiceTiempo){
	static uint8_t indiceLedInternal = 0;
	static uint8_t indiceTiempoInternal = 0;

	switch(state){
	case LED_ON:
		/* Si el delay no está corriendo, lo configuro y cambio el estado del led*/
		if (!blinkyDelay.running){
			delayConfig(&blinkyDelay, tiempoEncendido[indiceTiempoInternal]);
			/* para que el delay arranque hay que leerlo*/
			delayRead(&blinkyDelay);
			gpioToggle( leds[indiceLedInternal] );
		}
		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
		if (delayRead(&blinkyDelay))
			state = LED_OFF;
		break;

	case LED_OFF:
		/* Si el delay no está corriendo, lo configuro y cambio el estado del led*/
		if (!blinkyDelay.running){
			delayConfig(&blinkyDelay, tiempoEncendido[indiceTiempoInternal]);
			/* para que el delay arranque hay que leerlo*/
			delayRead(&blinkyDelay);
			gpioToggle( leds[indiceLedInternal] );
		}
		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
		if (delayRead(&blinkyDelay)){
			state = LED_ON;
			indiceLedInternal = indiceLed;
			indiceTiempoInternal = indiceTiempo;
		}
		break;

	/* Condición de error, reinicio la máquina de estados*/
	default:
		state = LED_ON;
		break;


	}

	return;
}

void fsmTec1Debouncer (void) {
	switch(state_t1){
	case BUTTON_UP:
		tec1Events.tecPresseed = FALSE;
		tec1Events.tecReleased = FALSE;
		//Si la tecla no esta pulsada
		if (gpioRead(TEC1)){
			state_t1 = BUTTON_FALLING;
		}
		else {
			state_t1 = BUTTON_UP;
		}
		break;
	case BUTTON_FALLING:
		tec1Events.tecPresseed = FALSE;
		tec1Events.tecReleased = FALSE;
		//Si el delay no esta corriendo lo pongo a correr
		if(!tec1DelayDebouncer.running){
			delayConfig(&tec1DelayDebouncer,T_Debouncer);
			//Activo el delay para que empiece a correr
			delayRead(&tec1DelayDebouncer);
			state_t1 = BUTTON_FALLING;
		}
		//Tiempo expirado
		if(delayRead(&tec1DelayDebouncer)){
			//Si la tecla no esta pulsada
			if(gpioRead(TEC1)){
				tec1Events.tecReleased = TRUE;
				tec1Events.tecPresseed = FALSE;
				state_t1 = BUTTON_DOWN;
			}
			else {
				tec1Events.tecReleased = FALSE;
				tec1Events.tecPresseed = FALSE;
				state_t1 = BUTTON_UP;
			}
		}
		break;
	case BUTTON_DOWN:
		tec1Events.tecPresseed = FALSE;
		tec1Events.tecReleased = FALSE;
		//Si la tecla esta pulsada
		if (!gpioRead(TEC1)){
			state_t1 = BUTTON_RISING;
		}
		else {
			state_t1 = BUTTON_DOWN;
		}
		break;
	case BUTTON_RISING:
		tec1Events.tecPresseed = FALSE;
		tec1Events.tecReleased = FALSE;
		//Si el delay no esta corriendo lo pongo a correr
		if(!tec1DelayDebouncer.running){
			delayConfig(&tec1DelayDebouncer,T_Debouncer);
			//Activo el delay para que empiece a correr
			delayRead(&tec1DelayDebouncer);
			state_t1 = BUTTON_RISING;
		}
		//Tiempo expirado
		if(delayRead(&tec1DelayDebouncer)){
			//Si la tecla  esta pulsada
			if(!gpioRead(TEC1)){
				tec1Events.tecReleased = FALSE;
				tec1Events.tecPresseed = TRUE;
				state_t1 = BUTTON_UP;
			}
			else {
				tec1Events.tecReleased = FALSE;
				tec1Events.tecPresseed = FALSE;
				state_t1 = BUTTON_DOWN;
			}
		}
		break;
	default:
		state_t1 = BUTTON_DOWN;
		break;
	}
}

void fsmTec2Debouncer (void) {
	switch(state_t2){
	case BUTTON_UP:
		tec2Events.tecPresseed = FALSE;
		tec2Events.tecReleased = FALSE;
		//Si la tecla no esta pulsada
		if (gpioRead(TEC2)){
			state_t2 = BUTTON_FALLING;
		}
		else {
			state_t2 = BUTTON_UP;
		}
		break;
	case BUTTON_FALLING:
		tec2Events.tecPresseed = FALSE;
		tec2Events.tecReleased = FALSE;
		//Si el delay no esta corriendo lo pongo a correr
		if(!tec2DelayDebouncer.running){
			delayConfig(&tec2DelayDebouncer,T_Debouncer);
			//Activo el delay para que empiece a correr
			delayRead(&tec2DelayDebouncer);
			state_t2 = BUTTON_FALLING;
		}
		//Tiempo expirado
		if(delayRead(&tec2DelayDebouncer)){
			//Si la tecla no esta pulsada
			if(gpioRead(TEC2)){
				tec2Events.tecReleased = TRUE;
				tec2Events.tecPresseed = FALSE;
				state_t2 = BUTTON_DOWN;
			}
			else {
				tec2Events.tecReleased = FALSE;
				tec2Events.tecPresseed = FALSE;
				state_t2 = BUTTON_UP;
			}
		}
		break;
	case BUTTON_DOWN:
		tec2Events.tecPresseed = FALSE;
		tec2Events.tecReleased = FALSE;
		//Si la tecla esta pulsada
		if (!gpioRead(TEC2)){
			state_t2 = BUTTON_RISING;
		}
		else {
			state_t2 = BUTTON_DOWN;
		}
		break;
	case BUTTON_RISING:
		tec2Events.tecPresseed = FALSE;
		tec2Events.tecReleased = FALSE;
		//Si el delay no esta corriendo lo pongo a correr
		if(!tec2DelayDebouncer.running){
			delayConfig(&tec2DelayDebouncer,T_Debouncer);
			//Activo el delay para que empiece a correr
			delayRead(&tec2DelayDebouncer);
			state_t2 = BUTTON_RISING;
		}
		//Tiempo expirado
		if(delayRead(&tec2DelayDebouncer)){
			//Si la tecla  esta pulsada
			if(!gpioRead(TEC2)){
				tec2Events.tecReleased = FALSE;
				tec2Events.tecPresseed = TRUE;
				state_t2 = BUTTON_UP;
			}
			else {
				tec2Events.tecReleased = FALSE;
				tec2Events.tecPresseed = FALSE;
				state_t2 = BUTTON_DOWN;
			}
		}
		break;
	default:
		state_t2 = BUTTON_DOWN;
		break;
	}
}

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
