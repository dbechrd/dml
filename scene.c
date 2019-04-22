#include "scene.h"
#include "file.h"
#include "parse.h"
#include "symbol.h"
#include "node.h"
#include "dlb_types.h"
#include "dlb_memory.h"
#include "dlb_vector.h"
#include "dlb_hash.h"

dlb_hash type_loaders;
void init_type_loaders() {
    dlb_hash_init(&type_loaders, DLB_HASH_STRING, "Type Loaders", 16);
    //dlb_hash_insert(&type_loaders, dlb_symbol_hash(sym_entity), 0);
};

scene *scene_init(const char *name) {
    scene *scn = dlb_calloc(1, sizeof(*scn));
    scn->name = name;
    scn->next_uid = 1;
    dlb_vec_reserve(scn->entities, 2);
    return scn;
}

void scene_free(scene *scn) {
    for (entity *e = scn->entities; e != dlb_vec_end(scn->entities); e++) {
        entity_free(e);
    }
    dlb_vec_free(scn->entities);
    free(scn);
}

void scene_print(scene *scn) {
    // Print loaded entities
    printf("name: %s\n", scn->name);
    for (entity *e = scn->entities; e != dlb_vec_end(scn->entities); e++) {
        entity_print(stdout, e);
    }
    fflush(stdout);
}

void scene_save(file *f, scene *scn) {
    fprintf(f->hnd, "%s\n", scn->name);
    for (entity *e = scn->entities; e != dlb_vec_end(scn->entities); e++) {
        entity_save(f, e);
    }
}

