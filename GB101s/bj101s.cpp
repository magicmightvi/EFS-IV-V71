/*------------------------------------------------------------------------
 Module:            bj101s.cpp
 Author:            linxueqin
 Project:           规约平台
 State:             V1.0
 Creation Date:     2014-4-24
 Description:       GB101规约--slave
------------------------------------------------------------------------*/

#include "Bj101s.h"
#include "..\Main.h"

#ifdef INCLUDE_BJ101_S

#undef BJ101_GZTEST

#define GRP_YCNUM   50//128         //每组最多发送遥测个数，增加加密，72改为50
#define GRP_YXNUM   127//128         //每组最多发送遥信字节

#define FRM_YCNUM    72//64         //每帧报文最多发送遥测个数
#define FRM_YXNUM    127//100        //每帧报文最多发送遥信个数


/***************************************************************
    Function：CBJ101S
        构造函数，暂空
    参数：无

    返回：无
***************************************************************/
CBJ101S::CBJ101S() : CPrtcSms()//
{
  m_YkWaitCount=0;//遥控执行命令发给rf后等待的时间 单位s
  m_LuboRe10Flag= 0;
  time_delay_set = 0;
}


void CBJ101S::Run(void)
{
   // if(!m_SendChgYcFlag && g_SendYc )//&& (g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_SEND_CHANGEYC])
     // CheckChangeYC();//需要放在OpenBatSmsGprs函数的前边
  /*  if(m_uartId == g_CmIdGPRS) 
      OpenBatSmsGprs();//放到run函数的最前端*/
    RcvData(&m_uartId);
    DoCommSendIdle();


}
/***************************************************************
    Function：Init
        规约初始化
    参数：
    返回：TRUE 成功，FALSE 失败
***************************************************************/
BOOL CBJ101S::Init(WORD uartId)
{
    BOOL rc;
    m_uartId = uartId;
    rc = CPrtcSms::Init(1);
    if (!rc)
    {
      return FALSE;
    }
    
    m_wSendYcNum = 0;
    m_wSendYxNum = 0;
    m_wSendZJNum = 0;
//      m_wSendPaNum = 0;
    m_wSendDYxNum=0;
    m_YKflag=0;
    m_YKSelectAck=0;
    m_callallflag=0;
    m_acdflag=0;
    m_zdflag=0;
    m_linkflag=0;
    m_testflag=0;
    m_timeflag = 0;m_timeREQflag = 0;
    m_time =0;
    m_resetflag = 0;
    m_groupflag=0;
    m_YxYcGrpFlag = 0;
    m_recfalg = 0;

    m_timeflag=0;m_timeREQflag = 0;
    m_YKstop=0;
    m_BeatFlag = 0;
    m_SendYcFlag = 0;
    //m_SendChgYcFlag = 0;
    m_SendStatusFlag = 0;
    m_SCosHeadPtr = 0;
    m_DCosHeadPtr = 0;
//--------------------------------------------------------------------------	
    //m_SSoeHeadPtr = EEPADD_SOESTARTADR;
    //Init_Rec();
    unsigned int untemp[4];
	CAT_SpiReadWords(EEPADD_SOESEND_E2ROMADR, 4, untemp);  //保存到EEPROM中
	if(pDbg != null)
		pDbg->m_SSoeHeadPtr=untemp[0];
	if(pGprs!= null)
		pGprs->m_SSoeHeadPtr=untemp[2];
	if(untemp[0]!=untemp[1])
		DelALLSOE();
	if(untemp[2]!=untemp[3])
		DelALLSOE();	
	if((untemp[0]<EEPADD_SOESTARTADR)||(untemp[0]>=EEPADD_SOEENDADR))
		DelALLSOE();
	if((untemp[2]<EEPADD_SOESTARTADR)||(untemp[2]>=EEPADD_SOEENDADR))
		DelALLSOE();    
//--------------------------------------------------------------------------
    m_SSoeHeadPtrBk = 0;
    m_DSoeHeadPtr = 0;
    m_DSoeHeadPtrBk = 0;
    m_WaitConfTpId = 0;
    m_comtradeflag =0;
	m_comtradeflag_YN =0;
    if(m_uartId == g_CmIdDBG)
    {
    //对应g_yxChangeflag 
      m_SYxBit = BIT0;
      m_DYxBit = BIT1;
      m_SSoeBit = BIT2;
      m_DSoeBit = BIT3;
       m_SmsSoeBit = BIT4;
       m_DSmsSoeBit = BIT5;
      
      m_BeatBit = BIT0;
      m_SendYcBit = BIT0;
      m_SendStatusBit = BIT0;
    }
    else if(m_uartId == g_CmIdGPRS)
    {
      m_SYxBit = BIT6;
      m_DYxBit = BIT7;
      m_SSoeBit = BIT8;
      m_DSoeBit = BIT9;
      m_SmsSoeBit = BITA;
      m_DSmsSoeBit = BITB;
      
      m_BeatBit = BIT1;
      m_SendYcBit = BIT1;
      m_SendStatusBit = BIT1;
    }

    initpara();
    if(m_guiyuepara.mode==1)
    {
       m_initflag=0;//7 modi lxq 主站下发请求链路状态后才将此变量设置为7
    }
    m_initfirstflag=1;
    
    return TRUE;
}

DWORD CBJ101S::CheckClearEqpFlag(DWORD dwFlagNo)
{
  DWORD rt = 0;
  if(dwFlagNo < 16 && dwFlagNo < CALL_DATA)
    rt = m_YxYcGrpFlag & BIT[dwFlagNo];
  if(rt)
    m_YxYcGrpFlag &= ~BIT[dwFlagNo];
  return rt;
}
void CBJ101S::ClearEqpFlag(DWORD dwFlagNo)
{
    if(dwFlagNo < 16 && dwFlagNo < CALL_DATA)
    m_YxYcGrpFlag &= ~BIT[dwFlagNo];
}

DWORD CBJ101S::GetEqpFlag(DWORD dwFlagNo)
{
  DWORD rt = 0;
  if(dwFlagNo < 16 && dwFlagNo < CALL_DATA)
    rt = m_YxYcGrpFlag & BIT[dwFlagNo];
  return rt;
}
void CBJ101S::SetEqpFlag(DWORD dwFlagNo)
{
      if(dwFlagNo < 16 && dwFlagNo < CALL_DATA)
        m_YxYcGrpFlag |= BIT[dwFlagNo];
}



void CBJ101S::initpara(BYTE flag)//包括启动时的初始化和设置参数后的初始化
{
    {
      m_guiyuepara.linkaddrlen= g_ucPara101[IECP_LINKADDR_NUM];//2;//链路地址长度
      m_guiyuepara.typeidlen=1;
      m_guiyuepara.conaddrlen=g_ucPara101[IECP_COMNADDR_NUM];//2;//公共地址长度
      m_guiyuepara.VSQlen=1;
      m_guiyuepara.COTlen=g_ucPara101[IECP_TRANSRSN_NUM];//2;//传输原因长度
      m_guiyuepara.infoaddlen=g_ucPara101[IECP_INFO_NUM];//2;//信息体地址长度
      m_guiyuepara.baseyear=2000;
      m_guiyuepara.mode=g_ucPara101[IECP_COM_MODE];//1=平衡式 0=非平衡
      m_guiyuepara.yxtype=3;
      m_guiyuepara.yctype=9;
    }
  

    if(m_guiyuepara.mode==1)
    {
      m_dwasdu.LinkAddr=GetOwnAddr();
    }
    if (m_guiyuepara.linkaddrlen == 1)
    {
            m_byLinkAdrShift = 0;

            m_byTypeIDShift = 1;
            m_byReasonShift = 3;
    }
    else
    {
            m_byLinkAdrShift = 0;
            m_byTypeIDShift = 2;
            m_byReasonShift = 4;
    }
    if (m_guiyuepara.COTlen == 1)
    {
        m_byCommAdrShift = m_byReasonShift + 1;
    }
    else
    {
        m_byCommAdrShift = m_byReasonShift + 2;
    }
    if (m_guiyuepara.conaddrlen == 1)
    {
        m_byInfoShift = m_byCommAdrShift + 1;
    }
    else
    {
        m_byInfoShift = m_byCommAdrShift + 2;
    }
}



void CBJ101S::SetBaseCfg(void)
{
    CPSecondary::SetBaseCfg();
    m_pBaseCfg->wBroadcastAddr = 0xFF;
}

/***************************************************************
    Function：DoReceive
        接收报文处理
    参数：无

    返回：TRUE 成功，FALSE 失败
***************************************************************/
  BOOL CBJ101S::DoReceive()
  {
     while(1)
     {
        if (SearchFrame() != TRUE)
        return FALSE;
  
        // 处理接收
        if(m_guiyuepara.mode==1)
        {
#ifdef YN_101S		
        	m_PRM =0;  //云南
#else
		m_PRM =1; 
#endif
     	}
        else
        {
            m_PRM=0;
        }
        pReceiveFrame = (VIec101Frame *)m_RecFrame.pBuf;
  
        if (pReceiveFrame->Frame10.Start == 0x10)
        {
            RecFrame10();
            continue;
         }
  
        if (pReceiveFrame->Frame68.Start1 == 0x68)
        {
            RecFrame68();
            continue;  
        }
        
        if (pReceiveFrame->Frame69.Start1 == 0x69)
        {

          RecFrame69();
          continue;
        }
	/*	*/
        if (pReceiveFrame->FrameAA.Start[0] == 0xAA)//张弢 0404 短信接收
        {

          RecFrameAA(&pReceiveFrame->FrameAA);
          continue;
        }  //张弢 0404 短信接收
         if(pReceiveFrame->FrameEB.Start1 == 0xEB)   //加密报文解析
        {
            BYTE FrameLen = MAKEWORD(pReceiveFrame->FrameEB.LengthL,pReceiveFrame->FrameEB.LengthH);
            RecFrameEB(FrameLen);//RecFrameEB();
            continue;
        }         
     }
  }


//固定帧格式报文接收处理
BOOL CBJ101S::RecFrame10(void)
{
	g_NolinkReset = 0;
	 //if(this == pDbg)  g_NolinkWifi= 0;
    switch(pReceiveFrame->Frame10.Control & BITS_CODE)
    {
          case 0x4A: //召唤一级用户数据
          case 0x4B: //远方链路状态完好或召唤二级用户数据
              if (pReceiveFrame->Frame10.Control == m_bReceiveControl && m_SendBuf.wWritePtr > 4)//重发
              {
                m_retxdnum++;
                if(m_retxdnum<MAX_FRM_RESEND)
                        return SendRetry();//lqh 尽快解决
                m_retxdnum=0;
                return FALSE;
              }
              break;
          default:
              break;
    }
    //dir bit auto fit
    if(pReceiveFrame->Frame10.Control & BITS_DIR)
      m_DIR=1;/*get主站dir*/
    else m_DIR=0;
    m_bReceiveControl = pReceiveFrame->Frame10.Control;//保存当前状态

    switch(pReceiveFrame->Frame10.Control & BITS_CODE)
    {
        case 0x42: /*心跳测试(平衡)*/
          if(m_linkflag)
          SendAck();
          
          return TRUE;
        case 0x0:
        case 0xb:
            m_recfalg=2;
            m_zdflag=0;
            RecACK();   
            m_retxdnum=0;
            m_reSendDelay=0;
            g_SendBeatFailureNum = 0;
#ifndef	YN_101S	
            if(this == pGprs)  g_GPRSSendLink = OFF;
#endif			
	     //if(this == pDbg)    g_DBGSendLink = OFF;		
          //  g_Soenum=0;
            m_resendflag=0;
            m_PaWaitflag_lubo = OFF;
            m_TxNum_lubo = 0;
            m_PaWaitCt_lubo = 0;
            
      if((mRecorder_flag.LIST_flag == ON)||(mRecorder_flag.xuchuanflag== ON))
        m_ackRecorder =ON;
      if((gRecorder_flag.LIST_flag == ON) &&(m_uartId == g_Cmid))//(( gRecorder_flag.CFG_flag ==ON )||(gRecorder_flag.DAT_flag ==ON )||)//正在读配置文件的数据
            {
              m_ackflag =1;
            }
      if((gRecorder_flag.LIST_flag_YN== ON) &&(m_uartId == g_Cmid))//(( gRecorder_flag.CFG_flag ==ON )||(gRecorder_flag.DAT_flag ==ON )||)//正在读配置文件的数据
            {
              m_ackflag =1;
            }			
            if(((pSendFrame->Frame68.Start1 == 0x68)||(pSendFrame->Frame68.Start1 == 0xEB)) && (m_WaitConfTpId > 0))//用以判断主站回的报文是不是对主动上报报文的确认
            {
                #ifndef GETSOEFROMRAM
                  if(m_WaitConfTpId == M_SP_TB)
                  {//在E2中保存flash中的SOE头指针
                    if(m_uartId == g_CmIdDBG)
                      CAT_SpiWriteWords(EEPADD_SOESEND_E2ROMADR, 1, &m_SSoeHeadPtr);
                    else if(m_uartId == g_CmIdGPRS)
                      CAT_SpiWriteWords(EEPADD_SOESEND_E2ROMADR+4, 1, &m_SSoeHeadPtr);
                  }
                #endif
                
                m_WaitConfTpId = 0;
                m_ucCosBkNum = 0;//上传的COS得到主站确认后备份个数清零
                m_SSoeHeadPtrBk = m_SSoeHeadPtr;
                m_DSoeHeadPtrBk = m_DSoeHeadPtr;
		return TRUE; //收到主站的确认帧(包括复位链路，请求链路状态等)		
            }
            return TRUE; //收到主站的确认帧(包括复位链路，请求链路状态等)
        case 0x40:
            RecResetLink();
            return TRUE; //复位链路
        case 0x49:
#ifdef CQ_101S	//重庆101 认证报文		
	     if((g_RenZLink&0x01) == 1)
              {
            RecReqLink();
	     	}
	     else
	     	{
	     	m_dwasdu.LinkAddr=GetOwnAddr();
              SendBaseFrame(1, 0x07);//认证 10 c7 xx xx xx 16
	     	}
		 g_RenZLink++;
#else
            RecReqLink();
	mRecorder_flag.LIST_flag = OFF;
      mRecorder_flag.xuchuanflag= OFF;
#endif
            return TRUE; //请求链路状态
        case 0x4A: 
            RecCallClass1();
            return TRUE; //召唤一级用户数据
        case 0x4B://二级用户数据
            RecCallClass2(); 
            return TRUE; 
        
        case 0x03:/*ACK 的处理 */
            DoRecAck();
            return TRUE;
        case 0x4c:
          SendAck();
          break;
        default:
          break;
  }
    return TRUE;
}

//可变帧长格式报文接收处理
BOOL CBJ101S::RecFrame68(void)
{
    //dir bit auto fit
    //unsigned char uartNum = 0;
    WORD addr = GetOwnAddr();
    
   if((pReceiveFrame->Frame68.Control == 0x05)&&(pReceiveFrame->Frame68.Length1 == 0x30))
    {//读GPRS状态返回帧
        if(1)       //计算校验 
        {
          if(m_RWPaSrcObj != null)
             m_RWPaSrcObj->SendReadPa(11,0x0F);
        }
    }
    else
    {
        getasdu();
    }
    if(pReceiveFrame->Frame68.Control & BITS_DIR)
      m_DIR=1;/*get主站dir*/
    else m_DIR=0;

    m_bReceiveControl = pReceiveFrame->Frame68.Control;
    m_acdflag=1;
    m_recfalg=1;
    if(((m_bReceiveControl&0xf)!=4)&&(SwitchToAddress(m_dwasdu.LinkAddr))&&(m_linkflag))//增加链路初始化完成后再响应短帧确认
    {    
        SendAck();
        delayms(200);
    } 	
    switch(m_dwasdu.TypeID)
    {
        case 0x2D:
        case 0x2E:
#ifdef YN_101S
	     if(m_uartId == 2)
	     	{
	     	RecResetRTU();
	     	break;
	     	}
#endif
        case 0x2F:
            RecYKCommand();
            break;//遥控
        case 0x64:
            if(g_bAppType)
            {
                SendEncFrameAck(0x0191, 0x0000, 0x1F);
                return TRUE;
            }			
            RecCallSomeGroup();
            break;//总召唤/召唤某一组数据
        case 0x65:
            //RecCallDDCommand();
            break;//召唤电度
        case 0x66:
            //RecReadData();
            m_zdflag=0;
            break;//读数据处理
        case 0x67:	
           if(g_bAppType)
           {
               SendEncFrameAck(0x0191, 0x0000, 0x1F);
               return TRUE;
           }			
            if(m_dwasdu.COT==0x05)			 
			{RecReadClock();}		
		else            		
			{RecSetClock();}
            break;//时钟同步
        case 0x68:
            RecTestLink();
            break; //测试链路
        case 0x69:
            if(g_bAppType)
            {
                SendEncFrameAck(0x0191, 0x0000, 0x1F);
                return TRUE;
            }  			
            RecResetRTU();
            break; //复位RTU
        case 0x6A:
            if(m_dwasdu.COT==6)
              RecDelaytime();
            else if(m_dwasdu.COT==3)
              RecDelaytrans();
            break; 
        case 0x6E:
            //RecSetPara();
            break;//设置参数
        //case 0xD2:
            //m_SendFixNum =0;   
            //Recfileprocess();
            //Recfileprocessing();
            //break;
        case 0xD3://远程升级
            //FileRemoteupdata(&pReceiveFrame->Frame68.Start1);
            break;           
        case 0x79:    
        case 0x7a://读文件
        case 0x7c:
		case 0x84://离散读参数	
            if(m_dwasdu.Info ==26882)
              RecReadFile();
            else
              {
                g_Cmid = m_uartId;
                m_comtradeflag = 0x55;
		    m_comtradeflag_YN = 0;		
        m_PaWaitflag_lubo = OFF;
                m_TxNum_lubo = 0;
                m_PaWaitCt_lubo = 0;
                wSendLISTNum = 0;
        BK_FRecorder_Current_COUNT = g_FRecorder_Current_COUNT;
        //for(int i =0;i<64;i++)
        //  lubo_valid[i] = 0;
                memcpy(gRecorder_flag.pRXBuff,&pReceiveFrame->Frame68.Start1,6+pReceiveFrame->Frame68.Length1);  
                //Code_Lubo(&pReceiveFrame->Frame68.Start1,m_SendBuf.pBuf);
              }	
		break;
    case 0xD2:
        /*g_Cmid = m_uartId;
                m_fileprocessing = 0x55;
        m_PaWaitflag_lubo = OFF;
              m_TxNum_lubo = 0;
                m_PaWaitCt_lubo = 0;
                wSendLISTNum = 0;
        //for(int i =0;i<64;i++)
          //lubo_valid[i] = 0;
                memcpy(gRecorder_flag.pRXBuff,&pReceiveFrame->Frame68.Start1,6+pReceiveFrame->Frame68.Length1); */ 
        if(g_bAppType)
        {
            SendEncFrameAck(0x0191, 0x0000, 0x1F);
            return TRUE;
        }     
		//for(int i =0;i<64;i++)
        //  lubo_valid[i] = 0;
        
        mRecorder_flag.LIST_flag = OFF;//收到短贞确认需要回数据时，置位改标志位
                mRecorder_flag.xuchuanflag= OFF;
                m_lubo_num =0; 
        m_fixpt_num =0;
        m_SendListNum =0;
        m_SendFixNum =0;  
        BK_FRecorder_Current_COUNT = g_FRecorder_Current_COUNT;
           
        Recfileprocessing(&pReceiveFrame->Frame68.Start1);
        break;//?		
        case 0x88://136 云南录波协议  读目录
        case 0x89://137 云南录波协议  读文件	
        case 0x8a://138 云南录波协议  补包
            if(m_dwasdu.Info ==26882)
              RecReadFile();
            else
              {
                g_Cmid = m_uartId;
                m_comtradeflag_YN = 0xAA;
		   m_comtradeflag = 0;		
                //Code_Lubo(&pReceiveFrame->Frame68.Start1,m_SendBuf.pBuf);
              }
            break;
        case 0x7d://写文件
            RecWriteFile();
            break;
        case 0x81:
            //RecTSData();//接收透明数据
            break;
        case 0x82:
            //RecFileData();
            break;
        case 0x30://遥调
            RecYTCommand();
            break;
       case 0x35:
       case 0x34://设置ip
       case 0x38://设置心跳周期
       case 0x3F://设置遥测上传周期  
       case 0x6C://设置链路地址  
       case 0x6B:
       //case 0x7A:
       case 0x44:// 置自检上传周期  
            RecYSCommand();
            m_zdflag=0;
            break;
	   case 0x96://舟山修改密码	
	   		break;
	   case 0x97://舟山修改密文	
	   		break;
//          case 200://远程升级
//          case 203:
//          case 210:
//          case 204:
//          case 205:
//          case 206:
//            m_recfalg = 0;
//            g_Cmid = m_uartId;
//            ParaBuff.SubAddrh = HIBYTE(GetAddress());
//            ParaBuff.SubAddrl = LOBYTE(GetAddress());
//            RemoteUpdata(&pReceiveFrame->Frame68.Start1, pReceiveFrame->Frame68.Length1);
//            return TRUE;      
        default:
            m_acdflag=0;
            m_errflag=2;
            break;
    }
    if(((m_bReceiveControl&0xf)!=4)&&(SwitchToAddress(m_dwasdu.LinkAddr))&&(m_linkflag))//增加链路初始化完成后再响应短帧确认
       ;// SendAck();
    if(m_guiyuepara.mode==1)
        RecACK();
    return TRUE;
}
//参数查询及设置部分 + 远程升级====================================================================================
//自定义报文
BOOL CBJ101S::RecFrame69(void)
{
  //pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;
	getasdu();
	m_bReceiveControl = pReceiveFrame->Frame68.Control;
    m_acdflag=1;
    m_recfalg=1;
    if(((m_bReceiveControl&0xf)!=4)&&(SwitchToAddress(m_dwasdu.LinkAddr))&&(m_linkflag))//增加链路初始化完成后再响应短帧确认
    {    
        SendAck();
        delayms(200);
    } 
	
	switch(m_dwasdu.TypeID)
    //switch(pReceiveFrame->Frame69.Type)
    {
    	case 0x79:    
        case 0x7a://读文件
        case 0x7c:
		case 0x84://离散读参数		
            if(m_dwasdu.Info ==26882)
              RecReadFile();
            else
           		{
            	g_Cmid = m_uartId;
                m_comtradeflag = 0x55;
		    	m_comtradeflag_YN = 0;		
        		m_PaWaitflag_lubo = OFF;
                m_TxNum_lubo = 0;
                m_PaWaitCt_lubo = 0;
                wSendLISTNum = 0;
        		BK_FRecorder_Current_COUNT = g_FRecorder_Current_COUNT;
                memcpy(gRecorder_flag.pRXBuff,&pReceiveFrame->Frame68.Start1,6+pReceiveFrame->Frame68.Length1);                 
              	}	
		break;
        //case 0x7a://读文件
        //    RecReadFile();
        //    break;
        case 0x7d://写文件
        case 0x88://离散写参数
            RecWriteFile();
            break;
        case 200://c8
              g_Cmid = m_uartId;
              Code_Load(&pReceiveFrame->Frame69.Start1,m_SendBuf.pBuf);
              break;
        default:
            break;
    }
    return TRUE;
}


//解析 复位远方链路
BOOL CBJ101S::RecResetLink(void)
{
    //BYTE PRM = 0;
    switch (pReceiveFrame->Frame10.Control & BITS_PRM)
    {
        case 0: //主站响应从站<复位远方链路>的帧
            //响应主站的链路复位应答，并发初始化结束
//#ifdef CQ_101S            
          //delayms(5000);
//#endif
            if (m_byRTUStatus == RTU_RESET)
            {
                SendInitFinish();
                m_byRTUStatus = RTU_INITEND;
            }
            if (m_byRTUStatus == RTU_RECCALL)
            {
                SendCallAll();
            }
            return TRUE;
        default: //主站下发的命令
            ClearEqpFlag(Rec_FCB);  //清接收FCB状态
            m_YKflag=0;
            m_callallflag=0;
            m_resetflag=0;
            m_acdflag=1;
            //m_yxchangeflag=0;
            //SendBaseFrame(PRM_SLAVE,0x00);
            if(m_guiyuepara.mode == 0)  //非平衡式
                m_linkflag=1;
            if(m_guiyuepara.mode==1)
            {
                m_initflag=0x0f;
				m_linkflag = 0;//涓荤珯鍙戝垵濮嬪寲鎸囦护鏃跺悓鏃舵柇寮€杩炴帴锛屼繚闅滃垵濮嬪寲杩囩▼涓嶈涓诲姩涓婃姤鎵撴柇
		  if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_SEND_FTYC])	
		 	m_initflag=0x0c;		  	
#ifdef YN_101S
		m_initflag=0x0c;
#endif		  
                m_recfalg=1;
            }
            else if(((m_initfirstflag==1))||(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_ISSENDINITFIN]))
            {//非平衡模式,只第一次上传或一直上传初始化长帧
              m_initflag=4;
            }
			
            SendBaseFrame(PRM_MASTER,0x00); //张弢重庆//

            return TRUE;
    }
}



//解析总召唤/召唤某一组数据
BOOL CBJ101S::RecCallSomeGroup(void)
{

  m_ztype=m_RecFrame.pBuf[5+m_dwasdu.Infooff+m_guiyuepara.infoaddlen];
  m_ztype=20;
  if(m_ztype==20) m_callallflag=20|0x80;
  if((m_ztype>20)&&(m_ztype<32)) m_groupflag=m_ztype|0x80;
  if(m_ztype==34)
  {
      m_QPM=1;
   }
    if((m_ztype<20)||(m_ztype>34))
        m_errflag=0x1;

//  while(m_callallflag)
//    {
//        SendCallAll();
//        delayms(200);
//    }
//#ifdef BJ101_GXTEST
//  if(m_ztype==20)
//      SendAck2(TRUE);     /*总召唤1级数据确认*/
//  else
//      SendAck2(FALSE);    /*组召唤2级数据确认*/
//
//      m_wSendYcNum=0;
//#endif

    return TRUE;
}
//总召唤回复报文 总召确认，结束，遥信，遥测
BOOL CBJ101S::SendCallAll(void)
{
  int i = 0,j = 0;
  
  if(m_callallflag&0x80)   //云南更改
   {
      m_callallflag&=0x7f;
      RecCallAllCommand();
      return TRUE;
   }
   for(i = CALL_YXGRP1,j = 0; i <CALL_DYXGRP1 ;i++,j++) //
   {//单点遥信
      if (GetEqpFlag(i))
        {
           ClearEqpFlag(i);
           m_wSendYxNum=0;
#ifdef YN_101S
	    m_ZongzhaoYX = 0;
           m_wSendYxNum=0;
 	    if (SendYXGroup(0,20,30))
               return TRUE;
#else		
           if (SendYXGroup(j,20,1))
              return TRUE;
#endif		   
        }
   }
  
 /*//张弢云南与标准101
       if(m_ZongzhaoYX == 0x55)
       {
           m_ZongzhaoYX = 0;
           m_wSendYxNum=0;
           if (SendYXGroup(0,20,30))
               return TRUE;
       }
 *///张弢云南与标准101
    for(i = CALL_YCGRP1,j = 0; i < CALL_ALLSTOP;i++,j++)
   {//遥测
      if (GetEqpFlag(i))
        {
           ClearEqpFlag(i);
           m_wSendYcNum=0;
#ifdef YN_101S
	    m_ZongzhaoYC = 0;
           m_wSendYcNum=0;
           if (SendYCGroup(0,20,35))
               return TRUE;
#else
            if(SendYCGroup(j,20,9))
              return TRUE;
#endif			
        }
   }
#ifdef YN_101S	
 	if(m_wSendZJNum == 0)
 	{//总召发送自检
 	SendZJGroup(0,1,36);
	m_wSendZJNum = 0x55;
	return TRUE;
 		}
#endif	
/*//张弢云南与标准101	
   if(m_ZongzhaoYC == 0x55)
   {
       m_ZongzhaoYC = 0;
       m_wSendYcNum=0;
       if (SendYCGroup(0,20,35))
           return TRUE;
   }
    *///张弢云南与标准101
  if (CheckClearEqpFlag(CALL_ALLSTOP))
      {
          m_callallflag=0;
          m_acdflag=0;m_wSendZJNum = 0;
        if(SendAllStop())
            return TRUE;
      }
  return FALSE;
}

