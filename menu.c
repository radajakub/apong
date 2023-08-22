/** @file
 * Module with functions to create, modify and control menu in this application \n
 * It modifies settings structure according to selections in the menu
 */

#include "menu.h"

/**
 * puts new item of same format into frame buffer
 *
 * @param y vrtical offset of top-left corner of the new menu
 * @param label string (pointer to first char) that is written in the menu item \n
 *        if it is too long it is cut off even in the middle of a character
 * @param frame frame buffer for the lcd display
 * @param font pointer to structure that holds desired font for the menu item
 * @param color color of the menu item (border and text) in rgb 565 format
 */
void put_menu_element(int y, char *label, uint16_t *frame, font_descriptor_t *font, uint16_t color) {
    /* create border around the item's contents */
    int inside_height = 2 * PADDING + MENU_FONT_SIZE;
    /* horizontal lines */
    for (int i = 0; i < PADDING; i++) {
        for (int j = 0; j < LCD_WIDTH; j++) {
            frame[(y + i) * LCD_WIDTH + j] = color;
            frame[(y + inside_height + i) * LCD_WIDTH + j] = color;
        }
    }
    /* vertical lines */
    for (int i = 0; i < inside_height; i++) {
        for (int j = 0; j < PADDING; j++) {
            frame[(y + i) * LCD_WIDTH + j] = color;
            frame[(y + i + 1) * LCD_WIDTH - j - 1] = color;
        }
    }
    for (int i = y + PADDING; i < y + inside_height; i++) {
        for (int j = PADDING; j < LCD_WIDTH - PADDING; j++) {
            frame[i * LCD_WIDTH + j] = MENU_BACKGROUND;
        }
    }
    put_string(4 * PADDING, y + 2 * PADDING, frame, font, label, color, MENU_BACKGROUND);
}

/**
 * fills whole screen with number of items (other will not fit) preset in ITEMS_ON_PAGE \n
 * it considers which item is currently selected and tries to place it in the middle of the screen (except first one) \n
 * it is not meant for settings menu
 *
 * @param y_offsets precomputed array of top-left corners of all items (3) on screen
 * @param labels list of strings (double char pointer) that are to be placed into menu items
 * @param count total number of items in the menu (not the number of visible ones)
 * @param selected index of the item that is currently selected
 * @param font pointer to font structure used for writing text
 * @param frame frame buffer that holds the menu items
 */
void fill_menu(int *y_offsets, char **labels, int count, int selected, font_descriptor_t *font, uint16_t *frame) {
    clear_frame(frame);
    int offset_index = 0;
    int i = selected == 0 ? 0 : selected - 1;
    /* compute index of the last item that will be shown */
    int max = i + ITEMS_ON_PAGE - 1 >= count ? count - 1 : i + ITEMS_ON_PAGE - 1;
    for (; i <= max; i++) {
        put_menu_element(y_offsets[offset_index++], labels[i], frame, font, i == selected ? SELECTED : UNSELECTED);
    }
}

/**
 * fills whole scrren with number of items preset in ITEMS_ON_PAGE \n
 * items contain different or none interactive aspects
 *
 * @param y_offsets precomputed array of top-left corners of all items (3) on screen
 * @param labels list of strings (double char pointer) that are to be placed into menu items
 * @param ai_label label of ai for which is displayed highscore
 * @param highscore number that represents highscore of selected ai (same as ai_label)
 * @param count total number of items in the menu (not the number of visible ones)
 * @param selected index of the item that is currently selected
 * @param font pointer to font structure used for writing text
 * @param frame frame buffer that holds the menu items
 */
