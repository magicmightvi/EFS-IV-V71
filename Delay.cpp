
#ifndef _DELAY
#define _DELAY
  #include "DataStruct.h"

#endif
/***************************************************************************
���ж��ݵ���ʱ,ʹ���߿��Ը���ʱ��Ƶ�����е�����ʱ����
***************************************************************************/
void delay(unsigned int unDelayNum)
{
    unsigned int i;
    for(i = 0;i < unDelayNum;i++)
    {
       ;
    }
}
//������ʱ
void delayms(unsigned int unMSNum)
{
    unsigned int i,j;
    for(i = 0;i < unMSNum;i++)
    {
       FEED_WATCH_DOG();
       for(j = 0;j < 2000;j++);
    }
}