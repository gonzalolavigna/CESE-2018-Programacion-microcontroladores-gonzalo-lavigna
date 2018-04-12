/*============================================================================/*
 * Autor:Pablo Slavkin
 * Licencia: para todos y todas
 * Fecha: 03-2018
 *===========================================================================*/
/*==================[inlcusiones]============================================*/

#include "sapi.h"								// <= Biblioteca sAPI
#include "fifo.h"								// <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/
//fifo generica, sin buffer asociado sino que va por parametro asi reutilizo la logica

bool_t fifoRead ( fifo_t* F, uint8_t* Data )
{
	if ( F->pR == F->pW ) {
		return false;
	}
	else {
		*Data		= F->buff[ F->pR ];
		F->pR	= ( F->pR + 1 ) % F->size;
		return true;
	}
}

bool_t fifoWrite ( fifo_t* F , uint8_t Data )
{
	if( ( F->pW + 1 ) % F->size == F->pR ) {
		debugPrintlnString("DEBUG -> oops.. uart buffer full, now what?");
		return false;
	}
	else {
		F->buff[ F->pW ]	= Data;
		F->pW				= ( F->pW + 1 ) % F->size;
		return true;
	}
}
/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
