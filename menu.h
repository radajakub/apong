/** @file
 * Module with functions to create, modify and control menu in this application \n
 * It modifies settings structure according to selections in the menu
 */

#ifndef MENU_H
#define MENU_H

#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "graphics.h"
#include "settings.h"
#include "font_types.h"
#include "rgb565.h"
#include "mzapo_parlcd.h"
#include "peripherals.h"

#define MAIN_MENU_HEADER "MAIN MENU: "
#define SETTINGS_MENU_HEADER "SETTINGS MENU: "
#define PLAY_MENU_HEADER "PLAY MENU: "
#define HIGHSCORE_MENU_HEADER "HIGHSCORE MENU: "

/* cosntants to detect if after exiting menu a game should be started or exited*/
#define STOP 0
#define START 1

/* colors for selected and unselected items in menu */
#define SELECTED RED
#define UNSELECTED BLUE
#define MENU_BACKGROUND BLACK

/* layout constans of items in menu*/
#define PADDING 4
#define SPACING 6
#define MENU_FONT_SIZE 88
#define MENU_SMALLFONT_SIZE 44
#define ITEMS_ON_PAGE 3

/* constants that hold number of items in menu and submenus */
#define MAIN_MENU_ITEMS 4
#define SETTINGS_MENU_ITEMS 6
#define PLAY_MENU_ITEMS 4
#define HIGHSCORE_MENU_ITEMS 3

/* indexes of items in main menu */
#define PLAY 0
#define HIGHSCORES 1
#define SETTINGS 2
#define QUIT 3

/* indexes of items in play mode submenu */
#define PLAYER_PLAYER 0
#define PLAYER_AI 1
#define AI_PLAYER 2
#define PLAY_BACK 3

/* settings values for players set according to choices above */
#define IS_PLAYER 1
#define IS_AI 0

/* indexes of items in settings submenu */
#define DIFFICULTY 0
#define SETTINGS_AI 1
#define BALL_COLOR 2
#define LEFT_COLOR 3
#define RIGHT_COLOR 4
#define SETTINGS_BACK 5

/* values that are returned from play submenu
 * (games starts / goes back to main menu) 
 */
#define GAME 0
#define MENU 1

/* indexes of highscore options */
#define HIGHSCORE_AI 0
#define HIGHSCORE_NUMBER 1
#define HIGHSCORE_BACK 2

/* chars that serve as menu controls from serial input */
#define DOWN 's'
#define UP 'w'
#define LEFT 'a'
#define RIGHT 'd'
#define BACK 'q'
#define ACTION '\n'

/* positions for put_label_settings */
#define NOT_CENTER 0
#define CENTER 1

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
void put_menu_element(int y, char *label, uint16_t *frame, font_descriptor_t *font, uint16_t color);

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
void fill_menu(int *y_offsets, char **labels, int count, int selected, font_descriptor_t *font, uint16_t *frame);

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
void fill_settings_menu(int *y_offsets, char **labels, int count, int selected, font_descriptor_t *bigfont, font_descriptor_t *smallfont, uint16_t *frame, settings_t *settings);

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
void fill_highscore_menu(int *y_offsets, char **labels, char *ai_label, int highscore, int count, int selected, font_descriptor_t *font, uint16_t *frame);

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
void put_label_settings(int y_offset, char *label, font_descriptor_t *font, uint16_t *frame, uint16_t color, int position);

/**
 * puts currently selected color setting into menu item
 *
 * @param y_offset offset of the top-left corner of the menu item
 * @param color currently selected color for settings whose menu item is currently placed
 * @param font pointer to font structure used for writing text
 * @param frame frame buffer that holds the menu items
 */
void put_color_settings(int y_offset, uint16_t color, font_descriptor_t *font, uint16_t *frame);

/**
 * checks knobs for input and convert it to menu suitable format
 *
 * @param input pointer to char which decides about conducted action
 * @param knobs_use pointer to variable which determines that knobs recorded input
 * @param knobs pointer to structure that holds information about state of knobs
 */
void check_knobs(char *input, int *knobs_use, knobs_t *knobs);

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
int main_menu(settings_t *settings, settings_fields_t *settings_fields, knobs_t *knobs, uint16_t *frame, font_descriptor_t *bigfont, font_descriptor_t *smallfont, unsigned char *lcd_membase);

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
int play_menu(settings_t *settings, knobs_t *knobs, uint16_t *frame, int *y_offsets, font_descriptor_t *font, unsigned char *lcd_membase);

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
void settings_menu(settings_t *settings, settings_fields_t *settings_fields, knobs_t *knobs, uint16_t *frame, int *y_offsets, font_descriptor_t *bigfont, font_descriptor_t *smallfont, unsigned char *lcd_membase);

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
void highscores_menu(settings_fields_t *settings_fields, knobs_t *knobs, uint16_t *frame, int *y_offsets, font_descriptor_t *font, unsigned char *lcd_membase);

#endif
