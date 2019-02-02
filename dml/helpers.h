#pragma once

#define PANIC(format, ...) \
    fprintf(stderr, (format), __VA_ARGS__); \
    exit(1);

#define PANIC_FILE(f, format, ...) \
    fprintf(stderr, "%s:%d:%d ", f->filename, f->line_number, f->line_column); \
    fprintf(stderr, (format), __VA_ARGS__); \
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