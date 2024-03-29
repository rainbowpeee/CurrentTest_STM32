/*********************************************************************************************************
*                               MiniSTM32 Evaluation BOARD SUPPORT PACKAGE
*
*									(c) Copyright 2009; GXU CCIE ES 
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************/

/*********************************************************************************************************
*
*                                   BOARD SUPPORT PACKAGE
*
*            ST Microelectronics STM32	with the MiniSTM32 Evaluation Board
*
*
*
* Filename      : KeyBoard_Drv.c
* Version       : V1.0
* Programmer(s) : Doggie
*********************************************************************************************************/

/*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************/
#include "main.h"

#include "KeyBoard_Drv.h"
#include "system.h"
// #include "SendData.h"

//公有变量定义区
unsigned int isKeyDown = 0;
unsigned int isKeyLongDown = 0;

unsigned int rotateCodeLeft = 0;
unsigned int rotateCodeRight = 0;

//私有常数、变量定义区
#define rotate_pressed_short  (1)			//旋转编码器短按时间    2ms
#define key_pressed_short  	(4)      //按键按下很短的时间	20ms
#define key_pressed_long   	(200)      //按键按下很长的时间	1000ms
#define OverTime   					(key_pressed_long+key_pressed_short)    //定义溢出时间
#define THRESHOLD_TIME  		160           // 定义阀值时间，在短按和长按之间�	800ms

static unsigned char key_value_g[16];     //按键的键值

/*********************************************************************************************************
*                                           KeyBoard_Drv_Init()
* Description : Initialize the board's Key.
* Argument(s) : none.
* Return(s)   : none.
* Caller(s)   : BSP_Init().
* Note(s)     : none.
*********************************************************************************************************/
void  KeyBoard_Drv_Init(void)
{
	GPIO_InitTypeDef  gpioinitstruct;

	gpioinitstruct.Pin    = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
  gpioinitstruct.Mode   = GPIO_MODE_AF_INPUT;
  gpioinitstruct.Pull   = GPIO_NOPULL;
  gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &gpioinitstruct);
	
	gpioinitstruct.Pin    = GPIO_PIN_4 | GPIO_PIN_5;
  gpioinitstruct.Mode   = GPIO_MODE_AF_INPUT;
  gpioinitstruct.Pull   = GPIO_NOPULL;
  gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &gpioinitstruct);

	gpioinitstruct.Pin    = GPIO_PIN_1 | GPIO_PIN_7;
  gpioinitstruct.Mode   = GPIO_MODE_AF_INPUT;
  gpioinitstruct.Pull   = GPIO_NOPULL;
  gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &gpioinitstruct);
	
	gpioinitstruct.Pin    = GPIO_PIN_2;
  gpioinitstruct.Mode   = GPIO_MODE_AF_INPUT;
  gpioinitstruct.Pull   = GPIO_NOPULL;
  gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &gpioinitstruct);


// 	/* 初始化输出高电平 */
// 	GPIO_SetBits(GPIOA, GPIO_Pin_All);
// 	GPIO_SetBits(GPIOB, GPIO_Pin_All);
// 	GPIO_SetBits(GPIOC, GPIO_Pin_All);
// 	GPIO_SetBits(GPIOD, GPIO_Pin_All);
// 	GPIO_SetBits(GPIOE, GPIO_Pin_All);
}

/*********************************************************************************************************
*                                        KeyBoard_Drv_GetStatus()
* Description : Get the status of the  KeyBoard.
* Argument(s) : none.
* Return(s)   : Bit-mapped status of KeyBoard :
* Caller(s)   : Application.
* Note(s)     : none.
*********************************************************************************************************/
unsigned char KeyBoardScan (unsigned char Pin)
{
		static unsigned int key_counter[16];
    static unsigned char status[16];
		static unsigned char Buf_value_g[16];
		
		status[Pin] = 0;
	
		/* 下面的程序段读取取按键状  */
		switch(Pin)
		{
			case 0:
				if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1) == 0) 
						status[Pin] = 1;
				break;
			case 1:
				if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 0) 
						status[Pin] = 1;
				break;
			case 2:
				if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == 0) 
						status[Pin] = 1;
				break;
			case 3:
				if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == 0) 
						status[Pin] = 1;
				break;
			case 4:
				if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2) == 0) 
						status[Pin] = 1;
				break;
			case 5:
				if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == 0) 
						status[Pin] = 1;
				break;
