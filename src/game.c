/** @file
*/

#include "game.h"
#include "game_view.h"
#include "player_input.h"
#include "mzapo_regs.h"
#include "log.h"
#include "graphics.h"
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "basic_ai.h"
#include "better_ai.h"

void init_game(void);
void init_data(void);
void update_loop(void);
void update(void);
void update_paddles(void);
void update_player_paddle(char is_right, char last_key, int knob_diff);
void update_ai_paddle(char is_right);
void move_ai_paddle(char is_right, char dir);
void move_paddle(char is_right, int distance);
char update_ball(void);
void check_ball_top_bot_edge_collision(void);
void check_ball_paddle_collision(void);
void on_hit_change_ball_vel_y(int paddle_y, int ball_y, int overshoot);
char check_ball_left_right_edge_collision(void);
void on_ball_left_right_edge_collision(char side);
char update_lives(char ball_ret);
void reset_ball(void);
void move_led_line(void);
void hit_blink(char is_right);
void ball_loss_blink(char is_right);
void update_diodes(void);
void post_game_screen(void);

static int ball_speed;
static struct game_data data;
static int score;
static settings_t* game_settings;
static char game_running;
static unsigned char* memory;
static knobs_t* input_knobs;
static char last_key[2];
static int hit_blink_left_countdown;
static int hit_blink_right_countdown;
static int ball_loss_blink_left_countdown;
static int ball_loss_blink_right_countdown;

/**
 * Initialize and start the game.
 * @param membase the base of the memory of the system
 * @param lcd_membase the base of the memory of the lcd display
 * @param knobs contains information about the input from the knobs
 * @param settings contains the settings from the menu
 * @return score if one player is human and one is bot, -1 otherwise
 */
int start_game(unsigned char* membase, unsigned char* lcd_membase, knobs_t* knobs, settings_t* settings) {
    memory = membase;
    input_knobs = knobs;
    game_settings = settings;
    led_settings_t* led_settings = init_led_settings(membase);
    init_game();
    init_view(lcd_membase, settings);
    update_loop();
    restore_led_settings(membase, led_settings);
    if ((settings->left == PLAYER && settings->right == PLAYER) || (settings->left == BOT && settings->right == BOT)) {
        uint16_t frame[LCD_HEIGHT * LCD_WIDTH];
        for (int i = 0; i < LCD_HEIGHT * LCD_WIDTH; i++) frame[i] = BACKGROUND;
        create_result_page(data.lives_left, data.lives_right, INITIAL_LIVES, settings->paddlecolors[data.lives_left ? 0 : 1], frame, lcd_membase);
        show_and_wait(frame, lcd_membase, knobs);
    }
    destroy_led_settings(led_settings);
    return score;
}

/**
 * Initialize the game.
 */
void init_game(void) {
    init_data();
    light_left_diode(memory, NORMAL_LED_COLOR);
    light_right_diode(memory, NORMAL_LED_COLOR);
    last_key[0] = 0;
    last_key[1] = 0;
    hit_blink_left_countdown = 0;
    hit_blink_right_countdown = 0;
    ball_loss_blink_left_countdown = 0;
    ball_loss_blink_right_countdown = 0;
    if ((game_settings->left == PLAYER && game_settings->right == BOT) || (game_settings->left == BOT && game_settings->right == PLAYER)) {
        score = 0;
    } else {
        score = -1;
    }
    if (LOG_GAME) print_log(LOG_HEAD_GAME, "game initialized");
}

/**
 * Initialize game data.
 */
void init_data(void) {
    switch (game_settings->difficulty) {
        case EASY:
            ball_speed = BALL_SPEED_EASY;
            break;
        case MEDIUM:
            ball_speed = BALL_SPEED_MEDIUM;
            break;
        case HARD:
            ball_speed = BALL_SPEED_HARD;
            break;
        default:
            ball_speed = 0;
            if (LOG_GAME) print_log(LOG_HEAD_GAME, "ERROR: wrong difficulty level in settings");
    }
    int paddle_init_pos = LIVES_FONT_SIZE + (LCD_HEIGHT - LIVES_FONT_SIZE - PADDLE_HEIGHT) / 2;
    data.paddle_left_pos = paddle_init_pos;
    data.paddle_right_pos = paddle_init_pos;
    if ((game_settings->left == PLAYER && game_settings->right == PLAYER) || (game_settings->left == BOT && game_settings->right == BOT)) {
        data.lives_left = INITIAL_LIVES;
        data.lives_right = INITIAL_LIVES;
    } else {
        data.lives_left = -1;
        data.lives_right = -1;
    }
    reset_ball();
}

/**
 * Handles the game update loop with set updates per second. \n
 * Calls update() to update the game data and update_view() to update the view.
 */
void update_loop(void) {
    int clocks_per_update = CLOCKS_PER_SEC / UPDATES_PER_SECOND;
    clock_t last, now;
    last = clock();
    int delta = 0;
    game_running = 1;
    if (LOG_GAME) print_log(LOG_HEAD_GAME, "update loop initialized");
    while(game_running) {
        now = clock();
        delta += now - last;
        last = now;
        if (delta >= clocks_per_update) {
            delta -= clocks_per_update;
            update();
            update_view(data, score);
        }
    }
}

/**
 * Check for user input, update game object positions, check for collisions.
 */
void update(void) {
    move_led_line();
    update_paddles();
    char ball_ret = update_ball();
    on_ball_left_right_edge_collision(ball_ret);
    update_diodes();
}

/**
 * Update the positions of the paddles according to the user input or AI decisions.
 */
void update_paddles(void) {
    struct input input = get_input();
    get_knob_value(input_knobs);
    if (game_settings->left == PLAYER) {
        char paddle_dir = 0;
        if (input.left_up && !input.left_down) paddle_dir = -1;
        if (input.left_down && !input.left_up) paddle_dir = 1;
        update_player_paddle(0, paddle_dir, get_knob_movement(input_knobs, RED_K));
    } else {
        update_ai_paddle(0);
    }
    if (game_settings->right == PLAYER) {
        char paddle_dir = 0;
        if (input.right_up && !input.right_down) paddle_dir = -1;
        if (input.right_down && !input.right_up) paddle_dir = 1;
        update_player_paddle(1, paddle_dir, get_knob_movement(input_knobs, BLUE_K));
    } else {
        update_ai_paddle(1);
    }
}


/**
 * Move the paddle accordingly to the given inputs from the keyboard and the knob.
 * @param is_right specifies which paddle is to be updated (0 for left, 1 for right)
 * @param key -1 if the key for "UP" has been pressed since the last update \n
 *             1 if the key for "DOWN" has been pressed since the last update \n
 *             0 if no key controls registered since the last update
 * @param knob_diff describes how has the knob moved relatively to the position upon previous update \n
 *                  <0 for counter-clockwise, >0 for clockwise, 0 for no movement
 */
void update_player_paddle(char is_right, char key, int knob_diff) {
    if (knob_diff) {
        last_key[(int)is_right] = 0;
        move_paddle(is_right, (is_right ? -1 : 1) * knob_diff * PADDLE_SPEED_KNOB);
    } else {
        if (key) last_key[(int)is_right] = key;
        if (last_key[(int)is_right] == (char)-1) {
            move_paddle(is_right, -PADDLE_SPEED_KEY);
        } else if (last_key[(int)is_right] == (char)1) {
            move_paddle(is_right, PADDLE_SPEED_KEY);
        }
    }
}

/**
 * Move the paddle according to the direction given by the AI.
 * @param is_right specifies which paddle is to be updated (0 for left, 1 for right)
 */
void update_ai_paddle(char is_right) {
    char c;
    switch (game_settings->ai) {
        case DUMB_AI:
            c = basic_ai_move(is_right, data);
            break;
        case SMARTER_AI:
            c = better_ai_move(is_right, data);
            break;
        default:
            if (LOG_GAME) print_log(LOG_HEAD_GAME, "ERROR: AI id number not recognized");
            return;
    }
    move_ai_paddle(is_right, c);
}

/**
 * Move the paddle according to the direction given by the AI.
 * @param is_right 0 for left paddle, 1 for right paddle
 * @param dir the direction given by the AI
 */
void move_ai_paddle(char is_right, char dir) {
    if (dir == (char)-1) {
        move_paddle(is_right, -PADDLE_SPEED_KEY);
    } else if (dir == (char)1) {
        move_paddle(is_right, PADDLE_SPEED_KEY);
    }
}

/**
 * Move a paddle by the given distance.
 * @param is_right set to 0 to move the left paddle, set to 1 to move the right paddle
 * @param distance the distance by which the paddle is to be moved; \n
 *                 negative values for upwards direction, positive values for downwards direction
 */
void move_paddle(char is_right, int distance) {
    if (is_right) {
        data.paddle_right_pos += distance;
        if (data.paddle_right_pos < LIVES_FONT_SIZE) data.paddle_right_pos = LIVES_FONT_SIZE;
        if (data.paddle_right_pos > LCD_HEIGHT - PADDLE_HEIGHT) data.paddle_right_pos = LCD_HEIGHT - PADDLE_HEIGHT;
    } else {
        data.paddle_left_pos += distance;
        if (data.paddle_left_pos < LIVES_FONT_SIZE) data.paddle_left_pos = LIVES_FONT_SIZE;
        if (data.paddle_left_pos > LCD_HEIGHT - PADDLE_HEIGHT) data.paddle_left_pos = LCD_HEIGHT - PADDLE_HEIGHT;
    }
}

/**
 * Check ball collisions and update the ball.
 * @return -1 if the ball touches the left edge of the game court, \n
 *          1 if the ball touches the right edge of the game court, \n
 *          0 otherwise
 */
char update_ball(void) {
    data.ball_pos_y += data.ball_vel_y;
    data.ball_pos_x += data.ball_vel_x;
    check_ball_top_bot_edge_collision();
    check_ball_paddle_collision();
    return check_ball_left_right_edge_collision();
}

/**
 * Check ball collisions with the top and bottom edges of the game court and invert the y velocity of the ball.
 */
void check_ball_top_bot_edge_collision(void) {
    if (data.ball_pos_y < LIVES_FONT_SIZE) {
        data.ball_pos_y = 2 * LIVES_FONT_SIZE - data.ball_pos_y;
        data.ball_vel_y = -data.ball_vel_y;
        if (LOG_GAME) print_log(LOG_HEAD_GAME, "top wall hit");
    }
    if (data.ball_pos_y > LCD_HEIGHT - BALL_SIZE) {
        data.ball_pos_y = 2 * LCD_HEIGHT - 2 * BALL_SIZE - data.ball_pos_y;
        data.ball_vel_y = -data.ball_vel_y;
        if (LOG_GAME) print_log(LOG_HEAD_GAME, "bot wall hit");
    }
}

/**
 * Check for ball collisions with the paddles,
 * move the ball to the correct x coordinate and invert the x velocity of the ball accordingly. \n
 * Call on_hit_change_ball_vel_y to handle y coordinate correction and y velocity update.
 */
void check_ball_paddle_collision(void) {
    static int left_limit = PADDLE_WIDTH;
    static int right_limit = LCD_WIDTH - PADDLE_WIDTH - BALL_SIZE;
    int previous_y = data.ball_pos_y - data.ball_vel_y;
    int previous_x = data.ball_pos_x - data.ball_vel_x;
    if (data.ball_pos_x < left_limit && previous_x >= left_limit) {
        double ball_dir = data.ball_vel_y / data.ball_vel_x;
        int hit_y = (double)previous_y + (double)(left_limit - previous_x) * ball_dir + 0.5;
        if ((data.paddle_left_pos > hit_y - PADDLE_HEIGHT) && (data.paddle_left_pos < hit_y + BALL_SIZE)) {
            on_hit_change_ball_vel_y(data.paddle_left_pos, hit_y, left_limit - data.ball_pos_x);
            data.ball_pos_x = 2 * left_limit - data.ball_pos_x;
            data.ball_vel_x = -data.ball_vel_x;
            hit_blink(0);
            if (score >= 0 && game_settings->left == PLAYER) score++;
            if (LOG_GAME) print_log(LOG_HEAD_GAME, "left paddle hit");
        }
    } else if (data.ball_pos_x > right_limit && previous_x <= right_limit) {
        double ball_dir = data.ball_vel_y / data.ball_vel_x;
        int hit_y = (double)previous_y + (double)(previous_x - right_limit) * ball_dir + 0.5;
        if ((data.paddle_right_pos > hit_y - PADDLE_HEIGHT) && (data.paddle_right_pos < hit_y + BALL_SIZE)) {
            on_hit_change_ball_vel_y(data.paddle_right_pos, hit_y, data.ball_pos_x - right_limit);
            data.ball_pos_x = 2 * right_limit - data.ball_pos_x;
            data.ball_vel_x = -data.ball_vel_x;
            hit_blink(1);
            if (score >= 0 && game_settings->right == PLAYER) score++;
            if (LOG_GAME) print_log(LOG_HEAD_GAME, "right paddle hit");
        }
    }
}

/**
 * Change the y velocity of the ball acording to with which part of the paddle it has been hit. \n
 * Also recalculate last update in ball y coordinate.
 * @param paddle_y the y coordinate of the paddle when the hit occured
 * @param ball_y the y coordinate of the ball when the hit occured
 * @param overshoot how much did the last update (which possibly went through the paddle) overshoot the point where the ball was supposed to change direction
 */
void on_hit_change_ball_vel_y(int paddle_y, int ball_y, int overshoot) {
    double paddle_half_span = (double)(PADDLE_HEIGHT + BALL_SIZE - 1) / 2;
    double ball_middle_y = (double)ball_y + (double)BALL_SIZE / 2 - 0.5;
    double paddle_middle_y = (double)paddle_y + (double)PADDLE_HEIGHT / 2 - 0.5;
    double relative_hit_y = ball_middle_y - paddle_middle_y;
    double ball_dir = BOUNCE_CONST * relative_hit_y / paddle_half_span;
    data.ball_vel_y = ball_dir * ball_speed + 0.5;
    data.ball_pos_y = ball_y + (int)((double)overshoot * ball_dir + 0.5);
}

/**
 * Check ball collisions with the left and right edges of the game court. \n
 * Return non-zero value on collision.
 * @return -1 on left edge collision \n
 *          1 on right edge collision \n
 *          0 on no collision
 */
char check_ball_left_right_edge_collision(void) {
    if (data.ball_pos_x < 0) {
        data.ball_pos_x = 0;
        if (LOG_GAME) print_log(LOG_HEAD_GAME, "left player lost");
        return -1;
    }
    if (data.ball_pos_x > LCD_WIDTH - BALL_SIZE) {
        data.ball_pos_x = LCD_WIDTH - BALL_SIZE;
        if (LOG_GAME) print_log(LOG_HEAD_GAME, "right player lost");
        return 1;
    }
    return 0;
}

/**
 * Update lives accordingly, reset the ball or end the game depending on remaining lives.
 * @param side -1 for collision with left edge \n
 *              1 for collision with right edge \n
 *              function does nothing if 0
 */
void on_ball_left_right_edge_collision(char side) {
    if (side) {
        ball_loss_blink(side == (char)-1 ? 0 : 1);
        if (data.lives_left >= 0) {
            char lives_ret = update_lives(side);
            if (lives_ret) {
                game_running = 0;
                if (lives_ret == -1) {
                    if (LOG_GAME) print_log(LOG_HEAD_GAME, "left player lost");
                } else if (lives_ret == 1) {
                    if (LOG_GAME) print_log(LOG_HEAD_GAME, "right player lost");
                }
            } else {
                reset_ball();
            }
        } else {
            if ((side == (char)-1 && game_settings->left == PLAYER) || (side == (char)1 && game_settings->right == PLAYER)) {
                game_running = 0;
                char str[21];
                sprintf(str, "player lost: score %d", score);
                if (LOG_GAME) print_log(LOG_HEAD_GAME, str);
            } else {
                score += BONUS_ON_AI_BALL_LOSS;
                reset_ball();
            }
        }
    }
}

/**
 * Update the player lives.
 * @param looser -1 if the left player lost, \n
 *                1 if the right player lost
 * @return -1 if the left player has lost all of his/her lives, \n
 *          1 if the right player has lost all of his/her lives \n
 *          0 otherwise
 */
char update_lives(char looser) {
    if (looser == (char)-1) {
        if (game_settings->left == PLAYER) {
            data.lives_left--;
            if (data.lives_left <= 0) return -1;
        }
    }
    if (looser == (char)1) {
        if (game_settings->right == PLAYER) {
            data.lives_right--;
            if (data.lives_right <= 0) return 1;
        }
    }
    return 0;
}

/**
 * Reset the ball to the center of the game court and set its velocity randomly.
 */
void reset_ball(void) {
    data.ball_pos_x = (LCD_WIDTH - BALL_SIZE) / 2;
    data.ball_pos_y = LIVES_FONT_SIZE + (LCD_HEIGHT - LIVES_FONT_SIZE - BALL_SIZE) / 2;
    if (rand() % 2 == 0) {
        data.ball_vel_y = -ball_speed;
    } else {
        data.ball_vel_y = ball_speed;
    }
    if (rand() % 2 == 0) {
        data.ball_vel_x = -ball_speed;
    } else {
        data.ball_vel_x = ball_speed;
    }
    static char first = 1;
    if (first) {
        first = 0;
    } else {
        if (LOG_GAME) print_log(LOG_HEAD_GAME, "ball reset");
    }
}

