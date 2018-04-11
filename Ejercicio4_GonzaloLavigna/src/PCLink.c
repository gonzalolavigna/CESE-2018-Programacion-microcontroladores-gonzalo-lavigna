#include "sapi.h"   // <= sAPI header
#include "PCLink.h"

typedef enum {NOMINAL,RX_BUFFER_FULL,TX_BUFFER_FULL} error_PC_LINK_t;

uint32_t indice_lectura_buffer_TX=0;
uint32_t indice_escritura_buffer_TX=0;
uint32_t indice_lectura_buffer_RX=0;
uint32_t indice_escritura_buffer_RX=0;

uint8_t buffer_RX[READ_BUFFER_SIZE];
uint8_t buffer_TX[WRITE_BUFFER_SIZE];


error_PC_LINK_t error_code = NOMINAL;

void PC_LINK_Update (void){
	uint8_t dato;
	if(indice_lectura_buffer_TX < indice_escritura_buffer_TX){
		uartWriteByte(UART_USB,buffer_TX[indice_lectura_buffer_TX]);
		indice_lectura_buffer_TX++;
	}
	else {
		indice_lectura_buffer_TX = 0;
		indice_escritura_buffer_TX = 0;
	}
	if(uartReadByte(UART_USB,&dato)!= FALSE){
		if(indice_escritura_buffer_RX < READ_BUFFER_SIZE){
			buffer_RX[indice_escritura_buffer_RX] = dato;
			indice_escritura_buffer_RX ++ ;
		}
		error_code = RX_BUFFER_FULL;
	}
}

uint8_t PC_LINK_Read_Char_From_Buffer (uint8_t * data){
	uint8_t flag_lectura;
	if(indice_lectura_buffer_RX < indice_escritura_buffer_RX){
		*data = buffer_RX[indice_lectura_buffer_RX];
		indice_lectura_buffer_RX ++;
		flag_lectura = BUFFER_DATA;
	}
	else {
		indice_lectura_buffer_RX = 0;
		indice_escritura_buffer_RX = 0;
		flag_lectura = BUFFER_EMPTY;
	}
	return flag_lectura;
}

void PC_LINK_Write_String_To_Buffer(uint8_t * str_ptr){
	uint32_t i = 0;
	while(str_ptr[i]!= '\0'){
		PC_LINK_Write_Char_To_Buffer(str_ptr[i]);
		i++;
	}
}

void PC_LINK_Write_Char_To_Buffer(uint8_t data){
	if(indice_escritura_buffer_TX<WRITE_BUFFER_SIZE){
		buffer_TX[indice_escritura_buffer_TX]=data;
		indice_escritura_buffer_TX ++;
	}
	else {
		error_code = TX_BUFFER_FULL;
	}
}
