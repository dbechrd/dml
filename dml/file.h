#pragma once
#include "dlb_types.h"
#include <stdlib.h>
#include <stdio.h>

typedef enum file_mode {
    FILE_READ,
    FILE_WRITE,
} file_mode;

static inline const char *file_mode_str(file_mode mode) {
    const char *str;
    switch(mode) {
    case FILE_READ: str = "rb"; break;
    case FILE_WRITE: str = "wb"; break;
    default: DLB_ASSERT(0);
    }
    return str;
}

typedef struct file {
    const char *filename;
    file_mode mode;
    FILE *hnd;
    char last;
    size_t line_number;
    size_t line_column;
} file;

file *file_open(const char *filename, file_mode mode);

static inline char file_getc(file *f) {
    if (f->last == '\n') {
        f->line_number++;
        f->line_column = 0;
    }
    f->line_column++;
    f->last = fgetc(f->hnd);
    return f->last;
}

static inline void file_expect(file *f, char chr) {
    char c = file_getc(f);
    if (c != chr) {
        fprintf(stderr, "%s:%d:%d [PARSE_ERROR] Missing expected character '%c'\n",
                f->filename, f->line_number, f->line_column, chr);
        getchar();
        exit(1);
    }
}

static inline void file_close(file *f) {
    fclose(f->hnd);
    free(f);
}