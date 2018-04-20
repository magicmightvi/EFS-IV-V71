/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  ģ����     : ��ʱ����������
  �ļ���     : DriverTimer.c
  ����ļ�       : DataStruct.h FuncDef.h Constant.h
  ʵ�ֹ���       : ��ɶԶ�ʱ��TimerA0��TimerA1��TimerB�ĳ�ʼ�������������1ms��0.625ms��0.125ms���жϴ�������
           �����жϴ�������PWM�������ں����ȡ���������GPS��ʱ��ʱ��оƬ������Ĳ����жϴ�������Ĳ���
  ����       : ����һ
  �汾       : 1.0
  
  --------------------------------------------------------------------------------
  ��ע       : -
  --------------------------------------------------------------------------------
  
  �޸ļ�¼       : 
    ��  ��      �汾        �޸���      �޸����� 
    2010/06/01      1.0         ����һ        ����

************************************************************************************/

#ifndef _DRIVER_TIMER

#define _DRIVER_TIMER
  #include "..\DataStruct.h"
#endif

unsigned char rep17,rep33,rep49;

//==============================================================================
//  ��������   : InitTimer
//  ��������   : ��ɶԶ�ʱ��TimerA0��TimerA1��TimerB��RTCTimer�ĳ�ʼ������������ADת��/1ms�ж�/�����ж�/PWM��������̵���
//                               TimerA0��ΪADת����ʱ��
//                               RTCTimer��Ϊ��ʱ��������1ms��ʱ���ж�
//                               TimerA1��Ϊ1s���岶��Ķ�ʱ��
//                               TimerB0��Ϊ�̵���������PWM����Ķ�ʱ��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����     ��
//==============================================================================
void InitTimer(void)
{    
    TA0CCTL0 = OUTMOD_4;                  //���ģʽ4 50%ռ�ձ� 
  
    //TA0CCR0 = 312; 			    //�������156.25uS �ŏ|�����ж�Ƕ�� 
    //TA0CCR0 = 1250;                    // �������1.25ms=2500=16��ÿ�ܲ�//1.6khz
     //TA0CCR0 = 2500;                   //0.625ms=1250=32��ÿ�ܲ� 800hz

    TA0CCR0 = 500; 			    //�������250uS ÿ�ܲ�80�� 4000hz    
    TA0CTL = TASSEL_1 + ID_3 + MC_1 + TACLR  ;         //TMA0ģʽ���� ACLKʱ��, 8��Ƶ ,������0��������  
#ifndef _TIME_AD
    TA0CCTL0 |= CCIE; //ʹ���ж� ���ʹ���ж�����AD��ѽ��仰
    ADC12CTL0 |= ADC12SC;
#endif

    P2SEL |= BIT3;                    // PPS��������
    P2DIR &= ~BIT3;
    TA1CCTL2 = CAP + SCS + CM_1 +CCIE;           //�����岶׽����
    TA1CCTL1 |= CCIE;                    //TA1CCR1�����ж�ʹ��  
    TA1CCR1 = 250;                      //�������  
    TA1CTL = TASSEL_1 + ID_3 + MC_2 + TAIE;        //TMA1ģʽ���� ACLKʱ��, 8��Ƶ ,������0����������
    TA1EX0 = TAIDEX_7;               //ʱ���ٷ�Ƶ /8  =250KHz
    TA1CTL |= TACLR;

    TB0CTL = TBSSEL_1 + ID_3 + MC_1 + TBCLR ;   //ʱ��ԴΪACLK,8��Ƶ,������0��
    
    if(g_gRunPara[RP_SENDYC_T] > 0)   //��ʱ�ϴ�ң��
#ifdef YN_101S
        g_gsendYCTimeNum = ((unsigned long)g_gRunPara[RP_SENDYC_T]*10);
#else
        g_gsendYCTimeNum = ((unsigned long)g_gRunPara[RP_SENDYC_T]);
#endif
    if(g_gRunPara[RP_SENDYX_T] > 0)   //��ʱ�ϴ�ң��
        g_gsendYXTimeNum = ((unsigned long)g_gRunPara[RP_SENDYX_T]);
    if(g_gRunPara[RP_BEAT_T] > 0)   //��ʱ����
        g_gBeatNum = ((unsigned long)g_gRunPara[RP_BEAT_T]);
    if(g_gRunPara[RP_SENDZJ_T] > 0)   //��ʱ�ϴ��Լ�
        g_gsendZJTimeNum = ((unsigned long)g_gRunPara[RP_SENDZJ_T])*10;	
}


//unsigned int TestAdcData[1][AC_BUFF_LEN];

