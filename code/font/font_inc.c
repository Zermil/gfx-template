#include "font_helper.c"

#ifdef FONT_USE_FREETYPE
# include "freetype/freetype_font_impl.c"
#else
# error font provider not specified font_inc.c
#endif
