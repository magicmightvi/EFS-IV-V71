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

//static unsigned char ReadBack[256] = {0};//��ȡ�ļ��е���Ϣ

static unsigned int  wSendCFGNum = 0;////�Ѿ����͵������ļ����ֽ��������ƶ�֡������
static unsigned int  wSendDATNum = 0;////�Ѿ����͵������ļ����ֽ��������ƶ�֡������
static unsigned int  wSendLISTNum = 0;////�Ѿ����͵�Ŀ¼������
static unsigned int  wFileNum = 0;////�Ѿ����͵�Ŀ¼������
static unsigned int  wSendDATNumOld = 0;
static char *strtemp ; //CFG�ַ�����ָ��
 //wCfgTotalNum =1;//�ܳ���static WORD 
 //section_current = 1;//��ǰ��*/static WORD
//gRecorder_flag.pRXBuff[256] = {0}; //�����յ�68��֡������������Ϊ���Ͷ���Ϣ��ʱ�򲻷��ͳ�֡


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
//*��������cfg_dat_length
//*���ܣ����ڡ�CFG�ļ��Ƕ�������ݣ���Ҫ�Զ����.CFG�ļ��޸���¼����ʼ��ʱ���¼��������ʱ��
//*����: ��������.CFG�ĳ���
//***********************************************************************************
void cfg_dat_length(int file_current_num)
{
     char ch[100]; 
     unsigned int tt=0,xt;
     char i;	 
    {  //.CFG�ĳ���
       //���վ��
      memcpy(ComtrderCfg1,g_gLBName,g_gLBNameLen);
      tt += g_gLBNameLen;	
      memcpy(&ComtrderCfg1[tt],",",1);//���","
      tt +=1;	  
	//���վ��ַ
      sprintf((char *)ch,"%0d",g_gRunPara[RP_COMM_ADDR]);        
      memcpy(&ComtrderCfg1[tt],ch,strlen(ch));
      tt +=strlen(ch);	  
      //�������
      //memcpy(&ComtrderCfg1[tt],ComtrderCfg,strlen(ComtrderCfg));//�������
      //tt +=strlen(ComtrderCfg);
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_1,strlen(ComtrderCfg_1));//�������
      tt +=strlen(ComtrderCfg_1);

      memcpy(&ComtrderCfg1[tt],ComtrderCfg_ua,strlen(ComtrderCfg_ua));//�������
      tt +=strlen(ComtrderCfg_ua);
      for(i = 0; (i< 6); i++,tt++)
	  	{ComtrderCfg1[tt]=ComtrderCfg_adj[i];}	  
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_2,strlen(ComtrderCfg_2));//�������
      tt +=strlen(ComtrderCfg_2);

      memcpy(&ComtrderCfg1[tt],ComtrderCfg_ub,strlen(ComtrderCfg_ub));//�������
      tt +=strlen(ComtrderCfg_ub);
      for(i = 0; (i< 6); i++,tt++)
	  	{ComtrderCfg1[tt]=ComtrderCfg_adj[i+6];}	  
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_2,strlen(ComtrderCfg_2));//�������
      tt +=strlen(ComtrderCfg_2);	  

      memcpy(&ComtrderCfg1[tt],ComtrderCfg_uc,strlen(ComtrderCfg_uc));//�������
      tt +=strlen(ComtrderCfg_uc);
      for(i = 0; (i< 6); i++,tt++)
	  	{ComtrderCfg1[tt]=ComtrderCfg_adj[12+i];}	  
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_2,strlen(ComtrderCfg_2));//�������
      tt +=strlen(ComtrderCfg_2);

      memcpy(&ComtrderCfg1[tt],ComtrderCfg_u0,strlen(ComtrderCfg_u0));//�������
      tt +=strlen(ComtrderCfg_u0);
      for(i = 0; (i< 6); i++,tt++)
	  	{ComtrderCfg1[tt]=ComtrderCfg_adj[i+18];}	  
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_2,strlen(ComtrderCfg_2));//�������
      tt +=strlen(ComtrderCfg_2);

      memcpy(&ComtrderCfg1[tt],ComtrderCfg_i0,strlen(ComtrderCfg_i0));//�������
      tt +=strlen(ComtrderCfg_i0);
      for(i = 0; (i< 6); i++,tt++)
	  	{ComtrderCfg1[tt]=ComtrderCfg_adj[i+24];}  
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_2,strlen(ComtrderCfg_2));//�������
      tt +=strlen(ComtrderCfg_2);
	  
      memcpy(&ComtrderCfg1[tt],ComtrderCfg_3,strlen(ComtrderCfg_3));//�������
      tt +=strlen(ComtrderCfg_3);
	//��Ӳ���Ƶ��
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
      //������� 
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
		{//ɽ����  ����Ƶ�ʾ�Ϊ2000
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

      //��ӽ�β
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
    {  //.CFG�ĳ���
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
//*��������FileDirectory
//*���ܣ���Ӧ��վ���ٻ�Ŀ¼������
//*����: ��������¼����Ŀ¼�ļ�
//***********************************************************************************
#ifdef YN_101S
//****************����¼���ļ�*****************************
unsigned char * FileDirectory(unsigned char *pTxBuf, unsigned char leng,unsigned int wave_total)//�ٻ�Ŀ¼
{
      unsigned int VSQ=0x80;
      char j=0;
      //unsigned int i;
      unsigned char *ptoBuf;

      ptoBuf = pTxBuf;

      pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]] = CALL_TYPE_YN;
      //pTxBuf[6+g_ucPara101[IECP_LINKADDR_NUM]] = wave_total| VSQ;//�ŏ| �������д
      pTxBuf += leng;
      gRecorder_flag.LIST_flag = OFF;

       for(;(wSendLISTNum <=  (g_sRecData.m_gRecANum+g_sRecData.m_gACTRecANum+g_sRecData.m_gXHRecANum));wSendLISTNum++,wFileNum++)
        {
        gRecorder_Readfilecfg.FileName = 0xff;
          for(;wFileNum<MAX_ALLREC_NUM;wFileNum++)
          	{
          	if(wFileNum<(MAX_REC_NUM+1))
          		{//����¼���ļ�
          		if((wFileNum!=g_sRecData.m_gRecCNum)
					&&((wFileNum<g_sRecData.m_gRecANum)
					      ||((wFileNum==g_sRecData.m_gRecANum)&&(wFileNum==MAX_REC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_DATA-256+ (unsigned long)(wFileNum+1)*0x2000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ����� 
				break;
				}
          		}
	   	else if(wFileNum<(MAX_REC_NUM+MAX_ACTREC_NUM+2))
	   		{//����¼��
          		if(((wFileNum-51)!=g_sRecData.m_gACTRecCNum)
					&&(((wFileNum-51)<g_sRecData.m_gACTRecANum)
					      ||(((wFileNum-51)==g_sRecData.m_gACTRecANum)&&((wFileNum-51)==MAX_ACTREC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_ACTDATA-256+ (unsigned long)(wFileNum-51+1)*0x90000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ�����       			
				break;
				}
	   		}
	   	else if(wFileNum<MAX_ALLREC_NUM)
	   		{//Ϩ��¼��
          		if(((wFileNum-62)!=g_sRecData.m_gXHRecCNum)
					&&(((wFileNum-62)<g_sRecData.m_gXHRecANum)
					      ||(((wFileNum-62)==g_sRecData.m_gXHRecANum)&&((wFileNum-62)==MAX_XHREC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_XHDATA-256+(unsigned long)(wFileNum-62+1)*0x8000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ�����       			
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
		{//�ļ�Ŀ¼�����꣬�����ļ�Ŀ¼����֡
		gRecorder_Readfilecfg.CFG_SOF = 0x20;  //<0>:=���滹��Ŀ¼�ļ���<1>:=���Ŀ¼�ļ�  
            	gRecorder_flag.LIST_flag = OFF;
	     //�ܰ�1�ֽ� ��ǰ��1�ֽ�
          	*pTxBuf++ = LOBYTE(wSendLISTNum/*wave_total*/);	   
          	*pTxBuf++ = LOBYTE(wSendLISTNum);	  
	   	ptoBuf[6+g_ucPara101[IECP_LINKADDR_NUM]] = VSQ;	//�ŏ|ȷ������	
	   	ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 10;	//����ԭ��=10
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
          Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_cfg[wSendLISTNum],sizeof(gRecorder_cfg[wSendLISTNum])); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ����� 
          if(wSendLISTNum >= (wave_total))//SOF״̬-1
          {//������֡
            gRecorder_cfg[wSendLISTNum].CFG_SOF = 0x20;  //<0>:=���滹��Ŀ¼�ļ���<1>:=���Ŀ¼�ļ�  
            gRecorder_flag.LIST_flag = OFF;
	     //�ܰ�1�ֽ� ��ǰ��1�ֽ�
          *pTxBuf++ = LOBYTE(wave_total);
          *pTxBuf++ = LOBYTE(wSendLISTNum);
	   ptoBuf[6+g_ucPara101[IECP_LINKADDR_NUM]] = VSQ;	//�ŏ|ȷ������	
	   ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 10;	//����ԭ��=10
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
//          if(gRecorder_cfg[wSendLISTNum].FileName<MAX_REC_NUM)//filename ���ܴ���MAX_REC_NUM�����ܱ�����
//          {
          //�ܰ�1�ֽ� ��ǰ��1�ֽ�
          *pTxBuf++ = LOBYTE(wave_total);
	   //*pTxBuf++ = HIBYTE(wave_total);	
          *pTxBuf++ = LOBYTE(wSendLISTNum);
	   //*pTxBuf++ = HIBYTE(wSendLISTNum);	   
          //cfg�ļ��� 8�ֽ� 7�ֽ�ʱ��+1�ֽڱ��	   
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC];//¼������ʱ��
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(gRecorder_Readfilecfg.comtrade_time[RTC_WEEK]<<5);
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_Readfilecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR];
		*pTxBuf++ = (gRecorder_Readfilecfg.FileName<<1) &0xfe;//0λ=0�� cfg�ļ�
          //�ļ����� 3�ֽ�      
          //*pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng;
         // *pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng>>8;
         // *pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng>>16; 
         *pTxBuf++ = 3;
	  *pTxBuf++ = 0x01;	  
	   *pTxBuf++ = 0;	 
          //SOF 1�ֽ� 	
        *pTxBuf++ = gRecorder_Readfilecfg.CFG_SOF;
          //�ļ�����ʱ�� 7�ֽ�       
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC];//¼������ʱ��
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(gRecorder_Readfilecfg.comtrade_time[RTC_WEEK]<<5);
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_Readfilecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR];
	   //*****************************************************	
          //data�ļ��� 8�ֽ� 7�ֽ�ʱ��+1�ֽڱ��		   
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC];//¼������ʱ��
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(gRecorder_Readfilecfg.comtrade_time[RTC_WEEK]<<5);
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_Readfilecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR];	
		*pTxBuf++ = (gRecorder_Readfilecfg.FileName<<1) |0x01;////0λ=1�� data�ļ�
          //�ļ����� 3�ֽ�      
          *pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng*20;
          *pTxBuf++ = (gRecorder_Readfilecfg.TOTAL_Leng*20)>>8;
          *pTxBuf++ = (gRecorder_Readfilecfg.TOTAL_Leng*20)>>16;  
          //SOF 1�ֽ� 	
        *pTxBuf++ = gRecorder_Readfilecfg.CFG_SOF;
          //�ļ�����ʱ�� 7�ֽ�       
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC];//¼������ʱ��
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(gRecorder_Readfilecfg.comtrade_time[RTC_WEEK]<<5);
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_Readfilecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR];		
	j+=2;	
        }
      
	ptoBuf[6+g_ucPara101[IECP_LINKADDR_NUM]] = j| VSQ;	//�ŏ|ȷ������
	wSendLISTNum++;            
       if( gRecorder_flag.LIST_flag == OFF)wSendLISTNum = 0;
     
     return pTxBuf;
}
//****************����¼���ļ�*****************************
#else

unsigned char * FileDirectory(unsigned char *pTxBuf, unsigned char leng,unsigned int wave_total)//�ٻ�Ŀ¼
{
      unsigned int VSQ=0x80;
      char j=0;
      //char filenum=0;
      //char k=0;
      unsigned char *ptoBuf;

      ptoBuf = pTxBuf;
      pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]] = CATALOG_TYPE;

      //pTxBuf[6+g_ucPara101[IECP_LINKADDR_NUM]] = wave_total| VSQ;//�ŏ| �������д
      pTxBuf += leng;
      gRecorder_flag.LIST_flag = OFF;

      if((g_sRecData.m_gACTRecANum==0)&&(g_sRecData.m_gRecANum==0)&&(g_sRecData.m_gXHRecANum==0))
      { //��¼���ļ�      
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
          		{//����¼���ļ�
          		if((wFileNum!=g_sRecData.m_gRecCNum)
					&&((wFileNum<g_sRecData.m_gRecANum)
					      ||((wFileNum==g_sRecData.m_gRecANum)&&(wFileNum==MAX_REC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_DATA-256+ (unsigned long)(wFileNum+1)*0x2000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ����� 
				break;
				}
          		}
	   	else if(wFileNum<(MAX_REC_NUM+MAX_ACTREC_NUM+2))
	   		{//����¼��
          		if(((wFileNum-51)!=g_sRecData.m_gACTRecCNum)
					&&(((wFileNum-51)<g_sRecData.m_gACTRecANum)
					      ||(((wFileNum-51)==g_sRecData.m_gACTRecANum)&&((wFileNum-51)==MAX_ACTREC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_ACTDATA-256+ (unsigned long)(wFileNum-51+1)*0x90000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ�����       			
				break;
				}
	   		}
	   	else if(wFileNum<MAX_ALLREC_NUM)
	   		{//Ϩ��¼��
          		if(((wFileNum-62)!=g_sRecData.m_gXHRecCNum)
					&&(((wFileNum-62)<g_sRecData.m_gXHRecANum)
					      ||(((wFileNum-62)==g_sRecData.m_gXHRecANum)&&((wFileNum-62)==MAX_XHREC_NUM))))
          			{
          			 long FADDR_RECORDER =FADDR_RECORDER_XHDATA-256+(unsigned long)(wFileNum-62+1)*0x8000;
          			 Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ�����       			
				break;
				}	   		
	   		}
	   	else
	   		{//erro
	   		gRecorder_Readfilecfg.FileName = 0xff;
	   		}
          	}
          //long FADDR_RECORDER =FADDR_RECORDER_INFO+ (long)wSendLISTNum*(long)FLINEADDR;
          //Sst26vf064b_Read(FADDR_RECORDER,(unsigned char *)&gRecorder_cfg[wSendLISTNum],sizeof(gRecorder_cfg[wSendLISTNum])); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ����� 
          if(wSendLISTNum >= (g_sRecData.m_gRecANum+g_sRecData.m_gACTRecANum+g_sRecData.m_gXHRecANum-1))//SOF״̬-1
          {
            gRecorder_Readfilecfg.CFG_SOF = 0x20;  //<0>:=���滹��Ŀ¼�ļ���<1>:=���Ŀ¼�ļ�  
            //wSendLISTNum = 0;
            gRecorder_flag.LIST_flag = OFF;
          }      
          else
          {
            gRecorder_Readfilecfg.CFG_SOF = 0;
            gRecorder_flag.LIST_flag = ON;
          }
          if((gRecorder_Readfilecfg.FileName<=MAX_ALLREC_NUM)&&(gRecorder_Readfilecfg.FileName==wFileNum))//filename ���ܴ���MAX_REC_NUM�����ܱ�����
          {
	   *pTxBuf++ = gRecorder_Readfilecfg.FileName;//�ŏ|
          *pTxBuf++ = gRecorder_Readfilecfg.FileName >> 8;       
      
          *pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng;
          *pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng>>8;
          *pTxBuf++ = gRecorder_Readfilecfg.TOTAL_Leng>>16;  
		  
          *pTxBuf++ = gRecorder_Readfilecfg.CFG_SOF;
      
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MICROSEC];//¼������ʱ��
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_HOUR];
        //*pTxBuf++= gRecorder_cfg[wSendLISTNum].comtrade_time[RTC_DATE]|(gRecorder_cfg[wSendLISTNum].comtrade_time[RTC_WEEK]<<5);
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_Readfilecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_YEAR];
	j++;	
        }
      }
	ptoBuf[6+g_ucPara101[IECP_LINKADDR_NUM]] = j| VSQ;	//�ŏ|ȷ������
	            
       if( gRecorder_flag.LIST_flag == OFF)
	   	{
	   	wSendLISTNum = 0;wFileNum=0;
		}
     }
     return pTxBuf;
}
#endif
//***********************************************************************************
//*��������ApprovalFile
//*���ܣ��õ���վ�ϿɵĽڴ���󣬷�����һ��׼�������ı���
//*����: �������ڵ�ʱ�������нڵ�У�飬������֤�ô�¼������ȷ��
//***********************************************************************************

unsigned char *  ApprovalFile(unsigned char *pTxBuf,unsigned char leng,RECORDER_CFG *pgRecorder_cfg,WORD wSecLenPtr)
{
  if(pTxBuf[leng + 3]==3)//�ڴ�����Ͽ�,AFQ
  {
    if(section_current==1) //�Ͽɵ�һ�ں��ͽ�2׼������
    {
      section_current=2;
      pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= SECTIONPREPAR_TYPE;
      pTxBuf += leng;
          
      *pTxBuf++ = gRecorder_flag.pRXBuff[leng];//�ļ���������ֽ�
      *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];
      *pTxBuf++ = section_current;//��ǰ������
          //FileName = MAKEWORD(gRecorder_flag.pRXBuff[leng + 1],gRecorder_flag.pRXBuff[leng + 2]);
       if(section_current == 1)//������Ϊ0����.CFG�Ľڳ���
       {
          *pTxBuf++ = pgRecorder_cfg->CFG_Leng;
          *pTxBuf++ = pgRecorder_cfg->CFG_Leng>>8; 
           GLubocfg_Sum =0;
          
       }
       else//����.DAT�ڵĳ���
       {
          *pTxBuf++ = pgRecorder_cfg->DATA_Leng;
          *pTxBuf++ = pgRecorder_cfg->DATA_Leng>>8;
           GLubodat_Sum =0;
       }
          *pTxBuf++ = 6;//SRQ=0,��һ��׼������
        
     }
     else if(section_current == 2)//�Ͽɵڶ��ں�������
     {
          pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= LASTSECTION_TYPE;//���͵�������Ϣ
          pTxBuf += leng;
          
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng];//�ļ���������ֽ�
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];
        *pTxBuf++ = section_current;//��ǰ������
        *pTxBuf++ =1;//LSQ=1 ����ֹͣ����Ľڴ���
          //pTxBuf[wSecLenPtr] = 1;//LSQ=3 ����ֹͣ����Ľڴ���
              //GLubocfg_Sum = ChkluboSum(ComtrderCfg1, strlen(ComtrderCfg1));
         *pTxBuf++ = GLubodat_Sum + GLubocfg_Sum;//�ڵ�У��λ
     }
   }
   else if(pTxBuf[leng + 3]==1)//�ļ�������Ͽ�AFQ
   {
      section_current=1;
      pTxBuf = NULL;
   }
  return pTxBuf;
}
//***********************************************************************************
//*��������FileDataCfg
//*���ܣ��ļ�1���䣬��Ӧ.cfg�ļ�
//*����: �����ļ����δ���
//***********************************************************************************

unsigned char *  FileDataCfg(unsigned char *pTxBuf, unsigned char leng,RECORDER_CFG *pgRecorder_cfg,int segment_leng)//WORD wSecLenPtr,
{
       //int segment_leng = SEGMENT_LENGTH;
       unsigned int j;
       
       unsigned char *pTxPos;
       
       wCfgTotalNum =strlen(ComtrderCfg1);//(pgRecorder_cfg->CFG_Leng);//�ŏ| 
           
       gRecorder_flag.LIST_flag = OFF;
         
       if(wSendCFGNum >= wCfgTotalNum)//���һ��
       {
          pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= LASTSECTION_TYPE;//���͵�������Ϣ
            pTxBuf += leng;
          *pTxBuf++ = gRecorder_flag.pRXBuff[leng ];//�ļ���������ֽ�
          *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];//
          *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 2];//������
          *pTxBuf++ = 3;//pTxBuf[wSecLenPtr]LSQ=3 ����ֹͣ����Ľڴ���
           GLubocfg_Sum = ChkluboSum((BYTE *)ComtrderCfg1, strlen(ComtrderCfg1));
          *pTxBuf++ = GLubocfg_Sum ;
          wSendCFGNum=0;
              
        }
        else
        {
              //for(i = wSendCFGNum,j = 0;(j < segment_leng)&&(i < wCfgTotalNum);i++,j++,w_gfgId++)
           pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= SEGMENT_TYPE;//���͵Ķ���Ϣ
           pTxBuf += leng;
          *pTxBuf++ = gRecorder_flag.pRXBuff[leng ];//�ļ���������ֽ�
          *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];//
          *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 2];//������
           pTxPos = pTxBuf;
          *pTxBuf++ =0; //pTxBuf[leng+3]  = 0;
           
           for(j = 0;(j < segment_leng)&&(wSendCFGNum < wCfgTotalNum);wSendCFGNum++,j++)
           {
               *pTxBuf++ = *strtemp++;
           }
          
           *pTxPos = pTxBuf - pTxPos-1;//LL
           //w_gfgId;//�γ���pTxBuf[wSecLenPtr]
           gRecorder_flag.LIST_flag = ON;
        }
             //wSendCFGNum += w_gfgId;
            //m_SendBuf.pBuf[wSecLenPtr] = m_SendBuf.wWritePtr - wSecLenPtr -1;
        return pTxBuf;    
}

