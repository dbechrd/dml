#pragma once
#include "dlb_types.h"

#define PANIC(format, ...) \
    fprintf(stderr, "\n---[PANIC]----------------------------------------------------------------------\n" \
        "Source file: %s:%d\n\n", __FILE__, __LINE__); \
    fprintf(stderr, (format), __VA_ARGS__); \
    fprintf(stderr, "--------------------------------------------------------------------------------\n"); \
    UNUSED(getchar()); \
    exit(1);

#define PANIC_FILE(f, format, ...) \
    fprintf(stderr, "\n---[PANIC_FILE]-----------------------------------------------------------------\n" \
        "Source file: %s:%d\n", __FILE__, __LINE__); \
    file_debug_context(f); \
    fprintf(stderr, (format), __VA_ARGS__); \
    fprintf(stderr, "--------------------------------------------------------------------------------\n"); \
    UNUSED(getchar()); \
    exit(1);

const char *char_printable(const char *c);
bool str_contains_chr(const char *str, char c);