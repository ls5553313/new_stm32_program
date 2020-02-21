#ifndef __MAIN_H_
#define	__MAIN_H_

#include "stm32f4xx.h"




/************************** BEEP ?????********************************/
void Swtmr1_Callback(void* parameter);

void TimerStart();
void TimerStop();



#endif /* __MAIN_H_ */
