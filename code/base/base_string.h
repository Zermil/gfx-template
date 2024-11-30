#ifndef BASE_STRING_H
#define BASE_STRING_H

typedef struct {
    u8 *data;
    usize size;
} String8;

typedef struct {
    u16 *data;
    usize size;
} String16;

typedef struct {
    u32 *data;
    usize size;
} String32;

typedef struct {
    u32 codepoint;

    // @Note: This isn't size in bytes, it's more contextual, for example:
    // when we use it with String16, size = 1 means 1 multiple of 16 bits
    // when we use it with String8, size = 1 means 1 multiple of 8 bits
    u32 size;
} Unicode_Codepoint;


internal usize str8_cstr_size(u8 *cstr);
internal String8 str8_make(u8 *data, usize size);
internal String8 str8_from_cstr(const char *cstr);
internal String8 str8_push_copy(Arena *arena, String8 str);

internal usize str16_cstr_size(u16 *cstr);
internal String16 str16_make(u16 *data, usize size);
internal String16 str16_from_cstr(u16 *cstr);

internal usize str32_cstr_size(u32 *cstr);
internal String32 str32_make(u32 *data, usize size);
internal String32 str32_from_cstr(u32 *cstr);

#define str8(cstr) (str8_make((u8 *) (cstr), sizeof(cstr) - 1))

#endif // BASE_STRING_H