// 			case 6:
// 				if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == 0) 
// 						status[Pin] = 1;
// 				break;
// 			case 7:
// 				if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0) 
// 						status[Pin] = 1;
// 				break;
			case 6:
				if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_7) == 0) 
						status[Pin] = 1;
				break;
			default :
				break;
		}
		
		
		if(status[Pin] == 1)
		{
			key_counter[Pin]++;
			if(key_counter[Pin] > key_pressed_long)								//防止key_counter溢出
			{
					key_counter[Pin] = key_pressed_long + 1;			// 一旦长按，不连续触发,不理会短按
			}
			if(key_counter[Pin] == (key_pressed_short))		// 短按
			{
					Buf_value_g[Pin] = 1;						// 获取按键值，将键值付给短按缓存
			//		return (1); 															//返回
			}
			if(key_counter[Pin] == (key_pressed_long))		// 长按
			{
					Buf_value_g[Pin] = 0;						// 清除短按缓存
					key_value_g[Pin] = 2;						// 获取按键值
					return (1); 															//返回
			}
			return(0);
		}
		if(key_counter[Pin] < THRESHOLD_TIME)
		{
				if(Buf_value_g[Pin] == 1)		// 短按
				{
						Buf_value_g[Pin] = 0;		// 清除短按缓存
					
						key_value_g[Pin] = 1;						// 获取按键值
						return (1); 															//返回
				}
		}

		status[Pin] = 0;
		key_counter[Pin] = 0;
		Buf_value_g[Pin] = 0;		// 清除短按缓存
		return (0);
}
/*********************************************************************************************************
*                                           RotateCodeScan()
* Description : 
* Argument(s) : none.
* Return(s)   : none.
* Caller(s)   : BSP_Init().
* Note(s)     : none.
*********************************************************************************************************/
void RotateCodeScan(void)
{
		static unsigned char i = 0;	
		static unsigned int rotateCnt = 0;
		static unsigned char stateBuf = 0;
		// Key_A 是按下
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == 0) 
		{
				for(i = 0; i < 10; i++)
					__nop();

				if(rotateCnt == 0)
				{
						stateBuf = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);		// 获取Key_B的状态
				}
				rotateCnt++;
				if(rotateCnt >= 1)
					rotateCnt = 1;
		}
		else
		{
				if(rotateCnt)
				{
						rotateCnt = 0;
						if(stateBuf)
						{
								if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0)
								{
										rotateCodeLeft++;
								}
						}
						else
						{
								if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) != 0)
								{
										rotateCodeRight++;
								}
						}
				}


		}
		
}
/*********************************************************************************************************
*                                           KeyScan()
* Description : 
* Argument(s) : none.
* Return(s)   : none.
* Caller(s)   : BSP_Init().
* Note(s)     : none.
*********************************************************************************************************/
void KeyScan (void)
{
		static unsigned char i = 0;	
		for(i = 0; i < 16; i++)
		{
				if(KeyBoardScan(i))
				{
					// 处理长短按信息
					if(key_value_g[i] == 1)
					{
						key_value_g[i] = 0;	// 清按键信息
						isKeyDown = isKeyDown | (1 << i);
					}
					if(key_value_g[i] == 2)
					{
						key_value_g[i] = 0;	// 清按键信息
						isKeyLongDown = isKeyLongDown | (1 << i);
					}
				}
		}
		RotateCodeScan();
}

void KeyLoop (void)
{
	if(SysTime)
	{
			KeyScan();					// 按键检测
	}
}
