#include "sapi.h"   // <= sAPI header

#define READ_BUFFER_SIZE 500
#define WRITE_BUFFER_SIZE 500
#define BUFFER_EMPTY 0
#define BUFFER_DATA 1

#define CONSOLE_TX_TIME_PERIOD 5
#define CONSOLE_RX_TIME_PERIOD 5

#define LINE_NOT_FOUND 0

void PC_LINK_Update(void);
void PC_Link_Update_TX (void);
void PC_Link_Update_RX (void);
void PC_LINK_Write_String_To_Buffer(uint8_t * str_ptr);
void PC_LINK_Write_Char_To_Buffer(uint8_t data);
uint8_t PC_LINK_Read_Char_From_Buffer (uint8_t * data);
void PC_Link_Update_TX_BURST (void);
uint32_t uartUSBBurstWriteByte( uint8_t * byte , uint32_t burst_length );
uint32_t PC_LINK_Find_Line_From_Buffer (void);
void PC_LINK_Get_String_From_Buffer (uint8_t * string_destino, uint32_t longitud);
void PC_LINK_Write_String_To_Buffer_ln(uint8_t * str_ptr);