void fill_highscore_menu(int *y_offsets, char **labels, char *ai_label, int highscore, int count, int selected, font_descriptor_t *font, uint16_t *frame) {
    clear_frame(frame);
    int offset_index = 0;
    int i = selected == 0 ? 0 : selected - 1;
    /* compute index of the last item that will be shown */
    int max = i + ITEMS_ON_PAGE - 1 >= count ? count - 1 : i + ITEMS_ON_PAGE - 1;
    for (; i <= max; i++) {
        put_menu_element(y_offsets[offset_index], labels[i], frame, font, i == selected ? SELECTED : UNSELECTED);
        if (i == HIGHSCORE_AI) {
            put_label_settings(y_offsets[offset_index], ai_label, font, frame, i == selected ? SELECTED : UNSELECTED, CENTER);
        } else if (i == HIGHSCORE_NUMBER) {
                char string[11];
            if (sprintf(string, "%d", highscore) < 0) {
                print_log(HIGHSCORE_MENU_HEADER, "highscore menu sprintf error");
                exit(1);
            }
            put_string((LCD_WIDTH - get_string_width(font, string)) / 2, y_offsets[offset_index] + 2 * PADDING, frame, font, string, i == selected ? SELECTED : UNSELECTED, MENU_BACKGROUND);
        }
        offset_index++;
    }
}

/**
 * fills whole screen with number of items preset in ITEMS_ON_PAGE \n
 * items contain different interactive aspect (colro square or different kind of label)
 *
 * @param y_offsets precomputed array of top-left corners of all items (3) on screen
 * @param labels list of strings (double char pointer) that are to be placed into menu items
 * @param count total number of items in the menu (not the number of visible ones)
 * @param selected index of the item that is currently selected
 * @param bigfont pointer to font structure used for writing big text
 * @param smallfont pointer to font structure used for writing small text
 * @param frame frame buffer that holds the menu items
 * @param settings pointer to structure that hold current settings and is both read and modified
 */
void fill_settings_menu(int *y_offsets, char **labels, int count, int selected, font_descriptor_t *bigfont, font_descriptor_t *smallfont, uint16_t *frame, settings_t *settings) {
    clear_frame(frame);
    int offset_index = 0;
    int i = selected == 0 ? 0 : selected - 1;
    /* compute index of the last item that will be shown */
    int max = i + ITEMS_ON_PAGE - 1 >= count ? count - 1 : i + ITEMS_ON_PAGE - 1;
    for (; i <= max; i++) {
        put_menu_element(y_offsets[offset_index], labels[i], frame, bigfont, i == selected ? SELECTED : UNSELECTED);
        /* check special features of menu item */
        if (i == BALL_COLOR) {
            put_color_settings(y_offsets[offset_index], settings->ballcolor, bigfont, frame);
        } else if (i == LEFT_COLOR) {
            put_color_settings(y_offsets[offset_index], settings->paddlecolors[0], bigfont, frame);
        } else if (i == RIGHT_COLOR) {
            put_color_settings(y_offsets[offset_index], settings->paddlecolors[1], bigfont, frame);
        } else if (i == DIFFICULTY) {
            put_label_settings(y_offsets[offset_index], settings->difficulty_label, bigfont, frame, i == selected ? SELECTED : UNSELECTED, CENTER);
        } else if (i == SETTINGS_AI) {
            put_label_settings(y_offsets[offset_index] + (MENU_FONT_SIZE - MENU_SMALLFONT_SIZE) / 2, settings->ai_label, smallfont, frame, i == selected ? SELECTED : UNSELECTED, NOT_CENTER);
        }
        offset_index++;
    }
}

/**
 * puts currently selected label setting item into itme \n
 * the menu item on y_offset has to have blank label, otherwise it will be rewritten
 *
 * @param y_offset offset of the top-left corner of the menu item
 * @param label label for currently selected settings of the game
 * @param font pointer to font structure used for writing text
 * @param frame frame buffer that holds the menu items
 * @param color color of the menu item in the rgb 565 format
 * @param position one of position macros that define if label is in the center of the menu element or aligned right
 */
void put_label_settings(int y_offset, char *label, font_descriptor_t *font, uint16_t *frame, uint16_t color, int position) {
    y_offset += 2 * PADDING;
    int arrow_width = get_char_width(font, '>');
    /* compute lengt of string in pixels*/
    int width = get_string_width(font, label);
    /* put string with arrows in the middle of the item */
    int x_offset;
    if (position == CENTER) {
        x_offset = (LCD_WIDTH - width) / 2;
    } else {
        x_offset = LCD_WIDTH - 10 * PADDING - width - arrow_width;
    }
    put_char(x_offset - 4 * PADDING - arrow_width, y_offset, frame, font, '<', GREY, MENU_BACKGROUND);
    put_string(x_offset, y_offset, frame, font, label, color, MENU_BACKGROUND);
    put_char(x_offset + width + 4 * PADDING, y_offset, frame, font, '>', GREY, MENU_BACKGROUND);
}

