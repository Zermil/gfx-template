#ifndef RENDER_HELPER_H
#define RENDER_HELPER_H

#ifndef R_MAX_QUAD_CHUNK
# define R_MAX_QUAD_CHUNK KB(4)
#endif

typedef struct R_Quad_Batch R_Quad_Batch;
struct R_Quad_Batch
{
  R_Quad_Batch *next;

  R_Quad_Node *first;
  R_Quad_Node *last;
  u64 count;
  u64 total_quad_count;

  R_Texture2D *texture;
};

typedef struct R_List R_List;
struct R_List
{
  R_Quad_Batch *first;
  R_Quad_Batch *last;
  u64 count;
};

typedef struct R_Ctx R_Ctx;
struct R_Ctx
{
  Arena *arena;
  R_List *list;
};

// @Note: Actual internal helpers
internal R_Ctx r_make_context(Arena *arena, R_List *list);
internal void r_new_batch(Arena *arena, R_List *list);
internal void r_prep_batch(Arena *arena, R_List *list, R_Texture2D *texture);
internal void r_push_quad(Arena *arena, R_Quad_Batch *batch, R_Quad *quad);

// @Note: Helpers more 'external', intended to be more user-friendly
internal void r_rect_ex(R_Ctx *ctx, RectF32 pos, u32 col, f32 radius, f32 theta);
internal void r_rect(R_Ctx *ctx, RectF32 pos, u32 col, f32 radius);
internal void r_circ(R_Ctx *ctx, HMM_Vec2 pos, f32 radius, u32 col);
internal void r_rect_tex_ex(R_Ctx *ctx, RectF32 pos, u32 tint, f32 radius, f32 theta, RectF32 uv, R_Texture2D *texture);
internal void r_rect_tex(R_Ctx *ctx, RectF32 pos, f32 radius, R_Texture2D *texture);
internal void r_flush_batches(GFX_Window *window, R_List *list);

#endif // RENDER_HELPER_H
