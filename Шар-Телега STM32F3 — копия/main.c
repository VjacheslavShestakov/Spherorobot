#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "stm32f3_discovery_lsm303dlhc.h"
#include "stm32f3_discovery_l3gd20.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////////////
// Functions
void InitTimer2(int );
void InitTimer3(int );
void InitTimer4(int );
void InitPorts(void);
void ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void InitUart4(void);
void USART_Transmit(uint8_t ch);
void USART_Transmit_str( char *str);
void led_on(void);
void led_off(void);
void obrabotka_bufera(void);
void Stop(void);
void Freq1(int );
void Freq2(int );
void Freq3(int );
void Motor_All(int, int, int, int, int, int);
void InitPriority(void);


int i=0;
int del=72;

//////////////////////////////////////////////////////////////////////////////////
// for motors
uint16_t PB = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2|GPIO_Pin_3| GPIO_Pin_4|GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14, // PB

K1_EN=GPIO_Pin_0,
K1_RESET=GPIO_Pin_1,
K1_SLEEP=GPIO_Pin_2,
K1_DIR=GPIO_Pin_3,
K1_FLT=GPIO_Pin_4,

K2_EN=GPIO_Pin_5,
K2_RESET=GPIO_Pin_6,
K2_SLEEP=GPIO_Pin_7,
K2_DIR=GPIO_Pin_8,
K2_FLT=GPIO_Pin_9,

K3_EN=GPIO_Pin_10,
K3_RESET=GPIO_Pin_11,
K3_SLEEP=GPIO_Pin_12,
K3_DIR=GPIO_Pin_13,
K3_FLT=GPIO_Pin_14,

TEMP=0;

/////////////////////////////////////////////////////////////////////////////////
// for UART's bufer 
#define NOM 11	//number of receiving bytes by UART

char buffer[NOM];	//bufer for receiving by UART

volatile char flag_uart=0;	//flag for UART, if 1 information get
volatile uint16_t timeout=0;		//timeout for UART
int timeout_set=30;							// setted value of timeout
volatile int count_bufer=0;	//counter of elements of UART's buffer

//////////////////////////////////////////////////////////////////////////
// Delay in ms
//////////////////////////////////////////////////////////////////////////
void delay(__IO uint32_t nTime)
{
  uint64_t nTicks=(SystemCoreClock/1000)*nTime/6;
  for(; nTicks != 0; nTicks--);
}


////////////////////////////////////////////////////////////////////////// 
// Init timer2
//////////////////////////////////////////////////////////////////////////
void InitTimer2(int period)
{
	TIM_TimeBaseInitTypeDef base_timer;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	
  TIM_TimeBaseStructInit(&base_timer);

  base_timer.TIM_Prescaler = del-1;
  base_timer.TIM_Period = period-1;

  TIM_TimeBaseInit(TIM2, &base_timer);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM2_IRQn);
	
  TIM_Cmd(TIM2, ENABLE);	  //вкл таймер
}

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		ToggleBits(GPIOA, GPIO_Pin_1);
    //TIM_SetCounter(TIM2,0);
  }
}

////////////////////////////////////////////////////////////////////////// 
// Init timer3
//////////////////////////////////////////////////////////////////////////
void InitTimer3(int period)
{
	TIM_TimeBaseInitTypeDef base_timer;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	
  TIM_TimeBaseStructInit(&base_timer);

  base_timer.TIM_Prescaler = del-1;
  base_timer.TIM_Period = period-1;

  TIM_TimeBaseInit(TIM3, &base_timer);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM3_IRQn);
	
  TIM_Cmd(TIM3, ENABLE);	  //вкл таймер
}

void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		
		ToggleBits(GPIOA, GPIO_Pin_2);
  }
}

////////////////////////////////////////////////////////////////////////// 
// Init timer4
//////////////////////////////////////////////////////////////////////////
void InitTimer4(int period)
{
	TIM_TimeBaseInitTypeDef base_timer;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	
  TIM_TimeBaseStructInit(&base_timer);

  base_timer.TIM_Prescaler = del-1;
  base_timer.TIM_Period = period-1;

  TIM_TimeBaseInit(TIM4, &base_timer);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	NVIC_EnableIRQ(TIM4_IRQn);
	
  TIM_Cmd(TIM4, ENABLE);	  //вкл таймер
}

