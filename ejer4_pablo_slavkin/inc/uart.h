/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

#ifndef _UART_PHISICAL_H_
#define _UART_PHISICAL_H_

/*==================[inclusiones]============================================*/

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

extern bool_t	uartRxFifoWrite	( void*		nothing	);
extern bool_t	uartRxFifoRead		( uint8_t	*data		);
extern bool_t	uartTxFifoWrite	( uint8_t	data		);
extern bool_t	uartTxFifoRead		( void* 		nothing	);
extern void 	putString			( uint8_t*	s			);
extern void 	putStringCrLf		( uint8_t*	s			);

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif // _UART_PHISICAL_H_


