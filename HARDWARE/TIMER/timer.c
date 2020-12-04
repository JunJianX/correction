#include "timer.h"
#include "led.h"

unsigned int counter = 0;
extern int flag0, flag1;
int led_counter = 0;
int led_flag = 0;

extern uint32_t motor_time_counter, stop_time_counter;
extern enum NEXT_MOTOR_STATUS motor_status;
extern uint32_t motor_time, stop_time;
extern int spd_km_h_rank; //      m/h
extern int spd_km_h_rank_old;
void TIM4_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //Ê±ÖÓÊ¹ÄÜ

	TIM_TimeBaseStructure.TIM_Period = arr;						//ÉèÖÃÔÚÏÂÒ»¸ö¸üÐÂÊÂ¼þ×°Èë»î¶¯µÄ×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷ÖÜÆÚµÄÖµ	 ¼ÆÊýµ½5000Îª500ms
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//ÉèÖÃÓÃÀ´×÷ÎªTIMxÊ±ÖÓÆµÂÊ³ýÊýµÄÔ¤·ÖÆµÖµ  10KhzµÄ¼ÆÊýÆµÂÊ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//ÉèÖÃÊ±ÖÓ·Ö¸î:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIMÏòÉÏ¼ÆÊýÄ£Ê½
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //¸ù¾ÝTIM_TimeBaseInitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯TIMxµÄÊ±¼ä»ùÊýµ¥Î»

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //Ê¹ÄÜÖ¸¶¨µÄTIM3ÖÐ¶Ï,ÔÊÐí¸üÐÂÖÐ¶Ï

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;			  //TIM3ÖÐ¶Ï
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //ÏÈÕ¼ÓÅÏÈ¼¶0¼¶
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  //´ÓÓÅÏÈ¼¶3¼¶
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQÍ¨µÀ±»Ê¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);							  //¸ù¾ÝNVIC_InitStructÖÐÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯ÍâÉèNVIC¼Ä´æÆ÷

	TIM_Cmd(TIM4, ENABLE); //Ê¹ÄÜTIMxÍâÉè
}
extern int status;
extern int set_status_flag;
extern int rpm, old_rpm;
extern int direction;
extern uint16_t old_direction;
extern int enable;
unsigned int count_times;
extern uint16_t mode;
extern int pulse_count;
extern uint8_t shutdown_flag;
void TIM4_IRQHandler(void) //TIM3ÖÐ¶Ï
{
	static uint8_t div_2 = 0;
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //¼ì²éÖ¸¶¨µÄTIMÖÐ¶Ï·¢ÉúÓë·ñ:TIM ÖÐ¶ÏÔ´
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update); //Çå³ýTIMxµÄÖÐ¶Ï´ý´¦ÀíÎ»:TIM ÖÐ¶ÏÔ´
		// GPB7 = direction ? 1 : 0;
		if(direction)
		{
			GPB7 = 1;
		}else
		{
			GPB7 = 0;
		}
		
		if (old_direction != direction)
		{
			old_direction = direction;
			shutdown_flag = 0;
		}
		if (shutdown_flag == 0)
		{
			GPB8 = 1 - GPB8;
			div_2 = 1 - div_2;
			if (div_2 == 1)
			{
				if(direction==0)
				{
					pulse_count--;
				}else
				{
					pulse_count++;
				}
				
				if (pulse_count >= EVOLUTION * NORMAL_TRANSFER)
				{
					shutdown_flag = 1; //����ʵ�ʵ�������
				}
				else if (pulse_count < 1)
				{
					shutdown_flag = 1; //����ʵ�ʵ�������
				}
			}
		}
		
	}
	
}
void TIM3_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //??????

	//?????TIM3?????
	TIM_TimeBaseStructure.TIM_Period = arr;						//??????????????????????????????????????????
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//???????????TIMx????????????????
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//?????????:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM?????????
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				//?????????????????TIMx??????????��

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //????????TIM3?��?,?????????��?

	//?��??????NVIC????
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;			  //TIM3?��?
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //????????0??
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  //???????3??
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQ????????
	NVIC_Init(&NVIC_InitStructure);							  //?????NVIC?????

	TIM_Cmd(TIM3, ENABLE); //???TIMx
}
extern uint16_t flag_10s;
extern uint8_t display_mode;
void TIM3_IRQHandler(void) //TIM3?��?
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //???TIM3?????��???????
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //???TIMx?????��???

		if (flag_10s > 0)
		{
			flag_10s--;
		}

		display_mode++;
	}
}

void TIM3_PWM_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);						//?????��?��?��3?��??
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); //????GPIO???��??AFIO???????????��?��??

	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3??��???????  TIM3_CH2->PB5

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = arr;						//?��???????????��??????����????????��?????����???????��????????
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//?��??????����??TIMx?��?????????????�衤?????
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//?��???��??��???:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM?��??????????
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				//?��??TIM_TimeBaseInitStruct?????��????????????TIMx???��???��??????

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  //?????��?��?��????:TIM?????��???��??????2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //��???????????
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //????????:TIM????��?????????
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);					  //?��??T???��???????????????��TIM3 OC2

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); //????TIM3??CCR2?????�����????????��

	TIM_Cmd(TIM3, ENABLE); //????TIM3
}
