/*------------------------------------------------------------------------
 Module:      protocol.cpp
 Author:      linxueqin
 Project:     ��Լƽ̨
 State:       V1.0
 Creation Date:   2014-4-28
 Description:   ��Լ����Ķ���
------------------------------------------------------------------------*/

#include "Protocol.h"
#include "..\Main.h"


/***************************************************************
  Function:CProtocol
    ��Լ���๹�캯��
  ��������
  ���أ���
***************************************************************/
CProtocol::CProtocol()    
{
  memset(&m_SendBuf, 0, sizeof(m_SendBuf));   
    memset(&m_RecBuf, 0, sizeof(m_RecBuf));      
    memset(&m_RecFrame, 0, sizeof(m_RecFrame));  
}
void CProtocol::ChangeIP(BYTE *pDa)    
{
 	if(pGprs != null) ((CPrtcSms *)pGprs)->SendWCmdToIHD(0,0,pDa,this); 
}

/***************************************************************
  Function:Init
      ��Լƽ̨��ʼ��
  ��������
  ���أ�TRUE �ɹ���FALSE ʧ��
***************************************************************/
BOOL CProtocol::Init(WORD wMinEqpNum)
{  
  m_SendBuf.wBufSize = MAX_SEND_LEN;
  m_RecBuf.wBufSize = MAX_RECV_LEN;

  
        
  m_SendBuf.pBuf = m_SBuf;
  m_RecBuf.pBuf = m_RBuf;
  m_wSendPaNum = 0;

  return TRUE;  
}

/***************************************************************
  Function:GetOwnAddr
    ��ȡ�����ַ
  ��������
  
  ���أ������ַ
***************************************************************/
WORD CProtocol::GetOwnAddr(void)      
{
  return  g_gRunPara[RP_COMM_ADDR];//
}
/***************************************************************
  Function:NeatenCommBuf
    ͨ�Ż������Ĺ���
  ������pCommIO
    pCommIO��ָ��ͨ�Ż�������ָ��
  ���أ���
***************************************************************/
void CProtocol::NeatenCommBuf(VCommBuf *pCommBuf)
{
 // register unsigned  int i,j;

  if (pCommBuf->wReadPtr == 0)
  {
    return ; //��ָ���Ѿ�Ϊ0
  }


  if (pCommBuf->wReadPtr >= pCommBuf->wWritePtr)
  {
    pCommBuf->wReadPtr = pCommBuf->wWritePtr=0;
    return ;
  }


  if (pCommBuf->wWritePtr >= pCommBuf->wBufSize)
  {
    pCommBuf->wReadPtr = 0;
    pCommBuf->wWritePtr = 0;
    return ;
  }
    pCommBuf->wReadPtr = 0; 
    pCommBuf->wWritePtr = 0;
    
    
//  i = 0; 
//  j = pCommBuf->wReadPtr;
//  while (j < pCommBuf->wWritePtr)
//  {
//    pCommBuf->pBuf[i++] = pCommBuf->pBuf[j++];
//  }
//
//  pCommBuf->wReadPtr = 0; 
//  pCommBuf->wWritePtr = i; 
}

/***************************************************************
  Function:SearchFrame
    ������Ϣ֡
  ��������
  
  ���أ�TRUE �ҵ���ȷ��һ֡���ģ�FALSE δ�ҵ�һ֡��ȷ�ı���
***************************************************************/
BOOL CProtocol::SearchFrame(void)
{
  DWORD Rc;
  DWORD Len;//�Ѵ�������ֽ���
  short MsgLen;

  while (1)
  {
    MsgLen = m_RecBuf.wWritePtr - m_RecBuf.wReadPtr;
    if (MsgLen <= 0)
    {
      return FALSE;
    }

    Rc = SearchOneFrame(&m_RecBuf.pBuf[m_RecBuf.wReadPtr], MsgLen);
    Len = Rc & ~FRAME_SHIELD; //�Ѵ�������ֽ���

    switch  (Rc & FRAME_SHIELD)
    {
      case FRAME_OK:
        DisableRetry();
        m_RecFrame.pBuf = &m_RecBuf.pBuf[m_RecBuf.wReadPtr];  //��¼��Ч���ĵ���ʼ��ַ
        m_RecFrame.wWritePtr = (WORD)Len; //��¼��Ч���ĵĳ���
        m_RecBuf.wReadPtr += (WORD)Len; //ָ���Ƶ���һ���Ĵ�
        return TRUE;

      case FRAME_ERR:
        if (!Len)
        {
          m_RecBuf.wReadPtr++;
        }
        else
        {
          m_RecBuf.wReadPtr += Len; //ָ���Ƶ���һ���Ĵ�
        }
        break;

      case FRAME_LESS:
        m_RecBuf.wReadPtr++;  //����һ���ֽڼ���������ȷ���� lxq
        break;
        //m_RecBuf.wReadPtr += (WORD)Len; //ָ���Ƶ���һ���Ĵ�
        //return FALSE;
      default:
        break;//??do what
    }//switch
  }//while
}

/***************************************************************
  Function:SearchHead
    ���������е�һ���ؼ���
  ������Buf, Len,��Offset1, Key1
    Buf ���Ļ�����ͷָ��
    Len Ҫ�����ı��ĳ���
    Offset1 �ؼ��ֵ��ڱ����е�ƫ��λ��
    Key1 �ؼ���
  ���أ��ؼ����ڱ��Ļ������е�ƫ��λ��
***************************************************************/
WORD CProtocol::SearchHead(BYTE *Buf, WORD Len,short Offset1,BYTE Key1)
{
  WORD i;
  for(i=0;i<(short)(Len-Offset1);i++)
  {
    if(Buf[Offset1+i]==Key1)
      return i;
  }

  return i;
}
/***************************************************************
  Function: RcvData
    ��Լ�������ݲ�����
    ��socket�ж�����ʱ�˺������أ����غ�socket�������������������ķ�ʽͣ����select��������
  ��������

  ���أ���
***************************************************************/
int CProtocol::RcvData(int* iSocket)
{
    int rxlen=0;
    int nLen=1024;    
    //while(1)//Ҫ��Ҫ�����ձ���? lxq
    {
      NeatenCommBuf(&m_RecBuf);
      nLen = m_RecBuf.wBufSize-m_RecBuf.wWritePtr;
      rxlen = CommRcvData((BYTE *)(m_RecBuf.pBuf+m_RecBuf.wWritePtr),nLen,*iSocket);
      
      if(rxlen > 0)
      {
          m_RecBuf.wWritePtr += (WORD)rxlen;
          if (m_RecBuf.wWritePtr > m_RecBuf.wBufSize)
          {
             m_RecBuf.wWritePtr = 0;
          }
          #ifdef RF_TEST
            if(*iSocket == g_CmIdRF)
            {//��RF���յ�������ת����ȥ
              CommSendData(m_RecBuf.pBuf,rxlen,g_CmIdEX);
            }
          #endif
          DoReceive();
       }
      else if(rxlen <= 0)//recv=0 �Զ˷����Ͽ�����
      {
         return 0;
      }
   }
  return 0;
}

int CProtocol::CommWrite( char* pbuf, int buflen, DWORD flags)
{
  CommSendData((BYTE*)pbuf,buflen,m_uartId);

  return 1;
}