//处理总召唤激活，停止激活
BOOL CBJ101S::RecCallAllCommand(void)
{
    switch (m_dwasdu.COT& 0x3F)//传送原因
    {
        case 6:
            RecCallAllStart();
            m_ZongzhaoYX = 0x55;   //云南规约
            m_ZongzhaoYC = 0x55;
            return TRUE;//总召唤激活
        case 8://停止激活
            RecCallAllStop();
            break;
        default:
            break;
    }
    return TRUE;
}

//回复总召唤激活命令
BOOL CBJ101S::RecCallAllStart(void)
{
    if(m_guiyuepara.mode ==1) //平衡式
    {
        m_byRTUStatus = RTU_RECCALL;
    }
	
    //SendBaseFrame(0,0);//张弢重庆 总召确认报文
   //delayms(100);
    SendCallAllStartAck();
    if(m_ztype==20)
    {/*
        if(g_gRunPara[RP_DEVICE_TYPE] == 1)  //二遥设备
        {
            for (WORD EqpFlag = CALL_YXGRP1; EqpFlag < CALL_ALLSTOP; EqpFlag++)
                SetEqpFlag(EqpFlag);
        }
        else if(g_gRunPara[RP_DEVICE_TYPE] == 0)   //一遥设备
        {
             for (WORD EqpFlag = CALL_YXGRP1; EqpFlag < CALL_YCGRP1; EqpFlag++)
                SetEqpFlag(EqpFlag);             
        }
	else if(g_gRunPara[RP_DEVICE_TYPE] == 2)   //信号源//张弢云南与标准101
        {  */           
                SetEqpFlag(CALL_YXGRP1);   SetEqpFlag(CALL_YCGRP1);            
       // }	//张弢云南与标准101
    }
    if((m_ztype>20)&&(m_ztype<33))
    {
       if(m_ztype-21+CALL_YXGRP1<CALL_ALLSTOP)
         SetEqpFlag(m_ztype-21+CALL_YXGRP1);
    }
      SetEqpFlag(CALL_ALLSTOP);


    return TRUE;
}
//回复停止总召唤激活命令
BOOL CBJ101S::RecCallAllStop(void)
{
    if(m_callallflag)
    {
        if(m_ztype==20)
          {
            for (WORD EqpFlag = CALL_YXGRP1; EqpFlag < CALL_ALLSTOP; EqpFlag++)
              ClearEqpFlag(EqpFlag);
          }
        if((m_ztype>20)&&(m_ztype<28))
          {
            if(m_ztype-21+CALL_YXGRP1<CALL_ALLSTOP)
              ClearEqpFlag(m_ztype-21+CALL_YXGRP1);
          }
    }
    m_callallflag=0;
    ClearEqpFlag(CALL_ALLSTOP);
    m_ZongzhaoYX = 0;   //云南规约
    m_ZongzhaoYC = 0;

    if(m_ztype==34)
    {
      m_QPM=1;
    }
    return TRUE;
}
//发送总召唤激活命令ACK确认
BOOL CBJ101S::SendCallAllStartAck(void)
{
    BYTE PRM = 0, dwCode = 3,Num = 1;
        SendFrameHead(C_IC_NA, 7);      /*广西测试组召插遥控出现重复遥控确认*/

    write_infoadd(0);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_ztype;
#ifdef SD_101S
    PRM = 0;//0x55;
    SendFrameTail(PRM , dwCode, Num,0);
#else	
    SendFrameTail(PRM, dwCode, Num,0);
#endif
    return TRUE;
}


//发送总召唤结束帧
BOOL CBJ101S::SendAllStop(void)
{
    BYTE Style = 0x64;
    BYTE Reason = 0x0A;
    BYTE PRM = 0;
    BYTE dwCode = 3;
    BYTE Num = 1;
    m_acdflag=0;

    SendFrameHead(Style, Reason);
    write_infoadd(0);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_ztype;
    SendFrameTail(PRM, dwCode, Num,0);
    return TRUE;
}







//发送组召或总召遥信，总召唤只上送不带时标信息
BOOL CBJ101S::SendYXGroup(WORD GroupNo, BYTE Reason, BYTE bType)
{
    BYTE Style = bType;
    BYTE PRM = 0, dwCode = 3; 
    WORD YXNo;
    WORD YXValue;
    BYTE VSQ=0x80;//顺序发送
#ifdef YN_101S  	
	VSQ=0x00;//离散发送
#endif    
    //WORD ucTemp = 0x01;
    YXNo = GroupNo * GRP_YXNUM;
    YXNo+=m_wSendYxNum;
    if((m_wSendYxNum+m_wSendDYxNum>=GRP_YXNUM)|| (YXNo >= m_pEqpInfo[m_wEqpNo].wSYXNum))
    {
        return FALSE;
    }

    SendFrameHead(Style, Reason);
    if(VSQ == 0x80)//顺序发送
   	write_infoadd(YXNo + g_gRunPara[RP_SYX_INFADDR]);//ADDR_YX_LO

    WORD i;
    //for(i=0;YXNo<RMT_INFO_NUM; i++, YXNo++)//张弢 遥信点表
    for(i=0;i<g_ucYxTransNum; i++, YXNo++)//张弢 遥信点表
    {
#ifdef YN_101S
        if(i != 1)//张弢云南 遥信没有0x81
#endif          
        {
        //g_ucYXAddr,
        //g_ucYxTransNum
        if(VSQ == 0)//离散发送
        	write_infoadd(YXNo + g_gRunPara[RP_SYX_INFADDR]);//ADDR_YX_LO        
        //YXValue = (g_gRmtInfo[0] & ((1<<(g_ucYXAddr[i]-1))));
        YXValue = g_gRmtInfo[(g_ucYXAddr[i]-1)] ;
        if(YXValue != 0)
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 1;
        else
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;
        
        
        if(bType == 30)
          write_time();
        }
        //ucTemp = (ucTemp << 1);
    }
#ifdef YN_101S	
    write_infoadd(0x10);//张弢云南 遥信0x10 电池欠压报警
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//张弢云南 遥信0x10 电池欠压报警
    if(bType == 30)//张弢云南 总召带时标 遥信0x10 电池欠压报警
          write_time();//张弢云南 遥信0x10 电池欠压报警
#endif   
    m_wSendYxNum+= i;

    //SendFrameTail(PRM, dwCode, ((i  )| VSQ));
   SendFrameTail(PRM, dwCode, ((i)| VSQ),0);
    return TRUE;
}
//继续发送一组遥信 单点
//  BOOL CBJ101S::SendYXGroupContinue(WORD GroupNo, BYTE Reason)
//  {
//      BYTE Style = 1;
//      BYTE PRM = 0, dwCode = 8;
//      WORD YXNo, YXSendNum;
//      WORD YXValue;
//  
//      YXNo = GroupNo * GRP_YXNUM + m_wSendYxNum;
//      if (YXNo >= m_pEqpInfo[m_wEqpNo].wSYXNum)//sfq
//          return FALSE;
//  
//      SendFrameHead(Style, Reason);
//  
//      write_infoadd(YXNo + ADDR_YX_LO);
//      for (YXSendNum = 0; (YXSendNum < (GRP_YXNUM - m_wSendYxNum)) && (YXNo < m_pEqpInfo[m_wEqpNo].wSYXNum); YXNo++, YXSendNum++)
//      {
//          YXValue = g_unYxTrans[YXNo];
//          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = YXValue ? 1 : 0;
//      }
//      m_wSendYxNum +=YXSendNum;
//      SendFrameTail(PRM, dwCode, YXSendNum | 0x80);
//  
//      if ((FRM_YXNUM < GRP_YXNUM) && (m_wSendYxNum != 0) && (YXNo < m_pEqpInfo[m_wEqpNo].wSYXNum))
//      {
//          delayms(8);
//          SendYXGroupContinue(GroupNo, Reason);
//          m_wSendYxNum = 0;
//      }
//      return TRUE;
//  }


//发送组召或总召双点遥信，总召唤只上送不带时标信息
BOOL CBJ101S::SendDYXGroup(WORD GroupNo, BYTE Reason)
{
    BYTE Style = M_DP_NA;
    BYTE PRM = 0, dwCode = 3;
    BYTE VSQ=0x00;
    WORD YXNo, YXSendNum;
    YXNo = GroupNo * GRP_YXNUM;
    YXNo+=m_wSendDYxNum;
    if((m_wSendDYxNum>=GRP_YXNUM)|| (YXNo >= m_pEqpInfo[m_wEqpNo].wDYXNum))
    {
      return FALSE;
    }

    SendFrameHead(Style, Reason);

    write_infoadd(YXNo + g_gRunPara[RP_DYX_INFADDR]);

    for (YXSendNum = 0;(m_SendBuf.wWritePtr<230)&& (YXSendNum < FRM_YXNUM) && (YXNo < m_pEqpInfo[m_wEqpNo].wDYXNum); YXNo++, YXSendNum++)
    {
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = GetRIStatus(YXNo,2);//DIQ(YXValue,YXValue->byValue2);
    }
    m_wSendDYxNum +=YXSendNum;

    SendFrameTail(PRM, dwCode, YXSendNum | VSQ,0);

    return TRUE;
}

//按组上送遥测，不带时标
BOOL CBJ101S::SendYCGroup(WORD GroupNo, BYTE Reason ,BYTE bType)
{
   // BYTE Style = m_guiyuepara.yctype; //M_ME_NA;
    BYTE Style = bType;
    BYTE PRM = 0;
    BYTE dwCode = 3;
    WORD YCNo, YCSendNum;
    //WORD ReadYCNum = 64;
    WORD YCValue; 
    //int YxIndex = 0;
    BYTE VSQ=0x80;//离散发送   
#ifdef YN_101S
    VSQ=0x00;//离散发送 
#endif
    YCNo = GroupNo * GRP_YCNUM;
    YCNo+=m_wSendYcNum;
    if(/*(m_wSendYcNum >= GRP_YCNUM)||*/(YCNo >= m_pEqpInfo[m_wEqpNo].wYCNum))
    {
      return FALSE;
    }
    switch(Style)
    {
        case M_ME_TA:
        case M_ME_TD:
            Style = M_ME_NA;
            break;
        case M_ME_TB:
        case M_ME_TE:
            Style = M_ME_TE;  //云南
            break;
        case M_ME_TC:
        case M_ME_TF:
            Style = M_ME_NC;
            break;
    }
    if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_YC_FLOAT])
    	{
    	Style = 13;//浮点数
    	}
    SendFrameHead(Style, Reason);
    if(VSQ == 0x80)//顺序发送	
   	write_infoadd(YCNo + g_gRunPara[RP_YC_INFADDR]);
    for (YCSendNum = 0;(YCNo < m_pEqpInfo[m_wEqpNo].wYCNum);YCNo++,YCSendNum++)
    {
#ifdef YN_101S     
        if(YCNo != 7)////张弢云南 遥测没有0x4088 0x4089为投切次数
#endif        
        {
          //write_infoadd(YCNo + ADDR_YC_LO);//张弢 遥测起始地址修改运行参数
          if(VSQ == 0)//离散发送   
	  	write_infoadd(YCNo + g_gRunPara[RP_YC_INFADDR]);	 //张弢 遥测起始地址修改运行参数 
          YCValue = g_unYcTrans[YCNo];
	   if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_YC_FLOAT])
    		 	{
    			FP32 flt;
			char *pdatachar=null;		
			flt = (FP32)YCValue;	
			pdatachar = (char *)&flt;
			m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = *pdatachar++;//LOBYTE(LOWORD(flt));
			m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = *pdatachar++;//HIBYTE(LOWORD(flt));
			m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = *pdatachar++;//LOBYTE(HIWORD(flt));
			m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = *pdatachar++;//HIBYTE(HIWORD(flt));	
    			}
	   else
	   	{
          	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(YCValue);
          	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(YCValue);
	   	}
          if(m_guiyuepara.yctype!=M_ME_ND) /* #108 - GB101主 配合 GB101从和BJ101从的问题 */
          {
          /*YxIndex = YcIdToYxId(g_ucYCAddr[YCNo] - 1);
          if(YxIndex == 255) 
             continue;
          if(GetRIStatus(YxIndex) == 1)           
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDS通道故障后最高位置1表示无效
          else*/
              m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
          }
          if(bType == 35)//带长时标
              write_time();
        }
        //YCSendNum+=ReadYCNum;
        //YCNo+=ReadYCNum;
    }
#ifdef CQ_101S
    BYTE i;
    for (i = 0;(i < 9);i++)
    {	
    write_infoadd(YCNo + g_gRunPara[RP_YC_INFADDR]);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//HIBYTE(YCValue);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//HIBYTE(YCValue); 
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
    YCNo++;
    	}    
#endif
    m_wSendYcNum+= YCSendNum;
#ifdef CQ_101S	
    m_wSendYcNum+= 9;YCSendNum=m_wSendYcNum;//重庆101 遥测9个备用
#endif		
#ifdef YN_101S	
    SendFrameTail(PRM,dwCode, (YCSendNum - 1)| VSQ,0);//SET ACD//张弢云南 遥测没有0x4088 0x4089为投切次数
#else    
   SendFrameTail(PRM,dwCode, (YCSendNum)| VSQ,0);//SET ACD//张弢 遥测点表
#endif   

//      if ((FRM_YCNUM < GRP_YCNUM) && (m_wSendYcNum != 0) && (YCNo < m_pEqpInfo[m_wEqpNo].wYCNum))
//      {
//          delayms(8);
//          SendYCGroupContinue(GroupNo, Reason);
//          m_wSendYcNum = 0;
//      }
     return TRUE;
}
BOOL CBJ101S::SendZJGroup(WORD GroupNo, BYTE Reason ,BYTE bType)
{
    BYTE Style = bType;
    BYTE PRM = 0;
    BYTE dwCode = 3;
    BYTE VSQ=0x00;
    WORD ZJNo, ZJSendNum,ZJAdr;
    WORD ZJValue; 
    ZJNo = 0;	
    if(/*(m_wSendYcNum >= GRP_YCNUM)||*/(ZJNo >= m_pEqpInfo[m_wEqpNo].wZJNum))
    {
      return FALSE;
    }
/*#ifndef YN_101S
    switch(Style)
    {
        case M_ME_TA:
        case M_ME_TD:
            Style = M_ME_NA;
            break;
        case M_ME_TB:
        case M_ME_TE:
            Style = M_ME_TE;  //云南
            break;
        case M_ME_TC:
        case M_ME_TF:
            Style = M_ME_NC;
            break;
    }
#endif*/
    SendFrameHead(Style, Reason);
    for (ZJSendNum = 0;(ZJNo < m_pEqpInfo[m_wEqpNo].wZJNum);ZJNo++,ZJSendNum++)
    {       
	   ZJAdr=ZJNo + 0x400A;//g_gRunPara[RP_YC_INFADDR]
          ZJValue = g_unZJTrans[ZJNo];
	   if	 (ZJAdr == 0x400C) ZJAdr = 0x400D;
	   m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(ZJAdr);
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(ZJAdr);	  
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(ZJValue);
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(ZJValue);
	   m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS	           
          //if(bType == 35)//带长时标
            write_time();       
    }       
    SendFrameTail(PRM,dwCode, (ZJSendNum)| VSQ,0);//SET ACD//张弢 遥测点
     return TRUE;
}

//继续发送一组遥测,用于一帧数据未发完一组的长度
BOOL CBJ101S::SendYCGroupContinue(WORD GroupNo, BYTE Reason)
{
    BYTE Style = M_ME_NA;
    BYTE PRM = 0;
    BYTE dwCode = 8;
    WORD YCNo, YCSendNum, YCValue;
    int YxIndex = 0;
    YCNo = GroupNo * GRP_YCNUM + m_wSendYcNum;
    if (YCNo >= m_pEqpInfo[m_wEqpNo].wYCNum)
        return FALSE;

    SendFrameHead(Style, Reason);

    //write_infoadd(YCNo + ADDR_YC_LO); //张弢 遥测起始地址修改运行参数 
    write_infoadd(YCNo + g_gRunPara[RP_YC_INFADDR]); //张弢 遥测起始地址修改运行参数 
    for (YCSendNum = 0; (YCSendNum < (GRP_YCNUM - m_wSendYcNum)) && (YCNo < m_pEqpInfo[m_wEqpNo].wYCNum); YCNo++, YCSendNum++)
    {
        YCValue = g_unYcTrans[YCNo];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(YCValue);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(YCValue);
        YxIndex = YcIdToYxId(g_ucYCAddr[YCNo] - 1);
          if(YxIndex == 255) 
             continue;
          if(GetRIStatus(YxIndex) == 1)           
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDS通道故障后最高位置1表示无效
          else
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
       // m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
    }
    m_wSendYcNum+= YCSendNum;
    SendFrameTail(PRM,dwCode, YCSendNum | 0x80,0);
    return TRUE;
}
void CBJ101S::SendlbRetry(void)
{
   CommWrite((char*)m_gprsSendBuf,m_gprsSendLen);
}

//通道发送空闲处理
void CBJ101S::DoCommSendIdle(void)
{

  //  if((m_uartId == g_CmIdGPRS) && (!g_GprsPowerSt))
     // return;//GPRS通道中如果GPRS没打开则不发任何数据

    if(m_guiyuepara.mode==1)		
#ifdef YN_101S		
        	m_PRM =0;  //云南
#else
		m_PRM =1; 
#endif
    else if(m_recfalg!=3)   //非平衡式
        return;
    if((m_uartId == 2) && (g_GPRSSendLink == ON)&&(m_linkflag == 0))
    {
        g_GPRSSendLink = 0;
        m_initflag = 0x0f;
	 if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_SEND_FTYC])	
		m_initflag=0x0c;		
        m_initfirstflag = 1;
        Initlink();
    }
    if(m_YKflag == 1) 
    {
        m_YKflag=0;
        SendYKSetAck();
        return;
    }
    
    if((g_SendBeatFailureNum >= 4) && (m_uartId == 2) /*&& (g_GPRSSendLink == OFF)*/&&(m_linkflag == 1))   //心跳4次不回次数到达出,重新连接，
    {
        g_SendBeatFailureNum = 0;
        //delayms(3000);
        m_initflag=0x0f;
	 if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_SEND_FTYC])	
		m_initflag=0x0c;			
        //m_recfalg=1;
        m_initfirstflag = 1;
        g_GPRSSendLink = 0;//张弢，开启1分钟一次的连接
        m_linkflag = 0;
    }
    
    //模式2=主动上报不受任何约束,总召不受约束
    if((g_ucPara101[IECP_AUT_MODE] == 2))  //发送延迟到时
    {
      if(m_uartId != g_CmIdGPRS)
        m_linkflag = 1;
      else if (g_SendFault == ON)
      {
         g_SendFault = OFF;
         m_linkflag = 1;
       }
    }
    
    //回应主站部分========================
    if(m_recfalg)
    {//a
        //模式1=只要收到主站报文就主动上报,总召不受约束
        if(g_ucPara101[IECP_AUT_MODE] == 1)
        {
          m_linkflag = 1;
        }
    
       if(m_recfalg==2)
           m_recfalg=0;
       if(m_initflag)
       {
           Initlink();
           return ;
       }
       m_acdflag=1;
       
       //模式0=只有初始化结束才响应主站召唤，才主动上报
       if(g_ucPara101[IECP_AUT_MODE] == 0)
       {
        if(!m_linkflag) 
            return;
       }
       
      if(m_callallflag)
      {
          if(SendCallAll())
              return ;
      }
      if(m_HistLoadFlag)
      {
          if(SendCallHistLoad())
              return;
      } 
      if(m_HistLuBoFlag)
      {
          if(SendCallHistLuBo())
              return;
      }
	  if(m_comtradeflag==0x55)
      {
        //Code_Lubo(&pReceiveFrame->Frame68.Start1,m_SendBuf.pBuf);
        Code_Lubo(gRecorder_flag.pRXBuff,m_SendBuf.pBuf);
        m_comtradeflag=0;gRes_rec.res_timeout = 0;
      }
	  if(m_comtradeflag_YN==0xAA)
      {
        Code_Lubo_YN(&pReceiveFrame->Frame68.Start1,m_SendBuf.pBuf);
	//gRes_rec.res_timeout = 1;	
        m_comtradeflag_YN=0;
	 // m_com101flag_YN=0x55;	
      }	  
      m_acdflag=0;


      if(m_timeflag)
      {
        m_timeflag=0;
        if(SendtimeAck())
        {
          m_zdflag = 0;
          return ;
        }
      }
     if(m_timeREQflag)
      {
        m_timeREQflag=0;
        if(SendtimeReq())
        {
          m_zdflag = 0;
          return ;
        }
      }	  
      if(m_delayflag)
      {
        m_delayflag=0;
        if(SenddelayeAck())
        return ;
      }

      if(m_groupflag)
      {
         if(SendCallgroup())
            return ;
      }
      if(m_testflag)
      {
         m_testflag=0;
         if(SendTsetLinkAck())
            return ;
      }
      if(m_resetflag)
      {
         m_resetflag=0;
         if(SendresetAck())
            return ;
      }

    }//a
 
    //链路已经初始化，重发时间间隔到
    if((m_zdflag==1)&&(m_linkflag)&&(m_resendflag)&&(m_reSendDelay==0))
    { 
            m_retxdnum++;
          if(m_retxdnum < MAX_FRM_RESEND)
          {
            SendRetry();
            
            m_reSendDelay = g_gRunPara[RP_YXRSD_T];
            return;
          }
          if(m_resendflag)   /*重发MAX_FRM_RESEND-1次*/
          {
              m_retxdnum=0;
              //m_zdflag=0;     
              //m_linkflag=0;   /*断开链路*/
              m_resendflag=0;
              m_reSendDelay = 0;
              m_WaitConfTpId = 0;//第2次重发后还会启动重发定时器，此次的定时器计数到才算重发结束
              return;
          }
  }
#ifdef  SD_101S	//山东 初始化完成立即总召，中间不能有cos和soe，所以初始化完成，9秒之后再cos和soe
	if((g_CosSoeDely<=0x2000)&&(m_linkflag))
		g_CosSoeDely++;	
#else
	g_CosSoeDely=0xffff;
#endif
    //主动上报部分===================================
    if((m_zdflag==0)&&(m_linkflag))
    {//b
      //主动上传之前先确定有没有需要重传的报文
        if(SendRetry())
           return;
	

#ifndef  YN_101S	//云南101 不发COS	

        //发送cos
      /* */ 
        if(g_CosSoeDely>6000)
        {
        if(SearchCos(m_SYxBit,0))
        {
           m_acdflag=1;
           if(SendCos())
           {
              m_reSendDelay = g_gRunPara[RP_YXRSD_T];//有报文发出才启动重传
               return ;
           }
           m_acdflag=0;
        }
        }
#endif 
#ifdef  YN_101S	//录波完成后 自动上传目录
	if((g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_SENDLUBOML])&&(m_luok==1))
	{
		m_luok=0;
		SendLBML();
		return ;
	}
#endif
        //发送突变遥测
        if(g_SendChgYcFlag)
        {
          g_SendChgYcFlag = 0;
            if(SendChangeYC())
            {
               ; 
            }
              return;
        }
        //发送SOE
        if(g_CosSoeDely>6000)
        {          
        if(SearchCos(m_SSoeBit,0))
        {
           m_acdflag=1;
           if(SendSoe())
            {
              m_reSendDelay = g_gRunPara[RP_YXRSD_T];//有报文发出才启动重传
              return ;
            }
            m_acdflag=0;
        }
        }
      /*  if(m_BeatFlag)
        {
          m_BeatFlag = 0;
          SendBaseFrame(PRM_MASTER,2);
        }*/
        if(g_SendBeat == 0x55 && m_uartId == 2)
        {
          g_SendBeat = 0; 
		  if(g_ucPara101[IECP_101_STY]==1)//舟山101
		  	SendBaseFrame(1,0x02);
		  else
          	SendBaseFrame(0,0x0c);
          if(g_SendBeatFailureNum < 4)
              g_SendBeatFailureNum++;
          m_zdflag = 0;
        }
        if(g_SendYcDingshi == 0x55)
        {
          g_SendYcDingshi = 0; 
          m_wSendYcNum = 0;
#ifdef YN_101S     
          if(SendYCGroup(0,1,35) == FALSE)
#else 		  
          if(SendYCGroup(0,1,9) == FALSE)
#endif		  	
          {
            //m_SendYcFlag = 0;
            m_wSendYcNum=0;
          }
          m_zdflag = 0;
          
        }
        if(g_SendZJDingshi == 0x55)
        {
          g_SendZJDingshi = 0; 
          m_wSendZJNum = 0;
	   if(pGprs != null)          pGprs->m_GprsVSQ = 0x55;
	   if(pDbg!= null)          pDbg->m_GprsVSQ = 0x55;
	   if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(84,11,this);	   
	   delayms(2000);
          if(SendZJGroup(0,1,36) == FALSE)
          {
            m_wSendZJNum=0;
          }
          m_zdflag = 0;
        }		
        if(g_SendYXDingshi == 0x55)
        {
          g_SendYXDingshi = 0; 
          m_wSendYxNum = 0;
          if(SendYXGroup(0,1,1) == FALSE)
          {
            //m_SendYcFlag = 0;
            m_wSendYxNum=0;
          }
          m_zdflag = 0;
        }
        if(m_SendStatusFlag == YES)
        {
            if(SendDYXGroup(0,3) == FALSE)
            {
                if(SendYXGroup(0,3,1) == FALSE)
                {
                    m_SendStatusFlag = NO;
                    m_wSendYxNum=0;   
                    m_wSendDYxNum = 0;
                }
            }
        }
         
    }//b
    if(m_recfalg)
    {
        m_recfalg=0;
        SendNoData();
    }
    if(m_PaWaitflag_lubo == TRUE && m_PaWaitCt_lubo == 0)
     {
          if(m_TxNum_lubo < 20)
          {
            
            m_PaWaitCt_lubo =  4;//g_gRunPara[RP_LUBOGPRS_T];
            if(m_uartId == g_CmIdGPRS)m_PaWaitCt_lubo =  100;
            m_PaWaitflag_lubo = ON;
           //Code_Lubo(gRecorder_flag.pRXBuff,m_SendBuf.pBuf);// 
           SendlbRetry();
            m_TxNum_lubo++;gRes_rec.res_timeout = 0x55;
			//g_gRunPara[RP_PLUSEXH_MODFK]++;
            return;
          }
          else
          {
            m_PaWaitflag_lubo = OFF;
            m_TxNum_lubo = 0;
            m_PaWaitCt_lubo = 0;
            gRecorder_flag.LIST_flag = OFF;
      }    
      
     }
     if((gRecorder_flag.LIST_flag == ON)&&(m_ackflag))//(( gRecorder_flag.CFG_flag ==ON )||(gRecorder_flag.DAT_flag ==ON )||)//正在读配置文件的数据
     {
              m_ackflag =0;
              //g_Cmid = m_uartId;
              //memcpy(&pReceiveFrame->Frame68.Start1,gRecorder_flag.pRXBuff,256); 
              //if(FILEPREPAR_TYPE)
              //Code_Lubo(&pReceiveFrame->Frame68.Start1,m_SendBuf.pBuf);
              Code_Lubo(gRecorder_flag.pRXBuff,m_SendBuf.pBuf);
		gRes_rec.res_timeout = 0;	  
     }
     if((gRecorder_flag.LIST_flag_YN== ON)&&(m_ackflag))//(( gRecorder_flag.CFG_flag ==ON )||(gRecorder_flag.DAT_flag ==ON )||)//正在读配置文件的数据
     {
              m_ackflag =0;
              Code_Lubo_YN(gRecorder_flag.pRXBuff,m_SendBuf.pBuf);
		gRes_rec.res_timeout = 0;	  
     }	 
