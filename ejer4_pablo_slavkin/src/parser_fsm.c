/*============================================================================/*
 * Autor:Pablo Slavkin
 * Licencia: para todos y todas
 * Fecha: 03-2018
 *===========================================================================*/
/*==================[inlcusiones]============================================*/

#include "sapi.h"								// <= Biblioteca sAPI
#include "buttons_fsm.h"	
#include "parser_fsm.h"	
#include "scan.h"								//escane a un string
#include "conversions.h"					//conversiones de tipo y comparaciones.. tutifruti
#include "cooperativeOs_scheduler.h"	//se usa para testear los botones

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE

#define MIN_CUSTOM_LED_PERIOD 10				//periodo de base de tiempo de led ingresado minimo en ms
#define MAX_CUSTOM_LED_PERIOD 2000			//periodo de base de tiempo de led ingresado maximo
#define CUSTOM_PERIOD_STR_LENGTH	4	 		// el usuario debera ingresar esta cantidadd de digitos cuanso pida cmabier el periodo de los leds, con 4 va de 0000 a 9999 y me entra en un entero 16b

uint8_t menu[] = {
	"\r\n\r\nMake my day:\r\n"
	"1-emulate button1\r\n"
	"2-emulate button2\r\n"
	"T-set led period 10-2000 [ms]\r\n"
	"?-help\r\n"
};

enum PARSER_STATES {
   BIRTH,				//inicializa fsm
   OPTIONS,				//parsea las opciones de usuario de 1 digito 
	T_SCAN_INT,			//escanea lo que ingresa el usuario hasta que completa la cantidad de digitos o [ENTER]
};

static enum PARSER_STATES	parserState		= BIRTH;
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

/*==================[definiciones de funciones internas]=====================*/
bool_t parserFsm( void* nothing ) {		//los parametros son para que coincida con el tipo de funcion que acepta el scheduler, pero no lo estoy usnando en la fsm
	switch ( parserState ) {
		case BIRTH:
			putString( menu );				//muestra el menu cuando arranca
			parserState = OPTIONS;			//a esperar opciones de usuario
		break;
		case OPTIONS: {
			uint8_t data;
			if( uartRxFifoRead( &data ) ) {				//si llega algo, switchea a ver que es
				switch ( data ) {
					case '1':
						putStringCrLf( "virtual Tec1");
						emulateButton( TEC1_INDEX );		//emula el presionado de un boton, moviendo a la fuerza el ack de la estructura de los botones.. con el permiso del sr. boton..de esta manera se aprovecha toda la logica..
					break;
					case '2':
						putStringCrLf( "virtual Tec2");
						emulateButton( TEC2_INDEX );		//emula el presionado de un boton, moviendo a la fuerza el ack de la estructura de los botones.. con el permiso del sr. boton..de esta manera se aprovecha toda la logica..
					break;
					case 'T':									//con T el usuario puede ingresar un periodo variable de base de tiempo.. asi que me pongo a ver que cosas extranias podria mandar el usuario..
						putStringCrLf( "Enter new led period:");
						scanInit( );							//es un modulito que me permite guardar en un arreglo datos y ademas controlar que sean del tipo correcto a medida que van llegando
						parserState = T_SCAN_INT;			//en este estado ire metiendo datos de usuario en ese arreglo hasta completar o [enter]
					break;
					case '?':
						putString( menu );					//help i need somebody
					break;
					default:
						putStringCrLf( "Command invalid, try ?");	//nack
					break;
				}
			}
		}
		break;
		case T_SCAN_INT: {															
			uint8_t data;
			if( uartRxFifoRead( &data ) ) {										//si llego alto lo paso por el scan a ver si es lo esperado
				switch ( scanInt(data,CUSTOM_PERIOD_STR_LENGTH) ) {		//y? que tal? llego algo util
					case	0:																//si pero aun no termina de ingresar... nos vemos en la proxima
					break;	
					case	1: {															//bien.. termino y es valido, veamos si esta dentro del rango
						uint16_t in = scanInt4Bcd( );								//paso a int el string ingresado para poder comparar
						putString("\r\nNew period ");							//y el ganador es.......
						if( rangeCompare( in, MIN_CUSTOM_LED_PERIOD, MAX_CUSTOM_LED_PERIOD ) ) {	//compara si estad dentro del rago inclusve los limites
							putStringCrLf( "accepted" );							//sip.. entro..
							setLedCustomTimeBase( in );							//se lo asigno a los leds como nueva base de tiempo. SOLO cuando los leds deseen hacerlo.. 
						}
						else {
							putStringCrLf( "out of range" );						//lastima... itenta de nuevo...
						}
						parserState = OPTIONS;										//de nuevo a las opciones..aca se podria imprimir de nuevo el menu.. pero que el usr decida...
					}
					break;		
					case	2:																//aja... el 2 es porque metio mal el dedo, en vez de numero mando cualquier otra cosa.. inaceptable.. tiro todo
					default:
						putStringCrLf( "not a number nor CR nor LF, aborting" );
						parserState = OPTIONS;
					break;	
				}
			}
		}
		break;
		default:
			debugPrintlnString( "DEBUG: Bad parserState state" );				//raaaaro.. vuelvo a empezar
			parserState = BIRTH;
		break;
	}
}

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
