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

const char *sym_entity;

const char *sym_name;
const char *sym_material;
const char *sym_mesh;
const char *sym_shader;
const char *sym_texture;
const char *sym_transform;
const char *sym_position;
const char *sym_rotation;
const char *sym_scale;
const char *sym_x;
const char *sym_y;
const char *sym_z;
const char *sym_w;
const char *sym_path;

const char *sym_kw_null;
const char *sym_kw_true;
const char *sym_kw_false;

#if 0
const char *sym_int;
const char *sym_float;
const char *sym_char;
const char *sym_string;
const char *sym_vec3;
const char *sym_vec4;
#endif

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

    sym_kw_null     = intern(CSTR(KEYWORD_NULL));
    sym_kw_true     = intern(CSTR(KEYWORD_TRUE));
    sym_kw_false    = intern(CSTR(KEYWORD_FALSE));

    sym_transform   = intern(CSTR("ta_transform"));
    sym_texture     = intern(CSTR("ta_texture"));
    sym_material    = intern(CSTR("ta_material"));
    sym_mesh        = intern(CSTR("ta_mesh"));
    sym_shader      = intern(CSTR("ta_shader"));
    sym_entity      = intern(CSTR("ta_entity"));

    sym_name        = intern(CSTR("name"));
    sym_material    = intern(CSTR("material"));
    sym_mesh        = intern(CSTR("mesh"));
    sym_shader      = intern(CSTR("shader"));
    sym_texture     = intern(CSTR("texture"));
    sym_transform   = intern(CSTR("transform"));
    sym_position    = intern(CSTR("position"));
    sym_rotation    = intern(CSTR("rotation"));
    sym_scale       = intern(CSTR("scale"));
    sym_x           = intern(CSTR("x"));
    sym_y           = intern(CSTR("y"));
    sym_z           = intern(CSTR("z"));
    sym_w           = intern(CSTR("w"));
    sym_path        = intern(CSTR("path"));

#if 0
    sym_int         = intern(CSTR("int32"));
    sym_float       = intern(CSTR("float32"));
    sym_char        = intern(CSTR("char"));
    sym_string      = intern(CSTR("string"));
    sym_vec3        = intern(CSTR("vec3"));
    sym_vec4        = intern(CSTR("vec4"));
#endif
}