/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  模块名         : IO驱动模块
  文件名         : DriverIO.c
  相关文件       : DataStruct.h FuncDef.h Constant.h
  实现功能       : 完成对系统用到IO功能的初始化，包括完成对开关量输入扫描的驱动，和开关量输出的驱动。
  作者           :  
  版本           : 1.0
  
  --------------------------------------------------------------------------------
  备注           : -
  --------------------------------------------------------------------------------
  
  修改记录       : 
    日  期          版本            修改人          修改内容 
    2010/06/01      1.0                          创建

************************************************************************************/


#ifndef _DRIVER_IO

#define _DRIVER_IO
  #include "..\DataStruct.h"
#endif

#include "HAL_UCS.h"
#include "HAL_PMM.h"

//==============================================================================
//  函数名称   : InitClk
//  功能描述   : 系统用到的时钟进行初始化
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ： 
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
    //InitPort();//IO端口初始化
    */
    SetVCore(PMMCOREV_3); 
    P5SEL |= 0x04;                            // Port select XT2 
    UCSCTL0 = 0x1F00;                           // Set lowest possible DCOx, MODx   *注：DCO =31 配合DCO更改为24M
    
    UCSCTL1 = DCORSEL_5;                      // Select range for 18MHz operation
    UCSCTL2 = 732;                            // Set DCO Multiplier for 16MHz
                                            // (N + 1) * FLLRef = Fdco
                                            // (733 + 1) * 32768 = 24MHz
    UCSCTL6 &= ~XT2OFF;                       // Enable XT2 even if not used
 //发现XT1晶振报错，由于晶振未使用，加程序软件关闭XT1  
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
//  函数名称   : ClkChange
//  功能描述   : 外部时钟XT2的切换
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
        UCSCTL4 |= SELS_4 + SELM_4 + SELA_5; // SMCLK=MCLK=DCO   经过试验发现如果出现切换DCO后，只要把异常标志清除应该就可以切换回来了                                                      //而UCSCTL4应该是没有变化的，因此这句话可能意义不是很大
    } 
}


//==============================================================================
//  函数名称   : InitPort
//  功能描述   : 系统用到的IO进行初始化
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ： 
//==============================================================================
void InitPort(void)
{

    P1SEL = 0x00;
    P2SEL = 0x00;
    P3SEL |= 0x3e;        //P3.1-2 SPI0引脚，P3.4-5 串口0引脚

    P4SEL = 0x00;        
    P5SEL |= 0xc7;        //P5.0-1 Vref+,-   P5.2 x2clkIN   P5.6-7 串口1
    P6SEL |= 0xf0;        //AD输入4-7 Ua/Ub/Uc/U0

    P7SEL |= 0x70;        //  P7.4 P7.5 P7.6AD输入I0
    P8SEL = 0x00;
    P9SEL |= 0x3e;        //P9.1-3 SPI2  4-5串口2 

    P10SEL |= 0x30;       //P10.4-5 串口3引脚
    P11SEL = 0x00;
    
    P1DIR = 0x00;         //
    P2DIR |= 0x04;        //P2.2 W5500RST
    P3DIR |= 0x98;        //P3.4 TXD0,P3.3 SPI0 CLK, P3.0 W5500INT, P3.7 SPI0 CS0

    P4DIR |= 0xc0;        //P4.3-5 KJA1-KJC1输入  P4.6-7 Ka1/Kb1
    P4OUT = 0;
    P5DIR |= 0x59;        //P5.0 输出VREF+ P5.2晶振输入 P5.3 WDG_CLR P5.4 Kc1输出 P5.5 P-V输入 P5.6 TXD1
    P6DIR = 0x0f;         //AD输入4-7  P6.0 运行灯RUN P6.1-3 时钟芯片IIC SCLK/DATA/CE

    P7DIR |= 0x0d;        //P7.2 Ka0, P7.3 Kb0
    P7OUT = 0;

    P8DIR = 0x83;   //P8.0 Kc0  P8.7 EEPROM_WP    P8.1FlashCS0
    P8OUT = 0;
        
    P9DIR |= 0x19;        //P9.0 SCS P9.3 CLK , P9.4 TXD2
    
}


