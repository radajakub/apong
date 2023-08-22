# Start of application

After executing the program a title page appears on the LCD display and diodes
start changing colors. When the whole LED strip is lit, the menu is shown.

# Menu

Menu serves to set settings of the game, view highscores for each enenmy AI and
select a gamemode to play.

First is displayed main menu from where are entered individual submenus by
controls described below.

## Controls

Menu is controlled by three colored knobs below the LCD display.
If program is executed from another computer via **ssh** it can be also
controlled by pressing specific keys on the keyboard of the connected computer.

Item in the menus can be scrolled through. Currently selected item has different
color than the others. Only three items can be shown at a time, the menu shows
one item below and one above if possible.

This table shows what action is invoked each knob movement or key press:

| Knob color | knob movement             | key pressed | action                        |
| :---:      | :---:                     | :---:       | :---:                         |
| RED        | COUNTERCLOCKWISE ROTATION | `w`         | MOVE ONE ITEM UP              |
| RED        | CLOCKWISE ROTATION        | `s`         | MOVE ONE ITEM DOWN            |
| RED        | CLICK                     | `Enter`     | GO TO SUBMENU OR EXECUTE      |
| GREEN      | COUNTERCLOCKWISE ROTATION | `a`         | SET PREVIOUS SETTING          |
| GREEN      | CLOCKWISE ROTATION        | `d`         | SET NEXT SETTING              |
| GREEN      | CLICK                     | `Enter`     | GO TO SUBMENU OR EXECUTE      |
| BLUE       | COUNTERCLOCKWISE ROTATION |             | NONE                          |
| BLUE       | CLOCKWISE ROTATION        |             | NONE                          |
| RED        | CLICK                     | `q`         | GO TO PARENT MENU OR EXIT APP |

## Main menu

In main menu there are four options with these actions when executed:

- **PLAY** - goes to gamemode selection submenu
- **SCORE** - goes to score submenu
- **SETTINGS** - goes to settings
- **QUIT** - quits the app

These options can be scrolled through and executed by using controls described
in the table above.

## Play submenu

In play submenu a gamemode is selected. (see Game)

There are four options with these actions when executed:

- **P V P** - gamemode in which a player plays against other player
- **P V A** - gamemode in which a player on the left side plays against AI on the right side
- **A V P** - gamemode in which an AI on the left side plays against a player on the left side
- **BACK** - goes back to main menu

## Score submenu

In score submenu a highscore can be displayed for each AI present in the game.

There are three items in this menu:

- **\<AI NAME\>** - by SET PREVIOUS SETTINGS or SET NEXT SETTINGS actions an AI
is selected for showing highscore
- **\<NUMBER\>** - displays number that represents highscore for ai that is
selected in previous item, this item is not interactive
- **BACK** - goes back to main menu

## Settings submenu

In settings submenu there are many settings to change. When changed they are saved
automatially, but only during one run of the application (they are not preserved).

There are six tems in this menu:

- **\<DIFFICULTY\>** - by SET PREVIOUS SETTINGS or SET NEXT SETTINGS actions a difficulty
can be changed [EASY, **NORMAL**, HARD]
- **\<AI LABEL\>** - by SET PREVIOUS SETTINGS or SET NEXT SETTINGS actions an AI
to play against is selected [**DUMB**, SMART];
- **BALL COLOR** - by SET PREVIOUS SETTINGS or SET NEXT SETTINGS actions a color
of the ball can be changed (color is displayed in a square)
- **LEFT** - by SET PREVIOUS SETTINGS or SET NEXT SETTINGS actions a color
of the left player's paddle can be changed (color is displayed in a square)
- **RIGHT** - by SET PREVIOUS SETTINGS or SET NEXT SETTINGS actions a color
of the right player's paddle can be changed (color is displayed in a square)
- **BACK** - goes back to main menu

# Start of game

After selecting one of gamemodes in *Player submenu* a start game page shows up.
It displays the gamemode with each player type highlighted by color from settings.
it stays there for few seconds for players to prepare for game.

Then a game starts.

# Game

This is a version of well-known game *Pong*. (see: https://en.wikipedia.org/wiki/Pong)

**Rules of the game**

Each player's goal is to keep the ball from reaching his/her edge of the screen by moving his/her paddle in its way to make it bounce back to the other side again.

In the multiplayer version (PvP), both players start with 3 lives and every time the ball reaches player's edge of the screen that player looses a life. The player who looses all his/her lives first if the looser.

In the singleplayer version (PvA / AvP), the player has only a single life. His/her goal is to get score as high as possible. For every successful ball hit, the player is awarded 1 score point. When the ball reaches the AI's edge of the screen the player is awarded 3 points and the ball is reset. When the ball reaches the player's edge of the screen the game ends.

**Keyboard controls**

You can see the key bindings used to control the paddles with keyboard below.

| ACTION           | LEFT PLAYER | RIGHT PLAYER |
| :--------------: | :---------: | :----------: |
| MOVE PADDLE UP   | 'w'         | 'o'          |
| MOVE PADDLE DOWN | 's'         | 'l'          |

When the player presses one of his/her keys, his/her paddle will start moving in the corresponding direction and won't stop until the he/she presses the other key to start moving in the opposite direction.

(This means that the paddle cannot be stopped once it is set in motion when controlling it via keyboard.)

**Knobs controls**

You can see the controls bindings of the knobs below.

| ACTION           | LEFT PLAYER               | RIGHT PLAYER               |
| :--------------: | :-----------------------: | :------------------------: |
| MOVE PADDLE UP   | red knob counterclockwise | blue knob clockwise        |
| MOVE PADDLE DOWN | red knob clockwise        | blue knob counterclockwise |

When the player rotates his/her knob in on of the two directions, his/her paddle will move in the corresponding direction proportionally to the angle of the rotation.

# Game View

At the top of the screen, there is a colored strip containing game statistics.

- In the multiplayer version (PvP), there are each player's lives displayed on their corresponding sides and the time elapsed from the start of the game displayed in the middle.

- In the singleplayer version (PvA / AvP), there is the current score of the player displayed in the middle.

The rest of the screen is dedicated to the game court divided to halves by a green interrupted line.

There are players' paddles (displayed as rectangles of the player's chosen color) on the sides of the screen and the ball (displayed as a square of the color set in the settings) bouncing between them and the top and bottom edge of the game court.

# LEDs in-game

Both RGB LEDs are lit up with a GREEN color by default.

Each RGB LED will briefly light up with a BLUE color upon ball being hit with the paddle on the corresponding side.

Each RGB LED will blink RED three times upon ball reaching the corresponding side of the game court.

# End of game

After the game ends a game summary page shows up. The content of the page depends
on played gamemode:

- **P V P** - it shows winner and score of the game (how many lives has player taken
from the other)
- **P V A** or **A V P** - if a highscore against the AI was made, only the new
highscore is shown, else, both achieved score and the current highscore against
the AI are shown.

This page is shown until a key or a knob is pressed. Then the application returns
to main menu.
