#include "helpers.h"
#include "ctype.h"

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
        default: return "?";
    }
}

bool str_contains_chr(const char *str, char c) {
    bool found = false;
    if (str) {
        const char *d = str;
        while (*d && *d != c) {
            d++;
        }
        found = *d;
    }
    return found;
}