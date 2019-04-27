#include "symbol.h"
#include "parse.h"
#include "dlb_types.h"
#include "dlb_arena.h"
#include "dlb_hash.h"
#include <string.h>

#define SYM_MAX_LEN 256

// TODO: It may be useful to have multiple symbol tables to allow freeing
//       symbols that are no longer in use (e.g. table per scene file). This
//       hasn't been necessary yet, so I'm not going to do it preemptively.
static dlb_hash symbol_table;

const char *sym_ident_name;
const char *sym_kw_null;
const char *sym_kw_true;
const char *sym_kw_false;

const char *intern(const char *s, u32 len) {
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
    sym_ident_name  = intern(CSTR(IDENT_NAME));
    sym_kw_null     = intern(CSTR(KEYWORD_NULL));
    sym_kw_true     = intern(CSTR(KEYWORD_TRUE));
    sym_kw_false    = intern(CSTR(KEYWORD_FALSE));
}