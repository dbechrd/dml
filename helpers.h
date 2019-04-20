#pragma once
#include "ctype.h"

#define PANIC(format, ...) \
    fprintf(stderr, (format), __VA_ARGS__); \
    getchar(); \
    exit(1);

#define PANIC_FILE(f, format, ...) \
    fprintf(stderr, "%s:%d:%d ", f->filename, f->pos.line, f->pos.column); \
    fprintf(stderr, (format), __VA_ARGS__); \
    getchar(); \
    exit(1);

static inline char str_find_char(const char *str, char c) {
    char found = 0;
    if (str) {
        const char *d = str;
        while (*d && *d != c) {
            d++;
        }
        found = *d;
    }
    return found;
}

const char *char_printable(const char *c) {
    // HACK: Static buffer, don't hold pointers to this
    static char buf[2] = { 0 };
    if (isprint(*c)) {
        buf[0] = *c;
        return buf;
    }

    switch (*c) {
        case '\t': return "\\t";
        case '\r': return "\\r";
        case '\n': return "\\n";
        case '\0': return "\\0";
        default:
            return "?";
    }
}