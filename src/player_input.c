/** @file
*/

#include "player_input.h"
#include "log.h"
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <fcntl.h>

void check_char(char c, struct input* input);

static struct termios original_termios;
static int flags;

/**
 * Returns an instance of struct input initialized with zeros for all values.
 * @return a new instance of struct input with all values set to zero
 */
struct input init_input_data(void) {
    struct input input;
    input.left_up = 0;
    input.left_down = 0;
    input.right_up = 0;
    input.right_down = 0;
    return input;
}

/**
 * Set the cmd input settings to raw mode.
 */
void init_input(void) {
    tcgetattr(STDIN_FILENO, &original_termios);
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    if (LOG_PLAYER_INPUT) print_log(LOG_HEAD_PLAYER_INPUT, "cmd raw mode enabled");
}

/**
 * Set the cmd input settings back to the original.
 */
void exit_input(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
    fcntl(STDIN_FILENO, F_SETFL, flags);
    if (LOG_PLAYER_INPUT) print_log(LOG_HEAD_PLAYER_INPUT, "cmd raw mode disabled");
}

/**
 * Check all characters in stdin for control input.
 * @return an instance of struct input with the information about pressed keys
 */
struct input get_input(void) {
    struct input input = init_input_data();
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1) check_char(c, &input); 
    return input;
}

/**
 * Check whether the given char in one of the player controls and update the given input struct accordingly.
 * @param c the char to be checked
 * @param input an instance of struct input to be udpated
 */
void check_char(char c, struct input* input) {
    switch (c) {
        case LEFT_PLAYER_UP:
            input->left_up = 1;
            break;
        case LEFT_PLAYER_DOWN:
            input->left_down = 1;
            break;
        case RIGHT_PLAYER_UP:
            input->right_up = 1;
            break;
        case RIGHT_PLAYER_DOWN:
            input->right_down = 1;
            break;
    }
}
