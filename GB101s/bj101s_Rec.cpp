/*------------------------------------------------------------------------
 Module:            bj101s.cpp
 Author:            linxueqin
 Project:           规约平台 录波及文件召唤相关
 State:             V1.0
 Creation Date:     2014-4-24
 Description:       GB101规约--slave
------------------------------------------------------------------------*/

#include "Bj101s.h"
#include "..\Main.h"

#ifdef INCLUDE_BJ101_S

static unsigned int  bR_FileNum = 0;////已经发送的目录的条数

/***************************************************************
    Function：Init_Rec
        规约初始化
    参数：
    返回：TRUE 成功，FALSE 失败
***************************************************************/
void CBJ101S::Init_Rec()
{
  
  m_fixpt_num =0;
  m_lubo_directory =0;
  m_soe_directory =0;
  m_fixpt_directory =0;
  m_ulog_directory =0;
  
  m_comtradeflag =0;
  m_fileprocessing =0;
  m_ackflag =0;
  mRecorder_flag.LIST_flag = OFF;
  mRecorder_flag.xuchuanflag= OFF;
  m_ackRecorder =OFF;
  mSendCFGNum =0;
  mSendDATNum =0;
  m_wavefix_total =0;
  m_SendFixNum =0;
  m_current_section =0;
  m_Fixpt_TOTAL_Leng =0;
  m_Fixpt_SECTION_Leng=0;
  m_houxu_flag = OFF;
  mwavelog_total =0;
  mLog_TOTAL_Leng =0;
  m_logcurrent_section =0;
  m_loghouxu_flag = OFF;
  m_tra_FI = OFF;
}
//读文件入口函数
BOOL CBJ101S::Recfileprocessing(unsigned char *pRxBuf)
{ 
    if(m_uartId == g_CmIdDBG)//if(pDbg != null)
    {
      CAT_SpiWriteBytes(EEPADD_DEGBF101, 6+pRxBuf[1], pRxBuf);
    }
    else if(m_uartId == g_CmIdGPRS)//if(pGprs != null)
    {
      CAT_SpiWriteBytes(EEPADD_GPRSBF101, 6+pRxBuf[1], pRxBuf);
    }
  //memcpy(mRecorder_flag.pRXBuff,pRxBuf,6+pRxBuf[1]);
  BYTE *pData =&pRxBuf[5+m_byInfoShift];
  WORD InfoAddr = MAKEWORD(pData[0],pData[1]);
    //pData=&pReceiveFrame->Frame68.Data[m_dwasdu.Infooff+m_guiyuepara.infoaddlen];
  BYTE Packet_typep =pData[2];//附加数据包类型
  BYTE Operational_logo = pData[3];//操作标识
  //BYTE directory_name_length;
  BYTE file_name_length;
  BYTE dir_name_length;
  DWORD Directory_ID;//目录ID
  WORD Directory_IDl,Directory_IDh;//目录ID
  DWORD start_date_time =0;
  DWORD end_date_time=0;
  DWORD start_minute_time=0;
  DWORD end_minute_time=0;
  WORD  MSecond=0;
  BYTE  call_sign;
  BYTE flag = OFF;
  //BYTE Call_sign;//召唤标志
  //char flag[4];
  BYTE time[20];
  if(Packet_typep==2)  //在信息体地址后面，附加数据包类型1个字节，备用1,3,4.文件传输 2
    {
      switch(Operational_logo)//附加数据包的第一个字节为操作标识
      {
          case OPERATING_LOGO1:
            if(g_bAppType)
            {
                SendEncFrameAck(0x0191, 0x0000, 0x1f);
                return TRUE;
            }
      Directory_IDl = MAKEWORD(pData[4],pData[5]);
      Directory_IDh = MAKEWORD(pData[6],pData[7]);
      
        Directory_ID= MAKEDWORD(Directory_IDl,Directory_IDh);
      mRecorder_flag.xuchuanflag = OFF;
      dir_name_length = pData[8];
      call_sign = pData[dir_name_length + 9];
      if(call_sign ==1)
      {
        MSecond = MAKEWORD(pData[dir_name_length +10], pData[dir_name_length +11]);
        start_minute_time = (MSecond/1000);
        start_minute_time+=((DWORD)pData[dir_name_length +12]<<8);
        start_minute_time+=((DWORD)pData[dir_name_length +13]<<16);
        
        start_date_time = (pData[dir_name_length +14] & 0x1F);
        start_date_time +=(((DWORD)pData[dir_name_length +15] & 0x0F)<<8);
        start_date_time +=(DWORD)(((DWORD)pData[dir_name_length +16] & 0x7F)<<16);//YEAR
        MSecond = MAKEWORD(pData[dir_name_length +17], pData[dir_name_length +18]);
        end_minute_time = (MSecond/1000);
        end_minute_time+=((DWORD)pData[dir_name_length +19]<<8);
        end_minute_time+=(DWORD)pData[dir_name_length +20]<<16;
        end_date_time = (pData[dir_name_length +21] & 0x1F);
        end_date_time+=(((DWORD)pData[dir_name_length +22] & 0x0F)<<8);
        end_date_time+=(((DWORD)pData[dir_name_length +23] & 0x7F)<<16);//YEAR
    
      }
      if(pData[9]=='C' && pData[10]=='O' && pData[11]=='M' && pData[12]=='T'&& pData[13]=='R')
        
        lubo_directory_confirm(InfoAddr,Directory_ID,call_sign,start_minute_time,start_date_time,end_minute_time,end_date_time);
      else if(pData[17]=='F' && pData[18]=='I' && pData[19]=='X' && pData[20]=='P'&& pData[21]=='T')  
        fixpt_directory_confirm(InfoAddr,Directory_ID,call_sign,start_minute_time,start_date_time,end_minute_time,end_date_time);
      else if(pData[17]=='S' && pData[18]=='O' && pData[19]=='E') 
        soe_directory_confirm(InfoAddr,Directory_ID);
      else if(pData[17]=='U' && pData[18]=='L' && pData[19]=='O'&& pData[20]=='G')  
        ulog_directory_confirm(InfoAddr,Directory_ID);
      /*if(Directory_ID == 1)
      {
        //m_SendListNum =0;
        lubo_directory_confirm(InfoAddr,Directory_ID,call_sign,start_minute_time,start_date_time,end_minute_time,end_date_time);
      }
        else if(Directory_ID == 2)
          fixpt_directory_confirm(InfoAddr,Directory_ID,call_sign,start_minute_time,start_date_time,end_minute_time,end_date_time); //m_soe_directory = 1;soe_directory_confirm();
        else if(Directory_ID == 3)
          soe_directory_confirm(InfoAddr,Directory_ID);//fixpt_directory_confirm();
        else if(Directory_ID == 4)
          ulog_directory_confirm(InfoAddr,Directory_ID);//ulog_directory_confirm();*/
        break;
      case OPERATING_LOGO3://读目录
         if(g_bAppType)
            {
                SendEncFrameAck(0x0191, 0x0000, 0x1f);
                return TRUE;
            }
        file_name_length = pData[4];
        
        if((mRecorder_flag.LIST_flag == OFF) &&(mRecorder_flag.xuchuanflag!= ON))
        {
          if(pData[5]=='B' && pData[6]=='A' && pData[7]=='Y' && pData[8]=='0'&& pData[9]=='1')
          {
            //Directory_IDl =  MAKEWORD((pData[11]-0x30),(pData[12]-0x30));
            //Directory_IDh = MAKEWORD((pData[13]-0x30),(pData[14]-0x30));
                     // Directory_ID= MAKEDWORD(Directory_IDl,Directory_IDh);
                                               /*( if((pData[11] >=0x30) &&(pData[12] >=0x30)&&(pData[13] >=0x30)&&(pData[14] >=0x30))
                                                {
                                                  flag[0] = (pData[11]-0x30);
                                                  flag[1] = (pData[12]-0x30);
                                                  flag[2] = (pData[13]-0x30);
                                                  flag[3] = (pData[14]-0x30);
                                                }*/
                                                //sscanf(flag,"%d",&Directory_ID);
            Directory_ID = ((pData[11]-0x30)*1000)+((pData[12]-0x30)*100)+((pData[13]-0x30)*10)+(pData[14]-0x30);
                                                mfile_ID = Directory_ID;
            
            if(((pData[4+file_name_length-2]=='C')||(pData[4+file_name_length-2]=='c')) &&((pData[4+file_name_length-1]=='F')||(pData[4+file_name_length-1]=='f'))&&((pData[4+file_name_length]=='G')||(pData[4+file_name_length]=='g')))
            {
              Send_ReadFile_Confirm((WORD)Directory_ID,1,InfoAddr,&pData[5],file_name_length);
              mfile_flag = 1;//CFG
            }
            else if(((pData[4+file_name_length-2]=='D')||(pData[4+file_name_length-2]=='d')) &&((pData[4+file_name_length-1]=='A')||(pData[4+file_name_length-1]=='a'))&&((pData[4+file_name_length]=='T')||(pData[4+file_name_length]=='t')))
            {
              Send_ReadFile_Confirm((WORD)Directory_ID,2,InfoAddr,&pData[5],file_name_length);
              mfile_flag = 2;//DAT
            }
          }
          else if(pData[5]=='f' && pData[6]=='i' && pData[7]=='x' && pData[8]=='p'&& pData[9]=='t')
          {
            //time[0]=(pData[10]-0x30)*1000+(pData[11]-0x30)*100+(pData[12]-0x30)*10+(pData[13]-0x30)-2000;
            //time[1]=(pData[14]-0x30)*10+(pData[15]-0x30);
            //time[3]=(pData[16]-0x30)*10+(pData[17]-0x30);
              for(BYTE i =0;i<31;i++)
            { 
              CAT_SpiReadBytes(EEPADD_FIXNUM+i*20,20,time);
              if((time[0] ==pData[10]) &&(time[1] ==pData[11])&&(time[2] ==pData[12])&&(time[3] ==pData[13])&&(time[4] ==pData[14])&&(time[5] ==pData[15])&&(time[6] ==pData[16])&&(time[7] ==pData[17]))
                            {
                Directory_ID = ((time[9]-0x30)*10)+((time[10]-0x30));
                flag = ON;
                                break;
                            }
                                                        
            }
            if(flag == OFF)//没找到时间一致的定点记录
              return FALSE;
            mfile_ID = Directory_ID;
            Send_ReadFile_Confirm((WORD)Directory_ID,3,InfoAddr,&pData[5],file_name_length);
            mfile_flag = 3;//FIXPT
          }
          else if(pData[5]=='s' && pData[6]=='o' && pData[7]=='e' )
          {
            Directory_ID =0;
            mfile_ID = Directory_ID;
            Send_ReadFile_Confirm((WORD)Directory_ID,4,InfoAddr,&pData[5],file_name_length);
            mfile_flag = 4;//FIXPT
          }
          else if(pData[5]=='u' && pData[6]=='l' && pData[7]=='o'&& pData[8]=='g')
          {
            Directory_ID =0;
                                                mfile_ID = Directory_ID;
            Send_ReadFile_Confirm((WORD)Directory_ID,5,InfoAddr,&pData[5],file_name_length);
            mfile_flag = 5;//FIXPT
          }
          mRecorder_flag.LIST_flag = ON;
          mRecorder_flag.xuchuanflag= OFF;
                    m_ackRecorder = OFF;
        }
        else if((mRecorder_flag.LIST_flag == ON)||(mRecorder_flag.xuchuanflag== ON))
        {
          
          //if(pData[5]="B" && pData[6]="A" && pData[7]="Y" && pData[8]="0"&& pData[9]="1")
          //{
            if(mfile_flag == 1)//(((pData[4+file_name_length-2]="C")||(pData[4+file_name_length-2]="c")) &&((pData[4+file_name_length-1]="F")||(pData[4+file_name_length-1]="f"))&&((pData[4+file_name_length]="G")||(pData[4+file_name_length]="g")))
            {
              if(mRecorder_flag.LIST_flag == ON)
                Send_ReadFile_Data((WORD)mfile_ID,1,InfoAddr,1);
              else if(mRecorder_flag.xuchuanflag== ON)
                Send_ReadFile_Data((WORD)mfile_ID,1,InfoAddr,0x55);
            }
            else if(mfile_flag == 2)//(((pData[4+file_name_length-2]="D")||(pData[4+file_name_length-2]="d")) &&((pData[4+file_name_length-1]="A")||(pData[4+file_name_length-1]="a"))&&((pData[4+file_name_length]="T")||(pData[4+file_name_length]="t")))
            {
              if(mRecorder_flag.LIST_flag == ON)
                Send_ReadFile_Data((WORD)mfile_ID,2,InfoAddr,1);
              else if(mRecorder_flag.xuchuanflag== ON)
                Send_ReadFile_Data((WORD)mfile_ID,2,InfoAddr,0x55);
              if(mluboYX_flag == ON)
              {
                mluboYX_flag =OFF;
                g_gRmtLockLB =0;
                //CreatNewSoe(RI_LUBO_FLAG,0);
              }
              
                                //mfile_flag = 2;//DAT
            }
            else if(mfile_flag == 3)
                                                {
              if(mRecorder_flag.LIST_flag == ON)
                Send_ReadFile_Data((WORD)mfile_ID,3,InfoAddr,1);
              else if(mRecorder_flag.xuchuanflag== ON)
                Send_ReadFile_Data((WORD)mfile_ID,3,InfoAddr,0x55);
                                                }
            else if(mfile_flag == 4)
                                                {
              if(mRecorder_flag.LIST_flag == ON)
                Send_ReadFile_Data((WORD)mfile_ID,4,InfoAddr,1);
              else if(mRecorder_flag.xuchuanflag== ON)
                Send_ReadFile_Data((WORD)mfile_ID,4,InfoAddr,0x55);
                                                }
            else if(mfile_flag == 5)
            {
              if(mRecorder_flag.LIST_flag == ON)
                Send_ReadFile_Data((WORD)mfile_ID,5,InfoAddr,1);
              else if(mRecorder_flag.xuchuanflag== ON)
                Send_ReadFile_Data((WORD)mfile_ID,5,InfoAddr,0x55);
            }
            mRecorder_flag.LIST_flag = OFF;
            //mRecorder_flag.xuchuanflag= ON;
          //}
        }
        
        break;
      case OPERATING_LOGO6:
        /*if(mfile_flag ==1)
          Send_ReadFile_Data((WORD)mfile_ID,1,InfoAddr,0x55);
        else if(mfile_flag == 2)
          Send_ReadFile_Data((WORD)mfile_ID,2,InfoAddr,0x55);
        else if(mfile_flag == 3)      
          Send_ReadFile_Data((WORD)mfile_ID,3,InfoAddr,0x55);
        else if(mfile_flag == 4)
              
          Send_ReadFile_Data((WORD)mfile_ID,4,InfoAddr,0x55);
        else if(mfile_flag == 5)
              
          Send_ReadFile_Data((WORD)mfile_ID,5,InfoAddr,0x55);*/
        break;
      default:
            
              break;
        
        
      }
  }
   
  return TRUE;
}
#ifndef YN_101S
BOOL CBJ101S:: RecYSCommand(void)
{/*
  BYTE bVsq = m_dwasdu.VSQ;
  WORD wval =0;
  WORD InfoAddr = MAKEWORD(pReceiveFrame->Frame68.Data[m_byInfoShift],pReceiveFrame->Frame68.Data[m_byInfoShift+1]);
  BYTE *pData = &pReceiveFrame->Frame68.Data[m_byInfoShift + m_guiyuepara.infoaddlen];
  BYTE bQos = (bVsq & 0x7F) *2;//参数值后面一个字节表示是预制命令还是激活命令
  g_RWFiInf.Fi_Info_Num = bQos;
  g_RWFiInf.Fi_Info = InfoAddr;
  WORD wInfoAddr = 0;
  if(pData[bQos] == 0)//选择预置命令
  {
    for( BYTE i =0; i<bQos;i+=2 )
    {
      wInfoAddr = InfoAddr+(i>>1);
      g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2] = pData[i];
      g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2+1] = pData[i+1];
    }
    if(bQos <=2)
      RecWriteData(48,pData[bQos],0);
    else
      RecWriteData(136,pData[bQos],0x80);
  }
  else//激活
  {
      g_RWFiInf.Fi_wflag = OFF;  
      for( BYTE i =0; i<bQos;i+=2 )
      {
        wInfoAddr = InfoAddr+(i>>1);
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2] = pData[i];
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2+1] = pData[i+1];
        if(wInfoAddr == 0x5002)
        {
          wval = MAKEWORD(pData[i],pData[i+1]);
          if(wval != g_gRunPara[RP_ALARM_BAT])
          {
            g_gRunPara[RP_ALARM_BAT] = wval;
            g_ucParaChang |=BIT0;
            SetEqpInfo();
          }
        }
        else if(wInfoAddr == 0x5003)
        {
          wval = MAKEWORD(pData[i],pData[i+1]);
          if(wval != g_gRunPara[RP_LBSTORAGE_TIME])
          {
            g_gRunPara[RP_LBSTORAGE_TIME] = wval;
            g_ucParaChang |=BIT0;
            SetEqpInfo();
          }
        }
        else if(wInfoAddr == 0x5005)
        {
          wval = MAKEWORD(pData[i],pData[i+1]);
          if(wval != g_gRunPara[RP_LUBO_NUM])
          {
            g_gRunPara[RP_LUBO_NUM] = wval;
            g_ucParaChang |=BIT0;
            SetEqpInfo();
          }
        }
        else if(wInfoAddr == 0x5006)
        {
          wval = MAKEWORD(pData[i],pData[i+1]);
          if(wval != g_gRunPara[RP_SENDYC_T])
          {
            g_gRunPara[RP_SENDYC_T] = wval;
            g_ucParaChang |=BIT0;
            SetEqpInfo();
          }
        }
        else if(wInfoAddr == 0x500C)
        {
          wval = MAKEWORD(pData[i],pData[i+1]);
          if(wval != g_gRunPara[RP_LUBO_CYCLE])
          {
            g_gRunPara[RP_LUBO_CYCLE] = wval;
            g_ucParaChang |=BIT0;
            SetEqpInfo();
          }
        }
        else if((wInfoAddr == 0x5008)||(wInfoAddr == 0x5009)||(wInfoAddr == 0x500B)||(wInfoAddr == 0x500D)||(wInfoAddr == 0x500E)||(wInfoAddr == 0x500F)||(wInfoAddr == 0x5011)||(wInfoAddr == 0x5014))
        {

            if(m_uartId == g_CmIdGPRS)
            {
              g_ZsqCmdRBit |= BIT[10];
            }
            else if(m_uartId == g_CmIdDBG)
            {
              g_ZsqCmdRBit |= BIT[11];    
            }  
            g_RWFiInf.Fi_wflag = ON;
            g_RWFiInf.FiphaseA_flag = ON;
            g_RWFiInf.FiphaseB_flag = OFF;
            g_RWFiInf.FiphaseC_flag = OFF;
        }
      }
      if(g_RWFiInf.Fi_wflag == OFF)
      {
          if(g_RWFiInf.Fi_Info_Num <=2)
            RecWriteData(48,1,0);
          else  
            RecWriteData(136,1,0x80);
      }
  }*/
  return TRUE;
}
#endif