/**
 * puts currently selected color setting into menu item
 *
 * @param y_offset offset of the top-left corner of the menu item
 * @param color currently selected color for settings whose menu item is currently placed
 * @param font pointer to font structure used for writing text
 * @param frame frame buffer that holds the menu items
 */
void put_color_settings(int y_offset, uint16_t color, font_descriptor_t *font, uint16_t *frame) {
    y_offset += 2 * PADDING;
    int width = get_char_width(font, '>');
    int size = MENU_FONT_SIZE - 8 * PADDING;
    int x_offset = LCD_WIDTH - 6 * PADDING - width;
    put_char(x_offset, y_offset, frame, font, '>', GREY, MENU_BACKGROUND);
    x_offset -= (size + 4 * PADDING);
    y_offset += 4 * PADDING;
    /* place square of passed color */
    for (int y = y_offset; y < y_offset + size; y++) {
        for (int x = x_offset; x < x_offset + size; x++) {
            frame[y * LCD_WIDTH + x] = color;
        }
    }
    y_offset -= 4 * PADDING;
    x_offset -= (4 * PADDING + width);
    put_char(x_offset, y_offset, frame, font, '<', GREY, MENU_BACKGROUND);
}

/**
 * checks knobs for input and convert it to menu suitable format
 *
 * @param input pointer to char which decides about conducted action
 * @param knobs_use pointer to variable which determines that knobs recorded input
 * @param knobs pointer to structure that holds information about state of knobs
 */
void check_knobs(char *input, int *knobs_use, knobs_t *knobs) {
    get_knob_value(knobs);
    if (get_knob_movement(knobs, RED_K) > 1) {
        *input = DOWN;
        *knobs_use = 1;
    } else if (get_knob_movement(knobs, RED_K) < -1) {
        *input = UP;
        *knobs_use = 1;
    } else if (get_knob_movement(knobs, GREEN_K) > 1) {
        *input = RIGHT;
        *knobs_use = 1;
    } else if (get_knob_movement(knobs, GREEN_K) < -1) {
        *input = LEFT;
        *knobs_use = 1;
    } else if (get_knob_movement(knobs, RED_B) > 0 || get_knob_movement(knobs, GREEN_B) > 0) {
        *input = ACTION;
        *knobs_use = 1;
    } else if (get_knob_movement(knobs, BLUE_B) > 0) {
        *input = BACK;
        *knobs_use = 1;
    }
}

/**
 * creates and handles input of main menu \n
 * it interprets user input and allows scrolling and starting submenu
 *
 * @param settings pointer to structure that holds settings and will be modified
 * @param settings_fields pointer to structure that holds possible settings values
 * @param knobs pointer to structure that holds state of knobs from last check
 * @param frame frame buffer that holds the menu items
 * @param bigfont pointer to font structure used for writing big text
 * @param smallfont pointer to font structure used for writing small text
 * @param lcd_membase pointer to base memory of the lcd display
 *
 * @returns START on selecting to play a game \n
 *          STOP on selecting to exit the app
 */
