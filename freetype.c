#include <sys/mman.h>
#include <stdio.h>
#include <linux/fb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wchar.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "lcd.h"


#define	SIM_TTY_PATH	"font/simsun.ttc"
int Show_FreeType_Bitmap(FT_Bitmap*  bitmap,int start_x,int start_y,int color, unsigned int *lcd_buf_ptr)
{
	int  buff_x, buff_y;	//遍历bitmap时使用
	int  x,y;	//循环遍历使用
	int  end_x = start_x + bitmap->width;	//图像宽度
	int  end_y = start_y + bitmap->rows;	//图像高度

	for ( x = start_x, buff_x = 0; x < end_x; buff_x++, x++ )	//y表示起点y，x表示起点x
	{
		for ( y = start_y, buff_y = 0; y < end_y; buff_y++, y++ )	//y表示起点y，x表示起点x
		{
			//LCD边界处理
			if ( x < 0 || y < 0 || x >=800   || y >= 480 )
				continue;

			if(bitmap->buffer[buff_y * bitmap->width + buff_x] ){
				lcd_draw_point((unsigned int)(start_x+buff_x), (unsigned int)(start_y+buff_y), (unsigned int)color);
			}	//判断该位上是不是为1，1则表明需要描点
						//在当前x位置加上p的偏移量（p表示buff中列的移动）
				//在当前y位置加上q的偏移量（q表示buff中行的移动）
			
		}
	}
} 

void Lcd_Show_FreeType(wchar_t *wtext, int size, int color, int start_x, int start_y, unsigned int *lcd_buf_ptr)
{

	//1.定义库所需要的变量
	FT_Library    library;
	FT_Face       face;
	FT_GlyphSlot  slot;	//用于指向face中的glyph
	FT_Vector     pen;                    
	FT_Error      error;

	int n;

	
	//2.初始化库
	error = FT_Init_FreeType( &library );            

	//3.打开一个字体文件，并加载face对象:
	error = FT_New_Face( library, SIM_TTY_PATH, 0, &face ); 
	slot = face->glyph;
	
	//4.设置字体大小
	error = FT_Set_Pixel_Sizes(face, size, 0); 
	
	//x起点位置：start_x。需要*64
	pen.x = start_x * 64;	
	//y起点位置：LCD高度 - start_y。需要*64
	pen.y = ( 480 - start_y ) * 64;
	
	//每次取出显示字符串中的一个字
	for ( n = 0; n < wcslen( wtext ); n++ )	
	{
		//5.设置显示位置和旋转角度，0为不旋转，pen为提前设置好的坐标
		FT_Set_Transform( face, 0, &pen );

		error = FT_Load_Char( face, wtext[n], FT_LOAD_RENDER );
			//FT_LOAD_RENDER表示转换RGB888的位图像素数据
			
		//出错判断	
		if ( error )
		  continue;                
		
		
		Show_FreeType_Bitmap(&slot->bitmap, slot->bitmap_left, 480 - slot->bitmap_top, color, lcd_buf_ptr);
		
		//增加笔位
		pen.x += slot->advance.x;
		
	}	
	
	FT_Done_Face(face);
	FT_Done_FreeType(library);
	
}






