/*------------------------------------------------------------------------
 Module:      ParaCfg.cpp
 Author:      linxueqin
 Project:     ��վƽ̨
 State:       V1.0
 Creation Date:   2014-4-29
 Description:   ���в�������Լ������ң�ţ�ң�⣬ң�ز����ĳ�ʼ�������漰��λ
------------------------------------------------------------------------*/


#ifndef _PARA_CFG

#define _PARA_CFG
  #include "DataStruct.h"
#endif

//==============================================================================
//  ��������   : InitPara
//  ��������   : ע����õ��Ĳ������п�����ʼ��������EEPROM��ȡ�����粻��ʧ������
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void InitcfgPara(void)
{
    unsigned int i;
    for(i = 0; i < PARA_NUM; i++)
    {
        g_gChangFlag[i] = OFF;
    }
    CheckRECPara();   //��ȡ�����ϵ���������ǰλ��	
      RstRMTInfo(0);	
    CheckCfgPara();
    InitTsYxInfo();
    InitTsYcInfo();
    
    CheckProtCnt(); //��ȡ������ֵ�������ֵ�����ö�Ӧ��ң��λ

    CheckAdjPara(); //��ȡ��ͨ��У��������������������ö�Ӧ��ң��λ��������Ĭ�ϲ���У��
    FEED_WATCH_DOG();
    CheckRunInfo(); //��ȡ������Ϣ�������Ϣ�������������Ϣ��0
   
    CheckTELNUMPara(); //��ȡ�绰����
     FEED_WATCH_DOG();

   // g_gChangFlag[CHNG_INFO] = ON;
    FEED_WATCH_DOG();
   // CheckParaData();    //���������ݵ���Ч��
}
void InitTsYxInfo(void)
{
    int i = 0;
    //unsigned int RIIndex = 0;
   // int sYxCount = 0;
   // int dYxCount = 0;
    //����ң�ŵ��������Ҫ�ϴ���ң�Ÿ���
    for(i = 0; i < IEC_YX_NUM; i++)
    {
        if(g_ucYXAddr[i] == 0)
            break;

    }
    g_ucYxTransNum = i;

    //g_ucSYxTrsStartId = 0xffff;


    //if(g_ucSYxTrsStartId == 0xffff)
      g_ucSYxTrsStartId = 0;

        
}
void InitTsYcInfo(void)
{
    int i = 0;
  unsigned int RIIndex = 0;
    //����ң����������Ҫ�ϴ���ң�����
    for(i = 0; i < IEC_YC_NUM; i++)
    {
        if(g_ucYCAddr[i] == 0)
            break;
    }
    g_ucYcTransNum = i;
    for(i = 0; i < g_ucYcTransNum; i++)   //������Ҫ�ϴ���ң��ֵ�����վ���������л���
    {
      RIIndex = g_ucYCAddr[i];
      if(RIIndex < 255)
          g_unYcTrans[i] = g_gRmtMeas[g_ucYCAddr[i] - 1];//(((unsigned long)g_gRmtMeas[g_ucYCAddr[i] - 1] * g_unYCCoe[i]) >> 10 )+ g_unYCOffset[i];
      else
        g_unYcTrans[i] = 0;
  }
}

//==============================================================================
//  ��������   : TELNUMTOggSmsPhone
//  ��������   : ����Һ��������ֻ��ż�������TEL_NUM[]��ת��Ϊ101��ʽ��g_gSmsPhone[],ͬʱ���ֻ����붨λ11λ
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ      : <��>
//  ����˵��   : 
//  ����       ��//�ŏ| 0328
//==============================================================================

void TELNUMTOggSmsPhone(void)
{

    //unsigned char i;

    //g_gSmsPhone[0]=11;g_gSmsPhone[16]=11;g_gSmsPhone[32]=11;g_gSmsPhone[48]=11;//���ֻ����붨λ11λ
    /*
    for(i=0;i<11;i++)	
    {
        g_gSmsPhone[1+i]=TEL_NUM[i];
    } 
    for(i=0;i<11;i++)	
    {
        g_gSmsPhone[17+i]=TEL_NUM[i+11];
    } 
    for(i=0;i<11;i++)	
    {
        g_gSmsPhone[33+i]=TEL_NUM[i+22];
    } 
    for(i=0;i<11;i++)	
    {
        g_gSmsPhone[49+i]=TEL_NUM[i+33];
    } 
 
    for(i=0;i<4;i++)	
    {
        g_gSmsPhone[64+i]=g_ucphone_perm[i];
    } 	
*/

}


