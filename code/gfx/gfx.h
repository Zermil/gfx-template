#ifndef GFX_H
#define GFX_H

#ifndef GFX_MAX_WINDOW_COUNT
# define GFX_MAX_WINDOW_COUNT 16
#endif

// @Note: These are OS specific things, that's why we're making them  opaque.
typedef void GFX_Window;

typedef void gfx_render_func(GFX_Window *window, void *data);
typedef void gfx_destroy_func(GFX_Window *window);

typedef u32 GFX_Event_Kind;
enum GFX_Event_Kind_Enum
{
  GFX_EVENT_NONE = 0,
  GFX_EVENT_QUIT,
  GFX_EVENT_MOUSE,
  GFX_EVENT_KEYDOWN,
  GFX_EVENT_LBUTTONUP,
  GFX_EVENT_MBUTTONUP,
  GFX_EVENT_LBUTTONDOWN,
  GFX_EVENT_MBUTTONDOWN,
  GFX_EVENT_MOUSEMOVE,
  GFX_EVENT_MOUSEWHEEL,
  GFX_EVENT_DROPFILES,
};

typedef u32 GFX_Cursor_Kind;
enum GFX_Cursor_Kind_Enum
{
  GFX_CURSOR_ARROW = 0,
  GFX_CURSOR_HAND,
  GFX_CURSOR_HSIZE,
};

typedef struct GFX_Drop_Files_Node GFX_Drop_Files_Node;
struct GFX_Drop_Files_Node
{
  String8 name;
};

typedef struct GFX_Drop_Files GFX_Drop_Files;
struct GFX_Drop_Files
{
  GFX_Drop_Files_Node *files;
  u64 count;
};

typedef struct GFX_Event GFX_Event;
struct GFX_Event
{
  GFX_Event *next;
  GFX_Window *window;

  GFX_Event_Kind kind;

  u64 character;
  GFX_Drop_Files drop_files;
  HMM_Vec2 mouse;
  f32 mouse_wheel;
};

typedef struct GFX_Event_List GFX_Event_List;
struct GFX_Event_List
{
  GFX_Event *first;
  GFX_Event *last;
  u64 count;
};

internal b32 gfx_init(void);
internal b32 gfx_is_init(void);

internal GFX_Event *gfx_events_push(GFX_Event_Kind kind, GFX_Window *window);
internal void gfx_events_eat(GFX_Event_List *list);
internal GFX_Event_List gfx_process_input(Arena *arena);

internal GFX_Window *gfx_window_create(String8 title, s32 width, s32 height);
internal void gfx_window_destroy(GFX_Window *window);
internal b32 gfx_window_is_valid(GFX_Window *window);

internal b32 gfx_window_set_visible(GFX_Window *window, b32 visible);
internal b32 gfx_window_set_title(GFX_Window *window, String8 title);
internal b32 gfx_window_set_resizable(GFX_Window *window, b32 resizable);
internal b32 gfx_window_set_render_func(GFX_Window *window, gfx_render_func *render, void *data);
internal b32 gfx_window_set_destroy_func(GFX_Window *window, gfx_destroy_func *destroy);
internal b32 gfx_window_set_drop_files(GFX_Window *window, b32 drop_files);

internal b32 gfx_window_get_rect(GFX_Window *window, f32 *width, f32 *height);
internal b32 gfx_window_get_resizable(GFX_Window *window);
internal void gfx_window_save_to_file(GFX_Window *window, Arena *arena);

internal b32 gfx_mouse_get_screen_pos(s32 *mx, s32 *my);
internal b32 gfx_mouse_get_relative_pos(GFX_Window *window, s32 *mx, s32 *my);
internal void gfx_mouse_set_cursor(GFX_Window *window, GFX_Cursor_Kind kind);
internal void gfx_mouse_set_capture(GFX_Window *window, b32 capture);

internal void gfx_error_display(GFX_Window *window, String8 text, String8 caption);
internal b32 gfx_open_save_dialog(GFX_Window *window, String8 *out);

#endif // GFX_H
