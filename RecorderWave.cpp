#ifndef  RECORDER_WAVE_GLOBALS

#define  RECORDER_WAVE_GLOBALS
  #include "DataStruct.h"
#endif

/*extern char ComtrderCfg1[]=
"The Recording File of Indicator 108,1999\n\
8,8A,0D\n\
1,Ia,A,,A,1,0,0,0,65535,1,1,S\n\
2,Ib,B,,A,1,0,0,0,65535,1,1,S\n\
3,Ic,C,,A,1,0,0,0,65535,1,1,S\n\
4,I0,0,,A,1,0,0,0,65535,1,1,S\n\
5,Ua,A,,V,1,0,0,0,65535,1,1,S\n\
6,Ub,B,,V,1,0,0,0,65535,1,1,S\n\
7,Uc,C,,V,1,0,0,0,65535,1,1,S\n\
8,U0,0,,V,1,0,0,0,65535,1,1,S\n\
50\n\
1\n\
4000,1200\n\
10/03/15,12:20:27.672\n\
10/03/15,12:20:27.772\n\
ASCII\n\
1";*/

//static unsigned char ReadBack[256] = {0};//读取文件中的信息

static unsigned int  wSendCFGNum = 0;////已经发送的配置文件的字节数，控制多帧传送用
static unsigned int  wSendDATNum = 0;////已经发送的数据文件的字节数，控制多帧传送用
static unsigned int  wSendLISTNum = 0;////已经发送的目录的条数
static unsigned int  wFileNum = 0;////已经发送的目录的条数
static unsigned int  wSendDATNumOld = 0;
static char *strtemp ; //CFG字符串的指针
 //wCfgTotalNum =1;//总长度static WORD 
 //section_current = 1;//当前节*/static WORD
//gRecorder_flag.pRXBuff[256] = {0}; //将接收的68长帧保存下来，因为发送段信息的时候不发送长帧


unsigned char ChkluboSum(BYTE *buf, unsigned int len)
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

//***********************************************************************************
//*函数名：cfg_dat_length
//*功能：由于。CFG文件是定义的内容，需要对定义的.CFG文件修改其录波开始的时间和录波结束的时间
//*描述: 并计算其.CFG的长度
//***********************************************************************************
void cfg_dat_length(int file_current_num)
{
     char ch[100]; 
     unsigned int tt=0,xt;
     char i;	 
    {  //.CFG的长度
       //添加站名
      memcpy(ComtrderCfg1,g_gLBName,g_gLBNameLen);
      tt += g_gLBNameLen;	
      memcpy(&ComtrderCfg1[tt],",",1);//添加","
      tt +=1;	  
	//添加站地址
      sprintf((char *)ch,"%0d",g_gRunPara[RP_COMM_ADDR]);        
      memcpy(&ComtrderCfg1[tt],ch,strlen(ch));
      tt +=strlen(ch);	  
      //添加主体
      //memcpy(&ComtrderCfg1[tt],ComtrderCfg,strlen(ComtrderCfg));//添加主体
      //tt +=strlen(ComtrderCfg);
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_1,strlen(ComtrderCfg_1));//添加主体
      tt +=strlen(ComtrderCfg_1);

      memcpy(&ComtrderCfg1[tt],ComtrderCfg_ua,strlen(ComtrderCfg_ua));//添加主体
      tt +=strlen(ComtrderCfg_ua);
      for(i = 0; (i< 6); i++,tt++)
	  	{ComtrderCfg1[tt]=ComtrderCfg_adj[i];}	  
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_2,strlen(ComtrderCfg_2));//添加主体
      tt +=strlen(ComtrderCfg_2);

      memcpy(&ComtrderCfg1[tt],ComtrderCfg_ub,strlen(ComtrderCfg_ub));//添加主体
      tt +=strlen(ComtrderCfg_ub);
      for(i = 0; (i< 6); i++,tt++)
	  	{ComtrderCfg1[tt]=ComtrderCfg_adj[i+6];}	  
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_2,strlen(ComtrderCfg_2));//添加主体
      tt +=strlen(ComtrderCfg_2);	  

      memcpy(&ComtrderCfg1[tt],ComtrderCfg_uc,strlen(ComtrderCfg_uc));//添加主体
      tt +=strlen(ComtrderCfg_uc);
      for(i = 0; (i< 6); i++,tt++)
	  	{ComtrderCfg1[tt]=ComtrderCfg_adj[12+i];}	  
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_2,strlen(ComtrderCfg_2));//添加主体
      tt +=strlen(ComtrderCfg_2);

      memcpy(&ComtrderCfg1[tt],ComtrderCfg_u0,strlen(ComtrderCfg_u0));//添加主体
      tt +=strlen(ComtrderCfg_u0);
      for(i = 0; (i< 6); i++,tt++)
	  	{ComtrderCfg1[tt]=ComtrderCfg_adj[i+18];}	  
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_2,strlen(ComtrderCfg_2));//添加主体
      tt +=strlen(ComtrderCfg_2);

      memcpy(&ComtrderCfg1[tt],ComtrderCfg_i0,strlen(ComtrderCfg_i0));//添加主体
      tt +=strlen(ComtrderCfg_i0);
      for(i = 0; (i< 6); i++,tt++)
	  	{ComtrderCfg1[tt]=ComtrderCfg_adj[i+24];}  
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_2,strlen(ComtrderCfg_2));//添加主体
      tt +=strlen(ComtrderCfg_2);
	  
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_3,strlen(ComtrderCfg_3));//添加主体
      tt +=strlen(ComtrderCfg_3);
	//添加采样频率
      if(gRecorder_Readfilecfg.CFG_Samp==4000)
      	{	  
      		memcpy(&ComtrderCfg1[tt],RECCfg,strlen(RECCfg));
		tt +=strlen(RECCfg);	
      	}
	else if(gRecorder_Readfilecfg.CFG_Samp==2000)
	  	{
	  	memcpy(&ComtrderCfg1[tt],RECCfg,strlen(RECCfg));
		tt +=strlen(RECCfg);	
	  	}
	else //if(gRecorder_filecfg.CFG_Samp==800)  
      	{	//gRecorder_cfg[temp[1]].CFG_EndSamp  
      		memcpy(&ComtrderCfg1[tt],ACTCfg,5);
		tt +=5;
		sprintf((char *)ch,"%u\n",gRecorder_Readfilecfg.CFG_EndSamp);
		memcpy(&ComtrderCfg1[tt],ch,strlen(ch));
              tt +=strlen(ch);
      	}	  
      //添加日期 
      if(gRecorder_Readfilecfg.CFG_Samp==4000)
      	{      
#ifdef SD_101S
      	   if(gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC]<320){xt=320;}
#else
      	   if(gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC]<160){xt=160;}
#endif		   
	  	else {xt=gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC];}
      	   sprintf((char *)ch,"%4d/%02d/%02d,%02d:%02d:%02d.%03d\n",gRecorder_Readfilecfg.comtrade_time[RTC_YEAR],gRecorder_Readfilecfg.comtrade_time[RTC_MONTH],
                                                                gRecorder_Readfilecfg.comtrade_time[RTC_DATE],gRecorder_Readfilecfg.comtrade_time[RTC_HOUR],
#ifdef SD_101S
                                                                gRecorder_Readfilecfg.comtrade_time[RTC_MINUT],gRecorder_Readfilecfg.comtrade_time[RTC_SEC],(xt-320));
