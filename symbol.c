#include "symbol.h"
#include "dlb_types.h"
#include "dlb_arena.h"
#include "dlb_hash.h"
#include <string.h>

#define SYM_MAX_LEN 256

// TODO: It may be useful to have multiple symbol tables to allow freeing
//       symbols that are no longer in use (e.g. table per scene file). This
//       hasn't been necessary yet, so I'm not going to do it preemptively.
static dlb_hash symbol_table;

const char *sym_entity;
const char *sym_texture;
const char *sym_material;
const char *sym_mesh;

const char *sym_int;
const char *sym_float;
const char *sym_char;
const char *sym_string;
const char *sym_vec3;
const char *sym_vec4;

const char *intern(const char *s, size_t len) {
    DLB_ASSERT(len);
    DLB_ASSERT(len < SYM_MAX_LEN);

    char *sym = dlb_hash_search(&symbol_table, s, len);
    if (sym) return sym;

    sym = dlb_symbol_alloc(s, len);
    dlb_hash_insert(&symbol_table, sym, len, sym);
    return sym;
}

void init_symbols() {
    dlb_hash_init(&symbol_table, DLB_HASH_STRING, "[symbol_table]", 128);

    sym_entity   = intern(CSTR("entity"));
    sym_texture  = intern(CSTR("texture"));
    sym_material = intern(CSTR("material"));
    sym_mesh     = intern(CSTR("mesh"));

    sym_int      = intern(CSTR("int32"));
    sym_float    = intern(CSTR("float32"));
    sym_char     = intern(CSTR("char"));
    sym_string   = intern(CSTR("string"));
    sym_vec3     = intern(CSTR("vec3"));
    sym_vec4     = intern(CSTR("vec4"));
}