//==============================================================================
//  ��������   : SaveCfgPara
//  ��������   : �����й����У�����g_gChangFlag�еĲ������ı�־�ж�ĳ�����ò��������仯�������ò������浽��Ӧ�Ĵ洢���ͱ�������
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void SaveCfgPara(void)  //�����й����У����ĳ�����ò��������仯�������ò������浽��Ӧ�Ĵ洢����
{
    unsigned int i;      //tPCSele[2],
    unsigned int temp[8];
    unsigned long ulAddr;
	unsigned char u_dellubo;
    //unsigned int ucTemp[2];
    if(g_gChangFlag[CHNG_INFO] == ON)       //��ֵ�����޸�
    {
        g_gChangFlag[CHNG_INFO] = OFF;
        g_gRunInfo[INF_CRC] = CrcCount(g_gRunInfo, INF_CRC);    //����CRC
       
        CAT_SpiWriteWords(EEPADD_COM, RUN_INFO_NUM, g_gRunInfo);  //���浽EEPROM��
        CAT_SpiWriteWords(EEPADDBK_COM, RUN_INFO_NUM, g_gRunInfo);    //���浽EEPROM�ı�������
           
    }

    //��ѹbais
    if(g_gChangFlag[CHNG_ADJ] == ON)    //��ֵ�����޸�
    {
        g_gChangFlag[CHNG_ADJ] = OFF;
        g_gAdjPara[ADJ_CRC] = CrcCount((unsigned int *)g_gAdjPara, ADJ_CRC);      //����CRC
        CAT_SpiWriteWords(EEPADD_BIAS_V, ADJ_PARA_NUM, (unsigned int *)g_gAdjPara);     //���浽EEPROM��
        CAT_SpiWriteWords(EEPADDBK_BIAS_V, ADJ_PARA_NUM, (unsigned int *)g_gAdjPara);   //���浽EEPROM�ı�������
        //����ϵͳ
           WDTCTL = WDTPW+WDTIS1+WDTIS0 + WDTIS2;//�޸Ŀ��Ź������ڣ��Ӷ��ܹ���������
           _DINT();       //�ر�ȫ���ж�
           while(1);
 
    }

    //������ֵ
    if(g_gChangFlag[CHNG_PC] == ON || g_gChangFlag[CHNG_MUBIAO] == ON) //��ֵ�����޸�
    {
        
        g_gChangFlag[CHNG_PC] = OFF;
        if(g_gProcCnt[PC_JAG_P] == 0 && g_gProcCnt[PC_JAG_Z] == 0)
        {
           g_gProcCnt[PC_CNL_MODEL] = 0;
           g_gRunPara[RP_CNL_MODEL] = 0;
           g_gRmtInfo[YX_FUN_ON] =0;
           g_gProcCnt[PC_JAG_P] = 0;       //���оݱ�־
           g_gProcCnt[PC_JAG_Z] = 0;       //�����оݱ�־
        }
        else if(g_gProcCnt[PC_JAG_P] == 0x55 && g_gProcCnt[PC_JAG_Z] == 0x55)
        {
           g_gProcCnt[PC_CNL_MODEL] = 1;
           g_gRunPara[RP_CNL_MODEL] = 1;
           g_gRmtInfo[YX_FUN_ON] =  1;
           g_gProcCnt[PC_JAG_P] = 0x55;       //���оݱ�־
           g_gProcCnt[PC_JAG_Z] = 0x55;       //�����оݱ�־
        }
        else
        {
            g_gProcCnt[PC_CNL_MODEL] = 1;
            g_gRunPara[RP_CNL_MODEL] = 1;
            g_gRmtInfo[YX_FUN_ON] =  1;
            //g_gProcCnt[PC_JAG_P] = 0;       //���оݱ�־
            //g_gProcCnt[PC_JAG_Z] = 0x55;       //�����оݱ�־
        }

        for(i = 0; i <= PROC_CNT_NUM; i++)
           g_gRunPara[i + RP_UA_ADJ] = g_gProcCnt[i];
        g_gRunPara[RP_CRC] = CrcCount((unsigned int *)g_gRunPara, RP_CRC);      //����CRC
        CAT_SpiWriteWords(EEPADD_RP, RUN_PARA_NUM, g_gRunPara); //���浽EEPROM��
        CAT_SpiWriteWords(EEPADDBK_RP, RUN_PARA_NUM, g_gRunPara); //���浽EEPROM��        
        /*
        //�ŏ| Ŀ��У׼����λ�����ز���10�ֽ� ��ʼֵΪ��ѹ60V,����2A g_gAdjObj[ADJ_PARA_NUM]	 
	      g_gAdjObj[ADJ_CRC] = CrcCount((unsigned int *)g_gAdjObj, ADJ_CRC);      //����CRC
             CAT_SpiWriteWords(EEPADD_ADJOBJ , ADJ_PARA_NUM, (unsigned int *)g_gAdjObj);     //���浽EEPROM��
             CAT_SpiWriteWords(EEPADDBK_ADJOBJ , ADJ_PARA_NUM, (unsigned int *)g_gAdjObj);   //���浽EEPROM�ı�������
             //�ŏ| Ŀ��У׼����λ�����ز���10�ֽ� ��ʼֵΪ��ѹ60V,����2A
        */
        //����ʵ�ʳ������ж���Ҫ���Ѷ�ֵ����ϵ��������ڲ�����Ķ�ֵ����
        CalcProtCnt();
        
       // g_gProcCnt[PC_CRC] = CrcCount((unsigned int *)g_gProcCnt, PC_CRC);      //����CRC
       // CAT_SpiWriteWords(EEPADD_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);     //���浽EEPROM��
      //  CAT_SpiWriteWords(EEPADDBK_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);    //���浽EEPROM�ı������� 
        if(g_gChangFlag[CHNG_MUBIAO] == ON)
        {
             g_gChangFlag[CHNG_MUBIAO] = OFF;		
		//�ŏ| Ŀ��У׼����λ�����ز���10�ֽ� ��ʼֵΪ��ѹ60V,����2A g_gAdjObj[ADJ_PARA_NUM]	 
	     g_gAdjObj[ADJ_CRC] = CrcCount((unsigned int *)g_gAdjObj, ADJ_CRC);//����CRC             
             CAT_SpiWriteWords(EEPADD_ADJOBJ, ADJ_PARA_NUM, g_gAdjObj);//���浽EEPROM��               
             CAT_SpiWriteWords(EEPADDBK_ADJOBJ , ADJ_PARA_NUM, g_gAdjObj);   //���浽EEPROM�ı�������
             //�ŏ| Ŀ��У׼����λ�����ز���10�ֽ� ��ʼֵΪ��ѹ60V,����2A
             
             //����ϵͳ
           WDTCTL = WDTPW+WDTIS1+WDTIS0 + WDTIS2;//�޸Ŀ��Ź������ڣ��Ӷ��ܹ���������
           _DINT();       //�ر�ȫ���ж�
           while(1);
        }
    }
    if(g_gChangFlag[CHNG_TEL] == ON)       //�绰���뷢���޸�
    {
        g_gChangFlag[CHNG_PC] = OFF;  

  	TELNUMTOggSmsPhone();//�ŏ| 0328
         g_gSmsPhone[PHONE_CS] = AddChar(g_gSmsPhone, PHONE_CS);      //����CS//�ŏ| 0328
        CAT_SpiWriteBytes(EEPADD_PHONE , PHONE_PA_NUM, g_gSmsPhone);     //���浽EEPROM��//�ŏ| 0328
    }
    RefreshPara();
 
    //���в���
    if((g_ucParaChang & BIT0) == BIT0)
    {
       u_dellubo =0;
        if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_DEL_LUBO])
    	{
    	u_dellubo = 1;
    	g_gRunPara[RP_CFG_KEY]=g_gRunPara[RP_CFG_KEY]&(~BIT[RPCFG_DEL_LUBO]);
    	}
    if(g_gRunPara[RP_PLUSE_TIME]<=g_gRunPara[RP_PLUSE_MODFK])
    {
        g_gRunPara[RP_PLUSE_MODFK]= 0;
    }
    if(g_gRunPara[RP_PLUSE_TIME]<=g_gRunPara[RP_PLUSE_AMODFK])
    {
        g_gRunPara[RP_PLUSE_AMODFK]= 0;
    }	
    if(g_gRunPara[RP_PLUSE_TIME]<=g_gRunPara[RP_PLUSE_BMODFK])
    {
        g_gRunPara[RP_PLUSE_BMODFK]= 0;
    }	
    if(g_gRunPara[RP_PLUSE_TIME]<=g_gRunPara[RP_PLUSE_CMODFK])
    {
        g_gRunPara[RP_PLUSE_CMODFK]= 0;
    }		
    if((g_gRunPara[RP_PLUSE_NUM]<6)||(g_gRunPara[RP_PLUSE_NUM]>8))
    {
        g_gRunPara[RP_PLUSE_NUM]= 8;
    }	
        g_ucParaChang &= NBIT0;
        g_gRunPara[RP_CRC] = CrcCount((unsigned int *)g_gRunPara, RP_CRC);      //����CRC
        CAT_SpiWriteWords(EEPADD_RP, RUN_PARA_NUM, g_gRunPara); //���浽EEPROM��
        CAT_SpiWriteWords(EEPADDBK_RP, RUN_PARA_NUM, g_gRunPara); //���浽EEPROM��           

	 /*			
        ucTemp[1] = g_ucPara_stime;ucTemp[0] = g_ucPara_stime; //�ŏ| 0329  �����ϴ����ʱ��
        CAT_SpiWriteWords(EEPADD_STIME_PRAR, 2,ucTemp) ;   //�ŏ| 0329  �����ϴ����ʱ��
        
        g_ucPara_stime = g_gRunPara[RP_SENDSMS_T];//�ŏ| ң����ʼ��ַ�޸����в���
        
	 if((g_ucPara_stime <2)||(g_ucPara_stime>48))  ////Ԥ�ó�24Сʱ����һ��
            g_ucPara_stime = 24; 
        */
        if(g_gRunPara[RP_SENDYC_T] > 0)   //��ʱ�ϴ�ң��
#ifdef YN_101S
            g_gsendYCTimeNum = ((unsigned long)g_gRunPara[RP_SENDYC_T]*10);
#else
            g_gsendYCTimeNum = ((unsigned long)g_gRunPara[RP_SENDYC_T]);
