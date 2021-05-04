#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <common.h>
#include <errno.h>
#include "lcd.h"
#include "font.h"
#include "EdpKit.h"
#include "ConnectOneNet.h"

extern int ts_x;
extern int ts_y;
extern int or_x;
extern int or_y;
extern int max_x;
extern int min_x;
extern int max_y;
extern int min_y;
extern unsigned long touch_count;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int ACSwitch = 0;
static unsigned int temp = 26;
unsigned long touch_count_old;
static char rec_string_buf[1024];

#define MY_PORT     0
#define ALI_PORT    80
#define ALI_IPV4    "47.104.94.62"
#define TP_PATH     "/dev/event0"
#define LCD_PATH    "/dev/fb0"
#define BMP_PATH    "/Cover"
#define FONT_PATH   "/OPPOSans-R.ttf"
#define FONT_SIZE   100
#define MINI_F_SIZE 40
#define FPS         30
// #define NTP_HOST    "ntp.aliyun.com"
#define LOG_PATH    "/main.log"

static void* touch_screen_thread(void* arg){
    while (1) {
        usleep((double)1/FPS * 1000 * 1000);
        get_xy(TP_PATH);
    }
}

static void* one_net_rece_thread(void* arg){
    char RecvBuffer[100] = {0};
    memset(rec_string_buf, '\0', sizeof(rec_string_buf));
    int i = 0;
    int skipFirst = 1;
    while(1){
        memset(RecvBuffer, '\0', sizeof(RecvBuffer));
        OneNet_RecvData((void*)&sockfd, RecvBuffer);
        if(1 == skipFirst){
            skipFirst -= 1;
            continue;
        }
        // lock
        if(pthread_mutex_lock(&mutex) != 0){
              perror("pthread_mutex_lock");
              exit(EXIT_FAILURE);
        }
        for(i = 0; i < strlen(RecvBuffer); ++i){
            rec_string_buf[i] = RecvBuffer[i];
        }
        rec_string_buf[i] = '\0';
        // unlock
        if(pthread_mutex_unlock(&mutex) != 0){
            perror("pthread_mutex_unlock");
            exit(EXIT_FAILURE);
        }
        printf("RecvBufferLen: %d\n", strlen(RecvBuffer));
        printf("RecvBuffer: %s\n", RecvBuffer);
        printf("rec_string_buf in thread: %s\n", rec_string_buf);
    }
}

static void* one_net_send_thread(void* arg){
    char buf[20] = {0};
    while (1){
        sleep(3);
        memset(buf ,0 ,sizeof(buf));
        OneNet_SendData(buf, "Temp", temp);
    }
}

static inline void adjustTemp(unsigned int* temp, int num){
    *temp += num;
    *temp = *temp<16?16:*temp;
    *temp = *temp>36?36:*temp;
}

static void* local_ctrl_thread(void* arg){
    while(1){
        usleep((double)1/FPS * 1000 * 1000);
        if (touch_count_old != touch_count) {
            touch_count_old = touch_count;
        }
        else {
            continue;
        }
        if(ts_x>100 && ts_x<300 && ts_y>70 && ts_y<170){
            ACSwitch = 0==ACSwitch?1:0;
#if LOCAL_CTRL
            printf("switch %s %3d : %-3d\n", ACSwitch==1?"ON":"OFF", ts_x, ts_y);
#endif
        }
        if(ts_x>500 && ts_x<700 && ts_y>70 && ts_y<170){
            adjustTemp(&temp, 1);
#if LOCAL_CTRL
            printf("temp add %3d : %-3d\n", ts_x, ts_y);
#endif
        }
        if(ts_x>500 && ts_x<700 && ts_y>310 && ts_y<410){
            adjustTemp(&temp, -1);
#if LOCAL_CTRL
            printf("temp del %3d : %-3d\n", ts_x, ts_y);
#endif
        }
    }
}

static void* flush_log_thread(void* arg) {
    while (1) {
        sleep(1);
        fflush(stdout);
    }
}

