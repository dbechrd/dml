#include "symbol.h"
#include "dlb_types.h"
#include "dlb_arena.h"
#include "dlb_hash.h"
#include <string.h>

#define SYM_MAX_LEN 256

// TODO: Use hash table instead of vector. Look up into hash table to find
//       previously interned symbol; if none exists, add to hash table.
static const char **symbols;

const char *sym_entity;
const char *sym_int;
const char *sym_float;
const char *sym_string;

const char *intern(const char *s, size_t len) {
    DLB_ASSERT(len < SYM_MAX_LEN);

    uint32_t hash = hash_string(s, len);
    for (const char **iter = symbols; iter != dlb_vec_end(symbols); iter++) {
        const char *sym = *iter;
        if (sym == s) return sym;

        // TODO: Cleanup test code
        dlb_symbol__hdr *hdr = dlb_symbol__hdr(sym);
        uint32_t sym_hash = dlb_symbol_hash(sym);
        uint32_t sym_len = dlb_symbol_len(sym);
        if (sym_hash == hash &&
            sym_len == len &&
            strncmp(sym, s, len) == 0) {
            return sym;
        }
    }

    const char *sym = dlb_symbol_alloc(s, len);
    dlb_vec_push(symbols, sym);
    return sym;
}

void init_symbols() {
    sym_entity = intern(CSTR("entity"));
    sym_int = intern(CSTR("int32"));
    sym_float  = intern(CSTR("float32"));
    sym_string = intern(CSTR("string"));
}