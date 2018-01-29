/*------------------------------------------------------------------------
 Module:      GB104S.cpp
 Author:      linxueqin
 Project:     ��Լƽ̨
 State:       V1.0
 Creation Date:   2012-10-8
 Description:   GB104��Լ--slave
------------------------------------------------------------------------*/
#include "Gb104s.h"
#include "..\Main.h"

#ifdef INCLUDE_GB104_S

BYTE gResetReason;
void CGB104S::Run(void)
{
  RcvData(&m_uartId);
  DoCommSendIdle();
}
 
  /*return value = -1:exit receive thread
   *             = 0 : continue */
  int CGB104S::RcvData(int* iSocket)
  {
      CPSecondary::RcvData(iSocket);
      return 0;
//       int rxlen=0;
//  //     int nLen=1024;
//    
//      NeatenCommBuf(&m_RecBuf);
//   //   nLen = m_RecBuf.wBufSize-m_RecBuf.wWritePtr;
//      //rxlen = pSvrSocket->SocketReceive((BYTE *)(m_RecBuf.pBuf+m_RecBuf.wWritePtr),&nLen,iSocket);
//      //rxlen = Read_SOCK_Data_Buffer(*iSocket, (BYTE *)(m_RecBuf.pBuf+m_RecBuf.wWritePtr));
//      if(rxlen > 0)
//      {
//        StartTimer(6);//when receive tcp data,restart T6 to judge socket idle time
//        m_RecBuf.wWritePtr += (WORD)rxlen;
//        if (m_RecBuf.wWritePtr > m_RecBuf.wBufSize)
//        {
//          m_RecBuf.wWritePtr = 0;
//        }
//        DoReceive();
//        return 0;
//      }
//      else
//        return -1;
  }
  /***************************************************************
    Function: CGB104S
      CGB104S��Ĺ��캯��
    ��������
  
    ���أ���
  ***************************************************************/
  CGB104S::CGB104S() : CPSecondary()
  {
    m_wSendNum = 0; //Send Counter
    m_wRecvNum = 0; //Receive Counter
    m_wAckNum = 0;    //Receive Ack Counter
    m_wAckRecvNum = 0; //Have Ack Receive Counter
    m_bDTEnable = FALSE;    //Data Transfer Enable Flag
    m_bContinueSend = TRUE;
  
    //memset(m_vBackFrame, 0xff, sizeof(m_vBackFrame));
    for(int i=0; i<PARA_K; i++)
    {
      m_wAckTimeOut[i] = 0;
      m_wAckRetry[i] = 0;
      m_vBackFrame[i].wSendNum = 0xFFFF;
    }
    
    m_dwYcSendFlag = 0;
    m_dwYxSendFlag = 0;
    m_dwYcSendFlagH = 0;
    m_dwYxSendFlagH = 0;
    
    m_wSendYcNum = 0;
    m_wSendYxNum = 0;
    m_wSendChangedYcNum = 0;
    m_wSendCosNum = 0;
    m_wSendSoeNum = 0;
    
    m_dwYcGroupFlag = 0;
    m_dwYxGroupFlag = 0;
    m_dwDdGroupFlag = 0;
    m_byYcYxQrp = 0;
    m_byDdQcc = 0;
    
    m_dwSendAllDataEqpNo = 0;
    m_wRecAddress = 0;
    m_wChangeYcEqpNo = 0;
    m_wAllDdEqpNo = 0;
    m_vTimer[1].bRun = FALSE;
    m_vTimer[1].wInitVal = PARA_T1;
    m_vTimer[2].bRun = FALSE;
    m_vTimer[2].wInitVal = PARA_T2;
    m_vTimer[3].bRun = FALSE;
    m_vTimer[3].wInitVal = PARA_T3;
    m_vTimer[4].bRun = FALSE;
    m_vTimer[4].wInitVal = PARA_T4;
    m_vTimer[5].bRun = FALSE;
    m_vTimer[5].wInitVal = PARA_T5;
    m_vTimer[6].bRun = FALSE;
    m_vTimer[6].wInitVal = PARA_T6;
  }
  
  
  /***************************************************************
    Function��Init
      ��Լ��ʼ��
    ������wSocket 
    ���أ�TRUE �ɹ���FALSE ʧ��
  ***************************************************************/
  BOOL CGB104S::Init(WORD wSocket)
  {
    BOOL rc;
    m_uartId = wSocket;
  //m_wSocketStatus = 0;
    rc = CProtocol::Init(1);
    if (!rc)
    {
      return FALSE;
    }
    m_PARA_K = PARA_K;
    m_PARA_W = PARA_W;
    m_bSendInitEnd = 0;
    byChangeYcInitFlag = 0;//��ʶ����ʱ�Ƿ��������ͱ仯ң��
    byCosInitFlag = 1;//��ʶ����ʱ�Ƿ��������ͱ�λ��Ϣ
    bySendAccidentFlag = 1;//ϵͳ����ʱ����λң�ţ��仯SOE���仯ң���˳��
    m_wSendNum = 0; //Send Counter
    m_wRecvNum = 0; //Receive Counter
  
    m_wYkTimeOut = 0;
    m_wYkNo = 0;
    //m_vTimer[4].wInitVal = m_pEqpInfo[m_wEqpNo].wTSendAllYc;

    initpara();
    return TRUE;
  }


  //��������ʱ�ĳ�ʼ�������ò�����ĳ�ʼ��
  void CGB104S::initpara(BYTE flag)
  {
        m_guiyuepara.conaddrlen=g_ucPara101[IECP_COMNADDR_NUM];//2;//������ַ����
        m_guiyuepara.COTlen=g_ucPara101[IECP_TRANSRSN_NUM];//2;//����ԭ�򳤶�
        m_guiyuepara.infoaddlen=g_ucPara101[IECP_INFO_NUM];//3;//��Ϣ���ַ����
        //m_guiyuepara.baseyear=2000;

        ASDUID_LEN = 2 + m_guiyuepara.COTlen + m_guiyuepara.conaddrlen; 

        
  }
  /***************************************************************
    Function�� StartTimer
      ������ʱ��
    ������TimerNo ��ʱ��index
  
    ���أ���
  ***************************************************************/
  void CGB104S::StartTimer(BYTE TimerNo)
  {
    if (TimerNo>TIMER_MAX-1)
        return;
      m_vTimer[TimerNo].wCounter = m_vTimer[TimerNo].wInitVal;
      m_vTimer[TimerNo].bRun = TRUE;  
  }
  /***************************************************************
    Function�� StopTimer
      ֹͣ��ʱ��
    ������TimerNo ��ʱ��index
  
    ���أ���
  ***************************************************************/
  void CGB104S::StopTimer(BYTE TimerNo)
  {
    if (TimerNo>TIMER_MAX-1)
        return;
      m_vTimer[TimerNo].bRun = FALSE; 
  }
  
  /***************************************************************
    Function�� DoTimeOut
      ��ʱ����ʱ�ж�
    ������wTimerID ��ʶ��TM_1S��ʱ������TM_100MS��ʱ��
  
    ���أ���
  ***************************************************************/
  BOOL CGB104S::DoTimeOut(WORD wTimerID)
  {
      //CPSecondary::DoTimeOut(wTimerID);
  
    /*t1ʱ������վδ����վ���͵�i֡ȷ����վӦ������*/
    if(IsAckTimeout()){
      CloseTcp(COI_IACKTIMEOUT);
    }
    if(m_YtWaitCount > 0)
      {//ң������
        m_YtWaitCount--;
        if(g_YtOrderFlag == TRUE ||g_YtOrderFlag == FALSE)
          m_YtWaitCount = 0;
        if(m_YtWaitCount <= 0)
        {
          m_YTflag=1;
        }
      }
    if(m_YkWaitCount > 0)
    {//ң�ش���
      m_YkWaitCount--;
      if(g_YkOrderFlag == TRUE ||g_YkOrderFlag == FALSE)
        m_YkWaitCount = 0;
      if(m_YkWaitCount <= 0)
      {
        m_YKflag=1;
      }
    }
    if(m_wYkTimeOut>0){
      m_wYkTimeOut--;
      if(m_wYkTimeOut==0) m_wYkNo = 0;
    }
  
    if (m_vTimer[1].bRun)
      {
        m_vTimer[1].wCounter--;
        if (m_vTimer[1].wCounter == 0)
        {
//            printf("iec104 slave T1 timeout,close TCP!\n");
            //when T1 timeout,close tcp connect
            CloseTcp(COI_UTIMEOUT);
                    
            StopTimer(1);
        }
      }
      if (m_vTimer[2].bRun)
      {
        m_vTimer[2].wCounter--;
        if (m_vTimer[2].wCounter == 0)
        {
            //when T2 timeout, send S frame
            SendSFrame();
            StopTimer(2);
      }
      }
      if (m_vTimer[3].bRun)
      {
        m_vTimer[3].wCounter--;
        if (m_vTimer[3].wCounter == 0)
        {
            //when T3 timeout send u frame for test
            SendUFrame(TESTFR_ACT);
            StopTimer(3);
          }
      }
        if (m_vTimer[4].bRun)
          {
            m_vTimer[4].wCounter--;
            if (m_vTimer[4].wCounter == 0)
            {
                //when T4 timeout, send all yc
              StartTimer(4);
              SendAllYcYx(1);
  
          }
          }
          if (m_vTimer[5].bRun)
          {
            m_vTimer[5].wCounter--;
            if (m_vTimer[5].wCounter == 0)
            {
//              //judge net status
//                int netSt = 0;
//                if(this->pSvrSocket->m_chObj == 'B')
//                  netSt = get_netlink_status("eth1");
//                else
//                  netSt = get_netlink_status("eth0");
//                if(netSt != 1 )
//                {
//                  printf("%c netlink down,close TCP!\n",this->pSvrSocket->m_chObj);
//                  CloseTcp();
//                }
                StartTimer(5);
              }
          }
        if (m_vTimer[6].bRun)
      {
        m_vTimer[6].wCounter--;
        if (m_vTimer[6].wCounter == 0)
        {//when T6 timeout,close tcp connect
//          printf("\n Tcp channel has %ds idle,closing tcp\r\n",m_vTimer[6].wInitVal);
          CloseTcp(COI_NETIDLETIMEOUT);
          StopTimer(6);
        }
      }
  return FALSE;    
  }
  
  /***************************************************************
    Function�� SendYkReturn
      ң�ط���
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendYkReturn(void)
  {
    BYTE Reason = COT_ACTCON;
    BYTE Qualifier = 1;
    VYKInfo *pYKInfo;
    VASDU *pSendAsdu;
      
    pYKInfo = &(pGetEqpInfo(m_wEqpNo)->YKInfo);
    
    if (!CanSendIFrame())
        return;
    if (pYKInfo->Head.byMsgID == MI_YKCANCEL)
    {
      Reason = COT_DEACTCON;  
    }
    SendFrameHead(FRM_I);
    
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = pYKInfo->Head.byTypeID;
    pSendAsdu->byQualifier = Qualifier;
    write_infoadd(pYKInfo->Head.wInfoAddr);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.byDco;//DCO
    if (pYKInfo->Info.byStatus != 0)
    {
      /* R225: �Ƿ�ң����ȷ����ԭ�� */
      if(pYKInfo->Info.byStatus == 1) 
        Reason = COT_PN_BIT|47; /*COT_E_IADDR*/
      else
        Reason = COT_PN_BIT|COT_ACTTERM;
    }
    write_COT(Reason);
    SendFrameTail();  
  
    /*ִ�н�������*/
    if (pYKInfo->Head.byMsgID == MI_YKOPRATE && pYKInfo->Info.byStatus == 0){
      //OSTimeDlyHMSM(0,0,0,25);
      if (!CanSendIFrame()) return;
  
      SendFrameHead(FRM_I);
  
      pSendAsdu = (VASDU *)m_pSend->byASDU;
      pSendAsdu->byTypeID = pYKInfo->Head.byTypeID;
      Reason = COT_ACTTERM;
      pSendAsdu->byQualifier = Qualifier;

      write_infoadd(pYKInfo->Head.wInfoAddr);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYKInfo->Head.byDco;//DCO
      write_COT(Reason);
      SendFrameTail();  
    }
  }
  
  /***************************************************************
    Function�� DoCommSendIdle
      ����ʱ��������
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::DoCommSendIdle(void)
  {
    if(m_YTflag == 1)
    {//ң������
      m_YTflag = 0;
      if(g_YtOrderFlag == TRUE)
        SendYtReturn(0);
      else
        SendBack(COT_ACTCON|COT_PN_BIT);
    }
    if(m_YKflag == 1)
    {//ң�ش���
      m_YKflag = 0;
      if(g_YkOrderFlag == TRUE)
        SendYkReturn();
      else
        SendBack(COT_ACTCON|COT_PN_BIT);
    }

    if(m_bSendInitEnd)
    {
      //if(SendInitEnd())//UCOS���Է����г�ʼ����������ʱ��������쳣����ȥ����֡����
      {
        m_bSendInitEnd = 0;
        StartTimer(4);//��ʼ��������������ʱ����ȫң��
      }
    }
  
    if( SendAckRetry(m_pEqpInfo[0].wYxType, COT_SPONT) )
      return;
    if( SendAckRetry(m_pEqpInfo[0].wSoeType, COT_SPONT) )
      return;
    if( SendAckRetry(GB104S_YC_TYPE, COT_SPONT) )/*�Ƿ��ش��仯ң�⣿����*/
      return;
  
    if (m_dwYcSendFlag || m_dwYxSendFlag || m_dwYcSendFlagH || m_dwYxSendFlagH)
    {
      SendAllYcYx(0);
      return; 
    }
    
    if (m_dwYcGroupFlag || m_dwYxGroupFlag)
    {
      SendSomeYcYx();
      return; 
    }
    CycleSendData();
      
  }
  
  /***************************************************************
    Function�� CloseTcp
      �ر�����  ��λ������   ����tcp���� server
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::CloseTcp(BYTE byReason)
  {
    StopTimer(1);
    StopTimer(2);
    StopTimer(3);
    StopTimer(4);
    
    m_bDTEnable = FALSE;
    
    m_wSendNum = 0; //Send Counter  
    m_wRecvNum = 0; //Receive Counter
    m_wAckNum = 0;    //Receive Ack Counter
    m_wAckRecvNum = 0; //Have Ack Receive Counter
    m_bSendInitEnd = 0;//initial end symbol,Call all end
  
    /*��λң�ţ��仯ң��*/
  //  int i=0;
  //   for (i=0;i < m_pEqpInfo[m_wEqpNo].wSYXNum;i++)
  //   {
  //    m_pEqpInfo[m_wEqpNo].OldYx[i]=0;
  //   }
  //   for (i=0;i<m_pEqpInfo[m_wEqpNo].wYCNum;i++)
  //   {
  //     m_pEqpInfo[m_wEqpNo].OldYc[i] = 0;
  //   }
    /*��λ��λң�ţ��仯soe���仯ң���ʶ */
     byChangeYcInitFlag = 1;
     byCosInitFlag = 1;
     bySendAccidentFlag = 1;
     /*����soe���λ�ø�λ */
     m_SoePos = 0;
  
  
    /*��������ʱ��δ��ȷ�ϵ�I֡������Ϣ��0������*/
  //  for(int i=0; i<PARA_K; i++)
  //  {
  //    m_wAckTimeOut[i] = 0;
  //    m_wAckRetry[i] = 0;
  //    m_vBackFrame[i].wSendNum = 0xFFFF;
  //  }
  
