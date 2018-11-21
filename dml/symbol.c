#include "symbol.h"
#include "dlb_types.h"
#include "dlb_arena.h"

#define SYM_MAX_LEN 256

static dlb_arena string_arena;
static symbol *symbols;

// TODO: Use a table, lookup by symbol_type
const char *sym_entity;
const char *sym_int;
const char *sym_float;
const char *sym_string;

const char *sym(const char *s, size_t len) {
    DLB_ASSERT(len < SYM_MAX_LEN);
    uint32_t hash = strhash(s);
    for (symbol *i = symbols; i != dlb_vec_end(symbols); i++) {
        if (i->hash == hash && streq(i->str, s, len)) {
            return i->str;
        }
    }

    char *str = dlb_arena_alloc(&string_arena, len + 1);
    memcpy(str, s, len);
    str[len] = 0;

    symbol *sym = dlb_vec_alloc(symbols);
    sym->hash = hash;
    sym->len = len;
    sym->str = str;
    return str;
}

void sym_init() {
    sym_entity = SYM("entity");
    sym_int = SYM("int32");
    sym_float  = SYM("float32");
    sym_string = SYM("string");
}