#endif
        else
            g_gsendYCTimeNum = 0;
        if(g_gRunPara[RP_SENDYX_T] > 0)   //��ʱ�ϴ�ң��
            g_gsendYXTimeNum = ((unsigned long)g_gRunPara[RP_SENDYX_T]);
        else
            g_gsendYXTimeNum = 0;
        if(g_gRunPara[RP_BEAT_T] > 0)   //��ʱ����
            g_gBeatNum = ((unsigned long)g_gRunPara[RP_BEAT_T]);
        else
            g_gBeatNum = 0;
	 if(g_gRunPara[RP_SENDZJ_T] > 0)   //��ʱ�ϴ��Լ�
            g_gsendZJTimeNum = ((unsigned long)g_gRunPara[RP_SENDZJ_T])*10;
        else
            g_gsendZJTimeNum = 0;	
        if(g_gRunPara[RP_CNL_MODEL] == 0)
        {
           g_gRmtInfo[YX_FUN_ON] = 0;
           g_gRunPara[RP_JAG_P] = 0;       //���оݱ�־
           g_gRunPara[RP_JAG_Z] = 0;       //�����оݱ�־
        }
        else
        {
           g_gRmtInfo[YX_FUN_ON] = 1;
          // g_gRunPara[RP_JAG_P] = 0x55;       //���оݱ�־
          // g_gRunPara[RP_JAG_Z] = 0x55;       //�����оݱ�־
        }
	 if((g_gRunPara[RP_T_DELAY]/10-200)<=g_gRunPara[RP_RHSEND_TIME1])
	 	{
	 	g_gRunPara[RP_RHSEND_TIME1]=100;
		g_gRunPara[RP_T_DELAY] = 10000;               //������ʱʱ��
	 	}
	if(u_dellubo==1 )
    	{
    	//g_gRunPara[RP_CFG_KEY]=g_gRunPara[RP_CFG_KEY]&(~BIT[RPCFG_DEL_LUBO]);
    	u_dellubo  = 0;
		DelALLSOE();
    		temp[0]=0;temp[1]=0;temp[2]=0;temp[3]=0;temp[4]=0;temp[5]=0;    	
       	CAT_SpiWriteWords(EEPADD_LUBONUM, 6, temp); 
        	g_sRecData.m_gRecANum=temp[0];//¼��������1~32
      		g_sRecData.m_gRecCNum=temp[1];//¼����ǰλ��0~31
      		g_sRecData.m_gACTRecANum=temp[2];//¼��������1~32
      		g_sRecData.m_gACTRecCNum=temp[3];//¼����ǰλ��0~31      
      		g_sRecData.m_gXHRecANum=temp[4];//¼��������1~32
      		g_sRecData.m_gXHRecCNum=temp[5];//¼����ǰλ��0~31

		  ulAddr = FADDR_RECORDER_DATA+(unsigned long)(g_sRecData.m_gRecCNum)*0x2000;//flash��ַ  
  Sector_Erase(ulAddr);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x1000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  
  ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;  
  g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ  
  Block_Erase(ulAddr);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x10000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x20000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x30000);//ERASE 1��BLOCK 
  delayms(100);WatchDog(); 
  Block_Erase(ulAddr+0x40000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x50000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x60000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x70000);//ERASE 1��BLOCK 
  delayms(100);WatchDog(); 
  Block_Erase(ulAddr+0x80000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();  

  ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;//flash��ַ
  //g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ  
  Sector_Erase(ulAddr);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x1000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x2000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x3000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x4000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x5000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x6000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x7000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();  
  FlashReading = 0;	
  
        }
	 if(g_gRunPara[RP_RHSEND_TIME1]<65)
	 	g_gRunPara[RP_RHSEND_TIME1]=100;
	 if(g_gRunPara[RP_CT_TRANS]==0)g_gRunPara[RP_CT_TRANS]=20;
         CalcProtCnt();

    }
    
    //IEC101��������
    if((g_ucParaChang & BIT1) == BIT1)
    {
        g_ucParaChang &= NBIT1;
        g_ucPara101[IECP_CRC] = AddChar(g_ucPara101, IECP_CRC);      //����CS
        CAT_SpiWriteBytes(EEPADD_IECPARA , IEC_PARA_NUM, g_ucPara101);     //���浽EEPROM��
        CAT_SpiWriteBytes(EEPADDBK_IECPARA, IEC_PARA_NUM, g_ucPara101);    //���浽EEPROM�ı�������
        //InitSCI();
    }
    //ң�ŵ�ַ���
    if((g_ucParaChang & BIT2) == BIT2)
    {
        g_ucParaChang &= NBIT2;
        /*g_ucYXAddr[IEC_YX_NUM] = AddChar(g_ucYXAddr, IEC_YX_NUM);      //����CS
        #define RDYXMUN_MAX 100 //���ζ�ȡe2������
        for(int i = 0;i<50;i++)
        {
          if((IEC_YX_NUM+1 - i*RDYXMUN_MAX) <= RDYXMUN_MAX)
          {
            CAT_SpiWriteWords(EEPADD_IECYXADDR+RDYXMUN_MAX*i*2, IEC_YX_NUM+1 - i*RDYXMUN_MAX, &g_ucYXAddr[RDYXMUN_MAX*i]);
            break;
          }
          else
          {
            CAT_SpiWriteWords(EEPADD_IECYXADDR+RDYXMUN_MAX*i*2, RDYXMUN_MAX, &g_ucYXAddr[RDYXMUN_MAX*i]);
          }
          WatchDog();
        }*/
        
        //CAT_SpiWriteBytes(EEPADD_IECYXADDR , IEC_YX_NUM*2 + 1, (unsigned char *)g_ucYXAddr);     //���浽EEPROM��
        //CAT_SpiWriteBytes(EEPADDBK_IECYXADDR, IEC_YX_NUM*2 + 1, (unsigned char *)g_ucYXAddr);    //���浽EEPROM�ı�������
        if(g_ucYXAddr[0] == 0)
        {
            RstIEC101YxAddr();  //ң�ŵ���ʼ��
            //DelALLSOE();
        }
        
        g_ucYXAddr[IEC_YX_NUM] = AddChar(g_ucYXAddr, IEC_YX_NUM);      //����CRC
        CAT_SpiWriteBytes(EEPADD_IECYXADDR , IEC_YX_NUM + 1, g_ucYXAddr);     //���浽EEPROM��
        CAT_SpiWriteBytes(EEPADDBK_IECYXADDR, IEC_YX_NUM + 1, g_ucYXAddr);    //���浽EEPROM�ı�������
        //DelALLSOE();
        //����ң�ŵ��������Ҫ�ϴ���ң�Ÿ���
        for(i = 0; i < IEC_YX_NUM; i++)
        {
            if(g_ucYXAddr[i] == 0)
                break;
        }
        g_ucYxTransNum = i;
       /* g_gRMTBitBackCount = 0; //���ң�ŵ���������ã������δ�ϴ��ĳ�ʱ��ң��
        g_gRMTBitBackCount_Aut = 0;
        g_EquInfo[COMM_INDEX_GPRS]  &= NBIT1;//���ʶ
        g_EquInfo_Aut  &= NBIT1;//���ʶ*/
        
   }
    //ң�����
    if((g_ucParaChang & BIT3) == BIT3)
    {
        g_ucParaChang &= NBIT3;             
        g_ucYCAddr[IEC_YC_NUM] = AddChar(g_ucYCAddr, IEC_YC_NUM);      //����CS
        CAT_SpiWriteBytes(EEPADD_IECYCADDR , IEC_YC_NUM + 1, g_ucYCAddr);     //���浽EEPROM��
        CAT_SpiWriteBytes(EEPADDBK_IECYCADDR, IEC_YC_NUM + 1, g_ucYCAddr);    //���浽EEPROM�ı�������
      
    }
    //ң�ص�ַ���
 /*   if((g_ucParaChang & BIT4) == BIT4)
    {
        g_ucParaChang &= NBIT4;
        g_ucYKPa[IEC_YK_NUM] = AddChar(g_ucYKPa, IEC_YK_NUM);      //����CS
        CAT_SpiWriteBytes(EEPADD_IECYKADDR , IEC_YK_NUM + 1, g_ucYKPa);     //���浽EEPROM��
        CAT_SpiWriteBytes(EEPADDBK_IECYKADDR, IEC_YK_NUM + 1, g_ucYKPa);    //���浽EEPROM�ı�������
    }
   *///�绰����� //�ŏ| 0328
    if((g_ucParaChang & BIT5) == BIT5)
    {
        g_ucParaChang &= NBIT5;
        g_gSmsPhone[PHONE_CS] = AddChar(g_gSmsPhone, PHONE_CS);      //����CS
        CAT_SpiWriteBytes(EEPADD_PHONE , PHONE_PA_NUM, g_gSmsPhone);     //���浽EEPROM��
        CAT_SpiWriteBytes(EEPADDBK_PHONE, PHONE_PA_NUM, g_gSmsPhone);    //���浽EEPROM�ı�������
        CheckTELNUMPara();//�ŏ| 0328 ��g_gSmsPhoneת��TEL_NUM g_ucphone_perm 
    }
 
   	//�ŏ| ������վ��
    if((g_ucParaChang & BIT6) == BIT6)
    {
        g_ucParaChang &= NBIT6;
        g_gCnName[CnName_NUM] = AddInt(g_gCnName, CnName_NUM);      //����CS
        CAT_SpiWriteWords(EEPADD_CNNAME   , CnName_NUM+1, g_gCnName);     //���浽EEPROM��
        CAT_SpiWriteWords(EEPADDBK_CNNAME  , CnName_NUM+1, g_gCnName);    //���浽EEPROM�ı�������
        g_gLBName[LBName_NUM] = AddChar(g_gLBName, LBName_NUM);      //����CS
        CAT_SpiWriteBytes(EEPADD_LBNAME   , LBName_NUM+1, g_gLBName);     //���浽EEPROM��
        CAT_SpiWriteBytes(EEPADD_LBNAMELEN, 1, &g_gLBNameLen);    //���浽EEPROM�ı�������
    }	
    if((g_ucParaChang & BIT7) == BIT7)
    {
        g_ucParaChang &= NBIT7;
    }	

}
//==============================================================================
//  ��������   : CheckCfgPara
//  ��������   : ��EEPROM�ж�ȡ����������У���ж����ݵ���Ч�ԣ����ݴ�����101��������ң�ţ�����101������ΪĬ��ֵ
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void CheckCfgPara(void)
{
    //unsigned char i;
    //unsigned int unTemp[2];
	
	
    //��EEPROM�ж�ȡ���в������ж���Ч�ԣ���Ч��ֵ��g_gRunPara����Ч���ʼ��g_gRunParaΪĬ��ֵ
    CAT_SpiReadWords(EEPADD_RP, RUN_PARA_NUM, g_gRunPara);  //������ǰ������ֵ
    if (g_gRunPara[RP_CRC] != CrcCount((unsigned int *)g_gRunPara, RP_CRC)) //CRCУ�����
    {
    	CAT_SpiReadWords(EEPADDBK_RP, RUN_PARA_NUM, g_gRunPara);  //������ǰ������ֵ
    	if (g_gRunPara[RP_CRC] != CrcCount((unsigned int *)g_gRunPara, RP_CRC)) //CRCУ�����
      {
      		RstRunPara();
      		CalcProtCnt();
      		g_ucParaChang |= BIT0;   //���ñ��溯��
    	}
	else
	{
		 CAT_SpiWriteWords(EEPADD_RP, RUN_PARA_NUM, g_gRunPara); //���浽EEPROM��
	}
    }
    if(g_gRunPara[RP_PLUSE_TIME]<=g_gRunPara[RP_PLUSE_MODFK])
    {
        g_gRunPara[RP_PLUSE_MODFK]= 0;
    }	
    if(g_gRunPara[RP_PLUSE_TIME]<=g_gRunPara[RP_PLUSE_AMODFK])
    {
        g_gRunPara[RP_PLUSE_AMODFK]= 0;
    }	
    if(g_gRunPara[RP_PLUSE_TIME]<=g_gRunPara[RP_PLUSE_BMODFK])
    {
        g_gRunPara[RP_PLUSE_BMODFK]= 0;
    }	
    if(g_gRunPara[RP_PLUSE_TIME]<=g_gRunPara[RP_PLUSE_CMODFK])
    {
        g_gRunPara[RP_PLUSE_CMODFK]= 0;
    }		
    if(g_gRunPara[RP_RHPLUSE_TIME2]<=g_gRunPara[RP_PLUSEXH_MODFK])
    {
        g_gRunPara[RP_PLUSEXH_MODFK]= 0;
    }
    if((g_gRunPara[RP_PLUSE_NUM]<6)||(g_gRunPara[RP_PLUSE_NUM]>8))
    {
        g_gRunPara[RP_PLUSE_NUM]= 8;
    }		
    if(g_gRunPara[RP_CNL_MODEL] == 0)
        {
           
           g_gRunPara[RP_JAG_P] = 0;       //���оݱ�־
           g_gRunPara[RP_JAG_Z] = 0;       //�����оݱ�־
           delayms(500);
           g_gRmtInfo[YX_FUN_ON] = 0;
        }
        else
        {
           
          // g_gRunPara[RP_JAG_P] = 0x55;       //���оݱ�־
          // g_gRunPara[RP_JAG_Z] = 0x55;       //�����оݱ�־
           delayms(500);
           g_gRmtInfo[YX_FUN_ON] = 1;
        }
    if((g_gRunPara[RP_SENDSMS_T]<2)||(g_gRunPara[RP_SENDSMS_T]>48))
    {
        g_gRunPara[RP_SENDSMS_T]= 24;
    }		
	
    //101��ϵͳ����
    CAT_SpiReadBytes(EEPADD_IECPARA, IEC_PARA_NUM, g_ucPara101);  //����
  
    if (g_ucPara101[IECP_CRC] != AddChar(g_ucPara101, IECP_CRC))       //CRCУ��
    {
        CAT_SpiReadBytes(EEPADDBK_IECPARA, IEC_PARA_NUM, g_ucPara101);    //����
       
        if (g_ucPara101[IECP_CRC] == AddChar(g_ucPara101, IECP_CRC))       //CRCУ��
        {
            CAT_SpiWriteBytes(EEPADD_IECPARA, IEC_PARA_NUM, g_ucPara101); //���������ǹ�ȥ
        }
        else
        {
            RstIEC101Para();  //IEC para initial
            g_ucParaChang |= BIT1;   //���ñ��溯��
        }
    }
    //101��ң�ŵ��
      CAT_SpiReadBytes(EEPADD_IECYXADDR, IEC_YX_NUM + 1, g_ucYXAddr);  //����
    if (g_ucYXAddr[IEC_YX_NUM] != AddChar(g_ucYXAddr, IEC_YX_NUM))       //CRCУ��
    {
        CAT_SpiReadBytes(EEPADDBK_IECYXADDR, IEC_YX_NUM + 1, g_ucYXAddr);    //����
        if (g_ucYXAddr[IEC_YX_NUM] == AddChar(g_ucYXAddr, IEC_YX_NUM))       //CRCУ��
        {
            CAT_SpiWriteBytes(EEPADD_IECYXADDR, IEC_YX_NUM + 1, g_ucYXAddr); //���������ǹ�ȥ
        }
        else
        {
            RstIEC101YxAddr();  //ң�ŵ���ʼ��
            g_ucParaChang |= BIT2;   //���ñ��溯��
        }
    }
  /*  if (g_ucYXAddr[IEC_YX_NUM] != AddChar((unsigned char *)g_ucYXAddr, IEC_YX_NUM*2))       //CRCУ��
    {
       RstIEC101YxAddr();  //ң�ŵ���ʼ��
       g_ucParaChang |= BIT2;   //���ñ��溯��
    }*/
    
    //101��ң����
    CAT_SpiReadBytes(EEPADD_IECYCADDR, IEC_YC_NUM + 1, g_ucYCAddr);  //����
    if (g_ucYCAddr[IEC_YC_NUM] != AddChar(g_ucYCAddr, IEC_YC_NUM))       //CRCУ��
    {
        CAT_SpiReadBytes(EEPADDBK_IECYCADDR, IEC_YC_NUM + 1, g_ucYCAddr);    //����
        if (g_ucYCAddr[IEC_YC_NUM] == AddChar(g_ucYCAddr, IEC_YC_NUM))       //CRCУ��
        {
            CAT_SpiWriteBytes(EEPADD_IECYCADDR, IEC_YC_NUM + 1, g_ucYCAddr); //���������ǹ�ȥ
        }
        else
        {
            RstIEC101YcAddr();  //ң�����ʼ��
            g_ucParaChang |= BIT3;   //���ñ��溯��
        }
    }
    //�ŏ| ������վ��
    CAT_SpiReadWords(EEPADD_CNNAME, CnName_NUM+1, g_gCnName);  //����
    if (g_gCnName[CnName_NUM] != AddInt(g_gCnName, CnName_NUM))       //CRCУ��
    {
        CAT_SpiReadWords(EEPADDBK_CNNAME, CnName_NUM+1, g_gCnName);    //����
        if (g_gCnName[CnName_NUM] == AddInt(g_gCnName, CnName_NUM))       //CRCУ��
        {
            CAT_SpiWriteWords(EEPADD_CNNAME, CnName_NUM+1, g_gCnName); //���������ǹ�ȥ
        }
        else
        {
            RstCnName();  //ң�����ʼ��
            g_ucParaChang |= BIT6;   //���ñ��溯��
        }
    }
    CAT_SpiReadBytes(EEPADD_LBNAME   , LBName_NUM+1, g_gLBName);     //���浽EEPROM��
    CAT_SpiReadBytes(EEPADD_LBNAMELEN, 1, &g_gLBNameLen);    //���浽EEPROM�ı�������    //101��ң�ص��
    if ((g_gLBName[CnName_NUM] != AddChar(g_gLBName, LBName_NUM))||(g_gLBNameLen>LBName_NUM))
    	{
    	 RstLBName(); 
    	}
    if(g_gRunPara[RP_FLOAD_T] !=0)g_gSaveload=g_gRunPara[RP_FLOAD_T]-1;//ÿ��һ��ʱ��洢���ɼ�¼
	
    	
   /* CAT_SpiReadBytes(EEPADD_IECYKADDR, IEC_YK_NUM + 1, g_ucYKPa);  //����
    if (g_ucYKPa[IEC_YK_NUM] != AddChar(g_ucYKPa, IEC_YK_NUM))       //CRCУ��
    {
        CAT_SpiReadBytes(EEPADDBK_IECYKADDR, IEC_YK_NUM + 1, g_ucYKPa);    //����
        if (g_ucYKPa[IEC_YK_NUM] == AddChar(g_ucYKPa, IEC_YK_NUM))       //CRCУ��
        {
            CAT_SpiWriteBytes(EEPADD_IECYKADDR, IEC_YK_NUM + 1, g_ucYKPa); //���������ǹ�ȥ
        }
        else
        {
            RstIEC101YkAddr();  //ң�ص���ʼ��
            g_ucParaChang |= BIT4;   //���ñ��溯��
        }
    }
    
    //�绰�����
    CAT_SpiReadBytes(EEPADD_PHONE, PHONE_PA_NUM, g_gSmsPhone);  //����
    
    if (g_gSmsPhone[PHONE_CS] != AddChar(g_gSmsPhone, PHONE_CS))       //CRCУ��
    {
        CAT_SpiReadBytes(EEPADDBK_PHONE, PHONE_PA_NUM, g_gSmsPhone);    //����
        if (g_gSmsPhone[PHONE_CS] == AddChar(g_gSmsPhone, PHONE_CS))       //CRCУ��
        {
            CAT_SpiWriteBytes(EEPADD_PHONE, PHONE_PA_NUM, g_gSmsPhone); //���������ǹ�ȥ
        }
        else
        {
            RstSmsPhoneInfo();  //�绰������ʼ��
            g_ucParaChang |= BIT5;   //���ñ��溯��
        }
    }
    */
}
void CheckCfgERR(void)
{    
    if (g_gRunPara[RP_CRC] != CrcCount((unsigned int *)g_gRunPara, RP_CRC)) //CRCУ�����
    {
    	CAT_SpiReadWords(EEPADDBK_RP, RUN_PARA_NUM, g_gRunPara);  //������ǰ������ֵ
    	if (g_gRunPara[RP_CRC] != CrcCount((unsigned int *)g_gRunPara, RP_CRC)) //CRCУ�����
      		{
      		RstRunPara();
      		//CalcProtCnt();      		
    		}
		g_ucParaChang |= BIT0;   //���ñ��溯��
    }
    if(g_gRunPara[RP_PLUSE_TIME]<=g_gRunPara[RP_PLUSE_MODFK])
    {
        g_gRunPara[RP_PLUSE_MODFK]= 0;
    }	
    if(g_gRunPara[RP_PLUSE_TIME]<=g_gRunPara[RP_PLUSE_AMODFK])
    {
        g_gRunPara[RP_PLUSE_AMODFK]= 0;
    }	
    if(g_gRunPara[RP_PLUSE_TIME]<=g_gRunPara[RP_PLUSE_BMODFK])
    {
        g_gRunPara[RP_PLUSE_BMODFK]= 0;
    }	
    if(g_gRunPara[RP_PLUSE_TIME]<=g_gRunPara[RP_PLUSE_CMODFK])
    {
        g_gRunPara[RP_PLUSE_CMODFK]= 0;
    }		
    if(g_gRunPara[RP_RHPLUSE_TIME2]<=g_gRunPara[RP_PLUSEXH_MODFK])
    {
        g_gRunPara[RP_PLUSEXH_MODFK]= 0;
    }
    if((g_gRunPara[RP_PLUSE_NUM]<6)||(g_gRunPara[RP_PLUSE_NUM]>8))
    {
        g_gRunPara[RP_PLUSE_NUM]= 8;
    }		
    if(g_gRunPara[RP_CNL_MODEL] == 0)
        {
           
           g_gRunPara[RP_JAG_P] = 0;       //���оݱ�־
           g_gRunPara[RP_JAG_Z] = 0;       //�����оݱ�־
           delayms(500);
           g_gRmtInfo[YX_FUN_ON] = 0;
        }
        else
        {
           
          // g_gRunPara[RP_JAG_P] = 0x55;       //���оݱ�־
          // g_gRunPara[RP_JAG_Z] = 0x55;       //�����оݱ�־
           delayms(500);
           g_gRmtInfo[YX_FUN_ON] = 1;
        }
    if((g_gRunPara[RP_SENDSMS_T]<2)||(g_gRunPara[RP_SENDSMS_T]>48))
    {
        g_gRunPara[RP_SENDSMS_T]= 24;
    }
	if(g_gRunPara[RP_KM_TIME]==0)
		g_gRunPara[RP_KM_TIME]=100;
	
    //101��ϵͳ����  
    if (g_ucPara101[IECP_CRC] != AddChar(g_ucPara101, IECP_CRC))       //CRCУ��
    {
        CAT_SpiReadBytes(EEPADDBK_IECPARA, IEC_PARA_NUM, g_ucPara101);    //����       
        if (g_ucPara101[IECP_CRC] != AddChar(g_ucPara101, IECP_CRC))       //CRCУ��
        	{        
            RstIEC101Para();  //IEC para initial
        	}
       	g_ucParaChang |= BIT1;   //���ñ��溯��        
    }
	
    //101��ң�ŵ��     
    if (g_ucYXAddr[IEC_YX_NUM] != AddChar(g_ucYXAddr, IEC_YX_NUM))       //CRCУ��
    {
        CAT_SpiReadBytes(EEPADDBK_IECYXADDR, IEC_YX_NUM + 1, g_ucYXAddr);    //����
        if (g_ucYXAddr[IEC_YX_NUM] != AddChar(g_ucYXAddr, IEC_YX_NUM))       //CRCУ��
        {
            RstIEC101YxAddr();  //ң�ŵ���ʼ��
        }
        g_ucParaChang |= BIT2;   //���ñ��溯��        
    }
     
    //101��ң����
    if (g_ucYCAddr[IEC_YC_NUM] != AddChar(g_ucYCAddr, IEC_YC_NUM))       //CRCУ��
    {
        CAT_SpiReadBytes(EEPADDBK_IECYCADDR, IEC_YC_NUM + 1, g_ucYCAddr);    //����
        if (g_ucYCAddr[IEC_YC_NUM] != AddChar(g_ucYCAddr, IEC_YC_NUM))       //CRCУ��
        {
             RstIEC101YcAddr();  //ң�����ʼ��
        }
        g_ucParaChang |= BIT3;   //���ñ��溯��        
    }
    
}