void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		
		ToggleBits(GPIOA, GPIO_Pin_3);
    //TIM_SetCounter(TIM2,0);
  }
}
void InitPorts()
{
	GPIO_InitTypeDef gpio;
		 
	//Clock of Ports
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOF, ENABLE);
	
    
	// LED on board STM		   
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;

	// Init Port E 
	
  GPIO_Init(GPIOE, &gpio);

	//Init Port as input, using user's button 
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
	gpio.GPIO_Pin = GPIO_Pin_0;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;

	// Init GPIO for PORT A 
	
  GPIO_Init(GPIOA, &gpio);
	
	//driver of motors			   
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Pin =  PB;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &gpio);
	
	// LED on my board
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &gpio);
	
	// Step for motors
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &gpio);
}

void ToggleBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
		if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin))
		{
				GPIO_ResetBits(GPIOx, GPIO_Pin);
		}
		else
		{
			GPIO_SetBits(GPIOx, GPIO_Pin);
		}
}

/////////////////////////////////////////////////////////////////////.....
// Init UART
//////////////////////////////////////////////////////////////////////////
void InitUart4(void)
{
	GPIO_InitTypeDef GPIO_uart;
	USART_InitTypeDef USART_InitStructure;
	//USART_ClockInitTypeDef USART_ClockInitStructure;
	 
	//enable bus clocks
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	 
	//Set UART4 Tx (PC.10) as AF push-pull
	 
	GPIO_uart.GPIO_Pin = GPIO_Pin_10;
	GPIO_uart.GPIO_Mode = GPIO_Mode_AF;
	GPIO_uart.GPIO_OType = GPIO_OType_PP;
	GPIO_uart.GPIO_Speed = GPIO_Speed_50MHz;
	 
	GPIO_Init(GPIOC, &GPIO_uart);
	 
	//Set UART4 Rx (PC.11) as input floating
	 
	GPIO_uart.GPIO_Pin = GPIO_Pin_11;
	GPIO_uart.GPIO_Mode = GPIO_Mode_AF;
	GPIO_uart.GPIO_OType = GPIO_OType_PP;
	GPIO_uart.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_uart.GPIO_Speed = GPIO_Speed_50MHz;
	 
	GPIO_Init(GPIOC, &GPIO_uart);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_5);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_5);
	 
	
	//USART_ClockStructInit(&USART_ClockInitStructure);
	 
	//USART_ClockInit(UART4, &USART_ClockInitStructure);
	 
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	 
	//Write UART4 parameters
	USART_Init(UART4, &USART_InitStructure);

	//?????????? ?????????? ?? ?????? ??????
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(UART4_IRQn);	  //??? ??????????
	//USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
	 
	//Enable USART4
	USART_Cmd(UART4, ENABLE);
 
}

//////////////////////////////////////////////////////////////////////////
// Send data by UART4
//////////////////////////////////////////////////////////////////////////
void USART_Transmit(uint8_t ch)
{
      USART_SendData(UART4, (uint8_t) ch);
      //Loop until the end of transmission
      while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
      USART_ClearFlag(UART4, USART_FLAG_TC);
}

//////////////////////////////////////////////////////////////////////////
// Send string by UART4
//////////////////////////////////////////////////////////////////////////
void USART_Transmit_str( char *str)
{
	
	while(*str != 0)
	{
		USART_Transmit( *str++ );
	}
}

///////////////////////////////////////////////////////////////////////////
// Interrupt of USART4
//////////////////////////////////////////////////////////////////////////
void UART4_IRQHandler()
{
    timeout=0;

	if(flag_uart==1)			
	{
		return;				//if buffer is at the work, then exit
	}

	//write receiving data to the buffer
	buffer[count_bufer] = USART_ReceiveData(UART4);

		
	//maximum is NOM bytes, if counter equal NOM, we got all bytes
	count_bufer++;
  if(count_bufer==NOM)
	{
		count_bufer=0;
		flag_uart=1;
		obrabotka_bufera();
	}
		
		 /* If Overrun occures, clear the OVR condition */
  if (USART_GetFlagStatus(UART4, USART_FLAG_ORE) != RESET)
  {
    (void)USART_ReceiveData(UART4);
  }

	
}


