/*------------------------------------------------------------------------
 Module:      protocol.cpp
 Author:      linxueqin
 Project:     规约平台
 State:       V1.0
 Creation Date:   2014-4-28
 Description:   规约基类的定义
------------------------------------------------------------------------*/

#include "Protocol.h"
#include "..\Main.h"


/***************************************************************
  Function:CProtocol
    规约基类构造函数
  参数：无
  返回：无
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
      规约平台初始化
  参数：无
  返回：TRUE 成功，FALSE 失败
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
    获取自身地址
  参数：无
  
  返回：自身地址
***************************************************************/
WORD CProtocol::GetOwnAddr(void)      
{
  return  g_gRunPara[RP_COMM_ADDR];//
}
/***************************************************************
  Function:NeatenCommBuf
    通信缓冲区的规整
  参数：pCommIO
    pCommIO　指向通信缓冲区的指针
  返回：无
***************************************************************/
void CProtocol::NeatenCommBuf(VCommBuf *pCommBuf)
{
 // register unsigned  int i,j;

  if (pCommBuf->wReadPtr == 0)
  {
    return ; //读指针已经为0
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
    搜索信息帧
  参数：无
  
  返回：TRUE 找到正确的一帧报文，FALSE 未找到一帧正确的报文
***************************************************************/
BOOL CProtocol::SearchFrame(void)
{
  DWORD Rc;
  DWORD Len;//已处理过的字节数
  short MsgLen;

  while (1)
  {
    MsgLen = m_RecBuf.wWritePtr - m_RecBuf.wReadPtr;
    if (MsgLen <= 0)
    {
      return FALSE;
    }

    Rc = SearchOneFrame(&m_RecBuf.pBuf[m_RecBuf.wReadPtr], MsgLen);
    Len = Rc & ~FRAME_SHIELD; //已处理过的字节数

    switch  (Rc & FRAME_SHIELD)
    {
      case FRAME_OK:
        DisableRetry();
        m_RecFrame.pBuf = &m_RecBuf.pBuf[m_RecBuf.wReadPtr];  //记录有效报文的起始地址
        m_RecFrame.wWritePtr = (WORD)Len; //记录有效报文的长度
        m_RecBuf.wReadPtr += (WORD)Len; //指针移到下一报文处
        return TRUE;

      case FRAME_ERR:
        if (!Len)
        {
          m_RecBuf.wReadPtr++;
        }
        else
        {
          m_RecBuf.wReadPtr += Len; //指针移到下一报文处
        }
        break;

      case FRAME_LESS:
        m_RecBuf.wReadPtr++;  //右移一个字节继续检索正确报文 lxq
        break;
        //m_RecBuf.wReadPtr += (WORD)Len; //指针移到下一报文处
        //return FALSE;
      default:
        break;//??do what
    }//switch
  }//while
}

/***************************************************************
  Function:SearchHead
    搜索报文中的一个关键字
  参数：Buf, Len,　Offset1, Key1
    Buf 报文缓冲区头指针
    Len 要搜索的报文长度
    Offset1 关键字的在报文中的偏移位置
    Key1 关键字
  返回：关键字在报文缓冲区中的偏移位置
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
    规约接收数据并解析
    当socket中断连接时此函数返回（返回后socket重新侦听，将以阻塞的方式停留在select函数处）
  参数：无

  返回：无
***************************************************************/
int CProtocol::RcvData(int* iSocket)
{
    int rxlen=0;
    int nLen=1024;    
    //while(1)//要不要继续收报文? lxq
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
            {//将RF口收到的数据转发出去
              CommSendData(m_RecBuf.pBuf,rxlen,g_CmIdEX);
            }
          #endif
          DoReceive();
       }
      else if(rxlen <= 0)//recv=0 对端发出断开命令
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
    判断DPRAM中命令反馈区是否正确
  参数：RamBase dpram地址
    buf  偏移量

  返回：1 命令正确； 0 错误
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



//校验和
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
//参数查询及设置部分 + 远程升级====================================================================================
//自定义报文
BOOL CProtocol::RecFrame69(void)
{
  pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;

    switch(pReceiveFrame->Frame69.Type)
    {
        case 0x7a://读文件
            RecReadFile();
            break;
        case 0x7d://写文件
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
//张弢 0404 短信接收
//******************************************************
/*
BOOL CProtocol::RecFrameAA(void)
//BOOL CPrtcSms::RecFrameAA(VFrameAA * pReceiveFrame)
{
  pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;

    switch(pReceiveFrame->FrameAA.Type)
    {
      
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
    
      case 0x16:      //读GPRS配置
          RecIHDPara(pReceiveFrame);
          break;		  
      case 0x18:
          //RecIHDWConf(pReceiveFrame);
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
  */
//******************************************************
/*
GRPS 模块短信回执：
GRPS 模块收到正确的数据之后立刻回复确认命令：
命令字：0x17
GPRS 模块: AA 55 AA 55 17 00 05 81 89 00 01 02 38 72
包体的内容为1 个字节，0x02表示收到正确短信数据。
短信发送完成之后，发送结果的回应包
命令字：0x17
GPRS 模块:AA 55 AA 55 17 00 05 81 89 00 01 00 2A 51
包体的内容为1 个字节，0x00表示发送失败，0x01 表示发送成功。
*/
//******************************************************
BOOL CProtocol::RecSendSMSOK(void)
{
    //BYTE i;
    BYTE *pData;/*
    WORD wFile;
    BYTE bySec;//节名
    BYTE bySCQ;//限定词
    WORD wStartNo;//起始序号
    WORD wSOENum;//读取的SOE个数
    WORD wLoadNum;//读取的负荷数据条数(每条以时间为单位)
    WORD wTemp;   //录波数据的临时节名
*/
    pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;//加这句代码的原因是，若是68报文跳转到此函数，CProtocol中的pReceiveFrame指针未赋值
    pData = &pReceiveFrame->FrameAA.byContent[0];
    
    pData +=4;

    switch(*pData)
    {
      case 0: //0x00表示发送失败，
	    //继续发送短信，发送已发送过的
	    g_unSms_s = 0;
	    g_unSms_Count=0;
	    g_unFailSms++;
	    if(g_unFailSms>=3)
	    	{
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
	    return FALSE;
           break;
      case 1://0x01 表示发送成功。
           g_unPhoneNo++;
	    if(g_unPhoneNo>=4)//4// 4个号码发完 发下一条
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
      case 2://0x02表示收到正确短信数据
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
使用手机给信号源发送 108101008216 用来召唤状态，过几秒钟会收到信号源上传的状态信息

短信接收格式和短信发送格式一样，只是55 AA 55 AA改成了 AA 55 AA 55。
包头:AA 55 AA 55
命令字:13 
包体长度:2字节
手机号码TLV:	Tag: 81 80
			Length: 电话号 Value: 字符串
短信格式 TLV:	Tag: 81 81 
			Length: 2 字节
			Value: 当前支持2 种格式,2 表示标准英文短信，3 表示标准中文短信
短信内容 TLV: 	Tag: 81 82
			Length: 短信正文长度
			Value: 短信正文
CRC:2 字节
*/
//******************************************************
BOOL CProtocol::RecSMS(void)
{
    BYTE i,Temp[21];
    BYTE *pData;/*
    WORD wFile;
    BYTE bySec;//节名
    BYTE bySCQ;//限定词
    WORD wStartNo;//起始序号
    WORD wSOENum;//读取的SOE个数
    WORD wLoadNum;//读取的负荷数据条数(每条以时间为单位)
    WORD wTemp;   //录波数据的临时节名
*/
    pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;//加这句代码的原因是，若是68报文跳转到此函数，CProtocol中的pReceiveFrame指针未赋值
    pData = &pReceiveFrame->FrameAA.byContent[0];
    
    pData +=4;

     for(i=0;i<11;i++)                         //////存储手机号码
        {
            ucTempTEL_NUM[i] = *pData++;
        } 	
     for(i=0;i<21;i++)                         //////存储手机号码
        {
            Temp[i] = *pData++;
        } 
    if((Temp[9]!=0x31)||(Temp[10]!=0x30)||(Temp[19]!=0x31)||(Temp[20]!=0x36))
		{return FALSE;}
    if((Temp[11]==0x39)&&(Temp[12]==0x31))
    	{
    	eight_select=0x8F; //eight_select|=BIT7;                    ////////////置首次发生8脉冲的标志		
    	Sign_Repeat(0,0);
    	return TRUE;
    	}
    switch(Temp[12])
    {
      case 0x31: //手机召唤信号源
      	    CreatNewSMS(PHONE_XUN);
	    return TRUE;		
           break;
      case 0x32://子站询问   
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
//张弢 0404 短信接收
//******************************************************
//解析读参数报文
BOOL CProtocol::RecReadFile(void)
{
    BYTE i;
    BYTE *pData;
    WORD wFile;
    BYTE bySec;//节名
    BYTE bySCQ;//限定词
    WORD wStartNo;//起始序号
    WORD wSOENum;//读取的SOE个数
    WORD wLoadNum;//读取的负荷数据条数(每条以时间为单位)
    WORD wTemp;   //录波数据的临时节名

    pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;//加这句代码的原因是，若是68报文跳转到此函数，CProtocol中的pReceiveFrame指针未赋值
    pData = &pReceiveFrame->Frame69.Data[0];


    	wFile = MAKEWORD(pData[0],pData[1]);
    	wTemp = MAKEWORD(pData[2],pData[3]);
    	pData +=2;
		
    bySec = *pData++;
    bySCQ = *pData++;
    
    if(bySCQ == 5 )
     {
        SendWrPaAck(wFile,bySec);//选择写参数
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
          SendSoeSavedStu();//读SOE状态
        }
        break;
      case 3:
        if(bySCQ == 6)
        {//触发子站测试
          SendTrigAck();
        }
        break;
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
	case 9://张弢 读汉字站名
       case 10://张弢 目标校准，上位机下载参数 初始值为电压60V,电流2A
         
      case 15:
      case 16:
      case 31:
      case 32:
        m_wSendPaNum = 0;
        SendReadPa(wFile,bySec);//读终端自身参数
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
          SendReadRecData(wFile,i);//读录波数据
          delayms(300);
        }
        
          break;
      case 34:
        g_sRecData.m_gRecNumCom      = CAT_SpiReadWord(EEPADD_REC_NUM);
        g_sRecData.m_CurWritePageCom = CAT_SpiReadWord(EEPADD_REC_POS);
        m_wSendPaNum = 0;
        
        SendReadRecData(wFile,wTemp);//读录波数据
          delayms(300);
        break;
		
	case 11:
		if(bySec==1)//短信接收
			{
			if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(bySec,wFile,this);
			break;
			}
		else if(bySec==2)//apn
			{
			if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(2,12,this);//读APN
			break;
			}
 		if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(bySec,wFile,this);
		delayms(2000);
		if(pGprs!= null) ((CPrtcSms*)pGprs)->SendRCmdToIHD(5,11,this);//读备用IP

          	break;
      default:
      break; 
    }
    return TRUE;
}

//解析写参数报文
BOOL CProtocol::RecWriteFile(void)
{
    BYTE *pData;
    WORD wFile;
    BYTE bySec;//节名
    WORD wSecLen;//段长度
    WORD i = 0,j = 0,k=0;
    WORD wPaStartId = 0;
    WORD wInfoAddr = 0;
    unsigned int unTemp = 0;
   // WORD wOldValBk;
  //  WORD wOldCfgKeyBk;
  //  BYTE bLine;
    pReceiveFrame = (VParaFrame*)m_RecFrame.pBuf;//加这句代码的原因是，若是68报文跳转到此函数，CProtocol中的pReceiveFrame指针未赋值
    
    wInfoAddr = MAKEWORD(pReceiveFrame->Frame69.InfoAddr[0],pReceiveFrame->Frame69.InfoAddr[1]);
    wPaStartId = wInfoAddr - 0x6902;

    pData = &pReceiveFrame->Frame69.Data[0];

    	wFile = MAKEWORD(pData[0],pData[1]);
    	pData +=2;

    bySec = *pData++;
    wSecLen= *pData++;//节长度
    
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
      case 0x81://设置GPRS为配置模式
     //   GPRS_SETA_L();
        break;
      case 0x82:
        //ExitGprsSet();
        break;
      case 4:
        if(bySec == 0 )//写运行参数
        {
          for(i = wPaStartId,j = 0; j < wSecLen/2 && i < RUN_PARA_NUM-1;i++,j++)
          {//b
             
                  //参数赋值
                  g_gRunPara[i] = MAKEWORD(pData[0],pData[1]);
                  pData += 2;

        }//b
        /*//张弢 遥测起始地址修改运行参数
        pData += 10;//张弢 0329  主动上传间隔时间
	 g_ucPara_stime =*pData;   //张弢 0329  主动上传间隔时间
	*/	//张弢 遥测起始地址修改运行参数
        if(g_gRunPara[RP_COMM_ADDR] == 0)//作为复位点表的方式，如果子站地址设置成0，则复位
        {
            RstRunPara();  //运行参数表复位
        } 
        g_ucParaChang |= BIT0;
        SetEqpInfo();
      }
      break;
      case 5:
        if(bySec == 0 )//写IEC参数
        {
          for(i = wPaStartId,j = 0; j < wSecLen && i < IEC_PARA_NUM-1;i++,j++)
          {
            g_ucPara101[i] = *pData++;
          }
          if(g_ucPara101[0] == 0)//作为复位点表的方式，如果第一个就设置为0，则利用这种方式复位点表
          {
              RstIEC101Para();  //IEC参数点表复位
          } 
          initpara();//更新通信内存参数
          g_ucParaChang |= BIT1;
        }
        break;  
      case 6:
        if(bySec == 0 )//写遥信点表
        {
          for(i = wPaStartId,j = 0; j < wSecLen/2 && i < IEC_YX_NUM;i++,j++)
          {
            g_ucYXAddr[i] = MAKEWORD(pData[0],pData[1]);
            pData += 2;
          }
	   g_ucYXAddr[i] = 0;	  
          if(g_ucYXAddr[0] == 0)//作为复位点表的方式，如果第一个就设置为0，则利用这种方式复位点表
          {
             RstIEC101YxAddr();  //遥信地址表复位
             //DelALLSOE();
          }
          InitTsYxInfo();  
          SetEqpInfo();
          g_ucParaChang |= BIT2; 
        }
        
        break;
      case 7:
        if(bySec == 0 )//写遥测点表
        {
          for(i = wPaStartId,j = 0; j < wSecLen && i < IEC_YC_NUM;i++,j++)
          {
            g_ucYCAddr[i] = *pData++;
          }
           g_ucYCAddr[i] = 0;		  
          if(g_ucYCAddr[0] == 0)//作为复位点表的方式，如果第一个就设置为0，则利用这种方式复位点表
          {
              RstIEC101YcAddr();  //遥测地址表复位
          } 
          InitTsYcInfo();
          SetEqpInfo();
          g_ucParaChang |= BIT3;
        }
      break;
      case 8:
        if(bySec == 0 )//写遥控点表
        {
        /*  for(i = wPaStartId,j = 0; j < wSecLen && i < IEC_YK_NUM;i++,j++)
          {
            g_ucYKPa[i] = *pData++;
          }
          if(g_ucYKPa[0] == 255)//作为复位点表的方式，如果第一个就设置为255，则利用这种方式复位点表
          {
              RstIEC101YkAddr();  //遥控地址表复位
          } */
          g_ucParaChang |= BIT4;
        }
      break;
      case 9://张弢 读汉字站名
        if(bySec == 1 )//短信用 汉字站名
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
          if(g_gCnName[0] == 0)//作为复位点表的方式，如果第一个电话号码的第一个字符设置为R，则利用这种方式复位点表
          {
              RstCnName();//张弢 
          } //张弢0328
          g_ucParaChang |= BIT6;
          SendWrPaSuc();
        }
	else if(bySec == 5 )//录波文件用 英文站名
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
          if(g_gLBName[0] == 0)//作为复位点表的方式，如果第一个电话号码的第一个字符设置为R，则利用这种方式复位点表
          {
            RstLBName(); 	  
          } //张弢0328
          g_ucParaChang |= BIT6;
          SendWrPaSuc();
        }	
      break;
      case 10://张弢 目标校准，上位机下载参数 初始值为电压60V,电流2A
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
            		unTemp = (((unsigned long)utempadj[j]* unTemp) /xjj); //张弢 目标校准，上位机下载参数 初始值为电压60V,电流2A
		
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
            unTemp = (((unsigned long)utempadj[3]*100 * unTemp) /g_gRmtMeas[RM_U0]); //张弢 目标校准，上位机下载参数 初始值为电压60V,电流2A
            if(unTemp > 3000 && unTemp < 6000)
                	g_gProcCnt[3] = unTemp;
            unTemp = g_gProcCnt[4];
            unTemp = (((unsigned long)utempadj[4]*100 * unTemp) /g_gRmtMeas[RM_I0]); //张弢 目标校准，上位机下载参数 初始值为电压60V,电流2A
            if(unTemp > 3000 && unTemp < 6000)
                	g_gProcCnt[4] = unTemp;
            for(i = 0; i <= 4; i++)
            		g_gRunPara[i + RP_UA_ADJ] = g_gProcCnt[i];   
            g_gChangFlag[CHNG_MUBIAO] = ON;   
         }	   
	  if(bySec == 1 )//低电压校准参数
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
      case 11:  //写GPRS参数
          if(bySec==1)//短信接收
			{
			if(pGprs!= null) ((CPrtcSms*)pGprs)->SendWCmdToIHD(1,bySec,pData-1,this);
			delayms(3000);	  
          		if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(6,RESET,g_gIPPort,this);//重启GPRS模块 		  
			break;
			}
	   else if	(bySec==2)//apn 
	   	{
	   	 if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(wSecLen,2,pData-1,this);//写APN
	   	 delayms(3000);	  
               if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(6,RESET,g_gIPPort,this);//重启GPRS模块 	
	   	}
          if(pGprs != null) ((CPrtcSms *)pGprs)->SendWCmdToIHD(6,0,pData-1,this);
	   delayms(3000);	  
	   if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(6,5,pData+5,this);
	   delayms(3000);	  
          if(pGprs != null)((CPrtcSms *)pGprs)->SendWCmdToIHD(6,RESET,g_gIPPort,this);//重启GPRS模块 		  
          break;	  
      case 16:
        if(bySec == 0 )//写电话号码表
        {
          for(i = wPaStartId,j = 0; j < wSecLen && i < PHONE_PA_NUM - 1;i++,j++)
          {
            g_gSmsPhone[i] = *pData++;
          }
          if(g_gSmsPhone[PHONE1] == 'R')//作为复位点表的方式，如果第一个电话号码的第一个字符设置为R，则利用这种方式复位点表
          {
              //RstSmsPhoneInfo();//张弢0328
          } //张弢0328
          g_ucParaChang |= BIT5;
          SendWrPaSuc();
        }
      break;
      case 31:
        if(bySec == 0 )//遥信对点
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
        if(bySec == 0 )//遥测对点
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
    m_SendBuf.wWritePtr += 6;//信息体地址2+文件名2+节名1+节长度1
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
    unsigned char ucTemp[230]; //临时数组
    unsigned long ulTemp;    //临时数据
    BYTE j = 0;
    SendFrameHeadForPa(Style, Reason);

    wInfoAddr = MAKEWORD(pReceiveFrame->Frame69.InfoAddr[0],pReceiveFrame->Frame69.InfoAddr[1]);
    wInfoAddr += m_wSendPaNum;
      
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = LOBYTE(wInfoAddr);//写信息体地址
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
      
   case 33://读录波索引

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
            m_SendBuf.pBuf[4] |= 0x80;//控制域最高位用来表示还有没后续报文
        SendFrameTailForPa();
        break;
   case 34://读录波数据
       ulTemp = (unsigned long)(0x30+(SecName-1)*5)<<16;//flash地址
       for(k= 0;k<100;k++)   //wPaSendNum++
       {
        SendFrameHeadForPa(Style, Reason);
        wInfoAddr = MAKEWORD(pReceiveFrame->Frame69.InfoAddr[0],pReceiveFrame->Frame69.InfoAddr[1]);
        wInfoAddr += m_wSendPaNum;      
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = LOBYTE(wInfoAddr);//写信息体地址
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
            m_SendBuf.pBuf[4] |= 0x80;//控制域最高位用来表示还有没后续报文
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
    //if(FileName==9 ||FileName==10 || FileName ==11)//张弢 读汉字站名
    if(FileName==10 || FileName ==11)//张弢 读汉字站名
    {
      wInfoAddr = 0x6902;
      m_wSendPaNum = 0;
    }
    else
    {
      wInfoAddr = MAKEWORD(pReceiveFrame->Frame69.InfoAddr[0],pReceiveFrame->Frame69.InfoAddr[1]);
      wInfoAddr += m_wSendPaNum;
    }
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = LOBYTE(wInfoAddr);//写信息体地址
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = HIBYTE(wInfoAddr);
    
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(FileName);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(FileName);

    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = SecName;
    
    wSecLenPtr = m_SendBuf.wWritePtr++;
    WORD i = 0;
    switch(FileName)
    {
      case 4://读运行参数
        wPaTotalNum = RUN_PARA_NUM-1;
        for(i = 0;i < RUN_PARA_NUM-1;i++,wPaSendNum++)
        {
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_gRunPara[i]);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(g_gRunPara[i]);
        } 
		//张弢 遥测起始地址修改运行参数
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

	   m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_ucPara_stime;   //张弢 0329  主动上传间隔时间
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0;//张弢 0329  主动上传间隔时间
          *///张弢 遥测起始地址修改运行参数
        break;
     case 5://读IEC参数
       wPaTotalNum = IEC_PARA_NUM-1;
       for(i = 0;i < IEC_PARA_NUM-1;i++,wPaSendNum++)
        {
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_ucPara101[i];
        } 
       break;
     case 6://读遥信点表
        wPaTotalNum = g_ucYxTransNum;//IEC_YX_NUM;
        for(i = m_wSendPaNum;(i < g_ucYxTransNum) && (m_SendBuf.wWritePtr < 250);i++,wPaSendNum++)
        {
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(g_ucYXAddr[i]);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(g_ucYXAddr[i]);
        }
        break;
     case 7://读遥测点表
        wPaTotalNum = g_ucYcTransNum;
        for(i = 0;i < g_ucYcTransNum;i++,wPaSendNum++)
        {
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_ucYCAddr[i];
        } 
        break;
      
      case 8://读遥控点表
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
      case 9://读ODU参数//张弢 读汉字站名
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
    case 10://读ODU参数
          /* if(pOdu1 == null) break;
           wPaTotalNum = (((CPrtcOdu *)pOdu1)->pReceiveFrame)->Frame67.Data[6];
           for(i = 0;i < wPaTotalNum;i++,wPaSendNum++)
           {
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] =(((CPrtcOdu *)pOdu1)->pReceiveFrame)->Frame67.Data[7+i];
           }
           *///张弢 目标校准，上位机下载参数 初始值为电压60V,电流2A
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
        		g_gChangFlag[CHNG_ADJ] = ON;  //置校正系数修改标志，校正后需重启
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
        case 0://IP和端口
            for(i = 1;i <= 12;i++)
            {
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_gIPPort[i];
		  
            }
            break;        
        case 1://短信功能
            m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_gSMSR;
	     break;		
        case 2://APN
            for(i = 1;i <= g_gAPN[0];i++)
            {
                m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_gAPN[i];
		  
            }
            break;           
        case 3://用户名
        case 4://密码
        case 0x0f:
            if(pGprs == null) break;
            wPaTotalNum = 8;//特殊处理，只取以下状态量
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = (((CBJ101S *)pGprs)->pReceiveFrame)->Frame68.Data[0] ^ 1;  //模块状态
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = (((CBJ101S *)pGprs)->pReceiveFrame)->Frame68.Data[1] ^ 1;  //SIM卡状态
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = (((CBJ101S *)pGprs)->pReceiveFrame)->Frame68.Data[9];  //CSQ值
            g_unZJTrans[2]=(((CBJ101S *)pGprs)->pReceiveFrame)->Frame68.Data[9];  //CSQ值
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = (((CBJ101S *)pGprs)->pReceiveFrame)->Frame68.Data[10]; //频段
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
       case 15://读终端版本号
           wPaTotalNum = strlen((char *)VerNum);
           for(i = 0;i < wPaTotalNum;i++,wPaSendNum++)
           {
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = VerNum[i];
           }
           #if 1 //for test 运行过程中查看变量值
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
      case 16://读电话号码 限制在一帧报文中
         /* */ //张弢 0328 
	    wPaTotalNum = PHONE_PA_NUM - 1;
           for(i = 0;i < wPaTotalNum;i++,wPaSendNum++)
           {
              m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = g_gSmsPhone[i];
           }
           break;
   case 31://读遥信值
        wPaTotalNum = g_ucYxTransNum;//IEC_YX_NUM;
        BYTE bValCur;
        for(i = m_wSendPaNum;(i < g_ucYxTransNum) && (m_SendBuf.wWritePtr < 250);i++,wPaSendNum++)
        {
          bValCur = GetRIStatus(g_ucYXAddr[i]-1);
          m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = bValCur;
        }
        break;
   case 32://读遥测值
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
      m_SendBuf.pBuf[4] |= 0x80;//控制域最高位用来表示还有没后续报文
    SendFrameTailForPa();
    
    if(FileName != 15)//读终端版本号报文附带运行过程中的实时内存数据查看
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
    m_SendBuf.wWritePtr += 5;//信息体地址2+文件名2+节名1
    switch(wFile)
    {//节长度
      case 4:
        {
      //    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (RUN_PARA_NUM-1)*2;
        }
        break;
      case 5:
        {//读IEC参数状态
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
    m_SendBuf.wWritePtr += 5;//信息体地址2+文件名2+节名1
    wSecLenPtr = m_SendBuf.wWritePtr++;
    
    FLoadAddr = FADDR_LOAD_START + (wCurLoadNum * FLASH_PLOAD_LEN);
    Sst26vf064b_Read(FLoadAddr,byLoadDa, FLASH_PLOAD_LEN);
    if(byLoadDa[0] != 0xEA || wFlag == 1)
      m_SendBuf.pBuf[wSecLenPtr] = 0;//节长度
    else
    {
      memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],&(byLoadDa[2]),5);//5字节时间
      m_SendBuf.wWritePtr += 5;
      BYTE YcIndex = 0;
      memcpy(wLoadMeas,&byLoadDa[7],18);
      for(int i = 0; i < g_ucYcTransNum; i++)   //计算需要上传的遥测值，按照具体情况进行换算
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
//  函数名称   : SendLoadData
//  功能描述   : 发送负荷数据
//  输入参数   : bySec:节名 =0表示读取最近一条 1:读所有负荷数据 2:读取指定条数
//               wStartNo:读取的负荷数据起始序号 min=0 
//               wLoadNum:需要读取的负荷数据条数(暂时未使用)
//               byQds
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       : 
//==============================================================================

void CProtocol::SendLoadData(BYTE bySec,WORD wStartNo,WORD w_LoadNum,BYTE byScq)
{
     //WORD wLoadNum = w_LoadNum;
      WORD wCurLoadNum = 0;
      static unsigned int FLoadInfo[FLOADINFONUM];
      
      //if(byScq == 0)
        CAT_SpiReadWords(EEPADD_LOADNUM, FLOADINFONUM, FLoadInfo);
      if((FLoadInfo[FLOAD_CS] != FLoadInfo[FLOAD_TOTALNUM] + FLoadInfo[FLOAD_NEW] + FLoadInfo[FLOAD_OLD]) || FLoadInfo[FLOAD_TOTALNUM] > FLASH_LOAD_MAXNUM || FLoadInfo[FLOAD_NEW] > FLASH_LOAD_MAXNUM)//如果FLASH地址不在负荷记录保存区域内
      {
          FLoadInfo[FLOAD_TOTALNUM] = 0; //清空负荷记录总条数
          FLoadInfo[FLOAD_NEW] = 0; //即将存储的记录与第一条记录的偏移位置(不是已存储的最新一条记录的位置)
          FLoadInfo[FLOAD_OLD] = 0;//最老一条记录的位置
      }
      //if(bySec == 0 || bySec == 1)
      { 
          if(FLoadInfo[FLOAD_NEW] == 0)
            wCurLoadNum = FLASH_LOAD_MAXNUM -1;
          else
            wCurLoadNum = FLoadInfo[FLOAD_NEW]-1; 
          if(FLoadInfo[FLOAD_TOTALNUM] == 0)//若没有负荷记录则上传一帧没有内容的报文
          {
            SendOneLoadData(wCurLoadNum,1);
            return;
          }
          if(bySec == 0)
          {//读最新1条
            SendOneLoadData(wCurLoadNum); 
          }
          else
          {//从最新记录开始，读取N条
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
//        {//从最老记录开始，读取N条
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
    {//读所有SOE   
      for(int i = 1; i <= soe_num;i=i+16)//张弢 SOE存FLASH     
      {
    WatchDog(); 
    tempx=i+15;
    if(tempx>soe_num)
		tempx=soe_num;
    bySoeNum = ReadSoe(bySoeDa,0,i,tempx);//张弢 SOE存FLASH
   
        if(bySoeNum <= SOE_NUM_SEG/2)
          byCurSoeNum = bySoeNum; 

        SendFrameHeadForPa(Style, Reason);
        memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],&pReceiveFrame->Frame69.InfoAddr[0],5);
       m_SendBuf.wWritePtr += 5;//信息体地址2+文件名2+节名1
        m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = byCurSoeNum*SOE_SENDDA_LEN; 
        
        memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],bySoeDa,byCurSoeNum*SOE_SENDDA_LEN);
        m_SendBuf.wWritePtr += byCurSoeNum*SOE_SENDDA_LEN;
        //bySdSoeNum = byCurSoeNum;
        SendFrameTailForPa();
        
        delayms(1000);//从接口软件中查看报文
	
      }
    }
    else if(bySec == 2)
    {//按段读
        BYTE read_seg = 0;
        BYTE StartNo_seg = 0;
        read_seg = ( read_seg + (wStartNo / SOE_NUM_SEG)) % SOE_SEG;
        StartNo_seg = wStartNo%SOE_NUM_SEG;
        bySoeNum = ReadSoe(bySoeDa,read_seg,StartNo_seg,wSOENum);
        if(bySoeNum <= SOE_NUM_SEG/2)
          byCurSoeNum = bySoeNum; 

        SendFrameHeadForPa(Style, Reason);
        memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],&pReceiveFrame->Frame69.InfoAddr[0],5);
        m_SendBuf.wWritePtr += 5;//信息体地址2+文件名2+节名1
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
    m_SendBuf.wWritePtr += 5;//信息体地址2+文件名2+节名1
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

//组织报文头,固定格式报文
BOOL CProtocol::SendFrameHeadForPa(BYTE Style, BYTE Reason)
{
      m_SendBuf.wReadPtr = 0;
      m_SendBuf.wWritePtr = 0;
      if(pReceiveFrame->Frame69.Start1 == 0x69)
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr]= 0x69;
      else
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr]= 0x68;
      m_SendBuf.wWritePtr += 3;//报文长度，预留，在帧尾时处理
      if(pReceiveFrame->Frame69.Start1 == 0x69)
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x69;
      else
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x68;
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x53; //控制域，设置成固定值 index=4
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = LOBYTE(g_gRunPara[RP_COMM_ADDR]);//g_gRunPara[RP_COMM_ADDR]
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = HIBYTE(g_gRunPara[RP_COMM_ADDR]);//g_gRunPara[RP_COMM_ADDR]
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = Style;
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x01;//可变结构限定词，设置成固定值
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = LOBYTE(Reason);

      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = HIBYTE(Reason);
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = LOBYTE(g_gRunPara[RP_COMM_ADDR]);//g_gRunPara[RP_COMM_ADDR]
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = HIBYTE(g_gRunPara[RP_COMM_ADDR]);//g_gRunPara[RP_COMM_ADDR]

    return TRUE;
}

//组织报文尾，并发送整帧报文
BOOL CProtocol::SendFrameTailForPa(void)
{
    m_SendBuf.pBuf[1] = m_SendBuf.pBuf[2] = m_SendBuf.wWritePtr - 4;//报文长度
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = (BYTE)ChkSum((BYTE *)&m_SendBuf.pBuf[4], m_SendBuf.pBuf[2]);
    m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = 0x16;
    CommWrite((char *)(m_SendBuf.pBuf+m_SendBuf.wReadPtr),m_SendBuf.wWritePtr-m_SendBuf.wReadPtr,0);
    

    return TRUE;
}





