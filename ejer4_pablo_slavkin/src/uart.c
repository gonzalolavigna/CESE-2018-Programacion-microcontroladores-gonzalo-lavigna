/*============================================================================/*
 * Autor:Pablo Slavkin
 * Licencia: para todos y todas
 * Fecha: 03-2018
 *===========================================================================*/
/*==================[inlcusiones]============================================*/

#include "sapi.h"								// <= Biblioteca sAPI
#include "fifo.h"								// <= Biblioteca sAPI
#include "cooperativeOs_scheduler.h"	//se usa para testear los botones

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE
#define MAX_UART_TX_BUFF	200			//con 200 bytes puedo meter todo el menu sin perdidas. pero si pedis varios helps '?' seguidos, buffer overrun! pero no se rompe nada.. el bash tambien lo hace..
#define MAX_UART_RX_BUFF	100			//para recibi ahora estoy recibiendo maximo 7 teclas, y las saco de la fifo antes de que el usuario pueda meter otra, casi que ni se usa., a menos que mande todo junto con un script o algo asi, y chau.. buffer fulll

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

uint8_t uartRxBuff[ MAX_UART_RX_BUFF ];				//buffer para guardar lo que llega por el pin RX de la uart
fifo_t uartRxFifo = {										//de tipo fifo_t para orderar los punteros, el tamanio y demas y aprovechar la logica para otras fifos
	.buff = uartRxBuff, .pR = 0, .pW = 0, .size = MAX_UART_RX_BUFF
};

bool_t uartRxFifoWrite ( void* nothing )			//productora de contenidos, lee desde el pin Rx de la uart y llena la fifo 
{
		uint8_t data;
		if( uartReadByte( UART_USB, &data ) ){
			fifoWrite( &uartRxFifo, data );
		}
}
bool_t uartRxFifoRead ( uint8_t *data )			//consumidor de contenidos.. la puede llamar el inereasado en leer datos.. por ahora el menu de usuario
{
		return fifoRead( &uartRxFifo, data );
}
//
//----------------------------------------------------------------------------------------------------
//
uint8_t uartTxBuff[ MAX_UART_TX_BUFF ];					//para los datos que se quieren mandar al pin Tx de la uart
fifo_t uartTxFifo = {											//de tipo fifoi_t, guardo punteros y demas
	.buff = uartTxBuff, .pR = 0, .pW = 0, .size = MAX_UART_TX_BUFF
};

bool_t uartTxFifoWrite ( uint8_t data )					//productora de datos (que alguine provee) y guarda en fifo
{
		return fifoWrite( &uartTxFifo, data );
}
bool_t uartTxFifoRead ( void* nothing )					//consumidor de datos de fifo y manda a pin tx de uart
{
		uint8_t data;
		if( fifoRead( &uartTxFifo, &data ) ) {
			uartWriteByte( UART_USB, data );
		}
}

void putString( uint8_t* s )								//para mandar una cadena null terminated o hasta que no haya mas nada
{
	while ( *s != '\0' && uartTxFifoWrite( *s ) )
		s ++;
}
void putStringCrLf( uint8_t* s )							//manda cadena y luego enter
{
	putString( s );
	putString( "\r\n" );
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
