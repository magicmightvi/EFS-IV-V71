/**********************************************************************************
Copyright (C) 1988-2007, CREAT Company 
模块名         : 保护逻辑模块
文件名         : ProtectLogic.c
相关文件       : DataStruct.h FuncDef.h Constant.h
实现功能       : 保护逻辑判断过程，包括启动元件判断、保护逻辑判断过程等，并根据结果启动相关出口和启动永磁状态分析
作者           :  
版本           : 1.0

--------------------------------------------------------------------------------
备注           : -
--------------------------------------------------------------------------------

修改记录       : 
日  期          版本            修改人          修改内容 
2010/06/01      1.0                          创建

************************************************************************************/

#ifndef _PROTECT_LOGIC

#define _PROTECT_LOGIC
#include "DataStruct.h"
//#include "IvtTable.h"
#endif
//unsigned char g_ucEarthFlg = 0;  //故障相别标志
unsigned char g_ucPhase_Chongfa = 0;  //重发参考相别，与发生故障后首次动作相别一致
static unsigned char  phasea_off= 0;
static unsigned char  phaseb_off= 0;
static unsigned char  phasec_off= 0;
//==============================================================================
//  函数名称   : ProtStart
//  功能描述   : 保护启动元件判断
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ： 
//==============================================================================
void ProtStart1(void)
{
    if(fault_time>0)                           /////////用于故障恢复
    {  
        if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0))  /////////用于故障恢复zero sequence in effective
     	{
     	    if(g_gProcMeas[RM_U0]<g_gProcCntJug1[PC1_LOW_Z])
     	    {
     	 	fault_begin=0;
           	fault_end=0x55;
              g_gRmtInfo[YX_EARTH_FAULT] = 0;
			g_gRmtInfo[YX_AEARTH_FAULT] = 0;g_gRmtInfo[YX_BEARTH_FAULT] = 0;g_gRmtInfo[YX_CEARTH_FAULT] = 0;
			//g_gRmtInfo[YX_EARTH_TDELA]=0;//SaveLOG(LOG_EARTH_TDELAY,0);
     	    }
     	} 
        else if((g_gProcCnt[PC_JAG_Z]==0)&&(g_gProcCnt[PC_JAG_P]==0x55))      //////////////////相故障判据  
     	{		                                   
            if((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UA]<g_gProcCntJug1[PC1_HIGH_P])&&(g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UB]<g_gProcCntJug1[PC1_HIGH_P])&&(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UC]<g_gProcCntJug1[PC1_HIGH_P])) /////////复归   单相接地判据
       	    { 	
                fault_begin=0;
                fault_end=0x55;
                g_gRmtInfo[YX_EARTH_FAULT] = 0;
		  		g_gRmtInfo[YX_AEARTH_FAULT] = 0;g_gRmtInfo[YX_BEARTH_FAULT] = 0;g_gRmtInfo[YX_CEARTH_FAULT] = 0;	
		  		//g_gRmtInfo[YX_EARTH_TDELA]=0;//SaveLOG(LOG_EARTH,0);SaveLOG(LOG_EARTH_TDELAY,0);
                //g_gRmtInfo[0] &= ~(YX_PHASEA_FAULT + YX_PHASEB_FAULT + YX_PHASEC_FAULT);
            }     
       	}    
        else if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0x55))      //////////////////相故障判据  
        {
            if(((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UA]<g_gProcCntJug1[PC1_HIGH_P])&&(g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UB]<g_gProcCntJug1[PC1_HIGH_P])&&(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UC]<g_gProcCntJug1[PC1_HIGH_P]))||(g_gProcMeas[RM_U0]<g_gProcCntJug1[PC1_LOW_Z])) /////////复归   单相接地判据
       	    { 	
                fault_begin=0;
                fault_end=0x55;
                g_gRmtInfo[YX_EARTH_FAULT] = 0;
		 		g_gRmtInfo[YX_AEARTH_FAULT] = 0;g_gRmtInfo[YX_BEARTH_FAULT] = 0;g_gRmtInfo[YX_CEARTH_FAULT] = 0;
		 		//g_gRmtInfo[YX_EARTH_TDELA]=0;//SaveLOG(LOG_EARTH,0);SaveLOG(LOG_EARTH_TDELAY,0);
                //g_gRmtInfo[0] &= ~(YX_PHASEA_FAULT + YX_PHASEB_FAULT + YX_PHASEC_FAULT);
            }     
       	 }  
    }
          
    if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0)) 	       ///////////////零序判据 	  	 
    {
        if((fault_save==0)&&(fault_time<g_gProcCnt[PC_T_DELAY]))
        {	
            if(g_gProcMeas[RM_U0]>g_gProcCntJug1[PC1_HIGH_Z])      //////检测到零序高电压，报故障
            {
           	 fault_begin=0x55;
           	 fault_end=0;	
			 //SaveLOG(LOG_EARTH,1);
			 if((g_gProcMeas[RM_UA]<g_gProcMeas[RM_UB])&&(g_gProcMeas[RM_UA]<g_gProcMeas[RM_UC]))
			 	{
			 	fault_pluse =RM_UA;
			 	}
			 else if((g_gProcMeas[RM_UB]<g_gProcMeas[RM_UA])&&(g_gProcMeas[RM_UB]<g_gProcMeas[RM_UC]))
			 	{
			 	fault_pluse =RM_UB;
			 	}
			 else if((g_gProcMeas[RM_UC]<g_gProcMeas[RM_UA])&&(g_gProcMeas[RM_UC]<g_gProcMeas[RM_UB]))
			 	{
			 	fault_pluse =RM_UC;
			 	}
            }              	 		
        }	
    }
    else if((g_gProcCnt[PC_JAG_Z]==0)&&(g_gProcCnt[PC_JAG_P]==0x55))      //////////////////相故障判据  
    {
        if((fault_save==0)&&(fault_time<g_gProcCnt[PC_T_DELAY]))
        {	
            if(((g_gProcMeas[RM_UA]<g_gProcCntJug1[PC1_LOW_P])&&((g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_HIGH_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_HIGH_P]))) /////////A相电压   单相接地判据
				||((g_gProcMeas[RM_UA]<g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UA]<g_gProcMeas[RM_UB])&&(g_gProcMeas[RM_UA]<g_gProcMeas[RM_UC])
					&&((g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_LOW_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)))
			{ 	
                fault_begin=0x55;
		  		fault_pluse =RM_UA;
           		fault_end=0;
				//SaveLOG(LOG_EARTH,1);
            }     
            else if(((g_gProcMeas[RM_UB]<g_gProcCntJug1[PC1_LOW_P])&&((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_HIGH_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_HIGH_P]))) ////////B相电压   单相接地判据
				||((g_gProcMeas[RM_UB]<g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UB]<g_gProcMeas[RM_UA])&&(g_gProcMeas[RM_UA]<g_gProcMeas[RM_UC])
					&&((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_LOW_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)))
			{ 	
    	 		fault_begin=0x55;
				fault_pluse =RM_UB;	
    	 		fault_end=0;
				//SaveLOG(LOG_EARTH,1);
    	    }    
	    	else if(((g_gProcMeas[RM_UC]<g_gProcCntJug1[PC1_LOW_P])&&((g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_HIGH_P])||(g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_HIGH_P]))) /////////C相电压   单相接地判据
					||((g_gProcMeas[RM_UC]<g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UC]<g_gProcMeas[RM_UB])&&(g_gProcMeas[RM_UC]<g_gProcMeas[RM_UA])
						&&((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_LOW_P])||(g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_LOW_P]))
						&&(g_gDebugP[Debug_SRECJU2]==0x55)))
				{ 	
    	 		fault_begin=0x55;
				fault_pluse =RM_UC;	
    	 		fault_end=0;
				//SaveLOG(LOG_EARTH,1);
    	  		}     
        }         
    }       	
    else if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0x55))     /////////相和零序同时做判据	 
    { 
        if((fault_save==0)&&(fault_time<g_gProcCnt[PC_T_DELAY]))
        {	         	 	
            if(((g_gProcMeas[RM_UA]<g_gProcCntJug1[PC1_LOW_P])&&((g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_HIGH_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_HIGH_P]))&&(g_gProcMeas[RM_U0]>g_gProcCntJug1[PC1_HIGH_Z])) /////////A相电压   单相接地判据
            	||((g_gProcMeas[RM_UA]<g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UA]<g_gProcMeas[RM_UB])&&(g_gProcMeas[RM_UA]<g_gProcMeas[RM_UC])
					&&((g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_LOW_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)&&(g_gProcMeas[RM_U0]>g_gProcCntJug1[PC1_HIGH_Z])))
       	    	{ 	
           	 	fault_begin=0x55;
			 	//SaveLOG(LOG_EARTH,1);
				fault_pluse =RM_UA;	 
           	 	fault_end=0;
            	}       
            else if(((g_gProcMeas[RM_UB]<g_gProcCntJug1[PC1_LOW_P])&&((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_HIGH_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_HIGH_P]))&&(g_gProcMeas[RM_U0]>g_gProcCntJug1[PC1_HIGH_Z])) /////////A相电压   单相接地判据
				||((g_gProcMeas[RM_UB]<g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UB]<g_gProcMeas[RM_UA])&&(g_gProcMeas[RM_UA]<g_gProcMeas[RM_UC])
					&&((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_LOW_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)&&(g_gProcMeas[RM_U0]>g_gProcCntJug1[PC1_HIGH_Z])))
       	    	{ 	
           	 	fault_begin=0x55;
				fault_pluse =RM_UB;	 
           	 	fault_end=0;
				//SaveLOG(LOG_EARTH,1);
            	}      
            else if(((g_gProcMeas[RM_UC]<g_gProcCntJug1[PC1_LOW_P])&&((g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_HIGH_P])||(g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_HIGH_P]))&&(g_gProcMeas[RM_U0]>g_gProcCntJug1[PC1_HIGH_Z])) /////////A相电压   单相接地判据
					||((g_gProcMeas[RM_UC]<g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UC]<g_gProcMeas[RM_UB])&&(g_gProcMeas[RM_UC]<g_gProcMeas[RM_UA])
						&&((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_LOW_P])||(g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_LOW_P]))
						&&(g_gDebugP[Debug_SRECJU2]==0x55)&&(g_gProcMeas[RM_U0]>g_gProcCntJug1[PC1_HIGH_Z])))
       	    	{ 	
           	 	fault_begin=0x55;
				fault_pluse =RM_UC;	 
           	 	fault_end=0;
				//SaveLOG(LOG_EARTH,1);
            	}          	     	     
       	 }     
      } 
     if((g_sRecData.m_ucActRecStart == CLOSE)&&(g_sRecData.m_ucRecSavingFlag == OFF)
	 	&&(g_sRecData.m_ucFaultRecStart ==OFF)&&(fault_begin==0x55)
	 	&&(fault_lubo==0)) //////故障已经开始
  		{
  		fault_lubo=0x55;
		g_sRecData.m_ucFaultRecStart = ON;//启动故障录波
		g_sRecData.m_LuboType = LuboType_EARTH;			
      	}
     /*if((g_sRecData.m_ucActRecStart == CLOSE)&&(g_sRecData.m_ucRecSavingFlag == OFF)
	 	&&(g_sRecData.m_ucFaultRecStart ==OFF)&&(fault_end==0x55)
	 	&&(fault_lubo==0x55)) //////故障已经恢复
  		{
  		fault_lubo=0;
		g_sRecData.m_ucFaultRecStart = ON;//启动故障恢复录波
		g_sRecData.m_LuboType = SREC_EARTH_RST;			
      	} */   
}
void ProtStart(void)
{/*
    unsigned int m;

    int tempi,tempi2;
    unsigned int tAcTail,tAcTail2,tAcTailN;
    int *pAcData;
*/
   // unsigned long ulProcCntJdg[8];  //临时保存部分保护定值
    //tAcTail = g_sSampleData.m_unAcDataTail; //最新的位置
    //tAcTail2 = (g_sSampleData.m_unAcDataTail - 8) & 0x1F;   //半个周期前的位置
    //tAcTailN = (g_sSampleData.m_unAcDataTail - 16) & 0x1F;  //一个周期前的位置
  //  for(i = 0; i < 3; i++)  //A相 B相 C相
  /*  if(g_gProcCnt[PC_OVERLOAD_CNT] != 0)   //过流保护投入
    {
        if(g_gProcMeas[PM_I0] >= g_gProcCntJug_I0[0])  //电流过载
        {
            if(g_gOverLoadTimeNum < g_gProcCntJug_I0[2])
                g_gOverLoadTimeNum++;
            if(g_gOverLoadTimeNum >= g_gProcCntJug_I0[2])
                g_gOverLoadFlg = ON;
        }
        else if(g_gProcMeas[PM_I0] < g_gProcCntJug_I0[1])  //降到回差之后
        {
            if(g_gOverLoadTimeNum > 0)
                g_gOverLoadTimeNum --;
            //g_gOverLoadFlg = 0;
        }
    }
    else
    {
         g_gOverLoadTimeNum = 0;
         g_gOverLoadFlg = 0; 
    }*/
  /*  {

        pAcData = g_sSampleData.m_gAcAdcData[4];
        tempi = pAcData[tAcTail] + pAcData[tAcTail2];   //Ik+I(k-2/n)
        tempi2 = pAcData[tAcTailN] + pAcData[tAcTail2]; //I(k-2/n) + I(k-n)
        if(tempi < 0)
        {
            tempi = -tempi;
        }
        
        if(tempi2 < 0)
        {
            tempi2  = -tempi2;
        }
        if((eight_pulse_flag>0)&&(pulse_flag!=0x55)&&(flash_write!=0x55))  //8脉冲程序已经启动，未发现8脉冲电流值，没有进行存储
        {
        if((long)(tempi - tempi2) >= (long)g_gProcCnt[PC_I0_START])    //0   
        {
            pulse_flag++;//SetRIStatus(TMP_TQD_A + i,1);//置突变量启动标志，根据标志进行突变量启动计时，如果在10ms内，没有过流事件发生，则清0，否则计时增加突变量启动时间
            if(pulse_flag==3 )   //&& (eight_pulse_flag%2)==0                   ///////连续检测到3次置故障标志，确定有8脉冲发出
            {   
   	       // if(flash_write==0)                  //////表示检测到了电流值，但还没有到向FLASH中写入的时候。
   		  //  flash_write=0x33;
   		    	     	
         	pulse_flag=0x55;
       
         	wave_counter=0;
         	if(pulse_counter>=7)
         	    pulse_counter=7;  
                if(pulse_counter==0)
                {   	                 
                    g_sSoeData.m_gRecBuff[0]=g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;      //年
      	            g_sSoeData.m_gRecBuff[1]=g_sRtcManager.m_gRealTimer[RTC_MONTH];      //月
      	            g_sSoeData.m_gRecBuff[2]=g_sRtcManager.m_gRealTimer[RTC_DATE];      //日
      	            g_sSoeData.m_gRecBuff[3]=g_sRtcManager.m_gRealTimer[RTC_HOUR];      //时
      	            g_sSoeData.m_gRecBuff[4]=g_sRtcManager.m_gRealTimer[RTC_MINUT];      //分
      	            g_sSoeData.m_gRecBuff[5]=g_sRtcManager.m_gRealTimer[RTC_SEC];      //秒
      	            g_sSoeData.m_gRecBuff[6]=g_sRtcManager.m_gRealTimer[RTC_MICROSEC] / 10;      //10毫秒 
      	            g_sSoeData.m_gRecBuff[7]=g_sRtcManager.m_gRealTimer[RTC_MICROSEC] % 10; 
      		}
      	        else
      		{
      		    m=pulse_counter*29+4;         	
      	            g_sSoeData.m_gRecBuff[m++]=g_sRtcManager.m_gRealTimer[RTC_MINUT];      //分
      	            g_sSoeData.m_gRecBuff[m++]=g_sRtcManager.m_gRealTimer[RTC_SEC];      //秒
      	            g_sSoeData.m_gRecBuff[m++]=g_sRtcManager.m_gRealTimer[RTC_MICROSEC] / 10;      //10毫秒 
      	            g_sSoeData.m_gRecBuff[m++]=g_sRtcManager.m_gRealTimer[RTC_MICROSEC] % 10;
      		}	        	      	
            }	
        }
        else
            pulse_flag = 0;
      }     
        
    }*/
   /* if((P5IN&BIT5)==0x00)    //检测到信号源掉电
    {
        g_gYXConformTime[0]++;
        if(g_gYXConformTime[0] >= DIN_CON_TIME)
            g_ucPowerOffFlg = 0x55;
            
    }
    else
    {
        if(g_gYXConformTime[0] > 0)
            g_gYXConformTime[0]--;
        if(g_gYXConformTime[0] == 0)
            g_ucPowerOffFlg = 0;
            
    }*/
    if(fault_time>0)                           /////////用于故障恢复
    {  
        if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0))  /////////用于故障恢复zero sequence in effective
     	{
     	    if(g_gRmtMeas[RM_U0]<g_gProcCntJug[PC_LOW_Z])
     	    {
     	 	fault_begin=0;
           	fault_end=0x55;
              g_gRmtInfo[YX_EARTH_FAULT] = 0;
			g_gRmtInfo[YX_AEARTH_FAULT] = 0;g_gRmtInfo[YX_BEARTH_FAULT] = 0;g_gRmtInfo[YX_CEARTH_FAULT] = 0;
			//g_gRmtInfo[YX_EARTH_TDELA]=0;//SaveLOG(LOG_EARTH_TDELAY,0);
     	    }
     	} 
        else if((g_gProcCnt[PC_JAG_Z]==0)&&(g_gProcCnt[PC_JAG_P]==0x55))      //////////////////相故障判据  
     	{		                                   
            if((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_HIGH_P])&&(g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_HIGH_P])&&(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_HIGH_P])) /////////复归   单相接地判据
       	    { 	
                fault_begin=0;
                fault_end=0x55;
                g_gRmtInfo[YX_EARTH_FAULT] = 0;
		  		g_gRmtInfo[YX_AEARTH_FAULT] = 0;g_gRmtInfo[YX_BEARTH_FAULT] = 0;g_gRmtInfo[YX_CEARTH_FAULT] = 0;	
		  		//g_gRmtInfo[YX_EARTH_TDELA]=0;//SaveLOG(LOG_EARTH,0);SaveLOG(LOG_EARTH_TDELAY,0);
                //g_gRmtInfo[0] &= ~(YX_PHASEA_FAULT + YX_PHASEB_FAULT + YX_PHASEC_FAULT);
            }     
       	}    
        else if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0x55))      //////////////////相故障判据  
        {
            if(((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_HIGH_P])&&(g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_HIGH_P])&&(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_U0]<g_gProcCntJug[PC_LOW_Z])) /////////复归   单相接地判据
       	    { 	
                fault_begin=0;
                fault_end=0x55;
                g_gRmtInfo[YX_EARTH_FAULT] = 0;
		 		g_gRmtInfo[YX_AEARTH_FAULT] = 0;g_gRmtInfo[YX_BEARTH_FAULT] = 0;g_gRmtInfo[YX_CEARTH_FAULT] = 0;
		 		//g_gRmtInfo[YX_EARTH_TDELA]=0;//SaveLOG(LOG_EARTH,0);SaveLOG(LOG_EARTH_TDELAY,0);
                //g_gRmtInfo[0] &= ~(YX_PHASEA_FAULT + YX_PHASEB_FAULT + YX_PHASEC_FAULT);
            }     
       	 }  
    }
          
    if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0)) 	       ///////////////零序判据 	  	 
    {
        if((fault_save==0)&&(fault_time<g_gProcCntJug[PC_T_DELAY]))
        {	
            if(g_gRmtMeas[RM_U0]>g_gProcCntJug[PC_HIGH_Z])      //////检测到零序高电压，报故障
            {
           	 fault_begin=0x55;
           	 fault_end=0;	
			 //SaveLOG(LOG_EARTH,1);
			 if((g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UC]))
			 	{
			 	fault_pluse =RM_UA;
			 	}
			 else if((g_gRmtMeas[RM_UB]<g_gRmtMeas[RM_UA])&&(g_gRmtMeas[RM_UB]<g_gRmtMeas[RM_UC]))
			 	{
			 	fault_pluse =RM_UB;
			 	}
			 else if((g_gRmtMeas[RM_UC]<g_gRmtMeas[RM_UA])&&(g_gRmtMeas[RM_UC]<g_gRmtMeas[RM_UB]))
			 	{
			 	fault_pluse =RM_UC;
			 	}
            }              	 		
        }	
    }
    else if((g_gProcCnt[PC_JAG_Z]==0)&&(g_gProcCnt[PC_JAG_P]==0x55))      //////////////////相故障判据  
    {
        if((fault_save==0)&&(fault_time<g_gProcCntJug[PC_T_DELAY]))
        {	
            if(((g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_LOW_P])&&((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_HIGH_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_HIGH_P]))) /////////A相电压   单相接地判据
				||((g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UC])
					&&((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)))
			{ 	
                fault_begin=0x55;
		  		fault_pluse =RM_UA;
           		fault_end=0;
				//SaveLOG(LOG_EARTH,1);
            }     
            else if(((g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_LOW_P])&&((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_HIGH_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_HIGH_P]))) ////////B相电压   单相接地判据
				||((g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UB]<g_gRmtMeas[RM_UA])&&(g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UC])
					&&((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)))
			{ 	
    	 		fault_begin=0x55;
				fault_pluse =RM_UB;	
    	 		fault_end=0;
				//SaveLOG(LOG_EARTH,1);
    	    }    
	    	else if(((g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_LOW_P])&&((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_HIGH_P])||(g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_HIGH_P]))) /////////C相电压   单相接地判据
					||((g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UC]<g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UC]<g_gRmtMeas[RM_UA])
						&&((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])||(g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P]))
						&&(g_gDebugP[Debug_SRECJU2]==0x55)))
				{ 	
    	 		fault_begin=0x55;
				fault_pluse =RM_UC;	
    	 		fault_end=0;
				//SaveLOG(LOG_EARTH,1);
    	  		}     
        }         
    }       	
    else if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0x55))     /////////相和零序同时做判据	 
    { 
        if((fault_save==0)&&(fault_time<g_gProcCntJug[PC_T_DELAY]))
        {	         	 	
            if(((g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_LOW_P])&&((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_HIGH_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_HIGH_P]))&&(g_gRmtMeas[RM_U0]>g_gProcCntJug[PC_HIGH_Z])) /////////A相电压   单相接地判据
            	||((g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UC])
					&&((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)&&(g_gRmtMeas[RM_U0]>g_gProcCntJug[PC_HIGH_Z])))
       	    	{ 	
           	 	fault_begin=0x55;
			 	//SaveLOG(LOG_EARTH,1);
				fault_pluse =RM_UA;	 
           	 	fault_end=0;
            	}       
            else if(((g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_LOW_P])&&((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_HIGH_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_HIGH_P]))&&(g_gRmtMeas[RM_U0]>g_gProcCntJug[PC_HIGH_Z])) /////////A相电压   单相接地判据
				||((g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UB]<g_gRmtMeas[RM_UA])&&(g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UC])
					&&((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)&&(g_gRmtMeas[RM_U0]>g_gProcCntJug[PC_HIGH_Z])))
       	    	{ 	
           	 	fault_begin=0x55;
				fault_pluse =RM_UB;	 
           	 	fault_end=0;
				//SaveLOG(LOG_EARTH,1);
            	}      
            else if(((g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_LOW_P])&&((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_HIGH_P])||(g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_HIGH_P]))&&(g_gRmtMeas[RM_U0]>g_gProcCntJug[PC_HIGH_Z])) /////////A相电压   单相接地判据
					||((g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UC]<g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UC]<g_gRmtMeas[RM_UA])
						&&((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])||(g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P]))
						&&(g_gDebugP[Debug_SRECJU2]==0x55)&&(g_gRmtMeas[RM_U0]>g_gProcCntJug[PC_HIGH_Z])))
       	    	{ 	
           	 	fault_begin=0x55;
				fault_pluse =RM_UC;	 
           	 	fault_end=0;
				//SaveLOG(LOG_EARTH,1);
            	}          	     	     
       	 }     
      } 
     if((g_sRecData.m_ucActRecStart == CLOSE)&&(g_sRecData.m_ucRecSavingFlag == OFF)
	 	&&(g_sRecData.m_ucFaultRecStart ==OFF)&&(fault_begin==0x55)
	 	&&(fault_lubo==0)) //////故障已经开始
  		{
  		fault_lubo=0x55;
		g_sRecData.m_ucFaultRecStart = ON;//启动故障录波
		g_sRecData.m_LuboType = LuboType_EARTH;			
      	}
     /*if((g_sRecData.m_ucActRecStart == CLOSE)&&(g_sRecData.m_ucRecSavingFlag == OFF)
	 	&&(g_sRecData.m_ucFaultRecStart ==OFF)&&(fault_end==0x55)
	 	&&(fault_lubo==0x55)) //////故障已经恢复
  		{
  		fault_lubo=0;
		g_sRecData.m_ucFaultRecStart = ON;//启动故障恢复录波
		g_sRecData.m_LuboType = SREC_EARTH_RST;			
      	} */   
}
//==============================================================================
//  函数名称   : PulseReady
//  功能描述   : 8脉冲发出前的准备工作
//               
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void PulseReady(void)
{
    if(efslatch_flag==0)       /////////判断闭锁已经结束
    {
    //SaveLOG(LOG_8FULS_STA,1);
    	if(chongfa==0)  ///////自己发生的八脉冲
	{ 
	    eight_select=0x8f;                    ////////////置首次发生8脉冲的标志	
            g_ucPhase_Chongfa = 0;   //首次发送8脉冲，参考标志清0
	    if(g_gProcCnt[PC_EFS_MODEL]==0)    //////I型信号源
            {
                pulse_phase_flag=1;
             	repeat_flag1=4;       ////////A相发生故障 
                g_ucEarthFlg = 4;
                g_gRmtInfo[YX_EARTH_FAULT] = 1;	
            }  
            else                     ///////II型信号源
            { 
	        if((g_gRmtMeas[RM_UC]<=g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UC]<=g_gRmtMeas[RM_UA]))		
	        {	
		    if(g_gRmtMeas[RM_UC] <= 2000 || ((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P]) && (g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P])))  //C相为接地相
		        g_ucEarthFlg = 3;////////C相发生故障	
                    else if(g_gRmtMeas[RM_UC] <= g_gProcCntJug[PC_LOW_P] &&(g_gRmtMeas[RM_UB] >= g_gRmtMeas[RM_UA] ) )
                        g_ucEarthFlg = 3;////////C相发生故障
                        else if(g_gRmtMeas[RM_UC] <= g_gProcCntJug[PC_LOW_P] &&(g_gRmtMeas[RM_UA] > g_gRmtMeas[RM_UB] ) )
					g_ucEarthFlg = 2;////////B相发生故障		
             	}		         	 
	        else if((g_gRmtMeas[RM_UB]<=g_gRmtMeas[RM_UA])&&(g_gRmtMeas[RM_UB]<=g_gRmtMeas[RM_UC]))	
	        {
		    if(g_gRmtMeas[RM_UB] <= 2000 || ((g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P]) && (g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P])))
                        g_ucEarthFlg = 2;////////B相发生故障  
                    else if(g_gRmtMeas[RM_UB] <= g_gProcCntJug[PC_LOW_P] &&(g_gRmtMeas[RM_UA] >= g_gRmtMeas[RM_UC] ) )
                        g_ucEarthFlg = 2;////////B相发生故障 	
                        else if(g_gRmtMeas[RM_UB] <= g_gProcCntJug[PC_LOW_P] &&(g_gRmtMeas[RM_UA] < g_gRmtMeas[RM_UC] ) )
					g_ucEarthFlg = 1;////////A相发生故障 	
             	}		
	        else if((g_gRmtMeas[RM_UA]<=g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UA]<=g_gRmtMeas[RM_UC]))	
	        {
	            if(g_gRmtMeas[RM_UA] <= 2000 || ((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P]) && (g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P])))
			g_ucEarthFlg = 1;////////a相发生故障  
		    else if(g_gRmtMeas[RM_UA] <= g_gProcCntJug[PC_LOW_P] &&(g_gRmtMeas[RM_UC] >= g_gRmtMeas[RM_UB] ))
                    	g_ucEarthFlg = 1;////////a相发生故障 
                    	else if(g_gRmtMeas[RM_UA] <= g_gProcCntJug[PC_LOW_P] &&(g_gRmtMeas[RM_UC] < g_gRmtMeas[RM_UB] ))
				g_ucEarthFlg = 3;////////C相发生故障
                }
			pulse_phase_flag = 3;
                switch(g_ucEarthFlg)
                {
                    case 1://a相发生故障
                      pulse_phase_flag = 3;
			 g_gRmtInfo[YX_AEARTH_FAULT] = 1;		  
                      break;
                    case 2://B相发生故障
                      pulse_phase_flag = 1;
			 g_gRmtInfo[YX_BEARTH_FAULT] = 1;		  
                      break;
                    case 3://C相发生故障
                      g_gRmtInfo[YX_CEARTH_FAULT] = 1;
                      if((g_gProcCnt[PC_JAG_ACT] == 3) ||(g_gProcCnt[PC_JAG_ACT] == 4) )   //动作相序=3或4，没有B相，A相动作
                          pulse_phase_flag=1;
                      else
                          pulse_phase_flag = 2;
                      break;  
                }
                repeat_flag1=pulse_phase_flag;
		g_ucPhase_Chongfa = pulse_phase_flag;
		g_gRmtInfo[YX_EARTH_FAULT] = 1;	
	    }
	 eight_pulse_flag=1; 	
	 g_TQBSCounter = 2;//投切、闭锁指示灯计数器  =0 是灭 =0x55 闭锁常亮  >=1投切闪烁
	 g_gRmtInfo[YX_EFS_ACT] = 1;   //投切状态 遥信置1 
	 g_I0RmtZeroNum = 0;
        }   
	if(moniguzhang==1)  
		{
		eight_pulse_flag=1; 	//=1为模拟故障 
		g_TQBSCounter = 2;//投切、闭锁指示灯计数器  =0 是灭 =0x55 闭锁常亮  >=1投切闪烁
		g_gRmtInfo[YX_EFS_ACT] = 1;   //投切状态 遥信置1 
		g_I0RmtZeroNum = 0;
		}
	//g_gModfk[0]=0;g_gModfk[1]=0;g_gModfk[2]=0;
	kat=0;kbt=0;kct=0;Numk=0;
	unsigned char i;
	for(i = 0;i <24;i++)
      		{ g_gModfk[i]=0;}
    }// end  efslatch_flag==0  		
}
/*
void PulseReady(void)
{
    if(efslatch_flag==0)       /////////判断闭锁已经结束
    {
        if(chongfa==0)  ///////自己发生的八脉冲
		{ 
			eight_select=0x8f;                    ////////////置首次发生8脉冲的标志	
            g_ucPhase_Chongfa = 0;   //首次发送8脉冲，参考标志清0
			if(g_gProcCnt[PC_EFS_MODEL]==0)    //////I型信号源
            {
                pulse_phase_flag=1;
             	repeat_flag1=4;       ////////A相发生故障 
                g_ucEarthFlg = 4;
                g_gRmtInfo[0] |= YX_EARTH_FAULT; 		
            }  
            else                     ///////II型信号源
            { 
	            if((g_gRmtMeas[RM_UC]<=g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UC]<=g_gRmtMeas[RM_UA]))		
	            {	
                        if(g_gRmtMeas[RM_UC] <= 2000 || ((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P]) && (g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P])))  //C相为接地相
                           {

          	  	         pulse_phase_flag=1;   //固定动作A     
                                 g_ucEarthFlg = 3;////////C相发生故障
                                 if(g_gProcCnt[PC_JAG_ACT] == 3)
                                    repeat_flag1=3;
                                 
                           }
                           else if(g_gRmtMeas[RM_UC] <= g_gProcCntJug[PC_LOW_P] &&((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P]) || (g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P])))
                           {

                               pulse_phase_flag=1;   //固定动作A     
                                 g_ucEarthFlg = 3;////////C相发生故障
                                 if(g_gProcCnt[PC_JAG_ACT] == 3)
                                    repeat_flag1=3;
                           }  

             	    }		         	 
	            else if((g_gRmtMeas[RM_UB]<=g_gRmtMeas[RM_UA])&&(g_gRmtMeas[RM_UB]<=g_gRmtMeas[RM_UC]))	
	            {
	           	if(g_gRmtMeas[RM_UB] <= 2000 || ((g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P]) && (g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P])))
                           {
                               if(phase_seq_flag==0)             /////////正常相序
          	 	       {
          	 	           pulse_phase_flag=1;
                                   if(g_gProcCnt[PC_JAG_ACT] == 2)    //动作滞后相
                                         pulse_phase_flag=3;
              	 	       }  
          	               else                              /////////错误相序，动作的相正好相反
          	  	       {
                                   pulse_phase_flag=3;
                                   if(g_gProcCnt[PC_JAG_ACT] == 2)    //动作滞后相
                                         pulse_phase_flag=1;
          	  	       }
                               g_ucEarthFlg = 2;////////B相发生故障  
                               if(g_gProcCnt[PC_JAG_ACT] == 3)
                                   repeat_flag1=2;
                           }
                           else if(g_gRmtMeas[RM_UB] <= g_gProcCntJug[PC_LOW_P] &&((g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P]) || (g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P])))
                           {

                                 pulse_phase_flag=1;
                                 g_ucEarthFlg = 2;////////B相发生故障  
                               if(g_gProcCnt[PC_JAG_ACT] == 3)
                                   repeat_flag1=2;
             	            }
             	    }		
	            else if((g_gRmtMeas[RM_UA]<=g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UA]<=g_gRmtMeas[RM_UC]))	
	            {
	                if(g_gRmtMeas[RM_UA] <= 2000 || ((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P]) && (g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P])))
                    {

						pulse_phase_flag=3;
						g_ucEarthFlg = 1;////////a相发生故障  
						if(g_gProcCnt[PC_JAG_ACT] == 3)
							repeat_flag1=1;                          
					}
					else if(g_gRmtMeas[RM_UA] <= g_gProcCntJug[PC_LOW_P] &&((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P]) || (g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P])))
					{
						pulse_phase_flag=3;
						g_ucEarthFlg = 1;////////a相发生故障  
						if(g_gProcCnt[PC_JAG_ACT] == 3)
							repeat_flag1=1;
					}
				}
				g_ucPhase_Chongfa = pulse_phase_flag;
				g_gRmtInfo[0] |= YX_EARTH_FAULT;

			}	
		}   
		else       //////下发的命令 chongfa!=0;
			chongfa=0; 
		eight_pulse_flag=1; 
       // pulse_time_flag=11;                 	   ///////////设置8脉冲延时保护       
    }// end  efslatch_flag==0  
    
}
*/
//==============================================================================
//  函数名称   : ProtLogic
//  功能描述   : 保护逻辑判断过程和计时过程
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ： 
//==============================================================================
void ProtLogic(void)
{
    unsigned char i;
  //  if(g_ucDuanxianFlg >= 50)
    {
    if((phase_off&BIT0)==BIT0)      /////////A相存在断线 ,判断是否复归
    {
	if((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_HIGH_P]))
	{
	    if(((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_NO_V]))
	    {
	  	if(((g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_NO_V]))
	  	{ 
                    phase_off&=~BIT0;
		      g_gRmtInfo[YX_PHASEA_OFF] = 0;
                }
	    }
	}
    }
    if((phase_off&BIT1)==BIT1)      /////////B相存在断线 ,判断是否复归
    {
	if((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_HIGH_P]))
	{
	    if(((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_NO_V]))
	    {
	  	if(((g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_NO_V]))
                {
                    phase_off&=~BIT1;
		      g_gRmtInfo[YX_PHASEB_OFF] = 0;
                }
                
	    }
	 }
    }
    if((phase_off&BIT2)==BIT2)      /////////C相存在断线 ,判断是否复归
    {
	if((g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_HIGH_P]))
	{
	    if(((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_NO_V]))
	  	{
	  	    if(((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_NO_V]))
                    {
                        phase_off&=~BIT2;
		      g_gRmtInfo[YX_PHASEC_OFF] = 0;
                    }
	  	}
	 }
    }	
    if((phase_off & 0x07) == 0)
        g_gRmtInfo[YX_PHASE_OFF] = 0;
     ///////在这里做复归判断的存储，防止三个复归同时出现还是存储三次
    if(phase_copy!=phase_off)       /////////说明发生复归
    {
	phase_copy=phase_off;
        for(i=0;i<6;i++)
           g_sSoeData.m_gBreakRSTBuff[i] = g_sRtcManager.m_gRealTimer[i];
        g_sSoeData.m_gBreakRSTBuff[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
	g_sSoeData.m_gBreakRSTBuff[6]=(unsigned char)(g_gRmtMeas[RM_UA]/100);
	g_sSoeData.m_gBreakRSTBuff[7]=(unsigned char)(g_gRmtMeas[RM_UB]/100);
	g_sSoeData.m_gBreakRSTBuff[8]=(unsigned char)(g_gRmtMeas[RM_UC]/100);
	save_phase_off=0x66;
       
    }
  /*  if((phase_off&BIT0)==BIT0)
        g_gRmtInfo[0] |= YX_PHASEA_OFF;
    if((phase_off&BIT1)==BIT1)
        g_gRmtInfo[0] |= YX_PHASEB_OFF;
    if((phase_off&BIT1)==BIT1)
        g_gRmtInfo[0] |= YX_PHASEC_OFF;*/
    
    if((phase_off&0x01)==0x00)       ////////这是判断A相是否有断线
    {
	if(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_NO_V])
	{
	    if(((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_NO_V]))
	    {
	  	if(((g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_NO_V]))
                {
                	phasea_off++;
			if(phasea_off>200)
				{
				phasea_off=200;
                    phase_off|=BIT0;
                    g_gRmtInfo[YX_PHASE_OFF] = 1;
		      g_gRmtInfo[YX_PHASEA_OFF] = 1;	
				}
                }
	    }
	}
	else
	{
	phasea_off = 0;
	}
    }
    if((phase_off&0x02)==0x00)       ////////这是判断B相是否有断线
    {
	if(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_NO_V])
	{
	    if(((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_NO_V]))
	    {
	  	if(((g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_NO_V]))
                {
			phaseb_off++;
			if(phaseb_off>200)
				{
				phaseb_off = 200;
			phase_off|=BIT1;
                    g_gRmtInfo[YX_PHASE_OFF] = 1;
		      g_gRmtInfo[YX_PHASEB_OFF] = 1;
				}
                }
	    }
	}
	else
	{
	phaseb_off = 0;
	}	
    }
    if((phase_off&0x04)==0x00)       ////////这是判断C相是否有断线
    {
	if(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_NO_V])
	{
	    if(((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_NO_V]))
	    {
	  	if(((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_NO_V]))
                {
                	phasec_off++;
			if(phasec_off>200)
				{
				phasec_off=200;
                   		 phase_off|=BIT2;
                    		g_gRmtInfo[YX_PHASE_OFF] = 1;
		      		g_gRmtInfo[YX_PHASEC_OFF] = 1;	
				}
                }
	    }
	}
	else
	{
	phasec_off = 0;
	}	
    }	
    //增加3相电压异常遥信位
  /*  if((g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_NO_V] && g_gRmtMeas[RM_UA] <= g_gProcCntJug[PC_LOW_P])||g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P])  //A相电压异常
        g_gRmtInfo[0] |= YX_PHASEA_ABNORMAL;
    else if(g_gRmtMeas[RM_UA] > g_gProcCntJug[PC_LOW_P] && g_gRmtMeas[RM_UA] < g_gProcCntJug[PC_HIGH_P])
        g_gRmtInfo[0] &= ~YX_PHASEA_ABNORMAL;
    if((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_NO_V] && g_gRmtMeas[RM_UB] <= g_gProcCntJug[PC_LOW_P])||g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P])  //B相电压异常
        g_gRmtInfo[0] |= YX_PHASEB_ABNORMAL;
    else if(g_gRmtMeas[RM_UB] > g_gProcCntJug[PC_LOW_P] && g_gRmtMeas[RM_UB] < g_gProcCntJug[PC_HIGH_P])
        g_gRmtInfo[0] &= ~YX_PHASEB_ABNORMAL;
    if((g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_NO_V] && g_gRmtMeas[RM_UC] <= g_gProcCntJug[PC_LOW_P])||g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P])  //C相电压异常
        g_gRmtInfo[0] |= YX_PHASEC_ABNORMAL;
    else if(g_gRmtMeas[RM_UC] > g_gProcCntJug[PC_LOW_P] && g_gRmtMeas[RM_UC] < g_gProcCntJug[PC_HIGH_P])
        g_gRmtInfo[0] &= ~YX_PHASEC_ABNORMAL;
     */ 
	  ///////在这里做断线判断的存储，防止三个断线同时出现还是存储三次
    if(phase_copy!=phase_off)       /////////说明发生断线
    {
	phase_copy=phase_off;
	for(i=0;i<6;i++)
           g_sSoeData.m_gBreakBuff[i] = g_sRtcManager.m_gRealTimer[i];
        g_sSoeData.m_gBreakBuff[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
	g_sSoeData.m_gBreakBuff[6]=(unsigned char)(g_gRmtMeas[RM_UA]/100);
	g_sSoeData.m_gBreakBuff[7]=(unsigned char)(g_gRmtMeas[RM_UB]/100);
	g_sSoeData.m_gBreakBuff[8]=(unsigned char)(g_gRmtMeas[RM_UC]/100);
	save_phase_off=0x55;
	CreatNewSMS(PHASE_BREAK);                      //产生断线短信  	   	 
	  	 
 	if(g_gProcCnt[PC_GPRS_MODEL]==0)                                       //////////////肯定会发短信息，但是会不会发送GPRS数据还需要
   	{
   	    upload_interval[3]=(3*upload_interval_set)-1;      //////////////断线 4 minutes
            upload_flag|=PHASE_BREAK;	
   	}
        else
   	{
   	    upload_interval[3]=0;      //////////////eight pulse set 4 minutes	
   	    upload_flag&=~PHASE_BREAK;	
   	}
    }
    }
    if(fault_recovery_flag==0x55)           ////////// 检测到故障复归
    {
    	fault_recovery_flag=0;	    	 	 
    	for(i=0;i<6;i++)
           g_sSoeData.m_gSoeRSTBuff[i] = g_sRtcManager.m_gRealTimer[i];
        g_sSoeData.m_gSoeRSTBuff[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
    	g_sSoeData.m_gSoeRSTBuff[6]=(unsigned char)(g_gRmtMeas[RM_UA]/100);
        g_sSoeData.m_gSoeRSTBuff[7]=(unsigned char)(g_gRmtMeas[RM_UB]/100);
        g_sSoeData.m_gSoeRSTBuff[8]=(unsigned char)(g_gRmtMeas[RM_UC]/100);
        g_sSoeData.m_gSoeRSTBuff[9]=(unsigned char)(g_gRmtMeas[RM_U0]/100);
        CreatNewSMS(FAULT_RST);                      //产生故障复归短信
        g_ucEarthFlg = 0;
    }
    if(latch_upload_flag==0x55 || state_counter==0)//;     ///////置主动上传标志（有故障与时间两种上传标志）	
    {
    	latch_upload_flag=0;     ///////置主动上传标志  
    	      	
    	CreatNewSMS(SEND_AUTO);                      //产生主动上传短信
    	
 	if(g_gProcCnt[PC_GPRS_MODEL]==0)
 	    upload_GPRS|=SEND_AUTO;
        else
 	    upload_GPRS&=~SEND_AUTO;
 	if((g_ucPara_stime <2)||(g_ucPara_stime>48))  ////预置成24小时发送一次
            g_ucPara_stime = 24; 
         
        state_counter= g_gRunPara[RP_SENDSMS_T]*60;	//张弢 遥测起始地址修改运行参数 
    }
    if(eight_delay_flag==0x55)    //////八脉冲延时结束，准备发出8脉冲
    {
    	 eight_delay_flag=0;
    	 PulseReady();
	 if(rh_send_ok == 0x55)
	 	{
	 	g_gRmtInfo[YX_RH_FAIL] = 1;   //燃弧失败遥信置位	
	 	g_gRmtInfo[YX_RH_SUCCESS]=0;
	 	}
	 rh_send_ok = 0x85;	
       if(g_gProcCnt[PC_EFS_MODEL]>0)                   //////信号源类型为II型，
       	 {
       	 	if(g_ucEarthFlg == 1)                               //(mm[0]<=mm[1])&&(mm[0]<=mm[2])
       	 		{fault_sms_type=1;}
       	 	else if(g_ucEarthFlg == 2)//else if((mm[1]<=mm[0])&&(mm[1]<=mm[2]))
       	 		{fault_sms_type=2;}
       	        else if(g_ucEarthFlg == 3)  //else if((mm[2]<=mm[1])&&(mm[2]<=mm[0]))			
       	 	  {fault_sms_type=3;}
       	 }   
       else
           fault_sms_type=4; 
	   //g_gRmtInfo[YX_EARTH_TDELA]=1;//SaveLOG(LOG_EARTH_TDELAY,1);
       CreatNewSMS(FAULT_OCCUR);                      //产生故障短信//张弢  
    }

}

