#ifndef RENDER_H
#define RENDER_H

typedef void R_Texture2D;

// @Note: Used for templates
typedef struct R_Vertex R_Vertex;
struct R_Vertex
{
  HMM_Vec2 pos;
};

// @Note: Used for instancing and batching
typedef struct R_Quad R_Quad;
struct R_Quad
{
  RectF32 pos;
  RectF32 uv;
  u32 col;
  f32 radius;
  f32 theta;
};

typedef struct R_Quad_Node R_Quad_Node;
struct R_Quad_Node
{
  R_Quad_Node *next;
  R_Quad *quads;
  u64 count;
};

internal b32 r_is_init(void);
internal b32 r_backend_init(void);
internal void r_backend_end(void);
internal b32 r_window_equip(GFX_Window *window);
internal void r_window_unequip(GFX_Window *window);
internal void r_frame_begin(GFX_Window *window, u32 clear_color);
internal void r_frame_end(GFX_Window *window);
internal b32 r_submit_quads(GFX_Window *window, R_Quad_Node *draw_data, u64 total_quad_count, R_Texture2D *texture);

// @ToDo: This is kinda useless and very problem-specific
internal u8 *r_end_frame_get_backbuffer(GFX_Window *window, Arena *arena);

// @ToDo: We're only allowing for textures in RGBA format for now
internal R_Texture2D *r_texture_create(void *data, u32 width, u32 height);
internal b32 r_texture_destroy(R_Texture2D *texture);
internal b32 r_texture_update(R_Texture2D *texture, void *data, u32 width, u32 height);

#endif // RENDER_H