//==============================================================================
//  ��������   : GetAcSamData
//  ��������   : ��ý����ɼ����ݣ����ѽ����ɼ������ݽ����˲�
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����     ��
//==============================================================================
void GetAcSamData(void)
{
    unsigned int Last3SamTail,Last2SamTail,LastSamTail,AcSamTail; //����Ϊ���µ�������������
    int Value1,Value2,Value3,Value4;
    int Polarity;
    int i;
    //��������˲�����Ҫ����m_unAcDataTail��Ϊ�����¼�������µ����ݣ����ɼ����ݵĴ��λ��Ϊm_unAcDataTail + AC_SAM_TAIL_OFFSET�����������ɿռ�����˲�����
    g_sSampleData.m_unAcDataTail++;     //���Ǻ�ԭ������ļ��ݣ�m_unAcDataTailΪ�����õ�������β��
    g_sSampleData.m_unAcDataTail &= 0x1F;
    AcSamTail = (g_sSampleData.m_unAcDataTail + AC_SAM_TAIL_OFFSET) & 0x1F; //���²ɼ����ݱ����λ�ã���������˲�����
    
    //���������˲��ķ������в���ֵ���˲�
    LastSamTail = (AcSamTail - 1)  & 0x1F;  //���²ɼ����ݴ��µ�
    Last2SamTail = (AcSamTail - 2)  & 0x1F; //���²ɼ����ݴ��µ�
    Last3SamTail = (AcSamTail - 3)  & 0x1F; //���²ɼ����ݴ��µ�
      
    //����ִ��Ч�ʣ�û����for����������
    for(i = 0; i < 6; i++)  //Ϊ���Բ������ѹ�ļ��㣬�ѽ��������ļ���˳Ѱ�ߵ�
    {    
            Polarity = g_unAdcData[i];        
        if(Polarity < 4097)
        {
            if(i == CHAN_U0 && g_gProcCnt[PC_U0_CAL] == 0)  //
                g_sSampleData.m_gAcAdcData[i][AcSamTail] = g_sSampleData.m_gAcAdcData[i - 1][AcSamTail] + g_sSampleData.m_gAcAdcData[i - 2][AcSamTail] + g_sSampleData.m_gAcAdcData[i - 3][AcSamTail]; 
            else
                g_sSampleData.m_gAcAdcData[i][AcSamTail] = (int)(((long)Polarity - g_gAdjPara[i]) * g_gProcCnt[PC_UA_ADJ + i] >> 12);    //* g_gAdjPara[ADJ_DEST_CHNIA + i] >> 10����һ�ν�����ѹ�����ɼ����ѽ�����ѹ�����ɼ����ݱ��浽g_sSampleData.m_gAcAdcData
           if(i==CHAN_Upt)//upt to uo//Uo to Upt
		  g_sSampleData.m_gAcAdcData[i][AcSamTail] = (int)(((long)Polarity - g_gAdjPara[3]) * g_gProcCnt[PC_UA_ADJ + 3] >> 12);    //* g_gAdjPara[ADJ_DEST_CHNIA + i] >> 10����һ�ν�����ѹ�����ɼ����ѽ�����ѹ�����ɼ����ݱ��浽g_sSampleData.m_gAcAdcData
		  
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
            else if(Value2 >= 0 && Value3 < 0 )    //��������ļ�����
            {
                if(phase_check_flag==0x55)     //////////��־�Ѿ����ã�׼�������λ
    	        {
    	            if((eight_pulse_flag ==0))   /////////û�н���8����
    	 	    {
                        if((g_gRmtMeas[RM_UA]>4000)&&(g_gRmtMeas[RM_UB]>4000)&&(g_gRmtMeas[RM_UC]>4000))     ////////�ٴμ��Ϊ����40�Ž��м���
      	     	        {
    	 	  	    
    	 	  	    phase_seq_flag=0x55;          /////////������Ϊ������
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
                        
                                    if(g_gucPhaseSQU[0] < g_gucPhaseSQU[1])     //////////ABC��CAB
          	     	            {
          	     	                if((g_gucPhaseSQU[1]<=g_gucPhaseSQU[2])||((g_gucPhaseSQU[2]<=g_gucPhaseSQU[1])&&(g_gucPhaseSQU[2]<=g_gucPhaseSQU[0])))                  ////////B��϶����A����һ������C���п��ܻᳬǰA���B��
          	     	 	            phase_seq_flag=0;                          /////////����Ϊ������
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
            if(Value1 >= 800 && Value2 >= 800 && Polarity == 3)//����������ݱ仯�������Ҽ����෴
            {
                g_sSampleData.m_gAcAdcData[i][LastSamTail] = g_sSampleData.m_gAcAdcData[i][Last2SamTail];   //����һ��ֵ����
               // SetRIStatus(FLT_NC17 + (i << 1), 1);
            }
            else if(Value1 >= 500 && Value2 <= 100 && Value3 >=500 && Polarity == 3)
            {
                g_sSampleData.m_gAcAdcData[i][LastSamTail] = g_sSampleData.m_gAcAdcData[i][Last3SamTail];   //����һ��ֵ����
                g_sSampleData.m_gAcAdcData[i][Last2SamTail] = g_sSampleData.m_gAcAdcData[i][Last3SamTail];  //����һ��ֵ����
              //  SetRIStatus(FLT_NC17 + (i << 1), 1);
            }

        }
        else//��������쳣������һ�����ֵ
        {
            g_sSampleData.m_gAcAdcData[i][AcSamTail] = g_sSampleData.m_gAcAdcData[i][LastSamTail];
            //SetRIStatus(FLT_NC18 + (i << 1), 1);
        }
    }

}

//==============================================================================
//  ��������   : CHECK8PLUS
//  ��������   : ����8�����壬��Ҫ��������Ƿ���Ч���Ͷ���
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����     ���ŏ|
//==============================================================================

void CHECK8PLUS(void)
{
//����Ӳң��
    if(g_MaichongNum >= 6)   //�״ζ�����Ч
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
     	KC1_ON;             ///�µ�·�� 
     	
	sign_RSSI_delay=5;                     //////////////������Ч8���������ʱ��������ʱʱ��������վѯ�ʣ�����Ϊ�ǹ���    
	//if(moniguzhang!=1) //=1Ϊģ����� �����Ͷ���
	newsms_8pluse = ON;
 	//CreatNewSMS(EIGHT_PULSE);                      //����8������� //�ŏ|//20161217
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
//  ��������   : JAGACTx
//  ��������   : ��������ֱ�Ϊ1,2,3,4
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����     ���ŏ|
//==============================================================================

void JAGACT1(void)//����2�� ��ǰ�ද�����ͺ��ද��
{
  //unsigned int unTemp = 0;
  if((eight_pulse_counter==0)&&(efslatch_flag==0))
  {	 	   
    if((eight_pulse_flag>0)&&(eight_pulse_flag<rep33))
    {
      eight_pulse_flag+=1;
      if(g_gOverLoadFlg == ON)
      {
        eight_pulse_flag = rep17;  //��⵽����ֱ���˳� 
      }
      if((eight_pulse_flag%2)==0)  ///////������
      {
        eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_MODFK];
        //g_sRecData.m_ucRecStart = ON;
        if(pulse_phase_flag==1)    ///////A��
        {g_gKONBK=1;
          KB0_OFF;
          KC0_OFF;
          KA0_ON;
          g_sRecData.m_ucRecStart = ON;
	   g_gKON=1;	 
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_AMODFK];
        }
        else if(pulse_phase_flag==2)  ///////B��
        {	g_gKONBK=2;
          KC0_OFF;
          KA0_OFF;
          KB0_ON;
	   g_gKON=2; 
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_BMODFK];
        }
        else if(pulse_phase_flag==3)  //////C��
        {g_gKONBK=3;
          KA0_OFF;
          KB0_OFF;
          KC0_ON;
	   g_gKON=3;		  
          g_sRecData.m_ucRecStart = ON;
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_CMODFK];	  
        }
        if(pulse_phase_flag != 2 ) //B�಻¼��
        {
        }
      }	
      else if((eight_pulse_flag%4)==3)  ///////������ 1S
      {
        eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
        if(pulse_phase_flag==1)    ///////A��
        {
          KA0_OFF;
          g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];	  
        }
        else if(pulse_phase_flag==2)  ///////B��
        {
          KB0_OFF;
          g_gKON=OFF;	
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];	  
        }
        else if(pulse_phase_flag==3)  //////C��
        {
          KC0_OFF; 
          g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_CMODFK];	  
        }
      }	
      else if((eight_pulse_flag%4)==1)  ///////������ 1.25S      2014.8.5�޸�Ϊ1.5S
      {
        eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
        if(pulse_phase_flag==1)    ///////A��
        {
          KA0_OFF;
          g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];	  
          //g_gRmtInfo[0] &= ~YX_A_ACTION;
        }
        else if(pulse_phase_flag==2)  ///////B��
        {
          KB0_OFF;          
          g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];	  
          // g_gRmtInfo[0] &= ~YX_B_ACTION;
        }
        else if(pulse_phase_flag==3)  //////C��
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
  if(eight_pulse_flag>=rep33)      ////////���ݷ������
  {
    CHECK8PLUS();
    eight_pulse_flag=0;
    pulse_phase_flag=0;      	
    efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];	        ///////����17����  
    latch_upload_flag=0x55;
    uart0_event_flag=0;         ///////��������0����Ϊ����״̬��������ʾ
    g_gRmtInfo[YX_EFS_LATCH] = 1;   //�ñ���ң��λ    
    SaveLOG(LOG_LATCH, 1);
    g_gRmtInfo[YX_EFS_ACT] = 0;   //Ͷ��״̬ ң����0
    chongfa=0;  moniguzhang=0;	
    g_gRmtMeas[RM_ACT_NUM] = 2;
   g_sRecData.m_gACTDelay = 200;//g_sRecData.m_ucActRecStart = OFF;//�ŏ|¼�� ����¼������
   //g_gKONBK=OFF;
  }
}
void JAGACT2(void)//����3�� ��ǰ�ද�����ͺ��ද���������ද��
{
  //unsigned int unTemp = 0;
  if((eight_pulse_counter==0)&&(efslatch_flag==0))
  {	 	   
    if((eight_pulse_flag>0)&&(eight_pulse_flag<rep49))
    {
      eight_pulse_flag+=1;
      if(g_gOverLoadFlg == ON)
      {
        eight_pulse_flag = rep17;  //��⵽����ֱ���˳� 
      }
      if((eight_pulse_flag%2)==0)  ///////������
      {
        eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_MODFK];
        //g_sRecData.m_ucRecStart = ON;
        if(pulse_phase_flag==1)    ///////A��
        {g_gKONBK=1;
          KB0_OFF;
          KC0_OFF;
          KA0_ON;
          g_gKON=1;		  
          g_sRecData.m_ucRecStart = ON;
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_AMODFK];	  
        }
        else if(pulse_phase_flag==2)  ///////B��
        {	g_gKONBK=2;
          KC0_OFF;
          KA0_OFF;
          KB0_ON;
          g_gKON=2;	 
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_BMODFK];	  
        }
        else if(pulse_phase_flag==3)  //////C��
        {g_gKONBK=3;
          KA0_OFF;
          KB0_OFF;
          KC0_ON;
          g_gKON=3;		  
          g_sRecData.m_ucRecStart = ON;
	  eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_CMODFK];	  
        }
        if(pulse_phase_flag != 2 ) //B�಻¼��
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
      else if((eight_pulse_flag%4)==3)  ///////������ 1S
      {
        eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
        if(pulse_phase_flag==1)    ///////A��
        {
          KA0_OFF;  g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];
        }
        else if(pulse_phase_flag==2)  ///////B��
        {
          KB0_OFF;  g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];
        }
        else if(pulse_phase_flag==3)  //////C��
        {
          KC0_OFF;  g_gKON=OFF; 
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_CMODFK];	  
        }
      }	
      else if((eight_pulse_flag%4)==1)  ///////������ 1.25S      2014.8.5�޸�Ϊ1.5S
      {
        eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
        if(pulse_phase_flag==1)    ///////A��
        {
          KA0_OFF;  g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];	  
          //g_gRmtInfo[0] &= ~YX_A_ACTION;
        }
        else if(pulse_phase_flag==2)  ///////B��
        {
          KB0_OFF;  g_gKON=OFF;
	   eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];	  
          // g_gRmtInfo[0] &= ~YX_B_ACTION;
        }
        else if(pulse_phase_flag==3)  //////C��
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
 if(eight_pulse_flag>=rep49)      ////////���ݷ������
  {
    CHECK8PLUS();
    eight_pulse_flag=0;
    pulse_phase_flag=0;    	
    //if(moniguzhang!=1) //=1Ϊģ����� ������	
    	efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////����17����
    	latch_upload_flag=0x55;
    //else 
    	{
    	 //g_gRmtInfo[0] &= ~YX_ACTION_SUCCESS;
	 //g_gRmtInfo[0] &= ~YX_PHASEA_ACT;	
	 //g_gRmtInfo[0] &= ~YX_PHASEB_ACT;
	 //g_gRmtInfo[0] &= ~YX_PHASEC_ACT;
    	}    	
    uart0_event_flag=0;         ///////��������0����Ϊ����״̬��������ʾ
    g_gRmtInfo[YX_EFS_LATCH] = 1;   //�ñ���ң��λ  
    SaveLOG(LOG_LATCH, 1);
    g_gRmtInfo[YX_EFS_ACT] = 0;   //Ͷ��״̬ ң����0    
    chongfa=0;moniguzhang=0;	
   g_gRmtMeas[RM_ACT_NUM] = 3;
   g_sRecData.m_gACTDelay = 200;//g_sRecData.m_ucActRecStart = OFF;//�ŏ|¼�� ����¼������	
   //g_gKONBK=OFF;
  }
}
void JAGACT3(void)//����2�� ֻ��AC���нӴ�������ǰ�ද������һ�ද��
{
  //unsigned int unTemp = 0;
  if((eight_pulse_counter==0)&&(efslatch_flag==0))
  {	 	   
    if((eight_pulse_flag>0)&&(eight_pulse_flag<rep33))
    {
      eight_pulse_flag+=1;
      if(g_gOverLoadFlg == ON)
      {
        eight_pulse_flag = rep17;  //��⵽����ֱ���˳� 
      }
      if((eight_pulse_flag%2)==0)  ///////������
      {
        eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_MODFK];
        //g_sRecData.m_ucRecStart = ON;
        if(pulse_phase_flag==1)    ///////A��
        {g_gKONBK=1;
          KB0_OFF;
          KC0_OFF;
          KA0_ON;  g_gKON=1;
          g_sRecData.m_ucRecStart = ON;
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_AMODFK];	  
        }
        else if(pulse_phase_flag==2)  ///////B��
        {g_gKONBK=2;
          KC0_OFF;
          KA0_OFF;
          KB0_ON;  g_gKON=2;
	   eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_BMODFK];	  
        }
        else if(pulse_phase_flag==3)  //////C��
        {g_gKONBK=3;
          KA0_OFF;
          KB0_OFF;
          KC0_ON;  g_gKON=3;
          g_sRecData.m_ucRecStart = ON;
	  eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_CMODFK];	  
        }
        if(pulse_phase_flag != 2 ) //B�಻¼��
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
      else if((eight_pulse_flag%4)==3)  ///////������ 1S
      {
        eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
        if(pulse_phase_flag==1)    ///////A��
        {
          KA0_OFF;  g_gKON=OFF;
	eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];	  
        }
        else if(pulse_phase_flag==2)  ///////B��
        {
          KB0_OFF;  g_gKON=OFF;
	eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];	  
        }
        else if(pulse_phase_flag==3)  //////C��
        {
          KC0_OFF;   g_gKON=OFF;
	eight_pulse_counter= g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_CMODFK];	  
        }
      }	
      else if((eight_pulse_flag%4)==1)  ///////������ 1.25S      2014.8.5�޸�Ϊ1.5S
      {
        eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
        if(pulse_phase_flag==1)    ///////A��
        {
          KA0_OFF;  g_gKON=OFF;
	eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];	  
          //g_gRmtInfo[0] &= ~YX_A_ACTION;
        }
        else if(pulse_phase_flag==2)  ///////B��
        {
          KB0_OFF;  g_gKON=OFF;
	eight_pulse_counter= g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];	  
          // g_gRmtInfo[0] &= ~YX_B_ACTION;
        }
        else if(pulse_phase_flag==3)  //////C��
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
  if(eight_pulse_flag>=rep33)      ////////���ݷ������
  {
    CHECK8PLUS();
    eight_pulse_flag=0;
    pulse_phase_flag=0;    	
    //if(moniguzhang!=1) //=1Ϊģ����� ������	
    	efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////����17����    	
    	latch_upload_flag=0x55;
    //else 
    	{
    	 //g_gRmtInfo[0] &= ~YX_ACTION_SUCCESS;
	 //g_gRmtInfo[0] &= ~YX_PHASEA_ACT;	
	 //g_gRmtInfo[0] &= ~YX_PHASEB_ACT;
	 //g_gRmtInfo[0] &= ~YX_PHASEC_ACT;
    	}    	
    uart0_event_flag=0;         ///////��������0����Ϊ����״̬��������ʾ
    g_gRmtInfo[YX_EFS_LATCH] = 1;   //�ñ���ң��λ  
    SaveLOG(LOG_LATCH, 1);
    g_gRmtInfo[YX_EFS_ACT] = 0;   //Ͷ��״̬ ң����0    
    chongfa=0;	moniguzhang=0;
    g_gRmtMeas[RM_ACT_NUM] = 2;
   g_sRecData.m_gACTDelay = 200;//g_sRecData.m_ucActRecStart = OFF;//�ŏ|¼�� ����¼������	
   //g_gKONBK=OFF;
  }
}

