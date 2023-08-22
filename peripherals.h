/* @file
 * Module with funtions to easily handle other peripherals than lcd display
 */

#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "mzapo_regs.h"
#include "mzapo_phys.h"
#include "log.h"

#define PERIPHERALS_HEADER "PERIPHERALS: "

#define EMPTY 0x0u
#define FULL 0xffffffffu

#define ROTATION_MASK 0xffu
#define BUTTON_MASK 0x1u

#define KNOB_COUNT 6

#define RED_K 0
#define GREEN_K 1
#define BLUE_K 2
#define RED_B 3
#define GREEN_B 4
#define BLUE_B 5

#define TRUE 1
#define FALSE 0

/**
 * Structure used for storing led settings.
 */
typedef struct led_settings {
    /** value written in led line */
    uint32_t led_line;
    /** value saved in each diode*/
    uint16_t left_diode, right_diode;
} led_settings_t;

/**
 * structure that holds last two states of knobs to determinem movement
 */
typedef struct knobs {
    /** base memory to peripherals */
    unsigned char *membase;
    /** values in previous check of state */
    uint8_t before[6];
    /** values in most recent check of state */
    uint8_t now[6];
} knobs_t;


/**
 * maps peripherals to memory and checks if mapping was successful
 *
 * @returns pointer to base memory of the peripherals
 */
unsigned char *init_peripherals();

/**
 * specifies blinking of diodes when showing title page
 *
 * @param membase pointer to base memory of the peripherals
 */
void title_blink(unsigned char *membase);

/**
 * specifies blinking of diodes when showing credits page
 *
 * @param membase pointer to base memory of the peripherals
 */
void end_blink(unsigned char *membase);

/**
 * lights passed sequence on led strip
 *
 * @param membase pointer to base memory of the peripherals
 * @param pattern 32bit number whose bits represent on/off (1/0) of the led
 */
void light_leds(unsigned char *membase, uint32_t pattern);

/**
 * lights left diode with color in 24bit format
 *
 * @param membase pointer to base memory of the peripherals
 * @param color number that specifies rgb (only first 24bits are used)
 */
void light_left_diode(unsigned char *membase, uint32_t color);

/**
 * lights right diode with color in 24bit format
 *
 * @param membase pointer to base memory of the peripherals
 * @param color number that specifies rgb (only first 24bits are used)
 */
void light_right_diode(unsigned char *membase, uint32_t color);

/**
 * turn off all peripherals
 *
 * @param membase pointer to base memory of the peripherals
 */
void reset_peripherals(unsigned char *membase);

/**
 * Initializes an instance of led_settings_t with the current setting values.
 *
 * @param membase pointer to base memory of peripherals
 */
led_settings_t* init_led_settings(unsigned char* membase);

/**
 * Restore the settings saved in the given led_settings_t.
 *
 * @param membase pointer to base memory of peripherals
 * @param led_settings pointer to structure that holds led settings
 */
void restore_led_settings(unsigned char* membase, led_settings_t* led_settings);

/**
 * Free the allocated memory for the given led settings.
 *
 * @param led_settings pointer to structure to destroy
 */
void destroy_led_settings(led_settings_t* led_settings);

/**
 * allocates memory for structure that holds state of knobs
 *
 * @param membase pointer to mapped memory base of the knobs
 *
 * @returns pointer to knobs_t structure
 */
knobs_t *init_knobs(unsigned char *membase);

/**
 * frees all allocated memory of knobs_t structure
 *
 * @param knobs structure to destroy
 */
void destroy_knobs(knobs_t *knobs);

/**
 * gets number of ticks that was the konb moved by (applies for buttons too) \n
 * works properly only if it is updated by get_knob_value frequently enough
 * that is not possible for human to make multple movements with the knob \n
 * (for example moving it back and forth in one check cycle will not be recognized)
 *
 * @param knobs: structure that holds information about previous and current states of knobs
 * @param knob: index of knob to check
 *
 * @returns < 0 if counterclockwise movement was detected (button released) \n
 *          0 if no movement was detected \n
 *          > 0 if clockwise movement was detected (button pushed)
 */
int get_knob_movement(knobs_t *knobs, int knob);

/**
 * fills knobs_t structure with current state of rotary knobs on the board
 *
 * @param knobs structure that will save the information
 */
void get_knob_value(knobs_t *knobs);

/**
 * makes both rgb diodes transition from red to blue \n
 * led strip makes cool effects
 *
 * @param membase pointer to base memory of peripherals
 */
void game_transition(unsigned char *membase);

/**
 * gets if any knob button was pushed
 *
 * @param knobs structure that holds knobs last state
 *
 * @returns 0 if no knob was pushed \n
 *          not 0 if any knob was pushed
 */
int knobs_pushed(knobs_t *knobs);

#endif