//***********************************************************************************
//*��������FileDatadat
//*���ܣ��ļ�2���䣬��Ӧ.dat�ļ�
//*����: ���ݴ��䣬һ��һ�еĴ�����
//***********************************************************************************

unsigned char *  FileDatadat(unsigned char *pTxBuf,unsigned char leng,WORD wSecLenPtr,int segment_leng)
{
    
    unsigned char datBuff[16] = {0}; //8��ͨ��,ÿ��ͨ������ֵ��2��
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
    Sample_num=gRecorder_Readfilecfg.CFG_EndSamp;	//�ŏ| ¼��������
	//Sample_num=300;
    if( wSendDATNum >= Sample_num )//���һ��
    {
      pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= LASTSECTION_TYPE;//���͵�������Ϣ
      //pTxBuf[wSecLenPtr] = 3;//LSQ=3 ����ֹͣ����Ľڴ���
              //GLubocfg_Sum = ChkluboSum(ComtrderCfg1, strlen(ComtrderCfg1));
       pTxBuf += leng;
       *pTxBuf++ = gRecorder_flag.pRXBuff[leng ];//�ļ���������ֽ�
       *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];//
       *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 2];//������
       *pTxBuf++ = 3;//pTxBuf[wSecLenPtr]LSQ=3 ��ֹͣ����Ľڴ���
       *pTxBuf++ = GLubodat_Sum;
       wSendDATNum = 0;
       //return pTxBuf;
     }
     else
     {
        pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= SEGMENT_TYPE;//���͵Ķ���Ϣ
         pTxBuf += leng;
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng ];//�ļ���������ֽ�
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];//
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 2];//������
        
         pTxPos = pTxBuf;
        *pTxBuf++  = 0;//SEGMENT_LENGTH;
        
        for(int i=0;(wSendDATNum < Sample_num);wSendDATNum++)//ÿһ�е�ֵ
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
	       Sst26vf064b_Read(FLbAddrV,&datBuff[0],10);//WRITEPECORDER+��ѹÿָֻʾ���ĵ�һ�����ݴ������Ϣ
	       
          unsigned long xt =250;
          if(gRecorder_Readfilecfg.CFG_Samp==800)xt=1250;//�ŏ| ����¼�� Ƶ��800 ʱ����1250
      	   if(gRecorder_Readfilecfg.CFG_Samp==1600)xt=625;//�ŏ| ����¼�� Ƶ��1600 ʱ����625
      	   if(gRecorder_Readfilecfg.CFG_Samp==2000)xt=500;//�ŏ| ����¼�� Ƶ��1600 ʱ����625
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
                                 MAKEWORD(datBuff[4],datBuff[5]),MAKEWORD(datBuff[6],datBuff[7]),dka,(unsigned int)a,(unsigned int)b,(unsigned int)c);//ÿ������80���㣬��������250΢��//�ŏ|
                                 
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
            
            //wSendDATNum--;//�ŏ|
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
//*��������SectionPrepareFile
//*���ܣ���׼����������
//*��������վ����ڴ��䣬�ն˻ظ���׼����������Ҫ��������ڵĳ���
//***********************************************************************************
unsigned char *  SectionPrepareFile(unsigned char *pTxBuf,unsigned char leng,RECORDER_CFG *pgRecorder_cfg)
{
  if(pTxBuf[leng + 3]==2)  //�����ļ�����SRQ=0
  {
     pTxBuf += leng;   
    *pTxBuf++ = gRecorder_flag.pRXBuff[leng ];//�ļ���������ֽ�
    *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];
    *pTxBuf++ = section_current;//��ǰ������
          //FileName = MAKEWORD(gRecorder_flag.pRXBuff[leng + 1],gRecorder_flag.pRXBuff[leng + 2]);
    if(section_current == 1)//������Ϊ0����.CFG�Ľڳ���
    {
      *pTxBuf++ = pgRecorder_cfg->CFG_Leng;
      *pTxBuf++ = pgRecorder_cfg->CFG_Leng>>8;  
    }
    else//����.DAT�ڵĳ���
    {
       *pTxBuf++ = pgRecorder_cfg->DATA_Leng;
       *pTxBuf++ = pgRecorder_cfg->DATA_Leng>>8;  
    }
       *pTxBuf++ = 0;//SRQ=0,��һ��׼������
  }
  return pTxBuf;
}

