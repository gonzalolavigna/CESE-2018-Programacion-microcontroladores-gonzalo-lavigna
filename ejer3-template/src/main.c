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

/*==================[macros and definitions]=================================*/
#define BLINK_TIME_PERIOD 500

/*==================[definiciones de datos internos]=========================*/

static bool_t ledState = OFF;

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

static void taskBlinkLed(void);

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

int main( void ){
	//Inicializar el hardware, perifericos, puertos, clock, etc.
	boardConfig();

	//FUNCION que inicializa el planificador de tareas
	schedulerInit();
	//Cargar las tareas del sistema operativo con sus periodicidades
	schedulerAddTask( (sAPI_FuncPtr_t) taskBlinkLed, 0, BLINK_TIME_PERIOD );

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

void taskBlinkLed(void){

   if (ledState == ON){
      ledState = OFF; // Apago el pin
   }
   else{
      ledState = ON; // Prendo el pin
   }

   // Use of digitalWrite
   gpioWrite( LED3, ledState );
}


/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/