//==============================================================================
//  ��������   : CheckProtCnt
//  ��������   : ���ݵ�ǰ������ֵ��Ŵ�EEPROM�ж�ȡ������ֵ������У���ж����ݵ���Ч�ԣ����ݴ����򱨱�����ֵ����ң��
//       ����g_gRunPara�е����ݣ�������ڲ��ж���Ҫ�õ������ݱ��浽g_gRunParaJdg��
//  �������   : <��>
//  �������   : g_gProcCnt - ������ֵ
//     : g_gProcCntJdg - ����ϵ�������ı�����ֵ�������ڲ������߼��ж�ʹ��
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ������һ
//==============================================================================
void CheckProtCnt(void)
{
  /*  unsigned int i;
    CAT_SpiReadWords(EEPADD_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);       //������ǰ������ֵ
    if (g_gProcCnt[PC_CRC] == CrcCount((unsigned int *)g_gProcCnt, PC_CRC)) //CRCУ��*/
    {
        //����ʵ�ʳ������ж���Ҫ���Ѷ�ֵ����ϵ��������ڲ�����Ķ�ֵ����
        CalcProtCnt();
     strcpy(ComtrderCfg_adj,"1.00001.00001.00001.00001.0000");	
     FP32  a;     
     a=8.138 * g_gProcCnt[PC_UA_ADJ] /4096;// 12); 
     sprintf(&ComtrderCfg_adj[0],"%-06.4f",a);
     a=8.138 * g_gProcCnt[PC_UB_ADJ] /4096;// 12); 
     sprintf(&ComtrderCfg_adj[6],"%-06.4f",a);
     a=8.138 * g_gProcCnt[PC_UC_ADJ] /4096;// 12); 
     sprintf(&ComtrderCfg_adj[12],"%-06.4f",a);
     a=8.138 * g_gProcCnt[PC_U0_ADJ] /4096;// 12); 
     sprintf(&ComtrderCfg_adj[18],"%-06.4f",a);
     a=0.379 *g_gRunPara[RP_CT_TRANS]* g_gProcCnt[PC_I0_ADJ] /4096/100;// 12); 
     sprintf(&ComtrderCfg_adj[24],"%-06.4f",a);	 
                                 
    }
  /*  else
    {
        CAT_SpiReadWords(EEPADDBK_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);     //������ǰ������ֵ
        if (g_gProcCnt[PC_CRC] == CrcCount((unsigned int *)g_gProcCnt, PC_CRC)) //CRCУ��
        {
            //����ʵ�ʳ������ж���Ҫ���Ѷ�ֵ����ϵ��������ڲ�����Ķ�ֵ����
            CalcProtCnt();

            CAT_SpiWriteWords(EEPADD_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);      //�ñ��������ǵ�ǰ������ֵ
        }       
        else
        {
           RstProcPara();//��λ��������
           CalcProtCnt();
           CAT_SpiWriteWords(EEPADD_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);
           CAT_SpiWriteWords(EEPADDBK_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);

        }
    }
    for(i = 0; i < 24 ; i++)
        g_gETHProcCnt[i] = g_gProcCnt[20 + i];*/
}