/***************************************************************
  Function: IsOrderFeedBack
    �ж�DPRAM����������Ƿ���ȷ
  ������RamBase dpram��ַ
    buf  ƫ����

  ���أ�1 ������ȷ�� 0 ����
***************************************************************/
//  WORD CProtocol::IsOrderFeedBack(DWORD RamBase,WORD wOffset,WORD wYTorYK)
//  {
//    WORD wRt=0;
//    int i=0;
//    OrderFeedBack stuFb;
//    for( i = 0; i<500;i++)
//    {
//      delayms(600);
//      //ReadAndClearDPRam16(RamBase,wOffset/2,sizeof(OrderFeedBack)/2,(WORD *)&stuFb);
//      if((stuFb.wSign == wYTorYK) && (stuFb.wStatus ==  0x5A5A))
//      {
//        wRt = 1;
//        break;
//      }
//      else
//        wRt = 0;
//    }
//    //printf("i=%d\n",i);
//    return wRt;
//  }



//У���
BYTE CProtocol::ChkSum(BYTE *buf, WORD len)
{
    WORD checksum, i;

    checksum = 0;
    for (i = 0; i < len; i++)
    {
        checksum = checksum + *buf;
        buf ++;
    }
    return LOBYTE(checksum & 0xff);
}
//������ѯ�����ò��� + Զ������====================================================================================
//�Զ��屨��
BOOL CProtocol::RecFrame69(void)
{
  pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;

    switch(pReceiveFrame->Frame69.Type)
    {
        case 0x7a://���ļ�
            RecReadFile();
            break;
        case 0x7d://д�ļ�
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
//******************************************************
//�ŏ| 0404 ���Ž���
//******************************************************
/*
BOOL CProtocol::RecFrameAA(void)
//BOOL CPrtcSms::RecFrameAA(VFrameAA * pReceiveFrame)
{
  pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;

    switch(pReceiveFrame->FrameAA.Type)
    {
      
      case 0x07:      //��GPRSʱ��
          if(pReceiveFrame->byContent[0] == 0x1)
          {
              //WORD MSecond;
              //MSecond = MAKEWORD(pData[0], pData[1]);
              unsigned int unHYM[7];
              unHYM[0] = ((pReceiveFrame->byContent[1]) & 0x7F);//YEAR
              unHYM[1] = (pReceiveFrame->byContent[2]) & 0x0F;//MONTH
              unHYM[2] = (pReceiveFrame->byContent[3]) & 0x1F;//DAY
              unHYM[3] =g_sRtcManager.m_gRealTimer[RTC_WEEK];//SNTP�޵ڼ��ܵ���Ϣ����RTCоƬ�ĵڼ��ܸ���
              unHYM[4] = pReceiveFrame->byContent[4];  //HOUR
              unHYM[5] = pReceiveFrame->byContent[5];//MINUTE
              unHYM[6] = pReceiveFrame->byContent[6];//SEC
              WriteRealTime(unHYM);  //�޸�ʱ��
              ReadRealTime(0);//��ʵʱоƬ�е�ʱ����µ�
              BYTE bBuf[2] = {0xFC,0XFC};
              MyPrintfD(bBuf,2);
          }
          else
          {
              BYTE bBuf[2] = {0xFB,0XFB};
              MyPrintfD(bBuf,2);
          }            
          break;
    
      case 0x16:      //��GPRS����
          RecIHDPara(pReceiveFrame);
          break;		  
      case 0x18:
          //RecIHDWConf(pReceiveFrame);
	   break;	  
        case 0x17://���Ͷ��ųɹ�
            RecSendSMSOK();
            break;
        case 0x13://���ն���
            RecSMS();
            break;
        default:
            break;
    }
    return TRUE;
}
  */
//******************************************************
/*
GRPS ģ����Ż�ִ��
GRPS ģ���յ���ȷ������֮�����̻ظ�ȷ�����
�����֣�0x17
GPRS ģ��: AA 55 AA 55 17 00 05 81 89 00 01 02 38 72
���������Ϊ1 ���ֽڣ�0x02��ʾ�յ���ȷ�������ݡ�
���ŷ������֮�󣬷��ͽ���Ļ�Ӧ��
�����֣�0x17
GPRS ģ��:AA 55 AA 55 17 00 05 81 89 00 01 00 2A 51
���������Ϊ1 ���ֽڣ�0x00��ʾ����ʧ�ܣ�0x01 ��ʾ���ͳɹ���
*/
//******************************************************
BOOL CProtocol::RecSendSMSOK(void)
{
    //BYTE i;
    BYTE *pData;/*
    WORD wFile;
    BYTE bySec;//����
    BYTE bySCQ;//�޶���
    WORD wStartNo;//��ʼ���
    WORD wSOENum;//��ȡ��SOE����
    WORD wLoadNum;//��ȡ�ĸ�����������(ÿ����ʱ��Ϊ��λ)
    WORD wTemp;   //¼�����ݵ���ʱ����
*/
    pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;//���������ԭ���ǣ�����68������ת���˺�����CProtocol�е�pReceiveFrameָ��δ��ֵ
    pData = &pReceiveFrame->FrameAA.byContent[0];
    
    pData +=4;

    switch(*pData)
    {
      case 0: //0x00��ʾ����ʧ�ܣ�
	    //�������Ͷ��ţ������ѷ��͹���
	    g_unSms_s = 0;
	    g_unSms_Count=0;
	    g_unFailSms++;
	    if(g_unFailSms>=3)
	    	{
	    	g_unPhoneNo++;
	    	g_unFailSms = 0;
	    	if(g_unPhoneNo>=4)// 4�����뷢�� ����һ��
			{
			g_unPhoneNo=0;
			if(g_unFrameFlag_Sms != g_unFrameFlag_curSms)
       			g_unFrameFlag_curSms++;
			if(g_unFrameFlag_curSms>=MAX_SMS_NUM)
				g_unFrameFlag_curSms=0;
	    		}
	    	}
	    return FALSE;
           break;
      case 1://0x01 ��ʾ���ͳɹ���
           g_unPhoneNo++;
	    if(g_unPhoneNo>=4)//4// 4�����뷢�� ����һ��
		{
		g_unPhoneNo=0;
		if(g_unFrameFlag_Sms != g_unFrameFlag_curSms)
           		g_unFrameFlag_curSms++;
	    	if(g_unFrameFlag_curSms>=MAX_SMS_NUM)
				g_unFrameFlag_curSms =0;
	    	}
	    g_unSms_s = 0;
	    g_unFailSms = 0;	
	    g_unSms_Count=0;	
           return TRUE;
           break;
      case 2://0x02��ʾ�յ���ȷ��������
           return TRUE;
           break;
      default:
	    //return FALSE;
           break; 
    }
    return FALSE;
}
//******************************************************
/*
ʹ���ֻ����ź�Դ���� 108101008216 �����ٻ�״̬���������ӻ��յ��ź�Դ�ϴ���״̬��Ϣ

���Ž��ո�ʽ�Ͷ��ŷ��͸�ʽһ����ֻ��55 AA 55 AA�ĳ��� AA 55 AA 55��
��ͷ:AA 55 AA 55
������:13 
���峤��:2�ֽ�
�ֻ�����TLV:	Tag: 81 80
			Length: �绰�� Value: �ַ���
���Ÿ�ʽ TLV:	Tag: 81 81 
			Length: 2 �ֽ�
			Value: ��ǰ֧��2 �ָ�ʽ,2 ��ʾ��׼Ӣ�Ķ��ţ�3 ��ʾ��׼���Ķ���
�������� TLV: 	Tag: 81 82
			Length: �������ĳ���
			Value: ��������
CRC:2 �ֽ�
*/
//******************************************************
BOOL CProtocol::RecSMS(void)
{
    BYTE i,Temp[21];
    BYTE *pData;/*
    WORD wFile;
    BYTE bySec;//����
    BYTE bySCQ;//�޶���
    WORD wStartNo;//��ʼ���
    WORD wSOENum;//��ȡ��SOE����
    WORD wLoadNum;//��ȡ�ĸ�����������(ÿ����ʱ��Ϊ��λ)
    WORD wTemp;   //¼�����ݵ���ʱ����
*/
    pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;//���������ԭ���ǣ�����68������ת���˺�����CProtocol�е�pReceiveFrameָ��δ��ֵ
    pData = &pReceiveFrame->FrameAA.byContent[0];
    
    pData +=4;

     for(i=0;i<11;i++)                         //////�洢�ֻ�����
        {
            ucTempTEL_NUM[i] = *pData++;
        } 	
     for(i=0;i<21;i++)                         //////�洢�ֻ�����
        {
            Temp[i] = *pData++;
        } 
    if((Temp[9]!=0x31)||(Temp[10]!=0x30)||(Temp[19]!=0x31)||(Temp[20]!=0x36))
		{return FALSE;}
    if((Temp[11]==0x39)&&(Temp[12]==0x31))
    	{
    	eight_select=0x8F; //eight_select|=BIT7;                    ////////////���״η���8����ı�־		
    	Sign_Repeat(0,0);
    	return TRUE;
    	}
    switch(Temp[12])
    {
      case 0x31: //�ֻ��ٻ��ź�Դ
      	    CreatNewSMS(PHONE_XUN);
	    return TRUE;		
           break;
      case 0x32://��վѯ��   
           CreatNewSMS(ZIZHAN_XUN);
	    return TRUE;	   
           break;     
      default:
	    //return FALSE;
           break; 
    }	
    
    return FALSE;
}
//******************************************************
//�ŏ| 0404 ���Ž���
//******************************************************
//��������������
BOOL CProtocol::RecReadFile(void)
{
    BYTE i;
    BYTE *pData;
    WORD wFile;
    BYTE bySec;//����
    BYTE bySCQ;//�޶���
    WORD wStartNo;//��ʼ���
    WORD wSOENum;//��ȡ��SOE����
    WORD wLoadNum;//��ȡ�ĸ�����������(ÿ����ʱ��Ϊ��λ)
    WORD wTemp;   //¼�����ݵ���ʱ����

    pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;//���������ԭ���ǣ�����68������ת���˺�����CProtocol�е�pReceiveFrameָ��δ��ֵ
    pData = &pReceiveFrame->Frame69.Data[0];


    	wFile = MAKEWORD(pData[0],pData[1]);
    	wTemp = MAKEWORD(pData[2],pData[3]);
    	pData +=2;
		
    bySec = *pData++;
    bySCQ = *pData++;
    
    if(bySCQ == 5 )
     {
        SendWrPaAck(wFile,bySec);//ѡ��д����
        return 1;
     }
    switch(wFile)
    {
      case 1:
        break;
      case 2:
          wStartNo = MAKEWORD(pData[0],pData[1]);
          pData +=2;
          wSOENum = MAKEWORD(pData[0],pData[1]);
          pData +=2;
          SendSavedSoe(bySec,wStartNo,wSOENum);
           break;
      case 0x0102:
        if(bySCQ == 6)
        {
          SendSoeSavedStu();//��SOE״̬
        }
        break;
      case 3:
        if(bySCQ == 6)
        {//������վ����
          SendTrigAck();
        }
        break;
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
	case 9://�ŏ| ������վ��
       case 10://�ŏ| Ŀ��У׼����λ�����ز��� ��ʼֵΪ��ѹ60V,����2A
         
      case 15:
      case 16:
      case 31:
      case 32:
        m_wSendPaNum = 0;
        SendReadPa(wFile,bySec);//���ն��������
        break;
   
      case 20:
          wStartNo = MAKEWORD(pData[0],pData[1]);
          pData +=2;
          wLoadNum = MAKEWORD(pData[0],pData[1]);
          pData +=2;
          SendLoadData(bySec,wStartNo,wLoadNum,bySCQ);
           break;
      case 33:
        g_sRecData.m_gRecNumCom      = CAT_SpiReadWord(EEPADD_REC_NUM);
        g_sRecData.m_CurWritePageCom = CAT_SpiReadWord(EEPADD_REC_POS);
        m_wSendPaNum = 0;
        for(i = 1; i<6; i++ )
        {
          SendReadRecData(wFile,i);//��¼������
          delayms(300);
        }
        
          break;
      case 34:
        g_sRecData.m_gRecNumCom      = CAT_SpiReadWord(EEPADD_REC_NUM);
        g_sRecData.m_CurWritePageCom = CAT_SpiReadWord(EEPADD_REC_POS);
        m_wSendPaNum = 0;
        
        SendReadRecData(wFile,wTemp);//��¼������
          delayms(300);
        break;
		
	case 11:
		if(bySec==1)//���Ž���
			{
			if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(bySec,wFile,this);
			break;
			}
		else if(bySec==2)//apn
			{
			if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(2,12,this);//��APN
			break;
			}
 		if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(bySec,wFile,this);
		delayms(2000);
		if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(5,11,this);//������IP

          	break;
      default:
      break; 
    }
    return TRUE;
}

//����д��������
BOOL CProtocol::RecWriteFile(void)
{
    BYTE *pData;
    WORD wFile;
    BYTE bySec;//����
    WORD wSecLen;//�γ���
    WORD i = 0,j = 0,k=0;
    WORD wPaStartId = 0;
    WORD wInfoAddr = 0;
    unsigned int unTemp = 0;
   // WORD wOldValBk;
  //  WORD wOldCfgKeyBk;
  //  BYTE bLine;
    pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;//���������ԭ���ǣ�����68������ת���˺�����CProtocol�е�pReceiveFrameָ��δ��ֵ
    
    wInfoAddr = MAKEWORD(pReceiveFrame->Frame69.InfoAddr[0],pReceiveFrame->Frame69.InfoAddr[1]);
    wPaStartId = wInfoAddr - 0x6902;

    pData = &pReceiveFrame->Frame69.Data[0];

    	wFile = MAKEWORD(pData[0],pData[1]);
    	pData +=2;

    bySec = *pData++;
    wSecLen= *pData++;//�ڳ���
    
    if(wFile >= 4 && wFile <= 8)
    {
        SendWrPaSuc();
    }
    else if(wFile == 0x81 || wFile == 0x82)
    {
      SendWrPaSuc();//SendBaseFrame(0,0);  
    }
    switch(wFile)
    {
      case 0x81://����GPRSΪ����ģʽ
     //   GPRS_SETA_L();
        break;
      case 0x82:
        //ExitGprsSet();
        break;
      case 4:
        if(bySec == 0 )//д���в���
        {
          for(i = wPaStartId,j = 0; j < wSecLen/2 && i < RUN_PARA_NUM-1;i++,j++)
          {//b
             
                  //������ֵ
                  g_gRunPara[i] = MAKEWORD(pData[0],pData[1]);
                  pData += 2;

        }//b
        /*//�ŏ| ң����ʼ��ַ�޸����в���
        pData += 10;//�ŏ| 0329  �����ϴ����ʱ��
	 g_ucPara_stime =*pData;   //�ŏ| 0329  �����ϴ����ʱ��
	*/	//�ŏ| ң����ʼ��ַ�޸����в���
        if(g_gRunPara[RP_COMM_ADDR] == 0)//��Ϊ��λ���ķ�ʽ�������վ��ַ���ó�0����λ
        {
            RstRunPara();  //���в�����λ
        } 
        g_ucParaChang |= BIT0;
        SetEqpInfo();
      }
      break;
      case 5:
        if(bySec == 0 )//дIEC����
        {
          for(i = wPaStartId,j = 0; j < wSecLen && i < IEC_PARA_NUM-1;i++,j++)
          {
            g_ucPara101[i] = *pData++;
          }
          if(g_ucPara101[0] == 0)//��Ϊ��λ���ķ�ʽ�������һ��������Ϊ0�����������ַ�ʽ��λ���
          {
              RstIEC101Para();  //IEC�������λ
          } 
          initpara();//����ͨ���ڴ����
          g_ucParaChang |= BIT1;
        }
        break;  
      case 6:
        if(bySec == 0 )//дң�ŵ��
        {
          for(i = wPaStartId,j = 0; j < wSecLen/2 && i < IEC_YX_NUM;i++,j++)
          {
            g_ucYXAddr[i] = MAKEWORD(pData[0],pData[1]);
            pData += 2;
          }
	   g_ucYXAddr[i] = 0;	  
          if(g_ucYXAddr[0] == 0)//��Ϊ��λ���ķ�ʽ�������һ��������Ϊ0�����������ַ�ʽ��λ���
          {
             RstIEC101YxAddr();  //ң�ŵ�ַ��λ
             //DelALLSOE();
          }
          InitTsYxInfo();  
          SetEqpInfo();
          g_ucParaChang |= BIT2; 
        }
        
        break;
      case 7:
        if(bySec == 0 )//дң����
        {
          for(i = wPaStartId,j = 0; j < wSecLen && i < IEC_YC_NUM;i++,j++)
          {
            g_ucYCAddr[i] = *pData++;
          }
           g_ucYCAddr[i] = 0;		  
          if(g_ucYCAddr[0] == 0)//��Ϊ��λ���ķ�ʽ�������һ��������Ϊ0�����������ַ�ʽ��λ���
          {
              RstIEC101YcAddr();  //ң���ַ��λ
          } 
          InitTsYcInfo();
          SetEqpInfo();
          g_ucParaChang |= BIT3;
        }
      break;
      case 8:
        if(bySec == 0 )//дң�ص��
        {
        /*  for(i = wPaStartId,j = 0; j < wSecLen && i < IEC_YK_NUM;i++,j++)
          {
            g_ucYKPa[i] = *pData++;
          }
          if(g_ucYKPa[0] == 255)//��Ϊ��λ���ķ�ʽ�������һ��������Ϊ255�����������ַ�ʽ��λ���
          {
              RstIEC101YkAddr();  //ң�ص�ַ��λ
          } */
          g_ucParaChang |= BIT4;
        }
      break;
      case 9://�ŏ| ������վ��
        if(bySec == 1 )//������ ����վ��
        {
          wSecLen =  pData[0]/2;
          pData++;	   	  
          for(i = 0;  i < RUN_INFO_NUM;i++)
          {
            	if(i<wSecLen)
            	{
            	    g_gCnName[i] = MAKEWORD(pData[0],pData[1]);
                  pData += 2;
            	}
		else
		{g_gCnName[i] =0x2000;}
          }
          if(g_gCnName[0] == 0)//��Ϊ��λ���ķ�ʽ�������һ���绰����ĵ�һ���ַ�����ΪR�����������ַ�ʽ��λ���
          {
              RstCnName();//�ŏ| 
          } //�ŏ|0328
          g_ucParaChang |= BIT6;
          SendWrPaSuc();
        }
	else if(bySec == 5 )//¼���ļ��� Ӣ��վ��
        {
          //wSecLen =  pData[0];
          //pData++;	   	  
          for(i = 0;  i < wSecLen;i++)
          {
            	if(i<wSecLen)
            	{
            	    g_gLBName[i] = pData[0];
                  pData ++;
            	}
		else
		{g_gLBName[i] =0x00;}
          }
	   //g_gLBName[wSecLen] =44;//",";
	   g_gLBNameLen=wSecLen;
          if(g_gLBName[0] == 0)//��Ϊ��λ���ķ�ʽ�������һ���绰����ĵ�һ���ַ�����ΪR�����������ַ�ʽ��λ���
          {
            RstLBName(); 	  
          } //�ŏ|0328
          g_ucParaChang |= BIT6;
          SendWrPaSuc();
        }	
      break;
      case 10://�ŏ| Ŀ��У׼����λ�����ز��� ��ʼֵΪ��ѹ60V,����2A
        if(bySec == 0 )
        {
           //wPaTotalNum = ADJ_PARA_NUM;
           unsigned int utempadj[ADJ_PARA_NUM];
           for(i = 0;i < ADJ_PARA_NUM;i++)
           {
 		utempadj[i] =  MAKEWORD(pData[0],pData[1]);
		pData += 2;	     	  
           }
	    for(j = 0; j < 3; j++)
           {  
		unsigned long xjj;
		xjj=0;
		for(k=0;k<32;k++)
		{
			xjj += g_gRmtMeasPJ[j][k];
		}
		xjj = xjj>>5;
		if(utempadj[j]>3000)
			{
            		unTemp = g_gProcCnt[j];
            		unTemp = (((unsigned long)utempadj[j]* unTemp) /xjj); //�ŏ| Ŀ��У׼����λ�����ز��� ��ʼֵΪ��ѹ60V,����2A
		
			if(unTemp > 3000 && unTemp < 6000)
                		g_gProcCnt[j] = unTemp;
			}
		else
			{
			if((xjj>utempadj[j])&&(xjj<3000))
				g_gAdjObj[j]=((unsigned long)(xjj-utempadj[j])*4096)/(3000-xjj);
			else
				g_gAdjObj[j]=0;
			}
			
            }
            unTemp = g_gProcCnt[3];
            unTemp = (((unsigned long)utempadj[3]*100 * unTemp) /g_gRmtMeas[RM_U0]); //�ŏ| Ŀ��У׼����λ�����ز��� ��ʼֵΪ��ѹ60V,����2A
            if(unTemp > 3000 && unTemp < 6000)
                	g_gProcCnt[3] = unTemp;
            unTemp = g_gProcCnt[4];
            unTemp = (((unsigned long)utempadj[4]*100 * unTemp) /g_gRmtMeas[RM_I0]); //�ŏ| Ŀ��У׼����λ�����ز��� ��ʼֵΪ��ѹ60V,����2A
            if(unTemp > 3000 && unTemp < 6000)
                	g_gProcCnt[4] = unTemp;
            for(i = 0; i <= 4; i++)
            		g_gRunPara[i + RP_UA_ADJ] = g_gProcCnt[i];   
            g_gChangFlag[CHNG_MUBIAO] = ON;   
         }	   
	  if(bySec == 1 )//�͵�ѹУ׼����
        {
            for(i = 0;i < 3;i++)
           {
 		g_gAdjObj[i] =  MAKEWORD(pData[0],pData[1]);
		pData += 2;	     	  
           }
	    g_gChangFlag[CHNG_MUBIAO] = ON; 		
	  }
	    SendWrPaSuc();	   
        
      break;
      case 11:  //дGPRS����
          if(bySec==1)//���Ž���
			{
			if(pGprs!= null) ((CPrtcSms*)pGprs)->SendWCmdToIHD(1,bySec,pData-1,this);
			delayms(3000);	  
          		if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(6,RESET,g_gIPPort,this);//����GPRSģ�� 		  
			break;
			}
	   else if	(bySec==2)//apn 
	   	{
	   	 if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(wSecLen,2,pData-1,this);//дAPN
	   	 delayms(3000);	  
               if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(6,RESET,g_gIPPort,this);//����GPRSģ�� 	
	   	}
          if(pGprs != null) ((CPrtcSms *)pGprs)->SendWCmdToIHD(6,0,pData-1,this);
	   delayms(3000);	  
	   if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(6,5,pData+5,this);
	   delayms(3000);	  
          if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(6,RESET,g_gIPPort,this);//����GPRSģ�� 		  
          break;	  
      case 16:
        if(bySec == 0 )//д�绰�����
        {
          for(i = wPaStartId,j = 0; j < wSecLen && i < PHONE_PA_NUM - 1;i++,j++)
          {
            g_gSmsPhone[i] = *pData++;
          }
          if(g_gSmsPhone[PHONE1] == 'R')//��Ϊ��λ���ķ�ʽ�������һ���绰����ĵ�һ���ַ�����ΪR�����������ַ�ʽ��λ���
          {
              //RstSmsPhoneInfo();//�ŏ|0328
          } //�ŏ|0328
          g_ucParaChang |= BIT5;
          SendWrPaSuc();
        }
      break;
      case 31:
        if(bySec == 0 )//ң�ŶԵ�
        {
          for(i = wPaStartId,j = 0; j < wSecLen && i < RMT_INFO_NUM;i++,j++)
          {
            BYTE bValCur = GetRIStatus(g_ucYXAddr[i]-1);
            BYTE bValnew = *pData++;
            
            if(bValnew > 3) bValnew = 1;
            if(bValCur != bValnew)
			{
			 g_gRmtInfo[(g_ucYXAddr[i]-1)]=bValnew;	
              	//CreatNewSoe(g_ucYXAddr[i]-1,bValnew,2);
            		}
          }
          SetYxTrans();
          SendWrPaSuc();
        }
        break;
        case 32:
        if(bySec == 0 )//ң��Ե�
        {
          for(i = wPaStartId,j = 0; j < wSecLen && i < IEC_YC_NUM;i++,j++)
          {
            g_unYcTrans[i] =MAKEWORD(pData[0],pData[1]);
		pData += 2;
          }

          if(pGprs != null)
		  	{
		  	((CBJ101S*)pGprs)->m_wSendYcNum = 0;
			((CBJ101S*)pGprs)->SendYCGroup(0,20,9);
			}
	    if(pDbg!= null)
			{
			((CBJ101S*)pDbg)->m_wSendYcNum = 0;
			((CBJ101S*)pDbg)->SendYCGroup(0,20,9);
			}	  
	   SendWrPaSuc();	  
        }
        break;
      default:
      break;    
    }
    
 return TRUE;
}
void CProtocol::SendWrPaSuc()
{
    BYTE Style = 125;
    BYTE Reason = 0X0D;
    SendFrameHeadForPa(Style, Reason);
    memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],&pReceiveFrame->Frame69.InfoAddr[0],6);
    m_SendBuf.wWritePtr += 6;//��Ϣ���ַ2+�ļ���2+����1+�ڳ���1
    SendFrameTailForPa();
    return;

}
void CProtocol::SendReadRecData(WORD FileName,WORD SecName)
{
    BYTE Style = 125;
    BYTE Reason = 0X0D;
    WORD wSecLenPtr;
   // WORD wPaTotalNum = 0;
    //WORD wPaSendNum = 0;
    WORD wInfoAddr = 0;
    unsigned char ucTemp[230]; //��ʱ����
    unsigned long ulTemp;    //��ʱ����
    BYTE j = 0;
    SendFrameHeadForPa(Style, Reason);

    wInfoAddr = MAKEWORD(pReceiveFrame->Frame69.InfoAddr[0],pReceiveFrame->Frame69.InfoAddr[1]);
    wInfoAddr += m_wSendPaNum;
      
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = LOBYTE(wInfoAddr);//д��Ϣ���ַ
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = HIBYTE(wInfoAddr);
    
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(FileName);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(FileName);
    
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(SecName);
  //   m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(SecName);
    
    wSecLenPtr = m_SendBuf.wWritePtr++;
    WORD i = 0;
    WORD k = 0;
    switch(FileName)
    {
      
   case 33://��¼������

        if(SecName <= 4)
        {
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(16664);
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(16664);
        }
        else
        {
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(781);
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(781);
        }
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(0);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(0);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(0xF800);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(0xF800);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(2048);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(2048);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(800);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(800);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(224);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(224);
        
        for(j = 9;j > 0;j--)
        {
            ulTemp = ((unsigned long)g_sRecData.m_CurWritePageCom - j*16 + 16) * 256;
            Sst26vf064b_Read(ulTemp,ucTemp,14);
            for(i = 0;i < 14;i++)  //,wPaSendNum++
            {
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = ucTemp[i];
            }
        }
        m_SendBuf.pBuf[wSecLenPtr] = 138;
        
        if (SecName <= 4)
            m_SendBuf.pBuf[4] |= 0x80;//���������λ������ʾ����û��������
        SendFrameTailForPa();
        break;
   case 34://��¼������
       ulTemp = (unsigned long)(0x30+(SecName-1)*5)<<16;//flash��ַ
       for(k= 0;k<100;k++)   //wPaSendNum++
       {
        SendFrameHeadForPa(Style, Reason);
        wInfoAddr = MAKEWORD(pReceiveFrame->Frame69.InfoAddr[0],pReceiveFrame->Frame69.InfoAddr[1]);
        wInfoAddr += m_wSendPaNum;      
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = LOBYTE(wInfoAddr);//д��Ϣ���ַ
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = HIBYTE(wInfoAddr);    
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(FileName);
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(FileName);    
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(SecName);
        Sst26vf064b_Read(ulTemp,ucTemp,128);
        for(i = 0;i < 128;i++)   //wPaSendNum++
        {
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ucTemp[i];         
        }
        //m_SendBuf.pBuf[wSecLenPtr] = 128;
        WDG_CLR;
        if (k <99)
            m_SendBuf.pBuf[4] |= 0x80;//���������λ������ʾ����û��������
        SendFrameTailForPa();
        ulTemp+=128;
	 delayms(100);	
       }
        break;
     }   
    
    return;
}
void CProtocol::SendReadPa(WORD FileName,BYTE SecName)
{
    BYTE Style = 125;
    BYTE Reason = 0X0D;
    WORD wSecLenPtr;
    WORD wPaTotalNum = 0;
    WORD wPaSendNum = 0;
    WORD wInfoAddr = 0;

    SendFrameHeadForPa(Style, Reason);
    //if(FileName==9 ||FileName==10 || FileName ==11)//�ŏ| ������վ��
    if(FileName==10 || FileName ==11)//�ŏ| ������վ��
    {
      wInfoAddr = 0x6902;
      m_wSendPaNum = 0;
    }
    else
    {
      wInfoAddr = MAKEWORD(pReceiveFrame->Frame69.InfoAddr[0],pReceiveFrame->Frame69.InfoAddr[1]);
      wInfoAddr += m_wSendPaNum;
    }
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = LOBYTE(wInfoAddr);//д��Ϣ���ַ
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = HIBYTE(wInfoAddr);
    
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(FileName);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(FileName);

    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = SecName;
    
    wSecLenPtr = m_SendBuf.wWritePtr++;
    WORD i = 0;
    switch(FileName)
    {
      case 4://�����в���
        wPaTotalNum = RUN_PARA_NUM-1;
        for(i = 0;i < RUN_PARA_NUM-1;i++,wPaSendNum++)
        {
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_gRunPara[i]);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(g_gRunPara[i]);
        } 
		//�ŏ| ң����ʼ��ַ�޸����в���
		/*
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_gRmtMeas[RM_UA]);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(g_gRmtMeas[RM_UA]);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_gRmtMeas[RM_UB]);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(g_gRmtMeas[RM_UB]);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_gRmtMeas[RM_UC]);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(g_gRmtMeas[RM_UC]);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_gRmtMeas[RM_U0]);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(g_gRmtMeas[RM_U0]);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_gRmtMeas[RM_I0]);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(g_gRmtMeas[RM_I0]);

	   m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_ucPara_stime;   //�ŏ| 0329  �����ϴ����ʱ��
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;//�ŏ| 0329  �����ϴ����ʱ��
          *///�ŏ| ң����ʼ��ַ�޸����в���
        break;
     case 5://��IEC����
       wPaTotalNum = IEC_PARA_NUM-1;
       for(i = 0;i < IEC_PARA_NUM-1;i++,wPaSendNum++)
        {
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_ucPara101[i];
        } 
       break;
     case 6://��ң�ŵ��
        wPaTotalNum = g_ucYxTransNum;//IEC_YX_NUM;
        for(i = m_wSendPaNum;(i < g_ucYxTransNum) && (m_SendBuf.wWritePtr < 250);i++,wPaSendNum++)
        {
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_ucYXAddr[i]);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(g_ucYXAddr[i]);
        }
        break;
     case 7://��ң����
        wPaTotalNum = g_ucYcTransNum;
        for(i = 0;i < g_ucYcTransNum;i++,wPaSendNum++)
        {
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_ucYCAddr[i];
        } 
        break;
      
      case 8://��ң�ص��
       /*  wPaTotalNum = IEC_YK_NUM;
         for(i = 0;i < IEC_YK_NUM;i++,wPaSendNum++)
         {
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_ucYKPa[i];
         }*/
         unsigned int uutempp;
            wPaTotalNum = 10;
	   uutempp=(unsigned int)g_unSSoeSaveFlashHead;
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_sRecData.m_gRecCNum;
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;	
	   uutempp=(unsigned int)(g_unSSoeSaveFlashHead>>16);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(uutempp);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(uutempp);	
	   uutempp=g_unSSoeSaveE2ROMPtr;
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(uutempp);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(uutempp);	
	   uutempp=g_unSSoeSaveE2ROMPtr;
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(uutempp);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(uutempp);	
	   uutempp=soe_num;
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(uutempp);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(uutempp);	

         break;
      case 9://��ODU����//�ŏ| ������վ��
         /* if(pOdu == null) break;
           wPaTotalNum = (((CPrtcOdu *)pOdu)->pReceiveFrame)->Frame67.Data[6];
           for(i = 0;i < wPaTotalNum;i++,wPaSendNum++)
           {
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] =(((CPrtcOdu *)pOdu)->pReceiveFrame)->Frame67.Data[7+i];
           }*/
        if(SecName==1)
        {
           wPaTotalNum = CnName_NUM;
	    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 40;		 
           for(i = 0;i < wPaTotalNum;i++,wPaSendNum++)
           {
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_gCnName[i]);	           
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(g_gCnName[i]);	 
              //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_gCnName[i]);		 
           }
        }
	else if(SecName==5)	
        {
           wPaTotalNum = g_gLBNameLen;		   
	    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_gLBNameLen;		 
           for(i = 0;i < wPaTotalNum;i++,wPaSendNum++)
           {
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_gLBName[i];	  
           }
        }		
           break;
    case 10://��ODU����
          /* if(pOdu1 == null) break;
           wPaTotalNum = (((CPrtcOdu *)pOdu1)->pReceiveFrame)->Frame67.Data[6];
           for(i = 0;i < wPaTotalNum;i++,wPaSendNum++)
           {
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] =(((CPrtcOdu *)pOdu1)->pReceiveFrame)->Frame67.Data[7+i];
           }
           *///�ŏ| Ŀ��У׼����λ�����ز��� ��ʼֵΪ��ѹ60V,����2A
           if(SecName==1)
           	{
           		wPaTotalNum = ADJ_PARA_NUM;
           		for(i = 0;i < wPaTotalNum;i++,wPaSendNum++)
           		{
              		m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_gAdjObj[i]);
				m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(g_gAdjObj[i]);	  
           		}
           	}
	    else if(SecName==2)
		{
		unsigned char j;
    		int Temp = 0;
		    	for(j = 0; j < AC_AD_CHAN_NUM; j++)
        		{  
            			for(i = 0; i < 32; i++)
            			{
               			Temp += g_sSampleData.m_gAcAdcData[j][i];
            			}
            			Temp = Temp>>5;

            			g_gAdjPara[j] = Temp + g_gAdjPara[j] ;

            			if( g_gAdjPara[j] <= 1500 ||g_gAdjPara[j] >= 2500)
                			g_gAdjPara[j] = g_gAdjPara[j] - Temp;
            			Temp = 0;
        		}
        		g_gChangFlag[CHNG_ADJ] = ON;  //��У��ϵ���޸ı�־��У����������
		}
	     else if(SecName==4)
		{		
		for(i = 0;i <24;i++)
            		{
                	m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_gModfk[i];
		  	m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
            		}
	     	}
           break;
    case 11:
        switch(SecName)
        {
        case 0://IP�Ͷ˿�
            for(i = 1;i <= 12;i++)
            {
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_gIPPort[i];
		  
            }
            break;        
        case 1://���Ź���
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_gSMSR;
	     break;		
        case 2://APN
            for(i = 1;i <= g_gAPN[0];i++)
            {
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_gAPN[i];
		  
            }
            break;           
        case 3://�û���
        case 4://����
        case 0x0f:
            if(pGprs == null) break;
            wPaTotalNum = 8;//���⴦��ֻȡ����״̬��
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = (((CBJ101S *)pGprs)->pReceiveFrame)->Frame68.Data[0] ^ 1;  //ģ��״̬
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = (((CBJ101S *)pGprs)->pReceiveFrame)->Frame68.Data[1] ^ 1;  //SIM��״̬
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = (((CBJ101S *)pGprs)->pReceiveFrame)->Frame68.Data[9];  //CSQֵ
            g_unZJTrans[2]=(((CBJ101S *)pGprs)->pReceiveFrame)->Frame68.Data[9];  //CSQֵ
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = (((CBJ101S *)pGprs)->pReceiveFrame)->Frame68.Data[10]; //Ƶ��
            for(i = 0; i < 4; i++)                                 //IP
            {
                m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = (((CBJ101S *)pGprs)->pReceiveFrame)->Frame68.Data[15 + i];
            }
            wPaSendNum = 8;            
            break;
        default:
            break;
        }       
        break;
       case 15://���ն˰汾��
           wPaTotalNum = strlen((char *)VerNum);
           for(i = 0;i < wPaTotalNum;i++,wPaSendNum++)
           {
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = VerNum[i];
           }
           #if 1 //for test ���й����в鿴����ֵ
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x56;//V
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x65;//e
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x72;//r
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = VerNumlcd[1]/10+0x30;//5
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = VerNumlcd[1]%10+0x30;//5
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x2e;//.
	    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x30;//0
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x30;//0
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x00;
           
        /*   if(pDbg != null)
           {
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(pDbg->m_SSoeHeadPtr);
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(pDbg->m_SSoeHeadPtr);
           }
           else
           {
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 98;
           }

            if(pGprs != null) 
            {
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(pGprs->m_SSoeHeadPtr);
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(pGprs->m_SSoeHeadPtr);
            }
           else
           {
             m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0; 
             m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 99;
           }*/
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = pGprs->m_WaitConfTpId;
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_ucRefYxYcTs;
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(pGprs->m_resendflag);
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(pGprs->m_reSendDelay);
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(pGprs->m_reSendDelay);
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_yxChangeflag);
           m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(g_yxChangeflag);
           
           
           #endif
           break;
      case 16://���绰���� ������һ֡������
         /* */ //�ŏ| 0328 
	    wPaTotalNum = PHONE_PA_NUM - 1;
           for(i = 0;i < wPaTotalNum;i++,wPaSendNum++)
           {
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_gSmsPhone[i];
           }
           break;
   case 31://��ң��ֵ
        wPaTotalNum = g_ucYxTransNum;//IEC_YX_NUM;
        BYTE bValCur;
        for(i = m_wSendPaNum;(i < g_ucYxTransNum) && (m_SendBuf.wWritePtr < 250);i++,wPaSendNum++)
        {
          bValCur = GetRIStatus(g_ucYXAddr[i]-1);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = bValCur;
        }
        break;
   case 32://��ң��ֵ
        wPaTotalNum = g_ucYcTransNum;
        for(i = m_wSendPaNum;(i < g_ucYcTransNum) && (m_SendBuf.wWritePtr < 250);i++,wPaSendNum++)
        {
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(g_unYcTrans[i]);
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(g_unYcTrans[i]);
        }
        break;
     }
    m_SendBuf.pBuf[wSecLenPtr] = m_SendBuf.wWritePtr - wSecLenPtr -1;
    m_wSendPaNum += wPaSendNum;
    if ((m_wSendPaNum < wPaTotalNum) && (m_wSendPaNum != 0))
      m_SendBuf.pBuf[4] |= 0x80;//���������λ������ʾ����û��������
    SendFrameTailForPa();
    
    if(FileName != 15)//���ն˰汾�ű��ĸ������й����е�ʵʱ�ڴ����ݲ鿴
    {
      if ((m_wSendPaNum < wPaTotalNum) && (m_wSendPaNum != 0))
      {
          delayms(300);
          SendReadPa(FileName,SecName);
      }
    }
    return;
}
void CProtocol::SendWrPaAck(WORD wFile,BYTE bySec)
{
    BYTE Style = 121;
    BYTE Reason = 0X0D;

    SendFrameHeadForPa(Style, Reason);
    memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],&pReceiveFrame->Frame69.InfoAddr[0],5);
    m_SendBuf.wWritePtr += 5;//��Ϣ���ַ2+�ļ���2+����1
    switch(wFile)
    {//�ڳ���
      case 4:
        {
      //    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (RUN_PARA_NUM-1)*2;
        }
        break;
      case 5:
        {//��IEC����״̬
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (IEC_PARA_NUM-1);
        }
        break;
      case 6:
        {
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = IEC_YC_NUM;
        }
        break;
      case 7:
        {
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (BYTE)IEC_YX_NUM;
        }
        break;
      case 8:
        {
       //   m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = IEC_YK_NUM;
        }
        break;
    }
    //FOR OLD ZIZHAN
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
    SendFrameTailForPa();
    return;
}

