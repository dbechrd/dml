#pragma once
#include "file.h"

#define INT_MAX_LEN 32
#define FLOAT_MAX_LEN 64
#define STRING_MAX_LEN 256

void eat_chars(char *buf, size_t buf_len, size_t *len, file *f, char delim);
uint32_t parse_int(file *f, char delim);
float parse_float(file *f, char delim);
const char *parse_string(file *f, char delim);