//==============================================================================
//  ��������   : CheckAdjPara
//  ��������   : ��EEPROM�ж�ȡУ������������У���ж����ݵ���Ч�ԣ����ݴ�����У����������ң�ţ�����У��������ΪĬ��ֵ
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ������һ
//==============================================================================
void CheckAdjPara(void)
{
    unsigned int i;
    CAT_SpiReadWords(EEPADD_BIAS_V, ADJ_PARA_NUM, (unsigned int*)g_gAdjPara);  //����
    if (g_gAdjPara[ADJ_CRC] == CrcCount((unsigned int *)g_gAdjPara, ADJ_CRC))       //CRCУ��
    {
       // SetRIStatus(TST_ADJ, 0);
    }
    else
    {
        CAT_SpiReadWords(EEPADDBK_BIAS_V, ADJ_PARA_NUM, (unsigned int*)g_gAdjPara);    //����
        if (g_gAdjPara[ADJ_CRC] == CrcCount((unsigned int *)g_gAdjPara, ADJ_CRC))       //CRCУ��
        {
            //SetRIStatus(TST_ADJ, 0);
            CAT_SpiWriteWords(EEPADD_BIAS_V, ADJ_PARA_NUM, (unsigned int*)g_gAdjPara); //���������ǹ�ȥ
        }
        else
        {             //Ϊ�˱�֤���������������У���ʹУ��ϵ������Ҳ��Ҫ��У��ϵ����Ĭ��ֵ1����
            
            for(i = 0; i < ADJ_CRC; i++)     //���adj��Ч�����adj����ֵ
            {
                g_gAdjPara[i] = 2048;
            }
        }
    }
    for(i = 0; i < ADJ_CRC; i++)     //����adֵ��ƫ��
   {
      g_gAdjAD[i] = (unsigned int)((long)(g_gAdjPara[i]) * g_gProcCnt[PC_UA_ADJ + i] >> 12);
    }
	//�ŏ| Ŀ��У׼����λ�����ز���10�ֽ� ��ʼֵΪ��ѹ60V,����2A g_gAdjObj[ADJ_PARA_NUM]	 
	      //g_gAdjObj[ADJ_CRC] = CrcCount((unsigned int *)g_gAdjObj, ADJ_CRC);      //����CRC
       CAT_SpiReadWords(EEPADD_ADJOBJ,ADJ_PARA_NUM,(unsigned int *)g_gAdjObj);     //���浽EEPROM��
       if (g_gAdjObj[ADJ_CRC] == CrcCount((unsigned int *)g_gAdjObj, ADJ_CRC))       //CRCУ��
      {       
      }
      else
      {	  
      		CAT_SpiReadWords(EEPADDBK_ADJOBJ , ADJ_PARA_NUM, (unsigned int *)g_gAdjObj);   //���浽EEPROM�ı�������
      		if (g_gAdjObj[ADJ_CRC] == CrcCount((unsigned int *)g_gAdjObj, ADJ_CRC))       //CRCУ��
      		{
      			CAT_SpiWriteWords(EEPADD_ADJOBJ,ADJ_PARA_NUM,(unsigned int *)g_gAdjObj);     //���浽EEPROM��
      		}
		else
		{
			g_gAdjObj[0]=0;g_gAdjObj[1]=0;g_gAdjObj[2]=0;g_gAdjObj[3]=0;g_gAdjObj[4]=0;	
			g_gAdjObj[ADJ_CRC] = CrcCount((unsigned int *)g_gAdjObj, ADJ_CRC);      //����CRC
             		CAT_SpiWriteWords(EEPADD_ADJOBJ,ADJ_PARA_NUM,(unsigned int *)g_gAdjObj);     //���浽EEPROM��
             		CAT_SpiWriteWords(EEPADDBK_ADJOBJ,ADJ_PARA_NUM,(unsigned int *)g_gAdjObj);//���浽EEPROM�ı�������
		}
      }      
	  //�ŏ| Ŀ��У׼����λ�����ز���10�ֽ� ��ʼֵΪ��ѹ60V,����2A
    
}

