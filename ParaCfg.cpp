/*------------------------------------------------------------------------
 Module:      ParaCfg.cpp
 Author:      linxueqin
 Project:     子站平台
 State:       V1.0
 Creation Date:   2014-4-29
 Description:   运行参数，规约参数，遥信，遥测，遥控参数的初始化，保存及复位
------------------------------------------------------------------------*/


#ifndef _PARA_CFG

#define _PARA_CFG
  #include "DataStruct.h"
#endif

//==============================================================================
//  函数名称   : InitPara
//  功能描述   : 注意对用到的参数进行开机初始化，并从EEPROM中取出掉电不丢失的数据
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void InitcfgPara(void)
{
    unsigned int i;
    for(i = 0; i < PARA_NUM; i++)
    {
        g_gChangFlag[i] = OFF;
    }
    CheckRECPara();   //读取各故障的条数及当前位置	
      RstRMTInfo(0);	
    CheckCfgPara();
    InitTsYxInfo();
    InitTsYcInfo();
    
    CheckProtCnt(); //读取保护定值，如果定值错误置对应的遥信位

    CheckAdjPara(); //读取各通道校正参数，如果参数错误，置对应的遥信位，并按照默认参数校正
    FEED_WATCH_DOG();
    CheckRunInfo(); //读取运行信息，如果信息错误，则把运行信息清0
   
    CheckTELNUMPara(); //读取电话号码
     FEED_WATCH_DOG();

   // g_gChangFlag[CHNG_INFO] = ON;
    FEED_WATCH_DOG();
   // CheckParaData();    //检查参数数据的有效性
}
void InitTsYxInfo(void)
{
    int i = 0;
    //unsigned int RIIndex = 0;
   // int sYxCount = 0;
   // int dYxCount = 0;
    //根据遥信点表计算出需要上传的遥信个数
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
    //根据遥测点表计算出需要上传的遥测个数
    for(i = 0; i < IEC_YC_NUM; i++)
    {
        if(g_ucYCAddr[i] == 0)
            break;
    }
    g_ucYcTransNum = i;
    for(i = 0; i < g_ucYcTransNum; i++)   //计算需要上传的遥测值，按照具体情况进行换算
    {
      RIIndex = g_ucYCAddr[i];
      if(RIIndex < 255)
          g_unYcTrans[i] = g_gRmtMeas[g_ucYCAddr[i] - 1];//(((unsigned long)g_gRmtMeas[g_ucYCAddr[i] - 1] * g_unYCCoe[i]) >> 10 )+ g_unYCOffset[i];
      else
        g_unYcTrans[i] = 0;
  }
}

//==============================================================================
//  函数名称   : TELNUMTOggSmsPhone
//  功能描述   : 将由液晶输入的手机号及控制字TEL_NUM[]，转换为101格式的g_gSmsPhone[],同时将手机号码定位11位
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值      : <无>
//  其他说明   : 
//  作者       ：//张弢 0328
//==============================================================================