#else
                                                                gRecorder_Readfilecfg.comtrade_time[RTC_MINUT],gRecorder_Readfilecfg.comtrade_time[RTC_SEC],(xt-160));
 #endif
         memcpy(&ComtrderCfg1[tt],ch,strlen(ch));
         tt +=strlen(ch);
         sprintf((char *)ch,"%4d/%02d/%02d,%02d:%02d:%02d.%03d\n",gRecorder_Readfilecfg.comtrade_time[RTC_YEAR],gRecorder_Readfilecfg.comtrade_time[RTC_MONTH],
                                                                gRecorder_Readfilecfg.comtrade_time[RTC_DATE],gRecorder_Readfilecfg.comtrade_time[RTC_HOUR],
                                                                gRecorder_Readfilecfg.comtrade_time[RTC_MINUT],gRecorder_Readfilecfg.comtrade_time[RTC_SEC],(xt));
	  
         memcpy(&ComtrderCfg1[tt],ch,strlen(ch));
         tt +=strlen(ch);	
      	}
	if(gRecorder_Readfilecfg.CFG_Samp==2000)
		{//山东版  采样频率均为2000
		if(file_current_num<(MAX_REC_NUM+1))
			{
			if(gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC]<320)
				{xt=320;}
			else 
				{xt=gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC];}
			sprintf((char *)ch,"%4d/%02d/%02d,%02d:%02d:%02d.%03d\n",gRecorder_Readfilecfg.comtrade_time[RTC_YEAR],gRecorder_Readfilecfg.comtrade_time[RTC_MONTH],
                                                                gRecorder_Readfilecfg.comtrade_time[RTC_DATE],gRecorder_Readfilecfg.comtrade_time[RTC_HOUR],
                                                                gRecorder_Readfilecfg.comtrade_time[RTC_MINUT],gRecorder_Readfilecfg.comtrade_time[RTC_SEC],(xt-320));
         		memcpy(&ComtrderCfg1[tt],ch,strlen(ch));
         		tt +=strlen(ch);
         		sprintf((char *)ch,"%4d/%02d/%02d,%02d:%02d:%02d.%03d\n",gRecorder_Readfilecfg.comtrade_time[RTC_YEAR],gRecorder_Readfilecfg.comtrade_time[RTC_MONTH],
                                                                gRecorder_Readfilecfg.comtrade_time[RTC_DATE],gRecorder_Readfilecfg.comtrade_time[RTC_HOUR],
                                                                gRecorder_Readfilecfg.comtrade_time[RTC_MINUT],gRecorder_Readfilecfg.comtrade_time[RTC_SEC],(xt));
	  
         		memcpy(&ComtrderCfg1[tt],ch,strlen(ch));
         		tt +=strlen(ch);			
			}
		else
			{
			sprintf((char *)ch,"%4d/%02d/%02d,%02d:%02d:%02d.%03d\n",gRecorder_Readfilecfg.comtrade_time[RTC_YEAR],gRecorder_Readfilecfg.comtrade_time[RTC_MONTH],
                                                                gRecorder_Readfilecfg.comtrade_time[RTC_DATE],gRecorder_Readfilecfg.comtrade_time[RTC_HOUR],
                                                                gRecorder_Readfilecfg.comtrade_time[RTC_MINUT],gRecorder_Readfilecfg.comtrade_time[RTC_SEC],gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC]);
 
            		memcpy(&ComtrderCfg1[tt],ch,strlen(ch));
            		tt +=strlen(ch);
	      		sprintf((char *)ch,"%4d/%02d/%02d,%02d:%02d:%02d.%03d\n",gRecorder_Readfilecfg.comtrade_time1[RTC_YEAR],gRecorder_Readfilecfg.comtrade_time1[RTC_MONTH],
                                                                gRecorder_Readfilecfg.comtrade_time1[RTC_DATE],gRecorder_Readfilecfg.comtrade_time1[RTC_HOUR],
                                                                gRecorder_Readfilecfg.comtrade_time1[RTC_MINUT],gRecorder_Readfilecfg.comtrade_time1[RTC_SEC],gRecorder_Readfilecfg.comtrade_time1[RTC_MICROSEC]);
 			
	     		memcpy(&ComtrderCfg1[tt],ch,strlen(ch));
            		tt +=strlen(ch);
			}
		}
	else //if(gRecorder_filecfg.CFG_Samp==800)
	{
	      sprintf((char *)ch,"%4d/%02d/%02d,%02d:%02d:%02d.%03d\n",gRecorder_Readfilecfg.comtrade_time[RTC_YEAR],gRecorder_Readfilecfg.comtrade_time[RTC_MONTH],
                                                                gRecorder_Readfilecfg.comtrade_time[RTC_DATE],gRecorder_Readfilecfg.comtrade_time[RTC_HOUR],
                                                                gRecorder_Readfilecfg.comtrade_time[RTC_MINUT],gRecorder_Readfilecfg.comtrade_time[RTC_SEC],gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC]);
 
            memcpy(&ComtrderCfg1[tt],ch,strlen(ch));
            tt +=strlen(ch);
	      sprintf((char *)ch,"%4d/%02d/%02d,%02d:%02d:%02d.%03d\n",gRecorder_Readfilecfg.comtrade_time1[RTC_YEAR],gRecorder_Readfilecfg.comtrade_time1[RTC_MONTH],
                                                                gRecorder_Readfilecfg.comtrade_time1[RTC_DATE],gRecorder_Readfilecfg.comtrade_time1[RTC_HOUR],
                                                                gRecorder_Readfilecfg.comtrade_time1[RTC_MINUT],gRecorder_Readfilecfg.comtrade_time1[RTC_SEC],gRecorder_Readfilecfg.comtrade_time1[RTC_MICROSEC]);
 			
	     memcpy(&ComtrderCfg1[tt],ch,strlen(ch));
            tt +=strlen(ch);		

	 }

      //添加结尾
	if(g_gRunPara[RP_CFG_KEY]&BIT[RP_COMTRADE_TYPE])
		{
      		memcpy(&ComtrderCfg1[tt],CfgEnd1,strlen(CfgEnd1));
      		tt +=strlen(CfgEnd1);		
		}
	else
		{
      		memcpy(&ComtrderCfg1[tt],CfgEnd,strlen(CfgEnd));
      		tt +=strlen(CfgEnd);
		}
      gRecorder_Readfilecfg.CFG_Leng=tt;//strlen(ComtrderCfg1);
      strtemp = ComtrderCfg1;
    
    }
          
}
/*
void cfg_dat_length(int file_current_num)
{
     char ch[100],ch1[10]; 
     unsigned int time_start,time_end;
    {  //.CFG的长度
      memcpy(ch,g_gLBName,g_gLBNameLen);
      sprintf((char *)ch1,"%0d",g_gRunPara[RP_COMM_ADDR]);
      memcpy(ch,ch1,strlen(ch1));	  
      sprintf((char *)ch,"%4d/%02d/%02d,%02d:%02d:%02d.%03d\n",gRecorder_cfg[file_current_num].comtrade_time[RTC_YEAR],gRecorder_cfg[file_current_num].comtrade_time[RTC_MONTH],
                                                                gRecorder_cfg[file_current_num].comtrade_time[RTC_DATE],gRecorder_cfg[file_current_num].comtrade_time[RTC_HOUR],
                                                                gRecorder_cfg[file_current_num].comtrade_time[RTC_MINUT],gRecorder_cfg[file_current_num].comtrade_time[RTC_SEC],gRecorder_cfg[file_current_num].comtrade_time[RTC_MICROSEC]);

      //gRecorder_cfg[file_current_num].CFG_Leng = strlen(ComtrderCfg1);//+strlen(ComtrderCfg2)+strlen(ch)*2;
      
      time_start = CRG_TIME_START;
      time_end = CRG_TIME_END;
      memcpy(&ComtrderCfg1[time_start],ch,strlen(ch));
      memcpy(&ComtrderCfg1[time_end],ch,strlen(ch));
      strtemp = ComtrderCfg1;
    
    }
          
}
*/
//***********************************************************************************
//*函数名：FileDirectory
//*功能：响应主站的召唤目录的命令
//*描述: 所存所有录波的目录文件
//***********************************************************************************
#ifdef YN_101S
//****************云南录波文件*****************************
unsigned char * FileDirectory(unsigned char *pTxBuf, unsigned char leng,unsigned int wave_total)//召唤目录
{
      unsigned int VSQ=0x80;
      char j=0;
      //unsigned int i;
      unsigned char *ptoBuf;

      ptoBuf = pTxBuf;

      pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]] = CALL_TYPE_YN;
      //pTxBuf[6+g_ucPara101[IECP_LINKADDR_NUM]] = wave_total| VSQ;//张弢 长度最后写
      pTxBuf += leng;
      gRecorder_flag.LIST_flag = OFF;

       for(;(wSendLISTNum <=  (g_sRecData.m_gRecANum+g_sRecData.m_gACTRecANum+g_sRecData.m_gXHRecANum));wSendLISTNum++,wFileNum++)
        {
        gRecorder_Readfilecfg.FileName = 0xff;
          for(;wFileNum<MAX_ALLREC_NUM;wFileNum++)
          	{
          	if(wFileNum<(MAX_REC_NUM+1))
          		{//故障录波文件
          		if((wFileNum!=g_sRecData.m_gRecCNum)
					&&((wFileNum<g_sRecData.m_gRecANum)
					      ||((wFileNum==g_sRecData.m_gRecANum)&&(wFileNum==MAX_REC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_DATA-256+ (unsigned long)(wFileNum+1)*0x2000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来 
				break;
				}
          		}
	   	else if(wFileNum<(MAX_REC_NUM+MAX_ACTREC_NUM+2))
	   		{//动作录波
          		if(((wFileNum-51)!=g_sRecData.m_gACTRecCNum)
					&&(((wFileNum-51)<g_sRecData.m_gACTRecANum)
					      ||(((wFileNum-51)==g_sRecData.m_gACTRecANum)&&((wFileNum-51)==MAX_ACTREC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_ACTDATA-256+ (unsigned long)(wFileNum-51+1)*0x90000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来       			
				break;
				}
	   		}
	   	else if(wFileNum<MAX_ALLREC_NUM)
	   		{//熄弧录波
          		if(((wFileNum-62)!=g_sRecData.m_gXHRecCNum)
					&&(((wFileNum-62)<g_sRecData.m_gXHRecANum)
					      ||(((wFileNum-62)==g_sRecData.m_gXHRecANum)&&((wFileNum-62)==MAX_XHREC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_XHDATA-256+(unsigned long)(wFileNum-62+1)*0x8000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来       			
				break;
				}	   		
	   		}
	   	else
	   		{//erro
	   		gRecorder_Readfilecfg.FileName = 0xff;
	   		}
          	}	
       	}
	//if(gRecorder_Readfilecfg.FileName = 0xff)	
	if((wSendLISTNum >= (g_sRecData.m_gRecANum+g_sRecData.m_gACTRecANum+g_sRecData.m_gXHRecANum-1))
		||((g_sRecData.m_gRecANum+g_sRecData.m_gACTRecANum+g_sRecData.m_gXHRecANum)==0))
		{//文件目录发送完，发送文件目录结束帧
		gRecorder_Readfilecfg.CFG_SOF = 0x20;  //<0>:=后面还有目录文件；<1>:=最后目录文件  
            	gRecorder_flag.LIST_flag = OFF;
	     //总包1字节 当前包1字节
          	*pTxBuf++ = LOBYTE(wSendLISTNum/*wave_total*/);	   
          	*pTxBuf++ = LOBYTE(wSendLISTNum);	  
	   	ptoBuf[6+g_ucPara101[IECP_LINKADDR_NUM]] = VSQ;	//张弢确定长度	
	   	ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 10;	//传送原因=10
      	   	wSendLISTNum = 0;     
          	return pTxBuf;
		}
	else
		{
		gRecorder_Readfilecfg.CFG_SOF = 0;
            	gRecorder_flag.LIST_flag = ON;
//		}
/*	
      for(i=wSendLISTNum;(wSendLISTNum <=(g_sRecData.m_gRecANum+g_sRecData.m_gACTRecANum+g_sRecData.m_gXHRecANum));i++,wSendLISTNum++)
      	{
          long FADDR_RECORDER =FADDR_RECORDER_INFO+ (long)wSendLISTNum*(long)FLINEADDR;
          Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_cfg[wSendLISTNum],sizeof(gRecorder_cfg[wSendLISTNum])); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来 
          if(wSendLISTNum >= (wave_total))//SOF状态-1
          {//发结束帧
            gRecorder_cfg[wSendLISTNum].CFG_SOF = 0x20;  //<0>:=后面还有目录文件；<1>:=最后目录文件  
            gRecorder_flag.LIST_flag = OFF;
	     //总包1字节 当前包1字节
          *pTxBuf++ = LOBYTE(wave_total);
          *pTxBuf++ = LOBYTE(wSendLISTNum);
	   ptoBuf[6+g_ucPara101[IECP_LINKADDR_NUM]] = VSQ;	//张弢确定长度	
	   ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 10;	//传送原因=10
      	   wSendLISTNum = 0;     
          return pTxBuf;
          }      
          else
          {
            gRecorder_cfg[wSendLISTNum].CFG_SOF = 0;
            gRecorder_flag.LIST_flag = ON;
          }
	   if(gRecorder_cfg[wSendLISTNum].FileName<MAX_REC_NUM) break;	  
      	}
*/      
//          if(gRecorder_cfg[wSendLISTNum].FileName<MAX_REC_NUM)//filename 不能大于MAX_REC_NUM，可能被擦除
//          {
          //总包1字节 当前包1字节
          *pTxBuf++ = LOBYTE(wave_total);
	   //*pTxBuf++ = HIBYTE(wave_total);	
          *pTxBuf++ = LOBYTE(wSendLISTNum);
	   //*pTxBuf++ = HIBYTE(wSendLISTNum);	   
          //cfg文件名 8字节 7字节时间+1字节编号	   
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC];//录波触点时间
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(gRecorder_Readfilecfg.comtrade_time[RTC_WEEK]<<5);
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_Readfilecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR];
		*pTxBuf++ = (gRecorder_Readfilecfg.FileName<<1) &0xfe;//0位=0， cfg文件
          //文件长度 3字节      
          //*pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng;
         // *pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng>>8;
         // *pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng>>16; 
         *pTxBuf++ = 3;
	  *pTxBuf++ = 0x01;	  
	   *pTxBuf++ = 0;	 
          //SOF 1字节 	
        *pTxBuf++ = gRecorder_Readfilecfg.CFG_SOF;
          //文件创建时间 7字节       
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC];//录波触点时间
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(gRecorder_Readfilecfg.comtrade_time[RTC_WEEK]<<5);
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_Readfilecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR];
	   //*****************************************************	
          //data文件名 8字节 7字节时间+1字节编号		   
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC];//录波触点时间
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(gRecorder_Readfilecfg.comtrade_time[RTC_WEEK]<<5);
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_Readfilecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR];	
		*pTxBuf++ = (gRecorder_Readfilecfg.FileName<<1) |0x01;////0位=1， data文件
          //文件长度 3字节      
          *pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng*20;
          *pTxBuf++ = (gRecorder_Readfilecfg.TOTAL_Leng*20)>>8;
          *pTxBuf++ = (gRecorder_Readfilecfg.TOTAL_Leng*20)>>16;  
          //SOF 1字节 	
        *pTxBuf++ = gRecorder_Readfilecfg.CFG_SOF;
          //文件创建时间 7字节       
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC];//录波触点时间
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(gRecorder_Readfilecfg.comtrade_time[RTC_WEEK]<<5);
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_Readfilecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR];		
	j+=2;	
        }
      
	ptoBuf[6+g_ucPara101[IECP_LINKADDR_NUM]] = j| VSQ;	//张弢确定长度
	wSendLISTNum++;            
       if( gRecorder_flag.LIST_flag == OFF)wSendLISTNum = 0;
     
     return pTxBuf;
}
//****************云南录波文件*****************************
#else

