/* @Note: This file is a test for font rendering
 * ideally we want something that just creates a texture atlas with all
 * glyphs and returns that as a texture. We just want a textured quad for now, nothing
 * too fancy.
* 
* [ ] Rectangle packing
* [ ] Baseline calculation
* [ ] Rendering diacritics -- Norwegian, Russian etc.
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

typedef struct Glyph_Info Glyph_Info;
struct Glyph_Info {
    RectF32 uv;
    f32 width;
    f32 height;
    f32 xo, yo;
    f32 advance;
};

typedef struct Font_Atlas Font_Atlas;
struct Font_Atlas {
    R_Texture2D *texture;
    Glyph_Info glyphs[GLYPH_COUNT];
};

// @ToDo: Proper rectangle packing algorithm.
internal Font_Atlas font_init(Arena *arena, String8 font_name, u32 font_size) 
{
    Font_Atlas result = {0};
    
    FT_Library ft = {0};
    FT_Face face = {0};
    
    FT_Init_FreeType(&ft);
    FT_New_Face(ft, (const char *) font_name.data, 0, &face);
    
    u32 char_size = (font_size << 6);
    u32 dpi = 96;
    FT_Set_Char_Size(face, 0, char_size, dpi, dpi);
    
    // @Note: Estimate font atlas size
    u32 max_dim = (1 + (face->size->metrics.height >> 6)) * ((u32) ceilf(sqrtf(GLYPH_COUNT))); // @Note: Experiment with this
    
    u32 tex_width = 1;
    while (tex_width < max_dim) tex_width <<= 1;
    u32 tex_height = tex_width;
    
    u32 *pixels = arena_push_array(arena, u32, tex_width*tex_height);
    
    f32 tex_x = 0.0f;
    f32 tex_y = 0.0f;
    for (u32 i = 0; i < GLYPH_COUNT; ++i) {
        FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
        FT_Bitmap *bmp = &face->glyph->bitmap;
        
        // @Note: Ensure that we're still in bounds.
        if (tex_x + bmp->width >= tex_width) {
            tex_x = 0.0f;
            tex_y += (face->size->metrics.height >> 6) + 1.0f;
        }
        
        // @Note: Copy glyph's pixels.
        for (u32 row = 0; row < bmp->rows; ++row) {
            for (u32 col = 0; col < bmp->width; ++col) {
                u32 x = (u32) tex_x + col;
                u32 y = (u32) tex_y + row;
                u8 pixel = bmp->buffer[row*bmp->pitch + col];
                
                if (pixel) {
                    pixels[y*tex_width + x] = (pixel << 3*8) | (pixel << 2*8) | (pixel << 1*8) | 0xFF;
                }
            }
        }
        
        // @Note: Info for rendering individual glyphs.
        result.glyphs[i].uv.x0 = tex_x/tex_width;
        result.glyphs[i].uv.y0 = tex_y/tex_height;
        result.glyphs[i].uv.x1 = (tex_x + bmp->width)/tex_width;
        result.glyphs[i].uv.y1 = (tex_y + bmp->rows)/tex_height;
        
        result.glyphs[i].width = (f32) bmp->width;
        result.glyphs[i].height = (f32) bmp->rows;
        
        result.glyphs[i].xo = (f32) face->glyph->bitmap_left;
        result.glyphs[i].yo = (f32) face->glyph->bitmap_top;
        result.glyphs[i].advance = (f32) (face->glyph->advance.x >> 6);
        
        tex_x += bmp->width + 1;
    }
    
    result.texture = r_texture_create(pixels, tex_width, tex_height);
    FT_Done_FreeType(ft);
    
    return(result);
}

internal void r_text(R_Ctx *ctx, Font_Atlas *font_atlas, HMM_Vec2 pos, String8 text)
{
    // @ToDo: This was just here to get started, please change it to something better.
    f32 max_h = -1.0f;
    for (u32 i = 0; i < text.size; ++i) {
        Glyph_Info glyph = font_atlas->glyphs[text.data[i]];
        max_h = MAX(max_h, glyph.height);
    }
    
    for (u32 i = 0; i < text.size; ++i) {
        Glyph_Info glyph = font_atlas->glyphs[text.data[i]];
        f32 x = pos.X + glyph.xo;
        f32 y = (pos.Y + max_h) - glyph.yo;
        
        RectF32 glyph_pos = {
            x, y,
            x + glyph.width, y + glyph.height
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
    
    Font_Atlas font_atlas = font_init(arena, str8("./Inconsolata-Regular.ttf"), 32);
    
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
        
        r_frame_begin(window);
        
        f32 w, h;
        gfx_window_get_rect(window, &w, &h);
        
        r_text(&ctx, &font_atlas, { 0.0f, 0.0f }, str8("Sample text"));
        r_text(&ctx, &font_atlas, { w/2.0f, h/2.0f }, str8("Nothing more to say"));
        
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
    }
    
    gfx_window_destroy(window);
    r_backend_end();
    
    return 0;
}
