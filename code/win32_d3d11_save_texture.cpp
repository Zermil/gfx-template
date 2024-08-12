#define FULL_WIN32 1
#define R_BACKEND_D3D11 1

#define FPS 60
#define FRAME_MS (1000.0f/FPS)

#define WIDTH 1280
#define HEIGHT 720

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stdio.h>
#include <stb_image_write.h>

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

b32 capture_frame = 0;

internal void render(GFX_Window *window, void *data)
{
    Arena *frame_arena = (Arena *) data;

    R_List list = {0};
    R_Ctx ctx = r_make_context(frame_arena, &list);

    r_frame_begin(window, 0x121212FF);
    
    f32 width, height;
    gfx_window_get_rect(window, &width, &height);

    f32 rect_size = 300.0f;
    f32 x0 = width/2.0f - rect_size/2.0f;
    f32 y0 = height/2.0f - rect_size/2.0f;
    
    RectF32 rect = {
        x0, y0, 
        x0 + rect_size, y0 + rect_size 
    };
    
    r_rect(&ctx, rect, 0x814A38FF, 10.0f);

    r_flush_batches(window, &list);
    r_frame_end(window);

    if (capture_frame) {
        R_List list1 = {0};
        R_Ctx ctx1 = r_make_context(frame_arena, &list1);

        s32 w = (s32) width;
        s32 h = (s32) height;
        
        GFX_Window *window1 = gfx_window_create(str8("A window"), w, h);
        gfx_window_set_destroy_func(window1, r_window_unequip);
        r_window_equip(window1);

        r_frame_begin(window1, 0x121212FF);
        r_rect(&ctx1, rect, 0xFF0000FF, 10.0f);

        r_flush_batches(window1, &list1);
        u8 *pixels = r_frame_end_get_backbuffer(window1, frame_arena);

        stbi_write_jpg("ss.jpg", w, h, 4, pixels, 100);
        
        gfx_window_destroy(window1);
        capture_frame = 0;
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
        frame_prev = frame_start;
        
        GFX_Event_List event_list = gfx_process_input(frame_arena);
        for (GFX_Event *event = event_list.first; event != 0; event = event->next) {
            gfx_events_eat(&event_list);
            if (event->kind == GFX_EVENT_QUIT) {
                should_quit = 1;
                goto frame_end;
            }

            switch (event->kind) { 
                case GFX_EVENT_KEYDOWN: {
                    if (event->character == 'R') {
                        capture_frame = 1;
                    }
                } break;
            }
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
    
    gfx_window_destroy(window);
    r_backend_end();
    
    OPTICK_SHUTDOWN();
    
    return 0;
}