unsigned char * FileDirectory(unsigned char *pTxBuf, unsigned char leng,unsigned int wave_total)//召唤目录
{
      unsigned int VSQ=0x80;
      char j=0;
      //char filenum=0;
      //char k=0;
      unsigned char *ptoBuf;

      ptoBuf = pTxBuf;
      pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]] = CATALOG_TYPE;

      //pTxBuf[6+g_ucPara101[IECP_LINKADDR_NUM]] = wave_total| VSQ;//张弢 长度最后写
      pTxBuf += leng;
      gRecorder_flag.LIST_flag = OFF;

      if((g_sRecData.m_gACTRecANum==0)&&(g_sRecData.m_gRecANum==0)&&(g_sRecData.m_gXHRecANum==0))
      { //无录波文件      
          wSendLISTNum = 0;   wFileNum=0;      
            for(int i=0;i<13;i++)
            {
              *pTxBuf++=0;
            }          
      }
      else
      {
        for(int i=0;(wSendLISTNum <=  (g_sRecData.m_gRecANum+g_sRecData.m_gACTRecANum+g_sRecData.m_gXHRecANum))&&(i< 18);i++,wSendLISTNum++,wFileNum++)
        {
        gRecorder_Readfilecfg.FileName = 0xff;
          for(;wFileNum<MAX_ALLREC_NUM;wFileNum++)
          	{
          	if(wFileNum<(MAX_REC_NUM+1))
          		{//故障录波文件
          		if((wFileNum!=g_sRecData.m_gRecCNum)
					&&((wFileNum<g_sRecData.m_gRecANum)
					      ||((wFileNum==g_sRecData.m_gRecANum)&&(wFileNum==MAX_REC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_DATA-256+ (unsigned long)(wFileNum+1)*0x2000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来 
				break;
				}
          		}
	   	else if(wFileNum<(MAX_REC_NUM+MAX_ACTREC_NUM+2))
	   		{//动作录波
          		if(((wFileNum-51)!=g_sRecData.m_gACTRecCNum)
					&&(((wFileNum-51)<g_sRecData.m_gACTRecANum)
					      ||(((wFileNum-51)==g_sRecData.m_gACTRecANum)&&((wFileNum-51)==MAX_ACTREC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_ACTDATA-256+ (unsigned long)(wFileNum-51+1)*0x90000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来       			
				break;
				}
	   		}
	   	else if(wFileNum<MAX_ALLREC_NUM)
	   		{//熄弧录波
          		if(((wFileNum-62)!=g_sRecData.m_gXHRecCNum)
					&&(((wFileNum-62)<g_sRecData.m_gXHRecANum)
					      ||(((wFileNum-62)==g_sRecData.m_gXHRecANum)&&((wFileNum-62)==MAX_XHREC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_XHDATA-256+(unsigned long)(wFileNum-62+1)*0x8000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来       			
				break;
				}	   		
	   		}
	   	else
	   		{//erro
	   		gRecorder_Readfilecfg.FileName = 0xff;
	   		}
          	}
          //long FADDR_RECORDER =FADDR_RECORDER_INFO+ (long)wSendLISTNum*(long)FLINEADDR;
          //Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_cfg[wSendLISTNum],sizeof(gRecorder_cfg[wSendLISTNum])); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来 
          if(wSendLISTNum >= (g_sRecData.m_gRecANum+g_sRecData.m_gACTRecANum+g_sRecData.m_gXHRecANum-1))//SOF状态-1
          {
            gRecorder_Readfilecfg.CFG_SOF = 0x20;  //<0>:=后面还有目录文件；<1>:=最后目录文件  
            //wSendLISTNum = 0;
            gRecorder_flag.LIST_flag = OFF;
          }      
          else
          {
            gRecorder_Readfilecfg.CFG_SOF = 0;
            gRecorder_flag.LIST_flag = ON;
          }
          if((gRecorder_Readfilecfg.FileName<=MAX_ALLREC_NUM)&&(gRecorder_Readfilecfg.FileName==wFileNum))//filename 不能大于MAX_REC_NUM，可能被擦除
          {
	   *pTxBuf++ = gRecorder_Readfilecfg.FileName;//张弢
          *pTxBuf++ = gRecorder_Readfilecfg.FileName >> 8;       
      
          *pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng;
          *pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng>>8;
          *pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng>>16;  
		  
          *pTxBuf++ = gRecorder_Readfilecfg.CFG_SOF;
      
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC];//录波触点时间
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_cfg[wSendLISTNum].comtrade_time[RTC_DATE]|(gRecorder_cfg[wSendLISTNum].comtrade_time[RTC_WEEK]<<5);
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_Readfilecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR];
	j++;	
        }
      }
	ptoBuf[6+g_ucPara101[IECP_LINKADDR_NUM]] = j| VSQ;	//张弢确定长度
	            
       if( gRecorder_flag.LIST_flag == OFF)
	   	{
	   	wSendLISTNum = 0;wFileNum=0;
		}
     }
     return pTxBuf;
}
#endif
//***********************************************************************************
//*函数名：ApprovalFile
//*功能：得到主站认可的节传输后，发送下一节准备就绪的报文
//*描述: 传输最后节的时候发送所有节的校验，用来验证该次录波的正确性
//***********************************************************************************

unsigned char *  ApprovalFile(unsigned char *pTxBuf,unsigned char leng,RECORDER_CFG *pgRecorder_cfg,WORD wSecLenPtr)
{
  if(pTxBuf[leng + 3]==3)//节传输的认可,AFQ
  {
    if(section_current==1) //认可第一节后发送节2准备就绪
    {
      section_current=2;
      pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= SECTIONPREPAR_TYPE;
      pTxBuf += leng;
          
      *pTxBuf++ = gRecorder_flag.pRXBuff[leng];//文件序号两个字节
      *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];
      *pTxBuf++ = section_current;//当前节名称
          //FileName = MAKEWORD(gRecorder_flag.pRXBuff[leng + 1],gRecorder_flag.pRXBuff[leng + 2]);
       if(section_current == 1)//节名称为0传输.CFG的节长度
       {
          *pTxBuf++ = pgRecorder_cfg->CFG_Leng;
          *pTxBuf++ = pgRecorder_cfg->CFG_Leng>>8; 
           GLubocfg_Sum =0;
          
       }
       else//传输.DAT节的长度
       {
          *pTxBuf++ = pgRecorder_cfg->DATA_Leng;
          *pTxBuf++ = pgRecorder_cfg->DATA_Leng>>8;
           GLubodat_Sum =0;
       }
          *pTxBuf++ = 6;//SRQ=0,第一节准备就绪
        
     }
     else if(section_current == 2)//认可第二节后发送最后节
     {
          pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= LASTSECTION_TYPE;//发送的最后节信息
          pTxBuf += leng;
          
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng];//文件序号两个字节
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];
        *pTxBuf++ = section_current;//当前节名称
        *pTxBuf++ =1;//LSQ=1 不带停止激活的节传输
          //pTxBuf[wSecLenPtr] = 1;//LSQ=3 不带停止激活的节传输
              //GLubocfg_Sum = ChkluboSum(ComtrderCfg1, strlen(ComtrderCfg1));
         *pTxBuf++ = GLubodat_Sum + GLubocfg_Sum;//节的校验位
     }
   }
   else if(pTxBuf[leng + 3]==1)//文件传输的认可AFQ
   {
      section_current=1;
      pTxBuf = NULL;
   }
  return pTxBuf;
}
//***********************************************************************************
//*函数名：FileDataCfg
//*功能：文件1传输，对应.cfg文件
//*描述: 整个文件依次传输
//***********************************************************************************

unsigned char *  FileDataCfg(unsigned char *pTxBuf, unsigned char leng,RECORDER_CFG *pgRecorder_cfg,int segment_leng)//WORD wSecLenPtr,
{
       //int segment_leng = SEGMENT_LENGTH;
       unsigned int j;
       
       unsigned char *pTxPos;
       
       wCfgTotalNum =strlen(ComtrderCfg1);//(pgRecorder_cfg->CFG_Leng);//张弢 
           
       gRecorder_flag.LIST_flag = OFF;
         
       if(wSendCFGNum >= wCfgTotalNum)//最后一段
       {
          pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= LASTSECTION_TYPE;//发送的最后段信息
            pTxBuf += leng;
          *pTxBuf++ = gRecorder_flag.pRXBuff[leng ];//文件序号两个字节
          *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];//
          *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 2];//节名称
          *pTxBuf++ = 3;//pTxBuf[wSecLenPtr]LSQ=3 不带停止激活的节传输
           GLubocfg_Sum = ChkluboSum((BYTE *)ComtrderCfg1, strlen(ComtrderCfg1));
          *pTxBuf++ = GLubocfg_Sum ;
          wSendCFGNum=0;
              
        }
        else
        {
              //for(i = wSendCFGNum,j = 0;(j < segment_leng)&&(i < wCfgTotalNum);i++,j++,w_gfgId++)
           pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= SEGMENT_TYPE;//发送的段信息
           pTxBuf += leng;
          *pTxBuf++ = gRecorder_flag.pRXBuff[leng ];//文件序号两个字节
          *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];//
          *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 2];//节名称
           pTxPos = pTxBuf;
          *pTxBuf++ =0; //pTxBuf[leng+3]  = 0;
           
           for(j = 0;(j < segment_leng)&&(wSendCFGNum < wCfgTotalNum);wSendCFGNum++,j++)
           {
               *pTxBuf++ = *strtemp++;
           }
          
           *pTxPos = pTxBuf - pTxPos-1;//LL
           //w_gfgId;//段长度pTxBuf[wSecLenPtr]
           gRecorder_flag.LIST_flag = ON;
        }
             //wSendCFGNum += w_gfgId;
            //m_SendBuf.pBuf[wSecLenPtr] = m_SendBuf.wWritePtr - wSecLenPtr -1;
        return pTxBuf;    
}

