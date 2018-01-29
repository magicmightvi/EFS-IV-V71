/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  模块名         : SMS短信收发、上报功能模块
  文件名         : PrtcSms.c
  相关文件       : DataStruct.h FuncDef.h Constant.h DriverComm.c Comm.c
  实现功能       : 发生需要短信上报的事件时组织发送短信内容，存放到发送缓冲区中；进行短消息的收发执行
  作者           : 
  版本           : 1.0
  
  --------------------------------------------------------------------------------
  备注           : -
  --------------------------------------------------------------------------------
  
  修改记录       : 
    日  期          版本            修改人          修改内容 
    2012/02/10      1.0             孙常林            创建

************************************************************************************/

#ifndef _PRTC_SMS

#define _PRTC_SMS
  #include "PrtcSms.h"
#include "..\Main.h"
//**********************************************************************************
// **                                   CRC
// ** //张弢 0324
//**********************************************************************************

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




//Calculate a new fcs given the current fcs and the new data.

unsigned short pppfcs16(unsigned short fcs, unsigned char *cp, unsigned short len)
{
       while (len--){
           fcs = (fcs >> 8) ^ fcsTab[(fcs ^ *cp++) & 0xff];
       }

       return (fcs);
}
unsigned char byAddCrc16(unsigned char *cp, unsigned short len)
{
	unsigned short trialfcs;

	//add on output 
	trialfcs = pppfcs16( 0xFFFF, cp, len );
	trialfcs ^= 0xffff;                 // complement 
	cp[len] = (trialfcs & 0x00ff);      //least significant byte first 
	cp[len+1] = ((trialfcs >> 8) & 0x00ff);

	return 0;
}

  /***************************************************************
      Function：CPrtcSms
          构造函数，暂空
      参数：无
  
      返回：无
  ***************************************************************/
   CPrtcSms::CPrtcSms() : CPSecondary()
  {
  
  }

//******************************************************
//张弢 0404 短信接收
//******************************************************

