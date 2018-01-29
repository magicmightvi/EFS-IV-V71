/*------------------------------------------------------------------------
 Module:      Comm.cpp
 Author:      linxueqin
 Project:     子站平台
 State:       V1.0
 Creation Date:   2014-4-29
 Description:   串口初始化，接收数据，发送数据，RF通道各指示器通信状态检测
------------------------------------------------------------------------*/

#ifndef _COMM
#define _COMM
  #include "DataStruct.h"
#endif
const static unsigned short fcsTab[256] = {
      0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
      0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
      0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
      0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
      0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
      0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
      0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
      0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
      0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
      0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
      0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
      0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
      0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
      0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
      0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
      0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
      0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
      0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
      0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
      0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
      0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
      0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
      0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
      0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
      0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
      0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
      0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
      0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
      0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
      0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
      0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
      0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
   };

//==============================================================================
//  函数名称   : InitComm
//  功能描述   : 通信模块初始化，包括对缓冲区的初始化
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：l
//==============================================================================
void InitComm(void)
{
    InitCommProgram();
    InitSCI();
    
}

//==============================================================================
//  函数名称   : CrcCount
//  功能描述   : 根据传递进来的数组指针p，计算长度为length字的CRC校验码
//  输入参数   : p－需校验数据的头指针
//           length－需校验数据的长度
//  输出参数   :<无>
//  返回值     : crcvalue:校验后得到的CRC校验码(16位)
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int CrcCount(unsigned int *p, unsigned int Length)
{
    unsigned char AH = 0xFF, AL = 0xFF;
    unsigned char GH = 0xA0, GL = 0x01;
    unsigned char i, j;
    for(i = 0;i < Length;i++, p++)
    {
        AL = (*p) ^ AL;
        for(j = 0;j < 8;j++)
        {
            if((AL & 0x01) == 1)    // AL最低位为1
            {
                AL = AL >> 1;
                if(AH & 0x01 == 1)
                    AL = AL | 0x80;
                AH = AH >> 1;
                AH = AH ^ GH;
                AL = AL ^ GL;
            }
            else
            {
                AL = AL >> 1;
                if((AH & 0x01) == 1)
                    AL = AL | 0x80;
                AH = AH >> 1;
            }
        }
    }
    return ((unsigned int)(AL * 0x100 + AH));       //低位在前，高位在后

}
//==============================================================================
//  函数名称   : AddChar
//  功能描述   : 根据传递进来的数组指针p，计算长度为length的加和校验码
//  输入参数   : p－需校验数据的头指针
//           length－需校验数据的长度
//  输出参数   :<无>
//  返回值     : AddValue:校验后得到的加和校验码(取8位存储)
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned char AddChar(unsigned char *p, unsigned int length)
{
    unsigned int AddValue = 0;
    unsigned int i;
    for(i = 0; i < length; i++, p++)
    {
        AddValue += *p;
    }
    AddValue+= TYPE_EQU;
    return (unsigned char)AddValue;    
}

unsigned int AddInt(unsigned int *p, unsigned int length)
{
    unsigned int AddValue = 0;
    unsigned int i;
    for(i = 0; i < length; i++, p++)
    {
        AddValue += *p;
    }
    AddValue+= TYPE_EQU;
    return AddValue;    
}

//==============================================================================
//  函数名称   : InitCommProgram
//  功能描述   : 通信部分数据初始化，包括标志位与初始数据
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void InitCommProgram(void)
{
    unsigned int i,j;
    for(i=0;i< COMM_PORT_NUM;i++)
    {
        g_sRxBuff[i].m_iBufH = 0;
        g_sRxBuff[i].m_iBufT = 0;
        g_sRxBuff[i].m_iRcvCount = 0;
        g_sRxBuff[i].m_iRcvFlag = OFF;
        g_sTxBuff[i].m_ucTx_Head = 0;
        g_sTxBuff[i].m_ucTx_Tail = 0;
        g_sTxBuff[i].m_unSendFlag = 0;
        for(j=0;j < MAX_RXBUFF_LEN ; j++ )
        {
            g_sRxBuff[i].m_gBuf[j] = 0;
            g_sTxBuff[i].m_gBuffer[j] = 0;
        }
     }

     g_RstartNum = 0;
}
int CommRcvData(unsigned char *pBuf,int MaxLen,int uartId)
{
  int rtDaLen = 0;
  unsigned int BfH = g_sRxBuff[uartId].m_iBufH;
  unsigned int BfT = g_sRxBuff[uartId].m_iBufT;
  if(uartId >= COMM_PORT_NUM)//端口号错误
    return 0;
  if( g_sRxBuff[uartId].m_iRcvFlag == ON)
  {
    if(BfH <= BfT) 
      rtDaLen = BfT - BfH;
    else
      rtDaLen = MAX_RXBUFF_LEN - BfH + BfT; 
    if(rtDaLen == 0) return 0;
    if(rtDaLen >= MaxLen) 
      rtDaLen = MaxLen;
    
    if(rtDaLen < (MAX_RXBUFF_LEN - BfH)) 
    {
      memcpy(pBuf,&(g_sRxBuff[uartId].m_gBuf[g_sRxBuff[uartId].m_iBufH]),rtDaLen);
      g_sRxBuff[uartId].m_iBufH += rtDaLen;
    }  
    else
    {
      memcpy(pBuf,&(g_sRxBuff[uartId].m_gBuf[g_sRxBuff[uartId].m_iBufH]),MAX_RXBUFF_LEN-BfH);
      memcpy(pBuf+(MAX_RXBUFF_LEN-BfH),&(g_sRxBuff[uartId].m_gBuf[0]),rtDaLen - (MAX_RXBUFF_LEN - BfH));
      g_sRxBuff[uartId].m_iBufH = rtDaLen - (MAX_RXBUFF_LEN - BfH);
    }
    if(g_sRxBuff[uartId].m_iBufH == g_sRxBuff[uartId].m_iBufT)
    {
      g_sRxBuff[uartId].m_iBufT = 0;
      g_sRxBuff[uartId].m_iBufH = 0;
    }
    g_sRxBuff[uartId].m_iBufH &= (MAX_RXBUFF_LEN - 1);
    g_sRxBuff[uartId].m_iRcvFlag = OFF;
  }
  return rtDaLen;
}