void JAGACT4(void)//����1�� ֻ��AC���нӴ�������ǰ�ද��
{
    //unsigned int unTemp = 0;
        if((eight_pulse_counter==0)&&(efslatch_flag==0))
        {	 	   
            if((eight_pulse_flag>0)&&(eight_pulse_flag<rep17))
            {
                eight_pulse_flag+=1;
                if(g_gOverLoadFlg == ON)
                {
                    eight_pulse_flag = rep17;  //��⵽����ֱ���˳� 
                }

                 if((eight_pulse_flag%2)==0)  ///////������
                 {
                     eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_MODFK];
                    // g_sRecData.m_ucRecStart = ON;
                     if(pulse_phase_flag==1)    ///////A��
                    {g_gKONBK=1;
                        KB0_OFF;
                        KC0_OFF;
                        KA0_ON;  g_gKON=1;
                        g_sRecData.m_ucRecStart = ON;
			eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_AMODFK];			
                  //      g_gRmtInfo[0] |= YX_A_ACTION;
                    }
                    else if(pulse_phase_flag==2)  ///////B��
                    {g_gKONBK=2;
                        KC0_OFF;
                        KA0_OFF;
                        KB0_ON;  g_gKON=2;
			eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_BMODFK];			
                   //     g_gRmtInfo[0] |= YX_B_ACTION;
                    }
                    else if(pulse_phase_flag==3)  //////C��
                    {g_gKONBK=3;
                        KA0_OFF;
                        KB0_OFF;
                        KC0_ON;  g_gKON=3;
                        g_sRecData.m_ucRecStart = ON;
			eight_pulse_counter= g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_CMODFK];			
                    //    g_gRmtInfo[0] |= YX_C_ACTION;
                    }
                    if(pulse_phase_flag != 2 ) //B�಻¼��
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
                 else if((eight_pulse_flag%4)==3)  ///////������ 1S
                 {
                     eight_pulse_counter=g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
                     if(pulse_phase_flag==1)    ///////A��
                    {
                          KA0_OFF;  g_gKON=OFF;
			eight_pulse_counter=g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];			  
                     //     g_gRmtInfo[0] &= ~YX_A_ACTION;
                    }
                    else if(pulse_phase_flag==2)  ///////B��
                    {
                          KB0_OFF;  g_gKON=OFF;
			eight_pulse_counter=g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];			  
                       //   g_gRmtInfo[0] &= ~YX_B_ACTION;
                    }
                    else if(pulse_phase_flag==3)  //////C��
                    {
                          KC0_OFF;   g_gKON=OFF;
			eight_pulse_counter=g_gProcCnt[PC_SEND_TIME1] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_CMODFK];			  
                       //   g_gRmtInfo[0] &= ~YX_C_ACTION;
                    }   
                 }
                 else if((eight_pulse_flag%4)==1)  ///////������ 1.25S
                 {
                     eight_pulse_counter=g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_MODFK];
                     if(pulse_phase_flag==1)    ///////A��
                    {
                          KA0_OFF;  g_gKON=OFF;
			eight_pulse_counter=g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_AMODFK];			  
                     //     g_gRmtInfo[0] &= ~YX_A_ACTION;
                    }
                    else if(pulse_phase_flag==2)  ///////B��
                    {
                          KB0_OFF;  g_gKON=OFF;
			eight_pulse_counter=g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_BMODFK];			  
                        //  g_gRmtInfo[0] &= ~YX_B_ACTION;
                    }
                    else if(pulse_phase_flag==3)  //////C��
                    {
                          KC0_OFF;   g_gKON=OFF;
			eight_pulse_counter=g_gProcCnt[PC_SEND_TIME2] - g_gProcCnt[PC_PLUSE_TIME]+g_gRunPara[RP_PLUSE_CMODFK];			  
                       //   g_gRmtInfo[0] &= ~YX_C_ACTION;
                    }   
                 }
              	
             } 	
         }      
         if(eight_pulse_flag>=rep17)      ////////���ݷ������
         {
             CHECK8PLUS();	
             eight_pulse_flag=0;
             pulse_phase_flag=0;
    				 
             //if(moniguzhang!=1) //=1Ϊģ����� ������			 
             	  efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////����17����
             	  latch_upload_flag=0x55;
    		//else 
    		{
    	 		//g_gRmtInfo[0] &= ~YX_ACTION_SUCCESS;
	 		//g_gRmtInfo[0] &= ~YX_PHASEA_ACT;	
	 		//g_gRmtInfo[0] &= ~YX_PHASEB_ACT;
	 		//g_gRmtInfo[0] &= ~YX_PHASEC_ACT;
    		}             	  
             uart0_event_flag=0;         ///////��������0����Ϊ����״̬��������ʾ
             g_gRmtInfo[YX_EFS_LATCH] = 1;   //�ñ���ң��λ 
             SaveLOG(LOG_LATCH, 1);
             g_gRmtInfo[YX_EFS_ACT] = 0;   //Ͷ��״̬ ң����0             
	      chongfa=0;moniguzhang=0;
             g_gRmtMeas[RM_ACT_NUM] = 1;
	      g_sRecData.m_gACTDelay = 200;//g_sRecData.m_ucActRecStart = OFF;//�ŏ|¼�� ����¼������	
	      //g_gKONBK=OFF;
         }
}

