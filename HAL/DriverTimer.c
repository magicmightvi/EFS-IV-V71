/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  模块名     : 定时器驱动程序
  文件名     : DriverTimer.c
  相关文件       : DataStruct.h FuncDef.h Constant.h
  实现功能       : 完成对定时器TimerA0、TimerA1、TimerB的初始化，包括对完成1ms、0.625ms、0.125ms的中断处理函数、
           捕获中断处理函数、PWM驱动出口函数等。还包括对GPS对时或时钟芯片秒脉冲的捕获中断处理、毫秒的产生
  作者       : 林中一
  版本       : 1.0
  
  --------------------------------------------------------------------------------
  备注       : -
  --------------------------------------------------------------------------------
  
  修改记录       : 
    日  期      版本        修改人      修改内容 
    2010/06/01      1.0         林中一        创建

************************************************************************************/

#ifndef _DRIVER_TIMER

#define _DRIVER_TIMER
  #include "..\DataStruct.h"
#endif

unsigned char rep17,rep33,rep49;

//==============================================================================
//  函数名称   : InitTimer
//  功能描述   : 完成对定时器TimerA0、TimerA1、TimerB、RTCTimer的初始化，包括触发AD转换/1ms中断/捕获中断/PWM输出驱动继电器
//                               TimerA0作为AD转换定时器
//                               RTCTimer作为定时器，产生1ms定时器中断
//                               TimerA1作为1s脉冲捕获的定时器
//                               TimerB0作为继电器驱动的PWM输出的定时器
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者     ：
//==============================================================================
void InitTimer(void)
{    
    TA0CCTL0 = OUTMOD_4;                  //输出模式4 50%占空比 
  
    //TA0CCR0 = 312; 			    //输出周期156.25uS 张弢测试中断嵌套 
    //TA0CCR0 = 1250;                    // 输出周期1.25ms=2500=16点每周波//1.6khz
     //TA0CCR0 = 2500;                   //0.625ms=1250=32点每周波 800hz

    TA0CCR0 = 500; 			    //输出周期250uS 每周波80点 4000hz    
    TA0CTL = TASSEL_1 + ID_3 + MC_1 + TACLR  ;         //TMA0模式配置 ACLK时钟, 8分频 ,计数清0，增计数  
#ifndef _TIME_AD
    TA0CCTL0 |= CCIE; //使用中断 如果使用中断启动AD需呀这句话
    ADC12CTL0 |= ADC12SC;
#endif

    P2SEL |= BIT3;                    // PPS引脚设置
    P2DIR &= ~BIT3;
    TA1CCTL2 = CAP + SCS + CM_1 +CCIE;           //秒脉冲捕捉设置
    TA1CCTL1 |= CCIE;                    //TA1CCR1毫秒中断使能  
    TA1CCR1 = 250;                      //毫秒计数  
    TA1CTL = TASSEL_1 + ID_3 + MC_2 + TAIE;        //TMA1模式配置 ACLK时钟, 8分频 ,计数清0，连续计数
    TA1EX0 = TAIDEX_7;               //时钟再分频 /8  =250KHz
    TA1CTL |= TACLR;

    TB0CTL = TBSSEL_1 + ID_3 + MC_1 + TBCLR ;   //时钟源为ACLK,8分频,计数清0，
    
    if(g_gRunPara[RP_SENDYC_T] > 0)   //定时上传遥测
#ifdef YN_101S
        g_gsendYCTimeNum = ((unsigned long)g_gRunPara[RP_SENDYC_T]*10);
#else
        g_gsendYCTimeNum = ((unsigned long)g_gRunPara[RP_SENDYC_T]);
#endif
    if(g_gRunPara[RP_SENDYX_T] > 0)   //定时上传遥信
        g_gsendYXTimeNum = ((unsigned long)g_gRunPara[RP_SENDYX_T]);
    if(g_gRunPara[RP_BEAT_T] > 0)   //定时心跳
        g_gBeatNum = ((unsigned long)g_gRunPara[RP_BEAT_T]);
    if(g_gRunPara[RP_SENDZJ_T] > 0)   //定时上传自检
        g_gsendZJTimeNum = ((unsigned long)g_gRunPara[RP_SENDZJ_T])*10;	
}


//unsigned int TestAdcData[1][AC_BUFF_LEN];

//==============================================================================
//  函数名称   : GetAcSamData
//  功能描述   : 获得交流采集数据，并把交流采集的数据进行滤波
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者     ：
//==============================================================================
void GetAcSamData(void)
{
    unsigned int Last3SamTail,Last2SamTail,LastSamTail,AcSamTail; //依次为最新的三个采样数据
    int Value1,Value2,Value3,Value4;
    int Polarity;
    int i;
    //根据软件滤波的需要，把m_unAcDataTail作为计算和录波的最新的数据，而采集数据的存放位置为m_unAcDataTail + AC_SAM_TAIL_OFFSET，这样可以由空间进行滤波处理
    g_sSampleData.m_unAcDataTail++;     //考虑和原来代码的兼容，m_unAcDataTail为运算用到的数据尾部
    g_sSampleData.m_unAcDataTail &= 0x1F;
    AcSamTail = (g_sSampleData.m_unAcDataTail + AC_SAM_TAIL_OFFSET) & 0x1F; //最新采集数据保存的位置，方便进行滤波处理
    
    //采用三角滤波的方法进行采样值的滤波
    LastSamTail = (AcSamTail - 1)  & 0x1F;  //最新采集数据次新点
    Last2SamTail = (AcSamTail - 2)  & 0x1F; //最新采集数据次新点
    Last3SamTail = (AcSamTail - 3)  & 0x1F; //最新采集数据次新点
      
    //考虑执行效率，没有用for语句进行运算
    for(i = 0; i < 6; i++)  //为了自产零序电压的计算，把交流采样的计算顺寻颠倒
    {    
            Polarity = g_unAdcData[i];        
        if(Polarity < 4097)
        {
            if(i == CHAN_U0 && g_gProcCnt[PC_U0_CAL] == 0)  //
                g_sSampleData.m_gAcAdcData[i][AcSamTail] = g_sSampleData.m_gAcAdcData[i - 1][AcSamTail] + g_sSampleData.m_gAcAdcData[i - 2][AcSamTail] + g_sSampleData.m_gAcAdcData[i - 3][AcSamTail]; 
            else
                g_sSampleData.m_gAcAdcData[i][AcSamTail] = (int)(((long)Polarity - g_gAdjPara[i]) * g_gProcCnt[PC_UA_ADJ + i] >> 12);    //* g_gAdjPara[ADJ_DEST_CHNIA + i] >> 10启动一次交流电压电流采集，把交流电压电流采集数据保存到g_sSampleData.m_gAcAdcData
           if(i==CHAN_Upt)//upt to uo//Uo to Upt
		  g_sSampleData.m_gAcAdcData[i][AcSamTail] = (int)(((long)Polarity - g_gAdjPara[3]) * g_gProcCnt[PC_UA_ADJ + 3] >> 12);    //* g_gAdjPara[ADJ_DEST_CHNIA + i] >> 10启动一次交流电压电流采集，把交流电压电流采集数据保存到g_sSampleData.m_gAcAdcData
		  
            Value1 = g_sSampleData.m_gAcAdcData[i][AcSamTail];
            Value2 = g_sSampleData.m_gAcAdcData[i][LastSamTail];
            Value3 = g_sSampleData.m_gAcAdcData[i][Last2SamTail];
            Value4 = g_sSampleData.m_gAcAdcData[i][Last3SamTail];
        
            if(Value1 >= Value2 && Value2 < Value3)
            {
                Value1 -= Value2;
                Value2 = Value3 - Value2;
                Polarity = 3;
            }
            else if(Value1 < Value2 && Value2 >= Value3)
            {
                Value1 = Value2 - Value1;
                Value2 -= Value3;
                Polarity = 3;
            }
            else if(Value1 >= Value2 && Value3 < Value4)
            {
                Value1 -= Value2;
                Value3 = Value4 - Value3;
                if(Value2 >= Value3)
                    Value2 -= Value3;
                else
                    Value2 = Value3 - Value2;
                Polarity = 3;
            }
            else if(Value1 < Value2 && Value3 >= Value4)
            {
                Value1 = Value2 - Value1;
                Value3 -= Value4;
                if(Value2 >= Value3)
                    Value2 -= Value3;
                else
                    Value2 = Value3 - Value2;
                Polarity = 3;
            } 
            else if(Value2 >= 0 && Value3 < 0 )    //补充相序的检测程序
            {
                if(phase_check_flag==0x55)     //////////标志已经设置，准备检测相位
    	        {
    	            if((eight_pulse_flag ==0))   /////////没有进行8脉冲
    	 	    {
                        if((g_gRmtMeas[RM_UA]>4000)&&(g_gRmtMeas[RM_UB]>4000)&&(g_gRmtMeas[RM_UC]>4000))     ////////再次检测为大于40才进行计算
      	     	        {
    	 	  	    
    	 	  	    phase_seq_flag=0x55;          /////////首先置为逆相序
                            if(i <= 2)
                            {
                                g_gucPhaseSQU[i] = LastSamTail;
                                g_gucPhaseSQUTestNum++;
                                if(g_gucPhaseSQUTestNum >= 3)
                                {
                                    g_gucPhaseSQUTestNum = 0;
                                    phase_check_flag=0;
                                    for(i = 0; i < 3; i++)
                                    {
                                        if(g_gucPhaseSQU[i] > 16)
                                            g_gucPhaseSQU[i] -= 16;
                                    }
                        
                                    if(g_gucPhaseSQU[0] < g_gucPhaseSQU[1])     //////////ABC、CAB
          	     	            {
          	     	                if((g_gucPhaseSQU[1]<=g_gucPhaseSQU[2])||((g_gucPhaseSQU[2]<=g_gucPhaseSQU[1])&&(g_gucPhaseSQU[2]<=g_gucPhaseSQU[0])))                  ////////B相肯定会和A相在一个，但C相有可能会超前A相和B相
          	     	 	            phase_seq_flag=0;                          /////////相序为正相序
          	     	            }
          	                    else if(g_gucPhaseSQU[0] > g_gucPhaseSQU[1])   //BCA 
                                    {
                                        if((g_gucPhaseSQU[0] >= g_gucPhaseSQU[2])&&(g_gucPhaseSQU[2] >= g_gucPhaseSQU[1]))
                                            phase_seq_flag=0; 
                                    }
                                }
                            }
          	     	 }
          	    }
      	     	 }	     	  	 
      
            }
            if(Value1 >= 800 && Value2 >= 800 && Polarity == 3)//如果两侧数据变化厉害，且极性相反
            {
                g_sSampleData.m_gAcAdcData[i][LastSamTail] = g_sSampleData.m_gAcAdcData[i][Last2SamTail];   //用上一个值代替
               // SetRIStatus(FLT_NC17 + (i << 1), 1);
            }
            else if(Value1 >= 500 && Value2 <= 100 && Value3 >=500 && Polarity == 3)
            {
                g_sSampleData.m_gAcAdcData[i][LastSamTail] = g_sSampleData.m_gAcAdcData[i][Last3SamTail];   //用上一个值代替
                g_sSampleData.m_gAcAdcData[i][Last2SamTail] = g_sSampleData.m_gAcAdcData[i][Last3SamTail];  //用上一个值代替
              //  SetRIStatus(FLT_NC17 + (i << 1), 1);
            }

        }
        else//如果数据异常，则赋上一个点的值
        {
            g_sSampleData.m_gAcAdcData[i][AcSamTail] = g_sSampleData.m_gAcAdcData[i][LastSamTail];
            //SetRIStatus(FLT_NC18 + (i << 1), 1);
        }
    }

}