void TELNUMTOggSmsPhone(void)
{

    //unsigned char i;

    //g_gSmsPhone[0]=11;g_gSmsPhone[16]=11;g_gSmsPhone[32]=11;g_gSmsPhone[48]=11;//将手机号码定位11位
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
//  函数名称   : SaveCfgPara
//  功能描述   : 在运行过程中，根据g_gChangFlag中的参数更改标志判断某各配置参数发生变化，把配置参数保存到对应的存储器和备份区中
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void SaveCfgPara(void)  //在运行过程中，如果某各配置参数发生变化，把配置参数保存到对应的存储器中
{
    unsigned int i;      //tPCSele[2],
    unsigned int temp[8];
    unsigned long ulAddr;
	unsigned char u_dellubo;
    //unsigned int ucTemp[2];
    if(g_gChangFlag[CHNG_INFO] == ON)       //定值发生修改
    {
        g_gChangFlag[CHNG_INFO] = OFF;
        g_gRunInfo[INF_CRC] = CrcCount(g_gRunInfo, INF_CRC);    //计算CRC
       
        CAT_SpiWriteWords(EEPADD_COM, RUN_INFO_NUM, g_gRunInfo);  //保存到EEPROM中
        CAT_SpiWriteWords(EEPADDBK_COM, RUN_INFO_NUM, g_gRunInfo);    //保存到EEPROM的备份区中
           
    }

    //电压bais
    if(g_gChangFlag[CHNG_ADJ] == ON)    //定值发生修改
    {
        g_gChangFlag[CHNG_ADJ] = OFF;
        g_gAdjPara[ADJ_CRC] = CrcCount((unsigned int *)g_gAdjPara, ADJ_CRC);      //计算CRC
        CAT_SpiWriteWords(EEPADD_BIAS_V, ADJ_PARA_NUM, (unsigned int *)g_gAdjPara);     //保存到EEPROM中
        CAT_SpiWriteWords(EEPADDBK_BIAS_V, ADJ_PARA_NUM, (unsigned int *)g_gAdjPara);   //保存到EEPROM的备份区中
        //重启系统
           WDTCTL = WDTPW+WDTIS1+WDTIS0 + WDTIS2;//修改看门狗的周期，从而能够更快重启
           _DINT();       //关闭全局中断
           while(1);
 
    }

    //保护定值
    if(g_gChangFlag[CHNG_PC] == ON || g_gChangFlag[CHNG_MUBIAO] == ON) //定值发生修改
    {
        
        g_gChangFlag[CHNG_PC] = OFF;
        if(g_gProcCnt[PC_JAG_P] == 0 && g_gProcCnt[PC_JAG_Z] == 0)
        {
           g_gProcCnt[PC_CNL_MODEL] = 0;
           g_gRunPara[RP_CNL_MODEL] = 0;
           g_gRmtInfo[YX_FUN_ON] =0;
           g_gProcCnt[PC_JAG_P] = 0;       //相判据标志
           g_gProcCnt[PC_JAG_Z] = 0;       //零序判据标志
        }
        else if(g_gProcCnt[PC_JAG_P] == 0x55 && g_gProcCnt[PC_JAG_Z] == 0x55)
        {
           g_gProcCnt[PC_CNL_MODEL] = 1;
           g_gRunPara[RP_CNL_MODEL] = 1;
           g_gRmtInfo[YX_FUN_ON] =  1;
           g_gProcCnt[PC_JAG_P] = 0x55;       //相判据标志
           g_gProcCnt[PC_JAG_Z] = 0x55;       //零序判据标志
        }
        else
        {
            g_gProcCnt[PC_CNL_MODEL] = 1;
            g_gRunPara[RP_CNL_MODEL] = 1;
            g_gRmtInfo[YX_FUN_ON] =  1;
            //g_gProcCnt[PC_JAG_P] = 0;       //相判据标志
            //g_gProcCnt[PC_JAG_Z] = 0x55;       //零序判据标志
        }

        for(i = 0; i <= PROC_CNT_NUM; i++)
           g_gRunPara[i + RP_UA_ADJ] = g_gProcCnt[i];
        g_gRunPara[RP_CRC] = CrcCount((unsigned int *)g_gRunPara, RP_CRC);      //计算CRC
        CAT_SpiWriteWords(EEPADD_RP, RUN_PARA_NUM, g_gRunPara); //保存到EEPROM中
        CAT_SpiWriteWords(EEPADDBK_RP, RUN_PARA_NUM, g_gRunPara); //保存到EEPROM中        
        /*
        //张弢 目标校准，上位机下载参数10字节 初始值为电压60V,电流2A g_gAdjObj[ADJ_PARA_NUM]	 
	      g_gAdjObj[ADJ_CRC] = CrcCount((unsigned int *)g_gAdjObj, ADJ_CRC);      //计算CRC
             CAT_SpiWriteWords(EEPADD_ADJOBJ , ADJ_PARA_NUM, (unsigned int *)g_gAdjObj);     //保存到EEPROM中
             CAT_SpiWriteWords(EEPADDBK_ADJOBJ , ADJ_PARA_NUM, (unsigned int *)g_gAdjObj);   //保存到EEPROM的备份区中
             //张弢 目标校准，上位机下载参数10字节 初始值为电压60V,电流2A
        */
        //根据实际程序中判断需要，把定值依据系数换算成内部程序的定值数据
        CalcProtCnt();
        
       // g_gProcCnt[PC_CRC] = CrcCount((unsigned int *)g_gProcCnt, PC_CRC);      //计算CRC
       // CAT_SpiWriteWords(EEPADD_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);     //保存到EEPROM中
      //  CAT_SpiWriteWords(EEPADDBK_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);    //保存到EEPROM的备份区中 
        if(g_gChangFlag[CHNG_MUBIAO] == ON)
        {
             g_gChangFlag[CHNG_MUBIAO] = OFF;		
		//张弢 目标校准，上位机下载参数10字节 初始值为电压60V,电流2A g_gAdjObj[ADJ_PARA_NUM]	 
	     g_gAdjObj[ADJ_CRC] = CrcCount((unsigned int *)g_gAdjObj, ADJ_CRC);//计算CRC             
             CAT_SpiWriteWords(EEPADD_ADJOBJ, ADJ_PARA_NUM, g_gAdjObj);//保存到EEPROM中               
             CAT_SpiWriteWords(EEPADDBK_ADJOBJ , ADJ_PARA_NUM, g_gAdjObj);   //保存到EEPROM的备份区中
             //张弢 目标校准，上位机下载参数10字节 初始值为电压60V,电流2A
             
             //重启系统
           WDTCTL = WDTPW+WDTIS1+WDTIS0 + WDTIS2;//修改看门狗的周期，从而能够更快重启
           _DINT();       //关闭全局中断
           while(1);
        }
    }
    if(g_gChangFlag[CHNG_TEL] == ON)       //电话号码发生修改
    {
        g_gChangFlag[CHNG_PC] = OFF;  

  	TELNUMTOggSmsPhone();//张弢 0328
         g_gSmsPhone[PHONE_CS] = AddChar(g_gSmsPhone, PHONE_CS);      //计算CS//张弢 0328
        CAT_SpiWriteBytes(EEPADD_PHONE , PHONE_PA_NUM, g_gSmsPhone);     //保存到EEPROM中//张弢 0328
    }
    RefreshPara();
 
    //运行参数
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
        g_gRunPara[RP_CRC] = CrcCount((unsigned int *)g_gRunPara, RP_CRC);      //计算CRC
        CAT_SpiWriteWords(EEPADD_RP, RUN_PARA_NUM, g_gRunPara); //保存到EEPROM中
        CAT_SpiWriteWords(EEPADDBK_RP, RUN_PARA_NUM, g_gRunPara); //保存到EEPROM中           

	 /*			
        ucTemp[1] = g_ucPara_stime;ucTemp[0] = g_ucPara_stime; //张弢 0329  主动上传间隔时间
        CAT_SpiWriteWords(EEPADD_STIME_PRAR, 2,ucTemp) ;   //张弢 0329  主动上传间隔时间
        
        g_ucPara_stime = g_gRunPara[RP_SENDSMS_T];//张弢 遥测起始地址修改运行参数
        
	 if((g_ucPara_stime <2)||(g_ucPara_stime>48))  ////预置成24小时发送一次
            g_ucPara_stime = 24; 
        */
        if(g_gRunPara[RP_SENDYC_T] > 0)   //定时上传遥信
#ifdef YN_101S
            g_gsendYCTimeNum = ((unsigned long)g_gRunPara[RP_SENDYC_T]*10);
#else
            g_gsendYCTimeNum = ((unsigned long)g_gRunPara[RP_SENDYC_T]);
#endif
        else
            g_gsendYCTimeNum = 0;
        if(g_gRunPara[RP_SENDYX_T] > 0)   //定时上传遥信
            g_gsendYXTimeNum = ((unsigned long)g_gRunPara[RP_SENDYX_T]);
        else
            g_gsendYXTimeNum = 0;
        if(g_gRunPara[RP_BEAT_T] > 0)   //定时心跳
            g_gBeatNum = ((unsigned long)g_gRunPara[RP_BEAT_T]);
        else
            g_gBeatNum = 0;
	 if(g_gRunPara[RP_SENDZJ_T] > 0)   //定时上传自检
            g_gsendZJTimeNum = ((unsigned long)g_gRunPara[RP_SENDZJ_T])*10;
        else
            g_gsendZJTimeNum = 0;	
        if(g_gRunPara[RP_CNL_MODEL] == 0)
        {
           g_gRmtInfo[YX_FUN_ON] = 0;
           g_gRunPara[RP_JAG_P] = 0;       //相判据标志
           g_gRunPara[RP_JAG_Z] = 0;       //零序判据标志
        }
        else
        {
           g_gRmtInfo[YX_FUN_ON] = 1;
          // g_gRunPara[RP_JAG_P] = 0x55;       //相判据标志
          // g_gRunPara[RP_JAG_Z] = 0x55;       //零序判据标志
        }
	 if((g_gRunPara[RP_T_DELAY]/10-200)<=g_gRunPara[RP_RHSEND_TIME1])
	 	{
	 	g_gRunPara[RP_RHSEND_TIME1]=100;
		g_gRunPara[RP_T_DELAY] = 10000;               //故障延时时间
	 	}
	if(u_dellubo==1 )
    	{
    	//g_gRunPara[RP_CFG_KEY]=g_gRunPara[RP_CFG_KEY]&(~BIT[RPCFG_DEL_LUBO]);
    	u_dellubo  = 0;
		DelALLSOE();
    		temp[0]=0;temp[1]=0;temp[2]=0;temp[3]=0;temp[4]=0;temp[5]=0;    	
       	CAT_SpiWriteWords(EEPADD_LUBONUM, 6, temp); 
        	g_sRecData.m_gRecANum=temp[0];//录波总条数1~32
      		g_sRecData.m_gRecCNum=temp[1];//录波当前位置0~31
      		g_sRecData.m_gACTRecANum=temp[2];//录波总条数1~32
      		g_sRecData.m_gACTRecCNum=temp[3];//录波当前位置0~31      
      		g_sRecData.m_gXHRecANum=temp[4];//录波总条数1~32
      		g_sRecData.m_gXHRecCNum=temp[5];//录波当前位置0~31

		  ulAddr = FADDR_RECORDER_DATA+(unsigned long)(g_sRecData.m_gRecCNum)*0x2000;//flash地址  
  Sector_Erase(ulAddr);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x1000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  
  ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;  
  g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址  
  Block_Erase(ulAddr);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x10000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x20000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x30000);//ERASE 1个BLOCK 
  delayms(100);WatchDog(); 
  Block_Erase(ulAddr+0x40000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x50000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x60000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x70000);//ERASE 1个BLOCK 
  delayms(100);WatchDog(); 
  Block_Erase(ulAddr+0x80000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();  

  ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;//flash地址
  //g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址  
  Sector_Erase(ulAddr);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x1000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x2000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x3000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x4000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x5000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x6000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x7000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();  
  FlashReading = 0;	
  
        }
	 if(g_gRunPara[RP_RHSEND_TIME1]<65)
	 	g_gRunPara[RP_RHSEND_TIME1]=100;
	 if(g_gRunPara[RP_CT_TRANS]==0)g_gRunPara[RP_CT_TRANS]=20;
         CalcProtCnt();

    }
    
    //IEC101参数设置
    if((g_ucParaChang & BIT1) == BIT1)
    {
        g_ucParaChang &= NBIT1;
        g_ucPara101[IECP_CRC] = AddChar(g_ucPara101, IECP_CRC);      //计算CS
        CAT_SpiWriteBytes(EEPADD_IECPARA , IEC_PARA_NUM, g_ucPara101);     //保存到EEPROM中
        CAT_SpiWriteBytes(EEPADDBK_IECPARA, IEC_PARA_NUM, g_ucPara101);    //保存到EEPROM的备份区中
        //InitSCI();
    }
    //遥信地址点表
    if((g_ucParaChang & BIT2) == BIT2)
    {
        g_ucParaChang &= NBIT2;
        /*g_ucYXAddr[IEC_YX_NUM] = AddChar(g_ucYXAddr, IEC_YX_NUM);      //计算CS
        #define RDYXMUN_MAX 100 //单次读取e2最大个数
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
        
        //CAT_SpiWriteBytes(EEPADD_IECYXADDR , IEC_YX_NUM*2 + 1, (unsigned char *)g_ucYXAddr);     //保存到EEPROM中
        //CAT_SpiWriteBytes(EEPADDBK_IECYXADDR, IEC_YX_NUM*2 + 1, (unsigned char *)g_ucYXAddr);    //保存到EEPROM的备份区中
        if(g_ucYXAddr[0] == 0)
        {
            RstIEC101YxAddr();  //遥信点表初始化
            //DelALLSOE();
        }
        
        g_ucYXAddr[IEC_YX_NUM] = AddChar(g_ucYXAddr, IEC_YX_NUM);      //计算CRC
        CAT_SpiWriteBytes(EEPADD_IECYXADDR , IEC_YX_NUM + 1, g_ucYXAddr);     //保存到EEPROM中
        CAT_SpiWriteBytes(EEPADDBK_IECYXADDR, IEC_YX_NUM + 1, g_ucYXAddr);    //保存到EEPROM的备份区中
        //DelALLSOE();
        //根据遥信点表计算出需要上传的遥信个数
        for(i = 0; i < IEC_YX_NUM; i++)
        {
            if(g_ucYXAddr[i] == 0)
                break;
        }
        g_ucYxTransNum = i;
       /* g_gRMTBitBackCount = 0; //如果遥信点表重新配置，则清空未上传的长时标遥信
        g_gRMTBitBackCount_Aut = 0;
        g_EquInfo[COMM_INDEX_GPRS]  &= NBIT1;//清标识
        g_EquInfo_Aut  &= NBIT1;//清标识*/
        
   }
    //遥测参数
    if((g_ucParaChang & BIT3) == BIT3)
    {
        g_ucParaChang &= NBIT3;             
        g_ucYCAddr[IEC_YC_NUM] = AddChar(g_ucYCAddr, IEC_YC_NUM);      //计算CS
        CAT_SpiWriteBytes(EEPADD_IECYCADDR , IEC_YC_NUM + 1, g_ucYCAddr);     //保存到EEPROM中
        CAT_SpiWriteBytes(EEPADDBK_IECYCADDR, IEC_YC_NUM + 1, g_ucYCAddr);    //保存到EEPROM的备份区中
      
    }
    //遥控地址点表
 /*   if((g_ucParaChang & BIT4) == BIT4)
    {
        g_ucParaChang &= NBIT4;
        g_ucYKPa[IEC_YK_NUM] = AddChar(g_ucYKPa, IEC_YK_NUM);      //计算CS
        CAT_SpiWriteBytes(EEPADD_IECYKADDR , IEC_YK_NUM + 1, g_ucYKPa);     //保存到EEPROM中
        CAT_SpiWriteBytes(EEPADDBK_IECYKADDR, IEC_YK_NUM + 1, g_ucYKPa);    //保存到EEPROM的备份区中
    }
   *///电话号码表 //张弢 0328
    if((g_ucParaChang & BIT5) == BIT5)
    {
        g_ucParaChang &= NBIT5;
        g_gSmsPhone[PHONE_CS] = AddChar(g_gSmsPhone, PHONE_CS);      //计算CS
        CAT_SpiWriteBytes(EEPADD_PHONE , PHONE_PA_NUM, g_gSmsPhone);     //保存到EEPROM中
        CAT_SpiWriteBytes(EEPADDBK_PHONE, PHONE_PA_NUM, g_gSmsPhone);    //保存到EEPROM的备份区中
        CheckTELNUMPara();//张弢 0328 将g_gSmsPhone转成TEL_NUM g_ucphone_perm 
    }
 
   	//张弢 读汉字站名
    if((g_ucParaChang & BIT6) == BIT6)
    {
        g_ucParaChang &= NBIT6;
        g_gCnName[CnName_NUM] = AddInt(g_gCnName, CnName_NUM);      //计算CS
        CAT_SpiWriteWords(EEPADD_CNNAME   , CnName_NUM+1, g_gCnName);     //保存到EEPROM中
        CAT_SpiWriteWords(EEPADDBK_CNNAME  , CnName_NUM+1, g_gCnName);    //保存到EEPROM的备份区中
        g_gLBName[LBName_NUM] = AddChar(g_gLBName, LBName_NUM);      //计算CS
        CAT_SpiWriteBytes(EEPADD_LBNAME   , LBName_NUM+1, g_gLBName);     //保存到EEPROM中
        CAT_SpiWriteBytes(EEPADD_LBNAMELEN, 1, &g_gLBNameLen);    //保存到EEPROM的备份区中
    }	
    if((g_ucParaChang & BIT7) == BIT7)
    {
        g_ucParaChang &= NBIT7;
    }	

}
//==============================================================================
//  函数名称   : CheckCfgPara
//  功能描述   : 从EEPROM中读取参数，并做校验判断数据的有效性，数据错误则报101参数错误遥信，并把101参数置为默认值
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void CheckCfgPara(void)
{
    //unsigned char i;
    //unsigned int unTemp[2];
	
	
    //从EEPROM中读取运行参数，判断有效性，有效则赋值给g_gRunPara，无效则初始化g_gRunPara为默认值
    CAT_SpiReadWords(EEPADD_RP, RUN_PARA_NUM, g_gRunPara);  //读出当前保护定值
    if (g_gRunPara[RP_CRC] != CrcCount((unsigned int *)g_gRunPara, RP_CRC)) //CRC校验错误
    {
    	CAT_SpiReadWords(EEPADDBK_RP, RUN_PARA_NUM, g_gRunPara);  //读出当前保护定值
    	if (g_gRunPara[RP_CRC] != CrcCount((unsigned int *)g_gRunPara, RP_CRC)) //CRC校验错误
      {
      		RstRunPara();
      		CalcProtCnt();
      		g_ucParaChang |= BIT0;   //调用保存函数
    	}
	else
	{
		 CAT_SpiWriteWords(EEPADD_RP, RUN_PARA_NUM, g_gRunPara); //保存到EEPROM中
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
           
           g_gRunPara[RP_JAG_P] = 0;       //相判据标志
           g_gRunPara[RP_JAG_Z] = 0;       //零序判据标志
           delayms(500);
           g_gRmtInfo[YX_FUN_ON] = 0;
        }
        else
        {
           
          // g_gRunPara[RP_JAG_P] = 0x55;       //相判据标志
          // g_gRunPara[RP_JAG_Z] = 0x55;       //零序判据标志
           delayms(500);
           g_gRmtInfo[YX_FUN_ON] = 1;
        }
    if((g_gRunPara[RP_SENDSMS_T]<2)||(g_gRunPara[RP_SENDSMS_T]>48))
    {
        g_gRunPara[RP_SENDSMS_T]= 24;
    }		
	
    //101的系统参数
    CAT_SpiReadBytes(EEPADD_IECPARA, IEC_PARA_NUM, g_ucPara101);  //读出
  
    if (g_ucPara101[IECP_CRC] != AddChar(g_ucPara101, IECP_CRC))       //CRC校验
    {
        CAT_SpiReadBytes(EEPADDBK_IECPARA, IEC_PARA_NUM, g_ucPara101);    //读出
       
        if (g_ucPara101[IECP_CRC] == AddChar(g_ucPara101, IECP_CRC))       //CRC校验
        {
            CAT_SpiWriteBytes(EEPADD_IECPARA, IEC_PARA_NUM, g_ucPara101); //备份区覆盖过去
        }
        else
        {
            RstIEC101Para();  //IEC para initial
            g_ucParaChang |= BIT1;   //调用保存函数
        }
    }
    //101的遥信点表
      CAT_SpiReadBytes(EEPADD_IECYXADDR, IEC_YX_NUM + 1, g_ucYXAddr);  //读出
    if (g_ucYXAddr[IEC_YX_NUM] != AddChar(g_ucYXAddr, IEC_YX_NUM))       //CRC校验
    {
        CAT_SpiReadBytes(EEPADDBK_IECYXADDR, IEC_YX_NUM + 1, g_ucYXAddr);    //读出
        if (g_ucYXAddr[IEC_YX_NUM] == AddChar(g_ucYXAddr, IEC_YX_NUM))       //CRC校验
        {
            CAT_SpiWriteBytes(EEPADD_IECYXADDR, IEC_YX_NUM + 1, g_ucYXAddr); //备份区覆盖过去
        }
        else
        {
            RstIEC101YxAddr();  //遥信点表初始化
            g_ucParaChang |= BIT2;   //调用保存函数
        }
    }
  /*  if (g_ucYXAddr[IEC_YX_NUM] != AddChar((unsigned char *)g_ucYXAddr, IEC_YX_NUM*2))       //CRC校验
    {
       RstIEC101YxAddr();  //遥信点表初始化
       g_ucParaChang |= BIT2;   //调用保存函数
    }*/
    
    //101的遥测点表
    CAT_SpiReadBytes(EEPADD_IECYCADDR, IEC_YC_NUM + 1, g_ucYCAddr);  //读出
    if (g_ucYCAddr[IEC_YC_NUM] != AddChar(g_ucYCAddr, IEC_YC_NUM))       //CRC校验
    {
        CAT_SpiReadBytes(EEPADDBK_IECYCADDR, IEC_YC_NUM + 1, g_ucYCAddr);    //读出
        if (g_ucYCAddr[IEC_YC_NUM] == AddChar(g_ucYCAddr, IEC_YC_NUM))       //CRC校验
        {
            CAT_SpiWriteBytes(EEPADD_IECYCADDR, IEC_YC_NUM + 1, g_ucYCAddr); //备份区覆盖过去
        }
        else
        {
            RstIEC101YcAddr();  //遥测点表初始化
            g_ucParaChang |= BIT3;   //调用保存函数
        }
    }
    //张弢 读汉字站名
    CAT_SpiReadWords(EEPADD_CNNAME, CnName_NUM+1, g_gCnName);  //读出
    if (g_gCnName[CnName_NUM] != AddInt(g_gCnName, CnName_NUM))       //CRC校验
    {
        CAT_SpiReadWords(EEPADDBK_CNNAME, CnName_NUM+1, g_gCnName);    //读出
        if (g_gCnName[CnName_NUM] == AddInt(g_gCnName, CnName_NUM))       //CRC校验
        {
            CAT_SpiWriteWords(EEPADD_CNNAME, CnName_NUM+1, g_gCnName); //备份区覆盖过去
        }
        else
        {
            RstCnName();  //遥测点表初始化
            g_ucParaChang |= BIT6;   //调用保存函数
        }
    }
    CAT_SpiReadBytes(EEPADD_LBNAME   , LBName_NUM+1, g_gLBName);     //保存到EEPROM中
    CAT_SpiReadBytes(EEPADD_LBNAMELEN, 1, &g_gLBNameLen);    //保存到EEPROM的备份区中    //101的遥控点表
    if ((g_gLBName[CnName_NUM] != AddChar(g_gLBName, LBName_NUM))||(g_gLBNameLen>LBName_NUM))
    	{
    	 RstLBName(); 
    	}
    if(g_gRunPara[RP_FLOAD_T] !=0)g_gSaveload=g_gRunPara[RP_FLOAD_T]-1;//每隔一段时间存储负荷记录
	
    	
   /* CAT_SpiReadBytes(EEPADD_IECYKADDR, IEC_YK_NUM + 1, g_ucYKPa);  //读出
    if (g_ucYKPa[IEC_YK_NUM] != AddChar(g_ucYKPa, IEC_YK_NUM))       //CRC校验
    {
        CAT_SpiReadBytes(EEPADDBK_IECYKADDR, IEC_YK_NUM + 1, g_ucYKPa);    //读出
        if (g_ucYKPa[IEC_YK_NUM] == AddChar(g_ucYKPa, IEC_YK_NUM))       //CRC校验
        {
            CAT_SpiWriteBytes(EEPADD_IECYKADDR, IEC_YK_NUM + 1, g_ucYKPa); //备份区覆盖过去
        }
        else
        {
            RstIEC101YkAddr();  //遥控点表初始化
            g_ucParaChang |= BIT4;   //调用保存函数
        }
    }
    
    //电话号码表
    CAT_SpiReadBytes(EEPADD_PHONE, PHONE_PA_NUM, g_gSmsPhone);  //读出
    
    if (g_gSmsPhone[PHONE_CS] != AddChar(g_gSmsPhone, PHONE_CS))       //CRC校验
    {
        CAT_SpiReadBytes(EEPADDBK_PHONE, PHONE_PA_NUM, g_gSmsPhone);    //读出
        if (g_gSmsPhone[PHONE_CS] == AddChar(g_gSmsPhone, PHONE_CS))       //CRC校验
        {
            CAT_SpiWriteBytes(EEPADD_PHONE, PHONE_PA_NUM, g_gSmsPhone); //备份区覆盖过去
        }
        else
        {
            RstSmsPhoneInfo();  //电话号码表初始化
            g_ucParaChang |= BIT5;   //调用保存函数
        }
    }
    */
}
void CheckCfgERR(void)
{    
    if (g_gRunPara[RP_CRC] != CrcCount((unsigned int *)g_gRunPara, RP_CRC)) //CRC校验错误
    {
    	CAT_SpiReadWords(EEPADDBK_RP, RUN_PARA_NUM, g_gRunPara);  //读出当前保护定值
    	if (g_gRunPara[RP_CRC] != CrcCount((unsigned int *)g_gRunPara, RP_CRC)) //CRC校验错误
      		{
      		RstRunPara();
      		//CalcProtCnt();      		
    		}
		g_ucParaChang |= BIT0;   //调用保存函数
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
           
           g_gRunPara[RP_JAG_P] = 0;       //相判据标志
           g_gRunPara[RP_JAG_Z] = 0;       //零序判据标志
           delayms(500);
           g_gRmtInfo[YX_FUN_ON] = 0;
        }
        else
        {
           
          // g_gRunPara[RP_JAG_P] = 0x55;       //相判据标志
          // g_gRunPara[RP_JAG_Z] = 0x55;       //零序判据标志
           delayms(500);
           g_gRmtInfo[YX_FUN_ON] = 1;
        }
    if((g_gRunPara[RP_SENDSMS_T]<2)||(g_gRunPara[RP_SENDSMS_T]>48))
    {
        g_gRunPara[RP_SENDSMS_T]= 24;
    }
	if(g_gRunPara[RP_KM_TIME]==0)
		g_gRunPara[RP_KM_TIME]=100;
	
    //101的系统参数  
    if (g_ucPara101[IECP_CRC] != AddChar(g_ucPara101, IECP_CRC))       //CRC校验
    {
        CAT_SpiReadBytes(EEPADDBK_IECPARA, IEC_PARA_NUM, g_ucPara101);    //读出       
        if (g_ucPara101[IECP_CRC] != AddChar(g_ucPara101, IECP_CRC))       //CRC校验
        	{        
            RstIEC101Para();  //IEC para initial
        	}
       	g_ucParaChang |= BIT1;   //调用保存函数        
    }
	
    //101的遥信点表     
    if (g_ucYXAddr[IEC_YX_NUM] != AddChar(g_ucYXAddr, IEC_YX_NUM))       //CRC校验
    {
        CAT_SpiReadBytes(EEPADDBK_IECYXADDR, IEC_YX_NUM + 1, g_ucYXAddr);    //读出
        if (g_ucYXAddr[IEC_YX_NUM] != AddChar(g_ucYXAddr, IEC_YX_NUM))       //CRC校验
        {
            RstIEC101YxAddr();  //遥信点表初始化
        }
        g_ucParaChang |= BIT2;   //调用保存函数        
    }
     
    //101的遥测点表
    if (g_ucYCAddr[IEC_YC_NUM] != AddChar(g_ucYCAddr, IEC_YC_NUM))       //CRC校验
    {
        CAT_SpiReadBytes(EEPADDBK_IECYCADDR, IEC_YC_NUM + 1, g_ucYCAddr);    //读出
        if (g_ucYCAddr[IEC_YC_NUM] != AddChar(g_ucYCAddr, IEC_YC_NUM))       //CRC校验
        {
             RstIEC101YcAddr();  //遥测点表初始化
        }
        g_ucParaChang |= BIT3;   //调用保存函数        
    }
    
}

//==============================================================================
//  函数名称   : CheckProtCnt
//  功能描述   : 根据当前保护定值组号从EEPROM中读取保护定值，并做校验判断数据的有效性，数据错误则报保护定值错误遥信
//       根据g_gRunPara中的数据，换算成内部判断需要用到的数据保存到g_gRunParaJdg中
//  输入参数   : <无>
//  输出参数   : g_gProcCnt - 保护定值
//     : g_gProcCntJdg - 经过系数换算后的保护定值，用于内部保护逻辑判断使用
//  返回值     : <无>
//  其他说明   : 
//  作者       ：林中一
//==============================================================================
void CheckProtCnt(void)
{
  /*  unsigned int i;
    CAT_SpiReadWords(EEPADD_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);       //读出当前保护定值
    if (g_gProcCnt[PC_CRC] == CrcCount((unsigned int *)g_gProcCnt, PC_CRC)) //CRC校验*/
    {
        //根据实际程序中判断需要，把定值依据系数换算成内部程序的定值数据
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
        CAT_SpiReadWords(EEPADDBK_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);     //读出当前保护定值
        if (g_gProcCnt[PC_CRC] == CrcCount((unsigned int *)g_gProcCnt, PC_CRC)) //CRC校验
        {
            //根据实际程序中判断需要，把定值依据系数换算成内部程序的定值数据
            CalcProtCnt();

            CAT_SpiWriteWords(EEPADD_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);      //用备份区覆盖当前保护定值
        }       
        else
        {
           RstProcPara();//复位保护参数
           CalcProtCnt();
           CAT_SpiWriteWords(EEPADD_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);
           CAT_SpiWriteWords(EEPADDBK_CAL_PRAR, PROC_CNT_NUM, g_gProcCnt);

        }
    }
    for(i = 0; i < 24 ; i++)
        g_gETHProcCnt[i] = g_gProcCnt[20 + i];*/
}

//==============================================================================
//  函数名称   : CheckAdjPara
//  功能描述   : 从EEPROM中读取校正参数，并做校验判断数据的有效性，数据错误则报校正参数错误遥信，并把校正参数置为默认值
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：林中一
//==============================================================================
void CheckAdjPara(void)
{
    unsigned int i;
    CAT_SpiReadWords(EEPADD_BIAS_V, ADJ_PARA_NUM, (unsigned int*)g_gAdjPara);  //读出
    if (g_gAdjPara[ADJ_CRC] == CrcCount((unsigned int *)g_gAdjPara, ADJ_CRC))       //CRC校验
    {
       // SetRIStatus(TST_ADJ, 0);
    }
    else
    {
        CAT_SpiReadWords(EEPADDBK_BIAS_V, ADJ_PARA_NUM, (unsigned int*)g_gAdjPara);    //读出
        if (g_gAdjPara[ADJ_CRC] == CrcCount((unsigned int *)g_gAdjPara, ADJ_CRC))       //CRC校验
        {
            //SetRIStatus(TST_ADJ, 0);
            CAT_SpiWriteWords(EEPADD_BIAS_V, ADJ_PARA_NUM, (unsigned int*)g_gAdjPara); //备份区覆盖过去
        }
        else
        {             //为了保证控制器能正常运行，即使校正系数错误，也需要把校正系数用默认值1代替
            
            for(i = 0; i < ADJ_CRC; i++)     //如果adj无效，则对adj赋初值
            {
                g_gAdjPara[i] = 2048;
            }
        }
    }
    for(i = 0; i < ADJ_CRC; i++)     //计算ad值得偏移
   {
      g_gAdjAD[i] = (unsigned int)((long)(g_gAdjPara[i]) * g_gProcCnt[PC_UA_ADJ + i] >> 12);
    }
	//张弢 目标校准，上位机下载参数10字节 初始值为电压60V,电流2A g_gAdjObj[ADJ_PARA_NUM]	 
	      //g_gAdjObj[ADJ_CRC] = CrcCount((unsigned int *)g_gAdjObj, ADJ_CRC);      //计算CRC
       CAT_SpiReadWords(EEPADD_ADJOBJ,ADJ_PARA_NUM,(unsigned int *)g_gAdjObj);     //保存到EEPROM中
       if (g_gAdjObj[ADJ_CRC] == CrcCount((unsigned int *)g_gAdjObj, ADJ_CRC))       //CRC校验
      {       
      }
      else
      {	  
      		CAT_SpiReadWords(EEPADDBK_ADJOBJ , ADJ_PARA_NUM, (unsigned int *)g_gAdjObj);   //保存到EEPROM的备份区中
      		if (g_gAdjObj[ADJ_CRC] == CrcCount((unsigned int *)g_gAdjObj, ADJ_CRC))       //CRC校验
      		{
      			CAT_SpiWriteWords(EEPADD_ADJOBJ,ADJ_PARA_NUM,(unsigned int *)g_gAdjObj);     //保存到EEPROM中
      		}
		else
		{
			g_gAdjObj[0]=0;g_gAdjObj[1]=0;g_gAdjObj[2]=0;g_gAdjObj[3]=0;g_gAdjObj[4]=0;	
			g_gAdjObj[ADJ_CRC] = CrcCount((unsigned int *)g_gAdjObj, ADJ_CRC);      //计算CRC
             		CAT_SpiWriteWords(EEPADD_ADJOBJ,ADJ_PARA_NUM,(unsigned int *)g_gAdjObj);     //保存到EEPROM中
             		CAT_SpiWriteWords(EEPADDBK_ADJOBJ,ADJ_PARA_NUM,(unsigned int *)g_gAdjObj);//保存到EEPROM的备份区中
		}
      }      
	  //张弢 目标校准，上位机下载参数10字节 初始值为电压60V,电流2A
    
}

//==============================================================================
//  函数名称   : CheckRunInfo
//  功能描述   : 从EEPROM中读取运行信息，并做校验判断数据的有效性，把运行信息清0
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：林中一
//==============================================================================
void CheckRunInfo(void)
{
    unsigned int i;
    CAT_SpiReadWords(EEPADD_COM, RUN_INFO_NUM, g_gRunInfo); //读出
    if (g_gRunInfo[INF_CRC] != CrcCount((unsigned int *)g_gRunInfo, INF_CRC))       //CRC校验
    {
        CAT_SpiReadWords(EEPADDBK_COM, RUN_INFO_NUM, g_gRunInfo);       //读出
        if (g_gRunInfo[INF_CRC] == CrcCount((unsigned int *)g_gRunInfo, INF_CRC))       //CRC校验
        {
            CAT_SpiWriteWords(EEPADD_COM, RUN_INFO_NUM, g_gRunInfo);    
        }
        else
        {
            //运行信息错误，把全部都清为0
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
//  函数名称   : CheckTELNUMPara
//  功能描述   : 从EEPROM中读取读取电话号码，并做校验判断数据的有效性
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void CheckTELNUMPara(void)
{
    //unsigned char unTemp[7];//张弢 0328
    //unsigned char i;//,j;//张弢 0328

    CAT_SpiReadBytes(EEPADD_PHONE, PHONE_PA_NUM, g_gSmsPhone);  //读出//张弢 0328 

     if (g_gSmsPhone[PHONE_CS] != AddChar(g_gSmsPhone, PHONE_CS))       //CRC校验
    {
        CAT_SpiReadBytes(EEPADDBK_PHONE, PHONE_PA_NUM, g_gSmsPhone);    //读出
        if (g_gSmsPhone[PHONE_CS] == AddChar(g_gSmsPhone, PHONE_CS))       //CRC校验
        {
            CAT_SpiWriteBytes(EEPADD_PHONE, PHONE_PA_NUM, g_gSmsPhone); //备份区覆盖过去
        }
        else
        {
            RstSmsPhoneInfo();  //电话号码表初始化//张弢 0328 RstSmsPhoneInfo不存在
            g_ucParaChang |= BIT5;   //调用保存函数//张弢 0328 RstSmsPhoneInfo不存在
        }
    }
/*
    for(i=0;i<4;i++)	
    {
        g_ucphone_perm[i]=g_gSmsPhone[64+i];
    } 	
*/
     
//读出4个手机号 4*11
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
        unTemp[0]=0x7f;     //////////第一个手机号，给中心站发
        for(i=1;i<4;i++)      //////////余下的手机号，给用户发
        {
             unTemp[i]=0x77;
        } 
    }
    for(i=1;i<4;i++)      //////////余下的手机号，给用户发
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
    CAT_SpiReadBytes(EEPADD_PHONE_NUM, 44, TEL_NUM); //读出4个手机号 4*11
*/	
 
}


unsigned char unTemp12[4] = {0xff,0xff,0xff,0xff};
//==============================================================================
//  函数名称   : CheckRECPara
//  功能描述   : 从EEPROM中读取各类故障数据信息，并做校验判断数据的有效性
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
	CAT_SpiReadWords(EEPADD_SOE_E2ROMADR, 4, untemp);  //保存到EEPROM中
	g_unSSoeSaveE2ROMPtr=untemp[0];
	soe_num=untemp[2];
	if((untemp[0]!=untemp[1])||(untemp[0]<EEPADD_SOESTARTADR)||(untemp[0]>=EEPADD_SOEENDADR))
		DelALLSOE();
	if((untemp[2]!=untemp[3])||(untemp[2]>512))
		DelALLSOE();		
	g_unSSoeSaveE2ROMPtr &= 0xfff0;	

 
}



//==============================================================================
//  函数名称   : CalcProtCnt
//  功能描述   : 把保护定值换算成逻辑判断过程中用到的数据，保存到g_gProcCntJdg中，需要换算成(原始值*系数)的平方*16
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：林中一
//==============================================================================
void CalcProtCnt(void)
{
    unsigned char i;
    for(i = 0; i <= PROC_CNT_NUM; i++)
        g_gProcCnt[i] = g_gRunPara[i + RP_UA_ADJ];
//#ifdef YN_101S
    g_gProcCntJug[PC_HIGH_P] = g_gProcCnt[PC_HIGH_P] ;                //相电压高定值
    g_gProcCntJug[PC_LOW_P] = g_gProcCnt[PC_LOW_P];                //相电压低定值
    g_gProcCntJug[PC_HIGH_Z] = g_gProcCnt[PC_HIGH_Z];          //零序电压高定值
    g_gProcCntJug[PC_LOW_Z] = g_gProcCnt[PC_LOW_Z];          //零序电压低定值
    g_gProcCntJug[PC_NO_V] = g_gProcCnt[PC_NO_V];       //无压门槛值
    g_gProcCntJug[PC_I0_START] = g_gProcCnt[PC_I0_START];//线电压高定值
    g_gProcCntJug[PC_PULSE_VALID] = g_gProcCnt[PC_PULSE_VALID] /10 / 15;       //8脉冲有效定值
	if(g_gProcCnt[PC_T_DELAY] > 2000)
        g_gProcCntJug[PC_T_DELAY] = g_gProcCnt[PC_T_DELAY] /10 - 200;
    else
        g_gProcCntJug[PC_T_DELAY] = g_gProcCnt[PC_T_DELAY] /10;               //故障延时时间 换算成10毫秒
/*
#else
    g_gProcCntJug[PC_HIGH_P] = g_gProcCnt[PC_HIGH_P] * 100;                //相电压高定值
    g_gProcCntJug[PC_LOW_P] = g_gProcCnt[PC_LOW_P] * 100;  
    g_gProcCntJug[PC_HIGH_Z] = g_gProcCnt[PC_HIGH_Z] * 100;          //零序电压高定值
    g_gProcCntJug[PC_LOW_Z] = g_gProcCnt[PC_LOW_Z] * 100;          //零序电压低定值
    g_gProcCntJug[PC_NO_V] = g_gProcCnt[PC_NO_V] * 100;       //无压门槛值
    g_gProcCntJug[PC_I0_START] = g_gProcCnt[PC_I0_START] * 100;
    g_gProcCntJug[PC_PULSE_VALID] = g_gProcCnt[PC_PULSE_VALID] * 100 / 15;       //8脉冲有效定值
    if(g_gProcCnt[PC_T_DELAY] > 2)
        g_gProcCntJug[PC_T_DELAY] = g_gProcCnt[PC_T_DELAY] * 100 - 200;
    else
        g_gProcCntJug[PC_T_DELAY] = g_gProcCnt[PC_T_DELAY] * 100;               //故障延时时间 换算成10毫秒
#endif 
*/
    g_gProcCntJug[PC_LACTH_TIME] = g_gProcCnt[PC_LACTH_TIME] * 60;       //闭锁时间换算成秒
   // g_gProcCntJug[PC_JAG_Z] = 0x55;       //零序判据标志
   // g_gProcCntJug[PC_EFS_MODEL] = 0;       //信号源型号标识
   // g_gProcCntJug[PC_JAG_ACT] = 0;       //动作相序判据

   // g_gProcCntJug[PC_GPRS_MODEL] = 0;      //GPRS方式定值
   // g_gProcCntJug[PC_REV_CURRENT] = 20;       //翻转电流定值  
    g_gProcCntJug_I0[0]  =  (unsigned long)g_gProcCnt[PC_OVERLOAD_I] * g_gProcCnt[PC_OVERLOAD_I] * COEF_I_0_AD2 >> 7;               //  22448=(100/15 * 4096/COEF_AD_I_0)*(100/15 * 4096/COEF_AD_I_0)*128
    g_gProcCntJug_I0[1]  =  (unsigned long)g_gProcCntJug_I0[0] - (g_gProcCntJug_I0[0] >> 4);
    g_gProcCntJug_I0[2]  =  (unsigned long)g_gProcCnt[PC_OVERLOAD_T] * 8;

}
//==============================================================================
//  函数名称   : RstRunPara
//  功能描述   : 复位内存中的运行参数为默认值
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void RstRunPara(void)
{
    g_gRunPara[RP_CFG_KEY] = 0x008C;//1000 1100   
    g_gRunPara[RP_BEAT_T] = 0;//默认不传输心跳
    g_gRunPara[RP_SENDYC_T] = 600; 
    g_gRunPara[RP_SENDYX_T] = 600;      //状态汇报上线时间
    //g_gRunPara[RP_STATUSRP_INTERVAL] = 1;  //状态汇报上线间隔（天）
    g_gRunPara[RP_FLOAD_T] = 15;  ////负荷记录存储时间间隔 分钟
    g_gRunPara[RP_COMM_ADDR] = 1;//通信站地址      站地址默认为1
    g_gRunPara[RP_SYX_INFADDR] = 128;
#ifdef  CQ_101S
     g_gRunPara[RP_SYX_INFADDR]=1;//张弢重庆 0x0700+171
     g_gRunPara[RP_SENDYC_T] = 0; 
    g_gRunPara[RP_SENDYX_T] = 0;  
#endif
    g_gRunPara[RP_DYX_INFADDR] = DYX_ADDR;
    g_gRunPara[RP_YCLIMIT] = 1000;//遥测阈值 绝对
    g_gRunPara[RP_YCCAP] = 50;//遥测阈值 相对
    
    g_gRunPara[RP_PLUSE_MODFK]=3;
    g_gRunPara[RP_PLUSE_AMODFK]=3;
    g_gRunPara[RP_PLUSE_BMODFK]=3;
    g_gRunPara[RP_PLUSE_CMODFK]=3;	
    g_gRunPara[RP_PLUSEXH_MODFK] = 5; 
    
    g_gRunPara[RP_YXRSD_T] = 600;//10分钟 
    
    g_gRunPara[YX_delay] = 20;
    
    g_gRunPara[RP_UA_ADJ] = 4096;                //UA校准系数值
    g_gRunPara[RP_UB_ADJ] = 4096;        //UB校准系数值
    g_gRunPara[RP_UC_ADJ] = 4096;                //UC校准系数值
    g_gRunPara[RP_U0_ADJ] = 4096;                //U0校准系数值
    g_gRunPara[RP_I0_ADJ] = 4096;                //I0校准系数值
//#ifdef YN_101S
    g_gRunPara[RP_I0_START] = 11000;                //零序电流门槛值
    g_gRunPara[RP_HIGH_P] = 7000;                //相电压高定值
    g_gRunPara[RP_LOW_P] = 5000;                //相电压低定值
    g_gRunPara[RP_HIGH_Z] = 2500;          //零序电压高定值
    g_gRunPara[RP_LOW_Z] = 1000;          //零序电压低定值
    g_gRunPara[RP_NO_V] = 1000;       //无压门槛值
    g_gRunPara[RP_T_DELAY] = 10000;               //故障延时时间
    g_gRunPara[RP_PULSE_VALID] = 3000;       //8脉冲有效定值		
/*
#else
    g_gRunPara[RP_I0_START] = 110;                //零序电流门槛值
    g_gRunPara[RP_HIGH_P] = 70;                //相电压高定值
    g_gRunPara[RP_LOW_P] = 50;                //相电压低定值
    g_gRunPara[RP_HIGH_Z] = 25;          //零序电压高定值
    g_gRunPara[RP_LOW_Z] = 10;          //零序电压低定值
    g_gRunPara[RP_NO_V] = 10;       //无压门槛值
     g_gRunPara[RP_PULSE_VALID] = 3;       //8脉冲有效定值
    g_gRunPara[RP_T_DELAY] = 10;               //故障延时时间
#endif
*/
    g_gRunPara[RP_JAG_P] = 0x55;       //相判据标志
    g_gRunPara[RP_JAG_Z] = 0x55;       //零序判据标志
    g_gRunPara[RP_EFS_MODEL] = 1;       //信号源型号标识
    g_gRunPara[RP_JAG_ACT] = 3;       //动作相序判据

    g_gRunPara[RP_GPRS_MODEL] = 0;      //GPRS方式定值
    g_gRunPara[RP_REV_CURRENT] = 20;       //翻转电流定值
    g_gRunPara[RP_U0_CAL] = 1;      //U0产生方式 默认采集

    g_gRunPara[RP_CNL_MODEL] = 1; 		//控制方式
    g_gRunPara[RP_LACTH_TIME] = 17; 		//控制方式
#ifdef  SD_101S  
    g_gRunPara[RP_LACTH_TIME] = 15; 		//控制方式
#endif
    g_gRunPara[RP_PLUSE_TIME] = 20; 		//控制方式
    g_gRunPara[RP_SEND_TIME1] = 100; 		//控制方式
    g_gRunPara[RP_SEND_TIME2] = 125; 		//控制方式
#ifdef  CQ_101S    
    g_gRunPara[RP_PLUSE_TIME] = 16; 		//控制方式
    g_gRunPara[RP_SEND_TIME1] = 96; 		//控制方式
    g_gRunPara[RP_SEND_TIME2] = 116; 		//控制方式
#endif	
    g_gRunPara[RP_PLUSE_NUM] = 8; 
    g_gRunPara[RP_OVERLOAD_I] = 10000; 
    g_gRunPara[RP_OVERLOAD_T] = 6; 
    g_gRunPara[RP_YC_INFADDR]=0x4001;//张弢 遥测起始地址修改运行参数	
#ifdef YN_101S 
    g_gRunPara[RP_YC_INFADDR]=0x4081;//张弢 遥测起始地址修改运行参数		
#endif
    g_gRunPara[RP_SENDSMS_T]=24;//张弢 遥测起始地址修改运行参数
    g_gRunPara[RP_SENDZJ_T] = 0; 
#ifdef  YN_101S
    g_gRunPara[RP_SENDZJ_T] = 60; 
#endif	
#ifdef  SD_101S
   g_gRunPara[RP_RHSEND_TIME1] = 100; 
   g_gRunPara[RP_RHPLUSE_TIME2] = 50; 
   g_gRunPara[RP_RHT_DELAY]=300;//5min=300s
   g_gRunPara[RP_CT_TRANS]=20;//CT变比 =20 
#else
   g_gRunPara[RP_RHSEND_TIME1] = 0; 
   g_gRunPara[RP_RHPLUSE_TIME2] = 0; 
   g_gRunPara[RP_RHT_DELAY]=0;//60s
   g_gRunPara[RP_CT_TRANS]=15;//CT变比 =20    
#endif
	g_gRunPara[RP_KM_TIME]=100;

    state_counter= g_gRunPara[RP_SENDSMS_T]*60;	//张弢 遥测起始地址修改运行参数    
}

void RstIEC101Para(void )
{
      g_ucPara101[IECP_LINKADDR_NUM] = 2;//2;
      g_ucPara101[IECP_TRANSRSN_NUM] = 2;//2;
#ifdef CQ_101S 
     g_ucPara101[IECP_TRANSRSN_NUM] = 1;//张弢重庆  传送原因1字节		
#endif
      g_ucPara101[IECP_COMNADDR_NUM] = 2;
      g_ucPara101[IECP_INFO_NUM] = 2;
      g_ucPara101[IECP_COM_MODE] = 1;//0;
      g_ucPara101[IECP_AUT_MODE] = 0;
        CAT_SpiWriteBytes(EEPADD_IECPARA , IEC_PARA_NUM, g_ucPara101);     //保存到EEPROM中
        CAT_SpiWriteBytes(EEPADDBK_IECPARA, IEC_PARA_NUM, g_ucPara101);    //保存到EEPROM的备份区中     


}

void RstIEC101YxAddr(void)  //遥信点表的初始化
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

void RstIEC101YcAddr(void)  //遥测点表的初始化
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

void RstIEC101YcSiqu(void)  //遥测死区的初始化
{
//      unsigned char i;
//      for(i = 0; i < IEC_YC_NUM; i++)
//      {
//          g_unYCSiqu[i] = 10;
//      }
}

void RstIEC101YkAddr(void)  //遥控点表的初始化
{
   
    
}

void RstSmsPhoneInfo(void)  //电话号码表的初始化
{
    char chDefPhone[] = "13811005566";

    g_gSmsPhone[0]=11;g_gSmsPhone[16]=11;g_gSmsPhone[32]=11;g_gSmsPhone[48]=11;//将手机号码定位11位	
    memcpy(&(g_gSmsPhone[1]),chDefPhone,1);
    memcpy(&(g_gSmsPhone[17]),chDefPhone,1);
    memcpy(&(g_gSmsPhone[33]),chDefPhone,1);
    memcpy(&(g_gSmsPhone[49]),chDefPhone,1);	
    /*
    char chDefPhone[] = "+86 13811005566";
    g_gSmsPhone[PHONE_ENCTL] = 1;//默认第一个电话号码启用
    g_gSmsPhone[PHONE1_CHARLEN] = PHONE_CHARNUM;
    g_gSmsPhone[PHONE2_CHARLEN] = PHONE_CHARNUM;
    g_gSmsPhone[PHONE3_CHARLEN] = PHONE_CHARNUM;
    memcpy(&(g_gSmsPhone[PHONE1]),chDefPhone,PHONE_CHARNUM);
    memcpy(&(g_gSmsPhone[PHONE2]),chDefPhone,PHONE_CHARNUM);
    memcpy(&(g_gSmsPhone[PHONE3]),chDefPhone,PHONE_CHARNUM);
    */
}

void RstCnName(void)  //站名及地址初始化
{
    //北京市海淀区西北旺东路10号院科锐大厦
    g_gCnName[0]=0x5317;g_gCnName[1]=0x4eac;g_gCnName[2]=0x5e02;g_gCnName[3]=0x6d77;
    g_gCnName[4]=0x6dc0;g_gCnName[5]=0x533a;g_gCnName[6]=0x897f;g_gCnName[7]=0x5317;
    g_gCnName[8]=0x65fa;g_gCnName[9]=0x4e1c;g_gCnName[10]=0x8def;g_gCnName[11]=0x0031;
    g_gCnName[12]=0x0030;g_gCnName[13]=0x53f7;g_gCnName[14]=0x9662;g_gCnName[15]=0x002d;
    g_gCnName[16]=0x79d1;g_gCnName[17]=0x9510;g_gCnName[18]=0x5927;g_gCnName[19]=0x53a6;	
}
void RstLBName(void)  //站名及地址初始化
{
    //BJ-CREAT-DA,
    char a[13]="BJ-CREAT-DA";
    memcpy(g_gLBName,a,11);
    g_gLBNameLen=11;
}