void CBJ101S:: ReadFileDataUlog(WORD FileName,BYTE section_current,BYTE flag)
{
  unsigned char *pTxPos;
  char *pdat_name;
  pdat_name="AU";
  //unsigned int object_addr_length =2;
  char ch[30]={0};
  char ch_1[5]={0};
  BYTE byLoadDa[128] = {0};
  
  //BYTE houxu_flag = OFF;
  BYTE logsum = 0;
  //unsigned char type = M_SP_TB;
  WORD log_leng =0;
  m_loghouxu_flag = OFF;
  BYTE n,i;
  unsigned long FLoadAddr; //= FADDR_LOG_START + ((unsigned long)(section_current-1) *FLASH_PLOG_LEN);
  pTxPos = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;  //数据段号
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //后续标志
  if(mlog_recorded.log_count_flag  == OFF)//(!soe_recorded.Soe_Area )&&(!soe_recorded.soe_count)&&(!soe_recorded.Soe_Ptr)
  {
    sprintf((char *)ch,"%.*s,%02x%02x\r\n",sizeof(pdat_name),pdat_name,mwavelog_total,(mwavelog_total >>8));
    for(  n = 0; n < strlen(ch); n++)
    {
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch[n];
      logsum += ch[n];
    }
          //CAT_SpiReadBytes(EEPADD_DAYNUM+FileName,1,&(fixpt_recorded.fixpt_count));
    if(mwavelog_total >0)
      mlog_recorded.log_Curren_count =1;//若有节数据数据，则对应的段至少为1，下面发送时再计算具体的段数
    mlog_recorded.log_Ptr =0;
    mlog_recorded.log_count_flag = ON;
  }
  else if(mlog_recorded.log_count_flag == ON)
  {
       FLoadAddr = FADDR_LOG_START + ((unsigned long)(section_current-1) *FLASH_PLOG_LEN);
      Sst26vf064b_Read(FLoadAddr,byLoadDa,FLASH_PLOAD_LEN);
            //memcpy(wLoadMeas,&byLoadDa[8],FLASH_PLOAD_LEN-8);
          
      if(mlog_recorded.log_Ptr < mlog_recorded.log_Curren_count)
      {
            
              
        sprintf((char *)ch,"%02x%02x,%02d%02d%02d_%02d%02d%02d,",byLoadDa[4],byLoadDa[3],byLoadDa[5],byLoadDa[6],byLoadDa[7],byLoadDa[8],byLoadDa[9],byLoadDa[10]);
        for(n = 0; n < strlen(ch); n++)
        {
            
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch[n];
          logsum+= ch[n];
        }
              //memcpy(pTxBuf,&byLoadDa[10],byLoadDa[1] -8);//leng
              //pTxBuf += (byLoadDa[1] -8);
        for(n = 0; n < byLoadDa[1] -9; n++)
        {
            
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =byLoadDa[11+n];
          logsum+= byLoadDa[11+n];
        }
        log_leng = strlen(ch) + (byLoadDa[1] -9);
        sprintf((char *)ch_1,",%02d",byLoadDa[2]);
        for(n = 0; n < strlen(ch_1); n++)
        {
            
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch_1[n];
          logsum+= ch_1[n];
        }
        log_leng = strlen(ch) + (byLoadDa[1] -9)+ strlen(ch_1);
        if(log_leng <126)
        {
          for(i=log_leng; i< 126; i++)//n < strlen(ch),
          {
            
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0x20;
            logsum+= 0x20;
                
          }
        }
        sprintf((char *)ch_1,"\r\n");
        for(n = 0; n < strlen(ch_1); n++)
        {
            
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch_1[n];
          logsum+= ch_1[n];
        }
        mlog_recorded.log_Ptr++;
        if((mlog_recorded.log_Ptr== mlog_recorded.log_Curren_count)&&(flag ==0x55))
          m_loghouxu_flag = ON;
    }
    
          
  }
   *pTxPos = section_current;//pTxBuf - pTxPos-1;//LL
          *(pTxPos +1)= section_current >>8;
   *(pTxPos +2)= mlog_recorded.log_Curren_count;
   *(pTxPos +3)= mlog_recorded.log_Ptr ;//>>24
   if(m_loghouxu_flag == OFF)
   {
    *(pTxPos +4)= 1;//有后续
    mRecorder_flag.xuchuanflag= ON;
   }
   else
   {
    *(pTxPos +4)= 0;//有后续
    mRecorder_flag.xuchuanflag= OFF;
   }
   m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = logsum;      
  
}

