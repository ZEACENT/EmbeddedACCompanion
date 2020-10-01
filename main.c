#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "lcd.h"
#include "font.h"
#include "EdpKit.h"
#include "ConnectOneNet.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

extern int ts_x;
extern int ts_y;
extern int or_x;
extern int or_y;
extern int max_x , min_x ;
extern int max_y , min_y ;
extern int finger;
unsigned int temp = 26;
char* RecvBuf;

void  * fun1(void * t) {
	while (1) {
		get_xy();
	}
}

void *OneNetReacv(void *arg){
	char RecvBuffer[100] = {0};  //保存接收的字符串
	memset(RecvBuf, '\0', sizeof(RecvBuf));
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
		//写到全局变量
		for(i = 0; i < strlen(RecvBuffer); ++i){
			RecvBuf[i] = RecvBuffer[i];
		}
		RecvBuf[i] = '\0';
    	// unlock
    	if(pthread_mutex_unlock(&mutex) != 0){
			perror("pthread_mutex_unlock");
			exit(EXIT_FAILURE);
    	}
		printf("RecvBufferLen: %d\n", strlen(RecvBuffer));
		printf("RecvBuffer: %s\n", RecvBuffer);
		printf("RecvBuf in thread: %s\n", RecvBuf);
	}
}

void *OneNetSendTemp(void *arg){	//onenet temp thread
	char buf[20] = {0};
	while (1){
		memset(buf ,0 ,sizeof(buf));
		OneNet_SendData(buf, "Temp", temp);
		sleep(3);
	}
}

void adjustTemp(unsigned int* temp, int num){
	*temp += num;
	*temp = *temp<16?16:*temp;
	*temp = *temp>36?36:*temp;
}

void delay(int x) {
	int i = x;
	int j = x;
	for (; i > 0; i--)
		for (; j > 0; j--);
}