//	 国网读录波文件规约
   if(((mRecorder_flag.LIST_flag == ON)||(mRecorder_flag.xuchuanflag== ON))&&(m_ackRecorder ==ON))//(( gRecorder_flag.CFG_flag ==ON )||(gRecorder_flag.DAT_flag ==ON )||)//正在读配置文件的数据
     {
         m_ackRecorder =OFF;
         BYTE mRecorder_RXBuff[256];
         if(m_uartId == g_CmIdDBG)//if(pDbg != null)
         {
            CAT_SpiReadBytes(EEPADD_DEGBF101, 255, mRecorder_RXBuff);
         }
         else if(m_uartId == g_CmIdGPRS)//if(pGprs != null)
         {
            CAT_SpiReadBytes(EEPADD_GPRSBF101, 255, mRecorder_RXBuff);
        }              
        Recfileprocessing(mRecorder_RXBuff);//(mRecorder_flag.pRXBuff);
     }
   
    //只针对GPRS通道进行判断提前关GPRS
  /*  if((g_sTimer[TM_GPRSPWOERDOWN].m_TmCountBk != 60) && (g_gRunPara[RP_POWER_MODE]!= REALTIME) && g_GprsPowerSt && (m_uartId == g_CmIdGPRS))
    {
        if((CheckHaveDataToSd() == FALSE) &&(!g_JdgPwMode))
        {
          OpenTimer(TM_GPRSPWOERDOWN,60);//延迟1分钟关闭GPRS
          BYTE bBuf[2] = {0xEB,0XEB};
          MyPrintfD(bBuf,2);
        }
    }*/

}

/***************************************************************
    Function：OnTimeOut
        定时处理
    参数：TimerID
        TimerID 定时器ID
    返回：无
***************************************************************/
BOOL CBJ101S::DoTimeOut(WORD wTimerID)
{
	CPrtcSms::DoTimeOut(wTimerID);
   	if(m_reSendDelay > 0)
   		{
      	int iTime = 0;
      	if(g_gRunParaYxRsdTTBk != g_gRunPara[RP_YXRSD_T])
      		{//遥信重发时间间隔发送变化
          	iTime = g_gRunPara[RP_YXRSD_T] - (g_gRunParaYxRsdTTBk - m_reSendDelay);
        	if(iTime < 0)  iTime = 1;
        	m_reSendDelay = iTime;
        	g_gRunParaYxRsdTTBk = g_gRunPara[RP_YXRSD_T];
      		}
     //去掉下边两句代码是因为，若两个故障间隔时间很短，第一个故障刚发出，
     //还没等到主站回复(与主站通信正常)，此时提前结束了重发计时，会将上次的故障又发一次。
     // if(SearchCos(m_NewFltBit,1) && SearchCos(m_SYxBit,1))
     //  m_reSendDelay = 1;//若有新产生的故障，提前结束重发计时
      	if(m_reSendDelay>0)//增加该部分是因为=0时，程序会跑飞
        	m_reSendDelay--;
      	if(m_reSendDelay == 0)
      		{
        	m_resendflag = 1;
        	return TRUE;
      		}
   		}
   	if(g_gRunParaYxRsdTTBk != g_gRunPara[RP_YXRSD_T]) 
      	g_gRunParaYxRsdTTBk = g_gRunPara[RP_YXRSD_T];


  	if(m_YkWaitCount > 0)
  		{//遥控计时
      	m_YkWaitCount--;
      	if(g_YkOrderFlag == TRUE ||g_YkOrderFlag == FALSE)
          	m_YkWaitCount = 0;
      	if(m_YkWaitCount <= 0)
      		{
        	m_YKflag=1;
       		return TRUE;
      		}
   		}
   	if(m_PaWaitflag_lubo == ON)
   		{
      	if(m_PaWaitCt_lubo > 0)
      		{//ODU写参数重发计时
         	m_PaWaitCt_lubo--;
          	if(m_PaWaitCt_lubo <= 0)
          		{
            	m_PaWaitflag_lubo = TRUE;
            	return TRUE;
          		}
       		}
    	}
    if(g_sTimer[TM_SENDYC].m_TmFlag & m_SendYcBit)
    {//启动定时传输遥测
        g_sTimer[TM_SENDYC].m_TmFlag &= ~m_SendYcBit;
        m_SendYcFlag = 1;
        m_wSendYcNum = 0;
        g_yxChangeflag |= BITF;//g_yxChangeflag的最高位标识有报文需要上传
        return TRUE;
    }
  /*  if(g_gRunPara[RP_BEAT_T] > 0)//如果心跳传输时间大于0才启动心跳传输
    {
        if(g_sTimer[TM_BEAT].m_TmCount == 0)
            OpenTimer(TM_BEAT);
    }
    if(g_SendYc && g_sTimer[TM_SENDYC].m_TmCount == 0 && (g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ISSENDYC]))
    {
        OpenTimer(TM_SENDYC);
    }*/
    
    
   if(g_sTimer[TM_BEAT].m_TmFlag & m_BeatBit)
   {//启动心跳传输
     g_sTimer[TM_BEAT].m_TmFlag &= ~m_BeatBit;
      m_BeatFlag = 1;
   }
   
  if(m_BeatFlag)
    return TRUE;


  return FALSE;
}

BOOL CBJ101S::SendCallgroup(void)
{
  int i = 0,j = 0;
  
    if(m_groupflag&0x80)
    {
      m_groupflag&=0x7f;
      RecCallAllCommand();
      return TRUE;
    }
   for(i = CALL_YXGRP1,j = 0; i < CALL_DYXGRP1;i++,j++)
   {//单点遥信
      if (GetEqpFlag(i))
        {
           ClearEqpFlag(i);
           m_wSendYxNum=0;
           if (SendYXGroup(j,21+j,1))
              return TRUE;
        }
   }
    if (GetEqpFlag(CALL_DYXGRP1))
    {//双点遥信
      ClearEqpFlag(CALL_DYXGRP1);
        m_wSendDYxNum=0;
      if (SendDYXGroup(0,20))
        return TRUE;
    }
    for(i = CALL_YCGRP1,j = 0; i < CALL_ALLSTOP;i++,j++)
   {//遥测
      if (GetEqpFlag(i))
        {
           ClearEqpFlag(i);
           m_wSendYcNum=0;
            if(SendYCGroup(j,29+j,9))
              return TRUE;
        }
   }
  if (CheckClearEqpFlag(CALL_ALLSTOP))
    {
      m_groupflag=0;
      if (SendAllStop())
          return TRUE;
    }
    return FALSE;

    
    
}

//时间同步ACK回复
BOOL CBJ101S::SendtimeAck(void)
{
    BYTE Style = 0x67, Reason = COT_ACTCON;
    BYTE PRM = 0, dwCode = 3, Num = 1;
        m_acdflag=0;
    SendFrameHead(Style, Reason);
    write_infoadd(0);
    write_time();
    if(SwitchToAddress(m_dwasdu.LinkAddr))
      SendFrameTail(PRM, dwCode, Num,0);

    return TRUE;
}
//时间请求REQ回复
BOOL CBJ101S::SendtimeReq(void)
{
    BYTE Style = 0x67, Reason = COT_REQ;
    BYTE PRM = 0, dwCode = 3, Num = 1;
        m_acdflag=0;
    SendFrameHead(Style, Reason);
    write_infoadd(0);
    write_time();
    if(SwitchToAddress(m_dwasdu.LinkAddr))
      SendFrameTail(PRM, dwCode, Num,0);

    return TRUE;
}
BOOL CBJ101S::SenddelayeAck(void)
  {
      BYTE Style = 0x6A, Reason = COT_ACTCON;
      BYTE PRM = 0, dwCode = 3, Num = 1;
          m_acdflag=0;
      SendFrameHead(Style, Reason);
      write_infoadd(0);
      
      ReadRealTime();
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(g_sRtcManager.m_gRealTimer[RTC_MICROSEC]+g_sRtcManager.m_gRealTimer[RTC_SEC]*1000)&0xff;
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(g_sRtcManager.m_gRealTimer[RTC_MICROSEC]+g_sRtcManager.m_gRealTimer[RTC_SEC]*1000)>>8;
      //write_time();
      if(SwitchToAddress(m_dwasdu.LinkAddr))
        SendFrameTail(PRM, dwCode, Num,0);
      return TRUE;
  }



//链路测试回复报文
BOOL CBJ101S::SendTsetLinkAck(void)
{
    //SendBaseFrame(0,0);//张弢重庆 总召确认报文
//delayms(100);
    BYTE Style = 0x68, Reason = COT_ACTCON;
    BYTE PRM = 0, dwCode =3, Num = 1;

    SendFrameHead(Style, Reason);
    write_infoadd(0);

    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0xAA;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x55;
    SendFrameTail(PRM, dwCode, Num,0);

    return TRUE;
}
//复位RTU的ACK回复报文
BOOL CBJ101S::SendresetAck(void)
{
    BYTE Style = 105, Reason = COT_ACTCON;
    BYTE PRM = 0, dwCode =3, Num = 1;
#ifdef YN_101S
	Style = 0x2e;
#endif
    SendFrameHead(Style, Reason);
#ifdef YN_101S
    write_infoadd(0x2001);
#else
    write_infoadd(0);
#endif
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_QRP;
    SendFrameTail(PRM, dwCode, Num,0);
    if(m_QRP==1)
    {
      delayms(100);
      _DINT();
      while(1); 
    }
    return TRUE;
}

#ifdef GETSOEFROMRAM
//发送单点SOE数据
#ifdef  ZT_SOE_IN_FLASH
/*
BOOL CBJ101S::SendSoe(void)
{
    BYTE Style = M_SP_TB, Reason = COT_SPONT;
    BYTE PRM = 0, dwCode = 3;
    WORD YXNo = 0;
    WORD SoeSendNum = 0; 
    BYTE *pBuf = null;
    WORD i;
   
    if(m_guiyuepara.yxtype==2)
        Style = M_SP_TA;        //短时标单点遥信
    WORD RecSoeNum = GetCosNum(m_SSoeHeadPtr,g_unSCosBuffTail);
    if (RecSoeNum == 0)
         return FALSE;
  else if(RecSoeNum > 24)
  {
    RecSoeNum =24;  
        g_yxChangeflag |= m_SSoeBit;  
  }
    m_SSoeHeadPtrBk = m_SSoeHeadPtr;//在准实时模式下，如果故障复位，等初始化之后只会上传复位信号
    SendFrameHead(Style, Reason);
    
    for(i = 0; i < RecSoeNum; i++,m_SSoeHeadPtr++)
    {
        m_SSoeHeadPtr &= (SOE_BUFF_NUM - 1);
        pBuf = g_gSCosBuff[m_SSoeHeadPtr];
        YXNo = MAKEWORD(pBuf[SOE_TYPEL],pBuf[SOE_TYPEH]);
        write_infoadd( YXNo + ADDR_YX_LO);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pBuf[SOE_STVAL];
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MSL];
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MSH];
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MINU];
        if(m_guiyuepara.yxtype==3)//长时标单点遥信
        {
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_HOUR] & 0x1F;
            BYTE week=0;
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (pBuf[SOE_DAY] & 0x1F) | ((week <<5) & 0xE0);
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MONTH];
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_YEAR];
        }
        SoeSendNum++;
    }
    SendFrameTail(PRM, 0xA3, SoeSendNum);
    return TRUE;
}
*/
#else
BOOL CBJ101S::SendSoe(void)
{
    BYTE Style = M_SP_TB, Reason = COT_SPONT;
    BYTE PRM = 0;//, dwCode = 3;
    WORD YXNo = 0;
    WORD SoeSendNum = 0; 
    //DWORD SSoeN = 0;	
    //BYTE *pBuf = null;
    DWORD i;
    //long RealNum;	
    //unsigned int RecSoeNum;
    unsigned char untemp[12],k;
   
    if(m_guiyuepara.yxtype==2)
        Style = M_SP_TA;        //短时标单点遥信
/*
    m_SendSoeAdr = m_SendSoeAdr&0x3ff0;	
    SSoeN= m_SendSoeAdr+240;//240 为15条soe的存储空间
    if(SSoeN>=EEPADD_SOEENDADR) SSoeN -= EEPADD_SOESTARTADR;
    if(SSoeN >= g_unSSoeSaveE2ROMPtr)
    	{SSoeN = g_unSSoeSaveE2ROMPtr;}
    else
    	{g_yxChangeflag |= m_SSoeBit;}
    if(m_SendSoeAdr<SSoeN)
    	{
    	RecSoeNum = (SSoeN-m_SendSoeAdr)>>4;
    	}
    else
    	{
    	RecSoeNum =(MAX_SOE_BYTE+SSoeN-m_SendSoeAdr)>>4;
    	}
    	
 */   	
    m_SSoeHeadPtrBk = m_SSoeHeadPtr;//在准实时模式下，如果故障复位，等初始化之后只会上传复位信号
    SendFrameHead(Style, Reason);
   m_SSoeHeadPtr &= 0x3ff0;g_unSSoeSaveE2ROMPtr &= 0x3ff0;
//    for(i =0; i < RecSoeNum; i++,m_SSoeHeadPtr++)
    for(i =0; (m_SSoeHeadPtr!= g_unSSoeSaveE2ROMPtr)&&(i<13); i++)
    {
    /*
        m_SSoeHeadPtr = m_SendSoeAdr;        
        //RealNum =(i-1)*16+g_unSSoeSaveFlashHead;
      	if(m_SendSoeAdr>=MAX_SOE_BYTE)
      		m_SendSoeAdr -= MAX_SOE_BYTE;
	Sst26vf064b_Read(m_SendSoeAdr,untemp,11); 
	m_SendSoeAdr+=16;    		
*/
	m_SSoeHeadPtr &= 0x3ff0;
	CAT_SpiReadBytes(m_SSoeHeadPtr, 11, untemp); 
	m_SSoeHeadPtr += 16;
	if(m_SSoeHeadPtr>=EEPADD_SOEENDADR) m_SSoeHeadPtr = EEPADD_SOESTARTADR;
	
        YXNo = MAKEWORD(untemp[SOE_TYPEL],untemp[SOE_TYPEH]);
	YXNo=	YXNo - g_gRunPara[RP_SYX_INFADDR] +  1;   //- 1 + TsSoeType - g_ucSYxTrsStartId
	for(k = 0; k < g_ucYxTransNum;k++)
       {
          if(g_ucYXAddr[k]==YXNo+1)
          {
                YXNo =k+g_gRunPara[RP_SYX_INFADDR]-1;
		 //YXNo=g_ucYXAddr[YXNo]+g_gRunPara[RP_SYX_INFADDR] -  1;
        	write_infoadd( YXNo + ADDR_YX_LO);
        	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = untemp[SOE_STVAL];
        	m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = untemp[SOE_MSL];
        	m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = untemp[SOE_MSH];
        	m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = untemp[SOE_MINU];
       	if(m_guiyuepara.yxtype==3)//长时标单点遥信
        	{
            		m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = untemp[SOE_HOUR] & 0x1F;
            		BYTE week=0;
            		m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (untemp[SOE_DAY] & 0x1F) | ((week <<5) & 0xE0);
            		m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = untemp[SOE_MONTH];
            		//m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = i;
            		m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = untemp[SOE_YEAR];
        	}
        	SoeSendNum++;	
		 k =g_ucYxTransNum;	
              break;
            }
        }
    }
#ifdef  YN_101S	
    SendFrameTail(PRM, 0xa3, SoeSendNum,0);
#else
  #ifdef  CQ_101S	
    SendFrameTail(PRM, 0xa3, SoeSendNum,0);
  #else
    SendFrameTail(PRM, 0x03, SoeSendNum,0);
  #endif
#endif
    //g_unSoeSendNum=RecSoeNum+1;
    unsigned int unTTemp[4];
    m_SendSoeAdr = m_SendSoeAdr&0x3ff0;
    unTTemp[0]=pDbg->m_SSoeHeadPtr;unTTemp[1]=unTTemp[0];
    unTTemp[2]=pGprs->m_SSoeHeadPtr;	unTTemp[3]=unTTemp[2];
    CAT_SpiWriteWords(EEPADD_SOESEND_E2ROMADR,4, unTTemp);	
    if(m_SSoeHeadPtr!=g_unSSoeSaveE2ROMPtr)
		g_yxChangeflag |= m_SSoeBit;	
    return TRUE;
}
#endif
#else //soe从外部flash中取
/*
//发送单点SOE数据
BOOL CBJ101S::SendSoe(void)
{
    BYTE Style = M_SP_TB, Reason = COT_SPONT;
    BYTE PRM = 0, dwCode = 3;
    WORD YXNo = 0;
    BYTE *pBuf = null;
    BYTE bSoeBuf[255]={0};
    WORD i;
    #define FSOE_NUM_MAX  24
   
    if(m_guiyuepara.yxtype==2)
        Style = M_SP_TA;        //短时标单点遥信

    WORD RecSoeNum =ReadSoeFromFlash((WORD *)(&m_SSoeHeadPtr),g_unSSoeInFlashTail,FSOE_NUM_MAX,bSoeBuf);
    if (RecSoeNum == 0)
         return FALSE;
    if(m_SSoeHeadPtr != g_unSSoeInFlashTail)
    {  
        g_yxChangeflag |= m_SSoeBit;  
    }
    m_SSoeHeadPtrBk = m_SSoeHeadPtr;
    SendFrameHead(Style, Reason);
    pBuf = bSoeBuf;
    for(i = 0; i < RecSoeNum; i++)
    {
        YXNo = MAKEWORD(pBuf[SOE_TYPEL],pBuf[SOE_TYPEH]);
        write_infoadd( YXNo + ADDR_YX_LO);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pBuf[SOE_STVAL];
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MSL];
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MSH];
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MINU];
        if(m_guiyuepara.yxtype==3)//长时标单点遥信
        {
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_HOUR] & 0x1F;
            BYTE week=0;
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (pBuf[SOE_DAY] & 0x1F) | ((week <<5) & 0xE0);
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MONTH];
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_YEAR];
        }
        pBuf += SOE_DATA_LEN;
    }
    SendFrameTail(PRM, dwCode, RecSoeNum);
    return TRUE;
}
*/
#endif

//发送双点SOE数据
BOOL CBJ101S::SendDSoe(void)
{
  /*  BYTE Style = M_DP_TB, Reason = COT_SPONT;
    BYTE PRM = 0, dwCode = 3;
    DWORD SoeSendNum = 0;
    WORD RecSoeNum;
    WORD YXNo;
    BYTE *pBuf = null;
    WORD i;

    if(m_guiyuepara.yxtype==2)
        Style = M_DP_TA;

     RecSoeNum = GetCosNum(m_DSoeHeadPtr,g_unDCosBuffTail,DSOE_BUFF_NUM);
    if (RecSoeNum == 0)
         return FALSE;
    m_DSoeHeadPtrBk = m_DSoeHeadPtr;
    SendFrameHead(Style, Reason);

   for(i = 0; i < RecSoeNum; i++,m_DSoeHeadPtr++)
    {
        m_DSoeHeadPtr &= (DSOE_BUFF_NUM - 1);
        pBuf = g_gDCosBuff[m_DSoeHeadPtr];
        YXNo = MAKEWORD(pBuf[SOE_TYPEL],pBuf[SOE_TYPEH]);
        write_infoadd( YXNo + ADDR_YX_LO);     
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pBuf[SOE_STVAL];
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MSL];
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MSH];
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MINU];
        if(m_guiyuepara.yxtype==3)//长时标单点遥信
        {
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_HOUR] & 0x1F;
            BYTE week=0;
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (pBuf[SOE_DAY] & 0x1F) | ((week <<5) & 0xE0);
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MONTH];
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_YEAR];
        }
        SoeSendNum++;
    }
    if (SoeSendNum == 0)
         return FALSE;
    SendFrameTail(PRM, dwCode, SoeSendNum);*/

    return TRUE;
}

//发送变化遥测数据
BOOL CBJ101S::SendChangeYC(void)
{
    BYTE  Reason = COT_SPONT;
    BYTE PRM = 0;//, dwCode = 0;
	char n=0;
  /*  WORD ChangeYCNum = 0;
    WORD m_dwPubBuf[MAX_PUBBUF_LEN];
    VDBYCF *pBuf = (VDBYCF *)m_dwPubBuf;
    WORD wReqNum = m_pEqpInfo[0].wYCNum;
    ChangeYCNum = SearchChangeYC(wReqNum, (VDBYCF *)pBuf);
    int YxIndex = 0;*/
    if(g_gChangeYCNum  == 0)
      return FALSE;
#ifdef  YN_101S
    m_guiyuepara.yctype = 35;//0x1e;
#else
    m_guiyuepara.yctype = 9;
#endif
#ifdef  CQ_101S
    m_guiyuepara.yctype = 9;//张弢重庆//遥测突变不要时标
#endif  
    if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_YC_FLOAT])
    	{
    	m_guiyuepara.yctype = 13;//浮点数
    	}
    SendFrameHead(m_guiyuepara.yctype, Reason);
    for (int i=0; i < g_gChangeYCNum; i++)
    {
        char k;
        for(k= 0;k< g_ucYcTransNum;k++)
        {
          if  (g_ucYCAddr[k]==(g_gYCchangData[i]+1))
          	{
          	  write_infoadd(k +g_gRunPara[RP_YC_INFADDR]);
		  if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_YC_FLOAT])
    		 	{
    			FP32 flt;
			char *pdatachar=null;	
			flt = (FP32)(g_gYCchangData[i + 10]);	
			pdatachar = (char *)&flt;
			m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = *pdatachar++;//LOBYTE(LOWORD(flt));
			m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = *pdatachar++;// HIBYTE(LOWORD(flt));
			m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = *pdatachar++;// LOBYTE(HIWORD(flt));
			m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = *pdatachar++;// HIBYTE(HIWORD(flt));	
    			}
		  else
		  	{
        	  	m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_gYCchangData[i + 10]);
        	  	m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(g_gYCchangData[i + 10]);
		  	}
			  n++;
		  break;
          	}
		  	
        } 

        if(m_guiyuepara.yctype!=M_ME_ND)
        {
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
        }
        if((m_guiyuepara.yctype==10)||(m_guiyuepara.yctype==12))
            write_time3();
        if((m_guiyuepara.yctype==34)||(m_guiyuepara.yctype==35))
            write_time();
#ifdef  YN_101S//云南突变遥测 状态标志为30 0X1E
        //if(m_guiyuepara.yctype==30)
		//write_time();	
#endif		
        //pBuf++;
    }
#ifdef  YN_101S	
    SendFrameTail(PRM, 0xa3, n);
#else
  #ifdef  CQ_101S	
    SendFrameTail(PRM, 0xa3, n,0);
  #else
    SendFrameTail(PRM, 0x03, n,0);
  #endif
#endif
    g_gChangeYCNum = 0;
    return TRUE;
}
//获取控制域
BYTE CBJ101S::GetCtrCode(BYTE PRM,BYTE dwCode,BYTE fcv)
{
    BYTE CodeTmp = 0x00;
    dwCode&=0xf;
    CodeTmp += dwCode;

    if (PRM)
        CodeTmp |= 0x40;  //云南更改
        
    if(m_guiyuepara.mode==0)
    {
        //CodeTmp |= 0x80;//lxq  DIR 方向位
        if (SearchClass1()&&(m_linkflag)&&(CodeTmp!=0xb))
            CodeTmp |= 0x20;
        m_acdflag=0;
    }
    else
    {
        if(m_DIR)//DIR auto fit
          CodeTmp&=0x7f;
        else 
          CodeTmp|=0x80;
/*#ifndef YN_101s*/
        if(fcv)
        {
            if(!m_resendflag)//非重发报文才进行fcb的翻转
            {
              if(m_fcb==0)
                  m_fcb=0x20;
              else
                  m_fcb=0;
            }
            CodeTmp|=(m_fcb|0x10);            
        }
/*#endif*/
    }
    return CodeTmp;
}

//发送固定帧格式基本报文
BOOL CBJ101S::SendBaseFrame(BYTE PRM,BYTE dwCode)
{
    WORD wLinkAddress;
    BYTE SendData[256];
    BYTE SendDataLen;
    m_SendBuf.wReadPtr = m_SendBuf.wWritePtr = 0;

    pSendFrame = (VIec101Frame *)m_SendBuf.pBuf;

    pSendFrame->Frame10.Start = 0x10;
    pSendFrame->Frame10.Control = GetCtrCode(PRM,dwCode,0);
    write_10linkaddr(GetAddress());
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (BYTE)ChkSum((BYTE *)&pSendFrame->Frame10.Control,m_guiyuepara.linkaddrlen+1);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x16;
    wLinkAddress = m_dwasdu.LinkAddr;
    if(pSendFrame->Frame10.Control &0x40)
      m_recfalg=0;
    if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ENCRYPT])
    {
        SendDataLen = m_SendBuf.wWritePtr;
        memcpy(SendData,m_SendBuf.pBuf,SendDataLen);
        SendFrameEBHead(0x0000,0x00);  
        SendAppData(SendDataLen,SendData);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
        //SendFrameEBEixampleData(SendDataLen,SendData);
        SendFrameEBTail();
    }
    else
    {
    m_SendBuf.wReadPtr = 0;
    if(SwitchToAddress(m_dwasdu.LinkAddr))
    WriteToComm(wLinkAddress);
    }  

    return TRUE;
}