//    if(SOCK_CLOSED == byReason
//        || SOCK_FIN_WAI == byReason)
//        gResetReason = COI_NETFT;
//    else if(SOCK_LISTEN == byReason)
//        gResetReason = COI_PEERCLOSE;
//    else
//        gResetReason = byReason;
//    if(m_wSocketStatus == 1)
//    {
//        Write_SOCK_1_Byte(m_AptSocket,Sn_CR,DISCON);    // DISCON Socket 0 /
//        Write_SOCK_1_Byte(m_AptSocket,Sn_CR,CLOSE);   // DISCON Socket 0 /
//      m_wSocketStatus = 0;
//    }
  }
  
  /***************************************************************
    Function�� DoReceive
      ���Ľ��պ���������ʽ����tcp����ʱ�ŷ���
    ��������
  
    ���أ���
  ***************************************************************/
  BOOL CGB104S::DoReceive()
  {
    while(1)
    {//�Ҳ��������򷵻أ����������������
      //m_SendBuf.wWritePtr = 0;
      //m_SendBuf.wReadPtr = 0;
      if (SearchFrame() != TRUE)
      {
        return FALSE;//û�ҵ���Ч����
      }
  
      m_pReceive = (VIec104Frame *)m_RecFrame.pBuf;
  
      m_pASDU = (VASDU *)&m_pReceive->byASDU;
      m_pASDUInfo = (BYTE *)&(m_pASDU->byInfo[m_guiyuepara.COTlen + m_guiyuepara.conaddrlen]);//(BYTE *)&m_pASDU->byInfo;
  
      StartTimer(3);  //receive any I,U,S frame, to start T3
      //to do :proc ASDUs
      switch (m_pReceive->byControl1 & 0x03)
      {
        case FRM_I:
          DoIFrame();
          break;
        case FRM_S:
          DoSFrame();
          break;
        case FRM_U:
          DoUFrame();
          break;
        default:
          DoIFrame();
          break;
      }
  
      if (((m_wSendNum + 1) - m_wAckNum) >= m_PARA_K)
      {
        m_bContinueSend = FALSE;
//        printf("k para overload\r\n");
      }
      else
      {
        m_bContinueSend = TRUE;
      }
    }
       
  }
  
  /***************************************************************
    Function�� SearchOneFrame
      Ѱ����ȷ��һ֡����
    ������ Buf ������������  len ���������ݵ��ܳ���
  
    ���أ����ر�֡�ܳ���
  ***************************************************************/
  DWORD CGB104S::SearchOneFrame(BYTE *Buf, WORD Len)
  {
    DWORD Rc;
    BYTE FrameLen;
    
    
    m_pReceive=(VIec104Frame *)Buf;
      
      if (Len < MIN_RECEIVE_LEN)
        return FRAME_LESS;
        
      Rc = SearchHead(Buf,Len,0,START_CODE);
      if (Rc)
         return FRAME_ERR|Rc;
  
    FrameLen = m_pReceive->byAPDULen;
    
    if (FrameLen < 4)
      return FRAME_ERR | 1;
      
      if (FrameLen > (MAX_FRAME_LEN - 2))
      return FRAME_ERR|1;
      
      if ((FrameLen + 2) > Len)
      return FRAME_LESS;
      
      m_pASDU = (VASDU *)&m_pReceive->byASDU;
      
      if (FrameLen > (CONTROL_LEN + ASDUID_LEN))
      {
      GetAsduId();
        m_wRecAddress  = m_ConAddr;
        /*�жϹ�����ַ*/
//        if (SwitchToAddress(m_wRecAddress) != TRUE)
//          return FRAME_ERR|1;
  
    }
    return FRAME_OK | (FrameLen + 2); //���ر�֡�ܳ���
  }
  
  /***************************************************************
    Function�� DoIFrame
      ����I��ʽ����
    ������ ��
  
    ���أ���
  ***************************************************************/
  void CGB104S::DoIFrame(void)
  {
    WORD wAckNum, wRecvNum;
    
    StartTimer(2);  //when receive I frame,start T2
    wAckNum = MAKEWORD(m_pReceive->byControl3, m_pReceive->byControl4);
    wRecvNum = MAKEWORD(m_pReceive->byControl1, m_pReceive->byControl2);
    wAckNum >>= 1;
    wRecvNum >>= 1;
  
    /*STOPDT ����վ�յ���վ���͵� I ֡Ӧ������*/
    if(m_bDTEnable==FALSE) {
//      printf("STOPDT rcv I,close TCP!\n");
      CloseTcp(COI_STOPDT_RCVI);
      return;
    }
    /*������������վ�������кŲ����㿪ʼ�������*/
    /*��վ�������кŲ�������Ҷ�*/
    if(m_wRecvNum != wRecvNum){
//      printf("m_wRecvNum != wRecvNum,close TCP!\n");
      CloseTcp(COI_RCVNUMERR);
      return;
    }
  
    m_wRecvNum = wRecvNum + 1;
    
    m_wAckNum = wAckNum; 
    DelAckFrame(m_wAckNum - 1);           
      
    StopTimer(1); //stop T1 
    
    
    if ((m_wRecvNum - m_wAckRecvNum) > m_PARA_W)
    {
        SendSFrame();        
    }        
  
    /*�жϹ�����ַ*/
    if(m_wRecAddress==0xFFFF && (m_pASDU->byTypeID==C_IC_NA || 
                                 m_pASDU->byTypeID==C_CI_NA || 
                                 m_pASDU->byTypeID==C_CS_NA || 
                                 m_pASDU->byTypeID==C_RP_NA) )
      m_wRecAddress =  GetOwnAddr();
  //  else if (SwitchToAddress(m_wRecAddress) != TRUE && m_pASDU->byTypeID!=C_CS_NA){
  //    SendBack(COT_E_CADDR);
  //    return;
  //  }
    
    switch (m_pASDU->byTypeID)  
    {
      case C_SC_NA:
        case C_DC_NA:
        case C_RC_NA://101 yk
            DoYk();
            break;
        case C_IC_NA://call all/call group
            DoCallYcYxData();
            break;
         case C_CS_NA://set time
          DoSetTime();
          break;
        case C_RD_NA://read data
            DoReadData();
            break;
        case C_RD_NA_1://read some data
         // DoReadMultiData();
          break;
        case C_TS_NA://test data link
            DoTestLink();
            break;
        case C_RP_NA://reset rtu
            DoReset();
            break;
        case C_SE_TD://multipoint set
//          DoMultiYtCommand();
          break;
        case C_SE_NA:
        case C_SE_NB:
          DoYtCommand();
          break;   
      default:
        SendBack(COT_E_TYPE| COT_PN_BIT);
        break;
    }
  }
  /***************************************************************
    Function�� SendBack
      ����һ֡���� �����������������쳣ʱ���ô˺�����������վ����ԭ��
    ������ Reason ����ԭ��
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendBack(BYTE Reason)
  {
    VASDU * pSendAsdu;
  
    if (!CanSendIFrame())
      return;
    
    SendFrameHead(FRM_I);
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = m_pASDU->byTypeID;
    
    pSendAsdu->byQualifier = m_pASDU->byQualifier;

    int info_len;
    info_len = m_pReceive->byAPDULen-4-ASDUID_LEN;
    if(info_len<0)    info_len = 0;
    if(info_len>243)  info_len = 243;
  
    for(int i=0; i<info_len; i++)
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_pASDUInfo[i];
    write_COT(Reason);  
    SendFrameTail();
  }
  /***************************************************************
    Function�� DoSFrame
      ����S��ʽ����
    ������ ��
  
    ���أ���
  ***************************************************************/
  void CGB104S::DoSFrame(void)
  {
    WORD wAckNum;
    wAckNum = MAKEWORD(m_pReceive->byControl3, m_pReceive->byControl4); 
    m_wAckNum = wAckNum >> 1;
    DelAckFrame(m_wAckNum - 1);
    StopTimer(1); //stop T1 
    
    return;
  }
  
  /***************************************************************
    Function�� DoUFrame
      ����U��ʽ����
    ������ ��
  
    ���أ���
  ***************************************************************/
  void CGB104S::DoUFrame(void)
  {
    switch (m_pReceive->byControl1)
    {
        case STARTDT_ACT:
            m_bDTEnable = TRUE;
            /*m_wSendNum = 0;
            m_wRecvNum = 0;
            m_wAckNum = 0;
            */
            m_bContinueSend = TRUE;//  important
  
            SendUFrame(STARTDT_CON);
            m_bSendInitEnd = 1;
            break;
        case STOPDT_ACT:
          StopTimer(1);
        StopTimer(2);
        /*stopdt����վ��������֡*/
        StopTimer(3);
        StopTimer(4);
            m_bDTEnable = FALSE;
            SendUFrame(STOPDT_CON);
            break;
        case TESTFR_ACT:
            SendUFrame(TESTFR_CON);
            break;
        case TESTFR_CON:
            StopTimer(1); //stop T1
            break;    
    }
    return;
  }
  
  /***************************************************************
    Function: DoYk
      ����ң������
    ������
  
    ���أ���
  ***************************************************************/
  void CGB104S::DoYk(void)
  {
      BYTE * pData = m_pASDUInfo;
    WORD  DCO;  //ң�������޶���
    WORD  YkNo; //ң��·��
    switch (m_guiyuepara.infoaddlen )
    {
        case 1:
            YkNo = pData[0];
            DCO = pData[1];
            break;
        case 2:
            YkNo = MAKEWORD(pData[0], pData[1]);
            DCO = pData[2];
            break;
        default:
            YkNo = MAKEWORD(pData[0], pData[1]);//���ֽ�ǿ��Ϊ0
            DCO = pData[3];
            break;
    }
    VYKInfo *pYKInfo;
    pYKInfo = &(pGetEqpInfo(m_wEqpNo)->YKInfo);
    pYKInfo->Head.byTypeID = m_pASDU->byTypeID;
    pYKInfo->Head.wInfoAddr = YkNo;
    pYKInfo->Head.byDco= DCO;//DCO
     
    /*ASDU45 ң������ cot ������վ���񶨣���ظ� COT �� 45 �ķ�*/
    if (m_Cot != COT_ACT && m_Cot != COT_DEACT){
      SendBack(COT_E_REASON | COT_PN_BIT);
      return;
    }
    //proc iec104 yk frame and make yk info
        switch (m_pASDU->byTypeID)
        {
          case C_SC_NA:
          case C_DC_NA: 
          //case C_RC_NA://��ѹ����Ϣ���ַ��ң����ͬһ��Χ
          if(YkNo<ADDR_YK_LO || YkNo>ADDR_YK_HI){
            SendBack(COT_E_IADDR| COT_PN_BIT);
            return;
          }
            YkNo = YkNo - ADDR_YK_LO + 1;//ykno minimum value = 1
            break;
        }
        pYKInfo->Info.wID = YkNo;
      /*ASDU45ң�ص�ȡ����վ�ظ�Ӧ��COT��9������cot��47*/
      if (m_Cot == COT_ACT)
      {
        switch (DCO & 0x80)
        {
          case 0x80://ѡ��
            pYKInfo->Head.byMsgID = MI_YKSELECT;
            /*ASDU45 ���� 2 ��ͬһ ioa ��ѡ�������*/
            /*ASDU45 ң���в���ң�ض��µ�ң�ز���*/
            if(m_wYkNo == 0) {
              m_wYkNo = YkNo;
              m_wYkTimeOut = 15; /**15��ң��Ԥ�ó�ʱ*/
            }
            else {
              SendBack(COT_ACTCON|COT_PN_BIT);
              return;
            }
            break;
          case 0x00://ִ��
            pYKInfo->Head.byMsgID = MI_YKOPRATE;
            /*�ٷ���һ ioa ��ִ��Ҳ����*/
            if(m_wYkNo != YkNo){
              SendBack(COT_ACTCON|COT_PN_BIT);
              return;
            }
            m_wYkNo = 0;
            break;
          default:
            break;
        }
      }
        if (m_Cot == COT_DEACT)//����
        {
          pYKInfo->Head.byMsgID = MI_YKCANCEL;
        m_wYkNo = 0;
  
        }
      if(m_pASDU->byTypeID!=C_SC_NA)//˫��ң�ء���ѹ
      {
        switch (DCO & 0x03)
        {
          case 0:
          case 3:
            /*˫��ң��DCS��0��3����Ӧ��Ӧ��*/
            SendBack(COT_ACTCON|COT_PN_BIT);
            return;
          case 1:
            pYKInfo->Info.byValue = 0;//0x06;
            break; //��
          case 2:
            pYKInfo->Info.byValue = 1;//0x05;
            break; //��
          default:
            break;
        }
      }
      if(m_pASDU->byTypeID==C_SC_NA)//����ң��
      {
        if(DCO&1)
          pYKInfo->Info.byValue = 1;//0x05; //��
        else
          pYKInfo->Info.byValue = 0;//0x06;  //��
      }
  
      /*���յ�ң�������Ĵ���*/
      if(pYKInfo->Head.byMsgID == MI_YKSELECT)
      {
        SendYkReturn();//ң��ѡ��ֱ�ӻظ�
      }
      else if(pYKInfo->Head.byMsgID == MI_YKOPRATE)
      {
        //if(pYKInfo->Info.byValue == 1)
          //InsertLcdMbCmd(5, REG_RC_BASE + g_ucYKAddr[pYKInfo->Info.wID - 1] - 1, 0);
          //InsertLcdMbCmd(5, REG_RC_BASE + + 0x10 + g_ucYKAddr[pYKInfo->Info.wID - 1] - 1, 0);
          m_YkWaitCount = 5;//���ȴ�5��
          g_YkOrderFlag = ON;
      }
      else if(pYKInfo->Head.byMsgID == MI_YKCANCEL)
      {
        SendBack(COT_DEACTCON);
      }
    
    return;
  }
  
  /*�������ٱ���*/
  void CGB104S::DoCallYcYxData(void)
  {
    /*����� ioa ���ٻ�������վӦ�� COT��47 �� 7 �ķ���Ӧ*/
    if (m_InfoAdd!=0){
      SendBack(COT_E_IADDR| COT_PN_BIT);
      return;
    }
    /*�����cot���ٻ�������վ����Ӧ��������վ�ظ�cot��45�ķ�����*/
    if(m_Cot != COT_ACT && m_Cot != COT_DEACT){
      SendBack(COT_E_REASON| COT_PN_BIT);
      return;
    }
    if (m_pASDUInfo[m_guiyuepara.infoaddlen] == COT_INTROGEN)   //���ٻ�
    {
      switch (m_Cot)
      {
        case COT_ACT:   //���ٻ����Ӧ���������� ����ң�⡢ң�ţ����������
          m_dwYcSendFlag = 0xffffffff;
          m_dwYxSendFlag = 0xffffffff;
          m_dwYcSendFlagH = 0xffffffff;
          m_dwYxSendFlagH = 0xffffffff;
          if (CanSendIFrame())
          {
            SendCallYcYxAck();
          }
          break;
        case COT_DEACT://ֹͣ����
          m_dwYcSendFlag = 0;
          m_dwYxSendFlag = 0;
          m_dwYcSendFlagH = 0;
          m_dwYxSendFlagH = 0;
          /*�������ٻ���ȡ��������ȷ�Ϻ�����ֹ*/
          SendBack(COT_DEACTCON| COT_PN_BIT);
          break;
        }
    }
    if  ((m_pASDUInfo[m_guiyuepara.infoaddlen] >= COT_INRO1) && (m_pASDUInfo[m_guiyuepara.infoaddlen] <= COT_INRO8))
    {
      m_dwYxGroupFlag = m_dwYxGroupFlag | (1 << (m_pASDUInfo[m_guiyuepara.infoaddlen] - COT_INRO1)) | 0x80000000;
      if (CanSendIFrame())
      {
        m_byYcYxQrp = m_pASDUInfo[m_guiyuepara.infoaddlen];
        SendSomeYcYxAck(m_pASDUInfo[m_guiyuepara.infoaddlen]);
      }
    }
    if ((m_pASDUInfo[m_guiyuepara.infoaddlen] >= COT_INRO9) && (m_pASDUInfo[m_guiyuepara.infoaddlen] <= COT_INRO12))
    {
      m_dwYcGroupFlag = m_dwYcGroupFlag | (1 << (m_pASDUInfo[m_guiyuepara.infoaddlen] - COT_INRO9)) | 0x80000000;
      if (CanSendIFrame())
      {
        m_byYcYxQrp = m_pASDUInfo[m_guiyuepara.infoaddlen];
        SendSomeYcYxAck(m_pASDUInfo[m_guiyuepara.infoaddlen]);
      }
    }
      return;
  }
  
  /***************************************************************
    Function: DoReadData
      �������������֡����=102��
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::DoReadData(void)
  {
    WORD wDataAddr;
  
    wDataAddr = MAKEWORD(m_pASDUInfo[0], m_pASDUInfo[1]);
  
    if ((wDataAddr >= ADDR_YX_LO) && (wDataAddr <= ADDR_YX_HI))
    {
      SendReadYx(wDataAddr - 1);
    }
    else if ((wDataAddr >= ADDR_YC_LO) && (wDataAddr <= ADDR_YC_HI))
    {
      SendReadYc(wDataAddr - ADDR_YC_LO);
    }
//    else if((wDataAddr >= REG_PC_BASE) && (wDataAddr < (REG_PC_BASE + PROC_CNT_NUM)))//������ֵ
//    {
//      SendReadPara(g_gProcCnt[wDataAddr - REG_PC_BASE],wDataAddr);
//    }
//    else if((wDataAddr >= REG_RP_BASE) && (wDataAddr < (REG_RP_BASE + RUN_PARA_NUM))) //���в���
//    {
//        SendReadPara(g_gRunPara[wDataAddr - REG_RP_BASE],wDataAddr);
//    }
//    else if((wDataAddr >= REG_YC_SIQU_BASE) && (wDataAddr < (REG_YC_SIQU_BASE + IEC_YC_NUM))) //ң������
//    {
//        SendReadPara(g_unYCSiqu[wDataAddr - REG_YC_SIQU_BASE],wDataAddr);
//    }
    else
    {
      SendNoData();
    }
    return;
  }
  /***************************************************************
    Function: DoReadMultiData
      �������������֡����=138��
    ��������
  
    ���أ���
  ***************************************************************/
