// @Note: For more information look here, amazing article!
// https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator

#ifndef BASE_ARENA_H
#define BASE_ARENA_H

#ifndef ARENA_HEADER_SIZE
# define ARENA_HEADER_SIZE KB(4)
#endif

#ifndef ARENA_DEFAULT_SIZE
# define ARENA_DEFAULT_SIZE MB(16)
#endif

#ifndef ARENA_DEFAULT_COMMIT
# define ARENA_DEFAULT_COMMIT KB(64)
#endif

typedef struct Arena Arena;
struct Arena
{
  Arena *current;
  Arena *prev;

  u64 chunk_cap;
  u64 chunk_pos;

  u64 commit_pos;

  b32 growing; // @Note: We want to allow arenas that are static i.e. not 'growing'
  u64 align;
  u64 base_pos; // @Note: Helper for when we 'pop' arenas
};

typedef struct Arena_Temp Arena_Temp;
struct Arena_Temp
{
  Arena *arena;
  u64 pos;
};

internal Arena *arena_make_sized(u64 size, b32 growing);
internal Arena *arena_make(void);
internal void arena_release(Arena *arena);

internal void *arena_push_no_zero(Arena *arena, u64 size);
internal void *arena_push(Arena *arena, u64 size);
internal void arena_pop_to(Arena *arena, u64 pos);

internal u64 arena_pos(Arena *arena);
internal void arena_put_back(Arena *arena, u64 size);
internal void arena_clear(Arena *arena);

internal Arena_Temp arena_temp_begin(Arena *arena);
internal void arena_temp_end(Arena_Temp *temp);

#define arena_push_array(a, t, s) ((t *) (arena_push((a), sizeof(t)*(s))))

#endif // BASE_ARENA_H
