ARDUINO_LIBS = SoftwareSerial esp8266
USER_LIB_PATH = $(abspath libraries)
include ../Makefile

t/test_esp8266: t/test_esp8266.c libraries/esp8266/esp8266.c libraries/esp8266/esp8266.h
	gcc -pedantic -Wall -Werror -o $@ -Ilibraries/esp8266 $(filter %.c,$^)

.PHONY: test
test: t/test_esp8266
	t/test_esp8266
