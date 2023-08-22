/** @file
 * This is an implementation of ai_interface.h
 */

#ifndef BASIC_AI_H
#define BASIC_AI_H

#include "game.h"
#include "graphics.h"

/**
 * This funciton is called every game update to determine the AI's movement. \n
 * Implementations of AI are to rename this function to "name_of_the_ai_move".
 * @param is_right 0 if the AI controls the left paddle, \n
 *                 1 if the AI controls the right paddle
 * @param game_data a structure containing all information about the state of the game
 * @return -1 for moving up \n
 *          0 for not moving \n
 *          1 for moving down
 * @see game.h for struct game_data
 */
char basic_ai_move(char is_right, struct game_data game_data);

#endif
