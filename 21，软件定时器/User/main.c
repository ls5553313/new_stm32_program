/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0 + STM32 软件定时器
  *********************************************************************
  * @attention
  *
  * 实验平台:野火  STM32全系列开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_debug_usart.h"
#include "./key/bsp_key.h"
#include "./IrDa/bsp_irda.h" 
#include "./tim/bsp_basic_tim.h"

extern uint8_t  frame_flag;
extern uint8_t  isr_cnt;
extern uint8_t  frame_cnt;
uint8_t key_val;

/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
/* Key任务句柄 */
static TaskHandle_t Key_Task_Handle = NULL;

static TaskHandle_t IrDa_Task_Handle = NULL;

/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */
static TimerHandle_t Swtmr1_Handle =NULL;   /* 软件定时器句柄 */
static TimerHandle_t Swtmr2_Handle =NULL;   /* 软件定时器句柄 */
/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
static uint32_t TmrCb_Count1 = 0; /* 记录软件定时器1回调函数执行次数 */
static uint32_t TmrCb_Count2 = 0; /* 记录软件定时器2回调函数执行次数 */

KEY Key1,Key2;

/******************************* 宏定义 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些宏定义。
 */

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void);/* 用于创建任务 */

static void Key_Task(void* parameter);

static void IrDa_Task(void* parameter);

static void Swtmr1_Callback(void* parameter);
static void Swtmr2_Callback(void* parameter);

static void BSP_Init(void);/* 用于初始化板载相关资源 */

/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  /* 开发板硬件初始化 */
  BSP_Init();
  
	printf("这是一个[野火]-STM32全系列开发板-FreeRTOS软件定时器实验！\n");
	

  /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )0, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
  /* 启动任务调度 */           
  if(pdPASS == xReturn)
	{
    vTaskStartScheduler();   /* 启动任务，开启调度 */
		
	}
  else
    return -1;  
  
  while(1);   /* 正常不会执行到这里 */    
}


/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
	printf("asdasdasda");
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	
  taskENTER_CRITICAL();           //进入临界区
	
	xReturn = xTaskCreate((TaskFunction_t)Key_Task,/* 任务入口函数 */
											 (const char*      )"Key_Task",/* 任务名称 */
											 (uint16_t         )256,     /* 任务栈大小 */
											 (void*            )NULL,    /* 任务入口函数参数 */
											 (UBaseType_t      )4,       /* 任务的优先级 */
											 (TaskHandle_t     )&Key_Task_Handle);/* 任务控制块指针 */
	if(pdPASS == xReturn)
		printf("创建Key_Task任务成功！\r\n");
	
	/*xReturn = xTaskCreate((TaskFunction_t)IrDa_Task,
											 (const char*      )"IrDa_Task",
											 (uint16_t         )256,     
											 (void*            )NULL,    
											 (UBaseType_t      )5,       
											 (TaskHandle_t     )&IrDa_Task_Handle);*/
	if(pdPASS == xReturn)
		printf("创建IrDa_Task任务成功！\r\n");

    
  /************************************************************************************
   * 创建软件周期定时器
   * 函数原型
   * TimerHandle_t xTimerCreate(	const char * const pcTimerName,
								const TickType_t xTimerPeriodInTicks,
								const UBaseType_t uxAutoReload,
								void * const pvTimerID,
                TimerCallbackFunction_t pxCallbackFunction )
    * @uxAutoReload : pdTRUE为周期模式，pdFALS为单次模式
   * 单次定时器，周期(1000个时钟节拍)，周期模式
   *************************************************************************************/
  Swtmr1_Handle=xTimerCreate((const char*		)"AutoReloadTimer",
                            (TickType_t			)1000,/* 定时器周期 1000(tick) */
                            (UBaseType_t		)pdTRUE,/* 周期模式 */
                            (void*				  )2,/* 为每个计时器分配一个索引的唯一ID */
                            (TimerCallbackFunction_t)Swtmr1_Callback); 
  if(Swtmr1_Handle != NULL)                          
  {
    /***********************************************************************************
     * xTicksToWait:如果在调用xTimerStart()时队列已满，则以tick为单位指定调用任务应保持
     * 在Blocked(阻塞)状态以等待start命令成功发送到timer命令队列的时间。 
     * 如果在启动调度程序之前调用xTimerStart()，则忽略xTicksToWait。在这里设置等待时间为0.
     **********************************************************************************/
    xTimerStart(Swtmr1_Handle,0);	//开启周期定时器
  }                            
  /************************************************************************************
   * 创建软件周期定时器
   * 函数原型
   * TimerHandle_t xTimerCreate(	const char * const pcTimerName,
								const TickType_t xTimerPeriodInTicks,
								const UBaseType_t uxAutoReload,
								void * const pvTimerID,
                TimerCallbackFunction_t pxCallbackFunction )
    * @uxAutoReload : pdTRUE为周期模式，pdFALS为单次模式
   * 单次定时器，周期(5000个时钟节拍)，单次模式
   *************************************************************************************/
	Swtmr2_Handle=xTimerCreate((const char*			)"OneShotTimer",
                             (TickType_t			)2000,/* 定时器周期 1000(tick) */
                             (UBaseType_t			)pdFALSE,/* 单次模式 */
                             (void*					  )2,/* 为每个计时器分配一个索引的唯一ID */
                             (TimerCallbackFunction_t)Swtmr2_Callback); 
 
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}

