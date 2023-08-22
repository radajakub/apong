/* @file
 * Module with funtions to easily handle other peripherals than lcd display
 */

#include "peripherals.h"

/**
 * maps peripherals to memory and checks if mapping was successful
 *
 * @returns pointer to base memory of the peripherals
 */
unsigned char *init_peripherals() {
    unsigned char *membase = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if (membase == NULL) {
        print_log(PERIPHERALS_HEADER, "mapping error");
        exit(1);
    }
    return membase;
}

/**
 * specifies blinking of diodes when showing title page
 *
 * @param membase pointer to base memory of the peripherals
 */
void title_blink(unsigned char *membase) {
    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000};
    uint32_t left = 0u;
    uint32_t right = 0u;
    uint32_t light = 0x00000000u;
    for (int i = 0; i < 16; i++) {
        right = (right << 1) | 0x1u;
        left = (left >> 1) | 0x80000000u | right;
        light = light >> 2;
        light |= 0xff000000u;
        light_leds(membase, left);
        light_left_diode(membase, light);
        light_right_diode(membase, light);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
    //light_leds(membase, EMPTY);
    light_left_diode(membase, EMPTY);
    light_right_diode(membase, EMPTY);
}

/**
 * specifies blinking of diodes when showing credits page
 *
 * @param membase pointer to base memory of the peripherals
 */
void end_blink(unsigned char *membase) {
    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000};
    uint32_t left = 0xffff0000u;
    uint32_t right = 0xffffu;
    uint32_t light = 0xffffffffu;
    for (int i = 0; i < 16; i++) {
        light_leds(membase, left | right);
        light_left_diode(membase, light);
        light_right_diode(membase, light);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
        left = left << 1;
        right = right >> 1;
        light = light << 2;
    }
    light_leds(membase, EMPTY);
    light_left_diode(membase, EMPTY);
    light_right_diode(membase, EMPTY);
}

/**
 * lights passed sequence on led strip
 *
 * @param membase pointer to base memory of the peripherals
 * @param pattern 32bit number whose bits represent on/off (1/0) of the led
 */
void light_leds(unsigned char *membase, uint32_t pattern) {
    *(volatile uint32_t*)(membase + SPILED_REG_LED_LINE_o) = pattern;
}

/**
 * lights left diode with color in 24bit format
 *
 * @param membase pointer to base memory of the peripherals
 * @param color number that specifies rgb (only first 24bits are used)
 */
void light_left_diode(unsigned char *membase, uint32_t color) {
    *(volatile uint32_t*)(membase + SPILED_REG_LED_RGB1_o) = color;
}

/**
 * lights right diode with color in 24bit format
 *
 * @param membase pointer to base memory of the peripherals
 * @param color number that specifies rgb (only first 24bits are used)
 */
void light_right_diode(unsigned char *membase, uint32_t color) {
    *(volatile uint32_t*)(membase + SPILED_REG_LED_RGB2_o) = color;
}

/**
 * turn off all peripherals
 *
 * @param membase pointer to base memory of the peripherals
 */
void reset_peripherals(unsigned char *membase) {
    *(volatile uint32_t*)(membase + SPILED_REG_LED_LINE_o) = 0;
    *(volatile uint32_t*)(membase + SPILED_REG_LED_RGB1_o) = 0;
    *(volatile uint32_t*)(membase + SPILED_REG_LED_RGB2_o) = 0;
}

/**
 * Initializes an instance of led_settings_t with the current setting values.
 *
 * @param membase pointer to base memory of peripherals
 */
led_settings_t* init_led_settings(unsigned char* membase) {
    led_settings_t* led_settings = (led_settings_t*)malloc(sizeof(led_settings_t));
    if (led_settings == NULL) {
        print_log(PERIPHERALS_HEADER, "ERROR while allocating memory for led_settings_t");
        exit(1);
    }
    led_settings->led_line = *(volatile uint32_t*)(membase + SPILED_REG_LED_LINE_o);
    led_settings->left_diode = *(volatile uint32_t*)(membase + SPILED_REG_LED_RGB1_o);
    led_settings->right_diode = *(volatile uint32_t*)(membase + SPILED_REG_LED_RGB2_o);
    return led_settings;
}

/**
 * Restore the settings saved in the given led_settings_t.
 *
 * @param membase pointer to base memory of peripherals
 * @param led_settings pointer to structure that holds led settings
 */
void restore_led_settings(unsigned char* membase, led_settings_t* led_settings) {
    light_leds(membase, led_settings->led_line);
    light_left_diode(membase, led_settings->left_diode);
    light_right_diode(membase, led_settings->right_diode);
}