BOOL CBJ101S::SendLinktesetFrame(BYTE PRM,BYTE dwCode)
{
    WORD wLinkAddress;
    BYTE SendData[256];
    BYTE SendDataLen;
    m_SendBuf.wReadPtr = m_SendBuf.wWritePtr = 0;

    pSendFrame = (VIec101Frame *)m_SendBuf.pBuf;

    pSendFrame->Frame10.Start = 0x10;
    pSendFrame->Frame10.Control = GetCtrCode(PRM,dwCode,1);
        write_10linkaddr(GetAddress());
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (BYTE)ChkSum((BYTE *)&pSendFrame->Frame10.Control,m_guiyuepara.linkaddrlen+1);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x16;
        wLinkAddress = m_dwasdu.LinkAddr;
        if(pSendFrame->Frame10.Control &0x40)
        m_recfalg=0;

    if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ENCRYPT])
    {
        SendDataLen = m_SendBuf.wWritePtr;
        memcpy(SendData,m_SendBuf.pBuf,SendDataLen);
        SendFrameEBHead(0x0000,0x00);  
        SendAppData(SendDataLen,SendData);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
        //SendFrameEBEixampleData(SendDataLen,SendData);
        SendFrameEBTail();
    }
    else
    {
    m_SendBuf.wReadPtr = 0;
    if(SwitchToAddress(m_dwasdu.LinkAddr))
    WriteToComm(wLinkAddress);
    }  

    return TRUE;
}


//发送复位远方链路 平衡式
BOOL CBJ101S::SendResetLink(BYTE PRM)
{
    return SendBaseFrame(PRM,0x00);
}

//发送请求远方链路状态 平衡式
BOOL CBJ101S::SendReqLink(void)
{
    return SendBaseFrame(1, 0x09);
}


//flag = 0:读出相应状态位并写0；=1:只读
BOOL CBJ101S::SearchCos(WORD wBit,BYTE flag)
{
    if(g_yxChangeflag & wBit)
    {
        if(flag != 1)
          g_yxChangeflag &= ~wBit;
        return TRUE;
    }
    else
      return FALSE;
}

//检查有无1级数据
BOOL CBJ101S::SearchClass1(void)
{
    if(m_guiyuepara.mode==1)
    {
      if(m_initflag)
              return TRUE;
      if (SearchCos(m_SYxBit|m_DYxBit|m_SSoeBit|m_DSoeBit,1))
        return TRUE;
      if (GetEqpFlag(CALL_DATA))
              return TRUE;
      if(m_acdflag)
          return TRUE;

    }
    else
    {
      if(m_initflag)
        return TRUE;
      if (m_YKflag)
        return TRUE;
      if (SearchCos(m_SYxBit|m_DYxBit|m_SSoeBit|m_DSoeBit,1))
        return TRUE;
      if (m_callallflag)
        return TRUE;
     if (m_YKstop)
       return TRUE; /*测试16*/
    }
    return FALSE;
}
//发送新增录波目录
//#ifdef YN_101S
BOOL CBJ101S::SendLBML(void)
{
    //BYTE Style = M_SP_NA;
    BYTE Reason = COT_SPONT;
    BYTE PRM = 0;
    //BYTE dwCode = 3;
    //unsigned int temp;	
    PRM = 1;

    SendFrameHead(0x88, Reason);//类型标示137 0x88
    /*
          temp = g_sRecData.m_gRecCNum;
	   if(g_sRecData.m_gRecCNum==0)
	   	temp = MAX_REC_NUM-1 ;
	   else
	   	temp = g_sRecData.m_gRecCNum-1;
          long FADDR_RECORDER =FADDR_RECORDER_INFO+ (long)temp*(long)FLINEADDR;
          Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_filecfg,sizeof(gRecorder_filecfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来 
*/
          //信息体地址 0x408a
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0x8a;// LOBYTE(wave_total);
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0x40;// LOBYTE(temp);
          //总包1字节 当前包1字节
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =1;// LOBYTE(wave_total);
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;// LOBYTE(temp);
          //cfg文件名 8字节 7字节时间+1字节编号	   
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//录波触点时间
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_SEC];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(gRecorder_filecfg.comtrade_time[RTC_WEEK]<<5);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR];
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = (gRecorder_filecfg.FileName<<1) &0xfe;//0位=0， cfg文件
          //文件长度 3字节      
          //*pTxBuf++ = gRecorder_filecfg.TOTAL_Leng;
         // *pTxBuf++ = gRecorder_filecfg.TOTAL_Leng>>8;
         // *pTxBuf++ = gRecorder_filecfg.TOTAL_Leng>>16; 
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 3;
	  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x01;	  
	   m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;	 
          //SOF 1字节 	
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = gRecorder_filecfg.CFG_SOF;
          //文件创建时间 7字节       
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//录波触点时间
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_SEC];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(gRecorder_filecfg.comtrade_time[RTC_WEEK]<<5);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR];
	   //*****************************************************	
          //data文件名 8字节 7字节时间+1字节编号		   
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//录波触点时间
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_SEC];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(gRecorder_filecfg.comtrade_time[RTC_WEEK]<<5);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR];	
	  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = (gRecorder_filecfg.FileName<<1) |0x01;////0位=1， data文件
          //文件长度 3字节      
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = gRecorder_filecfg.TOTAL_Leng*20;
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = (gRecorder_filecfg.TOTAL_Leng*20)>>8;
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = (gRecorder_filecfg.TOTAL_Leng*20)>>16;  
          //SOF 1字节 	
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = gRecorder_filecfg.CFG_SOF;
          //文件创建时间 7字节       
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//录波触点时间
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_SEC];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(gRecorder_filecfg.comtrade_time[RTC_WEEK]<<5);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR];	

    SendFrameTail(PRM, 0x03, 2,0);//可变结构限定词 2

    return TRUE;
}
//#endif
//发送变化遥信
BOOL CBJ101S::SendCos(void)
{
    BYTE Style = M_SP_NA;
    BYTE Reason = COT_SPONT;
    BYTE PRM = 0;
    //BYTE dwCode = 3;
    BYTE YXSendNum = 0;
    WORD YXNoInRmt;//总表中的序号
    WORD i,YXValue;
    	
    //WORD RecCosNum;
   // BYTE *pBuf = null;
    PRM = 1;

    SendFrameHead(Style, Reason);
    for(i = 0; (YXSendNum < FRM_MAX_COS_NUM) && (i < RMT_INFO_NUM); i++)   //m_pEqpInfo[m_wEqpNo].wSYXNum
    {
      if(g_ucYXAddr[i + g_ucSYxTrsStartId]==0)break;
      YXNoInRmt = g_ucYXAddr[i + g_ucSYxTrsStartId] - 1;
      if(GETBIT(m_gRmtChgFlag,YXNoInRmt))
      {
        write_infoadd( i + g_gRunPara[RP_SYX_INFADDR]);  

        //m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = GETBIT(g_gRmtInfo,i);
	YXValue = g_gRmtInfo[(g_ucYXAddr[i]-1)] ;
        if(YXValue != 0)
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 1;
        else
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;	
        //张弢 录波m_ucCosBk[YXSendNum] = YXNoInRmt;
        //清遥信变化标志位，在重传的地方将g_ucYxIdBk中保存的遥信对应的g_gRmtChgFlag置1
        //如果在重传过程中某位又产生COS，重传时将上传最新状态(前次未传成功的中间状态被丢弃)
        //如果在等待主站确认的过程中某位又产生了COS，那么g_gRmtChgFlag会被重新置1，下次检索时会将新状态上传
        //如果不用重传，则已传的cos到此为止
        SETBIT(m_gRmtChgFlag,YXNoInRmt,0);
        YXSendNum++; 
      }
    }
    m_ucCosBkNum = YXSendNum;
    if(YXSendNum == 0)
     return FALSE;
     
#ifdef  YN_101S	
    SendFrameTail(PRM, 0xa3, YXSendNum,0);
#else
  #ifdef  CQ_101S	
    SendFrameTail(PRM, 0xa3, YXSendNum,0);
  #else
    SendFrameTail(PRM, 0x03, YXSendNum,0);
  #endif
#endif
    return TRUE;
}
//发送变化遥信
BOOL CBJ101S::SendDCos(void)
{
    BYTE Style = M_DP_NA;
    BYTE Reason = COT_SPONT;
    BYTE PRM = 1;
    BYTE dwCode = 3;
    BYTE YXSendNum = 0;
    //WORD YXNoInRmt;//总表中的序号
    WORD i;
    PRM = 1;

    SendFrameHead(Style, Reason);
    for(i = 0; (YXSendNum < FRM_MAX_COS_NUM) && (i < m_pEqpInfo[m_wEqpNo].wDYXNum); i++)
    {
      //YXNoInRmt = g_ucYXAddr[i + g_ucDYxTrsStartId] - 1;
   /*   if(GETBIT(m_gRmtChgFlag,YXNoInRmt))
      {
        write_infoadd( i + g_gRunPara[RP_DYX_INFADDR]);  
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = GetRIStatus(i,2);
        m_ucCosBk[YXSendNum] = YXNoInRmt;
        SETBIT(m_gRmtChgFlag,YXNoInRmt,0);
        YXSendNum++; 
      }*/
    }
    if(YXSendNum == 0)
     return FALSE;    
     
    SendFrameTail(PRM, dwCode, YXSendNum,0);
    return TRUE;    
}

//组织报文头
BOOL CBJ101S::SendFrameHead(BYTE Style, BYTE Reason)
{
    m_SendBuf.wReadPtr = 0;
    m_SendBuf.wWritePtr=0;
     if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ENCRYPT]) //启动加密
    {
        m_SendBuf.wWritePtr += 2;  //加密预留2字节给应用数据类型和应用数据长度
        pSendFrame = (VIec101Frame *)&m_SendBuf.pBuf[2];
    }
    else
    {
        pSendFrame = (VIec101Frame *)m_SendBuf.pBuf;
    }
    
    {
      //pSendFrame->Frame68.Start1  = pSendFrame->Frame68.Start2 = 0x68;
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr]=0x68;
      m_SendBuf.wWritePtr+=3;
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++]=0x68;
    }
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++]=0;
    write_linkaddr(GetAddress());
    write_typeid(Style);
    //write_VSQ(int data);
    write_COT((GetAddress()<<8)|Reason);
    write_conaddr(GetAddress());

    return TRUE;
}

//组织报文尾，并发送整帧报文
BOOL CBJ101S::SendFrameTail(BYTE PRM, BYTE dwCode, BYTE Num,BYTE EncType)
{
    WORD wLinkAddress;
//    BYTE temp;
    BYTE SendDataLen;
    BYTE EncData[256];
    if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ENCRYPT]) //启动加密
    {
        
        pSendFrame->Frame68.Length1 = pSendFrame->Frame68.Length2 = m_SendBuf.wWritePtr - 6;
    }
        
    else
        pSendFrame->Frame68.Length1 = pSendFrame->Frame68.Length2 = m_SendBuf.wWritePtr - 4;
    if((m_guiyuepara.mode!=1)&&(3==(dwCode&0xf)))
    {
      dwCode&=0xf0;
      dwCode|=8;
    }
    if((m_guiyuepara.mode==1)&&(8==(dwCode&0xf)))
    {
      dwCode&=0xf0;
      dwCode|=3;
    }
    
    if(dwCode == 0xA5) 
       pSendFrame->Frame68.Control = 0xA5;
    else if(dwCode == 0xA3)		
       pSendFrame->Frame68.Control = 0xA3;
    else if(dwCode == 0xf7)
       pSendFrame->Frame68.Control = 0x08;
    else
#ifdef YN_101S	
	  pSendFrame->Frame68.Control = GetCtrCode(m_PRM, dwCode,0);
#else	  
      pSendFrame->Frame68.Control = GetCtrCode(m_PRM, dwCode,1);
#endif
//#ifdef CQ_101S
//	pSendFrame->Frame68.Control =  temp;	
//#endif	
    if(Num == 0xff)  //云南遥控处理
    {
        pSendFrame->Frame68.Control = 0x80;
        Num = 1;
    }
#ifdef BJ101_GZTEST
    /*广州测试-遥控预置返校ACD为0*/
    if(m_YKSelectAck) pSendFrame->Frame68.Control = pSendFrame->Frame68.Control&0xDF;
#endif

#ifdef YN_101S
 	if(PRM == 0x55)
    	pSendFrame->Frame68.Control = 0xf3;
#endif    
 
    pSendFrame->Frame68.Data[m_guiyuepara.linkaddrlen+m_guiyuepara.typeidlen] = Num;

    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (BYTE)ChkSum((BYTE *)&pSendFrame->Frame68.Control, pSendFrame->Frame68.Length1);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x16;
    if (m_guiyuepara.linkaddrlen ==1)
        wLinkAddress = pSendFrame->Frame68.Data[0];
    else
        wLinkAddress = MAKEWORD(pSendFrame->Frame68.Data[0],pSendFrame->Frame68.Data[1]);
    m_recfalg=0;
    
    
     m_zdflag=1;

    m_WaitConfTpId = pSendFrame->Frame68.Data[m_guiyuepara.linkaddrlen];
    //若发送的报文不需要重发则将重发次数写成最大，重发时间置数是为了让非重发报文发送后也能等待一段时间才发故障报文
    if(m_WaitConfTpId != M_SP_NA && m_WaitConfTpId != M_DP_NA
     && m_WaitConfTpId != M_SP_TB && m_WaitConfTpId != M_DP_TB)
     {
        //m_retxdnum = g_gRunPara[RP_YXRSD_NUM];
        m_reSendDelay = g_gRunPara[RP_YXRSD_T];
     }	
    if(SwitchToAddress(m_dwasdu.LinkAddr))
    {
        if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ENCRYPT]) //启动加密
        {
            m_SendBuf.pBuf[0] = EncType;  //应用数据 类型
             //-2的原因是前面预留了1字节应用类型+1字节应用类型长度 ，SendFrameHead函数中
            m_SendBuf.pBuf[1] = m_SendBuf.wWritePtr - 2; 
            switch(EncType)
            {
            case 0x00:
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;   //增加2字节信息扩展区长度
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
                SendDataLen = m_SendBuf.wWritePtr ;//- 1;
                //memcpy(SendData,m_SendBuf.pBuf,SendDataLen);
                SendDataLen = EncryptData(m_SendBuf.pBuf,SendDataLen,EncData);
                if(SendDataLen) 
                {
                    SendFrameEBHead(0x0008,201); //应用类型大于等于200，不写应用类型，已在加密数据区
                    SendEncData(SendDataLen,EncData);
                    SendFrameEBTail();
                }            
                break;
            case 0x02:
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x00;
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x08;
                GetChipRandom(m_bUnitRandom);
                for(BYTE i = 0; i < 8; i++)
                {
                    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = m_bUnitRandom[i];//zxx
                }
                /*m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_sRtcManager.m_gRealTimer[RTC_YEAR];
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_sRtcManager.m_gRealTimer[RTC_DATE];
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_sRtcManager.m_gRealTimer[RTC_SEC];*/

                SendDataLen = m_SendBuf.wWritePtr;
                //memcpy(SendData,m_SendBuf.pBuf,SendDataLen);
                SendDataLen = EncryptData(m_SendBuf.pBuf,SendDataLen,EncData);
                //memcpy(SendData,m_SendBuf.pBuf,SendDataLen);
                if(SendDataLen) 
                {
                    SendFrameEBHead(0x0018,201); 
                    SendEncData(SendDataLen,EncData);
                    SendFrameEBTail();
                } 
                break;
            case 0x05:
                break;
            case 0x07:
                break;
            default:
                break;
            }
        }
        else
        {
            WriteToComm(wLinkAddress);//0X69开头的报文不判断链路地址
        }
        
   }
   return TRUE;
}


//无所请求数据帧
BOOL CBJ101S::SendNoData(void)
{
    if(m_guiyuepara.mode==0)
      return SendBaseFrame(0, 0x09);
    else 
      return TRUE; /*R227: 平衡模式无数据不送报文*/
}




//发送错误应答
BOOL CBJ101S::SenderrtypeAck(void)
{
  if(m_errflag==1)
     SendFrameHead(m_dwasdu.TypeID,(m_dwasdu.COT)|0x40);
  if(m_errflag==2)
     SendFrameHead(m_dwasdu.TypeID,(44)|0x40);
  if(m_errflag==3)
    SendFrameHead(m_dwasdu.TypeID,(47)|0x40);

    
    write_infoadd(m_dwasdu.Info);
    SendFrameTail(0, 8, 1,0);
    m_errflag=0;
    return TRUE;
}
//发送遥控结束
BOOL CBJ101S::SendYKstop(void)
{
BYTE Style, Reason = 10;
BYTE PRM = 0, dwCode = 3, Num = 1; /* #22 遥控预置/撤销/停止 确认报文的链路功能码应该为3,不是0 */
BYTE * pData = &pReceiveFrame->Frame68.Data[m_byInfoShift];

//Style = pReceiveFrame->Frame68.Data[m_byTypeIDShift];
/*R227: 遥控操作插入总召等报文, TypeID出错, 做如下修改*/
    VYKInfo *pYKInfo;
    pYKInfo = &(pGetEqpInfo(m_wEqpNo)->YKInfo);
    Style = pYKInfo->Head.abyRsv[0];

   SendFrameHead(Style, Reason);
   
    BYTE byInfoLen = m_guiyuepara.infoaddlen;
    if(m_guiyuepara.infoaddlen > 3) byInfoLen = 2;
    memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],pData,byInfoLen + 1);
    m_SendBuf.wWritePtr += (byInfoLen + 1); 

    SendFrameTail(PRM, dwCode, Num,2);
    return TRUE;
}
void CBJ101S::Initlink(void)
{
    if(m_initflag&1)    //请求远方链路状态
    {
        m_initflag&=~1;
	delayms(1000);		
        SendReqLink();
        return;
    }
	if(g_ucPara101[IECP_101_STY]==1)//舟山101
		m_initflag=0x08;
    if(m_initflag&2)  //复位远方链路
    {
        m_initflag&=~2;
	//delayms(1000);		
        SendResetLink(PRM_SLAVE);  //云南
        return;
    }
    if(m_initflag&4)
    {
        //m_initflag=0;
        m_initflag&=~4;
        m_fcb=0x20;
        if(((m_initfirstflag==1))||(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_ISSENDINITFIN]))
        {//只第一次上传或一直上传初始化长帧时发送
          m_initfirstflag=0;		  	 		 
          return SendInitFinish();
        }
       else
          m_initflag = 0;
    }
    else if(m_initflag&8)
    {
       m_initflag&=~8;
	   g_gRmtInfo[YX_101_LINK]=1;//SaveLOG(LOG_101_LINK,1);
    } 
    if(!m_initflag && (m_guiyuepara.mode == 1))  //平衡式
       m_linkflag=1;      
    return;
}
void CBJ101S::SendInitFinish(void)
{
  //if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_ISSENDINITFIN])
  {//用控制字来选择是否发送初始化长帧
#ifdef CQ_101S  
    SendFrameHead(M_EI_NA, 3);//张弢重庆
#else    
    SendFrameHead(M_EI_NA, COT_INIT);
#endif
    write_infoadd(0);
//  #ifdef BJ101_GXTEST
//  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = gResetReason;
//  #else
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0; //初始化原因  云南
    //#endif
    SendFrameTail(PRM_MASTER, 0x03, 1,0);//funcode=0x0a?  
}
    //if(m_guiyuepara.mode == 1)  //平衡式
    //    m_linkflag=1;
    return;
}

void CBJ101S::SendAck(void)
{
    //SendBaseFrame(PRM_SLAVE, SFC_CONFIRM);
    BYTE SendData[256];
    BYTE SendDataLen;
    WORD wLinkAddress;
    m_SendBuf.wReadPtr = m_SendBuf.wWritePtr = 0;

    pSendFrame = (VIec101Frame *)m_SendBuf.pBuf;

    pSendFrame->Frame10.Start = 0x10;
    //pSendFrame->Frame10.Control = 0x0;
    //if(m_guiyuepara.mode==0)
        pSendFrame->Frame10.Control = GetCtrCode(0,0,0);
    write_10linkaddr(GetAddress());
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (BYTE)ChkSum((BYTE *)&pSendFrame->Frame10.Control,m_guiyuepara.linkaddrlen+1);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x16;
    if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ENCRYPT])
    {
        SendDataLen = m_SendBuf.wWritePtr;
        memcpy(SendData,m_SendBuf.pBuf,SendDataLen);
        SendFrameEBHead(0x0000,0x00);  
        SendAppData(SendDataLen,SendData);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
        SendFrameEBTail();
    }
    else
    {
    wLinkAddress = m_dwasdu.LinkAddr;
    m_SendBuf.wReadPtr = 0;
    WriteToComm(wLinkAddress);
    }  

}
/***************************************************************
    Function：SearchOneFrame
        搜索一帧正确的报文
    参数：Buf, Len
        Buf 接收缓冲区头指针
        Len 接收缓冲区内有效数据的长度
    返回：DWORD数据，其中
        高字：处理结果
            #define FRAME_OK       0x00010000      //检测到一个完整的帧
            #define FRAME_ERR      0x00020000      //检测到一个校验错误的帧
            #define FRAME_LESS     0x00030000      //检测到一个不完整的帧（还未收齐）
        低字：已经处理的数据长度
***************************************************************/
DWORD CBJ101S::SearchOneFrame(BYTE *Buf, WORD Len)
{
    unsigned short FrameLen;
    WORD wLinkAddress;
    if (Len < 5)
        return FRAME_LESS;

    pReceiveFrame = (VIec101Frame *)Buf;
    switch(pReceiveFrame->Frame10.Start)
    {
        
        case 0xAA:
              if((pReceiveFrame->FrameAA.Start[1] == 0x55) 
              && (pReceiveFrame->FrameAA.Start[2] == 0xAA) && (pReceiveFrame->FrameAA.Start[3] == 0x55))
              {
                  FrameLen = pReceiveFrame->FrameAA.FrameLen[1] + 9;
                  if(Len < FrameLen)
                      return FRAME_LESS;
                  BYTE crc[2],*ptr;
                  ptr = (BYTE*)pReceiveFrame;
                  crc[0] = ptr[FrameLen - 2];
                  crc[1] = ptr[FrameLen - 1];
                  ByAddCrc16(ptr,FrameLen);
                  
                  if((crc[0] != ptr[FrameLen - 2]) || (crc[1] != ptr[FrameLen - 1]))
                  {
                      return FRAME_ERR|1;
                  }
                  return FRAME_OK|FrameLen;
              }

        case 0x69:
            if (pReceiveFrame->Frame69.Length1 != pReceiveFrame->Frame69.Length2)
                return FRAME_ERR|1;
            if (pReceiveFrame->Frame69.Start2 != 0x69)
                return FRAME_ERR|1;
            FrameLen=pReceiveFrame->Frame69.Length1+6;
            if (FrameLen > Len)
                return FRAME_LESS;
            if (Buf[FrameLen-1] != 0x16)
                return FRAME_ERR|1;
            if (Buf[FrameLen-2] != (BYTE)ChkSum((BYTE *)&pReceiveFrame->Frame69.Control,pReceiveFrame->Frame69.Length1))
                return FRAME_ERR|1;
            return FRAME_OK|FrameLen;
  
        case 0x10:
            if (4+m_guiyuepara.linkaddrlen > Len)
                return FRAME_LESS;
            if(Buf[3+m_guiyuepara.linkaddrlen]!=0x16)
                return FRAME_ERR|1;
            FrameLen=4+m_guiyuepara.linkaddrlen;
            if((Buf[1]&0x4f)!=0x4c)
                if (Buf[2+m_guiyuepara.linkaddrlen] != (BYTE)ChkSum((BYTE *)&pReceiveFrame->Frame10.Control, m_guiyuepara.linkaddrlen+1))
                    return FRAME_ERR|1;
            if (m_guiyuepara.linkaddrlen==1)
                wLinkAddress = pReceiveFrame->Frame10.Data[0];
            else
                wLinkAddress = MAKEWORD(pReceiveFrame->Frame10.Data[0],pReceiveFrame->Frame10.Data[0+1]);
            //if (SwitchToAddress(wLinkAddress) != TRUE)
                //return FRAME_ERR|FrameLen;
            m_dwasdu.LinkAddr=wLinkAddress;
            return FRAME_OK|FrameLen;

        case 0x68:
            if((pReceiveFrame->Frame68.Control == 0x05)&&(pReceiveFrame->Frame68.Length1 == 0x30))
            {
                if (pReceiveFrame->Frame68.Length1 != pReceiveFrame->Frame68.Length2)
                    return FRAME_ERR|1;
                if (pReceiveFrame->Frame68.Start2 != 0x68)
                    return FRAME_ERR|1;
                FrameLen=pReceiveFrame->Frame68.Length1+6;
                if (Buf[FrameLen] != 0x16)
                    return FRAME_ERR|1;                
            }
            else
            {
                if (pReceiveFrame->Frame68.Length1 != pReceiveFrame->Frame68.Length2)
                    return FRAME_ERR|1;
                if (pReceiveFrame->Frame68.Start2 != 0x68)
                    return FRAME_ERR|1;
                FrameLen=pReceiveFrame->Frame68.Length1+6;
                if (FrameLen > Len)
                {
                    //MyPrintf("FRAME_LESS\r\n");
                    return FRAME_LESS;
                }
                if (Buf[FrameLen-1] != 0x16)
                    return FRAME_ERR|1;
                if (Buf[FrameLen-2] != (BYTE)ChkSum((BYTE *)&pReceiveFrame->Frame68.Control,pReceiveFrame->Frame68.Length1))
                {
                    //MyPrintf("crc err\r\n");
                    return FRAME_ERR|1;
                }
                if (m_guiyuepara.linkaddrlen==1)
                    wLinkAddress = pReceiveFrame->Frame68.Data[m_byLinkAdrShift];
                else
                    wLinkAddress = MAKEWORD(pReceiveFrame->Frame68.Data[m_byLinkAdrShift],pReceiveFrame->Frame68.Data[m_byLinkAdrShift+1]);
                if (SwitchToAddress(wLinkAddress) != TRUE)
                {
                    //MyPrintf("linkAddr err\r\n");
                    return FRAME_ERR|FrameLen;
                }
                m_dwasdu.LinkAddr=wLinkAddress;
            }

//          #ifdef BJ101_GXTEST /*测试19-增加公共地址判断*/
//          if (m_guiyuepara.conaddrlen==1)
//              wLinkAddress = pReceiveFrame->Frame68.Data[m_byCommAdrShift];
//          else
//              wLinkAddress = MAKEWORD(pReceiveFrame->Frame68.Data[m_byCommAdrShift],pReceiveFrame->Frame68.Data[m_byCommAdrShift+1]);
//          if (SwitchToAddress(wLinkAddress) != TRUE)
//              return FRAME_ERR|FrameLen;
//          m_dwasdu.Address=wLinkAddress;
//          #endif
           return FRAME_OK|FrameLen;
      case 0xEB:
            if (pReceiveFrame->FrameEB.Start2 != 0xEB)
                return FRAME_ERR|1;
            FrameLen=MAKEWORD(pReceiveFrame->FrameEB.LengthL,pReceiveFrame->FrameEB.LengthH) + 6;
            if (FrameLen > Len)
            {
                return FRAME_LESS;
            }
            if (Buf[FrameLen-1] != 0xD7)
                return FRAME_ERR|1;
            if (Buf[FrameLen-2] != (BYTE)ChkSum((BYTE *)&pReceiveFrame->FrameEB.FrameType[0],FrameLen - 6))
            {
                return FRAME_ERR|1;
            }            
            return FRAME_OK|FrameLen;
        default:
            return FRAME_ERR|1;
    }
}


