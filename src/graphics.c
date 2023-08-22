/** @file
 * Module that contains function for non-specific interactions with the lcd
 * display and its buffer. It does not include functions with specific use such
 * as menu related functions.
 */

#include "graphics.h"

/**
 * wraps around function from "mzapo_phys.h" that maps lcd address to memory \n
 * exits program if lcd was not mapped properly
 *
 * @returns pointer to base address of lcd display \n
 *          exits program on error
 */
unsigned char *init_lcd() {
    unsigned char *lcd_membase = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (lcd_membase == NULL) {
        print_log(GRAPHICS_HEADER, "lcd display was not mapped");
        exit(1);
    }
    return lcd_membase;
}

/**
 * allocates memory for new frame buffer
 *
 * @returns pointer to new frame buffer
 */
uint16_t *init_frame(void) {
    uint16_t *frame = (uint16_t*)malloc(LCD_WIDTH * LCD_HEIGHT * sizeof(uint16_t));
    if (frame == NULL) {
        print_log(GRAPHICS_HEADER, "error in frame allocation");
        exit(1);
    }
    return frame;
}

/**
 * frees all memory taken by passed frame buffer
 *
 * @param frame buffer to destroy
 */
void destroy_frame(uint16_t *frame) {
    free(frame);
}

/**
 * renders content of frame on the lcd display
 *
 * @param frame pointer to frame buffer that has content to be rendered
 * @param lcd_membase pointer to base address of lcd display to render on
 */
void show_frame(uint16_t *frame, unsigned char *lcd_membase) {
    parlcd_write_cmd(lcd_membase, LCD_WRITE);
    for (int i = 0; i < LCD_HEIGHT * LCD_WIDTH; i++) {
        parlcd_write_data(lcd_membase, frame[i]);
    }
}

/**
 * clears lcd display (turns it to black)
 *
 * @param lcd_membase pointer to base memory of display to clear
 */
void reset_lcd(unsigned char *lcd_membase) {
    parlcd_write_cmd(lcd_membase, LCD_WRITE);
    for (int i = 0; i < LCD_HEIGHT * LCD_WIDTH; i++) {
        parlcd_write_data(lcd_membase, 0x0u);
    }
}

/**
 * puts pixel on coordinates in frame buffer
 *
 * @param x horizontal coordinate to put the pixel on
 * @param y vertical coordinate to put the pixel on
 * @param color color of the placed pixel in rgb 565 format
 * @param frame pointer to frame buffer in which the pixel is placed
 */
void put_pixel(int x, int y, uint16_t color, uint16_t *frame) {
    /* check if the pixel is in bounds of the lcd display */
    if (x >= 0 && x < LCD_WIDTH && y >=0 && y < LCD_HEIGHT) {
        *(frame + y * LCD_WIDTH + x) = color;
    }
}

/**
 * creates title page and puts it into frame buffer
 *
 * @param frame pointer to frame buffer to put title page pixels in
 * @param font pointer to structure with font to write text in
 */
void create_title_page(uint16_t *frame, font_descriptor_t *font) {
    put_string((LCD_WIDTH - get_string_width(font, TITLE)) / 2, 50, frame, font, TITLE, BLUE, BACKGROUND);
    put_string((LCD_WIDTH - get_string_width(font, ART)) / 2, 200, frame, font, ART, PINK, BACKGROUND);
}

/**
 * creates credits page and puts it into frame buffer
 *
 * @param frame pointer to frame buffer to put credits page pixels in
 * @param bigfont pointer to structure with big font to write text in
 * @param smallfont pointer to structure with small font to write text in
 */
void create_end_page(uint16_t *frame, font_descriptor_t *bigfont, font_descriptor_t *smallfont) {
    put_string((LCD_WIDTH - get_string_width(smallfont, END_HEADLINE)) / 2, 30, frame, smallfont, END_HEADLINE, BLUE, BACKGROUND);
    put_string((LCD_WIDTH - get_string_width(bigfont, USER1)) / 2, 94, frame, bigfont, USER1, BLUE, BACKGROUND);
    put_string((LCD_WIDTH - get_string_width(bigfont, USER2)) / 2, 202, frame, bigfont, USER2, BLUE, BACKGROUND);
}

/**
 * fills frame with black pixels -> blank display after render
 *
 * @param frame pointer to frame buffer to be cleared
 */