void CProtocol::SendOneLoadData(WORD wCurLoadNum,WORD wFlag)
{
    BYTE Style = 125;
    BYTE Reason = 0X0D;
    BYTE byLoadDa[32] = {0};
    WORD wLoadMeas[9] = {0};
    unsigned long FLoadAddr;
    WORD wSecLenPtr;

    SendFrameHeadForPa(Style, Reason);
    memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],&pReceiveFrame->Frame69.InfoAddr[0],5);
    m_SendBuf.wWritePtr += 5;//��Ϣ���ַ2+�ļ���2+����1
    wSecLenPtr = m_SendBuf.wWritePtr++;
    
    FLoadAddr = FADDR_LOAD_START + (wCurLoadNum * FLASH_PLOAD_LEN);
    Sst26vf064b_Read(FLoadAddr,byLoadDa, FLASH_PLOAD_LEN);
    if(byLoadDa[0] != 0xEA || wFlag == 1)
      m_SendBuf.pBuf[wSecLenPtr] = 0;//�ڳ���
    else
    {
      memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],&(byLoadDa[2]),5);//5�ֽ�ʱ��
      m_SendBuf.wWritePtr += 5;
      BYTE YcIndex = 0;
      memcpy(wLoadMeas,&byLoadDa[7],18);
      for(int i = 0; i < g_ucYcTransNum; i++)   //������Ҫ�ϴ���ң��ֵ�����վ���������л���
       	{      		
          	YcIndex = g_ucYCAddr[i] - 1;             
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(wLoadMeas[YcIndex]);
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(wLoadMeas[YcIndex]);              	
       	}
       m_SendBuf.pBuf[wSecLenPtr] = m_SendBuf.wWritePtr - wSecLenPtr -1;
    }
    SendFrameTailForPa();
  
}