//ACK的处理
void CBJ101S::DoRecAck(void)
{
    if (m_byRTUStatus == RTU_RECCALL)
    {
        SendCallAll();
    }
    return;
}
//复位远方链路 平衡式
BOOL CBJ101S::RecACK(void)
{
    return true;
}



//解析请求远方链路状态
BOOL CBJ101S::RecReqLink(void)
{

#ifdef YN_101S
    SendBaseFrame(0,0x0b);  //yunnan
#else    
    SendBaseFrame(0,0x0b);  //张弢重庆&标准版
#endif    
    return TRUE;

}



//召唤一级用户数据
BOOL CBJ101S::RecCallClass1(void)
{
    if(m_linkflag==0)
    {
      return  TRUE;
    }
    /*测试18-总召唤可插入COS和SOE*/
      if(m_callallflag)
      {
        if(SendCallAll())
              return TRUE;
      }
      m_callallflag=0;
    if(m_initflag)
    {
      Initlink();
      m_initflag=0;
      return TRUE;
    }
    
    if(m_YKflag==1){
        m_YKflag=0;
        if( SendYKSetAck()) return TRUE;
      }
      if(m_YKstop){
        m_YKstop = 0;
        if(SendYKstop()) return TRUE;
      }
    //发送cos
    if(SearchCos(m_SYxBit,0))
    {
     m_acdflag=1;
     if(SendCos())
         return TRUE;
         m_acdflag=0;
    }
    if(SearchCos(m_DYxBit,0))
    {
     m_acdflag=1;
     if(SendDCos())
         return TRUE;
         m_acdflag=0;
    }
    //发送SOE
    if(SearchCos(m_SSoeBit,0))
    {
     m_acdflag=1;
     if(SendSoe())
         return TRUE;
         m_acdflag=0;
    }
  /*  if(SearchCos(m_DSoeBit,0))
    {
     m_acdflag=1;
     if(SendDSoe())
         return TRUE;
         m_acdflag=0;
    }*/
    if(m_errflag!=0)
      return  SenderrtypeAck();
    SendNoData();
    return TRUE;
}

//远方链路状态完好或召唤二级用户数据
BOOL CBJ101S::RecCallClass2(void)
{
        if(m_linkflag==0)
        {
            return true;
        }
//        if(m_timeflag)
//        {
//            m_timeflag=0;
//            if(SendtimeAck())
//                return TRUE;
//        }
        if(m_groupflag)
        {
            if(SendCallgroup())
                return TRUE;
        }
        if(m_testflag)
        {
            m_testflag=0;
            if(SendTsetLinkAck())
                return TRUE;
        }
      /* if(g_SendYc && (g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_SEND_CHANGEYC]))
       {
        if(SendChangeYC())
            return TRUE;
       }*/
       
        if(m_errflag!=0)
           return  SenderrtypeAck();
        if(m_resetflag)
          {
             m_resetflag=0;
             if(SendresetAck())
                return TRUE;
          }
//        if(SearchCos(BIT1,0))
//        {
//          if(SendSoe())
//            return TRUE;
//        }
//        if(SearchCos(BIT3,0))
//        {
//          if(SendDSoe())
//            return TRUE;
//        }
//        if(SearchCos(BIT0,0))
//        {
//          if(SendCos())
//             return TRUE;
//        }
//        if(SearchCos(BIT2,0))
//        {
//          if(SendDCos())
//            return TRUE;
//        }
//      if (SearchClass1())
//            m_acdflag=1;
      SendNoData();
      return TRUE; //远方链路状态完好或召唤二级用户数据
}

//修改密码处理
BOOL CBJ101S::RecYSPassWord(void)
{
	DWORD PassWord;
	WORD tempassWord1,tempassWord2;
	
	BYTE *pData = &pReceiveFrame->Frame68.Data[m_byReasonShift];//从信息体地址后边开始取数，这里可不关心信息体地址
        return TRUE;
}
//修改密文处理
BOOL CBJ101S::RecYSCiPHer(void)
{
	DWORD PassWord;
	WORD tempassWord1,tempassWord2;
	
	BYTE *pData = &pReceiveFrame->Frame68.Data[m_byReasonShift];//从信息体地址后边开始取数，这里可不关心信息体地址
        return TRUE;
}
//遥控处理
BOOL CBJ101S::RecYKCommand(void)
{
        RecYKSet();
        return TRUE;
}

//遥控预置/执行命令
BOOL CBJ101S::RecYKSet(void)
{
BYTE * pData = &pReceiveFrame->Frame68.Data[m_byInfoShift];
WORD  DCO;  //遥控命令限定词
WORD  YKNo; //遥控路号

switch (m_guiyuepara.infoaddlen )
{
    case 1:
        YKNo = pData[0];
        DCO = pData[1];
        break;
    case 2:
        YKNo = MAKEWORD(pData[0], pData[1]);
        DCO = pData[2];
        break;
    case 3:
        YKNo = MAKEWORD(pData[0], pData[1]);//高字节强制为0
        DCO = pData[3];
        break;
    default:
        YKNo = MAKEWORD(pData[0], pData[1]);
        DCO = pData[2];
        break;
}

VYKInfo *pYKInfo;
pYKInfo = &(pGetEqpInfo(m_wEqpNo)->YKInfo);


switch (DCO & 0x80)
{
    case 0x80:
        pYKInfo->Info.byStatus = 0x0;
        pYKInfo->Head.byMsgID = MI_YKSELECT;
        break;
    case 0x00:
        pYKInfo->Info.byStatus = 0x0;
        pYKInfo->Head.byMsgID = MI_YKOPRATE;
        break;
    default:
        break;
}
if(YKNo<ADDR_YK_LO || YKNo>ADDR_YK_HI)
  pYKInfo->Info.byStatus = 1;
else if (m_guiyuepara.infoaddlen != 1)
    YKNo = YKNo - ADDR_YK_LO + 1;
pYKInfo->Info.wID = YKNo;
if((m_dwasdu.COT&0xf)==8)
    pYKInfo->Head.byMsgID = MI_YKCANCEL;

if(m_dwasdu.TypeID!=C_SC_NA)
switch (DCO & 0x03)
{
    case 0:
    case 3:
        return FALSE;
    case 1:
        pYKInfo->Info.byValue = 0;//0x06;
        break; //分
    case 2:
        pYKInfo->Info.byValue = 1;//0x05;
        break; //合
    default:
        break;
}

    if(m_dwasdu.TypeID==C_SC_NA) {
        if(DCO&1)
            pYKInfo->Info.byValue = 1;//0x05;   //合
        else
            pYKInfo->Info.byValue = 0;//0x06;  //分
    }

pYKInfo->Head.abyRsv[0] = pReceiveFrame->Frame68.Data[m_byTypeIDShift];
if (m_guiyuepara.infoaddlen == 3)//也只取信息体的最低两个字节
{
    pYKInfo->Head.abyRsv[1] = pData[0];
    pYKInfo->Head.abyRsv[2] = pData[1];
    pYKInfo->Head.abyRsv[3] = pData[3];
}
else
{
    pYKInfo->Head.abyRsv[1] = pData[0];
    pYKInfo->Head.abyRsv[2] = pData[1];
    pYKInfo->Head.abyRsv[3] = pData[2];
}

  if(pYKInfo->Head.byMsgID == MI_YKOPRATE)
  {
      ExecuteYKOrder(pYKInfo->Info.wID,pYKInfo->Info.byValue);
      m_YkWaitCount = 20;
      m_YKflag = 2;
	  SendYKstop();
  }
  else
    m_YKflag=1;

return TRUE;
/*
if(YKNo == 0x2001)
{
switch (DCO & 0x03)
{
    case 1:
           m_YKflag=1;
           break;
    case 3:       
    case 0:      
    case 2:
        return FALSE;
    default:
        break;
}
}

pYKInfo->Head.abyRsv[0] = pReceiveFrame->Frame68.Data[m_byTypeIDShift];
if (m_guiyuepara.infoaddlen == 3)//也只取信息体的最低两个字节
{
    pYKInfo->Head.abyRsv[1] = pData[0];
    pYKInfo->Head.abyRsv[2] = pData[1];
    pYKInfo->Head.abyRsv[3] = pData[3];
}
else
{
    pYKInfo->Head.abyRsv[1] = pData[0];
    pYKInfo->Head.abyRsv[2] = pData[1];
    pYKInfo->Head.abyRsv[3] = pData[2];
}

  if(pYKInfo->Head.byMsgID == MI_YKOPRATE)
  {
      ExecuteYKOrder(pYKInfo->Info.wID,pYKInfo->Info.byValue);
      m_YkWaitCount = 20;
      m_YKflag = 2;
  }
  else
    m_YKflag=1;
*/
//return TRUE;
}

//执行遥控命令
BOOL CBJ101S::ExecuteYKOrder(unsigned int YkNo,unsigned int Val)
{
  unsigned char chYkPa; 
  //chYkPa = g_ucYKPa[YkNo-1];
  chYkPa = YkNo;
  
 
    if(chYkPa == 1)
    {
      if(Val) 
      	{
      	if((g_sRecData.m_ucActRecStart == CLOSE)&&(g_sRecData.m_ucRecSavingFlag == OFF)&&(g_sRecData.m_ucFaultRecStart == OFF))
      		{
      		g_sRecData.m_ucRecSavingFlag = YES; //置标志要求保存到FLASH中
      		g_sRecData.m_ucFaultRecStart = CLOSE;//录波结束，故障恢复后，恢复OFF
            g_sRecData.m_unRecAcLockCnt = 1000;
            g_sRecData.m_gFaultRecOver[REC_MSL] =  g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
            g_sRecData.m_gFaultRecOver[REC_MSH] =g_sRtcManager.m_gRealTimer[RTC_SEC];
            g_sRecData.m_gFaultRecOver[REC_MINU] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
            g_sRecData.m_gFaultRecOver[REC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
            g_sRecData.m_gFaultRecOver[REC_DAY] = g_sRtcManager.m_gRealTimer[RTC_DATE];
            g_sRecData.m_gFaultRecOver[REC_MONTH] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
            g_sRecData.m_gFaultRecOver[REC_YEAR] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000);				
      		}
      	}
        //DO00_ON;
      //else 
        //DO00_OFF;
    }
    else if(chYkPa ==2)
    {
      if(Val) 
      	{
      	eight_select=0x8F; //eight_select|=BIT7;                    ////////////置首次发生8脉冲的标志		
        Sign_Repeat(1,0);//模拟A相故障
      	}    
    }
    else if(chYkPa ==3)
    {
      if(Val) 
      	{
      	eight_select=0x8F; //eight_select|=BIT7;                    ////////////置首次发生8脉冲的标志		
        Sign_Repeat(2,0);//模拟b相故障
      	}    
    }	
    else if(chYkPa ==4)
    {
      if(Val) 
      	{
      	eight_select=0x8F; //eight_select|=BIT7;                    ////////////置首次发生8脉冲的标志		
        Sign_Repeat(3,0);//模拟c相故障
      	}    
    }
    g_YkOrderFlag = TRUE;
  
      
  return TRUE;
  
  
}

//发送遥控预置/执行

//发送遥控预置/执行
BOOL CBJ101S::SendYKSetAck(void)
{
    BYTE Style, Reason = 7;
    BYTE PRM = 0, dwCode = 3, Num = 1; /* #22 遥控预置/撤销/停止 确认报文的链路功能码应该为3,不是0 */
    VYKInfo *pYKInfo;

    pYKInfo = &(pGetEqpInfo(m_wEqpNo)->YKInfo);
    Style = pYKInfo->Head.abyRsv[0];
    if (pYKInfo->Head.byMsgID == MI_YKCANCEL)
    {
        Reason = COT_DEACTCON;
    }
    if (pYKInfo->Info.byStatus != 0)
    {
        /* R226: 非法遥控明确传送原因 */
        if(pYKInfo->Info.byStatus == 1)
            Reason = COT_PN_BIT|47;             /*未知的信息对象地址*/
        else
            Reason = COT_PN_BIT|COT_ACTTERM;    /*激活终止*/
    }
    if(pYKInfo->Head.byMsgID == MI_YKOPRATE && g_YkOrderFlag != TRUE)
       Reason |= COT_PN_BIT; //遥控执行失败
    SendFrameHead(Style, Reason);
    switch (m_guiyuepara.infoaddlen )
    {
        case 1:
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[1];//信息体地址
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[2];//DCO
            break;
        case 2:
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[1];//信息体地址
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[2];
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[3];//DCO
            break;
        case 3:
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[1];//信息体地址
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[2];
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[3];//DCO
            break;
        default:
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[1];//信息体地址
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[2];
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[3];//DCO
            break;
    }
    if (pYKInfo->Head.byMsgID == MI_YKOPRATE && m_guiyuepara.mode==0 && g_YkOrderFlag == TRUE)
    {
       m_YKstop = 1;
    }
    if (pYKInfo->Head.byMsgID == MI_YKSELECT) m_YKSelectAck = 1;
    SendFrameTail(PRM, dwCode, Num,2);
    m_YKSelectAck = 0;
    /*R227: 平衡模式执行确认紧接遥控结束*/
    if (pYKInfo->Head.byMsgID == MI_YKOPRATE && m_guiyuepara.mode==1 && g_YkOrderFlag == TRUE)
    {
       SendYKstop();
    }
    g_YkOrderFlag = OFF;//清遥控标识
    return TRUE;
}

#ifdef YN_101S
void CBJ101S::RecReadData()
{
  BYTE * pData = &pReceiveFrame->Frame68.Data[m_byInfoShift];
  //WORD wInfoAddr = MAKEWORD(pData[0], pData[1]);
  WORD wInfoAddr;
  WORD bVSQ = m_dwasdu.VSQ;
  //BYTE bLinePhase;
  WORD cmd;
  BYTE temp0,temp1,i;//,k; 
 // BYTE bLine;
 // BYTE bPhase;
 SendFrameHead(102,7);
  for(i = 0; i < bVSQ; i++ )
  {
  temp0=*pData;pData++;
  temp1=*pData;pData++;
  wInfoAddr = MAKEWORD(temp0, temp1);
  switch (wInfoAddr)
  {
  case 0x6001://读IP地址及端口
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 	 
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_gIPPort[1];
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_gIPPort[2];
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_gIPPort[3];
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_gIPPort[4];
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_gIPPort[6];
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_gIPPort[5];
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_gIPPort[7];
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_gIPPort[8];
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_gIPPort[9];
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_gIPPort[10];
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_gIPPort[12];
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_gIPPort[11];
      g_gGprsCXip = 0x55;
      if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(0,11,this);
	  delayms(3000);
      if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(5,11,this);
  
      break;
  case 0x6003://读链路地址
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_COMM_ADDR];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      break;
  case 0x6009://读信号源心跳周期
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_BEAT_T];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      break;
  case 0x6011://读遥测信息上传周期
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_SENDYC_T];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      break;  
  case 0x6014://读自检信息上传周期
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_SENDZJ_T];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      break;  
  case 0x6020://读零序电压过压
      //SendFrameHead(102,7);
      //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_HIGH_Z];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      //SendFrameTail(0, 0xa5, 1);
      break;
  case 0x6021://读线电压过压
      //SendFrameHead(102,7);
      //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_I0_START];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      //SendFrameTail(0, 0xa5, 1);
      break;
  case 0x6022://读相电压过压
      //SendFrameHead(102,7);
      //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_HIGH_P];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      //SendFrameTail(0, 0xa5, 1);
      break;
   case 0x6023://读相电压欠压
      //SendFrameHead(102,7);
       //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_LOW_P];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      //SendFrameTail(0, 0xa5, 1);
      break;
   case 0x6024://读换相电流比较值
      //SendFrameHead(102,7);
      //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(g_gRunPara[RP_PULSE_VALID]);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(g_gRunPara[RP_PULSE_VALID]);
      //SendFrameTail(0, 0xa5, 1);
      break;
    case 0x6025://读投切延时时间
      //SendFrameHead(102,7);
      //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(g_gRunPara[RP_T_DELAY]);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(g_gRunPara[RP_T_DELAY]);
      //SendFrameTail(0, 0xa5, 1);
      break;
    case 0x6026://读接地功能投退
      //SendFrameHead(102,7);
      //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(g_gRunPara[RP_CNL_MODEL]);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(g_gRunPara[RP_CNL_MODEL]);
      //SendFrameTail(0, 0xa5, 1);
      break;
    case 0x6027:	   //电压告警延时时间	
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(g_gRunPara[RP_REV_CURRENT]);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(g_gRunPara[RP_REV_CURRENT]);    
      break;
    case 0x6028:	   //是否主动上传录波目录	
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	
      if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_SENDLUBOML])	  
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 1;
      else 
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;	  	
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;    
      break;	  
  }
  }
  SendFrameTail(0, 0xa5, bVSQ,0);
  return;
}
#endif

//解析时钟同步报文
BOOL CBJ101S::RecSetClock(void)
{
    BYTE * pData = &pReceiveFrame->Frame68.Data[m_guiyuepara.infoaddlen];
    WORD MSecond;
    pData=&pReceiveFrame->Frame68.Data[m_dwasdu.Infooff+m_guiyuepara.infoaddlen];

    MSecond = MAKEWORD(pData[0], pData[1]);
    unsigned int unHYM[7];
    unHYM[0] = (pData[6] & 0x7F);//YEAR
    unHYM[1] = pData[5] & 0x0F;//MONTH
    unHYM[2] = pData[4] & 0x1F;//DAY
    unHYM[3] = pData[4] >> 5;//WEEK
    unHYM[4] = pData[3];  //HOUR
    unHYM[5] = pData[2];//MINUTE
  
        
    g_gWeekNum = unHYM[3];
    if((60000-MSecond) > time_delay_set)//考虑加入延时传递的时间后是否大于60000ms,如果大于则需要分钟加1
      MSecond+=time_delay_set;
    else
    {
      MSecond=MSecond+time_delay_set-60000;
      unHYM[5]+=1;
    }
      
    unHYM[6] = MSecond/1000;//SEC
    WriteRealTime(unHYM);  //修改时间
  //SendtimeAck();应先发短帧确认再发长针
   
    m_timeflag=1;
    return true;
}
//解析读时钟报文
BOOL CBJ101S::RecReadClock(void)
{
    //SendBaseFrame(0,0);//张弢重庆 总召要个确认报文
    m_timeREQflag =1;
    return true;
}
BOOL CBJ101S::RecDelaytime(void)
{
    BYTE * pData = &pReceiveFrame->Frame68.Data[m_guiyuepara.infoaddlen];
    WORD MSecond;
    pData=&pReceiveFrame->Frame68.Data[m_dwasdu.Infooff+m_guiyuepara.infoaddlen];

    MSecond = MAKEWORD(pData[0], pData[1]);
    unsigned int unHYM[7];


    ReadRealTime();
      
    unHYM[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR]-2000;
    unHYM[1] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
    unHYM[2] = g_sRtcManager.m_gRealTimer[RTC_DATE];
   // unHYM[3] = g_sRtcManager.m_gRealTimer[RTC_WEEK];
    unHYM[4] = g_sRtcManager.m_gRealTimer[RTC_HOUR];  
    unHYM[5] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
    unHYM[6] = MSecond/1000;
      //unHYM[7] = MSecond%1000;
    
     WriteRealTime(unHYM);  //修改时间
  //SendtimeAck();应先发短帧确认再发长针
     m_delayflag=1;
     return true;
}
BOOL CBJ101S::RecDelaytrans(void)
{
   BYTE * pData = &pReceiveFrame->Frame68.Data[m_guiyuepara.infoaddlen];
   pData=&pReceiveFrame->Frame68.Data[m_dwasdu.Infooff+m_guiyuepara.infoaddlen];
   time_delay_set = MAKEWORD(pData[0], pData[1]);
   return true;
}


//解析测试链路
BOOL CBJ101S::RecTestLink(void)
{
    m_testflag=1;
    //return SendTsetLinkAck();
    return TRUE;
}

//解析复位RTU
BOOL CBJ101S::RecResetRTU(void)
{
    //BYTE RecSoeNum=10;
    BYTE QRP = pReceiveFrame->Frame68.Data[m_dwasdu.Infooff+m_guiyuepara.infoaddlen];
    m_QRP=QRP;
    m_resetflag=1;
    if(QRP==2)
    {

    }
    return TRUE;
}


BOOL CBJ101S::RecYTCommand(void)
{
//    BYTE * pData = &pReceiveFrame->Frame68.Data[m_dwasdu.Infooff+m_guiyuepara.infoaddlen];
////  WORD  QOS;  //遥控命令限定词
//    WORD  YTNo;
//    //WORD YTValue; //遥控路号
//    //VDBYT *pDBYT;
//
//    YTNo=m_dwasdu.Info;
//                //YTValue = MAKEWORD(pData[0], pData[1]);
////              QOS = pData[2];
//    if (m_guiyuepara.infoaddlen!= 1)
//        YTNo = YTNo - ADDR_YT_LO + 1;
//    //pDBYT = (VDBYT *)m_pMsg->abyData;
//        //pDBYT->wID = YTNo;
//        //pDBYT->wValue = YTValue;
//        //TaskSendMsg(DBID, m_wTaskID, m_wEqpID, MI_YTOPRATE, MA_REQ, 5/*sizeof(VDBYT)*/, m_pMsg);
    return TRUE;
}


//链路地址域（子站站址）
DWORD CBJ101S::GetAddress(void)
{
    return GetOwnAddr();
}

void CBJ101S::getasdu(void)
{   BYTE off=0;
    if(m_guiyuepara.linkaddrlen==1)
    {
        m_dwasdu.LinkAddr=pReceiveFrame->Frame68.Data[off++];
    }
    if(m_guiyuepara.linkaddrlen==2)
    {
        m_dwasdu.LinkAddr=MAKEWORD(pReceiveFrame->Frame68.Data[off],pReceiveFrame->Frame68.Data[off+1]);
            off+=2;
    }
    if(m_guiyuepara.typeidlen==1)
    {
        m_dwasdu.TypeID=pReceiveFrame->Frame68.Data[off++];
    }
    if(m_guiyuepara.typeidlen==2)
    {
        m_dwasdu.TypeID=MAKEWORD(pReceiveFrame->Frame68.Data[off],pReceiveFrame->Frame68.Data[off+1]);
            off+=2;
    }
    if(m_guiyuepara.VSQlen==1)
    {
        m_dwasdu.VSQ=pReceiveFrame->Frame68.Data[off++];
    }
    if(m_guiyuepara.VSQlen==2)
    {
        m_dwasdu.VSQ=MAKEWORD(pReceiveFrame->Frame68.Data[off],pReceiveFrame->Frame68.Data[off+1]);
            off+=2;
    }
    if(m_guiyuepara.COTlen==1)
    {
        m_dwasdu.COT=pReceiveFrame->Frame68.Data[off++];
    }
    if(m_guiyuepara.COTlen==2)
    {
        m_dwasdu.COT=pReceiveFrame->Frame68.Data[off++];
        m_sourfaaddr=pReceiveFrame->Frame68.Data[off++];
    }
    if(m_guiyuepara.conaddrlen==1)
    {
        m_dwasdu.Address=pReceiveFrame->Frame68.Data[off++];
    }
    if(m_guiyuepara.conaddrlen==2)
    {
        m_dwasdu.Address=MAKEWORD(pReceiveFrame->Frame68.Data[off],pReceiveFrame->Frame68.Data[off+1]);
        off+=2;
    }
    m_dwasdu.Infooff=off;
    if(m_guiyuepara.infoaddlen==1)
    {
        m_dwasdu.Info=pReceiveFrame->Frame68.Data[off++];
    }
    if(m_guiyuepara.infoaddlen==2)
    {
        m_dwasdu.Info=MAKEWORD(pReceiveFrame->Frame68.Data[off],pReceiveFrame->Frame68.Data[off+1]);
        off+=2;
    }
    if(m_guiyuepara.infoaddlen==3)
    {
        m_dwasdu.Info=MAKEWORD(pReceiveFrame->Frame68.Data[off+1], pReceiveFrame->Frame68.Data[off+2]);
        m_dwasdu.Info<<=8;
        m_dwasdu.Info|=pReceiveFrame->Frame68.Data[off];
        off+=3;
    }
}
void CBJ101S::write_linkaddr(int  data)
{
    if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ENCRYPT])
    {
        m_SendBuf.wWritePtr=7;
    }
    else
    {
    m_SendBuf.wWritePtr=5;
    }
    for(BYTE i=0;i<m_guiyuepara.linkaddrlen;i++)
    {
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(data>>(i*8))&0xff;
    }

}
void CBJ101S::write_10linkaddr(int  data)
{
    m_SendBuf.wWritePtr=2;
    for(BYTE i=0;i<m_guiyuepara.linkaddrlen;i++)
    {
       m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(data>>(i*8))&0xff;
    }

}