static token *token_read(file *f, token **tokens)
{
    token *token = dlb_vec_alloc(*tokens);
    char c = file_peek(f);
    switch(c) {
        case EOF:
        {
            token->type = TOKEN_EOF;
            break;
        }
        case ' ':
        {
            file_expect_char(f, C_WHITESPACE, 1);
            token_type prev_token_type = TOKEN_UNKNOWN;
            int tokens_len = dlb_vec_len(*tokens);
            if (tokens_len > 1) {
                prev_token_type = (*tokens)[tokens_len - 2].type;
            }
            if (prev_token_type == TOKEN_NEWLINE ||
                prev_token_type == TOKEN_INDENT)
            {
                file_expect_char(f, C_WHITESPACE, 1);
                token->type = TOKEN_INDENT;
            } else {
                token->type = TOKEN_WHITESPACE;
            }
            break;
        }
        case '\n':
        {
            token->type = TOKEN_NEWLINE;
            file_expect_char(f, C_NEWLINE, 1);
            break;
        }
        case '#':
        {
            token->type = TOKEN_COMMENT;
            file_expect_char(f, C_COMMENT_START, 1);
            file_allow_char(f, C_WHITESPACE, 0);
            char buf[MAX_COMMENT_LEN + 1] = { 0 };
            int len = 0;
            file_read(f, buf, MAX_COMMENT_LEN, C_COMMENT, C_COMMENT_END, &len);
            token->length = len;
            token->value.string = intern(buf, len);
            break;
        }
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
        case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
        case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
        case 'v': case 'w': case 'x': case 'y': case 'z':
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
        case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
        case 'V': case 'W': case 'X': case 'Y': case 'Z':
        {
            DLB_ASSERT(str_contains_chr(C_IDENT_START, file_peek(f)));
            char buf[MAX_IDENT_LEN + 1] = { 0 };
            int len = 0;
            file_read(f, buf, MAX_IDENT_LEN, C_IDENT, 0, &len);
            token->length = len;
            token->value.string = intern(buf, len);
            if (file_allow_char(f, C_IDENT_END, 1)) {
                token->type = TOKEN_IDENTIFIER;
            } else if (token->value.string == sym_kw_null) {
                token->type = TOKEN_KW_NULL;
            } else if (token->value.string == sym_kw_true) {
                token->type = TOKEN_KW_TRUE;
            } else if (token->value.string == sym_kw_false) {
                token->type = TOKEN_KW_FALSE;
            }
            break;
        }
        case '+': case '-': case '0': case '1': case '2':case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        {
            DLB_ASSERT(str_contains_chr(C_NUMBER_START, file_peek(f)));
            char buf[MAX_NUMBER_LEN + 1] = { 0 };
            int len = 0;
            int read = 0;
            char next = file_peek(f);
            if (next == '0') {
                buf[len++] = file_char(f);
                next = file_peek(f);
                if (next == 'x') {
                    token->type = TOKEN_FLOAT;
                    buf[len++] = file_char(f);
                    next = file_read(f, buf + len, 8, C_NUMBER_HEX, 0, &read);
                    len += read;
                    if (file_allow_char(f, "(", 1)) {
                        file_read(f, 0, 0, 0, ")", 0);
                        file_expect_char(f, ")", 1);
                    }
                } else if (next == 'b') {
                    token->type = TOKEN_INT;
                    buf[len++] = file_char(f);
                    file_read(f, buf + len, 32, C_NUMBER_BINARY, 0, &read);
                    len += read;
                }
            }
            if (token->type == TOKEN_UNKNOWN) {
                file_read(f, buf, 1, C_NUMBER_SIGN, 0, &read);
                len += read;
                file_read(f, buf + len, MAX_NUMBER_LEN - len, C_NUMBER_INT, 0, &read);
                len += read;
                next = file_peek(f);
                if (next == '.') {
                    token->type = TOKEN_FLOAT;
                    int read = 0;
                    file_read(f, buf + len, MAX_NUMBER_LEN - len,
                        C_NUMBER_FLOAT, 0, &read);
                    len += read;
                } else {
                    token->type = TOKEN_INT;
                }
            }
            switch (token->type) {
                case TOKEN_INT: {
                    token->value.as_int = parse_int(buf);
                    break;
                } case TOKEN_FLOAT: {
                    token->value.as_float = parse_float(buf);
                    break;
                } default: {
                    DLB_ASSERT(!"Token type could not be resolved");
                }
            }
            break;
        }
        case '"':
        {
            token->type = TOKEN_STRING;
            file_expect_char(f, C_STRING_START, 1);
            char buf[MAX_STRING_LEN + 1] = { 0 };
            int len = 0;
            char delim = 0;
            int read = 0;
            do {
                delim = file_read(f, buf + len, MAX_STRING_LEN - len, C_STRING,
                    "\\" C_STRING_END, &read);
                len += read;
                if (delim == '\\') {
                    buf[len++] = file_char_escaped(f);
                }
            } while (delim == '\\');
            file_expect_char(f, C_STRING_END, 1);
            token->length = len;
            token->value.string = intern(buf, len);
            break;
        }
        case '[':
        {
            token->type = TOKEN_ARRAY_START;
            file_expect_char(f, C_ARRAY_START, 1);
            // TODO: Start buffering array elements
#if 0
            if (is_array) {
                file_expect_char(f, C_ARRAY_START, 1);
                file_allow_char(f, C_WHITESPACE, 0);
                if (array_len > 0) {
                    p->length = array_len;
                    dlb_vec_reserve(p->value.float_array, p->length);
                } else {
                    p->length = SIZE_MAX;
                    dlb_vec_alloc(p->value.int_array);
                }
                for (size_t i = 0; i < p->length; i++) {
                    p->value.float_array[i] = read_float(f, CHAR_SEPARATOR "(,");
                    file_allow_char(f, C_WHITESPACE, 0);
                    if (p->length != SIZE_MAX && i < p->length - 1) {
                        file_expect_char(f, ",", 1);
                    } else {
                        file_allow_char(f, ",", 1);
                    }
                    file_allow_char(f, C_WHITESPACE, 0);
                    if (p->length == SIZE_MAX &&
                        str_contains_chr(C_ARRAY_END, file_peek(f))) {
                        p->length = i + 1;
                        break;
                    }
                }
                file_expect_char(f, C_ARRAY_END, 1);
            }
#endif
            break;
        }
        case ']':
        {
            token->type = TOKEN_ARRAY_END;
            file_expect_char(f, C_ARRAY_END, 1);
            // TODO: Finalize array
            break;
        }
        case '{':
        {
            token->type = TOKEN_OBJECT_START;
            file_expect_char(f, C_OBJECT_START, 1);
            // TODO: Start buffering object properties
            break;
        }
        case '}':
        {
            token->type = TOKEN_OBJECT_END;
            file_expect_char(f, C_OBJECT_END, 1);
            // TODO: Finalize object
            break;
        }
        default:
        {
            PANIC_FILE(f, "I don't know what's going on.. weird tokens bro.");
        }
    }
    return token;
}

