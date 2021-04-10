#include <stdio.h>   	//printf scanf
#include <fcntl.h>		//open write read lseek close  	 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <common.h>
#include "lcd.h"
#include "font.h"

#if EN_LCD_SHOW_JPG
//#include "jpeglib.h"
#endif

static char g_color_buf[FB_SIZE] = { 0 };
static int  g_fb_fd;	//lcd fd
static unsigned int *g_pfb_memory;	//lcd mp
volatile int g_jpg_in_jpg_x;//video_chat.c 画中画显示的坐标
volatile int g_jpg_in_jpg_y;

//初始化LCD
struct LcdDevice *lcd_open(const char* lcd_path){
	struct LcdDevice* lcd = malloc(sizeof(struct LcdDevice));
	if(lcd == NULL){
		return NULL;
	}

    g_fb_fd = open(lcd_path, O_RDWR);
	lcd->fd = g_fb_fd;
	if(lcd->fd < 0){
		free(lcd);
		return NULL;
	}

    g_pfb_memory  = (unsigned int *)mmap(	NULL, 					//映射区的开始地址，设置为NULL时表示由系统决定映射区的起始地址
                                    FB_SIZE, 				//映射区的长度
                                    PROT_READ|PROT_WRITE, 	//内容可以被读取和写入
                                    MAP_SHARED,				//共享内存
                                    g_fb_fd, 				//有效的文件描述词
                                    0						//被映射对象内容的起点
	);
	lcd->mp = g_pfb_memory;

    return lcd;
}

//LCD画点
void lcd_draw_point(int x,int y, unsigned int color)
{
    *(g_pfb_memory+y*800+x)=color;
}

//LCD任意地址绘制图片
int lcd_draw_bmp(unsigned int x, unsigned int y, const char *pbmp_path){
	int bmp_fd;
	unsigned int blue, green, red;
	unsigned int color;
	unsigned int bmp_width;
	unsigned int bmp_height;
	unsigned int bmp_type;
	unsigned int bmp_size;
	unsigned int x_s = x;
	unsigned int y_s = y;
	unsigned int x_e;
	unsigned int y_e;
	unsigned char buf[54] = { 0 };
	char *pbmp_buf = g_color_buf;

	bmp_fd = open(pbmp_path, O_RDWR);

	if (bmp_fd == -1){
		printf("open bmp error\r\n");
		return -1;
	}
#if DEBUG
	printf("open %s successfully\n", pbmp_path);
#endif

	/* 读取位图头部信息 */
	read(bmp_fd, buf, 54);
#if DEBUG
	printf("read head successfully\n", pbmp_path);
#endif
	/* 宽度  */
	bmp_width = buf[18];
	bmp_width |= buf[19] << 8;
	/* 高度  */
	bmp_height = buf[22];
	bmp_height |= buf[23] << 8;
	/* 文件类型 */
	bmp_type = buf[28];
	bmp_type |= buf[29] << 8;
	/* 设置显示x、y坐标结束位置 */
	x_e = x + bmp_width;
	y_e = y + bmp_height;
	/* 获取位图文件的大小 */
	bmp_size = file_size_get(pbmp_path);
	/* 读取所有RGB数据 */
	read(bmp_fd, pbmp_buf, bmp_size - 54);

	for (; y < y_e; y++){
		for (x = x_s; x < x_e; x++){
			blue = *pbmp_buf++;
			green = *pbmp_buf++;
			red = *pbmp_buf++;
			if (bmp_type == 32){
				pbmp_buf++;
			}
			color = red << 16 | green << 8 | blue << 0;
			lcd_draw_point(x, y, color);
		}
	}
#if DEBUG
	printf("lcd_draw_point successfully\n");
#endif
	close(bmp_fd);
	return 0;
}

void lcd_close(void){
    munmap(g_pfb_memory, FB_SIZE);
    close(g_fb_fd);
}

unsigned long file_size_get(const char *pfile_path){
    unsigned long filesize = -1;
    struct stat statbuff;
    if(stat(pfile_path, &statbuff) < 0){
        return filesize;
    }
    else{
        filesize = statbuff.st_size;
    }
    return filesize;
}

int DrawPic_HWindows(int x, int y, const char *pbmp_path) {
	int bmp_fd;
	unsigned int blue, green, red;
	unsigned int color;
	unsigned int bmp_width;
	unsigned int bmp_height;
	unsigned int bmp_type;
	unsigned int bmp_size;
	int x_s = x;
	int y_s = y;
	int x_e;
	int y_e;
	unsigned char buf[54] = { 0 };
	char *pbmp_buf = g_color_buf;

	/* 申请位图资源，权限可读可写 */
	bmp_fd = open(pbmp_path, O_RDWR);

	if (bmp_fd == -1) {
		printf("open bmp error\r\n");
		return -1;
	}

	/* 读取位图头部信息 */
	read(bmp_fd, buf, 54);
	/* 宽度  */
	bmp_width = buf[18];
	bmp_width |= buf[19] << 8;
	/* 高度  */
	bmp_height = buf[22];
	bmp_height |= buf[23] << 8;
	/* 文件类型 */
	bmp_type = buf[28];
	bmp_type |= buf[29] << 8;
	/* 设置显示x、y坐标结束位置 */
	x_e = x + bmp_width;
	y_e = y + bmp_height;
	/* 获取位图文件的大小 */
	bmp_size = file_size_get(pbmp_path);
	/* 读取所有RGB数据 */
	read(bmp_fd, pbmp_buf, bmp_size - 54);

	for (y=y_s; y != (LCD_HEIGHT + 39); y = y + 40 ) {					//左上角为原点
		if (y >= LCD_HEIGHT) {
			++y;
			y = y - LCD_HEIGHT;
			if (bmp_type == 32) {
				pbmp_buf = pbmp_buf - 4 * LCD_WIDTH * 440;
			}
			pbmp_buf = pbmp_buf - 3 * LCD_WIDTH * 440;
		}
		else if( y != 0 ){
			if (bmp_type == 32) {
				pbmp_buf = pbmp_buf + 4 * LCD_WIDTH * 39;
			}
			pbmp_buf = pbmp_buf + 3 * LCD_WIDTH * 39 ;
		}
		for (x = x_s; x < x_e; x++) {
			blue = *pbmp_buf++;
			green = *pbmp_buf++;
			red = *pbmp_buf++;
			if (bmp_type == 32) {
				pbmp_buf++;
			}
			color = red << 16 | green << 8 | blue << 0;
			lcd_draw_point(x, y, color);
		}
	}
	close(bmp_fd);
	return 0;
}