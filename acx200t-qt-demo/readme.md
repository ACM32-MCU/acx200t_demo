# acx200t-qt-demo
This demo is a about how to use AisinoChip ACX200T chip library in Qt.

# Features

- Introduce AisinoChip Corporation
- Introduce ACX200T Chip
- Introduce AisinoChip V2X Solution
- ACX200T SM2 Sign and Verify Speed Demo

# Build

This project has been build successful in Qt 5.9.4 by arm-poky-linux-gnueabi-g++.

# Notes

- This demo used the spi device is spidev1.0 and gpio number is gpio101. Please change them as your hardware design. 
- The other API in libacx200t.so will been easily add in Qt application like the sm2 sign api.