//BOOL CProtocol::RecFrameAA(void)
BOOL CPrtcSms::RecFrameAA(VFrameAA * pReceiveFrame)
{
  //pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;

    switch(pReceiveFrame->Type)
    {
     /* 
      case 0x07:      //读GPRS时间
          if(pReceiveFrame->byContent[0] == 0x1)
          {
              //WORD MSecond;
              //MSecond = MAKEWORD(pData[0], pData[1]);
              unsigned int unHYM[7];
              unHYM[0] = ((pReceiveFrame->byContent[1]) & 0x7F);//YEAR
              unHYM[1] = (pReceiveFrame->byContent[2]) & 0x0F;//MONTH
              unHYM[2] = (pReceiveFrame->byContent[3]) & 0x1F;//DAY
              unHYM[3] =g_sRtcManager.m_gRealTimer[RTC_WEEK];//SNTP无第几周的信息，将RTC芯片的第几周更新
              unHYM[4] = pReceiveFrame->byContent[4];  //HOUR
              unHYM[5] = pReceiveFrame->byContent[5];//MINUTE
              unHYM[6] = pReceiveFrame->byContent[6];//SEC
              WriteRealTime(unHYM);  //修改时间
              ReadRealTime(0);//将实时芯片中的时间更新到
              BYTE bBuf[2] = {0xFC,0XFC};
              MyPrintfD(bBuf,2);
          }
          else
          {
              BYTE bBuf[2] = {0xFB,0XFB};
              MyPrintfD(bBuf,2);
          }            
          break;
    */
      case 0x15:
      case 0x16:      //读GPRS配置
          RecIHDPara(pReceiveFrame);
          break;		  
      case 0x18:
          RecIHDWConf(pReceiveFrame);
	   break;	  
        case 0x17://发送短信成功
            RecSendSMSOK();
            break;
        case 0x13://接收短信
            RecSMS();
            break;
        default:
            break;
    }
    return TRUE;
}

  /***************************************************************
      Function：SendRCmdToIHD
          给映瀚通模块发读配置命令
      参数：
      返回：TRUE 成功，FALSE 失败
      日期：2015年8月20 BY zx
  ***************************************************************/  
  BOOL CPrtcSms::SendRCmdToIHD(unsigned char Cmd,unsigned int Val,CProtocol *pDaSrc)
  {
      BYTE tempbuff[32];
      //BYTE  chDaSrcCm;
      if(pDaSrc != null ) m_RWPaSrcObj = pDaSrc;
      //chDaSrcCm = pDaSrc->m_uartId;
      //if(chDaSrcCm == g_CmIdGPRS)
      //{
      //    g_IHDCmdRBit |= (DWORD)BIT[Cmd];
      //}
      //else if(chDaSrcCm == g_CmIdDBG)
      //{
      //    g_IHDCmdRBit |= ((DWORD)BIT[Cmd] << 16);
      //}
      switch(Cmd)
      {
      case 0:       //读IP地址和端口
          SendSmsFrameHead(READ_SPE_CONFIG);
          tempbuff[0] = 0;
          tempbuff[1] = 0x19;
          tempbuff[2] = 0;
          tempbuff[3] = 0;
          TAGContent(tempbuff,4,INHAND_CHAXUN);
          break;
      case 1://读短信功能开启状态
          SendSmsFrameHead(READ_SPE_CONFIG);
          tempbuff[0] = 0x81;
          tempbuff[1] = 0x87;
          tempbuff[2] = 0;
          tempbuff[3] = 0;
          TAGContent(tempbuff,4,INHAND_CHAXUN);
          break;
      case 2:  //APN
      case 3:  //用户名
      case 4:  //密码
          SendSmsFrameHead(READ_SPE_CONFIG);
          tempbuff[0] = 0;
          tempbuff[1] = Cmd;
          tempbuff[2] = 0;
          tempbuff[3] = 0;
          TAGContent(tempbuff,4,INHAND_CHAXUN);
          break;
      case 5:       //读备用IP地址和端口
          SendSmsFrameHead(READ_SPE_CONFIG);
          tempbuff[0] = 0x80;
          tempbuff[1] = 0x0D;
          tempbuff[2] = 0;
          tempbuff[3] = 0;
          TAGContent(tempbuff,4,INHAND_CHAXUN);
          break;		  
      case 15://
          SendSmsFrameHead(READ_STATUS);
          TAGContent(tempbuff,0,INHAND_CHAXUN);
          break;
      case 84://读信号值 (CSQ)
          SendSmsFrameHead(READ_SPE_STATE);
          tempbuff[0] = 0x81;
          tempbuff[1] = 0x84;
          tempbuff[2] = 0;
          tempbuff[3] = 0;			  
          TAGContent(tempbuff,4,INHAND_CHAXUN);
          break;		  
      default:
          return false;
      }
      SendSmsFrame();
      //OpenTimer(TM_GPRS_RW);
      return true;
  }
  
  
   /***************************************************************
      Function：SendWCmdToIHD
          给映瀚通模块发写配置命令
      参数：
      返回：TRUE 成功，FALSE 失败
      日期：2015年8月20 BY zx
  ***************************************************************/  
  BOOL CPrtcSms::SendWCmdToIHD(unsigned int Val,unsigned char Cmd,BYTE *pDa,CProtocol *pDaSrc)
  {
      BYTE tempbuff[32];
      //BYTE  chDaSrcCm;
      BYTE dataLen;
      //chDaSrcCm = pDaSrc->m_uartId;
      if(pDaSrc != null ) m_RWPaSrcObj = pDaSrc;
      SendSmsFrameHead(WRITE_SPE_CONFIG);
      if(Val)
          dataLen = Val;
      else
          dataLen = pDa[0];
      if(dataLen <= 0)
          return false;
//        if(chDaSrcCm == g_CmIdGPRS)
//        {
//            g_IHDCmdWBit |= (DWORD)BIT[Cmd];
//        }
//        else if(chDaSrcCm == g_CmIdDBG)
//        {
//            g_IHDCmdWBit |= ((DWORD)BIT[Cmd] << 16);
//        }
      switch(Cmd)
      {
      case 0:       //写IP地址和端口
          
          tempbuff[0] = 0;
          tempbuff[1] = 0x19;
          if (dataLen > 6)
              return false;
          break;
      case 1://开/关短信功能
          tempbuff[0] = 0x81;
          tempbuff[1] = 0x87;
   
          break;
      case 2:  //APN
      case 3:  //用户名
      case 4:  //密码
          if(dataLen > 20)
              return false;
          tempbuff[0] = 0;
          tempbuff[1] = Cmd;
          break;
	 case 5:       //写备用IP地址和端口
          tempbuff[0] = 0x80;
          tempbuff[1] = 0x0d;
          if (dataLen > 6)
              return false;
          break;	  
	 case 0x12:       //重启GPRS模块
	   SendSmsFrameHead(RESET);
          SendSmsFrame();
          delayms(3000);
          main_reset_flag = 0x55;
	  return TRUE;	  
          break;          
      default:
          return false;
      }
      tempbuff[2] = 0;
      tempbuff[3] = dataLen;      
      for(char i = 0; i < dataLen; i++)
      {
          tempbuff[4 + i] = pDa[1 + i];
      }
      TAGContent(tempbuff,dataLen + 4,INHAND_CHAXUN);
      SendSmsFrame();
      //OpenTimer(TM_GPRS_RW);
      return true;
  }
     /***************************************************************
      Function：RecIHDPara
          接收映瀚通发送的配置参数確認
      参数：
      返回：TRUE 成功，FALSE 失败
      日期：2015年8月20 BY zx
  ***************************************************************/  
  BOOL CPrtcSms::RecIHDWConf(VFrameAA *pReceiveFrame)
  {
      WORD wTag = MAKEWORD(pReceiveFrame->byContent[1],pReceiveFrame->byContent[0]);//高位在前
     // BYTE secName = 0;
      switch(wTag)
      {
      case 0x0002:   //读APN
        //  secName = 2;
          break;
      case 0x0003:   //读用户名
         // secName = 3;
          break;
      case 0x0004:   //读密码
          //secName = 4;
          break;
      case 0x0019:   //读IP及端口
         
          if(m_RWPaSrcObj ->g_gGprsSETip== 0x55)return true;
          break;
      case 0x800D:
	   if(m_RWPaSrcObj ->g_gGprsSETip== 0x55)
	   	{
	   	m_RWPaSrcObj ->g_gGprsSETip=0;
		return true;
		}	
          break;
      case 0x8187:
         // secName = 1;        
          break;
      default:
          return false;
      }
      if(m_RWPaSrcObj !=null)
         m_RWPaSrcObj->SendWrPaSuc(); 
      
      return true;     
  }
    /***************************************************************
      Function：RecIHDPara
          接收映瀚通发送的配置参数
      参数：
      返回：TRUE 成功，FALSE 失败
      日期：2015年8月20 BY zx
  ***************************************************************/  
  BOOL CPrtcSms::RecIHDPara(VFrameAA *pReceiveFrame)
  {
      BYTE secName,i;
      WORD wTag = MAKEWORD(pReceiveFrame->byContent[1],pReceiveFrame->byContent[0]);//高位在前
      //WORD temp;
/*
#ifdef YN_101S      
      if((secName == 0x0f)&&(m_RWPaSrcObj ->g_gGprsCXVSQ == 0x55))
      {
        m_RWPaSrcObj ->g_gGprsCXVSQ = 0;
        if(pGprs == null) return false;
        g_unZJTrans[2]=(((CBJ101S *)pGprs)->pReceiveFrame)->Frame68.Data[9];  //CSQ值
        return true;
      }
#endif	
*/
      switch(wTag)
      {
      case 0x0002:   //读APN
          secName = 2;
		if(pGprs == null) break;   
		g_gAPN[0]=(((CBJ101S *)pGprs)->pReceiveFrame)->FrameAA.byContent[3];
              for(i = 0;i <g_gAPN[0];i++)
              {                  
		  g_gAPN[i+1]=(((CBJ101S *)pGprs)->pReceiveFrame)->FrameAA.byContent[i + 4];	                
              }	   	  
          break;
      case 0x0003:   //读用户名
          secName = 3;
          break;
      case 0x0004:   //读密码
          secName = 4;
          break;
      case 0x0019:   //读IP及端口
          secName = 0;
         {
		if(pGprs == null) break;            	
              for(i = 0;i <6;i++)
              {                  
		  g_gIPPort[i+1]=(((CBJ101S *)pGprs)->pReceiveFrame)->FrameAA.byContent[i + 4];	                
              }		
              return true;
           }
          break;
      case 0x800D://备用ip
           secName = 0;
	   if(pGprs == null) break;            	
          for(i = 0;i <6;i++) 
              {                  
		  g_gIPPort[i+7]=(((CBJ101S *)pGprs)->pReceiveFrame)->FrameAA.byContent[i + 4];		  
              }	   
	   if(m_RWPaSrcObj ->g_gGprsCXip == 0x55)//张弢云南 由主站68指令查询ip地址、端口信号强度
         {
              m_RWPaSrcObj ->g_gGprsCXip = 0;       
			  return true; 
/*
              if(m_RWPaSrcObj == null) return false;
    		m_RWPaSrcObj -> SendFrameHead(102,7);
		m_RWPaSrcObj ->write_infoadd(0x6001);
		m_RWPaSrcObj -> m_SendBuf.pBuf[m_RWPaSrcObj ->m_SendBuf.wWritePtr++] = g_gIPPort[1];
		m_RWPaSrcObj -> m_SendBuf.pBuf[m_RWPaSrcObj ->m_SendBuf.wWritePtr++] = g_gIPPort[2];
		m_RWPaSrcObj -> m_SendBuf.pBuf[m_RWPaSrcObj ->m_SendBuf.wWritePtr++] = g_gIPPort[3];
		m_RWPaSrcObj -> m_SendBuf.pBuf[m_RWPaSrcObj ->m_SendBuf.wWritePtr++] = g_gIPPort[4];
		m_RWPaSrcObj -> m_SendBuf.pBuf[m_RWPaSrcObj ->m_SendBuf.wWritePtr++] = g_gIPPort[6];
		m_RWPaSrcObj -> m_SendBuf.pBuf[m_RWPaSrcObj ->m_SendBuf.wWritePtr++] = g_gIPPort[5];
		m_RWPaSrcObj -> m_SendBuf.pBuf[m_RWPaSrcObj ->m_SendBuf.wWritePtr++] = g_gIPPort[7];
		m_RWPaSrcObj -> m_SendBuf.pBuf[m_RWPaSrcObj ->m_SendBuf.wWritePtr++] = g_gIPPort[8];
		m_RWPaSrcObj -> m_SendBuf.pBuf[m_RWPaSrcObj ->m_SendBuf.wWritePtr++] = g_gIPPort[9];
		m_RWPaSrcObj -> m_SendBuf.pBuf[m_RWPaSrcObj ->m_SendBuf.wWritePtr++] = g_gIPPort[10];
		m_RWPaSrcObj -> m_SendBuf.pBuf[m_RWPaSrcObj ->m_SendBuf.wWritePtr++] = g_gIPPort[12];
		m_RWPaSrcObj -> m_SendBuf.pBuf[m_RWPaSrcObj ->m_SendBuf.wWritePtr++] = g_gIPPort[11];	      		
		m_RWPaSrcObj ->SendFrameTail(0, 0xa5, 1);			
		
              return true;*/
           }     
          break;
      case 0x8187://收短信到本地串口
          secName = 1;        
		 g_gSMSR=(((CBJ101S *)pGprs)->pReceiveFrame)->FrameAA.byContent[4];
          break;
      case 0x8184://读信号值 (CSQ)
               secName = 1;        
		 g_unZJTrans[2]=(((CBJ101S *)pGprs)->pReceiveFrame)->FrameAA.byContent[4];
		 g_gRmtMeas[RM_CSQ] = g_unZJTrans[2];
		 return true;
          break;	  
      default:
          return false;
      }
    if(m_RWPaSrcObj != null)
          m_RWPaSrcObj->SendReadPa(11,secName);
      return true;
  }
	
  BOOL CPrtcSms::SendSmsFrame(void)
  {
      g_SmsSendBuf[6] = g_SmsWritePtr - 7;
      //byAddCrc16(unsigned char * cp, unsigned short len)(g_SmsSendBuf,g_SmsWritePtr);
      byAddCrc16(g_SmsSendBuf,g_SmsWritePtr);
      g_SmsWritePtr += 2;
      CommSendData((BYTE*)g_SmsSendBuf,g_SmsWritePtr,g_CmIdGPRS);
      return true;
  }
  BOOL CPrtcSms::SendSmsFrameHead(BYTE Command)
  {
  
      g_SmsWritePtr=0;
      g_SmsSendBuf[g_SmsWritePtr++] = 0x55; //包头
      g_SmsSendBuf[g_SmsWritePtr++] = 0xAA;
      g_SmsSendBuf[g_SmsWritePtr++] = 0x55;
      g_SmsSendBuf[g_SmsWritePtr++] = 0xAA;
      g_SmsSendBuf[g_SmsWritePtr++] = Command; //控制字
      g_SmsSendBuf[g_SmsWritePtr++] = 0;
      g_SmsWritePtr++;                 //预留帧长度
      return true;
  }  

  BOOL CPrtcSms::TAGContent(BYTE *ptr,BYTE num,BYTE type)
  {
      BYTE temp;
      //char ch[60];  //用于发送短信用的缓存
     
      if(type == INHAND_DUANXIN)
      {
          g_SmsSendBuf[g_SmsWritePtr++] = 0x81;
          g_SmsSendBuf[g_SmsWritePtr++] = 0x82;
          g_SmsSendBuf[g_SmsWritePtr++] = 0x00;
          g_SmsSendBuf[g_SmsWritePtr++] = 0x00; //数据长度
          BYTE LenthPtr = g_SmsWritePtr;
          for(temp = 0; temp < num; temp++)
          {
              g_SmsSendBuf[g_SmsWritePtr++] = ptr[temp];
          }
          g_SmsSendBuf[LenthPtr - 1] = g_SmsWritePtr - LenthPtr;
      }
      else if(type == INHAND_CHAXUN)
      {
          for(temp = 0; temp < num; temp++)
          {
              g_SmsSendBuf[g_SmsWritePtr++] = ptr[temp];
          }
      }
      return true;
  }
  BOOL CPrtcSms::TLVSContent(BYTE *ptr,unsigned int  num,unsigned int  type)
  {
      BYTE temp;
     	   g_SmsSendBuf[g_SmsWritePtr++]  = (BYTE)(type>>8);
	  g_SmsSendBuf[g_SmsWritePtr++]  = (BYTE)(type);
	  g_SmsSendBuf[g_SmsWritePtr++]  = (BYTE)(num>>8);
	  g_SmsSendBuf[g_SmsWritePtr++]  = (BYTE)(num);
          for(temp = 0; temp < num; temp++)
          {
              g_SmsSendBuf[g_SmsWritePtr++] = ptr[temp];
          } 
      return true;
  }
