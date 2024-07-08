/* @Note: This file is a test for font rendering
 * ideally we want something that just creates a texture atlas with all
 * glyphs and returns that as a texture. We just want a textured quad for now, nothing
 * too fancy.
 *
 * For rasterisation we could look into DWrite but its API and docs are kind of a pain
 * to go through.
 */
#define R_BACKEND_D3D11 1

#define FPS 60
#define FRAME_MS (1000.0/FPS)

#define WIDTH 1280
#define HEIGHT 720

#include <ft2build.h>
#include FT_FREETYPE_H

#include <HandmadeMath.h>
#include <optick.h>

#include "./base/base_inc.h"
#include "./os/os_inc.h"
#include "./gfx/gfx_inc.h"
#include "./render/render_inc.h"

#include "./base/base_inc.c"
#include "./os/os_inc.c"
#include "./gfx/gfx_inc.c"
#include "./render/render_inc.c"

#define FONT_GLYPH_COUNT 128
#define FONT_INIT_ATLAS_SIZE 128.0f

typedef struct Font_Glyph_Info Font_Glyph_Info;
struct Font_Glyph_Info
{
    HMM_Vec2 size;
    HMM_Vec2 origin;
    HMM_Vec2 offset;
    RectF32 uv;
    f32 advance;
};

typedef struct Font Font;
struct Font
{
    R_Texture2D *texture;
    HMM_Vec2 texture_size;
    Font_Glyph_Info glyphs[FONT_GLYPH_COUNT];
};

typedef struct Font_Rect_Node Font_Rect_Node;
struct Font_Rect_Node
{
    HMM_Vec2 size;
    HMM_Vec2 origin;
    b32 occupied;

    Font_Rect_Node *left;
    Font_Rect_Node *right;
};

// @Note: Thanks to -> https://straypixels.net/texture-packing-for-fonts/
internal Font_Rect_Node *font_rect_pack(Arena *arena, Font_Rect_Node *node, HMM_Vec2 size, HMM_Vec2 texture_size)
{
    if (node->left && node->right) {
        Font_Rect_Node *result = font_rect_pack(arena, node->left, size, texture_size);
        if (result != 0) {
            return(result);
        }

        result = font_rect_pack(arena, node->right, size, texture_size);
        return(result);
    }

    if (node->occupied) {
        Assert(node->left == 0 && node->right == 0);
        return(0);
    }

    HMM_Vec2 real_size = { node->size.X, node->size.Y };

    // @Note: Recalculate size if on edge
    if (node->size.X + node->origin.X == FLT_MAX) real_size.X = texture_size.X - node->origin.X;
    if (node->size.Y + node->origin.Y == FLT_MAX) real_size.Y = texture_size.Y - node->origin.Y;

    if (real_size.X < size.X || real_size.Y < size.Y) {
        // @Note: Doesn't fit, in our case this is an error
        return(0);
    }

    if (node->size.X == size.X && node->size.Y == size.Y) {
        // @Note: Perfect fit
        node->occupied = 1;
        return(node);
    }

    // @Note: Split and create new nodes
    Font_Rect_Node *left = arena_push_array(arena, Font_Rect_Node, 1);
    Font_Rect_Node *right = arena_push_array(arena, Font_Rect_Node, 1);

    f32 dx = real_size.X - size.X;
    f32 dy = real_size.Y - size.Y;
    b32 split_h = dx > dy;

    // @Note: Edge case
    if (dx == 0 && dy == 0) {
        if (node->size.X > node->size.Y) split_h = 1;
        else split_h = 0;
    }

    if (split_h) {
        // @Note: Split horizontally, left is up
        left->origin = { node->origin.X, node->origin.Y };
        left->size = { size.X, node->size.Y };

        right->origin = { node->origin.X + size.X, node->origin.Y };
        right->size = { node->size.X - size.X, node->size.Y };
    } else {
        // @Note: Split vertically
        left->origin = { node->origin.X, node->origin.Y };
        left->size = { node->size.X, size.Y };

        right->origin = { node->origin.X, node->origin.Y + size.Y };
        right->size = { node->size.X, node->size.Y - size.Y };
    }

    node->left = left;
    node->right = right;
    
    return(font_rect_pack(arena, node->left, size, texture_size));
}

