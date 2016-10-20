#include "esp8266.h"

int esp8266_init(struct esp8266 *esp) {
  esp->head = 0;
  esp->length = 0;

  return 0;
}

int esp8266_read(struct esp8266 *esp, int i) {
  int len;
  uint8_t c;

  len = 0;

  if (i < 0) {
    return len;
  }

  if (esp->length == ESP8266_BUFFER_SIZE) {
    return ESP8266_BUFFER_FULL;
  }

  len++;
  c = (uint8_t)(i & 0xFF);
  *esp8266_buffer_get(esp, esp->length) = c;
  esp->length++;

  return len;
}

int esp8266_parse_match(struct esp8266 *esp, int start,
                        const uint8_t *expected, int len) {
  int i;
  uint8_t c;

  esp->parsed = 0;
  for (i=start; i<esp->length; ++i) {
    if (esp->parsed == len) {
      break;
    }

    c = *esp8266_buffer_get(esp, i);
    if (c != expected[i]) {
      return ESP8266_NO_MATCH;
    }

    esp->parsed++;
  }

  if (esp->parsed == len) {
    return ESP8266_MATCH;
  }

  return ESP8266_CONTINUE;
}

int esp8266_parse_int(struct esp8266 *esp, int start,
                      int *result) {
  int i;
  uint8_t c;

  *result = 0;
  esp->parsed = 0;
  for (i=start; i<esp->length; ++i) {
    c = *esp8266_buffer_get(esp, i);

    if (c < '0' || c > '9') {
      if (i == 0) {
        return ESP8266_NO_MATCH;
      } else {
        return ESP8266_MATCH;
      }
    }

    esp->parsed++;
    *result *= 10;
    *result += c - '0';
  }

  return ESP8266_CONTINUE;
}

int esp8266_parse_verbatim(struct esp8266 *esp, int start,
                           int msglen,
                           uint8_t *buffer, int buflen) {
  int i;
  uint8_t c;

  esp->parsed = 0;
  for (i=start; i<esp->length; ++i) {
    if (esp->parsed == msglen) {
      return ESP8266_MATCH;
    }

    if (esp->parsed == buflen) {
      return ESP8266_NO_MATCH;
    }

    c = *esp8266_buffer_get(esp, i);
    buffer[esp->parsed] = c;
    esp->parsed++;
  }

  return ESP8266_CONTINUE;
}

int esp8266_parse_ok(struct esp8266 *esp, int start) {
  static const uint8_t ok[] = {'\r','\n','O','K','\r','\n'};
  int rv, total_parsed;

  total_parsed = 0;

  //  \r \n O K \r \n
  // |---------------|
  rv = esp8266_parse_match(esp, start, ok, sizeof(ok));
  start += esp->parsed;
  total_parsed += esp->parsed;
  esp->parsed = total_parsed;
  if (rv != ESP8266_MATCH) {
    return rv;
  }

  return ESP8266_MATCH;
}

int esp8266_parse_message(struct esp8266 *esp, int start,
                          int *channel, int *msglen,
                          uint8_t *buffer, int buflen) {
  static const uint8_t prefix[] = {'\r','\n','+','I','P','D'};
  static const uint8_t comma[] = {','};
  static const uint8_t colon[] = {':'};
  int rv, total_parsed;

  total_parsed = 0;

  //  \r \n + I P D , 0 , 1 : A \r \n O K \r \n
  // |-------------|
  rv = esp8266_parse_match(esp, start, prefix, sizeof(prefix));
  start += esp->parsed;
  total_parsed += esp->parsed;
  esp->parsed = total_parsed;
  if (rv != ESP8266_MATCH) {
    return rv;
  }

  //  \r \n + I P D , 0 , 1 : A \r \n O K \r \n
  //               |-|
  rv = esp8266_parse_match(esp, start, comma, sizeof(comma));
  start += esp->parsed;
  total_parsed += esp->parsed;
  esp->parsed = total_parsed;
  if (rv != ESP8266_MATCH) {
    return rv;
  }

  //  \r \n + I P D , 0 , 1 : A \r \n O K \r \n
  //                 |-|
  rv = esp8266_parse_int(esp, start, channel);
  start += esp->parsed;
  total_parsed += esp->parsed;
  esp->parsed = total_parsed;
  if (rv != ESP8266_MATCH) {
    return rv;
  }

  //  \r \n + I P D , 0 , 1 : A \r \n O K \r \n
  //                   |-|
  rv = esp8266_parse_match(esp, start, comma, sizeof(comma));
  start += esp->parsed;
  total_parsed += esp->parsed;
  esp->parsed = total_parsed;
  if (rv != ESP8266_MATCH) {
    return rv;
  }

  //  \r \n + I P D , 0 , 1 : A \r \n O K \r \n
  //                     |-|
  rv = esp8266_parse_int(esp, start, msglen);
  start += esp->parsed;
  total_parsed += esp->parsed;
  esp->parsed = total_parsed;
  if (rv != ESP8266_MATCH) {
    return rv;
  }

  //  \r \n + I P D , 0 , 1 : A \r \n O K \r \n
  //                       |-|
  rv = esp8266_parse_match(esp, start, colon, sizeof(colon));
  start += esp->parsed;
  total_parsed += esp->parsed;
  esp->parsed = total_parsed;
  if (rv != ESP8266_MATCH) {
    return rv;
  }

  //  \r \n + I P D , 0 , 1 : A \r \n O K \r \n
  //                         |-|
  rv = esp8266_parse_verbatim(esp, start, *msglen, buffer, buflen);
  start += esp->parsed;
  total_parsed += esp->parsed;
  esp->parsed = total_parsed;
  if (rv != ESP8266_MATCH) {
    return rv;
  }

  //  \r \n + I P D , 0 , 1 : A \r \n O K \r \n
  //                           |---------------|
  rv = esp8266_parse_ok(esp, start);
  start += esp->parsed;
  total_parsed += esp->parsed;
  esp->parsed = total_parsed;
  if (rv != ESP8266_MATCH) {
    return rv;
  }

  return ESP8266_MATCH;
}