//  void CGB104S::DoReadMultiData(void)
//  {
//    WORD wDataAddr;
//    WORD wDataNum;
//    WORD wDataAllNum;
//    WORD wIecAddrL;
//    WORD wFramDataNum = 119;/* (255-6-6-3-1)/2 = 119*/
//  
//    wDataAddr = MAKEWORD(m_pASDUInfo[0], m_pASDUInfo[1]);
//    wDataNum = MAKEWORD(m_pASDUInfo[3], m_pASDUInfo[4]);
//  
//    if ((wDataAddr >= ADDR_COLPARA_LO) && (wDataAddr <= ADDR_COLPARA_HI))
//    {/*�����Ʋ���  ������dpram��*/
//      wIecAddrL = ADDR_COLPARA_LO;
//      //wDataAllNum = m_pEqpInfo[m_wEqpNo].wColParaNum;
//    }
//    else if ((wDataAddr >= ADDR_YCPARA_LO) && (wDataAddr <= ADDR_YCPARA_HI))
//    {/*��ң����� ������ͨ�Ű�flash��*/
//      wIecAddrL = ADDR_YCPARA_LO;
//      //wDataAllNum = m_pEqpInfo[m_wEqpNo].wYcPaNum;
//    }
//    else if ((wDataAddr >= ADDR_SYSPARA_LO) && (wDataAddr <= ADDR_SYSPARA_HI))
//    {/*��ϵͳ���� ������ͨ�Ű�flash��*/
//      wIecAddrL = ADDR_SYSPARA_LO;
//      //wDataAllNum = m_pEqpInfo[m_wEqpNo].wSysPaNum;
//    }
//    else
//    {
//      SendNoData();
//      return;
//    }
//    if(wDataNum > wFramDataNum)
//    {
//      SendBack(COT_REQ| COT_PN_BIT);
//      return;
//    }
//    if(wDataNum > (wDataAllNum - (wDataAddr - wIecAddrL)) )
//      wDataNum = wDataAllNum - (wDataAddr - wIecAddrL);
//    SendReadMultiPara((wDataAddr - wIecAddrL),wIecAddrL,wDataNum);
//  
//  
//      return;
//  }
//  
  
  /***************************************************************
    Function: DoTestLink
      ����������·���ģ�֡����=104��
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::DoTestLink(void)
  {
    BYTE TypeID = C_TS_NA;
    BYTE Reason = COT_ACTCON;
    BYTE Qualifier = 1;
    VASDU * pSendAsdu;
    
    if (!CanSendIFrame())
      return;
    
    SendFrameHead(FRM_I);
    
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
    
    pSendAsdu->byQualifier = Qualifier;
     
    write_infoadd(0);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0xAA; 
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x55;
    write_COT(Reason);
    SendFrameTail();
      return;
  }
  
  /***************************************************************
    Function: SendInitEnd
      ���ͳ�ʼ���������� �������Ӻ��豸���������ͣ�U֡����֮��
    ��������
  
    ���أ�TRUE �ɹ���FALSE ʧ��
  ***************************************************************/
  BOOL CGB104S::SendInitEnd(void)
  {
    BYTE TypeID = M_EI_NA;
    BYTE Reason = COT_INIT;
    BYTE Qualifier = 1;
    VASDU * pSendAsdu;
    
    if (!CanSendIFrame())
      return FALSE;
    
    SendFrameHead(FRM_I);
    
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
    
    pSendAsdu->byQualifier = Qualifier;
    write_infoadd(0);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = gResetReason;
    write_COT(Reason); 
    SendFrameTail();
      return TRUE;
  }
  /***************************************************************
    Function: DoReset
      ������λ��·���� type=105
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::DoReset(void)
  {
      /*��ȷ�Ϻ�����*/
      BYTE TypeID = C_RP_NA;
      BYTE Reason = COT_ACTCON;
      BYTE Qualifier = 1;
      VASDU * pSendAsdu;
      
      BYTE * pData = m_pASDUInfo;
      
  #ifndef INCLUDE_DF104FORMAT
        pData += 3;  
  #else
        pData += 2;  
  #endif
      
      if (!CanSendIFrame())
        return;
      
      SendFrameHead(FRM_I);
      
      pSendAsdu = (VASDU *)m_pSend->byASDU;
      pSendAsdu->byTypeID = TypeID;
      pSendAsdu->byQualifier = Qualifier;
      write_infoadd(pData[0]);
  
      int err = 0;
      /*�� cot �� caa �������*/
      if(m_Cot != COT_ACT){
        Reason = COT_E_REASON;
        err = 1;
      } 
      /*ASDU105 ��ioa�������*/
      if (m_pASDUInfo[0]!=0 || m_pASDUInfo[1]!=0 || m_pASDUInfo[2]!=0){
        Reason = COT_E_IADDR;
        err = 1;
      }
      write_COT(Reason);
      SendFrameTail();
      if(err) return;
  
    sysRestart(COI_RESETRTU);//warm reset
      return;    
  }
  
  /***************************************************************
    Function: SendSFrame
      ����S��ʽ����
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendSFrame(void)
  {
      SendFrameHead(FRM_S);
      SendFrameTail();
      StopTimer(2); //when send S frame, stop T2
      return;    
  }
  
  /***************************************************************
    Function: SendUFrame
      ����U��ʽ����
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendUFrame(BYTE byControl)
  {
      SendFrameHead(FRM_U);
      m_pSend->byControl1 = byControl;
      SendFrameTail();
      if ((byControl == TESTFR_ACT) || (byControl == STARTDT_ACT) || (byControl == STOPDT_ACT))
      {
        StartTimer(1);    //statr T1  
      }
      return;    
  }
  
  /***************************************************************
    Function: SendFrameHead
      104������֯֡ͷ��������ţ�������ţ�������ַ�����жϲ���k
    ������byFrameType ��ʶI֡��U֡����S֡
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendFrameHead(BYTE byFrameType)
  {
      WORD wSendNum, wRecvNum;
      //VASDU * pSendAsdu;
    m_SendBuf.wReadPtr = m_SendBuf.wWritePtr = 0;//��շ��ͻ�����
    
    m_pSend = (VIec104Frame *)m_SendBuf.pBuf;
    //pSendAsdu = (VASDU *)m_pSend->byASDU;
    
    m_wSendNum %= (MAX_FRAME_COUNTER+1);
    m_wRecvNum %= (MAX_FRAME_COUNTER+1);
    wSendNum = m_wSendNum << 1;
    wRecvNum = m_wRecvNum << 1;
    
    m_pSend->byStartCode = START_CODE;
    
    switch (byFrameType)
    {
        case FRM_I:
            m_pSend->byControl1 = LOBYTE(wSendNum);
              m_pSend->byControl2 = HIBYTE(wSendNum);
              m_pSend->byControl3 = LOBYTE(wRecvNum);
              m_pSend->byControl4 = HIBYTE(wRecvNum);
              m_wAckRecvNum = m_wRecvNum;
              if (((m_wSendNum + 1) - m_wAckNum) >= m_PARA_K)
              {
                  m_bContinueSend = FALSE;    
                  //printf("%d--%d,%d,k para overload 0\r\n",this,m_wSendNum,m_wRecvNum);
              }
              else
              {
                  m_bContinueSend = TRUE;    
              }
            break;
        case FRM_S:
            m_pSend->byControl1 = FRM_S;
              m_pSend->byControl2 = 0;
              m_pSend->byControl3 = LOBYTE(wRecvNum);
              m_pSend->byControl4 = HIBYTE(wRecvNum);
              m_wAckRecvNum = m_wRecvNum;
            break;
        case FRM_U: 
            m_pSend->byControl1 = FRM_U;
              m_pSend->byControl2 = 0;
              m_pSend->byControl3 = 0;
              m_pSend->byControl4 = 0;   
            break;
        default:
            break;
    }
  if(FRM_I == byFrameType)
      write_conaddr(GetOwnAddr());
  else
    m_SendBuf.wWritePtr = APCI_LEN;
    return;
  }
  
  /***************************************************************
    Function: SendFrameTail
      ���㱨�ĳ��ȣ����������ͱ��ģ��洢I֡����
    ������byFrameType ��ʶI֡��U֡����S֡
  
    ���أ���
  ***************************************************************/
  BOOL CGB104S::SendFrameTail(void)
  {
  
    m_pSend->byAPDULen = m_SendBuf.wWritePtr - 2;
    
    WriteToComm(0);
    if ((m_pSend->byControl1 & 0x1) == FRM_I)
    {
      /*���洢I֡*/
      SaveSendFrame(m_wSendNum, m_SendBuf.wWritePtr, m_SendBuf.pBuf);
      m_wSendNum ++;
      //StartTimer(1);    //statr T1
      StopTimer(2);   //when send I frame, stop T2
    }
    return TRUE;
  }
  
  
  void CGB104S::CycleSendData(void)
  {
    if (!CanSendIFrame())
      return;
    if(bySendAccidentFlag == 0)
    {
      bySendAccidentFlag = 1;
      SendClassTwoData(); //���ͱ仯ң��
    }
    else if(bySendAccidentFlag == 1)
    {
      bySendAccidentFlag = 2;
      SendChangeYx();//���ͱ�λң��
    }
    else if(bySendAccidentFlag == 2)
    {
      bySendAccidentFlag = 0;
      SendSoe(SOE_GRP_NUM,0); //���ͱ仯��SOE �¼�
    }
    
  }
  
  /***************************************************************
    Function: SendChangeYx
      ���ͱ�λң�ű��� ֡����=1 �����ϱ�
    ��������
  
    ���أ���
  ***************************************************************/
  BOOL CGB104S::SendChangeYx(void)
  {
    WORD ReadNum;
    WORD RecCosNum = 0;
      //BYTE YxValue;
    
    BYTE TypeID = m_pEqpInfo[0].wYxType;
    BYTE Reason = COT_SPONT;
    VASDU * pSendAsdu;
    BYTE *pBuf = null;
    WORD YXNo;
    
    if (!CanSendIFrame())
      return FALSE;
  
    ReadNum = m_pEqpInfo[m_wEqpNo].wSYXNum;
    if(!ReadNum)
      return FALSE;

     RecCosNum = GetCosNum(m_SCosHeadPtr,g_unSCosBuffTail);
    if (RecCosNum == 0)
         return FALSE;
  
    for(;;)
    {
      if (!CanSendIFrame())
      {
        continue;
      }
      //add make cos frame
      SendFrameHead(FRM_I);
  
      pSendAsdu = (VASDU *)m_pSend->byASDU;
      pSendAsdu->byTypeID = TypeID;
      
      for(int i = 0; i < RecCosNum; i++,m_SCosHeadPtr++)
    {
        m_SCosHeadPtr &= (SOE_BUFF_NUM - 1);
        pBuf = g_gSCosBuff[m_SCosHeadPtr];
        YXNo = MAKEWORD(pBuf[SOE_TYPEL],pBuf[SOE_TYPEH]);
        write_infoadd( YXNo + ADDR_YX_LO);  
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pBuf[SOE_STVAL];
        m_wSendCosNum++;
    }
    
//        for (int i=m_wSendCosNum; (i < RecCosNum)  && (m_SendBuf.wWritePtr<=249); i++,OneFrmSoeNum++)
//        {
//          YxValue = pRecCOS->byValue;
//          write_infoadd(MAKEWORD(pRecCOS->byNol,pRecCOS->byNoh));
//          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = YxValue? 1 : 0;
//          pRecCOS ++;
//          m_wSendCosNum++;
//        }
      pSendAsdu->byQualifier = m_wSendCosNum;
      write_COT(Reason);
      SendFrameTail();
      if(m_wSendCosNum >= RecCosNum)
      {
        m_wSendCosNum = 0;
        return TRUE;
      }
    }
  }
  
  /***************************************************************
    Function: SendSingleSoe
      ���ʹ�ʱ��ĵ���ң�� soe tpye=2/30
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendSoe(WORD ReadNum,DWORD RamBase)
  {
    WORD RecSoeNum = 0;
    //VDBSOE *pRecSOE = NULL;
      //BYTE YxValue;
      
      BYTE Reason = COT_SPONT;
  
      BYTE TypeID = m_pEqpInfo[0].wSoeType;
  
    VASDU * pSendAsdu;
    WORD YXNo = 0; 
    BYTE *pBuf = null;
    
     if (!CanSendIFrame())
       return;
    RecSoeNum = GetCosNum(m_SSoeHeadPtr,g_unSCosBuffTail);
    if (RecSoeNum == 0)
         return;
  

    for(;;)
    {
      if (!CanSendIFrame())
      {
        continue;
      }
      //make soe frame
      SendFrameHead(FRM_I);
  
      pSendAsdu = (VASDU *)m_pSend->byASDU;
      pSendAsdu->byTypeID = TypeID;
      
      for(int i = 0; i < RecSoeNum; i++,m_SSoeHeadPtr++)
    {
        m_SSoeHeadPtr &= (SOE_BUFF_NUM - 1);
        pBuf = g_gSCosBuff[m_SSoeHeadPtr];
        YXNo = MAKEWORD(pBuf[SOE_TYPEL],pBuf[SOE_TYPEH]);
        write_infoadd( YXNo + ADDR_YX_LO);
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pBuf[SOE_STVAL];
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MSL];
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MSH];
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MINU];
        if (m_pEqpInfo[0].wSoeType == M_SP_TB || m_pEqpInfo[0].wSoeType == M_DP_TB)//7�ֽ�ʱ����Ϣ
        {
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_HOUR] & 0x1F;
            BYTE week=0;
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (pBuf[SOE_DAY] & 0x1F) | ((week <<5) & 0xE0);
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_MONTH];
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pBuf[SOE_YEAR];
        }
        m_wSendSoeNum++;
    }
//        for (int i = m_wSendSoeNum; (i < RecSoeNum) && (m_SendBuf.wWritePtr<=244); i++,OneFrmSoeNum++)
//        {
//          YxValue = pRecSOE->byValue;
//         write_infoadd(MAKEWORD(pRecSOE->byNol,pRecSOE->byNoh));
//    
//      if (m_pEqpInfo[0].wSoeType == M_SP_TA || m_pEqpInfo[0].wSoeType == M_SP_TB)//single yx
//          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = YxValue ? 1 : 0;
//      else
//          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = YxValue;
//    
//          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pRecSOE->byMSecondl;
//          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pRecSOE->byMSecondh;
//          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pRecSOE->byMinute;
//    if (m_pEqpInfo[0].wSoeType == M_SP_TB || m_pEqpInfo[0].wSoeType == M_DP_TB)//7�ֽ�ʱ����Ϣ
//          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pRecSOE->byHour & 0x1F;
//          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pRecSOE->byDay;//(SysTime.byDay & 0x1F) | ((SysTime.byWeek <<5) & 0xE0);
//          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pRecSOE->byMonth;
//          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pRecSOE->byYear;
//  
//          pRecSOE ++;
//          m_wSendSoeNum++;
//        }
      pSendAsdu->byQualifier = m_wSendSoeNum; 
    write_COT(Reason);
      SendFrameTail();
  
      if(m_wSendSoeNum >= RecSoeNum)
      {
        m_wSendSoeNum = 0;
        return;
      }
    }                                                                      
  }
  
  void CGB104S::SendClassTwoData(void)
  {
  #if (GB104S_YC_TYPE == M_ME_ND)
      SendChangeYC21();
  #elif (GB104S_YC_TYPE == M_ME_NC)
      SendChangeYC13();
  #elif (GB104S_YC_TYPE == M_ME_NA)
      SendChangeYC9();
  #elif (GB104S_YC_TYPE == M_ME_NB)
      SendChangeYC11();
  #endif
  }
  
  
  #if (GB104S_YC_TYPE == M_ME_NB)
  /***************************************************************
    Function: SendChangeYC11
      ���ͱ仯��ң��   ����=11
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendChangeYC11(void)
  {
    WORD m_dwPubBuf[MAX_PUBBUF_LEN];
    VDBYCF *pBuf = (VDBYCF *)m_dwPubBuf;
    WORD wReqNum = 0 ;
    WORD wChangeYcNum = 0;
    int  YxIndex=0;
    BYTE TypeID = M_ME_NB;
    BYTE Reason = COT_SPONT;
    VASDU * pSendAsdu;
  
    if (!CanSendIFrame())
      return;
  
    wReqNum = m_pEqpInfo[m_wEqpNo].wYCNum;
    if(!wReqNum)
      return;
  
    wChangeYcNum = SearchChangeYC(wReqNum, (VDBYCF *)pBuf);
    if(wChangeYcNum == 0)
      return;
  
    for(;;)
    {
      if (!CanSendIFrame())
      {
        continue;
      }
      SendFrameHead(FRM_I);
      pSendAsdu = (VASDU *)m_pSend->byASDU;
      pSendAsdu->byTypeID = TypeID;
      
      int OneFrmSoeNum=0;
      for (int i = m_wSendChangedYcNum; (i < wChangeYcNum) && (m_SendBuf.wWritePtr<=249); i++,OneFrmSoeNum++)
      {
        write_infoadd(pBuf->wNo+ADDR_YC_LO);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(pBuf->nValue);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(pBuf->nValue);
        
        YxIndex = YcIdToYxId(g_ucYCAddr[pBuf->wNo] - 1);
          if(YxIndex == 255) 
             continue;
          if(GetRIStatus(YxIndex) == 1)           
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDSͨ�����Ϻ����λ��1��ʾ��Ч
          else
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
        //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;//QDP
        pBuf++;
        m_wSendChangedYcNum++;
      }
      pSendAsdu->byQualifier = OneFrmSoeNum;
    write_COT(Reason);
      SendFrameTail();
      if(m_wSendChangedYcNum >= wChangeYcNum)
      {
        m_wSendChangedYcNum = 0;
        return;
      }
    }
  
//    return;
  }
  #elif (GB104S_YC_TYPE == M_ME_NA)
  /***************************************************************
    Function: SendChangeYC9
      ���ͱ仯��ң��   ����=9
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendChangeYC9(void)
  {
    VDBYCF *pBuf = NULL ;//= (VDBYCF *)m_dwPubBuf;
    WORD wReqNum = 0 ;//= REQ_CHANGEYC_NUM;
    WORD wChangeYcNum = 0;
    int YxIndex= 0;
    BYTE TypeID = M_ME_NA;
    BYTE Reason = COT_SPONT;
    VASDU * pSendAsdu;
  
    if (!CanSendIFrame())
      return;
    
    wReqNum = m_pEqpInfo[m_wEqpNo].wYCNum;
    if(!wReqNum)
      return;
  
    //wChangeYcNum = SearchChangeYC(wReqNum, m_pEqpInfo[m_wEqpNo].pCYcBuf);
    if (!wChangeYcNum)
      return;
  
    pBuf = m_pEqpInfo[m_wEqpNo].pCYcBuf;
    for(;;)
    {
      SendFrameHead(FRM_I);
      pSendAsdu = (VASDU *)m_pSend->byASDU;
      pSendAsdu->byTypeID = TypeID;
      
      int OneFrmSoeNum=0;
      for (int i = m_wSendChangedYcNum; (i < wChangeYcNum) && (m_SendBuf.wWritePtr<=249); i++,OneFrmSoeNum++)
      {
        write_infoadd(pBuf->wNo+ADDR_YC_LO);
  
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(pBuf->nValue);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(pBuf->nValue);
        YxIndex = YcIdToYxId(g_ucYCAddr[pBuf->wNo] - 1);
          if(YxIndex == 255) 
             continue;
          if(GetRIStatus(YxIndex) == 1)           
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDSͨ�����Ϻ����λ��1��ʾ��Ч
          else
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
        //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;//QDP
        pBuf++;
        m_wSendChangedYcNum++;
      }
      pSendAsdu->byQualifier = OneFrmSoeNum;
    write_COT(Reason);
      SendFrameTail();
      if(m_wSendChangedYcNum >= wChangeYcNum)
      {
        m_wSendChangedYcNum = 0;
        return;
      }
    }
  
    return;
  }
  
  #elif (GB104S_YC_TYPE == M_ME_ND)
  void CGB104S::SendChangeYC21(void)
  {
    VDBYCF *pBuf = NULL ;//= (VDBYCF *)m_dwPubBuf;
    WORD wReqNum = 0 ;//= REQ_CHANGEYC_NUM;
    WORD wChangeYcNum = 0;
  
    BYTE TypeID = M_ME_ND;
    BYTE Reason = COT_SPONT;
    VASDU * pSendAsdu;
  
    if (!CanSendIFrame())
      return;
    
    wReqNum = m_pEqpInfo[m_wEqpNo].wYCNum;
    if(!wReqNum)
      return;
  
    //wChangeYcNum = SearchChangeYC(wReqNum, m_pEqpInfo[m_wEqpNo].pCYcBuf);
    if (!wChangeYcNum)
      return;
    
    pBuf = m_pEqpInfo[m_wEqpNo].pCYcBuf;
  
    for(;;)
    {
      SendFrameHead(FRM_I);
      pSendAsdu = (VASDU *)m_pSend->byASDU;
      pSendAsdu->byTypeID = TypeID;
      

      int OneFrmSoeNum=0;
      int YxIndex=0;
      for (int i = m_wSendChangedYcNum; (i < wChangeYcNum) && (m_SendBuf.wWritePtr<=249); i++,OneFrmSoeNum++)
      {
        write_infoadd(pBuf->wNo+ADDR_YC_LO);
  
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(pBuf->nValue);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(pBuf->nValue);
        
        YxIndex = YcIdToYxId(g_ucYCAddr[pBuf->wNo] - 1);
          if(YxIndex == 255) 
             continue;
          if(GetRIStatus(YxIndex) == 1)           
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDSͨ�����Ϻ����λ��1��ʾ��Ч
          else
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
        //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;//QDP
        pBuf++;
        m_wSendChangedYcNum++;
      }
      pSendAsdu->byQualifier = OneFrmSoeNum;
    write_COT(Reason);
      SendFrameTail();
      if(m_wSendChangedYcNum >= wChangeYcNum)
      {
        m_wSendChangedYcNum = 0;
        return;
      }
    }
  
    return;
  }
  
  #elif (GB104S_YC_TYPE == M_ME_NC)
  
  void CGB104S::SendChangeYC13(void)
  {
    VDBYCF *pBuf = NULL;//= (VDBYCF *)m_dwPubBuf;
    WORD wReqNum = 0;//30; //REQ_CHANGEYC_NUM;243/8
    WORD wChangeYcNum = 0;
    
    BYTE TypeID = M_ME_NC;
    BYTE Reason = COT_SPONT;
    VASDU * pSendAsdu;
    int YxIndex = 0;
    wReqNum = m_pEqpInfo[m_wEqpNo].wYCNum;
    if(!wReqNum)
      return;
  
    //wChangeYcNum = SearchChangeYC(wReqNum, m_pEqpInfo[m_wEqpNo].pCYcBuf);
    if (!wChangeYcNum)
      return;
  
    pBuf = m_pEqpInfo[m_wEqpNo].pCYcBuf;
  
    for(;;)
    {
      SendFrameHead(FRM_I);
      pSendAsdu = (VASDU *)m_pSend->byASDU;
      pSendAsdu->byTypeID = TypeID;
      
      int OneFrmSoeNum=0;
      for (int i=m_wSendChangedYcNum,OneFrmSoeNum=0; (i < wChangeYcNum) && (m_SendBuf.wWritePtr<=247); i++,OneFrmSoeNum++)
      {
        write_infoadd(pBuf->wNo+ADDR_YC_LO);
    
        float temp = pBuf->nValue;  DWORD *pd = (DWORD *)&temp;
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(LOWORD(*pd));
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(LOWORD(*pd));
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(HIWORD(*pd));
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(HIWORD(*pd));
        
        YxIndex = YcIdToYxId(g_ucYCAddr[pBuf->wNo] - 1);
          if(YxIndex == 255) 
             continue;
          if(GetRIStatus(YxIndex) == 1)           
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDSͨ�����Ϻ����λ��1��ʾ��Ч
          else
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
        //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0; //QDS=Ʒ��������
  
        pBuf++;
        m_wSendChangedYcNum++;
      }
      pSendAsdu->byQualifier = OneFrmSoeNum;
    write_COT(Reason);
      SendFrameTail();
      if(m_wSendChangedYcNum >= wChangeYcNum)
      {
        m_wSendChangedYcNum = 0;
        return;
      }
    }
    return; 
  }
  
  #endif //GB104S_YC_TYPE /* #ifdef GB104S_YC_NVA */
  
  
  void CGB104S::SendYkCancelAck(void)
  {
      BYTE Reason = COT_DEACTCON;
    BYTE Qualifier = 1;
    BYTE *pData = m_pASDUInfo;
    VASDU * pSendAsdu;
    
    if (!CanSendIFrame())
        return;
    
    SendFrameHead(FRM_I);
    
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = m_pASDU->byTypeID;
    
    pSendAsdu->byQualifier = Qualifier;
    
   write_infoadd(m_InfoAdd);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pData[m_guiyuepara.infoaddlen]; //DCO
  
  write_COT(Reason);
    SendFrameTail();   
  }
  
  /***************************************************************
    Function�� SendAllYcYx
      ����ȫ��ң�ţ�ң�����ݡ�����ʱ�����е���
    ������
      byFlag = 1   ��ʱ����ȫң��
           = 0  ����
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendAllYcYx(BYTE byFlag)
  {
    WORD i,bitOffset, YxGroupNum, YcGroupNum;
    BYTE Reason = COT_INTROGEN;
    static BYTE flag = 0;
    
    if (!CanSendIFrame())
      return;
    
    YxGroupNum = (m_pEqpInfo[m_wEqpNo].wSYXNum + YX_GRP_NUM - 1) / YX_GRP_NUM;
    YcGroupNum = (m_pEqpInfo[m_wEqpNo].wYCNum + YC_GRP_NUM - 1) / YC_GRP_NUM;
    
    if(byFlag)
    {/*��ʱ����ȫң��*/
      flag = 1;
      m_dwYcSendFlag = 0xffffffff;
      m_dwYcSendFlagH = 0xffffffff;
    }
    if(flag == 1)
    {
      Reason = COT_PERCYC;
    }
    else
      Reason = COT_INTROGEN;
  
    /*���ٻ�����ң��*/
    for(i=0; (i<31) && (i<YxGroupNum); i++)
    {
      if( m_dwYxSendFlag & (1<<i) )
      {
        m_dwYxSendFlag &= ~(1<<i);
        SendGroupYx(i, Reason);
        return;
      }
    }
    for(i=31; (i<63) && (i<YxGroupNum); i++)
    {
      bitOffset = i-31;
      if( m_dwYxSendFlagH & (1<<bitOffset) )
      {
        m_dwYxSendFlagH &= ~(1<<bitOffset);
        SendGroupYx(i, Reason);
        return;
      }
    }
    /*ң��*/
    for(i=0; (i<31) && (i<YcGroupNum); i++)
    {
      if( m_dwYcSendFlag & (1<<i) )
      {
        m_dwYcSendFlag &= ~(1<<i);
        SendGroupYc(i, Reason);
        return;
      }
    }
    for(i=31; (i<63) && (i<YcGroupNum); i++)
    {
      bitOffset = i-31;
      if( m_dwYcSendFlagH & (1<<bitOffset) )
      {
        m_dwYcSendFlagH &= ~(1<<bitOffset);
        SendGroupYc(i, Reason);
        return;
      }
    }
  
    if( m_dwYcSendFlag & 0x80000000 )
    {
      m_dwSendAllDataEqpNo++;
      if ( m_dwSendAllDataEqpNo >= m_wEqpNum)
      {
        m_dwSendAllDataEqpNo = 0;
        m_dwYxSendFlag = 0;
        m_dwYcSendFlag = 0;
        m_dwYxSendFlagH = 0;
        m_dwYcSendFlagH = 0;
        if(flag)
          flag = 0;
        else
          SendStopYcYx();
      }
      else
      {
        
        m_dwYxSendFlag = 0xffffffff;
        m_dwYcSendFlag = 0xffffffff;
        m_dwYxSendFlagH = 0xffffffff;
        m_dwYcSendFlagH = 0xffffffff;
      }
    }
    return;
  }
  
  /***************************************************************
    Function�� SendSomeYcYx
      ���Ͳ���ң�ţ�ң�����ݣ���վ�����ٻ�ʱ����Ӧ
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendSomeYcYx(void)
  {
    WORD i, YxGroupNum, YcGroupNum;
    
    if (!CanSendIFrame())
      return;
    
    YxGroupNum = (m_pEqpInfo[m_wEqpNo].wSYXNum + YX_GRP_NUM - 1) / YX_GRP_NUM;
    YcGroupNum = (m_pEqpInfo[m_wEqpNo].wYCNum + YC_GRP_NUM - 1) / YC_GRP_NUM;
  
    /*����ң��*/
    for(i=0; (i<31) && (i<YxGroupNum); i++)
    {
      if( m_dwYxGroupFlag & (1<<i) )
      {
        m_dwYxGroupFlag &= ~(1<<i);
        SendGroupYx(i, i+COT_INRO1);
        return;
      }
    }
    for(i=0; (i<31) && (i<YcGroupNum); i++)
    {
      if( m_dwYcGroupFlag & (1<<i) )
      {
        m_dwYcGroupFlag &= ~(1<<i);
        SendGroupYc(i, i+COT_INRO9);
        return;
      }
    }
    
    if( m_dwYxGroupFlag & 0x80000000 )
    {
      m_dwYxGroupFlag = 0;
      SendStopSomeYcYx();
    } 
    
    if( m_dwYcGroupFlag & 0x80000000 )
    {
      m_dwYcGroupFlag = 0;
      SendStopSomeYcYx();
    } 
  }
  
  
  /*��ͻ��˷������н���*/
  void CGB104S::SendStopYcYx(void)
  {
    BYTE TypeID = C_IC_NA;
    BYTE Reason = COT_ACTTERM;
    BYTE Qualifier = 0x1;
    VASDU * pSendAsdu;
    
    SendFrameHead(FRM_I);
      
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
     
    write_infoadd(0);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x14;
    
    pSendAsdu->byQualifier = Qualifier;
    write_COT(Reason);
    SendFrameTail();
  //m_bSendInitEnd = 1;
    return; 
  }
  
  /*��ͻ��˷��ͷ����ٽ���*/
  void CGB104S::SendStopSomeYcYx(void)
  {
    BYTE TypeID = C_IC_NA;
    BYTE Reason = COT_ACTTERM;
    BYTE Qualifier = 0x1;
    VASDU * pSendAsdu;
    
    SendFrameHead(FRM_I);
      
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
     
    write_infoadd(0);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = m_byYcYxQrp;
    
    pSendAsdu->byQualifier = Qualifier;
  write_COT(Reason); 
    SendFrameTail();
    return; 
  }
  
  /***************************************************************
    Function: SendGroupYc9
      ����֡����=9��Ĭ�ϡ�   ��21����13  ��Ӧ����
    ������YcGroupNo �����
      Reason  ����ԭ��
    ���أ����ͳɹ�=TRUE ʧ��=FALSE
  ***************************************************************/
  BOOL CGB104S::SendGroupYc(WORD YcGroupNo,BYTE Reason)
  {
  #if (GB104S_YC_TYPE == M_ME_ND)
      return SendGroupYc21(YcGroupNo,Reason);
  #elif (GB104S_YC_TYPE == M_ME_NC)
      return SendGroupYc13(YcGroupNo,Reason);
  #elif (GB104S_YC_TYPE == M_ME_NA)
      return SendGroupYc9(YcGroupNo,Reason);
  #elif (GB104S_YC_TYPE == M_ME_NB)
      return SendGroupYc11(YcGroupNo,Reason);
  #endif
  }
  #if (GB104S_YC_TYPE == M_ME_NB)
  /***************************************************************
    Function: SendGroupYc11
      ����֡����=11 ���ͷ���ң��  ��Ӧ����
    ������YcGroupNo �����
      Reason  ����ԭ��
    ���أ����ͳɹ�=TRUE ʧ��=FALSE
  ***************************************************************/
  BOOL CGB104S::SendGroupYc11(WORD YcGroupNo,BYTE Reason)
  {
    BYTE TypeID = M_ME_NB;
    WORD YcNo;
    WORD YcValue;
    WORD ReadYcNum = 1;
    VASDU * pSendAsdu;
    int YxIndex = 0;
    if (!CanSendIFrame())
      return FALSE;
  
    YcNo = YcGroupNo * YC_GRP_NUM;
    if (YcNo >= m_pEqpInfo[m_wEqpNo].wYCNum)
      return FALSE;
      
    SendFrameHead(FRM_I);
    
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;  
  
    write_infoadd(YcNo + ADDR_YC_LO);
  
    WORD i;
    for( i=0; (i<YC_FRM_NUM) && (YcNo<m_pEqpInfo[m_wEqpNo].wYCNum) && (m_SendBuf.wWritePtr<=252); i++, YcNo++)
    {
      ReadRangeYC(YcNo, ReadYcNum,(short *)&YcValue);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(YcValue);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(YcValue);
      
      YxIndex = YcIdToYxId(g_ucYCAddr[YcNo] - 1);
        if(YxIndex == 255) 
           continue;
        if(GetRIStatus(YxIndex) == 1)           
           m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDSͨ�����Ϻ����λ��1��ʾ��Ч
        else
        {
           m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
        }
      //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;//QDS
    }
    
    pSendAsdu->byQualifier = i | VSQ_SQ;
    m_wSendYcNum = i;
    
    write_COT(Reason);
    SendFrameTail();
      
    if ((YC_FRM_NUM < YC_GRP_NUM) && (m_wSendYcNum != 0) && (YcNo < m_pEqpInfo[m_wEqpNo].wYCNum))
    {
      SendGroupYcContinue9(YcGroupNo, Reason);
      m_wSendYcNum = 0;
    }
    else
    {
      return TRUE;
    }
    return TRUE;
  }
  
  /***************************************************************
    Function: SendGroupYcContinue11
      ����֡����=11�� ���ͷ���ң�⣬ ң��������ʱʹ�ã� SendGroupYc11�����е��ã�  ��Ӧ����
    ������YcGroupNo �����
      Reason  ����ԭ��
    ���أ����ͳɹ�=TRUE ʧ��=FALSE
  ***************************************************************/
  BOOL CGB104S::SendGroupYcContinue11(WORD YcGroupNo,BYTE Reason)
  {
    BYTE TypeID = M_ME_NB;
    WORD YcNo;
    WORD YcValue;
    WORD ReadYcNum = 1;
    VASDU * pSendAsdu;
    int YxIndex=0;
    if (!CanSendIFrame())
      return FALSE;
  
    YcNo = YcGroupNo * YC_GRP_NUM + m_wSendYcNum;
    if (YcNo >= m_pEqpInfo[m_wEqpNo].wYCNum)
      return FALSE;
      
    SendFrameHead(FRM_I);
    
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
  
  
    write_infoadd(YcNo + ADDR_YC_LO);
  
    WORD i;
    for(i=0; (i<(YC_GRP_NUM-m_wSendYcNum)) && (YcNo<m_pEqpInfo[m_wEqpNo].wYCNum) && (m_SendBuf.wWritePtr<=252); i++, YcNo++)
    {
      ReadRangeYC(YcNo, ReadYcNum,(short *)&YcValue);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(YcValue);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(YcValue); 

       YxIndex = YcIdToYxId(g_ucYCAddr[YcNo] - 1);
        if(YxIndex == 255) 
           continue;
        if(GetRIStatus(YxIndex) == 1)           
           m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDSͨ�����Ϻ����λ��1��ʾ��Ч
        else
        {
           m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
        }
      
      //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;//QDS
    }
    
    pSendAsdu->byQualifier = i | VSQ_SQ;
    m_wSendYcNum = 0;
    
    write_COT(Reason);
    SendFrameTail();
      
    return TRUE;
  }
  #elif (GB104S_YC_TYPE == M_ME_NA)
  /***************************************************************
    Function: SendGroupYc9
      ����֡����=9 ���ͷ���ң��  ��Ӧ����
    ������YcGroupNo �����
      Reason  ����ԭ��
    ���أ����ͳɹ�=TRUE ʧ��=FALSE
  ***************************************************************/
  BOOL CGB104S::SendGroupYc9(WORD YcGroupNo,BYTE Reason)
  {
    BYTE TypeID = M_ME_NA;
    WORD YcNo;
    WORD YcValue;
    WORD ReadYcNum = 1;
    VASDU * pSendAsdu;
    int YxIndex= 0;
    if (!CanSendIFrame())
      return FALSE;
  
    YcNo = YcGroupNo * YC_GRP_NUM;
    if (YcNo >= m_pEqpInfo[m_wEqpNo].wYCNum)
      return FALSE;
      
    SendFrameHead(FRM_I);
    
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
  
  
    write_infoadd(YcNo + ADDR_YC_LO);
  
    WORD i;
    for( i=0; (i<YC_FRM_NUM) && (YcNo<m_pEqpInfo[m_wEqpNo].wYCNum) && (m_SendBuf.wWritePtr<=252); i++, YcNo++)
    {
      ReadRangeYC(YcNo, ReadYcNum,(short *)&YcValue);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(YcValue);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(YcValue);
      
      YxIndex = YcIdToYxId(g_ucYCAddr[YcNo] - 1);
        if(YxIndex == 255) 
           continue;
        if(GetRIStatus(YxIndex) == 1)           
           m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDSͨ�����Ϻ����λ��1��ʾ��Ч
        else
        {
           m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
        }
      //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;//QDS
    }
    
    pSendAsdu->byQualifier = i | VSQ_SQ;
    m_wSendYcNum = i;
    
    write_COT(Reason);
    SendFrameTail();
      
    if ((YC_FRM_NUM < YC_GRP_NUM) && (m_wSendYcNum != 0) && (YcNo < m_pEqpInfo[m_wEqpNo].wYCNum))
    {
      SendGroupYcContinue9(YcGroupNo, Reason);
      m_wSendYcNum = 0;
    }
    else
    {
      return TRUE;
    }
    return TRUE;
  }
  
  /***************************************************************
    Function: SendGroupYc9
      ����֡����=9�� ���ͷ���ң�⣬ ң��������ʱʹ�ã� SendGroupYc9�����е��ã�  ��Ӧ����
    ������YcGroupNo �����
      Reason  ����ԭ��
    ���أ����ͳɹ�=TRUE ʧ��=FALSE
  ***************************************************************/
  BOOL CGB104S::SendGroupYcContinue9(WORD YcGroupNo,BYTE Reason)
  {
    BYTE TypeID = M_ME_NA;
    WORD YcNo;
    WORD YcValue;
    WORD ReadYcNum = 1;
    VASDU * pSendAsdu;
    int YxIndex=0;
    if (!CanSendIFrame())
      return FALSE;
  
    YcNo = YcGroupNo * YC_GRP_NUM + m_wSendYcNum;
    if (YcNo >= m_pEqpInfo[m_wEqpNo].wYCNum)
      return FALSE;
  
    SendFrameHead(FRM_I);
  
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
  
  
    write_infoadd(YcNo + ADDR_YC_LO);
  
    WORD i;
    for(i=0; (i<(YC_GRP_NUM-m_wSendYcNum)) && (YcNo<m_pEqpInfo[m_wEqpNo].wYCNum) && (m_SendBuf.wWritePtr<=252); i++, YcNo++)
    {
      ReadRangeYC(YcNo, ReadYcNum,(short *)&YcValue);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(YcValue);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(YcValue);
      
      YxIndex = YcIdToYxId(g_ucYCAddr[YcNo] - 1);
        if(YxIndex == 255) 
           continue;
        if(GetRIStatus(YxIndex) == 1)           
           m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDSͨ�����Ϻ����λ��1��ʾ��Ч
        else
        {
           m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
        }
      //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;//QDS
      //if( m_SendBuf.wWritePtr>245) break;
    }
  
    pSendAsdu->byQualifier = i | VSQ_SQ;
    m_wSendYcNum = 0;
    
    write_COT(Reason);
    SendFrameTail();
  
    return TRUE;
  }
  #elif (GB104S_YC_TYPE == M_ME_ND)
  /*����֡����=21*/
  BOOL CGB104S::SendGroupYc21(WORD YcGroupNo,BYTE Reason)
  {
    BYTE TypeID = M_ME_ND;
    WORD YcNo;
    WORD YcValue;
    WORD ReadYcNum = 1;
    VASDU * pSendAsdu;
  
    if (!CanSendIFrame())
      return FALSE;
  
    YcNo = YcGroupNo * YC_GRP_NUM;
    if (YcNo >= m_pEqpInfo[m_wEqpNo].wYCNum)
      return FALSE;
  
    SendFrameHead(FRM_I);
  
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
  
   write_infoadd(YcNo + ADDR_YC_LO);
    WORD i;
    for( i=0; (i<YC_FRM_NUM) && (YcNo<m_pEqpInfo[m_wEqpNo].wYCNum) && (m_SendBuf.wWritePtr<=253); i++, YcNo++)
    {
      ReadRangeYC(YcNo, ReadYcNum,(short *)&YcValue);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(YcValue);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(YcValue);
    }
  
    pSendAsdu->byQualifier = i | VSQ_SQ;
    m_wSendYcNum = i;
    
    write_COT(Reason);
    SendFrameTail();
  
    if ((YC_FRM_NUM < YC_GRP_NUM) && (m_wSendYcNum != 0) && (YcNo < m_pEqpInfo[m_wEqpNo].wYCNum))
    {
      SendGroupYcContinue21(YcGroupNo, Reason);
      m_wSendYcNum = 0;
    }
    else
    {
      return TRUE;
    }
    return TRUE;
  }
  /*����֡����=21 ң��������ʱʹ�� SendGroupYc21�����е���*/
  BOOL CGB104S::SendGroupYcContinue21(WORD YcGroupNo,BYTE Reason)
  {
    BYTE TypeID = M_ME_ND;
    WORD YcNo;
    WORD YcValue;
    WORD ReadYcNum = 1;
    VASDU * pSendAsdu;
    
    if (!CanSendIFrame())
      return FALSE;
  
    YcNo = YcGroupNo * YC_GRP_NUM + m_wSendYcNum;
    if (YcNo >= m_pEqpInfo[m_wEqpNo].wYCNum)
      return FALSE;
      
    SendFrameHead(FRM_I);
    
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
  
   write_infoadd(YcNo + ADDR_YC_LO);
    WORD i;
    for(i=0; (i<(YC_GRP_NUM-m_wSendYcNum)) && (YcNo<m_pEqpInfo[m_wEqpNo].wYCNum) && (m_SendBuf.wWritePtr<=253); i++, YcNo++)
    {
      ReadRangeYC(YcNo, ReadYcNum,(short *)&YcValue);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(YcValue);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(YcValue);
      //if( m_SendBuf.wWritePtr>245) break;
    }
    
    pSendAsdu->byQualifier = i | VSQ_SQ;
    m_wSendYcNum = 0;
    
    write_COT(Reason);
    SendFrameTail();
      
    return TRUE;
  }
  #elif (GB104S_YC_TYPE == M_ME_NC)
  /*����֡����=13*/
  BOOL CGB104S::SendGroupYc13(WORD YcGroupNo,BYTE Reason)
  {
    BYTE TypeID = M_ME_NC;
    WORD YcNo;
    WORD YcValue;
    WORD ReadYcNum = 1;
    VASDU * pSendAsdu;
    int YxIndex = 0;
    if (!CanSendIFrame())
      return FALSE;
  
    YcNo = YcGroupNo * YC_GRP_NUM;
    if (YcNo >= m_pEqpInfo[m_wEqpNo].wYCNum)
      return FALSE;
      
    SendFrameHead(FRM_I);
    
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
  
    write_infoadd(YcNo + ADDR_YC_LO);
    WORD i;
    for( i=0; (i<YC_FRM_NUM) && (YcNo<m_pEqpInfo[m_wEqpNo].wYCNum) && (m_SendBuf.wWritePtr<=250); i++, YcNo++)
    {
      ReadRangeYC(YcNo, ReadYcNum,(short *)&YcValue);
  
      float temp = YcValue; DWORD *pd = (DWORD *)&temp;
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(LOWORD(*pd));
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(LOWORD(*pd));
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(HIWORD(*pd));
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(HIWORD(*pd));
      
       YxIndex = YcIdToYxId(g_ucYCAddr[YcNo] - 1);
        if(YxIndex == 255) 
           continue;
        if(GetRIStatus(YxIndex) == 1)           
           m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDSͨ�����Ϻ����λ��1��ʾ��Ч
        else
        {
           m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
        }
      //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0; //QDS=Ʒ��������
      
      //if( m_SendBuf.wWritePtr>245) break;
    }
    
    pSendAsdu->byQualifier = i | VSQ_SQ;
    m_wSendYcNum = i;
    
    write_COT(Reason);
    SendFrameTail();
      
    if ((YC_FRM_NUM < YC_GRP_NUM) && (m_wSendYcNum != 0) && (YcNo < m_pEqpInfo[m_wEqpNo].wYCNum))
    {
      SendGroupYcContinue13(YcGroupNo, Reason);
      m_wSendYcNum = 0;
    }
    else
    {
      return TRUE;
    }
    return TRUE;
  }
  /*����֡����=13 ң��������ʱʹ�� SendGroupYc13�����е���*/
  BOOL CGB104S::SendGroupYcContinue13(WORD YcGroupNo,BYTE Reason)
  {
    BYTE TypeID = M_ME_NC;
    WORD YcNo;
    WORD YcValue;
    WORD ReadYcNum = 1;
    VASDU * pSendAsdu;
    int YxIndex = 0;
    if (!CanSendIFrame())
      return FALSE;
  
    YcNo = YcGroupNo * YC_GRP_NUM + m_wSendYcNum;
    if (YcNo >= m_pEqpInfo[m_wEqpNo].wYCNum)
      return FALSE;
      
    SendFrameHead(FRM_I);
    
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
  
    write_infoadd(YcNo + ADDR_YC_LO);
    WORD i;
    for(i=0; (i<(YC_GRP_NUM-m_wSendYcNum)) && (YcNo<m_pEqpInfo[m_wEqpNo].wYCNum) && (m_SendBuf.wWritePtr<=250); i++, YcNo++)
    {
      ReadRangeYC(YcNo, ReadYcNum,(short *)&YcValue);
  
      float temp = YcValue; DWORD *pd = (DWORD *)&temp;
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(LOWORD(*pd));
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(LOWORD(*pd));
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(HIWORD(*pd));
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(HIWORD(*pd));
      
      YxIndex = YcIdToYxId(g_ucYCAddr[YcNo] - 1);
        if(YxIndex == 255) 
           continue;
        if(GetRIStatus(YxIndex) == 1)           
           m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDSͨ�����Ϻ����λ��1��ʾ��Ч
        else
        {
           m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
        }
      //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0; //QDS=Ʒ��������
    }
    
    pSendAsdu->byQualifier = i | VSQ_SQ;
    m_wSendYcNum = 0;
    
    write_COT(Reason);
    SendFrameTail();
      
    return TRUE;
  }
  #endif //GB104S_YC_TYPE /* #ifdef GB104S_YC_NVA */
  
  /***************************************************************
    Function: SendGroupYx
      ���ͷ���ң��   ����֡����=1  ��Ӧ����  ������
    ������YxGroupNo �����
      Reason ����ԭ��
  
    ���أ���
  ***************************************************************/
  BOOL CGB104S::SendGroupYx(WORD YxGroupNo,BYTE Reason)
  {
    BYTE TypeID = m_pEqpInfo[0].wYxType;
    WORD YxNo;
    BYTE YxValue[YX_FRM_NUM] = {0};
    WORD ReadYxNum = 1;
    VASDU * pSendAsdu;
    
    if (!CanSendIFrame())
      return FALSE;
  
    YxNo = YxGroupNo * YX_GRP_NUM;
    if (YxNo >= m_pEqpInfo[m_wEqpNo].wSYXNum)
      return FALSE;
      
    SendFrameHead(FRM_I);
    
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
  
    write_infoadd(YxNo + ADDR_YC_LO);
    ReadYxNum = YX_FRM_NUM;
    if((m_pEqpInfo[m_wEqpNo].wSYXNum - YxNo) < ReadYxNum)
      ReadYxNum = m_pEqpInfo[m_wEqpNo].wSYXNum - YxNo;
    ReadRangeSYXBit(YxNo,ReadYxNum,YxValue);
    WORD i;
    for(i=0; (i<YX_FRM_NUM) && (YxNo<m_pEqpInfo[m_wEqpNo].wSYXNum) && (m_SendBuf.wWritePtr<=254); i++, YxNo++)
    {
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = YxValue[i];
      //if( m_SendBuf.wWritePtr>245) break;
    }
    
    pSendAsdu->byQualifier = i | VSQ_SQ;
    m_wSendYxNum = i;
    
    write_COT(Reason);
    SendFrameTail();
      
    if ((YX_FRM_NUM < YX_GRP_NUM) && (m_wSendYxNum != 0) && (YxNo < m_pEqpInfo[m_wEqpNo].wSYXNum))
    {
      SendGroupYxContinue(YxGroupNo, Reason);
      m_wSendYxNum = 0;
    }
    else
    {
      return TRUE;
    }
    return TRUE;
  }
  /***************************************************************
    Function: SendGroupYxContinue
      ���ͷ���ң��  SendGroupYx�����е���  ����֡����=1
    ������YxGroupNo �����
      Reason ����ԭ��
  
    ���أ���
  ***************************************************************/
  BOOL CGB104S::SendGroupYxContinue(WORD YxGroupNo,BYTE Reason)
  {
      BYTE TypeID = m_pEqpInfo[0].wYxType;
      WORD YxNo;
      BYTE YxValue[YX_GRP_NUM];
      WORD ReadYxNum = 1;
      VASDU * pSendAsdu;
    
    if (!CanSendIFrame())
      return FALSE;
  
    YxNo = YxGroupNo * YX_GRP_NUM + m_wSendYxNum;
    if (YxNo >= m_pEqpInfo[m_wEqpNo].wSYXNum)
      return FALSE;
      
    SendFrameHead(FRM_I);
    
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
    
      
    write_infoadd(YxNo + ADDR_YC_LO);
    ReadYxNum = YX_GRP_NUM-m_wSendYxNum;
    if((m_pEqpInfo[m_wEqpNo].wSYXNum - YxNo) < ReadYxNum)
      ReadYxNum = m_pEqpInfo[m_wEqpNo].wSYXNum - YxNo;
    ReadRangeSYXBit(YxNo,ReadYxNum,YxValue);
    WORD i;
    for(i=0; (i<(YX_GRP_NUM-m_wSendYxNum)) && (YxNo<m_pEqpInfo[m_wEqpNo].wSYXNum) && (m_SendBuf.wWritePtr<=254); i++, YxNo++)
    {
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = YxValue[i];
      //if( m_SendBuf.wWritePtr>245) break;
    }
    
    pSendAsdu->byQualifier = i | VSQ_SQ;
    m_wSendYxNum = 0;
    write_COT(Reason);
    SendFrameTail();
      
    return TRUE;
  }
  
  /*����ȷ��*/
  void CGB104S::SendCallYcYxAck(void)
  {
    BYTE TypeID = C_IC_NA;
    BYTE Reason = COT_ACTCON;
    BYTE Qualifier = 0x1;
    VASDU * pSendAsdu;
  
    if (CanSendIFrame())
    {
      SendFrameHead(FRM_I);
  
      pSendAsdu = (VASDU *)m_pSend->byASDU;
      pSendAsdu->byTypeID = TypeID;
      write_infoadd(0);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x14;
  
      pSendAsdu->byQualifier = Qualifier;
      
      write_COT(Reason);
      SendFrameTail();
    }
    return;
  }
  
  /*����ȷ��*/
  void CGB104S::SendSomeYcYxAck(BYTE byQrp)
  {
    BYTE TypeID = C_IC_NA;
    BYTE Reason = COT_ACTCON;
    BYTE Qualifier = 0x1;
    VASDU * pSendAsdu;
    
    if (CanSendIFrame())
    {
      SendFrameHead(FRM_I);
      
      pSendAsdu = (VASDU *)m_pSend->byASDU;
      pSendAsdu->byTypeID = TypeID;
      write_infoadd(0);
      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = byQrp;
      
      pSendAsdu->byQualifier = Qualifier;
      
      write_COT(Reason);
      SendFrameTail();
    }
    return;
  }
  
  
  /***************************************************************
    Function: SendReadYx
      �������������֡����=102��ʱ�Ļظ���֡����=1��
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendReadYx(WORD wYxNo)
  {
    BYTE TypeID = m_pEqpInfo[0].wYxType;
    BYTE Reason = COT_REQ;
    BYTE Qualifier = 0x1;
    VASDU * pSendAsdu;
    
    WORD ReadYxNum = 1;
    BYTE YxValue;
    
    if (!CanSendIFrame())
      return;
      
    ReadRangeSYXBit(wYxNo, ReadYxNum, &YxValue);
    
    SendFrameHead(FRM_I);
      
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
  
     write_infoadd(wYxNo + ADDR_YX_LO);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = YxValue;
    
    pSendAsdu->byQualifier = Qualifier;
    
    write_COT(Reason);
    SendFrameTail();
    return;
  }
  /***************************************************************
    Function: SendReadYc
      �������������֡����=102��ʱ�Ļظ���֡����=9��
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendReadYc(WORD wYcNo)
  {
    BYTE TypeID = GB104S_YC_TYPE;
    BYTE Reason = COT_REQ;
    BYTE Qualifier = 0x1;
    VASDU * pSendAsdu;
    int YxIndex= 0;
    WORD ReadYcNum = 1;
    WORD YcValue;
    
    if (!CanSendIFrame())
      return;
    
    ReadRangeYC(wYcNo, ReadYcNum,(short *)&YcValue);
    
    SendFrameHead(FRM_I);
      
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
  
    write_infoadd(wYcNo + ADDR_YC_LO);
    
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(YcValue);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(YcValue);
    
    YxIndex = YcIdToYxId(g_ucYCAddr[wYcNo] - 1);
          if(YxIndex == 255) 
             continue;
          if(GetRIStatus(YxIndex) == 1)           
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x80;//QDSͨ�����Ϻ����λ��1��ʾ��Ч
          else
             m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//QDS
    //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
    
    pSendAsdu->byQualifier = Qualifier;
    
    write_COT(Reason);
    SendFrameTail();
    return;
  }
  /***************************************************************
    Function: SendReadPara
      �������������֡����=102��ʱ�Ļظ���֡����=110��
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendReadPara(WORD wParaValue,WORD IecAddr)
  {
    BYTE TypeID = P_ME_NA;
    BYTE Reason = COT_REQ;
    BYTE Qualifier = 0x1;
    VASDU * pSendAsdu;
  
    if (!CanSendIFrame())
      return;
  
    SendFrameHead(FRM_I);
  
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
  
    write_infoadd(IecAddr);
  
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(wParaValue);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(wParaValue);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
  
    pSendAsdu->byQualifier = Qualifier;
    
    write_COT(Reason);
    SendFrameTail();
    return;
  }
  
//  /***************************************************************
//    Function: SendReadMultiPara
//      �������������֡����=138��ʱ�Ļظ���֡����=110��
//    ��������
//  
//    ���أ���
//  ***************************************************************/
//  void CGB104S::SendReadMultiPara(WORD wParaNo,WORD IecAddrL,WORD wParaNum)
//  {
//    BYTE TypeID = P_ME_NA;
//    BYTE Reason = COT_REQ;
//    VASDU * pSendAsdu;
//    int i=0;
//  
//    WORD ReadParaNum = wParaNum;
//    WORD wParaValue[250] = {0};
//    WORD *pParaValue = wParaValue;
//  
//    if (!CanSendIFrame())
//      return;
//    BYTE byFlag = 0;
//    if(IecAddrL == ADDR_COLPARA_LO) byFlag = 0;
//    else if(IecAddrL == ADDR_YCPARA_LO) byFlag = 1;
//    else if(IecAddrL == ADDR_SYSPARA_LO) byFlag = 2;
//  
////    ReadRangePara(byFlag,wParaNo,ReadParaNum,(short *)pParaValue);
//  
//    SendFrameHead(FRM_I);
//  
//    pSendAsdu = (VASDU *)m_pSend->byASDU;
//    pSendAsdu->byTypeID = TypeID;
//    pSendAsdu->byReasonLow = Reason;
//  #ifndef INCLUDE_DF104FORMAT
//    pSendAsdu->byReasonHigh = GetEqpAddr();
//  #endif
//  
//    m_SendBuf.wWritePtr += ASDUID_LEN;
//  
//    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(wParaNo + IecAddrL);
//    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(wParaNo + IecAddrL);
//  #ifndef INCLUDE_DF104FORMAT
//    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
//  #endif
//    for(i=0; (i<ReadParaNum) && (m_SendBuf.wWritePtr<=254); i++,pParaValue++)
//    {
//      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(*pParaValue);
//      m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(*pParaValue);
//    }
//  
//    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
//  
//    pSendAsdu->byQualifier = pSendAsdu->byQualifier = i | VSQ_SQ;
//    SendFrameTail();
//    return;
//  }
  
  /***************************************************************
    Function: SendNoData
      �������������֡����=102��ʱ�Ļظ�����Ҫ��ȡ������û�ҵ�ʱ�Ļظ�
    ��������
  
    ���أ���
  ***************************************************************/
  void CGB104S::SendNoData(void)
  {
    SendSFrame();//???
    
  }
  
  /***************************************************************
    Function: SendAckRetry
      �ؽ�tcp����ʱ�����ͶϿ�����ǰδ��ȷ�ϵ�I֡���ģ�Ŀǰ����3�ֱ��ģ�1,30,11��
    ������id ��������  ȡֵ=1,30,11
      reason ����ԭ��
  
    ���أ���
  ***************************************************************/
  int CGB104S::SendAckRetry(BYTE id, BYTE reason)
  {
    if (!CanSendIFrame())
      return 0;
      
    for(int i=0; i<m_PARA_K; i++){
      if( m_vBackFrame[i].wSendNum != 0xffff && m_wAckRetry[i] &&
        m_vBackFrame[i].byBuf[6]==id &&
        m_vBackFrame[i].byBuf[8]==reason )
      {
        memcpy(m_SendBuf.pBuf, m_vBackFrame[i].byBuf, m_vBackFrame[i].wFrameLen);
        SendFrameHead(FRM_I);
        m_SendBuf.wWritePtr = m_vBackFrame[i].wFrameLen;
        SendFrameTail();
        m_vBackFrame[i].wSendNum = 0xFFFF;
        m_wAckRetry[i] = 0;
        return 1;
      }
    }
  
    return 0;
  }
  /***************************************************************
    Function: IsAckTimeout
      �ж�ACKʱ���Ƿ�ʱ����ʱ��λTCP����
    ��������
  
    ���أ�0 �������� 1 ��λtcp
  ***************************************************************/
  int CGB104S::IsAckTimeout(void)
  {
    int i;
    int ret = 0;
  
    //if(m_wSocketStatus == 0)
     // return 0;
    for(i=0; i<m_PARA_K; i++)
    {
      if( m_vBackFrame[i].wSendNum != 0xffff && m_wAckRetry[i]!=1 )
      {
        m_wAckTimeOut[i]++;
        if(m_wAckTimeOut[i]>PARA_T1*3)
        {
          ret = 1;
          break;
        }
      }   
    }
    
    if(ret){
      for(i=0; i<m_PARA_K; i++) 
        m_wAckTimeOut[i] = 0;
    }
    
    return ret;
  }
  /***************************************************************
    Function: DelAckFrame
      ȥ���ѱ��Է�ȷ�ϵı���
    ������SendNum �Է�ackȷ����� ��С�������ŵı��Ķ��ѱ��Զ�ȷ�ϣ�
  
    ���أ���
  ***************************************************************/
  void CGB104S::DelAckFrame(WORD SendNum)
  {
    int i;
    //int j;
    /*��ֹm_wAckNumΪ0, (m_wAckNum - 1)Ϊ0xFFFF*/
    SendNum = SendNum&MAX_FRAME_COUNTER;
  
    /*��ǰ����ŵ�����֡����ȷ��*/
    for(i=0; i<m_PARA_K; i++)
    {
      //for(j=0;j<=SendNum;j++)
      {
        if( m_vBackFrame[i].wSendNum <= SendNum )
        {
          m_vBackFrame[i].wSendNum = 0xffff;
          m_wAckTimeOut[i] = 0;
          m_wAckRetry[i] = 0;
        }
      }
  //    for(j=0; j<m_PARA_K; j++)
  //    {
  //      if( m_vBackFrame[j].wSendNum == SendNum ) {
  //        m_vBackFrame[j].wSendNum = 0xffff;
  //        m_wAckTimeOut[j] = 0;
  //        m_wAckRetry[i] = 0;
  //      }
  //    }
  //    if(SendNum == 0) SendNum = MAX_FRAME_COUNTER;
  //    else SendNum--;
    }
    //if(g_unTotYXCHGNum > 0)
    //{
    //  g_unTotYXCHGNum = 0;
    //  InsertLcdMbCmd(6, REG_CHG_YX_ACK, 1);   //����ȷ��֡������
    //}
    
  }
  
  /***************************************************************
    Function: SaveSendFrame
      ���淢�͵ı���
    ������SendNum ��ǰ���ͱ��ĵķ������
      FrameLen ֡��
      pFrame ֡����
  
    ���أ���
  ***************************************************************/
  void CGB104S::SaveSendFrame(WORD SendNum, WORD FrameLen, BYTE *pFrame)
  {
    WORD i;
    for(i = 0; i < m_PARA_K; i++)
    {
      if( m_vBackFrame[i].wSendNum == 0xffff )
      {
        m_vBackFrame[i].wSendNum = SendNum;
        m_vBackFrame[i].wFrameLen = FrameLen;
        memcpy(m_vBackFrame[i].byBuf, pFrame, FrameLen);
  
        m_wAckTimeOut[i] = 0;
        m_wAckRetry[i] = 0;
  
        return;
      }
    } 
  }
  void CGB104S::SetAckRetry()
  {//�ոս�������ʱ���ô˺���
  
    for(int i=0; i<PARA_K; i++)
    {
      m_wAckTimeOut[i] = 0;
      m_wAckRetry[i] = 0;
      if( m_vBackFrame[i].wSendNum != 0xffff )
      {
        if( (m_vBackFrame[i].byBuf[6]==m_pEqpInfo[0].wYxType
          || m_vBackFrame[i].byBuf[6]== m_pEqpInfo[0].wSoeType
          || m_vBackFrame[i].byBuf[6]==GB104S_YC_TYPE)/*�������Ҫ�ش��仯ң�⣬����ȥ�����д���*/
          && m_vBackFrame[i].byBuf[8]==COT_SPONT )
          m_wAckRetry[i] = 1; /*�������Ӻ����ٷ���*/
        else
          m_vBackFrame[i].wSendNum = 0xFFFF;
      }
    }
  
  }
  
  BOOL CGB104S::CanSendIFrame(void)
  {
      return (m_bDTEnable && m_bContinueSend);    
  }
  
  /*��������=48�ı���*/
  BOOL CGB104S::DoYtCommand(void)
  {
    BYTE * pData = m_pASDUInfo;
    WORD  YTNo, YTValue;
    //WORD wDataAddr;
    VYTInfo *pYTInfo;
    pYTInfo = &(pGetEqpInfo(m_wEqpNo)->YTInfo);
   switch (m_guiyuepara.infoaddlen )
    {
        case 1:
            YTNo = pData[0];
            YTValue = MAKEWORD(pData[1], pData[2]);
            break;
        case 2:
            YTNo = MAKEWORD(pData[0], pData[1]);
            YTValue = MAKEWORD(pData[2], pData[3]);
            break;
        default:
            YTNo = MAKEWORD(pData[0], pData[1]);//���ֽ�ǿ��Ϊ0
            YTValue = MAKEWORD(pData[3], pData[4]);
            break;
    }
    //wDataAddr = YTNo;
    
    pYTInfo->Info.wID = YTNo;//min=0
    pYTInfo->Head.byMsgID = MI_YTOPRATE;
    pYTInfo->Info.wValue = YTValue;
    pYTInfo->Head.byTypeID = m_pASDU->byTypeID;
    pYTInfo->Head.wInfoAddr = YTNo;
    pYTInfo->Head.byDco = pData[m_guiyuepara.infoaddlen+2];//QOS
  
    /*���յ�ң�������Ĵ���*/
    #define WAITTIME 5 //���ȴ�5��
    if(pYTInfo->Head.byMsgID == MI_YTOPRATE)
    {
//      if(((wDataAddr >= REG_PC_BASE) && (wDataAddr < (REG_PC_BASE + PROC_CNT_NUM)))//������ֵ
//      ||((wDataAddr >= REG_RP_BASE) && (wDataAddr < (REG_RP_BASE + RUN_PARA_NUM)))) //���в���
//      {
//          InsertLcdMbCmd(6, wDataAddr, YTValue);
//          m_YtWaitCount = WAITTIME;
//          g_YtOrderFlag = ON;
//      }
//      else if((wDataAddr >= REG_YC_SIQU_BASE) && (wDataAddr < (REG_YC_SIQU_BASE + IEC_YC_NUM))) //ң������
//      {
//          g_unYCSiqu[wDataAddr - REG_YC_SIQU_BASE] = YTValue;
//          InsertLcdMbCmd(16, REG_YC_SIQU_BASE, IEC_YC_NUM);
//          m_YtWaitCount = WAITTIME;
//          g_YtOrderFlag = ON;
//      }
//      
//      else
      {
        SendBack(COT_E_IADDR| COT_PN_BIT);
      }
    }
    return TRUE;
  }




  
  /*���ģ�68 0e 00 00 00 00 89 03 06 00 01 00 01 62 00 33 22 01 50 00 13 13 01 53 00 15 51 00 01 02 03 04 05 06 07*/
