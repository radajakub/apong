#include "better_ai.h"

int calculate_final_ball_y(struct game_data* game_data);
int calculate_bounces(int final_ball_y);
char get_ball_x_dir(struct game_data* game_data);
char ball_is_coming_towards_ai(char is_right, char ball_x_dir);
int random_bounce(void);

static char ball_x_dir = 0;
static int target_paddle_y;

/** @file
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
char better_ai_move(char is_right, struct game_data game_data) {
    // determine the desired position for the paddle every time the direction of the ball on the x axis changes
    char new_ball_dir = get_ball_x_dir(&game_data);
    if (ball_x_dir != new_ball_dir) {
        ball_x_dir = new_ball_dir;
        if (ball_is_coming_towards_ai(is_right, ball_x_dir)) {
            int final_ball_y = calculate_final_ball_y(&game_data);
            target_paddle_y = final_ball_y - PADDLE_HEIGHT + 1; // set target_paddle_y to minimum
            target_paddle_y += random_bounce();                 // add random bounce
        } else {
            target_paddle_y = LIVES_FONT_SIZE + (LCD_HEIGHT - LIVES_FONT_SIZE) / 2 - PADDLE_HEIGHT / 2;
        }
    }
    // determine the desired movement of the paddel to reach the desired paddle position
    int current_paddle_y = is_right ? game_data.paddle_right_pos : game_data.paddle_left_pos;
    if (target_paddle_y < current_paddle_y) {
        return -1;
    } else if (target_paddle_y > current_paddle_y) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Calculates the y coordinate the ball is going to have when it reaches the x coordinate where the paddle has to hit it. \n
 * Returns the center of the screen if the ball is not moving for some reason.
 * @param game_data a pointer to the struct game_data given to the ai by the game
 * @return the y coordinate the ball is going to have when it has to be hit with the paddle
 */
int calculate_final_ball_y(struct game_data* game_data) {
    int final_ball_y;
    if (game_data->ball_vel_x > 0) {
        int right_limit = LCD_WIDTH - PADDLE_WIDTH - BALL_SIZE;
        final_ball_y = game_data->ball_pos_y + (int)((double)((right_limit - game_data->ball_pos_x) * game_data->ball_vel_y) / (double)game_data->ball_vel_x + 0.5);
    } else if (game_data->ball_vel_x < 0) {
        int left_limit = PADDLE_WIDTH;
        final_ball_y = game_data->ball_pos_y + (int)((double)((left_limit - game_data->ball_pos_x) * game_data->ball_vel_y) / (double)game_data->ball_vel_x + 0.5);
    } else {
        return LIVES_FONT_SIZE + (LCD_HEIGHT - LIVES_FONT_SIZE) / 2 - (int)(BALL_SIZE / 2);
    }
    return calculate_bounces(final_ball_y);
}

/**
 * Gets the final y coordinate of the ball wihout top and bottom wall bounces accounted for and determines the actual final y coordinate of the ball.
 * @param final_ball_y the predicted y coordinate of the ball without bounces accounted for
 * @return the actual final y coordinate of the ball
 */
int calculate_bounces(int final_ball_y) {
    while (1) {
        if (final_ball_y < LIVES_FONT_SIZE) {
            final_ball_y = 2 * LIVES_FONT_SIZE - final_ball_y;
        } else if (final_ball_y > LCD_HEIGHT - BALL_SIZE) {
            final_ball_y = 2 * (LCD_HEIGHT - BALL_SIZE) - final_ball_y;
        } else {
            return final_ball_y;
        }
    }
}

/**
 * Determine the direction of travel of the ball on the x axis.
 * @param game_data a pointer to the struct game_data given to the ai by the game
 * @return -1 if the ball is traveling left \n
 *          0 if the ball is stationary for some reason \n
 *          1 if the ball is travelling right
 */
char get_ball_x_dir(struct game_data* game_data) {
    if (game_data->ball_vel_x < 0) return -1;
    if (game_data->ball_vel_x > 0) return 1;
    return 0;
}

/**
 * Determine whether the ball is coming towards the ai or not.
 * @param is_right 0 if the ai is on the left, 1 if the ai is on the right
 * @param ball_x_dir -1 if the ball is goiing to the left, 1 if the ball is going to the right
 * @return 1 if the ball is coming towards the ai, 0 otherwise
 */
char ball_is_coming_towards_ai(char is_right, char ball_x_dir) {
    if (is_right) {
        if (ball_x_dir == (char)1) {
            return 1;
        } else {
            return 0;
        }
    } else {
        if (ball_x_dir == (char)-1) {
            return 1;
        } else {
            return 0;
        }
    }
}

/**
 * Determine how much to change the target paddle position to make the ball bounce off the paddle in a random direction.
 * @return a offset of the paddle in pixels to randomize the ball bounce
 */
int random_bounce(void) {
    int range = PADDLE_HEIGHT + BALL_SIZE - 1;
    return rand() % range;
}
