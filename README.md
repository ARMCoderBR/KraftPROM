## Project

EEPROM Programmer

## Description

A Hardware + Software suite to program EEPROMs (28C16 and 28C64).

## Version & Date
1.0 - 2025-APR-24

## Author
Milton Maldonado Jr (ARM_Coder)

## License
GPL V2

## Target

PC Client Software: Any Linux with GCC

The repo for the PC client is here: https://github.com/ARMCoderBR/eeprom_sender

Firmware: Microchip PIC16F628A under MPLABX IDE 6.X and XC8 2.X (no PRO License
          needed). A suitable HEX file is already supplied for this PIC, so no
          IDE and compiler will be needed unless you will choose to use a
          different PIC.

## Hardware Materials

1  x  Universal PCB or Breadboard
1  x  PIC16F628A
3  x  74LS164 or HC164
1  x  74LS245 or HC245
1  x  74LS165 or HC165
1  x  ZIF Socket DIP 28 PIN
7  x  Ceramic Cap 100nF
2  x  Resistor 4k7
1  x  Jumper 3 pin
1  x  Molex 5 pin female
3  x  DIP 14 Socket (not needed for breadboard)
1  x  DIP 16 Socket   "     "    "      "
1  x  DIP 18 Socket   "     "    "      "
1  x  DIP 20 Socket   "     "    "      "
1  x  PC Serial-to-TTL adapter with 5V compatibility
1+ x  28C16 or 28C64 as target(s)

Wires, solder, etc.

See picture and schematics.

NOTE: 

## Equipment needed to Build and Program

PC
USB cables
Solder
Cutter and pliers
Multimeter
PIC programmer dongle (eg Pickit3)

## Building the Programmer

As said, if you have the recommended PIC16F628A, you can program it with the
supplied HEX file. This is the easiest way because you don't need to install
the complete MPLABX and the C compiler.

The rest of the build is a no-brainer, just be careful with the connections.
The Serial-to-TTL adapter is very practical, it supplies the power to the
hardware (5VDC) and the communication signals (TX/RX/CTS).

## Using the Programmer

All the control is done by the PC client software, as indicated above.

## Disclaimer

This project is supplied 'as is' with no warranty against bugs and errors.

