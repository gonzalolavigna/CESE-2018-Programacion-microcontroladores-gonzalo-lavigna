/*============================================================================/*
 * Autor:Pablo Slavkin
 * Licencia: para todos y todas
 * Fecha: 03-2018
 *===========================================================================*/
/*==================[inlcusiones]============================================*/

#include "sapi.h"				// <= Biblioteca sAPI
#include "buttons_fsm.h"	//se usa para testear los botones
#include "cooperativeOs_scheduler.h"	//se usa para testear los botones

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE

enum LED_STATES {      //lista de estados posibles para los leds, o esta prendido, o esta apagado..
   LED_ON,
   LED_OFF,
	BIRTH,					//estasdo inicial que se ejecuta una sola vez al arrancar
};

static const tick_t		timeBaseList[]		= { 100 , 200 , 300 , 400 , 500 , 50};					//lista de posibles valores de divisor de base de tiempo para los leds
static uint8_t 			timeBaseIndex		= 0;																//ojo, hasta 255 timeBase, parece sufi
static uint16_t 			customTimeBase 	= 0;
static const gpioMap_t 	colorList[] 		= { LEDR , LEDG , LEDB , LED1 , LED2 , LED3 };		//lista de leds diferentes.. solo agregando mas se los atendera uno por uno en la fsm
static uint8_t 			colorListIndex 	= 0;
static enum LED_STATES	ledState				= BIRTH;															//arranca en BIRTH que es un estado que se ejecuta una sola vez y alli hace las inicializaciones pertinentes.
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
static void setNextItemList	( uint8_t* index, uint8_t max );
static void setNextTimeBase	( void );
static void setNextLed			( void );
/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

/*==================[definiciones de funciones internas]=====================*/
void setNextItemList( uint8_t* index, uint8_t max ) {																//recorre toda la lista timeBaseList, olvidate de todo y pega la vuelta
		if( ++ (*index) >= max )
			*index = 0;
}

void setNextTimeBase( void ) {																							//recorre toda la lista timeBaseList, olvidate de todo y pega la vuelta
		setNextItemList( &timeBaseIndex, sizeof( timeBaseList )/sizeof( timeBaseList[ 0 ] ) );		//apuntoi al proximo en la lista o pego la vuelta
		schedulerUpdateActualTaskPeriod( timeBaseList[ timeBaseIndex ] );										//what? si me autoactualizo mi base de tiempo de ejecucion de la fsm
}
void setNextLed( void ) {																		//recorre toda la lista de colores de leds y olvdate de todo y pega la vuelta
		setNextItemList( &colorListIndex, sizeof( colorList )/sizeof( colorList[ 0 ] ) );
}
bool_t ledsFsm( void* nothing ) {		//los parametros son para que coincida con el tipo de funcion que acepta el scheduler, pero no lo estoy usnando en la fsm
		switch ( ledState ) {
			case LED_ON:
				gpioWrite( colorList[ colorListIndex ] , ON );	//si tocaba prender, prendo sin importar si hay un boton pendiente, ya que no quiero cambiar en la mitad de un ciclo
				ledState = LED_OFF;
			break;
			case LED_OFF:
				gpioWrite( colorList[ colorListIndex ] , OFF );
				ledState = LED_ON;
				if( readButton( TEC1_INDEX ) ) {						//ahora si es momento de preguntar si alguien apreto el boton,y decidir si cambiar de color
					setNextLed	( );
					ackButton	( TEC1_INDEX );
//					debugPrintlnString( "DEBUG: Next Led COlor" );
				}
				if( readButton( TEC2_INDEX ) ) {						//ahora si es momento de preguntar si alguien apreto el boton,y decidir si cambiar de base de tiempo
					setNextTimeBase( );
					ackButton		( TEC2_INDEX );
//					debugPrintlnString( "DEBUG: Next ledTimeBase" );
				} 
				else
					if( customTimeBase > 0 ) {										//si alguinen pidio un periodo customizado.. entonces esto vale otra cosa que cero
						schedulerUpdateActualTaskPeriod( customTimeBase );	//actualizo asi sin mas..
						customTimeBase = 0;											//borro el dato.. ya se uso..
//						debugPrintlnString( "DEBUG: custom ledTimeBase" );
					}
			break;
			case BIRTH: {															//arranco en este estado y me inicializoo
					uint8_t i;
					for( i = 0; i < ( sizeof( colorList ) / sizeof( colorList[ 0 ] ) ); i++ )		//inicializa todos los leds de la lista en off
						gpioWrite( colorList[ i ] , OFF );
					}
					schedulerUpdateActualTaskPeriod( timeBaseList[ timeBaseIndex ] );		//al arrancar le asigno el periodo segun la lista de periodos que tengo
					ledState = LED_OFF;																		//la cosa anda mal, pero bueno, lo mando a un estado conocido..
			break;
			default:
//				debugPrintlnString( "DEBUG: Bad ledState state" );
				ledState = BIRTH;										//la cosa anda mal, pero bueno, lo mando a un estado conocido..
			break;
		}
	return TRUE;
}

/*==================[definiciones de funciones externas]=====================*/
void setLedCustomTimeBase( uint16_t period ) {		//servicio para que escriban un periodo de base de tiempo customizado.. en este caso el parser lo usa para poner lo que pidio elusuario
	customTimeBase = period;
}
/*==================[fin del archivo]========================================*/
