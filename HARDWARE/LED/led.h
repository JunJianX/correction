#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK精英STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define GPB7 PBout(7)// PB5
#define GPB8 PBout(8)// PB6	
#define GPB9 PBout(9)// PB7
#define GPB10 PBout(10)// PE8	
#define GPC13 PCout(13)

// #define IN0  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)//????0
// #define IN1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)//????1

void LED_Init(void);//初始化

		 				    
#endif