static token *scene_tokenize(file *f, bool ignore_whitespace)
{
    token *tokens = 0;
    while (token_read(f, &tokens)->type != TOKEN_EOF) {}
    return tokens;
}

static void token_print(token *tokens)
{
    for (token *tok = tokens; tok != dlb_vec_end(tokens); tok++) {
        switch (tok->type) {
            case TOKEN_EOF: {
                break;
            } case TOKEN_WHITESPACE: {
                printf(" ");
                break;
            } case TOKEN_NEWLINE: {
                printf("\n");
                break;
            } case TOKEN_INDENT: {
                printf("  ");
                break;
            }
            case TOKEN_COMMENT: case TOKEN_IDENTIFIER: case TOKEN_STRING:
            case TOKEN_KW_NULL: case TOKEN_KW_TRUE: case TOKEN_KW_FALSE:
            {
                if (tok->type == TOKEN_COMMENT) {
                    printf("#");
                } else if (tok->type == TOKEN_STRING) {
                    printf("\"");
                }

                if (tok->length && tok->value.string) {
                    printf("%.*s", tok->length, tok->value.string);
                }

                if (tok->type == TOKEN_IDENTIFIER) {
                    printf(":");
                } else if (tok->type == TOKEN_STRING) {
                    printf("\"");
                }
                break;
            } case TOKEN_INT: {
                printf("%d", tok->value.as_int);
                break;
            } case TOKEN_FLOAT: {
                printf("%f", tok->value.as_float);
                break;
            } case TOKEN_ARRAY_START: {
                printf("[");
                break;
            } case TOKEN_ARRAY_END: {
                printf("]");
                break;
            } case TOKEN_OBJECT_START: {
                printf("{");
                break;
            } case TOKEN_OBJECT_END: {
                printf("}");
                break;
            } default: {
                DLB_ASSERT(!"Unexpected token type, don't know how to print");
            }
        }
    }
    printf("\n");
}