//***********************************************************************************
//*函数名：FileDatadat
//*功能：文件2传输，对应.dat文件
//*描述: 数据传输，一行一行的传数据
//***********************************************************************************

unsigned char *  FileDatadat(unsigned char *pTxBuf,unsigned char leng,WORD wSecLenPtr,int segment_leng)
{
    
    unsigned char datBuff[16] = {0}; //8个通道,每个通道的数值有2个
    unsigned char k;
    unsigned int Sample_num;// = 100;CRG_SAMPLE_NUM;CFG_EndSamp
    gRecorder_flag.LIST_flag = OFF;
    WORD FileName;
    char ch[250];
    unsigned char *pTxPos;
    unsigned long FLbAddrV;

   gRes_rec.res_pTxBuf = pTxBuf;
   gRes_rec.res_leng = leng;
   gRes_rec.res_wSecLenPtr = wSecLenPtr;
   gRes_rec.res_segment_leng =segment_leng;
   //gRes_rec.res_timeout = 1;
   if(gRes_rec.res_timeout >=4)
   	wSendDATNum=wSendDATNumOld;
   wSendDATNumOld =wSendDATNum;	
    FileName = MAKEWORD(gRecorder_flag.pRXBuff[leng],gRecorder_flag.pRXBuff[leng + 1])-1;
    Sample_num=gRecorder_Readfilecfg.CFG_EndSamp;	//张弢 录波的条数
	//Sample_num=300;
    if( wSendDATNum >= Sample_num )//最后一段
    {
      pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= LASTSECTION_TYPE;//发送的最后段信息
      //pTxBuf[wSecLenPtr] = 3;//LSQ=3 不带停止激活的节传输
              //GLubocfg_Sum = ChkluboSum(ComtrderCfg1, strlen(ComtrderCfg1));
       pTxBuf += leng;
       *pTxBuf++ = gRecorder_flag.pRXBuff[leng ];//文件序号两个字节
       *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];//
       *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 2];//节名称
       *pTxBuf++ = 3;//pTxBuf[wSecLenPtr]LSQ=3 带停止激活的节传输
       *pTxBuf++ = GLubodat_Sum;
       wSendDATNum = 0;
       //return pTxBuf;
     }
     else
     {
        pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= SEGMENT_TYPE;//发送的段信息
         pTxBuf += leng;
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng ];//文件序号两个字节
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];//
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 2];//节名称
        
         pTxPos = pTxBuf;
        *pTxBuf++  = 0;//SEGMENT_LENGTH;
        
        for(int i=0;(wSendDATNum < Sample_num);wSendDATNum++)//每一行的值
        {
              if(FileName<(MAX_REC_NUM+1))
              	{
              	FLbAddrV = FADDR_RECORDER_DATA+(unsigned long)(FileName)*0x2000+(unsigned long)wSendDATNum*10;
              	}
		else if(FileName<(MAX_REC_NUM+MAX_ACTREC_NUM+2))	
			{
			FLbAddrV =FADDR_RECORDER_ACTDATA+(unsigned long)(FileName-51)*0x90000+(unsigned long)wSendDATNum*10;
			}
		else if(FileName<(MAX_REC_NUM+MAX_ACTREC_NUM+MAX_XHREC_NUM+3))	
			{
			FLbAddrV = FADDR_RECORDER_XHDATA+(unsigned long)(FileName-62)*0x8000+(unsigned long)wSendDATNum*10;
			}
		else
			{//err
			}
			  
          	//unsigned long FLbAddrV = ((unsigned long)(FADDR_RECORDER_DATA+FileName*4)<<16)+(unsigned long)wSendDATNum*10;
	       Sst26vf064b_Read(FLbAddrV,&datBuff[0],10);//WRITEPECORDER+电压每只指示器的第一报数据存相关信息
	       
          unsigned long xt =250;
          if(gRecorder_Readfilecfg.CFG_Samp==800)xt=1250;//张弢 动作录波 频率800 时间间隔1250
      	   if(gRecorder_Readfilecfg.CFG_Samp==1600)xt=625;//张弢 动作录波 频率1600 时间间隔625
      	   if(gRecorder_Readfilecfg.CFG_Samp==2000)xt=500;//张弢 动作录波 频率1600 时间间隔625
          unsigned int dka;//,dkd;
	   dka = MAKEWORD(datBuff[8],datBuff[9]);
	   //dkd = (dka>>14)&0x1;
	   if(dka&((unsigned int)(1<<15)))
	   	dka |=(1<<14);
	   else
	   	dka &= NBITE;

	   unsigned char a,b,c;	   
	   a=(datBuff[1]>>6)&0x01;
	   b=(datBuff[3]>>6)&0x01;
	   c=(datBuff[5]>>6)&0x01;
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

	   //g=(datBuff[9]>>6)&0x01;
	   
	   //datBuff[1] = datBuff[1]&0xbf;
	   //datBuff[3] = datBuff[3]&0xbf;
	   //datBuff[5] = datBuff[5]&0xbf;
	   
          unsigned long xtt = (long)wSendDATNum*xt;
		  
          if(g_gRunPara[RP_CFG_KEY]&BIT[RP_COMTRADE_TYPE])
          	{
          	ch[0]=LOBYTE(wSendDATNum);ch[1]=HIBYTE(wSendDATNum);ch[2]=0;ch[3]=0;
		 ch[4] = LOBYTE(LOWORD(xtt));
               ch[5] = HIBYTE(LOWORD(xtt));
               ch[6] = LOBYTE(HIWORD(xtt));
               ch[7] = HIBYTE(HIWORD(xtt));
		 ch[8] = datBuff[0];ch[9] = datBuff[1];ch[10] = datBuff[2];ch[11] = datBuff[3];
		 ch[12] = datBuff[4];ch[13] = datBuff[5];ch[14] = datBuff[6];ch[15] = datBuff[7];
		 ch[16]=LOBYTE(dka);ch[17]=HIBYTE(dka);
		 //ch[18]=LOBYTE(dkd);ch[19]=HIBYTE(dkd);
		 ch[18]=a+b*2+c*4;ch[19]=0;//ch[19]=b;ch[20]=c;ch[21]=0;		 
		 //ch[20]=b;ch[21]=0;
		 //ch[22]=c;ch[23]=0;
		 i+=20;k=20;
          	}
	   else
	   	{
          	sprintf((char *)ch,"%d,%ld,%d,%d,%d,%d,%d,%d,%d,%d\n",wSendDATNum,xtt,MAKEWORD(datBuff[0],datBuff[1]),MAKEWORD(datBuff[2],datBuff[3]),
                                 MAKEWORD(datBuff[4],datBuff[5]),MAKEWORD(datBuff[6],datBuff[7]),dka,(unsigned int)a,(unsigned int)b,(unsigned int)c);//每个周期80个点，采样周期250微妙//张弢
                                 
          	i+= strlen(ch);k=strlen(ch);
	   	}
          if(i <=segment_leng)
          {
            for(BYTE n = 0; n < k; n++)
            {
                *pTxBuf++ = ch[n];
                 GLubodat_Sum += ch[n];
            }
            *pTxPos = pTxBuf - pTxPos-1;//LL
            gRecorder_flag.LIST_flag = ON;     
            //GLubodat_Sum += ChkluboSum((BYTE *)ch, strlen(ch));
            
              
          }
          else
          {
            
            //wSendDATNum--;//张弢
            break;//return pTxBuf;
          }
          if(wSendDATNum == Sample_num-1)
          {
            wSendDATNum++;
            break;//return pTxBuf;
          }
             
        }
        
     }
     return pTxBuf;
 }
