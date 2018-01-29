/*------------------------------------------------------------------------
 Module:      pcolsec.cpp
 Author:      linxueqin
 Project:     ��Լƽ̨
 State:       V1.0
 Creation Date:   2014-4-28
 Description:   ��Լ����Ķ���
------------------------------------------------------------------------*/

#include "Pcolsec.h"
#include "..\GB104s\Gb104pub.h"


/***************************************************************
  Function:CPSecond
    ��Լ���๹�캯��
  ��������
  ���أ���
***************************************************************/
CPSecondary::CPSecondary():CProtocol()
{
    m_wEqpNum = 1;
    m_wEqpNo = 0;
  DisableRetry();
  
}

/***************************************************************
  Function:Init
      ��Լƽ̨��ʼ��
  ��������
  ���أ�TRUE �ɹ���FALSE ʧ��
***************************************************************/
BOOL CPSecondary::Init(WORD wMinEqpNum)
{
  if (!InitEqpInfo(wMinEqpNum))
  {
    return FALSE;
  }
  SetBaseCfg();

    
  m_SendBuf.wBufSize = MAX_SEND_LEN;//m_pBaseCfg->wMaxDLlen+m_pBaseCfg->wMaxDLlen/2;
  m_RecBuf.wBufSize = MAX_RECV_LEN;//2*m_pBaseCfg->wMaxDLlen;

  
        
  m_SendBuf.pBuf = m_SBuf;
  m_RecBuf.pBuf = m_RBuf;

  return TRUE;  
}
/***************************************************************
  Function:InitEqpInfo
    ��ʼ��װ����Ϣ
  ��������
  
  ���أ�TRUE �ɹ���FALSE ʧ��
***************************************************************/
BOOL CPSecondary::InitEqpInfo(WORD wMinEqpNum)
{
        
        m_pEqpInfo = &m_VPtEqpInfo;//new VPtEqpInfo[m_wEqpNum];
        memset(m_pEqpInfo,0,sizeof(VPtEqpInfo));
        SetEqpInfo();
        
        byChangeYcInitFlag = 0;//1=ϵͳ����ʱ����仯ң��
        for(int i = 0;i < IEC_YC_NUM;i++)
        {
          //m_pEqpInfo[0].CurYc[i] = 0;
          m_pEqpInfo[0].OldYc[i] = 0;
          //m_pEqpInfo[0].YcPara[i] = 1;
          
        }
  return TRUE;
}
BOOL CPSecondary::SetEqpInfo(void)
{
        m_pEqpInfo[0].wLinkAddress =  g_gRunPara[RP_COMM_ADDR];    //g_gRunPara[RP_COMM_ADDR]��վ��ַ
        m_pEqpInfo[0].wAddress =  g_gRunPara[RP_COMM_ADDR];//
        m_pEqpInfo[0].wSYXNum     = g_ucYxTransNum;
        m_pEqpInfo[0].wDYXNum     = 0;
        m_pEqpInfo[0].wYCNum      = g_ucYcTransNum;
        m_pEqpInfo[0].wYTNum      = 0;
        m_pEqpInfo[0].wYKNum      = g_ucYkTransNum;
	m_pEqpInfo[0].wZJNum      = 3;//g_ucZJTransNum;	

        m_pEqpInfo[0].wYxType     =  M_SP_NA;
        m_pEqpInfo[0].wSoeType    =  M_SP_TB;
        return TRUE;
}


/***************************************************************
  Function:GetOwnAddr
    ��ȡ�����ַ
  ��������
  
  ���أ������ַ
***************************************************************/
WORD CPSecondary::GetOwnAddr(void)      
{
  return  m_pEqpInfo[m_wEqpNo].wAddress;
}