//  BOOL CGB104S::DoMultiYtCommand(void)
//  {
//    BYTE * pData = m_pASDUInfo;
//    WORD  YTNo, YTValue;
//    //YTNoMin[3]={0};
//    WORD YTCheck = 0;
//    WORD wParaNum; //max--(255-12-7-1)/5;
//    WORD wParaBuf[3][255];
//    BYTE bySQ = 0;
//    WORD i=0;
//    int iPaNo0=0,iPaNo1=0,iPaNo2=0;
//    int ret = -1;
//  
//    wParaNum = m_pASDU->byQualifier & 0x7f;
//    bySQ = m_pASDU->byQualifier & 0x80;
//    if(bySQ)
//    {//˳����
//      YTNo = MAKEWORD(pData[0], pData[1]);//���ֽ�ǿ��Ϊ0
//      if(YTNo>=ADDR_YT_LO && YTNo<=ADDR_YT_HI)
//      {//dpram
//        YTNo = YTNo - ADDR_YT_LO;
//        YTCheck += YTNo;
//        wParaBuf[0][iPaNo0++] = YTNo;//����IEC��ַ
//        for(i = 0; i < wParaNum; i++)
//        {
//          YTValue = MAKEWORD(pData[3+i*2], pData[4+i*2]);
//          wParaBuf[0][iPaNo0++] = YTValue;
//          YTCheck += YTValue;
//        }
//      }
//      else if(YTNo>=ADDR_YCPARA_LO && YTNo<=ADDR_YCPARA_HI)
//      {//flash
//        YTNo = YTNo - ADDR_YCPARA_LO;
//        for(i = 0; i < wParaNum; i++)
//        {
//          YTValue = MAKEWORD(pData[3+i*2], pData[4+i*2]);
//          wParaBuf[1][iPaNo1++] = YTValue;
//        }
//      }
//      else if(YTNo>=ADDR_SYSPARA_LO && YTNo<=ADDR_SYSPARA_HI)
//      {//flash
//        YTNo = YTNo - ADDR_SYSPARA_LO;
//        for(i = 0; i < wParaNum; i++)
//        {
//          YTValue = MAKEWORD(pData[3+i*2], pData[4+i*2]);
//          wParaBuf[2][iPaNo2++] = YTValue;
//        }
//      }
//      else
//      {
//        SendBack(COT_E_IADDR| COT_PN_BIT);
//        return FALSE;
//      }
//    }
//    else
//    {//��֧����ɢ��
//  //
//  //      for(int i = 0; i < wParaNum; i++)
//  //      {
//  //        YTNo = MAKEWORD(pData[0+i*5], pData[1+i*5]);//���ֽ�ǿ��Ϊ0
//  //        YTValue = MAKEWORD(pData[3+i*5], pData[4+i*5]);
//  //        if(YTNo>=ADDR_YT_LO && YTNo<=ADDR_YT_HI)
//  //        {
//  //          YTNo = YTNo - ADDR_YT_LO;
//  //          if(YTNoMin[0] == 0) YTNoMin[0] = YTNo;
//  //          wParaBuf[0][iPaNo0++] = YTNo;
//  //          wParaBuf[0][iPaNo0++] = YTValue;
//  //          YTCheck += YTNo + YTValue;
//  //        }
//  //        else if(YTNo>=ADDR_YCPARA_LO && YTNo<=ADDR_YCPARA_HI)
//  //        {
//  //          YTNo = YTNo - ADDR_YCPARA_LO;
//  //          if(YTNoMin[1] == 0) YTNoMin[1] = YTNo;
//  //          wParaBuf[1][iPaNo1++] = YTNo;
//  //          wParaBuf[1][iPaNo1++] = YTValue;
//  //        }
//  //        else if(YTNo>=ADDR_SYSPARA_LO && YTNo<=ADDR_SYSPARA_HI)
//  //        {
//  //          YTNo = YTNo - ADDR_SYSPARA_LO;
//  //          if(YTNoMin[2] == 0) YTNoMin[2] = YTNo;
//  //          wParaBuf[2][iPaNo2++] = YTNo;
//  //          wParaBuf[2][iPaNo2++] = YTValue;
//  //        }
//  //        else
//  //        {
//  //          SendBack(COT_E_IADDR| COT_PN_BIT);
//  //          return FALSE;
//  //        }
//      SendBack(COT_E_IADDR| COT_PN_BIT);
//      return FALSE;
//    }
//    YTCheck = YTCheck & 0xffff;
//  
//    /*���յ�ң�������Ĵ���*/
//    if(iPaNo0 > 0)
//    {//����dpram
//      //����״̬(0xA5A5)+��ʶ(16bit:0xC3)+��ַ(16bit)+ֵ(16bit)+ֵ1(16bit)+ ... + У��
////      OrderHead stuOrderHd;
////      stuOrderHd.wStatus = 0xA5A5;
////      stuOrderHd.wSign = DPRAM_ORDER_YT;
////      WriteToDPRam16(DPRAM_BASE_YT,sizeof(OrderHead)>>1,iPaNo0,wParaBuf[0]);
////      WriteToDPRam16(DPRAM_BASE_YT,((sizeof(OrderHead)>>1)+iPaNo0),1,&YTCheck);
////      WriteToDPRam8(DPRAM_BASE_YT,0,sizeof(OrderHead),(BYTE *)&stuOrderHd);
//    }
//    if(iPaNo1 >0)
//    {//����flash
//      /*do something*/
//      //ret = SaveParaToFlash(FLASH_ADDR_YCPARA,YTNo*2,iPaNo1*2,m_pEqpInfo[m_wEqpNo].wYcPaNum*2,(BYTE *)&wParaBuf[1]);
//    }
//    if(iPaNo2 >0)
//    {//����flash
//      /*do something*/
//      //ret = SaveParaToFlash(FLASH_ADDR_SYSPARA,YTNo*2,iPaNo2*2,m_pEqpInfo[m_wEqpNo].wSysPaNum*2,(BYTE *)&wParaBuf[2]);
//    }
//    /*����ֵ*/
//    if(iPaNo0 > 0)
//    {
//      WORD wFbOk = 0;
//      //wFbOk = IsOrderFeedBack(DPRAM_BASE_YTFEED,0,DPRAM_ORDER_YT);
//      if(wFbOk == 1)
//        SendMultiYtReturn(wParaNum);
//      else
//        SendBack(COT_ACTCON|COT_PN_BIT);
//    }
//    else
//    {
//      if(ret == 0)
//        SendMultiYtReturn(wParaNum);
//      else
//        SendBack(COT_ACTCON|COT_PN_BIT);
//    }
//  
//    return TRUE;
//  }
  /*����ң�����ر���*/
  void CGB104S::SendYtReturn(WORD wIecAddrL)
  {
    BYTE Reason = COT_ACTCON;
    BYTE Qualifier = 1;
    VYTInfo *pYTInfo;
    VASDU *pSendAsdu;
    //WORD YTNo;
    WORD YTValue;

    pYTInfo = &(pGetEqpInfo(m_wEqpNo)->YTInfo);
    //YTNo = pYTInfo->Info.wID;
    YTValue = pYTInfo->Info.wValue;
  
    if (!CanSendIFrame())
       return;
    if (pYTInfo->Head.byMsgID == MI_YTCANCEL)
    {
      Reason = COT_DEACTCON;
    }
    SendFrameHead(FRM_I);
  
    pSendAsdu = (VASDU *)m_pSend->byASDU;


    pSendAsdu->byTypeID = pYTInfo->Head.byTypeID;
    pSendAsdu->byQualifier = Qualifier;
    write_infoadd(pYTInfo->Head.wInfoAddr);
    
   m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(YTValue);
   m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(YTValue);
   m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pYTInfo->Head.byDco; //QOS
    if (pYTInfo->Info.byStatus != 0)
    {
      Reason |= COT_PN_BIT;    
    }
    write_COT(Reason);
    SendFrameTail();
  }
  /*���Ͷ��ң��ʱ�ķ��ر���*/