void CBJ101S::ReadFileDataSoe_Flash(WORD FileName)
{
/*
  unsigned char *pTxPos;
  char *pdat_name;
  pdat_name="AU";
  unsigned int object_addr_length =2;
  BYTE n;
  BYTE soesum =0;
  char ch[60]={0};
  BYTE bySoeDa[100]={0};
  BYTE byCurSoeNum = 0;
  WORD second;
    WORD Millisecond,i;
  BYTE houxu_flag = OFF;
  //WORD Num_total =0;
  unsigned char type = M_SP_TB;
  pTxPos = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;  //数据段号
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //后续标志
  BYTE soe_num_max =4;//FSOE_NUM_MAX
  if(msoe_recorded.Soe_count_flag == OFF)//(!soe_recorded.Soe_Area )&&(!soe_recorded.soe_count)&&(!soe_recorded.Soe_Ptr)
        {
      sprintf((char *)ch,"%.*s,%04d,%02d\r\n",sizeof(pdat_name),pdat_name,soe_num,object_addr_length);
      soesum =0;
      for(n = 0; n < strlen(ch); n++)
                  {
                    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch[n];
                soesum += ch[n];
                  }
      msoe_recorded.Soe_count_flag = ON;
      m_BkFlashhead =g_InFlashInfo.m_unSSoeInFlashHead;

        
  }
  else if(msoe_recorded.Soe_count_flag == ON)
  {
    
            byCurSoeNum =  ReadSoeFromFlash((WORD *)&m_BkFlashhead,g_unSSoeInFlashTail,soe_num_max,bySoeDa);
        
      msoe_recorded.Soe_Ptr = 0;
      for(i =0 ;i< byCurSoeNum;i++)
            {
        //if(bySoeDa[(i)*SOE_DATA_LEN] == SOEVALID )//SOE存储有效
        {
          second = (MAKEWORD(bySoeDa[(i)*SOE_DATA_LEN+4],bySoeDa[(i)*SOE_DATA_LEN+5]))/1000;//+soe_recorded.Soe_Ptr
                  Millisecond = (MAKEWORD(bySoeDa[(i)*SOE_DATA_LEN+4],bySoeDa[(i)*SOE_DATA_LEN+5]))%1000;//+soe_recorded.Soe_Ptr
                  sprintf((char *)ch,"%d %02x %02x %d %02d%02d%02d_%02d%02d%02d_%03d\r\n",type,MAKEWORD(bySoeDa[(i)*SOE_DATA_LEN+1],bySoeDa[(i)*SOE_DATA_LEN+2])+1,(MAKEWORD(bySoeDa[(i)*SOE_DATA_LEN+1],bySoeDa[(i)*SOE_DATA_LEN+2])+1)>>8,bySoeDa[(i)*SOE_DATA_LEN+3],bySoeDa[(i)*SOE_DATA_LEN+10], bySoeDa[(i)*SOE_DATA_LEN+9], bySoeDa[(i)*SOE_DATA_LEN+8], bySoeDa[(i)*SOE_DATA_LEN+7],bySoeDa[(i)*SOE_DATA_LEN+6],
                  second,Millisecond);
                
                  for(n = 0; n < strlen(ch); n++)
                  {
                
              m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch[n];
              soesum += ch[n];
                  }
        }
            }
      msoe_recorded.soe_count += byCurSoeNum;
      //if (byCurSoeNum == 0)
            //houxu_flag = ON;//pTxBuf = NULL;  
          if(msoe_recorded.soe_count >= soe_num)
        houxu_flag = ON;
          //msoe_recorded.Soe_Ptr += byCurSoeNum;
          //msoe_recorded.soe_count +=byCurSoeNum;
      //if(msoe_recorded.soe_count >= soe_num)
        //houxu_flag = ON;
              // *pTxPs = pTxBuf - pTxPs-1;
              //if(soe_recorded.Soe_Curren_count ==0)
              //pTxBuf = NULL;      
  }
  *pTxPos = msoe_recorded.soe_count;
  *(pTxPos+1)= msoe_recorded.soe_count>>8;
  *(pTxPos+2)= 0;//msoe_recorded.soe_count>>16;
  *(pTxPos+3)= 0;//msoe_recorded.soe_count>>24;
  *(pTxPos+4)= 1;//有后续
  mRecorder_flag.xuchuanflag= ON;
     if(houxu_flag ==ON)//最后一段
      {
          *(pTxPos+4)= 0;//无后续
          mRecorder_flag.xuchuanflag= OFF;
      }
   m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = soesum;
 */ 
}
void CBJ101S:: ReadFileDataSoe(WORD FileName)
{

  unsigned char *pTxPos;
  char *pdat_name;
  pdat_name="EFS";
  unsigned int object_addr_length =2;
  BYTE n;
  BYTE soesum =0;
  char ch[60]={0};
  BYTE byCurSoeNum = 0;
  WORD second;
    WORD Millisecond,i;
  BYTE houxu_flag = OFF;
  BYTE bySoeDa[64];
  unsigned char type = M_SP_TB;
  pTxPos = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;  //数据段号
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //后续标志
  if(msoe_recorded.Soe_count_flag == OFF)//(!soe_recorded.Soe_Area )&&(!soe_recorded.soe_count)&&(!soe_recorded.Soe_Ptr)
 	{//第一行
   	sprintf((char *)ch,"%.*s,%04d,%02d\r\n",sizeof(pdat_name),pdat_name,soe_num,object_addr_length);
   	soesum =0;
   	for(n = 0; n < strlen(ch); n++)
   		{
      	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch[n];
       	soesum += ch[n];
       	}
   	msoe_recorded.Soe_count_flag = ON;
  	}
  else if(msoe_recorded.Soe_count_flag == ON)
  	{
  	for(n = 0; n < SOE_NUM_SEG; n++)
   		{     	 
      	msoe_recorded.Soe_Area &= 0x3ff0;
		if(msoe_recorded.Soe_Area>=EEPADD_SOEENDADR) 
			msoe_recorded.Soe_Area = EEPADD_SOESTARTADR;	 
		CAT_SpiReadBytes(msoe_recorded.Soe_Area, SOE_SENDDA_LEN, &bySoeDa[n*SOE_SENDDA_LEN]); 
		msoe_recorded.Soe_Area += 16;
		if(msoe_recorded.Soe_Area>=EEPADD_SOEENDADR) 
			msoe_recorded.Soe_Area = EEPADD_SOESTARTADR;
		if(msoe_recorded.Soe_Area==msoe_recorded.Soe_Ptr)
			{
			msoe_recorded.Soe_Area=EEPADD_SOESTARTADR;
			break;
			}
  		}
	if(n>=SOE_NUM_SEG)
		{byCurSoeNum=SOE_NUM_SEG;}
	else
		{byCurSoeNum=n+1;}	
         
	for(i =0 ;i< byCurSoeNum;i++)
   		{
       	second = (MAKEWORD(bySoeDa[(i)*SOE_SENDDA_LEN+4],bySoeDa[(i)*SOE_SENDDA_LEN+5]))/1000;//+soe_recorded.Soe_Ptr
        Millisecond = (MAKEWORD(bySoeDa[(i)*SOE_SENDDA_LEN+4],bySoeDa[(i)*SOE_SENDDA_LEN+5]))%1000;//+soe_recorded.Soe_Ptr
		sprintf((char *)ch,"%d %02x %02x %d %02d%02d%02d_%02d%02d%02d_%03d\r\n",type,
				(MAKEWORD(bySoeDa[(i)*SOE_SENDDA_LEN+1],bySoeDa[(i)*SOE_SENDDA_LEN+2])+1),
			 	(MAKEWORD(bySoeDa[(i)*SOE_SENDDA_LEN+1],bySoeDa[(i)*SOE_SENDDA_LEN+2])+1)>>8,
			 	bySoeDa[(i)*SOE_SENDDA_LEN+3],//yx value
			 	bySoeDa[(i)*SOE_SENDDA_LEN+10],bySoeDa[(i)*SOE_SENDDA_LEN+9], 
			 	bySoeDa[(i)*SOE_SENDDA_LEN+8], bySoeDa[(i)*SOE_SENDDA_LEN+7], 
			 	bySoeDa[(i)*SOE_SENDDA_LEN+6],second,Millisecond);

	    for(n = 0; n < strlen(ch); n++)
      		{              
         	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch[n];
         	soesum += ch[n];
           	}
       	}
   	msoe_recorded.Soe_Ptr += byCurSoeNum;
  	msoe_recorded.Soe_Curren_count +=byCurSoeNum;
  	if(msoe_recorded.Soe_Curren_count >= msoe_recorded.soe_count)
        houxu_flag = ON;
              //*pTxPs = pTxBuf - pTxPs-1;
              //if(soe_recorded.Soe_Curren_count ==0)
              //pTxBuf = NULL;      
  }
  *pTxPos = msoe_recorded.Soe_Curren_count;
  *(pTxPos+1)= msoe_recorded.Soe_Curren_count>>8;
  *(pTxPos+2)= 0;//msoe_recorded.soe_count>>16;
  *(pTxPos+3)= 0;//msoe_recorded.soe_count>>24;
  *(pTxPos+4)= 1;//有后续
  mRecorder_flag.xuchuanflag= ON;
     if(houxu_flag ==ON)//最后一段
      {
          *(pTxPos+4)= 0;//无后续
          mRecorder_flag.xuchuanflag= OFF;
      }
   m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = soesum;  
}

/*void CBJ101S::ReadFileDataExv(WORD FileName,BYTE section_current,BYTE flag)
{
  unsigned char *pTxPos;
  unsigned long FLoadAddrh;
  WORD n,i;
  BYTE infoaddrl,infoaddrh;
  unsigned int offset =0;
  char *pdat_name;
  pdat_name="AU";
  //BYTE count;
  BYTE YcIndex = 0;
  BYTE FixPt_Sum = 0;
  BYTE YC_Type =9;//归一化? 
  char ch[202]={0};
  char ch_1[20]={0};
  BYTE byLoadDa[256] = {0};
  unsigned int object_addr_length =2;
  BYTE fix_YC_Num=0;
  WORD byFlashDaLen = FLASH_PLOG_LEN;//flash中读取的数据长度
  
  m_houxu_flag =OFF;
  unsigned long FLoadAddr = FADDR_EXV_START + ((unsigned long)FileName * byFlashDaLen);//+ ((unsigned long)(section_current) *FLASH_PLOAD_LEN);//-1
  pTxPos = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;  //数据段号
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //后续标志
  if(mfixpt_recorded.fixpt_count_flag == OFF)
  {
    FLoadAddrh = FADDR_EXV_START + ((unsigned long)FileName * byFlashDaLen);
    Sst26vf064b_Read(FLoadAddrh,byLoadDa,byFlashDaLen);
    sprintf((char *)ch,"%.*s,%02d%02d%02d,%02d,%02d\r\n",sizeof(pdat_name),pdat_name,byLoadDa[2],byLoadDa[3],byLoadDa[4],object_addr_length,YC_Type);
    FixPt_Sum =0;
    for(n = 0; n < strlen(ch); n++)
    {
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch[n];
      FixPt_Sum += ch[n];
    }
    //CAT_SpiReadBytes(EEPADD_DAYNUM+FileName,1,&count);//(mfixpt_recorded.fixpt_count)
    //if(count >0)
      mfixpt_recorded.fixpt_Curren_count = 1;//若有节数据数据，则对应的段至少为1，下面发送时再计算具体的段数
    mfixpt_recorded.fixpt_Ptr =0;
    mfixpt_recorded.fixpt_count_flag = ON;
    fix_YC_Num =0;
  }
  else if(mfixpt_recorded.fixpt_count_flag == ON)
  {
      Sst26vf064b_Read(FLoadAddr,byLoadDa,byFlashDaLen);   
      for( i = mfixpt_recorded.fixpt_count; i < g_ucYcTransNum; i++)   //计算需要上传的遥测值，按照具体情况进行换算
      {
        if(g_ucYCAddr[i]!=255)
        {
            YcIndex = g_ucYCAddr[i] - 1;
            if(YcIndex <RM_EXV_END)
            {         
                infoaddrl=(i+ADDR_YC_LO)&0xff;
                infoaddrh=((i+ADDR_YC_LO)>>8)&0xff;
                offset +=sprintf(ch + offset,"%02x%02x %02x%02x,",infoaddrl,infoaddrh,byLoadDa[8+YcIndex*2],byLoadDa[9+YcIndex*2]);
            }
            if (strlen(ch) >= 200)
            {
              mfixpt_recorded.fixpt_count ++;
              break;
            }
        }
        mfixpt_recorded.fixpt_count ++;
     }
     if(mfixpt_recorded.fixpt_count >= g_ucYcTransNum )//每一节后加换行
     {
        ch[offset -1]='\r';
        ch[offset]='\n';
     }
     if(mfixpt_recorded.fixpt_Ptr ==0)
     {
        fix_YC_Num =0;
        for( i = 0; i < g_ucYcTransNum; i++)   //计算需要上传的遥测值，按照具体情况进行换算
        {
              if(g_ucYCAddr[i]!=255)
            {
                YcIndex = g_ucYCAddr[i] - 1;
                  if(YcIndex <RM_EXV_END )
                    fix_YC_Num++; 
        
            }
        }
        mfixpt_recorded.fixpt_Curren_count = (fix_YC_Num *10 + 1)/200 +1 ;//每个遥信占用字节数为10 (0140 0000，),加1的原因是最
        sprintf((char *)ch_1,"%02d,%02d%02d%02d_%02d%02d%02d,",fix_YC_Num,byLoadDa[2],byLoadDa[3],byLoadDa[4],byLoadDa[5],byLoadDa[6],byLoadDa[7]);
        for(n = 0; n < strlen(ch_1); n++)
        {
           m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = ch_1[n];
           FixPt_Sum += ch_1[n];
        }
    }
        
    if(mfixpt_recorded.fixpt_Ptr < mfixpt_recorded.fixpt_Curren_count)
    {
        for(n =0;n<strlen(ch);n++)
        {
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch[n];
            FixPt_Sum += ch[n];        
        }
        if(((mfixpt_recorded.fixpt_Ptr +1)== mfixpt_recorded.fixpt_Curren_count)&&(flag ==0x55))
          m_houxu_flag = ON;
        mfixpt_recorded.fixpt_Ptr++;
    }
  }
   *pTxPos = section_current;
   *(pTxPos +1)= section_current >>8;
   *(pTxPos +2)= mfixpt_recorded.fixpt_Curren_count;
   *(pTxPos +3)= mfixpt_recorded.fixpt_Ptr;
   if(m_houxu_flag == OFF)
   {
      *(pTxPos +4)= 1;//有后续
      mRecorder_flag.xuchuanflag= ON;
   }
   else
   {
      *(pTxPos +4)= 0;//有后续
      mRecorder_flag.xuchuanflag= OFF;
   }
   m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = FixPt_Sum;
}*/

