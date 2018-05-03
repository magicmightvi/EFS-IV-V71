
#ifndef _DELAY
#define _DELAY
  #include "DataStruct.h"

#endif
/***************************************************************************
进行短暂的延时,使用者可以根据时钟频率自行调节延时长短
***************************************************************************/
void delay(unsigned int unDelayNum)
{
    unsigned int i;
    for(i = 0;i < unDelayNum;i++)
    {
       ;
    }
}
//毫秒延时
void delayms(unsigned int unMSNum)
{
    unsigned int i,j;
    for(i = 0;i < unMSNum;i++)
    {
       FEED_WATCH_DOG();
       for(j = 0;j < 2000;j++);
    }
}