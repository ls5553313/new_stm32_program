#include "bsp_key.h"
#include "./TiMbase/bsp_TiMbase.h" 
#include "bsp_i2c_gpio.h"
#include "OLED_I2C.h"
#include "oled.h"


void Show_fstPage(void)
{
	unsigned char i;
	for(i=0;i<4;i++)
		{
			OLED_ShowCN(22+i*16,0,i);									//测试显示中文
		}
	
	for(i=4;i<12;i++)
		{
			OLED_ShowCN(i*16-64,2,i);									//测试显示中文
		}
	OLED_ShowCN(48,6,12);
	OLED_ShowCN(64,6,11);	
};

void Show_secPage(char * ch)
{
	unsigned char i;
	for(i=0;i<4;i++)
		{
			OLED_ShowCN(22+i*16,0,i);									//测试显示中文
		}
	OLED_ShowCN(0,2,13);
	OLED_ShowCN(16,2,14);
	OLED_ShowCN(32,2,14);
	OLED_ShowCN(48,2,15);
	OLED_showStr(72,2,(char*)":",2);
	OLED_showStr(80,2,ch,2);	
	OLED_showStr(104,2,(char*)"S",2);
	
	OLED_ShowCN(0,6,18);
	OLED_ShowCN(16,6,19);	
		
	OLED_ShowCN(48,6,20);
	OLED_ShowCN(64,6,21);		

	OLED_ShowCN(95,6,16);
	OLED_ShowCN(111,6,17);			
}

