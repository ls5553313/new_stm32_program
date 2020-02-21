/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0 + STM32 �����ʱ��
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32ȫϵ�п����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_debug_usart.h"
#include "./key/bsp_key.h"
#include "./IrDa/bsp_irda.h" 
#include "./tim/bsp_basic_tim.h"

extern uint8_t  frame_flag;
extern uint8_t  isr_cnt;
extern uint8_t  frame_cnt;
uint8_t key_val;

/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
/* Key������ */
static TaskHandle_t Key_Task_Handle = NULL;

static TaskHandle_t IrDa_Task_Handle = NULL;

/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */
static TimerHandle_t Swtmr1_Handle =NULL;   /* �����ʱ����� */
static TimerHandle_t Swtmr2_Handle =NULL;   /* �����ʱ����� */
/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
static uint32_t TmrCb_Count1 = 0; /* ��¼�����ʱ��1�ص�����ִ�д��� */
static uint32_t TmrCb_Count2 = 0; /* ��¼�����ʱ��2�ص�����ִ�д��� */

KEY Key1,Key2;

/******************************* �궨�� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */

static void Key_Task(void* parameter);

static void IrDa_Task(void* parameter);

static void Swtmr1_Callback(void* parameter);
static void Swtmr2_Callback(void* parameter);

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */

/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  /* ������Ӳ����ʼ�� */
  BSP_Init();
  
	printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS�����ʱ��ʵ�飡\n");
	

  /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )0, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
	{
    vTaskStartScheduler();   /* �������񣬿������� */
		
	}
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
	printf("asdasdasda");
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	
  taskENTER_CRITICAL();           //�����ٽ���
	
	xReturn = xTaskCreate((TaskFunction_t)Key_Task,/* ������ں��� */
											 (const char*      )"Key_Task",/* �������� */
											 (uint16_t         )256,     /* ����ջ��С */
											 (void*            )NULL,    /* ������ں������� */
											 (UBaseType_t      )4,       /* ��������ȼ� */
											 (TaskHandle_t     )&Key_Task_Handle);/* ������ƿ�ָ�� */
	if(pdPASS == xReturn)
		printf("����Key_Task����ɹ���\r\n");
	
	/*xReturn = xTaskCreate((TaskFunction_t)IrDa_Task,
											 (const char*      )"IrDa_Task",
											 (uint16_t         )256,     
											 (void*            )NULL,    
											 (UBaseType_t      )5,       
											 (TaskHandle_t     )&IrDa_Task_Handle);*/
	if(pdPASS == xReturn)
		printf("����IrDa_Task����ɹ���\r\n");

    
  /************************************************************************************
   * ����������ڶ�ʱ��
   * ����ԭ��
   * TimerHandle_t xTimerCreate(	const char * const pcTimerName,
								const TickType_t xTimerPeriodInTicks,
								const UBaseType_t uxAutoReload,
								void * const pvTimerID,
                TimerCallbackFunction_t pxCallbackFunction )
    * @uxAutoReload : pdTRUEΪ����ģʽ��pdFALSΪ����ģʽ
   * ���ζ�ʱ��������(1000��ʱ�ӽ���)������ģʽ
   *************************************************************************************/
  Swtmr1_Handle=xTimerCreate((const char*		)"AutoReloadTimer",
                            (TickType_t			)1000,/* ��ʱ������ 1000(tick) */
                            (UBaseType_t		)pdTRUE,/* ����ģʽ */
                            (void*				  )2,/* Ϊÿ����ʱ������һ��������ΨһID */
                            (TimerCallbackFunction_t)Swtmr1_Callback); 
  if(Swtmr1_Handle != NULL)                          
  {
    /***********************************************************************************
     * xTicksToWait:����ڵ���xTimerStart()ʱ��������������tickΪ��λָ����������Ӧ����
     * ��Blocked(����)״̬�Եȴ�start����ɹ����͵�timer������е�ʱ�䡣 
     * ������������ȳ���֮ǰ����xTimerStart()�������xTicksToWait�����������õȴ�ʱ��Ϊ0.
     **********************************************************************************/
    xTimerStart(Swtmr1_Handle,0);	//�������ڶ�ʱ��
  }                            
  /************************************************************************************
   * ����������ڶ�ʱ��
   * ����ԭ��
   * TimerHandle_t xTimerCreate(	const char * const pcTimerName,
								const TickType_t xTimerPeriodInTicks,
								const UBaseType_t uxAutoReload,
								void * const pvTimerID,
                TimerCallbackFunction_t pxCallbackFunction )
    * @uxAutoReload : pdTRUEΪ����ģʽ��pdFALSΪ����ģʽ
   * ���ζ�ʱ��������(5000��ʱ�ӽ���)������ģʽ
   *************************************************************************************/
	Swtmr2_Handle=xTimerCreate((const char*			)"OneShotTimer",
                             (TickType_t			)2000,/* ��ʱ������ 1000(tick) */
                             (UBaseType_t			)pdFALSE,/* ����ģʽ */
                             (void*					  )2,/* Ϊÿ����ʱ������һ��������ΨһID */
                             (TimerCallbackFunction_t)Swtmr2_Callback); 
 
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

