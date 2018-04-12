/*============================================================================/*
 * Autor:Pablo Slavkin
 * Licencia: para todos y todas
 * Fecha: 03-2018
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

#include "sapi.h"								// <= Biblioteca sAPI
#include "main.h"
#include "uart.h"
#include "leds_fsm.h"						// <= maquina de estados para manjeo de leds
#include "parser_fsm.h"						// <= maquina de estados para manjeo de leds
#include "buttons_fsm.h"					// <= maquina de estados para manjeo de botones
#include "cooperativeOs_scheduler.h"	//so cooperativo
#include "cooperativeOs_isr.h"			//inicializacion del so cooperativo

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/
DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE
/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
static void initBoard( void );
/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/


uint32_t main( void ) {

	initBoard			( );
	schedulerInit		( );													//FUNCION que inicializa el planificador de tareas
	schedulerAddTask	( buttonsFsm,			0, BUTTONS_PERIOD 		);	//agrego la tarea de los botones
	schedulerAddTask	( ledsFsm,				0, LEDS_INITIAL_PERIOD 	);	//agrego la tarea de los leds
	schedulerAddTask	( uartRxFifoWrite,	1, 50							); //cada cuando se lee el pin Rx de la uart para llenar la fifo
	schedulerAddTask	( uartTxFifoRead,		2, 5							);	//cada cuando se lee la fifo para escribir al pin Tx de la uart
	schedulerAddTask	( parserFsm,			3, 100						);	//fsm para parsear los datos de usuario 
	schedulerStart		( 1 );												//Iniciar el planificador de tareas

	while( TRUE ){						//para siempre
		schedulerDispatchTasks();	//ejecuto las tareas segun ss periodos
	}
	return -1;							//prometo caminar a lujan.. si se ejecuta esto claro.
}
/*==================[definiciones de funciones internas]=====================*/
void initBoard(void ) {/*{{{*/
   // ---------- CONFIGURACIONES ------------------------------
   // Inicializar y configurar la plataforma
   boardConfig();

   // Inicializar UART_USB como salida Serial de debug
   debugPrintConfigUart( UART_USB, 115200 );
   debugPrintlnString( "DEBUG: UART_USB configurada." );

   // Inicializar UART_232 como salida Serial de consola
   consolePrintConfigUart( UART_232, 115200 );
   consolePrintlnString( "UART_232 configurada." );
   return;                                   //ya se devuelve void, pero retorna igualmente, es mi TOC
}/*}}}*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
