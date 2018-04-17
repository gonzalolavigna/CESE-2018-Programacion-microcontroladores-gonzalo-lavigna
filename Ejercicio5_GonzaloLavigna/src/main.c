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
#include "leds.h"
#include "alarma.h"

/*==================[macros and definitions]=================================*/
DEBUG_PRINT_ENABLE

#define CONSOLE_TIME_PERIOD 10
#define APP_TIME_PERIOD 100

/*==================[definiciones de datos internos]=========================*/


/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
//void app (void);
/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

int main( void ){
	//Inicializar el hardware, perifericos, puertos, clock, etc.
	boardConfig();
	// Inicializar UART_USB como salida de consola
	debugPrintConfigUart( UART_USB, 115200 );
	debugPrintlnString( "UART_USB configurada.\n\r" );
	fsmDebounceInit();
	ledInit();
	alarmaInit();
	//FUNCION que inicializa el planificador de tareas
	schedulerInit();
	//Cargar las tareas del sistema operativo con sus periodicidades
	schedulerAddTask( (sAPI_FuncPtr_t) fsmDebounceUpdate, 0, BUTTONS_TIME_PERIOD );
	schedulerAddTask( (sAPI_FuncPtr_t) PC_Link_Update_TX_BURST, 1, CONSOLE_TX_TIME_PERIOD );
	schedulerAddTask( (sAPI_FuncPtr_t) PC_Link_Update_RX, 2, CONSOLE_RX_TIME_PERIOD );
	schedulerAddTask( (sAPI_FuncPtr_t) alarmaUpdate,3, ALARMA_TIME_PERIOD );
	schedulerAddTask( (sAPI_FuncPtr_t) ledUpdate,4, LEDS_TIME_PERIOD );



	//Iniciar el planificador de tareas
	schedulerStart( 1 );

	//Lazo infinito
	while(TRUE) {
		//Ejecutar tareas
		schedulerDispatchTasks();
	}
	return 0;
}

uint8_t line_reception_buffer[50];
/*==================[definiciones de funciones internas]=====================*/
void app (void) {
	static uint32_t blink_period = 500;
	uint32_t line_receive_length;
	if(getTeclaEventReleasedWithClear(TEC1_INDICE)==TRUE){
			PC_LINK_Write_String_To_Buffer("TEC 1 PULSADA\n");
			setLedConfiguration(LED_RED_INDICE,LED_BI_LEVEL_HIGH,0);
		}
	if(getTeclaEventReleasedWithClear(TEC2_INDICE)==TRUE){
			PC_LINK_Write_String_To_Buffer("TEC 2 PULSADA\n");
			setLedConfiguration(LED_RED_INDICE,LED_BI_LEVEL_LOW,0);
		}
	if(getTeclaEventReleasedWithClear(TEC3_INDICE)==TRUE){
			PC_LINK_Write_String_To_Buffer("TEC 3 PULSADA\n");
			setLedConfiguration(LED_RED_INDICE,LED_BLINK,blink_period);
			blink_period += 100;
		}
	if(getTeclaEventReleasedWithClear(TEC4_INDICE)==TRUE){
			PC_LINK_Write_String_To_Buffer("TEC 4 PULSADA\n");
			setLedConfiguration(LED_RED_INDICE,LED_BLINK,blink_period);
			blink_period -= 100;
		}
	line_receive_length = PC_LINK_Find_Line_From_Buffer();
	if(line_receive_length != LINE_NOT_FOUND){
		PC_LINK_Get_String_From_Buffer(line_reception_buffer,line_receive_length);
		PC_LINK_Write_String_To_Buffer_ln("Linea Recibida\n");
		PC_LINK_Write_String_To_Buffer_ln(line_reception_buffer);
	}

}


/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/