/*
//==============================================================================
//  函数名称   : GetRIStatus
//  功能描述   : 获取遥信信息中的单点状态
//  输入参数   : RIIndex:遥信信息表中的全局索引号，具体见constant.h中RMT_INFO部分
//  输出参数   ：<无>
//  返回值     : 1或0
//  其他说明   : 
//  作者       ： 
//==============================================================================
unsigned int GetRIStatus( unsigned int RIIndex )
{
    if( g_gRmtInfo[(RIIndex >> 4)] & BIT[(RIIndex & 0x0F)])
        return 1;
    else
        return 0;
}

//==============================================================================
//  函数名称   : SetRIStatus
//  功能描述   : 设置遥信信息中的单点状态，根据实际写入1或者0
//  输入参数   : RIIndex:遥信信息表中的全局索引号，具体见constant.h中RMT_INFO部分
//                         : Status: 写入状态，1或者0
//  输出参数   : <无>
//  返回值     : 
//  其他说明   : 
//  作者       ： 
//==============================================================================

void SetRIStatus( unsigned int RIIndex , unsigned int Status )
{
    if( Status > 0 )
    {
        //新状态设为高，则进行或操作
        g_gRmtInfo[(RIIndex >> 4)] |=  BIT[(RIIndex & 0x0F)];
    }
    else
    {
        //新状态为低，操作数取反，然后进行与操作
        g_gRmtInfo[(RIIndex >> 4)] &= ~BIT[(RIIndex & 0x0F)];
    }
}

//==============================================================================
//  函数名称   : ScanOut
//  功能描述   : 出口判断和输出
//  输入参数   : g_ucActFlag[i] 出口类型，如果该标志位ON，则需要出口对应
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 在AD中断中被调用，管理出口控制
//  作者       ：
//==============================================================================
void ScanOut(void)
{

}*/
/******************************************
*		Name     : void sign_repeat(void)
*   Function : repeat the 8 pulse
*   Para     : uint8 repeat_flag ,uint8 rev_flag      ////////后一个参数用来表示是否相位翻转
*   Return   : 
******************************************/ 
//==============================================================================
//  函数名称   : Sign_Repeat
//  功能描述   : 重新发送信号控制接触器动作
//  输入参数   : repeat_flag 重发类型   rev_flag 翻转相位标志
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void Sign_Repeat(unsigned char repeat_flag,unsigned char rev_flag)
{

    if(repeat_flag==0)       ////////重发
    {
        if((efslatch_flag==0)&&(fault_save>0))     ////////解锁以后才能重发
        {
            chongfa=1;
            eight_delay_counter=0;	
            pulse_phase_flag=0;
          
            if(g_gProcCnt[PC_EFS_MODEL]==0)    //////I型信号源
            {
                pulse_phase_flag=1;
		  eight_pulse_flag =1;               
            } 
            else                      ///////////II型信号源
            {
                eight_pulse_flag =1; 
                pulse_phase_flag=repeat_flag1;                
	     }	
	     g_TQBSCounter = 2;//投切、闭锁指示灯计数器  =0 是灭 =0x55 闭锁常亮  >=1投切闪烁	
	     g_gRmtInfo[YX_EFS_ACT] = 1;   //投切状态 遥信置1 
	     g_I0RmtZeroNum = 0;
	     if((g_sRecData.m_ucActRecStart == CLOSE)&&(g_sRecData.m_ucRecSavingFlag == OFF))	
			{				
	    		g_test=0;
    			g_sRecData.m_unRecAcTail =0; 
          		g_sRecData.m_gFaultRecSOE[REC_MSL] =g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
              	g_sRecData.m_gFaultRecSOE[REC_MSH] = g_sRtcManager.m_gRealTimer[RTC_SEC];
           		g_sRecData.m_gFaultRecSOE[REC_MINU] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
           		g_sRecData.m_gFaultRecSOE[REC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
             	g_sRecData.m_gFaultRecSOE[REC_DAY] = g_sRtcManager.m_gRealTimer[RTC_DATE];
             	g_sRecData.m_gFaultRecSOE[REC_MONTH] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
             	g_sRecData.m_gFaultRecSOE[REC_YEAR] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000);  
  				unsigned long ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;//flash地址  
  				g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址 	
  				g_sRecData.m_ucActRecStart = ON;//张弢录波 动作录波开始	
  				g_sRecData.m_LuboType = LuboType_ACT;
				
			g_gRmtInfo[YX_8FULS_STA]=1;//SaveLOG(LOG_8FULS_STA,1);
	     	}
	}   
    }
    else                    ////////模拟故障
    {
        if(g_gOverLoadFlg != ON)
	    efslatch_flag=0;
	chongfa=1;  
	if(rev_flag==0)          /////////说明不是第二次发送的波形
	    eight_select=0x8F;//eight_select|=BIT7;                    ////////////置首次发生8脉冲的标志	 
        g_gRmtInfo[YX_MANUAL_ACTION] = 1;   //置手动投切
        eight_delay_counter=200;
	//g_sRecData.m_ucActRecStart = ON;//张弢录波 动作录波开始//录波要在继电器动作提前0。5秒	
        if(g_gProcCnt[PC_EFS_MODEL]==0)    //////I型信号源
        {
            pulse_phase_flag=1;
            //delayms(2000);
            //eight_pulse_flag =1;
            moniguzhang = 1;	
        }
        else                       ///////II型信号源
        {
        	pulse_phase_flag = 3;
            switch(repeat_flag)    //模拟故障
            {
                case 1://a相发生故障
                      pulse_phase_flag = 3;
                      break;
                case 2://B相发生故障
                      pulse_phase_flag = 1;
                      break;
                case 3://C相发生故障
                      if((g_gProcCnt[PC_JAG_ACT] == 3) ||(g_gProcCnt[PC_JAG_ACT] == 4) )   //动作相序=3或4，没有B相，A相动作
                          pulse_phase_flag=1;
                      else
                          pulse_phase_flag = 2;
                      break;  
            }
			/*
			unsigned long ulAddr = (unsigned long)(FADDR_RECORDER_DATA+g_sRecData.m_gRecCNum*4)<<16;//flash地址  
  			g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址 	
  			g_sRecData.m_unRecAcTail =0; 
	    	 	g_sRecData.m_ucActRecStart = ON;//张弢录波 动作录波开始	
	    		g_test=0;           		
                    g_sRecData.m_gFaultRecSOE[REC_MSL] =g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
                    g_sRecData.m_gFaultRecSOE[REC_MSH] = g_sRtcManager.m_gRealTimer[RTC_SEC];
                    g_sRecData.m_gFaultRecSOE[REC_MINU] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
                    g_sRecData.m_gFaultRecSOE[REC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
                    g_sRecData.m_gFaultRecSOE[REC_DAY] = g_sRtcManager.m_gRealTimer[RTC_DATE];
                    g_sRecData.m_gFaultRecSOE[REC_MONTH] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
                    g_sRecData.m_gFaultRecSOE[REC_YEAR] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000);
  			*/	
            //delayms(2000);
	    //eight_pulse_flag =1;
	    moniguzhang = 1;	           
        }   	         
    }
}
/*
void Sign_Repeat(unsigned char repeat_flag,unsigned char rev_flag)
{

    if(repeat_flag==0)       ////////重发
    {
        if((efslatch_flag==0)&&(fault_save>0))     ////////解锁以后才能重发
        {
            chongfa=0;
            eight_delay_counter=0;	
            pulse_phase_flag=0;
          
            if(g_gProcCnt[PC_EFS_MODEL]==0)    //////I型信号源
            {
                pulse_phase_flag=1;
                chongfa=1;
                eight_delay_counter=200;          	
                if(rev_flag==0)
          	    repeat_flag1=4;       ///////I型的信号源，固定动作A相
            } 
            else                      ///////////II型信号源
       	    {
                if(g_ucEarthFlg == 1)  //A相发生故障 ,动作了C相
                    pulse_phase_flag = g_ucPhase_Chongfa;
                else if(g_ucEarthFlg == 2)  //B相发生故障 ,动作了C相
                    pulse_phase_flag = g_ucPhase_Chongfa;
                else if(g_ucEarthFlg == 3)  //C相发生故障 ,动作了A相
                    pulse_phase_flag = g_ucPhase_Chongfa; 
          	chongfa=1;
                eight_delay_counter=200;	  
                if(g_gRunPara[RP_JAG_ACT] == 3)
          	    repeat_flag1=g_ucEarthFlg;       ///////II型的信号源，C相发生故障 
                g_ucPhase_Chongfa = 0;
	    }	
	}   
    }
    else                    ////////模拟故障
    {
        if(g_gOverLoadFlg != ON)
	    efslatch_flag=0;
	  
	if(rev_flag==0)          /////////说明不是第二次发送的波形
	    eight_select=0x8F;//eight_select|=BIT7;                    ////////////置首次发生8脉冲的标志	 
        chongfa=1;
        eight_delay_counter=200;	
        if(g_gProcCnt[PC_EFS_MODEL]==0)    //////I型信号源
        {
            pulse_phase_flag=1;
            if(rev_flag==0)
                repeat_flag1=4;       ///////I型的信号源，固定动作A相
        }
        else                       ///////II型信号源
        {
            if(rev_flag==0)    //模拟故障
            {
                g_ucEarthFlg = repeat_flag;
                if(g_gRunPara[RP_JAG_ACT] <= 3)
                    repeat_flag1=repeat_flag;       ///////II型的信号源，
                if(g_ucEarthFlg == 1)  //A相发生故障 ,动作C相
                {
                    pulse_phase_flag = 3;
                    if(g_gRunPara[RP_JAG_ACT] == 2)    //动作滞后相
                        pulse_phase_flag=2;
                }
                else if(g_ucEarthFlg == 2)  //B相发生故障 ,动作A相
                {
                    pulse_phase_flag = 1;
                    if(g_gRunPara[RP_JAG_ACT] == 2)    //动作滞后相
                        pulse_phase_flag=3;
                }
                else if(g_ucEarthFlg == 3)  //C相发生故障 ,动作B相
                {
                    pulse_phase_flag = 1;
                    if(g_gRunPara[RP_JAG_ACT] == 2)    //动作滞后相
                        pulse_phase_flag=1;
                }
            }
           // else    //相位翻转
            {
                if(repeat_flag1 == 1 && pulse_phase_flag == 3)  //A相发生故障 ,动作了C相
                    pulse_phase_flag = 1;
                
                else if(repeat_flag1 == 2 && pulse_phase_flag == 3)  //B相发生故障 ,动作了C相
                    pulse_phase_flag = 1;
                else if(repeat_flag1 == 2 && pulse_phase_flag == 1)  //B相发生故障 ,动作了A相
                    pulse_phase_flag = 3;
                else if(repeat_flag1 == 3 && pulse_phase_flag == 1)  //C相发生故障 ,动作了A相
                    pulse_phase_flag = 3;
                
            }//
          
        }   	         
    }
}
*/