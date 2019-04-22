#include "file.h"
#include "dlb_memory.h"

static const char *file_mode_str(file_mode mode) {
    const char *str = "";
    switch(mode) {
        case FILE_READ:
            str = "rb";
            break;
        case FILE_WRITE:
            str = "wb";
            break;
        default:
            DLB_ASSERT(0);
    }
    return str;
}

file *file_open(const char *filename, file_mode mode) {
    FILE *hnd = fopen(filename, file_mode_str(mode));
    if (!hnd) {
        perror("fopen error");
        PANIC("Failed to open file: %s\n", filename);
    }
    file *file = dlb_calloc(1, sizeof(*file));
    file->filename = filename;
    file->mode = mode;
    file->hnd = hnd;
    file->pos.line = 1;
    file->pos.column = 1;
    return file;
}

void file_close(file *f) {
    fclose(f->hnd);
    free(f);
}

void file_debug_context(file *f)
{
    fprintf(stderr, " Scene file: %s:%d:%d\n\n", f->filename, f->pos.line, f->pos.column);
    fprintf(stderr, "%04d:%.*s", f->pos.line, f->context_len, f->context_buf);
    size_t col = f->pos.column;

    char buf[1024] = { 0 };
    f->replay = false;
    file_read(f, buf, sizeof(buf) - 1, 0, "\r\n", 0);
    fprintf(stderr, "%s\n", buf);

    fprintf(stderr, "     ");
    for (u32 i = 0; i < col - 1; i++) {
        fprintf(stderr, "-");
    }
    fprintf(stderr, "^\n\n");
}

char file_char(file *f) {
    if (f->replay) {
        f->replay = false;
        return f->prev;
    }
    if (f->prev == '\n') {
        f->context_len = 0;
        f->pos.line++;
        f->pos.column = 0;
    }
    f->pos.column++;
    int c = fgetc(f->hnd);
    if (c == EOF) {
        f->eof = true;
    }
    if (f->context_len < sizeof(f->context_buf)) {
        f->context_buf[f->context_len] = c;
    }
    f->context_len++;
    f->prev = (char)c;
    return f->prev;
}

char file_char_escaped(file *f)
{
    file_expect_char(f, "\\", 1);
    char c = file_char(f);
    switch (c) {
        case '\"':
            c = '\"';
            break;
        case '\\':
            c = '\\';
            break;
        case 't':
            c = '\t';
            break;
        case 'r':
            c = '\r';
            break;
        case 'n':
            c = '\n';
            break;
        case '0':
            c = '\0';
            break;
        case 'x':
            // TODO: Handle hex byte codes
            PANIC_FILE(f,
                "[PARSE_ERROR] Hex byte codes not yet supported in char "
                "literals.\n"
            );
            break;
        case 'u':
            // TODO: Handle short unicode code points
            PANIC_FILE(f,
                "[PARSE_ERROR] Short Unicode hex code points not yet supported "
                "in char literals.\n"
            );
            break;
        case 'U':
            // TODO: Handle long unicode code points
            PANIC_FILE(f,
                "[PARSE_ERROR] Long unicode hex code points not yet supported "
                "in char literals.\n"
            );
            break;
        case EOF:
            PANIC_FILE(f,
                "[PARSE_ERROR] Unexpected EOF while reading character.\n"
            );
        default:
            PANIC_FILE(f,
                "[PARSE_ERROR] Invalid escape sequence in char literal '%s'."
                "\n", char_printable(&c)
            );
    }
    return c;
}

char file_peek(file *f) {
    char c = file_char(f);
    f->replay = true;
    return c;
}

char file_read(file *f, char *buf, size_t count, const char *valid_chars,
    const char *delims, int *len)
{
    DLB_ASSERT(!buf || count);
    DLB_ASSERT(valid_chars || delims || count);

    file_pos pos_start = f->pos;
    pos_start.column += 1;

    char delim = 0;
    u32 i;
    for (i = 0; !count || i < count; i++) {
        char c = file_char(f);
        if (f->eof) {
            break;
        }

        delim = str_contains_chr(delims, c);
        if (delim) {
            f->replay = true;
            break;
        }

        char valid = str_contains_chr(valid_chars, c);
        if (valid_chars && !valid) {
            if (delims) {
                PANIC_FILE(f,
                    "[PARSE_ERROR] Unexpected character '%s' in expression "
                    "starting at %d:%d. Expected [%s] or delimeter [%s].\n",
                    char_printable(&c), (int)pos_start.line,
                    (int)pos_start.column, valid_chars, delims
                );
            } else {
                f->replay = true;
                break;
            }
        }

        if (buf) buf[i] = c;
    }

    if (delims && !delim) {
        PANIC_FILE(f,
            "[PARSE_ERROR] Expected delim [%s] to end expression starting at "
            "%d:%d\n", delims, (int)pos_start.line, (int)pos_start.column
        );
    }

    if (len) *len = i;
    return delim;
}

int file_expect_char(file *f, const char *chars, int times) {
    int count;
    file_read(f, 0, times, chars, 0, &count);
    if (count != times) {
        char next = file_peek(f);
        PANIC_FILE(f,
            "[PARSE_ERROR] Missing expected character [%s]. Found '%s' instead."
            "\n", chars, char_printable(&next)
        );
    }
    return count;
}

int file_allow_char(file *f, const char *chars, int times) {
    int count;
    file_read(f, 0, times, chars, 0, &count);
    return count;
}

static void file_expect_string(file *f, const char *str) {
    const char *s = str;
    while (*s) {
        char c = file_char(f);
        if (c != *s) {
            PANIC_FILE(f,
                "[PARSE_ERROR] Missing expected character '%s' of string %s. "
                "Found '%s' instead.\n", char_printable(s), str,
                char_printable(&c)
            );
        }
        s++;
    }
}

//char *file_read_all(file *f) {
//    DLB_ASSERT(!fseek(f->hnd, 0, SEEK_END));
//    long pos = ftell(f->hnd);
//    DLB_ASSERT(pos <= SIZE_MAX);
//    DLB_ASSERT(!fseek(f->hnd, 0, SEEK_SET));
//
//    size_t end = (size_t)pos;
//    char *buf = dlb_malloc(end + 1);
//    fread(buf, end, 1, f->hnd);
//    buf[end] = 0;
//    return buf;
//}