#endif 
/*
const unsigned char g_gSMSInfo[4][14]={
      
     {0xDD,0x4F,0xA4,0x62,0x00,0x5F,0x73,0x51,0xF3,0x8D,0x00,0x5F},    ////保护开关跳开 0
     {0xD1,0x53,0xFA,0x51,0x09,0x67,0x48,0x65,0x38,0x00,0x09,0x81,0xB2,0x51},    ////发出有效8脉冲1
     {0xA5,0x63,0x30,0x57,0x45,0x65,0x9C,0x96,0xD1,0x53,0x1F,0x75},    ////接地故障发生2
     {0xA5,0x63,0x30,0x57,0x45,0x65,0x9C,0x96,0x0D,0x59,0x52,0x5F},    ////接地故障复归3

    };
    */
 const  unsigned char g_gSMSInfo[4][14]={
      
     {0x4F,0xDD,0x62,0xA4,0x5F,0x0,0x51,0x73,0x8D,0xF3,0x5F,0x00},    ////保护开关跳开 0
     {0x53,0xD1,0x51,0xFA,0x67,0x9,0x65,0x48,0x00,0x38,0x81,0x9,0x51,0xB2},    ////发出有效8脉冲1
     {0x63,0xA5,0x57,0x30,0x65,0x45,0x96,0x9C,0x53,0xD1,0x75,0x1F},    ////接地故障发生2
     {0x63,0xA5,0x57,0x30,0x65,0x45,0x96,0x9C,0x59,0x0D,0x5F,0x52},    ////接地故障复归3

    };
    

unsigned char SMS_SendFlg; 


//**********************************************************************************
// **                                   CRC
// ** //张弢 0324
//**********************************************************************************

//==============================================================================
//  函数名称   : Comm_GPRS_SMS
//  功能描述   : 短消息模式GPRS数据传输
//  输入参数   : <无> 
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       : 
//==============================================================================
void Comm_GPRS_SMS(void)
{
    //unsigned char i;//,j  
    //unsigned char ucSMSType;
    
    if(g_ucGPRSState!=GPRSState_IDLE)
		return;////张弢0330 如串口在空闲状态，则可以发送SMS	
   if(g_unFrameFlag_Sms == g_unFrameFlag_curSms)//短信已发完 不再继续发
      		return;
   if(g_unSms_s > 0)//未收到 "短信已发送"的回复,不发下一条
   		return;
    else
    {
	Send_GPRS_SMS(g_unSMS_Index[g_unFrameFlag_curSms],g_unPhoneNo);	
	if(g_unSms_s == 0)//短信发出后，g_unSms_s=1，
	{                              //由于类型或者号码原因，未发短信，则g_unSms_s仍为0，继续发下一条
	   g_unPhoneNo++;
	   g_unFailSms = 0;
	   if(g_unPhoneNo>=4)// 4个号码发完 发下一条
		{
		g_unPhoneNo=0;
		if(g_unFrameFlag_Sms != g_unFrameFlag_curSms)
       		g_unFrameFlag_curSms++;
		if(g_unFrameFlag_curSms>=MAX_SMS_NUM)
			g_unFrameFlag_curSms=0;
		}
	}
    }
    
}

//==============================================================================
//  函数名称   : PhoneNumJudge
//  功能描述   : 进行电话号码的提取与判断
//  输入参数   : 电话号码特征序列数
//  输出参数   : <无>
//  返回值     : 标志数   right:0x33    wrong:0x55 
//  其他说明   : 用于精简程序代码++
//  作者       : 孙常林
//==============================================================================
  
unsigned char PhoneNumJudge(unsigned char SMS_Sequence)
{ 
    unsigned char i;

 //   if(SMS_SendFlg == 0)	//张弢 0404 短信接收

        for(i=1;i<11;i++)                         //////判断手机号码是否正确
        {
            //if(TEL_NUM[SMS_Sequence * 11 + i] > 0x39 || TEL_NUM[SMS_Sequence * 11 + i] < 0x30 ||TEL_NUM[SMS_Sequence * 11] != 0x31)   //////手机号不正确
            if(g_gSmsPhone[SMS_Sequence * 12 + i+1] > 0x39 || g_gSmsPhone[SMS_Sequence * 12 + i+1] < 0x30 ||g_gSmsPhone[SMS_Sequence * 12+1] != 0x31)   //////手机号不正确            
            {            
                //SMS_delay=1;
                return(0x55);
            }
            else
            {
                //g_unGPRSTX_Buff[11 + i] = TEL_NUM[SMS_Sequence * 11 + i];
                g_unGPRSTX_Buff[11 + i] = g_gSmsPhone[SMS_Sequence * 12 + i+1];
            }
        } 
        //g_unGPRSTX_Buff[11] = TEL_NUM[ SMS_Sequence * 11];
       g_unGPRSTX_Buff[11] = g_gSmsPhone[ SMS_Sequence * 12+1];
     
    return(0x33);
}
unsigned char PhoneNumJudge1(unsigned char SMS_Sequence)
{ 
    unsigned char i;
    for(i=1;i<11;i++)                         //////判断手机号码是否正确
    {
      if(ucTempTEL_NUM[i] > 0x39 || ucTempTEL_NUM[i] < 0x30 ||ucTempTEL_NUM[0] < 0x31)   //////手机号不正确
      {            
        return(0x55);
      }
      else
      {
        g_unGPRSTX_Buff[11 + i] = ucTempTEL_NUM[i];
      }
    } 
    g_unGPRSTX_Buff[11] = ucTempTEL_NUM[0];
    return(0x33);
}
//==============================================================================
//  函数名称   : PhoneNumTake
//  功能描述   : 完成电话号码长度判断以及数据组帧
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 用于精简程序代码
//  作者       : 孙常林
//==============================================================================
  
void PhoneNumTake(void)
{ 
    //unsigned char i;
    g_unGPRSTX_Buff[0] = 0x55;      //包头
    g_unGPRSTX_Buff[1] = 0xAA;
    g_unGPRSTX_Buff[2] = 0x55;
    g_unGPRSTX_Buff[3] = 0xAA;
    g_unGPRSTX_Buff[7] = 0x81;     //开始发送手机号码
    g_unGPRSTX_Buff[8] = 0x80;
    g_unGPRSTX_Buff[9] = 0x00;
           /*  if(g_unGPRSTX_Buff[11] == 0x30)  //007 开头的号码
                 g_unGPRSTX_Buff[10]= 0x0D;    //手机号码长度 
             else if(g_unGPRSTX_Buff[11] == 0x2B) //+7 开头的号码
             {
                 g_unGPRSTX_Buff[10]= 0x0D;    //手机号码长度
                 for(i = 23; i > 12; i--)
                 {
                     g_unGPRSTX_Buff[i] = g_unGPRSTX_Buff[i - 1];
                 }
                 g_unGPRSTX_Buff[11]= 0x30;
                 g_unGPRSTX_Buff[12]= 0x30;
             }
             else */                        //国内 1 开头的号码        
    g_unGPRSTX_Buff[10]= 0x0b;    //手机号码长度
    g_unGPRSTX_Buff[11 + g_unGPRSTX_Buff[10]]= 0x81;    //短信格式
    g_unGPRSTX_Buff[12 + g_unGPRSTX_Buff[10]]= 0x81;
    g_unGPRSTX_Buff[13 + g_unGPRSTX_Buff[10]]= 0x00;
    g_unGPRSTX_Buff[14 + g_unGPRSTX_Buff[10]]= 0x01;              
}

//==============================================================================
//  函数名称   : Frame_General
//  功能描述   : 通用的数据帧
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 用于精简程序代码
//  作者       : 孙常林
//==============================================================================
  
