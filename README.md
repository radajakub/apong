# apong

Game pong implemented on MicroZed APO hardware as semetral project for Computer Architectures on FEE CTU

## Assignment

Classic game "Pong":

- output on lcd display
- multiplayer for 2 players against each other with lives
- singleplayer against AI for highscore, that is saved during the run of the application
- AI implementations - perfect AI which never looses, dumb AI simulating human player
- controlled by IRC rotation knobs or throuth serial port
- simple menu
- settings for ball speed, colors of ball and paddles, ...
- diode strip is changing patterns during game
- RGB diodes react to the state of the game (different blinks on ball hit and life los)
- players' lives, score, time displayed at the top of the display

## Compilation, installation, execution

This app is made for MicroZed APO hardware that uses **ARM** processor.

To compile the game to a file that is executable on MicroZed APO it is necessary to
have `arm-linux-gnueabihf-gcc` toolchain installed.

After that the program can be compiled by executing `make` command in the home directory of this project
(where the Makefile is located).

Then the binary executable file `pong` can be copied to target MicroZed APO kit and executed there.

When connection by *ssh* to the board is available, command `make TARGET_IP=mzapo.ip.address run` can be used to compile it and
run it remotely on MicroZed APO kit (`mzapo.ip.address` is replaced by *ip address* of the target hardware).

## Documentation

To generate technical documentation from the source files it is necessary to have `doxygen` installed.
Then the documentation can be created by executing command `doxygen Doxyfile`. This will create `html` page
located in `/docs` directory. Page can be viewed in any web browser.

To support dependency diagrams in the technical documentation it is necessary to have `graphviz` installed and set the `HAVE_DOT` option in `Doxyfile` to **YES**.

User manual can be viewed in the *wiki* of this project.
