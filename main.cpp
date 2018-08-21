#ifndef _MAIN

#define _MAIN
#include "DataStruct.h"
#include "Main.h"

  CBJ101S mBJ101S0;
  CBJ101S mBJ101S1;

#ifdef USE_GB104S
 CGB104S mGB104S;
#endif

void app(void)@"APPLICATION"
{
    WDTCTL = WDTPW + WDTHOLD;//禁止芯片自带的看门狗，采用外部看门狗
    InitSys();
    //strcpy(ComtrderCfg1,ComtrderCfg);
      if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(0,11,null);
	FEED_WATCH_DOG();delayms(3000);     
      if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(5,11,null);
	//WDG_CLR;delayms(3000);   WDG_CLR;    
	   //if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(84,11,null);//读CSQ	
    //启动时取出重启次数加1并保存
   /* CAT_SpiReadWords(EEPADD_RSTARTNUM, 1, &g_RstartNum);
    if(g_RstartNum >= 0xfffe) g_RstartNum = 0;
    FEED_WATCH_DOG();
    g_RstartNum++;
    CAT_SpiWriteWords(EEPADD_RSTARTNUM, 1, &g_RstartNum);*/
    FEED_WATCH_DOG();
	//SaveLOG(LOG_RESET,1);
    while(1)
    {    
        /*WatchDog();
        HandleTimerEvent();
        CalcuRmtMeas();//整理AD采集数据     
        if(g_sRtcManager.m_ucRtcSynFlag == YES) //每4分钟从时钟芯片读取一次时间，该标志在定时器中断中计数设标志
        {
            ReadRealTime(1);//从RTC芯片中读取系统时钟，并更新SysTime中的时间
            g_sRtcManager.m_ucRtcSynFlag = NO;
        }
        
        //OpenBatSmsGprs();
        SetYxTrans();
        SetYcTrans(); 
        SaveSoeData();
        #ifndef GETSOEFROMRAM
          CopySoeToFlash();
        #endif
        
        if(pDbg != null) pDbg->Run();
        if(pGprs != null) pGprs->Run();

        SaveCfgPara();*/
        SaveActRecData();
        FEED_WATCH_DOG();
		if(g_STimeout == ON)
			{
			g_STimeout = OFF;
			if(pGprs->m_PaWaitflag_lubo == ON)
   				{
      			if(pGprs->m_PaWaitCt_lubo > 0)
      				{//ODU写参数重发计时
          			pGprs->m_PaWaitCt_lubo--;
          			if(pGprs->m_PaWaitCt_lubo <= 0)
          				{
            				pGprs->m_PaWaitflag_lubo = TRUE;
            				//return TRUE;
          				}
       				}
    			}
         	if(pDbg->m_PaWaitflag_lubo == ON)
   				{
      			if(pDbg->m_PaWaitCt_lubo > 0)
      				{//ODU写参数重发计时
          			pDbg->m_PaWaitCt_lubo--;
          			if(pDbg->m_PaWaitCt_lubo <= 0)
          				{
            			pDbg->m_PaWaitflag_lubo = TRUE;
            				//return TRUE;
          				}
       				}
    			}					
			}
	SaveActRecData();
	 if(g_NolinkReset>1440)
	 	{
	 	g_NolinkReset=0;
	 	InitCommObj();
	 	}
        if(g_sRtcManager.m_ucRtcSynFlag == YES) //每4分钟从时钟芯片读取一次时间，该标志在定时器中断中计数设标志
        {
            ReadRealTime();//从RTC芯片中读取系统时钟，并更新SysTime中的时间
            g_sRtcManager.m_ucRtcSynFlag = NO;
#ifndef YN_101S
	    if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(84,11,null);//读CSQ	
#endif	
			CheckCfgERR();
        }
    SaveActRecData();
        CalcuRmtMeas();//有效值计算，并更新对应的遥测值
    SaveActRecData();
        ScanDin();
	 YCthCross();//遥测越限判断	
	 SaveActRecData();
     ProtLogic();   
	if(newsms_8pluse == ON)
		{	
		newsms_8pluse = OFF;
 		CreatNewSMS(EIGHT_PULSE);
		}
 	if(newsms_abn == ON)
		{	
		newsms_abn = OFF;
 		CreatNewSMS(ABN_CHECK);
		}
	SaveActRecData();
	FEED_WATCH_DOG();
        Comm_LED_101(); //液晶通信
       FEED_WATCH_DOG();
        //if(g_ucGPRSState==GPRSState_IDLE)
        SaveActRecData();
	 Comm_GPRS_SMS();////张弢0330 如串口在空闲状态，则可以发送SMS
        
        if(pDbg != null) pDbg->Run();
        if(pGprs != null) pGprs->Run();
        SaveActRecData();
        SaveCfgPara();
		SaveActRecData();

            if(g_sRecData.m_ucRecSavingFlag == YES)//如果有新的录波数据则分批次保存到FLASH中
            {
                SaveRecData();//按照COMTRADE格式整理录波数据，并分批次保存到Flash中
                g_sRecData.m_ucRecSavingFlag = OFF;
				if((g_gDebugP[Debug_ALLREC]==0x55)&&(g_sRecData.m_ucActRecStart == CLOSE))//正常录波模式
		      			{
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
		      			}
            } 
	SaveActRecData();	
        ClkChange();    //在实际试验过程发现MSP430容易出现由外部晶振自动切换为内部DCO，因此需要及时发现并切回来。
        FEED_WATCH_DOG();
        SaveSoeDataRepeat();

        //RmInfoChk();//张弢 移入主循环，否则栈太大无法中断嵌套
        g_gRmtInfo[YX_SYSRESET] =0;        
        if((g_gSaveload>=g_gRunPara[RP_FLOAD_T] )&&(g_gRunPara[RP_FLOAD_T] !=0))//每隔一段时间存储负荷记录
        	{
        	g_gSaveload=0;
		SaveLoad();	
        SaveActRecData();	}
		SaveFlashLOG();
		SaveActRecData();
        if(g_sRecData.m_EraseBlock == ON)
        	{
        	g_sRecData.m_EraseBlock = OFF;
		long ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;    
  		Block_Erase(ulAddr);//ERASE 1个BLOCK 
  		delayms(100);WatchDog();
  		Block_Erase(ulAddr+0x10000);//ERASE 1个BLOCK 
  		delayms(100);WatchDog();
  		Block_Erase(ulAddr+0x20000);//ERASE 1个BLOCK 
  		delayms(100);WatchDog();
  		Block_Erase(ulAddr+0x30000);//ERASE 1个BLOCK 
  		delayms(100);WatchDog(); 
  		Block_Erase(ulAddr+0x40000);//ERASE 1个BLOCK 
  		delayms(100);WatchDog();
  		Block_Erase(ulAddr+0x50000);//ERASE 1个BLOCK 
  		delayms(100);WatchDog();
  		Block_Erase(ulAddr+0x60000);//ERASE 1个BLOCK 
 		 delayms(100);WatchDog();
  		Block_Erase(ulAddr+0x70000);//ERASE 1个BLOCK 
  		delayms(100);WatchDog(); 
  		Block_Erase(ulAddr+0x80000);//ERASE 1个BLOCK 
  		delayms(100);WatchDog();   
        	}
		SaveActRecData();
        if(g_sRecData.m_EraseBlock == YES)
        	{
        	g_sRecData.m_EraseBlock = OFF;
		long ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;    
  		Sector_Erase(ulAddr);//ERASE 1个BLOCK 
  		delayms(100);WatchDog();
  		Sector_Erase(ulAddr+0x1000);//ERASE 1个BLOCK 
  		delayms(100);WatchDog();
  		Sector_Erase(ulAddr+0x2000);//ERASE 1个BLOCK 
  		delayms(100);WatchDog();
  		Sector_Erase(ulAddr+0x3000);//ERASE 1个BLOCK 
  		delayms(100);WatchDog(); 
  		Sector_Erase(ulAddr+0x4000);//ERASE 1个BLOCK 
  		delayms(100);WatchDog();
  		Sector_Erase(ulAddr+0x5000);//ERASE 1个BLOCK 
  		delayms(100);WatchDog();
  		Sector_Erase(ulAddr+0x6000);//ERASE 1个BLOCK 
 		 delayms(100);WatchDog();
  		Sector_Erase(ulAddr+0x7000);//ERASE 1个BLOCK 
  		delayms(100);WatchDog(); 

        	}		
    }//end of while(1)
    

}

