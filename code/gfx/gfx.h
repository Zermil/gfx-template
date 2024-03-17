#ifndef GFX_H
#define GFX_H

#ifndef GFX_MAX_WINDOW_COUNT
# define GFX_MAX_WINDOW_COUNT 16
#endif

// @Note: Window is an OS specific thing, that's why we're making it opaque.
typedef void GFX_Window;

typedef void gfx_render_func(GFX_Window *window, void *data);
typedef void gfx_destroy_func(GFX_Window *window);

internal b32 gfx_init(void);
internal b32 gfx_is_init(void);
internal void gfx_process_input(void);

internal GFX_Window *gfx_window_create(const char *title, s32 width, s32 height);
internal void gfx_window_destroy(GFX_Window *window);
internal b32 gfx_window_set_visible(GFX_Window *window, b32 visible);
internal b32 gfx_window_set_title(GFX_Window *window, const char *title);
internal b32 gfx_window_set_resizable(GFX_Window *window, b32 resizable);
internal b32 gfx_window_get_resizable(GFX_Window *window);
internal b32 gfx_window_is_valid(GFX_Window *window);
internal b32 gfx_window_set_render_func(GFX_Window *window, gfx_render_func *render, void *data);
internal b32 gfx_window_set_destroy_func(GFX_Window *window, gfx_destroy_func *destroy);
internal b32 gfx_window_get_rect(GFX_Window *window, f32 *width, f32 *height);
internal b32 gfx_window_wants_to_quit(GFX_Window *window);

#endif // GFX_H