//***********************************************************************************
//*��������FileDataCfg
//*���ܣ��ļ�1���䣬��Ӧ.cfg�ļ�
//*����: �����ļ����δ���
//***********************************************************************************

unsigned char *  FileDataCfg_YN(unsigned char *pTxBuf, unsigned char leng,RECORDER_CFG *pgRecorder_cfg,int segment_leng)//WORD wSecLenPtr,
{
       //int segment_leng = SEGMENT_LENGTH;
       unsigned int j;       
       unsigned char *pTxPos;
	unsigned char *ptoBuf;	  
	unsigned char temp;
      ptoBuf = pTxBuf;
       
       wCfgTotalNum =strlen(ComtrderCfg1);//(pgRecorder_cfg->CFG_Leng);//�ŏ|            
       gRecorder_flag.LIST_flag = OFF;
        {
           pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= CALL_FILE_YN;//���ͱ�ʾ 137 0x89
           pTxBuf += leng;
	//�ļ�����8���ֽڣ�
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_MICROSEC];//¼������ʱ��
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_SEC];
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_MINUT];
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_HOUR];        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_MONTH];
        if(pgRecorder_cfg->comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_YEAR];
	 *pTxBuf++ = (pgRecorder_cfg->FileName<<1) &0xfe;//0λ=0�� cfg�ļ�
	//�ܰ���2�ֽڣ���ǰ����2�ֽڣ�
	 *pTxBuf++= 2;*pTxBuf++=0;	
	if(wSendCFGNum>8)
		{*pTxBuf++=1;}
	else
		{*pTxBuf++=0;}
	*pTxBuf++=0;
	//�����ȣ�1�ֽڣ��������д
           pTxPos = pTxBuf;
          *pTxBuf++ =0; //pTxBuf[leng+3]  = 0;
	if(wSendCFGNum >= wCfgTotalNum)
	    	{//����֡
	    	gRecorder_flag.LIST_flag = OFF;
		wSendCFGNum=0;
		section_current = 0;
		ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 10;	//����ԭ��=13
              temp = g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
              ptoBuf[7+temp+12]=0;	//������
              ptoBuf[7+temp+10] = 2;	//��ǰ��
              return pTxBuf;
	    	}
           
           for(j = 0;(j < segment_leng)&&(wSendCFGNum < wCfgTotalNum);wSendCFGNum++,j++)
           {
               *pTxBuf++ = *strtemp++;
           }
          
           *pTxPos = pTxBuf - pTxPos-1;//LL
           //w_gfgId;//�γ���pTxBuf[wSecLenPtr]
           ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 13;	//����ԭ��=13
           temp = g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
           ptoBuf[7+temp+12] = j;	//������
           gRecorder_flag.LIST_flag = ON;


        }
             //wSendCFGNum += w_gfgId;
            //m_SendBuf.pBuf[wSecLenPtr] = m_SendBuf.wWritePtr - wSecLenPtr -1;
        return pTxBuf;    
}

