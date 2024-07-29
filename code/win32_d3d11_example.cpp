#define R_BACKEND_D3D11 1
#define HANDMADE_MATH_USE_TURNS 1

#define FPS 60
#define FRAME_MS (1000.0f/FPS)

#define WIDTH 1280
#define HEIGHT 720

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

R_Texture2D *texture = 0;

internal void render(GFX_Window *window, void *data)
{
    Arena *frame_arena = (Arena *) data;
    
    R_List list = {0};
    R_Ctx ctx = r_make_context(frame_arena, &list);
    
    r_frame_begin(window, 0x121212FF);
    
    f32 width = 0.0f;
    f32 height = 0.0f;
    gfx_window_get_rect(window, &width, &height);
    
    f32 rect_size = 300.0f;
    f32 pad = 20.0f;
    f32 x0 = width/2.0f - rect_size/2.0f;
    f32 y0 = height/2.0f - rect_size/2.0f;
    
    RectF32 pos1 = {
        x0, y0, 
        x0 + rect_size, y0 + rect_size 
    };
    
    RectF32 pos2 = { 
        x0 + rect_size + pad, y0, 
        x0 + 2.0f*rect_size + pad, y0 + rect_size
    };
    
    RectF32 pos3 = { 
        x0 - rect_size - pad, y0, 
        x0 - pad, y0 + rect_size
    };
    
    // @Note: Order matters, here we will get two batches, one for coloured quad
    // the other one for _both_ textured quads.
    r_rect(&ctx, pos1, 0x814A38FF, 10.0f);
    r_rect_tex(&ctx, pos2, 0.0f, texture);
    r_rect_tex(&ctx, pos3, 0.0f, texture);
    
    r_flush_batches(window, &list);
    r_frame_end(window);
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
    
    u32 *data = arena_push_array(arena, u32, 32*32);
    for (usize row = 0; row < 32; ++row) {
        for (usize col = 0; col < 32; ++col) {
            usize index = col + row*32;
            
            if ((row + col) % 2 == 0) {
                data[index] = 0xBA9B8DFF;
            } else {
                data[index] = 0x585651FF;
            }
        }
    }
    texture = r_texture_create(data, 32, 32);
    
    Arena *frame_arena = arena_make();
    
    GFX_Window *window = gfx_window_create(str8("A window"), WIDTH, HEIGHT);
    gfx_window_set_resizable(window, 1);
    gfx_window_set_visible(window, 1);
    gfx_window_set_render_func(window, render, frame_arena);
    gfx_window_set_destroy_func(window, r_window_unequip);
    
    r_window_equip(window);
    
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
            if (event->kind == GFX_EVENT_QUIT) {
                should_quit = 1;
                goto frame_end;
            }
            
            gfx_events_eat(&event_list);
        }
        
        render(window, frame_arena);
        
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
    
    r_texture_destroy(texture);
    gfx_window_destroy(window);
    r_backend_end();
    
    OPTICK_SHUTDOWN();
    
    return 0;
}