void CBJ101S::ReadFileDataFixpt(WORD FileName,BYTE section_current,BYTE flag)
{
  unsigned char *pTxPos;
  //BYTE LuboDat_Sum = 0;
    //int segment_leng = SEGMENT_LENGTH_NEW;
  //unsigned int  DAT_NUM;
  //BYTE Current_Section;
  unsigned long FLoadAddrh;
  WORD n,i;
  BYTE infoaddrl,infoaddrh;
  //WORD wLoadMeas[60] = {0};
  unsigned int offset =0;
  char *pdat_name;
  pdat_name="AU";
  BYTE count;
  BYTE YcIndex = 0;
  BYTE FixPt_Sum = 0;
  BYTE YC_Type =9;//归一化? 
    char ch[202]={0};
  char ch_1[20]={0};
  BYTE byLoadDa[128] = {0};
  //BYTE max_num;
        unsigned int object_addr_length =2;
  BYTE fix_YC_Num=0;
  m_houxu_flag =OFF;
  unsigned long FLoadAddr = FADDR_LOAD_START + ((unsigned long)FileName * FLASH_DAYLOAD_LEN)+ ((unsigned long)(section_current) *FLASH_PLOAD_LEN);//-1
  pTxPos = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;  //数据段号
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //后续标志
  if(mfixpt_recorded.fixpt_count_flag == OFF)//(!soe_recorded.Soe_Area )&&(!soe_recorded.soe_count)&&(!soe_recorded.Soe_Ptr)
  {
    FLoadAddrh = FADDR_LOAD_START + ((unsigned long)FileName * FLASH_DAYLOAD_LEN);
    Sst26vf064b_Read(FLoadAddrh,byLoadDa,FLASH_PLOAD_LEN);
    sprintf((char *)ch,"%.*s,%02d%02d%02d,%02d,%02d\r\n",sizeof(pdat_name),pdat_name,byLoadDa[2],byLoadDa[3],byLoadDa[4],object_addr_length,YC_Type);
    FixPt_Sum =0;
    for(n = 0; n < strlen(ch); n++)
    {
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch[n];
      FixPt_Sum += ch[n];
    }
    CAT_SpiReadBytes(EEPADD_DAYNUM+FileName,1,&count);//(mfixpt_recorded.fixpt_count)
    if(count >0)
      mfixpt_recorded.fixpt_Curren_count =1;//若有节数据数据，则对应的段至少为1，下面发送时再计算具体的段数
    mfixpt_recorded.fixpt_Ptr =0;
    mfixpt_recorded.fixpt_count_flag = ON;
    fix_YC_Num =0;
  }//下面未改
  else if(mfixpt_recorded.fixpt_count_flag == ON)
  {
        
        
        Sst26vf064b_Read(FLoadAddr,byLoadDa,FLASH_PLOAD_LEN);
        //memcpy(wLoadMeas,&byLoadDa[8],FLASH_PLOAD_LEN-8);
            
        for( i = mfixpt_recorded.fixpt_count; i < g_ucYcTransNum; i++)   //计算需要上传的遥测值，按照具体情况进行换算
        {
          if(g_ucYCAddr[i]!=255)
          {
              YcIndex = g_ucYCAddr[i] - 1;
                if(YcIndex <RM_1A_LUBO )//RM_1A_CABLE_T(YcIndex >= RM_BAT_U) && ((RM_1A_LOAD_I + RM_LOADI_NUM))
                {
                          //YcIndex = YcIndex - RM_1A_LOAD_I; 
                           
              infoaddrl=(i+ADDR_YC_LO)&0xff;
        infoaddrh=((i+ADDR_YC_LO)>>8)&0xff;
        offset +=sprintf(ch + offset,"%02x%02x %02x%02x,",infoaddrl,infoaddrh,byLoadDa[8+YcIndex*2],byLoadDa[9+YcIndex*2]);
                  //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = LOBYTE(wLoadMeas[YcIndex]);
                  //fix_YC_Num++;   //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(wLoadMeas[YcIndex]);
                }
        if (strlen(ch) >= 160)//200
        {
          mfixpt_recorded.fixpt_count ++;
          break;
        }
          
          }
      mfixpt_recorded.fixpt_count ++;
         }
    if(mfixpt_recorded.fixpt_count >= g_ucYcTransNum )//每一节后加换行
    {
      ch[offset -1]='\r';
      ch[offset]='\n';
    }
    if(mfixpt_recorded.fixpt_Ptr ==0)
    {
      fix_YC_Num =0;
      for( i = 0; i < g_ucYcTransNum; i++)   //计算需要上传的遥测值，按照具体情况进行换算
          {
                if(g_ucYCAddr[i]!=255)
              {
                  YcIndex = g_ucYCAddr[i] - 1;
                    if(YcIndex <RM_1A_LUBO )//RM_1A_CABLE_T(YcIndex >= RM_BAT_U) && ((RM_1A_LOAD_I + RM_LOADI_NUM))
                      
                      fix_YC_Num++;   //m_SendBuf.pBuf[m_SendBuf.wWritePtr++] = HIBYTE(wLoadMeas[YcIndex]);
          
              }
          }
      mfixpt_recorded.fixpt_Curren_count = (fix_YC_Num *10 + 1)/160 +1 ;//200 每个遥信占用字节数为10 (0140 0000，),加1的原因是最

      sprintf((char *)ch_1,"%02d,%02d%02d%02d_%02d%02d%02d,",fix_YC_Num,byLoadDa[2],byLoadDa[3],byLoadDa[4],byLoadDa[5],byLoadDa[6],byLoadDa[7]);
      for(n = 0; n < strlen(ch_1); n++)
      {
          
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = ch_1[n];
          FixPt_Sum += ch_1[n];
      }
      //mfixpt_recorded.fixpt_Curren_count= strlen(ch) / 200 +1;//传的数据段数
    }
        
    if(mfixpt_recorded.fixpt_Ptr < mfixpt_recorded.fixpt_Curren_count)
    {
          
      /*if((strlen(ch) - (mfixpt_recorded.fixpt_Ptr * 200)) > 200)
        max_num = 200;
      else
        max_num = strlen(ch) - mfixpt_recorded.fixpt_Ptr * 200;*/
      for(n =0;n<strlen(ch);n++)//for(n =(mfixpt_recorded.fixpt_Ptr * 200),i=0; i< max_num; n++,i++)//n < strlen(ch),
      {
          
          m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]   = ch[n];
           FixPt_Sum += ch[n];        
      }
      
      if(((mfixpt_recorded.fixpt_Ptr +1)== mfixpt_recorded.fixpt_Curren_count)&&(flag ==0x55))
        m_houxu_flag = ON;
      mfixpt_recorded.fixpt_Ptr++;
    }
          
         
    //if(mfixpt_recorded.fixpt_Ptr ==0)
        //pTxBuf = NULL;
  }
   *pTxPos = section_current;//pTxBuf - pTxPos-1;//LL
          *(pTxPos +1)= section_current >>8;
   *(pTxPos +2)= mfixpt_recorded.fixpt_Curren_count;
   *(pTxPos +3)= mfixpt_recorded.fixpt_Ptr ;//>>24
   if(m_houxu_flag == OFF)
   {
    *(pTxPos +4)= 1;//有后续
    mRecorder_flag.xuchuanflag= ON;
   }
   else
   {
    *(pTxPos +4)= 0;//有后续
    mRecorder_flag.xuchuanflag= OFF;
   }
   m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = FixPt_Sum;
  /*if(pTxBuf != NULL)
  {
    *pTxPos = pTxBuf - pTxPos-1;//LL
          
  }*/
  
}



void CBJ101S::ReadFileDataDat(WORD FileName,RECORDER_CFG *pgRecorder_cfg)
{

	unsigned char *pTxPos;
  	BYTE LuboDat_Sum = 0;
    int segment_leng = SEGMENT_LENGTH_NEW;
  	//unsigned int  DAT_NUM;
	unsigned long FLbAddrV;
	//unsigned char tt[8] = {0}; //8个通道,每个通道的数值有2个
    unsigned char datBuff[16] = {0};
	char ch[200];
	unsigned char k;
	
  	pTxPos = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;  //数据段号
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
  	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //后续标志



	for(int i=0;(mSendDATNum < pgRecorder_cfg->TOTAL_Leng);mSendDATNum++)//(DAT_NUM - offset_time_Max)每一行的值Sample_num
        {
        if(FileName<(MAX_REC_NUM+1))
        	{
          	FLbAddrV = FADDR_RECORDER_DATA+(unsigned long)(FileName)*0x2000+(unsigned long)mSendDATNum*10;
           	}
		else if(FileName<(MAX_REC_NUM+MAX_ACTREC_NUM+2))	
			{
			FLbAddrV =FADDR_RECORDER_ACTDATA+(unsigned long)(FileName-51)*0x90000+(unsigned long)mSendDATNum*10;
			}
		else if(FileName<(MAX_REC_NUM+MAX_ACTREC_NUM+MAX_XHREC_NUM+3))	
			{
			FLbAddrV = FADDR_RECORDER_XHDATA+(unsigned long)(FileName-62)*0x8000+(unsigned long)mSendDATNum*10;
			}
		else
			{//err
			}
		Sst26vf064b_Read(FLbAddrV,&datBuff[0],10);
		
		unsigned long xt =250;
       	if(pgRecorder_cfg->CFG_Samp==800)xt=1250;//张弢 动作录波 频率800 时间间隔1250
      	if(pgRecorder_cfg->CFG_Samp==1600)xt=625;//张弢 动作录波 频率1600 时间间隔625
      	if(pgRecorder_cfg->CFG_Samp==2000)xt=500;//张弢 动作录波 频率1600 时间间隔625

		unsigned int dka;//,dkd;
	   	dka = MAKEWORD(datBuff[8],datBuff[9]);
	   
	   	if(dka&((unsigned int)(1<<15)))
	   		dka |=(1<<14);
	   	else
	   		dka &= NBITE;

	   	unsigned char a,b,c;//,gkx;	   
	   a=(datBuff[1]>>6)&0x01;
	   b=(datBuff[3]>>6)&0x01;
	   c=(datBuff[5]>>6)&0x01;//高压接触器辅助触点
	   
	   if((datBuff[1]>>7)&0x01)
	   	datBuff[1] |= (1<<6);
	   else
	   	datBuff[1] &= 0xbf;
	   
	   if((datBuff[3]>>7)&0x01)
	   	datBuff[3] |= (1<<6);
	   else
	   	datBuff[3] &= 0xbf;
	   
	   if((datBuff[5]>>7)&0x01)
	   	datBuff[5] |= (1<<6);
	   else
	   	datBuff[5] &= 0xbf;
	   
		unsigned long xtt = (long)mSendDATNum*xt;
		if(g_gRunPara[RP_CFG_KEY]&BIT[RP_COMTRADE_TYPE])
       		{
          	ch[0]=LOBYTE(mSendDATNum);ch[1]=HIBYTE(mSendDATNum);ch[2]=0;ch[3]=0;
		 	ch[4] = LOBYTE(LOWORD(xtt));
           	ch[5] = HIBYTE(LOWORD(xtt));
           	ch[6] = LOBYTE(HIWORD(xtt));
          	ch[7] = HIBYTE(HIWORD(xtt));
		 	ch[8] = datBuff[0];ch[9] = datBuff[1];ch[10] = datBuff[2];ch[11] = datBuff[3];
		 	ch[12] = datBuff[4];ch[13] = datBuff[5];ch[14] = datBuff[6];ch[15] = datBuff[7];
		 	ch[16]=LOBYTE(dka);ch[17]=HIBYTE(dka);
#ifdef YN_101S
		 	ch[18]=(datBuff[9]>>6)&0x01;//信号源控制信号//云南录波取控制信号
#else
			ch[18]=a+b*2+c*4;//取三相接触器辅助触点信号
#endif
		 	ch[19]=0;
		 	i+=20;k=20;
          	}
	   else
	   		{
          	sprintf((char *)ch,"%ld,%ld,%d,%d,%d,%d,%d,%d,%d,%d\n",mSendDATNum,xtt,MAKEWORD(datBuff[0],datBuff[1]),MAKEWORD(datBuff[2],datBuff[3]),
                                 MAKEWORD(datBuff[4],datBuff[5]),MAKEWORD(datBuff[6],datBuff[7]),
                                 dka,(unsigned int)a,(unsigned int)b,(unsigned int)c);//每个周期80个点，采样周期250微妙//张弢
                                 
          	i+= strlen(ch);k=strlen(ch);
	   		}
  
     	if(i <=segment_leng)
       		{
           	for(BYTE n = 0; n < k; n++)
           		{
              	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch[n];
              	LuboDat_Sum += ch[n];
                } 
      		*pTxPos = mSendDATNum;//pTxBuf - pTxPos-1;//LL
       		*(pTxPos +1)= mSendDATNum >>8;
      		*(pTxPos +2)= mSendDATNum >>16;
      		*(pTxPos +3)= mSendDATNum >>24;
      		*(pTxPos +4)= 1;//有后续
       		mRecorder_flag.xuchuanflag= ON;                  
     		}
  		else
     		{
       		break;//return pTxBuf;
     		}
   		if(mSendDATNum == ((pgRecorder_cfg->TOTAL_Leng)-1))//DAT_NUM -offset_time_MaxSample_num-1
  			{
      		*(pTxPos +4)= 0;
      		mRecorder_flag.xuchuanflag= OFF;
      		mSendDATNum++;
      		mluboYX_flag = ON;//检测到该标志位表示录波召唤完成
                        //mRecorder_flag.LIST_flag = OFF;
     		}
             
   }
     
     m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LuboDat_Sum;
  
}


void CBJ101S::ReadFileDataCfg(RECORDER_CFG *pmRecorder_cfg)
{
	int segment_leng = SEGMENT_LENGTH_NEW;
   	BYTE j;
   	BYTE LuboCfg_Sum = 0;
   	unsigned char strtemp;
   	unsigned char *pTxPos;
     
    pTxPos = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];
   	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;  //数据段号
   	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
   	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
   	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; 
   	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //后续标志
      
   	for(j = 0;(j < segment_leng)&&(mSendCFGNum < pmRecorder_cfg->CFG_Leng);mSendCFGNum++,j++)
   		{
       	strtemp =  CAT_SpiReadByte(EEPADD_CFG+mSendCFGNum);
       	LuboCfg_Sum += strtemp;
       	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= strtemp;     
	   	}
   	*pTxPos = mSendCFGNum;
  	*(pTxPos+1)= mSendCFGNum>>8;
   	*(pTxPos+2)= mSendCFGNum>>16;
  	*(pTxPos+3)= mSendCFGNum>>24;
    *(pTxPos+4)= 1;//有后续
       
  	mRecorder_flag.xuchuanflag= ON;
  	if(mSendCFGNum >= pmRecorder_cfg->CFG_Leng)//最后一段
    	{
      	*(pTxPos+4)= 0;//无后续
      	mRecorder_flag.xuchuanflag= OFF;
       	}
   	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LuboCfg_Sum;
}