void CBJ101S::write_typeid(int  data)
{
    if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ENCRYPT])
    {
        m_SendBuf.wWritePtr=7+m_guiyuepara.linkaddrlen;
    }
    else
    {
    m_SendBuf.wWritePtr=5+m_guiyuepara.linkaddrlen;
    }
    for(BYTE i=0;i<m_guiyuepara.typeidlen;i++)
    {
       m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(data>>(i*8))&0xff;
    }

}
void CBJ101S::write_VSQ(int  data)
{
   for(BYTE i=0;i<m_guiyuepara.VSQlen;i++)
   {
     m_SendBuf.pBuf[ i+5+m_guiyuepara.linkaddrlen+m_guiyuepara.typeidlen ]=(data>>(i*8))&0xff;
   }

}
void CBJ101S::write_COT(int  data)
{
    if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ENCRYPT])
    {
        m_SendBuf.wWritePtr=7+m_guiyuepara.linkaddrlen+m_guiyuepara.typeidlen+m_guiyuepara.VSQlen;
    }
    else
    {
    m_SendBuf.wWritePtr=5+m_guiyuepara.linkaddrlen+m_guiyuepara.typeidlen+m_guiyuepara.VSQlen;
    }
    {
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr+0 ]=(data)&0xff;
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr+1] = 0;//m_sourfaaddr;
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr+2]=0;
        m_SendBuf.wWritePtr+=m_guiyuepara.COTlen;
    }

}
void CBJ101S::write_conaddr(int  data)
{
    if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ENCRYPT])
    {
        m_SendBuf.wWritePtr=7+m_guiyuepara.linkaddrlen+m_guiyuepara.typeidlen+m_guiyuepara.VSQlen+m_guiyuepara.COTlen;
    }
    else
    {
    m_SendBuf.wWritePtr=5+m_guiyuepara.linkaddrlen+m_guiyuepara.typeidlen+m_guiyuepara.VSQlen+m_guiyuepara.COTlen;
    }
    for(BYTE i=0;i<m_guiyuepara.conaddrlen;i++)
    {
       m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(data>>(i*8))&0xff;
    }

}
void CBJ101S::write_infoadd(int  data)
{
    for(BYTE i=0;i<m_guiyuepara.infoaddlen;i++)
    {
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(data>>(i*8))&0xff;
    }

}
 BYTE CBJ101S::QDS(BYTE data)
    {
    BYTE tt=0;
    if((data&1)==0)
        tt|=0x80;
    if((data&2))
        tt|=0x10;
    if((data&4))
        tt|=0x20;
    if((data&8))
        tt|=0x40;
    if((data&0x10))
        tt|=0x1;
        return tt;
 }
  BYTE CBJ101S::SIQ(BYTE data)
    {
    BYTE tt=0;
      if((data&1)==0)
          tt|=0x80;
      if((data&2))
          tt|=0x10;
      if((data&4))
          tt|=0x20;
      if((data&8))
          tt|=0x40;
      if((data&0x80))
          tt|=0x1;
        return tt;
 }
 BYTE CBJ101S::DIQ(BYTE data1,BYTE data2)
    {
    BYTE tt=0;
      if((data1&1)==0)
          tt|=0x80;
      if((data1&2))
          tt|=0x10;
      if((data1&4))
          tt|=0x20;
      if((data1&8))
          tt|=0x40;
      if((data1&0x80))
          tt|=0x1;
      if((data2&0x80))
          tt|=0x2;
          if((data1&0x80)==(data2&0x80))
          tt|=0x80;
    return tt;
 }

//7字节的长时标
void CBJ101S::write_time()
{
      ReadRealTime();
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(g_sRtcManager.m_gRealTimer[RTC_MICROSEC]+g_sRtcManager.m_gRealTimer[RTC_SEC]*1000)&0xff;
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(g_sRtcManager.m_gRealTimer[RTC_MICROSEC]+g_sRtcManager.m_gRealTimer[RTC_SEC]*1000)>>8;
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=g_sRtcManager.m_gRealTimer[RTC_MINUT];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=g_sRtcManager.m_gRealTimer[RTC_HOUR];

      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(g_sRtcManager.m_gRealTimer[RTC_DATE]|(g_gWeekNum<<5));//
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=g_sRtcManager.m_gRealTimer[RTC_MONTH];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=g_sRtcManager.m_gRealTimer[RTC_YEAR]-m_guiyuepara.baseyear;//year

}
//短时标
void CBJ101S::write_time3()
{
    ReadRealTime();
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(g_sRtcManager.m_gRealTimer[RTC_MICROSEC]+g_sRtcManager.m_gRealTimer[RTC_SEC]*1000)&0xff;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(g_sRtcManager.m_gRealTimer[RTC_MICROSEC]+g_sRtcManager.m_gRealTimer[RTC_SEC]*1000)>>8;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=g_sRtcManager.m_gRealTimer[RTC_MINUT];
}
//重发报文
BOOL CBJ101S::SendRetry(void)
{
  // int i = 0;
   
   if(m_WaitConfTpId == M_SP_NA || m_WaitConfTpId == M_DP_NA)//COS重传
   {
     if(m_ucCosBkNum)
     {
     //    for(i = 0;i < m_ucCosBkNum;i++) 
       //   SETBIT(m_gRmtChgFlag,m_ucCosBk[i],1);
         if(m_WaitConfTpId == M_SP_NA)
         {
          if(SendCos())
            return 1;
         }
         else if(m_WaitConfTpId == M_DP_NA)
         {//双点COS重传
          if(SendDCos())
            return 1;
         } 
     }
   }
   else if(m_WaitConfTpId == M_SP_TB)//单点SOE重传
   {
      if(m_SSoeHeadPtr != m_SSoeHeadPtrBk)
      {
        m_SSoeHeadPtr = m_SSoeHeadPtrBk;
        if(SendSoe())
          return 1;
      }
    }
   else if(m_WaitConfTpId == M_DP_TB)//双点SOE重传
   {
      if(m_DSoeHeadPtr != m_DSoeHeadPtrBk)
      {
        m_DSoeHeadPtr = m_DSoeHeadPtrBk;
        if(SendDSoe())
          return 1;
       }
   }
  return 0;  
}

#ifdef YN_101S
BOOL CBJ101S::RecYSCommand(void)
{
  BYTE i,k;
  //BYTE bHisDaTm[2][7]={0};
  BYTE bTypeID = m_dwasdu.TypeID;
  WORD wVal = 0;
  WORD wTemp = 0;
  WORD bVSQ = m_dwasdu.VSQ;
  WORD tempwInfoAddr;
  BYTE temp0,temp1,cip,temport;
 // memcpy(m_YsBufBk,&pReceiveFrame->Frame68.Start1,pReceiveFrame->Frame68.Length1+6);
  WORD wInfoAddr = MAKEWORD(pReceiveFrame->Frame68.Data[m_byInfoShift],pReceiveFrame->Frame68.Data[m_byInfoShift+1]);
  //BYTE *pData = &pReceiveFrame->Frame68.Data[m_byInfoShift + m_guiyuepara.infoaddlen];//从信息体地址后边开始取数，这里可不关心信息体地址
  BYTE *pData = &pReceiveFrame->Frame68.Data[m_byInfoShift];//从信息体地址后边开始取数，这里可不关心信息体地址
  cip = 0;
  if(bVSQ >= 1 && bTypeID == 53)   //设置多个参数
  {       
       for(i = 0; i < bVSQ; i++ )
       {
       	  temp0=*pData;pData++;
	  temp1=*pData;pData++;
          tempwInfoAddr= MAKEWORD(temp0,temp1);  
	  switch (tempwInfoAddr)
	  {
	    case 0x6001: //修改IP地址及端口
          for(k = 1; k< 13; k++ )
          {
            g_gIPPort[k] = *pData;
	    pData++;		      
      	  }
          temport=g_gIPPort[5];g_gIPPort[5]=g_gIPPort[6];g_gIPPort[6]=temport;
	   temport=g_gIPPort[11];g_gIPPort[11]=g_gIPPort[12];g_gIPPort[12]=temport;	  
	  g_ucParaChang |= BIT7;	
	  g_gIPPort[0]=6;
	  cip =0x55;//if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(0,0,g_gIPPort,this);
	      break;
	    case 0x6003: //修改链路地址
	      temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);
              wTemp = wVal;
              if(wTemp != g_gRunPara[RP_COMM_ADDR])
              {
                g_gRunPara[RP_COMM_ADDR] = wTemp;
                g_ucParaChang |= BIT0;
                SetEqpInfo();
              }
	      break;
	    case 0x6009:   //修改信号源心跳周期
              temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);
              wTemp = wVal;
              if(wTemp != g_gRunPara[RP_BEAT_T])
              {
                g_gRunPara[RP_BEAT_T] = wTemp;
                g_ucParaChang |= BIT0;
                SetEqpInfo();
              }          
              break;
	    case 0x6011:  //遥测上传周期
              temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);
              wTemp = wVal;
              if(wTemp != g_gRunPara[RP_SENDYC_T])
              {
                g_gRunPara[RP_SENDYC_T] = wTemp;
                g_ucParaChang |= BIT0;
                SetEqpInfo();
               }
              break;
            case 0x6014:  //自检信息上传周期
              temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);
              wTemp = wVal;
          if(wTemp != g_gRunPara[RP_SENDZJ_T])
              {
            g_gRunPara[RP_SENDZJ_T] = wTemp;
                g_ucParaChang |= BIT0;
                SetEqpInfo();
              }
              break;
            case 0x6020://零序电压告警定值
              temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);
              wTemp = wVal;
              if(wTemp != g_gRunPara[RP_HIGH_Z])
              {
                g_gRunPara[RP_HIGH_Z] = wTemp;
                g_ucParaChang |= BIT0;
                SetEqpInfo();
              }
              break;
            case 0x6021://线电压过压告警定值
              temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);
              wTemp = wVal;
              if(wTemp != g_gRunPara[RP_I0_START])  //线电压过压
              {
                g_gRunPara[RP_I0_START] = wTemp;
                g_ucParaChang |= BIT0;
                SetEqpInfo();
              }          
              break;
           case 0x6022://线电压过压告警定值
              temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);
              wTemp = wVal;
              if(wTemp != g_gRunPara[RP_HIGH_P])
              {
                g_gRunPara[RP_HIGH_P] = wTemp;
                g_ucParaChang |= BIT0;
                SetEqpInfo();
              }          
              break;
            case 0x6023://相电压欠压告警定值
              temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);
              wTemp = wVal;
              if(wTemp != g_gRunPara[RP_LOW_P])
              {
                g_gRunPara[RP_LOW_P] = wTemp;
                g_ucParaChang |= BIT0;
                SetEqpInfo();
              }         
              break;
            case 0x6024://换相电流比较定值
              temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);                 
              if(wVal != g_gRunPara[RP_PULSE_VALID])  //
              {
                g_gRunPara[RP_PULSE_VALID] = wVal;
                g_ucParaChang |= BIT0;
                SetEqpInfo();
              }          
              break;
            case 0x6025://投切延时时间
              temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);
              if(wVal != g_gRunPara[RP_T_DELAY])
              {
                g_gRunPara[RP_T_DELAY] = wVal;
                g_ucParaChang |= BIT0;
                SetEqpInfo();
              }          
              break;
            case 0x6026://装置功能投/退（1/0）
              temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);
              if(wVal != g_gRunPara[RP_CNL_MODEL])
              {
                       g_gRunPara[RP_CNL_MODEL] = wVal;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
              }          		
              break;	
	    case 0x6027:	   //电压告警延时时间
              temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);
		  if(wVal != g_gRunPara[RP_REV_CURRENT])
              {
                       g_gRunPara[RP_REV_CURRENT] = wVal;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
              }     
	      break;	
	    case 0x6028:	   //主动上报录波文件(1/0)
              temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);
		  if(wVal==0)
		  	g_gRunPara[RP_CFG_KEY] &= (0xfffd);		  	
		  else
		  	g_gRunPara[RP_CFG_KEY] |= BIT[RPCFG_SENDLUBOML];
		  g_ucParaChang |= BIT0;
	      break;
	  }
       }
	   SendFrameHead(53,7);
    for(i = m_byInfoShift; i < (pReceiveFrame->Frame68.Length1-1); i++ )
    {
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =pReceiveFrame->Frame68.Data[i];
    }
    SendFrameTail(0, 0x85, bVSQ,0);
    delayms(5000);
    if(cip == 0x55)
    {
      if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(0,0,g_gIPPort,this);
      if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(6,5,&g_gIPPort[6],this);
    }	  
  }
  cip = 0;
  if(wInfoAddr == 0x6001 && bTypeID == 0x34)   ////修改IP地址及端口
  {
    SendFrameHead(0x34,7);
              write_infoadd(wInfoAddr);
    k=1;
    for(i = (m_byInfoShift+ m_guiyuepara.infoaddlen); i < (pReceiveFrame->Frame68.Length1-1); i++ )
    {
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =pReceiveFrame->Frame68.Data[i];
      g_gIPPort[k] = pReceiveFrame->Frame68.Data[i];
      k++;
    }
    temport=g_gIPPort[5];g_gIPPort[5]=g_gIPPort[6];g_gIPPort[6]=temport;
    temport=g_gIPPort[11];g_gIPPort[11]=g_gIPPort[12];g_gIPPort[12]=temport;
    g_gIPPort[0] = 6;    
    //m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wVal);
    //m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wVal);
              SendFrameTail(0, 0x85, 1,0);
              delayms(5000);
    g_gGprsSETip =0x55;	
    if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(0,0,g_gIPPort,this);
    delayms(3000);
    if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(6,5,&g_gIPPort[6],this);	
    delayms(5000);
    if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(6,RESET,g_gIPPort,this);//重启GPRS模块
  }
  if(wInfoAddr == 0x6003 && bTypeID == 0x6C)   //设置链路地址
  {
    temp0=*pData;pData++;
    temp1=*pData;pData++;
    temp0=*pData;pData++;
    temp1=*pData;pData++;   
    wVal = MAKEWORD(temp0,temp1);
    wTemp = wVal;
    SendFrameHead(0x6C,7);
    for(i = (m_byInfoShift ); i < (pReceiveFrame->Frame68.Length1-1); i++ )
    {
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =pReceiveFrame->Frame68.Data[i];
    }    
    SendFrameTail(0, 0x85, 1,0);
    if(wTemp != g_gRunPara[RP_COMM_ADDR])
    {
      g_gRunPara[RP_COMM_ADDR] = wTemp;
      g_ucParaChang |= BIT0;
      SetEqpInfo();
      g_gRunPara[RP_CRC] = CrcCount((unsigned int *)g_gRunPara, RP_CRC);      //计算CRC
      CAT_SpiWriteWords(EEPADD_RP, RUN_PARA_NUM, g_gRunPara); //保存到EEPROM中	  
      delayms(300);
      main_reset_flag = 0x55;
    }	
    delayms(5000);
  }
  if(wInfoAddr == 0x6009 && bTypeID == 0x38)   //设置心跳周期
  {
    temp0=*pData;pData++;
    temp1=*pData;pData++;
    temp0=*pData;pData++;
    temp1=*pData;pData++;   
    wVal = MAKEWORD(temp0,temp1);
    wTemp = wVal;
    if(wTemp != g_gRunPara[RP_BEAT_T])
    {
      g_gRunPara[RP_BEAT_T] = wTemp;
      g_ucParaChang |= BIT0;
      SetEqpInfo(); 
    }          
    SendFrameHead(0x38,7);
    for(i = (m_byInfoShift ); i < (pReceiveFrame->Frame68.Length1-1); i++ )
    {
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =pReceiveFrame->Frame68.Data[i];
    }
    SendFrameTail(0, 0x85, 1,0);	
    delayms(5000);
  }
  if(wInfoAddr == 0x6011 && bTypeID == 0x3f)   //设置遥测上传周期
  {
    temp0=*pData;pData++;
    temp1=*pData;pData++;
    temp0=*pData;pData++;
    temp1=*pData;pData++;    
    wVal = MAKEWORD(temp0,temp1);
    wTemp = wVal;
    if(wTemp != g_gRunPara[RP_SENDYC_T])
    {
      g_gRunPara[RP_SENDYC_T] = wTemp;
      g_ucParaChang |= BIT0;
      SetEqpInfo();
    }
    SendFrameHead(0x3f,7);
    for(i = m_byInfoShift; i < (pReceiveFrame->Frame68.Length1-1); i++ )
    {
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =pReceiveFrame->Frame68.Data[i];
    }
    SendFrameTail(0, 0x85, 1,0);
    delayms(5000);
  }
  if(wInfoAddr == 0x6014 && bTypeID == 0x44)   //设置自检上传周期
  {
    temp0=*pData;pData++;
    temp1=*pData;pData++;
    temp0=*pData;pData++;
    temp1=*pData;pData++;    
    wVal = MAKEWORD(temp0,temp1);
    wTemp = wVal;
    if(wTemp != g_gRunPara[RP_SENDZJ_T])
    {
      g_gRunPara[RP_SENDZJ_T] = wTemp;
      g_ucParaChang |= BIT0;
      SetEqpInfo();
    }
    SendFrameHead(0x44,7);
    for(i = m_byInfoShift; i < (pReceiveFrame->Frame68.Length1-1); i++ )
    {
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =pReceiveFrame->Frame68.Data[i];
    }
    SendFrameTail(0, 0x85, 1,0);
    delayms(5000);
  }
  pData+=2;
  switch(bTypeID)
  { 
       /* 
        case  53: //信号源
        {
              switch(wInfoAddr)
              {
                case 0x6003:  //修改链路地址
                  wVal = MAKEWORD(pData[0],pData[1]);
                  wTemp = wVal;
                   if(wTemp != g_gRunPara[RP_COMM_ADDR])
                   {
                       g_gRunPara[RP_COMM_ADDR] = wTemp;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                    }
          
                   break;
                case 0x6009:  //修改信号源心跳周期
                  wVal = MAKEWORD(pData[0],pData[1]);
                  wTemp = wVal;
                   if(wTemp != g_gRunPara[RP_BEAT_T])
                   {
                       g_gRunPara[RP_BEAT_T] = wTemp;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                    }          
                   break;
                 case 0x6011:  //遥测上传
                  wVal = MAKEWORD(pData[0],pData[1]);
                  wTemp = wVal;
                   if(wTemp != g_gRunPara[RP_SENDYC_T])
                   {
                       g_gRunPara[RP_SENDYC_T] = wTemp;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                    }
          
                   break;
                case 0x6014:  //自检信息上传
                  wVal = MAKEWORD(pData[0],pData[1]);
                  wTemp = wVal;
                   if(wTemp != g_gRunPara[RP_DYX_INFADDR])
                   {
                       g_gRunPara[RP_DYX_INFADDR] = wTemp;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                    }
          
                   break;
                case 0x6020:
                  wVal = MAKEWORD(pData[0],pData[1]);
                  wTemp = wVal;
                   if(wTemp != g_gRunPara[RP_HIGH_Z])
                   {
                       g_gRunPara[RP_HIGH_Z] = wTemp;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                    }
          
                   break;
                case 0x6021:
                  wVal = MAKEWORD(pData[0],pData[1]);
                  wTemp = wVal;
                   if(wTemp != g_gRunPara[RP_I0_START])  //线电压过压
                   {
                       g_gRunPara[RP_I0_START] = wTemp;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                    }
          
                   break;
                case 0x6022:
                  wVal = MAKEWORD(pData[0],pData[1]);
                  wTemp = wVal;
                   if(wTemp != g_gRunPara[RP_HIGH_P])
                   {
                       g_gRunPara[RP_HIGH_P] = wTemp;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                    }
          
                   break;
                case 0x6023:
                  wVal = MAKEWORD(pData[0],pData[1]);
                  wTemp = wVal;
                   if(wTemp != g_gRunPara[RP_LOW_P])
                   {
                       g_gRunPara[RP_LOW_P] = wTemp;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                    }
          
                   break;
                case 0x6024:
                  wVal = MAKEWORD(pData[0],pData[1]);
                 
                   if(wVal != g_gRunPara[RP_PULSE_VALID])  //
                   {
                       g_gRunPara[RP_PULSE_VALID] = wVal;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                    }
          
                   break;
                case 0x6025:
                  wVal = MAKEWORD(pData[0],pData[1]);
                   if(wVal != g_gRunPara[RP_T_DELAY])
                   {
                       g_gRunPara[RP_T_DELAY] = wVal;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                    }
          
                   break;
                case 0x6026:
                   wVal = MAKEWORD(pData[0],pData[1]);
                   if(wVal != g_gRunPara[RP_CNL_MODEL])
                   {
                       g_gRunPara[RP_CNL_MODEL] = wVal;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                   }
          
                   break;
              
              }
              SendFrameHead(53,7);
              write_infoadd(wInfoAddr);
              m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wVal);
              m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wVal);
              SendFrameTail(0, 0x85, 1);
              delayms(5000);
                      
        }
        break;
		*/
        case  107: //信号源
          {
              switch(wInfoAddr)
              {
                case 0x6002:
                  for(i = 0; i<7; i++)
                  {
                      g_ChaxunSOE_STATTime[i] = pData[i];
                      g_ChaxunSOE_OVERTime[i] = pData[i + 7];
                      
                  }
          
                   break;
                
              
              }
              g_ChaxunSOE_STATTime[4] = (g_ChaxunSOE_STATTime[4]&0x1F);
              g_ChaxunSOE_OVERTime[4] = (g_ChaxunSOE_OVERTime[4]&0x1F);
              m_HistLoadFlag = 0xff;
              m_HistSoeSEG = 0;
              m_PtrSendSOE = 0;
              SendFrameHead(107,7);
              write_infoadd(wInfoAddr);
              for(i = 0; i<14; i++)
                m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pData[i];
             // m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wVal);
              SendFrameTail(0, 0x86, 1,0);
              
          }
        
       break;
       case  122: //信号源
          {
              switch(wInfoAddr)
              {
                case 0x4088:

          
                   break;
                
              
              }

              m_HistLuBoFlag = 0xff;

          /*    SendFrameHead(126,5);
              write_infoadd(wInfoAddr);
              m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 
              for(i = 0; i<14; i++)
                m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pData[i];
             // m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wVal);
              SendFrameTail(0, 0x83, 1);
             */ 
          }
        
       break;
  }
  return TRUE;

}
#endif
/*********************************************************
函数名称:SendCallHistLoad
输入参数：无
输出参数：布尔型
函数功能说明：用于控制召唤历史遥测数据的步骤
备注说明：for yn 特殊规约，详见规约说明
日期：2015年10月19日  by：zx
*********************************************************/
BOOL CBJ101S::SendCallHistLoad(void)
{
    if(m_HistLoadFlag & 1)
    {
        if(SendHistSOE())
        {
            if(m_PtrSendSOE)
                m_HistSoeSEG++;
            else
               m_PtrSendSOE = 21;
            return true;
        }
        m_HistLoadFlag &= ~BIT0;
    }
    if(m_HistLoadFlag & BIT3)
    {
        m_HistLoadFlag &= ~BIT3;
        m_acdflag = 0;
        SendCallHistLoadStop();
    }    
    else
    {
        m_HistLoadFlag = 0;
        return false;
    }
    return true;
}
/*******************************************************************
函数名称:SendHistSOE
输入参数：无
输出参数：布尔型,true表示有数据发送，false表示无数据发送
函数功能说明：读取内部Flash中存储的SOE，发送给主站，每次最多发送16条
              内部无Flash时不发任何数据
备注说明：for yn 特殊规约，详见规约说明
日期：2015年10月19日  by：zx
*********************************************************************/
BOOL CBJ101S::SendHistSOE(void)
{
    BYTE SendSoeNum;
    BYTE ByData[256];
    SendSoeNum = ReadSoeHistory(ByData,m_HistSoeSEG,m_PtrSendSOE,21);
    if(SendSoeNum)
    {
        SendFrameHead(0x1E,0x0B);
        
        //for(BYTE i = 0,j = 0; i < SendSoeNum*SOE_SENDDA_LEN; i++)
        for(BYTE i = 0; i < SendSoeNum*SOE_SENDDA_LEN; i++)         
        {
          /*  if(i == j * SOE_SENDDA_LEN)
            {
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = ByData[i] + g_gRunPara[RP_SYX_INFADDR];
                j++;
            }
            else*/
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = ByData[i];
        }
        SendFrameTail(0,0x83,SendSoeNum,0);
        return true;
    }
    else
        return false;
}
/*******************************************************************
函数名称:SendCallHistLoadStop
输入参数：无
输出参数：布尔型
函数功能说明：发送召唤历史数据结束帧              
备注说明：for yn 特殊规约，详见规约说明
日期：2015年10月19日  by：zx
*********************************************************************/
BOOL CBJ101S::SendCallHistLoadStop(void)
{
    
    SendFrameHead(0x6B,0x0A);
    write_infoadd(0);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
    SendFrameTail(0,0x86,1,0);
    return true;
}

/*********************************************************
函数名称:SendCallHistLoad
输入参数：无
输出参数：布尔型
函数功能说明：用于控制召唤历史遥测数据的步骤
备注说明：for yn 特殊规约，详见规约说明
日期：2015年10月19日  by：zx
*********************************************************/
BOOL CBJ101S::SendCallHistLuBo(void)
{
    if(m_HistLuBoFlag & 1)
    {
        if(SendCallHistLuBoStartAck())
        {
            m_HistLuBoFlag &= ~BIT0;
            return true;
        }
        
    }
    if(m_HistLuBoFlag & 2)
    {
        if(SendCallHistLuBoCFGAck())
        {
            m_HistLuBoFlag &= ~BIT1;
            return true;
        }
        
    }
    if(m_HistLuBoFlag & 4)
    {
        if(SendHistLuBo1())
        {
            m_HistLuBoFlag &= ~BIT2;
            return true;
        }
        
    }
    if(m_HistLuBoFlag & 8)
    {
        if(SendHistLuBo2())
        {
            m_HistLuBoFlag &= ~BIT3;
            return true;
        }
        
    }

    if(m_HistLuBoFlag & BIT4)
    {
        m_HistLuBoFlag &= ~BIT4;
        m_acdflag = 0;
        SendCallHistLuBoStop();
    }    
    else
    {
        m_HistLuBoFlag = 0;
        return false;
    }
    return true;
}
/*******************************************************************
函数名称:SendHistSOE
输入参数：无
输出参数：布尔型,true表示有数据发送，false表示无数据发送
函数功能说明：读取内部Flash中存储的SOE，发送给主站，每次最多发送16条
              内部无Flash时不发任何数据
备注说明：for yn 特殊规约，详见规约说明
日期：2015年10月19日  by：zx
*********************************************************************/
BOOL CBJ101S::SendHistLuBo1(void)
{
    //BYTE SendSoeNum;
   // BYTE ByData[256];
   // SendSoeNum = ReadSoeHistory(ByData,m_HistSoeSEG,m_PtrSendSOE,21);
  //  if(SendSoeNum)
    {
        SendFrameHead(0x7E,0x0B);
        write_infoadd(0x4088);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 1;
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 96;
       // m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 2;
        
        for(BYTE i = 0; i < 48; i++)
        {
//张弢 录波I0U0
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;//LOBYTE(g_sRecData.m_gRecAc[CHAN_I0][i]);
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;//HIBYTE(g_sRecData.m_gRecAc[CHAN_I0][i]);
        }
        SendFrameTail(0,0x83,1,0);
        return true;
    }
   // else
   //     return false;
    
}