int main_menu(settings_t *settings, settings_fields_t *settings_fields, knobs_t *knobs, uint16_t *frame, font_descriptor_t *bigfont, font_descriptor_t *smallfont, unsigned char *lcd_membase) {
    print_log(MAIN_MENU_HEADER, "entered main menu");
    char *log = (char *)malloc(50 * sizeof(char));
    int item_size = 4 * PADDING + MENU_FONT_SIZE;
    int selected = 0;
    /* compute offsets for menu items (considering the dimensions of the items) */
    int y_offsets[ITEMS_ON_PAGE];
    y_offsets[0] = 0;
    for (int i = 1; i < ITEMS_ON_PAGE; i++) {
        y_offsets[i] = y_offsets[i - 1] + item_size + SPACING;
    }
    char *labels[MAIN_MENU_ITEMS] = {"PLAY", "SCORES", "SETTINGS", "QUIT"};
    /* initial fill menu with first item selected*/
    fill_menu(y_offsets, labels, MAIN_MENU_ITEMS, selected, bigfont, frame);
    show_frame(frame, lcd_membase);
    if (sprintf(log, "%s is selected", labels[selected])) print_log(MAIN_MENU_HEADER, log);
    char input;
    int proceed = 1;
    int ret = STOP;
    int knobs_use = 0;
    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 1000 * 1000 * 10};
    while (proceed) {
        /* wait for user input and then process it */
        check_knobs(&input, &knobs_use, knobs);
        if (knobs_use || read(STDIN_FILENO, &input, 1) == 1) {
            switch (input) {
                case DOWN:
                    /* scroll down when registering DOWN character on stdin */
                    selected = selected < MAIN_MENU_ITEMS - 1 ? selected + 1 : selected;
                    if (sprintf(log, "%s is selected", labels[selected])) print_log(MAIN_MENU_HEADER, log);
                    fill_menu(y_offsets, labels, MAIN_MENU_ITEMS, selected, bigfont, frame);
                    break;
                case UP:
                    /* scroll down when registering DOWN character on stdin */
                    selected = selected > 0 ? selected - 1 : selected;
                    if (sprintf(log, "%s is selected", labels[selected])) print_log(MAIN_MENU_HEADER, log);
                    fill_menu(y_offsets, labels, MAIN_MENU_ITEMS, selected, bigfont, frame);
                    break;
                case ACTION:
                    /* on pressing of ACTION button take action based on selected item */
                    switch (selected) {
                        case PLAY:
                            /* show play menu, wait for start game or exit */
                            proceed = play_menu(settings, knobs, frame, y_offsets, bigfont, lcd_membase);
                            fill_menu(y_offsets, labels, MAIN_MENU_ITEMS, selected, bigfont, frame);
                            /* chagne return value if game was started or was pressed back item */
                            ret = proceed == GAME ? START : STOP;
                            break;
                        case HIGHSCORES:
                            highscores_menu(settings_fields, knobs, frame, y_offsets, bigfont, lcd_membase);
                            fill_menu(y_offsets, labels, MAIN_MENU_ITEMS, selected, bigfont, frame);
                            break;
                        case SETTINGS:
                            /* show setings menu */
                            settings_menu(settings, settings_fields, knobs, frame, y_offsets, bigfont, smallfont, lcd_membase);
                            fill_menu(y_offsets, labels, MAIN_MENU_ITEMS, selected, bigfont, frame);
                            break;
                        case QUIT:
                            print_log(MAIN_MENU_HEADER, "exited main menu");
                            proceed = 0;
                            ret = STOP;
                            break;
                    }
                    break;
                case BACK:
                    /* when registered BACK character exit the menu as well */
                    print_log(MAIN_MENU_HEADER, "exited main menu");
                    proceed = 0;
                    break;
            }
            show_frame(frame, lcd_membase);
            knobs_use = 0;
        }
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL); /* delay while loop */
    }
    free(log);
    return ret;
}

/**
 * creates and handles input of play menu \n
 * it interprets user input and allows scrolling and selecting play mode \n
 * selected play mode is saved into settngs structure
 *
 * @param settings pointer to structure that holds settings and will be modified
 * @param knobs pointer to structure that holds state of knobs from last check
 * @param frame frame buffer that holds the menu items
 * @param y_offsets precomputed offsets of menu items
 * @param font pointer to font structure used for writing text
 * @param lcd_membase pointer to base memory of the lcd display
 *
 * @returns GAME when any game mode is selected \n
 *          MENU when back item is selected
 */
