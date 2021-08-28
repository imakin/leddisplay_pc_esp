import sys
import time
import serial
MSG_HEAD = " " # as defined in esp12_leddisplay.ino, message started with " " is to be displayed on oled
with serial.Serial('/dev/ttyUSB0', 115200, timeout=1) as ser:
    def process_input(textline):
        textline = MSG_HEAD+textline
        ser.write(textline.encode('utf8'))

    for line in sys.stdin:
        process_input(line)