BOOL CPSecondary::SwitchToAddress(WORD wAddress)
{
    if ((wAddress == GetBroadcastAddr())||(wAddress==GetOwnAddr()))
    return TRUE;
  return FALSE;
}
//У���
BYTE CPSecondary::ChkSum(BYTE *buf, WORD len)
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
WORD CPSecondary::ReadSoeFromFlash(WORD *pHeadPtr,WORD TailPtr,BYTE bSoeMaxNum,BYTE *pSoeBuf)
{
    int RecNum = 0;
   /* BYTE bSoeBufTmp[SOE_DATA_LEN] = {0};
    while((*pHeadPtr) != TailPtr)
    {
        Sst26vf064b_Read(FADDR_SOE_START + (*pHeadPtr)*FSOE_LEN,bSoeBufTmp, SOE_DATA_LEN);
        if(bSoeBufTmp[0] == SOEVALID)
        {
          memcpy(pSoeBuf,bSoeBufTmp,SOE_DATA_LEN);
          pSoeBuf += SOE_DATA_LEN;
          RecNum++; 
        }
        (*pHeadPtr)++;
        (*pHeadPtr) &= (FSOE_TOLNUM - 1);
        if(RecNum >= bSoeMaxNum) 
            return RecNum;
     }*/
     
  return RecNum;
  
}

WORD CPSecondary::GetCosNum(WORD HeadPtr,WORD TailPtr,BYTE bSoeBufLen)
{
    int RecNum = 0;
    if(HeadPtr <= TailPtr) 
      RecNum = TailPtr - HeadPtr;
    else
      RecNum = bSoeBufLen - HeadPtr + TailPtr;//SOE_BUFF_NUM 
  return RecNum;
  
}

void CPSecondary::SetBaseCfg(void)
{
    m_pBaseCfg = &m_VProtocolBaseCfg;
    m_pBaseCfg->wMaxRetryCount=3;
    m_pBaseCfg->wBroadcastAddr=0xFF;
}

/***************************************************************
  Function:GetGBAddress
    ��ȡ��ǰ�Ĺ㲥��ַ
  ��������
  
  ���أ�DWORD�㲥��ַ
***************************************************************/
WORD CPSecondary::GetBroadcastAddr(void)
{
  return  m_pBaseCfg->wBroadcastAddr;
}
//��ȡ����ط�����
WORD CPSecondary::GetMaxRetryCount(void)
{
  return  m_pBaseCfg->wMaxRetryCount;
}
//��ȡ�ط���־
BOOL CPSecondary::GetRetryFlag(void)
{
  if (m_wRetryCount <  m_pBaseCfg->wMaxRetryCount)
    return TRUE;
  //if (m_wRetryCount!=65535)  CommCtrl(m_wTaskID, CCC_EVENT_CTRL|(m_wCommCtrl<<1), null);
  m_wRetryCount = 0;
  return FALSE;
}


//��ȡ�ط�����
WORD CPSecondary::GetRetryCount(void)
{
  return m_wRetryCount;
}

//�ط�������0
void CPSecondary::ResetRetryCount(void)
{ 
  m_wRetryCount = 0;
}

//�ط�������1
void CPSecondary::IncRetryCount(void)
{ 
  m_wRetryCount++;
}

//��ֹ�ط�
void CPSecondary::DisableRetry(void)
{  
  m_wRetryCount = 65535;
}

//�ط�����
BOOL CPSecondary::SendRetry(void)
{
  m_SendBuf.wReadPtr = m_SendBuf.wOldReadPtr; 
  WriteToComm(m_SendBuf.dwFlag);
  return TRUE;  
}

