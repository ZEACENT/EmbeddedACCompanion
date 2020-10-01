#ifndef	__FREE_TYPE
#define	__FREE_TYPE

#include <wchar.h>
#include <ft2build.h>
#include FT_FREETYPE_H

extern void Lcd_Show_FreeType(wchar_t *wtext, int size, int color, int start_x, int start_y, unsigned int *lcd_ptr);
extern int Show_FreeType_Bitmap(FT_Bitmap*  bitmap,int start_x,int start_y,int color, unsigned int *lcd_ptr);

#endif
