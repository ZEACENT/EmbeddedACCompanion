#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "lcd.h"

void lcd_draw_point2(unsigned int x, unsigned int y, unsigned int color, unsigned int *lcd_ptr)
{
	if( x>=0 && x<LCD_WIDTH && y>=0 && y<LCD_HEIGHT )
	{
		*(lcd_ptr+LCD_WIDTH*y+x) = color;
	}
}

void lcd_draw_full_srceen_single_color(unsigned int color, unsigned int *lcd_ptr)
{
	int x, y;
	for(y=0;y<LCD_HEIGHT;++y)
	{
		for(x=0;x<LCD_WIDTH;++x)
		{
			lcd_draw_point2(x, y, color, lcd_ptr);
		}
	}
}
