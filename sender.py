#sender for dotmatrix, text will be converted to the numbersequence as defined in esp12_leddisplay.ino
import time
import serial

import font

with serial.Serial('/dev/ttyUSB0', 115200, timeout=1) as ser:
    text = input()
    font_value_string = font.to_font_value_string(text)+'\n';
    ser.write(font_value_string.encode('utf8'))
