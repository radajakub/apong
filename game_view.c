/** @file
*/

#include "game_view.h"
#include "mzapo_parlcd.h"
#include "font_types.h"
#include "text.h"
#include "graphics.h"
#include "log.h"
#include "game.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void clear_buffer(void);
void add_lives_background(void);
void add_middle_line(void);
void add_lives(void);
void add_time(void);
void add_score(int score);
void add_paddles(void);
void add_ball(void);
void render(void);
void add_post_game_screen_reminder(void);
void easter_egg(void);
uint16_t random_color(void);

static unsigned char* lcd_mem;
static uint16_t display_buff[LCD_HEIGHT * LCD_WIDTH];
static struct game_data data;
static uint16_t ball_color, left_paddle_color, right_paddle_color;
static clock_t game_time = 0;
static clock_t last_update = 0;


/**
 * Save the given pointer to the lcd display memory.
 * @param lcd_membase the base of the memory of the lcd display to render to
 * @param settings the settings given from the menu
 */
void init_view(unsigned char* lcd_membase, settings_t* settings) {
    lcd_mem = lcd_membase;
    ball_color = settings->ballcolor;
    left_paddle_color = settings->paddlecolors[0];
    right_paddle_color = settings->paddlecolors[1];
    last_update = clock();
    if (LOG_GAME_VIEW) print_log(LOG_HEAD_GAME_VIEW, "initialized");
}

/**
 * Store the given game data and use it to render all game components.
 * @param game_data contains information about the state of the game
 * @param score the current score of the player; is set to -1 in PvP mode
 */
void update_view(struct game_data game_data, int score) {
    data = game_data;
    clear_buffer();
    add_lives_background();
    add_middle_line();
    if (data.lives_left >= 0) add_lives();
    score >= 0 ? add_score(score) : add_time();
    add_paddles();
    add_ball();
    render();
}

/**
 * Fill the display buffer with blackness.
 */
void clear_buffer(void) {
    int window_size = LCD_HEIGHT * LCD_WIDTH;
    for (int i = 0; i < window_size; i++) display_buff[i] = BACKGROUND_COLOR;
}

/**
 * Add the background above the game court where the lives are displayed.
 */
void add_lives_background(void) {
    for (int y = 0; y < LIVES_FONT_SIZE; y++) {
        for (int x = 0; x < LCD_WIDTH; x++) {
            display_buff[y * LCD_WIDTH + x] = LIVES_BACKGROUND_COLOR;
        }
    }
}

/**
 * Add the middle line.
 */
void add_middle_line(void) {
    int center_x = (LCD_WIDTH - MIDDLE_LINE_WIDTH) / 2;
    for (int y = LIVES_FONT_SIZE; y < LCD_HEIGHT; y++) {
        if (y / MIDDLE_LINE_LENGTH % 2 == 0) {
            for (int x = 0; x < MIDDLE_LINE_WIDTH; x++) {
                display_buff[y * LCD_WIDTH + center_x + x] = MIDDLE_LINE_COLOR;
            }
        }
    }
}

/**
 * Render player lives into the display buffer using the stored data.
 */
void add_lives(void) {
    char number[2];
    sprintf(number, "%d", data.lives_left);
    put_string(0, 0, display_buff, &font_wArial_44, number, (uint16_t)LIVES_COLOR, (uint16_t)LIVES_BACKGROUND_COLOR);
    sprintf(number, "%d", data.lives_right);
    put_string(LCD_WIDTH - get_char_width(&font_wArial_44, number[0]), 0, display_buff, &font_wArial_44, number, (uint16_t)LIVES_COLOR, (uint16_t)LIVES_BACKGROUND_COLOR);
}

/**
 * Render game time.
 */
void add_time(void) {
    clock_t now = clock();
    game_time += now - last_update;
    last_update = now;
    if (game_time < 0 || game_time > MAX_TIME) {
        easter_egg();
    } else {
        char time[6];
        int seconds = game_time / CLOCKS_PER_SEC;
        int minutes = seconds / 60;
        seconds = seconds % 60;
        sprintf(time, "%d:%d", minutes, seconds);
        put_string((LCD_WIDTH - get_string_width(&font_wArial_44, time)) / 2, 0, display_buff, &font_wArial_44, time, (uint16_t)TIME_SCORE_COLOR, (uint16_t)LIVES_BACKGROUND_COLOR);
    }
}

/**
 * Render player score.
 * @param score the current score of the player
 */
void add_score(int score) {
    if (score < -1) {
        return;
    } else if (score > MAX_SCORE) {
        easter_egg();
    } else {
        char score_text[6];
        sprintf(score_text, "%d", score);
        put_string((LCD_WIDTH - get_string_width(&font_wArial_44, score_text)) / 2, 0, display_buff, &font_wArial_44, score_text, (uint16_t)TIME_SCORE_COLOR, (uint16_t)LIVES_BACKGROUND_COLOR);
    }
}

/**
 * Render the paddles into the display buffer using the stored data.
 */