//==============================================================================
//  ��������   : CheckRunInfo
//  ��������   : ��EEPROM�ж�ȡ������Ϣ������У���ж����ݵ���Ч�ԣ���������Ϣ��0
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ������һ
//==============================================================================
void CheckRunInfo(void)
{
    unsigned int i;
    CAT_SpiReadWords(EEPADD_COM, RUN_INFO_NUM, g_gRunInfo); //����
    if (g_gRunInfo[INF_CRC] != CrcCount((unsigned int *)g_gRunInfo, INF_CRC))       //CRCУ��
    {
        CAT_SpiReadWords(EEPADDBK_COM, RUN_INFO_NUM, g_gRunInfo);       //����
        if (g_gRunInfo[INF_CRC] == CrcCount((unsigned int *)g_gRunInfo, INF_CRC))       //CRCУ��
        {
            CAT_SpiWriteWords(EEPADD_COM, RUN_INFO_NUM, g_gRunInfo);    
        }
        else
        {
            //������Ϣ���󣬰�ȫ������Ϊ0
            for(i=0;i<20;i++)
            {   	
                g_gRunInfo[i]=0x30;            
   	    }
             g_gRunInfo[20] =  1;
            g_gRunInfo[INF_CRC] = CrcCount((unsigned int *)g_gRunInfo, INF_CRC);
            CAT_SpiWriteWords(EEPADD_COM, RUN_INFO_NUM, g_gRunInfo);    
            CAT_SpiWriteWords(EEPADDBK_COM, RUN_INFO_NUM, g_gRunInfo);      
        }
    }
    
    
}
//==============================================================================
//  ��������   : CheckTELNUMPara
//  ��������   : ��EEPROM�ж�ȡ��ȡ�绰���룬����У���ж����ݵ���Ч��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void CheckTELNUMPara(void)
{
    //unsigned char unTemp[7];//�ŏ| 0328
    //unsigned char i;//,j;//�ŏ| 0328

    CAT_SpiReadBytes(EEPADD_PHONE, PHONE_PA_NUM, g_gSmsPhone);  //����//�ŏ| 0328 

     if (g_gSmsPhone[PHONE_CS] != AddChar(g_gSmsPhone, PHONE_CS))       //CRCУ��
    {
        CAT_SpiReadBytes(EEPADDBK_PHONE, PHONE_PA_NUM, g_gSmsPhone);    //����
        if (g_gSmsPhone[PHONE_CS] == AddChar(g_gSmsPhone, PHONE_CS))       //CRCУ��
        {
            CAT_SpiWriteBytes(EEPADD_PHONE, PHONE_PA_NUM, g_gSmsPhone); //���������ǹ�ȥ
        }
        else
        {
            RstSmsPhoneInfo();  //�绰������ʼ��//�ŏ| 0328 RstSmsPhoneInfo������
            g_ucParaChang |= BIT5;   //���ñ��溯��//�ŏ| 0328 RstSmsPhoneInfo������
        }
    }
/*
    for(i=0;i<4;i++)	
    {
        g_ucphone_perm[i]=g_gSmsPhone[64+i];
    } 	
*/
     
//����4���ֻ��� 4*11
/*
    for(i=0;i<11;i++)	
    {
        TEL_NUM[i]=g_gSmsPhone[1+i];
    } 
    for(i=0;i<11;i++)	
    {
        TEL_NUM[i+11]=g_gSmsPhone[13+i];
    } 
    for(i=0;i<11;i++)	
    {
        TEL_NUM[i+22]=g_gSmsPhone[25+i];
    } 
    for(i=0;i<11;i++)	
    {
        TEL_NUM[i+33]=g_gSmsPhone[37+i];
    } 
    */
/*
    CAT_SpiReadBytes(EEPADD_PHONE_NUM + 44, 7, unTemp);   //g_ucphone_perm
    
    j=0;
    for(i=0;i<4;i++)
    {
        j+=unTemp[i];
    }
    if((j&0xff)!= unTemp[4])
    {
        unTemp[0]=0x7f;     //////////��һ���ֻ��ţ�������վ��
        for(i=1;i<4;i++)      //////////���µ��ֻ��ţ����û���
        {
             unTemp[i]=0x77;
        } 
    }
    for(i=1;i<4;i++)      //////////���µ��ֻ��ţ����û���
    {
        if((unTemp[i]&BIT3)==BIT3)
 	{
            unTemp[i]&=~BIT3;
        }
    } 	 
    for(i=0;i<4;i++)	
    {
        g_ucphone_perm[i]=unTemp[i];
    } 
   
    if((unTemp[5]==unTemp[6])&&(unTemp[5]>=2)&&(unTemp[5]<=48))
    {
        g_ucPara_stime = unTemp[5];
    }
    else
    {
 	  unTemp[5]=24;
 	  unTemp[6]=24;
 	  g_ucPara_stime = 24;
    }
    state_counter= (unsigned int)g_ucPara_stime*60;   //
    CAT_SpiWriteBytes(EEPADD_PHONE_NUM + 44, 7, unTemp);
    CAT_SpiReadBytes(EEPADD_PHONE_NUM, 44, TEL_NUM); //����4���ֻ��� 4*11
*/	
 
}


unsigned char unTemp12[4] = {0xff,0xff,0xff,0xff};
//==============================================================================
//  ��������   : CheckRECPara
//  ��������   : ��EEPROM�ж�ȡ�������������Ϣ������У���ж����ݵ���Ч��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void CheckRECPara(void)
{
    unsigned char unTemp[4];
    
    CAT_SpiReadBytes(EEPADD_SOE_NUM , 2, unTemp);
    if(unTemp[0]>30)
 	unTemp[0] = 0;
    if(unTemp[1]>30)
 	unTemp[1] = 0;	
    CAT_SpiWriteBytes(EEPADD_SOE_NUM , 2, unTemp);
    g_sSoeData.m_ucSoeNum = unTemp[0];
    g_sSoeData.m_ucSoePos = unTemp[1];

	unsigned int untemp[4];		
	CAT_SpiReadWords(EEPADD_SOE_E2ROMADR, 4, untemp);  //���浽EEPROM��
	g_unSSoeSaveE2ROMPtr=untemp[0];
	soe_num=untemp[2];
	if((untemp[0]!=untemp[1])||(untemp[0]<EEPADD_SOESTARTADR)||(untemp[0]>=EEPADD_SOEENDADR))
		DelALLSOE();
	if((untemp[2]!=untemp[3])||(untemp[2]>512))
		DelALLSOE();		
	g_unSSoeSaveE2ROMPtr &= 0xfff0;	

 
}