/*******************************************************************
函数名称:SendHistSOE
输入参数：无
输出参数：布尔型,true表示有数据发送，false表示无数据发送
函数功能说明：读取内部Flash中存储的SOE，发送给主站，每次最多发送16条
              内部无Flash时不发任何数据
备注说明：for yn 特殊规约，详见规约说明
日期：2015年10月19日  by：zx
*********************************************************************/
BOOL CBJ101S::SendHistLuBo2(void)
{
    //BYTE SendSoeNum;
   // BYTE ByData[256];
  //  SendSoeNum = ReadSoeHistory(ByData,m_HistSoeSEG,m_PtrSendSOE,21);
  //  if(SendSoeNum)
    {
        SendFrameHead(0x7E,0x0B);
        write_infoadd(0x4088);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 1;
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 160;
       // m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 2;
        
        for(BYTE i = 0; i < 80; i++)
        {

           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] =0;//张弢 录波I0U0 LOBYTE(g_sRecData.m_gRecAc[CHAN_I0][i + 48]);
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] =0;//张弢 录波I0U0 HIBYTE(g_sRecData.m_gRecAc[CHAN_I0][i + 48]);
        }
        SendFrameTail(0,0x83,1,0);
        return true;
    }
 //   else
     //   return false;
}
/*******************************************************************
函数名称:SendCallHistLoadStop
输入参数：无
输出参数：布尔型
函数功能说明：发送召唤历史数据结束帧              
备注说明：for yn 特殊规约，详见规约说明
日期：2015年10月19日  by：zx
*********************************************************************/
BOOL CBJ101S::SendCallHistLuBoStop(void)
{
    
    SendFrameHead(0x7A,0x0A);
    write_infoadd(0x4088);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 2;
    SendFrameTail(0,0x03,1,0);
    return true;
}
/*****************************************************
函数名称:SendCallHistLoadStartAck
输入参数：无
输出参数：布尔型
函数功能说明：用于发送召唤历史遥测的确认帧
备注说明：for yn 特殊规约，详见规约说明
日期：2015年10月19日  by：zx
*****************************************************/
BOOL CBJ101S::SendCallHistLuBoStartAck(void)
{
     //BYTE * pData = &pReceiveFrame->Frame68.Data[m_byInfoShift];
     SendFrameHead(0x7E,5);
     write_infoadd(0x4088);
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 1;
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
    /* for(BYTE i = 0 ; i < 7; i++)  //信息体+起止日期，共16个字节，此处信息体地址为2个字节
     {
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = g_sRecData.m_gFaultRecSOE[REC_MSL + i];
     }*/
     //for(BYTE i = 0 ; i < 7; i++)  //信息体+起止日期，共16个字节，此处信息体地址为2个字节
     {
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = (g_sRtcManager.m_gRealTimer[RTC_SEC] * 1000);
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = ((g_sRtcManager.m_gRealTimer[RTC_SEC] * 1000)>>8);
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = g_sRtcManager.m_gRealTimer[RTC_DATE];
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = (g_sRtcManager.m_gRealTimer[RTC_YEAR]-2000);
     }
     SendFrameTail(0,0x83,1,0);
     return true;
}
/*****************************************************
函数名称:SendCallHistLoadStartAck
输入参数：无
输出参数：布尔型
函数功能说明：用于发送召唤历史遥测的确认帧
备注说明：for yn 特殊规约，详见规约说明
日期：2015年10月19日  by：zx
*****************************************************/
BOOL CBJ101S::SendCallHistLuBoCFGAck(void)
{
     //BYTE * pData = &pReceiveFrame->Frame68.Data[m_byInfoShift];
     SendFrameHead(0x7E,0x0b);
     write_infoadd(0x4088);
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x0C;
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 1;
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
     
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
     
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x08;
     
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0xF8;
     
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x20;
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x03;
     
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x80;
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
     
     SendFrameTail(0,0x83,1,0);
     return true;
}

WORD CBJ101S::GetDataFromeEncChip(BYTE *pRecData)
{
    int DataLen;
    BYTE pData[16];
    if(pRecData != NULL)
    {
        DataLen = RecBytesFromEsam(pRecData);
       // if(DataLen)
        //{
        //    return TRUE;
        //}
    }
    else
    {
        DataLen = RecBytesFromEsam(pData);
    }
    if(DataLen == 0)
    {
        //P6OUT |= BIT4;
        P1OUT |= BIT6;
        delayms(500);
        P1OUT &= ~BIT6;
        //P6OUT &= ~BIT4;
        delayms(500);
    }
    return DataLen;
}

BOOL CBJ101S::GetChipID(BYTE *pChipID)
{
    SendCmdToEncChip(GET_CHIP_ID,NULL,0);
    if(GetDataFromeEncChip(pChipID))
    {
        return TRUE;
    }
    return FALSE;
}

BOOL CBJ101S::GetChipKeyVer(BYTE *Key)
{
    SendCmdToEncChip(GET_RRI_KEY,NULL,0);
    if(GetDataFromeEncChip(Key))
    {
        
        return TRUE;
    }
    return FALSE;
}
BOOL CBJ101S::GetChipRandom(BYTE *pRandom)
{
    SendCmdToEncChip(GET_RANDOM,NULL,0);
    if(GetDataFromeEncChip(pRandom))
    {
        return TRUE;
    }
    return FALSE;
}
BOOL CBJ101S::GetChipRandomSign(BYTE*pHostRandom,BYTE *pRecSign)
{
    SendCmdToEncChip(GET_RANDOM_SIGN,pHostRandom,8);
    if(GetDataFromeEncChip(pRecSign))
    {
        return TRUE;
    }
    return FALSE;
}

BOOL CBJ101S::VerifySign(BYTE KeyIndex,BYTE*Sign)
{
    BYTE SendData[256] = {0};
    memcpy(SendData,&KeyIndex,1);
    SendData[2] = 0x40;   //数据长度 
    memcpy(&SendData[3],Sign,0x40);
    SendCmdToEncChip(VERIFY_SIGN,SendData,67);
    if(GetDataFromeEncChip(NULL))
    {
        return TRUE;
    }
    return FALSE; 
}
//数据加密 
WORD CBJ101S::EncryptData(BYTE *pClearData,WORD DataLen,BYTE *pEncData)
{
    WORD count = 0;
    BYTE SendData[256] = {0};//zxx 内存开的是否小了，如果101的报文每帧长度都在250个字节
    SendData[count++] = 0;  //预留字符长度
    SendData[count++] = 0;  //预留字符长度
    for(BYTE i = 0; i < 8; i++)
    {
        SendData[count++] = g_bHostRandom[i];//zxx 主站的随机数还未获取
    }
    for(BYTE i = 0; i < 8; i++)
    {
        SendData[count++] = ~g_bHostRandom[i];
    }
    for(BYTE i = 0; i < DataLen; i++)
    {
        SendData[count++] = pClearData[i];
    }
    SendData[0] = HIBYTE(count-2);
    SendData[1] = LOBYTE(count-2);
    SendCmdToEncChip(ENCRYPT_DATA,SendData,count);
    return GetDataFromeEncChip(pEncData); 
}
//数据解密
WORD CBJ101S::DecryptData(BYTE *EncData,WORD EncDataLen,BYTE *ClearData)
{
    WORD count = 0; 
    BYTE SendData[1024] = {0};
    count  += 2;  //预留2 字节长度
    for(BYTE i = 0; i < 8; i++)
    {
        SendData[count++] = g_bHostRandom[i];
    }
    for(BYTE i = 0; i < 8; i++)
    {
        SendData[count++] = ~g_bHostRandom[i];
    }
    for(WORD i = 0; i < EncDataLen; i++)
    {
        SendData[count++] = EncData[i];
    }
    SendData[0] = HIBYTE(count - 2);
    SendData[1] = LOBYTE(count - 2);
    SendCmdToEncChip(DECRYPT_DATA,SendData,count);
    return (GetDataFromeEncChip(ClearData));
}


BOOL CBJ101S::VerifyDataSign(BYTE asKey,BYTE* pData,WORD Datalen,BYTE* pSign,WORD SignLen)
{
    BYTE SendData[1024] = {0};  //zx 20170503修改
    SendData[0] = asKey;
    SendData[1] = HIBYTE(Datalen + SignLen);
    SendData[2] = LOBYTE(Datalen + SignLen);
    if(Datalen)
    {
        memcpy(&SendData[3],pData,Datalen);
    }
    memcpy(&SendData[3+Datalen],pSign,SignLen);
    SendCmdToEncChip(VERIFY_DATA_SIGN,SendData,SignLen+Datalen+3);
    if(GetDataFromeEncChip(NULL))
    {
        return TRUE;
    } 
    return FALSE;
}

BOOL CBJ101S::UpdataCer(BYTE CerID, BYTE* CerTxt, WORD CerTxtLen)
{
    if(CerID == 0x06)
    {
        CerTxt[0] = 0x00;
    }
    else
    {
        CerTxt[0] = CerID;
    }
    CerTxt[1] = 0x00;
    CerTxt[2] = HIBYTE(CerTxtLen - 4);  //减去预留的长度
    CerTxt[3] = LOBYTE(CerTxtLen - 4);
    //memcpy(&SendData[2],CerTxt,CerTxtLen);
    if(CerID == 0x06)
    {
        SendCmdToEncChip(TOOL_LOAD_CER,CerTxt,CerTxtLen);  
    }
    else
    {
        SendCmdToEncChip(UPDATA_CERTIFICATE,CerTxt,CerTxtLen);  
    }
    if(GetDataFromeEncChip(NULL))
    {
        return TRUE;
    }
    return FALSE;
}

BOOL CBJ101S::LoadCer(BYTE* CerTxt, WORD CerTxtLen)
{
    g_bEncCerTxt[0] = HIBYTE(CerTxtLen - 2);  //zx修改 20170503
    g_bEncCerTxt[1] = LOBYTE(CerTxtLen - 2);
    SendCmdToEncChip(LOAD_CERTIFICATE,CerTxt,CerTxtLen);
    if(GetDataFromeEncChip(NULL))
    {
        return TRUE;
    } 
    return FALSE; 
}
BOOL CBJ101S::ReadCerLen(BYTE *CerLen)
{
    SendCmdToEncChip(READ_CERTIFICATE_LEN,NULL,0);
    if(GetDataFromeEncChip(CerLen))
    {
        return TRUE;
    }
    return FALSE;
}

BOOL CBJ101S::ReadTestCer(BYTE *CerLen,BYTE *CerTxt)
{
    SendCmdToEncChip(READ_TEST_CERTIFICATE,CerLen,2);
    if(GetDataFromeEncChip(CerTxt))
    {
        return TRUE;
    }
    return FALSE;
}
WORD CBJ101S::ReadRealCer(BYTE *CerTxt)
{
    //BYTE CerLen[2];
    //if(ReadCerLen(CerLen) != TRUE)
    //{
    //    return false;
    //} 
    //SendCmdToEncChip(READ_REAL_CERTIFICATE,NULL,0);
    //SendCmdToEncChip(READ_TEST_CERTIFICATE,CerLen,2);
     //WORD CerLen;
    SendCmdToEncChip(READ_REAL_CERTIFICATE,NULL,0);
    //delayms(1000);
    return(GetDataFromeEncChip(CerTxt));
}
BOOL CBJ101S::UpdataPrivateKey(BYTE asKID,BYTE NewKeyVer,BYTE* keySignData,WORD dataLen)
{
    BYTE SendData[512] = {0};
    SendData[0] = asKID;
    SendData[1] = HIBYTE(dataLen + 1);  //数据长度+ 1个字节的密钥版本
    SendData[2] = LOBYTE(dataLen + 1);
    SendData[3] = NewKeyVer;
    memcpy(&SendData[4],keySignData,dataLen);
    SendCmdToEncChip(UPDATA_PRIVATE_KEY,SendData,dataLen + 4);
    if(GetDataFromeEncChip(NULL))
    {
        return TRUE;
    } 
    return FALSE;
  
}

BOOL CBJ101S::RecovPrivateKey(BYTE asKID,BYTE* keySignData,WORD dataLen)
{
    BYTE SendData[512] = {0};
    SendData[0] = asKID;
    SendData[1] = HIBYTE(dataLen + 1);
    SendData[2] = LOBYTE(dataLen + 1);
    SendData[3] = 0;  //密钥恢复时，密钥版本固定为0
    memcpy(&SendData[4],keySignData,dataLen);
    SendCmdToEncChip(RECOV_PRIVATE_KEY,SendData,dataLen + 4);
    if(GetDataFromeEncChip(NULL))
    {
        return TRUE;
    } 
    return FALSE;
}
BOOL CBJ101S::VerifyToolCer(BYTE *pCerTxt,WORD CerLen)
{
    pCerTxt[0] = HIBYTE(CerLen);
    pCerTxt[1] = LOBYTE(CerLen);
    SendCmdToEncChip(TOOL_VERIFY_CER,pCerTxt,CerLen + 2);//发送的数据+ 2字节的长度
    if(GetDataFromeEncChip(NULL))
    {
        return TRUE;
    } 
    return FALSE;    
}
BOOL CBJ101S::VerifyToolSignData(BYTE *pSignTxt,BYTE SignLen)
{
    SendCmdToEncChip(TOOL_VERIFY_SIGN,pSignTxt,SignLen);
    if(GetDataFromeEncChip(NULL))
    {
        return TRUE;
    } 
    return FALSE;      
}
BOOL CBJ101S::ReadPubKey(BYTE *pPubKey)
{
    SendCmdToEncChip(TOOL_READ_PUBK,NULL,0);
    if(GetDataFromeEncChip(pPubKey))
    {
        return TRUE;
    }
    return FALSE; 
}

WORD CBJ101S::EncryptToolData(BYTE *pOriginalData, WORD  OriginalDataLen,BYTE *pEncData)
{
    WORD count = 0;
    //+32是因为有8字节管理工具ID，8字节R1，16字节IVData
    WORD DataLen = 32 + OriginalDataLen;  
    pOriginalData[count++] = HIBYTE(DataLen);
    pOriginalData[count++] = LOBYTE(DataLen);
    for(BYTE i = 0; i < 8; i++)
    {
        pOriginalData[count++] = m_bCerToolID[i];
    }
    for(BYTE i = 0; i < 8; i++)
    {
        pOriginalData[count++] = m_bUnitRandom[i];
    }
    for(BYTE i = 0; i < 8; i++)   
    {
        pOriginalData[count++] = m_bUnitRandom[i];
    }
    for(BYTE i = 0; i < 8; i++)
    {
        pOriginalData[count++] = ~m_bUnitRandom[i];
    } 
    SendCmdToEncChip(TOOL_ENCRYPT_DATA,pOriginalData,DataLen+2);  //+2个字节数据长度
    return (GetDataFromeEncChip(pEncData));
}
WORD CBJ101S::DecryptToolData(BYTE *pEncData, WORD  EncDataLen,BYTE *pClearData)
{
    //+32是因为有8字节管理工具ID，8字节R1，16字节IVData
    BYTE SendData[1024] = {0};
   //WORD DataLen = 32 + EncDataLen;
    WORD count = 0;
    count += 2;
    //SendData[count++] = HIBYTE(DataLen);
    //SendData[count++] = LOBYTE(DataLen);
    for(BYTE i = 0; i < 8; i++)
    {
        SendData[count++] = m_bCerToolID[i];
    }
    for(BYTE i = 0; i < 8; i++)
    {
        SendData[count++] = m_bUnitRandom[i];
    }
    for(BYTE i = 0; i < 8; i++)   
    {
        SendData[count++] = m_bUnitRandom[i];
    }
    for(BYTE i = 0; i < 8; i++)
    {
        SendData[count++] = ~m_bUnitRandom[i];
    }
    for(WORD i = 0; i < EncDataLen; i++)
    {
        SendData[count++] = pEncData[i];
    }
    SendData[0] = HIBYTE(count - 2);
    SendData[1] = LOBYTE(count - 2);
    SendCmdToEncChip(TOOL_DECRYPT_DATA,SendData,count);
    return (GetDataFromeEncChip(pClearData));
}
BOOL CBJ101S::RecovToolKey(BYTE *pKeyData, BYTE KeyDataLen)
{
    pKeyData[0] = 0;
    pKeyData[1] = KeyDataLen;
    SendCmdToEncChip(TOOL_RECOV_PRIVATE_KEY,pKeyData,KeyDataLen + 2);
    if(GetDataFromeEncChip(null))
    {
        return TRUE;
    }
    return FALSE;
}

WORD CBJ101S::SignData(BYTE *pOriginalData,WORD OriginalDataLen,BYTE *pSrReqData)
{
    pOriginalData[0] = HIBYTE(OriginalDataLen - 2);
    pOriginalData[1] = LOBYTE(OriginalDataLen - 2);  //减去2个字节长度
    SendCmdToEncChip(TOOL_GET_SIGN,pOriginalData,OriginalDataLen);
    return (GetDataFromeEncChip(pSrReqData));
}

BOOL CBJ101S::SendFrameEBHead(WORD FrameType,BYTE AppType)
{
    m_SendBuf.wReadPtr = 0;
    m_SendBuf.wWritePtr=0;
    pSendFrame = (VIec101Frame *)m_SendBuf.pBuf; 
    
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr]   = 0xEB;
    m_SendBuf.wWritePtr+=3;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0xEB;   
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = HIBYTE(FrameType);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = LOBYTE(FrameType);
    if(AppType <= 200)
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = AppType;
    return TRUE;
}

BOOL CBJ101S::SendFrameEBAppData(BYTE AppDataBytes,BYTE *pAppData)
{
    if(AppDataBytes && (pAppData != NULL))
    {
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = AppDataBytes;
        for(BYTE i = 0; i < AppDataBytes; i++)
        {
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = pAppData[i];
        } 
    }
    return TRUE;
}

BOOL CBJ101S::SendFrameEBEixampleData(BYTE EixampleDataBytes,BYTE *pEixampleData)
{
    if(EixampleDataBytes && (pEixampleData != NULL))
    {
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = EixampleDataBytes;
        for(BYTE i = 0; i < EixampleDataBytes; i++)
        {
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = pEixampleData[i];
        } 
    }
    return TRUE;
}
BOOL CBJ101S::SendEncData(BYTE EncDataLen,BYTE *pEncData)
{
    for(BYTE i = 0; i < EncDataLen; i++)
    {
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = pEncData[i];
    }
    return TRUE;
}

BOOL CBJ101S::SendAppData(BYTE EixampleDataBytes,BYTE *pEixampleData)
{
    if(EixampleDataBytes && (pEixampleData != NULL))
    {
        //m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0;
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = EixampleDataBytes;
        for(BYTE i = 0; i < EixampleDataBytes; i++)
        {
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = pEixampleData[i];
        } 
    }
    return TRUE;
}
BOOL CBJ101S::SendFrameEBTail(void)
{
    pSendFrame->FrameEB.LengthH = 0;
    pSendFrame->FrameEB.LengthL= m_SendBuf.wWritePtr - 4;
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (BYTE)ChkSum((BYTE *)&pSendFrame->FrameEB.FrameType[0], pSendFrame->FrameEB.LengthL);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0xD7;
    WriteToComm(0);
    return TRUE;
}

BOOL CBJ101S::SendEncFrameAck(WORD Ack,WORD FrameType,BYTE AppType)
{
    SendFrameEBHead(FrameType,AppType);
    SendFrameEBEixampleData(2,(BYTE*)&Ack);
    SendFrameEBTail();
    return TRUE;
}
BOOL CBJ101S::RecCipherTxt(BYTE *EncData,WORD EncDataLen,BYTE *pClearData)
{
    //BYTE RecData[MAX_FRAME_LEN];
    if(DecryptData(EncData,EncDataLen, pClearData))
    {
        return TRUE;
    }
    return FALSE;
}
//认证网关与终端认证
BOOL CBJ101S::SendGatewayVerify(void)
{
     BYTE RecData[128];
    if(GetChipRandomSign(&pReceiveFrame->FrameEB.Data[2],RecData))
    {
        RecData[72] = 0x06;    //网关认证，签名标识固定为0x06
        SendFrameEBHead(0x0080,0x21);
        SendFrameEBEixampleData(0x49,RecData);
        SendFrameEBTail();
        return TRUE;
    }
    //SendEncFrameAck(0x9090, 0x0080,0x21);
    return FALSE;
}
BOOL CBJ101S::SendGatewayVerifyAck(void)
{
    //66的原因是2字节数据长度+64字节签名
    BYTE asKey = pReceiveFrame->FrameEB.Data[66];  //ZX 修改 20170503
    if(VerifySign(asKey,&pReceiveFrame->FrameEB.Data[2]))
    {
        SendEncFrameAck(0x0090,0x0080,0x23);
    }
    else
    {
        SendEncFrameAck(0x9090,0x0080,0x23);
    }
    return TRUE;
}
BOOL CBJ101S::SendHostVerify(void)
{
    BYTE RecData[128];
    memcpy(g_bHostRandom,&pReceiveFrame->FrameEB.Data[HOST_RANDOM],RANDOM_BYTES);
    if(GetChipRandomSign(g_bHostRandom,RecData))
    {
        RecData[72] = 0x01;
        SendFrameEBHead(0x0000,0x51);
        SendFrameEBEixampleData(0x49,RecData);
        SendFrameEBTail();
        return TRUE;
    }
    SendEncFrameAck(0x9090, 0x0000,0x51);
    return FALSE;     
}

BOOL CBJ101S::SendHostVerifyAck(void)
{
    //66的原因是2字节数据长度+64字节签名
    BYTE asKey = pReceiveFrame->FrameEB.Data[66];  
    if(VerifySign(asKey,&pReceiveFrame->FrameEB.Data[2]))
    {
        SendEncFrameAck(0x0090,0x0000,0x53);
        m_bHostCertifyFlag = 0x55;
    }
    else
    {
        SendEncFrameAck(0x9090,0x0000,0x53);
        m_bHostCertifyFlag = 0;
    }
    return TRUE;
}


BOOL CBJ101S::SendEncChipID(BYTE type)
{
    BYTE RecData[CHINP_ID_BYTES] = {0};
    if(GetChipID(RecData))
    {
        if(type == 0)  //发送给主站
            SendFrameEBHead(0x0000,0x55);
        else   //发送给运维工具
            SendFrameEBHead(0x0040,0x39);
        SendFrameEBEixampleData(CHINP_ID_BYTES,RecData);
        SendFrameEBTail();
        return TRUE;
    }
//    if(type == 0)
//    {
//        SendEncFrameAck(0x0991, 0x0010,0x1f);
//    }
//  else
//    {
//        SendEncFrameAck(0x0991, 0x0040,0x1f);
//    }
    return FALSE;
}
BOOL CBJ101S::SendChipKeyVer(BYTE type)
{
    BYTE RecData[16] = {0};

    if(!GetChipKeyVer(RecData))
    {
        //SendEncFrameAck(0x0991, 0x0010,0x1f);
        return FALSE;
    }
    if(!GetChipRandom(&RecData[1]))
    {
        //SendEncFrameAck(0x0991, 0x0010,0x1f);
        return FALSE;
    }
    //memcpy(&RecData[1],m_bUnitRandom,8);
    if(type == 0)  //主站
    {
        if(m_bHostCertifyFlag != 0x55)
        {
            return TRUE;
        }
        SendFrameEBHead(0x0000,0x61);
    }
    else if(type == 1)  //运维工具
    { 
        if(m_bToolCertifyFlag != 0x55)
        {
             //SendEncFrameAck(0x0991,0x0040,0x35);
             return TRUE;
         } 
         SendFrameEBHead(0x0040,0x35);
    }
        
    SendFrameEBEixampleData(9,RecData);
    SendFrameEBTail();
    return TRUE;
}
BOOL CBJ101S::SendUpdataKeyAck(void)
{
    BYTE KeyVer;  //密钥版本
    BYTE asKID;   //密钥索引
    //密钥更新包+签名结果 共244个字节
    BYTE *pDataSign = &pReceiveFrame->FrameEB.Data[UPDATA_KEY_DATA];
  
    KeyVer = pReceiveFrame->FrameEB.Data[NEW_KEY_VER];
  
    asKID = pReceiveFrame->FrameEB.Data[S_KEY_ID];
    if((m_bHostCertifyFlag != 0x55) || (KeyVer <= 0))
    {
        SendEncFrameAck(0x9190,0x0000,0x63);
    }
    else if(UpdataPrivateKey(asKID,KeyVer,pDataSign,244) == TRUE)
    {
        SendEncFrameAck(0x0090,0x0000,0x63);
    }
    else
    {
        SendEncFrameAck(0x9190,0x0000,0x63);
    }
    return TRUE;
}

BOOL CBJ101S::SendRecovKeyAck(void)
{
    BYTE asKID;   //密钥索引
    BYTE *pDataSign = &pReceiveFrame->FrameEB.Data[UPDATA_KEY_DATA];
    asKID = pReceiveFrame->FrameEB.Data[S_KEY_ID];
    if(m_bHostCertifyFlag != 0x55)
    {
        SendEncFrameAck(0x9290,0x0000,0x71);
    }
    else if(RecovPrivateKey(asKID,pDataSign,244) == TRUE)
    {
        SendEncFrameAck(0x0090,0x0000,0x71);
    }
    else
    {
        SendEncFrameAck(0x9290,0x0000,0x71);
    }
    return TRUE;
}

BOOL CBJ101S::SendCerUpdataAck(void)  //远程证书更新
{
    BYTE FrameNum =  pReceiveFrame->FrameEB.Data[3];
    BYTE FrameNO  =  pReceiveFrame->FrameEB.Data[4];
    BYTE *pData   = &pReceiveFrame->FrameEB.Data[5];
    WORD DataLen  =  MAKEWORD(pReceiveFrame->FrameEB.Data[1], pReceiveFrame->FrameEB.Data[0]) - 3;  //ZX修改2017年5月3日
    BYTE KeyID;   //密钥索引
    BYTE CerID;
    BYTE *pCerData;
    BYTE *pSign;
    WORD SignLen;
    WORD CerTxtLen;
    if(FrameNO == 1)  //第一帧 
    {
        g_wEncCerWPtr = 0;   
    }
//    if(FrameNO == 2) 
//    {
//        FrameNO = 2;
//    }      
//    if(FrameNO == 3) 
//    {
//        FrameNO = 3;
//    }       
    for(WORD i = 0; i < DataLen; i++)
    {
        g_bEncCerTxt[g_wEncCerWPtr++] = pData[i];
    }
    if(FrameNO >= FrameNum) //最后一帧
    {
        //解密出来的数据前面预留3字节,
        g_wEncCerWPtr  = DecryptData(g_bEncCerTxt, g_wEncCerWPtr, &g_bEncCerTxt[3]);   //解密数据
        if(!g_wEncCerWPtr)
        {
            SendEncFrameAck(0x9790,0x0000,0x71);
            return TRUE;
        }
        CerID = g_bEncCerTxt[3];
        pCerData = &g_bEncCerTxt[3];
        CerTxtLen = g_wEncCerWPtr - 71;  //减去71为1字节密钥标识，64字节签名，6字节时间信息
        pSign = &pCerData[CerTxtLen];
        SignLen = 70;  //64字节签名+ 6字节时间信息
        KeyID = pSign[SignLen];  //解密数据出来的最后一个字节是密钥标识
        if(m_bHostCertifyFlag != 0x55)
        {
            SendEncFrameAck(0x9790,0x0000,0x71);
            return TRUE;          
        }
        if(!VerifyDataSign(KeyID,pCerData,CerTxtLen,pSign,SignLen))  //验签失败,这里数据长度-1是因为把KeyID提取出来了
        {
            SendEncFrameAck(0x9790,0x0000,0x71);
            return TRUE;
        }
        //解密出来的Cer正文从第5 字节开始，第4 字节为CerID, 前面还预留了3 个字节
        if(UpdataCer(CerID,g_bEncCerTxt,CerTxtLen + 3) == FALSE)  
        {
            
            SendEncFrameAck(0x9790,0x0000,0x71);//证书更新失败
            return TRUE;
        }
        if(m_bEncObject == GATEWAY)
        {
            SendEncFrameAck(0x0090,0x0080,0x71);
        }
        else if(m_bEncObject == MASTER_STATION)
        {
            SendEncFrameAck(0x0090,0x0000,0x71);
        }
    }
    return TRUE;
}

