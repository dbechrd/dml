#pragma once

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