//==============================================================================
//  ��������   : CalcProtCnt
//  ��������   : �ѱ�����ֵ������߼��жϹ������õ������ݣ����浽g_gProcCntJdg�У���Ҫ�����(ԭʼֵ*ϵ��)��ƽ��*16
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ������һ
//==============================================================================
void CalcProtCnt(void)
{
    unsigned char i;
    for(i = 0; i <= PROC_CNT_NUM; i++)
        g_gProcCnt[i] = g_gRunPara[i + RP_UA_ADJ];
//#ifdef YN_101S
    g_gProcCntJug[PC_HIGH_P] = g_gProcCnt[PC_HIGH_P] ;                //���ѹ�߶�ֵ
    g_gProcCntJug[PC_LOW_P] = g_gProcCnt[PC_LOW_P];                //���ѹ�Ͷ�ֵ
    g_gProcCntJug[PC_HIGH_Z] = g_gProcCnt[PC_HIGH_Z];          //�����ѹ�߶�ֵ
    g_gProcCntJug[PC_LOW_Z] = g_gProcCnt[PC_LOW_Z];          //�����ѹ�Ͷ�ֵ
    g_gProcCntJug[PC_NO_V] = g_gProcCnt[PC_NO_V];       //��ѹ�ż�ֵ
    g_gProcCntJug[PC_I0_START] = g_gProcCnt[PC_I0_START];//�ߵ�ѹ�߶�ֵ
    g_gProcCntJug[PC_PULSE_VALID] = g_gProcCnt[PC_PULSE_VALID] /10 / 15;       //8������Ч��ֵ
	if(g_gProcCnt[PC_T_DELAY] > 2000)
        g_gProcCntJug[PC_T_DELAY] = g_gProcCnt[PC_T_DELAY] /10 - 200;
    else
        g_gProcCntJug[PC_T_DELAY] = g_gProcCnt[PC_T_DELAY] /10;               //������ʱʱ�� �����10����
/*
#else
    g_gProcCntJug[PC_HIGH_P] = g_gProcCnt[PC_HIGH_P] * 100;                //���ѹ�߶�ֵ
    g_gProcCntJug[PC_LOW_P] = g_gProcCnt[PC_LOW_P] * 100;  
    g_gProcCntJug[PC_HIGH_Z] = g_gProcCnt[PC_HIGH_Z] * 100;          //�����ѹ�߶�ֵ
    g_gProcCntJug[PC_LOW_Z] = g_gProcCnt[PC_LOW_Z] * 100;          //�����ѹ�Ͷ�ֵ
    g_gProcCntJug[PC_NO_V] = g_gProcCnt[PC_NO_V] * 100;       //��ѹ�ż�ֵ
    g_gProcCntJug[PC_I0_START] = g_gProcCnt[PC_I0_START] * 100;
    g_gProcCntJug[PC_PULSE_VALID] = g_gProcCnt[PC_PULSE_VALID] * 100 / 15;       //8������Ч��ֵ
    if(g_gProcCnt[PC_T_DELAY] > 2)
        g_gProcCntJug[PC_T_DELAY] = g_gProcCnt[PC_T_DELAY] * 100 - 200;
    else
        g_gProcCntJug[PC_T_DELAY] = g_gProcCnt[PC_T_DELAY] * 100;               //������ʱʱ�� �����10����
#endif 
*/
    g_gProcCntJug[PC_LACTH_TIME] = g_gProcCnt[PC_LACTH_TIME] * 60;       //����ʱ�任�����
   // g_gProcCntJug[PC_JAG_Z] = 0x55;       //�����оݱ�־
   // g_gProcCntJug[PC_EFS_MODEL] = 0;       //�ź�Դ�ͺű�ʶ
   // g_gProcCntJug[PC_JAG_ACT] = 0;       //���������о�

   // g_gProcCntJug[PC_GPRS_MODEL] = 0;      //GPRS��ʽ��ֵ
   // g_gProcCntJug[PC_REV_CURRENT] = 20;       //��ת������ֵ  
    g_gProcCntJug_I0[0]  =  (unsigned long)g_gProcCnt[PC_OVERLOAD_I] * g_gProcCnt[PC_OVERLOAD_I] * COEF_I_0_AD2 >> 7;               //  22448=(100/15 * 4096/COEF_AD_I_0)*(100/15 * 4096/COEF_AD_I_0)*128
    g_gProcCntJug_I0[1]  =  (unsigned long)g_gProcCntJug_I0[0] - (g_gProcCntJug_I0[0] >> 4);
    g_gProcCntJug_I0[2]  =  (unsigned long)g_gProcCnt[PC_OVERLOAD_T] * 8;

}
//==============================================================================
//  ��������   : RstRunPara
//  ��������   : ��λ�ڴ��е����в���ΪĬ��ֵ
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void RstRunPara(void)
{
    g_gRunPara[RP_CFG_KEY] = 0x008C;//1000 1100   
    g_gRunPara[RP_BEAT_T] = 0;//Ĭ�ϲ���������
    g_gRunPara[RP_SENDYC_T] = 600; 
    g_gRunPara[RP_SENDYX_T] = 600;      //״̬�㱨����ʱ��
    //g_gRunPara[RP_STATUSRP_INTERVAL] = 1;  //״̬�㱨���߼�����죩
    g_gRunPara[RP_FLOAD_T] = 15;  ////���ɼ�¼�洢ʱ���� ����
    g_gRunPara[RP_COMM_ADDR] = 1;//ͨ��վ��ַ      վ��ַĬ��Ϊ1
    g_gRunPara[RP_SYX_INFADDR] = 128;
#ifdef  CQ_101S
     g_gRunPara[RP_SYX_INFADDR]=1;//�ŏ|���� 0x0700+171
     g_gRunPara[RP_SENDYC_T] = 0; 
    g_gRunPara[RP_SENDYX_T] = 0;  
#endif
    g_gRunPara[RP_DYX_INFADDR] = DYX_ADDR;
    g_gRunPara[RP_YCLIMIT] = 1000;//ң����ֵ ����
    g_gRunPara[RP_YCCAP] = 50;//ң����ֵ ���
    
    g_gRunPara[RP_PLUSE_MODFK]=3;
    g_gRunPara[RP_PLUSE_AMODFK]=3;
    g_gRunPara[RP_PLUSE_BMODFK]=3;
    g_gRunPara[RP_PLUSE_CMODFK]=3;	
    g_gRunPara[RP_PLUSEXH_MODFK] = 5; 
    
    g_gRunPara[RP_YXRSD_T] = 600;//10���� 
    
    g_gRunPara[YX_delay] = 20;
    
    g_gRunPara[RP_UA_ADJ] = 4096;                //UAУ׼ϵ��ֵ
    g_gRunPara[RP_UB_ADJ] = 4096;        //UBУ׼ϵ��ֵ
    g_gRunPara[RP_UC_ADJ] = 4096;                //UCУ׼ϵ��ֵ
    g_gRunPara[RP_U0_ADJ] = 4096;                //U0У׼ϵ��ֵ
    g_gRunPara[RP_I0_ADJ] = 4096;                //I0У׼ϵ��ֵ
//#ifdef YN_101S
    g_gRunPara[RP_I0_START] = 11000;                //��������ż�ֵ
    g_gRunPara[RP_HIGH_P] = 7000;                //���ѹ�߶�ֵ
    g_gRunPara[RP_LOW_P] = 5000;                //���ѹ�Ͷ�ֵ
    g_gRunPara[RP_HIGH_Z] = 2500;          //�����ѹ�߶�ֵ
    g_gRunPara[RP_LOW_Z] = 1000;          //�����ѹ�Ͷ�ֵ
    g_gRunPara[RP_NO_V] = 1000;       //��ѹ�ż�ֵ
    g_gRunPara[RP_T_DELAY] = 10000;               //������ʱʱ��
    g_gRunPara[RP_PULSE_VALID] = 3000;       //8������Ч��ֵ		
/*
#else
    g_gRunPara[RP_I0_START] = 110;                //��������ż�ֵ
    g_gRunPara[RP_HIGH_P] = 70;                //���ѹ�߶�ֵ
    g_gRunPara[RP_LOW_P] = 50;                //���ѹ�Ͷ�ֵ
    g_gRunPara[RP_HIGH_Z] = 25;          //�����ѹ�߶�ֵ
    g_gRunPara[RP_LOW_Z] = 10;          //�����ѹ�Ͷ�ֵ
    g_gRunPara[RP_NO_V] = 10;       //��ѹ�ż�ֵ
     g_gRunPara[RP_PULSE_VALID] = 3;       //8������Ч��ֵ
    g_gRunPara[RP_T_DELAY] = 10;               //������ʱʱ��
#endif
*/
    g_gRunPara[RP_JAG_P] = 0x55;       //���оݱ�־
    g_gRunPara[RP_JAG_Z] = 0x55;       //�����оݱ�־
    g_gRunPara[RP_EFS_MODEL] = 1;       //�ź�Դ�ͺű�ʶ
    g_gRunPara[RP_JAG_ACT] = 3;       //���������о�

    g_gRunPara[RP_GPRS_MODEL] = 0;      //GPRS��ʽ��ֵ
    g_gRunPara[RP_REV_CURRENT] = 20;       //��ת������ֵ
    g_gRunPara[RP_U0_CAL] = 1;      //U0������ʽ Ĭ�ϲɼ�

    g_gRunPara[RP_CNL_MODEL] = 1; 		//���Ʒ�ʽ
    g_gRunPara[RP_LACTH_TIME] = 17; 		//���Ʒ�ʽ
#ifdef  SD_101S  
    g_gRunPara[RP_LACTH_TIME] = 15; 		//���Ʒ�ʽ
#endif
    g_gRunPara[RP_PLUSE_TIME] = 20; 		//���Ʒ�ʽ
    g_gRunPara[RP_SEND_TIME1] = 100; 		//���Ʒ�ʽ
    g_gRunPara[RP_SEND_TIME2] = 125; 		//���Ʒ�ʽ
#ifdef  CQ_101S    
    g_gRunPara[RP_PLUSE_TIME] = 16; 		//���Ʒ�ʽ
    g_gRunPara[RP_SEND_TIME1] = 96; 		//���Ʒ�ʽ
    g_gRunPara[RP_SEND_TIME2] = 116; 		//���Ʒ�ʽ
#endif	
    g_gRunPara[RP_PLUSE_NUM] = 8; 
    g_gRunPara[RP_OVERLOAD_I] = 10000; 
    g_gRunPara[RP_OVERLOAD_T] = 6; 
    g_gRunPara[RP_YC_INFADDR]=0x4001;//�ŏ| ң����ʼ��ַ�޸����в���	
#ifdef YN_101S 
    g_gRunPara[RP_YC_INFADDR]=0x4081;//�ŏ| ң����ʼ��ַ�޸����в���		
#endif
    g_gRunPara[RP_SENDSMS_T]=24;//�ŏ| ң����ʼ��ַ�޸����в���
    g_gRunPara[RP_SENDZJ_T] = 0; 
#ifdef  YN_101S
    g_gRunPara[RP_SENDZJ_T] = 60; 
#endif	
#ifdef  SD_101S
   g_gRunPara[RP_RHSEND_TIME1] = 100; 
   g_gRunPara[RP_RHPLUSE_TIME2] = 50; 
   g_gRunPara[RP_RHT_DELAY]=300;//5min=300s
   g_gRunPara[RP_CT_TRANS]=20;//CT��� =20 
#else
   g_gRunPara[RP_RHSEND_TIME1] = 0; 
   g_gRunPara[RP_RHPLUSE_TIME2] = 0; 
   g_gRunPara[RP_RHT_DELAY]=0;//60s
   g_gRunPara[RP_CT_TRANS]=15;//CT��� =20    
#endif
	g_gRunPara[RP_KM_TIME]=100;

    state_counter= g_gRunPara[RP_SENDSMS_T]*60;	//�ŏ| ң����ʼ��ַ�޸����в���    
}