//==============================================================================
//  函数名称   : CHECK8PLUS
//  功能描述   : 发完8个脉冲，需要检测脉冲是否有效发送短信
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者     ：张弢
//==============================================================================

void CHECK8PLUS(void)
{
//发送硬遥信
    if(g_MaichongNum >= 6)   //首次动作有效
    {
      g_FinishACTFlag = 0x55;   
    }
	SaveLOG(LOG_8FULS_STA,0);
    g_MaichongNum = 0;
    
    if(pulse_phase_flag ==1)
	g_gRmtInfo[YX_PHASEA_ACT] = 1;
    else if(pulse_phase_flag ==2)
	g_gRmtInfo[YX_PHASEB_ACT] = 1;
    else if(pulse_phase_flag ==3)	
	g_gRmtInfo[YX_PHASEC_ACT] = 1;	
    
    if(g_FinishACTFlag == 0x55)
    {
      g_FinishACTFlag = 0;
      g_gRmtInfo[YX_ACTION_SUCCESS] = 1;
      pulse_success=0x55;
    
	eight_ctl_flag=5;              
     	KC1_ON;             ///新电路板 
     	
	sign_RSSI_delay=5;                     //////////////发出有效8脉冲后，置延时，若在延时时间内有子站询问，则认为是故障    
	//if(moniguzhang!=1) //=1为模拟故障 不发送短信
	newsms_8pluse = ON;
 	//CreatNewSMS(EIGHT_PULSE);                      //产生8脉冲短信 //张弢//20161217
          sign_RSSI_time[5]=g_sRtcManager.m_gRealTimer[RTC_SEC];	
          sign_RSSI_time[4]= g_sRtcManager.m_gRealTimer[RTC_MINUT];
          sign_RSSI_time[3] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
          sign_RSSI_time[2] = g_sRtcManager.m_gRealTimer[RTC_DATE];
          sign_RSSI_time[1] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
          sign_RSSI_time[0] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000); 	
	  sign_RSSI_time1[5]=g_sRtcManager.m_gRealTimer[RTC_SEC];	
          sign_RSSI_time1[4]= g_sRtcManager.m_gRealTimer[RTC_MINUT];
          sign_RSSI_time1[3] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
          sign_RSSI_time1[2] = g_sRtcManager.m_gRealTimer[RTC_DATE];
          sign_RSSI_time1[1] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
          sign_RSSI_time1[0] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000); 
    	}
}
//==============================================================================
//  函数名称   : JAGACTx
//  功能描述   : 动作相序分别为1,2,3,4
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者     ：张弢
//==============================================================================

void JAGACT1(void)//动作2次 超前相动作，滞后相动作
{
  //unsigned int unTemp = 0;
  if((eight_pulse_counter==0)&&(efslatch_flag==0))
  {	 	   
    if((eight_pulse_flag>0)&&(eight_pulse_flag<rep33))
    {
      eight_pulse_flag+=1;
      if(g_gOverLoadFlg == ON)
      {
        eight_pulse_flag = rep17;  //检测到过流直接退出 
      }
      if((eight_pulse_flag%2)==0)  ///////高脉冲
      {
        eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_MODFK];
        //g_sRecData.m_ucRecStart = ON;
        if(pulse_phase_flag==1)    ///////A相
        {g_gKONBK=1;
          KB0_OFF;
          KC0_OFF;
          KA0_ON;
          g_sRecData.m_ucRecStart = ON;
	   g_gKON=1;	 
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_AMODFK];
        }
        else if(pulse_phase_flag==2)  ///////B相
        {	g_gKONBK=2;
          KC0_OFF;
          KA0_OFF;
          KB0_ON;
	   g_gKON=2; 
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_BMODFK];
        }
        else if(pulse_phase_flag==3)  //////C相
        {g_gKONBK=3;
          KA0_OFF;
          KB0_OFF;
          KC0_ON;
	   g_gKON=3;		  
          g_sRecData.m_ucRecStart = ON;
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_CMODFK];	  
        }
        if(pulse_phase_flag != 2 ) //B相不录播
        {
        }
      }	
      else if((eight_pulse_flag%4)==3)  ///////低脉冲 1S
      {
        eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
        if(pulse_phase_flag==1)    ///////A相
        {
          KA0_OFF;
          g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];	  
        }
        else if(pulse_phase_flag==2)  ///////B相
        {
          KB0_OFF;
          g_gKON=OFF;	
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];	  
        }
        else if(pulse_phase_flag==3)  //////C相
        {
          KC0_OFF; 
          g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_CMODFK];	  
        }
      }	
      else if((eight_pulse_flag%4)==1)  ///////低脉冲 1.25S      2014.8.5修改为1.5S
      {
        eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
        if(pulse_phase_flag==1)    ///////A相
        {
          KA0_OFF;
          g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];	  
          //g_gRmtInfo[0] &= ~YX_A_ACTION;
        }
        else if(pulse_phase_flag==2)  ///////B相
        {
          KB0_OFF;          
          g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];	  
          // g_gRmtInfo[0] &= ~YX_B_ACTION;
        }
        else if(pulse_phase_flag==3)  //////C相
        {
          KC0_OFF; 
          g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_CMODFK];	  
          // g_gRmtInfo[0] &= ~YX_C_ACTION;
        }  
      }		    	
    }
