#include "scene.h"
#include "parse.h"
#include "symbol.h"
#include "object.h"
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
    for (ta_entity *e = scn->entities; e != dlb_vec_end(scn->entities); e++) {
        entity_free(e);
    }
    dlb_vec_free(scn->entities);
    free(scn);
}

void scene_print(scene *scn) {
    // Print loaded entities
    printf("Scene name: %s\n", scn->name);
    printf("Entities:\n");
    for (ta_entity *e = scn->entities; e != dlb_vec_end(scn->entities); e++) {
        entity_print(stdout, e);
    }
    // TODO: Print materials, textures, etc.
    fflush(stdout);
}

void scene_save(file *f, scene *scn) {
    fprintf(f->hnd, "%s\n", scn->name);
    for (ta_entity *e = scn->entities; e != dlb_vec_end(scn->entities); e++) {
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

bool token_stream_peek(token_stream *stream, token **tok)
{
    if (stream->index < dlb_vec_len(stream->tokens)) {
        *tok = &stream->tokens[stream->index];
        return true;
    }
    return false;
}

bool token_stream_next(token_stream *stream, token **tok)
{
    if (stream->index < dlb_vec_len(stream->tokens)) {
        *tok = &stream->tokens[stream->index];
        stream->index++;
        return true;
    }
    return false;
}

token *token_stream_read(token_stream *stream)
{
    token *tok = 0;
    if (stream->index < dlb_vec_len(stream->tokens)) {
        tok = &stream->tokens[stream->index];
        stream->index++;
    }
    return tok;
}

token *token_stream_expect(token_stream *stream, token_type type)
{
    token *tok = token_stream_read(stream);
    if (!tok) {
        PANIC("Stream has run out of tokens.\n");
    } else if (tok->type != type) {
        PANIC("Expected token %s, found %s instead.\n",
            token_type_str(type), token_type_str(tok->type));
    }
    return tok;
}

void token_stream_indent(token_stream *stream)
{
    token_stream_expect(stream, TOKEN_NEWLINE);
    stream->level++;
}

void token_stream_unindent(token_stream *stream)
{
    DLB_ASSERT(stream->level > 0);
    stream->level--;
}

token *token_stream_identifier(token_stream *stream, const char *name)
{
    for (int i = 0; i < stream->level; i++) {
        token_stream_expect(stream, TOKEN_INDENT);
    }
    token *tok = token_stream_expect(stream, TOKEN_IDENTIFIER);
    if (tok->value.string != name) {
        PANIC("Expected child node '%s', found '%s' instead.\n", name,
            tok->value.string);
    }
    return tok;
}

const char *token_stream_string(token_stream *stream,
    const char *name)
{
    token_stream_identifier(stream, name);
    token_stream_expect(stream, TOKEN_WHITESPACE);
    token *tok = token_stream_expect(stream, TOKEN_STRING);
    token_stream_expect(stream, TOKEN_NEWLINE);
    return tok->value.string;
}

float token_stream_float(token_stream *stream,
    const char *name)
{
    token_stream_identifier(stream, name);
    token_stream_expect(stream, TOKEN_WHITESPACE);
    token *tok = token_stream_expect(stream, TOKEN_FLOAT);
    token_stream_expect(stream, TOKEN_NEWLINE);
    return tok->value.as_float;
}

typedef struct {
    ta_object_type type;
    void *ptr;
    int indent;
} obj_ptr;

#if 0
ta_entity *scene_parse_entity(scene *scn, token_stream *stream)
{
    ta_entity *entity = dlb_vec_alloc(scn->entities);
    entity->obj_type = OBJ_TA_ENTITY;
    //entity_init(entity);

    // TODO: Find properties by name and set them this way
    //ta_mesh *mesh = obj_field_find(entity->object, INTERN("mesh"));

    // Tokens on current line
    token *tok = 0;

    // Objects in current level
    obj_ptr stack[16] = { 0 };
    int level = 0;

    // Current line indent counter
    int indent = 0;

    // TODO: Refactor this into the loop, it's the same as everything else
    // Entity header
    //token_stream_identifier(stream, INTERN(STRING(ta_entity)));
    //token_stream_expect(stream, TOKEN_NEWLINE);
    //stack[level].type = entity->obj_type;
    //stack[level].ptr = entity;
    //stack[level].indent = indent;

    // Entity properties
    while (token_stream_next(stream, &tok)) {
        switch (tok->type) {
            case TOKEN_IDENTIFIER: {
                for (int i = level; i >= 0; i--) {
                    if (indent >= stack[i].indent) {
                        break;
                    }
                    level--;
                }
                stack[level].indent = indent;

                ta_object_type type = OBJ_NULL;
                void *ptr = 0;
                if (level) {
                    ta_object_field *field = obj_field_find(stack[level-1].type, tok->value.string);
                    type = field->type;
                    ptr = ((u8 *)stack[level-1].ptr + field->offset);
                } else {
                    ta_object *obj = dlb_hash_search(&tg_objects_by_name, tok->value.string, tok->length);
                    DLB_ASSERT(obj);
                    DLB_ASSERT(obj->type);
                    type = obj->type;
                    ptr = scene_obj_init(scn, type, 0);
                }
                ta_object *obj = &tg_objects[type];
                stack[level].type = obj->type;
                stack[level].ptr = ptr;
                level++;
                break;
            } case TOKEN_NEWLINE: {
                indent = 0;
                tok = 0;
                break;
            } case TOKEN_INDENT: {
                indent++;
                break;
            } case TOKEN_WHITESPACE: {
                break;
            } case TOKEN_STRING: {
                DLB_ASSERT(level);
                DLB_ASSERT(stack[level-1].type == OBJ_STRING);
                const char **fp = stack[level-1].ptr;
                *fp = tok->value.string;
                break;
            } case TOKEN_FLOAT: {
                DLB_ASSERT(level);
                DLB_ASSERT(stack[level-1].type == OBJ_FLOAT);
                float *fp = stack[level-1].ptr;
                *fp = tok->value.as_float;
                break;
            } default: {
                DLB_ASSERT(!"Unhandled token type");
            }
        }
        tok++;
    }

#if 0
    token_stream_identifier(stream, sym_entity);

    token_stream_indent(stream);
    {
        entity->name = token_stream_string(stream, sym_name);

        token_stream_identifier(stream, sym_material);
        token_stream_indent(stream);
        {
            entity->material.name = token_stream_string(stream, sym_name);
        }
        token_stream_unindent(stream);
        token_stream_identifier(stream, sym_mesh);
        token_stream_indent(stream);
        {
            entity->mesh.name = token_stream_string(stream, sym_name);
            entity->mesh.path = token_stream_string(stream, sym_path);
        }
        token_stream_unindent(stream);
        token_stream_identifier(stream, sym_shader);
        token_stream_indent(stream);
        {
            entity->shader.name = token_stream_string(stream, sym_name);
            entity->shader.path = token_stream_string(stream, sym_path);
        }
        token_stream_unindent(stream);
        token_stream_identifier(stream, sym_texture);
        token_stream_indent(stream);
        {
            entity->texture.name = token_stream_string(stream, sym_name);
            entity->texture.path = token_stream_string(stream, sym_path);
        }
        token_stream_unindent(stream);
        token_stream_identifier(stream, sym_transform);
        token_stream_indent(stream);
        {
            token_stream_identifier(stream, sym_position);
            token_stream_indent(stream);
            {
                entity->transform.position.x = token_stream_float(stream, sym_x);
                entity->transform.position.y = token_stream_float(stream, sym_y);
                entity->transform.position.z = token_stream_float(stream, sym_z);
            }
            token_stream_unindent(stream);
            token_stream_identifier(stream, sym_rotation);
            token_stream_indent(stream);
            {
                entity->transform.rotation.x = token_stream_float(stream, sym_x);
                entity->transform.rotation.y = token_stream_float(stream, sym_y);
                entity->transform.rotation.z = token_stream_float(stream, sym_z);
                entity->transform.rotation.w = token_stream_float(stream, sym_w);
            }
            token_stream_unindent(stream);
            token_stream_identifier(stream, sym_scale);
            token_stream_indent(stream);
            {
                entity->transform.scale.x = token_stream_float(stream, sym_x);
                entity->transform.scale.y = token_stream_float(stream, sym_y);
                entity->transform.scale.z = token_stream_float(stream, sym_z);
            }
        }
        token_stream_unindent(stream);
    }
    token_stream_unindent(stream);
#endif

    return entity;
}
#endif

void scene_parse(scene *scn, token *tokens)
{
    token_stream stream_ = { 0 };
    token_stream *stream = &stream_;
    stream->tokens = tokens;

    // Objects in current level
    obj_ptr stack[16] = { 0 };
    int level = 0;   // Current level of indentation
    int indent = 0;  // Current line indent counter

    token *tok = 0;
    while (token_stream_next(stream, &tok)) {
        switch (tok->type) {
            case TOKEN_EOF: {
                break;
            } case TOKEN_IDENTIFIER: {
                for (int i = level; i >= 0; i--) {
                    if (indent >= stack[i].indent) {
                        break;
                    }
                    level--;
                }
                stack[level].indent = indent;

                if (level) {
                    ta_object_field *field = obj_field_find(stack[level-1].type, tok->value.string);
                    DLB_ASSERT(field);
                    DLB_ASSERT(field->type);
                    stack[level].type = field->type;
                    stack[level].ptr = ((u8 *)stack[level-1].ptr + field->offset);
                } else {
                    ta_object *obj = dlb_hash_search(&tg_objects_by_name, tok->value.string, tok->length);
                    DLB_ASSERT(obj);
                    DLB_ASSERT(obj->type);
                    stack[level].type = obj->type;
                    stack[level].ptr = scene_obj_init(scn, obj->type, 0);
                }
                level++;
                break;
            } case TOKEN_WHITESPACE: {
                break;
            } case TOKEN_NEWLINE: {
                indent = 0;
                tok = 0;
                break;
            } case TOKEN_INDENT: {
                indent++;
                break;
            } case TOKEN_COMMENT: {
                break;
            } case TOKEN_INT: {
                DLB_ASSERT(level);
                DLB_ASSERT(stack[level-1].type == OBJ_INT);
                int *fp = stack[level-1].ptr;
                *fp = tok->value.as_int;
                level--;
                break;
            } case TOKEN_FLOAT: {
                DLB_ASSERT(level);
                DLB_ASSERT(stack[level-1].type == OBJ_FLOAT);
                float *fp = stack[level-1].ptr;
                *fp = tok->value.as_float;
                level--;
                break;
            } case TOKEN_STRING: {
                DLB_ASSERT(level);
                DLB_ASSERT(stack[level-1].type == OBJ_STRING);
                const char **fp = stack[level-1].ptr;
                *fp = tok->value.string;
                level--;
                break;
            } default: {
                PANIC("Unexpected token %s\n", token_type_str(tok->type));
            }
        }
        tok++;
    }
}

scene *scene_load(file *f)
{
    scene *scn = scene_init(f->filename);
    token *tokens = scene_tokenize(f, true);
    token_print(tokens);
    token_print_debug(tokens);
    scene_parse(scn, tokens);
    dlb_vec_free(tokens);
    return scn;
}

void *scene_obj_init(scene *scn, ta_object_type type, unsigned int uid)
{
    if (uid) {
        DLB_ASSERT(uid >= scn->next_uid);
        scn->next_uid = uid;
    }

    switch (type) {
        case OBJ_TA_ENTITY: {
            ta_entity *ptr = dlb_vec_alloc(scn->entities);
            ptr->obj_type = type;
            ptr->uid = scn->next_uid++;
            return ptr;
        } case OBJ_TA_MATERIAL: {
            ta_material *ptr = dlb_vec_alloc(scn->materials);
            ptr->obj_type = type;
            ptr->uid = scn->next_uid++;
            return ptr;
            break;
        } case OBJ_TA_MESH: {
            ta_mesh *ptr = dlb_vec_alloc(scn->meshes);
            ptr->obj_type = type;
            ptr->uid = scn->next_uid++;
            return ptr;
            break;
        } case OBJ_TA_SHADER: {
            ta_shader *ptr = dlb_vec_alloc(scn->shaders);
            ptr->obj_type = type;
            ptr->uid = scn->next_uid++;
            return ptr;
            break;
        } case OBJ_TA_TEXTURE: {
            ta_texture *ptr = dlb_vec_alloc(scn->textures);
            ptr->obj_type = type;
            ptr->uid = scn->next_uid++;
            return ptr;
            break;
        } default: {
            DLB_ASSERT(!"Cannot initialize this type as a standalone object");
        }
    }

    return NULL;
}

#if 0
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
#endif