//***********************************************************************************
//*��������FileDatadat
//*���ܣ��ļ�2���䣬��Ӧ.dat�ļ�
//*����: ���ݴ��䣬һ��һ�еĴ�����
//***********************************************************************************

unsigned char *  FileDatadat_YN(unsigned char *pTxBuf,unsigned char leng,WORD wSecLenPtr,int segment_leng)
{
    
    unsigned char datBuff[16] = {0}; //8��ͨ��,ÿ��ͨ������ֵ��2��
    unsigned char k,temp;
    unsigned int Sample_num;// = 100;CRG_SAMPLE_NUM;CFG_EndSamp
    gRecorder_flag.LIST_flag = OFF;
    WORD FileName;
    char ch[250];
    unsigned char snum=0;
    unsigned char *pTxPos;
    unsigned char *ptoBuf;	 	
    ptoBuf = pTxBuf;
		  
    temp= gRecorder_flag.pRXBuff[leng+7];//�ļ���8���ֽ� 7�ֽ�ʱ��+1�ֽڱ��
    FileName = temp>>1;
    Sample_num=gRecorder_filecfg.CFG_EndSamp;	//�ŏ| ¼��������
	
     {
        pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= CALL_FILE_YN;//���ͱ�ʾ 137 0x89
         pTxBuf += leng;
	//�ļ�����8���ֽڣ�
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//¼������ʱ��
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_HOUR];        
	 *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          	*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          	*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR];
	 *pTxBuf++ = (gRecorder_filecfg.FileName<<1) |0x1;//0λ=0�� cfg�ļ�
	//�ܰ���2�ֽڣ���ǰ����2�ֽڣ�
	 *pTxBuf++= LOBYTE(Sample_num/10);
	 *pTxBuf++=HIBYTE(Sample_num/10);	
	*pTxBuf++=LOBYTE(wSendDATNum/10);
	*pTxBuf++=HIBYTE(wSendDATNum/10);
	//�����ȣ�1�ֽڣ��������д        
         pTxPos = pTxBuf;
        *pTxBuf++  = 0;//SEGMENT_LENGTH;
        if( wSendDATNum >= Sample_num )//���һ��
     		{//����֡
     		wSendDATNum = 0;
		gRecorder_flag.LIST_flag = OFF;
		section_current = 0;
           	ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 10;	//����ԭ��=13
           	temp = g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
           	ptoBuf[7+temp+12] = 0;	//������
           	ptoBuf[7+temp+10] = LOBYTE(Sample_num/10);
		ptoBuf[7+temp+11] = HIBYTE(Sample_num/10);			
     		return pTxBuf;		
     		}          
        
        for(int i=0;(wSendDATNum < Sample_num);wSendDATNum++)//ÿһ�е�ֵ
        {
          	unsigned long FLbAddrV = ((unsigned long)(FADDR_RECORDER_DATA+FileName*4)<<16)+(unsigned long)wSendDATNum*10;
	       Sst26vf064b_Read(FLbAddrV,&datBuff[0],10);//WRITEPECORDER+��ѹÿָֻʾ���ĵ�һ�����ݴ������Ϣ
	       
          unsigned long xt =250;
          if(gRecorder_filecfg.CFG_Samp==800)xt=1250;//�ŏ| ����¼�� Ƶ��800 ʱ����1250
      	   if(gRecorder_filecfg.CFG_Samp==1600)xt=625;//�ŏ| ����¼�� Ƶ��1600 ʱ����625
      	   if(gRecorder_filecfg.CFG_Samp==2000)xt=500;//�ŏ| ����¼�� Ƶ��1600 ʱ����625
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
                                 MAKEWORD(datBuff[4],datBuff[5]),MAKEWORD(datBuff[6],datBuff[7]),dka,dkd);//ÿ������80���㣬��������250΢��//�ŏ|
                                 
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
            
            //wSendDATNum--;//�ŏ|
            break;//return pTxBuf;
          }
          if(wSendDATNum == Sample_num-1)
          {
            wSendDATNum++;
            break;//return pTxBuf;
          }
             
        }
