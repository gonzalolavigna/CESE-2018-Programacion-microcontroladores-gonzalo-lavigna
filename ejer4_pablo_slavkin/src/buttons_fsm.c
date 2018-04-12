/*============================================================================/*
 * Autor:Pablo Slavkin
 * Licencia: para todos y todas
 * Fecha: 03-2018
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

#include "sapi.h"					// <= Biblioteca sAPI
#include "buttons_fsm.h"    	// <= maquina de estados para manjeo de butones

/*==================[definiciones y macros]==================================*/

#define BOUNCE_THRESHOLD	2     //numero de veces seguidas que se debera capturar el pulsador para darlo por valido
typedef enum {
	UP,
	FALLING,
	WAITING_ACK,			//en este estado espera a que se haga el ack del ultimo pulsado
	DOWN,
	RISING,
} buttonState_t;

typedef struct {
	gpioMap_t		tec;				//es el numero de la tecla
	bool_t 			elecState;		//lleva cuenta del estado electrico del pin en cada sampleo
	buttonState_t	state;			//este es el estado luego de analizar repetidas veces el estado electrico
	bool_t			ack;				//flag que se usa para que las aplicaciones puedan acusar de que han leido el boton
} button_t;
/*==================[definiciones de datos internos]=========================*/

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE

static uint8_t		bounceCount;														//usado para contar cierta cantidad de veces consecutivas un pulso igual para darlo por valido...con 1 o 2 estaria ok.
static button_t	buttons []	= {													//para cada tecla lleva cuenta del estado electrico, si se apreto o no y el estado en la fsm, con solo agregar mas teclas se los atenderan de la misma manera
	 {.tec = TEC1 , .elecState = HIGH , .state = UP , .ack = true },
	 {.tec = TEC2 , .elecState = HIGH , .state = UP , .ack = true }
};
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

/*==================[definiciones de funciones internas]=====================*/
void 	emulateButton	( const buttonGpioIndex_t tec ) {
	buttons[ tec ].ack = false;
}
bool_t 	readButton	( const buttonGpioIndex_t tec ) {
	return buttons[ tec ].ack == false;				//cuando hay un pilsado valido, esto devuelve true
}
void 		ackButton	( const buttonGpioIndex_t tec ) {
	buttons[ tec ].ack = true;							//esta funcion la llama la aplicacion para dar acuse de que ya se entero de que se apreto el boton...
}

bool_t buttonsFsm( void* nothing ) {		//los parametros son para que coincida con el tipo de funcion que acepta el scheduler, pero no lo estoy usnando en la fsm
		uint8_t i;
		for( i = 0 ; i < ( sizeof( buttons )/sizeof( buttons[ 0 ] )) ; i ++ ) {		//recorre toda la lista de botones
			button_t* b = &buttons[ i ];															//una local para no tener que indexar cada vez y que el codigo quede mas legible
			b->elecState = gpioRead( b->tec );													//leo la tecla y guardo el estado en la estructura, es el estado electrico del pin, no el esstado de boton.
			switch( b->state ) {
				case UP:
					if( !b->elecState )  {															//si detecto un pulsado, me voy a fallling a contar algunos bounceCount hasta decidir que es un pulsado valido
							b->state 	= FALLING;
							bounceCount = 0;
//							debugPrintlnString( "DEBUG: ->Falling" );
					}
				break;
				case FALLING:
						if( !b->elecState ) {
							if( ++ bounceCount >= BOUNCE_THRESHOLD ) {						//si vinieron muchos consecutivos, lo doy por bueno
								b->state = WAITING_ACK;												//espera que la aplicacino se entere que se apreto el boton.. hasta tanto se quedra alli, solo tiene memoria de 1 tiro
								b->ack 	= false;
//								debugPrintlnString( "DEBUG: ->Waiting Ack" );
							}
						}
						else {
							b->state = UP;
//							debugPrintlnString( "DEBUG: ->Up" );
						}
				break;
				case WAITING_ACK:
					if( b->ack == true ) {															//listo, la aplicacion ya se entero de que apretaron y habra hecho algo con eso... por aca, supongo que sigue presionado...pero podria ser que no...
							b->state = DOWN;
//							debugPrintlnString( "DEBUG: ->Down" );
						}
				break;
				case DOWN:
					if( b->elecState )  {															//eb dibde detecte que soldaton boton, voy a contar algunos pulsos consegutivos a rising...
							b->state 	= RISING;
							bounceCount = 0;
//							debugPrintlnString( "DEBUG: ->Rising" );
					}
				break;
				case RISING:
						if( b->elecState ) {
							if( ++ bounceCount >= BOUNCE_THRESHOLD ) {					//listo, parece que ya es suficiente pulso consecutivo en up como para darlo por valido
								b->state = UP;
//								debugPrintlnString( "DEBUG: ->Up" );
							}
						}
						else {
							b->state = DOWN;														//en donde uno solo falle, reseteo 
//							debugPrintlnString( "DEBUG: ->Down" );
						}	
				break;
				default:
						debugPrintlnString( "DEBUG: Bad Button State" );
				break;
			}
		}
   return;                                   //ya se devuelve void, pero retorna igualmente, es mi TOC
}

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
