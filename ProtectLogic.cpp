/**********************************************************************************
Copyright (C) 1988-2007, CREAT Company 
ģ����         : �����߼�ģ��
�ļ���         : ProtectLogic.c
����ļ�       : DataStruct.h FuncDef.h Constant.h
ʵ�ֹ���       : �����߼��жϹ��̣���������Ԫ���жϡ������߼��жϹ��̵ȣ������ݽ��������س��ں���������״̬����
����           :  
�汾           : 1.0

--------------------------------------------------------------------------------
��ע           : -
--------------------------------------------------------------------------------

�޸ļ�¼       : 
��  ��          �汾            �޸���          �޸����� 
2010/06/01      1.0                          ����

************************************************************************************/

#ifndef _PROTECT_LOGIC

#define _PROTECT_LOGIC
#include "DataStruct.h"
//#include "IvtTable.h"
#endif
//unsigned char g_ucEarthFlg = 0;  //��������־
unsigned char g_ucPhase_Chongfa = 0;  //�ط��ο�����뷢�����Ϻ��״ζ������һ��
static unsigned char  phasea_off= 0;
static unsigned char  phaseb_off= 0;
static unsigned char  phasec_off= 0;
//==============================================================================
//  ��������   : ProtStart
//  ��������   : ��������Ԫ���ж�
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       �� 
//==============================================================================
void ProtStart1(void)
{
    if(fault_time>0)                           /////////���ڹ��ϻָ�
    {  
        if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0))  /////////���ڹ��ϻָ�zero sequence in effective
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
        else if((g_gProcCnt[PC_JAG_Z]==0)&&(g_gProcCnt[PC_JAG_P]==0x55))      //////////////////������о�  
     	{		                                   
            if((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UA]<g_gProcCntJug1[PC1_HIGH_P])&&(g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UB]<g_gProcCntJug1[PC1_HIGH_P])&&(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UC]<g_gProcCntJug1[PC1_HIGH_P])) /////////����   ����ӵ��о�
       	    { 	
                fault_begin=0;
                fault_end=0x55;
                g_gRmtInfo[YX_EARTH_FAULT] = 0;
		  		g_gRmtInfo[YX_AEARTH_FAULT] = 0;g_gRmtInfo[YX_BEARTH_FAULT] = 0;g_gRmtInfo[YX_CEARTH_FAULT] = 0;	
		  		//g_gRmtInfo[YX_EARTH_TDELA]=0;//SaveLOG(LOG_EARTH,0);SaveLOG(LOG_EARTH_TDELAY,0);
                //g_gRmtInfo[0] &= ~(YX_PHASEA_FAULT + YX_PHASEB_FAULT + YX_PHASEC_FAULT);
            }     
       	}    
        else if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0x55))      //////////////////������о�  
        {
            if(((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UA]<g_gProcCntJug1[PC1_HIGH_P])&&(g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UB]<g_gProcCntJug1[PC1_HIGH_P])&&(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UC]<g_gProcCntJug1[PC1_HIGH_P]))||(g_gProcMeas[RM_U0]<g_gProcCntJug1[PC1_LOW_Z])) /////////����   ����ӵ��о�
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
          
    if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0)) 	       ///////////////�����о� 	  	 
    {
        if((fault_save==0)&&(fault_time<g_gProcCnt[PC_T_DELAY]))
        {	
            if(g_gProcMeas[RM_U0]>g_gProcCntJug1[PC1_HIGH_Z])      //////��⵽����ߵ�ѹ��������
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
    else if((g_gProcCnt[PC_JAG_Z]==0)&&(g_gProcCnt[PC_JAG_P]==0x55))      //////////////////������о�  
    {
        if((fault_save==0)&&(fault_time<g_gProcCnt[PC_T_DELAY]))
        {	
            if(((g_gProcMeas[RM_UA]<g_gProcCntJug1[PC1_LOW_P])&&((g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_HIGH_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_HIGH_P]))) /////////A���ѹ   ����ӵ��о�
				||((g_gProcMeas[RM_UA]<g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UA]<g_gProcMeas[RM_UB])&&(g_gProcMeas[RM_UA]<g_gProcMeas[RM_UC])
					&&((g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_LOW_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)))
			{ 	
                fault_begin=0x55;
		  		fault_pluse =RM_UA;
           		fault_end=0;
				//SaveLOG(LOG_EARTH,1);
            }     
            else if(((g_gProcMeas[RM_UB]<g_gProcCntJug1[PC1_LOW_P])&&((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_HIGH_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_HIGH_P]))) ////////B���ѹ   ����ӵ��о�
				||((g_gProcMeas[RM_UB]<g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UB]<g_gProcMeas[RM_UA])&&(g_gProcMeas[RM_UA]<g_gProcMeas[RM_UC])
					&&((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_LOW_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)))
			{ 	
    	 		fault_begin=0x55;
				fault_pluse =RM_UB;	
    	 		fault_end=0;
				//SaveLOG(LOG_EARTH,1);
    	    }    
	    	else if(((g_gProcMeas[RM_UC]<g_gProcCntJug1[PC1_LOW_P])&&((g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_HIGH_P])||(g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_HIGH_P]))) /////////C���ѹ   ����ӵ��о�
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
    else if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0x55))     /////////�������ͬʱ���о�	 
    { 
        if((fault_save==0)&&(fault_time<g_gProcCnt[PC_T_DELAY]))
        {	         	 	
            if(((g_gProcMeas[RM_UA]<g_gProcCntJug1[PC1_LOW_P])&&((g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_HIGH_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_HIGH_P]))&&(g_gProcMeas[RM_U0]>g_gProcCntJug1[PC1_HIGH_Z])) /////////A���ѹ   ����ӵ��о�
            	||((g_gProcMeas[RM_UA]<g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UA]<g_gProcMeas[RM_UB])&&(g_gProcMeas[RM_UA]<g_gProcMeas[RM_UC])
					&&((g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_LOW_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)&&(g_gProcMeas[RM_U0]>g_gProcCntJug1[PC1_HIGH_Z])))
       	    	{ 	
           	 	fault_begin=0x55;
			 	//SaveLOG(LOG_EARTH,1);
				fault_pluse =RM_UA;	 
           	 	fault_end=0;
            	}       
            else if(((g_gProcMeas[RM_UB]<g_gProcCntJug1[PC1_LOW_P])&&((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_HIGH_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_HIGH_P]))&&(g_gProcMeas[RM_U0]>g_gProcCntJug1[PC1_HIGH_Z])) /////////A���ѹ   ����ӵ��о�
				||((g_gProcMeas[RM_UB]<g_gProcCntJug1[PC1_LOW_P])&&(g_gProcMeas[RM_UB]<g_gProcMeas[RM_UA])&&(g_gProcMeas[RM_UA]<g_gProcMeas[RM_UC])
					&&((g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_LOW_P])||(g_gProcMeas[RM_UC]>g_gProcCntJug1[PC1_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)&&(g_gProcMeas[RM_U0]>g_gProcCntJug1[PC1_HIGH_Z])))
       	    	{ 	
           	 	fault_begin=0x55;
				fault_pluse =RM_UB;	 
           	 	fault_end=0;
				//SaveLOG(LOG_EARTH,1);
            	}      
            else if(((g_gProcMeas[RM_UC]<g_gProcCntJug1[PC1_LOW_P])&&((g_gProcMeas[RM_UB]>g_gProcCntJug1[PC1_HIGH_P])||(g_gProcMeas[RM_UA]>g_gProcCntJug1[PC1_HIGH_P]))&&(g_gProcMeas[RM_U0]>g_gProcCntJug1[PC1_HIGH_Z])) /////////A���ѹ   ����ӵ��о�
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
	 	&&(fault_lubo==0)) //////�����Ѿ���ʼ
  		{
  		fault_lubo=0x55;
		g_sRecData.m_ucFaultRecStart = ON;//��������¼��
		g_sRecData.m_LuboType = LuboType_EARTH;			
      	}
     /*if((g_sRecData.m_ucActRecStart == CLOSE)&&(g_sRecData.m_ucRecSavingFlag == OFF)
	 	&&(g_sRecData.m_ucFaultRecStart ==OFF)&&(fault_end==0x55)
	 	&&(fault_lubo==0x55)) //////�����Ѿ��ָ�
  		{
  		fault_lubo=0;
		g_sRecData.m_ucFaultRecStart = ON;//�������ϻָ�¼��
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
   // unsigned long ulProcCntJdg[8];  //��ʱ���沿�ֱ�����ֵ
    //tAcTail = g_sSampleData.m_unAcDataTail; //���µ�λ��
    //tAcTail2 = (g_sSampleData.m_unAcDataTail - 8) & 0x1F;   //�������ǰ��λ��
    //tAcTailN = (g_sSampleData.m_unAcDataTail - 16) & 0x1F;  //һ������ǰ��λ��
  //  for(i = 0; i < 3; i++)  //A�� B�� C��
  /*  if(g_gProcCnt[PC_OVERLOAD_CNT] != 0)   //��������Ͷ��
    {
        if(g_gProcMeas[PM_I0] >= g_gProcCntJug_I0[0])  //��������
        {
            if(g_gOverLoadTimeNum < g_gProcCntJug_I0[2])
                g_gOverLoadTimeNum++;
            if(g_gOverLoadTimeNum >= g_gProcCntJug_I0[2])
                g_gOverLoadFlg = ON;
        }
        else if(g_gProcMeas[PM_I0] < g_gProcCntJug_I0[1])  //�����ز�֮��
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
        if((eight_pulse_flag>0)&&(pulse_flag!=0x55)&&(flash_write!=0x55))  //8��������Ѿ�������δ����8�������ֵ��û�н��д洢
        {
        if((long)(tempi - tempi2) >= (long)g_gProcCnt[PC_I0_START])    //0   
        {
            pulse_flag++;//SetRIStatus(TMP_TQD_A + i,1);//��ͻ����������־�����ݱ�־����ͻ����������ʱ�������10ms�ڣ�û�й����¼�����������0�������ʱ����ͻ��������ʱ��
            if(pulse_flag==3 )   //&& (eight_pulse_flag%2)==0                   ///////������⵽3���ù��ϱ�־��ȷ����8���巢��
            {   
   	       // if(flash_write==0)                  //////��ʾ��⵽�˵���ֵ������û�е���FLASH��д���ʱ��
   		  //  flash_write=0x33;
   		    	     	
         	pulse_flag=0x55;
       
         	wave_counter=0;
         	if(pulse_counter>=7)
         	    pulse_counter=7;  
                if(pulse_counter==0)
                {   	                 
                    g_sSoeData.m_gRecBuff[0]=g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;      //��
      	            g_sSoeData.m_gRecBuff[1]=g_sRtcManager.m_gRealTimer[RTC_MONTH];      //��
      	            g_sSoeData.m_gRecBuff[2]=g_sRtcManager.m_gRealTimer[RTC_DATE];      //��
      	            g_sSoeData.m_gRecBuff[3]=g_sRtcManager.m_gRealTimer[RTC_HOUR];      //ʱ
      	            g_sSoeData.m_gRecBuff[4]=g_sRtcManager.m_gRealTimer[RTC_MINUT];      //��
      	            g_sSoeData.m_gRecBuff[5]=g_sRtcManager.m_gRealTimer[RTC_SEC];      //��
      	            g_sSoeData.m_gRecBuff[6]=g_sRtcManager.m_gRealTimer[RTC_MICROSEC] / 10;      //10���� 
      	            g_sSoeData.m_gRecBuff[7]=g_sRtcManager.m_gRealTimer[RTC_MICROSEC] % 10; 
      		}
      	        else
      		{
      		    m=pulse_counter*29+4;         	
      	            g_sSoeData.m_gRecBuff[m++]=g_sRtcManager.m_gRealTimer[RTC_MINUT];      //��
      	            g_sSoeData.m_gRecBuff[m++]=g_sRtcManager.m_gRealTimer[RTC_SEC];      //��
      	            g_sSoeData.m_gRecBuff[m++]=g_sRtcManager.m_gRealTimer[RTC_MICROSEC] / 10;      //10���� 
      	            g_sSoeData.m_gRecBuff[m++]=g_sRtcManager.m_gRealTimer[RTC_MICROSEC] % 10;
      		}	        	      	
            }	
        }
        else
            pulse_flag = 0;
      }     
        
    }*/
   /* if((P5IN&BIT5)==0x00)    //��⵽�ź�Դ����
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
    if(fault_time>0)                           /////////���ڹ��ϻָ�
    {  
        if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0))  /////////���ڹ��ϻָ�zero sequence in effective
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
        else if((g_gProcCnt[PC_JAG_Z]==0)&&(g_gProcCnt[PC_JAG_P]==0x55))      //////////////////������о�  
     	{		                                   
            if((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_HIGH_P])&&(g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_HIGH_P])&&(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_HIGH_P])) /////////����   ����ӵ��о�
       	    { 	
                fault_begin=0;
                fault_end=0x55;
                g_gRmtInfo[YX_EARTH_FAULT] = 0;
		  		g_gRmtInfo[YX_AEARTH_FAULT] = 0;g_gRmtInfo[YX_BEARTH_FAULT] = 0;g_gRmtInfo[YX_CEARTH_FAULT] = 0;	
		  		//g_gRmtInfo[YX_EARTH_TDELA]=0;//SaveLOG(LOG_EARTH,0);SaveLOG(LOG_EARTH_TDELAY,0);
                //g_gRmtInfo[0] &= ~(YX_PHASEA_FAULT + YX_PHASEB_FAULT + YX_PHASEC_FAULT);
            }     
       	}    
        else if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0x55))      //////////////////������о�  
        {
            if(((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_HIGH_P])&&(g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_HIGH_P])&&(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_HIGH_P]))||(g_gRmtMeas[RM_U0]<g_gProcCntJug[PC_LOW_Z])) /////////����   ����ӵ��о�
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
          
    if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0)) 	       ///////////////�����о� 	  	 
    {
        if((fault_save==0)&&(fault_time<g_gProcCntJug[PC_T_DELAY]))
        {	
            if(g_gRmtMeas[RM_U0]>g_gProcCntJug[PC_HIGH_Z])      //////��⵽����ߵ�ѹ��������
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
    else if((g_gProcCnt[PC_JAG_Z]==0)&&(g_gProcCnt[PC_JAG_P]==0x55))      //////////////////������о�  
    {
        if((fault_save==0)&&(fault_time<g_gProcCntJug[PC_T_DELAY]))
        {	
            if(((g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_LOW_P])&&((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_HIGH_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_HIGH_P]))) /////////A���ѹ   ����ӵ��о�
				||((g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UC])
					&&((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)))
			{ 	
                fault_begin=0x55;
		  		fault_pluse =RM_UA;
           		fault_end=0;
				//SaveLOG(LOG_EARTH,1);
            }     
            else if(((g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_LOW_P])&&((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_HIGH_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_HIGH_P]))) ////////B���ѹ   ����ӵ��о�
				||((g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UB]<g_gRmtMeas[RM_UA])&&(g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UC])
					&&((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)))
			{ 	
    	 		fault_begin=0x55;
				fault_pluse =RM_UB;	
    	 		fault_end=0;
				//SaveLOG(LOG_EARTH,1);
    	    }    
	    	else if(((g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_LOW_P])&&((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_HIGH_P])||(g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_HIGH_P]))) /////////C���ѹ   ����ӵ��о�
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
    else if((g_gProcCnt[PC_JAG_Z]==0x55)&&(g_gProcCnt[PC_JAG_P]==0x55))     /////////�������ͬʱ���о�	 
    { 
        if((fault_save==0)&&(fault_time<g_gProcCntJug[PC_T_DELAY]))
        {	         	 	
            if(((g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_LOW_P])&&((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_HIGH_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_HIGH_P]))&&(g_gRmtMeas[RM_U0]>g_gProcCntJug[PC_HIGH_Z])) /////////A���ѹ   ����ӵ��о�
            	||((g_gRmtMeas[RM_UA]<g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UC])
					&&((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_LOW_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)&&(g_gRmtMeas[RM_U0]>g_gProcCntJug[PC_HIGH_Z])))
       	    	{ 	
           	 	fault_begin=0x55;
			 	//SaveLOG(LOG_EARTH,1);
				fault_pluse =RM_UA;	 
           	 	fault_end=0;
            	}       
            else if(((g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_LOW_P])&&((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_HIGH_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_HIGH_P]))&&(g_gRmtMeas[RM_U0]>g_gProcCntJug[PC_HIGH_Z])) /////////A���ѹ   ����ӵ��о�
				||((g_gRmtMeas[RM_UB]<g_gProcCntJug[PC_LOW_P])&&(g_gRmtMeas[RM_UB]<g_gRmtMeas[RM_UA])&&(g_gRmtMeas[RM_UA]<g_gRmtMeas[RM_UC])
					&&((g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_LOW_P])||(g_gRmtMeas[RM_UC]>g_gProcCntJug[PC_LOW_P]))
					&&(g_gDebugP[Debug_SRECJU2]==0x55)&&(g_gRmtMeas[RM_U0]>g_gProcCntJug[PC_HIGH_Z])))
       	    	{ 	
           	 	fault_begin=0x55;
				fault_pluse =RM_UB;	 
           	 	fault_end=0;
				//SaveLOG(LOG_EARTH,1);
            	}      
            else if(((g_gRmtMeas[RM_UC]<g_gProcCntJug[PC_LOW_P])&&((g_gRmtMeas[RM_UB]>g_gProcCntJug[PC_HIGH_P])||(g_gRmtMeas[RM_UA]>g_gProcCntJug[PC_HIGH_P]))&&(g_gRmtMeas[RM_U0]>g_gProcCntJug[PC_HIGH_Z])) /////////A���ѹ   ����ӵ��о�
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
	 	&&(fault_lubo==0)) //////�����Ѿ���ʼ
  		{
  		fault_lubo=0x55;
		g_sRecData.m_ucFaultRecStart = ON;//��������¼��
		g_sRecData.m_LuboType = LuboType_EARTH;			
      	}
     /*if((g_sRecData.m_ucActRecStart == CLOSE)&&(g_sRecData.m_ucRecSavingFlag == OFF)
	 	&&(g_sRecData.m_ucFaultRecStart ==OFF)&&(fault_end==0x55)
	 	&&(fault_lubo==0x55)) //////�����Ѿ��ָ�
  		{
  		fault_lubo=0;
		g_sRecData.m_ucFaultRecStart = ON;//�������ϻָ�¼��
		g_sRecData.m_LuboType = SREC_EARTH_RST;			
      	} */   
}
//==============================================================================
//  ��������   : PulseReady
//  ��������   : 8���巢��ǰ��׼������
//               
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void PulseReady(void)
{
    if(efslatch_flag==0)       /////////�жϱ����Ѿ�����
    {
    //SaveLOG(LOG_8FULS_STA,1);
    	if(chongfa==0)  ///////�Լ������İ�����
	{ 
	    eight_select=0x8f;                    ////////////���״η���8����ı�־	
            g_ucPhase_Chongfa = 0;   //�״η���8���壬�ο���־��0
	    if(g_gProcCnt[PC_EFS_MODEL]==0)    //////I���ź�Դ
            {
                pulse_phase_flag=1;
             	repeat_flag1=4;       ////////A�෢������ 
                g_ucEarthFlg = 4;
                g_gRmtInfo[YX_EARTH_FAULT] = 1;	
            }  
            else                     ///////II���ź�Դ
            { 
	        if((g_gRmtMeas[RM_UC]<=g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UC]<=g_gRmtMeas[RM_UA]))		
	        {	
		    if(g_gRmtMeas[RM_UC] <= 2000 || ((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P]) && (g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P])))  //C��Ϊ�ӵ���
		        g_ucEarthFlg = 3;////////C�෢������	
                    else if(g_gRmtMeas[RM_UC] <= g_gProcCntJug[PC_LOW_P] &&(g_gRmtMeas[RM_UB] >= g_gRmtMeas[RM_UA] ) )
                        g_ucEarthFlg = 3;////////C�෢������
                        else if(g_gRmtMeas[RM_UC] <= g_gProcCntJug[PC_LOW_P] &&(g_gRmtMeas[RM_UA] > g_gRmtMeas[RM_UB] ) )
					g_ucEarthFlg = 2;////////B�෢������		
             	}		         	 
	        else if((g_gRmtMeas[RM_UB]<=g_gRmtMeas[RM_UA])&&(g_gRmtMeas[RM_UB]<=g_gRmtMeas[RM_UC]))	
	        {
		    if(g_gRmtMeas[RM_UB] <= 2000 || ((g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P]) && (g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P])))
                        g_ucEarthFlg = 2;////////B�෢������  
                    else if(g_gRmtMeas[RM_UB] <= g_gProcCntJug[PC_LOW_P] &&(g_gRmtMeas[RM_UA] >= g_gRmtMeas[RM_UC] ) )
                        g_ucEarthFlg = 2;////////B�෢������ 	
                        else if(g_gRmtMeas[RM_UB] <= g_gProcCntJug[PC_LOW_P] &&(g_gRmtMeas[RM_UA] < g_gRmtMeas[RM_UC] ) )
					g_ucEarthFlg = 1;////////A�෢������ 	
             	}		
	        else if((g_gRmtMeas[RM_UA]<=g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UA]<=g_gRmtMeas[RM_UC]))	
	        {
	            if(g_gRmtMeas[RM_UA] <= 2000 || ((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P]) && (g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P])))
			g_ucEarthFlg = 1;////////a�෢������  
		    else if(g_gRmtMeas[RM_UA] <= g_gProcCntJug[PC_LOW_P] &&(g_gRmtMeas[RM_UC] >= g_gRmtMeas[RM_UB] ))
                    	g_ucEarthFlg = 1;////////a�෢������ 
                    	else if(g_gRmtMeas[RM_UA] <= g_gProcCntJug[PC_LOW_P] &&(g_gRmtMeas[RM_UC] < g_gRmtMeas[RM_UB] ))
				g_ucEarthFlg = 3;////////C�෢������
                }
			pulse_phase_flag = 3;
                switch(g_ucEarthFlg)
                {
                    case 1://a�෢������
                      pulse_phase_flag = 3;
			 g_gRmtInfo[YX_AEARTH_FAULT] = 1;		  
                      break;
                    case 2://B�෢������
                      pulse_phase_flag = 1;
			 g_gRmtInfo[YX_BEARTH_FAULT] = 1;		  
                      break;
                    case 3://C�෢������
                      g_gRmtInfo[YX_CEARTH_FAULT] = 1;
                      if((g_gProcCnt[PC_JAG_ACT] == 3) ||(g_gProcCnt[PC_JAG_ACT] == 4) )   //��������=3��4��û��B�࣬A�ද��
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
	 g_TQBSCounter = 2;//Ͷ�С�����ָʾ�Ƽ�����  =0 ���� =0x55 ��������  >=1Ͷ����˸
	 g_gRmtInfo[YX_EFS_ACT] = 1;   //Ͷ��״̬ ң����1 
	 g_I0RmtZeroNum = 0;
        }   
	if(moniguzhang==1)  
		{
		eight_pulse_flag=1; 	//=1Ϊģ����� 
		g_TQBSCounter = 2;//Ͷ�С�����ָʾ�Ƽ�����  =0 ���� =0x55 ��������  >=1Ͷ����˸
		g_gRmtInfo[YX_EFS_ACT] = 1;   //Ͷ��״̬ ң����1 
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
    if(efslatch_flag==0)       /////////�жϱ����Ѿ�����
    {
        if(chongfa==0)  ///////�Լ������İ�����
		{ 
			eight_select=0x8f;                    ////////////���״η���8����ı�־	
            g_ucPhase_Chongfa = 0;   //�״η���8���壬�ο���־��0
			if(g_gProcCnt[PC_EFS_MODEL]==0)    //////I���ź�Դ
            {
                pulse_phase_flag=1;
             	repeat_flag1=4;       ////////A�෢������ 
                g_ucEarthFlg = 4;
                g_gRmtInfo[0] |= YX_EARTH_FAULT; 		
            }  
            else                     ///////II���ź�Դ
            { 
	            if((g_gRmtMeas[RM_UC]<=g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UC]<=g_gRmtMeas[RM_UA]))		
	            {	
                        if(g_gRmtMeas[RM_UC] <= 2000 || ((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P]) && (g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P])))  //C��Ϊ�ӵ���
                           {

          	  	         pulse_phase_flag=1;   //�̶�����A     
                                 g_ucEarthFlg = 3;////////C�෢������
                                 if(g_gProcCnt[PC_JAG_ACT] == 3)
                                    repeat_flag1=3;
                                 
                           }
                           else if(g_gRmtMeas[RM_UC] <= g_gProcCntJug[PC_LOW_P] &&((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P]) || (g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P])))
                           {

                               pulse_phase_flag=1;   //�̶�����A     
                                 g_ucEarthFlg = 3;////////C�෢������
                                 if(g_gProcCnt[PC_JAG_ACT] == 3)
                                    repeat_flag1=3;
                           }  

             	    }		         	 
	            else if((g_gRmtMeas[RM_UB]<=g_gRmtMeas[RM_UA])&&(g_gRmtMeas[RM_UB]<=g_gRmtMeas[RM_UC]))	
	            {
	           	if(g_gRmtMeas[RM_UB] <= 2000 || ((g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P]) && (g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P])))
                           {
                               if(phase_seq_flag==0)             /////////��������
          	 	       {
          	 	           pulse_phase_flag=1;
                                   if(g_gProcCnt[PC_JAG_ACT] == 2)    //�����ͺ���
                                         pulse_phase_flag=3;
              	 	       }  
          	               else                              /////////�������򣬶������������෴
          	  	       {
                                   pulse_phase_flag=3;
                                   if(g_gProcCnt[PC_JAG_ACT] == 2)    //�����ͺ���
                                         pulse_phase_flag=1;
          	  	       }
                               g_ucEarthFlg = 2;////////B�෢������  
                               if(g_gProcCnt[PC_JAG_ACT] == 3)
                                   repeat_flag1=2;
                           }
                           else if(g_gRmtMeas[RM_UB] <= g_gProcCntJug[PC_LOW_P] &&((g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P]) || (g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P])))
                           {

                                 pulse_phase_flag=1;
                                 g_ucEarthFlg = 2;////////B�෢������  
                               if(g_gProcCnt[PC_JAG_ACT] == 3)
                                   repeat_flag1=2;
             	            }
             	    }		
	            else if((g_gRmtMeas[RM_UA]<=g_gRmtMeas[RM_UB])&&(g_gRmtMeas[RM_UA]<=g_gRmtMeas[RM_UC]))	
	            {
	                if(g_gRmtMeas[RM_UA] <= 2000 || ((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P]) && (g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P])))
                    {

						pulse_phase_flag=3;
						g_ucEarthFlg = 1;////////a�෢������  
						if(g_gProcCnt[PC_JAG_ACT] == 3)
							repeat_flag1=1;                          
					}
					else if(g_gRmtMeas[RM_UA] <= g_gProcCntJug[PC_LOW_P] &&((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P]) || (g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P])))
					{
						pulse_phase_flag=3;
						g_ucEarthFlg = 1;////////a�෢������  
						if(g_gProcCnt[PC_JAG_ACT] == 3)
							repeat_flag1=1;
					}
				}
				g_ucPhase_Chongfa = pulse_phase_flag;
				g_gRmtInfo[0] |= YX_EARTH_FAULT;

			}	
		}   
		else       //////�·������� chongfa!=0;
			chongfa=0; 
		eight_pulse_flag=1; 
       // pulse_time_flag=11;                 	   ///////////����8������ʱ����       
    }// end  efslatch_flag==0  
    
}
*/
//==============================================================================
//  ��������   : ProtLogic
//  ��������   : �����߼��жϹ��̺ͼ�ʱ����
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       �� 
//==============================================================================
void ProtLogic(void)
{
    unsigned char i;
  //  if(g_ucDuanxianFlg >= 50)
    {
    if((phase_off&BIT0)==BIT0)      /////////A����ڶ��� ,�ж��Ƿ񸴹�
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
    if((phase_off&BIT1)==BIT1)      /////////B����ڶ��� ,�ж��Ƿ񸴹�
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
    if((phase_off&BIT2)==BIT2)      /////////C����ڶ��� ,�ж��Ƿ񸴹�
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
     ///////�������������жϵĴ洢����ֹ��������ͬʱ���ֻ��Ǵ洢����
    if(phase_copy!=phase_off)       /////////˵����������
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
    
    if((phase_off&0x01)==0x00)       ////////�����ж�A���Ƿ��ж���
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
    if((phase_off&0x02)==0x00)       ////////�����ж�B���Ƿ��ж���
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
    if((phase_off&0x04)==0x00)       ////////�����ж�C���Ƿ��ж���
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
    //����3���ѹ�쳣ң��λ
  /*  if((g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_NO_V] && g_gRmtMeas[RM_UA] <= g_gProcCntJug[PC_LOW_P])||g_gRmtMeas[RM_UA] >= g_gProcCntJug[PC_HIGH_P])  //A���ѹ�쳣
        g_gRmtInfo[0] |= YX_PHASEA_ABNORMAL;
    else if(g_gRmtMeas[RM_UA] > g_gProcCntJug[PC_LOW_P] && g_gRmtMeas[RM_UA] < g_gProcCntJug[PC_HIGH_P])
        g_gRmtInfo[0] &= ~YX_PHASEA_ABNORMAL;
    if((g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_NO_V] && g_gRmtMeas[RM_UB] <= g_gProcCntJug[PC_LOW_P])||g_gRmtMeas[RM_UB] >= g_gProcCntJug[PC_HIGH_P])  //B���ѹ�쳣
        g_gRmtInfo[0] |= YX_PHASEB_ABNORMAL;
    else if(g_gRmtMeas[RM_UB] > g_gProcCntJug[PC_LOW_P] && g_gRmtMeas[RM_UB] < g_gProcCntJug[PC_HIGH_P])
        g_gRmtInfo[0] &= ~YX_PHASEB_ABNORMAL;
    if((g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_NO_V] && g_gRmtMeas[RM_UC] <= g_gProcCntJug[PC_LOW_P])||g_gRmtMeas[RM_UC] >= g_gProcCntJug[PC_HIGH_P])  //C���ѹ�쳣
        g_gRmtInfo[0] |= YX_PHASEC_ABNORMAL;
    else if(g_gRmtMeas[RM_UC] > g_gProcCntJug[PC_LOW_P] && g_gRmtMeas[RM_UC] < g_gProcCntJug[PC_HIGH_P])
        g_gRmtInfo[0] &= ~YX_PHASEC_ABNORMAL;
     */ 
	  ///////�������������жϵĴ洢����ֹ��������ͬʱ���ֻ��Ǵ洢����
    if(phase_copy!=phase_off)       /////////˵����������
    {
	phase_copy=phase_off;
	for(i=0;i<6;i++)
           g_sSoeData.m_gBreakBuff[i] = g_sRtcManager.m_gRealTimer[i];
        g_sSoeData.m_gBreakBuff[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
	g_sSoeData.m_gBreakBuff[6]=(unsigned char)(g_gRmtMeas[RM_UA]/100);
	g_sSoeData.m_gBreakBuff[7]=(unsigned char)(g_gRmtMeas[RM_UB]/100);
	g_sSoeData.m_gBreakBuff[8]=(unsigned char)(g_gRmtMeas[RM_UC]/100);
	save_phase_off=0x55;
	CreatNewSMS(PHASE_BREAK);                      //�������߶���  	   	 
	  	 
 	if(g_gProcCnt[PC_GPRS_MODEL]==0)                                       //////////////�϶��ᷢ����Ϣ�����ǻ᲻�ᷢ��GPRS���ݻ���Ҫ
   	{
   	    upload_interval[3]=(3*upload_interval_set)-1;      //////////////���� 4 minutes
            upload_flag|=PHASE_BREAK;	
   	}
        else
   	{
   	    upload_interval[3]=0;      //////////////eight pulse set 4 minutes	
   	    upload_flag&=~PHASE_BREAK;	
   	}
    }
    }
    if(fault_recovery_flag==0x55)           ////////// ��⵽���ϸ���
    {
    	fault_recovery_flag=0;	    	 	 
    	for(i=0;i<6;i++)
           g_sSoeData.m_gSoeRSTBuff[i] = g_sRtcManager.m_gRealTimer[i];
        g_sSoeData.m_gSoeRSTBuff[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
    	g_sSoeData.m_gSoeRSTBuff[6]=(unsigned char)(g_gRmtMeas[RM_UA]/100);
        g_sSoeData.m_gSoeRSTBuff[7]=(unsigned char)(g_gRmtMeas[RM_UB]/100);
        g_sSoeData.m_gSoeRSTBuff[8]=(unsigned char)(g_gRmtMeas[RM_UC]/100);
        g_sSoeData.m_gSoeRSTBuff[9]=(unsigned char)(g_gRmtMeas[RM_U0]/100);
        CreatNewSMS(FAULT_RST);                      //�������ϸ������
        g_ucEarthFlg = 0;
    }
    if(latch_upload_flag==0x55 || state_counter==0)//;     ///////�������ϴ���־���й�����ʱ�������ϴ���־��	
    {
    	latch_upload_flag=0;     ///////�������ϴ���־  
    	      	
    	CreatNewSMS(SEND_AUTO);                      //���������ϴ�����
    	
 	if(g_gProcCnt[PC_GPRS_MODEL]==0)
 	    upload_GPRS|=SEND_AUTO;
        else
 	    upload_GPRS&=~SEND_AUTO;
 	if((g_ucPara_stime <2)||(g_ucPara_stime>48))  ////Ԥ�ó�24Сʱ����һ��
            g_ucPara_stime = 24; 
         
        state_counter= g_gRunPara[RP_SENDSMS_T]*60;	//�ŏ| ң����ʼ��ַ�޸����в��� 
    }
    if(eight_delay_flag==0x55)    //////��������ʱ������׼������8����
    {
    	 eight_delay_flag=0;
    	 PulseReady();
	 if(rh_send_ok == 0x55)
	 	{
	 	g_gRmtInfo[YX_RH_FAIL] = 1;   //ȼ��ʧ��ң����λ	
	 	g_gRmtInfo[YX_RH_SUCCESS]=0;
	 	}
	 rh_send_ok = 0x85;	
       if(g_gProcCnt[PC_EFS_MODEL]>0)                   //////�ź�Դ����ΪII�ͣ�
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
       CreatNewSMS(FAULT_OCCUR);                      //�������϶���//�ŏ|  
    }

}

/*
//==============================================================================
//  ��������   : GetRIStatus
//  ��������   : ��ȡң����Ϣ�еĵ���״̬
//  �������   : RIIndex:ң����Ϣ���е�ȫ�������ţ������constant.h��RMT_INFO����
//  �������   ��<��>
//  ����ֵ     : 1��0
//  ����˵��   : 
//  ����       �� 
//==============================================================================
unsigned int GetRIStatus( unsigned int RIIndex )
{
    if( g_gRmtInfo[(RIIndex >> 4)] & BIT[(RIIndex & 0x0F)])
        return 1;
    else
        return 0;
}

//==============================================================================
//  ��������   : SetRIStatus
//  ��������   : ����ң����Ϣ�еĵ���״̬������ʵ��д��1����0
//  �������   : RIIndex:ң����Ϣ���е�ȫ�������ţ������constant.h��RMT_INFO����
//                         : Status: д��״̬��1����0
//  �������   : <��>
//  ����ֵ     : 
//  ����˵��   : 
//  ����       �� 
//==============================================================================

void SetRIStatus( unsigned int RIIndex , unsigned int Status )
{
    if( Status > 0 )
    {
        //��״̬��Ϊ�ߣ�����л����
        g_gRmtInfo[(RIIndex >> 4)] |=  BIT[(RIIndex & 0x0F)];
    }
    else
    {
        //��״̬Ϊ�ͣ�������ȡ����Ȼ����������
        g_gRmtInfo[(RIIndex >> 4)] &= ~BIT[(RIIndex & 0x0F)];
    }
}

//==============================================================================
//  ��������   : ScanOut
//  ��������   : �����жϺ����
//  �������   : g_ucActFlag[i] �������ͣ�����ñ�־λON������Ҫ���ڶ�Ӧ
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : ��AD�ж��б����ã�������ڿ���
//  ����       ��
//==============================================================================
void ScanOut(void)
{

}*/
/******************************************
*		Name     : void sign_repeat(void)
*   Function : repeat the 8 pulse
*   Para     : uint8 repeat_flag ,uint8 rev_flag      ////////��һ������������ʾ�Ƿ���λ��ת
*   Return   : 
******************************************/ 
//==============================================================================
//  ��������   : Sign_Repeat
//  ��������   : ���·����źſ��ƽӴ�������
//  �������   : repeat_flag �ط�����   rev_flag ��ת��λ��־
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void Sign_Repeat(unsigned char repeat_flag,unsigned char rev_flag)
{

    if(repeat_flag==0)       ////////�ط�
    {
        if((efslatch_flag==0)&&(fault_save>0))     ////////�����Ժ�����ط�
        {
            chongfa=1;
            eight_delay_counter=0;	
            pulse_phase_flag=0;
          
            if(g_gProcCnt[PC_EFS_MODEL]==0)    //////I���ź�Դ
            {
                pulse_phase_flag=1;
		  eight_pulse_flag =1;               
            } 
            else                      ///////////II���ź�Դ
            {
                eight_pulse_flag =1; 
                pulse_phase_flag=repeat_flag1;                
	     }	
	     g_TQBSCounter = 2;//Ͷ�С�����ָʾ�Ƽ�����  =0 ���� =0x55 ��������  >=1Ͷ����˸	
	     g_gRmtInfo[YX_EFS_ACT] = 1;   //Ͷ��״̬ ң����1 
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
  				unsigned long ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;//flash��ַ  
  				g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ 	
  				g_sRecData.m_ucActRecStart = ON;//�ŏ|¼�� ����¼����ʼ	
  				g_sRecData.m_LuboType = LuboType_ACT;
				
			g_gRmtInfo[YX_8FULS_STA]=1;//SaveLOG(LOG_8FULS_STA,1);
	     	}
	}   
    }
    else                    ////////ģ�����
    {
        if(g_gOverLoadFlg != ON)
	    efslatch_flag=0;
	chongfa=1;  
	if(rev_flag==0)          /////////˵�����ǵڶ��η��͵Ĳ���
	    eight_select=0x8F;//eight_select|=BIT7;                    ////////////���״η���8����ı�־	 
        g_gRmtInfo[YX_MANUAL_ACTION] = 1;   //���ֶ�Ͷ��
        eight_delay_counter=200;
	//g_sRecData.m_ucActRecStart = ON;//�ŏ|¼�� ����¼����ʼ//¼��Ҫ�ڼ̵���������ǰ0��5��	
        if(g_gProcCnt[PC_EFS_MODEL]==0)    //////I���ź�Դ
        {
            pulse_phase_flag=1;
            //delayms(2000);
            //eight_pulse_flag =1;
            moniguzhang = 1;	
        }
        else                       ///////II���ź�Դ
        {
        	pulse_phase_flag = 3;
            switch(repeat_flag)    //ģ�����
            {
                case 1://a�෢������
                      pulse_phase_flag = 3;
                      break;
                case 2://B�෢������
                      pulse_phase_flag = 1;
                      break;
                case 3://C�෢������
                      if((g_gProcCnt[PC_JAG_ACT] == 3) ||(g_gProcCnt[PC_JAG_ACT] == 4) )   //��������=3��4��û��B�࣬A�ද��
                          pulse_phase_flag=1;
                      else
                          pulse_phase_flag = 2;
                      break;  
            }
			/*
			unsigned long ulAddr = (unsigned long)(FADDR_RECORDER_DATA+g_sRecData.m_gRecCNum*4)<<16;//flash��ַ  
  			g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ 	
  			g_sRecData.m_unRecAcTail =0; 
	    	 	g_sRecData.m_ucActRecStart = ON;//�ŏ|¼�� ����¼����ʼ	
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

    if(repeat_flag==0)       ////////�ط�
    {
        if((efslatch_flag==0)&&(fault_save>0))     ////////�����Ժ�����ط�
        {
            chongfa=0;
            eight_delay_counter=0;	
            pulse_phase_flag=0;
          
            if(g_gProcCnt[PC_EFS_MODEL]==0)    //////I���ź�Դ
            {
                pulse_phase_flag=1;
                chongfa=1;
                eight_delay_counter=200;          	
                if(rev_flag==0)
          	    repeat_flag1=4;       ///////I�͵��ź�Դ���̶�����A��
            } 
            else                      ///////////II���ź�Դ
       	    {
                if(g_ucEarthFlg == 1)  //A�෢������ ,������C��
                    pulse_phase_flag = g_ucPhase_Chongfa;
                else if(g_ucEarthFlg == 2)  //B�෢������ ,������C��
                    pulse_phase_flag = g_ucPhase_Chongfa;
                else if(g_ucEarthFlg == 3)  //C�෢������ ,������A��
                    pulse_phase_flag = g_ucPhase_Chongfa; 
          	chongfa=1;
                eight_delay_counter=200;	  
                if(g_gRunPara[RP_JAG_ACT] == 3)
          	    repeat_flag1=g_ucEarthFlg;       ///////II�͵��ź�Դ��C�෢������ 
                g_ucPhase_Chongfa = 0;
	    }	
	}   
    }
    else                    ////////ģ�����
    {
        if(g_gOverLoadFlg != ON)
	    efslatch_flag=0;
	  
	if(rev_flag==0)          /////////˵�����ǵڶ��η��͵Ĳ���
	    eight_select=0x8F;//eight_select|=BIT7;                    ////////////���״η���8����ı�־	 
        chongfa=1;
        eight_delay_counter=200;	
        if(g_gProcCnt[PC_EFS_MODEL]==0)    //////I���ź�Դ
        {
            pulse_phase_flag=1;
            if(rev_flag==0)
                repeat_flag1=4;       ///////I�͵��ź�Դ���̶�����A��
        }
        else                       ///////II���ź�Դ
        {
            if(rev_flag==0)    //ģ�����
            {
                g_ucEarthFlg = repeat_flag;
                if(g_gRunPara[RP_JAG_ACT] <= 3)
                    repeat_flag1=repeat_flag;       ///////II�͵��ź�Դ��
                if(g_ucEarthFlg == 1)  //A�෢������ ,����C��
                {
                    pulse_phase_flag = 3;
                    if(g_gRunPara[RP_JAG_ACT] == 2)    //�����ͺ���
                        pulse_phase_flag=2;
                }
                else if(g_ucEarthFlg == 2)  //B�෢������ ,����A��
                {
                    pulse_phase_flag = 1;
                    if(g_gRunPara[RP_JAG_ACT] == 2)    //�����ͺ���
                        pulse_phase_flag=3;
                }
                else if(g_ucEarthFlg == 3)  //C�෢������ ,����B��
                {
                    pulse_phase_flag = 1;
                    if(g_gRunPara[RP_JAG_ACT] == 2)    //�����ͺ���
                        pulse_phase_flag=1;
                }
            }
           // else    //��λ��ת
            {
                if(repeat_flag1 == 1 && pulse_phase_flag == 3)  //A�෢������ ,������C��
                    pulse_phase_flag = 1;
                
                else if(repeat_flag1 == 2 && pulse_phase_flag == 3)  //B�෢������ ,������C��
                    pulse_phase_flag = 1;
                else if(repeat_flag1 == 2 && pulse_phase_flag == 1)  //B�෢������ ,������A��
                    pulse_phase_flag = 3;
                else if(repeat_flag1 == 3 && pulse_phase_flag == 1)  //C�෢������ ,������A��
                    pulse_phase_flag = 3;
                
            }//
          
        }   	         
    }
}
*/