/*     if( wSendDATNum >= Sample_num )//���һ��
     	{
     	wSendDATNum = 0;
	gRecorder_flag.LIST_flag = OFF;
	section_current = 0;
     	}  */    
     }
           ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 13;	//����ԭ��=13
           temp = g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
           ptoBuf[7+temp+12] = snum;	//������	 
     return pTxBuf;
 }
//***********************************************************************************
//*��������RecCallFile_YN
//*���ܣ���Ӧ�ļ��ٻ� ����¼��Э��
//*����: 
//***********************************************************************************

unsigned char * RecCallFile_YN(unsigned char *pTxBuf, unsigned char leng,unsigned char filename)
{
      unsigned char *ptoBuf;
      unsigned char filetype = 0;	 
	  
      ptoBuf = pTxBuf;

	if(filename&1)filetype=1;
	filename = filename>>1;
	  
	pTxBuf += leng;
          //cfg�ļ��� 8�ֽ� 7�ֽ�ʱ��+1�ֽڱ��	   
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//¼������ʱ��
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_HOUR];        
	 *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR];
		*pTxBuf++ = (gRecorder_filecfg.FileName<<1) &0xfe;//0λ=0�� cfg�ļ�
          //�ļ����� 3�ֽ�
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
          //SOF 1�ֽ� 	
        *pTxBuf++ = 0x80;
          //�ļ�����ʱ�� 7�ֽ�       
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//¼������ʱ��
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_HOUR];        *pTxBuf++= gRecorder_Readfilecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR];
	//�ļ���������(1���ֽ�)
	*pTxBuf++= 200;	//�ļ���������(1���ֽ�)
	ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 7;	//����ԭ��=7
		
        	section_current=1;
		gRecorder_flag.LIST_flag = ON;

	  return pTxBuf;		
}

