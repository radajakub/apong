/** @file
 * Module that provides tools for saving and editing settings during one run
 * of the application. It also contains all possible values for spearate settings
 */

#include "settings.h"

/**
 * allocates memory for settings structure and sets default values \n
 * exits app on error
 *
 * @returns pointer to created structure
 */
settings_t *init_settings(void) {
    settings_t *settings = (settings_t*)malloc(sizeof(settings_t));
    if (settings == NULL) {
        print_log(SETTINGS_HEADER, "error in settings_t allocation");
        exit(1);
    }
    settings->difficulty = MEDIUM;
    settings->difficulty_label = MEDIUM_LABEL;
    settings->ai = DUMB_AI;
    settings->ai_label = DUMB_AI_LABEL;
    settings->highscore = 0;
    settings->ballcolor = WHITE;
    settings->paddlecolors[0] = WHITE;
    settings->paddlecolors[1] = WHITE;
    return settings;
}

/**
 * frees memory allocated for settings structure
 *
 * @param settings: pointer to structure to be destroyed
 */
void destroy_settings(settings_t *settings) {
    free(settings);
}

/**
 * allocates memory for settings_fields structure and sets defalut vales \n
 * exits app on error
 *
 * @returns pointer to created settings_fields structure
 */
settings_fields_t *init_settings_fields(void) {
    settings_fields_t *settings = (settings_fields_t*)malloc(sizeof(settings_t));
    if (settings == NULL) {
        print_log(SETTINGS_HEADER, "error in settings_field allocation");
        exit(1);
    }
    settings->color_count = COLOR_COUNT;
    settings->colors = (uint16_t*)malloc(settings->color_count * sizeof(uint16_t));
    if (settings->colors == NULL) {
        print_log(SETTINGS_HEADER, "error in color allocation");
        exit(1);
    }
    settings->colors[0] = WHITE;
    settings->colors[1] = RED;
    settings->colors[2] = PINK;
    settings->colors[3] = YELLOW;
    settings->colors[4] = GREEN;
    settings->colors[5] = BLUE;
    settings->difficulty_count = DIFFICULTY_COUNT;
    settings->difficulties = (char **)malloc(settings->difficulty_count * sizeof(char *));
    if (settings->difficulties == NULL) {
        print_log(SETTINGS_HEADER, "error in difficulty allocation");
        exit(1);
    }
    settings->difficulties[EASY] = "EASY";
    settings->difficulties[MEDIUM] = "MEDIUM";
    settings->difficulties[HARD] = "HARD";
    settings->ai_count = AI_COUNT;
    settings->ai_labels = (char**)malloc(settings->ai_count * sizeof(char *));
    if (settings->ai_labels == NULL) {
        print_log(SETTINGS_HEADER, "error in ai labels allocation");
        exit(1);
    }
    settings->ai_labels[DUMB_AI] = DUMB_AI_LABEL;
    settings->ai_labels[SMARTER_AI] = SMARTER_AI_LABEL;
    settings->highscores = (int*)malloc(settings->ai_count * sizeof(int));
    if (settings->highscores == NULL) {
        print_log(SETTINGS_HEADER, "error in highscores allocation");
        exit(1);
    }
    for (int i = 0; i < settings->ai_count; i++) {
        settings->highscores[i] = 0;
    }
    return settings;
}

/**
 * frees memory allocated for settings_fields structure
 *
 * @param settings pointer to settings_field structure to destroy
 */
void destroy_settings_fields(settings_fields_t *settings) {
    free(settings->colors);
    free(settings->difficulties);
    free(settings->ai_labels);
    free(settings->highscores);
    free(settings);
}

/**
 * gets index of a color in settings_fields array
 *
 * @param settings_fields structure that holds possible values of colors to choose from
 * @param color color whose index is desired
 *
 * @returns index of passed color in structure \n
 *          0 when no was found
 */
int get_color_index(settings_fields_t *settings_fields, uint16_t color) {
    int index = 0;
    for (int i = 0; i < settings_fields->color_count; i++) {
        if (color == settings_fields->colors[i]) {
            index = i;
            break;
        }
    }
    return index;
}

/**
 * gets difficulty that is on next index in fields structure
 *
 * @param settings_fields structure that holds possible values of difficulties to choose from
 * @param difficulty currently selected difficulty index
 *
 * @returns next difficulty index \n
 *          first difficulty if passed difficulty was last in array
 */
int get_next_difficulty(settings_fields_t *settings_fields, int difficulty) {
    return difficulty == settings_fields->difficulty_count - 1 ? 0 : difficulty + 1;
}

/**
 * gets difficulty that is on previous index in fields structure
 *
 * @param settings_fields structure that holds possible values of difficulties to choose from
 * @param difficulty currently selected difficulty index
 *
 * @returns previous difficulty index
 *          last item if passed difficulty was first in array
 */
int get_previous_difficulty(settings_fields_t *settings_fields, int difficulty) {
    return difficulty == 0 ? settings_fields->difficulty_count - 1 : difficulty - 1;
}

/**
 * gets ai that is on next index in fields structure
 *
 * @param settings_fields structure that holds possible values of ai to choose from
 * @param ai currently selected ai index
 *
 * @returns next ai index \n
 *          first ai if passed ai was last in array
 */
int get_next_ai(settings_fields_t *settings_fields, int ai) {
    return ai == settings_fields->ai_count - 1 ? 0 : ai + 1;
}

/**
 * gets ai that is on previous index in fields structure
 *
 * @param settings_fields structure that holds possible values of ai to choose from
 * @param ai currently selected ai index
 *
 * @returns previous ai index \n
 *          last item if passed ai was first in array
 */
int get_previous_ai(settings_fields_t *settings_fields, int ai) {
    return ai == 0 ? settings_fields->ai_count - 1 : ai - 1;
}

/**
 * gets color that is on next index in fields structure
 *
 * @param settings_fields structure that holds possible values of colors to choose from
 * @param color color for which is searched next
 *
 * @returns next color in the array in rgb 565 format \n
 *          first color if passed difficulty was last in array
 */
uint16_t get_next_color(settings_fields_t *settings_fields, uint16_t color) {
    int index = get_color_index(settings_fields, color);
    return settings_fields->colors[index == settings_fields->color_count - 1 ? 0 : index + 1];
}

/**
 * gets color that is on previous index in fields structure
 *
 * @param settings_fields structure that holds possible values of colors to choose from
 * @param color color for which is searched previous
 *
 * @returns previous color in the array in rgb 565 format \n
 *          last color if passed difficulty was first in array
 */
uint16_t get_previous_color(settings_fields_t *settings_fields, uint16_t color) {
    int index = get_color_index(settings_fields, color);
    return settings_fields->colors[index == 0 ? settings_fields->color_count - 1 : index - 1];
}
