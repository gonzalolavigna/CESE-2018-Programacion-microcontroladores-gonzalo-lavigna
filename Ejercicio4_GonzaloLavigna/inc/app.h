#include "sapi.h"   // <= sAPI header

#define LINE_HIT 0
#define LINE_NOT_FOUND 1

void app (void);
void puerto_led_pos_menu (void);
void led_delay_menu(void);
void show_menu (void);
void show_status (void);
uint8_t receive_line (uint8_t * str_ptr);

