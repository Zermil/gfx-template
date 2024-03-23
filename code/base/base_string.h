#ifndef BASE_STRING_H
#define BASE_STRING_H

typedef struct {
    u8 *data;
    usize size;
} String8;

typedef struct {
    String8 contents;
    usize cap;
} String8_Buffer;

internal String8 str8_make(u8 *data, usize size);
internal String8 str8_push_cstr(Arena *arena, const char *cstr);
internal String8 str8_cstr(const char *cstr);
internal String8 str8_push_copy(Arena *arena, String8 str);
internal usize str8_cstr_size(const char *cstr);

internal String8_Buffer str8_buffer_push_empty(Arena *arena, usize cap);
internal b32 str8_buffer_hold(String8_Buffer *buff, String8 str);

#define str8(cstr) str8_make((u8 *) (cstr), sizeof(cstr) - 1)

#endif // BASE_STRING_H
