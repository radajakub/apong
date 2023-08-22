/** @file
 * Module that handles printing of putting text int the buffer \n
 * is usable for any font that complies with font_descriptor_t type
 */

#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>
#include "graphics.h"
#include "font_types.h"

/* mask to get if first bit is 1 or 0 */
#define MASK 0x8000u

/**
 * gets width of passed character in passed font
 *
 * @param font font descriptor that defines the widht for chars
 * @param ch character whose width is desired
 *
 * @returns widht of char in pixels
 */
int get_char_width(font_descriptor_t *font, char ch);

/**
 * gets width of passed string in passed font in pixels
 *
 * @param font font descriptor that defines the width for chars
 * @param string string whose width is desired
 *
 * @returns widht of the string in pixels
 */
int get_string_width(font_descriptor_t *font, char *string);

/**
 * puts char on passed cooridnates in frame buffer
 *
 * @param x horizontal coordinate of top-left corner of the character
 * @param y vertical coordinate of top-left corner of the character
 * @param frame buffer to put char pixels int
 * @param font font descriptor in which font is char written
 * @param ch char the is being put
 * @param text_color color of the char in rgb 565 format
 * @param background_color color of the pixels around the char in rgb 565
 */
void put_char(int x, int y, uint16_t *frame, font_descriptor_t *font, char ch, uint16_t text_color, uint16_t background_color);

/**
 * puts string on passed cooridnates in frame buffer
 *
 * @param x horizontal coordinate of top-left corner of the first character
 * @param y vertical coordinate of top-left corner of the first character
 * @param font font descriptor in which font string is written
 * @param string string that is being put
 * @param text_color color of the string in rgb 565 format
 * @param background_color color of the pixels around the chars in rgb 565
 */
void put_string(int x, int y, uint16_t *frame, font_descriptor_t *font, char *string, uint16_t text_color, uint16_t background_color);

#endif