int main(int argc, char **argv) {
    int ali_sock_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in alicloud_addr;
    struct LcdDevice* lcd = NULL;
    font* my_font      = NULL;
    font* my_mini_font = NULL;
    bitmap* bmAC       = NULL;
    bitmap* bmTmp      = NULL;
    bitmap* bmAdd      = NULL;
    bitmap* bmSub      = NULL;
    bitmap* bmVoice    = NULL;
    char buff[2048]    = {0};
    FILE *log_stream   = NULL;

    int PicNum = 1;
    int color = 0x00ff0000;    //如影随形颜色
    int r = 30;    //半径
    const char bufAC[]    = "空调";
    char bufTmp[4]        = "26C";
    const char bufAdd[]   = "+";
    const char bufSub[]   = "-";
    const char bufVoice[] = "V";

    pthread_t oneNetReceThread;                             //onenetReacv
    pthread_t oneNetSendThread;                             //onenetSend
    pthread_t touchScreenThread;                            //touchscreen
    pthread_t localCtrlTempThread;                          //ctrl temp
    pthread_t flushLogThread;                               //flush log

    // redirect stdout for log
    if (NULL == freopen(LOG_PATH, "w", stdout)) {
        printf("Failed to redirect stdout log to file\n");
    }
    if (NULL == freopen(LOG_PATH, "w", stderr)) {
        printf("Failed to redirect stderr log to file\n");
    }

    // First, sync time
    // system("ntpclient -s -d -c 1 -i 5 -h "NTP_HOST);

#if !BAN_ONE_NET
    if(OneNet_Init())
        printf("Failed to init OneNet\n");

    if(pthread_create(&oneNetReceThread, NULL, one_net_rece_thread, NULL))
        printf("Failed to create oneNetReceThread\n");
    if(pthread_create(&oneNetSendThread, NULL, one_net_send_thread, NULL))
        printf("Failed to create oneNetSendThread\n");
#endif
    if(pthread_create(&touchScreenThread, NULL, touch_screen_thread, NULL))
        printf("Failed to create touchScreenThread\n");
    if(pthread_create(&localCtrlTempThread, NULL, local_ctrl_thread, NULL))
        printf("Failed to create localCtrlTempThread\n");
    if(pthread_create(&flushLogThread, NULL, flush_log_thread, NULL))
        printf("Failed to create flushLogThread\n");

#if !BAN_ALI_CLD
    if(-1 == (ali_sock_fd = socket(AF_INET, SOCK_STREAM, 0))){
        printf("Failed to create ali_sock_fd\n");
        return -1;
    }
    memset(&my_addr, 0, sizeof my_addr);
    my_addr.sin_family      = AF_INET;
    my_addr.sin_port        = htons(MY_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(ali_sock_fd, (struct sockaddr *)&my_addr, sizeof my_addr)){
        printf("Failed to bind ali_sock_fd\n");
        return -1;
    }
    memset(&alicloud_addr, 0, sizeof alicloud_addr);
    alicloud_addr.sin_family        = AF_INET;
    alicloud_addr.sin_port          = htons(ALI_PORT);
    alicloud_addr.sin_addr.s_addr   = inet_addr(ALI_IPV4);
    if(connect(ali_sock_fd, (struct sockaddr *)&alicloud_addr, sizeof alicloud_addr)){
        printf("Failed to connect ali_sock_fd: %s\n", strerror(errno));
        return -1;
    }
#endif

    if(!(lcd = lcd_open(LCD_PATH)))
        printf("Failed to lcd_open\n");
#if DEBUG
        printf("lcd_open successfully\n");
#endif

    if(lcd_draw_bmp(0, 0, BMP_PATH))
        printf("Failed to draw cover lcd_draw_bmp\n");
#if DEBUG
        printf("draw cover successfully\n");
#endif

    if(!(my_font = fontLoad(FONT_PATH)) || !(my_mini_font = fontLoad(FONT_PATH)))
        printf("Failed to fontLoad\n");
#if DEBUG
        printf("fontLoad successfully\n");
#endif

    fontSetSize(my_font, FONT_SIZE);
    fontSetSize(my_mini_font, MINI_F_SIZE);
    bmAC    = createBitmapWithInit(200, 100, 4, getColor(255,0,0,0));
    bmTmp   = createBitmapWithInit(200, 100, 4, getColor(255,0,0,0));
    bmAdd   = createBitmapWithInit(200, 100, 4, getColor(255,0,0,0));
    bmSub   = createBitmapWithInit(200, 100, 4, getColor(255,0,0,0));
    bmVoice = createBitmapWithInit(50, 50, 4, getColor(255,0,0,0));
    fontPrint(my_font, bmAC, 0, 0, bufAC, getColor(0,0,255,0), 0);
    fontPrint(my_font, bmTmp, 0, 0, bufTmp, getColor(0,0,255,0), 0);
    fontPrint(my_font, bmAdd, 50, 0, bufAdd, getColor(0,0,255,0), 0);
    fontPrint(my_font, bmSub, 50, 0, bufSub, getColor(0,0,255,0), 0);
    fontPrint(my_mini_font, bmVoice, 0, 0, bufVoice, getColor(0,0,255,0), 0);
    show_font_to_lcd(lcd->mp, 100, 70, bmAC);
    show_font_to_lcd(lcd->mp, 100, 310, bmTmp);
    show_font_to_lcd(lcd->mp, 500, 70, bmAdd);
    show_font_to_lcd(lcd->mp, 500, 310, bmSub);
    show_font_to_lcd(lcd->mp, 375, 225, bmVoice);
    destroyBitmap(bmAC);
    destroyBitmap(bmTmp);
    destroyBitmap(bmAdd);
    destroyBitmap(bmSub);
    destroyBitmap(bmVoice);

    while (1) {
#if !BAN_ONE_NET
        if('1' == (rec_string_buf)[0] && 1 == strlen(rec_string_buf)){    //远程开关
            ACSwitch = 1;
            printf("switch on\n");
            strcpy(rec_string_buf, "N");
        }else if('0' == (rec_string_buf)[0] && 1 == strlen(rec_string_buf)){
            ACSwitch = 0;
            printf("switch off\n");
            strcpy(rec_string_buf, "N");
        }else if('N' != (rec_string_buf)[0] && 6 < strlen(rec_string_buf)){
            printf("check exp\n");
            sprintf(buff, "GET  /composite/queryexpress?number=%s HTTP/2.0\r\n"
            "Host:qyexpress.market.alicloudapi.com\r\n"
            "Authorization:APPCODE 0bccb88e30e14222bc8306b742d871b0 \r\n\r\n", rec_string_buf);
            printf("ExpNum: %s\n", rec_string_buf);
            //发送报文
            send(ali_sock_fd, buff, 1024, 0);
            sleep(2);
            printf("send ok\n");
            //获取正文的查询结果 获取JSON长度
            char mess[2000] = {0};
            int flag = 0;
            while(1){
                int ret = read(ali_sock_fd, mess+flag, 1);
                if(-1 == ret){
                    perror("读取内容失败");
                    return -1;
                }
                flag = flag + ret;
                if (strstr(mess, "\r\n\r\n") != NULL){
                    break;
                }
            }
            printf("mess ok\n");
            int mess_size =    atoi(strstr(mess, "Content-Length: ")+strlen("Content-Length: "));
            char mess_buff[10240] = {0};
            //获取JSON的查询结果
            read(ali_sock_fd, mess_buff, mess_size);
            printf("read ok\n");
            //CJSON解析
            for(int i = 0; i < mess_size; ++i){
                printf("%c", mess_buff[i]);
            }
            printf("\n");
            cJSON *root = cJSON_Parse(mess_buff);
            cJSON *data = cJSON_GetObjectItem(root, "data");    //L1
            cJSON *list = cJSON_GetObjectItem(data, "list");    //L2
            printf("json ok\n");
            int array_size = cJSON_GetArraySize(list);
            char buf[20] = {0};
            memset(buf, 0, sizeof(buf));
            OneNet_SendData(buf, "Express", 0);
            for (int i = 0; i < array_size; ++i){
                cJSON *array_item = cJSON_GetArrayItem(list,i);
                cJSON *status = cJSON_GetObjectItem(array_item,"status");
                cJSON *time = cJSON_GetObjectItem(array_item,"time");
                printf("status = %s\n", status->valuestring);
                printf("time = %s\n", time->valuestring);
                if(NULL != strstr(status->valuestring, "派件")){
                    OneNet_SendData(buf, "Express", 1);
                };
            }
            // rec_string_buf = malloc(sizeof(char) * 100);
            rec_string_buf[0] = 'N'; rec_string_buf[1] = '\0';
        }
#endif
        // Draw Screen
        sprintf(bufTmp, "%d" , temp);
        strcat (bufTmp, "C");    //Add unit
        if(ACSwitch){    //ON Green
            bitmap *bmAC1    = createBitmapWithInit(200,100,4,getColor(0,255,0,0));
            bitmap *bmTmp1   = createBitmapWithInit(200,100,4,getColor(0,255,0,0));
            bitmap *bmAdd1   = createBitmapWithInit(200,100,4,getColor(0,255,0,0));
            bitmap *bmSub1   = createBitmapWithInit(200,100,4,getColor(0,255,0,0));
            bitmap *bmVoice1 = createBitmapWithInit(50,50,4,getColor(0,255,0,0));
            fontPrint(my_font, bmAC1, 0, 0, bufAC, getColor(0,0,255,0), 0);
            fontPrint(my_font, bmTmp1, 0, 0, bufTmp, getColor(0,0,255,0), 0);
            fontPrint(my_font, bmAdd1, 50, 0, bufAdd, getColor(0,0,255,0), 0);
            fontPrint(my_font, bmSub1, 50, 0, bufSub, getColor(0,0,255,0), 0);
            fontPrint(my_mini_font, bmVoice1, 0, 0, bufVoice, getColor(0,0,255,0), 0);
            show_font_to_lcd(lcd->mp, 100, 70, bmAC1);
            show_font_to_lcd(lcd->mp, 100, 310, bmTmp1);
            show_font_to_lcd(lcd->mp, 500, 70, bmAdd1);
            show_font_to_lcd(lcd->mp, 500, 310, bmSub1);
            show_font_to_lcd(lcd->mp, 375, 225, bmVoice1);
            destroyBitmap(bmAC1);
            destroyBitmap(bmTmp1);
            destroyBitmap(bmAdd1);
            destroyBitmap(bmSub1);
            destroyBitmap(bmVoice1);
        }else{            //OFF Red
            bitmap *bmAC0 = createBitmapWithInit(200,100,4,getColor(255,0,0,0));
            bitmap *bmTmp0 = createBitmapWithInit(200,100,4,getColor(255,0,0,0));
            bitmap *bmAdd0 = createBitmapWithInit(200,100,4,getColor(255,0,0,0));
            bitmap *bmSub0 = createBitmapWithInit(200,100,4,getColor(255,0,0,0));
            bitmap *bmVoice0 = createBitmapWithInit(50,50,4,getColor(255,0,0,0));
            fontPrint(my_font, bmAC0, 0, 0, bufAC, getColor(0,0,255,0), 0);
            fontPrint(my_font, bmTmp0, 0, 0, bufTmp, getColor(0,0,255,0), 0);
            fontPrint(my_font, bmAdd0, 50, 0, bufAdd, getColor(0,0,255,0), 0);
            fontPrint(my_font, bmSub0, 50, 0, bufSub, getColor(0,0,255,0), 0);
            fontPrint(my_mini_font, bmVoice0, 0, 0, bufVoice, getColor(0,0,255,0), 0);
            show_font_to_lcd(lcd->mp, 100, 70, bmAC0);
            show_font_to_lcd(lcd->mp, 100, 310, bmTmp0);
            show_font_to_lcd(lcd->mp, 500, 70, bmAdd0);
            show_font_to_lcd(lcd->mp, 500, 310, bmSub0);
            show_font_to_lcd(lcd->mp, 375, 225, bmVoice0);
            destroyBitmap(bmAC0);
            destroyBitmap(bmTmp0);
            destroyBitmap(bmAdd0);
            destroyBitmap(bmSub0);
            destroyBitmap(bmVoice0);
        }
#if 0
        // lcd_draw_bmp(0, 0, "Cover");    //刷新背景
        // // 绘制圆指针    独立线程
        // if (0 < or_x && or_x < 800){            //移动
        //     for (int y = 0; y < LCD_HEIGHT; y++){
        //         for (int x = 0; x < LCD_WIDTH; x++){
        //             if ((x - ts_x)*(x - ts_x) + (y - ts_y)*(y - ts_y) <= r * r) {
        //                 lcd_draw_point(x, y, color);    //画圆指针
        //             }
        //         }
        //     }
        // }
#endif
        usleep((double)1/FPS * 1000 * 1000);
    }
    
    TcpClientClose();
    pthread_join(oneNetReceThread, NULL);
    pthread_join(oneNetSendThread, NULL);
    pthread_join(touchScreenThread, NULL);
    pthread_join(localCtrlTempThread, NULL);
    pthread_join(flushLogThread, NULL);
    lcd_close();
    fclose(log_stream);

    return 0;
}


