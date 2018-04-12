/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

#ifndef _BUTTONS_FSM_H_
#define _BUTTONS_FSM_H_

/*==================[inclusiones]============================================*/

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/
typedef enum {
	TEC1_INDEX = 0,		//esta lista tiene que coincidr con la estrcutura de  buttons y me dice en que posicion esta cada tecla dentro de la estructura.. sino tendria que buscarla una por una.. y por ahora se supone fija, asi que no hace falta..
	TEC2_INDEX
} buttonGpioIndex_t;

#define BUTTONS_PERIOD	50		//los botones se samplean cada cierto tiempo regular 

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

extern void 	emulateButton			( const buttonGpioIndex_t tec );
extern bool_t 	readButton				( const buttonGpioIndex_t tec );
extern void 	ackButton				( const buttonGpioIndex_t tec );
extern bool_t	buttonsFsm				( void* nothing );

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif // _BUTTONS_FSM_H_


