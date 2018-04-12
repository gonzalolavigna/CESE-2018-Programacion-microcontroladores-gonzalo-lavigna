#include "sapi.h"   // <= sAPI header

#define READ_BUFFER_SIZE 500
#define WRITE_BUFFER_SIZE 500
#define BUFFER_EMPTY 0
#define BUFFER_DATA 1

void PC_LINK_Update(void);
void PC_LINK_Write_String_To_Buffer(uint8_t * str_ptr);
void PC_LINK_Write_Char_To_Buffer(uint8_t data);
uint8_t PC_LINK_Read_Char_From_Buffer (uint8_t * data);