void Frame_General(void)
{ 
    unsigned char i;            
    g_unGPRSTX_Buff[24 + g_unGPRSTX_Buff[10]]=0x5E;         //////年
    g_unGPRSTX_Buff[25 + g_unGPRSTX_Buff[10]]=0x74;
    
    g_unGPRSTX_Buff[30 + g_unGPRSTX_Buff[10]]=0x67;         //////月
    g_unGPRSTX_Buff[31 + g_unGPRSTX_Buff[10]]=0x08;
               
    g_unGPRSTX_Buff[36 + g_unGPRSTX_Buff[10]]=0x65;         //////日
    g_unGPRSTX_Buff[37 + g_unGPRSTX_Buff[10]]=0xE5;

    g_unGPRSTX_Buff[42 + g_unGPRSTX_Buff[10]]=0x65;         //////时
    g_unGPRSTX_Buff[43 + g_unGPRSTX_Buff[10]]=0xF6;

    g_unGPRSTX_Buff[48 + g_unGPRSTX_Buff[10]]=0x52;         //////分
    g_unGPRSTX_Buff[49 + g_unGPRSTX_Buff[10]]=0x06;

    g_unGPRSTX_Buff[54 + g_unGPRSTX_Buff[10]]=0x79;         //////秒
    g_unGPRSTX_Buff[55 + g_unGPRSTX_Buff[10]]=0xD2;

    for(i=0;i<20;i++)
    {
        g_unGPRSTX_Buff[56 + g_unGPRSTX_Buff[10] + 2 * i ] = (unsigned char)(g_gCnName[i]);  
        g_unGPRSTX_Buff[56 + g_unGPRSTX_Buff[10] + 2 * i+ 1] = (unsigned char)(g_gCnName[i]>> 8);//2014-2-14 孙常林更改
    }                    
    g_unGPRSTX_Buff[96 + g_unGPRSTX_Buff[10]]=0x4F;      ///////信号源
    g_unGPRSTX_Buff[97 + g_unGPRSTX_Buff[10]]=0xE1;
    g_unGPRSTX_Buff[98 + g_unGPRSTX_Buff[10]]=0x53;
    g_unGPRSTX_Buff[99 + g_unGPRSTX_Buff[10]]=0xF7;
    g_unGPRSTX_Buff[100 + g_unGPRSTX_Buff[10]]=0x6E;
    g_unGPRSTX_Buff[101 + g_unGPRSTX_Buff[10]]=0x90;
             
             
}
//==============================================================================
//  函数名称   : SMS_CENTER
//  功能描述   : 中心站短信发送
//  输入参数   : ucSMSType   BIT6       BIT5      BIT4       BIT3      
//                      PHASE_BREAK  FAULT_OCCUR  FAULT_RST  SEND_AUTO
//                           BIT2     BIT1      BIT0
//                      EIGHT_PULSE  POWER_OFF  ABN_CHECK
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       : 
//==============================================================================
void SMS_CENTER(unsigned char ucSMSType)
{
    unsigned char i,sum;
    unsigned char ucLength;
    unsigned int temp;	
    
    if((ucSMSType == PHONE_XUN)||(ucSMSType == ZIZHAN_XUN))//张弢 0404 短信接收
    {   
      sum = PhoneNumJudge1(ucSMSType);
    }
    else
    {
        sum = PhoneNumJudge(0);
    }
    if(sum != 0x33)      //电话号码不正确，不需要发送短信
      {   
       return;

    	}
    PhoneNumTake();
    g_unGPRSTX_Buff[4] = 0x13;     //控制字
    
    g_unGPRSTX_Buff[15 + g_unGPRSTX_Buff[10]]= 0x00;   //定义短信格式数值   //00为英文 //01为中文	
    g_unGPRSTX_Buff[16 + g_unGPRSTX_Buff[10]]= 0x81;   //短信内容
    g_unGPRSTX_Buff[17 + g_unGPRSTX_Buff[10]]= 0x82;
    g_unGPRSTX_Buff[18 + g_unGPRSTX_Buff[10]]= 0x00;
    
    g_unGPRSTX_Buff[25 + g_unGPRSTX_Buff[10]]= (unsigned char)(g_gRunPara[RP_COMM_ADDR]);  //////链路地址
    g_unGPRSTX_Buff[26 + g_unGPRSTX_Buff[10]]= (unsigned char)(g_gRunPara[RP_COMM_ADDR] >> 8);   //////链路地址

    if(ucSMSType == ABN_CHECK)
    {   
        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 0x0F;  
             
        g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10]]=0x6C;
        g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10]]=0x09;
        g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10]]=0x09;
        g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10]]=0x6C;
        g_unGPRSTX_Buff[24 + g_unGPRSTX_Buff[10]]=0x57;
         
        for(i=0;i<6;i++)
            g_unGPRSTX_Buff[27 + g_unGPRSTX_Buff[10]+i]= g_sSoeData.m_gAbnBuff[i];
        g_unGPRSTX_Buff[33 + g_unGPRSTX_Buff[10]]=0;
        for(i=(24 + g_unGPRSTX_Buff[10]);i<(33 + g_unGPRSTX_Buff[10]);i++)
            g_unGPRSTX_Buff[33 + g_unGPRSTX_Buff[10]]+=g_unGPRSTX_Buff[i];
        g_unGPRSTX_Buff[34 + g_unGPRSTX_Buff[10]]=0x16; 
        g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
        g_unGPRSTX_Buff[6] = 0x1C + g_unGPRSTX_Buff[10];     //数据总长度低位
             
             ucLength = 36 + g_unGPRSTX_Buff[10];
    }
    else if(ucSMSType == POWER_OFF)
    {
      	
             g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 0x0F;  
             
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10]]=0x6C;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10]]=0x09;
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10]]=0x09;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10]]=0x6C;
             g_unGPRSTX_Buff[24 + g_unGPRSTX_Buff[10]]=0x58;
             
             for(i=0;i<6;i++)
                 g_unGPRSTX_Buff[27 + g_unGPRSTX_Buff[10]+i] = g_sSoeData.m_gPowerOFFBuff[i]; 
             g_unGPRSTX_Buff[33 + g_unGPRSTX_Buff[10]]=0;
             for(i=(24 + g_unGPRSTX_Buff[10]);i<(33 + g_unGPRSTX_Buff[10]);i++)
                 g_unGPRSTX_Buff[33 + g_unGPRSTX_Buff[10]]+=g_unGPRSTX_Buff[i];
             g_unGPRSTX_Buff[34 + g_unGPRSTX_Buff[10]]=0x16;
             g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
             g_unGPRSTX_Buff[6] = 0x1C + g_unGPRSTX_Buff[10];     //数据总长度低位

             ucLength = 36 + g_unGPRSTX_Buff[10];
    }
    else if(ucSMSType == EIGHT_PULSE)
    {

             g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 0x21;  
             
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10]]=0x6C;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10]]=0x1B;
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10]]=0x1B;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10]]=0x6C;
             g_unGPRSTX_Buff[24 + g_unGPRSTX_Buff[10]]=0x59;
 
             
             for(i=0;i<8;i++)
             {
                 temp= yc[i]*g_gRunPara[RP_CT_TRANS];
                 if(temp<1200)
			temp=temp/10+128;
		   else
			temp =temp/100;				 
                 if((eight_select&0x01)==0x01)                    ////////////第一次发生8脉冲	
                     g_unGPRSTX_Buff[27 + g_unGPRSTX_Buff[10]+i]=temp;//yc[i];                 	
                 else
                     g_unGPRSTX_Buff[27 + g_unGPRSTX_Buff[10]+i]=temp;//yc1[i];                ////////////第二次发生8脉冲	
                 	
             }	
             for(i=0;i<6;i++)
             {
                 if((eight_select&0x01)==0x01)                    ////////////第一次发生8脉冲	
               	     g_unGPRSTX_Buff[35 + g_unGPRSTX_Buff[10]+i]=sign_RSSI_time[i];     ///////eight pulse ok  
               	 else
               	     g_unGPRSTX_Buff[35 + g_unGPRSTX_Buff[10]+i]=sign_RSSI_time1[i];      ////////////第二次发生8脉冲	
             }	
             eight_select&=~BIT0;//0x0e;   
               
             for(i=0;i<10;i++)
                 g_unGPRSTX_Buff[41 + g_unGPRSTX_Buff[10]+i]= g_sSoeData.m_gSoeBuff[i];  
             g_unGPRSTX_Buff[51 + g_unGPRSTX_Buff[10]]=0;
             for(i=(24 + g_unGPRSTX_Buff[10]);i<(51 + g_unGPRSTX_Buff[10]);i++)
                 g_unGPRSTX_Buff[51 + g_unGPRSTX_Buff[10]]+=g_unGPRSTX_Buff[i];
             g_unGPRSTX_Buff[52 + g_unGPRSTX_Buff[10]]=0x16;  
             g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
             g_unGPRSTX_Buff[6] = 0x2E + g_unGPRSTX_Buff[10];     //数据总长度低位

             ucLength = 54 + g_unGPRSTX_Buff[10];
    }
    else if((ucSMSType == SEND_AUTO)||(ucSMSType == PHONE_XUN))
    {
          g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 0x24;  
   
    	  g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10]]=0x6C;
          g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10]]=0x1E;
          g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10]]=0x1E;
          g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10]]=0x6C;
          g_unGPRSTX_Buff[24 + g_unGPRSTX_Buff[10]]=0x53;
                    
         /* for(i=0;i<5;i++)               /////////模拟量
          {
              j=(unsigned char)(g_gRmtMeas[i]);
              g_unGPRSTX_Buff[27 + g_unGPRSTX_Buff[10]+i*2]=j;
              j=(unsigned char)(g_gRmtMeas[i]>>8);
              g_unGPRSTX_Buff[28 + g_unGPRSTX_Buff[10]+i*2]=j;				     
          } */
          g_unGPRSTX_Buff[27 + g_unGPRSTX_Buff[10]]= (unsigned char)(g_gRmtMeas[RM_UA]);  //////链路地址
          g_unGPRSTX_Buff[28 + g_unGPRSTX_Buff[10]]= (unsigned char)(g_gRmtMeas[RM_UA] >> 8);   //////链路地址
          
          g_unGPRSTX_Buff[29 + g_unGPRSTX_Buff[10]]= (unsigned char)(g_gRmtMeas[RM_UB]);  //////链路地址
          g_unGPRSTX_Buff[30 + g_unGPRSTX_Buff[10]]= (unsigned char)(g_gRmtMeas[RM_UB] >> 8);   //////链路地址
          
          g_unGPRSTX_Buff[31 + g_unGPRSTX_Buff[10]]= (unsigned char)(g_gRmtMeas[RM_UC]);  //////链路地址
          g_unGPRSTX_Buff[32 + g_unGPRSTX_Buff[10]]= (unsigned char)(g_gRmtMeas[RM_UC] >> 8);   //////链路地址
          
          g_unGPRSTX_Buff[33 + g_unGPRSTX_Buff[10]]= (unsigned char)(g_gRmtMeas[RM_U0]);  //////链路地址
          g_unGPRSTX_Buff[34 + g_unGPRSTX_Buff[10]]= (unsigned char)(g_gRmtMeas[RM_U0] >> 8);   //////链路地址
          
          g_unGPRSTX_Buff[35 + g_unGPRSTX_Buff[10]]= (unsigned char)(g_gRmtMeas[RM_I0]);  //////链路地址
          g_unGPRSTX_Buff[36 + g_unGPRSTX_Buff[10]]= (unsigned char)(g_gRmtMeas[RM_I0] >> 8);   //////链路地址
          if(fault_save>0)	                        ///////in fault state
          {
              if(g_gProcCnt[PC_EFS_MODEL]>0)               /////////II型或铁路型
                  g_unGPRSTX_Buff[37 + g_unGPRSTX_Buff[10]]=g_ucEarthFlg;
              else// if(zero_flag==0x55) 
           	  g_unGPRSTX_Buff[37 + g_unGPRSTX_Buff[10]]=4;       	       
          } 
          else
              g_unGPRSTX_Buff[37 + g_unGPRSTX_Buff[10]]=0;        	
          	
          if(efslatch_flag>0)                 ////////latch 17 minutes
       	      g_unGPRSTX_Buff[38 + g_unGPRSTX_Buff[10]]=0x55;
          else
       	      g_unGPRSTX_Buff[38 + g_unGPRSTX_Buff[10]]=0x00;	         	       	 
       
          g_unGPRSTX_Buff[39 + g_unGPRSTX_Buff[10]]=0;                  ////////disconnect or connect	 
           
          if((phase_copy&BIT0)==BIT0)         ///////A相断线   	         
    	      g_unGPRSTX_Buff[39 + g_unGPRSTX_Buff[10]]|=0x01;
    	  if((phase_copy&BIT1)==BIT1)         ///////B相断线	
    	      g_unGPRSTX_Buff[39 + g_unGPRSTX_Buff[10]]|=0x02;
    	  if((phase_copy&BIT2)==BIT2)          ///////C相断线
    	      g_unGPRSTX_Buff[39 + g_unGPRSTX_Buff[10]]|=0x04;	 
       
           //g_unGPRSTX_Buff[51]=0;                  ////////self check  right or no
           if(abnormal_counter>5)        ////////self check  right or no
               g_unGPRSTX_Buff[40 + g_unGPRSTX_Buff[10]]=0x55;
           else
               g_unGPRSTX_Buff[40 + g_unGPRSTX_Buff[10]]=0;
       
           if((P2IN&BIT4)==0)                 /////////信号源掉电检测	
               g_unGPRSTX_Buff[41 + g_unGPRSTX_Buff[10]]=0x55;
           else
       	       g_unGPRSTX_Buff[41 + g_unGPRSTX_Buff[10]]=0;	         	 
          	 
          g_unGPRSTX_Buff[42 + g_unGPRSTX_Buff[10]]= g_gProcCnt[PC_EFS_MODEL];       ////////DEVICE TYPE
          for(i=0;i<10;i++) 
              g_unGPRSTX_Buff[43 + g_unGPRSTX_Buff[10]+i]= g_sSoeData.m_gSoeBuff[i];   ////////the last fault message
          
          g_unGPRSTX_Buff[53 + g_unGPRSTX_Buff[10]]=phase_seq_flag;  
            
          g_unGPRSTX_Buff[54 + g_unGPRSTX_Buff[10]]=0;                                                                              
          for(i=(24 + g_unGPRSTX_Buff[10]);i<(54 + g_unGPRSTX_Buff[10]);i++) 
              g_unGPRSTX_Buff[54 + g_unGPRSTX_Buff[10]] += g_unGPRSTX_Buff[i];
          
          g_unGPRSTX_Buff[55 + g_unGPRSTX_Buff[10]]=0x16; 
          
          g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
          g_unGPRSTX_Buff[6] = 0x31 + g_unGPRSTX_Buff[10];     //数据总长度低位
 
          ucLength = 57 + g_unGPRSTX_Buff[10];
    }
    else if(ucSMSType == FAULT_OCCUR)
    {


             g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 0x14;  
             
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10]]=0x6C;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10]]=0x0E;
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10]]=0x0E;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10]]=0x6C;
             g_unGPRSTX_Buff[24 + g_unGPRSTX_Buff[10]]=0x5A;
 
             
             for(i=0;i<10;i++)
                 g_unGPRSTX_Buff[27 + g_unGPRSTX_Buff[10]+i]= g_sSoeData.m_gSoeBuff[i]; 
             g_unGPRSTX_Buff[37 + g_unGPRSTX_Buff[10]]=fault_sms_type;
             fault_sms_type=0;
             g_unGPRSTX_Buff[38 + g_unGPRSTX_Buff[10]]=0;
             for(i=(24 + g_unGPRSTX_Buff[10]);i<(38 + g_unGPRSTX_Buff[10]);i++)
                 g_unGPRSTX_Buff[38 + g_unGPRSTX_Buff[10]]+=g_unGPRSTX_Buff[i];
             g_unGPRSTX_Buff[39 + g_unGPRSTX_Buff[10]]=0x16;  
             g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
             g_unGPRSTX_Buff[6] = 0x21 + g_unGPRSTX_Buff[10];     //数据总长度低位

             ucLength = 41 + g_unGPRSTX_Buff[10];
	
    }
    else if(ucSMSType == FAULT_RST)
    {
       
             g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 0x13;  
             
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10]]=0x6C;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10]]=0x0D;
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10]]=0x0D;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10]]=0x6C;
             g_unGPRSTX_Buff[24 + g_unGPRSTX_Buff[10]]=0x5B;

             
             for(i=0;i<10;i++)
               {g_unGPRSTX_Buff[27 + g_unGPRSTX_Buff[10]+i]= g_sSoeData.m_gSoeRSTBuff[i];} 
             g_unGPRSTX_Buff[37 + g_unGPRSTX_Buff[10]]=0;
             for(i=(24 + g_unGPRSTX_Buff[10]);i<(37 + g_unGPRSTX_Buff[10]);i++)
               {g_unGPRSTX_Buff[37 + g_unGPRSTX_Buff[10]]+=g_unGPRSTX_Buff[i];}
             g_unGPRSTX_Buff[38 + g_unGPRSTX_Buff[10]]=0x16;  
             g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
             g_unGPRSTX_Buff[6] = 0x20 + g_unGPRSTX_Buff[10];     //数据总长度低位

             ucLength = 40 + g_unGPRSTX_Buff[10];
    }
    else if(ucSMSType == PHASE_BREAK) 
    {

             g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 0x12;  
             
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10]]=0x6C;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10]]=0x0C;
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10]]=0x0C;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10]]=0x6C;
             g_unGPRSTX_Buff[24 + g_unGPRSTX_Buff[10]]=0x5C;
             
              for(i=0;i<9;i++)
               {g_unGPRSTX_Buff[27 + g_unGPRSTX_Buff[10]+i] = g_sSoeData.m_gBreakBuff[i];} 
             g_unGPRSTX_Buff[36 + g_unGPRSTX_Buff[10]]=0;
             for(i=(24 + g_unGPRSTX_Buff[10]);i<(36 + g_unGPRSTX_Buff[10]);i++)
               {g_unGPRSTX_Buff[36 + g_unGPRSTX_Buff[10]]+=g_unGPRSTX_Buff[i];}
             g_unGPRSTX_Buff[37 + g_unGPRSTX_Buff[10]]=0x16; 
             g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
             g_unGPRSTX_Buff[6] = 0x1F + g_unGPRSTX_Buff[10];     //数据总长度低位
             
            ucLength = 39 + g_unGPRSTX_Buff[10];

    }
    /**/ else if(ucSMSType == ZIZHAN_XUN)
    {
          g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 0x06;  
             
             
             if(sign_RSSI_delay>0)
          	 {
          	  sign_RSSI_delay=0;
          	  g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10]]=0xA1;
          	 }
              else
          	 {g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10]]=0xA0;}	 
    	  	 g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10]]=0x10;
    	  	 //g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10]]=xhy_addr[0];   //////链路地址//张弢 0404 短信接收
              //g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10]]=xhy_addr[1];  //////链路地址 //张弢 0404 短信接收 lobyte(g_gRunPara[RP_COMM_ADDR] 
		g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10]]=LOBYTE(g_gRunPara[RP_COMM_ADDR]);//张弢 0404 短信接收
		g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10]]=HIBYTE(g_gRunPara[RP_COMM_ADDR]); //张弢 0404 短信接收

		g_unGPRSTX_Buff[24 + g_unGPRSTX_Buff[10]]=g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10]]+g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10]]+g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10]]; 
 
             g_unGPRSTX_Buff[25 + g_unGPRSTX_Buff[10]]=0x16; 
             g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
             g_unGPRSTX_Buff[6] = 0x1E;     //数据总长度低位
             
            ucLength = 27 + g_unGPRSTX_Buff[10];;
    }
    //g_unGPRSTX_Buff[ucLength - 1]=0x16;     //张弢0324
    //g_unGPRSTX_Buff[ucLength]=0x16;         //张弢0324
    byAddCrc16(g_unGPRSTX_Buff, ucLength - 1); //张弢0324
    g_ucGPRSTxlen = ucLength + 1;
    g_ucGPRSTxNum = 1; 