int CommSendData(unsigned char *pBuf,int BufLen,int uartId)
{
    if(uartId >= COMM_PORT_NUM)//不允许发送到无效串口 
      return 0;

  //  if((uartId == g_CmIdGPRS) && !g_GprsPowerSt)//for test 模拟GPRS关闭状态，关闭时子站应该不发数据
       // return 0;
              
    //若是当前串口正在发数据，等待其发完再将数据放入发送链表
   if(g_sTxBuff[uartId].m_unSendFlag)
   {
      while(1)
      {
        if(!g_sTxBuff[uartId].m_unSendFlag)//若当前未启动数据发送或数据发送完成
          break;
      }
    }
    for(int i = 0;i<BufLen;i++)
    {
            g_sTxBuff[uartId].m_gBuffer[g_sTxBuff[uartId].m_ucTx_Tail] = pBuf[i];
            g_sTxBuff[uartId].m_ucTx_Tail++;
            g_sTxBuff[uartId].m_ucTx_Tail &= (MAX_RXBUFF_LEN - 1);
           if(g_sTxBuff[uartId].m_ucTx_Tail == g_sTxBuff[uartId].m_ucTx_Head) 
             g_sTxBuff[uartId].m_ucTx_Head++;//若尾指针追上头指针，则头指针向后推移
     }
     if(g_sTxBuff[uartId].m_unSendFlag == 0)
     {
       g_sTxBuff[uartId].m_unSendFlag = 2;
     
     /*  if(uartId == CMID0)
          UCA0TXBUF = g_sTxBuff[uartId].m_gBuffer[g_sTxBuff[uartId].m_ucTx_Head];
       else */
       if(uartId == 1) 
          UCA1TXBUF = g_sTxBuff[uartId].m_gBuffer[g_sTxBuff[uartId].m_ucTx_Head];
       else if(uartId == 2) 
       {
          	UCA2TXBUF = g_sTxBuff[uartId].m_gBuffer[g_sTxBuff[uartId].m_ucTx_Head];
		g_ucGPRSState_TCounter = 0;					//张弢0330 	
		g_ucGPRSState = GPRSState_GPRS101;			//张弢0330 串口在发送gprs101状态		
       }
      /* else if(uartId == CMID3)
          UCA3TXBUF = g_sTxBuff[uartId].m_gBuffer[g_sTxBuff[uartId].m_ucTx_Head];*/
       
       g_sTxBuff[uartId].m_ucTx_Head++;
       g_sTxBuff[uartId].m_ucTx_Head &= (MAX_RXBUFF_LEN - 1);
     }  
     

  return 1;
}


//根据遥测序号得到设备通道状态的遥信序号 
unsigned int YcIdToYxId(unsigned int RYxNo)
{
return TRUE;

}



unsigned short Fcs16(unsigned short fcs, unsigned char *cp, unsigned short len)
{
    while (len--)
    {
        fcs = (fcs >> 8) ^ fcsTab[(fcs ^ *cp++) & 0xff];
    }
    return (fcs);
}

unsigned short ByAddCrc16(unsigned char *cp, unsigned short len)
{
    unsigned short trialfcs;

    /* add on output */
    trialfcs = Fcs16( 0xFFFF, cp, len );
    trialfcs ^= 0xffff;                 /* complement */
    cp[len] = (trialfcs & 0x00ff);      /* least significant byte first */
    cp[len+1] = ((trialfcs >> 8) & 0x00ff);
    return 0;
}
void MyPrintf(char *ch)
{
#if 0
  WORD wLen = strlen(ch);
  CommSendData((unsigned char *)ch,wLen,g_CmIdDBG);
#endif
}
void MyPrintfD(BYTE *p,WORD wLen)
{
 // if(g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_PRINTDUG])
  //  CommSendData((unsigned char *)p,wLen,g_CmIdDBG);
}







