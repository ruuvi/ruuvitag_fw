# Board definitions
This folder contains board definitions for various existing nRF5 boards.
Ruuvi supports only Ruuvi boards.

To add your board, add .h file with board name to this folder,
then add the include to "custom board.h" and
change "-DBOARD\_RUUVITAG\_XY" in makefile to your board.
