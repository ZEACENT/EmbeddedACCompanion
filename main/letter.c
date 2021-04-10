#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"
#include "to_wchar.h"
#include "freetype.h"

int letter(){
	int lcd_fd;
	int dis_ret;
	unsigned int *lcd_ptr;
	
	//显示宽字符
	wchar_t *w_text = L"已经翻到底部了";
	Lcd_Show_FreeType(w_text,64,0x00000000,200,+220, lcd_ptr);
	
	//多字节字符转宽字符	
	wchar_t *w_pathname = NULL;
	if( mchars_to_wchars("It has turned to the bottom", &w_pathname) == 0)//将多字节转换为宽字符(汉字未能转换成功)
	{
		//显示字体             字、大小、字体颜色与背景颜色、起始x坐标，起始y坐标、LCD 操作指针
		Lcd_Show_FreeType(w_pathname,48,0x000000,80,270, lcd_ptr);
		free(w_pathname);
	}
	return 0;
}