void CBJ101S::Send_ReadFile_Data(WORD File_ID,BYTE File_TYPE,WORD InfoAddr,unsigned int Packet_Num)
{
  BYTE Style = F_FP_FA;
  BYTE Reason = COT_REQ;
    BYTE PRM = 0;
    BYTE dwCode = 8;
    //WORD wave_total;
  unsigned int VSQ=1;
  RECORDER_CFG m_Recorder_cfg;
  //unsigned int  DAT_NUM_1;
  //unsigned int  offset_time_Max =0;
  //unsigned int  Total_packet;
  BYTE Loadday=0;
  //unsigned int t_FileNum;
  long FADDR_RECORDER;

  m_Recorder_cfg.FileName = 0xff;
  	if(File_ID<(MAX_REC_NUM+1))
  		{
		FADDR_RECORDER =FADDR_RECORDER_DATA-256+ (unsigned long)(File_ID+1)*0x2000;
  		}
	else if(File_ID<(MAX_REC_NUM+MAX_ACTREC_NUM+2))	
		{
		FADDR_RECORDER = FADDR_RECORDER_ACTDATA-256 +(unsigned long)(File_ID+1)*0x90000;
		}
	else if(File_ID<(MAX_REC_NUM+MAX_ACTREC_NUM+MAX_XHREC_NUM+3))	
		{
		FADDR_RECORDER = FADDR_RECORDER_XHDATA-256 +(unsigned long)(File_ID+1)*0x8000;
		}
	else
		{//err
		return;
		}
	Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&m_Recorder_cfg,sizeof(m_Recorder_cfg));
	if(m_Recorder_cfg.FileName>=MAX_ALLREC_NUM)
		{//err
		return;
		}


    SendFrameHead(Style, Reason);
  write_infoadd(InfoAddr);
  
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =2;  //在信息体地址后面，附加数据包类型1个字节，备用1,3,4.文件传输 2
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = OPERATING_LOGO5;

  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = File_ID;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = File_ID >>8;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;
  if(File_TYPE ==1)//CFG
  {
      if(Packet_Num==1)
      {
        mSendCFGNum =0;
        mSendDATNum =0;
        cfg_dat_length(&m_Recorder_cfg,File_ID);
        //cfg_dat_length(File_ID);
        m_Recorder_cfg.FileName=File_ID;
		m_Recorder_cfg.CFG_Leng=CAT_SpiReadWord(EEPADD_CFG-2);
      }
      if(mSendCFGNum >= (m_Recorder_cfg.CFG_Leng))//最后一段
      {
        mSendCFGNum =0;
        //mRecorder_flag.xuchuanflag= OFF;
        return;
      }
      else 
          m_Recorder_cfg.CFG_Leng=CAT_SpiReadWord(EEPADD_CFG-2);
            ReadFileDataCfg(&m_Recorder_cfg);//wSecLenPtr,
  }
  else if(File_TYPE ==2)//DAT
  {
      if(Packet_Num==1)
      {
        mSendCFGNum =0;
        mSendDATNum =0;
          mluboYX_flag=OFF;
      }
	  /* 录波周期不为16 截取RP_LUBO_CYCLE个周波
       DAT_NUM_1 = DAT_AD_NUM;
        CAT_SpiReadWords(EEPADD_COMLEN + File_ID*EELEN_LENG, 1, &offset_time_Max); 
      if(g_gRunPara[RP_LUBO_CYCLE] ==0)//对时没对上
        Total_packet = DAT_NUM_1-offset_time_Max;
      else
        Total_packet = (g_gRunPara[RP_LUBO_CYCLE] * 80);
        */
       //读FILE_ID的m_Recorder_cfg  
      if(mSendDATNum >= m_Recorder_cfg.TOTAL_Leng)//最后一段
      {
        mSendDATNum =0;
        return;
      }
      else  //前面流程已发送过数据
        
            ReadFileDataDat(File_ID,&m_Recorder_cfg);//wSecLenPtr,
  }
  else if(File_TYPE ==3)//FIXPT
  {
      if(Packet_Num==1)
      {
        mfixpt_recorded.fixpt_Ptr =0;
              mfixpt_recorded.fixpt_count =0;
              mfixpt_recorded.fixpt_Curren_count =0;
              mfixpt_recorded.fixpt_count_flag =OFF;
        m_current_section =0;
        m_houxu_flag = OFF;
      }
      
      CAT_SpiReadBytes(EEPADD_DAYNUM + File_ID ,1,&Loadday);  
      
      if(m_current_section + 1 < Loadday)
      {
        if((mfixpt_recorded.fixpt_Ptr >= mfixpt_recorded.fixpt_Curren_count ) &&(mfixpt_recorded.fixpt_count_flag == ON))//最后一段Sample_num
        {
          m_current_section++;
          mfixpt_recorded.fixpt_Ptr =0;
              mfixpt_recorded.fixpt_count =0;
              mfixpt_recorded.fixpt_Curren_count =1;
        
        }
        if(m_current_section +1 == Loadday)
          ReadFileDataFixpt(File_ID,m_current_section,0x55);//wSecLenPtr
        else
          ReadFileDataFixpt(File_ID,m_current_section,1);
      }
      else
      {
        if(m_houxu_flag == ON)
        {
          //mfixpt_recorded.fixpt_count_flag = OFF;
          m_houxu_flag = OFF;
          //mRecorder_flag.xuchuanflag= OFF;
          return;
        }
        else if(m_houxu_flag == OFF)
          ReadFileDataFixpt(File_ID,m_current_section,0x55);
      }
              
  }
  else if(File_TYPE == 4)//SOE
  {
    if(Packet_Num==1)
    	{
      	msoe_recorded.Soe_Area =g_unSSoeSaveE2ROMPtr;
     	msoe_recorded.Soe_Ptr =g_unSSoeSaveE2ROMPtr;
       	msoe_recorded.soe_count =soe_num;
		if(soe_num<512)msoe_recorded.Soe_Area=EEPADD_SOESTARTADR;
       	msoe_recorded.Soe_Curren_count =0;
       	msoe_recorded.Soe_count_flag =OFF;
      	#ifndef GETSOEFROMRAM//从外部FLASH取个数
          	unsigned int Flashheadbk  =g_InFlashInfo.m_unSSoeInFlashHead;
            soe_num =  ReadNumSoeFromFlash((WORD *)&Flashheadbk,g_unSSoeInFlashTail);
          	//m_BkFlashhead =g_InFlashInfo.m_unSSoeInFlashHead;
     	#endif      
    	}
    if(( msoe_recorded.Soe_Curren_count >= soe_num ) &&(msoe_recorded.Soe_count_flag == ON))//最后一段Sample_num
    	{
       	msoe_recorded.Soe_Curren_count = 0;
      	// mRecorder_flag.xuchuanflag= OFF;
      	return;
    	}
    else
    	{
      	#ifdef GETSOEFROMRAM
       		ReadFileDataSoe(File_ID);//wSecLenPtr,
       	#else
       		ReadFileDataSoe_Flash(File_ID);        
      	#endif
    	}
  }
  else if(File_TYPE == 5)
  {
    if(Packet_Num==1)
    {
        mlog_recorded.log_count_flag =OFF; 
      mlog_recorded.log_Ptr  =0;
          mlog_recorded.log_Curren_count =0;
      mlog_recorded.log_count =0;
      m_logcurrent_section =0;
      m_loghouxu_flag = OFF;
          //log_sum_section =0;  
    }
    if(m_logcurrent_section < mwavelog_total) //认可第一节后发送节2准备就绪
    {
      if(mlog_recorded.log_count_flag  == ON)
        m_logcurrent_section++;//=2;
        mlog_recorded.log_Ptr  =0;
            mlog_recorded.log_Curren_count =1;
        mlog_recorded.log_count =0;
      //else
      //mlog_recorded.log_Curren_count =1;
        //mlog_recorded.log_Ptr  =0;
          //mlog_recorded.log_Curren_count =1;
      //mlog_recorded.log_count =0;
      if(m_logcurrent_section  == mwavelog_total)//Loadday
        ReadFileDataUlog(File_ID,m_logcurrent_section,0x55);//wSecLenPtr
      else
        ReadFileDataUlog(File_ID,m_logcurrent_section,1);
      
    }
    else
    {
      if(m_loghouxu_flag == ON)
      {
          //mfixpt_recorded.fixpt_count_flag = OFF;
        m_loghouxu_flag = OFF;
        return;
      }
      //else if(m_loghouxu_flag == OFF)
        //ReadFileDataFixpt(File_ID,m_current_section,0x55);
    }
   
  }
   SendFrameTail(PRM, dwCode, VSQ,0);

}


void CBJ101S::Send_ReadFile_Confirm(WORD File_ID,BYTE File_TYPE,WORD InfoAddr,unsigned char *pRxBuf,unsigned char length)
{
  BYTE Style = F_FP_FA;
  BYTE Reason = COT_ACTCON;
    BYTE PRM = 0;
    BYTE dwCode = 8;
    //WORD wave_total;
  unsigned int VSQ=1;
  BYTE LoadDay = 0;
  BYTE YC_NUM =0;
  //unsigned int t_FileNum;
  BYTE YcIndex;
  //char ph[10];
   // int YxIndex = 0;
  //unsigned char sc_num =0;
  //unsigned char *pTxPos,*pTxPos1;
  RECORDER_CFG m_Recorder_cfg;
  long FADDR_RECORDER;

  m_Recorder_cfg.FileName = 0xff;
  	if(File_ID<(MAX_REC_NUM+1))
  		{
		FADDR_RECORDER =FADDR_RECORDER_DATA-256+ (unsigned long)(File_ID+1)*0x2000;
  		}
	else if(File_ID<(MAX_REC_NUM+MAX_ACTREC_NUM+2))	
		{
		FADDR_RECORDER = FADDR_RECORDER_ACTDATA-256 +(unsigned long)(File_ID+1)*0x90000;
		}
	else if(File_ID<(MAX_REC_NUM+MAX_ACTREC_NUM+MAX_XHREC_NUM+3))	
		{
		FADDR_RECORDER = FADDR_RECORDER_XHDATA-256 +(unsigned long)(File_ID+1)*0x8000;
		}
	else
		{//err
		return;
		}
	Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&m_Recorder_cfg,sizeof(m_Recorder_cfg));
	if(m_Recorder_cfg.FileName>=MAX_ALLREC_NUM)
		{//err
		return;
		}


  SendFrameHead(Style, Reason);
  write_infoadd(InfoAddr);
  
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =2;  //在信息体地址后面，附加数据包类型1个字节，备用1,3,4.文件传输 2
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = OPERATING_LOGO4;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//结果描述字
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = length;//结果描述字
  for(BYTE i =0;i<length;i++)
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = pRxBuf[i];
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = File_ID;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = File_ID >>8;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;
  if(File_TYPE == 1)
  {
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=m_Recorder_cfg.CFG_Leng;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=m_Recorder_cfg.CFG_Leng >>8;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=m_Recorder_cfg.CFG_Leng >>16;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=m_Recorder_cfg.CFG_Leng >>24;
  }
  else if(File_TYPE == 2)
  {
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=m_Recorder_cfg.DATA_Leng;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=m_Recorder_cfg.DATA_Leng >>8;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=m_Recorder_cfg.DATA_Leng >>16;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=m_Recorder_cfg.DATA_Leng >>24;
  }
  else if(File_TYPE == 3)
  {
    
    CAT_SpiReadBytes(EEPADD_DAYNUM+File_ID,1,&LoadDay);
    for(BYTE m = 0; m < g_ucYcTransNum; m++)   //计算需要上传的遥测值，按照具体情况进行换算
          {
            if(g_ucYCAddr[m]!=255)  
            {
      YcIndex = g_ucYCAddr[m] - 1;
      if(YcIndex <RM_1A_LUBO )//RM_1A_CABLE_T(YcIndex >= RM_BAT_U) && ((RM_1A_LOAD_I + RM_LOADI_NUM))
                  {
        YC_NUM++;
      }

    }
              }
         //m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//文件属性
         m_Fixpt_SECTION_Leng = 17+18+YC_NUM*10;
         m_Fixpt_TOTAL_Leng =17+((DWORD)(18+YC_NUM*10)* LoadDay);//文件头17个字节+节头+YC的个数
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=m_Fixpt_TOTAL_Leng;
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= m_Fixpt_TOTAL_Leng>>8;
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= m_Fixpt_TOTAL_Leng >>16;
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= m_Fixpt_TOTAL_Leng >>24;
  }
  else if(File_TYPE == 4)
  {
    TOTAL_Leng = 12+soe_num * SOE_RECORD_LEN;//报文头9个字节
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = TOTAL_Leng;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = TOTAL_Leng>>8;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = TOTAL_Leng>>16;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = TOTAL_Leng>>24;
  }
  else if(File_TYPE == 5)
  {
    unsigned int FLogInfo[FLOGINFONUM]; 
    CAT_SpiReadWords(EEPADD_LOGP, FLOGINFONUM, FLogInfo);
    if(FLogInfo[FLOG_CS] != (FLogInfo[FLOG_TOTALNUM] + FLogInfo[FLOG_NEW] + FLogInfo[FLOG_OLD]))//+FLoadInfo[FLOAD_DAY]) || FLoadInfo[FLOAD_TOTALNUM] > FLASH_LOAD_MAXNUM || FLoadInfo[FLOAD_NEW] > FLASH_LOAD_MAXNUM)//如果FLASH地址不在负荷记录保存区域内
          FLogInfo[FLOG_TOTALNUM] = 0; //清空负荷记录总条数
      mwavelog_total = FLogInfo[FLOG_TOTALNUM];
    mLog_TOTAL_Leng = 9+mwavelog_total * FLASH_PLOAD_LEN;//报文头9个字节
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = mLog_TOTAL_Leng;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = mLog_TOTAL_Leng>>8;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = mLog_TOTAL_Leng>>16;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = mLog_TOTAL_Leng>>24;
    
  }
   SendFrameTail(PRM, dwCode, VSQ,0);
}

void CBJ101S::ulog_directory_confirm(WORD InfoAddr,DWORD Directory_ID)
{
  
  BYTE Style = F_FP_FA;
  BYTE Reason = COT_REQ;
  BYTE PRM = 0;
  BYTE dwCode = 8;
  unsigned int FLogInfo[FLOGINFONUM];    //从EEPROM中读取出来的FLASH中保存负荷记录的相关信息，总条数+即将存储记录的位置+最老一条记录的位置+校验
  unsigned long FLogAddr;
  BYTE byLogDa[128] = {0};  
  unsigned char *pTxPos,*pTxPos1;
  char ch[5]={0};
  CAT_SpiReadWords(EEPADD_LOGNUM, FLOGINFONUM, FLogInfo);
  if(FLogInfo[FLOG_CS] != (FLogInfo[FLOG_TOTALNUM] + FLogInfo[FLOG_NEW] + FLogInfo[FLOG_OLD]))//+FLoadInfo[FLOAD_DAY]) || FLoadInfo[FLOAD_TOTALNUM] > FLASH_LOAD_MAXNUM || FLoadInfo[FLOAD_NEW] > FLASH_LOAD_MAXNUM)//如果FLASH地址不在负荷记录保存区域内
          FLogInfo[FLOG_TOTALNUM] = 0; //清空负荷记录总条数
    mwavelog_total = FLogInfo[FLOG_TOTALNUM];
  
  SendFrameHead(Style, Reason);
  write_infoadd(InfoAddr);
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =2;  //在信息体地址后面，附加数据包类型1个字节，备用1,3,4.文件传输 2
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =OPERATING_LOGO2;
    
   pTxPos = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //结果描述字
    
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID >>8;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID >>16;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID >>24;
  pTxPos1 = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];//后续标志
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //后续标志
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =1; //文件数量
  mRecorder_flag.LIST_flag = OFF;
  if(mwavelog_total ==0)
  {
  	
   		*pTxPos = 1;//结果描述字失败 
	    *pTxPos1 =0;
	  *(pTxPos1+1) = 0;
	  SendFrameTail(PRM, dwCode, 0x01,0);
	  return;
  }
  sprintf((char *)ch,"ulog");
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = strlen(ch);
    for(BYTE j = 0; j < strlen(ch); j++)
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch[j];
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;
  mLog_TOTAL_Leng = 9+mwavelog_total * FLASH_PLOAD_LEN;//报文头9个字节
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = mLog_TOTAL_Leng;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = mLog_TOTAL_Leng>>8;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = mLog_TOTAL_Leng>>16;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = mLog_TOTAL_Leng>>24;
  FLogAddr = FADDR_LOG_START;// + ((unsigned long)wSendLISTNum * FLASH_DAYLOAD_LEN);
  Sst26vf064b_Read(FLogAddr,byLogDa, FLASH_PLOAD_LEN);
  //m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;//= 0x20;//m_Recorder_cfg.CFG_SOF;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= (byLogDa[10]*1000)>>8;   
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= (byLogDa[10]*1000);
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= byLogDa[9];
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= byLogDa[8];
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= byLogDa[7];
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= byLogDa[6];      
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= byLogDa[5];
  SendFrameTail(PRM, dwCode, 0x01,0);
  
  
}

