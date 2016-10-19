#ifndef _ESP8266_H_
#define _ESP8266_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ESP8266_BUFFER_SIZE 128

#define ESP8266_BUFFER_FULL -1
#define ESP8266_NO_MATCH -1
#define ESP8266_MATCH 0
#define ESP8266_CONTINUE 1

struct esp8266 {
  uint8_t buffer[ESP8266_BUFFER_SIZE];
  int head;
  int length;
  int parsed;
};

static inline uint8_t *esp8266_buffer_get(struct esp8266 *esp, int index) {
  return &esp->buffer[(esp->head + index) % ESP8266_BUFFER_SIZE];
}

int esp8266_init(struct esp8266 *esp);
int esp8266_read(struct esp8266 *esp, int i);

int esp8266_parse_match(struct esp8266 *esp, int start,
                        const uint8_t *expected, int len);
int esp8266_parse_int(struct esp8266 *esp, int start,
                      int *result);
int esp8266_parse_verbatim(struct esp8266 *esp, int start,
                           int msglen,
                           uint8_t *buffer, int buflen);

int esp8266_parse_ok(struct esp8266 *esp, int start);
int esp8266_parse_message(struct esp8266 *esp, int start,
                          int *channel, int *msglen,
                          uint8_t *buffer, int buflen);

#ifdef __cplusplus
}
#endif


#endif