/*	
    if(eight_pulse_flag == 17 && pulse_phase_flag==1)
	pulse_phase_flag = 3;
    else if(eight_pulse_flag == 17 && pulse_phase_flag==2)
        pulse_phase_flag = 1; 
    else if(eight_pulse_flag == 17 && pulse_phase_flag==3)
        pulse_phase_flag = 2;
        */
  
  if(eight_pulse_flag==rep17)
  	CHECK8PLUS();

    if(eight_pulse_flag == rep17 && pulse_phase_flag==1)
	pulse_phase_flag = 3;
    else if(eight_pulse_flag == rep17 && pulse_phase_flag==2)
        pulse_phase_flag = 1; 
    else if(eight_pulse_flag == rep17 && pulse_phase_flag==3)
        pulse_phase_flag = 2;
  }	
  if(eight_pulse_flag>=rep33)      ////////数据发送完毕
  {
    CHECK8PLUS();
    eight_pulse_flag=0;
    pulse_phase_flag=0;      	
    efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];	        ///////闭锁17分钟  
    latch_upload_flag=0x55;
    uart0_event_flag=0;         ///////在这里置0，是为了让状态量最早显示
    g_gRmtInfo[YX_EFS_LATCH] = 1;   //置闭锁遥信位    
    SaveLOG(LOG_LATCH, 1);
    g_gRmtInfo[YX_EFS_ACT] = 0;   //投切状态 遥信置0
    chongfa=0;  moniguzhang=0;	
    g_gRmtMeas[RM_ACT_NUM] = 2;
   g_sRecData.m_gACTDelay = 200;//g_sRecData.m_ucActRecStart = OFF;//张弢录波 动作录波结束
   //g_gKONBK=OFF;
  }
}
void JAGACT2(void)//动作3次 超前相动作，滞后相动作，故障相动作
{
  //unsigned int unTemp = 0;
  if((eight_pulse_counter==0)&&(efslatch_flag==0))
  {	 	   
    if((eight_pulse_flag>0)&&(eight_pulse_flag<rep49))
    {
      eight_pulse_flag+=1;
      if(g_gOverLoadFlg == ON)
      {
        eight_pulse_flag = rep17;  //检测到过流直接退出 
      }
      if((eight_pulse_flag%2)==0)  ///////高脉冲
      {
        eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_MODFK];
        //g_sRecData.m_ucRecStart = ON;
        if(pulse_phase_flag==1)    ///////A相
        {g_gKONBK=1;
          KB0_OFF;
          KC0_OFF;
          KA0_ON;
          g_gKON=1;		  
          g_sRecData.m_ucRecStart = ON;
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_AMODFK];	  
        }
        else if(pulse_phase_flag==2)  ///////B相
        {	g_gKONBK=2;
          KC0_OFF;
          KA0_OFF;
          KB0_ON;
          g_gKON=2;	 
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_BMODFK];	  
        }
        else if(pulse_phase_flag==3)  //////C相
        {g_gKONBK=3;
          KA0_OFF;
          KB0_OFF;
          KC0_ON;
          g_gKON=3;		  
          g_sRecData.m_ucRecStart = ON;
	  eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_CMODFK];	  
        }
        if(pulse_phase_flag != 2 ) //B相不录播
        {/*
          g_sRecData.m_gFaultRecSOE[REC_TYPE] = pulse_phase_flag;
          unTemp = (g_sRtcManager.m_gRealTimer[RTC_SEC]*1000 + g_sRtcManager.m_gRealTimer[RTC_MICROSEC]); 
          g_sRecData.m_gFaultRecSOE[REC_MSL] = unTemp;
          g_sRecData.m_gFaultRecSOE[REC_MSH] = (unTemp>>8);
          g_sRecData.m_gFaultRecSOE[REC_MINU] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
          g_sRecData.m_gFaultRecSOE[REC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
          g_sRecData.m_gFaultRecSOE[REC_DAY] = g_sRtcManager.m_gRealTimer[RTC_DATE];
          g_sRecData.m_gFaultRecSOE[REC_MONTH] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
          g_sRecData.m_gFaultRecSOE[REC_YEAR] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000);*/
        }
       }	
      else if((eight_pulse_flag%4)==3)  ///////低脉冲 1S
      {
        eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
        if(pulse_phase_flag==1)    ///////A相
        {
          KA0_OFF;  g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];
        }
        else if(pulse_phase_flag==2)  ///////B相
        {
          KB0_OFF;  g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];
        }
        else if(pulse_phase_flag==3)  //////C相
        {
          KC0_OFF;  g_gKON=OFF; 
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_CMODFK];	  
        }
      }	
      else if((eight_pulse_flag%4)==1)  ///////低脉冲 1.25S      2014.8.5修改为1.5S
      {
        eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
        if(pulse_phase_flag==1)    ///////A相
        {
          KA0_OFF;  g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];	  
          //g_gRmtInfo[0] &= ~YX_A_ACTION;
        }
        else if(pulse_phase_flag==2)  ///////B相
        {
          KB0_OFF;  g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];	  
          // g_gRmtInfo[0] &= ~YX_B_ACTION;
        }
        else if(pulse_phase_flag==3)  //////C相
        {
          KC0_OFF;   g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_CMODFK];	  
          // g_gRmtInfo[0] &= ~YX_C_ACTION;
        }  
      }		    	
  	}
	/*
  	if(eight_pulse_flag == 17 && pulse_phase_flag==1)
      pulse_phase_flag = 3;
    else if(eight_pulse_flag == 17 && pulse_phase_flag==2)
      pulse_phase_flag = 1;  
    else if(eight_pulse_flag == 17 && pulse_phase_flag==3)
      pulse_phase_flag = 2;
    
    if(eight_pulse_flag == 33 && pulse_phase_flag==1)
      pulse_phase_flag = 3;
    else if(eight_pulse_flag == 33 && pulse_phase_flag==2)
      pulse_phase_flag = 1;  
    else if(eight_pulse_flag == 33 && pulse_phase_flag==3)
      pulse_phase_flag = 2; 
      */
	
 if(eight_pulse_flag==rep17)
  	CHECK8PLUS(); 
 if(eight_pulse_flag==rep33)
  	CHECK8PLUS(); 

  if(eight_pulse_flag == rep17 && pulse_phase_flag==1)
      pulse_phase_flag = 3;
    else if(eight_pulse_flag == rep17 && pulse_phase_flag==2)
      pulse_phase_flag = 1;  
    else if(eight_pulse_flag == rep17 && pulse_phase_flag==3)
      pulse_phase_flag = 2;
    
    if(eight_pulse_flag == rep33 && pulse_phase_flag==1)
      pulse_phase_flag = 3;
    else if(eight_pulse_flag == rep33 && pulse_phase_flag==2)
      pulse_phase_flag = 1;  
    else if(eight_pulse_flag == rep33 && pulse_phase_flag==3)
      pulse_phase_flag = 2; 
 } 
 if(eight_pulse_flag>=rep49)      ////////数据发送完毕
  {
    CHECK8PLUS();
    eight_pulse_flag=0;
    pulse_phase_flag=0;    	
    //if(moniguzhang!=1) //=1为模拟故障 不闭锁	
    	efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////闭锁17分钟
    	latch_upload_flag=0x55;
    //else 
    	{
    	 //g_gRmtInfo[0] &= ~YX_ACTION_SUCCESS;
	 //g_gRmtInfo[0] &= ~YX_PHASEA_ACT;	
	 //g_gRmtInfo[0] &= ~YX_PHASEB_ACT;
	 //g_gRmtInfo[0] &= ~YX_PHASEC_ACT;
    	}    	
    uart0_event_flag=0;         ///////在这里置0，是为了让状态量最早显示
    g_gRmtInfo[YX_EFS_LATCH] = 1;   //置闭锁遥信位  
    SaveLOG(LOG_LATCH, 1);
    g_gRmtInfo[YX_EFS_ACT] = 0;   //投切状态 遥信置0    
    chongfa=0;moniguzhang=0;	
   g_gRmtMeas[RM_ACT_NUM] = 3;
   g_sRecData.m_gACTDelay = 200;//g_sRecData.m_ucActRecStart = OFF;//张弢录波 动作录波结束	
   //g_gKONBK=OFF;
  }
}
void JAGACT3(void)//动作2次 只有AC相有接触器，超前相动作，另一相动作
{
  //unsigned int unTemp = 0;
  if((eight_pulse_counter==0)&&(efslatch_flag==0))
  {	 	   
    if((eight_pulse_flag>0)&&(eight_pulse_flag<rep33))
    {
      eight_pulse_flag+=1;
      if(g_gOverLoadFlg == ON)
      {
        eight_pulse_flag = rep17;  //检测到过流直接退出 
      }
      if((eight_pulse_flag%2)==0)  ///////高脉冲
      {
        eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_MODFK];
        //g_sRecData.m_ucRecStart = ON;
        if(pulse_phase_flag==1)    ///////A相
        {g_gKONBK=1;
          KB0_OFF;
          KC0_OFF;
          KA0_ON;  g_gKON=1;
          g_sRecData.m_ucRecStart = ON;
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_AMODFK];	  
        }
        else if(pulse_phase_flag==2)  ///////B相
        {g_gKONBK=2;
          KC0_OFF;
          KA0_OFF;
          KB0_ON;  g_gKON=2;
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_BMODFK];	  
        }
        else if(pulse_phase_flag==3)  //////C相
        {g_gKONBK=3;
          KA0_OFF;
          KB0_OFF;
          KC0_ON;  g_gKON=3;
          g_sRecData.m_ucRecStart = ON;
	  eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_CMODFK];	  
        }
        if(pulse_phase_flag != 2 ) //B相不录播
        {/*
          g_sRecData.m_gFaultRecSOE[REC_TYPE] = pulse_phase_flag;
          unTemp = (g_sRtcManager.m_gRealTimer[RTC_SEC]*1000 + g_sRtcManager.m_gRealTimer[RTC_MICROSEC]); 
          g_sRecData.m_gFaultRecSOE[REC_MSL] = unTemp;
          g_sRecData.m_gFaultRecSOE[REC_MSH] = (unTemp>>8);
          g_sRecData.m_gFaultRecSOE[REC_MINU] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
          g_sRecData.m_gFaultRecSOE[REC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
          g_sRecData.m_gFaultRecSOE[REC_DAY] = g_sRtcManager.m_gRealTimer[RTC_DATE];
          g_sRecData.m_gFaultRecSOE[REC_MONTH] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
          g_sRecData.m_gFaultRecSOE[REC_YEAR] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000);*/
        }
       }	
      else if((eight_pulse_flag%4)==3)  ///////低脉冲 1S
      {
        eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
        if(pulse_phase_flag==1)    ///////A相
        {
          KA0_OFF;  g_gKON=OFF;
	eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];	  
        }
        else if(pulse_phase_flag==2)  ///////B相
        {
          KB0_OFF;  g_gKON=OFF;
	eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];	  
        }
        else if(pulse_phase_flag==3)  //////C相
        {
          KC0_OFF;   g_gKON=OFF;
	eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_CMODFK];	  
        }
      }	
      else if((eight_pulse_flag%4)==1)  ///////低脉冲 1.25S      2014.8.5修改为1.5S
      {
        eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
        if(pulse_phase_flag==1)    ///////A相
        {
          KA0_OFF;  g_gKON=OFF;
	eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];	  
          //g_gRmtInfo[0] &= ~YX_A_ACTION;
        }
        else if(pulse_phase_flag==2)  ///////B相
        {
          KB0_OFF;  g_gKON=OFF;
	eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];	  
          // g_gRmtInfo[0] &= ~YX_B_ACTION;
        }
        else if(pulse_phase_flag==3)  //////C相
        {
          KC0_OFF;   g_gKON=OFF;
	eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_CMODFK];	  
          // g_gRmtInfo[0] &= ~YX_C_ACTION;
        }  
      }			
    } 
	/*
    if(eight_pulse_flag == 17 && pulse_phase_flag==1)
      pulse_phase_flag = 3;
    else if(eight_pulse_flag == 17 && pulse_phase_flag==3)
      pulse_phase_flag = 1;
      */
      
 if(eight_pulse_flag==rep17)
  	CHECK8PLUS();  

    if(eight_pulse_flag == rep17 && pulse_phase_flag==1)
      pulse_phase_flag = 3;
    else if(eight_pulse_flag == rep17 && pulse_phase_flag==3)
      pulse_phase_flag = 1;
  }  
  if(eight_pulse_flag>=rep33)      ////////数据发送完毕
  {
    CHECK8PLUS();
    eight_pulse_flag=0;
    pulse_phase_flag=0;    	
    //if(moniguzhang!=1) //=1为模拟故障 不闭锁	
    	efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////闭锁17分钟    	
    	latch_upload_flag=0x55;
    //else 
    	{
    	 //g_gRmtInfo[0] &= ~YX_ACTION_SUCCESS;
	 //g_gRmtInfo[0] &= ~YX_PHASEA_ACT;	
	 //g_gRmtInfo[0] &= ~YX_PHASEB_ACT;
	 //g_gRmtInfo[0] &= ~YX_PHASEC_ACT;
    	}    	
    uart0_event_flag=0;         ///////在这里置0，是为了让状态量最早显示
    g_gRmtInfo[YX_EFS_LATCH] = 1;   //置闭锁遥信位  
    SaveLOG(LOG_LATCH, 1);
    g_gRmtInfo[YX_EFS_ACT] = 0;   //投切状态 遥信置0    
    chongfa=0;	moniguzhang=0;
    g_gRmtMeas[RM_ACT_NUM] = 2;
   g_sRecData.m_gACTDelay = 200;//g_sRecData.m_ucActRecStart = OFF;//张弢录波 动作录波结束	
   //g_gKONBK=OFF;
  }
}

