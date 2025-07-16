#ifdef _WIN32
# include "win32/win32_gfx_impl.c"
#else
# error no backend for GFX on this operating system
#endif