//  void CGB104S::SendMultiYtReturn(WORD wYtNum)
//  {
//    BYTE Reason = COT_ACTCON;
//    VASDU *pSendAsdu;
//  
//    if (!CanSendIFrame())
//       return;
//    SendFrameHead(FRM_I);
//    
//    pSendAsdu = (VASDU *)m_pSend->byASDU;
//    pSendAsdu->byTypeID = m_pASDU->byTypeID;
//    pSendAsdu->byReasonLow = Reason;
//    pSendAsdu->byReasonHigh = GetEqpAddr();
//  
//    pSendAsdu->byQualifier = wYtNum | VSQ_SQ;
//    
//    m_SendBuf.wWritePtr += ASDUID_LEN;
//    
//    memcpy(m_SendBuf.pBuf+m_SendBuf.wWritePtr,m_pASDUInfo,wYtNum*2+8+3);//3(infoaddr) + 8(time)
//    m_SendBuf.wWritePtr += wYtNum*2+8+3;
//  
//    SendFrameTail();  
//  }

  void CGB104S::DoSetTime(void)
  {
    BYTE * pData = m_pASDUInfo;
    BYTE TypeID = C_CS_NA;
    BYTE Reason = COT_ACTCON;
    BYTE Qualifier = 1;
    VASDU * pSendAsdu;
    VSysClock SysTime;  
    WORD MSecond;
    pData += m_guiyuepara.infoaddlen;  //ָ��ʱ����Ϣ��

    MSecond = MAKEWORD(pData[0], pData[1]); 
    SysTime.wMSecond = MSecond % 1000;
    SysTime.bySecond  = MSecond / 1000;
    SysTime.byMinute = pData[2];
    SysTime.byHour = pData[3] &0x1F;
    SysTime.byDay = pData[4] & 0x1F;
    SysTime.byWeek = pData[4] >> 5;
    SysTime.byMonth = pData[5] & 0x0F;
#ifndef INCLUDE_DF104FORMAT
    SysTime.wYear = (pData[6] & 0x7F) + 2000;
#else
    SysTime.wYear = ((pData[5] & 0xF0) << 4)+pData[6];
#endif
  
    /*�ж�ԭ��,ʱ��ĺ�����*/
    int err = 0;
    if (SwitchToAddress(m_wRecAddress) != TRUE){
      Reason = COT_E_CADDR;
      err = 1;
    }
    if(m_Cot != COT_ACT){
      Reason = COT_E_REASON;
      err = 1;
    }
    if( SysTime.wYear<2000 || SysTime.byWeek>7 ||
      SysTime.byMonth==0 || SysTime.byMonth>12 || 
      SysTime.byDay==0 || SysTime.byDay>31 ||
      SysTime.byHour>23 || SysTime.byMinute>=59 ||
      SysTime.bySecond>59 ){
      Reason = COT_E_IADDR;
      err = 1;
    }
    if(err == 0)
    {
      g_sRtcManager.m_gRealTimer[6]= SysTime.wYear;   //year_get;
          g_sRtcManager.m_gRealTimer[5]= SysTime.byMonth & 0x0f;      //month_get;
          g_sRtcManager.m_gRealTimer[4]= SysTime.byDay & 0x1f;      //day_get;
          g_sRtcManager.m_gRealTimer[3]= SysTime.byHour & 0x1f;      //hour_get;
          g_sRtcManager.m_gRealTimer[2]= SysTime.byMinute & 0x3f;      //minute_get;
      g_sRtcManager.m_gRealTimer[1]= SysTime.bySecond;
      g_sRtcManager.m_gRealTimer[0]= SysTime.wMSecond;
      //InsertLcdMbCmd(127, REG_CLOCK_BASE, RTC_NUM);//send time to 5438
    }

//#define SENDSACK//�еĵ������Ҫ��ֻ����S֡����ȷ��
#ifdef SENDSACK
    SendSFrame();
    return;
#endif

    if (!CanSendIFrame())
      return;
    SendFrameHead(FRM_I);
    
    pSendAsdu = (VASDU *)m_pSend->byASDU;
    pSendAsdu->byTypeID = TypeID;
    pSendAsdu->byQualifier = Qualifier;
          
    write_infoadd(0);
    
    for (WORD i=0; i<7; i++)
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pData[i];
    
    write_COT(Reason);
    SendFrameTail();
    return;

    
  }

