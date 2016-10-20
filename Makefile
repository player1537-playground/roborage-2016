ARDUINO_DIR   = /home/thobson/src/arduino/arduino-1.6.12
ARDMK_DIR     = /usr/share/arduino
AVR_TOOLS_DIR = /usr
MONITOR_PORT  = /dev/ttyACM0
BOARD_TAG     = uno
include $(ARDMK_DIR)/Arduino.mk

driver: driver.c libraries/esp8266/esp8266.c libraries/esp8266/esp8266.h
	gcc -pedantic -Wall -Werror -o $@ -Ilibraries/esp8266 $(filter %.c,$^)

js-driver: js-driver.c
	gcc -pedantic -Wall -Werror -o $@ $(filter %.c,$^)

connect-to-ardunio: connect-to-arduino.c
	gcc -std=gnu11 -pedantic -Wall -Werror -o $@ $(filter %.c,$^)
