#pragma once
#include "dlb_memory.h"
#include "dlb_hash.h"
#include <string.h>
#include <stdint.h>

#define CSTR(s) (s), sizeof(s) - 1

extern const char *sym_entity;
extern const char *sym_texture;
extern const char *sym_material;
extern const char *sym_mesh;

extern const char *sym_int;
extern const char *sym_float;
extern const char *sym_char;
extern const char *sym_string;
extern const char *sym_vec3;
extern const char *sym_vec4;

const char *intern(const char *s, size_t len);
void init_symbols();

typedef struct dlb_symbol__hdr {
    size_t len;
} dlb_symbol__hdr;

#define dlb_symbol__hdr(s) ((dlb_symbol__hdr *)((char *)s - sizeof(dlb_symbol__hdr)))

#define dlb_symbol_len(s) ((s) ? dlb_symbol__hdr(s)->len : 0)
//#define dlb_symbol_end(s) ((s) + dlb_symbol_len(s))
//#define dlb_symbol_last(s) (&(s)[dlb_symbol__hdr(s)->len-1])
//#define dlb_symbol_cstr(s) (dlb_symbol__alloc((s), sizeof(s)))
#define dlb_symbol_alloc(s, len) (dlb_symbol__alloc((s), (len)))
#define dlb_symbol_free(s) ((s) ? (free(dlb_symbol__hdr(s)), (s) = NULL) : 0)

static inline char *dlb_symbol__alloc(const char *buf, size_t len) {
    size_t new_size = sizeof(dlb_symbol__hdr) + len + 1;
    dlb_symbol__hdr *sym = dlb_calloc(1, new_size);
    sym->len = len;
    char *str = (char *)sym + sizeof(dlb_symbol__hdr);
    memcpy(str, buf, len);
    str[len] = 0;
    return str;
}