/***********************************************************************
  * @ 函数名  ： Swtmr1_Callback
  * @ 功能说明： 软件定时器1 回调函数，打印回调函数信息&当前系统时间
  *              软件定时器请不要调用阻塞函数，也不要进行死循环，应快进快出
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void Swtmr1_Callback(void* parameter)
{		
  TickType_t tick_num1;

  TmrCb_Count1++;						/* 每回调一次加一 */

  tick_num1 = xTaskGetTickCount();	/* 获取滴答定时器的计数值 */
  
  //LED1_TOGGLE;
  
  //printf("Swtmr1_Callback函数执行 %d 次\n", TmrCb_Count1);
  //printf("滴答定时器数值=%d\n", tick_num1);
}

/***********************************************************************
  * @ 函数名  ： Swtmr2_Callback
  * @ 功能说明： 软件定时器2 回调函数，打印回调函数信息&当前系统时间
  *              软件定时器请不要调用阻塞函数，也不要进行死循环，应快进快出
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void Swtmr2_Callback(void* parameter)
{	
  TickType_t tick_num2;

  TmrCb_Count2++;						/* 每回调一次加一 */

  tick_num2 = xTaskGetTickCount();	/* 获取滴答定时器的计数值 */
	LED2_TOGGLE

  printf("Swtmr2_Callback函数执行 %d 次\n", TmrCb_Count2);
  printf("滴答定时器数值=%d\n", tick_num2);
}


/**
  * @brief 按键任务主体
  * @note 无
  * @param 无
  * @retval 无
  */
static void Key_Task(void* parameter)
{
	while(1)
	{
		Key_RefreshState(&Key1);//刷新按键状态
		Key_RefreshState(&Key2);//刷新按键状态
		if(Key_AccessTimes(&Key1,KEY_ACCESS_READ)!=0)//按键被按下过
		{
			//LED2_TOGGLE;
			printf("按键1被按下\n");
			if(Swtmr2_Handle != NULL)
      {
       /***********************************************************************************
       * xTicksToWait:如果在调用xTimerStart()时队列已满，则以tick为单位指定调用任务应保持
       * 在Blocked(阻塞)状态以等待start命令成功发送到timer命令队列的时间。 
       * 如果在启动调度程序之前调用xTimerStart()，则忽略xTicksToWait。在这里设置等待时间为0.
       **********************************************************************************/   
        xTimerStart(Swtmr2_Handle,0);	//开启周期定时器
      }	
			Key_AccessTimes(&Key1,KEY_ACCESS_WRITE_CLEAR);
		}
		if(Key_AccessTimes(&Key2,KEY_ACCESS_READ)!=0)//按键被按下过
		{
			//LED1_TOGGLE;
			printf("按键2被按下\n");
			Key_AccessTimes(&Key2,KEY_ACCESS_WRITE_CLEAR);
		}
		vTaskDelay(50);
	}
}


static void IrDa_Task(void* parameter)
{	
  for(;;)
	{	  
    if( frame_flag == 1 ) /* 一帧红外数据接收完成 */
    {
      key_val = IrDa_Process();
      printf("\r\n key_val=%d \r\n",key_val);
      printf("\r\n 按键次数frame_cnt=%d \r\n",frame_cnt);
      printf("\r\n 中断次数isr_cnt=%d \r\n",isr_cnt);
      
      /* 不同的遥控器面板对应不同的键值，需要实际测量 */
      switch( key_val )
      {
        case 0:
        LED1_TOGGLE;  
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n Error \r\n");
        break;
        
        case 162:
        LED1_TOGGLE;  
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n POWER \r\n");
        break;
        
        case 226:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n MENU \r\n");
        break;
        
        case 34:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n TEST \r\n");
        break;
        
        case 2:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n + \r\n");
        break;
        
        case 194:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n RETURN \r\n");
        break;
        
        case 224:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n |<< \r\n");
        break;
        
        case 168:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n > \r\n");
        break;
        
        case 144:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n >>| \r\n");
        break;
        
        case 104:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 0 \r\n");
        break;
        
        case 152:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n - \r\n");
        break;
        
        case 176:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n C \r\n");
        break;
        
        case 48:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 1 \r\n");
        break;
        
        case 24:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 2 \r\n");
        break;
        
        case 122:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 3 \r\n");
        break;
        
        case 16:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 4 \r\n");
        break;
        
        case 56:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 5 \r\n");
        break;
        
        case 90:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 6 \r\n");
        break;
        
          case 66:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 7 \r\n");
        break;
        
        case 74:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 8 \r\n");
        break;
        
        case 82:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 9 \r\n");
        break;
        
        default:       
        break;
      }      
    }
	}
}


/***********************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED 初始化 */
	LED_GPIO_Config();
	
	/* 初始化基本定时器定时，0.5s产生一次中断 */
	TIMx_Configuration();

	/* 串口初始化	*/
	Debug_USART_Config();
  
  /* 按键初始化	*/
	/* 按键初始化 */
	Key1_GPIO_Config();
	Key2_GPIO_Config();
	KeyCreate(&Key1,GetPinStateOfKey1);
	KeyCreate(&Key2,GetPinStateOfKey2);
	
	
	/* 初始化红外接收头CP1838用到的IO */
	IrDa_Init();  
	printf("\r\n 这是一个红外遥控发射与接收实验 \r\n");
}

/********************************END OF FILE****************************/