#if 0
    while (1){
        sleep(1);
        if (250 < or_x && or_x < 650 && or_x != 0) {            //移动
            for (int y = 0; y < LCD_HEIGHT; y++){
                for (int x = 0; x < LCD_WIDTH; x++){
                    if ((x - ts_x)*(x - ts_x) + (y - ts_y)*(y - ts_y) <= r * r) {
                        lcd_draw_point(x, y, color);    //画圆
                    }
                }
            }
        }

        if (or_x < 100 && or_x != 0) {//左边缘向右滑动 上一张
            if (max_x > 300) {
                while (finger);
                --PicNum;
                if (PicNum < 1) PicNum = 10;
                switch (PicNum)
                {
                case 1:DrawPic_HWindows(0, 0, "1"); break;
                case 2:DrawPic_HWindows(0, 0, "2"); break;
                case 3:DrawPic_HWindows(0, 0, "3"); break;
                case 4:letter(); break;
                }
            }
        }
        if (or_x > 700) {//右边缘向左滑动 下一张
            if (min_x < 500 && min_x!=0 ) {
                while (finger);
                ++PicNum;
                if (PicNum > 10) PicNum = 1;
                switch (PicNum)
                {
                case 1:DrawPic_HWindows(0, 0, "1"); break;
                case 2:DrawPic_HWindows(0, 0, "2"); break;
                case 3:DrawPic_HWindows(0, 0, "3"); break;
                case 4:letter(); break;
                }
            }
        }
    }
#endif
