#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "lcd.h"
#include "common.h"

int ts_x = 0;
int ts_y = 0;
int or_x = 0;
int or_y = 0;
int min_x = LCD_WIDTH;
int max_x = 0;
int min_y = LCD_HEIGHT;
int max_y = 0;

unsigned long touch_count = 0;

time_t time_stamp_now = 0;
time_t time_stamp_old = 0;

int get_xy(const char* touch_screen_path){
    ts_x = 0;
    ts_y = 0;
    or_x = 0;
    or_y = 0;
    max_x = 0;
    min_x = LCD_WIDTH;
    max_y = 0;
    min_y = LCD_HEIGHT;
    int is_first = 1;
    time_stamp_now = 0;
    time_stamp_old = 0;
    struct input_event touch_event;
    int ts_fd = open(touch_screen_path, O_RDONLY);

#if 0
    while(1){
        read(ts_fd, &touch_event, sizeof(touch_event));
        if(touch_event.type == EV_ABS){
            time_stamp_old = time_stamp_now;
            time_stamp_now = time(NULL);
            if (touch_event.code == ABS_Y){
                if(ts_y && (touch_event.value > ts_y+100 || touch_event.value < ts_y-100) && (time_stamp_now - time_stamp_old) < 2){
#if PRINT_X_Y
                    printf("ts_y invalid jump touch: sub=%d time%d\n", touch_event.value - ts_y, (int)(time_stamp_now - time_stamp_old));
#endif
                    continue;                                                               //invalid jump touch
                }
                ts_y = touch_event.value;
            }
            if (touch_event.code == ABS_X){
                if(ts_x && (touch_event.value > ts_x+100 || touch_event.value < ts_x-100) && (time_stamp_now - time_stamp_old) < 2){
#if PRINT_X_Y
                    printf("ts_x invalid jump touch: sub=%d time%d\n", touch_event.value - ts_y, (int)(time_stamp_now - time_stamp_old));
#endif
                    continue;                                                               //invalid jump touch
                }
                ts_x = touch_event.value;
            }
        }
        if(touch_event.type==EV_KEY && touch_event.code==BTN_TOUCH && touch_event.value==0){
#if PRINT_X_Y
            printf("leave touch screen\n");
#endif
            break;
        }
        touch_count++;
#if PRINT_X_Y
        static char printf_buf_old[1024];
        static char printf_buf_now[1024];
        snprintf(printf_buf_now, sizeof printf_buf_now, "x: %3d, y: %3d", ts_x, ts_y);
        if(strcmp(printf_buf_now, printf_buf_old)){
            printf("%s count=%lu\n", printf_buf_now, touch_count);
            strcpy(printf_buf_old, printf_buf_now);
        }
#endif
        if (is_first){
            or_x = ts_x;
            or_y = ts_y;
            is_first = 0;
        }
        if (ts_x && ts_x > max_x)
            max_x = ts_x;
        if (ts_y && ts_y > max_y)
            max_y = ts_y;
        if (ts_x && ts_x < min_x)
            min_x = ts_x;
        if (ts_y && ts_y < min_y)
            min_y = ts_y;
    }
    close(ts_fd);
    return 0;

#else

    while(1){
        read(ts_fd, &touch_event, sizeof(touch_event));
        if (touch_event.type == EV_ABS){
            if (touch_event.code == ABS_Y) {
                ts_y = touch_event.value;
            }
            if (touch_event.code == ABS_X){
                ts_x = touch_event.value;
            }
        }
        touch_count++;
#if PRINT_X_Y
        static char printf_buf_old[1024];
        static char printf_buf_now[1024];
        snprintf(printf_buf_now, sizeof printf_buf_now, "x: %3d, y: %3d", ts_x, ts_y);
        if(strcmp(printf_buf_now, printf_buf_old)){
            printf("%s count=%lu\n", printf_buf_now, touch_count);
            strcpy(printf_buf_old, printf_buf_now);
        }
#endif
        if (is_first){
            or_x = ts_x;
            or_y = ts_y;
            is_first = 0;
        }

        if (ts_x > max_x && ts_x != 0) max_x = ts_x;
        if (ts_y > max_y && ts_y != 0) max_y = ts_y;
        if (ts_x < min_x && ts_x != 0) min_x = ts_x;
        if (ts_y < min_y && ts_y != 0) min_y = ts_y;

        if(touch_event.type==EV_KEY && touch_event.code==BTN_TOUCH && touch_event.value==0){
#if PRINT_X_Y
            printf("leave touch screen\n");
#endif
            break;
        }
    }
    close(ts_fd);//关闭触摸屏
    return 0;
#endif
}