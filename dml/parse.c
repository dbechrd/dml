#include "parse.h"
#include "file.h"
#include "symbol.h"
#include "helpers.h"
#include "dlb_types.h"

char eat_chars(char *buf, size_t buf_len, size_t *len, file *f,
               const char *delims, const char *valid_chars) {
    DLB_ASSERT(valid_chars);
    size_t f_lineno = f->line_number;
    size_t f_column = f->line_column + 1;

    char delim = 0;
    size_t i;
    for (i = 0; i < buf_len && f->last >= 0; i++) {
        char c = file_char(f);

        // Check for delimiter
        delim = str_find_char(delims, c);
        if (delim) {
            f->replay = true;
            break;
        }

        // Validate character
        char valid = str_find_char(valid_chars, c);
        if (valid_chars && !valid) {
            fprintf(stderr, "%s:%d:%d [PARSE_ERROR] Unexpected character '%c' in expression starting at %d:%d. Expected [%s] or delimeter '[%s]'.\n",
                    f->filename, f->line_number, f->line_column, c, f_lineno, f_column, valid_chars, delims);
            getchar();
            exit(1);
        }

        // Discard end-of-line whitespace and comments
        //if (delim == '\n' && (c == ' ' || c == '#')) {
        //    if (len) *len = i;
        //    while (c != '\n') {
        //        c = file_char(f);
        //    }
        //    return;
        //}

        buf[i] = c;
    }

    if (delims && !delim) {
        fprintf(stderr, "%s:%d:%d [PARSE_ERROR] Expected delim [%s] to end expression starting at %d:%d\n",
                f->filename, f->line_number, f->line_column, delims, f_lineno, f_column);
        getchar();
        exit(1);
    }

    if (len) *len = i;
    return delim;
}

unsigned int read_uint(file *f, const char *delim) {
    char buf[INT_MAX_LEN + 1] = { 0 };
    eat_chars(buf, INT_MAX_LEN, 0, f, delim, CHAR_DIGIT);
    unsigned int value = parse_uint(buf);
    return value;
}

unsigned int parse_uint(char *buf) {
    unsigned long value = strtoul(buf, 0, 10);
    DLB_ASSERT(value < ULONG_MAX);
    return (unsigned int)value;
}

int read_int(file *f, const char *delims) {
    char buf[INT_MAX_LEN + 1] = { 0 };
    eat_chars(buf, INT_MAX_LEN, 0, f, delims, CHAR_DIGIT);
    int value = parse_int(buf);
    return value;
}

int parse_int(char *buf) {
    long value = strtol(buf, 0, 10);
    DLB_ASSERT(value > 0);
    DLB_ASSERT(value < LONG_MAX);
    return (int)value;
}

float parse_float_hex(char *buf) {
    float value;
    unsigned l = strtoul(buf, 0, 16);
    DLB_ASSERT(l < ULONG_MAX);
    value = *(float *)&l;
    return value;
}

float read_float(file *f, const char *delims) {
    char buf[FLOAT_MAX_LEN + 1] = { 0 };
    char sep = eat_chars(buf, FLOAT_MAX_LEN, 0, f, delims, CHAR_FLOAT);
    float value = parse_float(buf);

    if (sep == ':') {
        file_expect_char(f, ":", 1);
        file_allow_char(f, "0", 1);
        file_allow_char(f, "x", 1);
        char hex_buf[FLOAT_HEX_LEN + 1] = { 0 };
        eat_chars(hex_buf, FLOAT_HEX_LEN, 0, f, 0, CHAR_HEX);

        float hex_value = parse_float_hex(hex_buf);
        float delta = hex_value - value;
        if (delta < FLOAT_HEX_MAX_DELTA) {
            value = hex_value;
        }
    }

    return value;
}

float parse_float(char *buf) {
    float value;
    if (buf[0] == '0' && buf[1] == 'x') {
        value = parse_float_hex(buf);
    } else {
        value = strtof(buf, 0);
    }
    return value;
}

const char *read_string(file *f, const char *delims, const char *valid_chars) {
    char buf[STRING_MAX_LEN + 1] = { 0 };
    size_t len;
    eat_chars(buf, STRING_MAX_LEN, &len, f, delims, valid_chars);
    return intern(buf, len);
}