/*	*/ 
   while(g_ucGPRSState!=GPRSState_IDLE);		//张弢0330 如串口在空闲状态，则可以发送
    g_ucGPRSState_TCounter=0;				//张弢0330 
    g_ucGPRSState = GPRSState_SMS;			//张弢0330 串口在发送短信状态
    g_unSms_s=1;
    //g_unFailSms++;	
    g_unSms_Count=15;
    UCA2TXBUF=g_unGPRSTX_Buff[0];  //开始发送数据	//张弢0324
    //UCA1TXBUF=g_unGPRSTX_Buff[0];  //开始发送数据       //张弢0324
}
//==============================================================================
//  函数名称   : SMS_CHINESE
//  功能描述   : 中心站短信发送
//  输入参数   : ucSMSType   BIT6       BIT5      BIT4             
//                      PHASE_BREAK  FAULT_OCCUR  FAULT_RST  
//                           BIT2     BIT1      BIT0
//                      EIGHT_PULSE  POWER_OFF  ABN_CHECK
//               SMS_Sequence: 电话号码序号
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       : 
//==============================================================================
/*
void SMS_CHINESE(unsigned char SMS_Sequence, unsigned char ucSMSType)
{
    unsigned char i,sum;
    unsigned char ucLength;
    unsigned char ucTemp;
    
    sum = PhoneNumJudge(SMS_Sequence);
    if(sum != 0x33)      //电话号码不正确，不需要发送短信
      return;
    
    PhoneNumTake();
    g_unGPRSTX_Buff[4] = 0x13;     //控制字  	
    g_unGPRSTX_Buff[15 + g_unGPRSTX_Buff[10]]= 0x03;   //定义短信格式数值   //00为英文 //01为中文	
    g_unGPRSTX_Buff[16 + g_unGPRSTX_Buff[10]]= 0x81;   //短信内容
    g_unGPRSTX_Buff[17 + g_unGPRSTX_Buff[10]]= 0x82;
    g_unGPRSTX_Buff[18 + g_unGPRSTX_Buff[10]]= 0x00;
    Frame_General();
    if(ucSMSType == ABN_CHECK)
    {   
        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 94;      //113-20+1 
        
        for(i = 0;i < 6;i++)
        {
             ucTemp = g_sSoeData.m_gAbnBuff[i]/10;       ///////年      
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             
             ucTemp = g_sSoeData.m_gAbnBuff[i]%10;       //////年    
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
        }      	
        for(i = 0;i < 12;i++)
        { 	                
             g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10] + i]= g_gSMSInfo[0][i];     ////保护开关跳开
        }    
             
             g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
             g_unGPRSTX_Buff[6] = 0x6B + g_unGPRSTX_Buff[10];     //数据总长度低位
     				     
             ucLength = 115 + g_unGPRSTX_Buff[10];

    }
    else if(ucSMSType == POWER_OFF)
    {
      	
        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 102;  // 

        for(i = 0;i < 6;i++)
        {
             ucTemp = g_sSoeData.m_gPowerOFFBuff[i]/10;       ///////年      
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             
             ucTemp = g_sSoeData.m_gPowerOFFBuff[i]%10;       //////年    
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
        }
                          
         g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10]]=0x89;     ////掉电
         g_unGPRSTX_Buff[103 + g_unGPRSTX_Buff[10]]=0x63;
         g_unGPRSTX_Buff[104 + g_unGPRSTX_Buff[10]]=0x35;
         g_unGPRSTX_Buff[105 + g_unGPRSTX_Buff[10]]=0x75; 
             
         g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
         g_unGPRSTX_Buff[6] = 0x63 + g_unGPRSTX_Buff[10];     //数据总长度低位
     				     
         ucLength = 107 + g_unGPRSTX_Buff[10];
    }
    else if(ucSMSType == EIGHT_PULSE)
    {

        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 96; //115-20+1
        //if((eight_select&0x7f)>0)                    ////////////第一次发生8脉冲	
        if(((SMS_array[0]==10)&&((eight_select&0x02)==0x02))||((SMS_array[0]==11)&&((eight_select&0x04)==0x04))||((SMS_array[0]==12)&&((eight_select&0x08)==0x08)))	
        {
            if((SMS_array[0]==10)&&((eight_select&0x02)==0x02))
               		eight_select&=~BIT1;
            else if((SMS_array[0]==11)&&((eight_select&0x04)==0x04))
               		eight_select&=~BIT2;
            else if((SMS_array[0]==12)&&((eight_select&0x08)==0x08))
               		eight_select&=~BIT3;
            for(i = 0;i < 6;i++)
            {
                ucTemp = sign_RSSI_time[i]/10;       ///////年      
                g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
                g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             
                ucTemp = sign_RSSI_time[i]%10;       //////年    
                g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
                g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
            }
               		
        }
        else                                         ////////////第二次发生8脉冲	
        {
            for(i = 0;i < 6;i++)
            {
                ucTemp = sign_RSSI_time1[i]/10;       ///////年      
                g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
                g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             
                ucTemp = sign_RSSI_time1[i]%10;       //////年    
                g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
                g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
            }
               		
        }               

        for(i = 0;i < 14;i++)
        { 	                
            g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10] + i]= g_gSMSInfo[1][i];     ////发出有效8脉冲
        } 
             
         g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
         g_unGPRSTX_Buff[6] = 0x6D + g_unGPRSTX_Buff[10];     //数据总长度低位
     				     
         ucLength = 117 + g_unGPRSTX_Buff[10];
    }
    else if(ucSMSType == FAULT_OCCUR)
    {
        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 94; //113-20+1 
            
        for(i = 0;i < 6;i++)
        {
             ucTemp = g_sSoeData.m_gSoeBuff[i]/10;       ///////年      
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             
             ucTemp = g_sSoeData.m_gSoeBuff[i]%10;       //////年    
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
        }
             
        for(i = 0;i < 14;i++)
        { 	                
             g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10] + i]= g_gSMSInfo[2][i];     ////接地故障发生
        }
             
        g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
        g_unGPRSTX_Buff[6] = 0x6B + g_unGPRSTX_Buff[10];     //数据总长度低位
        ucLength = 115 + g_unGPRSTX_Buff[10];

    }
    else if(ucSMSType == FAULT_RST)    
    {
        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 94;  //113-20+1             

        for(i = 0;i < 6;i++)
        {
             ucTemp = g_sSoeData.m_gSoeRSTBuff[i]/10;       ///////年      
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             
             ucTemp = g_sSoeData.m_gSoeRSTBuff[i]%10;       //////年    
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
        }
             
        for(i = 0;i < 14;i++)
        { 	                
             g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10] + i]= g_gSMSInfo[3][i];     ////接地故障复归
        } 
        g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
        g_unGPRSTX_Buff[6] = 0x6B + g_unGPRSTX_Buff[10];     //数据总长度低位
   	ucLength = 115 + g_unGPRSTX_Buff[10];
    }
    else if(ucSMSType == PHASE_BREAK) 
    {
        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 98;   //117-20+1
             
        for(i = 0;i < 6;i++)
        {
             ucTemp = g_sSoeData.m_gBreakBuff[i]/10;       ///////年      
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             
             ucTemp = g_sSoeData.m_gBreakBuff[i]%10;       //////年    
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
        }

        if((phase_copy&BIT0)==BIT0)    /////////A相
        {
             g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10]]=0x41;     ////断线
             g_unGPRSTX_Buff[103 + g_unGPRSTX_Buff[10]]=0x00;
             g_unGPRSTX_Buff[104 + g_unGPRSTX_Buff[10]]=0xF8;
             g_unGPRSTX_Buff[105 + g_unGPRSTX_Buff[10]]=0x76;
        }
        else
        {
             g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10]]=0x20;     ////空格
             g_unGPRSTX_Buff[103 + g_unGPRSTX_Buff[10]]=0x00;
             g_unGPRSTX_Buff[104 + g_unGPRSTX_Buff[10]]=0x20;
             g_unGPRSTX_Buff[105 + g_unGPRSTX_Buff[10]]=0x00;
        }	 
        if((phase_copy&BIT1)==BIT1)   //////////B相
        {
             g_unGPRSTX_Buff[106 + g_unGPRSTX_Buff[10]]=0x42;     ////断线
             g_unGPRSTX_Buff[107 + g_unGPRSTX_Buff[10]]=0x00;
             g_unGPRSTX_Buff[108 + g_unGPRSTX_Buff[10]]=0xF8;
             g_unGPRSTX_Buff[109 + g_unGPRSTX_Buff[10]]=0x76;
        } 
        else
        {
             g_unGPRSTX_Buff[106 + g_unGPRSTX_Buff[10]]=0x20;     ////空格
             g_unGPRSTX_Buff[107 + g_unGPRSTX_Buff[10]]=0x00;
             g_unGPRSTX_Buff[108 + g_unGPRSTX_Buff[10]]=0x20;
             g_unGPRSTX_Buff[109 + g_unGPRSTX_Buff[10]]=0x00;
        }	 
        if((phase_copy&BIT2)==BIT2)   //////////C相
        {
             g_unGPRSTX_Buff[110 + g_unGPRSTX_Buff[10]]=0x43;     ////断线
             g_unGPRSTX_Buff[111 + g_unGPRSTX_Buff[10]]=0x00;
             g_unGPRSTX_Buff[112 + g_unGPRSTX_Buff[10]]=0xF8;
             g_unGPRSTX_Buff[113 + g_unGPRSTX_Buff[10]]=0x76;
        }
        else
        {
             g_unGPRSTX_Buff[110 + g_unGPRSTX_Buff[10]]=0x20;     ////空格
             g_unGPRSTX_Buff[111 + g_unGPRSTX_Buff[10]]=0x00;
             g_unGPRSTX_Buff[112 + g_unGPRSTX_Buff[10]]=0x20;
             g_unGPRSTX_Buff[113 + g_unGPRSTX_Buff[10]]=0x00;
        }	
             	   	           	  
        g_unGPRSTX_Buff[114 + g_unGPRSTX_Buff[10]]=0xAD;     ////断线
        g_unGPRSTX_Buff[115 + g_unGPRSTX_Buff[10]]=0x65;
        g_unGPRSTX_Buff[116 + g_unGPRSTX_Buff[10]]=0xBF;
        g_unGPRSTX_Buff[117 + g_unGPRSTX_Buff[10]]=0x7E;
             
        g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
        g_unGPRSTX_Buff[6] = 0x6F + g_unGPRSTX_Buff[10];     //数据总长度低位
    				     
        ucLength =119 + g_unGPRSTX_Buff[10];

    }
	
    //g_unGPRSTX_Buff[ucLength - 1]=0x16;             //张弢0324
    //g_unGPRSTX_Buff[ucLength]=0x16;                 //张弢0324
    byAddCrc16(g_unGPRSTX_Buff, ucLength - 1); //张弢0324
      
    g_ucGPRSTxlen = ucLength + 1;
    g_ucGPRSTxNum = 1;    
	
    while(g_ucGPRSState!=GPRSState_IDLE);		//张弢0330 如串口在空闲状态，则可以发送
    g_ucGPRSState_TCounter=0;				//张弢0330    
    g_ucGPRSState = GPRSState_SMS;			//张弢0330 串口在发送短信状态
    
    UCA2TXBUF=g_unGPRSTX_Buff[0];  //开始发送数据	//张弢0324  
    //UCA1TXBUF=g_unGPRSTX_Buff[0];  //开始发送数据       //张弢0324
}
*/
//==============================================================================
//  函数名称   : SMS_CHINESE
//  功能描述   : 中心站短信发送
//  输入参数   : ucSMSType   BIT6       BIT5      BIT4             
//                      PHASE_BREAK  FAULT_OCCUR  FAULT_RST  
//                           BIT2     BIT1      BIT0
//                      EIGHT_PULSE  POWER_OFF  ABN_CHECK
//               SMS_Sequence: 电话号码序号
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ://张弢汉字短信
//==============================================================================
void SMS_CHINESE(unsigned char SMS_Sequence, unsigned char ucSMSType)
{
    unsigned char i,sum;
    unsigned char ucLength;
    unsigned char ucTemp;
    
    sum = PhoneNumJudge(SMS_Sequence);
    if(sum != 0x33)      //电话号码不正确，不需要发送短信
    	{	
      return;
    	}
    
    PhoneNumTake();
    g_unGPRSTX_Buff[4] = 0x13;     //控制字  	
    g_unGPRSTX_Buff[15 + g_unGPRSTX_Buff[10]]= 0x03;   //定义短信格式数值   //00为英文 //01为中文	
    g_unGPRSTX_Buff[16 + g_unGPRSTX_Buff[10]]= 0x81;   //短信内容
    g_unGPRSTX_Buff[17 + g_unGPRSTX_Buff[10]]= 0x82;
    g_unGPRSTX_Buff[18 + g_unGPRSTX_Buff[10]]= 0x00;
    Frame_General();
    if(ucSMSType == ABN_CHECK)
    {   
        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 94;      //113-20+1 
        
        for(i = 0;i < 6;i++)
        {
             ucTemp = g_sSoeData.m_gAbnBuff[i]/10;       ///////年      
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             
             ucTemp = g_sSoeData.m_gAbnBuff[i]%10;       //////年    
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
        }      	
        for(i = 0;i < 12;i++)
        { 	                
             g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10] + i]= g_gSMSInfo[0][i];     ////保护开关跳开
        }    
             
             g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
             g_unGPRSTX_Buff[6] = 0x6B + g_unGPRSTX_Buff[10];     //数据总长度低位
     				     
             ucLength = 115 + g_unGPRSTX_Buff[10];

    }
    else if(ucSMSType == POWER_OFF)
    {
      	
        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 86;//102;  // 

        for(i = 0;i < 6;i++)
        {
             ucTemp = g_sSoeData.m_gPowerOFFBuff[i]/10;       ///////年      
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             
             ucTemp = g_sSoeData.m_gPowerOFFBuff[i]%10;       //////年    
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
        }
                          
         g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10]]=0x63;     ////掉电
         g_unGPRSTX_Buff[103 + g_unGPRSTX_Buff[10]]=0x89;
         g_unGPRSTX_Buff[104 + g_unGPRSTX_Buff[10]]=0x75;
         g_unGPRSTX_Buff[105 + g_unGPRSTX_Buff[10]]=0x35; 
             
         g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
         g_unGPRSTX_Buff[6] = 0x63 + g_unGPRSTX_Buff[10];     //数据总长度低位
     				     
         ucLength = 107 + g_unGPRSTX_Buff[10];
    }
