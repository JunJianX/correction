#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK��ӢSTM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define GPB7 PBout(7)// PB5
#define GPB8 PBout(8)// PB6	
#define GPB9 PBout(9)// PB7
#define GPB10 PBout(10)// PE8	
#define GPC13 PCout(13)

// #define IN0  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)//????0
// #define IN1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)//????1

void LED_Init(void);//��ʼ��

		 				    
#endif
