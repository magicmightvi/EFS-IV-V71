/*------------------------------------------------------------------------
 Module:      pcolsec.cpp
 Author:      linxueqin
 Project:     规约平台
 State:       V1.0
 Creation Date:   2014-4-28
 Description:   规约基类的定义
------------------------------------------------------------------------*/

#include "Pcolsec.h"
#include "..\GB104s\Gb104pub.h"


/***************************************************************
  Function:CPSecond
    规约基类构造函数
  参数：无
  返回：无
***************************************************************/
CPSecondary::CPSecondary():CProtocol()
{
    m_wEqpNum = 1;
    m_wEqpNo = 0;
  DisableRetry();
  
}

/***************************************************************
  Function:Init
      规约平台初始化
  参数：无
  返回：TRUE 成功，FALSE 失败
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
    初始化装置信息
  参数：无
  
  返回：TRUE 成功，FALSE 失败
***************************************************************/
BOOL CPSecondary::InitEqpInfo(WORD wMinEqpNum)
{
        
        m_pEqpInfo = &m_VPtEqpInfo;//new VPtEqpInfo[m_wEqpNum];
        memset(m_pEqpInfo,0,sizeof(VPtEqpInfo));
        SetEqpInfo();
        
        byChangeYcInitFlag = 0;//1=系统启动时传输变化遥测
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
        m_pEqpInfo[0].wLinkAddress =  g_gRunPara[RP_COMM_ADDR];    //g_gRunPara[RP_COMM_ADDR]子站地址
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
    获取自身地址
  参数：无
  
  返回：自身地址
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
//校验和
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
    获取当前的广播地址
  参数：无
  
  返回：DWORD广播地址
***************************************************************/
WORD CPSecondary::GetBroadcastAddr(void)
{
  return  m_pBaseCfg->wBroadcastAddr;
}
//获取最大重发次数
WORD CPSecondary::GetMaxRetryCount(void)
{
  return  m_pBaseCfg->wMaxRetryCount;
}
//获取重发标志
BOOL CPSecondary::GetRetryFlag(void)
{
  if (m_wRetryCount <  m_pBaseCfg->wMaxRetryCount)
    return TRUE;
  //if (m_wRetryCount!=65535)  CommCtrl(m_wTaskID, CCC_EVENT_CTRL|(m_wCommCtrl<<1), null);
  m_wRetryCount = 0;
  return FALSE;
}


//获取重发次数
WORD CPSecondary::GetRetryCount(void)
{
  return m_wRetryCount;
}

//重发次数清0
void CPSecondary::ResetRetryCount(void)
{ 
  m_wRetryCount = 0;
}

//重发次数加1
void CPSecondary::IncRetryCount(void)
{ 
  m_wRetryCount++;
}

//禁止重发
void CPSecondary::DisableRetry(void)
{  
  m_wRetryCount = 65535;
}

//重发报文
BOOL CPSecondary::SendRetry(void)
{
  m_SendBuf.wReadPtr = m_SendBuf.wOldReadPtr; 
  WriteToComm(m_SendBuf.dwFlag);
  return TRUE;  
}

/*系统重启*/
void CPSecondary::sysRestart(DWORD dwRestartType)
{
  CloseTcp(dwRestartType);
  delay(300);
  //IWDG_SetReload(0x01FF); // 计数器数值,设置2s如果没有喂狗，复位。实际应用 
  //IWDG_ReloadCounter(); /*重启计数器*/
  //IWDG_Enable(); /*启动看门狗*/
  //__disable_irq();  //关闭全部中断
  while(1);


}
/***************************************************************
  Function:WriteToComm
    向通信端口写数据
  参数：Address,Flag
    Address 地址，暂未用
    Flag　标志，暂未用
  返回：实际写到端口的数据的字节个数 <0 ERROR
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
    判断有没有突变遥测需要上传
  参数:
  返回：
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
                    if(m_uartId == g_CmIdGPRS) g_yxChangeflag |= BITF;//g_yxChangeflag的最高位标识有报文需要上传,开GPRS
                    break;//只要检测到一个突变遥测即置标识位
                 }
             } 
          }*/
    }
  }
  return;

}

/***************************************************************
  Function: SearchChangeYC
    检索变化遥测
  参数：wNum 读取的遥测个数  目前是检索所有遥测  m_pEqpInfo[m_wEqpNo].wYCNum
    buf  装变化遥测的信息

  返回：有变化的遥测个数 （目前为总个数）
***************************************************************/
WORD CPSecondary::SearchChangeYC(WORD wNum, VDBYCF *pBuf)
{
    #define FRM_CHGYCNUM 46  //每帧传输变化遥测个数
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
    读取遥测量
  参数：wBeginNo 遥测序号（从0开始）
    wNum   需要读取的遥测个数  字为单位
    buf    保存读取的遥测值

  返回：未使用
***************************************************************/
WORD CPSecondary::ReadRangeYC(WORD wBeginNo,WORD wNum, short *buf)
{
  memcpy(buf,&(g_unYcTrans[wBeginNo]),wNum*sizeof(WORD));
  return 1;
}
//  /***************************************************************
//    Function: ReadRangeSYXBit
//      读取单点遥信
//    参数：wBeginNo 遥信序号（从0开始）
//      wNum   需要读取的遥信个数  从1开始
//      buf    保存读取的遥信值
//  
//    返回：未使用
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
    获取装置信息
  参数：wDevNo
    wDevNo　装置号（数组序号）
  
  返回：指向装置信息的指针
***************************************************************/
VPtEqpInfo* CPSecondary::pGetEqpInfo(WORD wEqpNo)
{
  if (wEqpNo >= m_wEqpNum)
    return 0;
  return &m_pEqpInfo[wEqpNo];
}

//flag= 0:读出位并清0 1:只读不清
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