//***********************************************************************************
//*��������FileCfgOneFrame_YN
//*���ܣ��ļ�1���䣬��Ӧ.cfg�ļ�
//*����: �����ļ����δ���
//***********************************************************************************

unsigned char *  FileCfgOneFrame_YN(unsigned char *pTxBuf, unsigned char leng,RECORDER_CFG *pgRecorder_cfg,int framenum)//WORD wSecLenPtr,
{
       //int segment_leng = SEGMENT_LENGTH;
       unsigned int j;       
       unsigned char *pTxPos;
	unsigned char *ptoBuf;	  
	unsigned char temp;
      ptoBuf = pTxBuf;
       
       wCfgTotalNum =strlen(ComtrderCfg1);//(pgRecorder_cfg->CFG_Leng);//�ŏ|            
       gRecorder_flag.LIST_flag = OFF;

           pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= CALL_ONEFRAME_YN;//���ͱ�ʾ 137 0x89
           pTxBuf += leng;
	//�ļ�����8���ֽڣ�
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_MICROSEC];//¼������ʱ��
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_SEC];
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_MINUT];
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_HOUR];        
	 *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_MONTH];
        if(pgRecorder_cfg->comtrade_time[RTC_YEAR] >=2000)
          *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_YEAR]-2000;
        else
          *pTxBuf++= pgRecorder_cfg->comtrade_time[RTC_YEAR];
	 *pTxBuf++ = (pgRecorder_cfg->FileName<<1) &0xfe;//0λ=0�� cfg�ļ�
	//�ܰ���2�ֽڣ���ǰ����2�ֽڣ�
	 *pTxBuf++= 2;*pTxBuf++=0;	
	*pTxBuf++=LOBYTE(framenum);
	*pTxBuf++=HIBYTE(framenum);
	//�����ȣ�1�ֽڣ��������д
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
           ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 7;	//����ԭ��=7
           temp = g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
           ptoBuf[7+temp+12] = j;	//������
       gRecorder_flag.LIST_flag = OFF;
	   
        return pTxBuf;    
}
//***********************************************************************************
//*��������FileDatOneFrame_YN
//*���ܣ��ļ�2���䣬��Ӧ.dat�ļ�
//*����: ���ݴ��䣬һ��һ�еĴ�����
//***********************************************************************************

