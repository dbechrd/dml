#pragma once
#include "file.h"

#define INT_MAX_LEN 32
#define FLOAT_MAX_LEN 64
#define STRING_MAX_LEN 256

#define CHAR_DIGIT "0123456789"
#define CHAR_FLOAT CHAR_DIGIT "-.fxABCDEFe+"
#define CHAR_ALPHA_LOWER "abcdefghijklmnopqrstuvwxyz"
#define CHAR_ALPHA_UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHAR_ALPHA CHAR_ALPHA_LOWER CHAR_ALPHA_UPPER
#define CHAR_ALPHA_NUM CHAR_ALPHA CHAR_DIGIT
#define CHAR_SPECIAL "`~!@#$%^&*()-_=+[{]}\\|;:',<.>/?"
#define CHAR_SPACE " "
#define CHAR_ESCAPE "\t\"\r\n"

#define CHAR_SCENE_NAME CHAR_ALPHA_NUM CHAR_SPECIAL CHAR_SPACE
#define CHAR_PROP_IDENTIFIER CHAR_ALPHA_LOWER
#define CHAR_TYPE_IDENTIFIER CHAR_ALPHA_NUM CHAR_SPECIAL CHAR_SPACE

void eat_chars(char *buf, size_t buf_len, size_t *len, file *f, char delim,
               const char *valid_chars);
unsigned int read_uint(file *f, char delim);
unsigned int parse_uint(char *buf);
int read_int(file *f, char delim);
int parse_int(char *buf);
float read_float(file *f, char delim);
float parse_float(char *buf);
const char *read_string(file *f, char delim, const char *valid_chars);