//***********************************************************************************
//*函数名：SectionPrepareFile
//*功能：节准备就绪传输
//*描述：主站请求节传输，终端回复节准备就绪，主要传输各个节的长度
//***********************************************************************************
unsigned char *  SectionPrepareFile(unsigned char *pTxBuf,unsigned char leng,RECORDER_CFG *pgRecorder_cfg)
{
  if(pTxBuf[leng + 3]==2)  //请求文件传输SRQ=0
  {
     pTxBuf += leng;   
    *pTxBuf++ = gRecorder_flag.pRXBuff[leng ];//文件序号两个字节
    *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];
    *pTxBuf++ = section_current;//当前节名称
          //FileName = MAKEWORD(gRecorder_flag.pRXBuff[leng + 1],gRecorder_flag.pRXBuff[leng + 2]);
    if(section_current == 1)//节名称为0传输.CFG的节长度
    {
      *pTxBuf++ = pgRecorder_cfg->CFG_Leng;
      *pTxBuf++ = pgRecorder_cfg->CFG_Leng>>8;  
    }
    else//传输.DAT节的长度
    {
       *pTxBuf++ = pgRecorder_cfg->DATA_Leng;
       *pTxBuf++ = pgRecorder_cfg->DATA_Leng>>8;  
    }
       *pTxBuf++ = 0;//SRQ=0,第一节准备就绪
  }
  return pTxBuf;
}

//***********************************************************************************
//*函数名：FileDataCfg
//*功能：文件1传输，对应.cfg文件
//*描述: 整个文件依次传输
//***********************************************************************************

unsigned char *  FileDataCfg_YN(unsigned char *pTxBuf, unsigned char leng,RECORDER_CFG *pgRecorder_cfg,int segment_leng)//WORD wSecLenPtr,
{
       //int segment_leng = SEGMENT_LENGTH;
       unsigned int j;       
       unsigned char *pTxPos;
	unsigned char *ptoBuf;	  
	unsigned char temp;
      ptoBuf = pTxBuf;
       
       wCfgTotalNum =strlen(ComtrderCfg1);//(pgRecorder_cfg->CFG_Leng);//张弢            
       gRecorder_flag.LIST_flag = OFF;
        {
           pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= CALL_FILE_YN;//类型标示 137 0x89
           pTxBuf += leng;
	//文件名（8个字节）
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_MICROSEC];//录波触点时间
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_SEC];
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_MINUT];
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_HOUR];        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_MONTH];
        if(pgRecorder_cfg->comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_YEAR];
	 *pTxBuf++ = (pgRecorder_cfg->FileName<<1) &0xfe;//0位=0， cfg文件
	//总包（2字节）当前包（2字节）
	 *pTxBuf++= 2;*pTxBuf++=0;	
	if(wSendCFGNum>8)
		{*pTxBuf++=1;}
	else
		{*pTxBuf++=0;}
	*pTxBuf++=0;
	//包长度（1字节）放在最后写
           pTxPos = pTxBuf;
          *pTxBuf++ =0; //pTxBuf[leng+3]  = 0;
	if(wSendCFGNum >= wCfgTotalNum)
	    	{//结束帧
	    	gRecorder_flag.LIST_flag = OFF;
		wSendCFGNum=0;
		section_current = 0;
		ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 10;	//传送原因=13
              temp = g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
              ptoBuf[7+temp+12]=0;	//包长度
              ptoBuf[7+temp+10] = 2;	//当前包
              return pTxBuf;
	    	}
           
           for(j = 0;(j < segment_leng)&&(wSendCFGNum < wCfgTotalNum);wSendCFGNum++,j++)
           {
               *pTxBuf++ = *strtemp++;
           }
          
           *pTxPos = pTxBuf - pTxPos-1;//LL
           //w_gfgId;//段长度pTxBuf[wSecLenPtr]
           ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 13;	//传送原因=13
           temp = g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
           ptoBuf[7+temp+12] = j;	//包长度
           gRecorder_flag.LIST_flag = ON;


        }
             //wSendCFGNum += w_gfgId;
            //m_SendBuf.pBuf[wSecLenPtr] = m_SendBuf.wWritePtr - wSecLenPtr -1;
        return pTxBuf;    
}