void RstIEC101Para(void )
{
      g_ucPara101[IECP_LINKADDR_NUM] = 2;//2;
      g_ucPara101[IECP_TRANSRSN_NUM] = 2;//2;
#ifdef CQ_101S 
     g_ucPara101[IECP_TRANSRSN_NUM] = 1;//�ŏ|����  ����ԭ��1�ֽ�		
#endif
      g_ucPara101[IECP_COMNADDR_NUM] = 2;
      g_ucPara101[IECP_INFO_NUM] = 2;
      g_ucPara101[IECP_COM_MODE] = 1;//0;
      g_ucPara101[IECP_AUT_MODE] = 0;
        CAT_SpiWriteBytes(EEPADD_IECPARA , IEC_PARA_NUM, g_ucPara101);     //���浽EEPROM��
        CAT_SpiWriteBytes(EEPADDBK_IECPARA, IEC_PARA_NUM, g_ucPara101);    //���浽EEPROM�ı�������     


}

void RstIEC101YxAddr(void)  //ң�ŵ��ĳ�ʼ��
{
    unsigned int i;//,j=0
    
      memset(g_ucYXAddr,0,sizeof(g_ucYXAddr));
       
    for(i = 0; i < IEC_YX_NUM; i++)
    {
	        g_ucYXAddr[i] = i+1;
    }
#ifdef YN_101S 
    g_ucYXAddr[0] =1;g_ucYXAddr[1] =2;g_ucYXAddr[2] =3;g_ucYXAddr[3] =4;g_ucYXAddr[4] =5;
	g_ucYXAddr[5] =6;g_ucYXAddr[6] =7;g_ucYXAddr[7] =8;g_ucYXAddr[8] =9;g_ucYXAddr[9] =10;
	g_ucYXAddr[10] =11;g_ucYXAddr[11] =12;g_ucYXAddr[12] =13;g_ucYXAddr[13] =14;g_ucYXAddr[14] =0;
	g_ucYXAddr[15] =0;g_ucYXAddr[16] =0;g_ucYXAddr[17] =0;g_ucYXAddr[18] =0;g_ucYXAddr[19] =0;
#endif
#ifdef CQ_101S 
    g_ucYXAddr[0] =12;g_ucYXAddr[1] =13;g_ucYXAddr[2] =14;g_ucYXAddr[3] =6;g_ucYXAddr[4] =11;
	g_ucYXAddr[5] =0;g_ucYXAddr[6] =0;g_ucYXAddr[7] =0;g_ucYXAddr[9] =0;g_ucYXAddr[10] =0;
	g_ucYXAddr[11] =0;g_ucYXAddr[12] =0;g_ucYXAddr[13] =0;g_ucYXAddr[8] =0;g_ucYXAddr[14] =0;
#endif	
}

void RstIEC101YcAddr(void)  //ң����ĳ�ʼ��
{
    unsigned char i;
    for(i = 0; i < IEC_YC_NUM; i++)
    {
        g_ucYCAddr[i] = i+1;
    }

#ifdef CQ_101S 	
       g_ucYCAddr[0] =2;
	g_ucYCAddr[1] =3;
	g_ucYCAddr[2] =4;
	g_ucYCAddr[3] =1;
	g_ucYCAddr[4] =0;g_ucYCAddr[5] =0;g_ucYCAddr[6] =0;g_ucYCAddr[7] =0;
	g_ucYCAddr[8] =0;g_ucYCAddr[9] =0;
#endif
}

void RstIEC101YcSiqu(void)  //ң�������ĳ�ʼ��
{
//      unsigned char i;
//      for(i = 0; i < IEC_YC_NUM; i++)
//      {
//          g_unYCSiqu[i] = 10;
//      }
}

void RstIEC101YkAddr(void)  //ң�ص��ĳ�ʼ��
{
   
    
}

void RstSmsPhoneInfo(void)  //�绰�����ĳ�ʼ��
{
    char chDefPhone[] = "13811005566";

    g_gSmsPhone[0]=11;g_gSmsPhone[16]=11;g_gSmsPhone[32]=11;g_gSmsPhone[48]=11;//���ֻ����붨λ11λ	
    memcpy(&(g_gSmsPhone[1]),chDefPhone,1);
    memcpy(&(g_gSmsPhone[17]),chDefPhone,1);
    memcpy(&(g_gSmsPhone[33]),chDefPhone,1);
    memcpy(&(g_gSmsPhone[49]),chDefPhone,1);	
    /*
    char chDefPhone[] = "+86 13811005566";
    g_gSmsPhone[PHONE_ENCTL] = 1;//Ĭ�ϵ�һ���绰��������
    g_gSmsPhone[PHONE1_CHARLEN] = PHONE_CHARNUM;
    g_gSmsPhone[PHONE2_CHARLEN] = PHONE_CHARNUM;
    g_gSmsPhone[PHONE3_CHARLEN] = PHONE_CHARNUM;
    memcpy(&(g_gSmsPhone[PHONE1]),chDefPhone,PHONE_CHARNUM);
    memcpy(&(g_gSmsPhone[PHONE2]),chDefPhone,PHONE_CHARNUM);
    memcpy(&(g_gSmsPhone[PHONE3]),chDefPhone,PHONE_CHARNUM);
    */
}

void RstCnName(void)  //վ������ַ��ʼ��
{
    //�����к�������������·10��Ժ�������
    g_gCnName[0]=0x5317;g_gCnName[1]=0x4eac;g_gCnName[2]=0x5e02;g_gCnName[3]=0x6d77;
    g_gCnName[4]=0x6dc0;g_gCnName[5]=0x533a;g_gCnName[6]=0x897f;g_gCnName[7]=0x5317;
    g_gCnName[8]=0x65fa;g_gCnName[9]=0x4e1c;g_gCnName[10]=0x8def;g_gCnName[11]=0x0031;
    g_gCnName[12]=0x0030;g_gCnName[13]=0x53f7;g_gCnName[14]=0x9662;g_gCnName[15]=0x002d;
    g_gCnName[16]=0x79d1;g_gCnName[17]=0x9510;g_gCnName[18]=0x5927;g_gCnName[19]=0x53a6;	
}
void RstLBName(void)  //վ������ַ��ʼ��
{
    //BJ-CREAT-DA,
    char a[13]="BJ-CREAT-DA";
    memcpy(g_gLBName,a,11);
    g_gLBNameLen=11;
}

