#include "parse.h"
#include "file.h"
#include "symbol.h"
#include "dlb_types.h"

void eat_chars(char *buf, size_t buf_len, size_t *len, file *f, char delim) {
    for (size_t i = 0; i < buf_len; i++) {
        char c = file_getc(f);
        if (c == delim) {
            if (len) *len = i;
            return;
        }
        buf[i] = c;
    }
    DLB_ASSERT(0); // TODO: Eat extra input until delim found?
}

uint32_t parse_int(file *f, char delim) {
    char buf[INT_MAX_LEN + 1] = { 0 };
    eat_chars(buf, INT_MAX_LEN, 0, f, delim);
    uint32_t value = (uint32_t)atoi(buf);
    return value;
}

float parse_float(file *f, char delim) {
    char buf[FLOAT_MAX_LEN + 1] = { 0 };
    eat_chars(buf, FLOAT_MAX_LEN, 0, f, delim);
    float value = (float)atof(buf);
    return value;
}

const char *parse_string(file *f, char delim) {
    char buf[STRING_MAX_LEN + 1] = { 0 };
    size_t len;
    eat_chars(buf, STRING_MAX_LEN, &len, f, delim);
    return sym(buf, len);
}