void clear_frame(uint16_t *frame) {
    for (int i = 0; i < LCD_HEIGHT * LCD_WIDTH; i++) {
        frame[i] = BACKGROUND;
    }
}

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
void create_result_page(int left_lives, int right_lives, int max_lives, uint16_t winner_color, uint16_t *frame, unsigned char *lcd_membase) {
    put_string((LCD_WIDTH - get_string_width(&font_wArial_88, RESULT_HEADLINE)) / 2, 20, frame, &font_wArial_88, RESULT_HEADLINE, BLUE, BACKGROUND);
    char *winner = left_lives > right_lives ? LEFT_WINNER : RIGHT_WINNER;
    put_string((LCD_WIDTH - get_string_width(&font_wArial_44, winner)) / 2, 108, frame, &font_wArial_44, winner, winner_color, BACKGROUND);
    char result[6];
    if (!sprintf(result, "%c - %c", (char)(max_lives - right_lives + '0'), (char)(max_lives - left_lives + '0'))) {
        print_log(GRAPHICS_HEADER, "result page sprintf error");
        exit(1);
    }
    put_string((LCD_WIDTH - get_string_width(&font_wArial_44, result)) / 2, 172, frame, &font_wArial_44, result, BLUE, BACKGROUND);
}

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
void create_highscore_page(int highscore, uint16_t player_color, uint16_t *frame, unsigned char *lcd_membase, font_descriptor_t *smallfont, font_descriptor_t *bigfont) {
    clear_frame(frame);
    put_string((LCD_WIDTH - get_string_width(bigfont, HIGHSCORE_HEADLINE)) / 2, 20, frame, bigfont, HIGHSCORE_HEADLINE, BLUE, BACKGROUND);
    char score_string[11];
    if (!sprintf(score_string, "%d", highscore)) {
        print_log(GRAPHICS_HEADER, "highscore page sprintf error");
        exit(1);
    }
    put_string((LCD_WIDTH - get_string_width(bigfont, score_string)) / 2, 128, frame, bigfont, score_string, player_color, BACKGROUND);
}

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
void create_not_highscore_page(int score, int highscore, uint16_t player_color, uint16_t *frame, unsigned char *lcd_membase, font_descriptor_t *smallfont) {
    clear_frame(frame);
    put_string((LCD_WIDTH - get_string_width(smallfont, NOT_HIGHSCORE_HEADLINE)) / 2, 20, frame, smallfont, NOT_HIGHSCORE_HEADLINE, BLUE, BACKGROUND);
    char score_string[11];
    if (!sprintf(score_string, "%d", score)) {
        print_log(GRAPHICS_HEADER, "not highscore page sprintf error 1");
        exit(1);
    }
    put_string((LCD_WIDTH - get_string_width(smallfont, score_string)) / 2, 74, frame, smallfont, score_string, player_color, BACKGROUND);
    put_string((LCD_WIDTH - get_string_width(smallfont, NOT_HIGHSCORE_SUBHEADLINE)) / 2, 138, frame, smallfont, NOT_HIGHSCORE_SUBHEADLINE, BLUE, BACKGROUND);
    if (!sprintf(score_string, "%d", highscore)) {
        print_log(GRAPHICS_HEADER, "not highscore page sprintf error 2");
        exit(1);
    }
    put_string((LCD_WIDTH - get_string_width(smallfont, score_string)) / 2, 192, frame, smallfont, score_string, RED, BACKGROUND);
}

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
void create_start_game_page(settings_t *settings, uint16_t *frame, unsigned char* lcd_membase, font_descriptor_t *bigfont, font_descriptor_t *smallfont) {
    clear_frame(frame);
    put_string((LCD_WIDTH - get_string_width(bigfont, GAME_START)) / 2, 20, frame, bigfont, GAME_START, BLUE, BACKGROUND);
    char *player = settings->left == PLAYER ? "PLAYER" : settings->ai_label;
    put_string((LCD_WIDTH - get_string_width(smallfont, player)) / 2, 128, frame, smallfont, player, settings->paddlecolors[0], BACKGROUND);
    put_string((LCD_WIDTH - get_string_width(smallfont, GAME_START_SEPARATOR)) / 2, 192, frame, smallfont, GAME_START_SEPARATOR, BLUE, BACKGROUND);
    player = settings->right == PLAYER ? "PLAYER" : settings->ai_label;
    put_string((LCD_WIDTH - get_string_width(smallfont, player)) / 2, 256, frame, smallfont, player, settings->paddlecolors[1], BACKGROUND);
}

/**
 * adds end message to frame, displays it and waits for any key press
 *
 * @param frame pointer to frame buffer to put result page pixels in
 * @param lcd_membase base memory to mapped lcd display
 * @param knobs structure that enables getting input from knobs on the board
 */
void show_and_wait(uint16_t *frame, unsigned char *lcd_membase, knobs_t *knobs) {
    put_string((LCD_WIDTH - get_string_width(&font_wArial_44, END_MESSAGE)) / 2, SHOW_AND_WAIT_Y_OFFSET, frame, &font_wArial_44, END_MESSAGE, GREY, BACKGROUND);
    show_frame(frame, lcd_membase);
    char c;
    while (read(STDIN_FILENO, &c, 1) != 1 && !knobs_pushed(knobs)) {}
}