/*ϵͳ����*/
void CPSecondary::sysRestart(DWORD dwRestartType)
{
  CloseTcp(dwRestartType);
  delay(300);
  //IWDG_SetReload(0x01FF); // ��������ֵ,����2s���û��ι������λ��ʵ��Ӧ�� 
  //IWDG_ReloadCounter(); /*����������*/
  //IWDG_Enable(); /*�������Ź�*/
  //__disable_irq();  //�ر�ȫ���ж�
  while(1);


}
/***************************************************************
  Function:WriteToComm
    ��ͨ�Ŷ˿�д����
  ������Address,Flag
    Address ��ַ����δ��
    Flag����־����δ��
  ���أ�ʵ��д���˿ڵ����ݵ��ֽڸ��� <0 ERROR
***************************************************************/
int CPSecondary::WriteToComm(DWORD dwFlag)
{
  int Rc=0; 

  if (m_SendBuf.wWritePtr - m_SendBuf.wReadPtr)
  {
    m_SendBuf.wOldReadPtr = m_SendBuf.wReadPtr;

    m_SendBuf.dwFlag = dwFlag;
    IncRetryCount();
    
    Rc = CommWrite((char *)(m_SendBuf.pBuf+m_SendBuf.wReadPtr),m_SendBuf.wWritePtr-m_SendBuf.wReadPtr,dwFlag);

    if (Rc!=ERROR)      m_SendBuf.wReadPtr += Rc;
    if (LOWORD(dwFlag) == GetBroadcastAddr())
    {
      DisableRetry();
      return Rc;
    }
  }
  return Rc;
}
/***************************************************************
  Function: CheckChangeYC
    �ж���û��ͻ��ң����Ҫ�ϴ�
  ����:
  ���أ�
***************************************************************/
void CPSecondary::CheckChangeYC()
{
  WORD i=0;
  //short  wCurYcVal,wOldYcVal;
  //WORD wYcDval;
  WORD  CurYc[IEC_YC_NUM];
  WORD wYcNum = m_pEqpInfo[0].wYCNum;
  memcpy(CurYc,g_unYcTrans,wYcNum*sizeof(WORD));
  if(byChangeYcInitFlag == 0)
  {
    byChangeYcInitFlag = 1;
    memcpy(m_pEqpInfo[m_wEqpNo].OldYc,CurYc,wYcNum*2);
  }
  else
  {
    for (i=0; i < wYcNum; i++)
    {
    /*    if((g_ucYCAddr[i] - 1 != RM_BAT_U)
           && (g_ucYCAddr[i] - 1 != RM_CAP_U)
           && (g_ucYCAddr[i] - 1 != RM_SUN_U)
           )
         {
              wCurYcVal = CurYc[i];
              wOldYcVal = m_pEqpInfo[m_wEqpNo].OldYc[i];
              if(wCurYcVal!=(short)INVALIDYC)
              {
                if(wOldYcVal==(short)INVALIDYC)
                    wOldYcVal=0;
                if(wCurYcVal > wOldYcVal)
                  wYcDval = wCurYcVal - wOldYcVal;
                else
                  wYcDval = wOldYcVal - wCurYcVal;
                if((((wYcDval > g_gRunPara[RP_YCLIMIT])&&(g_ucYCAddr[i]<RM_TEMP_START))&&((wYcDval*100)>(g_gRunPara[RP_YCCAP]*wOldYcVal)))||((g_ucYCAddr[i]>=RM_TEMP_START)&&(wYcDval > g_gRunPara[RP_TELIMIT])&&(g_ucYCAddr[i]!=255)))//m_pEqpInfo[m_wEqpNo].YcPara[i]
                 {
                    m_SendChgYcFlag = 1;
                    if(m_uartId == g_CmIdGPRS) g_yxChangeflag |= BITF;//g_yxChangeflag�����λ��ʶ�б�����Ҫ�ϴ�,��GPRS
                    break;//ֻҪ��⵽һ��ͻ��ң�⼴�ñ�ʶλ
                 }
             } 
          }*/
    }
  }
  return;

}

