#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "lcd.h"

int ts_x = 0;
int ts_y = 0;
int or_x = 0;
int or_y = 0;
int max_x = 0, min_x = 0;
int max_y = 0, min_y = 0;
int finger = 0;

int get_xy(void){
	ts_x = 0;
	ts_y = 0;
	or_x = 0;
	or_y = 0;
	max_x = 0;
	min_x = LCD_WIDTH;
	max_y = 0;
	min_y = LCD_HEIGHT;
	int first = 2;
	struct input_event touch_event;
	int ts_fd = open("/dev/input/event0", O_RDONLY);//打开触摸屏
	if(ts_fd == -1){
		perror("打开触摸屏失败");
		exit(0);
	}

	while(1){
		read(ts_fd, &touch_event, sizeof(touch_event));
		finger = 1;	//触摸状态
		if (touch_event.type == EV_ABS){
			if (touch_event.code == ABS_Y) {
				ts_y = touch_event.value;
			}
			if (touch_event.code == ABS_X){
				ts_x = touch_event.value;
			}
		}
		if (first!=0){
			or_x = ts_x;
			or_y = ts_y;
			first--;
		}

		if (first <= 1) {
			if (ts_x > max_x && ts_x != 0) max_x = ts_x;
			if (ts_y > max_y && ts_y != 0) max_y = ts_y;
			if (ts_x < min_x && ts_x != 0) min_x = ts_x;
			if (ts_y < min_y && ts_y != 0) min_y = ts_y;
		}

		if(touch_event.type==EV_KEY && touch_event.code==BTN_TOUCH && touch_event.value==0){//手指离开
			delay(1000000);
			finger = 0;
			break;

		}
	}
	close(ts_fd);//关闭触摸屏
	return 0;
}