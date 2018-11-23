#include "symbol.h"
#include "dlb_types.h"
#include "dlb_arena.h"
#include "dlb_hash.h"
#include <string.h>

#define SYM_MAX_LEN 256

static struct dlb_hash symbol_table;

const char *sym_entity;
const char *sym_int;
const char *sym_float;
const char *sym_string;

const char *intern(const char *s, size_t len) {
    DLB_ASSERT(len < SYM_MAX_LEN);

    char *sym = dlb_hash_search(&symbol_table, s, len);
    if (sym) return sym;

    sym = dlb_symbol_alloc(s, len);
    dlb_hash_insert(&symbol_table, s, len, sym);
    return sym;
}

void init_symbols() {
    dlb_hash_init(&symbol_table, "[symbol_table]", 20, 3);
    sym_entity = intern(CSTR("entity"));
    sym_int = intern(CSTR("int32"));
    sym_float  = intern(CSTR("float32"));
    sym_string = intern(CSTR("string"));
}