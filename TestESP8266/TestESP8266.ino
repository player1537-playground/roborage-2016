#include <SoftwareSerial.h>

#define ESP8266_RX 2
#define ESP8266_TX 3

SoftwareSerial esp8266(ESP8266_RX, ESP8266_TX);

void setup() {
  pinMode(ESP8266_RX, INPUT);
  pinMode(ESP8266_TX, OUTPUT);
  esp8266.begin(57600);
  Serial.begin(9600);
  esp8266.listen();
}

void loop() {
  uint8_t buffer[128];
  uint8_t index;
  enum { NONE, GOT_R, GOT_N } flag;
  int c;

  esp8266.write("AT+CWSAP=\"ESP_12345\",\"failsafe\",11,0\r\n");

  index = 0;
  flag = NONE;
  while (index < 128) {
    c = esp8266.read();
    if (c < 0) {
      continue;
    }

    /* Serial.print("Before "); */
    /* Serial.println(c); */
    c &= 0xFF;
    /* Serial.print("After "); */
    /* Serial.println(c); */

    buffer[index++] = c;

    if (flag == NONE && c == '\r') {
      Serial.println("flag == NONE && c == '\\r'");
      flag = GOT_R;
    } else if (flag == GOT_R && c == '\n') {
      Serial.println("flag == GOT_R && c == '\\n'");
      flag = GOT_N;
      break;
    } else {
      Serial.print("else '");
      Serial.print((char)c);
      Serial.print("' = ");
      Serial.print((int)c);
      Serial.println("");
      flag = NONE;
    }
  }

  Serial.write(buffer, index);
}
