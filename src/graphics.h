/** @file
 * Module that contains function for non-specific interactions with the lcd
 * display and its buffer. It does not include functions with specific use such
 * as menu related functions.
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "rgb565.h"
#include "text.h"
#include "font_types.h"
#include "log.h"
#include "peripherals.h"
#include "settings.h"

#define LCD_WIDTH 480
#define LCD_HEIGHT 320
#define LCD_WRITE 0x2c

#define BACKGROUND EMPTY

#define GRAPHICS_HEADER "GRAPHICS: "

#define TITLE "A P O N G !"
#define ART "<3 APO"

#define GAME_START "NEW GAME"
#define GAME_START_SEPARATOR "VS"

#define END_HEADLINE "MADE BY"
#define USER1 "radajak5"
#define USER2 "soldasim"

#define RESULT_HEADLINE "WINNER"
#define LEFT_WINNER "LEFT PLAYER"
#define RIGHT_WINNER "RIGHT PLAYER"
#define END_MESSAGE "> press any key <"

#define HIGHSCORE_HEADLINE "HIGHSCORE"
#define NOT_HIGHSCORE_HEADLINE "YOUR SCORE"
#define NOT_HIGHSCORE_SUBHEADLINE "HIGHSCORE"

#define SHOW_AND_WAIT_Y_OFFSET (270)

/**
 * wraps around function from "mzapo_phys.h" that maps lcd address to memory \n
 * exits program if lcd was not mapped properly
 *
 * @returns pointer to base address of lcd display \n
 *          exits program on error
 */
unsigned char *init_lcd(void);

/**
 * allocates memory for new frame buffer
 *
 * @returns pointer to new frame buffer
 */
uint16_t *init_frame(void);

/**
 * frees all memory taken by passed frame buffer
 *
 * @param frame buffer to destroy
 */
void destroy_frame(uint16_t *frame);

/**
 * renders content of frame on the lcd display
 *
 * @param frame pointer to frame buffer that has content to be rendered
 * @param lcd_membase pointer to base address of lcd display to render on
 */
void show_frame(uint16_t *frame, unsigned char *lcd_membase);

/**
 * clears lcd display (turns it to black)
 *
 * @param lcd_membase pointer to base memory of display to clear
 */
void reset_lcd(unsigned char *lcd_membase);

/**
 * puts pixel on coordinates in frame buffer
 *
 * @param x horizontal coordinate to put the pixel on
 * @param y vertical coordinate to put the pixel on
 * @param color color of the placed pixel in rgb 565 format
 * @param frame pointer to frame buffer in which the pixel is placed
 */
void put_pixel(int x, int y, uint16_t color, uint16_t *frame);

/**
 * creates title page and puts it into frame buffer
 *
 * @param frame pointer to frame buffer to put title page pixels in
 * @param font pointer to structure with font to write text in
 */
void create_title_page(uint16_t *frame, font_descriptor_t *font);

/**
 * creates credits page and puts it into frame buffer
 *
 * @param frame pointer to frame buffer to put credits page pixels in
 * @param bigfont pointer to structure with big font to write text in
 * @param smallfont pointer to structure with small font to write text in
 */
void create_end_page(uint16_t *frame, font_descriptor_t *bigfont, font_descriptor_t *smallfont);

/**
 * fills frame with black pixels -> blank display after render
 *
 * @param frame pointer to frame buffer to be cleared
 */
void clear_frame(uint16_t *frame);

/**
 * creates page that displays winning player and the scores of the two of them \n
 * it is expected to call function show_and_wait after this function
 *
 * @param left_lives remaining lives of the left player
 * @param right_lives remaining lives of the right player
 * @param max_lives lives that had both players on the beggining of the game
 * @param winner_color paddle color of the winning player
 * @param frame pointer to frame buffer to put result page pixels in
 * @param lcd_membase base memory to mapped lcd display
 */
void create_result_page(int left_lives, int right_lives, int max_lives, uint16_t winner_color, uint16_t *frame, unsigned char *lcd_membase);

/**
 * creates page that displays new highscore \n
 * it is expected to call function show_and_wait after this function
 *
 * @param highscore new hihgscore that was made
 * @param player_color paddle color of the player who made new highscore
 * @param frame pointer to frame buffer to put result page pixels in
 * @param lcd_membase base memory to mapped lcd display
 * @param smallfont pointer to structure with small font to write text in
 * @param bigfont pointer to structure with big font to write text in
 */
void create_highscore_page(int highscore, uint16_t player_color, uint16_t *frame, unsigned char *lcd_membase, font_descriptor_t *smallfont, font_descriptor_t *bigfont);

/**
 * creates page that displays score that is not a highscore \n
 * it is expected to call function show_and_wait after this function
 *
 * @param score score made in the last game
 * @param highscore current hihgscore
 * @param player_color paddle color of the player who made the score
 * @param frame pointer to frame buffer to put result page pixels in
 * @param lcd_membase base memory to mapped lcd display
 * @param smallfont pointer to structure with small font to write text in
 */
void create_not_highscore_page(int score, int highscore, uint16_t player_color, uint16_t *frame, unsigned char *lcd_membase, font_descriptor_t *smallfont);

/**
 * creates page that show which type of game is played to create time for players
 * to prepare
 *
 * @param settings settings of the new game
 * @param frame frame to put the page in
 * @param lcd_membase pointer to base memory of lcd display
 * @param bigfont font in which is printed big text
 * @param smallfont font in which is printed small text
 */
void create_start_game_page(settings_t *settings, uint16_t *frame, unsigned char* lcd_membase, font_descriptor_t *bigfont, font_descriptor_t *smallfont);

/**
 * adds end message to frame, displays it and waits for any key press
 *
 * @param frame pointer to frame buffer to put result page pixels in
 * @param lcd_membase base memory to mapped lcd display
 * @param knobs structure that enables getting input from knobs on the board
 */
void show_and_wait(uint16_t *frame, unsigned char *lcd_membase, knobs_t *knobs);

#endif
