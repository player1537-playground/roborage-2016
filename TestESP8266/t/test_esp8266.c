#include <esp8266.h>
#include <string.h>
#include "tap.c"

void _esp8266_read_string(struct esp8266 *esp, const char *s) {
  int i, len, rv;

  len = strlen(s);
  for (i=0; i<len; ++i) {
    rv = esp8266_read(esp, (int)s[i]);
    if (rv != 1) {
      cmp_ok(rv, "==", 1, "_read_string should add each character");
      BAIL_OUT("_esp8266_read_string(\"%s\") should not fail", s);
    }
  }
}

void _esp8266_copy_to_buffer(struct esp8266 *esp, uint8_t *buffer, int buflen) {
  uint8_t c;
  int i;

  for (i=0; i<ESP8266_BUFFER_SIZE; ++i) {
    if (i > buflen) {
      cmp_ok(i, "<=", buflen, "_copy_to_buffer should not overwrite buffer");
      BAIL_OUT("_esp8266_copy_to_buffer() should not fail");
    }

    c = *esp8266_buffer_get(esp, i);
    buffer[i] = c;
  }
}

void test_esp8266_init(struct esp8266 *esp) {
  int rv;

  rv = esp8266_init(esp);
  cmp_ok(rv, "==", 0, "esp should initialize correctly");

  cmp_ok(esp->head, "==", 0, "head should be 0 initially");
  cmp_ok(esp->length, "==", 0, "length should be 0 initially");
}

void test_esp8266_read(struct esp8266 *esp) {
  int rv, i, prev_head, prev_length;
  const char *s;
  char buffer[ESP8266_BUFFER_SIZE];

  i = 'A';
  esp8266_init(esp);
  rv = esp8266_read(esp, i);
  cmp_ok(rv, "==", 1, "normal read should return 1 byte");
  rv = (int)*esp8266_buffer_get(esp, esp->head);
  cmp_ok(rv, "==", i, "the correct value should be stored");

  i = -1;
  esp8266_init(esp);
  prev_head = esp->head;
  prev_length = esp->length;
  rv = esp8266_read(esp, i);
  cmp_ok(rv, "==", 0, "invalid read should return 0 bytes");
  cmp_ok(esp->head, "==", prev_head, "invalid read should not move head");
  cmp_ok(esp->length, "==", prev_length, "invalid read should not move head");

  esp8266_init(esp);
  for (i=0; i<ESP8266_BUFFER_SIZE; ++i) {
    rv = esp8266_read(esp, i & 0xFF);
    if (rv <= 0) {
      cmp_ok(rv, ">", 0, "filling buffer should not fail");
      diag("index = %d", i);
      goto buffer_fill_failed;
    }
  }
  prev_head = esp->head;
  prev_length = esp->length;
  rv = esp8266_read(esp, i);
  cmp_ok(rv, "==", ESP8266_BUFFER_FULL, "filled read should error");
  cmp_ok(esp->head, "==", prev_head, "filled read should not move head");
  cmp_ok(esp->length, "==", prev_length, "filled read should not move head");
 buffer_fill_failed: ;

  s = "hello world";
  esp8266_init(esp);
  _esp8266_read_string(esp, s);
  _esp8266_copy_to_buffer(esp, (uint8_t *)buffer, ESP8266_BUFFER_SIZE);
  rv = strncmp(s, buffer, strlen(s));
  cmp_ok(rv, "==", 0, "_read_string + _copy_to_buffer should reproduce string");
  if (rv != 0) {
    is((const char *)buffer, (const char *)s, "should be the same");
  }
}

void test_esp8266_parse_ok(struct esp8266 *esp) {
  const char *s, *s2;
  int rv;

  s = "\r\nOK\r\n";
  esp8266_init(esp);
  _esp8266_read_string(esp, s);
  rv = esp8266_parse_ok(esp, 0);
  cmp_ok(rv, "==", ESP8266_MATCH, "correct parse ok should match");
  rv = esp->parsed;
  cmp_ok(rv, "==", strlen(s), "correct parse ok should have correct length");


  s = "\r\nOK\r";
  esp8266_init(esp);
  _esp8266_read_string(esp, s);
  rv = esp8266_parse_ok(esp, 0);
  cmp_ok(rv, "==", ESP8266_CONTINUE, "truncated parse ok should continue");
  rv = esp->parsed;
  cmp_ok(rv, "==", strlen(s), "truncated parse ok should have correct length");

  s = "\r\nOK\r\n";
  s2 = "FOO";
  esp8266_init(esp);
  _esp8266_read_string(esp, s);
  _esp8266_read_string(esp, s2);
  rv = esp8266_parse_ok(esp, 0);
  cmp_ok(rv, "==", ESP8266_MATCH, "overfilled parse ok should match");
  rv = esp->parsed;
  cmp_ok(rv, "==", strlen(s), "overfilled parse ok should have correct length");

  s = "";
  esp8266_init(esp);
  _esp8266_read_string(esp, s);
  rv = esp8266_parse_ok(esp, 0);
  cmp_ok(rv, "==", ESP8266_CONTINUE, "empty parse ok should continue");
  rv = esp->parsed;
  cmp_ok(rv, "==", strlen(s), "empty parse ok should have correct length");

  s = "\r\n";
  s2 = "FOO";
  esp8266_init(esp);
  _esp8266_read_string(esp, s);
  _esp8266_read_string(esp, s2);
  rv = esp8266_parse_ok(esp, 0);
  cmp_ok(rv, "==", ESP8266_NO_MATCH, "partial parse ok should not match");
  rv = esp->parsed;
  cmp_ok(rv, "==", strlen(s), "partial parse ok should have correct length");

  s = "";
  s2 = "FOO";
  esp8266_init(esp);
  _esp8266_read_string(esp, s);
  _esp8266_read_string(esp, s2);
  rv = esp8266_parse_ok(esp, 0);
  cmp_ok(rv, "==", ESP8266_NO_MATCH, "incorrect parse ok should continue");
  rv = esp->parsed;
  cmp_ok(rv, "==", strlen(s), "incorrect parse ok should have correct length");
}

int main(int argc, char **argv) {
  struct esp8266 esp;

  plan(NO_PLAN);
  test_esp8266_init(&esp);
  test_esp8266_read(&esp);
  test_esp8266_parse_ok(&esp);
  done_testing();
}
