/** @file
*/

#include "log.h"
#include "font_types.h"
#include "graphics.h"
#include <stdio.h>

void print_log(char* head, char* msg) {
    printf("%s%s\n", head, msg);
}

void print_msg(unsigned char* lcd_membase, char* msg1, char* msg2, char* msg3) {
    int buffer_size = LCD_HEIGHT * LCD_WIDTH;
    uint16_t display_buff[buffer_size];
    for (int i = 0; i < buffer_size; i++) display_buff[i] = 0;
    put_string(MSG_X, 0, display_buff, &font_wArial_88, msg1, (uint16_t)MSG_COLOR, (uint16_t)MSG_BACKGROUND);
    put_string(MSG_X, 100, display_buff, &font_wArial_88, msg2, (uint16_t)MSG_COLOR, (uint16_t)MSG_BACKGROUND);
    put_string(MSG_X, 200, display_buff, &font_wArial_88, msg3, (uint16_t)MSG_COLOR, (uint16_t)MSG_BACKGROUND);
    parlcd_write_cmd(lcd_membase, LCD_WRITE);
    for (int i = 0; i < buffer_size; i++) parlcd_write_data(lcd_membase, display_buff[i]);
}