static void token_print_debug(token *tokens)
{
    for (token *tok = tokens; tok != dlb_vec_end(tokens); tok++) {
        printf("%-16s", token_type_str(tok->type));
        switch (tok->type) {
            case TOKEN_EOF:
            {
                break;
            }
            case TOKEN_WHITESPACE:
            case TOKEN_NEWLINE:
            case TOKEN_INDENT:
            {
                break;
            }
            case TOKEN_COMMENT: case TOKEN_IDENTIFIER: case TOKEN_STRING:
            case TOKEN_KW_NULL: case TOKEN_KW_TRUE: case TOKEN_KW_FALSE:
            {
                if (tok->type == TOKEN_COMMENT) {
                    printf("#");
                } else if (tok->type == TOKEN_STRING) {
                    printf("\"");
                }

                if (tok->length && tok->value.string) {
                    printf("%.*s", tok->length, tok->value.string);
                }

                if (tok->type == TOKEN_IDENTIFIER) {
                    printf(":");
                } else if (tok->type == TOKEN_STRING) {
                    printf("\"");
                }
                break;
            } case TOKEN_INT: {
                printf("%d", tok->value.as_int);
                break;
            } case TOKEN_FLOAT: {
                printf("%f", tok->value.as_float);
                break;
            }
            case TOKEN_ARRAY_START: case TOKEN_ARRAY_END:
            case TOKEN_OBJECT_START: case TOKEN_OBJECT_END:
            {
                break;
            } default: {
                DLB_ASSERT(!"Unexpected token type, don't know how to print");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void entity_descriptor(entity_type type)
{
    switch (type) {
        case ENTITY_GENERAL: {
            break;
        } case ENTITY_TEXTURE: {
            break;
        } case ENTITY_MATERIAL: {
            break;
        } case ENTITY_MESH: {
            break;
        } default: {
            break;
        }
    }
}

typedef enum {
    PARSE_INIT,
    PARSE_ENTITY,
} parse_state;

bool token_stream_read(token_stream *stream, token **tok)
{
    if (stream->index < dlb_vec_len(stream->tokens)) {
        *tok = &stream->tokens[stream->index];
        stream->index++;
        return true;
    }
    return false;
}

token *token_stream_expect(token_stream *stream, token_type type)
{
    token *tok = 0;
    if (!token_stream_read(stream, &tok)) {
        PANIC("Stream has run out of tokens.\n");
    } else if (tok->type != type) {
        PANIC("Didn't find expected token, found %s instead.\n",
            token_type_str(tok->type));
    }
    return tok;
}

token *token_stream_expect_child(token_stream *stream, const char *name,
    int level)
{
    token_stream_expect(stream, TOKEN_NEWLINE);
    for (int i = 0; i < level; i++) {
        token_stream_expect(stream, TOKEN_INDENT);
    }
    token *tok = token_stream_expect(stream, TOKEN_IDENTIFIER);
    if (tok->value.string != name) {
        PANIC("Expected child node '%s', found '%s' instead.\n", name,
            tok->value.string);
    }
    return tok;
}

const char *token_stream_expect_child_string(token_stream *stream,
    const char *name, int level)
{
    token_stream_expect_child(stream, name, level);
    token_stream_expect(stream, TOKEN_WHITESPACE);
    token *tok = token_stream_expect(stream, TOKEN_STRING);
    return tok->value.string;
}

float token_stream_expect_child_float(token_stream *stream,
    const char *name, int level)
{
    token_stream_expect_child(stream, name, level);
    token_stream_expect(stream, TOKEN_WHITESPACE);
    token *tok = token_stream_expect(stream, TOKEN_FLOAT);
    return tok->value.as_float;
}

ta_entity *scene_parse_entity(token_stream *stream)
{
    ta_entity *entity = dlb_vec_alloc(tg_root);

    entity->name = token_stream_expect_child_string(stream, sym_name, 1);

    token_stream_expect_child(stream, sym_material, 1);
    entity->material.name = token_stream_expect_child_string(stream, sym_name, 2);

    token_stream_expect_child(stream, sym_mesh, 1);
    entity->mesh.name = token_stream_expect_child_string(stream, sym_name, 2);
    entity->mesh.path = token_stream_expect_child_string(stream, sym_path, 2);

    token_stream_expect_child(stream, sym_shader, 1);
    entity->shader.name = token_stream_expect_child_string(stream, sym_name, 2);
    entity->shader.path = token_stream_expect_child_string(stream, sym_path, 2);

    token_stream_expect_child(stream, sym_texture, 1);
    entity->texture.name = token_stream_expect_child_string(stream, sym_name, 2);
    entity->texture.path = token_stream_expect_child_string(stream, sym_path, 2);

    token_stream_expect_child(stream, sym_transform, 1);
    token_stream_expect_child(stream, sym_position, 2);
    entity->transform.position.x = token_stream_expect_child_float(stream, sym_x, 3);
    entity->transform.position.y = token_stream_expect_child_float(stream, sym_y, 3);
    entity->transform.position.z = token_stream_expect_child_float(stream, sym_z, 3);
    token_stream_expect_child(stream, sym_rotation, 2);
    entity->transform.rotation.x = token_stream_expect_child_float(stream, sym_x, 3);
    entity->transform.rotation.y = token_stream_expect_child_float(stream, sym_y, 3);
    entity->transform.rotation.z = token_stream_expect_child_float(stream, sym_z, 3);
    entity->transform.rotation.w = token_stream_expect_child_float(stream, sym_w, 3);
    token_stream_expect_child(stream, sym_scale, 2);
    entity->transform.scale.x = token_stream_expect_child_float(stream, sym_x, 3);
    entity->transform.scale.y = token_stream_expect_child_float(stream, sym_y, 3);
    entity->transform.scale.z = token_stream_expect_child_float(stream, sym_z, 3);

    return entity;
}

void scene_parse(token *tokens)
{
    parse_state state = PARSE_INIT;
    token_stream stream_ = { 0 };
    token_stream *stream = &stream_;
    stream->tokens = tokens;

    token *tok = 0;
#if 1
    for (;;) {
        tok = token_stream_expect(stream, TOKEN_IDENTIFIER);
        if (tok->value.string == sym_entity) {
            scene_parse_entity(stream);
        } else {
            DLB_ASSERT(!"Unexpected identifier"); // wtf?
        }
    }
#else
    while (token_stream_read(&stream, &tok)) {
        // TODO: Build a scene, use a state machine to track when inside of
        //       arrays, objects.
        switch (state) {
            case PARSE_INIT: {
                tok = token_stream_expect(stream, TOKEN_IDENTIFIER);
                // TODO: Register type names (e.g. entity) in lookup table that maps
                //       them to their respective loader
                if (tok->value.string == sym_entity) {
                    scene_parse_entity(stream);
                } else if (tok->value.string == sym_texture) {
                    //entity_load(f, scn, ENTITY_TEXTURE, uid);
                } else if (tok->value.string == sym_material) {
                    //entity_load(f, scn, ENTITY_MATERIAL, uid);
                } else if (tok->value.string == sym_mesh) {
                    //entity_load(f, scn, ENTITY_MESH, uid);
                } else {
                    DLB_ASSERT(!"Unexpected identifier"); // wtf?
                }
            }
        }
    }
#endif
}

scene *scene_load(file *f)
{
    scene *scn = scene_init(f->filename);
    token *tokens = scene_tokenize(f, true);
    token_print(tokens);
    token_print_debug(tokens);
    scene_parse(tokens);
    dlb_vec_free(tokens);
    return scn;
}

void entity_print(FILE *f, entity *e) {
    fprintf(f, "Entity: %d\n", e->uid);
#if 0
    const char *type_str = 0;
    switch (e->type) {
        case ENTITY_GENERAL:
            type_str = sym_entity;
            break;
        case ENTITY_TEXTURE:
            type_str = sym_texture;
            break;
        case ENTITY_MATERIAL:
            type_str = sym_material;
            break;
        case ENTITY_MESH:
            type_str = sym_mesh;
            break;
        default:
            DLB_ASSERT("Unknown entity type");
    }
    fprintf(f, "!%s:%d\n", type_str, e->uid);
    for (prop *prop = e->properties; prop != dlb_vec_end(e->properties); prop++) {
        fprintf(f, "  %s: ", prop->name);
        if (prop->type_alias == NULL) {
            fprintf(f, "%s", prop_type_str(prop->type));
            if (prop->length > 0 && prop->type != PROP_STRING) {
                fprintf(f, CHAR_ARRAY_LEN_START "%d" CHAR_ARRAY_LEN_END, (int)prop->length);
            }
        } else {
            fprintf(f, "%s", prop->type_alias);
        }
        fprintf(f, " = ");
        switch (prop->type) {
            case PROP_INT:
                if (prop->length == 0) {
                    fprintf(f, "%d", prop->value.as_int);
                } else {
                    fprintf(f, C_ARRAY_START "\n    ");
                    for (size_t i = 0; i < prop->length; i++) {
                        fprintf(f, "%d", prop->value.int_array[i]);
                        if (i == prop->length - 1) {
                            fprintf(f, "\n");
                        } else if ((i + 1) % 8) {
                            fprintf(f, ", ");
                        } else {
                            fprintf(f, ",\n    ");
                        }
                    }
                    fprintf(f, "  " C_ARRAY_END);
                }
                break;
            case PROP_FLOAT:
                if (prop->length == 0) {
                    fprintf(f, prop->value.as_float == 0 ? "0" : "0x%08x(%g)",
                        *(unsigned *)&prop->value.as_float,
                        prop->value.as_float);
                } else {
                    fprintf(f, C_ARRAY_START "\n    ");
                    for (size_t i = 0; i < prop->length; i++) {
                        fprintf(f, prop->value.float_array[i] == 0 ? "0" : "0x%08x(%g)",
                            *(unsigned *)&prop->value.float_array[i],
                            prop->value.float_array[i]);
                        if (i == prop->length - 1) {
                            fprintf(f, "\n");
                        } else {
                            fprintf(f, ",\n    ");
                        }
                    }
                    fprintf(f, "  " C_ARRAY_END);
                }
                break;
            case PROP_CHAR:
                if (prop->length == 0) {
                    fprintf(f, "'%s'", char_printable(&prop->value.as_char));
                } else {
                    fprintf(f, C_ARRAY_START "\n    ");
                    for (size_t i = 0; i < prop->length; i++) {
                        fprintf(f, "'%s'", char_printable(&prop->value.char_array[i]));
                        if (i == prop->length - 1) {
                            fprintf(f, "\n");
                        } else if ((i + 1) % 8) {
                            fprintf(f, ", ");
                        } else {
                            fprintf(f, ",\n    ");
                        }
                    }
                    fprintf(f, "  " C_ARRAY_END);
                }
                break;
            case PROP_STRING:
                fprintf(f, "\"%.*s\"", (int)prop->length, prop->value.string);
                break;
            default:
                DLB_ASSERT(0);
        }
        fprintf(f, "\n");
    }
#endif
}

void entity_save(file *f, entity *e) {
    entity_print(f->hnd, e);
}

entity *entity_init(scene *scn, entity_type type, unsigned int uid) {
    if (uid) {
        DLB_ASSERT(uid >= scn->next_uid);
        scn->next_uid = uid;
    }
    entity *e = dlb_vec_alloc(scn->entities);
    e->type = type;
    e->uid = scn->next_uid++;
    return e;
}

void entity_load(file *f, scene *scn, entity_type type, unsigned int uid)
{
    entity *e = entity_init(scn, type, uid);

    /*token_type token = 0;
    do {
        token = token_infer(f);
        printf("%s\n", token_type_str(token));
    } while(token != TOKEN_EOF);*/
}

void entity_free(entity *e) {
    for (prop *prop = e->properties; prop != dlb_vec_end(e->properties); prop++) {
        if (prop->length > 0 && prop->type != PROP_STRING) {
            dlb_vec_free(prop->value.buffer);
        }
    }
    dlb_vec_free(e->properties);
}

prop *prop_find(entity *e, const char *name) {
    bool found = false;
    prop *p = e->properties;
    while (p != dlb_vec_end(e->properties)) {
        if (p->name == name) {
            found = true;
            break;
        }
        p++;
    }
    return found ? p : NULL;
}

prop *prop_create(entity *e, const char *name) {
    prop *p = prop_find(e, name);
    if (p != NULL) {
        return NULL;  // Prevent duplicates
    }

    p = dlb_vec_alloc(e->properties);
    p->name = name;
    return p;
}

void prop_set_int(entity *e, const char *name, int value) {
    prop_clear(e, name);
    prop *p = prop_create(e, name);
    p->type = PROP_INT;
    p->value.as_int = value;
}

void prop_set_float(entity *e, const char *name, float value) {
    prop_clear(e, name);
    prop *p = prop_create(e, name);
    p->type = PROP_FLOAT;
    p->value.as_float = value;
}

void prop_set_char(entity *e, const char *name, char value) {
    prop_clear(e, name);
    prop *p = prop_create(e, name);
    p->type = PROP_CHAR;
    p->value.as_char = value;
}

void prop_set_string(entity *e, const char *name, const char *value) {
    prop_clear(e, name);
    prop *p = prop_create(e, name);
    p->type = PROP_STRING;
    p->length = strlen(value);
    DLB_ASSERT(p->length <= MAX_STRING_LEN);
    p->value.string = value;
}

void prop_clear(entity *e, const char *name) {
    prop *p = prop_find(e, name);
    if (p) {
        // NOTE: Don't free interned strings because they might be shared
        if (p->length > 0 && p->type != PROP_STRING) {
            free(p->value.buffer);
        }
        memset(p, 0, sizeof(*p));
        p->name = "[PROPERTY CLEARED]";
    }
}