//***********************************************************************************
//*函数名：FileDatadat
//*功能：文件2传输，对应.dat文件
//*描述: 数据传输，一行一行的传数据
//***********************************************************************************

unsigned char *  FileDatadat_YN(unsigned char *pTxBuf,unsigned char leng,WORD wSecLenPtr,int segment_leng)
{
    
    unsigned char datBuff[16] = {0}; //8个通道,每个通道的数值有2个
    unsigned char k,temp;
    unsigned int Sample_num;// = 100;CRG_SAMPLE_NUM;CFG_EndSamp
    gRecorder_flag.LIST_flag = OFF;
    WORD FileName;
    char ch[250];
    unsigned char snum=0;
    unsigned char *pTxPos;
    unsigned char *ptoBuf;	 	
    ptoBuf = pTxBuf;
		  
    temp= gRecorder_flag.pRXBuff[leng+7];//文件名8个字节 7字节时间+1字节编号
    FileName = temp>>1;
    Sample_num=gRecorder_filecfg.CFG_EndSamp;	//张弢 录波的条数
	
     {
        pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= CALL_FILE_YN;//类型标示 137 0x89
         pTxBuf += leng;
	//文件名（8个字节）
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//录波触点时间
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_HOUR];        
	 *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          	*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          	*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR];
	 *pTxBuf++ = (gRecorder_filecfg.FileName<<1) |0x1;//0位=0， cfg文件
	//总包（2字节）当前包（2字节）
	 *pTxBuf++= LOBYTE(Sample_num/10);
	 *pTxBuf++=HIBYTE(Sample_num/10);	
	*pTxBuf++=LOBYTE(wSendDATNum/10);
	*pTxBuf++=HIBYTE(wSendDATNum/10);
	//包长度（1字节）放在最后写        
         pTxPos = pTxBuf;
        *pTxBuf++  = 0;//SEGMENT_LENGTH;
        if( wSendDATNum >= Sample_num )//最后一段
     		{//结束帧
     		wSendDATNum = 0;
		gRecorder_flag.LIST_flag = OFF;
		section_current = 0;
           	ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 10;	//传送原因=13
           	temp = g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
           	ptoBuf[7+temp+12] = 0;	//包长度
           	ptoBuf[7+temp+10] = LOBYTE(Sample_num/10);
		ptoBuf[7+temp+11] = HIBYTE(Sample_num/10);			
     		return pTxBuf;		
     		}          
        
        for(int i=0;(wSendDATNum < Sample_num);wSendDATNum++)//每一行的值
        {
          	unsigned long FLbAddrV = ((unsigned long)(FADDR_RECORDER_DATA+FileName*4)<<16)+(unsigned long)wSendDATNum*10;
	       Sst26vf064b_Read(FLbAddrV,&datBuff[0],10);//WRITEPECORDER+电压每只指示器的第一报数据存相关信息
	       
          unsigned long xt =250;
          if(gRecorder_filecfg.CFG_Samp==800)xt=1250;//张弢 动作录波 频率800 时间间隔1250
      	   if(gRecorder_filecfg.CFG_Samp==1600)xt=625;//张弢 动作录波 频率1600 时间间隔625
      	   if(gRecorder_filecfg.CFG_Samp==2000)xt=500;//张弢 动作录波 频率1600 时间间隔625
          unsigned int dka,dkd;
	   dka = MAKEWORD(datBuff[8],datBuff[9]);
	   dkd = (dka>>14)&0x1;
	   if(dka&((unsigned int)(1<<15)))
	   	dka |=(1<<14);
	   else
	   	dka &= NBITE;
	   
          unsigned long xtt = (long)wSendDATNum*xt;
		  
          if(g_gRunPara[RP_CFG_KEY]&BIT[RP_COMTRADE_TYPE])
          	{
          	ch[0]=LOBYTE(wSendDATNum);ch[1]=HIBYTE(wSendDATNum);ch[2]=0;ch[3]=0;
		 ch[4] = LOBYTE(LOWORD(xtt));
               ch[5] = HIBYTE(LOWORD(xtt));
               ch[6] = LOBYTE(HIWORD(xtt));
               ch[7] = HIBYTE(HIWORD(xtt));
		 ch[8] = datBuff[0];ch[9] = datBuff[1];ch[10] = datBuff[2];ch[11] = datBuff[3];
		 ch[12] = datBuff[4];ch[13] = datBuff[5];ch[14] = datBuff[6];ch[15] = datBuff[7];
		 ch[16]=LOBYTE(dka);ch[17]=HIBYTE(dka);
		 ch[18]=LOBYTE(dkd);ch[19]=HIBYTE(dkd);
		 i+=20;k=20;
          	}
	   else
	   	{
          	sprintf((char *)ch,"%d,%ld,%d,%d,%d,%d,%d,%d\n",wSendDATNum,xtt,MAKEWORD(datBuff[0],datBuff[1]),MAKEWORD(datBuff[2],datBuff[3]),
                                 MAKEWORD(datBuff[4],datBuff[5]),MAKEWORD(datBuff[6],datBuff[7]),dka,dkd);//每个周期80个点，采样周期250微妙//张弢
                                 
          	i+= strlen(ch);k=strlen(ch);
	   	}
          if(i <=segment_leng)
          {
            for(BYTE n = 0; n < k; n++)
            {
                *pTxBuf++ = ch[n];
                 GLubodat_Sum += ch[n];
		   snum++;
            }
            *pTxPos = pTxBuf - pTxPos-1;//LL
            gRecorder_flag.LIST_flag = ON;     
            //GLubodat_Sum += ChkluboSum((BYTE *)ch, strlen(ch));
            
              
          }
          else
          {
            
            //wSendDATNum--;//张弢
            break;//return pTxBuf;
          }
          if(wSendDATNum == Sample_num-1)
          {
            wSendDATNum++;
            break;//return pTxBuf;
          }
             
        }
/*     if( wSendDATNum >= Sample_num )//最后一段
     	{
     	wSendDATNum = 0;
	gRecorder_flag.LIST_flag = OFF;
	section_current = 0;
     	}  */    
     }
           ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 13;	//传送原因=13
           temp = g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
           ptoBuf[7+temp+12] = snum;	//包长度	 
     return pTxBuf;
 }
//***********************************************************************************
//*函数名：RecCallFile_YN
//*功能：相应文件召唤 云南录波协议
//*描述: 
//***********************************************************************************

unsigned char * RecCallFile_YN(unsigned char *pTxBuf, unsigned char leng,unsigned char filename)
{
      unsigned char *ptoBuf;
      unsigned char filetype = 0;	 
	  
      ptoBuf = pTxBuf;

	if(filename&1)filetype=1;
	filename = filename>>1;
	  
	pTxBuf += leng;
          //cfg文件名 8字节 7字节时间+1字节编号	   
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//录波触点时间
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_HOUR];        
	 *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR];
		*pTxBuf++ = (gRecorder_filecfg.FileName<<1) &0xfe;//0位=0， cfg文件
          //文件长度 3字节
          if(filetype==1)
          	{
          	*pTxBuf++ = gRecorder_filecfg.TOTAL_Leng*20;
          	*pTxBuf++ = (gRecorder_filecfg.TOTAL_Leng*20)>>8;
          	*pTxBuf++ = (gRecorder_filecfg.TOTAL_Leng*20)>>16;            	
          	}
	   else
	   	{
	   	cfg_dat_length(filename);
          	*pTxBuf++ = gRecorder_filecfg.CFG_Leng;
          	*pTxBuf++ = (gRecorder_filecfg.CFG_Leng)>>8;
          	*pTxBuf++ = (gRecorder_filecfg.CFG_Leng)>>16; 		
         	// *pTxBuf++ = 0x11;
	  	// *pTxBuf++ = 0x01;	  
	   	// *pTxBuf++ = 0;	 
	   	}
          //SOF 1字节 	
        *pTxBuf++ = 0x80;
          //文件创建时间 7字节       
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//录波触点时间
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_HOUR];        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR];
	//文件单包长度(1个字节)
	*pTxBuf++= 200;	//文件单包长度(1个字节)
	ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 7;	//传送原因=7
		
        	section_current=1;
		gRecorder_flag.LIST_flag = ON;

	  return pTxBuf;		
}

//***********************************************************************************
//*函数名：FileCfgOneFrame_YN
//*功能：文件1传输，对应.cfg文件
//*描述: 整个文件依次传输
//***********************************************************************************

