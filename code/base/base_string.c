internal String8 str8_make(u8 *data, usize size)
{
    String8 result = {0};
    result.data = data;
    result.size = size;
    return(result);
}

internal String8 str8_cstr(const char *str)
{
    u8 *data = (u8 *) str; 
    u8 *ptr = data;
    
    for (;*ptr != 0; ++ptr);
    usize size = (ptr - data);
    
    String8 result = str8_make(data, size);
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