internal Font font_init(String8 font_name, u32 font_size) 
{
    Arena *arena_pack = arena_make();
    
    Font_Rect_Node *root = arena_push_array(arena_pack, Font_Rect_Node, 1);
    root->size.X = FLT_MAX;
    root->size.Y = FLT_MAX;
    root->origin.X = 0.0f;
    root->origin.Y = 0.0f;
    
    FT_Library ft = {0};
    FT_Init_FreeType(&ft);
    
    FT_Face face = {0};
    FT_New_Face(ft, (const char *) font_name.data, 0, &face);
    FT_Set_Char_Size(face, 0, (font_size << 6), 96, 96);
    
    Font result = {0};
    result.texture_size = { FONT_INIT_ATLAS_SIZE, FONT_INIT_ATLAS_SIZE };
    u32 *pixels = arena_push_array(arena_pack, u32, (usize) (result.texture_size.X*result.texture_size.Y));
    
    for (u32 i = 0; i < FONT_GLYPH_COUNT; ++i) {
        FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
        FT_Bitmap *bmp = &face->glyph->bitmap;
        HMM_Vec2 glyph_size = { (f32) bmp->width, (f32) bmp->rows };

        Font_Rect_Node *node = font_rect_pack(arena_pack, root, glyph_size, result.texture_size);
        if (node == 0) {
            // @Note: We have to ask for a bigger texture.
            HMM_Vec2 new_size = { result.texture_size.X * 2.0f, result.texture_size.Y * 2.0f };
            u32 *new_pixels = arena_push_array(arena_pack, u32, (usize) (new_size.X*new_size.Y));

            // @Note: We have to be mindful of texture_size
            for (u32 pr = 0; pr < (u32) result.texture_size.X; ++pr) {
                for (u32 pc = 0; pc < (u32) result.texture_size.Y; ++pc) {
                    usize pn = (usize) (pr*new_size.X + pc); // @Note: Pixels New
                    usize po = (usize) (pr*result.texture_size.X + pc); // @Note: Pixels old
                    new_pixels[pn] = pixels[po];
                }
            }

            result.texture_size = new_size;
            pixels = new_pixels;
            node = font_rect_pack(arena_pack, root, glyph_size, result.texture_size);
            
            Assert(node != 0);
        }

        // @Note: Copy glyph's pixels.
        for (u32 row = 0; row < bmp->rows; ++row) {
            for (u32 col = 0; col < bmp->width; ++col) {
                u32 x = (u32) (node->origin.X + col);
                u32 y = (u32) (node->origin.Y + row);
                u8 pixel = bmp->buffer[row*bmp->pitch + col];
                
                if (pixel) {
                    usize index = (usize) (y*result.texture_size.X + x);
                    pixels[index] = (pixel << 3*8) | (pixel << 2*8) | (pixel << 1*8) | 0xFF;
                }
            }
        }

        // @Note: Info for rendering individual glyphs.
        result.glyphs[i].size = glyph_size;
        result.glyphs[i].origin = node->origin;

        result.glyphs[i].offset.X = (f32) face->glyph->bitmap_left;
        result.glyphs[i].offset.Y = (f32) face->glyph->bitmap_top;
        
        result.glyphs[i].advance = (f32) (face->glyph->advance.x >> 6);
    }

    for (u32 i = 0; i < FONT_GLYPH_COUNT; ++i) {
        result.glyphs[i].uv = {
            result.glyphs[i].origin.X/result.texture_size.X,
            result.glyphs[i].origin.Y/result.texture_size.Y,
            (result.glyphs[i].origin.X + result.glyphs[i].size.X)/result.texture_size.X,
            (result.glyphs[i].origin.Y + result.glyphs[i].size.Y)/result.texture_size.Y
        };        
    }
    
    result.texture = r_texture_create(pixels, (u32) result.texture_size.X, (u32) result.texture_size.Y);
    
    FT_Done_FreeType(ft);
    arena_release(arena_pack);
    
    return(result);
}