void JAGACT4(void)//动作1次 只有AC相有接触器，超前相动作
{
    //unsigned int unTemp = 0;
        if((eight_pulse_counter==0)&&(efslatch_flag==0))
        {	 	   
            if((eight_pulse_flag>0)&&(eight_pulse_flag<rep17))
            {
                eight_pulse_flag+=1;
                if(g_gOverLoadFlg == ON)
                {
                    eight_pulse_flag = rep17;  //检测到过流直接退出 
                }

                 if((eight_pulse_flag%2)==0)  ///////高脉冲
                 {
                     eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_MODFK];
                    // g_sRecData.m_ucRecStart = ON;
                     if(pulse_phase_flag==1)    ///////A相
                    {g_gKONBK=1;
                        KB0_OFF;
                        KC0_OFF;
                        KA0_ON;  g_gKON=1;
                        g_sRecData.m_ucRecStart = ON;
			eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_AMODFK];			
                  //      g_gRmtInfo[0] |= YX_A_ACTION;
                    }
                    else if(pulse_phase_flag==2)  ///////B相
                    {g_gKONBK=2;
                        KC0_OFF;
                        KA0_OFF;
                        KB0_ON;  g_gKON=2;
			eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_BMODFK];			
                   //     g_gRmtInfo[0] |= YX_B_ACTION;
                    }
                    else if(pulse_phase_flag==3)  //////C相
                    {g_gKONBK=3;
                        KA0_OFF;
                        KB0_OFF;
                        KC0_ON;  g_gKON=3;
                        g_sRecData.m_ucRecStart = ON;
			eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_CMODFK];			
                    //    g_gRmtInfo[0] |= YX_C_ACTION;
                    }
                    if(pulse_phase_flag != 2 ) //B相不录播
                    {/*
                        g_sRecData.m_gFaultRecSOE[REC_TYPE] = pulse_phase_flag;
                        unTemp = (g_sRtcManager.m_gRealTimer[RTC_SEC]*1000 + g_sRtcManager.m_gRealTimer[RTC_MICROSEC]); 
                        g_sRecData.m_gFaultRecSOE[REC_MSL] = unTemp;
                        g_sRecData.m_gFaultRecSOE[REC_MSH] = (unTemp>>8);
                        g_sRecData.m_gFaultRecSOE[REC_MINU] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
                        g_sRecData.m_gFaultRecSOE[REC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
                        g_sRecData.m_gFaultRecSOE[REC_DAY] = g_sRtcManager.m_gRealTimer[RTC_DATE];
                        g_sRecData.m_gFaultRecSOE[REC_MONTH] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
                        g_sRecData.m_gFaultRecSOE[REC_YEAR] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000);*/
                    }
                 }	
                 else if((eight_pulse_flag%4)==3)  ///////低脉冲 1S
                 {
                     eight_pulse_counter=g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
                     if(pulse_phase_flag==1)    ///////A相
                    {
                          KA0_OFF;  g_gKON=OFF;
			eight_pulse_counter=g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];			  
                     //     g_gRmtInfo[0] &= ~YX_A_ACTION;
                    }
                    else if(pulse_phase_flag==2)  ///////B相
                    {
                          KB0_OFF;  g_gKON=OFF;
			eight_pulse_counter=g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];			  
                       //   g_gRmtInfo[0] &= ~YX_B_ACTION;
                    }
                    else if(pulse_phase_flag==3)  //////C相
                    {
                          KC0_OFF;   g_gKON=OFF;
			eight_pulse_counter=g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_CMODFK];			  
                       //   g_gRmtInfo[0] &= ~YX_C_ACTION;
                    }   
                 }
                 else if((eight_pulse_flag%4)==1)  ///////低脉冲 1.25S
                 {
                     eight_pulse_counter=g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
                     if(pulse_phase_flag==1)    ///////A相
                    {
                          KA0_OFF;  g_gKON=OFF;
			eight_pulse_counter=g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];			  
                     //     g_gRmtInfo[0] &= ~YX_A_ACTION;
                    }
                    else if(pulse_phase_flag==2)  ///////B相
                    {
                          KB0_OFF;  g_gKON=OFF;
			eight_pulse_counter=g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];			  
                        //  g_gRmtInfo[0] &= ~YX_B_ACTION;
                    }
                    else if(pulse_phase_flag==3)  //////C相
                    {
                          KC0_OFF;   g_gKON=OFF;
			eight_pulse_counter=g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_CMODFK];			  
                       //   g_gRmtInfo[0] &= ~YX_C_ACTION;
                    }   
                 }
              	
             } 	
         }      
         if(eight_pulse_flag>=rep17)      ////////数据发送完毕
         {
             CHECK8PLUS();	
             eight_pulse_flag=0;
             pulse_phase_flag=0;
    				 
             //if(moniguzhang!=1) //=1为模拟故障 不闭锁			 
             	  efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////闭锁17分钟
             	  latch_upload_flag=0x55;
    		//else 
    		{
    	 		//g_gRmtInfo[0] &= ~YX_ACTION_SUCCESS;
	 		//g_gRmtInfo[0] &= ~YX_PHASEA_ACT;	
	 		//g_gRmtInfo[0] &= ~YX_PHASEB_ACT;
	 		//g_gRmtInfo[0] &= ~YX_PHASEC_ACT;
    		}             	  
             uart0_event_flag=0;         ///////在这里置0，是为了让状态量最早显示
             g_gRmtInfo[YX_EFS_LATCH] = 1;   //置闭锁遥信位 
             SaveLOG(LOG_LATCH, 1);
             g_gRmtInfo[YX_EFS_ACT] = 0;   //投切状态 遥信置0             
	      chongfa=0;moniguzhang=0;
             g_gRmtMeas[RM_ACT_NUM] = 1;
	      g_sRecData.m_gACTDelay = 200;//g_sRecData.m_ucActRecStart = OFF;//张弢录波 动作录波结束	
	      //g_gKONBK=OFF;
         }
}

