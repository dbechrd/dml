#pragma once
#include "file.h"

#define MAX_COMMENT_LEN     256
#define MAX_IDENT_LEN       31
#define MAX_NUMBER_LEN      64
#define MAX_STRING_LEN      1024

#define MAX_INT_LEN         32
#define MAX_FLOAT_LEN       64
#define MAX_FLOAT_HEX_LEN   8
#define MAX_FLOAT_HEX_DELTA 0.00001f

#define C__DIGIT            "0123456789"
#define C__SIGN             "+-"
#define C__ALPHA_LOWER      "abcdefghijklmnopqrstuvwxyz"
#define C__ALPHA_UPPER      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define C__ALPHA            C__ALPHA_LOWER C__ALPHA_UPPER
#define C__ALPHA_NUM        C__ALPHA C__DIGIT
#define C__ALPHA_SPECIAL    C__ALPHA_NUM "`~!@#$%^&*()-_=+[{]}\\|;:,<.>/?'"
#define C__WHITESPACE       " \t\r\n"

#define C_WHITESPACE        C__WHITESPACE

#define C_COMMENT_START     "#"
#define C_COMMENT           C__ALPHA_SPECIAL " \t\""
#define C_COMMENT_END       "\r\n"

#define C_IDENT_START       C__ALPHA
#define C_IDENT             C__ALPHA_LOWER C__DIGIT "_"
#define C_IDENT_END         ":"

#define C_NUMBER_START      C__SIGN C__DIGIT
#define C_NUMBER_HEX        C__DIGIT "abcdefABCDEF"
#define C_NUMBER_BINARY    "01"
#define C_NUMBER_SIGN       C__SIGN
#define C_NUMBER_INT        C__DIGIT
#define C_NUMBER_FLOAT      C__DIGIT "."
#define C_NUMBER_END        C__WHITESPACE

#define C_STRING_START      "\""
#define C_STRING            C__ALPHA_SPECIAL C_WHITESPACE
#define C_STRING_END        "\""

#define C_ARRAY_START       "["
#define C_ARRAY_END         "]"

#define C_OBJECT_START      "{"
#define C_OBJECT_END        "}"

unsigned int parse_uint(char *buf);
int parse_int(char *buf);
float parse_float_hex(char *buf);
float parse_float(char *buf);
void parse_tests();