void add_paddles(void) {
    for (int y = data.paddle_left_pos; y < data.paddle_left_pos + PADDLE_HEIGHT; y++) {
        for (int x = 0; x < PADDLE_WIDTH; x++) {
            display_buff[y * LCD_WIDTH + x] = left_paddle_color;
        }
    }
    for (int y = data.paddle_right_pos; y < data.paddle_right_pos + PADDLE_HEIGHT; y++) {
        for (int x = LCD_WIDTH - PADDLE_WIDTH; x < LCD_WIDTH; x++) {
            display_buff[y * LCD_WIDTH + x] = right_paddle_color;
        }
    }
}

/**
 * Render the ball into the display buffer using the stored data.
 */
void add_ball(void) {
    for (int y = 0; y < BALL_SIZE; y++) {
        for (int x = 0; x < BALL_SIZE; x++) {
            display_buff[(data.ball_pos_y + y) * LCD_WIDTH + (data.ball_pos_x + x)] = ball_color;
        }
    }
}

/**
 * Copy the pixels from the display buffer to the actual display memory.
 */
void render(void) {
    parlcd_write_cmd(lcd_mem, LCD_WRITE);
    int buffer_size = LCD_HEIGHT * LCD_WIDTH;
    for (int i = 0; i < buffer_size; i++) parlcd_write_data(lcd_mem, display_buff[i]);
}

/**
 * View post-game screen with displayed score.
 * @param score the score to be displayed
 */
void view_score_screen(int score) {
    for (int i = 0; i < LCD_HEIGHT * LCD_WIDTH; i++) display_buff[i] = POST_GAME_SCREEN_BACKGROUND;
    if (0 <= score && score <= MAX_SCORE) {
        char score_text[13];
        sprintf(score_text, "SCORE: %d", score);
        add_post_game_screen_reminder();
        put_string((LCD_WIDTH - get_string_width(&font_wArial_88, score_text)) / 2, (LCD_HEIGHT - 88) / 2, display_buff, &font_wArial_88, score_text, POST_GAME_SCREEN_FOREGROUND, POST_GAME_SCREEN_BACKGROUND);
    }
    render();
}

/**
 * View post-game screen with the winner displayed.
 * @param winner 0 for left \n
 *               1 for right
 */
void view_victory_screen(char winner) {
    for (int i = 0; i < LCD_HEIGHT * LCD_WIDTH; i++) display_buff[i] = POST_GAME_SCREEN_BACKGROUND;
    add_post_game_screen_reminder();
    if (winner) {
        char str[] = "RIGHT PLAYER WINS";
        put_string((LCD_WIDTH - get_string_width(&font_wArial_44, str)) / 2, (LCD_HEIGHT - 44) / 2, display_buff, &font_wArial_44, str, POST_GAME_SCREEN_FOREGROUND, POST_GAME_SCREEN_BACKGROUND);
    } else {
        char str[] = "LEFT PLAYER WINS";
        put_string((LCD_WIDTH - get_string_width(&font_wArial_44, str)) / 2, (LCD_HEIGHT - 44) / 2, display_buff, &font_wArial_44, str, POST_GAME_SCREEN_FOREGROUND, POST_GAME_SCREEN_BACKGROUND);
    }
    render();
}

/**
 * Add a reminder how to exit the post-game screen.
 */
void add_post_game_screen_reminder(void) {
    char str[] = "PRESS 'ENTER'";
    put_string((LCD_WIDTH - get_string_width(&font_wArial_44, str)) / 2, LCD_HEIGHT - 44, display_buff, &font_wArial_44, str, POST_GAME_SCREEN_FOREGROUND, POST_GAME_SCREEN_BACKGROUND);
}

/**
 * Muhaha
 */
void easter_egg(void) {
    int window_size = LCD_HEIGHT * LCD_WIDTH;
    uint8_t m = 0;
    char str[3] = ":)";
    uint16_t color1 = 0;
    uint16_t color2 = 0;
    while (1) {
        color1 = random_color();
        color2 = random_color();
        for (int i = 0; i < window_size; i++) display_buff[i] = color1;
        put_string((LCD_WIDTH - get_string_width(&font_wArial_88, str)) / 2, (LCD_HEIGHT - 88) / 2, display_buff, &font_wArial_88, str, color2, color1);
        render();
        if (m == (uint8_t)0) {
            printf("poop");
            light_leds(lcd_mem, 0xf0f0f0f0u);
            m++;
        } else if (m == (uint8_t)1){
            light_leds(lcd_mem, 0xc3c3c3c3u);
            m++;
        } else if (m == (uint8_t)2) {
            light_leds(lcd_mem, 0x0f0f0f0fu);
            m++;
        } else if (m == (uint8_t)3) {
            light_leds(lcd_mem, 0x3c3c3c3cu);
            m = 0;
        }
        usleep(200000);
    }
}

uint16_t random_color(void) {
    switch (rand() % 10) {
        case 0:
            return EMPTY;
        case 1:
            return BLACK;
        case 2:
            return GREY;
        case 3:
            return WHITE;
        case 4:
            return RED;
        case 5:
            return GREEN;
        case 6:
            return BLUE;
        case 7:
            return YELLOW;
        case 8:
            return PINK;
        case 9:
            return (uint16_t)1024;
        default:
            return (uint16_t)0;
    }
}