//张弢汉字短信 自动上传指令
    /*
    else if(ucSMSType == SEND_AUTO)
    {
      	
        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 90;  // 

        for(i = 0;i < 6;i++)
        {
             ucTemp = g_sSoeData.m_gPowerOFFBuff[i]/10;       ///////年      
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             
             ucTemp = g_sSoeData.m_gPowerOFFBuff[i]%10;       //////年    
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
        }
                          
         g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10]]=0x81;     ////自动上传\u81ea\u52a8\u4e0a\u4f20
         g_unGPRSTX_Buff[103 + g_unGPRSTX_Buff[10]]=0xEA;
         g_unGPRSTX_Buff[104 + g_unGPRSTX_Buff[10]]=0x52;
         g_unGPRSTX_Buff[105 + g_unGPRSTX_Buff[10]]=0xA8; 
         g_unGPRSTX_Buff[106 + g_unGPRSTX_Buff[10]]=0x4E;
         g_unGPRSTX_Buff[107 + g_unGPRSTX_Buff[10]]=0x0A; 		 
         g_unGPRSTX_Buff[108 + g_unGPRSTX_Buff[10]]=0x4F;
         g_unGPRSTX_Buff[109 + g_unGPRSTX_Buff[10]]=0x20; 
		 
         g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
         g_unGPRSTX_Buff[6] = 103 + g_unGPRSTX_Buff[10];     //数据总长度低位
     				     
         ucLength = 111 + g_unGPRSTX_Buff[10];
    }*/
