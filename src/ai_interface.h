/** @file
 * This is an interface describing the functions which AI implementations should implement. \n
 * This file is not to be included anywhere nor to be implemented directly (as ai_interface.c).
 */

#include "game.h" // struct game_data
#include "graphics.h" // LCD_HEIGHT, LCD_WIDTH

/**
 * This funciton is called every game update to determine the AI's movement. \n
 * Implementations of AI are to rename this function to "name_of_the_ai_move".
 * @param is_right 0 if the AI controls the left paddle, \n
 *                 1 if the AI controls the right paddle
 * @param game_data a structure containing all information about the state of the game
 * @return -1 for moving up \n
 *          0 for not moving \n
 *          1 for moving down \n
 * @see game.h for struct game_data
 */
char ai_move(char is_right, struct game_data game_data);
