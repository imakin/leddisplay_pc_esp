sender.py: read stdin for string and send it to the nodemcuboard so that it display on dotmatrix

font.py: has 8x8 font byte data and api to translate text into the data that will be read by nodemcuboard 

nodemcu_dotmatrix.ino: serial input is a string terminated by newline, contains sequence of 3 digit number, each number is a value of a line in dotmatrix display. from display0 line 0 to display 4 line 7
