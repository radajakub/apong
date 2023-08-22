/** @file
*/

#ifndef PLAYER_INPUT_H
#define PLAYER_INPUT_H

#define LEFT_PLAYER_UP 'w'
#define LEFT_PLAYER_DOWN 's'
#define RIGHT_PLAYER_UP 'o'
#define RIGHT_PLAYER_DOWN 'l'
#define ENTER (10)

#define LOG_HEAD_PLAYER_INPUT "INPUT: "
#define LOG_PLAYER_INPUT 1

/**
 * Contains information about pressed keys. \n
 * The keys which have been pressed are set to value 1. \n
 * The ones which have not are set to value 0.
 */
struct input {
    char left_up, left_down;
    char right_up, right_down;
};

/**
 * Returns an instance of struct input initialized with zeros for all values.
 * @return a new instance of struct input with all values set to zero
 */
struct input init_input_data(void);

/**
 * Call before using the get_input function.
 */
void init_input(void);

/**
 * Returns information about which keys have been pressed since the last call of this function.
 * @return an instance of struct input with the information about pressed keys
 */
struct input get_input(void);

/**
 * Call after the last use of the get_input function.
 */
void exit_input(void);

#endif
