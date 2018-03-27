# MultiTerminal

MultiTerminal is a work in progress graphical serial terminal program for Linux which supports multiple number systems. It is inspired by the discontinued hTerm. The main design goal of this program is to support various embedded devices, even those which use non ASCII commands or dump out raw data.

## Main features
* Supports ASCII, Hex, Decimal, Octal and Binary IO
* Received data can be viewed in multiple number systems
* Input data can be in mixed number systems
* Input History with configurable length
* Compact GUI to save screen space for your data
* Configurable newline character
* Configurable maximum line size, useful to view raw binary data
* RX and TX data is shown in one single window
* RX and TX counters
* Manual control over DSR, CTS, DCD, RNG, DTR and RTS pins
* Logging sessions to file
* Transmit files over serial port
* Handles multiple simultaneous connections
* Configurable line end sequences
* Sessions, settings and history are stored when the program is closed
* Licensed under GNU GPLv3

### Screenshot

![MultiTerminal](https://raw.githubusercontent.com/mmmaisel/MultiTerminal/master/MultiTerminal.png "MultiTerminal")

## Build instructions
### Linux
Run:
```
./bootstrap
./configure
make
make install
```
#### Dependencies
* yaml-cpp >= 0.5
* wxWidgets >= 3.2.1
* boost >= 1.58 (headers only)
### Other systems
Other *NIX systems may work but are not tested (yet).

## Planned features
* Support other interface types than serial
* RX and TX timestamps
* Additional logging formats
* Copy and paste to/from input/output box in multiple number systems
* Add mouse support to input and output box
