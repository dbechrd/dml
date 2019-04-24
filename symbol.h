#pragma once
#include "dlb_memory.h"
#include "dlb_hash.h"
#include <string.h>
#include <stdint.h>

#define CSTR(s) (s), sizeof(s) - 1
#define INTERN(s) intern(CSTR(s))

#if 0
extern const char *sym_entity;
extern const char *sym_name;
extern const char *sym_material;
extern const char *sym_mesh;
extern const char *sym_shader;
extern const char *sym_texture;
extern const char *sym_transform;
extern const char *sym_position;
extern const char *sym_rotation;
extern const char *sym_scale;
extern const char *sym_x;
extern const char *sym_y;
extern const char *sym_z;
extern const char *sym_w;
extern const char *sym_path;
#endif

extern const char *sym_kw_null;
extern const char *sym_kw_true;
extern const char *sym_kw_false;

#if 0
extern const char *sym_int;
extern const char *sym_float;
extern const char *sym_char;
extern const char *sym_string;
extern const char *sym_vec3;
extern const char *sym_vec4;
#endif

const char *intern(const char *s, u32 len);
void init_symbols();

typedef struct dlb_symbol__hdr {
    u32 len;
} dlb_symbol__hdr;

#define dlb_symbol__hdr(s) ((dlb_symbol__hdr *)((char *)s - sizeof(dlb_symbol__hdr)))

#define dlb_symbol_len(s) ((s) ? dlb_symbol__hdr(s)->len : 0)
//#define dlb_symbol_end(s) ((s) + dlb_symbol_len(s))
//#define dlb_symbol_last(s) (&(s)[dlb_symbol__hdr(s)->len-1])
//#define dlb_symbol_cstr(s) (dlb_symbol__alloc((s), sizeof(s)))
#define dlb_symbol_alloc(s, len) (dlb_symbol__alloc((s), (len)))
#define dlb_symbol_free(s) ((s) ? (free(dlb_symbol__hdr(s)), (s) = NULL) : 0)

static inline char *dlb_symbol__alloc(const char *buf, u32 len) {
    u32 new_size = sizeof(dlb_symbol__hdr) + len + 1;
    dlb_symbol__hdr *sym = dlb_malloc(new_size);
    sym->len = len;
    char *str = (char *)sym + sizeof(dlb_symbol__hdr);
    memcpy(str, buf, len);
    str[len] = 0;
    return str;
}