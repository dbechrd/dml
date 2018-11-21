#pragma once
#include "dlb_hash.h"
#include <string.h>

#define CSTR(s) (s), sizeof(s) - 1
#define SYM(s) sym(CSTR(s))

typedef struct symbol {
    uint32_t hash;
    size_t len;
    const char *str;
} symbol;

extern const char *sym_entity;
extern const char *sym_int;
extern const char *sym_float;
extern const char *sym_string;

const char *sym(const char *s, size_t len);
void sym_init();

static inline uint32_t strhash(const char *s) {
    uint32_t hash;
    MurmurHash3_x86_32(s, sizeof(s) - 1, (void *)&hash);
    return hash;
}

static inline bool streq(const char *a, const char *b, size_t len) {
    return (a == b || strncmp(a, b, len) == 0);
}