//张弢汉字短信 自动上传指令	
    else if(ucSMSType == EIGHT_PULSE)
    {

        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 96; //115-20+1
        //if((eight_select&0x7f)>0)                    ////////////第一次发生8脉冲	
        if(((SMS_array[0]==10)&&((eight_select&0x02)==0x02))||((SMS_array[0]==11)&&((eight_select&0x04)==0x04))||((SMS_array[0]==12)&&((eight_select&0x08)==0x08)))	
        {
            if((SMS_array[0]==10)&&((eight_select&0x02)==0x02))
               		eight_select&=~BIT1;
            else if((SMS_array[0]==11)&&((eight_select&0x04)==0x04))
               		eight_select&=~BIT2;
            else if((SMS_array[0]==12)&&((eight_select&0x08)==0x08))
               		eight_select&=~BIT3;
            for(i = 0;i < 6;i++)
            {
                ucTemp = sign_RSSI_time[i]/10;       ///////年      
                g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
                g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             
                ucTemp = sign_RSSI_time[i]%10;       //////年    
                g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
                g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
            }
               		
        }
        else                                         ////////////第二次发生8脉冲	
        {
            for(i = 0;i < 6;i++)
            {
                ucTemp = sign_RSSI_time1[i]/10;       ///////年      
                g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
                g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             
                ucTemp = sign_RSSI_time1[i]%10;       //////年    
                g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
                g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
            }
               		
        }               

        for(i = 0;i < 14;i++)
        { 	                
            g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10] + i]= g_gSMSInfo[1][i];     ////发出有效8脉冲
        } 
             
         g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
         g_unGPRSTX_Buff[6] = 0x6D + g_unGPRSTX_Buff[10];     //数据总长度低位
     				     
         ucLength = 117 + g_unGPRSTX_Buff[10];
    }
    else if(ucSMSType == FAULT_OCCUR)
    {
        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 94; //113-20+1 
            
        for(i = 0;i < 6;i++)
        {
             ucTemp = g_sSoeData.m_gSoeBuff[i]/10;       ///////年      
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             
             ucTemp = g_sSoeData.m_gSoeBuff[i]%10;       //////年    
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= 0x00;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
        }
             
        for(i = 0;i < 14;i++)
        { 	                
             g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10] + i]= g_gSMSInfo[2][i];     ////接地故障发生
        }
             
        g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
        g_unGPRSTX_Buff[6] = 0x6B + g_unGPRSTX_Buff[10];     //数据总长度低位
        ucLength = 115 + g_unGPRSTX_Buff[10];

    }
    else if(ucSMSType == FAULT_RST)    
    {
        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 94;  //113-20+1             

        for(i = 0;i < 6;i++)
        {
             ucTemp = g_sSoeData.m_gSoeRSTBuff[i]/10;       ///////年      
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= 0;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             
             ucTemp = g_sSoeData.m_gSoeRSTBuff[i]%10;       //////年    
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= 0;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
        }
             
        for(i = 0;i < 14;i++)
        { 	                
             g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10] + i]= g_gSMSInfo[3][i];     ////接地故障复归
        } 
        g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
        g_unGPRSTX_Buff[6] = 0x6B + g_unGPRSTX_Buff[10];     //数据总长度低位
   	ucLength = 115 + g_unGPRSTX_Buff[10];
    }
    else if(ucSMSType == PHASE_BREAK) 
    {
        g_unGPRSTX_Buff[19 + g_unGPRSTX_Buff[10]]= 98;   //117-20+1
             
        for(i = 0;i < 6;i++)
        {
             ucTemp = g_sSoeData.m_gBreakBuff[i]/10;       ///////年      
             g_unGPRSTX_Buff[20 + g_unGPRSTX_Buff[10] + i * 6]= 0;
             g_unGPRSTX_Buff[21 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
             
             ucTemp = g_sSoeData.m_gBreakBuff[i]%10;       //////年    
             g_unGPRSTX_Buff[22 + g_unGPRSTX_Buff[10] + i * 6]= 0;
             g_unGPRSTX_Buff[23 + g_unGPRSTX_Buff[10] + i * 6]= ucTemp+0x30;
        }

        if((phase_copy&BIT0)==BIT0)    /////////A相
        {
             g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10]]=0x00;     ////A相
             g_unGPRSTX_Buff[103 + g_unGPRSTX_Buff[10]]=0x41;
             g_unGPRSTX_Buff[104 + g_unGPRSTX_Buff[10]]=0x76;
             g_unGPRSTX_Buff[105 + g_unGPRSTX_Buff[10]]=0xF8;
        }
        else
        {
             g_unGPRSTX_Buff[102 + g_unGPRSTX_Buff[10]]=0x00;     ////空格
             g_unGPRSTX_Buff[103 + g_unGPRSTX_Buff[10]]=0x20;
             g_unGPRSTX_Buff[104 + g_unGPRSTX_Buff[10]]=0x00;
             g_unGPRSTX_Buff[105 + g_unGPRSTX_Buff[10]]=0x20;
        }	 
        if((phase_copy&BIT1)==BIT1)   //////////B相
        {
             g_unGPRSTX_Buff[106 + g_unGPRSTX_Buff[10]]=0x00;     ////B相
             g_unGPRSTX_Buff[107 + g_unGPRSTX_Buff[10]]=0x42;
             g_unGPRSTX_Buff[108 + g_unGPRSTX_Buff[10]]=0x76;
             g_unGPRSTX_Buff[109 + g_unGPRSTX_Buff[10]]=0xF8;
        } 
        else
        {
             g_unGPRSTX_Buff[106 + g_unGPRSTX_Buff[10]]=0x00;     ////空格
             g_unGPRSTX_Buff[107 + g_unGPRSTX_Buff[10]]=0x20;
             g_unGPRSTX_Buff[108 + g_unGPRSTX_Buff[10]]=0x00;
             g_unGPRSTX_Buff[109 + g_unGPRSTX_Buff[10]]=0x20;
        }	 
        if((phase_copy&BIT2)==BIT2)   //////////C相
        {
             g_unGPRSTX_Buff[110 + g_unGPRSTX_Buff[10]]=0x00;     ////C相
             g_unGPRSTX_Buff[111 + g_unGPRSTX_Buff[10]]=0x43;
             g_unGPRSTX_Buff[112 + g_unGPRSTX_Buff[10]]=0x76;
             g_unGPRSTX_Buff[113 + g_unGPRSTX_Buff[10]]=0xF8;
        }
        else
        {
             g_unGPRSTX_Buff[110 + g_unGPRSTX_Buff[10]]=0x00;     ////空格
             g_unGPRSTX_Buff[111 + g_unGPRSTX_Buff[10]]=0x20;
             g_unGPRSTX_Buff[112 + g_unGPRSTX_Buff[10]]=0x00;
             g_unGPRSTX_Buff[113 + g_unGPRSTX_Buff[10]]=0x20;
        }	
             	   	           	  
        g_unGPRSTX_Buff[114 + g_unGPRSTX_Buff[10]]=0x65;     ////断线
        g_unGPRSTX_Buff[115 + g_unGPRSTX_Buff[10]]=0xAD;
        g_unGPRSTX_Buff[116 + g_unGPRSTX_Buff[10]]=0x7E;
        g_unGPRSTX_Buff[117 + g_unGPRSTX_Buff[10]]=0xBF;
             
        g_unGPRSTX_Buff[5] = 0x00;     //数据总长度高位
        g_unGPRSTX_Buff[6] = 0x6F + g_unGPRSTX_Buff[10];     //数据总长度低位
    				     
        ucLength =119 + g_unGPRSTX_Buff[10];

    }
	
    //g_unGPRSTX_Buff[ucLength - 1]=0x16;             //张弢0324
    //g_unGPRSTX_Buff[ucLength]=0x16;                 //张弢0324
    byAddCrc16(g_unGPRSTX_Buff, ucLength - 1); //张弢0324
      
    g_ucGPRSTxlen = ucLength + 1;
    g_ucGPRSTxNum = 1;    
	/**/
    while(g_ucGPRSState!=GPRSState_IDLE);		//张弢0330 如串口在空闲状态，则可以发送
    g_ucGPRSState_TCounter=0;				//张弢0330    
    g_ucGPRSState = GPRSState_SMS;			//张弢0330 串口在发送短信状态
    g_unSms_s=1;
    //g_unFailSms++;	
    g_unSms_Count = 15;	
    UCA2TXBUF=g_unGPRSTX_Buff[0];  //开始发送数据	//张弢0324  
    //UCA1TXBUF=g_unGPRSTX_Buff[0];  //开始发送数据       //张弢0324
}

