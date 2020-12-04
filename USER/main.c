#include <stdio.h>
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include <string.h>
#include "flash.h"
#include "cJSON.h"
#include "adc.h"
#include "oled.h"

#define IN1 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)
#define IN2 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)
#define IN3 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)
#define IN4 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)
#define IN5 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)
#define IN6 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)

/************************************************
 PA1--ADC1
 PA2--ADC0
 PA5		SCL
 PA7		SDA
 PB3		IN1
 PB6		IN2
 PB9		IN3
 PB10		IN4
 PB11		IN5
 PB12		IN6
 
************************************************/

extern u8 rec_buff[128];
extern unsigned int counter;

#define FLASH_SAVE_ADDR 0X08010000 //设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
#define SIZE 10

#define LEFT_POSITIVE \
	{                 \
		GPB7 = 1;     \
		GPB8 = 0;     \
	}
#define LEFT_NEGATIVE \
	{                 \
		GPB7 = 0;     \
		GPB8 = 1;     \
	}
#define RIGHT_POSITIVE \
	{                  \
		GPB9 = 1;      \
		GPB10 = 0;     \
	}
#define RIGHT_NEGATIVE \
	{                  \
		GPB9 = 0;      \
		GPB10 = 1;     \
	}

#define LEFT_STOP \
	{             \
		GPB7 = 0; \
		GPB8 = 0; \
	}
#define RIGHT_STOP \
	{              \
		GPB9 = 0;  \
		GPB10 = 0; \
	}

#define DEBUG_LINE printf("%d\n", __LINE__);

// #define VOL_BOUND 5000

// #define EVOLUTION 900
// #define ONE_CIRCLE_MM 40
// #define FREQ 72000000

// #define NORMAL_TRANSFER 150	//正转圈数
// #define REVERSE_TRANSFER 150 //反转圈数
// #define DISPLAY_BUF_SIZE 20

#define ABS(a, b) ((a) >= (b) ? ((a) - (b)) : ((b) - (a)))
enum NEXT_MOTOR_STATUS motor_status = 0;
u8 number[10];
u8 num = 0;
u8 datatemp[SIZE];
u8 TEXT_Buffer[5] = {0};
int number_result = 300;
int mul = 1;
int flag0 = 0;
int flag1 = 0;
int set_status_flag = 0;
uint16_t spd_km_h_rank = 0;		//      m/h
uint16_t spd_km_h_rank_old = 0; //
uint16_t spd_index = 1;
int rpm = 0;
int old_rpm = 0;
int rotate_total = 0; //大于0正转，小于0反转
int enable = 0;
uint16_t direction = 1;
uint16_t old_direction = 1;
uint16_t mode = 1;
uint16_t reset_position_flag = 0;
uint16_t flag_10s = 3;
uint32_t motor_time = 600;
uint32_t stop_time = 0;
uint32_t vol_threshold = 0;
uint32_t motor_speed = 0; //vol_threshold单位是mV,motor_speed单位是百分比
uint32_t motor_time_counter = 0;
uint32_t stop_time_counter = 0;
uint8_t shutdown_flag = 0;
int pulse_count = 0;
int last_pulse_count = 0;
char display_buffer[DISPLAY_BUF_SIZE] = {0};
uint8_t display_mode = 0;
extern unsigned int count_times;

/*
 *		km/h		r/min		r/s      pulse(500)/s
 * 		  1			  417		6.95		6950				
 * 		  2			  833		13.8833		13883
 * 		  3			  1250		20.8333		20833
 * 		  4			  1667		27.7833		27783
 * 		  5			  2083		34.7167		34717
 * 		  6			  2500		41.6667		41667
 * 
 * 		695*spd_100m_h
 * 
 * */