unsigned char *  FileCfgOneFrame_YN(unsigned char *pTxBuf, unsigned char leng,RECORDER_CFG *pgRecorder_cfg,int framenum)//WORD wSecLenPtr,
{
       //int segment_leng = SEGMENT_LENGTH;
       unsigned int j;       
       unsigned char *pTxPos;
	unsigned char *ptoBuf;	  
	unsigned char temp;
      ptoBuf = pTxBuf;
       
       wCfgTotalNum =strlen(ComtrderCfg1);//(pgRecorder_cfg->CFG_Leng);//张弢            
       gRecorder_flag.LIST_flag = OFF;

           pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= CALL_ONEFRAME_YN;//类型标示 137 0x89
           pTxBuf += leng;
	//文件名（8个字节）
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_MICROSEC];//录波触点时间
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_SEC];
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_MINUT];
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_HOUR];        
	 *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_MONTH];
        if(pgRecorder_cfg->comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_YEAR];
	 *pTxBuf++ = (pgRecorder_cfg->FileName<<1) &0xfe;//0位=0， cfg文件
	//总包（2字节）当前包（2字节）
	 *pTxBuf++= 2;*pTxBuf++=0;	
	*pTxBuf++=LOBYTE(framenum);
	*pTxBuf++=HIBYTE(framenum);
	//包长度（1字节）放在最后写
           pTxPos = pTxBuf;
          *pTxBuf++ =0; //pTxBuf[leng+3]  = 0;
	if(framenum>=2)
		{return pTxBuf; }
	else if(framenum==1)
		{strtemp+=200;wSendCFGNum+=200;}           
           for(j = 0;(j < 200)&&(wSendCFGNum < wCfgTotalNum);wSendCFGNum++,j++)
           {
               *pTxBuf++ = *strtemp++;
           }
          
           *pTxPos = pTxBuf - pTxPos-1;//LL
           ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 7;	//传送原因=7
           temp = g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
           ptoBuf[7+temp+12] = j;	//包长度
       gRecorder_flag.LIST_flag = OFF;
	   
        return pTxBuf;    
}
//***********************************************************************************
//*函数名：FileDatOneFrame_YN
//*功能：文件2传输，对应.dat文件
//*描述: 数据传输，一行一行的传数据
//***********************************************************************************

unsigned char *  FileDatOneFrame_YN(unsigned char *pTxBuf,unsigned char leng,WORD wSecLenPtr,int framenum)
{
    
    unsigned char datBuff[16] = {0}; //8个通道,每个通道的数值有2个
    unsigned char k,temp;
    unsigned int Sample_num;// = 100;CRG_SAMPLE_NUM;CFG_EndSamp
    gRecorder_flag.LIST_flag = OFF;
    WORD FileName;
    char ch[250];
    unsigned char *pTxPos;
    unsigned char *ptoBuf;	 	
    ptoBuf = pTxBuf;

    framenum = framenum*10;		  
    temp= gRecorder_flag.pRXBuff[leng+7];//文件名8个字节 7字节时间+1字节编号
    FileName = temp>>1;
    Sample_num=gRecorder_filecfg.CFG_EndSamp;	//张弢 录波的条数	

        pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= CALL_ONEFRAME_YN;//类型标示 138 0x8a
         pTxBuf += leng;
	//文件名（8个字节）
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//录波触点时间
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_HOUR];        
	 *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//张弢	
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          	*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          	*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR];
	 *pTxBuf++ = (gRecorder_filecfg.FileName<<1) |0x1;//0位=0， cfg文件
	//总包（2字节）当前包（2字节）
	 *pTxBuf++= LOBYTE(Sample_num/10);
	 *pTxBuf++=HIBYTE(Sample_num/10);	
	*pTxBuf++=LOBYTE(framenum);
	*pTxBuf++=HIBYTE(framenum);
	//包长度（1字节）放在最后写        
         pTxPos = pTxBuf;
        *pTxBuf++  = 0;//SEGMENT_LENGTH; 
        
        for(int i=0;(framenum < Sample_num);framenum++)//每一行的值
        {
          	unsigned long FLbAddrV = ((unsigned long)(FADDR_RECORDER_DATA+FileName*4)<<16)+(unsigned long)framenum*10;
	       Sst26vf064b_Read(FLbAddrV,&datBuff[0],10);//WRITEPECORDER+电压每只指示器的第一报数据存相关信息
	       
          unsigned long xt =250;
          if(gRecorder_filecfg.CFG_Samp==800)xt=1250;//张弢 动作录波 频率800 时间间隔1250
      	   if(gRecorder_filecfg.CFG_Samp==1600)xt=625;//张弢 动作录波 频率1600 时间间隔625
      	   if(gRecorder_filecfg.CFG_Samp==2000)xt=500;//张弢 动作录波 频率1600 时间间隔625
          unsigned int dka,dkd;
	   dka = MAKEWORD(datBuff[8],datBuff[9]);
	   dkd = (dka>>14)&0x1;
	   if(dka&((unsigned int)(1<<15)))
	   	dka |=(1<<14);
	   else
	   	dka &= NBITE;
	   
          unsigned long xtt = (long)framenum*xt;
		  
          if(g_gRunPara[RP_CFG_KEY]&BIT[RP_COMTRADE_TYPE])
          	{
          	ch[0]=LOBYTE(framenum);ch[1]=HIBYTE(framenum);ch[2]=0;ch[3]=0;
		 ch[4] = LOBYTE(LOWORD(xtt));
               ch[5] = HIBYTE(LOWORD(xtt));
               ch[6] = LOBYTE(HIWORD(xtt));
               ch[7] = HIBYTE(HIWORD(xtt));
		 ch[8] = datBuff[0];ch[9] = datBuff[1];ch[10] = datBuff[2];ch[11] = datBuff[3];
		 ch[12] = datBuff[4];ch[13] = datBuff[5];ch[14] = datBuff[6];ch[15] = datBuff[7];
		 ch[16]=LOBYTE(dka);ch[17]=HIBYTE(dka);
		 ch[18]=LOBYTE(dkd);ch[19]=HIBYTE(dkd);
		 i+=20;k=20;
          	}
	   else
	   	{
          	sprintf((char *)ch,"%d,%ld,%d,%d,%d,%d,%d,%d\n",framenum,xtt,MAKEWORD(datBuff[0],datBuff[1]),MAKEWORD(datBuff[2],datBuff[3]),
                                 MAKEWORD(datBuff[4],datBuff[5]),MAKEWORD(datBuff[6],datBuff[7]),dka,dkd);//每个周期80个点，采样周期250微妙//张弢
                                 
          	i+= strlen(ch);k=strlen(ch);
	   	}
          if(i <=200)
          {
            for(BYTE n = 0; n < k; n++)
            {
                *pTxBuf++ = ch[n];
                 GLubodat_Sum += ch[n];
            }
            *pTxPos = pTxBuf - pTxPos-1;//LL              
          }
          else
          {
            break;//return pTxBuf;
          }             
        }

           ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 7;	//传送原因=7
           temp = g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
           ptoBuf[7+temp+12] = 200;	//包长度	
           gRecorder_flag.LIST_flag = OFF; 
     return pTxBuf;
 }
