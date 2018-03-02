/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  ģ����         : IO����ģ��
  �ļ���         : DriverIO.c
  ����ļ�       : DataStruct.h FuncDef.h Constant.h
  ʵ�ֹ���       : ��ɶ�ϵͳ�õ�IO���ܵĳ�ʼ����������ɶԿ���������ɨ����������Ϳ����������������
  ����           :  
  �汾           : 1.0
  
  --------------------------------------------------------------------------------
  ��ע           : -
  --------------------------------------------------------------------------------
  
  �޸ļ�¼       : 
    ��  ��          �汾            �޸���          �޸����� 
    2010/06/01      1.0                          ����

************************************************************************************/


#ifndef _DRIVER_IO

#define _DRIVER_IO
  #include "..\DataStruct.h"
#endif

#include "HAL_UCS.h"
#include "HAL_PMM.h"

//==============================================================================
//  ��������   : InitClk
//  ��������   : ϵͳ�õ���ʱ�ӽ��г�ʼ��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       �� 
//==============================================================================
void InitClk(void)
{   
    unsigned int i;
    /*
    PMMCTL0_H = 0xA5; // Open PMM module
    PMMCTL0 = 0xA500 + PMMCOREV_3; // Set VCore
    SVSMLCTL = SVMLE + (PMMCOREV_3 * SVSMLRRL0);// Set SVM new Level 
    while ((PMMIFG & SVSMLDLYIFG) == 0); // Wait till SVM is settled (Delay)
    PMMIFG &= ~(SVMLVLRIFG + SVMLIFG); // Clear already set flags
    if ((PMMIFG & SVMLIFG)) 
     while ((PMMIFG & SVMLVLRIFG) == 0); // Wait till level is reached
    PMMCTL0_H = 0x00; // Lock PMM module registers 
    //InitPort();//IO�˿ڳ�ʼ��
    */
    SetVCore(PMMCOREV_3); 
    P5SEL |= 0x04;                            // Port select XT2 
    UCSCTL0 = 0x1F00;                           // Set lowest possible DCOx, MODx   *ע��DCO =31 ���DCO����Ϊ24M
    
    UCSCTL1 = DCORSEL_5;                      // Select range for 18MHz operation
    UCSCTL2 = 732;                            // Set DCO Multiplier for 16MHz
                                            // (N + 1) * FLLRef = Fdco
                                            // (733 + 1) * 32768 = 24MHz
    UCSCTL6 &= ~XT2OFF;                       // Enable XT2 even if not used
 //����XT1���񱨴����ھ���δʹ�ã��ӳ�������ر�XT1  
    UCSCTL6 |= XT1OFF;                     
    UCSCTL3 |= SELREF_2;                     // FLLref = REFO
                                            // Since LFXT1 is not used,
                                            // sourcing FLL with LFXT1 can cause
                                            // XT1OFFG flag to set
    UCSCTL4 |= SELA_3;                        // ACLK=REFO,SMCLK=DCO,MCLK=DCO
    UCSCTL6 |= XT2BYPASS;
    UCSCTL6 &= ~(XT2DRIVE_2);
  // Loop until XT2 & DCO stabilize
    do
    {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
        SFRIFG1 &= ~OFIFG;                      // Clear fault flags
        for(i=0;i<0xFFFF;i++);                  // Delay for Osc to stabilize
    }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
    UCSCTL4 |= SELS_4 + SELM_4+SELA_5;               // SMCLK=MCLK=DCO  ACLK = XT2       
}


//==============================================================================
//  ��������   : ClkChange
//  ��������   : �ⲿʱ��XT2���л�
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void ClkChange(void)
{
    if(SFRIFG1&OFIFG)
    {
        do
        {
            UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                                // Clear XT2,XT1,DCO fault flags
            SFRIFG1 &= ~OFIFG;                    // Clear fault flags
          
        } while (SFRIFG1&OFIFG);  
        UCSCTL4 |= SELS_4 + SELM_4 + SELA_5; // SMCLK=MCLK=DCO   �������鷢����������л�DCO��ֻҪ���쳣��־���Ӧ�þͿ����л�������                                                      //��UCSCTL4Ӧ����û�б仯�ģ������仰�������岻�Ǻܴ�
    } 
}


//==============================================================================
//  ��������   : InitPort
//  ��������   : ϵͳ�õ���IO���г�ʼ��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       �� 
//==============================================================================
void InitPort(void)
{

    P1SEL = 0x00;
    P2SEL = 0x00;
    P3SEL |= 0x3e;        //P3.1-2 SPI0���ţ�P3.4-5 ����0����

    P4SEL = 0x00;        
    P5SEL |= 0xc7;        //P5.0-1 Vref+,-   P5.2 x2clkIN   P5.6-7 ����1
    P6SEL |= 0xf0;        //AD����4-7 Ua/Ub/Uc/U0

    P7SEL |= 0x70;        //  P7.4 P7.5 P7.6AD����I0
    P8SEL = 0x00;
    P9SEL |= 0x3e;        //P9.1-3 SPI2  4-5����2 

    P10SEL |= 0x30;       //P10.4-5 ����3����
    P11SEL = 0x00;
    
    P1DIR = 0x00;         //
    P2DIR |= 0x04;        //P2.2 W5500RST
    P3DIR |= 0x98;        //P3.4 TXD0,P3.3 SPI0 CLK, P3.0 W5500INT, P3.7 SPI0 CS0

    P4DIR |= 0xc0;        //P4.3-5 KJA1-KJC1����  P4.6-7 Ka1/Kb1
    P4OUT = 0;
    P5DIR |= 0x59;        //P5.0 ���VREF+ P5.2�������� P5.3 WDG_CLR P5.4 Kc1��� P5.5 P-V���� P5.6 TXD1
    P6DIR = 0x0f;         //AD����4-7  P6.0 ���е�RUN P6.1-3 ʱ��оƬIIC SCLK/DATA/CE

    P7DIR |= 0x0d;        //P7.2 Ka0, P7.3 Kb0
    P7OUT = 0;

    P8DIR = 0x83;   //P8.0 Kc0  P8.7 EEPROM_WP    P8.1FlashCS0
    P8OUT = 0;
        
    P9DIR |= 0x19;        //P9.0 SCS P9.3 CLK , P9.4 TXD2
    
}


