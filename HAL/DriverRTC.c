#ifndef _DRIVER_RTC

#define _DRIVER_RTC
  #include "..\DataStruct.h"
#endif


//==============================================================================
//  函数名称   : ReadRealTime
//  功能描述   : 从时钟芯片中读取系统时间
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void ReadRealTime(void)
{
    unsigned char ucTimetemp[9];
    unsigned char  unHYM[6];
    g_gRealTimerbk[RTC_SEC] = 0x55;
    DS1302_GetData(ucTimetemp);                      //////read time from ds1302

        g_sRtcManager.m_gRealTimer[RTC_SEC]   = BCDtoINT(ucTimetemp[6]&0x7f);
        g_sRtcManager.m_gRealTimer[RTC_MINUT] = BCDtoINT(ucTimetemp[5]&0x7f);
        g_sRtcManager.m_gRealTimer[RTC_HOUR]  = BCDtoINT(ucTimetemp[4]&0x3f);
        g_sRtcManager.m_gRealTimer[RTC_DATE]  = BCDtoINT(ucTimetemp[3]&0x3f);
        g_sRtcManager.m_gRealTimer[RTC_MONTH] = BCDtoINT(ucTimetemp[2]&0x1f);
        g_sRtcManager.m_gRealTimer[RTC_YEAR]  = BCDtoINT(ucTimetemp[0])+ 2000;
        if(g_sRtcManager.m_gRealTimer[RTC_MONTH] > 12 || g_sRtcManager.m_gRealTimer[RTC_MONTH] <= 0 || 
           g_sRtcManager.m_gRealTimer[RTC_DATE] > 31 || g_sRtcManager.m_gRealTimer[RTC_DATE] <= 0 || 
           g_sRtcManager.m_gRealTimer[RTC_HOUR] > 23 || g_sRtcManager.m_gRealTimer[RTC_MINUT] > 59 || g_sRtcManager.m_gRealTimer[RTC_SEC] > 59)  
        {
          
            unHYM[0] = 10 ;
            //unHYM[1] = 1
            unHYM[1] = 9;
            unHYM[2] = 12;
            unHYM[3] = 9;  
            unHYM[4] = 9;
            unHYM[5] = 9;
            DS1302_Reset();                 //////////时钟芯片复位			  				  
            DS1302_SettingData(unHYM);           //////////设置时间
        }
	g_gRealTimerbk[RTC_YEAR] =g_sRtcManager.m_gRealTimer[RTC_YEAR];
	g_gRealTimerbk[RTC_MONTH] =g_sRtcManager.m_gRealTimer[RTC_MONTH];
	g_gRealTimerbk[RTC_DATE] =g_sRtcManager.m_gRealTimer[RTC_DATE];
	g_gRealTimerbk[RTC_HOUR] =g_sRtcManager.m_gRealTimer[RTC_HOUR];	
       g_gRealTimerbk[RTC_MINUT] =g_sRtcManager.m_gRealTimer[RTC_MINUT];
	g_gRealTimerbk[RTC_SEC] = 0x0;
}

//==============================================================================
//  函数名称   : WriteRealTime
//  功能描述   : 对时钟芯片写入系统时间
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================

