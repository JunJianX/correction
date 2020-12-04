#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
enum NEXT_MOTOR_STATUS{
	RUN = 0,
	STOP,
	NULL_ACTION
};
#define VOL_BOUND 5000

#define EVOLUTION 900
#define ONE_CIRCLE_MM 40
#define FREQ 72000000

#define NORMAL_TRANSFER 15	//正转圈数
#define REVERSE_TRANSFER 15 //反转圈数
#define DISPLAY_BUF_SIZE 20

#define RANK_MAX 3
#define RANK_MIN 1
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
#endif