//==============================================================================
//  ��������   : ContronlRelay
//  ��������   : �Ӵ�������ģ�飬��Ϊ���ֿ��Ʒ�ʽ������Ͷ����3��Ͷ��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����     ��
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
	
    if((g_gProcCnt[PC_JAG_ACT] == 4)||(g_gProcCnt[PC_EFS_MODEL]==0))          //����2�� ֻ��AC���нӴ�������ǰ�ද������һ�ද��
        JAGACT4();
    else if(g_gProcCnt[PC_JAG_ACT] == 3)  //����1�� ֻ��AC���нӴ�������ǰ�ද��
        JAGACT3();
    else if(g_gProcCnt[PC_JAG_ACT] == 1)  //����2�� ��ǰ���ͺ�
        JAGACT1();
    else if(g_gProcCnt[PC_JAG_ACT] == 2)  //����3�� ��ǰ���ͺ󡢹���
        JAGACT2();


}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
   // unsigned int i,j;
      static unsigned char M125SecCount = 0;  // 1.25�����ʱ // �ŏ|�����ж�Ƕ��
	static unsigned char pjno=0;
	static unsigned char McSecCount=0;
 //#ifdef SD_101S     
      static unsigned char M05SecCount = 0;  // 0.5�����ʱ // �ŏ|�����ж�Ƕ��
 //#endif     
  //static unsigned char ucTime = 0;
    
   // WDTCTL = WDTPW + WDTCNTCL;

    ADC12IFG = 0x3FF;   //�����ж��쳣
    M125SecCount++;// �ŏ|�����ж�Ƕ�� 
    
    g_unAdcData[0] = ADC12MEM0; //UAΪ�˱�֤ADC12������ͬ�����ұ�֤������ADC12MEM�������Ѿ�ȡ�����������˻�����
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

    if(M125SecCount>4)//ÿ5�β���5*250uS=1.25ms �����洢����// �ŏ|�����ж�Ƕ��
    	{// �ŏ|�����ж�Ƕ��
        GetAcSamData();//�ѽ������ݾ�������˲��󣬴�ŵ�g_sSampleData�� 
        M125SecCount = 0;
        //CalcuProtMeas();//�ŏ|ȡ���˳���//���þ������㷨���㣬�Ѽ��������浽����������  
        ProtStart();//����Ԫ���ж�
        //ProtLogic();//�ŏ| �������CalcuRmtMeas(void) 5msִ��1��       
        RecActData();
		McSecCount++;	
		if(McSecCount>=16)//ÿ32 ����32*56.25uS=625us �����洢����// �ŏ|�����ж�Ƕ��
    		{	
		 	g_unRmCaluFlag = ON; 
		 	McSecCount = 0;
    		}		
    	}
