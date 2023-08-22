*You can see basic description of different program modules and their functions within the program below.*

*More thorough documentation for individual functions can be generated with doxygen from source files using provided Doxyfile*

## ai_interface.h

An interface describing what functions an AI implementation needs to implement and specifies their parameters and return values.

Steps to add a new AI implementation to the game:

- Copy this file, change its name and names of its functions and use it as the new AI implementation's header file.

- In *settings.h*:
    - increment AI_COUNT macro
    - define new macro \<NAME\>_AI that will have next number from the *_AI group of macros (they serve as indexes)
    - define new macro \<NAME\>_AI_LABEL that will hold label for the AI (something short, <7 characters, otherwise it will not fit)

- In *settings.c* in *init_settings_fields* function add the \<NAME\>_AI_LABEL macro to *ai_labels* array on the position of the \<NAME\>_AI macro

- In *game.c* in *update_ai_paddle* function add a call of the new AI implementation's *ai_move* function as a new case.

## game.h

Contains all constants used in *game.c*. That includes:

- **Game properties:** sizes and speeds of game objects at different difficulty settings, initial lives etc.

- **Log options:** turn logging of *game.c* on/off, change its log header

- **LED settings:** change how LEDs react to different states of the game

Contains *struct game_data* used to pass information about the game state to other modules.

Contains declarations of functions of game used by different modules. (Declarations of functions used only within the game module are included in *game.c*.)

## game.c

Handles all game logic and game update loop. Uses the game_view module to handle the game graphics.

## game_view.h

Contains all constants used in *game_view.c*. That includes:

- **Colors and appearance:** change colors and appearance of different parts of the game court

- **Log options:** turn logging of *game_view.c* on/off, change its log header

Contains declarations of functions of game_view used by different modules. (Declarations of functions used only within the game_view module are included in *game_view.c*.)

## game_view.c

Handles the game graphics and rendering. Counts time in multiplayer mode to display it.

## graphics.h

Contains constants and function headers used in graphics.c. That includes:

- **properties of the LCD display** - dimensions

- **labels** for all pages created by functions in graphics.c

## graphics.c

Contains functions that create most of visuals in the app and handles the lcd display.

It is responsible for frame and lcd initialization and destruction. Then it is
able to show content of frame on the display, reset frame and lcd.

Also it contains functions that create certain pages (title page, result page, ...)

Text rendering is handled by different module.

## menu.h

Contains constants for menu.c and headers of functions implemented by menu.c.
For example:

- counts of items in individual menus

- indexes of individual items in menus

- controls that allow to move in menus

## menu.c

Contains functions to create custom menus with both interactive and noninteractive items.

Contains functions that create specific menus present in this application.

It is able to redraw the menus and alter settings based on user input.

## peripherals.h

Contains constants for peripherals.c such as masks to get only some bits from peripherals.

Contains definition of structures that hold state of other peripherals than the lcd.
One for rotary knobs and one for leds.

## peripherals.c

Consists of functions that handle other peripherals used than the lcd.

Contains functions to light rgb diodes or led strip.

Contains functions to detect movement of knobs.

Contains functions to creates specific light effects.

## pong.c

The main module of the application. Creates flow between menus and game through
settings.

Contains function to compare scores with highscores.

Manages memory and its release.

## player_input.h

Contains all constants used in *player_input.c*. That includes:

- **Keyboard settings:** change the keyboard controls

- **Log options:** turn logging of *player_input.c* on/off, change its log header

Contains *struct input* used to pass information about player input to the game module.

Contains declarations of functions of player_input used by different modules. (Declarations of functions used only within the player_input module are included in *player_input.c*.)

## player_input.c

Handles the initialization of command line raw mode and setting the command line back to its original settings.

Handles the keyboard input.

## rgb565.h

Contains definitions of colors used in the app in the rgb 565 format (16bit)

## settings.h

Contains constants for settings.c. That includes:

- labels and indexes for difficulty and ai settings

- counts of individual settings' items

Contains definition of two structures that provide communication between menu and game.

- **settings_t** - holds current settings selected by user (colors, ai, difficulty, gamemode)

- **settings_fields_t** - holds all possible items for each setting

## settings.c

contains initializtion and destruction of the two structures from settings.h and
setting their default values.

Contains functions to get next or previous setting based on given one.

## text.h

Contains function headers used in text.c and mask used in rendering fonts.

## text.c

Contains functions to draw chars and strings to frame on given positions and
computing their widths based on used font.
