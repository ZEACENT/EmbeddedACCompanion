// #include <stdio.h>
// #include "font.h"

// //初始化Lcd
// struct LcdDevice *init_lcd(const char *device){
// 	//申请空间
// 	struct LcdDevice* lcd = malloc(sizeof(struct LcdDevice));
// 	if(lcd == NULL){
// 		return NULL;
// 	}
// 	printf("lcd malloc ok\n");

// 	//1打开设备
// 	lcd->fd = open(device, O_RDWR);
// 	if(lcd->fd < 0){
// 		perror("open lcd fail");
// 		free(lcd);
// 		return NULL;
// 	}
// 	printf("open lcd ok\n");
	
// 	//映射
// 	lcd->mp = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd->fd,0);
// 	printf("lcd mmap ok\n");

// 	return lcd;
// }

// int main()
// {
	
//     //初始化Lcd
// 	struct LcdDevice* lcd = init_lcd("/dev/fb0");
// 	printf("init ok\n");
			
// 	//打开字体	
// 	font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
// 	printf("fontload ok\n");

// 	//字体大小的设置
// 	fontSetSize(f,72);
	
	
// 	//创建一个画板（点阵图）
// 	// bitmap *bm = createBitmap(200,72,4);
// 	bitmap *bm = createBitmapWithInit(200,72,4,getColor(0,128,128,128));
// 	//bitmap *bm = createBitmap(288, 100, 4);
// 	printf("bm ok\n");
	
// 	char buf[] = "空调";
	
// 	//将字体写到点阵图上
// 	fontPrint(f, bm, 0, 0, buf, getColor(0,255,0,0), 0);
// 	printf("fontPrint ok\n");
	
// 	// lcd->fd: 文件描述符
// 	// lcd->mp: 内存映射
// 	// bm: 包含内容的点阵图

// 	//把字体框输出到LCD屏幕上
// 	show_font_to_lcd(lcd->mp, 0, 0, bm);
// 	printf("showFont ok\n");

// 	//把字体框输出到LCD屏幕上
// 	show_font_to_lcd(lcd->mp, 200, 200, bm);

	
// 	//关闭字体，关闭画板
// 	fontUnload(f);
// 	destroyBitmap(bm);
// 	printf("close ok\n");
	
// }