//==============================================================================
//  ��������   : ScanDin
//  ��������   : ����������ɨ�躯����������ɨ��Ľ��������Ӧ��SOE��ң�ű�λ��Ϣ
//  �������   : <��>
//  �������   : ���ݲɼ�����ı�g_gRmtInfo�ж�Ӧ��DIN������SOE
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void ScanDin(void)
{
     unsigned char i;
 
#ifndef  CONDIN_3
    if(KJc1==0)     /////////��⵽�����������ϣ�����()
    {
        if(abnormal_counter<7)	
            abnormal_counter++;
        if(abnormal_counter==7)              ///////״̬�쳣��������⵽������������������
    	{	
            abnormal_counter=8;
    	    abnormal_ctl_flag=5;              ///////״̬�쳣ң�ر�־
    	    save_abnormal=0x55;
            g_gRmtInfo[YX_EFS_ABNORMAL] = 1;
            KA1_ON;	             

	     //CreatNewSMS(ABN_CHECK);                      //��������������������//�ŏ|
            for(i=0;i<6;i++)
                g_sSoeData.m_gAbnBuff[i] = g_sRtcManager.m_gRealTimer[i];
            g_sSoeData.m_gAbnBuff[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
           
            CreatNewSMS(ABN_CHECK);                      //��������������������
    	   /*  if(g_gProcCnt[PC_GPRS_MODEL]==0)
	    {
	        upload_interval[0]=(3*upload_interval_set)-1;      //////////////״̬�쳣set 4 minutes
                upload_flag|=ABN_CHECK;	
	    }
            else
	    {
	        upload_interval[0]=0;      //////////////eight pulse set 4 minutes	
	        upload_flag&=~ABN_CHECK;	
	    }*/
    	}
    }
    else
    {
        if(abnormal_counter>0)	
        {
            if(abnormal_counter>1)	
                abnormal_counter-=2;
            else
              	abnormal_counter--;              	
            if(abnormal_counter==0)
            {
                 save_abnormal=0x66;
                 g_gRmtInfo[YX_EFS_ABNORMAL] =0;
             	 KA1_OFF; 
                 for(i=0;i<6;i++)
                     g_sSoeData.m_gAbnOFFBuff[i] = g_sRtcManager.m_gRealTimer[i]; 
                 g_sSoeData.m_gAbnOFFBuff[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
              /*  */ 
             }	
         }
    }
#endif	
    if((Powerdown_counter == 0)&&(power_off_flag==1))    /////////�ź�Դ����ָ�	
    {
       power_off_flag=0;
       g_gRmtInfo[YX_EFS_OFF] = 0;
       for(i=0;i<6;i++)
           g_sSoeData.m_gPowerONBuff[i] = g_sRtcManager.m_gRealTimer[i]; 
       g_sSoeData.m_gPowerONBuff[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
        
       save_power_off=0x66; 
       //KB1_OFF;//KB1��ΪͶ�е�
    }
    else if((Powerdown_counter >= 50)&&(power_off_flag==0))    /////////�ź�Դ������	(�µ�·��)			 
    {
        power_off_flag=1;	  
        power_ctl_flag=5;              ///////�ϵ�ң�ر�־   ��ʱ5����
        save_power_off=0x55; 
        g_gRmtInfo[YX_EFS_OFF] = 1;
        //KB1_ON; //KB1��ΪͶ�е�           ///�µ�·�� 	 
        //CreatNewSMS(POWER_OFF);                      //�����������//�ŏ|
        
       for(i=0;i<6;i++)
            g_sSoeData.m_gPowerOFFBuff[i] = g_sRtcManager.m_gRealTimer[i];
        g_sSoeData.m_gPowerOFFBuff[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
        CreatNewSMS(POWER_OFF);                      //�����������
/* 
 	 if(g_gProcCnt[PC_GPRS_MODEL]==0)                                       //////////////�϶��ᷢ����Ϣ�����ǻ᲻�ᷢ��GPRS���ݻ���Ҫ
   	 {
   	      upload_interval[1]=(3*upload_interval_set)-1;      //////////////����4 minutes
              upload_flag |= POWER_OFF;	
   	 }
         else
   	 {
   	      upload_interval[1]=0;      //////////////eight pulse set 4 minutes	
   	      upload_flag &= (~POWER_OFF);	
   	 }*/
    }
 //   YX_RMT();   //
    if(abnormal_ctl_flag==0)           ///////�쳣ң�ر�־       �µ�·��
       KA1_OFF;  
    //if(power_ctl_flag==0)              ///////�ϵ�ң�ر�־//KB1��ΪͶ�е�
       //KB1_OFF;  //KB1��ΪͶ�е�
    if(eight_ctl_flag==0)              ///////��Ч8�����־
       KC1_OFF; 
    
}

void RmInfoChk(void)
{
    unsigned char m;
    //unsigned int unRmtTemp = 0;
    //unsigned int j=1;
    for(m= 0;m < RMT_INFO_NUM;m++)
    {
         if(g_gRmtInfoBak[m] != g_gRmtInfo[m])
        {       
        /*
               for(k = 0; k < g_ucYxTransNum;k++)
               {
                   if(m == g_ucYXAddr[k]-1)
                   {
                       //unRmtTemp = (g_gRmtInfo[0] & j)>>i;
                       CreatNewSoe(m,g_gRmtInfo[m],2);//g_ucYXAddr[i]-1
                       break;
                   }
             
        	 }  */
        CreatNewSoe(m,g_gRmtInfo[m],2);	 
        g_gRmtInfoBak[m] = g_gRmtInfo[m];   
       }
    }    
}
void ScanDinYX(void)
{
	unsigned char ka,kb,kc;
	if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_CON_NC])
		{//0=nc
		ka=0x08;kb=0x10;kc=0x20;
		}
	else
		{//0=no
		ka=0;kb=0;kc=0;
		}	
// **************��⿪��λ���쳣 **************
    	if(KJa1==ka)//&&(g_gKON!=100)     /////////��⵽Ӳң��1
    		{
    		g_gPhasErrTimer[0]++;
		if(g_gPhasErrTimer[0]>100)
			{
			g_gPhasErrTimer[0] = 100;
    			g_gRmtInfo[YX_PHASEA_ERR]=1;
			}
    		}
    	else
    		{
    		if(g_gPhasErrTimer[0]==0)
			g_gRmtInfo[YX_PHASEA_ERR]=0;
		else
			g_gPhasErrTimer[0]--;
    		}
    	if(KJb1==kb)//&&(g_gKON!=100)     /////////��⵽Ӳң��1
    		{
    		g_gPhasErrTimer[1]++;
		if(g_gPhasErrTimer[1]>100)
			{
			g_gPhasErrTimer[1] = 100;
    			g_gRmtInfo[YX_PHASEB_ERR]=1;
			}
    		}
    	else
    		{
    		if(g_gPhasErrTimer[1]==0)
			g_gRmtInfo[YX_PHASEB_ERR]=0;
		else
			g_gPhasErrTimer[1]--;
    		}
    	if(KJc1==kc)//&&(g_gKON!=100)     /////////��⵽Ӳң��1
    		{
    		g_gPhasErrTimer[2]++;
		if(g_gPhasErrTimer[2]>100)
			{
			g_gPhasErrTimer[2] = 100;
    			g_gRmtInfo[YX_PHASEC_ERR]=1;
			}
    		}
    	else
    		{
    		if(g_gPhasErrTimer[2]==0)
			g_gRmtInfo[YX_PHASEC_ERR]=0;
		else
			g_gPhasErrTimer[2]--;
    		}		
//*******************************************

// **************    ���I0��ʱ      **************
	if(g_gRmtMeas[RM_I0] >= g_gProcCntJug[PC_PULSE_VALID])
		{
		g_gI0OverTimer++;
		if(g_gI0OverTimer>100)
			{
			g_gI0OverTimer=100;
			g_gRmtInfo[YX_I0_TIMEOVER]=1;
			}
		}
	else
		{
		if(g_gI0OverTimer==0)
			g_gRmtInfo[YX_I0_TIMEOVER]=0;
		else
			g_gI0OverTimer--;
		}
//*******************************************

    g_gExtDinStat[0] =  g_gExtDinStat[1];
    
    if(KJa1==ka)     /////////��⵽Ӳң��1
    {
        g_gExtDinYesTimer[0]++;
        g_gExtDinNoTimer[0] = 0;
        if(g_gExtDinYesTimer[0] >= g_gRunPara[YX_delay])
        {
            g_gExtDinYesTimer[0] = g_gRunPara[YX_delay];
            g_gExtDinStat[1] |= 1;
            //ucTemp[0] = 1;
	     g_gRmtInfo[YX_KJA]=1;
        }
    }
    else
    {
        g_gExtDinNoTimer[0]++;
        g_gExtDinYesTimer[0] = 0;
        if(g_gExtDinNoTimer[0] >= g_gRunPara[YX_delay])
        {
            g_gExtDinNoTimer[0] = g_gRunPara[YX_delay];
            g_gExtDinStat[1] &= (~0x01);
            //ucTemp[0] = 0;
            g_gRmtInfo[YX_KJA]=0;
        }
    }
    if(KJb1==kb)     /////////��⵽Ӳң��1
    {
        g_gExtDinYesTimer[1]++;
        g_gExtDinNoTimer[1] = 0;
        if(g_gExtDinYesTimer[1] >= g_gRunPara[YX_delay])
        {
            g_gExtDinYesTimer[1] = g_gRunPara[YX_delay];
            g_gExtDinStat[1] |= 0x02;
            //ucTemp[1] = 1;
            g_gRmtInfo[YX_KJB]=1;
        }
    }
    else
    {
        g_gExtDinNoTimer[1]++;
        g_gExtDinYesTimer[1] = 0;
        if(g_gExtDinNoTimer[1] >= g_gRunPara[YX_delay])
        {
            g_gExtDinNoTimer[1] = g_gRunPara[YX_delay];
            g_gExtDinStat[1] &= (~0x02);
            //ucTemp[1] = 0;
            g_gRmtInfo[YX_KJB]=0;
        }      
    }
#ifdef CONDIN_3	
    if(KJc1==kc)     /////////��⵽Ӳң��1
    {
        g_gExtDinYesTimer[2]++;
        g_gExtDinNoTimer[2] = 0;
        if(g_gExtDinYesTimer[2] >= g_gRunPara[YX_delay])
        {
            g_gExtDinYesTimer[2] = g_gRunPara[YX_delay];
            g_gExtDinStat[1] |= 1;
            //ucTemp[0] = 1;
	     g_gRmtInfo[YX_KJC]=1;
        }
    }
    else
    {
        g_gExtDinNoTimer[2]++;
        g_gExtDinYesTimer[2] = 0;
        if(g_gExtDinNoTimer[2] >= g_gRunPara[YX_delay])
        {
            g_gExtDinNoTimer[2] = g_gRunPara[YX_delay];
            g_gExtDinStat[1] &= (~0x01);
            //ucTemp[0] = 0;
            g_gRmtInfo[YX_KJC]=0;
        }
    }	
#endif
	/*
	    unsigned char ucTemp[2];  
    if((g_gExtDinStat[0] & 0x01) !=  (g_gExtDinStat[1] & 0x01))
    {
        CreatNewSoe(YX_P4IN3,ucTemp[0],2);
    }      
    if((g_gExtDinStat[0] & 0x02) !=  (g_gExtDinStat[1] & 0x02))
    {
        CreatNewSoe(YX_P4IN4,ucTemp[1],2); 
    }*/
}

/*
void YX_RMT(void)
{
    unsigned char i,j=1;
    YX_BAK = YX;
    YX = 0;

         if(fault_save>0)         	 	
         {
	     YX |=  0x01;
    	 }
         else if(fault_save==0)	
         {
             YX &= 0xFE; 	
    	 }

     for(i = 0; i < 3 ; i++ )
     {
         if((YX_BAK & j) != (YX & j))
         {
           g_ucYXSengFlg = 0x01;
           fault_num++;
           YX_BianWei[fault_num - 1] = j;
           YX_Time[fault_num - 1][0]=((g_sRtcManager.m_gRealTimer[RTC_MICROSEC])&0xff);
           YX_Time[fault_num - 1][1]=((g_sRtcManager.m_gRealTimer[RTC_MICROSEC])&0xff00)>>8;
           YX_Time[fault_num - 1][2]= g_sRtcManager.m_gRealTimer[RTC_MINUT];
           YX_Time[fault_num - 1][3]= g_sRtcManager.m_gRealTimer[RTC_HOUR];
           YX_Time[fault_num - 1][4]= g_sRtcManager.m_gRealTimer[RTC_DATE];
           YX_Time[fault_num - 1][5]= g_sRtcManager.m_gRealTimer[RTC_MONTH];
           YX_Time[fault_num - 1][6]= g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
         }
         j = j<<1;
     }
}

//==============================================================================
//  ��������   : InitClk
//  ��������   : ϵͳ�õ���ʱ�ӽ��г�ʼ��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void InitClk(void)
{ 
//   unsigned int i;  
//    P5SEL |= 0x0C;                            // Port select XT2 
//    UCSCTL0 = 0x1F00;                           // Set lowest possible DCOx, MODx   *ע��DCO =31 ���DCO����Ϊ24M
//  
//    UCSCTL1 = DCORSEL_5;                      // Select range for 18MHz operation
//    UCSCTL2 = 732;                            // Set DCO Multiplier for 16MHz
//                                            // (N + 1) * FLLRef = Fdco
//                                            // (733 + 1) * 32768 = 24MHz
//    UCSCTL6 &= ~XT2OFF;                       // Enable XT2 even if not used
// //����XT1���񱨴����ھ���δʹ�ã��ӳ�������ر�XT1  
//    UCSCTL6 |= XT1OFF;                     
//    UCSCTL3 |= SELREF_2;                     // FLLref = REFO
//                                            // Since LFXT1 is not used,
//                                            // sourcing FLL with LFXT1 can cause
//                                            // XT1OFFG flag to set
//    UCSCTL4 |= SELA_3;                        // ACLK=REFO,SMCLK=DCO,MCLK=DCO
//    UCSCTL6 |= XT2BYPASS;
//    UCSCTL6 &= ~(XT2DRIVE_2);
//  // Loop until XT2 & DCO stabilize
//    do
//    {
//        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
//                                            // Clear XT2,XT1,DCO fault flags
//        SFRIFG1 &= ~OFIFG;                      // Clear fault flags
//        for(i=0;i<0xFFFF;i++);                  // Delay for Osc to stabilize
//    }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
//    UCSCTL4 |= SELS_4 + SELM_4+SELA_5;   
//  
    UCSCTL3 = SELREF_2;                                        //FLL = REFCLK  32768kHz
    UCSCTL4 = SELS__DCOCLK + SELA__XT1CLK + SELM__DCOCLK;
    __bis_SR_register(SCG0);
    UCSCTL0 = 0;
    UCSCTL1 = DCORSEL_5;
    UCSCTL2 = FLLD_1 + 249;   
    P7SEL |= 0x03;                            // Select XT1
    UCSCTL6 &= (~XT1OFF);//8MHz
    UCSCTL6 |= XCAP_3; 
    __bic_SR_register(SCG0);   
   do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
    WatchDog();
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

  UCSCTL6 &= ~(XT1DRIVE_3);                 // Xtal is now stable, reduce drive
                                            // strength   


  
}
void IoInit(void)
{    
    P1SEL = 0x00;   //1.0-1.7�ɼ�����ң�ţ�ȫ������Ϊ����
    P1DIR = 0x00;
    P2SEL = 0x00;   //2.0-2.7�ɼ�����ң�ţ�ȫ������Ϊ����
    P2DIR = 0x00;
    
    
    P3SEL = 0x3E;  //3.0- flash hold 3.1-SIMO 3.2-SOMI 3.3-CLK;3.4-TX0 3.5-RX0 3.6 CS1;3.7 CS0
    P3DIR = 0xDB;
    P3OUT = 0x01; //flash hold ��Ϊ�ߵ�ƽ
     
    P4SEL = 0x00;  //4.0-RF��Դ 4.1-RF��λ 4.2-RF����(in) 4.3-GPRS��Դ  4.4-4.5 DI���� 4.6-BAT��� 4.7-BAT�ŵ�
    P4DIR = 0xCB;
    P4OUT = 0x00;

    P5SEL = 0xC0;  //P56 TXD1  P57 RXD1 ��GPRSͨѶ  5.4 DI���� 5.5-��ػ���
    P5DIR = 0x6F;
    P5OUT = 0x00;
    
    P6SEL = 0x0F;  //P60-P63�ɼ�AD�� P60-BATFU-AD0 P61-SAM_C P62-SAM_SUN P63-SAM_BAT P64-��λGPRSģ��  P65-GPRSģ������  P66-G_ON/OFF ��/�ر� P67 ��ظ���ѹ����
    P6DIR = 0xFF;
    P6OUT = 0x00;
     
    P7SEL = 0x00; //7.2-flash_wp 7.3-eeprom_wp 7.5-WD 7.4-RUN LED
    P7DIR = 0xFF;
    P7OUT = 0x00;    

    P8SEL = 0x00; //8.0-8.2  8.4-8.5 ң������ P8.3 ʱ���ж� P8.6 Ӳң�ŵ�Դ״̬���  P8.7 �̵����������
    P8DIR = 0x80; 
    P8OUT = 0x80;    

    P9SEL = 0x30; //P94 TXD2  P95 RXD2 9.1-9.2 I2C 9.0 �̵���������� 9.3 �ܿ�24V��Դ���
    P9DIR = 0xDF;
    P9OUT = 0x01;    

    P10SEL = 0x30;  //P10.0-10.2 AD�������� P10.3 Ӳң�ŵ�Դ���� P10.4 TXD3  P10.5 RXD3 ���ÿ�ʹ��  
    P10DIR = 0xDF;
    P10OUT = 0x00;      
  
}



//==============================================================================
//  ��������   : InitDin
//  ��������   : ��ʼ������������ɨ��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void InitDin(void)
{
    unsigned long i,biti;
    unsigned long ExtIoStat = 0;//��ǰ24���ⲿDIN�����״̬ 
    unsigned int ExtIo1=0,ExtIo2=0;
   
	OPEN_DI_POWER();
     for(i=0;i<1000;i++)
     {
            if((READ_DI_POWER_ST) < 1) 
            {  
              break;
            }
     }
	if((READ_DI_POWER_ST) < 1)
	{
		delay(300);
        P1SEL = 0;    //YX00~YX07
    	P1DIR = 0;

    	P2SEL = 0;    //YX08~YX15
    	P2DIR = 0;

		P4SEL &= ~(0x30);    //P2.4~P2.7  YX16~YX19 
    	P4DIR &= ~(0x30);

		P5SEL &= ~(0x10);    //P2.4~P2.7  YX16~YX19 
    	P5DIR &= ~(0x10);
    
    	P8SEL &= ~(0x37);    //P2.4~P2.7  YX20~YX24 
    	P8DIR &= ~(0x37);

    
    //��CPU�ܽ���ȡ��ӦDIN��IO�ܽŵ�ƽ  
      	ExtIo1 = (P1IN & BIT0) +((P1IN & BIT2)>>1)+((P1IN & BIT4)>>2)+((P1IN & BIT6)>>3)+((P2IN & BIT0)<<4)
      	+((P2IN & BIT2)<<3)+((P2IN & BIT4)<<2)+((P2IN & BIT6)<<1); 
	
		ExtIo2=((P4IN & BIT4)>>4) +((P5IN & BIT4)>>3)+((P8IN & BIT1)<<1)+((P8IN & BIT4)>>1)+((P1IN & BIT1)<<3)
      	+((P1IN & BIT3)<<2)+((P1IN & BIT5)<<1)+(P1IN & BIT7); 
		ExtIo1=ExtIo1+(ExtIo2<<8);
      	ExtIoStat = ((P2IN & BIT1)>>1) +((P2IN & BIT3)>>2)+((P2IN & BIT5)>>3)+((P2IN & BIT7)>>4)+((P4IN & BIT5)>>1)
      	+((P8IN & BIT0)<<5)+((P8IN & BIT2)<<4)+((P8IN & BIT5)<<2); 
      	ExtIoStat = ((ExtIoStat << 16) & 0x00ff0000) + ExtIo1;//��24��DIN��IO״̬���θ�ֵ���ñ��� 
		ExtIoStat = ExtIoStat | 0xff000000;
        g_gExtDinStat = ~ExtIoStat;
		
		
     	
      	g_ucRefYxYcTs |= BIT0;
		for(i = 0; i < EXT_DIN_NUM; i++)
      	{
			if(((g_gRunPara[YX_DSl]+g_gRunPara[YX_DSh]*65536)&((unsigned long)1<<i))>0)//˫��
			{
				if(((g_gExtDinStat  >> 2*(i/2)) & 0x03) == 1 )
				{
								//CreatNewSoe(RI_SD_YX1 + i/2,(2),1);				
					SetRIStatus(RI_ZD_DYX1 +i/2,2);
							
				}
							
				else if(((g_gExtDinStat  >> 2*(i/2)) & 0x03) == 2 )
            	{
                    		
					SetRIStatus(RI_ZD_DYX1 +i/2, 1);
													
				}//��
            	else if(((g_gExtDinStat  >> 2*(i/2)) & 0x03) == 0 )
            	{
                    			
					SetRIStatus(RI_ZD_DYX1 +i/2, 0);	
									
								
				}//����
            	else 
            	{
					SetRIStatus(RI_ZD_DYX1 +i/2, 3);	
														
				}  //����
			}
			else
			{
				 biti = 1; //ԭ����ֱ��biti=1<<i,��i=16ʱ������ 
          		biti = biti << i;
          		if( (g_gExtDinStat  & biti) == biti)//Ϊ����DIN�ĵ�ƽ
          		{
              		SetRIStatus(RI_ZD_YX1 +i,1);
          		}
         	 	else
          		{
              		SetRIStatus(RI_ZD_YX1 +i, 0);
          		}
			}
			
		}
		
      	
	}
	CLOSE_DI_POWER();

}


//==============================================================================
//  ��������   : ScanDin
//  ��������   : ����������ɨ�躯����������ɨ��Ľ��������Ӧ��SOE��ң�ű�λ��Ϣ
//  �������   : 
//  ����ֵ     : 1=�б�λ 0=�ޱ�λ
//  ����˵��   : 
//  ����       ��
//==============================================================================
bool ScanDin(void)
{
    unsigned long i,m,n,ExtIoStat = 0;//��ǰ24���ⲿDIN�����״̬
    volatile unsigned int ExtIo1=0,ExtIo2=0,j;
    unsigned int f1_cnt = 0xff;
   static unsigned long lExtDinStat_new;
   unsigned char ucReadDiPowerSt = 0;
   unsigned long biti;
	
   //��CPU�ܽ���ȡ��ӦDIN��IO�ܽŵ�ƽ
	  OPEN_DI_POWER();
   
   	  //delay(3000);
        if (g_gRunPara[Terminal_Type]==AERIAL_TERMINAL)
        {//�ܿ��Ͳ�����Ӳң�ŵ�Դ״̬���
          ucReadDiPowerSt = 1;
        }
        else
        {
	    for(m=0;m<700;m++)//1000
            {
              if((READ_DI_POWER_ST) < 1) 
              {
                ucReadDiPowerSt = 1;
                break;
              }
            }
        }
        if(ucReadDiPowerSt == 1)
        {
          	delay(1500);
           
            ExtIo1 = (P1IN & BIT0) +((P1IN & BIT2)>>1)+((P1IN & BIT4)>>2)+((P1IN & BIT6)>>3)+((P2IN & BIT0)<<4)
      	    +((P2IN & BIT2)<<3)+((P2IN & BIT4)<<2)+((P2IN & BIT6)<<1); 
	
		    ExtIo2=((P4IN & BIT4)>>4) +((P5IN & BIT4)>>3)+((P8IN & BIT1)<<1)+((P8IN & BIT4)>>1)+((P1IN & BIT1)<<3)
      	    +((P1IN & BIT3)<<2)+((P1IN & BIT5)<<1)+(P1IN & BIT7); 
            
		    ExtIo1=ExtIo1+(ExtIo2<<8);
            
      	    ExtIoStat = ((P2IN & BIT1)>>1) +((P2IN & BIT3)>>2)+((P2IN & BIT5)>>3)+((P2IN & BIT7)>>4)+((P4IN & BIT5)>>1)
      	    +((P8IN & BIT0)<<5)+((P8IN & BIT2)<<4)+((P8IN & BIT5)<<2); 
            
      	    ExtIoStat = ((ExtIoStat << 16) & 0x00ff0000) | ExtIo1;//?
            ExtIoStat = ExtIoStat | 0xff000000; 
            lExtDinStat_new = ~ExtIoStat;
            
		    CLOSE_DI_POWER();
            //delay(20);
     	if(lExtDinStat_new  != g_gExtDinStat)
        {
		    mtemp=lExtDinStat_new^g_gExtDinStat;
			YX_delay_clear_flag=1;
		    for(i=0;i<24;i++)
		    {
			    if(mtemp%2==1)
			    {
				    g_gExtDinYesTimer[i]++;
			    }
			    else
			    {
				    g_gExtDinYesTimer[i]=0;
			    }
			    if(g_gExtDinYesTimer[i] == 1)
			    {
				    for(j=0;j<8;j++)
				    {
					    g_gDinRtcLock[i][j]=g_sRtcManager.m_gRealTimer[j];
				    }
			    }
			    if(((g_gRunPara[YX_DSl]+g_gRunPara[YX_DSh]*65536)&((unsigned long)1<<i)) > 0 )//// ˫������һ����λʱ�������һ���Ѿ����ڱ�λ��ʱ�У�����������1��������ʱ
			    {
				    if(g_gExtDinYesTimer[i]==1)
				    {
					    if(i%2==0)
					    {
						    if(g_gExtDinYesTimer[i+1]>0)
							    g_gExtDinYesTimer[i+1]=1;
					    }
					    else
					    {
						    if(g_gExtDinYesTimer[i-1]>0)
							    g_gExtDinYesTimer[i-1]=1;
					    }
				    }
			    }
			    if(g_gExtDinYesTimer[i]>g_gRunPara[YX_delay])
			    {
				    if(((g_gRunPara[YX_DSl]+g_gRunPara[YX_DSh]*65536)&((unsigned long)1<<i))>0)//˫��
				    {
					    if(i%2==0)
					    {
						    if((g_gExtDinYesTimer[i+1]>g_gRunPara[YX_delay]-1)||(g_gExtDinYesTimer[i+1]==0))
						    {
							    f1_cnt=0x80+i;
							    g_gExtDinYesTimer[i]=0;
							
							    g_gExtDinStat^=((unsigned long)1<<i);
							
        					
							    if(g_gExtDinYesTimer[i+1]>0)
							    {
								    g_gExtDinYesTimer[i+1]=0;
								
								    g_gExtDinStat^=((unsigned long)1<<(i+1));
							    }
							
							    if(((g_gExtDinStat  >> 2*(i/2)) & 0x03) == 1 )
							    {
							
							    	CreatNewSoe(RI_ZD_DYX1 + (i/2),(2),1);	
							    }
							    else if(((g_gExtDinStat >> 2*(i/2)) & 0x03) == 2 )
                    		    {
                    				CreatNewSoe(RI_ZD_DYX1 + (i/2),(1),1);		
							    }//��
                    		    else if(((g_gExtDinStat  >> 2*(i/2)) & 0x03) == 0 )
                    		    {
								    CreatNewSoe(RI_ZD_DYX1 + (i/2),(0),1);		
							    }//����
                    		    else 
                    		    {
								    CreatNewSoe(RI_ZD_DYX1 + (i/2),(3),1);	
							    }  //����
								
						    }
						    else
						    {
							    g_gExtDinYesTimer[i]=0;
							    g_gExtDinYesTimer[i+1]=0;
						    }
					    }
					    else
					    {
						    if((g_gExtDinYesTimer[i-1]>g_gRunPara[YX_delay]-1)||(g_gExtDinYesTimer[i-1]==0))
						    {
							    f1_cnt=0x80+i-1;
							    g_gExtDinYesTimer[i]=0;
							    g_gExtDinStat^=((unsigned long)1<<i);
							
							    if(g_gExtDinYesTimer[i-1]>0)
							    {
								    g_gExtDinYesTimer[i-1]=0;
								   g_gExtDinStat^=((unsigned long)1<<(i-1));
        						
							    }
							    if(((g_gExtDinStat  >> 2*(i/2)) & 0x03) == 1 )
							    {						
									CreatNewSoe(RI_ZD_DYX1 + (i/2),(2),1);	
								}
							
							    else if(((g_gExtDinStat  >> 2*(i/2)) & 0x03) == 2 )
                    		    {
									CreatNewSoe(RI_ZD_DYX1 + (i/2),(1),1);		
								}//��
                    		    else if(((g_gExtDinStat  >> 2*(i/2)) & 0x03) == 0 )
                    		    {
									CreatNewSoe(RI_ZD_DYX1 + (i/2),(0),1);																		
							    }//����
                    		    else 
                    		    {                    			
									CreatNewSoe(RI_ZD_DYX1 + (i/2),(3),1);	
									
							    }  //����
								
						    }
						    else
						    {
							    g_gExtDinYesTimer[i]=0;
							    g_gExtDinYesTimer[i-1]=0;
						    }
					    }
				    }
				    else  //����
				    {
					    g_gExtDinYesTimer[i]=0;
					    g_gExtDinStat ^=((unsigned long)1<<i);
 			
					  if(((g_gRunPara[YX_MODEl]+g_gRunPara[YX_MODEh]*65536)& ((unsigned long)1<<i) ) > 0 )// ����ģʽ
                      {
                        // �����������ʱ��û��������ң�Ŵ� 1 -> 0 ������������
                      	if(g_gDIMcLock[i] > 0)
                        {
							f1_cnt = 0xff;
                        }
                        else if(( g_gExtDinStat & ((unsigned long)1 << i)) != ((unsigned long)1 << i)) 
                        {
                          f1_cnt = 0xff;//������
                        }
                        
                        else
                        { 
					        f1_cnt = 0x40 + i;
							CreatNewSoe(RI_ZD_YX1  + i,(1),0);
                            g_gDIMcLock[i]=g_gRunPara[RP_FLTLK_T];
                        }    
                     }
                     else
                     { 
                       f1_cnt = i;
          				biti = 1; //ԭ����ֱ��biti=1<<i,��i=16ʱ������ 
          		        biti = biti << i;
          		        if( (g_gExtDinStat  & biti) == biti)//Ϊ����DIN�ĵ�ƽ
          		        {
              		       CreatNewSoe(RI_ZD_YX1 + i,1,1);
          		        }
         	 	        else
          		        {
              		        CreatNewSoe(RI_ZD_YX1 + i,0,1);
          		        }
                     }	
				}
			}
			
			mtemp >>= 1;			
        }
	    	
	    }
		else
		{
			if(YX_delay_clear_flag > 0)
        	{
            	for(n=0;n<24;n++)
                
            	g_gExtDinYesTimer[n] = 0;
            
            	YX_delay_clear_flag = 0;    //��ֹ�� �ж����ظ� �����ʱ����
        	}
		}
        //CLOSE_DI_POWER();
     }
     if((f1_cnt<0xFF))
	    {
		    return 1;
	    }
        else
            return 0;
}


//==============================================================================
//  ��������   : ActOut
//  ��������   : �Ծ���Ӳ������Ĳ��������ݲ������ͣ���������
//  �������   : OutType-�������ں�
//                 OutValue-����ֵ��ON��OFF
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================

void ActOut(unsigned int OutType, unsigned int OutValue)
{

}

*/

void ScanSoftLacth(void)
{
	if(g_gRmtInfo[YX_EFS_LATCH] == 1)
		return;
	if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_PT_LATCH])	
	{
	if((g_gRmtInfo[YX_PHASEA_OFF] == 1)
		&&(g_gRmtInfo[YX_PHASEB_OFF] == 1)
		&&(g_gRmtInfo[YX_PHASEC_OFF] == 1))
		{
		if(ptoff_timeout > 0)
			{
			ptoff_timeout--;
			if(ptoff_timeout==0)
				{
				ptoff_timeout =0;
				KA0_OFF; KB0_OFF;KC0_OFF; g_gKON=OFF;
				eight_pulse_flag=0;
    				pulse_phase_flag=0; 
    				efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////����17����
    				latch_upload_flag=0x55;      	
    				uart0_event_flag=0;         ///////��������0����Ϊ����״̬��������ʾ
    				g_gRmtInfo[YX_EFS_LATCH] = 1;   //�ñ���ң��λ 
    				chongfa=0;	moniguzhang=0;
    				g_gRmtMeas[RM_ACT_NUM] = 0;
				g_gRmtInfo[YX_EFS_ACT] = 0;   //Ͷ��״̬ ң����0	
				}
			}
		else
			{ptoff_timeout =500;}
		}
	else
		{
		ptoff_timeout = 500;
		}
	}
	/*
	if((g_gRmtInfo[YX_PHASEA_OFF] == 0)
		||(g_gRmtInfo[YX_PHASEB_OFF] == 0)
		||(g_gRmtInfo[YX_PHASEC_OFF] == 0))
		{
		if(soft_latch ==1)
			{
			g_gRmtInfo[YX_EFS_LATCH] = 0;   //�ñ���ң��λ 
			soft_latch =0;
			}
		}
	*/
	unsigned char ka,kb,kc;
	if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_CON_NC])
		{//0=nc
		ka=0x08;kb=0x10;kc=0x20;
		}
	else
		{//0=no
		ka=0;kb=0;kc=0;
		}		
	if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_CON_LATCH])	
	{
#ifdef CONDIN_3
	if((KJa1==ka)||(KJb1==kb)||(KJc1==kc))
#else		
	if(g_gRmtInfo[YX_KJB]==0)
#endif
		{
		if(km_timeout > 0)
			{
			km_timeout--;
			if(km_timeout==0)
				{
				km_timeout =0;
				KA0_OFF; KB0_OFF;KC0_OFF; g_gKON=OFF;
				eight_pulse_flag=0;
    				pulse_phase_flag=0; 
    				efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////����17����
    				latch_upload_flag=0x55;      	
    				uart0_event_flag=0;         ///////��������0����Ϊ����״̬��������ʾ
    				g_gRmtInfo[YX_EFS_LATCH] = 1;   //�ñ���ң��λ 
    				chongfa=0;	moniguzhang=0;
    				g_gRmtMeas[RM_ACT_NUM] = 0;
				g_gRmtInfo[YX_EFS_ACT] = 0;   //Ͷ��״̬ ң����0	
				}
			}
		else
			{km_timeout =g_gRunPara[RP_KM_TIME];}
		}
	else
		{
		km_timeout = g_gRunPara[RP_KM_TIME];
		}
	}

	if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_I0_LATCH])	
	{
	if(g_gRmtMeas[RM_I0]>g_gRunPara[RP_OVERLOAD_I])		
		{
		if(i0_timeout > 0)
			{
			i0_timeout--;
			if(i0_timeout==0)
				{
				i0_timeout =0;
				KA0_OFF; KB0_OFF;KC0_OFF; g_gKON=OFF;
				eight_pulse_flag=0;
    				pulse_phase_flag=0; 
    				efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////����17����
    				latch_upload_flag=0x55;      	
    				uart0_event_flag=0;         ///////��������0����Ϊ����״̬��������ʾ
    				g_gRmtInfo[YX_EFS_LATCH] = 1;   //�ñ���ң��λ 
    				chongfa=0;	moniguzhang=0;
    				g_gRmtMeas[RM_ACT_NUM] = 0;
				g_gRmtInfo[YX_EFS_ACT] = 0;   //Ͷ��״̬ ң����0	
				}
			}
		else
			{
			i0_timeout=g_gProcCnt[PC_OVERLOAD_T];//(g_gRunPara[RP_PLUSE_TIME]-g_gRunPara[RP_PLUSE_MODFK])/3;
			if(i0_timeout==0)i0_timeout=1;
			if(i0_timeout>g_gRunPara[RP_PLUSE_TIME])i0_timeout=1;
			}
		}
	else
		{
		i0_timeout = g_gProcCnt[PC_OVERLOAD_T];//(g_gRunPara[RP_PLUSE_TIME]-g_gRunPara[RP_PLUSE_MODFK])/3;
		if(i0_timeout==0)i0_timeout=1;
		if(i0_timeout>g_gRunPara[RP_PLUSE_TIME])i0_timeout=1;
		}
	}
}


