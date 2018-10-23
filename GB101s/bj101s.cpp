/*------------------------------------------------------------------------
 Module:            bj101s.cpp
 Author:            linxueqin
 Project:           ��Լƽ̨
 State:             V1.0
 Creation Date:     2014-4-24
 Description:       GB101��Լ--slave
------------------------------------------------------------------------*/

#include "Bj101s.h"
#include "..\Main.h"

#ifdef INCLUDE_BJ101_S

#undef BJ101_GZTEST

#define GRP_YCNUM   50//128         //ÿ����෢��ң����������Ӽ��ܣ�72��Ϊ50
#define GRP_YXNUM   127//128         //ÿ����෢��ң���ֽ�

#define FRM_YCNUM    72//64         //ÿ֡������෢��ң�����
#define FRM_YXNUM    127//100        //ÿ֡������෢��ң�Ÿ���


/***************************************************************
    Function��CBJ101S
        ���캯�����ݿ�
    ��������

    ���أ���
***************************************************************/
CBJ101S::CBJ101S() : CPrtcSms()//
{
  m_YkWaitCount=0;//ң��ִ�������rf��ȴ���ʱ�� ��λs
  m_LuboRe10Flag= 0;
  time_delay_set = 0;
}


void CBJ101S::Run(void)
{
   // if(!m_SendChgYcFlag && g_SendYc )//&& (g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_SEND_CHANGEYC])
     // CheckChangeYC();//��Ҫ����OpenBatSmsGprs������ǰ��
  /*  if(m_uartId == g_CmIdGPRS) 
      OpenBatSmsGprs();//�ŵ�run��������ǰ��*/
    RcvData(&m_uartId);
    DoCommSendIdle();


}
/***************************************************************
    Function��Init
        ��Լ��ʼ��
    ������
    ���أ�TRUE �ɹ���FALSE ʧ��
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
	CAT_SpiReadWords(EEPADD_SOESEND_E2ROMADR, 4, untemp);  //���浽EEPROM��
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
    //��Ӧg_yxChangeflag 
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
       m_initflag=0;//7 modi lxq ��վ�·�������·״̬��Ž��˱�������Ϊ7
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



void CBJ101S::initpara(BYTE flag)//��������ʱ�ĳ�ʼ�������ò�����ĳ�ʼ��
{
    {
      m_guiyuepara.linkaddrlen= g_ucPara101[IECP_LINKADDR_NUM];//2;//��·��ַ����
      m_guiyuepara.typeidlen=1;
      m_guiyuepara.conaddrlen=g_ucPara101[IECP_COMNADDR_NUM];//2;//������ַ����
      m_guiyuepara.VSQlen=1;
      m_guiyuepara.COTlen=g_ucPara101[IECP_TRANSRSN_NUM];//2;//����ԭ�򳤶�
      m_guiyuepara.infoaddlen=g_ucPara101[IECP_INFO_NUM];//2;//��Ϣ���ַ����
      m_guiyuepara.baseyear=2000;
      m_guiyuepara.mode=g_ucPara101[IECP_COM_MODE];//1=ƽ��ʽ 0=��ƽ��
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
    Function��DoReceive
        ���ձ��Ĵ���
    ��������

    ���أ�TRUE �ɹ���FALSE ʧ��
***************************************************************/
  BOOL CBJ101S::DoReceive()
  {
     while(1)
     {
        if (SearchFrame() != TRUE)
        return FALSE;
  
        // �������
        if(m_guiyuepara.mode==1)
        {
#ifdef YN_101S		
        	m_PRM =0;  //����
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
        if (pReceiveFrame->FrameAA.Start[0] == 0xAA)//�ŏ| 0404 ���Ž���
        {

          RecFrameAA(&pReceiveFrame->FrameAA);
          continue;
        }  //�ŏ| 0404 ���Ž���
         if(pReceiveFrame->FrameEB.Start1 == 0xEB)   //���ܱ��Ľ���
        {
            BYTE FrameLen = MAKEWORD(pReceiveFrame->FrameEB.LengthL,pReceiveFrame->FrameEB.LengthH);
            RecFrameEB(FrameLen);//RecFrameEB();
            continue;
        }         
     }
  }


//�̶�֡��ʽ���Ľ��մ���
BOOL CBJ101S::RecFrame10(void)
{
	g_NolinkReset = 0;
	 //if(this == pDbg)  g_NolinkWifi= 0;
    switch(pReceiveFrame->Frame10.Control & BITS_CODE)
    {
          case 0x4A: //�ٻ�һ���û�����
          case 0x4B: //Զ����·״̬��û��ٻ������û�����
              if (pReceiveFrame->Frame10.Control == m_bReceiveControl && m_SendBuf.wWritePtr > 4)//�ط�
              {
                m_retxdnum++;
                if(m_retxdnum<MAX_FRM_RESEND)
                        return SendRetry();//lqh ������
                m_retxdnum=0;
                return FALSE;
              }
              break;
          default:
              break;
    }
    //dir bit auto fit
    if(pReceiveFrame->Frame10.Control & BITS_DIR)
      m_DIR=1;/*get��վdir*/
    else m_DIR=0;
    m_bReceiveControl = pReceiveFrame->Frame10.Control;//���浱ǰ״̬

    switch(pReceiveFrame->Frame10.Control & BITS_CODE)
    {
        case 0x42: /*��������(ƽ��)*/
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
      if((gRecorder_flag.LIST_flag == ON) &&(m_uartId == g_Cmid))//(( gRecorder_flag.CFG_flag ==ON )||(gRecorder_flag.DAT_flag ==ON )||)//���ڶ������ļ�������
            {
              m_ackflag =1;
            }
      if((gRecorder_flag.LIST_flag_YN== ON) &&(m_uartId == g_Cmid))//(( gRecorder_flag.CFG_flag ==ON )||(gRecorder_flag.DAT_flag ==ON )||)//���ڶ������ļ�������
            {
              m_ackflag =1;
            }			
            if(((pSendFrame->Frame68.Start1 == 0x68)||(pSendFrame->Frame68.Start1 == 0xEB)) && (m_WaitConfTpId > 0))//�����ж���վ�صı����ǲ��Ƕ������ϱ����ĵ�ȷ��
            {
                #ifndef GETSOEFROMRAM
                  if(m_WaitConfTpId == M_SP_TB)
                  {//��E2�б���flash�е�SOEͷָ��
                    if(m_uartId == g_CmIdDBG)
                      CAT_SpiWriteWords(EEPADD_SOESEND_E2ROMADR, 1, &m_SSoeHeadPtr);
                    else if(m_uartId == g_CmIdGPRS)
                      CAT_SpiWriteWords(EEPADD_SOESEND_E2ROMADR+4, 1, &m_SSoeHeadPtr);
                  }
                #endif
                
                m_WaitConfTpId = 0;
                m_ucCosBkNum = 0;//�ϴ���COS�õ���վȷ�Ϻ󱸷ݸ�������
                m_SSoeHeadPtrBk = m_SSoeHeadPtr;
                m_DSoeHeadPtrBk = m_DSoeHeadPtr;
		return TRUE; //�յ���վ��ȷ��֡(������λ��·��������·״̬��)		
            }
            return TRUE; //�յ���վ��ȷ��֡(������λ��·��������·״̬��)
        case 0x40:
            RecResetLink();
            return TRUE; //��λ��·
        case 0x49:
#ifdef CQ_101S	//����101 ��֤����		
	     if((g_RenZLink&0x01) == 1)
              {
            RecReqLink();
	     	}
	     else
	     	{
	     	m_dwasdu.LinkAddr=GetOwnAddr();
              SendBaseFrame(1, 0x07);//��֤ 10 c7 xx xx xx 16
	     	}
		 g_RenZLink++;
#else
            RecReqLink();
	mRecorder_flag.LIST_flag = OFF;
      mRecorder_flag.xuchuanflag= OFF;
#endif
            return TRUE; //������·״̬
        case 0x4A: 
            RecCallClass1();
            return TRUE; //�ٻ�һ���û�����
        case 0x4B://�����û�����
            RecCallClass2(); 
            return TRUE; 
        
        case 0x03:/*ACK �Ĵ��� */
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

//�ɱ�֡����ʽ���Ľ��մ���
BOOL CBJ101S::RecFrame68(void)
{
    //dir bit auto fit
    //unsigned char uartNum = 0;
    WORD addr = GetOwnAddr();
    
   if((pReceiveFrame->Frame68.Control == 0x05)&&(pReceiveFrame->Frame68.Length1 == 0x30))
    {//��GPRS״̬����֡
        if(1)       //����У�� 
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
      m_DIR=1;/*get��վdir*/
    else m_DIR=0;

    m_bReceiveControl = pReceiveFrame->Frame68.Control;
    m_acdflag=1;
    m_recfalg=1;
    if(((m_bReceiveControl&0xf)!=4)&&(SwitchToAddress(m_dwasdu.LinkAddr))&&(m_linkflag))//������·��ʼ����ɺ�����Ӧ��֡ȷ��
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
            break;//ң��
        case 0x64:
            if(g_bAppType)
            {
                SendEncFrameAck(0x0191, 0x0000, 0x1F);
                return TRUE;
            }			
            RecCallSomeGroup();
            break;//���ٻ�/�ٻ�ĳһ������
        case 0x65:
            //RecCallDDCommand();
            break;//�ٻ����
        case 0x66:
            //RecReadData();
            m_zdflag=0;
            break;//�����ݴ���
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
            break;//ʱ��ͬ��
        case 0x68:
            RecTestLink();
            break; //������·
        case 0x69:
            if(g_bAppType)
            {
                SendEncFrameAck(0x0191, 0x0000, 0x1F);
                return TRUE;
            }  			
            RecResetRTU();
            break; //��λRTU
        case 0x6A:
            if(m_dwasdu.COT==6)
              RecDelaytime();
            else if(m_dwasdu.COT==3)
              RecDelaytrans();
            break; 
        case 0x6E:
            //RecSetPara();
            break;//���ò���
        //case 0xD2:
            //m_SendFixNum =0;   
            //Recfileprocess();
            //Recfileprocessing();
            //break;
        case 0xD3://Զ������
            //FileRemoteupdata(&pReceiveFrame->Frame68.Start1);
            break;           
        case 0x79:    
        case 0x7a://���ļ�
        case 0x7c:
		case 0x84://��ɢ������	
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
        
        mRecorder_flag.LIST_flag = OFF;//�յ�����ȷ����Ҫ������ʱ����λ�ı�־λ
                mRecorder_flag.xuchuanflag= OFF;
                m_lubo_num =0; 
        m_fixpt_num =0;
        m_SendListNum =0;
        m_SendFixNum =0;  
        BK_FRecorder_Current_COUNT = g_FRecorder_Current_COUNT;
           
        Recfileprocessing(&pReceiveFrame->Frame68.Start1);
        break;//?		
        case 0x88://136 ����¼��Э��  ��Ŀ¼
        case 0x89://137 ����¼��Э��  ���ļ�	
        case 0x8a://138 ����¼��Э��  ����
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
        case 0x7d://д�ļ�
            RecWriteFile();
            break;
        case 0x81:
            //RecTSData();//����͸������
            break;
        case 0x82:
            //RecFileData();
            break;
        case 0x30://ң��
            RecYTCommand();
            break;
       case 0x35:
       case 0x34://����ip
       case 0x38://������������
       case 0x3F://����ң���ϴ�����  
       case 0x6C://������·��ַ  
       case 0x6B:
       //case 0x7A:
       case 0x44:// ���Լ��ϴ�����  
            RecYSCommand();
            m_zdflag=0;
            break;
	   case 0x96://��ɽ�޸�����	
	   		break;
	   case 0x97://��ɽ�޸�����	
	   		break;
//          case 200://Զ������
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
    if(((m_bReceiveControl&0xf)!=4)&&(SwitchToAddress(m_dwasdu.LinkAddr))&&(m_linkflag))//������·��ʼ����ɺ�����Ӧ��֡ȷ��
       ;// SendAck();
    if(m_guiyuepara.mode==1)
        RecACK();
    return TRUE;
}
//������ѯ�����ò��� + Զ������====================================================================================
//�Զ��屨��
BOOL CBJ101S::RecFrame69(void)
{
  //pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;
	getasdu();
	m_bReceiveControl = pReceiveFrame->Frame68.Control;
    m_acdflag=1;
    m_recfalg=1;
    if(((m_bReceiveControl&0xf)!=4)&&(SwitchToAddress(m_dwasdu.LinkAddr))&&(m_linkflag))//������·��ʼ����ɺ�����Ӧ��֡ȷ��
    {    
        SendAck();
        delayms(200);
    } 
	
	switch(m_dwasdu.TypeID)
    //switch(pReceiveFrame->Frame69.Type)
    {
    	case 0x79:    
        case 0x7a://���ļ�
        case 0x7c:
		case 0x84://��ɢ������		
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
        //case 0x7a://���ļ�
        //    RecReadFile();
        //    break;
        case 0x7d://д�ļ�
        case 0x88://��ɢд����
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


//���� ��λԶ����·
BOOL CBJ101S::RecResetLink(void)
{
    //BYTE PRM = 0;
    switch (pReceiveFrame->Frame10.Control & BITS_PRM)
    {
        case 0: //��վ��Ӧ��վ<��λԶ����·>��֡
            //��Ӧ��վ����·��λӦ�𣬲�����ʼ������
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
        default: //��վ�·�������
            ClearEqpFlag(Rec_FCB);  //�����FCB״̬
            m_YKflag=0;
            m_callallflag=0;
            m_resetflag=0;
            m_acdflag=1;
            //m_yxchangeflag=0;
            //SendBaseFrame(PRM_SLAVE,0x00);
            if(m_guiyuepara.mode == 0)  //��ƽ��ʽ
                m_linkflag=1;
            if(m_guiyuepara.mode==1)
            {
                m_initflag=0x0f;
				m_linkflag = 0;//主站发初始化指令时同时断弢�连接，保障初始化过程不被主动上报打断
		  if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_SEND_FTYC])	
		 	m_initflag=0x0c;		  	
#ifdef YN_101S
		m_initflag=0x0c;
#endif		  
                m_recfalg=1;
            }
            else if(((m_initfirstflag==1))||(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_ISSENDINITFIN]))
            {//��ƽ��ģʽ,ֻ��һ���ϴ���һֱ�ϴ���ʼ����֡
              m_initflag=4;
            }
			
            SendBaseFrame(PRM_MASTER,0x00); //�ŏ|����//

            return TRUE;
    }
}



//�������ٻ�/�ٻ�ĳһ������
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
//      SendAck2(TRUE);     /*���ٻ�1������ȷ��*/
//  else
//      SendAck2(FALSE);    /*���ٻ�2������ȷ��*/
//
//      m_wSendYcNum=0;
//#endif

    return TRUE;
}
//���ٻ��ظ����� ����ȷ�ϣ�������ң�ţ�ң��
BOOL CBJ101S::SendCallAll(void)
{
  int i = 0,j = 0;
  
  if(m_callallflag&0x80)   //���ϸ���
   {
      m_callallflag&=0x7f;
      RecCallAllCommand();
      return TRUE;
   }
   for(i = CALL_YXGRP1,j = 0; i <CALL_DYXGRP1 ;i++,j++) //
   {//����ң��
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
  
 /*//�ŏ|�������׼101
       if(m_ZongzhaoYX == 0x55)
       {
           m_ZongzhaoYX = 0;
           m_wSendYxNum=0;
           if (SendYXGroup(0,20,30))
               return TRUE;
       }
 *///�ŏ|�������׼101
    for(i = CALL_YCGRP1,j = 0; i < CALL_ALLSTOP;i++,j++)
   {//ң��
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
 	{//���ٷ����Լ�
 	SendZJGroup(0,1,36);
	m_wSendZJNum = 0x55;
	return TRUE;
 		}
#endif	
/*//�ŏ|�������׼101	
   if(m_ZongzhaoYC == 0x55)
   {
       m_ZongzhaoYC = 0;
       m_wSendYcNum=0;
       if (SendYCGroup(0,20,35))
           return TRUE;
   }
    *///�ŏ|�������׼101
  if (CheckClearEqpFlag(CALL_ALLSTOP))
      {
          m_callallflag=0;
          m_acdflag=0;m_wSendZJNum = 0;
        if(SendAllStop())
            return TRUE;
      }
  return FALSE;
}

//�������ٻ����ֹͣ����
BOOL CBJ101S::RecCallAllCommand(void)
{
    switch (m_dwasdu.COT& 0x3F)//����ԭ��
    {
        case 6:
            RecCallAllStart();
            m_ZongzhaoYX = 0x55;   //���Ϲ�Լ
            m_ZongzhaoYC = 0x55;
            return TRUE;//���ٻ�����
        case 8://ֹͣ����
            RecCallAllStop();
            break;
        default:
            break;
    }
    return TRUE;
}

//�ظ����ٻ���������
BOOL CBJ101S::RecCallAllStart(void)
{
    if(m_guiyuepara.mode ==1) //ƽ��ʽ
    {
        m_byRTUStatus = RTU_RECCALL;
    }
	
    //SendBaseFrame(0,0);//�ŏ|���� ����ȷ�ϱ���
   //delayms(100);
    SendCallAllStartAck();
    if(m_ztype==20)
    {/*
        if(g_gRunPara[RP_DEVICE_TYPE] == 1)  //��ң�豸
        {
            for (WORD EqpFlag = CALL_YXGRP1; EqpFlag < CALL_ALLSTOP; EqpFlag++)
                SetEqpFlag(EqpFlag);
        }
        else if(g_gRunPara[RP_DEVICE_TYPE] == 0)   //һң�豸
        {
             for (WORD EqpFlag = CALL_YXGRP1; EqpFlag < CALL_YCGRP1; EqpFlag++)
                SetEqpFlag(EqpFlag);             
        }
	else if(g_gRunPara[RP_DEVICE_TYPE] == 2)   //�ź�Դ//�ŏ|�������׼101
        {  */           
                SetEqpFlag(CALL_YXGRP1);   SetEqpFlag(CALL_YCGRP1);            
       // }	//�ŏ|�������׼101
    }
    if((m_ztype>20)&&(m_ztype<33))
    {
       if(m_ztype-21+CALL_YXGRP1<CALL_ALLSTOP)
         SetEqpFlag(m_ztype-21+CALL_YXGRP1);
    }
      SetEqpFlag(CALL_ALLSTOP);


    return TRUE;
}
//�ظ�ֹͣ���ٻ���������
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
    m_ZongzhaoYX = 0;   //���Ϲ�Լ
    m_ZongzhaoYC = 0;

    if(m_ztype==34)
    {
      m_QPM=1;
    }
    return TRUE;
}
//�������ٻ���������ACKȷ��
BOOL CBJ101S::SendCallAllStartAck(void)
{
    BYTE PRM = 0, dwCode = 3,Num = 1;
        SendFrameHead(C_IC_NA, 7);      /*�����������ٲ�ң�س����ظ�ң��ȷ��*/

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


//�������ٻ�����֡
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







//�������ٻ�����ң�ţ����ٻ�ֻ���Ͳ���ʱ����Ϣ
BOOL CBJ101S::SendYXGroup(WORD GroupNo, BYTE Reason, BYTE bType)
{
    BYTE Style = bType;
    BYTE PRM = 0, dwCode = 3; 
    WORD YXNo;
    WORD YXValue;
    BYTE VSQ=0x80;//˳����
#ifdef YN_101S  	
	VSQ=0x00;//��ɢ����
#endif    
    //WORD ucTemp = 0x01;
    YXNo = GroupNo * GRP_YXNUM;
    YXNo+=m_wSendYxNum;
    if((m_wSendYxNum+m_wSendDYxNum>=GRP_YXNUM)|| (YXNo >= m_pEqpInfo[m_wEqpNo].wSYXNum))
    {
        return FALSE;
    }

    SendFrameHead(Style, Reason);
    if(VSQ == 0x80)//˳����
   	write_infoadd(YXNo + g_gRunPara[RP_SYX_INFADDR]);//ADDR_YX_LO

    WORD i;
    //for(i=0;YXNo<RMT_INFO_NUM; i++, YXNo++)//�ŏ| ң�ŵ��
    for(i=0;i<g_ucYxTransNum; i++, YXNo++)//�ŏ| ң�ŵ��
    {
#ifdef YN_101S
        if(i != 1)//�ŏ|���� ң��û��0x81
#endif          
        {
        //g_ucYXAddr,
        //g_ucYxTransNum
        if(VSQ == 0)//��ɢ����
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
    write_infoadd(0x10);//�ŏ|���� ң��0x10 ���Ƿѹ����
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//�ŏ|���� ң��0x10 ���Ƿѹ����
    if(bType == 30)//�ŏ|���� ���ٴ�ʱ�� ң��0x10 ���Ƿѹ����
          write_time();//�ŏ|���� ң��0x10 ���Ƿѹ����
#endif   
    m_wSendYxNum+= i;

    //SendFrameTail(PRM, dwCode, ((i  )| VSQ));
   SendFrameTail(PRM, dwCode, ((i)| VSQ),0);
    return TRUE;
}
//��������һ��ң�� ����
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


//�������ٻ�����˫��ң�ţ����ٻ�ֻ���Ͳ���ʱ����Ϣ
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

//��������ң�⣬����ʱ��
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
    BYTE VSQ=0x80;//��ɢ����   
#ifdef YN_101S
    VSQ=0x00;//��ɢ���� 
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
            Style = M_ME_TE;  //����
            break;
        case M_ME_TC:
        case M_ME_TF:
            Style = M_ME_NC;
            break;
    }
    if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_YC_FLOAT])
    	{
    	Style = 13;//������
    	}
    SendFrameHead(Style, Reason);
    if(VSQ == 0x80)//˳����	
   	write_infoadd(YCNo + g_gRunPara[RP_YC_INFADDR]);
    for (YCSendNum = 0;(YCNo < m_pEqpInfo[m_wEqpNo].wYCNum);YCNo++,YCSendNum++)
    {
#ifdef YN_101S     
        if(YCNo != 7)////�ŏ|���� ң��û��0x4088 0x4089ΪͶ�д���
#endif        
        {
          //write_infoadd(YCNo + ADDR_YC_LO);//�ŏ| ң����ʼ��ַ�޸����в���
          if(VSQ == 0)//��ɢ����   
	  	write_infoadd(YCNo + g_gRunPara[RP_YC_INFADDR]);	 //�ŏ| ң����ʼ��ַ�޸����в��� 
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
          if(m_guiyuepara.yctype!=M_ME_ND) /* #108 - GB101�� ��� GB101�Ӻ�BJ101�ӵ����� */
          {
          /*YxIndex = YcIdToYxId(g_ucYCAddr[YCNo] - 1);
          if(YxIndex == 255) 
             continue;
          if(GetRIStatus(YxIndex) == 1)           
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDSͨ�����Ϻ����λ��1��ʾ��Ч
          else*/
              m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
          }
          if(bType == 35)//����ʱ��
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
    m_wSendYcNum+= 9;YCSendNum=m_wSendYcNum;//����101 ң��9������
#endif		
#ifdef YN_101S	
    SendFrameTail(PRM,dwCode, (YCSendNum - 1)| VSQ,0);//SET ACD//�ŏ|���� ң��û��0x4088 0x4089ΪͶ�д���
#else    
   SendFrameTail(PRM,dwCode, (YCSendNum)| VSQ,0);//SET ACD//�ŏ| ң����
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
            Style = M_ME_TE;  //����
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
          //if(bType == 35)//����ʱ��
            write_time();       
    }       
    SendFrameTail(PRM,dwCode, (ZJSendNum)| VSQ,0);//SET ACD//�ŏ| ң���
     return TRUE;
}

//��������һ��ң��,����һ֡����δ����һ��ĳ���
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

    //write_infoadd(YCNo + ADDR_YC_LO); //�ŏ| ң����ʼ��ַ�޸����в��� 
    write_infoadd(YCNo + g_gRunPara[RP_YC_INFADDR]); //�ŏ| ң����ʼ��ַ�޸����в��� 
    for (YCSendNum = 0; (YCSendNum < (GRP_YCNUM - m_wSendYcNum)) && (YCNo < m_pEqpInfo[m_wEqpNo].wYCNum); YCNo++, YCSendNum++)
    {
        YCValue = g_unYcTrans[YCNo];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(YCValue);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(YCValue);
        YxIndex = YcIdToYxId(g_ucYCAddr[YCNo] - 1);
          if(YxIndex == 255) 
             continue;
          if(GetRIStatus(YxIndex) == 1)           
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDSͨ�����Ϻ����λ��1��ʾ��Ч
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

//ͨ�����Ϳ��д���
void CBJ101S::DoCommSendIdle(void)
{

  //  if((m_uartId == g_CmIdGPRS) && (!g_GprsPowerSt))
     // return;//GPRSͨ�������GPRSû���򲻷��κ�����

    if(m_guiyuepara.mode==1)		
#ifdef YN_101S		
        	m_PRM =0;  //����
#else
		m_PRM =1; 
#endif
    else if(m_recfalg!=3)   //��ƽ��ʽ
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
    
    if((g_SendBeatFailureNum >= 4) && (m_uartId == 2) /*&& (g_GPRSSendLink == OFF)*/&&(m_linkflag == 1))   //����4�β��ش��������,�������ӣ�
    {
        g_SendBeatFailureNum = 0;
        //delayms(3000);
        m_initflag=0x0f;
	 if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_SEND_FTYC])	
		m_initflag=0x0c;			
        //m_recfalg=1;
        m_initfirstflag = 1;
        g_GPRSSendLink = 0;//�ŏ|������1����һ�ε�����
        m_linkflag = 0;
    }
    
    //ģʽ2=�����ϱ������κ�Լ��,���ٲ���Լ��
    if((g_ucPara101[IECP_AUT_MODE] == 2))  //�����ӳٵ�ʱ
    {
      if(m_uartId != g_CmIdGPRS)
        m_linkflag = 1;
      else if (g_SendFault == ON)
      {
         g_SendFault = OFF;
         m_linkflag = 1;
       }
    }
    
    //��Ӧ��վ����========================
    if(m_recfalg)
    {//a
        //ģʽ1=ֻҪ�յ���վ���ľ������ϱ�,���ٲ���Լ��
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
       
       //ģʽ0=ֻ�г�ʼ����������Ӧ��վ�ٻ����������ϱ�
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
 
    //��·�Ѿ���ʼ�����ط�ʱ������
    if((m_zdflag==1)&&(m_linkflag)&&(m_resendflag)&&(m_reSendDelay==0))
    { 
            m_retxdnum++;
          if(m_retxdnum < MAX_FRM_RESEND)
          {
            SendRetry();
            
            m_reSendDelay = g_gRunPara[RP_YXRSD_T];
            return;
          }
          if(m_resendflag)   /*�ط�MAX_FRM_RESEND-1��*/
          {
              m_retxdnum=0;
              //m_zdflag=0;     
              //m_linkflag=0;   /*�Ͽ���·*/
              m_resendflag=0;
              m_reSendDelay = 0;
              m_WaitConfTpId = 0;//��2���ط��󻹻������ط���ʱ�����˴εĶ�ʱ�������������ط�����
              return;
          }
  }
#ifdef  SD_101S	//ɽ�� ��ʼ������������٣��м䲻����cos��soe�����Գ�ʼ����ɣ�9��֮����cos��soe
	if((g_CosSoeDely<=0x2000)&&(m_linkflag))
		g_CosSoeDely++;	
#else
	g_CosSoeDely=0xffff;
#endif
    //�����ϱ�����===================================
    if((m_zdflag==0)&&(m_linkflag))
    {//b
      //�����ϴ�֮ǰ��ȷ����û����Ҫ�ش��ı���
        if(SendRetry())
           return;
	

#ifndef  YN_101S	//����101 ����COS	

        //����cos
      /* */ 
        if(g_CosSoeDely>6000)
        {
        if(SearchCos(m_SYxBit,0))
        {
           m_acdflag=1;
           if(SendCos())
           {
              m_reSendDelay = g_gRunPara[RP_YXRSD_T];//�б��ķ����������ش�
               return ;
           }
           m_acdflag=0;
        }
        }
#endif 
#ifdef  YN_101S	//¼����ɺ� �Զ��ϴ�Ŀ¼
	if((g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_SENDLUBOML])&&(m_luok==1))
	{
		m_luok=0;
		SendLBML();
		return ;
	}
#endif
        //����ͻ��ң��
        if(g_SendChgYcFlag)
        {
          g_SendChgYcFlag = 0;
            if(SendChangeYC())
            {
               ; 
            }
              return;
        }
        //����SOE
        if(g_CosSoeDely>6000)
        {          
        if(SearchCos(m_SSoeBit,0))
        {
           m_acdflag=1;
           if(SendSoe())
            {
              m_reSendDelay = g_gRunPara[RP_YXRSD_T];//�б��ķ����������ش�
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
		  if(g_ucPara101[IECP_101_STY]==1)//��ɽ101
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
     if((gRecorder_flag.LIST_flag == ON)&&(m_ackflag))//(( gRecorder_flag.CFG_flag ==ON )||(gRecorder_flag.DAT_flag ==ON )||)//���ڶ������ļ�������
     {
              m_ackflag =0;
              //g_Cmid = m_uartId;
              //memcpy(&pReceiveFrame->Frame68.Start1,gRecorder_flag.pRXBuff,256); 
              //if(FILEPREPAR_TYPE)
              //Code_Lubo(&pReceiveFrame->Frame68.Start1,m_SendBuf.pBuf);
              Code_Lubo(gRecorder_flag.pRXBuff,m_SendBuf.pBuf);
		gRes_rec.res_timeout = 0;	  
     }
     if((gRecorder_flag.LIST_flag_YN== ON)&&(m_ackflag))//(( gRecorder_flag.CFG_flag ==ON )||(gRecorder_flag.DAT_flag ==ON )||)//���ڶ������ļ�������
     {
              m_ackflag =0;
              Code_Lubo_YN(gRecorder_flag.pRXBuff,m_SendBuf.pBuf);
		gRes_rec.res_timeout = 0;	  
     }	 
//	 ������¼���ļ���Լ
   if(((mRecorder_flag.LIST_flag == ON)||(mRecorder_flag.xuchuanflag== ON))&&(m_ackRecorder ==ON))//(( gRecorder_flag.CFG_flag ==ON )||(gRecorder_flag.DAT_flag ==ON )||)//���ڶ������ļ�������
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
   
    //ֻ���GPRSͨ�������ж���ǰ��GPRS
  /*  if((g_sTimer[TM_GPRSPWOERDOWN].m_TmCountBk != 60) && (g_gRunPara[RP_POWER_MODE]!= REALTIME) && g_GprsPowerSt && (m_uartId == g_CmIdGPRS))
    {
        if((CheckHaveDataToSd() == FALSE) &&(!g_JdgPwMode))
        {
          OpenTimer(TM_GPRSPWOERDOWN,60);//�ӳ�1���ӹر�GPRS
          BYTE bBuf[2] = {0xEB,0XEB};
          MyPrintfD(bBuf,2);
        }
    }*/

}

/***************************************************************
    Function��OnTimeOut
        ��ʱ����
    ������TimerID
        TimerID ��ʱ��ID
    ���أ���
***************************************************************/
BOOL CBJ101S::DoTimeOut(WORD wTimerID)
{
	CPrtcSms::DoTimeOut(wTimerID);
   	if(m_reSendDelay > 0)
   		{
      	int iTime = 0;
      	if(g_gRunParaYxRsdTTBk != g_gRunPara[RP_YXRSD_T])
      		{//ң���ط�ʱ�������ͱ仯
          	iTime = g_gRunPara[RP_YXRSD_T] - (g_gRunParaYxRsdTTBk - m_reSendDelay);
        	if(iTime < 0)  iTime = 1;
        	m_reSendDelay = iTime;
        	g_gRunParaYxRsdTTBk = g_gRunPara[RP_YXRSD_T];
      		}
     //ȥ���±������������Ϊ�����������ϼ��ʱ��̣ܶ���һ�����ϸշ�����
     //��û�ȵ���վ�ظ�(����վͨ������)����ʱ��ǰ�������ط���ʱ���Ὣ�ϴεĹ����ַ�һ�Ρ�
     // if(SearchCos(m_NewFltBit,1) && SearchCos(m_SYxBit,1))
     //  m_reSendDelay = 1;//�����²����Ĺ��ϣ���ǰ�����ط���ʱ
      	if(m_reSendDelay>0)//���Ӹò�������Ϊ=0ʱ��������ܷ�
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
  		{//ң�ؼ�ʱ
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
      		{//ODUд�����ط���ʱ
         	m_PaWaitCt_lubo--;
          	if(m_PaWaitCt_lubo <= 0)
          		{
            	m_PaWaitflag_lubo = TRUE;
            	return TRUE;
          		}
       		}
    	}
    if(g_sTimer[TM_SENDYC].m_TmFlag & m_SendYcBit)
    {//������ʱ����ң��
        g_sTimer[TM_SENDYC].m_TmFlag &= ~m_SendYcBit;
        m_SendYcFlag = 1;
        m_wSendYcNum = 0;
        g_yxChangeflag |= BITF;//g_yxChangeflag�����λ��ʶ�б�����Ҫ�ϴ�
        return TRUE;
    }
  /*  if(g_gRunPara[RP_BEAT_T] > 0)//�����������ʱ�����0��������������
    {
        if(g_sTimer[TM_BEAT].m_TmCount == 0)
            OpenTimer(TM_BEAT);
    }
    if(g_SendYc && g_sTimer[TM_SENDYC].m_TmCount == 0 && (g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ISSENDYC]))
    {
        OpenTimer(TM_SENDYC);
    }*/
    
    
   if(g_sTimer[TM_BEAT].m_TmFlag & m_BeatBit)
   {//������������
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
   {//����ң��
      if (GetEqpFlag(i))
        {
           ClearEqpFlag(i);
           m_wSendYxNum=0;
           if (SendYXGroup(j,21+j,1))
              return TRUE;
        }
   }
    if (GetEqpFlag(CALL_DYXGRP1))
    {//˫��ң��
      ClearEqpFlag(CALL_DYXGRP1);
        m_wSendDYxNum=0;
      if (SendDYXGroup(0,20))
        return TRUE;
    }
    for(i = CALL_YCGRP1,j = 0; i < CALL_ALLSTOP;i++,j++)
   {//ң��
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

//ʱ��ͬ��ACK�ظ�
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
//ʱ������REQ�ظ�
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



//��·���Իظ�����
BOOL CBJ101S::SendTsetLinkAck(void)
{
    //SendBaseFrame(0,0);//�ŏ|���� ����ȷ�ϱ���
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
//��λRTU��ACK�ظ�����
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
//���͵���SOE����
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
        Style = M_SP_TA;        //��ʱ�굥��ң��
    WORD RecSoeNum = GetCosNum(m_SSoeHeadPtr,g_unSCosBuffTail);
    if (RecSoeNum == 0)
         return FALSE;
  else if(RecSoeNum > 24)
  {
    RecSoeNum =24;  
        g_yxChangeflag |= m_SSoeBit;  
  }
    m_SSoeHeadPtrBk = m_SSoeHeadPtr;//��׼ʵʱģʽ�£�������ϸ�λ���ȳ�ʼ��֮��ֻ���ϴ���λ�ź�
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
        if(m_guiyuepara.yxtype==3)//��ʱ�굥��ң��
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
        Style = M_SP_TA;        //��ʱ�굥��ң��
/*
    m_SendSoeAdr = m_SendSoeAdr&0x3ff0;	
    SSoeN= m_SendSoeAdr+240;//240 Ϊ15��soe�Ĵ洢�ռ�
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
    m_SSoeHeadPtrBk = m_SSoeHeadPtr;//��׼ʵʱģʽ�£�������ϸ�λ���ȳ�ʼ��֮��ֻ���ϴ���λ�ź�
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
       	if(m_guiyuepara.yxtype==3)//��ʱ�굥��ң��
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
#else //soe���ⲿflash��ȡ
/*
//���͵���SOE����
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
        Style = M_SP_TA;        //��ʱ�굥��ң��

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
        if(m_guiyuepara.yxtype==3)//��ʱ�굥��ң��
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

//����˫��SOE����
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
        if(m_guiyuepara.yxtype==3)//��ʱ�굥��ң��
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

//���ͱ仯ң������
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
    m_guiyuepara.yctype = 9;//�ŏ|����//ң��ͻ�䲻Ҫʱ��
#endif  
    if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_YC_FLOAT])
    	{
    	m_guiyuepara.yctype = 13;//������
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
#ifdef  YN_101S//����ͻ��ң�� ״̬��־Ϊ30 0X1E
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
//��ȡ������
BYTE CBJ101S::GetCtrCode(BYTE PRM,BYTE dwCode,BYTE fcv)
{
    BYTE CodeTmp = 0x00;
    dwCode&=0xf;
    CodeTmp += dwCode;

    if (PRM)
        CodeTmp |= 0x40;  //���ϸ���
        
    if(m_guiyuepara.mode==0)
    {
        //CodeTmp |= 0x80;//lxq  DIR ����λ
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
            if(!m_resendflag)//���ط����ĲŽ���fcb�ķ�ת
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

//���͹̶�֡��ʽ��������
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


//���͸�λԶ����· ƽ��ʽ
BOOL CBJ101S::SendResetLink(BYTE PRM)
{
    return SendBaseFrame(PRM,0x00);
}

//��������Զ����·״̬ ƽ��ʽ
BOOL CBJ101S::SendReqLink(void)
{
    return SendBaseFrame(1, 0x09);
}


//flag = 0:������Ӧ״̬λ��д0��=1:ֻ��
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

//�������1������
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
       return TRUE; /*����16*/
    }
    return FALSE;
}
//��������¼��Ŀ¼
//#ifdef YN_101S
BOOL CBJ101S::SendLBML(void)
{
    //BYTE Style = M_SP_NA;
    BYTE Reason = COT_SPONT;
    BYTE PRM = 0;
    //BYTE dwCode = 3;
    //unsigned int temp;	
    PRM = 1;

    SendFrameHead(0x88, Reason);//���ͱ�ʾ137 0x88
    /*
          temp = g_sRecData.m_gRecCNum;
	   if(g_sRecData.m_gRecCNum==0)
	   	temp = MAX_REC_NUM-1 ;
	   else
	   	temp = g_sRecData.m_gRecCNum-1;
          long FADDR_RECORDER =FADDR_RECORDER_INFO+ (long)temp*(long)FLINEADDR;
          Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_filecfg,sizeof(gRecorder_filecfg)); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ����� 
*/
          //��Ϣ���ַ 0x408a
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0x8a;// LOBYTE(wave_total);
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0x40;// LOBYTE(temp);
          //�ܰ�1�ֽ� ��ǰ��1�ֽ�
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =1;// LOBYTE(wave_total);
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;// LOBYTE(temp);
          //cfg�ļ��� 8�ֽ� 7�ֽ�ʱ��+1�ֽڱ��	   
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//¼������ʱ��
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_SEC];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(gRecorder_filecfg.comtrade_time[RTC_WEEK]<<5);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR];
	 m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = (gRecorder_filecfg.FileName<<1) &0xfe;//0λ=0�� cfg�ļ�
          //�ļ����� 3�ֽ�      
          //*pTxBuf++ = gRecorder_filecfg.TOTAL_Leng;
         // *pTxBuf++ = gRecorder_filecfg.TOTAL_Leng>>8;
         // *pTxBuf++ = gRecorder_filecfg.TOTAL_Leng>>16; 
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 3;
	  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x01;	  
	   m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;	 
          //SOF 1�ֽ� 	
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = gRecorder_filecfg.CFG_SOF;
          //�ļ�����ʱ�� 7�ֽ�       
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//¼������ʱ��
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_SEC];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(gRecorder_filecfg.comtrade_time[RTC_WEEK]<<5);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR];
	   //*****************************************************	
          //data�ļ��� 8�ֽ� 7�ֽ�ʱ��+1�ֽڱ��		   
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//¼������ʱ��
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_SEC];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(gRecorder_filecfg.comtrade_time[RTC_WEEK]<<5);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR];	
	  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = (gRecorder_filecfg.FileName<<1) |0x01;////0λ=1�� data�ļ�
          //�ļ����� 3�ֽ�      
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = gRecorder_filecfg.TOTAL_Leng*20;
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = (gRecorder_filecfg.TOTAL_Leng*20)>>8;
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = (gRecorder_filecfg.TOTAL_Leng*20)>>16;  
          //SOF 1�ֽ� 	
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = gRecorder_filecfg.CFG_SOF;
          //�ļ�����ʱ�� 7�ֽ�       
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//¼������ʱ��
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_SEC];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(gRecorder_filecfg.comtrade_time[RTC_WEEK]<<5);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= gRecorder_filecfg.comtrade_time[RTC_YEAR];	

    SendFrameTail(PRM, 0x03, 2,0);//�ɱ�ṹ�޶��� 2

    return TRUE;
}
//#endif
//���ͱ仯ң��
BOOL CBJ101S::SendCos(void)
{
    BYTE Style = M_SP_NA;
    BYTE Reason = COT_SPONT;
    BYTE PRM = 0;
    //BYTE dwCode = 3;
    BYTE YXSendNum = 0;
    WORD YXNoInRmt;//�ܱ��е����
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
        //�ŏ| ¼��m_ucCosBk[YXSendNum] = YXNoInRmt;
        //��ң�ű仯��־λ�����ش��ĵط���g_ucYxIdBk�б����ң�Ŷ�Ӧ��g_gRmtChgFlag��1
        //������ش�������ĳλ�ֲ���COS���ش�ʱ���ϴ�����״̬(ǰ��δ���ɹ����м�״̬������)
        //����ڵȴ���վȷ�ϵĹ�����ĳλ�ֲ�����COS����ôg_gRmtChgFlag�ᱻ������1���´μ���ʱ�Ὣ��״̬�ϴ�
        //��������ش������Ѵ���cos����Ϊֹ
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
//���ͱ仯ң��
BOOL CBJ101S::SendDCos(void)
{
    BYTE Style = M_DP_NA;
    BYTE Reason = COT_SPONT;
    BYTE PRM = 1;
    BYTE dwCode = 3;
    BYTE YXSendNum = 0;
    //WORD YXNoInRmt;//�ܱ��е����
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

//��֯����ͷ
BOOL CBJ101S::SendFrameHead(BYTE Style, BYTE Reason)
{
    m_SendBuf.wReadPtr = 0;
    m_SendBuf.wWritePtr=0;
     if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ENCRYPT]) //��������
    {
        m_SendBuf.wWritePtr += 2;  //����Ԥ��2�ֽڸ�Ӧ���������ͺ�Ӧ�����ݳ���
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

//��֯����β����������֡����
BOOL CBJ101S::SendFrameTail(BYTE PRM, BYTE dwCode, BYTE Num,BYTE EncType)
{
    WORD wLinkAddress;
//    BYTE temp;
    BYTE SendDataLen;
    BYTE EncData[256];
    if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ENCRYPT]) //��������
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
    if(Num == 0xff)  //����ң�ش���
    {
        pSendFrame->Frame68.Control = 0x80;
        Num = 1;
    }
#ifdef BJ101_GZTEST
    /*���ݲ���-ң��Ԥ�÷�УACDΪ0*/
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
    //�����͵ı��Ĳ���Ҫ�ط����ط�����д������ط�ʱ��������Ϊ���÷��ط����ķ��ͺ�Ҳ�ܵȴ�һ��ʱ��ŷ����ϱ���
    if(m_WaitConfTpId != M_SP_NA && m_WaitConfTpId != M_DP_NA
     && m_WaitConfTpId != M_SP_TB && m_WaitConfTpId != M_DP_TB)
     {
        //m_retxdnum = g_gRunPara[RP_YXRSD_NUM];
        m_reSendDelay = g_gRunPara[RP_YXRSD_T];
     }	
    if(SwitchToAddress(m_dwasdu.LinkAddr))
    {
        if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_ENCRYPT]) //��������
        {
            m_SendBuf.pBuf[0] = EncType;  //Ӧ������ ����
             //-2��ԭ����ǰ��Ԥ����1�ֽ�Ӧ������+1�ֽ�Ӧ�����ͳ��� ��SendFrameHead������
            m_SendBuf.pBuf[1] = m_SendBuf.wWritePtr - 2; 
            switch(EncType)
            {
            case 0x00:
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;   //����2�ֽ���Ϣ��չ������
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
                SendDataLen = m_SendBuf.wWritePtr ;//- 1;
                //memcpy(SendData,m_SendBuf.pBuf,SendDataLen);
                SendDataLen = EncryptData(m_SendBuf.pBuf,SendDataLen,EncData);
                if(SendDataLen) 
                {
                    SendFrameEBHead(0x0008,201); //Ӧ�����ʹ��ڵ���200����дӦ�����ͣ����ڼ���������
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
            WriteToComm(wLinkAddress);//0X69��ͷ�ı��Ĳ��ж���·��ַ
        }
        
   }
   return TRUE;
}


//������������֡
BOOL CBJ101S::SendNoData(void)
{
    if(m_guiyuepara.mode==0)
      return SendBaseFrame(0, 0x09);
    else 
      return TRUE; /*R227: ƽ��ģʽ�����ݲ��ͱ���*/
}




//���ʹ���Ӧ��
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
//����ң�ؽ���
BOOL CBJ101S::SendYKstop(void)
{
BYTE Style, Reason = 10;
BYTE PRM = 0, dwCode = 3, Num = 1; /* #22 ң��Ԥ��/����/ֹͣ ȷ�ϱ��ĵ���·������Ӧ��Ϊ3,����0 */
BYTE * pData = &pReceiveFrame->Frame68.Data[m_byInfoShift];

//Style = pReceiveFrame->Frame68.Data[m_byTypeIDShift];
/*R227: ң�ز����������ٵȱ���, TypeID����, �������޸�*/
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
    if(m_initflag&1)    //����Զ����·״̬
    {
        m_initflag&=~1;
	delayms(1000);		
        SendReqLink();
        return;
    }
	if(g_ucPara101[IECP_101_STY]==1)//��ɽ101
		m_initflag=0x08;
    if(m_initflag&2)  //��λԶ����·
    {
        m_initflag&=~2;
	//delayms(1000);		
        SendResetLink(PRM_SLAVE);  //����
        return;
    }
    if(m_initflag&4)
    {
        //m_initflag=0;
        m_initflag&=~4;
        m_fcb=0x20;
        if(((m_initfirstflag==1))||(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_ISSENDINITFIN]))
        {//ֻ��һ���ϴ���һֱ�ϴ���ʼ����֡ʱ����
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
    if(!m_initflag && (m_guiyuepara.mode == 1))  //ƽ��ʽ
       m_linkflag=1;      
    return;
}
void CBJ101S::SendInitFinish(void)
{
  //if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_ISSENDINITFIN])
  {//�ÿ�������ѡ���Ƿ��ͳ�ʼ����֡
#ifdef CQ_101S  
    SendFrameHead(M_EI_NA, 3);//�ŏ|����
#else    
    SendFrameHead(M_EI_NA, COT_INIT);
#endif
    write_infoadd(0);
//  #ifdef BJ101_GXTEST
//  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = gResetReason;
//  #else
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0; //��ʼ��ԭ��  ����
    //#endif
    SendFrameTail(PRM_MASTER, 0x03, 1,0);//funcode=0x0a?  
}
    //if(m_guiyuepara.mode == 1)  //ƽ��ʽ
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
    Function��SearchOneFrame
        ����һ֡��ȷ�ı���
    ������Buf, Len
        Buf ���ջ�����ͷָ��
        Len ���ջ���������Ч���ݵĳ���
    ���أ�DWORD���ݣ�����
        ���֣�������
            #define FRAME_OK       0x00010000      //��⵽һ��������֡
            #define FRAME_ERR      0x00020000      //��⵽һ��У������֡
            #define FRAME_LESS     0x00030000      //��⵽һ����������֡����δ���룩
        ���֣��Ѿ���������ݳ���
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

//          #ifdef BJ101_GXTEST /*����19-���ӹ�����ַ�ж�*/
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


//ACK�Ĵ���
void CBJ101S::DoRecAck(void)
{
    if (m_byRTUStatus == RTU_RECCALL)
    {
        SendCallAll();
    }
    return;
}
//��λԶ����· ƽ��ʽ
BOOL CBJ101S::RecACK(void)
{
    return true;
}



//��������Զ����·״̬
BOOL CBJ101S::RecReqLink(void)
{

#ifdef YN_101S
    SendBaseFrame(0,0x0b);  //yunnan
#else    
    SendBaseFrame(0,0x0b);  //�ŏ|����&��׼��
#endif    
    return TRUE;

}



//�ٻ�һ���û�����
BOOL CBJ101S::RecCallClass1(void)
{
    if(m_linkflag==0)
    {
      return  TRUE;
    }
    /*����18-���ٻ��ɲ���COS��SOE*/
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
    //����cos
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
    //����SOE
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

//Զ����·״̬��û��ٻ������û�����
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
      return TRUE; //Զ����·״̬��û��ٻ������û�����
}

//�޸����봦��
BOOL CBJ101S::RecYSPassWord(void)
{
	DWORD PassWord;
	WORD tempassWord1,tempassWord2;
	
	BYTE *pData = &pReceiveFrame->Frame68.Data[m_byReasonShift];//����Ϣ���ַ��߿�ʼȡ��������ɲ�������Ϣ���ַ
        return TRUE;
}
//�޸����Ĵ���
BOOL CBJ101S::RecYSCiPHer(void)
{
	DWORD PassWord;
	WORD tempassWord1,tempassWord2;
	
	BYTE *pData = &pReceiveFrame->Frame68.Data[m_byReasonShift];//����Ϣ���ַ��߿�ʼȡ��������ɲ�������Ϣ���ַ
        return TRUE;
}
//ң�ش���
BOOL CBJ101S::RecYKCommand(void)
{
        RecYKSet();
        return TRUE;
}

//ң��Ԥ��/ִ������
BOOL CBJ101S::RecYKSet(void)
{
BYTE * pData = &pReceiveFrame->Frame68.Data[m_byInfoShift];
WORD  DCO;  //ң�������޶���
WORD  YKNo; //ң��·��

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
        YKNo = MAKEWORD(pData[0], pData[1]);//���ֽ�ǿ��Ϊ0
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
        break; //��
    case 2:
        pYKInfo->Info.byValue = 1;//0x05;
        break; //��
    default:
        break;
}

    if(m_dwasdu.TypeID==C_SC_NA) {
        if(DCO&1)
            pYKInfo->Info.byValue = 1;//0x05;   //��
        else
            pYKInfo->Info.byValue = 0;//0x06;  //��
    }

pYKInfo->Head.abyRsv[0] = pReceiveFrame->Frame68.Data[m_byTypeIDShift];
if (m_guiyuepara.infoaddlen == 3)//Ҳֻȡ��Ϣ�����������ֽ�
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
if (m_guiyuepara.infoaddlen == 3)//Ҳֻȡ��Ϣ�����������ֽ�
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

//ִ��ң������
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
      		g_sRecData.m_ucRecSavingFlag = YES; //�ñ�־Ҫ�󱣴浽FLASH��
      		g_sRecData.m_ucFaultRecStart = CLOSE;//¼�����������ϻָ��󣬻ָ�OFF
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
      	eight_select=0x8F; //eight_select|=BIT7;                    ////////////���״η���8����ı�־		
        Sign_Repeat(1,0);//ģ��A�����
      	}    
    }
    else if(chYkPa ==3)
    {
      if(Val) 
      	{
      	eight_select=0x8F; //eight_select|=BIT7;                    ////////////���״η���8����ı�־		
        Sign_Repeat(2,0);//ģ��b�����
      	}    
    }	
    else if(chYkPa ==4)
    {
      if(Val) 
      	{
      	eight_select=0x8F; //eight_select|=BIT7;                    ////////////���״η���8����ı�־		
        Sign_Repeat(3,0);//ģ��c�����
      	}    
    }
    g_YkOrderFlag = TRUE;
  
      
  return TRUE;
  
  
}

//����ң��Ԥ��/ִ��

//����ң��Ԥ��/ִ��
BOOL CBJ101S::SendYKSetAck(void)
{
    BYTE Style, Reason = 7;
    BYTE PRM = 0, dwCode = 3, Num = 1; /* #22 ң��Ԥ��/����/ֹͣ ȷ�ϱ��ĵ���·������Ӧ��Ϊ3,����0 */
    VYKInfo *pYKInfo;

    pYKInfo = &(pGetEqpInfo(m_wEqpNo)->YKInfo);
    Style = pYKInfo->Head.abyRsv[0];
    if (pYKInfo->Head.byMsgID == MI_YKCANCEL)
    {
        Reason = COT_DEACTCON;
    }
    if (pYKInfo->Info.byStatus != 0)
    {
        /* R226: �Ƿ�ң����ȷ����ԭ�� */
        if(pYKInfo->Info.byStatus == 1)
            Reason = COT_PN_BIT|47;             /*δ֪����Ϣ�����ַ*/
        else
            Reason = COT_PN_BIT|COT_ACTTERM;    /*������ֹ*/
    }
    if(pYKInfo->Head.byMsgID == MI_YKOPRATE && g_YkOrderFlag != TRUE)
       Reason |= COT_PN_BIT; //ң��ִ��ʧ��
    SendFrameHead(Style, Reason);
    switch (m_guiyuepara.infoaddlen )
    {
        case 1:
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[1];//��Ϣ���ַ
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[2];//DCO
            break;
        case 2:
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[1];//��Ϣ���ַ
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[2];
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[3];//DCO
            break;
        case 3:
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[1];//��Ϣ���ַ
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[2];
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[3];//DCO
            break;
        default:
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.abyRsv[1];//��Ϣ���ַ
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
    /*R227: ƽ��ģʽִ��ȷ�Ͻ���ң�ؽ���*/
    if (pYKInfo->Head.byMsgID == MI_YKOPRATE && m_guiyuepara.mode==1 && g_YkOrderFlag == TRUE)
    {
       SendYKstop();
    }
    g_YkOrderFlag = OFF;//��ң�ر�ʶ
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
  case 0x6001://��IP��ַ���˿�
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
  case 0x6003://����·��ַ
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_COMM_ADDR];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      break;
  case 0x6009://���ź�Դ��������
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_BEAT_T];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      break;
  case 0x6011://��ң����Ϣ�ϴ�����
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_SENDYC_T];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      break;  
  case 0x6014://���Լ���Ϣ�ϴ�����
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_SENDZJ_T];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      break;  
  case 0x6020://�������ѹ��ѹ
      //SendFrameHead(102,7);
      //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_HIGH_Z];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      //SendFrameTail(0, 0xa5, 1);
      break;
  case 0x6021://���ߵ�ѹ��ѹ
      //SendFrameHead(102,7);
      //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_I0_START];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      //SendFrameTail(0, 0xa5, 1);
      break;
  case 0x6022://�����ѹ��ѹ
      //SendFrameHead(102,7);
      //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_HIGH_P];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      //SendFrameTail(0, 0xa5, 1);
      break;
   case 0x6023://�����ѹǷѹ
      //SendFrameHead(102,7);
       //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      cmd = g_gRunPara[RP_LOW_P];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(cmd);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(cmd);
      //SendFrameTail(0, 0xa5, 1);
      break;
   case 0x6024://����������Ƚ�ֵ
      //SendFrameHead(102,7);
      //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(g_gRunPara[RP_PULSE_VALID]);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(g_gRunPara[RP_PULSE_VALID]);
      //SendFrameTail(0, 0xa5, 1);
      break;
    case 0x6025://��Ͷ����ʱʱ��
      //SendFrameHead(102,7);
      //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(g_gRunPara[RP_T_DELAY]);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(g_gRunPara[RP_T_DELAY]);
      //SendFrameTail(0, 0xa5, 1);
      break;
    case 0x6026://���ӵع���Ͷ��
      //SendFrameHead(102,7);
      //write_infoadd(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(g_gRunPara[RP_CNL_MODEL]);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(g_gRunPara[RP_CNL_MODEL]);
      //SendFrameTail(0, 0xa5, 1);
      break;
    case 0x6027:	   //��ѹ�澯��ʱʱ��	
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(wInfoAddr);	 
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(g_gRunPara[RP_REV_CURRENT]);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(g_gRunPara[RP_REV_CURRENT]);    
      break;
    case 0x6028:	   //�Ƿ������ϴ�¼��Ŀ¼	
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