void process(void)
{
	u8 *p = rec_buff;
	u8 flag = 0;
	cJSON *items = NULL, *item = NULL;
	if (rec_buff[0] == '\0')
	{
		return;
	}

	items = cJSON_Parse((const char *)rec_buff);
	if (items == NULL)
	{
		printf("invalid input rec_buff\n");
	}

	item = cJSON_GetObjectItem(items, "rpm");
	if (item != NULL)
	{
		flag = 1;
		TEXT_Buffer[0] = ((item->valueint) & 0xff00) >> 8;
		TEXT_Buffer[1] = ((item->valueint) & 0x00ff);
		old_rpm = rpm;

		rpm = item->valueint;
		// stop_time_counter = stop_time;
		STMFLASH_Write(FLASH_SAVE_ADDR, (u16 *)TEXT_Buffer, 4);
		printf("getting rpm:%d\r\n", item->valueint);
	}

	item = cJSON_GetObjectItem(items, "rotate_total");
	if (item != NULL)
	{
		flag = 1;
		TEXT_Buffer[0] = ((item->valueint) & 0xff00) >> 8;
		TEXT_Buffer[1] = ((item->valueint) & 0x00ff);
		rotate_total = item->valueint;
		if (rotate_total > 0)
		{
			count_times = 0;
			enable = 1;
		}
		STMFLASH_Write(FLASH_SAVE_ADDR + 4, (u16 *)TEXT_Buffer, 4);
		printf("getting vol_threshold:%d\r\n", item->valueint);
	}

	item = cJSON_GetObjectItem(items, "direction");
	if (item != NULL)
	{
		flag = 1;
		direction = item->valueint;
		if (direction == 0 || direction == 1)
		{
			count_times = 0;
			enable = 1;
			STMFLASH_Write(FLASH_SAVE_ADDR + 8, (u16 *)&direction, 4);
			printf("getting direction:%d\r\n", direction);
		}
		else
		{
			/* code */
			printf("ERROR direction number!\r\n");
		}
	}

	item = cJSON_GetObjectItem(items, "mode");
	if (item != NULL)
	{
		flag = 1;
		mode = item->valueint;
		if (mode >= 1 && mode <= 2)
		{
			TEXT_Buffer[0] = 0;
			TEXT_Buffer[1] = ((item->valueint) & 0x00ff);
			STMFLASH_Write(FLASH_SAVE_ADDR + 12, (u16 *)&TEXT_Buffer, 1);
			printf("getting mode:%d\r\n", mode);
		}
		else
		{
			printf("ERROR mode number!\r\n");
		}
	}

	item = cJSON_GetObjectItem(items, "spd_km_h_rank");
	if (item != NULL)
	{
		flag = 1;
		spd_km_h_rank_old = spd_km_h_rank;
		spd_km_h_rank = item->valueint; //spd_100m_h = 500  -->  500m/h
		TEXT_Buffer[0] = 0;
		TEXT_Buffer[1] = ((item->valueint) & 0x00ff);
		STMFLASH_Write(FLASH_SAVE_ADDR + 16, (u16 *)&TEXT_Buffer, 1);
		printf("getting spd_km_h_rank:%d\r\n", spd_km_h_rank);
	}

	// item = cJSON_GetObjectItem(items, "spd_km_h_rank_direction");
	// if (item != NULL)
	// {
	// 	flag = 1;
	// 	direction = item->valueint;
	// 	TEXT_Buffer[0] = 0;
	// 	TEXT_Buffer[1] = ((item->valueint) & 0x00ff);
	// 	STMFLASH_Write(FLASH_SAVE_ADDR + 20, (u16 *)&TEXT_Buffer, 1);
	// 	printf("getting spd_km_h_rank_dir:%d\n", direction);
	// }

	item = cJSON_GetObjectItem(items, "spd_index");
	if (item != NULL)
	{
		flag = 1;
		// TEXT_Buffer[0]=((item->valueint) & 0xff00)>>8;
		// TEXT_Buffer[1]=((item->valueint) & 0x00ff);
		// spd_km_h_rank_old = spd_km_h_rank;
		if (item->valueint >= 1 && item->valueint <= 6)
		{
			spd_index = item->valueint;
			TEXT_Buffer[0] = 0;
			TEXT_Buffer[1] = ((item->valueint) & 0x00ff);
			STMFLASH_Write(FLASH_SAVE_ADDR + 24, (u16 *)spd_index, 1);
			printf("getting spd_index:%d\n", spd_index);
		}
	}

	item = cJSON_GetObjectItem(items, "reset_position");
	if (item != NULL)
	{
		flag = 1;
		reset_position_flag = item->valueint;
		TEXT_Buffer[0] = 0;
		TEXT_Buffer[1] = ((item->valueint) & 0x00ff);
		STMFLASH_Write(FLASH_SAVE_ADDR + 28, (u16 *)&reset_position_flag, 1);
		printf("getting reset_position_flag:%d\n", reset_position_flag);

		if (reset_position_flag == 1)
		{
			reset_position_flag = 0;
			pulse_count = 0;
			enable = 0;
		}
	}

	item = cJSON_GetObjectItem(items, "enable");
	if (item != NULL)
	{
		flag = 1;
		// TEXT_Buffer[0]=((item->valueint) & 0xff00)>>8;
		// TEXT_Buffer[1]=((item->valueint) & 0x00ff);
		// spd_km_h_rank_old = spd_km_h_rank;
		if (item->valueint == 1 || item->valueint == 0)
		{
			enable = item->valueint;
			TEXT_Buffer[0] = 0;
			TEXT_Buffer[1] = ((item->valueint) & 0x00ff);
			STMFLASH_Write(FLASH_SAVE_ADDR + 32, (u16 *)enable, 1);
			printf("getting enable:%d\n", enable);
		}
	}

	printf("-------------\n%s\n old_dir:%d\n-------------\n", rec_buff,old_direction);
	memset(rec_buff, 0, 128);
	if (items != NULL)
	{
		cJSON_Delete(items);
	}
	if (flag == 0)
	{
		printf("no useful information!\n");
	}
}
void log_out(void)
{
	printf("------------ADC----------------\r\n");
	printf("adc1: %d ,adc2: %d \r\n", Get_Adc_Average(ADC_Channel_1, 5), Get_Adc_Average(ADC_Channel_0, 5));
	printf("Voltage1: %d mV,Voltage2: %d mV\r\n", Get_Adc_Average(ADC_Channel_1, 5) * 3300 * 4 / 4096, Get_Adc_Average(ADC_Channel_0, 5) * 3300 * 4 / 4096);
	printf("-----------STATUS---------------\r\n");
	printf("         %s        \n", motor_status == RUN ? "RUN" : "STOP");
	printf("-------------------------------\r\n");
}
int status = 0;
int counter1 = 0;
int counter2 = 0;
int counter3 = 0;