//==============================================================================
//  函数名称   : CheckInLPM3
//  功能描述   : 判断是否进入低功耗
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : TRUE = 可进入低功耗 FALSE = 还有事件未处理，不能进入低功耗
//  其他说明   : 
//  作者       ：
//==============================================================================
bool CheckInLPM3(void)
{
   // int iCmId = 0;


  return TRUE;
}



//==============================================================================
//  函数名称   : InitCommObj
//  功能描述   : 初始化端口对象
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : 
//  其他说明   : 
//  作者       ：
//==============================================================================
void InitCommObj(void)
{
  //初始化通道，给各个串口分配对象==============================================
    //如果通道号配置错误，可以将DEBUG宏去掉，让程序处于正式运行状态，
    //此状态下调试串口默认被分配到c

     g_CmIdDBG = 1;//运行时调试通道默认配置到串口1,不认参数设置中的值
     g_CmIdGPRS = 2;
  //========================================================================
     pDbg = null;
     pGprs = null;
     pRf = null;
     pOdu = null;
     pOdu1 = null;
    int iCmId = 0;
    
    //调试口
    iCmId = g_CmIdDBG;
    if( iCmId != 4)
    {
      pDbg = &mBJ101S0;
      pDbg->Init(iCmId);
    }

    //GPRS口
    iCmId = g_CmIdGPRS;
    if(iCmId!= 4)
    {
      #ifdef USE_GB104S
      pGprs = &mGB104S;
      pGprs->Init(iCmId);
      #else
      pGprs = &mBJ101S1;
      pGprs->Init(iCmId);
      #endif
     }
    

   //端口协议的4个参数进行备份
  //  for(int i = 0;i < 4;i++)
    //    g_unParaObjComBk[i] = g_ucPara101[IECP_OBJ_COM0 + i];
		if(pDbg != null)
			pDbg->m_SendSoeAdr=g_unSSoeSaveE2ROMPtr;
		if(pGprs!= null)
			pGprs->m_SendSoeAdr=g_unSSoeSaveE2ROMPtr;    

   g_GPRSSendLink = 55;
    return;
}

