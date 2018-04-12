/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

#ifndef _LEDS_FSM_H_
#define _LEDS_FSM_H_

/*==================[inclusiones]============================================*/

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/
#define LEDS_INITIAL_PERIOD		1	//esto solo se usa para que la tarea de los leds se ejecute la porimera vez, y en el primer salto, se inicializa el periodo de la tarea segun la lista de periodos solicitada, de esta manera no tengo que publicar mi lista de periodos ni cambiar el periodo inicial y el del index 0 de mi lista si tengo que cambiarlo, solo cambio en la lista y ya

/*==================[declaraciones de funciones externas]====================*/

extern void 	setLedCustomTimeBase	( uint16_t period ); 
extern bool_t	ledsFsm					( void* nothing );

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif // _LEDS_FSM_H_


