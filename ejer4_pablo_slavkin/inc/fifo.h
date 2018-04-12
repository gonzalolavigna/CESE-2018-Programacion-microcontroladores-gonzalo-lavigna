/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

#ifndef _FIFO_H_
#define _FIFO_H_

/*==================[inclusiones]============================================*/

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

typedef struct  {
	uint8_t *buff;
	uint8_t	pW;
	uint8_t	pR;
	uint16_t	size;
} fifo_t;


/*==================[declaraciones de funciones externas]====================*/

extern bool_t fifoWrite 	( fifo_t* F , uint8_t Data );
extern bool_t fifoRead 		( fifo_t* F, uint8_t* Data );

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif


