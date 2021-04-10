#ifndef __LCD_H__
#define __LCD_H__

#define LCD_WIDTH  			800
#define LCD_HEIGHT 			480
#define FB_SIZE				(LCD_WIDTH * LCD_HEIGHT * 4)

int get_xy(const char* touch_screen_path);
struct LcdDevice *lcd_open(const char* lcd_path);
void lcd_draw_point(int x,int y, unsigned int color);
int lcd_draw_bmp(unsigned int x, unsigned int y, const char *pbmp_path);
void lcd_close(void);
unsigned long file_size_get(const char *pfile_path);
int DrawPic_HWindows(int x, int y, const char *pbmp_path);
int letter();

#define EN_LCD_SHOW_JPG		1

/* video_chat.c 画中画显示的坐标 */
extern volatile int g_jpg_in_jpg_x;
extern volatile int g_jpg_in_jpg_y;

unsigned long file_size_get(const char *pfile_path);

#endif