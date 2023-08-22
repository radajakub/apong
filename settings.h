/** @file
 * Module that provides tools for saving and editing settings during one run
 * of the application. It also contains all possible values for spearate settings
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdlib.h>
#include <stdint.h>
#include "rgb565.h"
#include "log.h"

#define SETTINGS_HEADER "SETTINGS: "

/* constants for human and ai player */
#define PLAYER 1
#define BOT 0

/* values for three difficulty levels */
#define EASY 0
#define MEDIUM 1
#define HARD 2

/* lables fro three difficulty levels */
#define EASY_LABEL "EASY"
#define MEDIUM_LABEL "NORMAL"
#define HARD_LABEL "HARD"

#define COLOR_COUNT 6
#define DIFFICULTY_COUNT 3
#define AI_COUNT 2

#define DUMB_AI 0
#define SMARTER_AI 1

#define DUMB_AI_LABEL "DUMB"
#define SMARTER_AI_LABEL "SMART"

/**
 * structure that holds current values of settings
 */
typedef struct settings {
    /** index of current difficulty in difficulty array */
    int difficulty;
    /** string that describes current difficulty level */
    char *difficulty_label;
    /** index of currently selected ai for non pvp games  */
    int ai;
    /** label for selected ai */
    char *ai_label;
    /** highscore for currently selected ai */
    int highscore;
    /** currently set color of ball */
    uint16_t ballcolor;
    /** set colors of paddles [left, right] */
    uint16_t paddlecolors[2];
    /** left is player (PLAYER) or ai (BOT) */
    int left;
    /** right is player (PLAYER) or ai (BOT) */
    int right;
} settings_t;

/**
 * structure that holds possible values for fields in settings structure
 */
typedef struct settings_fields {
    /** number of colors to choose from  */
    int color_count;
    /** number of difficulty levels to choose from */
    int difficulty_count;
    /** number of ai to choose from */
    int ai_count;
    /** labels of ai to choose from */
    char **ai_labels;
    /** array of highscores \n for each ai one */
    int *highscores;
    /** array of colors that can be selected for each setting */
    uint16_t *colors;
    /** array of difficulty levels that can be selected */
    char **difficulties;
} settings_fields_t;

/**
 * allocates memory for settings structure and sets default values \n
 * exits app on error
 *
 * @returns pointer to created structure
 */
settings_t *init_settings(void);

/**
 * frees memory allocated for settings structure
 *
 * @param settings: pointer to structure to be destroyed
 */
void destroy_settings(settings_t *settings);

/**
 * allocates memory for settings_fields structure and sets defalut vales \n
 * exits app on error
 *
 * @returns pointer to created settings_fields structure
 */
settings_fields_t *init_settings_fields(void);

/**
 * frees memory allocated for settings_fields structure
 *
 * @param settings pointer to settings_field structure to destroy
 */
void destroy_settings_fields(settings_fields_t *settings);

/**
 * gets index of a color in settings_fields array
 *
 * @param settings_fields structure that holds possible values of colors to choose from
 * @param color color whose index is desired
 *
 * @returns index of passed color in structure \n
 *          0 when no was found
 */
int get_color_index(settings_fields_t *settings_fields, uint16_t color);

/**
 * gets difficulty that is on next index in fields structure
 *
 * @param settings_fields structure that holds possible values of difficulties to choose from
 * @param difficulty currently selected difficulty index
 *
 * @returns next difficulty index \n
 *          first difficulty if passed difficulty was last in array
 */
int get_next_difficulty(settings_fields_t *settings_fields, int difficulty);

/**
 * gets difficulty that is on previous index in fields structure
 *
 * @param settings_fields structure that holds possible values of difficulties to choose from
 * @param difficulty currently selected difficulty index
 *
 * @returns previous difficulty index
 *          last item if passed difficulty was first in array
 */
int get_previous_difficulty(settings_fields_t *settings_fields, int difficulty);

/**
 * gets ai that is on previous index in fields structure
 *
 * @param settings_fields structure that holds possible values of ai to choose from
 * @param ai currently selected ai index
 *
 * @returns previous ai index \n
 *          last item if passed ai was first in array
 */
int get_previous_ai(settings_fields_t *settings_fields, int ai);

/**
 * gets ai that is on next index in fields structure
 *
 * @param settings_fields structure that holds possible values of ai to choose from
 * @param ai currently selected ai index
 *
 * @returns next ai index \n
 *          first ai if passed ai was last in array
 */
int get_next_ai(settings_fields_t *settings_fields, int ai);

/**
 * gets color that is on next index in fields structure
 *
 * @param settings_fields structure that holds possible values of colors to choose from
 * @param color color for which is searched next
 *
 * @returns next color in the array in rgb 565 format \n
 *          first color if passed difficulty was last in array
 */
uint16_t get_next_color(settings_fields_t *settings_fields, uint16_t color);

/**
 * gets color that is on previous index in fields structure
 *
 * @param settings_fields structure that holds possible values of colors to choose from
 * @param color color for which is searched previous
 *
 * @returns previous color in the array in rgb 565 format \n
 *          last color if passed difficulty was first in array
 */
uint16_t get_previous_color(settings_fields_t *settings_fields, uint16_t color);

#endif