/***************************************************************
  Function: SearchChangeYC
    �����仯ң��
  ������wNum ��ȡ��ң�����  Ŀǰ�Ǽ�������ң��  m_pEqpInfo[m_wEqpNo].wYCNum
    buf  װ�仯ң�����Ϣ

  ���أ��б仯��ң����� ��ĿǰΪ�ܸ�����
***************************************************************/
WORD CPSecondary::SearchChangeYC(WORD wNum, VDBYCF *pBuf)
{
    #define FRM_CHGYCNUM 46  //ÿ֡����仯ң�����
    WORD i=0,wRtNum = 0;
   // short  wCurYcVal,wOldYcVal;
  //WORD wYcDval;
        WORD  CurYc[IEC_YC_NUM];
  memcpy(CurYc,g_unYcTrans,wNum*sizeof(WORD));
  if(byChangeYcInitFlag == 0)
  {
    byChangeYcInitFlag = 1;
    memcpy(m_pEqpInfo[m_wEqpNo].OldYc,CurYc,wNum*2);
  }
  else
  {
    for (i=0; i < wNum; i++)
    {
       /* if((g_ucYCAddr[i] - 1 != RM_BAT_U)
           && (g_ucYCAddr[i] - 1 != RM_CAP_U)
           && (g_ucYCAddr[i] - 1 != RM_SUN_U)
          )
         {
              wCurYcVal = CurYc[i];
              wOldYcVal = m_pEqpInfo[m_wEqpNo].OldYc[i];
              if(wCurYcVal!=(short)INVALIDYC)
              {
                if(wOldYcVal==(short)INVALIDYC)
                    wOldYcVal=0;
                if(wCurYcVal > wOldYcVal)
                  wYcDval = wCurYcVal - wOldYcVal;
                else
                  wYcDval = wOldYcVal - wCurYcVal;
            if((((wYcDval > g_gRunPara[RP_YCLIMIT])&&(g_ucYCAddr[i]<RM_TEMP_START))&&((wYcDval*100)>(g_gRunPara[RP_YCCAP]*wOldYcVal)))||((g_ucYCAddr[i]>=RM_TEMP_START)&&(wYcDval > g_gRunPara[RP_TELIMIT])&&(g_ucYCAddr[i]!=255)))//m_pEqpInfo[m_wEqpNo].YcPara[i]
                {
          
                  pBuf->nValue = CurYc[i];
                  pBuf->wNo = i;
                  m_pEqpInfo[m_wEqpNo].OldYc[i] = CurYc[i];
                  wRtNum++;
                  pBuf++;
                  if(wRtNum >= FRM_CHGYCNUM)
                    break;
                }
             } 
          }*/
    }
  }
  return wRtNum;

}
/***************************************************************
  Function: ReadRangeYC
    ��ȡң����
  ������wBeginNo ң����ţ���0��ʼ��
    wNum   ��Ҫ��ȡ��ң�����  ��Ϊ��λ
    buf    �����ȡ��ң��ֵ

  ���أ�δʹ��
***************************************************************/
WORD CPSecondary::ReadRangeYC(WORD wBeginNo,WORD wNum, short *buf)
{
  memcpy(buf,&(g_unYcTrans[wBeginNo]),wNum*sizeof(WORD));
  return 1;
}
//  /***************************************************************
//    Function: ReadRangeSYXBit
//      ��ȡ����ң��
//    ������wBeginNo ң����ţ���0��ʼ��
//      wNum   ��Ҫ��ȡ��ң�Ÿ���  ��1��ʼ
//      buf    �����ȡ��ң��ֵ
//  
//    ���أ�δʹ��
//  ***************************************************************/
//  WORD CPSecondary::ReadRangeSYXBit(WORD wBeginNo,WORD wNum,BYTE *buf)
//  {
//    unsigned int wBuf[IEC_YX_NUM/16+1] = {0};
//    WORD wDataNum = 0;
//  
//  
//    wDataNum = wBeginNo & 0x0f;
//    wDataNum = (wDataNum + wNum-1)>>4;
//    memcpy(wBuf,g_unYxTrans,(wDataNum+1)*sizeof(unsigned int));
//  
//    WORD i=0,j=0;
//    WORD wValue = 0;
//    for(i = 0; (i< wNum); i++)
//    {
//      wValue = (wBuf[j] & BIT[(wBeginNo + i) & 0x0f]);
//      *(buf++) = wValue ? 1 : 0;
//      if(((wBeginNo + i + 1) & 0x0f) == 0)
//        j++;
//    }
//    return 1;
//  }
/***************************************************************
  Function:pGetEqpInfo
    ��ȡװ����Ϣ
  ������wDevNo
    wDevNo��װ�úţ�������ţ�
  
  ���أ�ָ��װ����Ϣ��ָ��
***************************************************************/
VPtEqpInfo* CPSecondary::pGetEqpInfo(WORD wEqpNo)
{
  if (wEqpNo >= m_wEqpNum)
    return 0;
  return &m_pEqpInfo[wEqpNo];
}

//flag= 0:����λ����0 1:ֻ������
BOOL CPSecondary::SearchCos(WORD wBit,BYTE flag)
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