void CBJ101S::soe_directory_confirm(WORD InfoAddr,DWORD Directory_ID)
{
  BYTE Style = F_FP_FA;
  BYTE Reason = COT_REQ;
    BYTE PRM = 0;
    BYTE dwCode = 8;
  unsigned char *pTxPos,*pTxPos1;
    char ch[5]={0};
  
    SendFrameHead(Style, Reason);
  write_infoadd(InfoAddr);
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =2;  //在信息体地址后面，附加数据包类型1个字节，备用1,3,4.文件传输 2
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =OPERATING_LOGO2;
  
   pTxPos = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //结果描述字
  
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID >>8;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID >>16;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID >>24;
  pTxPos1 = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];//后续标志
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //后续标志
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =1; //文件数量
  mRecorder_flag.LIST_flag = OFF;

   #ifndef GETSOEFROMRAM//从外部FLASH取个数       
       unsigned int Flashheadbk  =g_InFlashInfo.m_unSSoeInFlashHead;
       soe_num =  ReadNumSoeFromFlash((WORD *)&Flashheadbk,g_unSSoeInFlashTail);  
   #endif
   if(soe_num ==0)//没有SOE记录时的回复
   	{
   		*pTxPos = 1;//结果描述字失败 
	    *pTxPos1 =0;
	  *(pTxPos1+1) = 0;
	  SendFrameTail(PRM, dwCode, 0x01,0);
	  return;
   	}
  TOTAL_Leng = 12+soe_num * SOE_RECORD_LEN;//报文头9个字节
       
  sprintf((char *)ch,"soe");
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = strlen(ch);
    for(BYTE j = 0; j < strlen(ch); j++)
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch[j];
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//文件属性
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = TOTAL_Leng;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = TOTAL_Leng>>8;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = TOTAL_Leng>>16;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = TOTAL_Leng>>24;
  
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = (g_sRtcManager.m_gRealTimer[RTC_MICROSEC]+g_sRtcManager.m_gRealTimer[RTC_SEC]*1000)&0xff;//录波触点时间
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = (g_sRtcManager.m_gRealTimer[RTC_MICROSEC]+g_sRtcManager.m_gRealTimer[RTC_SEC]*1000)>>8;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = g_sRtcManager.m_gRealTimer[RTC_DATE]|(g_sRtcManager.m_gRealTimer[RTC_WEEK]<<5);
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = g_sRtcManager.m_gRealTimer[RTC_MONTH];    
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = g_sRtcManager.m_gRealTimer[RTC_YEAR]-2000;
  SendFrameTail(PRM, dwCode, 0x01,0);
}

void CBJ101S::fixpt_directory_confirm(WORD InfoAddr,DWORD Directory_ID,BYTE call_sign,DWORD start_minute_time,DWORD start_date_time,DWORD end_minute_time,DWORD end_date_time)
{
  BYTE Style = F_FP_FA;
  BYTE Reason = COT_REQ;
    BYTE PRM = 0;
    BYTE dwCode = 8;
  unsigned char *pTxPos,*pTxPos1;
   unsigned int FLoadInfo[FLOADINFONUM]; 
  unsigned long FLoadAddr;
  BYTE byLoadDa[128] = {0};
  BYTE LoadDay[31] = {0};
  //BYTE wCurLoadDay;
  BYTE YC_NUM =0;
  //BYTE Fixpt_SOF;
  //WORD i;
  BYTE YcIndex;
  BYTE fix_num =0;
  //char *pdat_name;
  char ch[20]={0};
  DWORD date_time =0;
  BYTE fixpt_sign =0;
  BYTE num_flag =0;
  //pdat_name="fixpt";
        SendFrameHead(Style, Reason);
  write_infoadd(InfoAddr);
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =2;  //在信息体地址后面，附加数据包类型1个字节，备用1,3,4.文件传输 2
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =OPERATING_LOGO2;
  
  pTxPos = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];
       m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //结果描述字
  
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID >>8;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID >>16;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID >>24;
  pTxPos1 = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];//后续标志
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //后续标志
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //文件数量
  mRecorder_flag.LIST_flag = OFF;

  CAT_SpiReadWords(EEPADD_LOADNUM, FLOADINFONUM, FLoadInfo);
  if((FLoadInfo[FLOAD_CS] != FLoadInfo[FLOAD_TOTALNUM] + FLoadInfo[FLOAD_NEW] + FLoadInfo[FLOAD_OLD]+FLoadInfo[FLOAD_DAY]) || FLoadInfo[FLOAD_TOTALNUM] > FLASH_LOAD_MAXNUM || FLoadInfo[FLOAD_NEW] > FLASH_LOAD_MAXNUM)//如果FLASH地址不在负荷记录保存区域内

          FLoadInfo[FLOAD_TOTALNUM] = 0; //清空负荷记录总条数
          if(FLoadInfo[FLOAD_TOTALNUM] >0)//计算该上传的文件个数
            m_wavefix_total = FLoadInfo[FLOAD_DAY]+1;//FLoadInfo[FLOAD_TOTALNUM] / 96 +1;
    else
      m_wavefix_total = 0;
    if(call_sign ==1)
    {
      for(BYTE n =0;n < m_wavefix_total;n++)
      {
        FLoadAddr = FADDR_LOAD_START + ((unsigned long)n * FLASH_DAYLOAD_LEN);
      Sst26vf064b_Read(FLoadAddr,byLoadDa, FLASH_PLOAD_LEN);
      date_time = byLoadDa[4];
            date_time += ((DWORD)byLoadDa[3] <<8);
            date_time +=((DWORD)byLoadDa[2] <<16);
      if((date_time >= start_date_time) &&(date_time <=end_date_time)) 
      {
        num_flag ++;//fixpt_sign = ON;//满足时间召唤条件条件    
      }
      
      }
    }
      
    /*if(m_wavefix_total- m_SendFixNum < 6)
    fix_num = (m_wavefix_total-m_SendFixNum);
    else
      fix_num = 6;*/
      
      //gRecorder_flag.LIST_flag = OFF; 
      
    /*if(FLoadInfo[FLOAD_NEW] == 0)//最后文件的位置
              wCurLoadDay = 31 -1;
       else
              wCurLoadDay = FLoadInfo[FLOAD_NEW]/96;//-1; */
       if((m_SendFixNum > m_wavefix_total) ||(m_wavefix_total==0))//最后一段=
       {    
          m_SendFixNum = 0;
          if(m_wavefix_total == 0)
          {
  
              *pTxPos = 1;
            *pTxPos1 = 0;//后续标志  
            *(pTxPos1 + 1) = 0;//文件数量     
      }
        
    }
    else
    {
      if(m_SendFixNum == m_wavefix_total) m_wavefix_total=0;
      for( BYTE i=0;(m_SendFixNum < m_wavefix_total)&&(i< 8);i++,m_SendFixNum++)
      {
          fixpt_sign = OFF;
        FLoadAddr = FADDR_LOAD_START + ((unsigned long)m_SendFixNum * FLASH_DAYLOAD_LEN);
        Sst26vf064b_Read(FLoadAddr,byLoadDa, FLASH_PLOAD_LEN);
        YC_NUM =0;
        CAT_SpiReadBytes(EEPADD_DAYNUM+m_SendFixNum,1,&LoadDay[m_SendFixNum]);
        if(call_sign ==1)
        {
          date_time = byLoadDa[4];
                    date_time += ((DWORD)byLoadDa[3] <<8);
                    date_time +=((DWORD)byLoadDa[2] <<16);
          if((date_time >= start_date_time) &&(date_time <=end_date_time)) 
          {
            fixpt_sign = ON;//满足时间召唤条件条件  
            m_fixpt_num++;
          }
                    if((fixpt_sign ==OFF) &&(m_SendFixNum < m_wavefix_total)&&(i==7))
                          i =0; 
        }
        if((call_sign ==0) ||((call_sign ==1)&&(fixpt_sign == ON)))
        {
         //*pTxBuf++ = m_SendFixNum;
              //*pTxBuf++ = m_SendFixNum >> 8;
        //FLoadAddrh = FADDR_LOAD_START + ((unsigned long)FileName * FLASH_DAYLOAD_LEN);
        //Sst26vf064b_Read(FLoadAddrh,byLoadDa,FLASH_PLOAD_LEN);
        fix_num++;
        sprintf((char *)ch,"fixpt%04d%02d%02d",(byLoadDa[2]+2000),byLoadDa[3],byLoadDa[4]);//strlen(pdat_name),pdat_name,
        //sprintf((char *)ch,"FIX");
              m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = strlen(ch);
            for(BYTE j = 0; j < strlen(ch); j++)
            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = ch[j];

        sprintf((char *)ch,"%04d%02d%02d_%02d",(byLoadDa[2]+2000),byLoadDa[3],byLoadDa[4],m_SendFixNum);

        CAT_SpiWriteBytes(EEPADD_FIXNUM+m_SendFixNum*20, strlen(ch),(unsigned char *) ch);
        
        for(BYTE m = 0; m < g_ucYcTransNum; m++)   //计算需要上传的遥测值，按照具体情况进行换算
            {
                  if(g_ucYCAddr[m]!=255)  
              {
                  YcIndex = g_ucYCAddr[m] - 1;
              if(YcIndex <RM_1A_LUBO )//RM_1A_CABLE_T(YcIndex >= RM_BAT_U) && ((RM_1A_LOAD_I + RM_LOADI_NUM))
                    {
              YC_NUM++;
              }

              }
             }
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//文件属性
         m_Fixpt_SECTION_Leng = 17+18+YC_NUM*10;
         m_Fixpt_TOTAL_Leng =17+((DWORD)(18+YC_NUM*10)* (LoadDay[m_SendFixNum]));//文件头17个字节+节头+YC的个数
        //*pTxBuf++ = (LoadDay[wSendLISTNum] * FLASH_PLOAD_LEN) ;
        //*pTxBuf++ = m_Recorder_cfg.lubo_total_num >> 8;
       
      
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Fixpt_TOTAL_Leng;
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Fixpt_TOTAL_Leng>>8;
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Fixpt_TOTAL_Leng>>16;
         m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Fixpt_TOTAL_Leng>>24;
        
        if(m_SendFixNum >= m_wavefix_total-1)//SOF状态
        {
        //m_Recorder_cfg.CFG_SOF = 0x20;  /*<0>:=后面还有目录文件；<1>:=最后目录文件*/  
        mRecorder_flag.LIST_flag = OFF;
        mRecorder_flag.xuchuanflag= OFF;
        
        }    
        else
        {
        //m_Recorder_cfg.CFG_SOF = 0;
        mRecorder_flag.LIST_flag = ON;// = TRUE;//ON;新修改
        }
        
      //*pTxBuf++ =Fixpt_SOF;
      
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = 0;//录波触点时间
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = byLoadDa[7];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = byLoadDa[6];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = byLoadDa[5];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = byLoadDa[4];
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = byLoadDa[3];
      
        m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = byLoadDa[2];
      }
    }
      /*if(gRecorder_flag.LIST_flag == OFF)
      {
      wSendLISTNum = 0;
      }*/
     }
              *pTxPos = 0;
    if(call_sign ==1)
    {
      if(m_fixpt_num >= num_flag )
        *pTxPos1 = 0;
      else
        *pTxPos1 = 1;
    }
    else
    {
        if(m_SendFixNum >= m_wavefix_total)//+718每报数传8条录波数据，每条对应CFG DAT两个目录
        *pTxPos1 = 0;//后续标志
        else
        *pTxPos1 = 1;//后续标志
    }
         if(mRecorder_flag.LIST_flag == OFF)
          {
              m_SendFixNum = 0;
          }
  //if(g_gRunPara[RP_LBSTORAGE_TIME] > 0)
  //{
    *(pTxPos1+1) = fix_num;//sc_num*2;
    if(fix_num ==0)
    {
		*pTxPos = 1;//结果描述字失败 
	    *pTxPos1 =0;
		if(m_fixpt_num ==0)
		 SendFrameTail(PRM, dwCode, 0x01,0);
		else
	  	return;
    }
    else
      SendFrameTail(PRM, dwCode, 0x01,0);   //(sc_num*2)| VSQ
  
}

//按时间查询返回符合条件的录波总个数