void led_on()
{
 	 GPIO_SetBits(GPIOE, GPIO_Pin_11);
}
void led_off()
{
 	 GPIO_ResetBits(GPIOE, GPIO_Pin_11);
}
void SysTick_Handler()
{

	static int i1=0, i2=0, i3=0;
	
	i1++;
	i2++;
	i3++;
	
	if(i1==500)
	{
	  ToggleBits(GPIOE, GPIO_Pin_15);
	  i1=0;
	}
	
	
	
	//if we got something by UART4, we are waiting for timeout...
	if(count_bufer != 0)
	{
		timeout++;
		//if timeout is finished, we will get nothing 
		if(timeout>timeout_set)	
		{
			count_bufer=0;
			flag_uart=1;
			timeout=0;
			
			obrabotka_bufera();
		}
	}	
}


///////////////////////////////////////////////////////////////////////////
// analysis of UART's data
//////////////////////////////////////////////////////////////////////////
void obrabotka_bufera(void)
{
	int i=0;
	
	int speed1, speed2, speed3;
	
	if(flag_uart==1)			// if data is got by UART
	{
			if(buffer[0]==125)	// if we received speeds of motors
			{
				// wheel 1
								
				speed1=buffer[2]<<8;				// rotational velocity of the 1 wheel, RPM
				speed1+=buffer[3];
				
				
				// wheel 2
				speed2=buffer[5]<<8;				// rotational velocity of the 2 wheel, RPM
				speed2+=buffer[6];																			
				
				
				// wheel 3
				speed3=buffer[8]<<8;				// rotational velocity of the 2 wheel, RPM
				speed3+=buffer[9];																			
				
								
				// direction of moving
				if(buffer[10]==1)	//forward
				{
					USART_Transmit(1);
					Motor_All(buffer[1], speed1, buffer[4], speed2, buffer[7], speed3);
				}
				else if(buffer[10]==0)	//stop
				{
					Stop();
					USART_Transmit(0);
					
				}
				
			}
			
			//clear bufer
			for(i=0;i<NOM;i++)
			{
				buffer[i]=0;
			}

			flag_uart=0;	//clear flag
		}
}



void Stop()
{
	
	Freq1(0);
	GPIO_ResetBits(GPIOB, K1_RESET|K1_SLEEP);
	
	Freq2(0);
	GPIO_ResetBits(GPIOB, K2_RESET|K2_SLEEP);
	
	Freq3(0);
	GPIO_ResetBits(GPIOB, K3_RESET|K3_SLEEP);
}

void Freq1(int hz)
{
	if(hz==0)
	{
		InitTimer2(0);
	}
	else
	{
		InitTimer2(1000000 / (2*hz) );
	}
}

void Freq2(int hz)
{
	if(hz==0)
	{
		InitTimer3(0);
	}
	else
	{
		InitTimer3(1000000 / (2*hz) );
	}
}

void Freq3(int hz)
{
	if(hz==0)
	{
		InitTimer4(0);
	}
	else
	{
		InitTimer4(1000000 / (2*hz) );
	}
}



