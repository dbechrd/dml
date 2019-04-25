#pragma once
#include "helpers.h"
#include "dlb_types.h"
#include <stdlib.h>
#include <stdio.h>

typedef enum file_mode {
    FILE_READ,
    FILE_WRITE,
} file_mode;

typedef struct file_pos {
    size_t line;
    size_t column;
} file_pos;

typedef struct file {
    const char *filename;
    file_mode mode;
    FILE *hnd;
    char prev;
    bool replay;
    bool eof;

    // Debug info
    file_pos pos;
    char context_buf[80];  // Line buffer for debug context
    int context_len;
} file;

file *file_open(const char *filename, file_mode mode);
void file_close(file *f);
void file_debug_context(file *f);
char file_char(file *f);
char file_char_escaped(file *f);
char file_peek(file *f);
char file_read(file *f, char *buf, size_t count, const char *valid_chars,
    const char *delims, int *len);
int file_expect_char(file *f, const char *chars, int times);
int file_allow_char(file *f, const char *chars, int times);