#pragma once
#include "file.h"

#define INT_MAX_LEN         32
#define FLOAT_MAX_LEN       64
#define FLOAT_HEX_LEN       8
#define FLOAT_HEX_MAX_DELTA 0.00001f
#define STRING_MAX_LEN      256

#define CHAR_DIGIT              "0123456789"
#define CHAR_FLOAT              CHAR_DIGIT "-+.f"
#define CHAR_HEX_LOWER          CHAR_DIGIT "abcdef"
#define CHAR_HEX_UPPER          CHAR_DIGIT "ABCDEF"
#define CHAR_HEX                CHAR_DIGIT "abcdefABCDEF"
#define CHAR_ALPHA_LOWER        "abcdefghijklmnopqrstuvwxyz"
#define CHAR_ALPHA_UPPER        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHAR_ALPHA              CHAR_ALPHA_LOWER CHAR_ALPHA_UPPER
#define CHAR_ALPHA_NUM          CHAR_ALPHA CHAR_DIGIT
#define CHAR_SPECIAL            "`~!@#$%^&*()-_=+[{]}\\|;:,<.>/?"
//#define CHAR_ESCAPE         "\t\"\r\n"

#define CHAR_CHAR_DELIM         "'"
#define CHAR_STRING_DELIM       "\""
#define CHAR_COMMENT            "#"
#define CHAR_WHITESPACE         " \t\n"
#define CHAR_EOL                "\n"
#define CHAR_ARRAY_LEN_START    "["
#define CHAR_ARRAY_LEN_END      "]"
#define CHAR_ARRAY_START        "{"
#define CHAR_ARRAY_END          "}"

#define CHAR_IDENTIFIER_START   CHAR_ALPHA_LOWER
#define CHAR_IDENTIFIER         CHAR_ALPHA_LOWER CHAR_DIGIT "_"
#define CHAR_TYPE               CHAR_ALPHA_LOWER CHAR_DIGIT
#define CHAR_CHAR_LITERAL       CHAR_ALPHA_NUM CHAR_SPECIAL " "
#define CHAR_STRING_LITERAL     CHAR_ALPHA_NUM CHAR_SPECIAL CHAR_WHITESPACE "'"
#define CHAR_COMMENT_LITERAL     CHAR_ALPHA_NUM CHAR_SPECIAL " \t'\""
#define CHAR_SEPARATOR          CHAR_WHITESPACE CHAR_EOL

char eat_chars(char *buf, size_t buf_len, size_t *len, file *f,
               const char *delims, const char *valid_chars);
unsigned int read_uint(file *f, const char *delims);
unsigned int parse_uint(char *buf);
int read_int(file *f, const char *delims);
int parse_int(char *buf);
float read_float(file *f, const char *delims);
float parse_float(char *buf);
char read_char(file *f, const char *delims, const char *valid_chars);
const char *read_string(file *f, const char *delims, const char *valid_chars);