unsigned char *  FileDatOneFrame_YN(unsigned char *pTxBuf,unsigned char leng,WORD wSecLenPtr,int framenum)
{
    
    unsigned char datBuff[16] = {0}; //8��ͨ��,ÿ��ͨ������ֵ��2��
    unsigned char k,temp;
    unsigned int Sample_num;// = 100;CRG_SAMPLE_NUM;CFG_EndSamp
    gRecorder_flag.LIST_flag = OFF;
    WORD FileName;
    char ch[250];
    unsigned char *pTxPos;
    unsigned char *ptoBuf;	 	
    ptoBuf = pTxBuf;

    framenum = framenum*10;		  
    temp= gRecorder_flag.pRXBuff[leng+7];//�ļ���8���ֽ� 7�ֽ�ʱ��+1�ֽڱ��
    FileName = temp>>1;
    Sample_num=gRecorder_filecfg.CFG_EndSamp;	//�ŏ| ¼��������	

        pTxBuf[5+g_ucPara101[IECP_LINKADDR_NUM]]= CALL_ONEFRAME_YN;//���ͱ�ʾ 138 0x8a
         pTxBuf += leng;
	//�ļ�����8���ֽڣ�
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MICROSEC];//¼������ʱ��
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_SEC];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MINUT];
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_HOUR];        
	 *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_DATE]|(g_gWeekNum<<5);	//�ŏ|	
        *pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_MONTH];
        if(gRecorder_filecfg.comtrade_time[RTC_YEAR] >=2000)
          	*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR]-2000;
        else
          	*pTxBuf++= gRecorder_filecfg.comtrade_time[RTC_YEAR];
	 *pTxBuf++ = (gRecorder_filecfg.FileName<<1) |0x1;//0λ=0�� cfg�ļ�
	//�ܰ���2�ֽڣ���ǰ����2�ֽڣ�
	 *pTxBuf++= LOBYTE(Sample_num/10);
	 *pTxBuf++=HIBYTE(Sample_num/10);	
	*pTxBuf++=LOBYTE(framenum);
	*pTxBuf++=HIBYTE(framenum);
	//�����ȣ�1�ֽڣ��������д        
         pTxPos = pTxBuf;
        *pTxBuf++  = 0;//SEGMENT_LENGTH; 
        
        for(int i=0;(framenum < Sample_num);framenum++)//ÿһ�е�ֵ
        {
          	unsigned long FLbAddrV = ((unsigned long)(FADDR_RECORDER_DATA+FileName*4)<<16)+(unsigned long)framenum*10;
	       Sst26vf064b_Read(FLbAddrV,&datBuff[0],10);//WRITEPECORDER+��ѹÿָֻʾ���ĵ�һ�����ݴ������Ϣ
	       
          unsigned long xt =250;
          if(gRecorder_filecfg.CFG_Samp==800)xt=1250;//�ŏ| ����¼�� Ƶ��800 ʱ����1250
      	   if(gRecorder_filecfg.CFG_Samp==1600)xt=625;//�ŏ| ����¼�� Ƶ��1600 ʱ����625
      	   if(gRecorder_filecfg.CFG_Samp==2000)xt=500;//�ŏ| ����¼�� Ƶ��1600 ʱ����625
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
                                 MAKEWORD(datBuff[4],datBuff[5]),MAKEWORD(datBuff[6],datBuff[7]),dka,dkd);//ÿ������80���㣬��������250΢��//�ŏ|
                                 
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

           ptoBuf[7+g_ucPara101[IECP_LINKADDR_NUM]] = 7;	//����ԭ��=7
           temp = g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
           ptoBuf[7+temp+12] = 200;	//������	
           gRecorder_flag.LIST_flag = OFF; 
     return pTxBuf;
 }