void WriteRealTime(unsigned int *RealTime)
{
    unsigned char ucTimetemp[7];
    ucTimetemp[0] = (unsigned char)(RealTime[0]);
    //ucTimetemp[1] = 1;
    ucTimetemp[1] = ((unsigned char)RealTime[1]);
    ucTimetemp[2] = ((unsigned char)RealTime[2]);
    ucTimetemp[3] = ((unsigned char)RealTime[4]);
    ucTimetemp[4] = ((unsigned char)RealTime[5]);
    ucTimetemp[5] = ((unsigned char)RealTime[6]);
    DS1302_Reset();                 //////////时钟芯片复位			  				  
    DS1302_SettingData(ucTimetemp);           //////////设置时间
}
//==============================================================================
//  函数名称   : InitRtc
//  功能描述   : 开机从时钟芯片从读取系统时间
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：林中一
//==============================================================================
void InitRtc(void)
{
   // unsigned char ucTimetemp[7];
    g_sRtcManager.m_unStartCount = 3;   //初始化为4S，在开机3s的时间内灯常亮
    LED_RUN_ON;//初始化完毕后，亮灯
  
    ReadRealTime();    //读出全部RTC数据
    
}
/*
#define WRITEADD   0x64
#define READADD    0x65
#define SECADDR    0x00
#define WORDADD1   0xe0

INT8U Hex2Bcd(INT8U h)
{
	unsigned char x;
	x=(h/10)*16+h%10;
	return x;
}

void InitRtc(void)
{
   _DINT();
   WDTCTL = WDTPW + WDTHOLD;               ////禁止内部看门狗
   FEED_WATCH_DOG();                           ////清外部看门狗
   I2C_start();
   FEED_WATCH_DOG();    
   I2C_senddata(WRITEADD);
   FEED_WATCH_DOG(); 
   I2C_ack();
   FEED_WATCH_DOG(); 
   I2C_senddata(WORDADD1);
   FEED_WATCH_DOG(); 
   I2C_ack();
   FEED_WATCH_DOG(); 
   I2C_senddata(0x20);         //20设置为24小时制,无中断方式
   FEED_WATCH_DOG(); 
   I2C_ack();
   I2C_stop();
   FEED_WATCH_DOG();
   
   _EINT();
   return; 
}


//==============================================================================
//  函数名称   : ReadRealTime
//  功能描述   : 从时钟芯片中读取系统时间
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int* ReadRealTime(unsigned char type)
{
  INT8U  temp;//,a,b;
    INT8U  Time[7]; //Second,Minute,Hour,Week,Day,Month,Year

    unsigned int  unHYM[8];
    
    _DINT();
    WDTCTL = WDTPW + WDTHOLD;               ////禁止内部看门狗
    FEED_WATCH_DOG();                            ////清外部看门狗
    I2C_start();
    FEED_WATCH_DOG(); 
    I2C_senddata(WRITEADD);I2C_ack();
    FEED_WATCH_DOG(); 
    I2C_senddata(0x04);I2C_ack();
    FEED_WATCH_DOG();         
    temp = (I2C_receivedata() & 0x7F);
    Time[0]=(temp >> 4) * 10 + (temp & 0x0f);//Second 

    I2C_sendack();
    FEED_WATCH_DOG(); 
    temp = (I2C_receivedata() & 0x7F);

    Time[1]=(temp >> 4) * 10 + (temp & 0x0F);I2C_sendack();	// Minute   
    FEED_WATCH_DOG(); 
    temp = (I2C_receivedata() & 0x3F);//

    Time[2] =(temp >> 4) * 10 + (temp & 0x0F);I2C_sendack(); //  Hour
    
    FEED_WATCH_DOG(); 
    temp = (I2C_receivedata() & 0x07);//;

    Time[3] =(temp >> 4) * 10 + (temp & 0x0F);I2C_sendack();	   
    
    FEED_WATCH_DOG();    
    temp = (I2C_receivedata()&0x3f);// 

    Time[4] =(temp >> 4) * 10 + (temp & 0x0F);I2C_sendack();   
    
    FEED_WATCH_DOG();     
    temp = (I2C_receivedata()&0x1f);//;

    Time[5] =(temp >> 4) * 10 + (temp & 0x0F);I2C_sendack();	 
    
    FEED_WATCH_DOG(); 
    temp = (I2C_receivedata() & 0xFF);//;

    Time[6] =(temp >> 4) * 10 + (temp & 0x0F); 

    I2C_sendnotack();
    FEED_WATCH_DOG(); 
    I2C_stop();
    FEED_WATCH_DOG();  
    _EINT();
    if((Time[0] > 59) || (Time[1] > 59) || (Time[2] > 23)  \
      || (Time[5] < 1) || (Time[5] > 12) || (Time[6] > 40) || (Time[6] < 6))
    {
            InitRtc();
            g_sRtcManager.m_gRealTimer[RTC_SEC]   = 7;
            g_sRtcManager.m_gRealTimer[RTC_MINUT] = 8;
            g_sRtcManager.m_gRealTimer[RTC_HOUR]  = 9;
            g_sRtcManager.m_gRealTimer[RTC_WEEK] = 3;
            g_sRtcManager.m_gRealTimer[RTC_DATE]  = 10;
            g_sRtcManager.m_gRealTimer[RTC_MONTH] = 11;
            g_sRtcManager.m_gRealTimer[RTC_YEAR]  = 12+2000;
            unHYM[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR]-2000;
            unHYM[1] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
            unHYM[2] = g_sRtcManager.m_gRealTimer[RTC_DATE];
            unHYM[3] = g_sRtcManager.m_gRealTimer[RTC_WEEK];
            unHYM[4] = g_sRtcManager.m_gRealTimer[RTC_HOUR];  
            unHYM[5] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
            unHYM[6] = g_sRtcManager.m_gRealTimer[RTC_SEC];
            unHYM[7] = g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
            WriteRealTime(unHYM);
    }
    else
    {
        g_sRtcManager.m_gRealTimer[RTC_SEC]   =Time[0];
        g_sRtcManager.m_gRealTimer[RTC_MINUT] =Time[1];
        g_sRtcManager.m_gRealTimer[RTC_HOUR]  =Time[2];
        g_sRtcManager.m_gRealTimer[RTC_WEEK]  =Time[3];
        g_sRtcManager.m_gRealTimer[RTC_DATE]  =Time[4]; 
        g_sRtcManager.m_gRealTimer[RTC_MONTH] =Time[5];
        g_sRtcManager.m_gRealTimer[RTC_YEAR]  =Time[6]+2000;
        if(type && (g_StatusTime != g_sRtcManager.m_gRealTimer[RTC_HOUR]))
        {
            g_StatusTime = g_sRtcManager.m_gRealTimer[RTC_HOUR];
            if((g_StatusTime == g_gRunPara[RP_STATUSRP_TIME]) && g_DateCount)
            {
                g_DateCount--;
                if(!g_DateCount)
                {
                    OpenTimer(TM_SENDSTATUSDELAY);
                    g_DateCount = g_gRunPara[RP_STATUSRP_INTERVAL];
                }
            }
        }
    }
    return g_sRtcManager.m_gRealTimer;
}

//==============================================================================
//  函数名称   : WriteRealTime
//  功能描述   : 对时钟芯片写入系统时间
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================

void WriteRealTime(unsigned int *RealTime)
{
  INT8U mixbcd,tt;
    
    _DINT();
    WDTCTL = WDTPW + WDTHOLD;               ////禁止内部看门狗
   FEED_WATCH_DOG();                             ////清外部看门狗
	//WatchDogClose();
    I2C_start();
    
    I2C_senddata(WRITEADD);
    FEED_WATCH_DOG();
    I2C_ack();
    FEED_WATCH_DOG();
    I2C_senddata(SECADDR);
    FEED_WATCH_DOG();
    I2C_ack();
    FEED_WATCH_DOG();

	//WatchDogClose();
    tt=RealTime[6];
    mixbcd=Hex2Bcd(tt);//& 0x7f
    FEED_WATCH_DOG();
    I2C_senddata(mixbcd);	    //SysTime.Second	秒
    FEED_WATCH_DOG();
    I2C_ack();
    FEED_WATCH_DOG();
  
	//WatchDogClose();
    mixbcd=Hex2Bcd(RealTime[5]);//Hex2Bcd()& 0x7f;
    FEED_WATCH_DOG();
    I2C_senddata(mixbcd);
    FEED_WATCH_DOG();
    I2C_ack();
    FEED_WATCH_DOG();
    
    mixbcd=Hex2Bcd(RealTime[4]);  //Hour Hex2Bcd	 & 0x3f
    FEED_WATCH_DOG();
    I2C_senddata(mixbcd);		   
    FEED_WATCH_DOG();
    I2C_ack();
    FEED_WATCH_DOG();
    
    mixbcd=Hex2Bcd(RealTime[3]);//Week Hex2Bcd & 0x07
    FEED_WATCH_DOG();
    I2C_senddata(mixbcd);//3
    FEED_WATCH_DOG();
    I2C_ack();
    FEED_WATCH_DOG();

	//WatchDogClose();
    mixbcd=Hex2Bcd(RealTime[2]) ;//Day Hex2Bcd& 0x3f		   
    FEED_WATCH_DOG();
    I2C_senddata(mixbcd);//		   
    FEED_WATCH_DOG();
    I2C_ack();
    FEED_WATCH_DOG();
    mixbcd=Hex2Bcd(RealTime[1]);//Month Hex2Bcd	 & 0x1f	   
    FEED_WATCH_DOG();
    I2C_senddata(mixbcd);
    FEED_WATCH_DOG();
    I2C_ack();
    FEED_WATCH_DOG();
    
    mixbcd=Hex2Bcd(RealTime[0]& 0xff);//Year Hex2Bcd & 0xff
    FEED_WATCH_DOG();
    I2C_senddata(mixbcd);	//	年
    FEED_WATCH_DOG();
    I2C_ack();
    FEED_WATCH_DOG();
    I2C_stop();
    FEED_WATCH_DOG();    
    _EINT();  
}
*/