/** @file
 * Module that handles printing of putting text int the buffer \n
 * is usable for any font that complies with font_descriptor_t type
 */

#include "text.h"

/**
 * gets width of passed character in passed font
 *
 * @param font font descriptor that defines the widht for chars
 * @param ch character whose width is desired
 *
 * @returns widht of char in pixels
 */
int get_char_width(font_descriptor_t *font, char ch) {
    int width = 0;
    if ((ch >= font->firstchar) && (ch - font->firstchar < font->size)) {
        ch -= font->firstchar;
        if (!font->width) {
            width = font->maxwidth;
        } else {
            width = font->width[(int)ch];
        }
    }
    return width;
}

/**
 * gets width of passed string in passed font in pixels
 *
 * @param font font descriptor that defines the width for chars
 * @param string string whose width is desired
 *
 * @returns widht of the string in pixels
 */
int get_string_width(font_descriptor_t *font, char *string) {
    char *ch = string;
    int width = 0;
    while(*ch) {
        width += get_char_width(font, *ch);
        ch++;
    }
    return width;
}

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
void put_char(int x, int y, uint16_t *frame, font_descriptor_t *font, char ch, uint16_t text_color, uint16_t background_color) {
    int x0 = x;
    uint32_t offset = font->offset[(int)ch - font->firstchar];
    int width = get_char_width(font, ch);
    uint16_t bits = 0x0u;
    for (int i = 0; i < font->height; i++) {
        for (int j = 0; j < width ; j++) {
            if (j % 16 == 0) {
                bits = font->bits[offset++];
            }
            if (bits & MASK) {
                put_pixel(x, y, text_color, frame);
            } else {
                put_pixel(x, y, background_color, frame);
            }
            bits = bits << 1;
            x++;
        }
        x = x0;
        y++;
    }
}

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
void put_string(int x, int y, uint16_t *frame, font_descriptor_t *font, char *string, uint16_t text_color, uint16_t background_color) {
    while (*string) {
        put_char(x, y, frame, font, *string, text_color, background_color);
        x += get_char_width(font, *string);
        string++;
    }
}
