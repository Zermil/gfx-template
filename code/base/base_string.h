#ifndef BASE_STRING_H
#define BASE_STRING_H

typedef struct String8 String8;
struct String8
{
  u8 *data;
  u64 size;
};

typedef struct String16 String16;
struct String16
{
  u16 *data;
  u64 size;
};

typedef struct String32 String32;
struct String32
{
  u32 *data;
  u64 size;
};

typedef struct Unicode_Codepoint Unicode_Codepoint;
struct Unicode_Codepoint
{
  u32 codepoint;

  // @Note: This isn't size in bytes, it's more contextual, for example:
  // when we use it with String16, size = 1 means 1 multiple of 16 bits
  // when we use it with String8, size = 1 means 1 multiple of 8 bits
  u32 size;
};

internal u64 str8_cstr_size(u8 *cstr);
internal String8 str8_make(u8 *data, u64 size);
internal String8 str8_from_cstr(const char *cstr);
internal String8 str8_push_copy(Arena *arena, String8 str);

internal u64 str16_cstr_size(u16 *cstr);
internal String16 str16_make(u16 *data, u64 size);
internal String16 str16_from_cstr(u16 *cstr);

internal u64 str32_cstr_size(u32 *cstr);
internal String32 str32_make(u32 *data, u64 size);
internal String32 str32_from_cstr(u32 *cstr);

#define str8(cstr) (str8_make((u8 *) (cstr), sizeof(cstr) - 1))

#endif // BASE_STRING_H