int play_menu(settings_t *settings, knobs_t *knobs, uint16_t *frame, int *y_offsets, font_descriptor_t *font, unsigned char *lcd_membase) {
    print_log(PLAY_MENU_HEADER, "entered play menu");
    char *log = (char *)malloc(50 * sizeof(char));
    clear_frame(frame);
    int selected = 0;
    char *labels[PLAY_MENU_ITEMS] = {"P vs P", "P vs A", "A vs P", "BACK"};
    /* initial show of play menu with first item selected */
    fill_menu(y_offsets, labels, PLAY_MENU_ITEMS, selected, font, frame);
    show_frame(frame, lcd_membase);
    if (sprintf(log, "%s is selected", labels[selected])) print_log(PLAY_MENU_HEADER, log);
    char input;
    int proceed = 1;
    int ret = MENU;
    int knobs_use = 0;
    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 1000 * 1000 * 10};
    while (proceed) {
        /* wait for user input and then process it */
        check_knobs(&input, &knobs_use, knobs);
        if (knobs_use || read(STDIN_FILENO, &input, 1) == 1) {
            switch(input) {
                case DOWN:
                    /* scroll down one item on DOWN char */
                    selected = selected < PLAY_MENU_ITEMS - 1 ? selected + 1 : selected;
                    if (sprintf(log, "%s is selected", labels[selected])) print_log(PLAY_MENU_HEADER, log);
                    fill_menu(y_offsets, labels, PLAY_MENU_ITEMS, selected, font, frame);
                    break;
                case UP:
                    /* scroll up one item on UP char*/
                    selected = selected > 0 ? selected - 1 : selected;
                    if (sprintf(log, "%s is selected", labels[selected])) print_log(PLAY_MENU_HEADER, log);
                    fill_menu(y_offsets, labels, PLAY_MENU_ITEMS, selected, font, frame);
                    break;
                case BACK:
                    /* exiting settings menu on BACK char in stdin */
                    proceed = 0;
                    break;
                case ACTION:
                    /* save selected mode on ACTION char and exit menu with appropriate value */
                    switch(selected) {
                        case PLAYER_PLAYER:
                            settings->left = IS_PLAYER;
                            settings->right = IS_PLAYER;
                            print_log(PLAY_MENU_HEADER, "player-player mode");
                            ret = GAME;
                            break;
                        case AI_PLAYER:
                            settings->left = IS_AI;
                            settings->right = IS_PLAYER;
                            print_log(PLAY_MENU_HEADER, "ai-player mode");
                            ret = GAME;
                            break;
                        case PLAYER_AI:
                            settings->left = IS_PLAYER;
                            settings->right = IS_AI;
                            print_log(PLAY_MENU_HEADER, "playre-ai mode");
                            ret = GAME;
                            break;
                        case PLAY_BACK:
                            ret = MENU;
                            break;
                    }
                    proceed = 0;
                    break;
            }
            show_frame(frame, lcd_membase);
            knobs_use = 0;
        }
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
    free(log);
    print_log(PLAY_MENU_HEADER, "exited play menu");
    return ret;
}

/**
 * creates and handles input of settings menu \n
 * it interprets user input and allows scrolling and change of settings by
 *      LEFT and RIGHT chars on stdin
 *
 * @param settings pointer to structure that holds settings and will be modified
 * @param settings_fields pointer to structure that holds possible settings values
 * @param knobs pointer to structure that holds state of knobs from last check
 * @param frame frame buffer that holds the menu items
 * @param y_offsets precomputed offsets of menu items
 * @param bigfont pointer to font structure used for writing big text
 * @param smallfont pointer to font structure used for writing small text
 * @param lcd_membase pointer to base memory of the lcd display
 */