int main(int argc, char **argv) {
	pthread_t thread;	//onenetReacv
	pthread_t threadOneNetSend;	//onenetSend
	OneNet_Init();
	printf("HardWare Init Success!\n");
	RecvBuf = malloc(sizeof(char) * 100);
	printf("malloc success\n");
	pthread_create(&thread, NULL, OneNetReacv, NULL);
	pthread_create(&threadOneNetSend, NULL, OneNetSendTemp, NULL);

	//创建一个用来连接阿里云的套接字
	int sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if (-1 == sock_fd){
		perror("socket 失败");
		return -1;
	}
	int port = 50001;
	struct sockaddr_in myaddr;
	myaddr.sin_family = AF_INET;//IP地址协议
	myaddr.sin_port = htons(port);//端口号
	myaddr.sin_addr.s_addr = inet_addr("172.16.6.222");		//本机地址
	int ret = bind(sock_fd,(struct sockaddr *)&myaddr,16);
	if (-1 == ret){
		perror("bind fail");
		return -1;
	}else if (0 == ret){
		printf("绑定成功！\n");
	}
	struct sockaddr_in alicloud_addr;
	alicloud_addr.sin_family = AF_INET;//IP地址协议
	alicloud_addr.sin_port = htons(80);//网页的端口号
	alicloud_addr.sin_addr.s_addr = inet_addr("47.104.94.62");
	ret = connect(sock_fd,(struct sockaddr *)&alicloud_addr,16);
	if (-1 == ret){
		perror("connect fail");
		return -1;
	}else if (0 == ret){
		printf("连接成功！\n");
	}

	pthread_t thread1;	//touchscreen
	int i;
	ret = pthread_create(&thread1, NULL, fun1, NULL);	//触摸屏线程
	if (ret == 0) {
		printf("touchscreen thread create success!\r\n");
	}
	else {
		printf("touchscreen thread create fail!\r\n");
	}

	int PicNum = 1;
	int color = 0x00ff0000;	//如影随形颜色
	int r = 30;	//半径

	struct LcdDevice* lcd = lcd_open();//打开显示屏
	lcd_draw_bmp(0, 0, "Cover");//显示封面
	// sleep(1);

	//打开字体	
	font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
	printf("fontload ok\n");
	char bufAC[] = "空调";
	char *bufTmp = "26C";
	char bufAdd[] = "+";
	char bufSub[] = "-";

	//字体大小的设置
	fontSetSize(f, 100);
	//创建一个画板（点阵图） RED
	bitmap *bmAC = createBitmapWithInit(200,100,4,getColor(255,0,0,0));
	bitmap *bmTmp = createBitmapWithInit(200,100,4,getColor(255,0,0,0));
	bitmap *bmAdd = createBitmapWithInit(200,100,4,getColor(255,0,0,0));
	bitmap *bmSub = createBitmapWithInit(200,100,4,getColor(255,0,0,0));
	//将字体写到点阵图上
	fontPrint(f, bmAC, 0, 0, bufAC, getColor(0,0,255,0), 0);
	fontPrint(f, bmTmp, 0, 0, bufTmp, getColor(0,0,255,0), 0);
	fontPrint(f, bmAdd, 50, 0, bufAdd, getColor(0,0,255,0), 0);
	fontPrint(f, bmSub, 50, 0, bufSub, getColor(0,0,255,0), 0);
	//把字体框输出到LCD屏幕上
	show_font_to_lcd(lcd->mp, 100, 70, bmAC);
	show_font_to_lcd(lcd->mp, 100, 310, bmTmp);
	show_font_to_lcd(lcd->mp, 500, 70, bmAdd);
	show_font_to_lcd(lcd->mp, 500, 310, bmSub);
	//关闭画板
	destroyBitmap(bmAC);
	destroyBitmap(bmTmp);
	destroyBitmap(bmAdd);
	destroyBitmap(bmSub);

	int ACSwitch = 0;
	char buff[1024] = {0};
	char str[4];	//空调温度临时字串

	while (1){
		sprintf(str, "%d" , temp);
		strcat (str, "C");	//附加单位
		bufTmp = str;
		if(ts_x>100 && ts_x<300 && ts_y>70 && ts_y<170){	//本地开关
			while(ts_x>100 && ts_x<300 && ts_y>70 && ts_y<170);	//等待释放
			ACSwitch = 0==ACSwitch?1:0;
			printf("switch\n");
		}
		// OneNet
		if('1' == (RecvBuf)[0] && 1 == strlen(RecvBuf)){	//远程开关
			ACSwitch = 1;
			printf("switch on\n");
			strcpy(RecvBuf, "N");
		}else if('0' == (RecvBuf)[0] && 1 == strlen(RecvBuf)){
			ACSwitch = 0;
			printf("switch off\n");
			strcpy(RecvBuf, "N");
		}else if('N' != (RecvBuf)[0] && 6 < strlen(RecvBuf)){
			printf("check exp\n");
			sprintf(buff, "GET  /composite/queryexpress?number=%s HTTP/2.0\r\n"
			"Host:qyexpress.market.alicloudapi.com\r\n"
			"Authorization:APPCODE 0bccb88e30e14222bc8306b742d871b0 \r\n\r\n", RecvBuf);
			printf("ExpNum: %s\n", RecvBuf);
			//发送报文
			send(sock_fd, buff, 1024, 0);
			sleep(2);
			printf("send ok\n");
			//获取正文的查询结果 获取JSON长度
			char mess[2000] = {0};
			int flag = 0;
			while(1){
				int ret = read(sock_fd, mess+flag, 1);
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
			int mess_size =	atoi(strstr(mess, "Content-Length: ")+strlen("Content-Length: "));
			char mess_buff[10240] = {0};
			//获取JSON的查询结果
			read(sock_fd, mess_buff, mess_size);
			printf("read ok\n");
			//CJSON解析
			for(int i = 0; i < mess_size; ++i){
				printf("%c", mess_buff[i]);
			}
			printf("\n");
			cJSON *root = cJSON_Parse(mess_buff);
			cJSON *data = cJSON_GetObjectItem(root, "data");	//L1
			cJSON *list = cJSON_GetObjectItem(data, "list");	//L2
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
			RecvBuf = malloc(sizeof(char) * 100);
			RecvBuf[0] = 'N'; RecvBuf[1] = '\0';
		}	//END exp
		// Draw Screen
		delay(1000000);
		if(ACSwitch){	//ON Green
			bitmap *bmAC1 = createBitmapWithInit(200,100,4,getColor(0,255,0,0));
			bitmap *bmTmp1 = createBitmapWithInit(200,100,4,getColor(0,255,0,0));
			bitmap *bmAdd1 = createBitmapWithInit(200,100,4,getColor(0,255,0,0));
			bitmap *bmSub1 = createBitmapWithInit(200,100,4,getColor(0,255,0,0));
			fontPrint(f, bmAC1, 0, 0, bufAC, getColor(0,0,255,0), 0);
			fontPrint(f, bmTmp1, 0, 0, bufTmp, getColor(0,0,255,0), 0);
			fontPrint(f, bmAdd1, 50, 0, bufAdd, getColor(0,0,255,0), 0);
			fontPrint(f, bmSub1, 50, 0, bufSub, getColor(0,0,255,0), 0);
			show_font_to_lcd(lcd->mp, 100, 70, bmAC1);
			show_font_to_lcd(lcd->mp, 100, 310, bmTmp1);
			show_font_to_lcd(lcd->mp, 500, 70, bmAdd1);
			show_font_to_lcd(lcd->mp, 500, 310, bmSub1);
			destroyBitmap(bmAC1);destroyBitmap(bmTmp1);destroyBitmap(bmAdd1);destroyBitmap(bmSub1);
			// sleep(1);
		}else{			//OFF Red
			bitmap *bmAC0 = createBitmapWithInit(200,100,4,getColor(255,0,0,0));
			bitmap *bmTmp0 = createBitmapWithInit(200,100,4,getColor(255,0,0,0));
			bitmap *bmAdd0 = createBitmapWithInit(200,100,4,getColor(255,0,0,0));
			bitmap *bmSub0 = createBitmapWithInit(200,100,4,getColor(255,0,0,0));
			fontPrint(f, bmAC0, 0, 0, bufAC, getColor(0,0,255,0), 0);
			fontPrint(f, bmTmp0, 0, 0, bufTmp, getColor(0,0,255,0), 0);
			fontPrint(f, bmAdd0, 50, 0, bufAdd, getColor(0,0,255,0), 0);
			fontPrint(f, bmSub0, 50, 0, bufSub, getColor(0,0,255,0), 0);
			show_font_to_lcd(lcd->mp, 100, 70, bmAC0);
			show_font_to_lcd(lcd->mp, 100, 310, bmTmp0);
			show_font_to_lcd(lcd->mp, 500, 70, bmAdd0);
			show_font_to_lcd(lcd->mp, 500, 310, bmSub0);
			destroyBitmap(bmAC0);destroyBitmap(bmTmp0);destroyBitmap(bmAdd0);destroyBitmap(bmSub0);
			// sleep(1);
		}
		// 	Adjust Temp
		if(ts_x>500 && ts_x<700 && ts_y>70 && ts_y<170){ //+
			while(ts_x>500 && ts_x<700 && ts_y>70 && ts_y<170);	//等待释放
			adjustTemp(&temp, 1);
		}
		if(ts_x>500 && ts_x<700 && ts_y>310 && ts_y<410){ //-
			while(ts_x>500 && ts_x<700 && ts_y>310 && ts_y<410);	//等待释放
			adjustTemp(&temp, -1);
		}
		// lcd_draw_bmp(0, 0, "Cover");	//刷新背景
		// // 绘制圆指针	独立线程
		// if (0 < or_x && or_x < 800){			//移动
		// 	for (int y = 0; y < LCD_HEIGHT; y++){
		// 		for (int x = 0; x < LCD_WIDTH; x++){
		// 			if ((x - ts_x)*(x - ts_x) + (y - ts_y)*(y - ts_y) <= r * r) {
		// 				lcd_draw_point(x, y, color);	//画圆指针
		// 			}
		// 		}
		// 	}
		// }
		delay(1000000);	//屏幕刷新延迟
	}
	
	
	// while (1){
	// 	sleep(1);
	// 	if (250 < or_x && or_x < 650 && or_x != 0) {			//移动
	// 		for (int y = 0; y < LCD_HEIGHT; y++){
	// 			for (int x = 0; x < LCD_WIDTH; x++){
	// 				if ((x - ts_x)*(x - ts_x) + (y - ts_y)*(y - ts_y) <= r * r) {
	// 					lcd_draw_point(x, y, color);	//画圆
	// 				}
	// 			}
	// 		}
	// 	}

	// 	if (or_x < 100 && or_x != 0) {//左边缘向右滑动 上一张
	// 		if (max_x > 300) {
	// 			while (finger);
	// 			--PicNum;
	// 			if (PicNum < 1) PicNum = 10;
	// 			switch (PicNum)
	// 			{
	// 			case 1:DrawPic_HWindows(0, 0, "1"); break;
	// 			case 2:DrawPic_HWindows(0, 0, "2"); break;
	// 			case 3:DrawPic_HWindows(0, 0, "3"); break;
	// 			case 4:letter(); break;
	// 			}
	// 		}
	// 	}
	// 	if (or_x > 700) {//右边缘向左滑动 下一张
	// 		if (min_x < 500 && min_x!=0 ) {
	// 			while (finger);
	// 			++PicNum;
	// 			if (PicNum > 10) PicNum = 1;
	// 			switch (PicNum)
	// 			{
	// 			case 1:DrawPic_HWindows(0, 0, "1"); break;
	// 			case 2:DrawPic_HWindows(0, 0, "2"); break;
	// 			case 3:DrawPic_HWindows(0, 0, "3"); break;
	// 			case 4:letter(); break;
	// 			}
	// 		}
	// 	}
	// }


	//wont run
	TcpClientClose();
	pthread_join(thread, NULL);
	pthread_join(threadOneNetSend, NULL);
	pthread_join(thread1, NULL);
	lcd_close();

	return 0;
}