/***********************************************************************
  * @ ������  �� Swtmr1_Callback
  * @ ����˵���� �����ʱ��1 �ص���������ӡ�ص�������Ϣ&��ǰϵͳʱ��
  *              �����ʱ���벻Ҫ��������������Ҳ��Ҫ������ѭ����Ӧ������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void Swtmr1_Callback(void* parameter)
{		
  TickType_t tick_num1;

  TmrCb_Count1++;						/* ÿ�ص�һ�μ�һ */

  tick_num1 = xTaskGetTickCount();	/* ��ȡ�δ�ʱ���ļ���ֵ */
  
  //LED1_TOGGLE;
  
  //printf("Swtmr1_Callback����ִ�� %d ��\n", TmrCb_Count1);
  //printf("�δ�ʱ����ֵ=%d\n", tick_num1);
}

/***********************************************************************
  * @ ������  �� Swtmr2_Callback
  * @ ����˵���� �����ʱ��2 �ص���������ӡ�ص�������Ϣ&��ǰϵͳʱ��
  *              �����ʱ���벻Ҫ��������������Ҳ��Ҫ������ѭ����Ӧ������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void Swtmr2_Callback(void* parameter)
{	
  TickType_t tick_num2;

  TmrCb_Count2++;						/* ÿ�ص�һ�μ�һ */

  tick_num2 = xTaskGetTickCount();	/* ��ȡ�δ�ʱ���ļ���ֵ */
	LED2_TOGGLE

  printf("Swtmr2_Callback����ִ�� %d ��\n", TmrCb_Count2);
  printf("�δ�ʱ����ֵ=%d\n", tick_num2);
}


/**
  * @brief ������������
  * @note ��
  * @param ��
  * @retval ��
  */
static void Key_Task(void* parameter)
{
	while(1)
	{
		Key_RefreshState(&Key1);//ˢ�°���״̬
		Key_RefreshState(&Key2);//ˢ�°���״̬
		if(Key_AccessTimes(&Key1,KEY_ACCESS_READ)!=0)//���������¹�
		{
			//LED2_TOGGLE;
			printf("����1������\n");
			if(Swtmr2_Handle != NULL)
      {
       /***********************************************************************************
       * xTicksToWait:����ڵ���xTimerStart()ʱ��������������tickΪ��λָ����������Ӧ����
       * ��Blocked(����)״̬�Եȴ�start����ɹ����͵�timer������е�ʱ�䡣 
       * ������������ȳ���֮ǰ����xTimerStart()�������xTicksToWait�����������õȴ�ʱ��Ϊ0.
       **********************************************************************************/   
        xTimerStart(Swtmr2_Handle,0);	//�������ڶ�ʱ��
      }	
			Key_AccessTimes(&Key1,KEY_ACCESS_WRITE_CLEAR);
		}
		if(Key_AccessTimes(&Key2,KEY_ACCESS_READ)!=0)//���������¹�
		{
			//LED1_TOGGLE;
			printf("����2������\n");
			Key_AccessTimes(&Key2,KEY_ACCESS_WRITE_CLEAR);
		}
		vTaskDelay(50);
	}
}


static void IrDa_Task(void* parameter)
{	
  for(;;)
	{	  
    if( frame_flag == 1 ) /* һ֡�������ݽ������ */
    {
      key_val = IrDa_Process();
      printf("\r\n key_val=%d \r\n",key_val);
      printf("\r\n ��������frame_cnt=%d \r\n",frame_cnt);
      printf("\r\n �жϴ���isr_cnt=%d \r\n",isr_cnt);
      
      /* ��ͬ��ң��������Ӧ��ͬ�ļ�ֵ����Ҫʵ�ʲ��� */
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
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED ��ʼ�� */
	LED_GPIO_Config();
	
	/* ��ʼ��������ʱ����ʱ��0.5s����һ���ж� */
	TIMx_Configuration();

	/* ���ڳ�ʼ��	*/
	Debug_USART_Config();
  
  /* ������ʼ��	*/
	/* ������ʼ�� */
	Key1_GPIO_Config();
	Key2_GPIO_Config();
	KeyCreate(&Key1,GetPinStateOfKey1);
	KeyCreate(&Key2,GetPinStateOfKey2);
	
	
	/* ��ʼ���������ͷCP1838�õ���IO */
	IrDa_Init();  
	printf("\r\n ����һ������ң�ط��������ʵ�� \r\n");
}

/********************************END OF FILE****************************/
