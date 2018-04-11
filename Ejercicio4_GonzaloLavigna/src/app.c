#include "sapi.h"   // <= sAPI header
#include "app.h"
#include "PCLink.h"
#include "cooperativeOs_isr.h"       // <= dispatcher and task management header
#include "cooperativeOs_scheduler.h" // <= scheduler and system initialization header
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

extern sTask_t schedulerTasks[SCHEDULER_MAX_TASKS];
extern uint16_t indiceLed;
extern gpioMap_t leds[];
extern int32_t appTaskId;
extern int32_t blinkLedTaskId;
extern void buttonReleased(gpioMap_t tecla);

uint8_t receiver_buffer_line[20];

static uint8_t menu[] =
		"\n\r"
		"********************* MENU *************************\n\r"
		"\n\r"
		"h:Repite el menu.\n\r"
		"s:Muestra Configuracion Blink LEDS.\n\r"
		"TEC1: cambia el tiempo de parpadeo del led activo.\n\r"
		"TEC2: cambia el led activo.\n\r"
		"T: permite ingresar un tiempo desde el puerto serie Rango:1-1000-.\n\r"
		"L: permite ingresar un led desde el puerto serie Rango:0-5.\n\r"
		"Todos los comandos son validados con Enter.\n\r"
		;

void app (void){
	if(receive_line(receiver_buffer_line)==LINE_HIT){
		//PC_LINK_Write_String_To_Buffer(receiver_buffer_line);
		if(memcmp(receiver_buffer_line,"h",1)==0){
			show_menu();
		}
		else if (memcmp(receiver_buffer_line,"s",1)==0){
			show_status();
		}
		else if (memcmp(receiver_buffer_line,"TEC1",4)==0){
			buttonReleased(TEC1);
			PC_LINK_Write_String_To_Buffer("TEC1 Pulsada\n");
		}
		else if (memcmp(receiver_buffer_line,"TEC2",4)==0){
			buttonReleased(TEC2);
			PC_LINK_Write_String_To_Buffer("TEC2 Pulsada\n");
		}
		else if (memcmp(receiver_buffer_line,"L",1)==0){
			schedulerTasks[appTaskId].pTask=( (sAPI_FuncPtr_t) puerto_led_pos_menu);
			PC_LINK_Write_String_To_Buffer("Elija un valor en 0 y 5 para cambiar la posicion del led\n");
		}
		else if (memcmp(receiver_buffer_line,"T",1)==0){
			schedulerTasks[appTaskId].pTask=( (sAPI_FuncPtr_t) led_delay_menu);
			PC_LINK_Write_String_To_Buffer("Elija un valor en 1 y 1000 para cambiar el blink delay en (ms)\n");
		}
	}
}

void led_delay_menu(void){
	uint8_t i=0;
	uint32_t led_delay=0;
	bool_t flag_is_a_number = TRUE;
	if(receive_line(receiver_buffer_line)==LINE_HIT){
		while(receiver_buffer_line[i]!='\0'){
			if(receiver_buffer_line[i]<'0' && receiver_buffer_line[i]>'0' ){
				flag_is_a_number = FALSE;
			}
			i++;
		}
		if(i<5 || flag_is_a_number == TRUE){
			led_delay = atoi(receiver_buffer_line);
			if(led_delay <= 1000 && led_delay >= 1 ){
				schedulerTasks[appTaskId].pTask=( (sAPI_FuncPtr_t) app);
				schedulerTasks[blinkLedTaskId].period = led_delay;
				PC_LINK_Write_String_To_Buffer("BLINK Delay cambiado\n");
			}
			else {
				schedulerTasks[appTaskId].pTask=( (sAPI_FuncPtr_t) app);
				PC_LINK_Write_String_To_Buffer("Valor Fuera de Rango volviendo a Menu Principal\n");
			}
		}
		else {
			schedulerTasks[appTaskId].pTask=( (sAPI_FuncPtr_t) app);
			PC_LINK_Write_String_To_Buffer("Valor Fuera de Rango volviendo a Menu Principal\n");
		}
	}
}

void puerto_led_pos_menu (void){
	uint8_t i=0;
	uint8_t led_pos;
	if(receive_line(receiver_buffer_line)==LINE_HIT){
		while(receiver_buffer_line[i]!='\0'){
			led_pos = receiver_buffer_line[i]-'0';
			i++;
		}
		if(i == 1 && led_pos <6 ){
			gpioWrite(leds[indiceLed], OFF);
			indiceLed = led_pos;
			schedulerTasks[appTaskId].pTask=( (sAPI_FuncPtr_t) app);
			PC_LINK_Write_String_To_Buffer("Led Cambiado\n");
		}
		else{
			schedulerTasks[appTaskId].pTask=( (sAPI_FuncPtr_t) app);
			PC_LINK_Write_String_To_Buffer("Valor Fuera de Rango volviendo a Menu Principal\n");
		}
	}
}

void show_menu (void){
	PC_LINK_Write_String_To_Buffer(menu);
}
void show_status (void){
	uint8_t buffer_led[5] = {0,0,0,0,0};
	PC_LINK_Write_String_To_Buffer("Periodo Blink LED (ms): ");
	itoa(schedulerTasks[blinkLedTaskId].period,buffer_led,10);
	PC_LINK_Write_String_To_Buffer(buffer_led);
	PC_LINK_Write_Char_To_Buffer('\n');
	PC_LINK_Write_String_To_Buffer("Periodo Posicion LED: ");
	switch (leds[indiceLed]) {
		case LEDR:
			PC_LINK_Write_String_To_Buffer("LEDR\n");
			break;
		case LEDG:
			PC_LINK_Write_String_To_Buffer("LEDG\n");
			break;
		case LEDB:
			PC_LINK_Write_String_To_Buffer("LEDB\n");
			break;
		case LED1:
			PC_LINK_Write_String_To_Buffer("LED1\n");
			break;
		case LED2:
			PC_LINK_Write_String_To_Buffer("LED2\n");
			break;
		case LED3:
			PC_LINK_Write_String_To_Buffer("LED3\n");
			break;
		default:
			break;
	}
}
uint8_t receive_line (uint8_t * str_ptr){
	static indice_receiver_buffer_line = 0;
	uint8_t dato;
	uint8_t line_hit_flag = LINE_NOT_FOUND;
	if(PC_LINK_Read_Char_From_Buffer(&dato)!= BUFFER_EMPTY ){
		if(dato == '\n'){
			line_hit_flag = LINE_HIT;
			str_ptr[indice_receiver_buffer_line] = 0;
			indice_receiver_buffer_line = 0;
		}
		else {
			line_hit_flag = LINE_NOT_FOUND;
			str_ptr[indice_receiver_buffer_line] = dato;
			indice_receiver_buffer_line ++;
		}
	}
	return line_hit_flag;
}