void Motor_All(int dir1, int freq1, int dir2, int freq2, int dir3, int freq3)
{
	// direction = 0 -> stop
	// direction = 1 -> forward
	// direction = 2 -> back
	
	int k1,k2,k3;
	int spd1, spd2,spd3;
	int number_points;
	int start_speed1, start_speed2, start_speed3;
	int break_speed;
	int temp;
	
	//Motor 1
	if(dir1 == 1)
	{
   	GPIO_SetBits(GPIOB, K1_RESET|K1_SLEEP|K1_DIR);
		GPIO_ResetBits(GPIOB, K1_EN);
	}
	else if(dir1 == 2)
	{
		GPIO_SetBits(GPIOB, K1_RESET|K1_SLEEP);
		GPIO_ResetBits(GPIOB, K1_EN|K1_DIR);
	}
	else if(dir1 == 0)
	{
		//Freq1(0);
		
		//GPIO_ResetBits(GPIOB, K1_RESET|K1_SLEEP);
	}
	
	//Motor 2
	if(dir2 == 1)
	{
   	GPIO_SetBits(GPIOB, K2_RESET|K2_SLEEP|K2_DIR);
		GPIO_ResetBits(GPIOB, K2_EN);
	}
	else if(dir2 == 2)
	{
		GPIO_SetBits(GPIOB, K2_RESET|K2_SLEEP);
		GPIO_ResetBits(GPIOB, K2_EN|K2_DIR);
	}
	else if(dir2 == 0)
	{
		//Freq2(0);
		
		//GPIO_ResetBits(GPIOB, K2_RESET|K2_SLEEP);
	}
	
	//Motor 3
	if(dir3 == 1)
	{
   	GPIO_SetBits(GPIOB, K3_RESET|K3_DIR);
		GPIO_ResetBits(GPIOB, K3_SLEEP|K3_EN);
	}
	else if(dir3 == 2)
	{
		GPIO_SetBits(GPIOB, K3_RESET|K3_SLEEP);
		GPIO_ResetBits(GPIOB, K3_EN|K3_DIR);
	}
	else if(dir3 == 0)
	{
		//Freq3(0);
		
		//GPIO_ResetBits(GPIOB, K3_RESET|K3_SLEEP);
	}
	

//acceleration
///////////////////////////////////////////////////////////////////////////////////////////////////
		
	start_speed1=1000000/(2*((short)(TIM2->ARR)+1));
	start_speed2=1000000/(2*((short)(TIM3->ARR)+1));
	start_speed3=1000000/(2*((short)(TIM4->ARR)+1));
		
	
	break_speed=200;
	number_points=20;
	
///////////////////////////////////////////////////////////////////////////////////////////////////
//Calculate k1, k2, k3 for acceleration and braking
	if(dir1 != 0)
	{
		k1=(freq1-start_speed1)/number_points;
	}
	else
	{
		temp = 1000000/(2*(TIM2->ARR+1));
		if(temp<200)
		{
			k1=0;
		}
		else
		{
			k1 = ( temp - break_speed )/number_points;
		}
	}
	
	if(dir2 !=0)
	{
		k2=(freq2-start_speed2)/number_points;
	}
	else
	{
		temp = 1000000/(2*(TIM3->ARR+1));
		if(temp<200)
		{
			k2=0;
		}
		else
		{
			k2= ( temp-break_speed )/number_points;
		}
	}
	
	if(dir3 !=0)
	{
		k3=(freq3-start_speed3)/number_points;
	}
	else
	{
		temp = 1000000/(2*(TIM4->ARR+1));
		if(temp<200)
		{
			k3=0;
		}
		else
		{
			k3= ( temp-break_speed )/number_points;
		}
	}
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	
///////////////////////////////////////////////////////////////////////////////////////////////////
//if speed < start speed -> started speed of each motors = setted speed
//if speed >= start speed	-> started speed of each motors = start_speed
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	if(freq1<break_speed && start_speed1==0)
	{
		spd1=freq1;
		k1=0;
	}
	else
	{
		spd1=start_speed1;
	}
	
	if(freq2<break_speed && start_speed2==0)
	{
		spd2=freq2;
		k2=0;
	}
	else
	{
		spd2=start_speed2;
	}
	
	if(freq3<break_speed && start_speed3==0)
	{
		spd3=freq3;
		k3=0;
	}
	else
	{
		spd3=start_speed3;
	}
///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////// ||  || start_speed2>0 || start_speed3>0
//acceleration or braking

		for(i=0;i<number_points;i++)
		{
			if(dir1 !=0 )
			{
				if(freq1>200 || start_speed1>0)
				{
					Freq1(spd1);
					spd1+=k1;
				}
				else
				{
					Freq1(freq1);
				}
				
			}
			else
			{
				if(spd1>200)
				{
					spd1 = 1000000/(2*(TIM2->ARR+1));
					spd1-=k1;
					Freq1(spd1);
				}
				else
				{
					Freq1(0);
					GPIO_ResetBits(GPIOB, K1_RESET|K1_SLEEP);
				}
			}
			
			if(dir2 !=0)
			{
				if(freq2>200 || start_speed2>0)
				{
					Freq2(spd2);
					spd2+=k2;
				}
				else
				{
					Freq2(freq2);
				}
			}
			else
			{
				if(spd2>200)
				{
					spd2 = 1000000/(2*(TIM3->ARR+1));
					spd2-=k2;
					Freq2(spd2);
				}
				else
				{
						Freq2(0);
						GPIO_ResetBits(GPIOB, K2_RESET|K2_SLEEP);
				}
				
			}
			
			if(dir3 != 0)
			{
				if(freq3>200 || start_speed3>0)
				{
					Freq3(spd3);
					spd3+=k3;
				}
				else
				{
					Freq3(freq3);
				}
			}
			else
			{
				if(spd3>200)
				{
					spd3 = 1000000/(2*(TIM4->ARR+1));
					spd3-=k3;
					Freq3(spd3);
				}
				else
				{
					Freq3(0);
					GPIO_ResetBits(GPIOB, K3_RESET|K3_SLEEP);
				}
			}
			
			delay(20);
		}

		
	if(dir1 != 0)
	{
		Freq1(freq1);
	}
	else
	{
		Freq1(0);
		GPIO_ResetBits(GPIOB, K1_RESET|K1_SLEEP);
	}
	
	if(dir2 != 0)
	{
		Freq2(freq2);
	}
	else
	{
		Freq2(0);
		GPIO_ResetBits(GPIOB, K2_RESET|K2_SLEEP);
	}
	
	if(dir3 != 0)
	{
		Freq3(freq3);
	}
	else
	{
		Freq3(0);
		GPIO_ResetBits(GPIOB, K3_RESET|K3_SLEEP);
	}
///////////////////////////////////////////////////////////////////////////////////////////////////
	
}