void display(uint8_t mode_d)
{
	if (mode_d == 0)
	{
		memset(display_buffer, 0, DISPLAY_BUF_SIZE);
		sprintf(display_buffer, "en   :%d dir:%d", enable, direction);
		// OLED_ShowString(0, 0, "                ", 16);
		OLED_ShowString(0, 0, display_buffer, 16);

		memset(display_buffer, 0, DISPLAY_BUF_SIZE);
		sprintf(display_buffer, "rank :%d mode:%d", spd_index, mode);
		OLED_ShowString(0, 2, display_buffer, 16);

		memset(display_buffer, 0, DISPLAY_BUF_SIZE);
		sprintf(display_buffer, "%d", pulse_count);
		OLED_ShowString(0, 4, display_buffer, 16);

		memset(display_buffer, 0, DISPLAY_BUF_SIZE);
		sprintf(display_buffer, "%d", old_direction/*spd_km_h_rank*/);
		OLED_ShowString(56+16, 4, display_buffer, 16);

		memset(display_buffer, 0, DISPLAY_BUF_SIZE);
		sprintf(display_buffer, "shut:%d", shutdown_flag);
		OLED_ShowString(0, 6, display_buffer, 16);

		memset(display_buffer, 0, DISPLAY_BUF_SIZE);
		sprintf(display_buffer, " %d%d%d%d%d%d", IN1,IN2,IN3,IN4,IN5,IN6);
		OLED_ShowString(64, 6, display_buffer, 16);
	}
	else if (mode_d == 1)
	{
	}
	else if (mode_d == 10)
	{
		memset(display_buffer, 0, DISPLAY_BUF_SIZE);
		sprintf(display_buffer, "%ds", flag_10s);
		OLED_ShowString(0, 0, display_buffer, 16);
	}
}
void direction_contorl(void)
{
	if (IN1 == 1) //根据实际调整方向
	{
		direction = 1;
	}

	if (IN6 == 1)
	{
		direction = 0;
	}
}
int main(void)
{
	unsigned long i = 0;
	uint16_t rank[7] = {0, 6950, 13883, 20833, 27783, 34717, 41667}; //6个档位的脉冲数，1000个脉冲对应一圈（实际调整为500个脉冲）
	delay_init();													 //延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);												 //串口初始化为115200
	LED_Init();														 //LED端口初始化
	Adc_Init();
	OLED_Init(); //??????OLED
	OLED_Clear();
	// TIM3_Int_Init(10000, 7199); //72 000 000 /7199
	printf("%s:%d\n", __FILE__, __LINE__);
	//	TIM4_Int_Init(9000, 0);
	//(9+1)*(71+1)/72 000 000 = 1/100 000 s = 10^-5s = 10^-2 ms = 10 us
	//(9+1)*(719+1)/72000000 = 7200/72000000= 10khz
	//(899+1)*1/72000000 = 900/72000000= 1/80000  0.0000125 = 0.0125ms = 12.5us
	//72 000 000 /41667 =

	STMFLASH_Read(FLASH_SAVE_ADDR, (u16 *)datatemp, 2);
	old_rpm = rpm;
	rpm = (datatemp[0] << 8) + datatemp[1];
	printf("Read from flash  { rpm }:%d\n", rpm);

	STMFLASH_Read(FLASH_SAVE_ADDR + 4, (u16 *)datatemp, 2);
	rotate_total = (datatemp[0] << 8) + datatemp[1];
	printf("Read from flash  { rotate_total }:%d\n", rotate_total);

	// STMFLASH_Read(FLASH_SAVE_ADDR + 8, (u16 *)datatemp, 2);
	// direction = (datatemp[0] << 8) + datatemp[1];
	// printf("Read from flash  { direction }:%d\n", direction);
	TIM4_Int_Init(9000, 0);
	if (direction > 0)
	{
		count_times = 0;
		enable = 1;
	}
	/*
		圈数计算清零
		位置存储
		红外管对射检测
		遮挡取消后方向改变
		重置位置按钮
		(加速到达下一落脚点)

		1km/h  --  1000/3600 (m/s) -- 1000000/3600 (mm/s)  = 277.78 (mm/s)  ≈ 6.9圈/s ≈ 277.78*500
	*/
	DEBUG_LINE
	// while (flag_10s)
	// {
	// 	process();
	// 	display(10);
	// }
	DEBUG_LINE
	flag_10s = 100;
	while (1)
	{
		process();
		display(0);
		// direction_contorl();
		switch (mode)
		{
		case 1: //指定转速运行
			spd_km_h_rank = FREQ / rank[spd_index];
			break;
		case 2: //按键控制

			break;

		default:
			break;
		}
		// if (pulse_count >= EVOLUTION * NORMAL_TRANSFER)
		// {
		// 	shutdown_flag = 1; //根据实际调整方向
		// 	// direction = 1;
		// 	if(direction!=old_direction)
		// 	{
		// 		old_direction = direction;
		// 		shutdown_flag = 0;
		// 	}
		// }
		// if (pulse_count < 0)
		// {
		// 	shutdown_flag = 1; //根据实际调整方向
		// 	// direction = 0;
		// 	if(direction!=old_direction)
		// 	{
		// 		old_direction = direction;
		// 		shutdown_flag = 0;
		// 	}
		// }
		if (ABS(last_pulse_count, pulse_count) >= 1000)
		{
			//存儲數據
			//更新上次記錄
			last_pulse_count = pulse_count;
			printf("write to simulate flash:%d\n", last_pulse_count);
		}

		if (spd_km_h_rank != spd_km_h_rank_old)
		{
			TIM_SetAutoreload(TIM4, spd_km_h_rank);
			spd_km_h_rank_old = spd_km_h_rank;
		}

		i++;
		if (i > 800)
		{
			i = 0;
			printf("pulse_count:%d\r\n", pulse_count);
		}
	}
}
