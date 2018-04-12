/*============================================================================/*
 * Autor:Pablo Slavkin
 * Licencia: para todos y todas
 * Fecha: 03-2018
 *===========================================================================*/
/*==================[inlcusiones]============================================*/

#include "sapi.h"				// <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

bool_t rangeCompare( uint16_t value, uint16_t min, uint16_t max ) {
	return ( value >= min && value <= max );
}


uint16_t bcd2Int( uint8_t* bcd, uint8_t index )	 //ineficiente conversor de bcd a int, listo para usar.
{
	uint16_t ans = 0, factor=1;
	while ( index > 0 ) {
		ans += ( bcd[ --index ] - '0' ) * factor;
		factor *= 10;
	}
	return ans;
}
/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