//unsigned long FLbAddrStart;
//unsigned long FLbAddrEnd;
//***********************************************************************************
//*��������Code_Lubo
//*���ܣ�����Ӧ��ASDU����������Ӧ
//*������¼���ļ��Ĵ���
//***********************************************************************************
#ifdef YN_101S
void Code_Lubo(unsigned char *pRxBuf,unsigned char *pTXBuff)
{//1
  unsigned char *pTxBuf = pTXBuff;//���¶Է��ͻ���д������
  
  unsigned int wave_total;
  
  unsigned char leng;

  unsigned char temp;

  WORD FileName = 0;//�ļ����
  WORD framenum=0;  
  WORD wSecLenPtr=0;
 
  
  CAT_SpiReadWords(EEPADD_LUBONUM, 1, &wave_total);
  if(wave_total > MAX_REC_NUM)  wave_total=0;
  gRecorder_flag.LIST_flag = OFF;
  
  //�ӽ��յı����д����͵����ݣ��ӱ���ͷ����Ϣ���ַ68 00 00 68 08
  leng=7+g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
  if(pRxBuf != null )
  {
    memcpy(gRecorder_flag.pRXBuff,pRxBuf,6+pRxBuf[1]);  
  }
  
    memcpy(pTXBuff,gRecorder_flag.pRXBuff,6+pRxBuf[1]);//leng


  if(pTXBuff[7+g_ucPara101[IECP_LINKADDR_NUM]] == 8)//����ԭ��==8��ֹͣ����
  	{
    	section_current = 0;
	gRecorder_flag.LIST_flag = OFF;
	pTxBuf += leng;
	pTXBuff[7+g_ucPara101[IECP_LINKADDR_NUM]] =9;
	wSendDATNum = 0;
	wSendCFGNum=0;
	wSendLISTNum =0;
    	}
   else if(pTXBuff[7+g_ucPara101[IECP_LINKADDR_NUM]] == REQUEST_COT)//�ٻ�Ŀ¼�Ĵ���ԭ��
  {
    if(pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]] == CALL_TYPE_YN)//(wInfoAddr == 0x690A)//�ٻ�¼���ļ���Ŀ¼
    {
      pTxBuf = FileDirectory(pTxBuf,leng,wave_total);    
    }
    else if(pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]] == CALL_FILE_YN)//(wInfoAddr == 0x690A)//�ٻ�¼���ļ�
    	{
    	if((section_current!=1)&&(section_current!=2))
    		{//��Ӧ�ٻ��ļ�
    		temp= gRecorder_flag.pRXBuff[leng+7];//�ļ���8���ֽ� 7�ֽ�ʱ��+1�ֽڱ��
    		FileName = temp>>1;
    		long FADDR_RECORDER = FADDR_RECORDER_INFO +(long)FileName*(long)FLINEADDR;
    		Sst26vf064b_Read(FADDR_RECORDER ,(unsigned char *)&gRecorder_filecfg,sizeof(gRecorder_filecfg)); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ�����        
		pTxBuf = RecCallFile_YN(pTxBuf,leng,temp);
		if(temp&0x01)
			section_current = 2;
		else
			section_current = 1;
    		}
	else if(section_current==1)
		{//����cfg�ļ�
		temp= gRecorder_flag.pRXBuff[leng+7];//�ļ���8���ֽ� 7�ֽ�ʱ��+1�ֽڱ��
    		FileName = temp>>1;
		if(wSendCFGNum ==0)
              	cfg_dat_length(FileName);
           	pTxBuf = FileDataCfg_YN(pTxBuf,leng,&gRecorder_filecfg,200);//wSecLenPtr,
           	//section_current = 0;
		}
	else if(section_current==2)
		{//����data�ļ�
		
		pTxBuf = FileDatadat_YN(pTxBuf,leng,wSecLenPtr,200);
		//section_current = 0;
		}		
    	}
    else if(pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]] == CALL_ONEFRAME_YN)//�������ٻ�¼���ļ��е�һ֡
    	{
	temp= gRecorder_flag.pRXBuff[leng+7];//�ļ���8���ֽ� 7�ֽ�ʱ��+1�ֽڱ��
    	FileName = temp>>1; 
    	long FADDR_RECORDER = FADDR_RECORDER_INFO +(long)FileName*(long)FLINEADDR;
    	Sst26vf064b_Read(FADDR_RECORDER ,(unsigned char *)&gRecorder_filecfg,sizeof(gRecorder_filecfg)); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ�����        
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
  unsigned char *pTxBuf = pTXBuff;//���¶Է��ͻ���д������  
  unsigned int wave_total;  
  unsigned char leng; 
  WORD FileName = 0;//�ļ���� 
  WORD wSecLenPtr=0; 
  long FADDR_RECORDER;
  
  CAT_SpiReadWords(EEPADD_LUBONUM, 1, &wave_total);
  if(wave_total > MAX_REC_NUM)  wave_total=0;
  gRecorder_flag.LIST_flag = OFF;
  
  //�ӽ��յı����д����͵����ݣ��ӱ���ͷ����Ϣ���ַ68 00 00 68 08
  leng=7+g_ucPara101[IECP_LINKADDR_NUM]+g_ucPara101[IECP_TRANSRSN_NUM]+g_ucPara101[IECP_COMNADDR_NUM]+g_ucPara101[IECP_INFO_NUM];
  if(pRxBuf != null )
  {
    memcpy(gRecorder_flag.pRXBuff,pRxBuf,6+pRxBuf[1]);  
  }
  
    memcpy(pTXBuff,gRecorder_flag.pRXBuff,6+pRxBuf[1]);//leng
	
  if(pTXBuff[7+g_ucPara101[IECP_LINKADDR_NUM]] == REQUEST_COT)//�ٻ�Ŀ¼�Ĵ���ԭ��
  {
    if(pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]] == CALL_TYPE)//(wInfoAddr == 0x690A)//�ٻ�¼���ļ���Ŀ¼
    {
      //wSendLISTNum =0;
      pTxBuf = FileDirectory(pTxBuf,leng,wave_total);    
    }
  }
  else if(pTXBuff[7+g_ucPara101[IECP_LINKADDR_NUM]] == FILETRANS_COT)//�ٻ��ļ��Ĵ���ԭ��
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
    Sst26vf064b_Read(FADDR_RECORDER ,(unsigned char *)&gRecorder_Readfilecfg,sizeof(gRecorder_Readfilecfg)); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ�����        
    
    if(pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]]== CALL_TYPE)// 122 0x7A //�ٻ�Ŀ¼����ѡ����ѡ���ļ�����ѡ�����ٻ��ļ����ٻ���
    {

      if(pTXBuff[leng + 3]==1)  //ѡ���ļ���SCQ=1
      {
         pTxBuf += leng;
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng ];//�ļ���������ֽ�
        *pTxBuf++ = gRecorder_flag.pRXBuff[leng + 1];//
        //FileName = MAKEWORD(gRecorder_flag.pRXBuff[leng + 1],gRecorder_flag.pRXBuff[leng + 2]);
        *pTxBuf++ =gRecorder_Readfilecfg.TOTAL_Leng;
        *pTxBuf++ =gRecorder_Readfilecfg.TOTAL_Leng>>8;
        *pTxBuf++ =gRecorder_Readfilecfg.TOTAL_Leng>>16;
        *pTxBuf++ =0x80;//�ļ�׼������  //ɽ��¼��Э��  =0��
        //pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]] = FILEPREPAR_TYPE;//120 0x78//ɽ��¼��Э�� 
        section_current=1;
      }
      else if(pTXBuff[leng + 3]==6)//�����
      {
         int segment_leng = SEGMENT_LENGTH;
         if( gRecorder_flag.pRXBuff[leng + 2]==1)//������Ϊ1 ���������ļ���������Ϣ
         {
           if(wSendCFGNum ==0)
              cfg_dat_length(FileName);
           pTxBuf = FileDataCfg(pTxBuf,leng,&gRecorder_Readfilecfg,segment_leng);//wSecLenPtr,
              
         }
         else if(gRecorder_flag.pRXBuff[leng + 2]==2)//������Ϊ2 ���������ļ���������Ϣ
         {
            pTxBuf = FileDatadat(pTxBuf,leng,wSecLenPtr,segment_leng);           
         }       
      }
	  /*//ɽ��¼��Э�� 
      else if(pTXBuff[leng + 3]==2)  //�����ļ����䣬SCQ=2
      	{
      	pTxBuf = SectionPrepareFile(pTxBuf,leng,&gRecorder_Readfilecfg);//��ȷ��
      	}  */      
    }
    else if(pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]]== APPROVALFILE_TYPE)//124 0x7C//�Ͽ��ļ����ڵ����̴���
    {
      pTxBuf = ApprovalFile(pTxBuf,leng,&gRecorder_Readfilecfg,wSecLenPtr);
      if(pTxBuf == NULL) return;
         
    }
    else if((pTXBuff[5+g_ucPara101[IECP_LINKADDR_NUM]]== SECTIONPREPAR_TYPE))//121 0x79
    {
        pTxBuf = SectionPrepareFile(pTxBuf,leng,&gRecorder_Readfilecfg);
        
         
    } 
   }
  if(pRxBuf[4]>=0x80)//������BIT7ΪҪ����
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

//FLbAddr = FADDR_RECORDER_START + lubo_absolute_time;//����ʱ���ʱ�䶼ȡA���ʱ��

//FLbAddr = FADDR_RECORDER_START + lubo_absolute_time;//�ڶ��ξ���ʱ���ʱ��

//Sst26vf064b_Read(FLbAddr,ReadBack,RTC_NUM);//��ȡ���Ե�ַ��ʱ��



