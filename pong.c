/** @mainpage
 * @section Description
 * Technical documentation for game pong implemented on MicroZed APO hardware
 */

/** @file
 */

#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"

#include "graphics.h"
#include "text.h"
#include "settings.h"
#include "rgb565.h"
#include "menu.h"
#include "peripherals.h"
#include "game.h"
#include "player_input.h"

#define MAIN_HEADER "MAIN: "

/**
 * compares score with current highscore and displays appropriate screen
 *
 * @param new_score score that was made in previous game
 * @param settings settings of the last game
 * @param settings_fields pointer to structure that holds all possible values for different settings
 * @param frame buffer to put pixels into
 * @param lcd_membase base memory of lcd display
 * @param bigfont font to print big text
 * @param smallfont font to print smaller text
 */
void eval_score(int new_score, settings_t *settings, settings_fields_t *settings_fields, uint16_t *frame, unsigned char *lcd_membase, font_descriptor_t *bigfont, font_descriptor_t *smallfont) {
    int player_index = settings->left == PLAYER ? 0 : 1;
    if (new_score > settings->highscore) {
        create_highscore_page(new_score, settings->paddlecolors[player_index], frame, lcd_membase, smallfont, bigfont);
        settings->highscore = new_score;
        settings_fields->highscores[settings->ai] = new_score;
    } else {
        create_not_highscore_page(new_score, settings->highscore, settings->paddlecolors[player_index], frame, lcd_membase, smallfont);
    }
}

/**
 * main function
 */
int main(int argc, char *argv[]) {

    unsigned char *lcd_membase = init_lcd();

    unsigned char *membase = init_peripherals();

    uint16_t *frame = init_frame();
    settings_t *settings = init_settings();
    settings_fields_t *settings_fields = init_settings_fields();
    knobs_t *knobs = init_knobs(membase);

    font_descriptor_t *smallfont = &font_wArial_44;
    font_descriptor_t *bigfont = &font_wArial_88;

    init_input();

    parlcd_hx8357_init(lcd_membase);

    clear_frame(frame);
    create_title_page(frame, bigfont);
    show_frame(frame, lcd_membase);
    title_blink(membase);

    int new_score;
    while (main_menu(settings, settings_fields, knobs, frame, bigfont, smallfont, lcd_membase)) {
        /* create delay between start of game for player to prepare */
        create_start_game_page(settings, frame, lcd_membase, bigfont, smallfont);
        show_frame(frame, lcd_membase);
        game_transition(membase);
        print_log(MAIN_HEADER, "new game started");
        if ((new_score = start_game(membase, lcd_membase, knobs, settings)) != -1) {
            eval_score(new_score, settings, settings_fields, frame, lcd_membase, bigfont, smallfont);
            show_and_wait(frame, lcd_membase, knobs);
        }
    }

    print_log(MAIN_HEADER, "application ends");

    clear_frame(frame);
    create_end_page(frame, bigfont, smallfont);
    show_frame(frame, lcd_membase);
    end_blink(membase);

    // turn off desk and clean up
    reset_lcd(lcd_membase);
    reset_peripherals(membase);
    destroy_knobs(knobs);
    destroy_settings(settings);
    destroy_settings_fields(settings_fields);
    destroy_frame(frame);
    exit_input();
    return 0;
}
