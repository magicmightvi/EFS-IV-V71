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
    WDTCTL = WDTPW + WDTHOLD;//��ֹоƬ�Դ��Ŀ��Ź��������ⲿ���Ź�
    InitSys();
    //strcpy(ComtrderCfg1,ComtrderCfg);
      if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(0,11,null);
	FEED_WATCH_DOG();delayms(3000);     
      if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(5,11,null);
	//WDG_CLR;delayms(3000);   WDG_CLR;    
	   //if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(84,11,null);//��CSQ	
    //����ʱȡ������������1������
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
        CalcuRmtMeas();//����AD�ɼ�����     
        if(g_sRtcManager.m_ucRtcSynFlag == YES) //ÿ4���Ӵ�ʱ��оƬ��ȡһ��ʱ�䣬�ñ�־�ڶ�ʱ���ж��м������־
        {
            ReadRealTime(1);//��RTCоƬ�ж�ȡϵͳʱ�ӣ�������SysTime�е�ʱ��
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
      				{//ODUд�����ط���ʱ
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
      				{//ODUд�����ط���ʱ
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
        if(g_sRtcManager.m_ucRtcSynFlag == YES) //ÿ4���Ӵ�ʱ��оƬ��ȡһ��ʱ�䣬�ñ�־�ڶ�ʱ���ж��м������־
        {
            ReadRealTime();//��RTCоƬ�ж�ȡϵͳʱ�ӣ�������SysTime�е�ʱ��
            g_sRtcManager.m_ucRtcSynFlag = NO;
#ifndef YN_101S
	    if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(84,11,null);//��CSQ	
#endif	
			CheckCfgERR();
        }
    SaveActRecData();
        CalcuRmtMeas();//��Чֵ���㣬�����¶�Ӧ��ң��ֵ
    SaveActRecData();
        ScanDin();
	 YCthCross();//ң��Խ���ж�	
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
        Comm_LED_101(); //Һ��ͨ��
       FEED_WATCH_DOG();
        //if(g_ucGPRSState==GPRSState_IDLE)
        SaveActRecData();
	 Comm_GPRS_SMS();////�ŏ|0330 �紮���ڿ���״̬������Է���SMS
        
        if(pDbg != null) pDbg->Run();
        if(pGprs != null) pGprs->Run();
        SaveActRecData();
        SaveCfgPara();
		SaveActRecData();

            if(g_sRecData.m_ucRecSavingFlag == YES)//������µ�¼������������α��浽FLASH��
            {
                SaveRecData();//����COMTRADE��ʽ����¼�����ݣ��������α��浽Flash��
                g_sRecData.m_ucRecSavingFlag = OFF;
				if((g_gDebugP[Debug_ALLREC]==0x55)&&(g_sRecData.m_ucActRecStart == CLOSE))//����¼��ģʽ
		      			{
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
		      			}
            } 
	SaveActRecData();	
        ClkChange();    //��ʵ��������̷���MSP430���׳������ⲿ�����Զ��л�Ϊ�ڲ�DCO�������Ҫ��ʱ���ֲ��л�����
        FEED_WATCH_DOG();
        SaveSoeDataRepeat();

        //RmInfoChk();//�ŏ| ������ѭ��������ջ̫���޷��ж�Ƕ��
        g_gRmtInfo[YX_SYSRESET] =0;        
        if((g_gSaveload>=g_gRunPara[RP_FLOAD_T] )&&(g_gRunPara[RP_FLOAD_T] !=0))//ÿ��һ��ʱ��洢���ɼ�¼
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
  		Block_Erase(ulAddr);//ERASE 1��BLOCK 
  		delayms(100);WatchDog();
  		Block_Erase(ulAddr+0x10000);//ERASE 1��BLOCK 
  		delayms(100);WatchDog();
  		Block_Erase(ulAddr+0x20000);//ERASE 1��BLOCK 
  		delayms(100);WatchDog();
  		Block_Erase(ulAddr+0x30000);//ERASE 1��BLOCK 
  		delayms(100);WatchDog(); 
  		Block_Erase(ulAddr+0x40000);//ERASE 1��BLOCK 
  		delayms(100);WatchDog();
  		Block_Erase(ulAddr+0x50000);//ERASE 1��BLOCK 
  		delayms(100);WatchDog();
  		Block_Erase(ulAddr+0x60000);//ERASE 1��BLOCK 
 		 delayms(100);WatchDog();
  		Block_Erase(ulAddr+0x70000);//ERASE 1��BLOCK 
  		delayms(100);WatchDog(); 
  		Block_Erase(ulAddr+0x80000);//ERASE 1��BLOCK 
  		delayms(100);WatchDog();   
        	}
		SaveActRecData();
        if(g_sRecData.m_EraseBlock == YES)
        	{
        	g_sRecData.m_EraseBlock = OFF;
		long ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;    
  		Sector_Erase(ulAddr);//ERASE 1��BLOCK 
  		delayms(100);WatchDog();
  		Sector_Erase(ulAddr+0x1000);//ERASE 1��BLOCK 
  		delayms(100);WatchDog();
  		Sector_Erase(ulAddr+0x2000);//ERASE 1��BLOCK 
  		delayms(100);WatchDog();
  		Sector_Erase(ulAddr+0x3000);//ERASE 1��BLOCK 
  		delayms(100);WatchDog(); 
  		Sector_Erase(ulAddr+0x4000);//ERASE 1��BLOCK 
  		delayms(100);WatchDog();
  		Sector_Erase(ulAddr+0x5000);//ERASE 1��BLOCK 
  		delayms(100);WatchDog();
  		Sector_Erase(ulAddr+0x6000);//ERASE 1��BLOCK 
 		 delayms(100);WatchDog();
  		Sector_Erase(ulAddr+0x7000);//ERASE 1��BLOCK 
  		delayms(100);WatchDog(); 

        	}		
    }//end of while(1)
    

}

