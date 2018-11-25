#include "parse.h"
#include "file.h"
#include "symbol.h"
#include "dlb_types.h"

void eat_chars(char *buf, size_t buf_len, size_t *len, file *f, char delim,
               const char *valid_chars) {
    size_t f_lineno = f->line_number;
    size_t f_column = f->line_column;

    if (len) *len = 0;
    for (size_t i = 0; i < buf_len && f->last >= 0; i++) {
        char c = file_getc(f);
        // Check for delimiter
        if (c == delim) {
            if (len) *len = i;
            return;
        // Discard end-of-line whitespace and comments
        } else if (delim == '\n' && (c == ' ' || c == '#')) {
            if (len) *len = i;
            while (c != '\n') {
                c = file_getc(f);
            }
            return;
        }
        // Validate character
        if (valid_chars) {
            const char *vc = valid_chars;
            while (*vc && *vc != c) { vc++; }
            if (!*vc) {
                fprintf(stderr, "%s:%d:%d [PARSE_ERROR] Unexpected character '%c' in expression starting on line %d, column %d. Expected [%s] or delimeter '%c'.\n",
                        f->filename, f->line_number, f->line_column, c, f_lineno, f_column, valid_chars, delim);
                getchar();
                exit(1);
            }
        }
        buf[i] = c;
    }

    fprintf(stderr, "%s:%d:%d [PARSE_ERROR] Expected delim '%c' to end expression starting on line %d, column %d\n",
            f->filename, f->line_number, f->line_column, delim, f_lineno, f_column);
    getchar();
    exit(1);
}

unsigned int read_uint(file *f, char delim) {
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

int read_int(file *f, char delim) {
    char buf[INT_MAX_LEN + 1] = { 0 };
    eat_chars(buf, INT_MAX_LEN, 0, f, delim, CHAR_DIGIT);
    int value = parse_int(buf);
    return value;
}

int parse_int(char *buf) {
    long value = strtol(buf, 0, 10);
    DLB_ASSERT(value > 0);
    DLB_ASSERT(value < LONG_MAX);
    return (int)value;
}

float read_float(file *f, char delim) {
    char buf[FLOAT_MAX_LEN + 1] = { 0 };
    eat_chars(buf, FLOAT_MAX_LEN, 0, f, delim, CHAR_FLOAT);
    float value = parse_float(buf);
    return value;
}

float parse_float(char *buf) {
    float value;
    if (buf[0] == '0' && buf[1] == 'x') {
        unsigned l = strtoul(buf, 0, 16);
        DLB_ASSERT(l < ULONG_MAX);
        value = *(float *)&l;
    } else {
        value = strtof(buf, 0);
    }
    return value;
}

const char *read_string(file *f, char delim, const char *valid_chars) {
    char buf[STRING_MAX_LEN + 1] = { 0 };
    size_t len;
    eat_chars(buf, STRING_MAX_LEN, &len, f, delim, valid_chars);
    return intern(buf, len);
}