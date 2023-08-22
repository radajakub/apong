/** @file
*/

#ifndef GAME_VIEW_H
#define GAME_VIEW_H

#include "game.h"
#include "settings.h"

#define BACKGROUND_COLOR (0)
#define MIDDLE_LINE_COLOR (1024)
#define LIVES_BACKGROUND_COLOR (1024)
#define LIVES_COLOR (0xef44u)
#define TIME_SCORE_COLOR (0xef44u)
#define POST_GAME_SCREEN_FOREGROUND (1024)
#define POST_GAME_SCREEN_BACKGROUND (0)
#define MIDDLE_LINE_WIDTH (4)
#define MIDDLE_LINE_LENGTH (12)
#define MAX_SCORE (99999)
#define MAX_TIME (2100000000)

#define LOG_HEAD_GAME_VIEW "GAME_VIEW: "
#define LOG_GAME_VIEW 1

/**
 * Call when initializing the game.
 * @param lcd_membase a pointer to the base of the lcd display memory to render to
 * @param settings the game settings given from the menu
 */
void init_view(unsigned char* lcd_membase, settings_t* settings);

/**
 * Call to update the game view.
 * @param game_data an instance of struct game_data from the game.h file
 * @param score the score the player achieved in the game
 */
void update_view(struct game_data game_data, int score);

/**
 * REPLACED WITH POST-GAME SCREEN IMPLEMENTATION IN GRAPHICS.H
 * 
 * View post-game screen with displayed score.
 * @param score the score to be displayed
 */
void view_score_screen(int score);

/**
 * REPLACED WITH POST-GAME SCREEN IMPLEMENTATION IN GRAPHICS.H
 * 
 * View post-game screen with the winner displayed.
 * @param winner 0 for left \n
 *               1 for right
 */
void view_victory_screen(char winner);

#endif