//==============================================================================
//  ��������   : SendLoadData
//  ��������   : ���͸�������
//  �������   : bySec:���� =0��ʾ��ȡ���һ�� 1:�����и������� 2:��ȡָ������
//               wStartNo:��ȡ�ĸ���������ʼ��� min=0 
//               wLoadNum:��Ҫ��ȡ�ĸ�����������(��ʱδʹ��)
//               byQds
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       : 
//==============================================================================

void CProtocol::SendLoadData(BYTE bySec,WORD wStartNo,WORD w_LoadNum,BYTE byScq)
{
     //WORD wLoadNum = w_LoadNum;
      WORD wCurLoadNum = 0;
      static unsigned int FLoadInfo[FLOADINFONUM];
      
      //if(byScq == 0)
        CAT_SpiReadWords(EEPADD_LOADNUM, FLOADINFONUM, FLoadInfo);
      if((FLoadInfo[FLOAD_CS] != FLoadInfo[FLOAD_TOTALNUM] + FLoadInfo[FLOAD_NEW] + FLoadInfo[FLOAD_OLD]) || FLoadInfo[FLOAD_TOTALNUM] > FLASH_LOAD_MAXNUM || FLoadInfo[FLOAD_NEW] > FLASH_LOAD_MAXNUM)//���FLASH��ַ���ڸ��ɼ�¼����������
      {
          FLoadInfo[FLOAD_TOTALNUM] = 0; //��ո��ɼ�¼������
          FLoadInfo[FLOAD_NEW] = 0; //�����洢�ļ�¼���һ����¼��ƫ��λ��(�����Ѵ洢������һ����¼��λ��)
          FLoadInfo[FLOAD_OLD] = 0;//����һ����¼��λ��
      }
      //if(bySec == 0 || bySec == 1)
      { 
          if(FLoadInfo[FLOAD_NEW] == 0)
            wCurLoadNum = FLASH_LOAD_MAXNUM -1;
          else
            wCurLoadNum = FLoadInfo[FLOAD_NEW]-1; 
          if(FLoadInfo[FLOAD_TOTALNUM] == 0)//��û�и��ɼ�¼���ϴ�һ֡û�����ݵı���
          {
            SendOneLoadData(wCurLoadNum,1);
            return;
          }
          if(bySec == 0)
          {//������1��
            SendOneLoadData(wCurLoadNum); 
          }
          else
          {//�����¼�¼��ʼ����ȡN��
            if(wStartNo > wCurLoadNum)
              wCurLoadNum = FLASH_LOAD_MAXNUM - (wStartNo - wCurLoadNum);
            else
              wCurLoadNum = wCurLoadNum - wStartNo;
			
            if(wStartNo >= FLoadInfo[FLOAD_TOTALNUM])
            {
              SendOneLoadData(wCurLoadNum,1);
              return;
            }
            SendOneLoadData(wCurLoadNum);
          }
      }
//        else if(bySec == 2)
//        {//�����ϼ�¼��ʼ����ȡN��
//            wCurLoadNum = FLoadInfo[FLOAD_OLD] + wStartNo;
//            if(wCurLoadNum >= FLASH_LOAD_MAXNUM)
//              wCurLoadNum = 0;
//            SendOneLoadData(wCurLoadNum);
//       }
    return;
  
}