/**
 * Move the dot on the led line each call.
 */
void move_led_line(void) {
    static char dir = 0;
    static uint32_t value = 1;
    if (dir == 0 && value == 0b10000000000000000000000000000000) {
        dir = 1;
    } else if (value == 1) {
        dir = 0;
    }
    if (dir == 0) {
        value *= 2;
    } else {
        value /= 2;
    }
    *(volatile uint32_t*)(memory + SPILED_REG_LED_LINE_o) = value;
}

/**
 * Called upon ball-paddle collision. Start on-hit diode blink, set countdown.
 * @param is_right 0 for left diode, 1 for right diode
 */
void hit_blink(char is_right) {
    if (is_right) {
        if (!ball_loss_blink_right_countdown) {
            light_right_diode(memory, HIT_BLINK_COLOR);
            hit_blink_right_countdown = (double)(HIT_BLINK_DURATION * UPDATES_PER_SECOND) / (double)1000 + 0.5;
        }
    } else {
        if (!ball_loss_blink_left_countdown) {
            light_left_diode(memory, HIT_BLINK_COLOR);
            hit_blink_left_countdown = (double)(HIT_BLINK_DURATION * UPDATES_PER_SECOND) / (double)1000 + 0.5;
        }
    }
}

/**
 * Called upon loss of the ball. Start on-ball-loss diode blink, set countdown.
 * @param is_right 0 for left diode, 1 for right diode
 */
void ball_loss_blink(char is_right) {
    if (is_right) {
        light_right_diode(memory, BALL_LOSS_BLINK_COLOR);
        ball_loss_blink_right_countdown = (double)(BALL_LOSS_BLINK_DURATION * UPDATES_PER_SECOND) / (double)1000 + 0.5;
        hit_blink_right_countdown = 0;
    } else {
        light_left_diode(memory, BALL_LOSS_BLINK_COLOR);
        ball_loss_blink_left_countdown = (double)(BALL_LOSS_BLINK_DURATION * UPDATES_PER_SECOND) / (double)1000 + 0.5;
        hit_blink_left_countdown = 0;
    }
}

/**
 * Called every game update. Counts down and ends the diode blink.
 */
void update_diodes(void) {
    static int ball_loss_blink_period = (double)(BALL_LOSS_BLINK_PERIOD * UPDATES_PER_SECOND) / (double)1000 + 0.5;
    // right diode
    if (ball_loss_blink_right_countdown) {
        ball_loss_blink_right_countdown--;
        if (!ball_loss_blink_right_countdown) {
            light_right_diode(memory, NORMAL_LED_COLOR);
        } else if (ball_loss_blink_right_countdown % ball_loss_blink_period == 0) {
            if (ball_loss_blink_right_countdown / ball_loss_blink_period % 2 == 0) {
                light_right_diode(memory, LED_OFF_COLOR);
            } else {
                light_right_diode(memory, BALL_LOSS_BLINK_COLOR);
            }
        }
    } else if (hit_blink_right_countdown) {
        hit_blink_right_countdown--;
        if (!hit_blink_right_countdown) {
            light_right_diode(memory, NORMAL_LED_COLOR);
        }
    }
    // left didode
    if (ball_loss_blink_left_countdown) {
        ball_loss_blink_left_countdown--;
        if (!ball_loss_blink_left_countdown) {
            light_left_diode(memory, NORMAL_LED_COLOR);
        } else if (ball_loss_blink_left_countdown % ball_loss_blink_period == 0) {
            if (ball_loss_blink_left_countdown / ball_loss_blink_period % 2 == 0) {
                light_left_diode(memory, LED_OFF_COLOR);
            } else {
                light_left_diode(memory, BALL_LOSS_BLINK_COLOR);
            }
        }
    } else if (hit_blink_left_countdown) {
        hit_blink_left_countdown--;
        if (!hit_blink_left_countdown) {
            light_left_diode(memory, NORMAL_LED_COLOR);
        }
    }
}

/**
 * REPLACED WITH POST-GAME SCREEN IMPLEMENTATION IN GRAPHICS.H
 * 
 * The post game screen logic.
 */
void post_game_screen(void) {
    if (score >= 0) {
        view_score_screen(score);
    } else {
        view_victory_screen(data.lives_left ? 0 : 1);
    }
    while (getchar() != ENTER);
}
