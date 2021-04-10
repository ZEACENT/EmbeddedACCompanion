#ifndef __LCD_H_
#define __LCD_H_

#ifndef LCD_HEIGHT
#define LCD_HEIGHT        480
#endif /* end with #ifndef LCD_HEIGHT */

#ifndef LCD_WIDTH
#define LCD_WIDTH        800
#endif /* end with #idndef LCD_WIDTH */

#ifndef LCD_SIZE
#define LCD_SIZE        (LCD_WIDTH)*(LCD_HEIGHT)*4
#endif /* end width #ifndef LCD_SIZE */

#ifndef WHITE_COLOR
#define WHITE_COLOR	0x00FFFFFF
#endif /* end width #ifndef WHITE_COLOR*/

#ifndef GREEN_COLOR
#define GREEN_COLOR	0x0000FF00
#endif /* end width #ifndef GREEN_COLOR*/

void lcd_draw_full_srceen_single_color(unsigned int color, unsigned int *lcd_ptr);
void lcd_draw_point(unsigned int x, unsigned int y, unsigned int color, unsigned int *lcd_ptr);
int open_lcd_device(unsigned int **lcd_ptr);
int close_lcd_device(int lcd_fd, unsigned int *lcd_ptr);

#endif /* end with #ifndef __LCD_H_ */
