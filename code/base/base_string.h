#ifndef BASE_STRING_H
#define BASE_STRING_H

typedef struct {
    u8 *data;
    usize size;
} String8;

internal String8 str8_make(u8 *data, usize size);
#define str8(cstr) str8_make((u8 *) (cstr), sizeof(cstr) - 1)
internal String8 str8_cstr(const char *str);
internal String8 str8_push_copy(Arena *arena, String8 str);

#endif // BASE_STRING_H
