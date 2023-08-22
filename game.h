/** @file
*/

#ifndef GAME_H
#define GAME_H

#include "settings.h"
#include "peripherals.h"

#define LIVES_FONT_SIZE (44)
#define BALL_SIZE (20)
#define PADDLE_HEIGHT (80)
#define PADDLE_WIDTH (20)
#define INITIAL_LIVES (3)
#define BOUNCE_CONST (2)
#define BONUS_ON_AI_BALL_LOSS (3)

// ball speed is fixed for the x axis, and varies from this initial value slightly on the y axis to change the angle of travel
// updates per second directly affect speeds of objects as the speeds are equal to the number of pixels per update
#define UPDATES_PER_SECOND (50)
#define PADDLE_SPEED_KEY (3)
#define PADDLE_SPEED_KNOB (3)
#define BALL_SPEED_MEDIUM (5)
#define BALL_SPEED_EASY (3)
#define BALL_SPEED_HARD (8)

#define LOG_HEAD_GAME "GAME: "
#define LOG_GAME 1

#define HIT_BLINK_DURATION (200)
#define BALL_LOSS_BLINK_DURATION (1000)
#define BALL_LOSS_BLINK_PERIOD (200)
#define NORMAL_LED_COLOR (0x00ff00u)
#define HIT_BLINK_COLOR (0x0000ffu)
#define BALL_LOSS_BLINK_COLOR (0xff0000u)
#define LED_OFF_COLOR (0x0u)

/**
 * Contains information about game object positions and player lives. \n
 * The coordinates of an object are the coordinates of its top left corner. \n
 * The y coordinate is always within < LIVES_FONT_SIZE ; LCD_HEIGHT - [height of the object] > \n
 * The x coordinate is always within < 0 ; LCD_WIDTH - [width of the object] > \n
 * @see graphics.h for LCD_HEIGHT, LCD_WIDTH
 */
struct game_data {
    int ball_pos_x, ball_pos_y;
    int ball_vel_x, ball_vel_y;
    int paddle_left_pos;
    int paddle_right_pos;
    int lives_left;
    int lives_right;
};

/**
 * Call this function to initialize the game.
 * @param membase the base of the memory of the system
 * @param lcd_membase the base of the memory of the lcd display
 * @param knobs contains information about the input from the knobs
 * @param settings contains the settings from the menu
 * @return score if one player is human and one is bot, -1 otherwise
 */
int start_game(unsigned char* membase, unsigned char* lcd_membase, knobs_t* knobs, settings_t* settings);

#endif
