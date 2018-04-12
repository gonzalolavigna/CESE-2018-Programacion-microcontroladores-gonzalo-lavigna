/* Copyright 2017, Agustin Bassi.
 * All rights reserved.
 *
 * This file is part sAPI library for microcontrollers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Date: 2017-11-24
 */

/*==================[inlcusiones]============================================*/

#include "main.h"

#include "cooperativeOs_isr.h"       // <= dispatcher and task management header
#include "cooperativeOs_scheduler.h" // <= scheduler and system initialization header
#include "sapi.h"
#include "PCLink.h"
#include "debounceTec.h"

/*==================[macros and definitions]=================================*/
DEBUG_PRINT_ENABLE

#define CONSOLE_TIME_PERIOD 10
#define APP_TIME_PERIOD 5

/*==================[definiciones de datos internos]=========================*/


/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
void app (void);
/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

int main( void ){
	//Inicializar el hardware, perifericos, puertos, clock, etc.
	boardConfig();
	// Inicializar UART_USB como salida de consola
	debugPrintConfigUart( UART_USB, 115200 );
	debugPrintlnString( "UART_USB configurada.\n\r" );
	debounceInit();
	//FUNCION que inicializa el planificador de tareas
	schedulerInit();
	//Cargar las tareas del sistema operativo con sus periodicidades
	schedulerAddTask( (sAPI_FuncPtr_t) debounceUpdate, 0, BUTTONS_TIME_PERIOD );
	schedulerAddTask( (sAPI_FuncPtr_t) PC_LINK_Update, 1, CONSOLE_TIME_PERIOD );
	schedulerAddTask( (sAPI_FuncPtr_t) app,2, APP_TIME_PERIOD );



	//Iniciar el planificador de tareas
	schedulerStart( 1 );

	//Lazo infinito
	while(TRUE) {
		//Ejecutar tareas
		schedulerDispatchTasks();
	}
	return 0;
}

/*==================[definiciones de funciones internas]=====================*/
void app (void) {
	if(getTeclaStatus(TEC1_INDICE)==TECLA_PULSADA)
		PC_LINK_Write_String_To_Buffer("TEC 1 PULSADA\n");
	if(getTeclaStatus(TEC2_INDICE)==TECLA_PULSADA)
		PC_LINK_Write_String_To_Buffer("TEC 2 PULSADA\n");
	if(getTeclaStatus(TEC3_INDICE)==TECLA_PULSADA)
		PC_LINK_Write_String_To_Buffer("TEC 3 PULSADA\n");
	if(getTeclaStatus(TEC4_INDICE)==TECLA_PULSADA)
		PC_LINK_Write_String_To_Buffer("TEC 4 PULSADA\n");
}

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/

