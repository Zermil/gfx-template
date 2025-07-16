#ifndef FONT_HELPER_H
#define FONT_HELPER_H

typedef struct Font_Rect_Node Font_Rect_Node;
struct Font_Rect_Node
{
  HMM_Vec2 size;
  HMM_Vec2 origin;
  b32 occupied;

  Font_Rect_Node *left;
  Font_Rect_Node *right;
};

internal Font_Rect_Node *font_rect_pack(Arena *arena, Font_Rect_Node *node, HMM_Vec2 size, HMM_Vec2 texture_size);

#endif // FONT_HELPER_H