void settings_menu(settings_t *settings, settings_fields_t *settings_fields, knobs_t *knobs, uint16_t *frame, int *y_offsets, font_descriptor_t *bigfont, font_descriptor_t *smallfont, unsigned char *lcd_membase) {
    print_log(SETTINGS_MENU_HEADER, "entered settings menu");
    char *log = (char *)malloc(100 * sizeof(char));
    clear_frame(frame);
    int selected = 0;
    char *labels[SETTINGS_MENU_ITEMS] = {"", "AI", "BALL", "LEFT", "RIGHT", "BACK"};
    /* intital showing menu with first item selected */
    fill_settings_menu(y_offsets, labels, SETTINGS_MENU_ITEMS, selected, bigfont, smallfont, frame, settings);
    show_frame(frame, lcd_membase);
    if (sprintf(log, "%s is selected", !(*labels[selected]) ? "difficulty" : labels[selected])) print_log(SETTINGS_MENU_HEADER, log);
    char input;
    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 1000 * 1000 * 10};
    int proceed = 1;
    int knobs_use = 0;
    while(proceed) {
        /* wait for user input and process it */
        check_knobs(&input, &knobs_use, knobs);
        if (knobs_use || read(STDIN_FILENO, &input, 1) == 1) {
            switch(input) {
                case DOWN:
                    /* scroll down one item on DOWN char */
                    selected = selected < SETTINGS_MENU_ITEMS - 1 ? selected + 1 : selected;
                    if (sprintf(log, "%s is selected", !(*labels[selected]) ? "difficulty" : labels[selected])) print_log(SETTINGS_MENU_HEADER, log);
                    fill_settings_menu(y_offsets, labels, SETTINGS_MENU_ITEMS, selected, bigfont, smallfont, frame, settings);
                    break;
                case UP:
                    /* scroll up one item on UP char*/
                    selected = selected > 0 ? selected - 1 : selected;
                    if (sprintf(log, "%s is selected", !(*labels[selected]) ? "difficulty" : labels[selected])) print_log(SETTINGS_MENU_HEADER, log);
                    fill_settings_menu(y_offsets, labels, SETTINGS_MENU_ITEMS, selected, bigfont, smallfont, frame, settings);
                    break;
                case LEFT:
                    /* change selected setting to next value (depeding on selected setting -> looking up in settings_field values */
                    if (selected == BALL_COLOR) {
                        settings->ballcolor = get_previous_color(settings_fields, settings->ballcolor);
                        if (sprintf(log, "ball color changed to 0x%x", settings->ballcolor)) print_log(SETTINGS_MENU_HEADER, log);
                    } else if (selected == LEFT_COLOR) {
                        settings->paddlecolors[0] = get_previous_color(settings_fields, settings->paddlecolors[0]);
                        if (sprintf(log, "left paddle color changed to 0x%x", settings->paddlecolors[0])) print_log(SETTINGS_MENU_HEADER, log);
                    } else if (selected == RIGHT_COLOR) {
                        settings->paddlecolors[1] = get_previous_color(settings_fields, settings->paddlecolors[1]);
                        if (sprintf(log, "right paddle color changed to 0x%x", settings->paddlecolors[1])) print_log(SETTINGS_MENU_HEADER, log);
                    } else if (selected == DIFFICULTY) {
                        settings->difficulty = get_previous_difficulty(settings_fields, settings->difficulty);
                        settings->difficulty_label = settings_fields->difficulties[settings->difficulty];
                        if (sprintf(log, "difficulty changed to %s", settings->difficulty_label)) print_log(SETTINGS_MENU_HEADER, log);
                    } else if (selected == SETTINGS_AI) {
                        settings->ai = get_previous_ai(settings_fields, settings->ai);
                        settings->ai_label = settings_fields->ai_labels[settings->ai];
                        settings->highscore = settings_fields->highscores[settings->ai];
                        if (sprintf(log, "ai changed to %s", settings->ai_label)) print_log(SETTINGS_MENU_HEADER, log);
                    }
                    fill_settings_menu(y_offsets, labels, SETTINGS_MENU_ITEMS, selected, bigfont, smallfont, frame, settings);
                    break;
                case RIGHT:
                    /* change selected setting to previous value (depeding on selected setting -> looking up in settings_field values */
                    if (selected == BALL_COLOR) {
                        settings->ballcolor = get_next_color(settings_fields, settings->ballcolor);
                        if (sprintf(log, "ball color changed to 0x%x", settings->ballcolor)) print_log(SETTINGS_MENU_HEADER, log);
                    } else if (selected == LEFT_COLOR) {
                        settings->paddlecolors[0] = get_next_color(settings_fields, settings->paddlecolors[0]);
                        if (sprintf(log, "left paddle color changed to 0x%x", settings->paddlecolors[0])) print_log(SETTINGS_MENU_HEADER, log);
                    } else if (selected == RIGHT_COLOR) {
                        settings->paddlecolors[1] = get_previous_color(settings_fields, settings->paddlecolors[1]);
                        if (sprintf(log, "right paddle color changed to 0x%x", settings->paddlecolors[1])) print_log(SETTINGS_MENU_HEADER, log);
                    } else if (selected == DIFFICULTY) {
                        settings->difficulty = get_next_difficulty(settings_fields, settings->difficulty);
                        settings->difficulty_label = settings_fields->difficulties[settings->difficulty];
                        if (sprintf(log, "difficulty changed to %s", settings->difficulty_label)) print_log(SETTINGS_MENU_HEADER, log);
                    } else if (selected == SETTINGS_AI) {
                        settings->ai = get_next_ai(settings_fields, settings->ai);
                        settings->ai_label = settings_fields->ai_labels[settings->ai];
                        settings->highscore = settings_fields->highscores[settings->ai];
                        if (sprintf(log, "ai changed to %s", settings->ai_label)) print_log(SETTINGS_MENU_HEADER, log);
                    }
                    fill_settings_menu(y_offsets, labels, SETTINGS_MENU_ITEMS, selected, bigfont, smallfont, frame, settings);
                    break;
                case BACK:
                    /* exiting settings menu on BACK char in stdin */
                    proceed = 0;
                    break;
                case ACTION:
                    /* if ACTION char is registered when BACK item is selected*/
                    if (selected == SETTINGS_BACK) {
                        proceed = 0;
                    }
                    break;
            }
            show_frame(frame, lcd_membase);
            knobs_use = 0;
        }
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
    print_log(SETTINGS_MENU_HEADER, "settings menu exited");
    free(log);
}