//==============================================================================
//  函数名称   : ContronlRelay
//  功能描述   : 接触器控制模块，分为两种控制方式，单相投切与3相投切
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者     ：
//==============================================================================
void ContronlRelay(void)
{
   // unsigned int unTemp = 0;
    switch(g_gRunPara[RP_PLUSE_NUM])
    	{
    	case 8:
		rep17=17;rep33=33;rep49=49;
		break;
    	case 7:
		rep17=15;rep33=29;rep49=43;	
		break;
    	case 6:
		rep17=13;rep33=25;rep49=37;	
		break;
    	default:
		g_gRunPara[RP_PLUSE_NUM]=8;	
		break;		
    	}
	
    if((g_gProcCnt[PC_JAG_ACT] == 4)||(g_gProcCnt[PC_EFS_MODEL]==0))          //动作2次 只有AC相有接触器，超前相动作，另一相动作
        JAGACT4();
    else if(g_gProcCnt[PC_JAG_ACT] == 3)  //动作1次 只有AC相有接触器，超前相动作
        JAGACT3();
    else if(g_gProcCnt[PC_JAG_ACT] == 1)  //动作2次 超前、滞后
        JAGACT1();
    else if(g_gProcCnt[PC_JAG_ACT] == 2)  //动作3次 超前、滞后、故障
        JAGACT2();


}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
   // unsigned int i,j;
      static unsigned char M125SecCount = 0;  // 1.25毫秒计时 // 张弢测试中断嵌套
	static unsigned char pjno=0;
	static unsigned char McSecCount=0;
 //#ifdef SD_101S     
      static unsigned char M05SecCount = 0;  // 0.5毫秒计时 // 张弢测试中断嵌套
 //#endif     
  //static unsigned char ucTime = 0;
    
   // WDTCTL = WDTPW + WDTCNTCL;

    ADC12IFG = 0x3FF;   //以免中断异常
    M125SecCount++;// 张弢测试中断嵌套 
    
    g_unAdcData[0] = ADC12MEM0; //UA为了保证ADC12启动的同步，且保证启动后ADC12MEM的数据已经取出来，建立此缓冲区
    g_unAdcData[1] = ADC12MEM1;//UB
    g_unAdcData[2] = ADC12MEM2;//UC
    g_unAdcData[3] = ADC12MEM3;//U0
    g_unAdcData[4] = ADC12MEM4;//I0
    g_unAdcData[5] = ADC12MEM5;//UPt	
    g_unAdcData[5] = g_unAdcData[3];//Uo to Upt
    g_unAdcData[6] = ADC12MEM6;//UCap	
    g_gRmtMeas[RM_UCAP] = g_unAdcData[6];
    //if(g_unAdcData[6]>2800)g_gRmtInfo[YX_SBP_OFF]=0;//5.87
    //if(g_unAdcData[6]<2600)g_gRmtInfo[YX_SBP_OFF]=1;//5.4v		
    ADC12CTL0 |= ADC12SC;     
#ifdef SD_101S
    if(M05SecCount&0x01)
    	{		
		RecData();
    	}
     M05SecCount++; 	
#endif

    if(M125SecCount>4)//每5次采样5*250uS=1.25ms 则计算存储数据// 张弢测试中断嵌套
    	{// 张弢测试中断嵌套
        GetAcSamData();//把交流数据经过软件滤波后，存放到g_sSampleData中 
        M125SecCount = 0;
        //CalcuProtMeas();//张弢取消此程序//利用均方根算法计算，把计算结果保存到保护电量中  
        ProtStart();//启动元件判断
        //ProtLogic();//张弢 程序放入CalcuRmtMeas(void) 5ms执行1次       
        RecActData();
		McSecCount++;	
		if(McSecCount>=16)//每32 采样32*56.25uS=625us 则计算存储数据// 张弢测试中断嵌套
    		{	
		 	g_unRmCaluFlag = ON; 
		 	McSecCount = 0;
    		}		
    	}
#ifndef SD_101S	
    RecData();
#endif	

    if((M05SecCount&0x7f)==0)//每32 采样32*56.25uS=625us 则计算存储数据// 张弢测试中断嵌套
    	{	 
	 	g_gRmtMeasPJ[0][pjno]=g_gRmtMeas[1];
	 	g_gRmtMeasPJ[1][pjno]=g_gRmtMeas[2];
		g_gRmtMeasPJ[2][pjno]=g_gRmtMeas[3];
       	pjno++;
		if(pjno>31)pjno=0;;	
    	}

        //扫描开关量输入
       /*ScanDin();

        {
 
            ProtStart();//启动元件判断
            //启动元件判断，若计时满足，则启动永磁驱动，禁止0.625ms中断，禁止1ms中断(根据当前ms时刻的位置计算下次的计时，减少ms误差)
            ProtLogic();
              
        }

        ScanOut();      //完成出口动作的判断和启动，包括遥控的合分闸也是在大循环中置相关标志，在中断中启动。
*/
  
}

	
// Timer A1 interrupt service routine


#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)    //毫秒中断函数
{
    unsigned char i = 0;
    //unsigned int unTemp = 0;
    
    static unsigned int MicSecCount = 0;  //10毫秒计时
    static unsigned int Mic50SecCount = 0;  //50毫秒计时//张弢 遥测越限	
    static unsigned int SecCount = 0;     //秒计时
    static unsigned char NumKON = 0;     //8脉冲时 每个脉冲的10ms计数
    static unsigned char Numyc = 0;     //8脉冲遥测值 yc[] 的数组下标
    //static unsigned char Numk=0;     //8脉冲遥测值 yc[] 的数组下标
    switch(TA1IV)
    {
         case 0x02:
//#ifdef INT_NEST  			 	
//UCA1IE &= (~(UCRXIE + UCTXIE));              //关闭 USCI_A1RX 中断  // 张弢测试中断嵌套	 
//UCA2IE &= (~(UCRXIE + UCTXIE));              // 关闭 USCI_A2 RX 中断  // 张弢测试中断嵌套// 张弢测试中断嵌套	
_EINT();//开总中断// 张弢测试中断嵌套	
//delay(2000);	// 张弢测试中断嵌套
//#endif 
            TA1CCR1 += 250;
            for(i = 0;i <= 2;i++)
    {
      if(g_sRxBuff[i].m_iRcvCount > 0)
      {
        g_sRxBuff[i].m_iRcvCount--;
        if(g_sRxBuff[i].m_iRcvCount == 0)
        {
          g_sRxBuff[i].m_iRcvFlag = ON;
          
        }
      }
    }
            g_sRtcManager.m_gRealTimer[RTC_MICROSEC]++;  //系统实时时钟g_sRtcManager.m_gRealTimer的毫秒累加
            ScanDinYX();
			ScanLOG();
            MicSecCount++;
	     Mic50SecCount++;		
            SecCount = g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
            //RmInfoChk();//张弢 移入主循环，否则栈太大无法中断嵌套
            //ScanDinYX();//张弢 移入主循环，否则栈太大无法中断嵌套
            /*
            if((g_gRealTimerbk[RTC_YEAR]!=g_sRtcManager.m_gRealTimer[RTC_YEAR])
				||(g_gRealTimerbk[RTC_MONTH]!=g_sRtcManager.m_gRealTimer[RTC_MONTH])
				||(g_gRealTimerbk[RTC_DATE]!=g_sRtcManager.m_gRealTimer[RTC_DATE])
				||(g_gRealTimerbk[RTC_HOUR]!=g_sRtcManager.m_gRealTimer[RTC_HOUR]))
            	{
            	if(g_gRealTimerbk[RTC_SEC]==0)
            		ReadRealTime();	
            	}*/
//******************************************************
////8脉冲校准       g_gModfk[6]
	unsigned char ka,kb,kc;
	if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_CON_NC])
		{//0=nc
		ka=0x08;kb=0x10;kc=0x20;
		}
	else
		{//0=no
		ka=0;kb=0;kc=0;
		}	
	if(KJa1==ka)//||(KJb1==kb)||(KJc1==kc))
		{
		kat++;
		}
	else
		{
		if(kat>0)
			{g_gModfk[Numk]=kat;Numk++;}
		kat=0;
		}
	if(KJb1==kb)
		{
		kbt++;
		}
	else
		{
		if(kbt>0)
			{g_gModfk[Numk]=kbt;Numk++;}
		kbt=0;
		}	
	if(KJc1==kc)
		{
		kct++;
		}
	else
		{
		if(kct>0)
			{g_gModfk[Numk]=kct;Numk++;}
		kct=0;
		}
	if(Numk>=24)Numk=0;