//  int CGB104S::CommWrite(int num, char* pbuf, int buflen, DWORD flags)
//  {
//    //Write_SOCK_Data_Buffer(m_AptSocket, (unsigned char *)pbuf, buflen);
//    return 1;
//  }
//д����ԭ��
void CGB104S::write_COT(int  data)
{
    WORD  wCurWritePtr = 8;//APCI�(6)+֡����(1)+�ɱ�ṹ�޶���(1)
  for(BYTE i=0;i<m_guiyuepara.COTlen;i++)
    {
       m_SendBuf.pBuf[ wCurWritePtr++ ]=(data>>(i*8))&0xff;
    }

}
//д������ַ
void CGB104S::write_conaddr(int  data)
{
    m_SendBuf.wWritePtr = 8 + m_guiyuepara.COTlen;
    for(BYTE i=0;i<m_guiyuepara.conaddrlen;i++)
    {
       m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(data>>(i*8))&0xff;
    }

}
////д��Ϣ���ַ
void CGB104S::write_infoadd(int  data)
{
    for(BYTE i=0;i<m_guiyuepara.infoaddlen;i++)
    {
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=(data>>(i*8))&0xff;
    }

}
void CGB104S::GetAsduId(void)
{   BYTE off = 0;
    //ȡ����ԭ��
    off = 2;     
    if(m_guiyuepara.COTlen == 1)
    {
      m_Cot = m_pReceive->byASDU[off];
    }
    else if(m_guiyuepara.COTlen == 2)
    {
        m_Cot = MAKEWORD(m_pReceive->byASDU[off], m_pReceive->byASDU[off+1]);
    }
    else if(m_guiyuepara.COTlen == 3)
    {
        m_Cot = MAKEWORD(m_pReceive->byASDU[off], m_pReceive->byASDU[off+1]);
        m_Cot <<= 8;
        m_Cot |= m_pReceive->byASDU[off+2];
    }
    //ȡ������ַ
    off = 2 + m_guiyuepara.COTlen;
    if(m_guiyuepara.conaddrlen== 1)
    {
      m_ConAddr= m_pReceive->byASDU[off];
    }
    else if(m_guiyuepara.conaddrlen == 2)
    {
        m_ConAddr = MAKEWORD(m_pReceive->byASDU[off], m_pReceive->byASDU[off+1]);
    }
    else if(m_guiyuepara.conaddrlen == 3)
    {
        m_ConAddr = MAKEWORD(m_pReceive->byASDU[off], m_pReceive->byASDU[off+1]);
        m_ConAddr <<= 8;
        m_ConAddr |= m_pReceive->byASDU[off+2];
    }
    //ȡ��Ϣ���ַ
    off = 2 + m_guiyuepara.COTlen + m_guiyuepara.conaddrlen;
    if(m_guiyuepara.infoaddlen== 1)
    {
      m_InfoAdd = m_pReceive->byASDU[off];
    }
    else if(m_guiyuepara.infoaddlen == 2)
    {
        m_InfoAdd = MAKEWORD(m_pReceive->byASDU[off], m_pReceive->byASDU[off+1]);
    }
    else if(m_guiyuepara.infoaddlen == 3)
    {
        m_InfoAdd = MAKEWORD(m_pReceive->byASDU[off], m_pReceive->byASDU[off+1]);
        m_InfoAdd <<= 8;
        m_InfoAdd |= m_pReceive->byASDU[off+2];
    }

}




#endif