unsigned char CBJ101S::Get_time_read(WORD wave_total,DWORD start_minute,DWORD start_date,DWORD end_minute,DWORD end_date)
{
  long FADDR_RECORDER;
  DWORD date_time =0;
  DWORD minute_time=0;
  RECORDER_CFG m_Recorder_cfg;
  BYTE m_send_num =0;
  for(int m =0;m <wave_total;m++)
  {
    for(int n=0;n<3;n++)
        {
            FADDR_RECORDER =FADDR_RECORDER_INFO+ (long)m*(long)FLINEADDR +(long)n*(long)FPHASEADDR; 
            Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&m_Recorder_cfg,sizeof(m_Recorder_cfg)); //(unsigned char *)不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来 
              //flag_lb++;
            if(m_Recorder_cfg.lubo_flag ==0x55)
            {
                  //flag_lb =0;
                break;
            }
        
    }
    date_time = m_Recorder_cfg.comtrade_time[RTC_DATE] ;
    date_time+= (DWORD)m_Recorder_cfg.comtrade_time[RTC_MONTH]<< 8;
    date_time+=((DWORD)m_Recorder_cfg.comtrade_time[RTC_YEAR]-2000)<<16;
    minute_time = m_Recorder_cfg.comtrade_time[RTC_SEC];
    minute_time += (DWORD)m_Recorder_cfg.comtrade_time[RTC_MINUT]<<8;
    minute_time +=  (DWORD)m_Recorder_cfg.comtrade_time[RTC_HOUR] <<16;
    if((date_time >= start_date) &&(date_time <=end_date)) 
    {
          
          if((date_time == start_date) &&(date_time ==end_date))
            {
              if((minute_time >= start_minute ) &&(minute_time <= end_minute))
          m_send_num++;//lubo_sign = ON;//满足时间召唤条件条件
            }
            else if(date_time == start_date)
            {
              if(minute_time >= start_minute) 
          m_send_num++;//lubo_sign = ON;//满足
            }
            else if(date_time ==end_date)
            {
              if(minute_time <= end_minute)
                  m_send_num++;//lubo_sign = ON;//满足
             }
             else
                m_send_num++;//lubo_sign = ON;//满足
      }
    }
    return m_send_num;
  
}