/**
 * Free the allocated memory for the given led settings.
 *
 * @param led_settings pointer to structure to destroy
 */
void destroy_led_settings(led_settings_t* led_settings) {
    free(led_settings);
}

/**
 * allocates memory for structure that holds state of knobs
 *
 * @param membase pointer to mapped memory base of the knobs
 *
 * @returns pointer to knobs_t structure
 */
knobs_t *init_knobs(unsigned char *membase) {
    knobs_t *knobs = (knobs_t*)malloc(sizeof(knobs_t));
    if (knobs == NULL) {
        print_log(PERIPHERALS_HEADER, "error in knobs allocation");
        exit(1);
    }
    knobs->membase = membase;
    for (int i = 0; i < KNOB_COUNT; i++) {
        knobs->before[i] = -1;
        knobs->now[i] = -1;
    }
    return knobs;
}

/**
 * frees all allocated memory of knobs_t structure
 *
 * @param knobs structure to destroy
 */
void destroy_knobs(knobs_t *knobs) {
    free(knobs);
}

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
int get_knob_movement(knobs_t *knobs, int knob) {
    if (knobs->before[knob] == (uint8_t)-1) return 0;
    int ret = knobs->now[knob] - knobs->before[knob];
    if (ret < -128) {
        ret = 256 + ret;
    } else if (ret > 128) {
        ret = 256 - ret;
    }
    return ret;
}

/**
 * fills knobs_t structure with current state of rotary knobs on the board
 *
 * @param knobs structure that will save the information
 */
void get_knob_value(knobs_t *knobs) {
    for (int i = 0; i < KNOB_COUNT; i++) {
        knobs->before[i] = knobs->now[i];
    }
    uint32_t values = *(volatile uint32_t*)(knobs->membase + SPILED_REG_KNOBS_8BIT_o);
    knobs->now[BLUE_K] = values & ROTATION_MASK;
    values = values >> 8;
    knobs->now[GREEN_K] = values & ROTATION_MASK;
    values = values >> 8;
    knobs->now[RED_K] = values & ROTATION_MASK;
    values = values >> 8;
    knobs->now[BLUE_B] = values & BUTTON_MASK;
    values = values >> 1;
    knobs->now[GREEN_B] = values & BUTTON_MASK;
    values = values >> 1;
    knobs->now[RED_B] = values & BUTTON_MASK;
}

/**
 * makes both rgb diodes transition from red to blue \n
 * led strip makes cool effects
 *
 * @param membase pointer to base memory of peripherals
 */
void game_transition(unsigned char *membase) {
    uint32_t red = 0x0u;
    uint32_t green = 0x0u;
    uint32_t blue = 0x0u;
    uint32_t value = 0x0u;
    uint32_t strip_1 = 0xcccc3333;
    uint32_t strip_2 = 0x3333cccc;
    struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 2 * 1000 * 1000};
    light_leds(membase, strip_1);
    for (int i = 0; i < 256; i++) {
        red += 0x1u;
        value = ((red << 16) | (green << 8)) | blue;
        light_left_diode(membase, value);
        light_right_diode(membase, value);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
    light_leds(membase, strip_2);
    for (int i = 0; i < 256; i++) {
        red -= 0x1u;
        green += 0x1u;
        value = ((red << 16) | (green << 8)) | blue;
        light_left_diode(membase, value);
        light_right_diode(membase, value);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
    light_leds(membase, strip_1);
    for (int i = 0; i < 256; i++) {
        green -= 0x1u;
        blue += 0x1u;
        value = ((red << 16) | (green << 8)) | blue;
        light_left_diode(membase, value);
        light_right_diode(membase, value);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
    light_leds(membase, strip_2);
    for (int i = 0; i < 256; i++) {
        blue -= 0x1u;
        value = ((red << 16) | (green << 8)) | blue;
        light_left_diode(membase, value);
        light_right_diode(membase, value);
        clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
    }
    light_leds(membase, EMPTY);
}

/**
 * gets if any knob button was pushed
 *
 * @param knobs structure that holds knobs last state
 *
 * @returns 0 if no knob was pushed \n
 *          not 0 if any knob was pushed
 */
int knobs_pushed(knobs_t *knobs) {
    get_knob_value(knobs);
    return get_knob_movement(knobs, RED_B) ||
           get_knob_movement(knobs, GREEN_B) ||
           get_knob_movement(knobs, BLUE_B);
}