void CProtocol::SendSavedSoe(BYTE bySec,WORD wStartNo,WORD wSOENum)
{
    BYTE Style = 125;
    BYTE Reason = 0X0D;
    BYTE bySoeDa[180];
    BYTE bySoeNum = 0;
    //BYTE bySdSoeNum = 0;
    BYTE byCurSoeNum = SOE_NUM_SEG/2;
	int tempx;
    if(bySec == 0)
    {//������SOE   
      for(int i = 1; i <= soe_num;i=i+16)//�ŏ| SOE��FLASH     
      {
    WatchDog(); 
    tempx=i+15;
    if(tempx>soe_num)
		tempx=soe_num;
    bySoeNum = ReadSoe(bySoeDa,0,i,tempx);//�ŏ| SOE��FLASH
   
        if(bySoeNum <= SOE_NUM_SEG/2)
          byCurSoeNum = bySoeNum; 

        SendFrameHeadForPa(Style, Reason);
        memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],&pReceiveFrame->Frame69.InfoAddr[0],5);
       m_SendBuf.wWritePtr += 5;//��Ϣ���ַ2+�ļ���2+����1
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = byCurSoeNum*SOE_SENDDA_LEN; 
        
        memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],bySoeDa,byCurSoeNum*SOE_SENDDA_LEN);
        m_SendBuf.wWritePtr += byCurSoeNum*SOE_SENDDA_LEN;
        //bySdSoeNum = byCurSoeNum;
        SendFrameTailForPa();
        
        delayms(1000);//�ӽӿ�����в鿴����
	
      }
    }
    else if(bySec == 2)
    {//���ζ�
        BYTE read_seg = 0;
        BYTE StartNo_seg = 0;
        read_seg = ( read_seg + (wStartNo / SOE_NUM_SEG)) % SOE_SEG;
        StartNo_seg = wStartNo%SOE_NUM_SEG;
        bySoeNum = ReadSoe(bySoeDa,read_seg,StartNo_seg,wSOENum);
        if(bySoeNum <= SOE_NUM_SEG/2)
          byCurSoeNum = bySoeNum; 

        SendFrameHeadForPa(Style, Reason);
        memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],&pReceiveFrame->Frame69.InfoAddr[0],5);
        m_SendBuf.wWritePtr += 5;//��Ϣ���ַ2+�ļ���2+����1
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = byCurSoeNum*SOE_SENDDA_LEN; 
        
        memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],bySoeDa,byCurSoeNum*SOE_SENDDA_LEN);
        m_SendBuf.wWritePtr += byCurSoeNum*SOE_SENDDA_LEN;
        //bySdSoeNum = byCurSoeNum;
        SendFrameTailForPa();
    }
    return;

}
void CProtocol::SendSoeSavedStu()
{
    BYTE Style = 125;
    BYTE Reason = 0X0D;
    WORD wSecLenPtr;

    SendFrameHeadForPa(Style, Reason);
    memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],&pReceiveFrame->Frame69.InfoAddr[0],5);
    m_SendBuf.wWritePtr += 5;//��Ϣ���ַ2+�ļ���2+����1
    wSecLenPtr = m_SendBuf.wWritePtr++;
   // read_ptr();
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = soe_area;
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = soe_ptr;
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(soe_num);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(soe_num);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;
    
    m_SendBuf.pBuf[wSecLenPtr] = m_SendBuf.wWritePtr - wSecLenPtr -1;
    SendFrameTailForPa();
    return;

}
void CProtocol::SendTrigAck()
{
return;
}