int esp8266_send_verbatim(struct esp8266 *esp, int start,
                          const uint8_t *tosend, int len,
                          int *msglen,
                          uint8_t *buffer, int buflen) {
  int i;

  *msglen = 0;
  for (i=0; i<len; ++i) {
    buffer[start+i] = tosend[i];
    *msglen += 1;
  }

  return 0;
}

int esp8266_send_attention(struct esp8266 *esp, int start,
                           int *msglen,
                           uint8_t *buffer, int buflen) {
  static const uint8_t tosend[] = {'A','T','\r','\n'};

  return esp8266_send_verbatim(esp, start,
                               tosend, sizeof(tosend),
                               msglen,
                               buffer, buflen);
}

int esp8266_send_cwmode(struct esp8266 *esp, int start,
						   uint8_t mode,
                           int *msglen,
                           uint8_t *buffer, int buflen) {
  uint8_t tosend[] = {'A','T', '+', 'C', 'W', 'M', 'O', 'D', 'E', '=', mode + '0', '\r','\n'};

  return esp8266_send_verbatim(esp, start,
                               tosend, sizeof(tosend),
                               msglen,
                               buffer, buflen);
}

int esp8266_send_cwjap(struct esp8266 *esp, int start,
						   uint8_t *ssid, int ssid_len, uint8_t *pwid, int pwid_len,
                           int *msglen,
                           uint8_t *buffer, int buflen) {
							   
  uint8_t tosend[9 + ssid_len + pwid_len + 2];
  
  static const prefix = {'A','T', '+', 'C', 'W', 'J', 'A', 'P', '='};
  for(int i = 0; i < 9; i++) {
	  to_send[i] = prefix[i];
  }
  for(int i = 0; i < ssid_len; i++) {
	  to_send[i + 9] = ssid[i];
  }
  for(int i = 0; i < pwid_len; i++) {
	  to_send[i + 9 + ssid_len] = pwid[i];
  }
  
  to_send[9 + ssid_len + pwid_len] = '\r';
  to_send[9 + ssid_len + pwid_len + 1] = '\n';
  

  return esp8266_send_verbatim(esp, start,
                               tosend, sizeof(tosend),
                               msglen,
                               buffer, buflen);
}

int esp8266_send_cipmux(struct esp8266 *esp, int start,
						   uint8_t mode,
                           int *msglen,
                           uint8_t *buffer, int buflen) {
  uint8_t tosend[] = {'A','T', '+', 'C', 'I', 'P', 'M', 'U', 'X', '=', mode + '0', '\r','\n'};

  return esp8266_send_verbatim(esp, start,
                               tosend, sizeof(tosend),
                               msglen,
                               buffer, buflen);
}

int esp8266_send_cipmode(struct esp8266 *esp, int start,
						   uint8_t mode,
                           int *msglen,
                           uint8_t *buffer, int buflen) {
  uint8_t tosend[] = {'A','T', '+', 'C', 'I', 'P', 'M', 'O', 'D', 'E', '=', mode + '0', '\r','\n'};

  return esp8266_send_verbatim(esp, start,
                               tosend, sizeof(tosend),
                               msglen,
                               buffer, buflen);
}

int esp8266_send_cipserver(struct esp8266 *esp, int start,
						   uint8_t mode,
                           int *msglen,
                           uint8_t *buffer, int buflen) {
  uint8_t tosend[] = {'A','T', '+', 'C', 'I', 'P', 'S', 'E', 'R', 'V', 'E', 'R', '=', mode + '0', '\r','\n'};

  return esp8266_send_verbatim(esp, start,
                               tosend, sizeof(tosend),
                               msglen,
                               buffer, buflen);
}

int esp8266_send_cipserver(struct esp8266 *esp, int start,
						   uint8_t mode,
                           int *msglen,
                           uint8_t *buffer, int buflen) {
  uint8_t tosend[] = {'A','T', '+', 'C', 'I', 'P', 'S', 'E', 'R', 'V', 'E', 'R', '=', mode + '0', '\r','\n'};

  return esp8266_send_verbatim(esp, start,
                               tosend, sizeof(tosend),
                               msglen,
                               buffer, buflen);
}

int esp8266_send_cipsta(struct esp8266 *esp, int start,
                           int *msglen,
                           uint8_t *buffer, int buflen) {
  static const uint8_t tosend[] = {'A','T', '+', 'C', 'I', 'P', 'S', 'T', 'A', 'V', 'E', 'R', '=',
  '1', '9', '2', '.', '1', '6', '8', '.', '1', '0', '1', '.', '0', '2', '0', '\r','\n'};

  return esp8266_send_verbatim(esp, start,
                               tosend, sizeof(tosend),
                               msglen,
                               buffer, buflen);
}