#ifndef SD_101S	
    RecData();
#endif	

    if((M05SecCount&0x7f)==0)//ÿ32 ����32*56.25uS=625us �����洢����// �ŏ|�����ж�Ƕ��
    	{	 
	 	g_gRmtMeasPJ[0][pjno]=g_gRmtMeas[1];
	 	g_gRmtMeasPJ[1][pjno]=g_gRmtMeas[2];
		g_gRmtMeasPJ[2][pjno]=g_gRmtMeas[3];
       	pjno++;
		if(pjno>31)pjno=0;;	
    	}

        //ɨ�迪��������
       /*ScanDin();

        {
 
            ProtStart();//����Ԫ���ж�
            //����Ԫ���жϣ�����ʱ���㣬������������������ֹ0.625ms�жϣ���ֹ1ms�ж�(���ݵ�ǰmsʱ�̵�λ�ü����´εļ�ʱ������ms���)
            ProtLogic();
              
        }

        ScanOut();      //��ɳ��ڶ������жϺ�����������ң�صĺϷ�բҲ���ڴ�ѭ��������ر�־�����ж���������
*/
  
}

	
// Timer A1 interrupt service routine


#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)    //�����жϺ���
{
    unsigned char i = 0;
    //unsigned int unTemp = 0;
    
    static unsigned int MicSecCount = 0;  //10�����ʱ
    static unsigned int Mic50SecCount = 0;  //50�����ʱ//�ŏ| ң��Խ��	
    static unsigned int SecCount = 0;     //���ʱ
    static unsigned char NumKON = 0;     //8����ʱ ÿ�������10ms����
    static unsigned char Numyc = 0;     //8����ң��ֵ yc[] �������±�
    //static unsigned char Numk=0;     //8����ң��ֵ yc[] �������±�
    switch(TA1IV)
    {
         case 0x02:
//#ifdef INT_NEST  			 	
//UCA1IE &= (~(UCRXIE + UCTXIE));              //�ر� USCI_A1RX �ж�  // �ŏ|�����ж�Ƕ��	 
//UCA2IE &= (~(UCRXIE + UCTXIE));              // �ر� USCI_A2 RX �ж�  // �ŏ|�����ж�Ƕ��// �ŏ|�����ж�Ƕ��	
_EINT();//�����ж�// �ŏ|�����ж�Ƕ��	
//delay(2000);	// �ŏ|�����ж�Ƕ��
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
            g_sRtcManager.m_gRealTimer[RTC_MICROSEC]++;  //ϵͳʵʱʱ��g_sRtcManager.m_gRealTimer�ĺ����ۼ�
            ScanDinYX();
			ScanLOG();
            MicSecCount++;
	     Mic50SecCount++;		
            SecCount = g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
            //RmInfoChk();//�ŏ| ������ѭ��������ջ̫���޷��ж�Ƕ��
            //ScanDinYX();//�ŏ| ������ѭ��������ջ̫���޷��ж�Ƕ��
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
////8����У׼       g_gModfk[6]
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
////�ŏ|0330 ������ڳ�ʱ�䲻����IDLE״̬����λ����
	     g_ucGPRSState_TCounter++;
	     if(g_ucGPRSState_TCounter>5000)
	     	{
	     		g_ucGPRSState = GPRSState_IDLE;
			g_ucGPRSState_TCounter=0;
	     	}
//******************************************************
//LED_RUN_OFF;
            if(MicSecCount >= 10)  //10�����ʱ
            {           
             	MicSecCount = 0;
                if(eight_pulse_counter>0)
                    eight_pulse_counter--;
                if(g_ucDuanxianFlg <= 50)
                    g_ucDuanxianFlg++;
                
				ScanSoftLacth();		
				
                if((P5IN&BIT5)== 0)     /////////��⵽���磬����(�µ�·��)
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
		      			{//����¼��Ҫ�ڼ̵�������ǰ����0.5�뿪ʼ�������趨��ǰ0.8s
						unsigned long ulAddr =FADDR_RECORDER_ACTDATA+ (unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;//flash��ַ  
  						g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ 	
  						g_sRecData.m_unRecAcTail =0; 
	    	 			g_sRecData.m_ucActRecStart = ON;//�ŏ|¼�� ����¼����ʼ	
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
                ContronlRelay();       //Ͷ�нӴ���                

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
				{//8����̵����պ�50ms�󣬼�¼����ֵ
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
			else //if(g_gKON==OFF)//����δ�պ�
				{
		  		NumKON=0;		  		
		  		if(Numyc>=8)Numyc=0;
		  		if(g_I0RmtNum >= 3) //((g_gProcCnt[PC_PLUSE_TIME]-g_gRunPara[RP_PLUSE_MODFK])/2))     //��⵽��Ч����
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
			g_gRmtInfo[YX_EFS_ACT] = 0;   //Ͷ��״̬ ң����0
                        g_I0RmtZeroNum = 0;
			if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_BREAK_STOP8PUL])
				{
				//ֹͣ8����
				KA0_OFF; KB0_OFF;KC0_OFF; g_gKON=OFF;//g_gKONBK=OFF;
				eight_pulse_flag=0;
    				pulse_phase_flag=0; 
    				efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////����17����
    				latch_upload_flag=0x55;      	
    				uart0_event_flag=0;         ///////��������0����Ϊ����״̬��������ʾ
    				g_gRmtInfo[YX_EFS_LATCH] = 1;   //�ñ���ң��λ 
    				SaveLOG(LOG_LATCH, 1);
    				chongfa=0;	moniguzhang=0;
    				g_gRmtMeas[RM_ACT_NUM] = 0;
  				 g_sRecData.m_gACTDelay = 200;//g_sRecData.m_ucActRecStart = OFF;//�ŏ|¼�� ����¼������	
				}
			}

                
                if(main_reset_flag==0)
    	        {
    	            WDG_SET;                         //////���ÿ��Ź�    watch dog  hardware      
    	        }
                else if(main_reset_flag == 0x55)
                {
                    //����ϵͳ
                    WDTCTL = WDTPW+WDTIS1+WDTIS0 + WDTIS2;//�޸Ŀ��Ź������ڣ��Ӷ��ܹ���������
                    _DINT();       //�ر�ȫ���ж�
                    while(1);
                }
                if(fault_begin == 0x55 && fault_time == 10 && efslatch_flag == 0)   //100��������¼��
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
                if(fault_begin==0x55)//////�����Ѿ���ʼ
                {
                    fault_time++;
                    fault_begin=0;
                   // fault_begin_first = 0x55;
                    fault_end=0;
			g_gRmtInfo[YX_RH_SUCCESS]=0;//ȼ���ɹ�ң����λ
			rh_counter=0;
                } 
                else if(fault_end==0x55)      //////////���ϸ���   	 
                {
                    g_sRecData.m_ucFaultRecStart = OFF;
                    fault_end=0;
                    fault_begin=0;
                    if(fault_time<=5)
                        {
                        fault_time=0;
			   if(rh_send_ok==0x55)	
			   	{
			   	g_gRmtInfo[YX_RH_SUCCESS]=1;//ȼ���ɹ�ң����λ
				rh_counter=g_gRunPara[RP_RHT_DELAY];//ң����ʱ��ʼ
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
		    if((g_gRunPara[RP_RHSEND_TIME1]>0)&&(g_gRunPara[RP_RHPLUSE_TIME2]>0))//RP_RHSEND_TIME1==0 ����RP_RHPLUSE_TIME2==0���ر�ȼ������
		  	{
		  	if((fault_time>=(g_gRunPara[RP_RHSEND_TIME1]-60))&&(g_sRecData.m_ucActRecStart == CLOSE))
		  		{
				//��ʼ¼��
	    			g_test=0;
    				g_sRecData.m_unRecAcTail =0; 
          			g_sRecData.m_gFaultRecSOE[REC_MSL] =g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
              		g_sRecData.m_gFaultRecSOE[REC_MSH] = g_sRtcManager.m_gRealTimer[RTC_SEC];
           			g_sRecData.m_gFaultRecSOE[REC_MINU] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
           			g_sRecData.m_gFaultRecSOE[REC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
             			g_sRecData.m_gFaultRecSOE[REC_DAY] = g_sRtcManager.m_gRealTimer[RTC_DATE];
             			g_sRecData.m_gFaultRecSOE[REC_MONTH] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
             			g_sRecData.m_gFaultRecSOE[REC_YEAR] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000);  
  				unsigned long ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;//flash��ַ  
  				g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ 	
  				g_sRecData.m_LuboType = LuboType_XH;
  				g_sRecData.m_ucActRecStart = ON;//�ŏ|¼�� ����¼����ʼ	
  				g_sRecData.m_gXHDelay=fault_time;
		  		}
				
                	//if(fault_time>=g_gRunPara[RP_RHSEND_TIME1])
                	if(g_sRecData.m_gXHDelay>=g_gRunPara[RP_RHSEND_TIME1])
				{//��������̵���
				g_gRmtInfo[YX_RH_ACT]=1; //װ��Ϩ������ ң��
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
		  		{//�ع�����̵���
		  		g_gRmtInfo[YX_RH_ACT]=0; //װ��Ϩ������ ң��
		  		KA0_OFF;
				KB0_OFF;	
				KC0_OFF;g_gKON = OFF; //g_gKONBK=OFF;	
				rh_send_ok = 0x55;
				//g_sRecData.m_ucActRecStart = OFF;//�ŏ|¼�� ����¼������
				g_sRecData.m_gACTDelay = 30;//Ϩ����������¼300ms
				g_sRecData.m_gXHDelay=0;
		  		}
		  	}
		  }
		  /*
		  if((g_gRmtInfo[YX_EARTH_FAULT]==0)&&(rh_send_ok == 0x55))//ȼ�����巢�ͺ󣬳��ֹ��ϸ��飬˵��ȼ���ɹ�
		  {
			g_gRmtInfo[YX_RH_SUCCESS]=1;//ȼ���ɹ�ң����λ
			rh_counter=g_gRunPara[RP_RHT_DELAY];//ң����ʱ��ʼ
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
              /*  if(pulse_time_flag!=0)   ////////��8���崦��
                {
                    if(pulse_flag==0x55)         ////////��⵽8���壬�Ͳ��ٽ��е�ѹ���㣬ֻ�������������
    	            {          	      
                        i=pulse_counter*29+8;	                
                        i=i+wave_counter; 
                        if(g_gRmtMeas[RM_I0]<1200)                                   	
                            g_sSoeData.m_gRecBuff[i]= g_gRmtMeas[RM_I0]/10 + 128;      //ң��ֵ
                        else
                            g_sSoeData.m_gRecBuff[i]= g_gRmtMeas[RM_I0]/100;
                        wave_counter++; 
                    
                        if(wave_counter>=25)  //////////25���ܲ��ɼ����
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
                          g_sRecData.m_ucActRecStart = OFF;//�ŏ|¼�� ����¼������	
                          g_gKONBK=OFF;
                	}
		  //SaveActRecData();			 
            }
	     if(Mic50SecCount >= 50)   //50mS��ʱ//�ŏ| ң��Խ��	
             {
               g_gYCYueXian=0x55;//�ŏ| ң��Խ��	
               Mic50SecCount =0;
		 //if((g_sRecData.m_ucActRecStart != OFF))	   
		 SaveActRecData();	
		 ScanPT();
             }
	     		 
            if(SecCount >= 1000)   //���ʱ
            {
               g_STimeout = ON;                  
            	if(gRes_rec.res_timeout > 0)
            		{
            		//gRes_rec.res_timeout ++;
            		}
                if(g_unSms_Count>0)//���ŷ��ͺ�g_unSms_Count��15
                	{
                	g_unSms_Count--;//���ŷ��ͺ�δ�յ��ظ�g_unSms_Count--
                	if(g_unSms_Count==0)
                		{
                		g_unSms_s = 0;//���ŷ��ͺ�15sδ�յ��ظ�,�ط�
                		g_unFailSms++;
				if(g_unFailSms>=3)
					{
					g_unFailSms = 0;
					g_unPhoneNo++;	   				
	   				if(g_unPhoneNo>=4)// 4�����뷢�� ����һ��
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
			KB1_ON;//Ͷ�С�����ָʾ�Ƽ�����  =0 ���� =0x55 ��������  >=1Ͷ����˸ = 0x55;//Ͷ�С�����ָʾ�Ƽ�����  =0 ���� =0x55 ��������  >=1Ͷ����˸
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
				g_gRmtInfo[YX_RH_SUCCESS]=0;//ȼ���ɹ�ң����λ
				}
		  	}
		  
                if(efslatch_flag>0)
                        {
                            g_TQBSCounter = 0x55;//Ͷ�С�����ָʾ�Ƽ�����  =0 ���� =0x55 ��������  >=1Ͷ����˸
                            efslatch_flag--;
                            if(efslatch_flag==0)
                            {
               	                latch_upload_flag=0x55;     ///////�������ϴ���־
               	                pulse_success=0;
                                g_gOverLoadFlg = 0;
                                g_gRmtInfo[YX_ACTION_SUCCESS]  = 0;
				   g_gRmtInfo[YX_PHASEA_ACT] = 0;	
				   g_gRmtInfo[YX_PHASEB_ACT] = 0;
				   g_gRmtInfo[YX_PHASEC_ACT] = 0;
                                g_gRmtInfo[YX_EFS_LATCH] = 0;   //�ý���ң��λ 
                                SaveLOG(LOG_LATCH, 0);
                                g_gRmtInfo[YX_RH_FAIL] = 0;   //ȼ��ʧ��ң�Ÿ���
                                g_gRmtInfo[YX_MANUAL_ACTION] = 0;   //���ֶ�Ͷ�н���
                               // g_gRmtInfo[0] &= ~YX_EIGHT_PULSE;   //��Ч8������� 
                               g_TQBSCounter = 0;//Ͷ�С�����ָʾ�Ƽ�����  =0 ���� =0x55 ��������  >=1Ͷ����˸
                               g_I0RmtZeroNum = 0;
                               //rh_send_ok = 0;
                            }      ///////����ʱ�䵽����Ч8����ҲҪ����
                        }
                ///////////����17���� 
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
                       // g_yxChangeflag |= BITF;//g_yxChangeflag�����λ��ʶ�б�����Ҫ�ϴ�
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
                       // g_yxChangeflag |= BITF;//g_yxChangeflag�����λ��ʶ�б�����Ҫ�ϴ�
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
                       // g_yxChangeflag |= BITF;//g_yxChangeflag�����λ��ʶ�б�����Ҫ�ϴ�
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
                
                if(power_ctl_flag>0)       //8����ң�ر�־
                    power_ctl_flag--;
                if(eight_ctl_flag>0)       //�ϵ�ң�ر�־
                    eight_ctl_flag--;         
                if(abnormal_ctl_flag>0)    //�쳣ң�ر�־
                   abnormal_ctl_flag--;
               // ScanDin();     //ɨ���ⲿң��
                 
                     
              /*  if(pulse_time_flag>0)   /////////�ṩ8����ʱ�䷶Χ
                {
                    pulse_time_flag--;
                    if(pulse_time_flag==0) 
          	    {
          	        if(flash_write==0)    ///////////˵��û�м�⵽��ʼ����
          	 	    flash_write=0x44;
          	        else
          	 	 flash_write=0x55; ///////////˵����⵽��ʼ����	 
          	     } 
                }*/
                if(g_sRtcManager.m_gRealTimer[RTC_SEC] >= 60)     //���Ӽ�ʱ
	            {			
			g_NolinkReset++;
	                g_sRtcManager.m_gRealTimer[RTC_SEC] = 0;
	                g_sRtcManager.m_gRealTimer[RTC_MINUT]++;
			  g_gSaveload++;					
                        if(state_counter>0)              ////////////�����ϱ�����ʱ
       	                {
                            state_counter--;

       	                }
                        
	                if((g_sRtcManager.m_gRealTimer[RTC_MINUT] & 3) == 0)  //Ϊ�˾����ܼ��ٶ�ʱʱ���ϵĲ����ʱʱ����5���Ӹ�Ϊ4����
	                {
	        
	                    g_sRtcManager.m_ucRtcSynFlag = YES;
                            if(phase_check_time>0)                    //////ÿ������Сʱ���һ����λ
       	                        phase_check_time--; 
                            if(phase_check_time==0)  
       	                    {
       	                        phase_check_time=30;
       	                        if(g_gProcCnt[PC_EFS_MODEL]>0)                    ///////ֻ��II���ź�Դ�ż����λ
       	  	                    phase_check_flag=0x55;             //////�ü����λ��־  
       	                        else if(g_gProcCnt[PC_EFS_MODEL]==0)               ///////ֻ��II���ź�Դ�ż����λ
       	  	                    phase_check_flag=0;                //////�ü����λ��־  	   	  	       	  
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
             	     	                upload_flag|=ABN_CHECK;     //////���������������´�
             	                    else if(i==1)
             	     	                upload_flag|=POWER_OFF;          //////�������´�
             	                    else if(i==2)
             	     	                upload_flag|=EIGHT_PULSE;          //////��Ч8�������´�
             	                    else if(i==3)
             	     	                upload_flag|=PHASE_BREAK;	      //////�������´�	 
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
  UCA1IE |= (UCRXIE + UCTXIE);              // ʹ�� USCI_A0 RX �ж�  // �ŏ|�����ж�Ƕ��	 
  UCA2IE |= (UCRXIE + UCTXIE);              // ʹ�� USCI_A0 RX �ж�  // �ŏ|�����ж�Ƕ��	
//  LED_RUN_OFF;
}