//==============================================================================
//  ��������   : CheckInLPM3
//  ��������   : �ж��Ƿ����͹���
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : TRUE = �ɽ���͹��� FALSE = �����¼�δ�������ܽ���͹���
//  ����˵��   : 
//  ����       ��
//==============================================================================
bool CheckInLPM3(void)
{
   // int iCmId = 0;


  return TRUE;
}



//==============================================================================
//  ��������   : InitCommObj
//  ��������   : ��ʼ���˿ڶ���
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : 
//  ����˵��   : 
//  ����       ��
//==============================================================================
void InitCommObj(void)
{
  //��ʼ��ͨ�������������ڷ������==============================================
    //���ͨ�������ô��󣬿��Խ�DEBUG��ȥ�����ó�������ʽ����״̬��
    //��״̬�µ��Դ���Ĭ�ϱ����䵽c

     g_CmIdDBG = 1;//����ʱ����ͨ��Ĭ�����õ�����1,���ϲ��������е�ֵ
     g_CmIdGPRS = 2;
  //========================================================================
     pDbg = null;
     pGprs = null;
     pRf = null;
     pOdu = null;
     pOdu1 = null;
    int iCmId = 0;
    
    //���Կ�
    iCmId = g_CmIdDBG;
    if( iCmId != 4)
    {
      pDbg = &mBJ101S0;
      pDbg->Init(iCmId);
    }

    //GPRS��
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
    

   //�˿�Э���4���������б���
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
//  ��������   : InitSys
//  ��������   : CPU�������г�ʼ�������������������衢������ģ��ĳ�ʼ������ʼ�������������жϣ������ѭ��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void InitSys(void)
{
   // unsigned char Data[5];
   // static unsigned char ucPcData[5] = {1,2,3,4,5};
    InitClk();//ϵͳʱ�ӳ�ʼ��
    WDG_SET;
    InitPort();//IO�˿ڳ�ʼ��
  
    

    UCB2SPIInit();  //SPI���߳�ʼ��
    UCB0SPIInit();  //SPI���߳�ʼ��

    InitComm();//ͨ�ų�ʼ��,����RFͨ�ź�GPRSͨ�š�232ͨ�š�
    FEED_WATCH_DOG();
    InitRtc();//ϵͳʱ�ӳ�ʼ��
    FEED_WATCH_DOG();
    InitcfgPara();//ϵͳ������ʼ��������ϵͳ�õ��Ĳ�����ʼ���ʹӴ洢���ж�ȡ�����Ĳ�����
    FEED_WATCH_DOG();
    InitDataProc();       //�����ڲ�����ʼ��֮����Ϊ���õ����в���
    InitSCI();
    InitDMA();      //DMA��ͨ����ʼ��   

    InitAdc();//AD������ʼ��,����AD����˳��   //������TIMER��ʼ�����պ���������AD��������֤��һ�ν�TIMER�ж�ʱ���Ѿ����һ�β�������˰�ADC��ʼ��ת�Ƶ�TIMER֮ǰ
    InitTimer();//��ʱ����ʼ��������1ms�жϡ�0.625ms�жϡ�0.125ms�жϡ�����ȶ�ʱ���ĳ�ʼ��
    InitCommObj();
    __bis_SR_register(GIE);       //ʹ��ȫ���ж�
    delayms(500);
    ReadLEDVer();  // ��ȡҺ���汾��
   g_unSms_s = 0;
   g_gRmtInfo[YX_BREAK]=0;//��λ�󣬶��߼��ң��Ҳ��λ
   g_gRmtInfo[YX_SYSRESET] =1;
   if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(84,11,null);//��CSQ
    
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
//1s��ʱ���ж���������ж�ʱ�����Ľӿ�
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
//  ��������   : RefreshPara
//  ��������   :����g_ucParaChang�仯���¸������ڴ��еĲ�����һ�������ͨ�Ų����仯ʱ����������ڴ���Ҳ��Ҫ����
//  �������   : 
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       
//==============================================================================
 void RefreshPara(void)
 {
    if(((g_ucParaChang & BIT0) == BIT0) //���в���
    || ((g_ucParaChang & BIT2) == BIT2)//ң�ŵ�ַ���
    || ((g_ucParaChang & BIT3) == BIT3))//ң�����
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
    //IEC101��������
    if((g_ucParaChang & BIT1) == BIT1)
    {    	
        if(pDbg != null)
           pDbg->initpara();
       if(pGprs != null)
          pGprs->initpara();
          
       //���Ƕ˿����·��䣬����Ҫ���ж�������������ֹͣ
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
//  ��������   : InitGPRS
//  ��������   : GPRS���ó�ʼ��
//  �������   : <��>
//                
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
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
//�ж�GPRSͨ���Ƿ���������Ҫ�ϴ�
//1 :������δ�ϴ�
//0: �������ϴ�
//��ǰ��GPRSʱ��Ҫ�˺��������жϣ�������ǰ�������Դָ�ODUҲ��Ҫ���ж�
BOOL CheckHaveDataToSd()
{
   if(pGprs == null)
    return 0;//GPRSͨ����û���ã�����������Ҫ�ϴ�

  if((pGprs->m_WaitConfTpId == M_SP_NA) || (pGprs->m_WaitConfTpId == M_DP_NA)
    || (pGprs->m_WaitConfTpId == M_SP_TB) || (pGprs->m_WaitConfTpId == M_DP_TB)
    || g_ucRefYxYcTs || pGprs->m_SendYcFlag || pGprs->m_SendChgYcFlag
    || pGprs->m_reSendDelay || pGprs->m_resendflag
    || (g_yxChangeflag & 0x03c0))// 0011 1100 0000  SSMS,DSMS��ʶλ���ܼ��룬��Ϊ��һ��������
     return 1;
  
  return 0;
}

#endif
