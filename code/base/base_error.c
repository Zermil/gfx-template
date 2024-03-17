thread_var Arena *er_arena = {0};

internal void er_init(void)
{
    er_arena = arena_make_sized(KB(64), 0);
}

internal void er_accum(void)
{
    OPTICK_EVENT();
    
    arena_pop_to(er_arena, 0);
}

internal void er_push(const char *msg)
{
    UNUSED(msg);
}

internal void er_end(void)
{
    arena_release(er_arena);
}