internal void r_text(R_Ctx *ctx, Font *font_atlas, HMM_Vec2 pos, String8 text)
{    
    // @ToDo: This was just here to get started, please change it to something better.
    f32 max_h = -1.0f;
    for (u32 i = 0; i < text.size; ++i) {
        Font_Glyph_Info glyph = font_atlas->glyphs[text.data[i]];
        max_h = MAX(max_h, glyph.size.Y);
    }
    
    for (u32 i = 0; i < text.size; ++i) {
        Font_Glyph_Info glyph = font_atlas->glyphs[text.data[i]];
        f32 x = pos.X + glyph.offset.X;
        f32 y = (pos.Y + max_h) - glyph.offset.Y;
        
        RectF32 glyph_pos = {
            x, y,
            x + glyph.size.X, y + glyph.size.Y
        };

        r_rect_tex_ex(ctx, glyph_pos, 0.0f, glyph.uv, font_atlas->texture);
        pos.X += glyph.advance;
    }
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
    UNUSED(instance);
    UNUSED(prev_instance);
    UNUSED(cmd_line);
    UNUSED(cmd_show);
    
    // @Note: Init modules
    {
        os_main_init();
        gfx_init();
        r_backend_init();
    }
    
    Arena *arena = arena_make();
    Arena *frame_arena = arena_make();
    
    GFX_Window *window = gfx_window_create(str8("A window"), WIDTH, HEIGHT);
    gfx_window_set_resizable(window, 1);
    gfx_window_set_visible(window, 1);
    gfx_window_set_destroy_func(window, r_window_unequip);
    
    r_window_equip(window);
    
    Font font_atlas = font_init(str8("./Inconsolata-Regular.ttf"), 32);
    
    b32 should_quit = 0;
    f64 frame_prev = os_ticks_now();
    
    while (!should_quit) {
        OPTICK_FRAME("Main");
        
        arena_clear(frame_arena);
        
#ifndef NDEBUG
        er_accum_start();
#endif
        
        f64 frame_start = os_ticks_now();
        // f64 delta = frame_start - frame_prev;
        frame_prev = frame_start;
        
        GFX_Event_List event_list = gfx_process_input(frame_arena);
        for (GFX_Event *event = event_list.first; event != 0; event = event->next) {
            gfx_events_eat(&event_list);
            
            if (event->kind == GFX_EVENT_QUIT) {
                should_quit = 1;
                goto frame_end;
            }
        }
        
        R_List list = {0};
        R_Ctx ctx = r_make_context(frame_arena, &list);
        
        r_frame_begin(window);
        
        f32 w, h;
        gfx_window_get_rect(window, &w, &h);
        
        r_rect_tex(&ctx, { 0.0f, 0.0f, font_atlas.texture_size.X, font_atlas.texture_size.Y }, 0.0f, font_atlas.texture);
        r_text(&ctx, &font_atlas, { w/2.0f, h/2.0f }, str8("Nothing more to say!&"));
        
        r_flush_batches(window, &list);
        r_frame_end(window);
        
        frame_end:
        {
#ifndef NDEBUG
            Arena_Temp temp = arena_temp_begin(arena);
            String8 error = er_accum_end(temp.arena);
            if (error.size != 0) {
                gfx_error_display(window, error, str8("Error"));
                os_exit_process(1);
            }
            arena_temp_end(&temp);
#endif
        }
        
        f64 frame_time = os_ticks_now() - frame_start;
        if (frame_time < FRAME_MS) {
            os_wait(FRAME_MS - frame_time);
        }
    }
    
    gfx_window_destroy(window);
    r_backend_end();
    
    return 0;
}