void InitPriority()
{
	//NVIC_SetPriority (SysTick_IRQn, 2); 	// Set priority for SysTick
	NVIC_SetPriority (TIM2_IRQn, 1);
	NVIC_SetPriority (TIM3_IRQn, 1);
	NVIC_SetPriority(TIM4_IRQn,1);
	
	NVIC_SetPriority(UART4_IRQn,15);

	
	
}
int main(void)
{
	const uint16_t a[]={GPIO_Pin_8,GPIO_Pin_9,GPIO_Pin_10,GPIO_Pin_11,GPIO_Pin_12,GPIO_Pin_13,GPIO_Pin_14,GPIO_Pin_15};
	uint8_t s=0;
	int sp=500;

	 InitPriority();
	 // Init timer SysTick for interrupt 1000 time in a second 
	 SysTick_Config(SystemCoreClock / 1000);	

	 InitTimer2(0);
	 InitTimer3(0);
	 InitTimer4(0);

	 
	 InitUart4();

   GPIO_Write(GPIOE, a[s]);
   
	
		
	 USART_Transmit(0);	   //first byte

	 USART_Transmit_str("OK");


	delay(1500);
	InitPorts();
	Motor_All(1, sp, 1, sp, 1, sp);
	delay(1500);
	Stop();
	
	while(1)
	{
		
		//Motor_All(1, 300, 2, 300, 1, 300);
		//delay(5000);
		//Stop();
			

		if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
		{
			while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1);
			delay(10);

			s++;
			if(s>6)
			{
				s=0;
			}
			GPIO_Write(GPIOE, a[s]);
			
			
			
// 			Motor1(1,100);
// 			Motor2(1,100);
// 			Motor3(1,100);
// 			
// 			delay(2000);
// 			USART_Transmit(1);
// 			
// 			Motor1(0,0);
// 			Motor2(0,0);
// 			Motor3(0,0);
// 			USART_Transmit(0);
// 			
// 			Motor1(2,200);
// 			Motor2(2,200);
// 			Motor3(2,200);
// 			USART_Transmit(1);
// 			delay(2000);
// 			
// 			Motor1(0,0);
// 			Motor2(0,0);
// 			Motor3(0,0);
// 			USART_Transmit(0);
// 			
// 			delay(1000);
			
		
		}

		
		//delay(30);
	}
   
}
