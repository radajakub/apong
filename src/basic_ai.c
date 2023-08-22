/** @file
*/
#include "game.h"

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
char basic_ai_move(char is_right, struct game_data game_data) {
    int paddle_middle = (is_right ? game_data.paddle_right_pos : game_data.paddle_left_pos) + PADDLE_HEIGHT / 2 - 1;
    int ball_middle = game_data.ball_pos_y + BALL_SIZE / 2 - 1;
    if (paddle_middle < ball_middle) return 1;
    if (paddle_middle > ball_middle) return -1;
    return 0;
}