//==============================================================================
//  函数名称   : InitSys
//  功能描述   : CPU开机所有初始化工作，包括各个外设、参数、模块的初始化，初始化结束后启动中断，进入大循环
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void InitSys(void)
{
   // unsigned char Data[5];
   // static unsigned char ucPcData[5] = {1,2,3,4,5};
    InitClk();//系统时钟初始化
    WDG_SET;
    InitPort();//IO端口初始化
  
    

    UCB2SPIInit();  //SPI总线初始化
    UCB0SPIInit();  //SPI总线初始化

    InitComm();//通信初始化,包括RF通信和GPRS通信、232通信。
    FEED_WATCH_DOG();
    InitRtc();//系统时钟初始化
    FEED_WATCH_DOG();
    InitcfgPara();//系统参数初始化，包括系统用到的参数初始化和从存储器中读取出来的参数。
    FEED_WATCH_DOG();
    InitDataProc();       //必须在参数初始化之后，因为有用到运行参数
    InitSCI();
    InitDMA();      //DMA三通道初始化   

    InitAdc();//AD采样初始化,定义AD采样顺序   //由于在TIMER初始化接收后，立即启动AD采样，保证第一次进TIMER中断时，已经完成一次采样，因此把ADC初始化转移到TIMER之前
    InitTimer();//定时器初始化，包括1ms中断、0.625ms中断、0.125ms中断、捕获等定时器的初始化
    InitCommObj();
    __bis_SR_register(GIE);       //使能全局中断
    delayms(500);
    ReadLEDVer();  // 读取液晶版本号
   g_unSms_s = 0;
   g_gRmtInfo[YX_BREAK]=0;//复位后，断线检测遥信也复位
   g_gRmtInfo[YX_SYSRESET] =1;
   if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(84,11,null);//读CSQ
    
}