void CBJ101S::lubo_directory_confirm(WORD InfoAddr,DWORD Directory_ID,BYTE call_sign,DWORD start_minute_time,DWORD start_date_time,DWORD end_minute_time,DWORD end_date_time)
{
	BYTE Style = F_FP_FA;
  	BYTE Reason = COT_REQ;
    BYTE PRM = 0;
    BYTE dwCode = 8;
    WORD wave_total;
  	char ph[50];
  	//DWORD date_time =0;
  	//DWORD minute_time=0;
  	//BYTE  lubo_sign =OFF;
  	//BYTE m_send_flag =0;
  	//long FADDR_RECORDER;
  	//unsigned char sc_num =0;
  	//BYTE count_num =0;
  	unsigned char *pTxPos,*pTxPos1;
  	RECORDER_CFG m_Recorder_cfg;

	if(m_SendListNum==0)
		bR_FileNum=0;
  	SendFrameHead(Style, Reason);
  	write_infoadd(InfoAddr);
  	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =2;  //在信息体地址后面，附加数据包类型1个字节，备用1,3,4.文件传输 2
  	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =OPERATING_LOGO2;
  
  	pTxPos = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0; //结果描述字
  
  	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID;
  	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID >>8;
  	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID >>16;
  	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = Directory_ID >>24;
  	pTxPos1 = &m_SendBuf.pBuf[m_SendBuf.wWritePtr];//后续标志
  	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =1; //1=有后续标志
  	m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =(m_SendListNum+1)*2; //文件数量
  	mRecorder_flag.LIST_flag = OFF;
  	CAT_SpiReadWords(EEPADD_LUBONUM, 1, (unsigned int*)&wave_total);
  	//if(g_gRunPara[RP_LBSTORAGE_TIME] >0)
    	{  
    	//LuBoGetNum(wave_total);//lubo_total = 
    	}
  	//if(g_gRunPara[RP_LUBO_NUM] >0)
    	{  
    	//LuBoNum(wave_total);//lubo_total = 
    	}
  	if(wave_total >= 0xFE)  wave_total = 0;

	for(;(m_SendListNum <=  (g_sRecData.m_gRecANum+g_sRecData.m_gACTRecANum+g_sRecData.m_gXHRecANum));m_SendListNum++,bR_FileNum++)
  		{
        m_Recorder_cfg.FileName = 0xff;
      	for(;bR_FileNum<MAX_ALLREC_NUM;bR_FileNum++)
       		{
          	if(bR_FileNum<(MAX_REC_NUM+1))
          		{//故障录波文件
          		if((bR_FileNum!=g_sRecData.m_gRecCNum)
					&&((bR_FileNum<g_sRecData.m_gRecANum)
					      ||((bR_FileNum==g_sRecData.m_gRecANum)&&(bR_FileNum==MAX_REC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_DATA-256+ (unsigned long)(bR_FileNum+1)*0x2000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&m_Recorder_cfg,sizeof(m_Recorder_cfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来 
					break;
					}
          		}
	   		else if(bR_FileNum<(MAX_REC_NUM+MAX_ACTREC_NUM+2))
	   			{//动作录波
          		if(((bR_FileNum-51)!=g_sRecData.m_gACTRecCNum)
					&&(((bR_FileNum-51)<g_sRecData.m_gACTRecANum)
					      ||(((bR_FileNum-51)==g_sRecData.m_gACTRecANum)&&((bR_FileNum-51)==MAX_ACTREC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_ACTDATA-256+ (unsigned long)(bR_FileNum-51+1)*0x90000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&m_Recorder_cfg,sizeof(m_Recorder_cfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来       			
					break;
					}
	   			}
	   		else if(bR_FileNum<MAX_ALLREC_NUM)
	   			{//熄弧录波
          		if(((bR_FileNum-62)!=g_sRecData.m_gXHRecCNum)
					&&(((bR_FileNum-62)<g_sRecData.m_gXHRecANum)
					      ||(((bR_FileNum-62)==g_sRecData.m_gXHRecANum)&&((bR_FileNum-62)==MAX_XHREC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_XHDATA-256+(unsigned long)(bR_FileNum-62+1)*0x8000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&m_Recorder_cfg,sizeof(m_Recorder_cfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来       			
					break;
					}	   		
	   			}
	   		else
	   			{//erro
	   			m_Recorder_cfg.FileName = 0xff;
	   			}
          	}				
     
  		//if((m_SendListNum > wave_total) ||(wave_total==0))//最后一段=
  		if((m_SendListNum >= (g_sRecData.m_gRecANum+g_sRecData.m_gACTRecANum+g_sRecData.m_gXHRecANum))
			||((g_sRecData.m_gRecANum+g_sRecData.m_gACTRecANum+g_sRecData.m_gXHRecANum)==0))
   			{ //最后一段=      
      		m_SendListNum = 0;
			bR_FileNum = 0;
      		if((g_sRecData.m_gRecANum+g_sRecData.m_gACTRecANum+g_sRecData.m_gXHRecANum)==0)
      			{
       			*pTxPos = 1;
       			*pTxPos1 = 0;//后续标志  
        		*(pTxPos1 + 1) = 0;//文件数量     
        		}
			*pTxPos1 = 0;//后续标志  
        	*(pTxPos1 + 1) = 0;//文件数量
      		SendFrameTail(PRM, dwCode, 0x01,0);
	  		return;
      		}
  		else
      		{
	  		sprintf((char *)ph,"BAY01_%04d_%04d%02d%02d_%02d%02d%02d_%03d.cfg",(m_Recorder_cfg.FileName),m_Recorder_cfg.comtrade_time[RTC_YEAR],m_Recorder_cfg.comtrade_time[RTC_MONTH],
			  	m_Recorder_cfg.comtrade_time[RTC_DATE],m_Recorder_cfg.comtrade_time[RTC_HOUR],m_Recorder_cfg.comtrade_time[RTC_MINUT],m_Recorder_cfg.comtrade_time[RTC_SEC],m_Recorder_cfg.comtrade_time[RTC_MICROSEC]);
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = strlen(ph);
	  		for(BYTE n = 0; n < strlen(ph); n++)
				m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = ph[n]; 
	  		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;//文件属性
	  		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= m_Recorder_cfg.CFG_Leng;//文件大小
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.CFG_Leng>>8;
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.CFG_Leng>>16; 
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.CFG_Leng>>24;
	  		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = (m_Recorder_cfg.comtrade_time[RTC_MICROSEC]+m_Recorder_cfg.comtrade_time[RTC_SEC]*1000)&0XFF;
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = (m_Recorder_cfg.comtrade_time[RTC_MICROSEC]+m_Recorder_cfg.comtrade_time[RTC_SEC]*1000)>>8;
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_MINUT];
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_HOUR];
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_MONTH];
      		if(m_Recorder_cfg.comtrade_time[RTC_YEAR] >=2000)
      			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_YEAR]-2000;
      		else
      			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_YEAR];
						
	  		sprintf((char *)ph,"BAY01_%04d_%04d%02d%02d_%02d%02d%02d_%03d.dat",(m_Recorder_cfg.FileName),m_Recorder_cfg.comtrade_time[RTC_YEAR],m_Recorder_cfg.comtrade_time[RTC_MONTH],
			  	m_Recorder_cfg.comtrade_time[RTC_DATE],m_Recorder_cfg.comtrade_time[RTC_HOUR],m_Recorder_cfg.comtrade_time[RTC_MINUT],m_Recorder_cfg.comtrade_time[RTC_SEC],m_Recorder_cfg.comtrade_time[RTC_MICROSEC]);
	  		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = strlen(ph);
	  		for(BYTE n = 0; n < strlen(ph); n++)
		  		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = ph[n]; 
	  		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;//文件属性
	  		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= m_Recorder_cfg.TOTAL_Leng*20;//文件大小
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = (m_Recorder_cfg.TOTAL_Leng*20)>>8;
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = (m_Recorder_cfg.TOTAL_Leng*20)>>16; 
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = (m_Recorder_cfg.TOTAL_Leng*20)>>24;
	 		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = (m_Recorder_cfg.comtrade_time[RTC_MICROSEC]+m_Recorder_cfg.comtrade_time[RTC_SEC]*1000)&0XFF;
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = (m_Recorder_cfg.comtrade_time[RTC_MICROSEC]+m_Recorder_cfg.comtrade_time[RTC_SEC]*1000)>>8;
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_MINUT];
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_HOUR];
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);
      		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_MONTH];
      		if(m_Recorder_cfg.comtrade_time[RTC_YEAR] >=2000)
      			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_YEAR]-2000;
      		else
      			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_YEAR];
			m_SendListNum++;bR_FileNum++;
			if(m_SendListNum >= (g_sRecData.m_gRecANum+g_sRecData.m_gACTRecANum+g_sRecData.m_gXHRecANum))
				*pTxPos1 = 0;//0=无后续标志
	  		SendFrameTail(PRM, dwCode, 0x01,0);
			mRecorder_flag.LIST_flag = ON; 
			
	  		return;
  	  		}
		}
  /*
  	{
      if(m_SendListNum == wave_total) m_SendListNum=0;
    
      if(call_sign ==1)
    	{
      	m_send_flag = Get_time_read(wave_total,start_minute_time,start_date_time,end_minute_time,end_date_time);
    	}
      for(int i=0;(m_SendListNum < wave_total)&&(i< 2);i++,m_SendListNum++)//i++
        {
      	  //flag_lb =0;
          lubo_sign = OFF;
          for(int j=0;j<3;j++)
          	{
            //long FADDR_RECORDER =FADDR_RECORDER_INFO+ (long)m_SendListNum*(long)FLINEADDR +(long)j*(long)FPHASEADDR; 
      		if(g_Recorder_Total_COUNT < FRECORDER_TOLNUM)//总条数小于64，从头开始取值 g_FRecorder_Current_COUNT;//flash保存到第几条了
      			{
        		FADDR_RECORDER =FADDR_RECORDER_INFO+ (long)m_SendListNum*(long)FLINEADDR +(long)j*(long)FPHASEADDR; 
        		count_num = m_SendListNum;
      			}
      		else
      			{
        
        		FADDR_RECORDER =FADDR_RECORDER_INFO+ (long)BK_FRecorder_Current_COUNT*(long)FLINEADDR +(long)j*(long)FPHASEADDR; 
        		count_num = BK_FRecorder_Current_COUNT;
      			}
      		Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&m_Recorder_cfg,sizeof(m_Recorder_cfg)); //(unsigned char *)不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来 
            //flag_lb++;
            if(m_Recorder_cfg.lubo_flag ==0x55)
            	{
              //flag_lb =0;
                break;
            	}
          	}
          BK_FRecorder_Current_COUNT++;
          if(BK_FRecorder_Current_COUNT >= FRECORDER_TOLNUM)
          	BK_FRecorder_Current_COUNT =0;
          if(m_SendListNum >= wave_total-1)//SOF状态
          	{
              //m_Recorder_cfg.CFG_SOF = 0x20;  //<0>:=后面还有目录文件；<1>:=最后目录文件 
            mRecorder_flag.LIST_flag = OFF;
        	mRecorder_flag.xuchuanflag= OFF;
            
            }      
          else
            {
              //m_Recorder_cfg.CFG_SOF = 0;
              mRecorder_flag.LIST_flag= ON;//TRUE;//新修改
            }
      //if(flag_lb >=3) 
        //continue;
          if((((lubo_valid[count_num] ==0x55)&&(g_gRunPara[RP_LBSTORAGE_TIME] > 0))||(g_gRunPara[RP_LBSTORAGE_TIME] == 0)) &&(((lubonum_valid[count_num] ==ON)&&(g_gRunPara[RP_LUBO_NUM] > 0))||(g_gRunPara[RP_LUBO_NUM] == 0)))
          	{
      		if(call_sign == 1)
      			{
        		date_time = m_Recorder_cfg.comtrade_time[RTC_DATE] ;
        		date_time+= (DWORD)m_Recorder_cfg.comtrade_time[RTC_MONTH]<< 8;
        		date_time+=((DWORD)m_Recorder_cfg.comtrade_time[RTC_YEAR]-2000)<<16;
        		minute_time = m_Recorder_cfg.comtrade_time[RTC_SEC];
        		minute_time += (DWORD)m_Recorder_cfg.comtrade_time[RTC_MINUT]<<8;
        		minute_time +=  (DWORD)m_Recorder_cfg.comtrade_time[RTC_HOUR] <<16;
        		if((date_time >= start_date_time) &&(date_time <=end_date_time)) 
        			{
          
                  	if((date_time == start_date_time) &&(date_time ==end_date_time))
                    	{
                      	if((minute_time >= start_minute_time ) &&(minute_time <= end_minute_time))
                        	{
              				lubo_sign = ON;//满足时间召唤条件条件
              				m_lubo_num++;
                        	}
                    	}
                    else if(date_time == start_date_time)
                    	{
                      	if(minute_time >= start_minute_time ) 
                        	{
                			lubo_sign = ON;//满足
                			m_lubo_num++;
                        	}
                    	}
                    else if(date_time ==end_date_time)
                  		{
                      	if(minute_time <= end_minute_time)
                        	{
                          	lubo_sign = ON;//满足
                            m_lubo_num++;
                        	}
                    	}
                    else
                    	{
                      	lubo_sign = ON;//满足
                      	m_lubo_num++;
                    	}
            
        		}
            	if((lubo_sign == OFF)&&(i==1)&&(m_SendListNum < wave_total))//每条报文发送2条CFG和dat,若其中一条不符合继续寻找
          		i=0;
		
      		}
	 
      	    if((call_sign == 0) ||((call_sign == 1) &&(lubo_sign ==ON)))//召唤全部录波或满足条件的录波
      			{
        		sc_num ++;

        		for(BYTE j = 0; j < 2; j++)
        			{
          			if(j ==0)
            			sprintf((char *)ph,"BAY01_%04d_%04d%02d%02d_%02d%02d%02d_%03d.cfg",(m_Recorder_cfg.lubo_total_num),m_Recorder_cfg.comtrade_time[RTC_YEAR],m_Recorder_cfg.comtrade_time[RTC_MONTH],
                            m_Recorder_cfg.comtrade_time[RTC_DATE],m_Recorder_cfg.comtrade_time[RTC_HOUR],m_Recorder_cfg.comtrade_time[RTC_MINUT],m_Recorder_cfg.comtrade_time[RTC_SEC],m_Recorder_cfg.comtrade_time[RTC_MICROSEC]);
          			else
            			sprintf((char *)ph,"BAY01_%04d_%04d%02d%02d_%02d%02d%02d_%03d.dat",(m_Recorder_cfg.lubo_total_num),m_Recorder_cfg.comtrade_time[RTC_YEAR],m_Recorder_cfg.comtrade_time[RTC_MONTH],
                            m_Recorder_cfg.comtrade_time[RTC_DATE],m_Recorder_cfg.comtrade_time[RTC_HOUR],m_Recorder_cfg.comtrade_time[RTC_MINUT],m_Recorder_cfg.comtrade_time[RTC_SEC],m_Recorder_cfg.comtrade_time[RTC_MICROSEC]);
                            m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = strlen(ph);
          			for(BYTE n = 0; n < strlen(ph); n++)
                  		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = ph[n]; 
          			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =0;//文件属性
            		if(j ==0)//文件大小需要修改
            			{
                		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]= m_Recorder_cfg.CFG_Leng;//文件大小
            			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.CFG_Leng>>8;
           				m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.CFG_Leng>>16; 
            			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.CFG_Leng>>24; 
                		}
            		else
             			{
               			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]=  m_Recorder_cfg.DATA_Leng;//文件大小
               			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.DATA_Leng>>8;
              			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.DATA_Leng>>16; 
              			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.DATA_Leng>>24; 
                		}
          			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = (m_Recorder_cfg.comtrade_time[RTC_MICROSEC]+m_Recorder_cfg.comtrade_time[RTC_SEC]*1000)&0XFF;
          			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ]  = (m_Recorder_cfg.comtrade_time[RTC_MICROSEC]+m_Recorder_cfg.comtrade_time[RTC_SEC]*1000)>>8;
          			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_MINUT];
         			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_HOUR];
          			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_DATE]|(m_Recorder_cfg.comtrade_time[RTC_WEEK]<<5);
          			m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_MONTH];
          			if(m_Recorder_cfg.comtrade_time[RTC_YEAR] >=2000)
                  		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_YEAR]-2000;
          			else
                  		m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = m_Recorder_cfg.comtrade_time[RTC_YEAR];
            		}
      	   		}
          }
       
   }
      
  	  }
    *pTxPos = 0;
	if(call_sign == 0)
    	{
        if(m_SendListNum >= wave_total)//+218每报数传8条录波数据，每条对应CFG DAT两个目录
      		*pTxPos1 = 0;//后续标志
        else
      		*pTxPos1 = 1;//后续标志
    	}
    else if(call_sign == 1)
    	{
      	if(m_lubo_num >= m_send_flag)
      		{
           	*pTxPos1 = 0;//后续标志 
            }
      	else
      		{ 
          	*pTxPos1 = 1;//后续标志
      		}
    }
 	if(mRecorder_flag.LIST_flag == OFF)
  		{
      	m_SendListNum = 0;
        }
  //if(g_gRunPara[RP_LBSTORAGE_TIME] > 0)
  //{
    *(pTxPos1+1) = sc_num*2;
	if(sc_num ==0)//按条件查询，没查询到相关的录波
        {
      	mRecorder_flag.xuchuanflag= OFF;
	  	*pTxPos = 1;//结果描述字失败 
		*pTxPos1 =0;
       	if(m_lubo_num ==0)//确认回复时该变量不为0
			SendFrameTail(PRM, dwCode, 0x01,0);
      	else
      		return;
        }
   	else
    	SendFrameTail(PRM, dwCode, 0x01,0);
*/
    
}

void CBJ101S::RecReadParaData(BYTE Style,BYTE VSQ)
{/*
   BYTE Reason = 0X05;
  BYTE PRM = 0, dwCode = 3;
  BYTE cpypara=0;
  WORD wInfoAddr,InfoAddr;
  wInfoAddr = g_RWFiInf.Fi_Info;
  SendFrameHead(Style, Reason);

  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(g_RWFiInf.Fi_Info);
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(g_RWFiInf.Fi_Info);
  cpypara = g_RWFiInf.Fi_Info_Num;

    for(BYTE i =0 ;i < cpypara;i+=2)
  {
    InfoAddr =wInfoAddr+(i>>1);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_RWFiInf.Fi_InfoPara[(InfoAddr - 0x5001)*2];
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_RWFiInf.Fi_InfoPara[(InfoAddr - 0x5001)*2 +1];
  } 
  SendFrameTail(PRM,dwCode, (cpypara/2) | VSQ,0);//SET ACD
}

void CBJ101S::RecWriteData(BYTE Style,BYTE mQos,BYTE VSQ)
{
  //BYTE Style = 125;
    BYTE Reason = 0X07;
  BYTE PRM = 0, dwCode = 3;
    //BYTE VSQ=0;//0x80;
  BYTE cpypara=0;
  WORD wInfoAddr,InfoAddr;
  //BYTE * pData = &pReceiveFrame->Frame68.Data[m_byInfoShift];
  wInfoAddr = g_RWFiInf.Fi_Info;
    SendFrameHead(Style, Reason);
   
    //BYTE byInfoLen = m_guiyuepara.infoaddlen;
    //if(m_guiyuepara.infoaddlen > 3) byInfoLen = 2;
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = LOBYTE(g_RWFiInf.Fi_Info);
  m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] = HIBYTE(g_RWFiInf.Fi_Info);
  cpypara = g_RWFiInf.Fi_Info_Num;
  for(BYTE i =0 ;i < cpypara;i+=2)
  {
    InfoAddr = wInfoAddr +(i>>1);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_RWFiInf.Fi_InfoPara[(InfoAddr - 0x5001)*2];
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++ ] =g_RWFiInf.Fi_InfoPara[(InfoAddr - 0x5001)*2 +1];
  }
    //memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],pData,byInfoLen + cpypara);
    //m_SendBuf.wWritePtr += (byInfoLen + cpypara); 
  
  m_SendBuf.pBuf[ (m_SendBuf.wWritePtr++) ] = mQos;
    SendFrameTail(PRM,dwCode, (cpypara/2) | VSQ,0);//SET ACD
   
    //memcpy(&m_SendBuf.pBuf[m_SendBuf.wWritePtr],&pReceiveFrame->Frame68.InfoAddr[0],cpypara + 2 );
    //m_SendBuf.wWritePtr += 6;//信息体地址2+文件名2+节名1+节长度1
      //SendFrameTailForPa();
}
void CBJ101S::RecFIUpdata(BYTE Type)
{
  BYTE pCSPos;
    WORD wLinkAddress;
  if(Type ==0XDB)
    m_tra_FI = ON;
  else
    m_tra_FI = OFF;
  m_SendBuf.wReadPtr = 0;
    m_SendBuf.wWritePtr=0;
    pSendFrame = (VIec101Frame *)m_SendBuf.pBuf;
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr]=0x68;
      m_SendBuf.wWritePtr+=3;
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++]=0x68;
    pCSPos = m_SendBuf.wWritePtr;
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++]=0xD3;
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++]= LOBYTE(g_gRunPara[RP_COMM_ADDR]);//m
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++]= HIBYTE(g_gRunPara[RP_COMM_ADDR]);//m
      m_SendBuf.pBuf[ m_SendBuf.wWritePtr++]=Type;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] =0;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] =0;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] =0;
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] =0;
   
    m_SendBuf.pBuf[1] = m_SendBuf.wWritePtr - pCSPos;//LL
      m_SendBuf.pBuf[2] = m_SendBuf.wWritePtr - pCSPos;//LL
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = (BYTE)ChkSum((BYTE *)&(m_SendBuf.pBuf[4]),m_SendBuf.pBuf[1]);//AddByte(&(m_SendBuf.pBuf[4]),m_SendBuf.pBuf[1]);
    m_SendBuf.pBuf[ m_SendBuf.wWritePtr++] = 0x16;
    wLinkAddress = m_dwasdu.LinkAddr;
    WriteToComm(wLinkAddress);
  */
}
#ifndef YN_101S
void CBJ101S::RecReadData(void)
{/*
  BYTE *pData = &pReceiveFrame->Frame68.Data[m_byInfoShift];
  WORD InfoAddr = MAKEWORD(pData[0],pData[1]);
  WORD wInfoAddr =0;

  BYTE bVsq = m_dwasdu.VSQ;
  BYTE bQos = (bVsq & 0x7F) *2;//参数值后面一个字节表示是预制命令还是激活命令
  g_RWFiInf.Fi_Info_Num = bQos;
  
  g_RWFiInf.Fi_Info = InfoAddr;
  g_RWFiInf.Fi_rflag = OFF;
  for( BYTE i =0; i<bQos;i+=2 )
  {
      wInfoAddr = InfoAddr+(i>>1);
      
      if((wInfoAddr == 0x5001)||(wInfoAddr == 0x5004)||(wInfoAddr == 0x5007)||(wInfoAddr == 0x500A)||(wInfoAddr == 0x5010)||(wInfoAddr == 0x5012)||(wInfoAddr == 0x5013))
      {
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2] = 0;
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2+1] = 0;
      }
      else if(wInfoAddr == 0x5002)
      {
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2] = LOBYTE(g_gRunPara[RP_ALARM_BAT]);
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2+1] = HIBYTE(g_gRunPara[RP_ALARM_BAT]); 
      }
      else if(wInfoAddr == 0x5003)
      {
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2] = LOBYTE(g_gRunPara[RP_LBSTORAGE_TIME]);
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2+1]= HIBYTE(g_gRunPara[RP_LBSTORAGE_TIME]); 
      }
      else if(wInfoAddr == 0x5005)
      {
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2] = LOBYTE(g_gRunPara[RP_LUBO_NUM]);
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2+1] = HIBYTE(g_gRunPara[RP_LUBO_NUM]); 
      }
      else if(wInfoAddr == 0x5006)
      {
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2] = LOBYTE(g_gRunPara[RP_SENDYC_T]);
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2+1] = HIBYTE(g_gRunPara[RP_SENDYC_T]); 
      }
      else if(wInfoAddr == 0x500C)
      {
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2] = LOBYTE(g_gRunPara[RP_LUBO_CYCLE]);
        g_RWFiInf.Fi_InfoPara[(wInfoAddr - 0x5001)*2+1]= HIBYTE(g_gRunPara[RP_LUBO_CYCLE]); 
      }
      else if((wInfoAddr == 0x5008)||(wInfoAddr == 0x5009)||(wInfoAddr == 0x500B)||(wInfoAddr == 0x500D)||(wInfoAddr == 0x500E)||(wInfoAddr == 0x500F)||(wInfoAddr == 0x5011)||(wInfoAddr == 0x5014))
      {
        if(m_uartId == g_CmIdGPRS)
        {
            g_ZsqCmdRBit |= BIT[8];
        }
        else if(m_uartId == g_CmIdDBG)
        {
           g_ZsqCmdRBit |= BIT[9];    
        }  
        g_RWFiInf.Fi_rflag = ON;
        g_RWFiInf.FiphaseA_flag = ON;
        g_RWFiInf.FiphaseB_flag = OFF;
        g_RWFiInf.FiphaseC_flag = OFF;
      }
        
  }
  if(g_RWFiInf.Fi_rflag == OFF)
  {
      if(g_RWFiInf.Fi_Info_Num <=2)
        RecReadParaData(102,0);
      else  
        RecReadParaData(132,0x80);  
  }  */
}
#endif

#endif /*#ifdef INCLUDE_GB101_S*/