BOOL CBJ101S::SendCerDownloadAck(void)  //证书下载
{
    BYTE FrameNum = pReceiveFrame->FrameEB.Data[3];
    BYTE FrameNO = pReceiveFrame->FrameEB.Data[4];
    BYTE *pData = &pReceiveFrame->FrameEB.Data[5];
    //WORD DataLen = MAKEDWORD(pReceiveFrame->FrameEB.Data[1], pReceiveFrame->FrameEB.Data[0]);
    WORD DataLen = MAKEWORD(pReceiveFrame->FrameEB.Data[1], pReceiveFrame->FrameEB.Data[0]) - 3;
  
    if(FrameNO == 1)  //第一帧 
    {
        g_wEncCerWPtr = 2;   //预留2字节给长度
    }
    if(FrameNO == 2)
    {
        FrameNO = 2;
    }
    for(WORD i = 0; i < DataLen; i++)
    {
        g_bEncCerTxt[g_wEncCerWPtr++] = pData[i];
    }
    if(FrameNO >= FrameNum) //最后一帧
    {
        if(m_bHostCertifyFlag != 0x55)
        {
            SendEncFrameAck(0x9790,0x0000,0x73);  //未通过认证
            return TRUE;
        }
        if(LoadCer(g_bEncCerTxt, g_wEncCerWPtr))   
        {
            SendEncFrameAck(0x0090,0x0000,0x73);  
            return TRUE;
        }
        else
        {
            SendEncFrameAck(0x9390,0x0000,0x73);//证书导入失败
            return TRUE;
        }
    }
    return TRUE;
}
BOOL CBJ101S::SendCer(BYTE Type)  //读取芯片证书
{
    //BYTE RecData[1024] = {0};
    BYTE FrameInfo[5];
    WORD Cerlen;
    WORD pWrite = 0;
    BYTE FrameTotal;
    BYTE FrameNum = 1;
    const BYTE MaxSendByte = 240;
    Cerlen = ReadRealCer(g_bEncCerTxt);
    if(Cerlen)
    {
        FrameTotal = Cerlen/MaxSendByte;
        if(Cerlen % MaxSendByte)
        {
            FrameTotal++;
        }       
        while(pWrite < Cerlen)
        {
            if(pWrite + MaxSendByte < Cerlen)
            {
                if(Type == 0)   //主站通信
                {
                    if(m_bHostCertifyFlag != 0x55)
                    {
                        SendEncFrameAck(0x9590,0x0000,0x75);//证书导出失败
                        return TRUE;
                    }                  
                    SendFrameEBHead(0x0000,0x75);
                }
                else   //运维工具通信
                {
                    SendFrameEBHead(0x0040,0x43);
                }
                FrameInfo[0] = 0;
                FrameInfo[1] = MaxSendByte + 3;
                FrameInfo[2] = 0x06;
                FrameInfo[3] = FrameTotal;
                FrameInfo[4] = FrameNum++;
                SendEncData(5,FrameInfo);
                SendEncData(MaxSendByte,&g_bEncCerTxt[pWrite]);
                SendFrameEBTail();
                pWrite += MaxSendByte;
                delayms(500);
            }
            else
            {
                if(Type == 0)   //主站通信
                {                    
                    SendFrameEBHead(0x0000,0x75);
                }
                else   //运维工具通信
                {
                    SendFrameEBHead(0x0040,0x43);
                }
                FrameInfo[0] = 0;
                FrameInfo[1] = 3 + Cerlen - pWrite; 
                FrameInfo[2] = 0x06;
                FrameInfo[3] = FrameTotal;
                FrameInfo[4] = FrameNum;;
                SendEncData(5,FrameInfo);
                SendEncData((Cerlen - pWrite),&g_bEncCerTxt[pWrite]);
                SendFrameEBTail();
                pWrite = Cerlen;
                //delayms(5000);
            }
        }
        return TRUE;
    }
  if(Type == 0)
    {
        SendEncFrameAck(0x9790, 0x0000,0x75);
  }
  else
    {
    SendEncFrameAck(0x9790, 0x0040,0x43);
  }
    
    return FALSE;
}


BOOL CBJ101S::SendToolVerifyAck(void)
{
    BYTE FrameNum = pReceiveFrame->FrameEB.Data[3];
    BYTE FrameNO = pReceiveFrame->FrameEB.Data[4];
    BYTE *pData = &pReceiveFrame->FrameEB.Data[5];
    //减去证书标识1字节，总帧数1字节，当前帧数1字节，证书管理工具ID8 字节
    WORD DataLen = MAKEWORD(pReceiveFrame->FrameEB.Data[1], pReceiveFrame->FrameEB.Data[0]) - 11;  
    if(FrameNO == 1)  //第一帧
    {
        g_wEncCerWPtr = 2;  //预留前2字节给长度
    }
    for(WORD i = 0; i < DataLen; i++)
    {
        g_bEncCerTxt[g_wEncCerWPtr++] = pData[i];
    }
    for(BYTE i = 0; i < 8; i++)   //证书管理工具保存
    {
        m_bCerToolID[i] = pData[DataLen + i];
    }
    if(FrameNO >= FrameNum) //最后一帧
    {
        //g_bEncCerTxt[0] = HIBYTE(g_wEncCerWPtr - 2);
        //g_bEncCerTxt[1] = LOBYTE(g_wEncCerWPtr - 2);
        if(!VerifyToolCer(g_bEncCerTxt, g_wEncCerWPtr-2)) 
        {
            m_bToolCertifyFlag = 0;
            SendEncFrameAck(0x9090,0x0040,0x31);//管理证书验证失败
            return TRUE;
        }
        if(GetChipRandom(m_bUnitRandom))
        {
            SendFrameEBHead(0x0040,0x31);
            SendFrameEBEixampleData(8,m_bUnitRandom);
            SendFrameEBTail();
            m_bToolCertifyFlag = 0x55;
            return TRUE;
        }
        else
        {
            m_bToolCertifyFlag = 0;
            SendEncFrameAck(0x9090,0x0040,0x31);//随机数未获取
            return TRUE;
        }
    }
    SendEncFrameAck(0x0090,0x0040,0x45);
    return TRUE;
}
BOOL CBJ101S::VerifyToolSign(void)
{
    BYTE *pSign;
    //BYTE RecData[2];
    pSign = &pReceiveFrame->FrameEB.Data[2];
    if(VerifyToolSignData(pSign, 0x40)) 
    {
        SendEncFrameAck(0x0090,0x0040,0x33);
//        SendFrameEBHead(0x0042,0x33);
//        RecData[0] = 0x90;
//        RecData[1] = 0x00;
//        SendFrameEBEixampleData(0x02,RecData);
//        SendFrameEBTail();  
//        return TRUE;        
    }
    else
    {
        m_bToolCertifyFlag = 0;
        SendEncFrameAck(0x9090,0x0040,0x33);
    }
    return TRUE;
}
BOOL CBJ101S::SendUnitID(void)
{
    //BYTE UnitID[24];
//    for(BYTE i = 1; i <= 24; i++)
//    {
//        g_bUnitID[i] = i;
//    }
    SendFrameEBHead(0x0040,0x37);
    SendFrameEBEixampleData(24,(BYTE*)&g_bUnitID[1]);
    SendFrameEBTail();  
    return TRUE;       
}
BOOL CBJ101S::SendEncPubKey(void)
{
    BYTE PubKey[256] = {0};
    WORD EncDataLen = 0;
    //预留34个字节，2字节长度，8字节管理工具ID，8字节R1，16字节IVDATA
    //多预留3个字节 2字节扩展区长度，1字节报文类型
    if(!ReadPubKey(&PubKey[37])) 
    {
        //SendEncFrameAck(0x0991,0x0040,0x3B);
        return TRUE;
        //获取公钥失败
    }
//    {
//        SendFrameEBHead(0x0040,0x3B);   //AppType大于200
//        SendEncData(64,&PubKey[37]);
//        SendFrameEBTail();
//    }
    
    PubKey[34] = 0x3B;  //应用类型
    PubKey[35] = 0;
    PubKey[36] = 0x40;   //数据长度
    EncDataLen = EncryptToolData(PubKey,67,PubKey);
    if(EncDataLen)
    {
        SendFrameEBHead(0x0048,201);   //AppType大于200
        SendEncData(EncDataLen,PubKey);
        SendFrameEBTail();
    }
    else
    {
        SendEncFrameAck(0x0991,0x0040,0x3B);
    }
    return TRUE;
}
BOOL CBJ101S::SendSignCerTxt(void)
{
    BYTE SrReqData[64];
    BYTE FrameNum = pReceiveFrame->FrameEB.Data[2];
    BYTE FrameNO = pReceiveFrame->FrameEB.Data[3];  
    BYTE *pData = &pReceiveFrame->FrameEB.Data[4];
    WORD DataLen = 0;
    //WORD ClearDataLen;
    if(FrameNO == 1)  //第一帧
    {
        g_wEncCerWPtr = 2;  //预留前2字节给长度
    }
    DataLen = MAKEWORD(pReceiveFrame->FrameEB.Data[1],pReceiveFrame->FrameEB.Data[0]) - 2;
    for(WORD i = 0; i < DataLen; i++)
    {
        g_bEncCerTxt[g_wEncCerWPtr++] = pData[i];
    }
    if(FrameNO >= FrameNum) //最后一帧
    {
        if(SignData(g_bEncCerTxt,g_wEncCerWPtr,SrReqData))
        {
            SendFrameEBHead(0x0040,0x3D);
            SendFrameEBEixampleData(64,SrReqData);
            SendFrameEBTail();
            return TRUE;
        }
        else
        {
            return TRUE;
        }
    }
    SendEncFrameAck(0x0090,0x40,0x45);  //返回分帧接成功
    return TRUE;
}
BOOL CBJ101S::SendLoadCerAck(void)
{
    BYTE CerID = pReceiveFrame->FrameEB.Data[2];
    BYTE FrameNum = pReceiveFrame->FrameEB.Data[3];
    BYTE FrameNO = pReceiveFrame->FrameEB.Data[4];
    BYTE *pData = &pReceiveFrame->FrameEB.Data[5];
    WORD DataLen;
    if(FrameNO == 1)  //第一帧
    {
        g_wEncCerWPtr = 4;   //留2字节CerID，2字节长度
    }
    DataLen = MAKEWORD(pReceiveFrame->FrameEB.Data[1],pReceiveFrame->FrameEB.Data[0]) - 3;
    for(WORD i = 0; i < DataLen; i++)
    {
        g_bEncCerTxt[g_wEncCerWPtr++] = pData[i];
    }
    if(FrameNO >= FrameNum)
    {
        if(UpdataCer(CerID, g_bEncCerTxt, g_wEncCerWPtr))
        {
            SendEncFrameAck(0x0090,0x0040,0x3F);
        }
        else
        {
            SendEncFrameAck(0x9790,0x0040,0x3F);
        }
        return TRUE;
    }
    SendEncFrameAck(0x0090,0x0040,0x45);  //分帧接收成功
    return TRUE;
}
BOOL CBJ101S::SendReWriteCerAck(void)
{
    BYTE RecData[2];
    WORD CerLen;
    if(!ReadCerLen((BYTE *)RecData))
    {
        SendEncFrameAck(0x9390,0x0040,0x1f);
        return TRUE;
    }
    CerLen = MAKEWORD(RecData[1],RecData[0]);
    //从第二 个字节开始存是因问预留2字节长度
    if(!ReadTestCer(RecData, &g_bEncCerTxt[2])) 
    {
        SendEncFrameAck(0x9390,0x0040,0x41);
        return TRUE;
    }
    if(LoadCer(g_bEncCerTxt,CerLen + 2))  //装载原始证书成功
    {
        SendEncFrameAck(0x0090,0x0040,0x41);
        return TRUE;        
    }
    SendEncFrameAck(0x9390,0x0040,0x41);
    return TRUE;
}


BOOL CBJ101S::SendToolRecovKey(void)
{
    BYTE KeyData[256];
    BYTE DataLen = pReceiveFrame->FrameEB.Data[1];  //该命令字节数不会超过256
    memcpy(&KeyData[2], &pReceiveFrame->FrameEB.Data[2],DataLen);
    if(m_bToolCertifyFlag != 0x55)
    {
        SendEncFrameAck(0x9290,0x0040,0x47);
        return TRUE;
    }
    if(RecovToolKey(KeyData,DataLen))
    {
        SendEncFrameAck(0x0090,0x0040,0x47);
    }
    else
    {
        SendEncFrameAck(0x9290,0x0040,0x47);
    }
    return TRUE;
}

BOOL CBJ101S::RecClearTxt(void)
{
    //    if(pReceiveFrame == NULL)
//    {
//        return false;
//    }
    BYTE AppType = pReceiveFrame->FrameEB.AppType;
    g_bAppType = AppType;
    BYTE AppLen = 0;     //APP应用数据长度
    BYTE *pAppData;
    BYTE *pEiData;
    WORD FrameLen = MAKEWORD(pReceiveFrame->FrameEB.LengthL,pReceiveFrame->FrameEB.LengthH);
    WORD EiDataLen = 0;  //扩展区数据长度
    BYTE asKey;
    BYTE Random[8];
    BYTE EncTime[6];
    BYTE Consel_MD5[16] = {0};
    //BYTE CheckTimeFlag = 0;
    //BYTE CheckTimeCount = 0;
    //DWORD TimeStamp;  //时间戳，用于对比时间
    memcpy(Random,m_bUnitRandom,8);
    EncTime[0] = (BYTE)((g_sRtcManager.m_gRealTimer[RTC_YEAR]-2000) & 0xFF);
    EncTime[1] = (BYTE)(g_sRtcManager.m_gRealTimer[RTC_MONTH] & 0xFF);
    EncTime[2] = (BYTE)(g_sRtcManager.m_gRealTimer[RTC_DATE] & 0x1F);
    EncTime[3] = (BYTE)(g_sRtcManager.m_gRealTimer[RTC_HOUR] & 0xFF);
    EncTime[4] = (BYTE)(g_sRtcManager.m_gRealTimer[RTC_MINUT] & 0xFF);
    EncTime[5] = (BYTE)(g_sRtcManager.m_gRealTimer[RTC_SEC] & 0xFF);
    //写入当前时间
    //WORD DataSignlen = 0;
    if(AppType <= 0x08)
    {
        //还未进行过主站认证就收到报文
        if(m_bHostCertifyFlag != 0x55)
        {
            SendEncFrameAck(0x0791, 0x0000,0x1f);
            return TRUE;
        }
        //收到未加密的68 帧
        if((pReceiveFrame->FrameEB.Data[1] == 0x68) && (m_bEncDataFlag != 0x55))
        {
            SendEncFrameAck(0x0691, 0x0000,0x1f);
            return TRUE;
        }
    }
    m_bEncDataFlag = 0; //数据加密标志位清零
    switch(AppType)
    {
    case 0x00:
    case 0x02:
    case 0x04:
    case 0x06:    
        if(pReceiveFrame->FrameEB.Data[1] == 0x68)
        {
            pReceiveFrame = (VIec101Frame*)&pReceiveFrame->FrameEB.Data[1];
            RecFrame68();           
        }
        else if(pReceiveFrame->FrameEB.Data[1] == 0x10)
        {
            pReceiveFrame = (VIec101Frame*)&pReceiveFrame->FrameEB.Data[1];
            RecFrame10();
        }
        
        break;
    //数据带签名，需要验签
    case 0x01:
    case 0x03:
    case 0x05:
    case 0x07:    //zx修改 20170503
    case 0x08:
        AppLen = pReceiveFrame->FrameEB.Data[0];
        if(AppLen)  //AppType 为01 03 05 07
        {
            pAppData = &pReceiveFrame->FrameEB.Data[1];
            EiDataLen = MAKEWORD(pAppData[AppLen + 1],pAppData[AppLen]);
            pEiData  = &pAppData[AppLen + 2];
        }
        else if(AppType == 0x08)       //AppType 为08
        {
            pAppData = NULL;
            EiDataLen = MAKEWORD(pReceiveFrame->FrameEB.Data[1],pReceiveFrame->FrameEB.Data[0]);
            pEiData  = &pReceiveFrame->FrameEB.Data[2];
        }
        else
        {
            SendEncFrameAck(0x0191,0x0000,0x1f);
            return TRUE;        
        }
        //memcpy(&pReceiveFrame->FrameEB.Data[AppLen + 1],&pReceiveFrame->FrameEB.Data[AppLen + 3],EiDataLen);
        //校验数据长度是否正确
        if((FrameLen < (AppLen + EiDataLen + 6))
           ||(EiDataLen < 65) 
           ||((AppType == 0x03) && (EiDataLen < 73))
           ||((AppType == 0x05) && (EiDataLen < 71))
           ||((AppType == 0x07) && (EiDataLen < 79)))
           //||((AppType == 0x08) && (EiDataLen < 79)))
          
        {
            SendEncFrameAck(0x0191,0x0000,0x1f);
            return TRUE;
        }
        else if((AppType == 0x08) && (EiDataLen < 79))
        {
            m_wUpdataFaultFlag = 0x0191;
            return true;
        }
        if(AppType == 0x03) 
        {
            memcpy(Random,pEiData,8);
        } 
        else if(AppType == 0x05)
        {
            memcpy(EncTime,pEiData,6);
        }
        else if(AppType == 0x07)
        {
            memcpy(EncTime,pEiData,6);
            memcpy(Random,&pEiData[6],8);
        }
        else if(AppType == 0x08)
        {
            memcpy(EncTime,pEiData,6);
            memcpy(Random,&pEiData[6],8);           
        }
//#ifdef DEBUGENC
        for(BYTE i = 0; i < 8; i++)//验证随机数
        {
            if(Random[i] != m_bUnitRandom[i])
            {
                if(AppType == 0x08)
                {
                    m_wUpdataFaultFlag = 0x0491;
                }
                else
                {
                    SendEncFrameAck(0x0491,0x0000,0x1f);
                }
                return TRUE;
            }
        }
        if(EncTime[0] != (BYTE)((g_sRtcManager.m_gRealTimer[RTC_YEAR]-2000) & 0xFF))  //检查年
        {
            if(AppType == 0x08)
            {
                m_wUpdataFaultFlag = 0x0591;
            }
            else
            {
                SendEncFrameAck(0x0591,0x0000,0x1f);
            }
            return TRUE;
        }
        if(EncTime[1] != (BYTE)(g_sRtcManager.m_gRealTimer[RTC_MONTH] & 0xFF))   //检查月
        {
            if(AppType == 0x08)
            {
                m_wUpdataFaultFlag = 0x0591;
            }
            else
            {
                SendEncFrameAck(0x0591,0x0000,0x1f);
            }
            
            return TRUE;
        }
        if(EncTime[2] != (BYTE)(g_sRtcManager.m_gRealTimer[RTC_DATE] & 0x1F))  //检查日
        {
            if(AppType == 0x08)
            {
                m_wUpdataFaultFlag = 0x0591;
            }
            else
            {
                SendEncFrameAck(0x0591,0x0000,0x1f);  
            }
            
            return TRUE;
        }   
        asKey = pEiData[EiDataLen - 1];
        //DataSignlen = EiDataLen + (WORD)AppLen;
        if(AppType == 0x08)
        {
            if(g_wEncCerWPtr)
            {
                MD5(g_bEncCerTxt,g_wEncCerWPtr,Consel_MD5);  
                pAppData = Consel_MD5;
                AppLen   = 16;
            }
        }
        if(VerifyDataSign(asKey,pAppData,AppLen,pEiData,EiDataLen - 1)) //验签成功  ZX修改20170503
        {
            if(AppType == 0x08)
            {
                m_wUpdataFaultFlag = 0;
                return true;
            }
            else if(pReceiveFrame->FrameEB.Data[1] == 0x68)
            {
                pReceiveFrame = (VIec101Frame*)&pReceiveFrame->FrameEB.Data[1];
                RecFrame68(); 
                return true;
            }            
        }
        else
        {
            if(AppType == 0x08)
            {
                m_wUpdataFaultFlag = 0x0291;
            }
            else
            {
                SendEncFrameAck(0x0291,0x0000,0x1f);
            }            
        }
        break;
    case 0x20:   
        SendGatewayVerify();
        break;
    case 0x22:
        SendGatewayVerifyAck();
        break;
    case 0x50:
        SendHostVerify();   
        break;
    case 0x52:
        SendHostVerifyAck();
        break;
    case 0x54:    //主站获取终端芯片序列号
        SendEncChipID(0);
        break;
    case 0x60:
        SendChipKeyVer(0);
        break;
    case 0x62:
        SendUpdataKeyAck();
        break;
    case 0x64:
        SendRecovKeyAck();
        break;
    case 0x70:
        SendCerUpdataAck();
        break;
    case 0x72:
        SendCerDownloadAck();
        break;
    case 0x74:
        SendCer(0);
        break;
    case 0x30:  //运维工具身份认证
        SendToolVerifyAck();  
        break;
    case 0x32:
        VerifyToolSign();
        break;
    case 0x34:
        SendChipKeyVer(1);
        break;
    case 0x36:
        SendUnitID();
        break;
    case 0x38:
        SendEncChipID(1);
        break;
    case 0x3A:
        SendEncPubKey();
        break;
    case 0x3C:
        SendSignCerTxt();
        break;
    case 0x3E:
        SendLoadCerAck();
        break;
    case 0x40:
        SendReWriteCerAck();
        break;
     case 0x42:
        SendCer(1);
        break;
    case 0x46:
        SendToolRecovKey();
    }
    return TRUE;
}
BOOL CBJ101S::RecFrameEB(WORD ProcBytes)
{
     //BYTE data[1024] = {0};
    WORD FrameLen = 0;
    //for(WORD i = 0; i < ProcBytes; i++)
    //{
    //    data[i] = g_EncRxBuf.bBuf[(g_EncRxBuf.wCheckFrame + i) & 0x3FF];
    //}
    //pReceiveFrame = (VIec101Frame*)data;
    m_bEncObject = pReceiveFrame->FrameEB.FrameType[1] & CONNECT_OBJECT_BIT;
    m_bSymmetricKeyFlag = pReceiveFrame->FrameEB.FrameType[1] & m_bSymmetricKeyFlag;
    FrameLen = MAKEWORD(pReceiveFrame->FrameEB.LengthL,pReceiveFrame->FrameEB.LengthH);
//#ifdef DEBUGENC
    //m_bHostCertifyFlag = 0x55;  //测试使用
//#endif
    if((pReceiveFrame->FrameEB.FrameType[1] & ENC_TURN_BIT) == ENC_ON)   //该帧报文加密
    {
//        for(BYTE i = 0; i < 4; i++)
//        {
//            m_bHostMac[i] = pReceiveFrame->FrameEB.Data[pReceiveFrame->FrameEB.Data[APPDATA_LEN] + 2 + i];
//        }
        //解密报文长度为报文长度- 2 个字节报文类型
        if((pReceiveFrame->FrameEB.FrameType[1] & CONNECT_OBJECT_BIT) == SET_TOOL)  //运维工具 
            DecryptToolData(&pReceiveFrame->FrameEB.AppType, FrameLen - 2, &pReceiveFrame->FrameEB.AppType);
        else
        {
            m_bEncDataFlag = 0x55;
 
            if(RecCipherTxt(&pReceiveFrame->FrameEB.AppType, FrameLen - 2, &pReceiveFrame->FrameEB.AppType) == FALSE)
            {
                SendEncFrameAck(0x0391,0x0000,0x1f);
                return TRUE;
            }
      }
        //RecCipherTxt();   //收到密文数据}
    }
    //else
    {
        RecClearTxt();   //收到到明文数据
    }  
    return TRUE;
}

WORD CBJ101S::SearchEncFrame(void)
{
    //DWORD Rc;
    /*WORD FrameLen;
    WORD MsgLen;
    //BYTE* pCheckFrame;
    //WORD  m_pCheckFrame;
    //while (1)
    {
        if(g_EncRxBuf.wBufT >= g_EncRxBuf.wBufH)  //尾指针大于头指针
        {
            MsgLen = g_EncRxBuf.wBufT - g_EncRxBuf.wBufH;
        }
        else
        {
            MsgLen = g_EncRxBuf.wBufH - g_EncRxBuf.wBufT + 1024;
        }
    
        if(MsgLen <= 10)
        {
            g_EncRxBuf.wBufH += MsgLen;
            g_EncRxBuf.wBufH &= 0x3FF;
            return 0;
        }
        //pCheckByte  = g_EncRxBuf.bBuf[g_EncRxBuf.wBufH];
        g_EncRxBuf.wCheckFrame = g_EncRxBuf.wBufH;
        while(MsgLen)
        {
            //switch(*pCheckFrame)
            
            if(g_EncRxBuf.bBuf[g_EncRxBuf.wCheckFrame] != 0xEB)
            {
                g_EncRxBuf.wCheckFrame++;
                g_EncRxBuf.wCheckFrame &= 0x3FF;    
                g_EncRxBuf.wBufH = g_EncRxBuf.wCheckFrame;
                MsgLen--;
                continue;
            }
            FrameLen = GET_ENC_FRAME_LEN(g_EncRxBuf.wCheckFrame,g_EncRxBuf.bBuf) + 6;
            if(FrameLen > MsgLen)   //超时未接收全所有帧，整帧丢弃
            {
                g_EncRxBuf.wCheckFrame += MsgLen;
                g_EncRxBuf.wCheckFrame &= 0x3FF;    //与上0x3FF都是为了防止指针越界
                g_EncRxBuf.wBufH = g_EncRxBuf.wCheckFrame;      
                MsgLen = 0;
                continue;
           }
            if(GET_ENC_FRAME_TAIL(g_EncRxBuf.wCheckFrame,g_EncRxBuf.bBuf,FrameLen)!= 0xD7)
            {
                g_EncRxBuf.wCheckFrame++;
                g_EncRxBuf.wCheckFrame &= 0x3FF;    
                g_EncRxBuf.wBufH = g_EncRxBuf.wCheckFrame;
                MsgLen--;
                continue;                
            }
            g_EncRxBuf.wBufH += FrameLen;
            g_EncRxBuf.wBufH &= 0x3FF;
            return FrameLen;
        }            
    }*/
    return 0;
}

BOOL CBJ101S::RcvEncData(void)
{
    /*WORD FrameLen = 0;
    if(g_sRxBuff[m_uartId].m_iRcvCount == 0)
    {
        FrameLen = SearchEncFrame();
    }
    if(FrameLen <= 0)
    {
        return FALSE;
    }
    if(g_EncRxBuf.bBuf[g_EncRxBuf.wCheckFrame] == 0xEB)
    {
        RecFrameEB(FrameLen);
    }*/
    return TRUE;
}


#endif /*#ifdef INCLUDE_GB101_S*/