//******************************************************
////张弢0330 如果串口长时间不处于IDLE状态，复位串口
	     g_ucGPRSState_TCounter++;
	     if(g_ucGPRSState_TCounter>5000)
	     	{
	     		g_ucGPRSState = GPRSState_IDLE;
			g_ucGPRSState_TCounter=0;
	     	}
//******************************************************
//LED_RUN_OFF;
            if(MicSecCount >= 10)  //10毫秒计时
            {           
             	MicSecCount = 0;
                if(eight_pulse_counter>0)
                    eight_pulse_counter--;
                if(g_ucDuanxianFlg <= 50)
                    g_ucDuanxianFlg++;
                
				ScanSoftLacth();		
				
                if((P5IN&BIT5)== 0)     /////////检测到掉电，报警(新电路板)
       	         {
       	           if(Powerdown_counter<50)	
                       {Powerdown_counter++;}	
       	         }
                 else
       	         {
       	             if(Powerdown_counter>0)	
                     {
             	        if(Powerdown_counter>1)	
             		    Powerdown_counter-=5;
                      else
              	         Powerdown_counter--;
              	
                      }
       	         }
                 if(eight_delay_counter>0)
                	{
                    eight_delay_counter--;
		      		if((eight_delay_counter==80)&&(g_sRecData.m_ucActRecStart == CLOSE)&&(g_sRecData.m_ucRecSavingFlag == OFF))	
		      			{//动作录波要在继电器动作前最少0.5秒开始，现在设定提前0.8s
						unsigned long ulAddr =FADDR_RECORDER_ACTDATA+ (unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;//flash地址  
  						g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址 	
  						g_sRecData.m_unRecAcTail =0; 
	    	 			g_sRecData.m_ucActRecStart = ON;//张弢录波 动作录波开始	
	    	 			g_sRecData.m_LuboType = LuboType_ACT;
	    				g_test=0;           		
                    	g_sRecData.m_gFaultRecSOE[REC_MSL] =g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
                    	g_sRecData.m_gFaultRecSOE[REC_MSH] = g_sRtcManager.m_gRealTimer[RTC_SEC];
                    	g_sRecData.m_gFaultRecSOE[REC_MINU] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
                    	g_sRecData.m_gFaultRecSOE[REC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
                    	g_sRecData.m_gFaultRecSOE[REC_DAY] = g_sRtcManager.m_gRealTimer[RTC_DATE];
                    	g_sRecData.m_gFaultRecSOE[REC_MONTH] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
                    	g_sRecData.m_gFaultRecSOE[REC_YEAR] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000);
						SaveLOG(LOG_8FULS_STA,1);
		      			}		
                    if(eight_delay_counter==0)
                        eight_delay_flag=0x55;
                	} 
                ContronlRelay();       //投切接触器                

			unsigned char ka,kb,kc;
			if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_CON_NC])
				{//0=nc
				ka=0x08;kb=0x10;kc=0x20;
				}
			else
				{//0=no
				ka=0;kb=0;kc=0;
				}	
#ifdef CONDIN_3
             if(g_gRmtMeas[RM_I0] >= g_gProcCntJug[PC_PULSE_VALID])
            	{
             	g_I0RmtZeroNum=0;
			//g_gRmtInfo[YX_BREAK]=0;
             	}
	  	  if((KJa1==ka)||(KJb1==kb)||(KJc1==kc)||(g_gKON==1)||(g_gKON==2)||(g_gKON==3))
#else	                
          if(KJb1==kb)
