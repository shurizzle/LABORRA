#ifndef __JSON_H__
#define __JSON_H__

#include <stdio.h>
#include <string.h>

static int json_get_utf8_char_len(unsigned char ch) {
  if ((ch & 0x80) == 0) return 1;
  switch (ch & 0xf0) {
    case 0xf0:
      return 4;
    case 0xe0:
      return 3;
    default:
      return 2;
  }
}

int json_escape(FILE *out, const char *p, size_t len) {
  size_t i, cl, n = 0;
  const char *hex_digits = "0123456789abcdef";
  const char *specials = "btnvfr";

  for (i = 0; i < len; i++) {
    unsigned char ch = ((unsigned char *) p)[i];
    if (ch == '"' || ch == '\\') {
      n += fwrite("\\", sizeof(char), 1, out);
      n += fwrite(p + 1, sizeof(char), 1, out);
    } else if (ch >= '\b' && ch <= '\r') {
      n += fwrite("\\", sizeof(char), 1, out);
      n += fwrite(&specials[ch - '\b'], sizeof(char), 1, out);
    } else if (isprint(ch)) {
      n += fwrite(p + i, sizeof(char), 1, out);
    } else if ((cl = json_get_utf8_char_len(ch)) == 1) {
      n += fwrite("\\u00", sizeof(char), 4, out);
      n += fwrite(&hex_digits[(ch >> 4) % 0xf], sizeof(char), 1, out);
      n += fwrite(&hex_digits[ch % 0xf], sizeof(char), 1, out);
    } else {
      n += fwrite(p + i, sizeof(char), 1, out);
      i += cl - 1;
    }
  }

  return n;
}

int json_print_string(FILE *out, char *str) {
    size_t n = 0;
    if (str) {
        n += fwrite("\"", sizeof(char), 1, out);
        n += json_escape(out, str, strlen(str));
        n += fwrite("\"", sizeof(char), 1, out);
    } else {
        n += fwrite("null", sizeof(char), 4, out);
    }

    return n;
}

#endif
