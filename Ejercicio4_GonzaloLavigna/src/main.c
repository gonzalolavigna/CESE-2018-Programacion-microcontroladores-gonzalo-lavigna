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
#include "PCLink.h"
#include "cooperativeOs_isr.h"       // <= dispatcher and task management header
#include "cooperativeOs_scheduler.h" // <= scheduler and system initialization header
#include "sapi.h"
#include "app.h"

DEBUG_PRINT_ENABLE

/*==================[macros and definitions]=================================*/
#define BLINK_TIME_PERIOD 500
#define DEBOUNCE_TIME 40
#define CONSOLE_TIME_PERIOD 2
#define APP_TIME_PERIOD 120

typedef enum {LED_ON,LED_OFF} fsmBlinky_t;
typedef enum {BUTTON_UP, BUTTON_FALLING, BUTTON_DOWN, BUTTON_RAISING} fsmDebounce_t;

typedef struct {
	gpioMap_t tecla;
	fsmDebounce_t state;
	delay_t delay;
}debounceData_t;

/*==================[definiciones de datos internos]=========================*/

/*Arreglo con tiempos de blink*/
static uint16_t tiempoEncendido[]={50,100,1000};
/*Arreglo con los distintos led disponibles en la EDU-CIAA*/
gpioMap_t leds[] = {LEDR,LEDG,LEDB,LED1,LED2,LED3};

static uint16_t indiceTiempo = 0;
uint16_t indiceLed = 0;

#define TIEMPOS_VALIDOS sizeof(tiempoEncendido)/sizeof(uint16_t) - 1
#define LEDS_VALIDOS sizeof(leds)/sizeof(gpioMap_t) - 1

fsmBlinky_t blinkyState = LED_ON;
debounceData_t dataTecla1,dataTecla2;

int32_t blinkLedTaskId;
int32_t appTaskId;
extern sTask_t schedulerTasks[SCHEDULER_MAX_TASKS];

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

void fsmBlinkLed(void);
void fsmDebounceInit(debounceData_t * dataStruct, gpioMap_t dbnTec);
void fsmDebounceUpdate(debounceData_t * ptrDataStruct);
void buttonPressed(gpioMap_t tecla);
void buttonReleased(gpioMap_t tecla);
void debounceTec1(void);
void debounceTec2(void);

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

int main( void ){
	//Inicializar el hardware, perifericos, puertos, clock, etc.
	boardConfig();

	debugPrintConfigUart( UART_USB, 115200 );
	debugPrintlnString( "UART_USB configurada.\n\r" );

	fsmDebounceInit(&dataTecla1,TEC1);
	fsmDebounceInit(&dataTecla2,TEC2);
	//FUNCION que inicializa el planificador de tareas
	schedulerInit();
	//Cargar las tareas del sistema operativo con sus periodicidades
	blinkLedTaskId=schedulerAddTask( (sAPI_FuncPtr_t) fsmBlinkLed, 0, BLINK_TIME_PERIOD );
	schedulerAddTask( (sAPI_FuncPtr_t) debounceTec1, 1, DEBOUNCE_TIME );
	schedulerAddTask( (sAPI_FuncPtr_t) debounceTec2, 2, DEBOUNCE_TIME );
	schedulerAddTask( (sAPI_FuncPtr_t) PC_LINK_Update , 3 , CONSOLE_TIME_PERIOD );
	appTaskId=schedulerAddTask( (sAPI_FuncPtr_t) app , 4 , APP_TIME_PERIOD );

	PC_LINK_Write_String_To_Buffer("START ");
	show_menu();
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
/*void app (void) {
	uint8_t dato;
	if(PC_LINK_Read_Char_From_Buffer(&dato)!= BUFFER_EMPTY ){
		PC_LINK_Write_String_To_Buffer("TECLA RECIBIDA: ");
		PC_LINK_Write_Char_To_Buffer(dato);
	}
}*/


void debounceTec1(void){
	fsmDebounceUpdate(&dataTecla1);
}

void debounceTec2(void){
	fsmDebounceUpdate(&dataTecla2);
}


/*
void taskBlinkLed(void){

   if (ledState == ON){
      ledState = OFF; // Apago el pin
   }
   else{
      ledState = ON; // Prendo el pin
   }

   // Use of digitalWrite
   gpioWrite( LED1, ledState );
}
*/
void fsmBlinkLed (void){
	switch(blinkyState){
	case LED_ON:
		gpioToggle(leds[indiceLed]);
		blinkyState = LED_OFF;
		break;
	case LED_OFF:
		gpioToggle(leds[indiceLed]);
		blinkyState = LED_ON;
		break;
	default:
		blinkyState = LED_ON;
	}
}

void fsmDebounceInit(debounceData_t * dataStruct, gpioMap_t dbnTec){
	dataStruct->tecla = dbnTec;
	dataStruct->state = BUTTON_UP;
	return;
}

void fsmDebounceUpdate(debounceData_t * ptrDataStruct){
	switch(ptrDataStruct->state){
	case BUTTON_UP:
		if(!gpioRead(ptrDataStruct->tecla))
			ptrDataStruct->state=BUTTON_FALLING;
		break;
	case BUTTON_FALLING:
		if(!gpioRead(ptrDataStruct->tecla)){
			buttonPressed(ptrDataStruct->tecla);
			ptrDataStruct->state=BUTTON_DOWN;
		}
		else
			ptrDataStruct->state = BUTTON_UP;
		break;
	case BUTTON_DOWN:
		if(gpioRead(ptrDataStruct->tecla))
			ptrDataStruct->state = BUTTON_RAISING;
		break;
	case BUTTON_RAISING:
		if(gpioRead(ptrDataStruct->tecla)){
			buttonReleased(ptrDataStruct->tecla);
			ptrDataStruct->state=BUTTON_UP;
		}
		break;
	default:
		break;
	}
}

void buttonPressed(gpioMap_t tecla){
	switch(tecla){
	case TEC1:
		break;
	case TEC2:
		break;
	default:
		break;
	}
	return;
}

void buttonReleased(gpioMap_t tecla){
	switch(tecla){
	case TEC1:
		if( indiceTiempo >= TIEMPOS_VALIDOS)
			/* El índice estaba en el valor máximo y debe reiniciarse */
			indiceTiempo = 0;
		else
			/* El índice era menor al valor máximo y debe incrementarse*/
			indiceTiempo++;
		schedulerTasks[blinkLedTaskId].period = tiempoEncendido[indiceTiempo];
		break;
	case TEC2:
		/* Apago el led que estaba parpadeando*/
		gpioWrite(leds[indiceLed], OFF);


		if( indiceLed >= LEDS_VALIDOS) {
			/* El índice estaba en el valor máximo y debe reiniciarse */
			indiceLed = 0;
		}
		else
			/* El índice era menor al valor máximo*/
			indiceLed++;
		break;
	default:
		break;
	}
	return;
}

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/

