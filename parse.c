#include "parse.h"
#include "file.h"
#include "symbol.h"
#include "helpers.h"
#include "dlb_types.h"

unsigned int parse_uint(char *buf)
{
    unsigned long value = strtoul(buf, 0, 10);
    return (unsigned int)value;
}

int parse_int_binary(char *buf)
{
    DLB_ASSERT(!"[PARSE_ERROR] Binary integers not yet supported.");
    return 0;
}

int parse_int(char *buf)
{
    long value;
    if (buf[0] == '0' && buf[1] == 'b') {
        value = parse_int_binary(buf);
    } else {
        value = strtol(buf, 0, 10);
    }
    return (int)value;
}

float parse_float_hex(char *buf)
{
    float value;
    unsigned long l = strtoul(buf, 0, 16);
    value = *(float *)&l;
    return value;
}

float parse_float(char *buf)
{
    float value;
    if (buf[0] == '0' && buf[1] == 'x') {
        value = parse_float_hex(buf);
    } else {
        value = strtof(buf, 0);
    }
    return value;
}

void parse_tests()
{
    float a = 123.0f;
    float b = parse_float("123.0f");
    float c = parse_float("0x42f60000");
    float d = parse_float("0x42f60000(123)");
    DLB_ASSERT(b == a);
    DLB_ASSERT(c == a);
    DLB_ASSERT(d == a);
}