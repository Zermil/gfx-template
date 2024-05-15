/* @Note: This file is a test for font rendering
 * ideally we want something that just creates a texture atlas with all
 * glyphs and returns that as a texture. We just want a textured quad for now, nothing
 * too fancy.
 */
#define R_BACKEND_D3D11 1

#define FPS 60

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

#define GLYPH_COUNT 128
#define FONT_SIZE 64

typedef struct Glyph_Info Glyph_Info;
struct Glyph_Info
{
    u32 x0, y0;
    u32 x1, y1;
    u32 xo, yo;
    u32 advance;
};

global Glyph_Info glyphs[GLYPH_COUNT] = {0};
global R_Texture2D *font_atlas = 0;

internal void font_init(Arena *arena)
{
    FT_Library ft = {0};
    FT_Face face = {0};

    Arena_Temp font_arena = arena_temp_begin(arena);
    
    FT_Init_FreeType(&ft);
    FT_New_Face(ft, "./Inconsolata-Regular.ttf", 0, &face);
    FT_Set_Char_Size(face, 0, FONT_SIZE << 6, 96, 96);

    // @Note: Estimate font atlas size
    u32 max_dim = (1 + (face->size->metrics.height >> 6)) * ((u32) ceilf(sqrtf(GLYPH_COUNT))); // @Note: Experiment with this
    
    u32 tex_width = 1;
    while (tex_width < max_dim) tex_width <<= 1;
    u32 tex_height = tex_width;

    u8 *pixels = arena_push_array(font_arena.arena, u8, tex_width*tex_height);
    
    u32 pen_x = 0;
    u32 pen_y = 0;
    
    for (u32 i = 0; i < GLYPH_COUNT; ++i) {
        FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
        FT_Bitmap *bmp = &face->glyph->bitmap;

        // @Note: Ensure that we're still in bounds.
        if (pen_x + bmp->width >= tex_width) {
            pen_x = 0;
            pen_y += (face->size->metrics.height >> 6) + 1;
        }

        // @Note: Copy glyph's pixels.
        for (u32 row = 0; row < bmp->rows; ++row) {
            for (u32 col = 0; col < bmp->width; ++col) {
                u32 x = pen_x + col;
                u32 y = pen_y + row;
                pixels[y*tex_width + x] = bmp->buffer[row*bmp->pitch + col];
            }
        }

        // @Note: Info for rendering individual glyphs.
        glyphs[i].x0 = pen_x;
        glyphs[i].y0 = pen_y;
        glyphs[i].x1 = pen_x + bmp->width;
        glyphs[i].y1 = pen_y + bmp->rows;

        glyphs[i].xo = face->glyph->bitmap_left;
        glyphs[i].yo = face->glyph->bitmap_top;
        glyphs[i].advance = (face->glyph->advance.x >> 6);
        
        pen_x += bmp->width + 1;
    }

    FT_Done_FreeType(ft);

    u32 *atlas_data = arena_push_array(arena, u32, tex_width*tex_height);
    for (u32 i = 0; i < (tex_width*tex_height); ++i) {
        if (pixels[i]) {
            atlas_data[i] = 0xFFFFFFFF;
        }
    }

    font_atlas = r_texture_create(atlas_data, tex_width, tex_height);
    
    arena_temp_end(&font_arena);
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

    font_init(arena);
    
    b32 should_quit = 0;
    while (!should_quit) {
        OPTICK_FRAME("Main");
        
        arena_clear(frame_arena);
#ifndef NDEBUG
        er_accum_start();
#endif
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

        RectF32 pos = {
            20.0f, 20.0f,
            WIDTH - 20, HEIGHT - 20
        };
        
        r_frame_begin(window);
        {
            r_rect_tex(&ctx, pos, 0.0f, font_atlas);
        }
        r_flush_batches(window, &list);
        r_frame_end(window);
        
    frame_end:
#ifndef NDEBUG
        {
            Arena_Temp temp = arena_temp_begin(arena);
            String8 error = er_accum_end(temp.arena);
            if (error.size != 0) {
                gfx_error_display(window, error, str8("Error"));
                os_exit_process(1);
            }
            arena_temp_end(&temp);
        }
#endif
    }

    gfx_window_destroy(window);
    r_backend_end();
    
    return 0;
}
