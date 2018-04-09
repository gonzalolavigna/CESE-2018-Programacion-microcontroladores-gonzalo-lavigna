/*============================================================================
 * Autor:
 * Licencia:
 * Fecha:
 *===========================================================================*/

/*==================[inclusiones]============================================*/

#include "sapi.h"        // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

typedef enum {LED_ON, LED_OFF} fsmBlinky_t;
typedef enum {UP, FALLING,DOWN,RAISING} fsmAntiRebote_t;
typedef struct {
	delay_t antireboteDelay;
	fsmAntiRebote_t tecEstado;
	gpioMap_t pin;
}fmsDebounce_t;

#define T_1 50
#define T_2 100
#define T_3 1000
#define DELAY_ANTIREBOTE 40

/* Tamaño del arreglo dividido el tamaño de un elemento individual.
 * Se resta uno porque el primer índice es cero*/
#define TIEMPOS_VALIDOS sizeof(tiempoEncendido)/sizeof(uint16_t) - 1
#define LEDS_VALIDOS sizeof(leds)/sizeof(gpioMap_t) - 1

/*==================[definiciones de datos internos]=========================*/

const uint16_t tiempoEncendido[] = {T_1,T_2,T_3};
const gpioMap_t leds[] = {LEDR,  LEDG,  LEDB};

delay_t blinkyDelay;
fmsDebounce_t tec1,tec2;

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

bool_t fsmAntiRebote(fmsDebounce_t *tec);
void updateFrec(uint8_t *indiceTiempo, bool_t frecRotate);
void updateLed(uint8_t *indiceLed, bool_t ledRotate);
void initsytem(uint8_t indiceTiempo,uint8_t indiceLed);

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void ){

	// ---------- CONFIGURACIONES ------------------------------
	uint8_t indiceTiempo = 2;
	uint8_t indiceLed = 0;
	bool_t presionada;

	// Inicializar y configurar la plataforma
	boardConfig();
	initsytem(indiceTiempo,indiceLed);


	// ---------- REPETIR POR SIEMPRE --------------------------
	while( TRUE )
	{


		presionada=fsmAntiRebote(&tec1);
		/* Si la tecla1 fue presionada y es un flanco creciente actualizo Frecuencia parpadeo*/
		if ( presionada==TRUE && tec1.tecEstado == UP )
			updateFrec(&indiceTiempo,presionada);

		presionada=fsmAntiRebote(&tec2);
		/* Si la tecla2 fue presionada y es un flanco creciente actualizo led */
		if ( presionada==TRUE && tec2.tecEstado == UP )
			updateLed(&indiceLed,presionada);

		/* Si se cumplio el primer ciclo debo volver a ingresar e reiniciar el proceso */
		if (!blinkyDelay.running){
			delayConfig(&blinkyDelay, tiempoEncendido[indiceTiempo]);
			/* para que el delay arranque hay que leerlo*/
			delayRead(&blinkyDelay);
		}
			/* Si el tiempo del delay expiró, cambio el estado del led */
		if (delayRead(&blinkyDelay))
			gpioToggle(leds[indiceLed]);
	}

	// NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
	// directamenteno sobre un microcontroladore y no es llamado por ningun
	// Sistema Operativo, como en el caso de un programa para PC.
	return 0;
}



/*==================[definiciones de funciones internas]=====================*/

void initsytem(uint8_t indiceTiempo,uint8_t indiceLed){
	uint8_t i=0;
	/* Inicio la tecla 1 que va a controlar la frecuencia */
	tec1.pin=TEC1;
	tec1.tecEstado=UP;
	/* Inicio la tecla 2 que va a controlar el led que va a estar encendido*/
	tec2.pin=TEC2;
	tec2.tecEstado=UP;

	/* Apago todos los leds */
	for(i=0;i<LEDS_VALIDOS; i++)
		gpioWrite(leds[i], OFF);

	/* Configuro la frecuencia de inicio */
	delayConfig(&blinkyDelay, tiempoEncendido[indiceTiempo]);
	/* para que el delay arranque hay que leerlo*/
	delayRead(&blinkyDelay);
}

/*
 * De argumento recive un puntero a la una estructura del tipo fmsDebounce_t,
 * Actualiza el estado de esa estructura
 * Retorna: TRUE si hubo un cambio de estado, de lo contrario FALSE
 * */
bool_t fsmAntiRebote(fmsDebounce_t *tec){
	bool_t ret=FALSE;
	switch(tec->tecEstado){

	case UP:
		/* Si la tecla fue presionada */
		if ( !gpioRead( tec->pin ) == ON ){
			/* Verifico que el delay no este corriendo */
			if (!(tec->antireboteDelay.running)){
				delayConfig(&(tec->antireboteDelay), DELAY_ANTIREBOTE);
				/* para que el delay arranque hay que leerlo*/
				delayRead(&(tec->antireboteDelay));
			}
			tec->tecEstado=FALLING;
		}
		break;
	case FALLING:
			/* Verifico que el DELAY_ANTIREBOTE haya terminado*/
		if (!delayRead(&(tec->antireboteDelay))){
			/* Si la tecla continua presionada */
			if ( !gpioRead( tec->pin ) == ON ){
				tec->tecEstado=DOWN; /* Laa tecla fue presionada */
				ret=TRUE; /* Flag correspondiente a un flanco */
			}else{
				tec->tecEstado=UP; /* En este caso fue solo ruido */
			}
		}
		break;
	case DOWN:
			/* Si la tecla fue liberada*/
		if ( !gpioRead( tec->pin ) == OFF ){
			/* Verifico que el delay no este corriendo */
			if (!tec->antireboteDelay.running){
				delayConfig(&(tec->antireboteDelay), DELAY_ANTIREBOTE);
				/* para que el delay arranque hay que leerlo*/
				delayRead(&(tec->antireboteDelay));
			}
			tec->tecEstado=RAISING;
		}
		break;
	case RAISING:
			/* Verifico que el DELAY_ANTIREBOTE haya terminado*/
			if (!delayRead(&(tec->antireboteDelay))){
				/* Si la tecla continua liberada*/
				if ( !gpioRead( tec->pin ) == OFF ){
					tec->tecEstado=UP; /* Laa tecla fue liberada*/
					ret=TRUE; /* Flag correspondiente a un flanco */
				}else{
					tec->tecEstado=DOWN; /* En este caso fue solo ruido */
				}
			}
			break;
	/* Condición de error, reinicio la máquina de estados*/
	default:
		while(1){};
		break;
	}
	return ret;
}

void updateLed(uint8_t* indiceLed, bool_t ledRotate){
	if(ledRotate){
		/* TEC1 fue apretada*/
		/* Apago el led que estaba parpadeando*/
		gpioWrite(leds[*indiceLed], OFF);

		if( (*indiceLed) >= LEDS_VALIDOS) {
			/* El índice estaba en el valor máximo y debe reiniciarse */
			*indiceLed = 0;
		}else{
			/* El índice era menor al valor máximo*/
			(*indiceLed)++;
		}
	}
}

void updateFrec(uint8_t *indiceTiempo, bool_t frecRotate){
	if (frecRotate){
		/* TEC1 fue apretada*/
		if( (*indiceTiempo) >= TIEMPOS_VALIDOS)
			/* El índice estaba en el valor máximo y debe reiniciarse */
			*indiceTiempo = 0;
		else{
			/* El índice era menor al valor máximo y debe incrementarse*/
			(*indiceTiempo)++;
		}
	}
}

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
