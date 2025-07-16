#ifndef ERROR_H
#define ERROR_H

typedef struct ER_Node ER_Node;
struct ER_Node
{
  ER_Node *next;

  u64 arena_pos;
  String8 error;
};

typedef struct ER_Thread_Local ER_Thread_Local;
struct ER_Thread_Local
{
  Arena *arena;
  ER_Node *stack;
};

internal void er_accum_start(void);
internal void er_push(String8 error);
internal String8 er_accum_end(Arena *arena);

#endif // ERROR_H