//==============================================================================
//  函数名称   : ScanDin
//  功能描述   : 开关量输入扫描函数，并根据扫描的结果产生对应的SOE和遥信变位信息
//  输入参数   : <无>
//  输出参数   : 根据采集结果改变g_gRmtInfo中对应的DIN，产生SOE
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void ScanDin(void)
{
     unsigned char i;
 
#ifndef  CONDIN_3
    if(KJc1==0)     /////////检测到辅助触点吸合，报警()
    {
        if(abnormal_counter<7)	
            abnormal_counter++;
        if(abnormal_counter==7)              ///////状态异常报警，检测到保护开关跳开报警。
    	{	
            abnormal_counter=8;
    	    abnormal_ctl_flag=5;              ///////状态异常遥控标志
    	    save_abnormal=0x55;
            g_gRmtInfo[YX_EFS_ABNORMAL] = 1;
            KA1_ON;	             

	     //CreatNewSMS(ABN_CHECK);                      //产生保护开关跳开短信//张弢
            for(i=0;i<6;i++)
                g_sSoeData.m_gAbnBuff[i] = g_sRtcManager.m_gRealTimer[i];
            g_sSoeData.m_gAbnBuff[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
           
            CreatNewSMS(ABN_CHECK);                      //产生保护开关跳开短信
    	   /*  if(g_gProcCnt[PC_GPRS_MODEL]==0)
	    {
	        upload_interval[0]=(3*upload_interval_set)-1;      //////////////状态异常set 4 minutes
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
    if((Powerdown_counter == 0)&&(power_off_flag==1))    /////////信号源掉电恢复	
    {
       power_off_flag=0;
       g_gRmtInfo[YX_EFS_OFF] = 0;
       for(i=0;i<6;i++)
           g_sSoeData.m_gPowerONBuff[i] = g_sRtcManager.m_gRealTimer[i]; 
       g_sSoeData.m_gPowerONBuff[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
        
       save_power_off=0x66; 
       //KB1_OFF;//KB1改为投切灯
    }
    else if((Powerdown_counter >= 50)&&(power_off_flag==0))    /////////信号源掉电检测	(新电路板)			 
    {
        power_off_flag=1;	  
        power_ctl_flag=5;              ///////断电遥控标志   延时5秒钟
        save_power_off=0x55; 
        g_gRmtInfo[YX_EFS_OFF] = 1;
        //KB1_ON; //KB1改为投切灯           ///新电路板 	 
        //CreatNewSMS(POWER_OFF);                      //产生掉电短信//张弢
        
       for(i=0;i<6;i++)
            g_sSoeData.m_gPowerOFFBuff[i] = g_sRtcManager.m_gRealTimer[i];
        g_sSoeData.m_gPowerOFFBuff[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
        CreatNewSMS(POWER_OFF);                      //产生掉电短信
/* 
 	 if(g_gProcCnt[PC_GPRS_MODEL]==0)                                       //////////////肯定会发短信息，但是会不会发送GPRS数据还需要
   	 {
   	      upload_interval[1]=(3*upload_interval_set)-1;      //////////////掉电4 minutes
              upload_flag |= POWER_OFF;	
   	 }
         else
   	 {
   	      upload_interval[1]=0;      //////////////eight pulse set 4 minutes	
   	      upload_flag &= (~POWER_OFF);	
   	 }*/
    }
 //   YX_RMT();   //
    if(abnormal_ctl_flag==0)           ///////异常遥控标志       新电路板
       KA1_OFF;  
    //if(power_ctl_flag==0)              ///////断电遥控标志//KB1改为投切灯
       //KB1_OFF;  //KB1改为投切灯
    if(eight_ctl_flag==0)              ///////有效8脉冲标志
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
// **************检测开关位置异常 **************
    	if(KJa1==ka)//&&(g_gKON!=100)     /////////检测到硬遥信1
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
    	if(KJb1==kb)//&&(g_gKON!=100)     /////////检测到硬遥信1
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
    	if(KJc1==kc)//&&(g_gKON!=100)     /////////检测到硬遥信1
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

// **************    检测I0超时      **************
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
    
    if(KJa1==ka)     /////////检测到硬遥信1
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
    if(KJb1==kb)     /////////检测到硬遥信1
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
    if(KJc1==kc)     /////////检测到硬遥信1
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
//  函数名称   : InitClk
//  功能描述   : 系统用到的时钟进行初始化
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void InitClk(void)
{ 
//   unsigned int i;  
//    P5SEL |= 0x0C;                            // Port select XT2 
//    UCSCTL0 = 0x1F00;                           // Set lowest possible DCOx, MODx   *注：DCO =31 配合DCO更改为24M
//  
//    UCSCTL1 = DCORSEL_5;                      // Select range for 18MHz operation
//    UCSCTL2 = 732;                            // Set DCO Multiplier for 16MHz
//                                            // (N + 1) * FLLRef = Fdco
//                                            // (733 + 1) * 32768 = 24MHz
//    UCSCTL6 &= ~XT2OFF;                       // Enable XT2 even if not used
// //发现XT1晶振报错，由于晶振未使用，加程序软件关闭XT1  
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
    P1SEL = 0x00;   //1.0-1.7采集单点遥信，全部配置为输入
    P1DIR = 0x00;
    P2SEL = 0x00;   //2.0-2.7采集单点遥信，全部配置为输入
    P2DIR = 0x00;
    
    
    P3SEL = 0x3E;  //3.0- flash hold 3.1-SIMO 3.2-SOMI 3.3-CLK;3.4-TX0 3.5-RX0 3.6 CS1;3.7 CS0
    P3DIR = 0xDB;
    P3OUT = 0x01; //flash hold 需为高电平
     
    P4SEL = 0x00;  //4.0-RF电源 4.1-RF复位 4.2-RF测试(in) 4.3-GPRS电源  4.4-4.5 DI输入 4.6-BAT充电 4.7-BAT放电
    P4DIR = 0xCB;
    P4OUT = 0x00;

    P5SEL = 0xC0;  //P56 TXD1  P57 RXD1 和GPRS通讯  5.4 DI输入 5.5-电池活化输出
    P5DIR = 0x6F;
    P5OUT = 0x00;
    
    P6SEL = 0x0F;  //P60-P63采集AD量 P60-BATFU-AD0 P61-SAM_C P62-SAM_SUN P63-SAM_BAT P64-复位GPRS模块  P65-GPRS模块配置  P66-G_ON/OFF 打开/关闭 P67 电池负电压测试
    P6DIR = 0xFF;
    P6OUT = 0x00;
     
    P7SEL = 0x00; //7.2-flash_wp 7.3-eeprom_wp 7.5-WD 7.4-RUN LED
    P7DIR = 0xFF;
    P7OUT = 0x00;    

    P8SEL = 0x00; //8.0-8.2  8.4-8.5 遥信输入 P8.3 时钟中断 P8.6 硬遥信电源状态监测  P8.7 继电器控制输出
    P8DIR = 0x80; 
    P8OUT = 0x80;    

    P9SEL = 0x30; //P94 TXD2  P95 RXD2 9.1-9.2 I2C 9.0 继电器控制输出 9.3 架空24V电源输出
    P9DIR = 0xDF;
    P9OUT = 0x01;    

    P10SEL = 0x30;  //P10.0-10.2 AD采样控制 P10.3 硬遥信电源控制 P10.4 TXD3  P10.5 RXD3 设置口使用  
    P10DIR = 0xDF;
    P10OUT = 0x00;      
  
}



//==============================================================================
//  函数名称   : InitDin
//  功能描述   : 初始化开关量输入扫描
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void InitDin(void)
{
    unsigned long i,biti;
    unsigned long ExtIoStat = 0;//当前24个外部DIN输入的状态 
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

    
    //从CPU管脚提取对应DIN的IO管脚电平  
      	ExtIo1 = (P1IN & BIT0) +((P1IN & BIT2)>>1)+((P1IN & BIT4)>>2)+((P1IN & BIT6)>>3)+((P2IN & BIT0)<<4)
      	+((P2IN & BIT2)<<3)+((P2IN & BIT4)<<2)+((P2IN & BIT6)<<1); 
	
		ExtIo2=((P4IN & BIT4)>>4) +((P5IN & BIT4)>>3)+((P8IN & BIT1)<<1)+((P8IN & BIT4)>>1)+((P1IN & BIT1)<<3)
      	+((P1IN & BIT3)<<2)+((P1IN & BIT5)<<1)+(P1IN & BIT7); 
		ExtIo1=ExtIo1+(ExtIo2<<8);
      	ExtIoStat = ((P2IN & BIT1)>>1) +((P2IN & BIT3)>>2)+((P2IN & BIT5)>>3)+((P2IN & BIT7)>>4)+((P4IN & BIT5)>>1)
      	+((P8IN & BIT0)<<5)+((P8IN & BIT2)<<4)+((P8IN & BIT5)<<2); 
      	ExtIoStat = ((ExtIoStat << 16) & 0x00ff0000) + ExtIo1;//把24个DIN的IO状态依次赋值到该变量 
		ExtIoStat = ExtIoStat | 0xff000000;
        g_gExtDinStat = ~ExtIoStat;
		
		
     	
      	g_ucRefYxYcTs |= BIT0;
		for(i = 0; i < EXT_DIN_NUM; i++)
      	{
			if(((g_gRunPara[YX_DSl]+g_gRunPara[YX_DSh]*65536)&((unsigned long)1<<i))>0)//双点
			{
				if(((g_gExtDinStat  >> 2*(i/2)) & 0x03) == 1 )
				{
								//CreatNewSoe(RI_SD_YX1 + i/2,(2),1);				
					SetRIStatus(RI_ZD_DYX1 +i/2,2);
							
				}
							
				else if(((g_gExtDinStat  >> 2*(i/2)) & 0x03) == 2 )
            	{
                    		
					SetRIStatus(RI_ZD_DYX1 +i/2, 1);
													
				}//分
            	else if(((g_gExtDinStat  >> 2*(i/2)) & 0x03) == 0 )
            	{
                    			
					SetRIStatus(RI_ZD_DYX1 +i/2, 0);	
									
								
				}//过渡
            	else 
            	{
					SetRIStatus(RI_ZD_DYX1 +i/2, 3);	
														
				}  //错误
			}
			else
			{
				 biti = 1; //原因是直接biti=1<<i,当i=16时出问题 
          		biti = biti << i;
          		if( (g_gExtDinStat  & biti) == biti)//为产生DIN的电平
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
//  函数名称   : ScanDin
//  功能描述   : 开关量输入扫描函数，并根据扫描的结果产生对应的SOE和遥信变位信息
//  输出参数   : 
//  返回值     : 1=有变位 0=无变位
//  其他说明   : 
//  作者       ：
//==============================================================================
bool ScanDin(void)
{
    unsigned long i,m,n,ExtIoStat = 0;//当前24个外部DIN输入的状态
    volatile unsigned int ExtIo1=0,ExtIo2=0,j;
    unsigned int f1_cnt = 0xff;
   static unsigned long lExtDinStat_new;
   unsigned char ucReadDiPowerSt = 0;
   unsigned long biti;
	
   //从CPU管脚提取对应DIN的IO管脚电平
	  OPEN_DI_POWER();
   
   	  //delay(3000);
        if (g_gRunPara[Terminal_Type]==AERIAL_TERMINAL)
        {//架空型不进行硬遥信电源状态监测
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
			    if(((g_gRunPara[YX_DSl]+g_gRunPara[YX_DSh]*65536)&((unsigned long)1<<i)) > 0 )//// 双点其中一个变位时，如果另一个已经处在变位延时中，则两个都置1，重新延时
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
				    if(((g_gRunPara[YX_DSl]+g_gRunPara[YX_DSh]*65536)&((unsigned long)1<<i))>0)//双点
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
							    }//分
                    		    else if(((g_gExtDinStat  >> 2*(i/2)) & 0x03) == 0 )
                    		    {
								    CreatNewSoe(RI_ZD_DYX1 + (i/2),(0),1);		
							    }//过渡
                    		    else 
                    		    {
								    CreatNewSoe(RI_ZD_DYX1 + (i/2),(3),1);	
							    }  //错误
								
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
								}//分
                    		    else if(((g_gExtDinStat  >> 2*(i/2)) & 0x03) == 0 )
                    		    {
									CreatNewSoe(RI_ZD_DYX1 + (i/2),(0),1);																		
							    }//过渡
                    		    else 
                    		    {                    			
									CreatNewSoe(RI_ZD_DYX1 + (i/2),(3),1);	
									
							    }  //错误
								
						    }
						    else
						    {
							    g_gExtDinYesTimer[i]=0;
							    g_gExtDinYesTimer[i-1]=0;
						    }
					    }
				    }
				    else  //单点
				    {
					    g_gExtDinYesTimer[i]=0;
					    g_gExtDinStat ^=((unsigned long)1<<i);
 			
					  if(((g_gRunPara[YX_MODEl]+g_gRunPara[YX_MODEh]*65536)& ((unsigned long)1<<i) ) > 0 )// 脉冲模式
                      {
                        // 如果脉冲屏蔽时间没到，或者遥信从 1 -> 0 ，不触发故障
                      	if(g_gDIMcLock[i] > 0)
                        {
							f1_cnt = 0xff;
                        }
                        else if(( g_gExtDinStat & ((unsigned long)1 << i)) != ((unsigned long)1 << i)) 
                        {
                          f1_cnt = 0xff;//待测试
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
          				biti = 1; //原因是直接biti=1<<i,当i=16时出问题 
          		        biti = biti << i;
          		        if( (g_gExtDinStat  & biti) == biti)//为产生DIN的电平
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
            
            	YX_delay_clear_flag = 0;    //防止在 中断里重复 清空延时计数
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
//  函数名称   : ActOut
//  功能描述   : 对具体硬件对象的操作，根据操作类型，产生出口
//  输入参数   : OutType-操作出口号
//                 OutValue-动作值，ON或OFF
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
    				efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////闭锁17分钟
    				latch_upload_flag=0x55;      	
    				uart0_event_flag=0;         ///////在这里置0，是为了让状态量最早显示
    				g_gRmtInfo[YX_EFS_LATCH] = 1;   //置闭锁遥信位 
    				chongfa=0;	moniguzhang=0;
    				g_gRmtMeas[RM_ACT_NUM] = 0;
				g_gRmtInfo[YX_EFS_ACT] = 0;   //投切状态 遥信置0	
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
			g_gRmtInfo[YX_EFS_LATCH] = 0;   //置闭锁遥信位 
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
    				efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////闭锁17分钟
    				latch_upload_flag=0x55;      	
    				uart0_event_flag=0;         ///////在这里置0，是为了让状态量最早显示
    				g_gRmtInfo[YX_EFS_LATCH] = 1;   //置闭锁遥信位 
    				chongfa=0;	moniguzhang=0;
    				g_gRmtMeas[RM_ACT_NUM] = 0;
				g_gRmtInfo[YX_EFS_ACT] = 0;   //投切状态 遥信置0	
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
    				efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];		 	         ///////闭锁17分钟
    				latch_upload_flag=0x55;      	
    				uart0_event_flag=0;         ///////在这里置0，是为了让状态量最早显示
    				g_gRmtInfo[YX_EFS_LATCH] = 1;   //置闭锁遥信位 
    				chongfa=0;	moniguzhang=0;
    				g_gRmtMeas[RM_ACT_NUM] = 0;
				g_gRmtInfo[YX_EFS_ACT] = 0;   //投切状态 遥信置0	
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


