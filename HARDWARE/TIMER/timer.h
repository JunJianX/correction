#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
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

#define NORMAL_TRANSFER 15	//��תȦ��
#define REVERSE_TRANSFER 15 //��תȦ��
#define DISPLAY_BUF_SIZE 20

#define RANK_MAX 3
#define RANK_MIN 1
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
#endif