#endif					
          	{
         	NumKON++;
		   	if(NumKON==5)
				{//8脉冲继电器闭合50ms后，记录电流值
				if(Numyc<8)
					{
			  		yc[Numyc]=g_gRmtMeas[RM_I0]; 
                   	Numyc++;
					} 
		      	}                    
            if(g_gRmtMeas[RM_I0] >= g_gProcCntJug[PC_PULSE_VALID])
            	{
             	g_I0RmtZeroNum=0;
			   	g_gRmtInfo[YX_BREAK]=0; 			
            	if(g_I0RmtNum < 19)
                	g_I0RmtNum++;
            	}
			else
		      	{
		      	g_I0RmtZeroNum++;
		      	}
            }
			else //if(g_gKON==OFF)//开关未闭合
				{
		  		NumKON=0;		  		
		  		if(Numyc>=8)Numyc=0;
		  		if(g_I0RmtNum >= 3) //((g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_MODFK])/2))     //检测到有效电流
                  	{
                    g_I0RmtNum = 0;
                    if(g_MaichongNum < 8)
                        g_MaichongNum++;             
                  	}
                    g_I0RmtNum = 0;
				}
		if( g_gRmtInfo[YX_EARTH_FAULT] == 0)g_I0RmtZeroNum = 0; 
		if(g_I0RmtZeroNum>=2*(g_gRunPara[RP_PLUSE_TIME]-g_gRunPara[RP_PLUSE_MODFK]))
			{
			g_gRmtInfo[YX_BREAK]=1; newsms_abn= ON;
			SaveLOG(LOG_BREAK, 1);
			g_gRmtInfo[YX_EFS_ACT] = 0;   //投切状态 遥信置0
                        g_I0RmtZeroNum = 0;
			if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_BREAK_STOP8PUL])
				{
				//停止8脉冲
				KA0_OFF; KB0_OFF;KC0_OFF; g_gKON=OFF;//g_gKONBK=OFF;
				eight_pulse_flag=0;
    				pulse_phase_flag=0; 
    				efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////闭锁17分钟
    				latch_upload_flag=0x55;      	
    				uart0_event_flag=0;         ///////在这里置0，是为了让状态量最早显示
    				g_gRmtInfo[YX_EFS_LATCH] = 1;   //置闭锁遥信位 
    				SaveLOG(LOG_LATCH, 1);
    				chongfa=0;	moniguzhang=0;
    				g_gRmtMeas[RM_ACT_NUM] = 0;
  				 g_sRecData.m_gACTDelay = 200;//g_sRecData.m_ucActRecStart = OFF;//张弢录波 动作录波结束	
				}
			}

                
                if(main_reset_flag==0)
    	        {
    	            WDG_SET;                         //////设置看门狗    watch dog  hardware      
    	        }
                else if(main_reset_flag == 0x55)
                {
                    //重启系统
                    WDTCTL = WDTPW+WDTIS1+WDTIS0 + WDTIS2;//修改看门狗的周期，从而能够更快重启
                    _DINT();       //关闭全局中断
                    while(1);
                }
                if(fault_begin == 0x55 && fault_time == 10 && efslatch_flag == 0)   //100毫秒启动录波
                {
                    //fault_begin_first = 0;
                  //  g_sRecData.m_ucRecStart = ON;
                    //unTemp = (g_sRtcManager.m_gRealTimer[RTC_SEC]*1000 + g_sRtcManager.m_gRealTimer[RTC_MICROSEC]);
                    /*
                    g_sRecData.m_gFaultRecSOE[REC_MSL] =g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
                    g_sRecData.m_gFaultRecSOE[REC_MSH] = g_sRtcManager.m_gRealTimer[RTC_SEC];
                    g_sRecData.m_gFaultRecSOE[REC_MINU] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
                    g_sRecData.m_gFaultRecSOE[REC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
                    g_sRecData.m_gFaultRecSOE[REC_DAY] = g_sRtcManager.m_gRealTimer[RTC_DATE];
                    g_sRecData.m_gFaultRecSOE[REC_MONTH] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
                    g_sRecData.m_gFaultRecSOE[REC_YEAR] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000);
                    */
                }
                if(fault_begin==0x55)//////故障已经开始
                {
                    fault_time++;
                    fault_begin=0;
                   // fault_begin_first = 0x55;
                    fault_end=0;
			g_gRmtInfo[YX_RH_SUCCESS]=0;//燃弧成功遥信置位
			rh_counter=0;
                } 
                else if(fault_end==0x55)      //////////故障复归   	 
                {
                    g_sRecData.m_ucFaultRecStart = OFF;
                    fault_end=0;
                    fault_begin=0;
                    if(fault_time<=5)
                        {
                        fault_time=0;
			   if(rh_send_ok==0x55)	
			   	{
			   	g_gRmtInfo[YX_RH_SUCCESS]=1;//燃弧成功遥信置位
				rh_counter=g_gRunPara[RP_RHT_DELAY];//遥信延时开始
			   	}
			   rh_send_ok = 0;
			   }
                    else
          	        {
          	        fault_time-=5;
                    	 }
                }
		  if((g_gRmtInfo[YX_EARTH_FAULT]==0)&&(rh_send_ok == 0)&&(eight_pulse_flag==0)&&(efslatch_flag==0))
		  {
		    if((g_gRunPara[RP_RHSEND_TIME1]>0)&&(g_gRunPara[RP_RHPLUSE_TIME2]>0))//RP_RHSEND_TIME1==0 或者RP_RHPLUSE_TIME2==0，关闭燃弧功能
		  	{
		  	if((fault_time>=(g_gRunPara[RP_RHSEND_TIME1]-60))&&(g_sRecData.m_ucActRecStart == CLOSE))
		  		{
				//开始录波
	    			g_test=0;
    				g_sRecData.m_unRecAcTail =0; 
          			g_sRecData.m_gFaultRecSOE[REC_MSL] =g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
              		g_sRecData.m_gFaultRecSOE[REC_MSH] = g_sRtcManager.m_gRealTimer[RTC_SEC];
           			g_sRecData.m_gFaultRecSOE[REC_MINU] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
           			g_sRecData.m_gFaultRecSOE[REC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
             			g_sRecData.m_gFaultRecSOE[REC_DAY] = g_sRtcManager.m_gRealTimer[RTC_DATE];
             			g_sRecData.m_gFaultRecSOE[REC_MONTH] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
             			g_sRecData.m_gFaultRecSOE[REC_YEAR] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000);  
  				unsigned long ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;//flash地址  
  				g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址 	
  				g_sRecData.m_LuboType = LuboType_XH;
  				g_sRecData.m_ucActRecStart = ON;//张弢录波 动作录波开始	
  				g_sRecData.m_gXHDelay=fault_time;
		  		}
				
                	//if(fault_time>=g_gRunPara[RP_RHSEND_TIME1])
                	if(g_sRecData.m_gXHDelay>=g_gRunPara[RP_RHSEND_TIME1])
				{//开故障相继电器
				g_gRmtInfo[YX_RH_ACT]=1; //装置熄弧动作 遥信
				if(fault_pluse==1)
					{
					KA0_ON;g_gKON = 1;g_gKONBK=1; 
					KB0_OFF;KC0_OFF;
					}
				else if(fault_pluse==2)
					{
					KB0_ON;g_gKON = 2;g_gKONBK=2;
					KA0_OFF;KC0_OFF;
					}
				else if(fault_pluse==3)
					{
					KC0_ON;g_gKON = 3;g_gKONBK=3; 
					KB0_OFF;KA0_OFF;
					}
                		}
		  	if(g_sRecData.m_gXHDelay>=(g_gRunPara[RP_RHSEND_TIME1]+g_gRunPara[RP_RHPLUSE_TIME2]-g_gRunPara[RP_PLUSEXH_MODFK]))
		  		{//关故障相继电器
		  		g_gRmtInfo[YX_RH_ACT]=0; //装置熄弧动作 遥信
		  		KA0_OFF;
				KB0_OFF;	
				KC0_OFF;g_gKON = OFF; //g_gKONBK=OFF;	
				rh_send_ok = 0x55;
				//g_sRecData.m_ucActRecStart = OFF;//张弢录波 动作录波结束
				g_sRecData.m_gACTDelay = 30;//熄弧结束，再录300ms
				g_sRecData.m_gXHDelay=0;
		  		}
		  	}
		  }
		  /*
		  if((g_gRmtInfo[YX_EARTH_FAULT]==0)&&(rh_send_ok == 0x55))//燃弧脉冲发送后，出现故障复归，说明燃弧成功
		  {
			g_gRmtInfo[YX_RH_SUCCESS]=1;//燃弧成功遥信置位
			rh_counter=g_gRunPara[RP_RHT_DELAY];//遥信延时开始
			rh_send_ok = 0;
		  }	*/	  
                if((fault_time>=g_gProcCntJug[PC_T_DELAY]) && fault_save == 0)
                    fault_save=0x55;   	  		
                else if(fault_time==0)
                {
                    if(fault_save>0)	
                    {
                        fault_save=0;
                        fault_recovery_flag=0x55; 
                    }    
                }
              /*  if(pulse_time_flag!=0)   ////////做8脉冲处理
                {
                    if(pulse_flag==0x55)         ////////检测到8脉冲，就不再进行电压计算，只计算零序电流。
    	            {          	      
                        i=pulse_counter*29+8;	                
                        i=i+wave_counter; 
                        if(g_gRmtMeas[RM_I0]<1200)                                   	
                            g_sSoeData.m_gRecBuff[i]= g_gRmtMeas[RM_I0]/10 + 128;      //遥测值
                        else
                            g_sSoeData.m_gRecBuff[i]= g_gRmtMeas[RM_I0]/100;
                        wave_counter++; 
                    
                        if(wave_counter>=25)  //////////25个周波采集完毕
          	        {
          	            pulse_counter++;
          	            wave_counter=0; 
          	            pulse_flag=0;
          	        }          	   
    	             }
    	         } */
    	         if(g_sRecData.m_gXHDelay >0)g_sRecData.m_gXHDelay++;
    	         if(g_sRecData.m_gACTDelay > 0)
                	{
                	g_sRecData.m_gACTDelay--;//
                	if(g_sRecData.m_gACTDelay == 0)
                          g_sRecData.m_ucActRecStart = OFF;//张弢录波 动作录波结束	
                          g_gKONBK=OFF;
                	}
		  //SaveActRecData();			 
            }
	     if(Mic50SecCount >= 50)   //50mS计时//张弢 遥测越限	
             {
               g_gYCYueXian=0x55;//张弢 遥测越限	
               Mic50SecCount =0;
		 //if((g_sRecData.m_ucActRecStart != OFF))	   
		 SaveActRecData();	
		 ScanPT();
             }
	     		 
            if(SecCount >= 1000)   //秒计时
            {
               g_STimeout = ON;                  
            	if(gRes_rec.res_timeout > 0)
            		{
            		//gRes_rec.res_timeout ++;
            		}
                if(g_unSms_Count>0)//短信发送后g_unSms_Count置15
                	{
                	g_unSms_Count--;//短信发送后，未收到回复g_unSms_Count--
                	if(g_unSms_Count==0)
                		{
                		g_unSms_s = 0;//短信发送后，15s未收到回复,重发
                		g_unFailSms++;
				if(g_unFailSms>=3)
					{
					g_unFailSms = 0;
					g_unPhoneNo++;	   				
	   				if(g_unPhoneNo>=4)// 4个号码发完 发下一条
						{
						g_unPhoneNo=0;
						if(g_unFrameFlag_Sms != g_unFrameFlag_curSms)
       						g_unFrameFlag_curSms++;
						if(g_unFrameFlag_curSms>=MAX_SMS_NUM)
							g_unFrameFlag_curSms=0;
						}
                			}
                		}
                	}
                if(g_YXLBOKCounter>0)
                	{
                	g_YXLBOKCounter--;
			if(g_YXLBOKCounter==0)
				g_gRmtInfo[YX_LBOK]=0;
                	}
                if(g_TQBSCounter==0x55)
			KB1_ON;//投切、闭锁指示灯计数器  =0 是灭 =0x55 闭锁常亮  >=1投切闪烁 = 0x55;//投切、闭锁指示灯计数器  =0 是灭 =0x55 闭锁常亮  >=1投切闪烁
   		  else if(g_TQBSCounter==0)
		  	KB1_OFF;
		  else if(g_TQBSCounter>0)
		  	g_TQBSCounter++;

		  if((g_TQBSCounter&0x01)==0)
		  	KB1_OFF;
		  else if((g_TQBSCounter&0x01)==1)
		  	KB1_ON;

		  if(rh_counter>0)
		  	{
		  	rh_counter--;
			if(rh_counter==0)
				{
				g_gRmtInfo[YX_RH_SUCCESS]=0;//燃弧成功遥信置位
				}
		  	}
		  
                if(efslatch_flag>0)
                        {
                            g_TQBSCounter = 0x55;//投切、闭锁指示灯计数器  =0 是灭 =0x55 闭锁常亮  >=1投切闪烁
                            efslatch_flag--;
                            if(efslatch_flag==0)
                            {
               	                latch_upload_flag=0x55;     ///////置主动上传标志
               	                pulse_success=0;
                                g_gOverLoadFlg = 0;
                                g_gRmtInfo[YX_ACTION_SUCCESS]  = 0;
				   g_gRmtInfo[YX_PHASEA_ACT] = 0;	
				   g_gRmtInfo[YX_PHASEB_ACT] = 0;
				   g_gRmtInfo[YX_PHASEC_ACT] = 0;
                                g_gRmtInfo[YX_EFS_LATCH] = 0;   //置解锁遥信位 
                                SaveLOG(LOG_LATCH, 0);
                                g_gRmtInfo[YX_RH_FAIL] = 0;   //燃弧失败遥信复归
                                g_gRmtInfo[YX_MANUAL_ACTION] = 0;   //置手动投切结束
                               // g_gRmtInfo[0] &= ~YX_EIGHT_PULSE;   //有效8脉冲清除 
                               g_TQBSCounter = 0;//投切、闭锁指示灯计数器  =0 是灭 =0x55 闭锁常亮  >=1投切闪烁
                               g_I0RmtZeroNum = 0;
                               //rh_send_ok = 0;
                            }      ///////闭锁时间到，有效8脉冲也要复归
                        }
                ///////////闭锁17分钟 
                if((g_GPRSSendLink  < 60)&&(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_AUTOLINK]))
                {
                  g_GPRSSendLink ++;
                  if(g_GPRSSendLink  == 60)
                     g_GPRSSendLink  = ON;
                } 
		 if((g_GPRSSendLink  >= 60)&&(g_GPRSSendLink  !=ON)&&(g_GPRSSendLink  !=OFF))
		 	g_GPRSSendLink=0;
                if(g_gBeatNum > 0)
                {
                    g_gBeatNum--;
                    if(g_gBeatNum == 0)
                    {
                        g_gBeatNum = ((unsigned long)g_gRunPara[RP_BEAT_T]);
                        g_SendBeat = 0x55;
                       // m_wSendYcNum = 0;
                       // g_yxChangeflag |= BITF;//g_yxChangeflag的最高位标识有报文需要上传
                    }
                            
                }
                if(g_gsendYCTimeNum > 0)
                {
                    g_gsendYCTimeNum--;
                    if(g_gsendYCTimeNum == 0)
                    {
#ifdef YN_101S
                        g_gsendYCTimeNum = ((unsigned long)g_gRunPara[RP_SENDYC_T]*10);
#else
                        g_gsendYCTimeNum = ((unsigned long)g_gRunPara[RP_SENDYC_T]);
#endif
                        g_SendYcDingshi = 0x55;
                    }
                }
		   if(g_gsendZJTimeNum > 0)
                {
                    g_gsendZJTimeNum--;
                    if(g_gsendZJTimeNum == 0)
                    {
                        g_gsendZJTimeNum = ((unsigned long)g_gRunPara[RP_SENDZJ_T])*10;
                        g_SendZJDingshi = 0x55;
                       // m_wSendYcNum = 0;
                       // g_yxChangeflag |= BITF;//g_yxChangeflag的最高位标识有报文需要上传
                    }
                            
                }
                if(g_gsendYXTimeNum > 0)
                {
                    g_gsendYXTimeNum--;
                    if(g_gsendYXTimeNum == 0)
                    {
                        g_gsendYXTimeNum = ((unsigned long)g_gRunPara[RP_SENDYX_T]);
                        g_SendYXDingshi = 0x55;
                       // m_wSendYcNum = 0;
                       // g_yxChangeflag |= BITF;//g_yxChangeflag的最高位标识有报文需要上传
                    }
                            
                }
		
                if(g_sRtcManager.m_unStartCount > 0)
                {
                    g_sRtcManager.m_unStartCount--;
                    LED_RUN_ON;
                }
                else
                {
                    LED_RUN_TOGG;    //LED00
                }
          
                SecCount = 0;
                g_sRtcManager.m_gRealTimer[RTC_SEC]++;
                g_sRtcManager.m_gRealTimer[RTC_MICROSEC] = 0;
                
                if(power_ctl_flag>0)       //8脉冲遥控标志
                    power_ctl_flag--;
                if(eight_ctl_flag>0)       //断电遥控标志
                    eight_ctl_flag--;         
                if(abnormal_ctl_flag>0)    //异常遥控标志
                   abnormal_ctl_flag--;
               // ScanDin();     //扫描外部遥信
                 
                     
              /*  if(pulse_time_flag>0)   /////////提供8脉冲时间范围
                {
                    pulse_time_flag--;
                    if(pulse_time_flag==0) 
          	    {
          	        if(flash_write==0)    ///////////说明没有检测到起始电流
          	 	    flash_write=0x44;
          	        else
          	 	 flash_write=0x55; ///////////说明检测到起始电流	 
          	     } 
                }*/
                if(g_sRtcManager.m_gRealTimer[RTC_SEC] >= 60)     //分钟计时
	            {			
			g_NolinkReset++;
	                g_sRtcManager.m_gRealTimer[RTC_SEC] = 0;
	                g_sRtcManager.m_gRealTimer[RTC_MINUT]++;
			  g_gSaveload++;					
                        if(state_counter>0)              ////////////主动上报的延时
       	                {
                            state_counter--;

       	                }
                        
	                if((g_sRtcManager.m_gRealTimer[RTC_MINUT] & 3) == 0)  //为了尽可能减少对时时间上的差错，对时时间由5分钟改为4分钟
	                {
	        
	                    g_sRtcManager.m_ucRtcSynFlag = YES;
                            if(phase_check_time>0)                    //////每隔两个小时检测一次相位
       	                        phase_check_time--; 
                            if(phase_check_time==0)  
       	                    {
       	                        phase_check_time=30;
       	                        if(g_gProcCnt[PC_EFS_MODEL]>0)                    ///////只有II型信号源才检测相位
       	  	                    phase_check_flag=0x55;             //////置检测相位标志  
       	                        else if(g_gProcCnt[PC_EFS_MODEL]==0)               ///////只有II型信号源才检测相位
       	  	                    phase_check_flag=0;                //////置检测相位标志  	   	  	       	  
       	                     }	
	                }
                        for(i=0;i<4;i++)                  //////////check the time interval confirm the type of the upload
                        {
                            if(upload_interval[i]>1)
             	            {
             	                upload_interval[i]--;	
                                if((upload_interval[i]%upload_interval_set)==0) 
             	                {
             	                    if(i==0)
             	     	                upload_flag|=ABN_CHECK;     //////保护开关跳开重新传
             	                    else if(i==1)
             	     	                upload_flag|=POWER_OFF;          //////掉电重新传
             	                    else if(i==2)
             	     	                upload_flag|=EIGHT_PULSE;          //////有效8脉冲重新传
             	                    else if(i==3)
             	     	                upload_flag|=PHASE_BREAK;	      //////断线重新传	 
             	                 } 	  
             	             }	
                         }
                        
	    
	             }

     //g_gRmtMeas[RM_UCAP] = g_unAdcData[6];
    if(g_gRmtMeas[RM_UCAP]>2800){g_gRmtInfo[YX_SBP_OFF]=0;SaveLOG(LOG_UCAP, 0);}//5.87
    if(g_gRmtMeas[RM_UCAP]<2600){g_gRmtInfo[YX_SBP_OFF]=1;SaveLOG(LOG_UCAP, 1);}//5.4v	               
            }    
            
                       
          break;

     default:
          break;
    }
  UCA1IE |= (UCRXIE + UCTXIE);              // 使能 USCI_A0 RX 中断  // 张弢测试中断嵌套	 
  UCA2IE |= (UCRXIE + UCTXIE);              // 使能 USCI_A0 RX 中断  // 张弢测试中断嵌套	
//  LED_RUN_OFF;
}