//����ʱ��ͬ������
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
    if((60000-MSecond) > time_delay_set)//���Ǽ�����ʱ���ݵ�ʱ����Ƿ����60000ms,�����������Ҫ���Ӽ�1
      MSecond+=time_delay_set;
    else
    {
      MSecond=MSecond+time_delay_set-60000;
      unHYM[5]+=1;
    }
      
    unHYM[6] = MSecond/1000;//SEC
    WriteRealTime(unHYM);  //�޸�ʱ��
  //SendtimeAck();Ӧ�ȷ���֡ȷ���ٷ�����
   
    m_timeflag=1;
    return true;
}
//������ʱ�ӱ���
BOOL CBJ101S::RecReadClock(void)
{
    //SendBaseFrame(0,0);//�ŏ|���� ����Ҫ��ȷ�ϱ���
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
    
     WriteRealTime(unHYM);  //�޸�ʱ��
  //SendtimeAck();Ӧ�ȷ���֡ȷ���ٷ�����
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


//����������·
BOOL CBJ101S::RecTestLink(void)
{
    m_testflag=1;
    //return SendTsetLinkAck();
    return TRUE;
}

//������λRTU
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
////  WORD  QOS;  //ң�������޶���
//    WORD  YTNo;
//    //WORD YTValue; //ң��·��
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


//��·��ַ����վվַ��
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

//7�ֽڵĳ�ʱ��
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
//��ʱ��
void CBJ101S::write_time3()
{
    ReadRealTime();
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(g_sRtcManager.m_gRealTimer[RTC_MICROSEC]+g_sRtcManager.m_gRealTimer[RTC_SEC]*1000)&0xff;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(g_sRtcManager.m_gRealTimer[RTC_MICROSEC]+g_sRtcManager.m_gRealTimer[RTC_SEC]*1000)>>8;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=g_sRtcManager.m_gRealTimer[RTC_MINUT];
}
//�ط�����
BOOL CBJ101S::SendRetry(void)
{
  // int i = 0;
   
   if(m_WaitConfTpId == M_SP_NA || m_WaitConfTpId == M_DP_NA)//COS�ش�
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
         {//˫��COS�ش�
          if(SendDCos())
            return 1;
         } 
     }
   }
   else if(m_WaitConfTpId == M_SP_TB)//����SOE�ش�
   {
      if(m_SSoeHeadPtr != m_SSoeHeadPtrBk)
      {
        m_SSoeHeadPtr = m_SSoeHeadPtrBk;
        if(SendSoe())
          return 1;
      }
    }
   else if(m_WaitConfTpId == M_DP_TB)//˫��SOE�ش�
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
  //BYTE *pData = &pReceiveFrame->Frame68.Data[m_byInfoShift + m_guiyuepara.infoaddlen];//����Ϣ���ַ��߿�ʼȡ��������ɲ�������Ϣ���ַ
  BYTE *pData = &pReceiveFrame->Frame68.Data[m_byInfoShift];//����Ϣ���ַ��߿�ʼȡ��������ɲ�������Ϣ���ַ
  cip = 0;
  if(bVSQ >= 1 && bTypeID == 53)   //���ö������
  {       
       for(i = 0; i < bVSQ; i++ )
       {
       	  temp0=*pData;pData++;
	  temp1=*pData;pData++;
          tempwInfoAddr= MAKEWORD(temp0,temp1);  
	  switch (tempwInfoAddr)
	  {
	    case 0x6001: //�޸�IP��ַ���˿�
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
	    case 0x6003: //�޸���·��ַ
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
	    case 0x6009:   //�޸��ź�Դ��������
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
	    case 0x6011:  //ң���ϴ�����
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
            case 0x6014:  //�Լ���Ϣ�ϴ�����
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
            case 0x6020://�����ѹ�澯��ֵ
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
            case 0x6021://�ߵ�ѹ��ѹ�澯��ֵ
              temp0=*pData;pData++;
	      temp1=*pData;pData++;
	      wVal = MAKEWORD(temp0,temp1);
              wTemp = wVal;
              if(wTemp != g_gRunPara[RP_I0_START])  //�ߵ�ѹ��ѹ
              {
                g_gRunPara[RP_I0_START] = wTemp;
                g_ucParaChang |= BIT0;
                SetEqpInfo();
              }          
              break;
           case 0x6022://�ߵ�ѹ��ѹ�澯��ֵ
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
            case 0x6023://���ѹǷѹ�澯��ֵ
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
            case 0x6024://��������Ƚ϶�ֵ
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
            case 0x6025://Ͷ����ʱʱ��
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
            case 0x6026://װ�ù���Ͷ/�ˣ�1/0��
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
	    case 0x6027:	   //��ѹ�澯��ʱʱ��
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
	    case 0x6028:	   //�����ϱ�¼���ļ�(1/0)
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
  if(wInfoAddr == 0x6001 && bTypeID == 0x34)   ////�޸�IP��ַ���˿�
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
    if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(6,RESET,g_gIPPort,this);//����GPRSģ��
  }
  if(wInfoAddr == 0x6003 && bTypeID == 0x6C)   //������·��ַ
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
      g_gRunPara[RP_CRC] = CrcCount((unsigned int *)g_gRunPara, RP_CRC);      //����CRC
      CAT_SpiWriteWords(EEPADD_RP, RUN_PARA_NUM, g_gRunPara); //���浽EEPROM��	  
      delayms(300);
      main_reset_flag = 0x55;
    }	
    delayms(5000);
  }
  if(wInfoAddr == 0x6009 && bTypeID == 0x38)   //������������
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
  if(wInfoAddr == 0x6011 && bTypeID == 0x3f)   //����ң���ϴ�����
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
  if(wInfoAddr == 0x6014 && bTypeID == 0x44)   //�����Լ��ϴ�����
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
        case  53: //�ź�Դ
        {
              switch(wInfoAddr)
              {
                case 0x6003:  //�޸���·��ַ
                  wVal = MAKEWORD(pData[0],pData[1]);
                  wTemp = wVal;
                   if(wTemp != g_gRunPara[RP_COMM_ADDR])
                   {
                       g_gRunPara[RP_COMM_ADDR] = wTemp;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                    }
          
                   break;
                case 0x6009:  //�޸��ź�Դ��������
                  wVal = MAKEWORD(pData[0],pData[1]);
                  wTemp = wVal;
                   if(wTemp != g_gRunPara[RP_BEAT_T])
                   {
                       g_gRunPara[RP_BEAT_T] = wTemp;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                    }          
                   break;
                 case 0x6011:  //ң���ϴ�
                  wVal = MAKEWORD(pData[0],pData[1]);
                  wTemp = wVal;
                   if(wTemp != g_gRunPara[RP_SENDYC_T])
                   {
                       g_gRunPara[RP_SENDYC_T] = wTemp;
                       g_ucParaChang |= BIT0;
                       SetEqpInfo();
                    }
          
                   break;
                case 0x6014:  //�Լ���Ϣ�ϴ�
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
                   if(wTemp != g_gRunPara[RP_I0_START])  //�ߵ�ѹ��ѹ
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
        case  107: //�ź�Դ
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
       case  122: //�ź�Դ
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
��������:SendCallHistLoad
�����������
���������������
��������˵�������ڿ����ٻ���ʷң�����ݵĲ���
��ע˵����for yn �����Լ�������Լ˵��
���ڣ�2015��10��19��  by��zx
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
��������:SendHistSOE
�����������
���������������,true��ʾ�����ݷ��ͣ�false��ʾ�����ݷ���
��������˵������ȡ�ڲ�Flash�д洢��SOE�����͸���վ��ÿ����෢��16��
              �ڲ���Flashʱ�����κ�����
��ע˵����for yn �����Լ�������Լ˵��
���ڣ�2015��10��19��  by��zx
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
��������:SendCallHistLoadStop
�����������
���������������
��������˵���������ٻ���ʷ���ݽ���֡              
��ע˵����for yn �����Լ�������Լ˵��
���ڣ�2015��10��19��  by��zx
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
��������:SendCallHistLoad
�����������
���������������
��������˵�������ڿ����ٻ���ʷң�����ݵĲ���
��ע˵����for yn �����Լ�������Լ˵��
���ڣ�2015��10��19��  by��zx
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
��������:SendHistSOE
�����������
���������������,true��ʾ�����ݷ��ͣ�false��ʾ�����ݷ���
��������˵������ȡ�ڲ�Flash�д洢��SOE�����͸���վ��ÿ����෢��16��
              �ڲ���Flashʱ�����κ�����
��ע˵����for yn �����Լ�������Լ˵��
���ڣ�2015��10��19��  by��zx
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
//�ŏ| ¼��I0U0
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
��������:SendHistSOE
�����������
���������������,true��ʾ�����ݷ��ͣ�false��ʾ�����ݷ���
��������˵������ȡ�ڲ�Flash�д洢��SOE�����͸���վ��ÿ����෢��16��
              �ڲ���Flashʱ�����κ�����
��ע˵����for yn �����Լ�������Լ˵��
���ڣ�2015��10��19��  by��zx
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

           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] =0;//�ŏ| ¼��I0U0 LOBYTE(g_sRecData.m_gRecAc[CHAN_I0][i + 48]);
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] =0;//�ŏ| ¼��I0U0 HIBYTE(g_sRecData.m_gRecAc[CHAN_I0][i + 48]);
        }
        SendFrameTail(0,0x83,1,0);
        return true;
    }
 //   else
     //   return false;
}
/*******************************************************************
��������:SendCallHistLoadStop
�����������
���������������
��������˵���������ٻ���ʷ���ݽ���֡              
��ע˵����for yn �����Լ�������Լ˵��
���ڣ�2015��10��19��  by��zx
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
��������:SendCallHistLoadStartAck
�����������
���������������
��������˵�������ڷ����ٻ���ʷң���ȷ��֡
��ע˵����for yn �����Լ�������Լ˵��
���ڣ�2015��10��19��  by��zx
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
    /* for(BYTE i = 0 ; i < 7; i++)  //��Ϣ��+��ֹ���ڣ���16���ֽڣ��˴���Ϣ���ַΪ2���ֽ�
     {
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = g_sRecData.m_gFaultRecSOE[REC_MSL + i];
     }*/
     //for(BYTE i = 0 ; i < 7; i++)  //��Ϣ��+��ֹ���ڣ���16���ֽڣ��˴���Ϣ���ַΪ2���ֽ�
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
��������:SendCallHistLoadStartAck
�����������
���������������
��������˵�������ڷ����ٻ���ʷң���ȷ��֡
��ע˵����for yn �����Լ�������Լ˵��
���ڣ�2015��10��19��  by��zx
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
    SendData[2] = 0x40;   //���ݳ��� 
    memcpy(&SendData[3],Sign,0x40);
    SendCmdToEncChip(VERIFY_SIGN,SendData,67);
    if(GetDataFromeEncChip(NULL))
    {
        return TRUE;
    }
    return FALSE; 
}
//���ݼ��� 
WORD CBJ101S::EncryptData(BYTE *pClearData,WORD DataLen,BYTE *pEncData)
{
    WORD count = 0;
    BYTE SendData[256] = {0};//zxx �ڴ濪���Ƿ�С�ˣ����101�ı���ÿ֡���ȶ���250���ֽ�
    SendData[count++] = 0;  //Ԥ���ַ�����
    SendData[count++] = 0;  //Ԥ���ַ�����
    for(BYTE i = 0; i < 8; i++)
    {
        SendData[count++] = g_bHostRandom[i];//zxx ��վ���������δ��ȡ
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
//���ݽ���
WORD CBJ101S::DecryptData(BYTE *EncData,WORD EncDataLen,BYTE *ClearData)
{
    WORD count = 0; 
    BYTE SendData[1024] = {0};
    count  += 2;  //Ԥ��2 �ֽڳ���
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
    BYTE SendData[1024] = {0};  //zx 20170503�޸�
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
    CerTxt[2] = HIBYTE(CerTxtLen - 4);  //��ȥԤ���ĳ���
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
    g_bEncCerTxt[0] = HIBYTE(CerTxtLen - 2);  //zx�޸� 20170503
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
    SendData[1] = HIBYTE(dataLen + 1);  //���ݳ���+ 1���ֽڵ���Կ�汾
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
    SendData[3] = 0;  //��Կ�ָ�ʱ����Կ�汾�̶�Ϊ0
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
    SendCmdToEncChip(TOOL_VERIFY_CER,pCerTxt,CerLen + 2);//���͵�����+ 2�ֽڵĳ���
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
    //+32����Ϊ��8�ֽڹ�����ID��8�ֽ�R1��16�ֽ�IVData
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
    SendCmdToEncChip(TOOL_ENCRYPT_DATA,pOriginalData,DataLen+2);  //+2���ֽ����ݳ���
    return (GetDataFromeEncChip(pEncData));
}
WORD CBJ101S::DecryptToolData(BYTE *pEncData, WORD  EncDataLen,BYTE *pClearData)
{
    //+32����Ϊ��8�ֽڹ�����ID��8�ֽ�R1��16�ֽ�IVData
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
    pOriginalData[1] = LOBYTE(OriginalDataLen - 2);  //��ȥ2���ֽڳ���
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
//��֤�������ն���֤
BOOL CBJ101S::SendGatewayVerify(void)
{
     BYTE RecData[128];
    if(GetChipRandomSign(&pReceiveFrame->FrameEB.Data[2],RecData))
    {
        RecData[72] = 0x06;    //������֤��ǩ����ʶ�̶�Ϊ0x06
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
    //66��ԭ����2�ֽ����ݳ���+64�ֽ�ǩ��
    BYTE asKey = pReceiveFrame->FrameEB.Data[66];  //ZX �޸� 20170503
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
    //66��ԭ����2�ֽ����ݳ���+64�ֽ�ǩ��
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
        if(type == 0)  //���͸���վ
            SendFrameEBHead(0x0000,0x55);
        else   //���͸���ά����
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
    if(type == 0)  //��վ
    {
        if(m_bHostCertifyFlag != 0x55)
        {
            return TRUE;
        }
        SendFrameEBHead(0x0000,0x61);
    }
    else if(type == 1)  //��ά����
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
    BYTE KeyVer;  //��Կ�汾
    BYTE asKID;   //��Կ����
    //��Կ���°�+ǩ����� ��244���ֽ�
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
    BYTE asKID;   //��Կ����
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

BOOL CBJ101S::SendCerUpdataAck(void)  //Զ��֤�����
{
    BYTE FrameNum =  pReceiveFrame->FrameEB.Data[3];
    BYTE FrameNO  =  pReceiveFrame->FrameEB.Data[4];
    BYTE *pData   = &pReceiveFrame->FrameEB.Data[5];
    WORD DataLen  =  MAKEWORD(pReceiveFrame->FrameEB.Data[1], pReceiveFrame->FrameEB.Data[0]) - 3;  //ZX�޸�2017��5��3��
    BYTE KeyID;   //��Կ����
    BYTE CerID;
    BYTE *pCerData;
    BYTE *pSign;
    WORD SignLen;
    WORD CerTxtLen;
    if(FrameNO == 1)  //��һ֡ 
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
    if(FrameNO >= FrameNum) //���һ֡
    {
        //���ܳ���������ǰ��Ԥ��3�ֽ�,
        g_wEncCerWPtr  = DecryptData(g_bEncCerTxt, g_wEncCerWPtr, &g_bEncCerTxt[3]);   //��������
        if(!g_wEncCerWPtr)
        {
            SendEncFrameAck(0x9790,0x0000,0x71);
            return TRUE;
        }
        CerID = g_bEncCerTxt[3];
        pCerData = &g_bEncCerTxt[3];
        CerTxtLen = g_wEncCerWPtr - 71;  //��ȥ71Ϊ1�ֽ���Կ��ʶ��64�ֽ�ǩ����6�ֽ�ʱ����Ϣ
        pSign = &pCerData[CerTxtLen];
        SignLen = 70;  //64�ֽ�ǩ��+ 6�ֽ�ʱ����Ϣ
        KeyID = pSign[SignLen];  //�������ݳ��������һ���ֽ�����Կ��ʶ
        if(m_bHostCertifyFlag != 0x55)
        {
            SendEncFrameAck(0x9790,0x0000,0x71);
            return TRUE;          
        }
        if(!VerifyDataSign(KeyID,pCerData,CerTxtLen,pSign,SignLen))  //��ǩʧ��,�������ݳ���-1����Ϊ��KeyID��ȡ������
        {
            SendEncFrameAck(0x9790,0x0000,0x71);
            return TRUE;
        }
        //���ܳ�����Cer���Ĵӵ�5 �ֽڿ�ʼ����4 �ֽ�ΪCerID, ǰ�滹Ԥ����3 ���ֽ�
        if(UpdataCer(CerID,g_bEncCerTxt,CerTxtLen + 3) == FALSE)  
        {
            
            SendEncFrameAck(0x9790,0x0000,0x71);//֤�����ʧ��
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

BOOL CBJ101S::SendCerDownloadAck(void)  //֤������
{
    BYTE FrameNum = pReceiveFrame->FrameEB.Data[3];
    BYTE FrameNO = pReceiveFrame->FrameEB.Data[4];
    BYTE *pData = &pReceiveFrame->FrameEB.Data[5];
    //WORD DataLen = MAKEDWORD(pReceiveFrame->FrameEB.Data[1], pReceiveFrame->FrameEB.Data[0]);
    WORD DataLen = MAKEWORD(pReceiveFrame->FrameEB.Data[1], pReceiveFrame->FrameEB.Data[0]) - 3;
  
    if(FrameNO == 1)  //��һ֡ 
    {
        g_wEncCerWPtr = 2;   //Ԥ��2�ֽڸ�����
    }
    if(FrameNO == 2)
    {
        FrameNO = 2;
    }
    for(WORD i = 0; i < DataLen; i++)
    {
        g_bEncCerTxt[g_wEncCerWPtr++] = pData[i];
    }
    if(FrameNO >= FrameNum) //���һ֡
    {
        if(m_bHostCertifyFlag != 0x55)
        {
            SendEncFrameAck(0x9790,0x0000,0x73);  //δͨ����֤
            return TRUE;
        }
        if(LoadCer(g_bEncCerTxt, g_wEncCerWPtr))   
        {
            SendEncFrameAck(0x0090,0x0000,0x73);  
            return TRUE;
        }
        else
        {
            SendEncFrameAck(0x9390,0x0000,0x73);//֤�鵼��ʧ��
            return TRUE;
        }
    }
    return TRUE;
}
BOOL CBJ101S::SendCer(BYTE Type)  //��ȡоƬ֤��
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
                if(Type == 0)   //��վͨ��
                {
                    if(m_bHostCertifyFlag != 0x55)
                    {
                        SendEncFrameAck(0x9590,0x0000,0x75);//֤�鵼��ʧ��
                        return TRUE;
                    }                  
                    SendFrameEBHead(0x0000,0x75);
                }
                else   //��ά����ͨ��
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
                if(Type == 0)   //��վͨ��
                {                    
                    SendFrameEBHead(0x0000,0x75);
                }
                else   //��ά����ͨ��
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
    //��ȥ֤���ʶ1�ֽڣ���֡��1�ֽڣ���ǰ֡��1�ֽڣ�֤�������ID8 �ֽ�
    WORD DataLen = MAKEWORD(pReceiveFrame->FrameEB.Data[1], pReceiveFrame->FrameEB.Data[0]) - 11;  
    if(FrameNO == 1)  //��һ֡
    {
        g_wEncCerWPtr = 2;  //Ԥ��ǰ2�ֽڸ�����
    }
    for(WORD i = 0; i < DataLen; i++)
    {
        g_bEncCerTxt[g_wEncCerWPtr++] = pData[i];
    }
    for(BYTE i = 0; i < 8; i++)   //֤������߱���
    {
        m_bCerToolID[i] = pData[DataLen + i];
    }
    if(FrameNO >= FrameNum) //���һ֡
    {
        //g_bEncCerTxt[0] = HIBYTE(g_wEncCerWPtr - 2);
        //g_bEncCerTxt[1] = LOBYTE(g_wEncCerWPtr - 2);
        if(!VerifyToolCer(g_bEncCerTxt, g_wEncCerWPtr-2)) 
        {
            m_bToolCertifyFlag = 0;
            SendEncFrameAck(0x9090,0x0040,0x31);//����֤����֤ʧ��
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
            SendEncFrameAck(0x9090,0x0040,0x31);//�����δ��ȡ
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
    //Ԥ��34���ֽڣ�2�ֽڳ��ȣ�8�ֽڹ�����ID��8�ֽ�R1��16�ֽ�IVDATA
    //��Ԥ��3���ֽ� 2�ֽ���չ�����ȣ�1�ֽڱ�������
    if(!ReadPubKey(&PubKey[37])) 
    {
        //SendEncFrameAck(0x0991,0x0040,0x3B);
        return TRUE;
        //��ȡ��Կʧ��
    }
//    {
//        SendFrameEBHead(0x0040,0x3B);   //AppType����200
//        SendEncData(64,&PubKey[37]);
//        SendFrameEBTail();
//    }
    
    PubKey[34] = 0x3B;  //Ӧ������
    PubKey[35] = 0;
    PubKey[36] = 0x40;   //���ݳ���
    EncDataLen = EncryptToolData(PubKey,67,PubKey);
    if(EncDataLen)
    {
        SendFrameEBHead(0x0048,201);   //AppType����200
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
    if(FrameNO == 1)  //��һ֡
    {
        g_wEncCerWPtr = 2;  //Ԥ��ǰ2�ֽڸ�����
    }
    DataLen = MAKEWORD(pReceiveFrame->FrameEB.Data[1],pReceiveFrame->FrameEB.Data[0]) - 2;
    for(WORD i = 0; i < DataLen; i++)
    {
        g_bEncCerTxt[g_wEncCerWPtr++] = pData[i];
    }
    if(FrameNO >= FrameNum) //���һ֡
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
    SendEncFrameAck(0x0090,0x40,0x45);  //���ط�֡�ӳɹ�
    return TRUE;
}
BOOL CBJ101S::SendLoadCerAck(void)
{
    BYTE CerID = pReceiveFrame->FrameEB.Data[2];
    BYTE FrameNum = pReceiveFrame->FrameEB.Data[3];
    BYTE FrameNO = pReceiveFrame->FrameEB.Data[4];
    BYTE *pData = &pReceiveFrame->FrameEB.Data[5];
    WORD DataLen;
    if(FrameNO == 1)  //��һ֡
    {
        g_wEncCerWPtr = 4;   //��2�ֽ�CerID��2�ֽڳ���
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
    SendEncFrameAck(0x0090,0x0040,0x45);  //��֡���ճɹ�
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
    //�ӵڶ� ���ֽڿ�ʼ��������Ԥ��2�ֽڳ���
    if(!ReadTestCer(RecData, &g_bEncCerTxt[2])) 
    {
        SendEncFrameAck(0x9390,0x0040,0x41);
        return TRUE;
    }
    if(LoadCer(g_bEncCerTxt,CerLen + 2))  //װ��ԭʼ֤��ɹ�
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
    BYTE DataLen = pReceiveFrame->FrameEB.Data[1];  //�������ֽ������ᳬ��256
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
    BYTE AppLen = 0;     //APPӦ�����ݳ���
    BYTE *pAppData;
    BYTE *pEiData;
    WORD FrameLen = MAKEWORD(pReceiveFrame->FrameEB.LengthL,pReceiveFrame->FrameEB.LengthH);
    WORD EiDataLen = 0;  //��չ�����ݳ���
    BYTE asKey;
    BYTE Random[8];
    BYTE EncTime[6];
    BYTE Consel_MD5[16] = {0};
    //BYTE CheckTimeFlag = 0;
    //BYTE CheckTimeCount = 0;
    //DWORD TimeStamp;  //ʱ��������ڶԱ�ʱ��
    memcpy(Random,m_bUnitRandom,8);
    EncTime[0] = (BYTE)((g_sRtcManager.m_gRealTimer[RTC_YEAR]-2000) & 0xFF);
    EncTime[1] = (BYTE)(g_sRtcManager.m_gRealTimer[RTC_MONTH] & 0xFF);
    EncTime[2] = (BYTE)(g_sRtcManager.m_gRealTimer[RTC_DATE] & 0x1F);
    EncTime[3] = (BYTE)(g_sRtcManager.m_gRealTimer[RTC_HOUR] & 0xFF);
    EncTime[4] = (BYTE)(g_sRtcManager.m_gRealTimer[RTC_MINUT] & 0xFF);
    EncTime[5] = (BYTE)(g_sRtcManager.m_gRealTimer[RTC_SEC] & 0xFF);
    //д�뵱ǰʱ��
    //WORD DataSignlen = 0;
    if(AppType <= 0x08)
    {
        //��δ���й���վ��֤���յ�����
        if(m_bHostCertifyFlag != 0x55)
        {
            SendEncFrameAck(0x0791, 0x0000,0x1f);
            return TRUE;
        }
        //�յ�δ���ܵ�68 ֡
        if((pReceiveFrame->FrameEB.Data[1] == 0x68) && (m_bEncDataFlag != 0x55))
        {
            SendEncFrameAck(0x0691, 0x0000,0x1f);
            return TRUE;
        }
    }
    m_bEncDataFlag = 0; //���ݼ��ܱ�־λ����
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
    //���ݴ�ǩ������Ҫ��ǩ
    case 0x01:
    case 0x03:
    case 0x05:
    case 0x07:    //zx�޸� 20170503
    case 0x08:
        AppLen = pReceiveFrame->FrameEB.Data[0];
        if(AppLen)  //AppType Ϊ01 03 05 07
        {
            pAppData = &pReceiveFrame->FrameEB.Data[1];
            EiDataLen = MAKEWORD(pAppData[AppLen + 1],pAppData[AppLen]);
            pEiData  = &pAppData[AppLen + 2];
        }
        else if(AppType == 0x08)       //AppType Ϊ08
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
        //У�����ݳ����Ƿ���ȷ
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
        for(BYTE i = 0; i < 8; i++)//��֤�����
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
        if(EncTime[0] != (BYTE)((g_sRtcManager.m_gRealTimer[RTC_YEAR]-2000) & 0xFF))  //�����
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
        if(EncTime[1] != (BYTE)(g_sRtcManager.m_gRealTimer[RTC_MONTH] & 0xFF))   //�����
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
        if(EncTime[2] != (BYTE)(g_sRtcManager.m_gRealTimer[RTC_DATE] & 0x1F))  //�����
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
        if(VerifyDataSign(asKey,pAppData,AppLen,pEiData,EiDataLen - 1)) //��ǩ�ɹ�  ZX�޸�20170503
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
    case 0x54:    //��վ��ȡ�ն�оƬ���к�
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
    case 0x30:  //��ά���������֤
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
    //m_bHostCertifyFlag = 0x55;  //����ʹ��
//#endif
    if((pReceiveFrame->FrameEB.FrameType[1] & ENC_TURN_BIT) == ENC_ON)   //��֡���ļ���
    {
//        for(BYTE i = 0; i < 4; i++)
//        {
//            m_bHostMac[i] = pReceiveFrame->FrameEB.Data[pReceiveFrame->FrameEB.Data[APPDATA_LEN] + 2 + i];
//        }
        //���ܱ��ĳ���Ϊ���ĳ���- 2 ���ֽڱ�������
        if((pReceiveFrame->FrameEB.FrameType[1] & CONNECT_OBJECT_BIT) == SET_TOOL)  //��ά���� 
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
        //RecCipherTxt();   //�յ���������}
    }
    //else
    {
        RecClearTxt();   //�յ�����������
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
        if(g_EncRxBuf.wBufT >= g_EncRxBuf.wBufH)  //βָ�����ͷָ��
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
            if(FrameLen > MsgLen)   //��ʱδ����ȫ����֡����֡����
            {
                g_EncRxBuf.wCheckFrame += MsgLen;
                g_EncRxBuf.wCheckFrame &= 0x3FF;    //����0x3FF����Ϊ�˷�ָֹ��Խ��
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