/**
 * creates and handles input of highscores menu \n
 * it interprets user input and allows scrolling and change of settings by
 *      LEFT and RIGHT chars on stdin
 *
 * @param settings_fields pointer to structure that holds possible settings values
 * @param knobs pointer to structure that holds state of knobs from last check
 * @param frame frame buffer that holds the menu items
 * @param y_offsets precomputed offsets of menu items
 * @param font pointer to font structure used for writing text
 * @param lcd_membase pointer to base memory of the lcd display
 */
void highscores_menu(settings_fields_t *settings_fields, knobs_t *knobs, uint16_t *frame, int *y_offsets, font_descriptor_t *font, unsigned char *lcd_membase) {
    print_log(HIGHSCORE_MENU_HEADER, "entered highscore menu");
    int index = 0;
    int selected = 0;
    char *labels[HIGHSCORE_MENU_ITEMS] = {"", "", "BACK"};
    fill_highscore_menu(y_offsets, labels, settings_fields->ai_labels[index], settings_fields->highscores[index], HIGHSCORE_MENU_ITEMS, selected, font, frame);
    show_frame(frame, lcd_membase);
    char input;
    int knobs_use = 0;
    int proceed = 1;
    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 1000 * 1000 * 10};
    while (proceed) {
        /* wait for user input and then process it */
        check_knobs(&input, &knobs_use, knobs);
        if (knobs_use || read(STDIN_FILENO, &input, 1) == 1) {
            switch(input) {
                case DOWN:
                    /* scroll down one item on DOWN char */
                    selected = selected < HIGHSCORE_MENU_ITEMS - 1 ? selected + 1 : selected;
                    fill_highscore_menu(y_offsets, labels, settings_fields->ai_labels[index], settings_fields->highscores[index], HIGHSCORE_MENU_ITEMS, selected, font, frame);
                    break;
                case UP:
                    /* scroll up one item on UP char*/
                    selected = selected > 0 ? selected - 1 : selected;
                    fill_highscore_menu(y_offsets, labels, settings_fields->ai_labels[index], settings_fields->highscores[index], HIGHSCORE_MENU_ITEMS, selected, font, frame);
                    break;
                case RIGHT:
                    if (selected == HIGHSCORE_AI) {
                        index = get_next_ai(settings_fields, index);
                        fill_highscore_menu(y_offsets, labels, settings_fields->ai_labels[index], settings_fields->highscores[index], HIGHSCORE_MENU_ITEMS, selected, font, frame);
                    }
                    break;
                case LEFT:
                    if (selected == HIGHSCORE_AI) {
                        index = get_previous_ai(settings_fields, index);
                        fill_highscore_menu(y_offsets, labels, settings_fields->ai_labels[index], settings_fields->highscores[index], HIGHSCORE_MENU_ITEMS, selected, font, frame);
                    }
                    break;
                case ACTION:
                    if (selected == HIGHSCORE_BACK) {
                        proceed = 0;
                    }
                    break;
                case BACK:
                    /* exiting settings menu on BACK char in stdin */
                    proceed = 0;
                    break;
            }
            show_frame(frame, lcd_membase);
            knobs_use = 0;
        }
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
    print_log(HIGHSCORE_MENU_HEADER, "highscore menu exited");
}
