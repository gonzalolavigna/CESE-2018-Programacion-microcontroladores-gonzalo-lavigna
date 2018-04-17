#include "sapi.h"   // <= sAPI header
#include "PCLink.h"
#include "sapi_uart.h"
#include "string.h"

#define UART_485_LPC LPC_USART0  /* UART0 (RS485/Profibus) */
#define UART_USB_LPC LPC_USART2  /* UART2 (USB-UART) */
#define UART_232_LPC LPC_USART3  /* UART3 (RS232) */

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

void PC_Link_Update_TX (void){

	if(indice_lectura_buffer_TX < indice_escritura_buffer_TX){
		uartWriteByte(UART_USB,buffer_TX[indice_lectura_buffer_TX]);
		indice_lectura_buffer_TX++;
	}
	else {
		indice_lectura_buffer_TX = 0;
		indice_escritura_buffer_TX = 0;
	}
}

void PC_Link_Update_RX (void){
	uint8_t dato;
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

void PC_LINK_Write_String_To_Buffer_ln(uint8_t * str_ptr){
	uint32_t i = 0;
	while(str_ptr[i]!= '\0'){
		PC_LINK_Write_Char_To_Buffer(str_ptr[i]);
		i++;
	}
	PC_LINK_Write_Char_To_Buffer('\n');
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

void PC_Link_Update_TX_BURST (void){
	uint32_t burst_length = 8;

	if(indice_lectura_buffer_TX + burst_length < indice_escritura_buffer_TX){
		indice_lectura_buffer_TX += uartUSBBurstWriteByte(&buffer_TX[indice_lectura_buffer_TX],burst_length);
	}
	else if (indice_lectura_buffer_TX < indice_escritura_buffer_TX){
		indice_lectura_buffer_TX += uartUSBBurstWriteByte(&buffer_TX[indice_lectura_buffer_TX],indice_escritura_buffer_TX - indice_lectura_buffer_TX);
	}
	else {
		indice_lectura_buffer_TX = 0;
		indice_escritura_buffer_TX = 0;
	}
}


uint32_t uartUSBBurstWriteByte( uint8_t * array , uint32_t burst_length ){
	uint32_t data_transmitted = 0;
	while (data_transmitted < burst_length){
		if ((Chip_UART_ReadLineStatus(UART_USB_LPC) & UART_LSR_THRE) != 0) { // Wait for space in FIFO
			  Chip_UART_SendByte(UART_USB_LPC, array[data_transmitted]);
			  data_transmitted++;
		}
		else {
			return data_transmitted;
		}
	}
	return data_transmitted;
}

uint32_t PC_LINK_Find_Line_From_Buffer (void){
	uint32_t line_hit_index = LINE_NOT_FOUND;
	uint32_t i;
	for(i=0;i < (indice_escritura_buffer_RX - indice_lectura_buffer_RX);i++){
		if(buffer_RX[indice_lectura_buffer_RX + i] == '\n'){
			line_hit_index = i+1;
			break;
		}
	}
	return line_hit_index;
}

void PC_LINK_Get_String_From_Buffer (uint8_t * string_destino, uint32_t longitud) {
	uint32_t i;
	for (i=0; i < longitud; i++){
		if (PC_LINK_Read_Char_From_Buffer(&string_destino[i])==BUFFER_EMPTY){
			break;
		}
	}
	PC_LINK_Read_Char_From_Buffer(&string_destino[longitud]);
	string_destino[longitud] = '\0';
}