//==============================================================================
//  函数名称   : Send_GPRS_SMS
//  功能描述   : 短消息模式GPRS数据传输
//  输入参数   : ucSMSType   BIT6       BIT5      BIT4       BIT3      
//                      PHASE_BREAK  FAULT_OCCUR  FAULT_RST  SEND_AUTO
//                           BIT2     BIT1      BIT0
//                      EIGHT_PULSE  POWER_OFF  ABN_CHECK
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       : 
//==============================================================================
void Send_GPRS_SMS(unsigned char ucSMSType,unsigned char ucPhoneNo)
{
    //unsigned char i;
    if((ucSMSType == ZIZHAN_XUN)||(ucSMSType == PHONE_XUN))
   {
   	SMS_CENTER(ucSMSType);
       delayms(300);
   }	
    else
    {
    //for(i = 0; i < 4; i++)
    //g_gSmsPhone{
    //    if((g_ucphone_perm[i] & ucSMSType) != 0)
    //if((g_ucphone_perm[ucPhoneNo] & ucSMSType) != 0)
    if((g_gSmsPhone[ucPhoneNo+48] & ucSMSType) != 0)
        {
            if(ucPhoneNo == 0)   //发给中心站，格式特殊
            {
                SMS_CENTER(ucSMSType);
                delayms(300);
            }
            else           //其他号码，发送中文
            {
                if (ucSMSType != SEND_AUTO)
                {  
                SMS_CHINESE(ucPhoneNo,ucSMSType);
                delayms(300);
                }
            }
        }
    //}
    }
   
 
}
//==============================================================================
//  函数名称   : CreatNewSMS
//  功能描述   : 把需要发送短信的条数及在内存中的数据保存下来，在大循环进一步处理
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void CreatNewSMS(unsigned char ucSMSType)
{
    //unsigned char unTemp;    //临时数据
    if((g_gProcCnt[PC_GPRS_MODEL]==0x55)||(ucSMSType==ZIZHAN_XUN)||(ucSMSType==PHONE_XUN))
    	{	
       g_unSMS_Index[g_unFrameFlag_Sms] = ucSMSType;
	g_unFrameFlag_Sms++;
	if(g_unFrameFlag_Sms>=MAX_SMS_NUM)
		g_unFrameFlag_Sms=0;
    	}                
}

