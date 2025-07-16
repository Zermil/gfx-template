internal u64 str8_cstr_size(u8 *cstr)
{
  u8 *ptr = cstr;
  for (;*ptr != 0; ++ptr);
  u64 result = ptr - cstr;
  return(result);
}

internal String8 str8_make(u8 *data, u64 size)
{
  String8 result = {0};
  result.data = data;
  result.size = size;
  return(result);
}

internal String8 str8_from_cstr(const char *cstr)
{
  u64 size = str8_cstr_size((u8 *) cstr);
  String8 result = str8_make((u8 *) cstr, size);
  return(result);
}

internal String8 str8_push_copy(Arena *arena, String8 str)
{
  String8 result = {0};
  result.data = arena_push_array(arena, u8, str.size + 1);
  result.size = str.size;
  MemoryCopy(result.data, str.data, str.size);
  result.data[result.size] = 0;
  return(result);
}