//��֯����ͷ,�̶���ʽ����
BOOL CProtocol::SendFrameHeadForPa(BYTE Style, BYTE Reason)
{
      m_SendBuf.wReadPtr = 0;
      m_SendBuf.wWritePtr = 0;
      if(pReceiveFrame->Frame69.Start1 == 0x69)
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr]= 0x69;
      else
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr]= 0x68;
      m_SendBuf.wWritePtr += 3;//���ĳ��ȣ�Ԥ������֡βʱ����
      if(pReceiveFrame->Frame69.Start1 == 0x69)
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x69;
      else
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x68;
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x53; //���������óɹ̶�ֵ index=4
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = LOBYTE(g_gRunPara[RP_COMM_ADDR]);//g_gRunPara[RP_COMM_ADDR]
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = HIBYTE(g_gRunPara[RP_COMM_ADDR]);//g_gRunPara[RP_COMM_ADDR]
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = Style;
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x01;//�ɱ�ṹ�޶��ʣ����óɹ̶�ֵ
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = LOBYTE(Reason);

      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = HIBYTE(Reason);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = LOBYTE(g_gRunPara[RP_COMM_ADDR]);//g_gRunPara[RP_COMM_ADDR]
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = HIBYTE(g_gRunPara[RP_COMM_ADDR]);//g_gRunPara[RP_COMM_ADDR]

    return TRUE;
}

//��֯����β����������֡����
BOOL CProtocol::SendFrameTailForPa(void)
{
    m_SendBuf.pBuf[1] = m_SendBuf.pBuf[2] = m_SendBuf.wWritePtr - 4;//���ĳ���
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (BYTE)ChkSum((BYTE *)&m_SendBuf.pBuf[4], m_SendBuf.pBuf[2]);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x16;
    CommWrite((char *)(m_SendBuf.pBuf+m_SendBuf.wReadPtr),m_SendBuf.wWritePtr-m_SendBuf.wReadPtr,0);
    

    return TRUE;
}





