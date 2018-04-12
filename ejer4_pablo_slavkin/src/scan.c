/*============================================================================/*
 * Autor:Pablo Slavkin
 * Licencia: para todos y todas
 * Fecha: 03-2018
 *===========================================================================*/
/*==================[inlcusiones]============================================*/

#include "sapi.h"				// <= Biblioteca sAPI
#include "cooperativeOs_scheduler.h"	//se usa para testear los botones

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE


#define MAX_SCAN_BUF				20					//tamano de arreglo lineal para recibir datos y luego procesarlos , con 20 me sobra.. estouy usando 4 ahora...para parsear un entero
static uint8_t scanIndex;							//puntero para ir escibiendo
static uint8_t scanBuf[ MAX_SCAN_BUF ];		//arreglo
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

void scanInit( void ) {
	scanIndex=0;
}
uint8_t scanInt( uint8_t data, uint8_t length) {	//0 si esta todo ok, pero aun no termina, 1 si termino todo ok, 2 si fallo
	uint8_t ans;
	switch ( data ) {		//a ver que llego...
		case '0':			//de 0 a 9 vale..
		case '1':	
		case '2':	
		case '3':	
		case '4':	
		case '5':	
		case '6':	
		case '7':	
		case '8':	
		case '9':	
			scanBuf[ scanIndex++ ] = data;				//ingreso al arreglo y incremento.
			ans = ( scanIndex >= length ) ? 1 : 0;		//me fio si termino lo que me pidieron (podria usar el comparador como resultado, poero es bool, y yo devuelvo entero...MISRA FAIL
		break;	
		case '\r':												//con alguno de estos 2 termina... que si es mac, nix, que se yo.. dejo volando uno de los dos si es que mandaron 2 y se parseara luego
		case '\n':	
			ans = 1;												//listo
		break;
		default:
			ans = 2;												//error.. dto invalido
		break;
	}
	return ans;
}
uint16_t scanInt4Bcd( void ) {							//convierte a entero el ultimo dato recibido
	return bcd2Int( scanBuf, scanIndex );
}
/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