void WatchDog(void)
{
 FEED_WATCH_DOG();
}
void ExitGprsSet(void)
{
    
}
void HandleTimerEvent(void)
{

}
//1s定时器中断与类对象中定时函数的接口
bool TimeOut(int flag)
{
      bool ret[5] = {0};
      bool RetAll = 0;
      if(pDbg != null)  ret[0] = pDbg->DoTimeOut(0);
      if(pGprs != null) ret[1] = pGprs->DoTimeOut(0);
      if(pRf != null)   ret[2] = pRf->DoTimeOut(0);
      if(pOdu != null)  ret[3] = pOdu->DoTimeOut(0);
      if(pOdu1 != null) ret[4] = pOdu1->DoTimeOut(0);
      
      RetAll = ret[0]||ret[1]||ret[2]||ret[3]||ret[4];
    return RetAll;
}
 //==============================================================================
//  函数名称   : RefreshPara
//  功能描述   :根据g_ucParaChang变化更新各对象内存中的参数，一个对象的通信参数变化时其他对象的内存区也需要更新
//  输入参数   : 
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       
//==============================================================================
 void RefreshPara(void)
 {
    if(((g_ucParaChang & BIT0) == BIT0) //运行参数
    || ((g_ucParaChang & BIT2) == BIT2)//遥信地址点表
    || ((g_ucParaChang & BIT3) == BIT3))//遥测参数
    {
      
       if((g_ucParaChang & BIT2) == BIT2)
          InitTsYxInfo();
       if((g_ucParaChang & BIT3) == BIT3)
          InitTsYcInfo();
          
       if(pDbg != null)
           pDbg->SetEqpInfo();
       if(pGprs != null)
          pGprs->SetEqpInfo();
    }
    //IEC101参数设置
    if((g_ucParaChang & BIT1) == BIT1)
    {    	
        if(pDbg != null)
           pDbg->initpara();
       if(pGprs != null)
          pGprs->initpara();
          
       //若是端口重新分配，则需要重判断类对象的启动或停止
        BYTE bChange= 0;
       for(int i = 0;i < 4;i++)
       {
          //if(g_unParaObjComBk[i] != g_ucPara101[IECP_OBJ_COM0 + i])
          {
            bChange = 1;
            break;
           }
       }
       if(bChange == 1)
          InitCommObj();
       
    }
    
    return;
 }
//==============================================================================
//  函数名称   : InitGPRS
//  功能描述   : GPRS配置初始化
//  输入参数   : <无>
//                
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================

void InitGPRS(void)
{
   
}

void CloseGPRS(BYTE bDa,BYTE Close_Type)
{
  

}
void OpenGPRS(BYTE bDa)
{
 
  
}
//判断GPRS通道是否有数据需要上传
//1 :有数据未上传
//0: 无数据上传
//提前关GPRS时需要此函数进行判断，发送提前关主板电源指令到ODU也需要此判断
BOOL CheckHaveDataToSd()
{
   if(pGprs == null)
    return 0;//GPRS通道都没配置，返回无数据要上传

  if((pGprs->m_WaitConfTpId == M_SP_NA) || (pGprs->m_WaitConfTpId == M_DP_NA)
    || (pGprs->m_WaitConfTpId == M_SP_TB) || (pGprs->m_WaitConfTpId == M_DP_TB)
    || g_ucRefYxYcTs || pGprs->m_SendYcFlag || pGprs->m_SendChgYcFlag
    || pGprs->m_reSendDelay || pGprs->m_resendflag
    || (g_yxChangeflag & 0x03c0))// 0011 1100 0000  SSMS,DSMS标识位不能加入，因为不一定发短信
     return 1;
  
  return 0;
}

#endif