//unsigned long FLbAddrStart;
//unsigned long FLbAddrEnd;
//***********************************************************************************
//*函数名：Code_Lubo
//*功能：对相应的ASDU报文做出响应
//*描述：录波文件的传输
//***********************************************************************************
#ifdef YN_101S
void Code_Lubo(unsigned char *pRxBuf,unsigned char *pTXBuff)
{//1
  unsigned char *pTxBuf = pTXBuff;//重新对发送缓存写入数据
  
  unsigned int wave_total;
  
  unsigned char leng;

  unsigned char temp;

  WORD FileName = 0;//文件序号
  WORD framenum=0;  
  WORD wSecLenPtr=0;
 
  
  CAT_SpiReadWords(EEPADD_LUBONUM, 1, &wave_total);
  if(wave_total > MAX_REC_NUM)  wave_total=0;
  gRecorder_flag.LIST_flag = OFF;
  
  //从接收的报文中处理发送的数据，从报文头到信息体地址68 00 00 68 08
  leng=7+g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
  if(pRxBuf != null )
  {
    memcpy(gRecorder_flag.pRXBuff,pRxBuf,6+pRxBuf[1]);  
  }
  
    memcpy(pTXBuff,gRecorder_flag.pRXBuff,6+pRxBuf[1]);//leng


  if(pTXBuff[7+g_ucPara101[IECP_LINKADDR_NUM]] == 8)//传送原因==8，停止传输
  	{
    	section_current = 0;
	gRecorder_flag.LIST_flag = OFF;
	pTxBuf += leng;
	pTXBuff[7+g_ucPara101[IECP_LINKADDR_NUM]] =9;
	wSendDATNum = 0;
	wSendCFGNum=0;
	wSendLISTNum =0;
    	}
   else if(pTXBuff[7+g_ucPara101[IECP_LINKADDR_NUM]] == REQUEST_COT)//召唤目录的传送原因
  {
    if(pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]] == CALL_TYPE_YN)//(wInfoAddr == 0x690A)//召唤录波文件的目录
    {
      pTxBuf = FileDirectory(pTxBuf,leng,wave_total);    
    }
    else if(pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]] == CALL_FILE_YN)//(wInfoAddr == 0x690A)//召唤录波文件
    	{
    	if((section_current!=1)&&(section_current!=2))
    		{//响应召唤文件
    		temp= gRecorder_flag.pRXBuff[leng+7];//文件名8个字节 7字节时间+1字节编号
    		FileName = temp>>1;
    		long FADDR_RECORDER = FADDR_RECORDER_INFO +(long)FileName*(long)FLINEADDR;
    		Sst26vf064b_Read(FADDR_RECORDER ,(unsigned char *)&gRecorder_filecfg,sizeof(gRecorder_filecfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来        
		pTxBuf = RecCallFile_YN(pTxBuf,leng,temp);
		if(temp&0x01)
			section_current = 2;
		else
			section_current = 1;
    		}
	else if(section_current==1)
		{//发送cfg文件
		temp= gRecorder_flag.pRXBuff[leng+7];//文件名8个字节 7字节时间+1字节编号
    		FileName = temp>>1;
		if(wSendCFGNum ==0)
              	cfg_dat_length(FileName);
           	pTxBuf = FileDataCfg_YN(pTxBuf,leng,&gRecorder_filecfg,200);//wSecLenPtr,
           	//section_current = 0;
		}
	else if(section_current==2)
		{//发送data文件
		
		pTxBuf = FileDatadat_YN(pTxBuf,leng,wSecLenPtr,200);
		//section_current = 0;
		}		
    	}
    else if(pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]] == CALL_ONEFRAME_YN)//补包，召唤录波文件中的一帧
    	{
	temp= gRecorder_flag.pRXBuff[leng+7];//文件名8个字节 7字节时间+1字节编号
    	FileName = temp>>1; 
    	long FADDR_RECORDER = FADDR_RECORDER_INFO +(long)FileName*(long)FLINEADDR;
    	Sst26vf064b_Read(FADDR_RECORDER ,(unsigned char *)&gRecorder_filecfg,sizeof(gRecorder_filecfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来        
       framenum = MAKEWORD(gRecorder_flag.pRXBuff[leng+8], gRecorder_flag.pRXBuff[leng+9]);
	if(temp&0x01)
		{
		pTxBuf = FileDatOneFrame_YN(pTxBuf,leng,wSecLenPtr,framenum);
		}
	else
		{
		cfg_dat_length(FileName);
           	pTxBuf = FileCfgOneFrame_YN(pTxBuf,leng,&gRecorder_filecfg,framenum);//wSecLenPtr,
		}    				
    	}
  }

  
    pTXBuff[1] = pTxBuf - &(pTXBuff[4]);//LL
    pTXBuff[2] = pTxBuf -&(pTXBuff[4]);//LL
   *pTxBuf++ = ChkluboSum(&(pTXBuff[4]),pTXBuff[1]);
   *pTxBuf++ = 0x16;
    CommSendData(pTXBuff,pTXBuff[1]+6,g_Cmid);
    
  }
#else
void Code_Lubo(unsigned char *pRxBuf,unsigned char *pTXBuff)
{//1
  unsigned char *pTxBuf = pTXBuff;//重新对发送缓存写入数据  
  unsigned int wave_total;  
  unsigned char leng; 
  WORD FileName = 0;//文件序号 
  WORD wSecLenPtr=0; 
  long FADDR_RECORDER;
  
  CAT_SpiReadWords(EEPADD_LUBONUM, 1, &wave_total);
  if(wave_total > MAX_REC_NUM)  wave_total=0;
  gRecorder_flag.LIST_flag = OFF;
  
  //从接收的报文中处理发送的数据，从报文头到信息体地址68 00 00 68 08
  leng=7+g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
  if(pRxBuf != null )
  {
    memcpy(gRecorder_flag.pRXBuff,pRxBuf,6+pRxBuf[1]);  
  }
  
    memcpy(pTXBuff,gRecorder_flag.pRXBuff,6+pRxBuf[1]);//leng
	
  if(pTXBuff[7+g_ucPara101[IECP_LINKADDR_NUM]] == REQUEST_COT)//召唤目录的传送原因
  {
    if(pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]] == CALL_TYPE)//(wInfoAddr == 0x690A)//召唤录波文件的目录
    {
      //wSendLISTNum =0;
      pTxBuf = FileDirectory(pTxBuf,leng,wave_total);    
    }
  }
  else if(pTXBuff[7+g_ucPara101[IECP_LINKADDR_NUM]] == FILETRANS_COT)//召唤文件的传送原因
  {
    FileName = MAKEWORD(gRecorder_flag.pRXBuff[leng],gRecorder_flag.pRXBuff[leng + 1])-1;
    if(FileName<(MAX_REC_NUM+1))
    	{
    	FADDR_RECORDER =FADDR_RECORDER_DATA-256+ (unsigned long)(FileName+1)*0x2000;
    	}
    else if(FileName<(MAX_REC_NUM+MAX_ACTREC_NUM+2))
    	{
    	FADDR_RECORDER =FADDR_RECORDER_ACTDATA-256+ (unsigned long)(FileName-51+1)*0x90000;
    	}
    else if(FileName<MAX_ALLREC_NUM)
    	{
    	FADDR_RECORDER =FADDR_RECORDER_XHDATA-256+ (unsigned long)(FileName-62+1)*0x8000;
    	}
   else
   	{//erro
   	}
    //long FADDR_RECORDER = FADDR_RECORDER_INFO +(long)FileName*(long)FLINEADDR;
    Sst26vf064b_Read(FADDR_RECORDER ,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来        
    
    if(pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]]== CALL_TYPE)// 122 0x7A //召唤目录（可选），选择文件（可选），召唤文件，召唤节
    {

      if(pTXBuff[leng + 3]==1)  //选择文件，SCQ=1
      {
         pTxBuf += leng;
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng ];//文件序号两个字节
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];//
        //FileName = MAKEWORD(gRecorder_flag.pRXBuff[leng + 1],gRecorder_flag.pRXBuff[leng + 2]);
        *pTxBuf++ =gRecorder_Readfilecfg.TOTAL_Leng;
        *pTxBuf++ =gRecorder_Readfilecfg.TOTAL_Leng>>8;
        *pTxBuf++ =gRecorder_Readfilecfg.TOTAL_Leng>>16;
        *pTxBuf++ =0x80;//文件准备就绪  //山东录波协议  =0；
        //pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]] = FILEPREPAR_TYPE;//120 0x78//山东录波协议 
        section_current=1;
      }
      else if(pTXBuff[leng + 3]==6)//请求节
      {
         int segment_leng = SEGMENT_LENGTH;
         if( gRecorder_flag.pRXBuff[leng + 2]==1)//节名称为1 则是配置文件的数据信息
         {
           if(wSendCFGNum ==0)
              cfg_dat_length(FileName);
           pTxBuf = FileDataCfg(pTxBuf,leng,&gRecorder_Readfilecfg,segment_leng);//wSecLenPtr,
              
         }
         else if(gRecorder_flag.pRXBuff[leng + 2]==2)//节名称为2 则是数据文件的数据信息
         {
            pTxBuf = FileDatadat(pTxBuf,leng,wSecLenPtr,segment_leng);           
         }       
      }
	  /*//山东录波协议 
      else if(pTXBuff[leng + 3]==2)  //请求文件传输，SCQ=2
      	{
      	pTxBuf = SectionPrepareFile(pTxBuf,leng,&gRecorder_Readfilecfg);//节确认
      	}  */      
    }
    else if(pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]]== APPROVALFILE_TYPE)//124 0x7C//认可文件、节的流程处理
    {
      pTxBuf = ApprovalFile(pTxBuf,leng,&gRecorder_Readfilecfg,wSecLenPtr);
      if(pTxBuf == NULL) return;
         
    }
    else if((pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]]== SECTIONPREPAR_TYPE))//121 0x79
    {
        pTxBuf = SectionPrepareFile(pTxBuf,leng,&gRecorder_Readfilecfg);
        
         
    } 
   }
  if(pRxBuf[4]>=0x80)//控制域BIT7为要反向
  	{
  	pTXBuff[4] =  pRxBuf[4]-0x80;
  	}
  else
  	{
  	pTXBuff[4] =  pRxBuf[4]+0x80;
  	}
    pTXBuff[1] = pTxBuf - &(pTXBuff[4]);//LL
    pTXBuff[2] = pTxBuf -&(pTXBuff[4]);//LL
   *pTxBuf++ = ChkluboSum(&(pTXBuff[4]),pTXBuff[1]);
   *pTxBuf++ = 0x16;
    CommSendData(pTXBuff,pTXBuff[1]+6,g_Cmid);
    
  }
#endif         

//FLbAddr = FADDR_RECORDER_START + lubo_absolute_time;//绝对时标的时间都取A相的时间

//FLbAddr = FADDR_RECORDER_START + lubo_absolute_time;//第二次绝对时标的时间

//Sst26vf064b_Read(FLbAddr,ReadBack,RTC_NUM);//获取绝对地址的时标



