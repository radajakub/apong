/** @file
*/

#ifndef LOG_H
#define LOG_H

#include "rgb565.h"

#define MSG_COLOR (1024)
#define MSG_BACKGROUND BLACK
#define MSG_X (20)

/**
 * Print the given message with the given header to the command line. \n
 * New line is included at the end.
 */
void print_log(char* head, char* msg);

/**
 * Prints the given message on the display in big letters. \n
 * Good for sending a message to a mentally challanged individuals fighting for control over the machine you are using.
 * @param lcd_membase the lcd display memory
 * @param msg a message to "be sent"
 */
void print_msg(unsigned char* lcd_membase, char* msg1, char* msg2, char* msg3);

#endif