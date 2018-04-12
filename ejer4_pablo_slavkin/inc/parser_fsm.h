/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

#ifndef _PARSER_FSM_H_
#define _PARSER_FSM_H_

/*==================[inclusiones]============================================*/

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/
#define PARSER_PERIOD		100	//considero que como consumidora de datos de uar de usuario el usuario no enviarar mas de 10 teclas por seg, con lo cual me aseguro ese promedio mas algun pico de un superveloz. Para envio a fondo a 115200, esto explota.. hay que hacerlo mucho mas rapido..pero no es el caso

/*==================[declaraciones de funciones externas]====================*/

extern uint16_t	readCustomPeriod	( void );
extern void 		ackCustomPeriod	( void );
extern bool_t		parserFsm			( void* nothing );

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif 


