/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  模块名     : 液晶数据解析模块
  文件名     : Comm.c
  相关文件       : DataStruct.h FuncDef.h Constant.h DriverComm.c Main.c 
  实现功能       : 
  作者       : 
  版本       : 1.0
  
  --------------------------------------------------------------------------------
  备注       : -
  --------------------------------------------------------------------------------
  
  修改记录       : 
    日  期      版本        修改人      修改内容 


************************************************************************************/

#ifndef _COMM_LED

#define _COMM_LED
  #include "DataStruct.h"
#endif
 struct  check{                   //实时时钟结构 判据参数
	unsigned char high_phase;
	unsigned char low_phase;
	unsigned char high_zero;
	unsigned char low_zero;
	unsigned char voltage_zero;
}compare; 
//==============================================================================
//  函数名称   : InitComm
//  功能描述   : 通信模块初始化，包括对缓冲区的初始化
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ： 
//==============================================================================
/*void InitComm(void)
{
    InitSCI();
}*/
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
/*unsigned int CrcCount(unsigned int *p, unsigned int Length)
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

}*/


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
unsigned char AddComChar(unsigned char unCommPortIndex, unsigned char length,unsigned char ucFlag)
{
    unsigned int AddValue = 0;
    unsigned char i;
    for(i = 0; i < length; i++)
    {
        if(ucFlag == 0)
            AddValue += g_sRxBuff[0].m_gBuf[BuffPositionLED(g_uc101BuffHead + i + 4)];
        //else
           // AddValue += g_unTX_Buff[unCommPortIndex][4 + i];
    }
    return (unsigned char)AddValue;    
}

//==============================================================================
//  函数名称   : BuffCapacity_LED
//  功能描述   : 计算缓冲区接收到的新数据个数
//  输入参数   : <无>
//  输出参数   ：unCapacity：一帧数据的个数 
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int BuffCapacity_LED(void)
{
   unsigned int unCapacity;

   if(g_uc101BuffHead <= g_uc101BuffTail)   
       unCapacity = g_uc101BuffTail - g_uc101BuffHead;
   else       
       unCapacity = 128 - g_uc101BuffHead + g_uc101BuffTail;
  
   return unCapacity;
}
//==============================================================================
//  函数名称   : BuffPositionLED
//  功能描述   : 缓冲区中位置的计算
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int BuffPositionLED(unsigned int unpointer)
{
    // if Rx buff reach its end, restart from the beginning.
    if( unpointer >= 128)
        unpointer %= 128;

    return unpointer;
}
//==============================================================================
//  函数名称   : TakeFrame101
//  功能描述   : 将接收缓冲区的数据帧提取到接收数组
//  输入参数   : unCommPortIndex：通信端口 unRxLength：数据帧长度(去掉帧头帧尾)
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：

//==============================================================================
void TakeFrame_LED(unsigned int unCommPortIndex, unsigned int unRxLength)
{
    unsigned int unCounter = 0;
    /**10 CC AA cs 16***/
    /**68 LL LL 68 CC AA DD    cs 16***/
     if(g_sRxBuff[0].m_gBuf[g_uc101BuffHead] == 0x68) //启动字符为68H
    {
        //g_unFrameFlag_101 = NUM_68H;
        for(unCounter = 0;unCounter < unRxLength ;unCounter++)
            g_unRX_Buff[unCommPortIndex][unCounter] = g_sRxBuff[0].m_gBuf[BuffPositionLED(g_uc101BuffHead + unCounter + 4)];
        
        g_sRxBuff[0].m_iBufH=BuffPositionLED(g_uc101BuffHead + unRxLength + 6);// 调整缓冲区数据头指针位置
    }
    g_uc101BuffHead =  g_sRxBuff[0].m_iBufH;

}
//==============================================================================
//  函数名称   : DetectFrame
//  功能描述   : 侦测新的数据帧
//  输入参数   : unCommPortIndex：通信端口 
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void DetectFrame_LED(unsigned int unCommPortIndex)
{//0 
    unsigned int unCurrentPointer;
    unsigned int unRxLength,unRxADD;  
    unsigned int unFramLen = 0;
   // unsigned int unAddFlag;
  //  unsigned int Addr = 0;

    do
    {//1
        if(BuffCapacity_LED()>=5) //数据帧字节合法
        {//2  
          /**10 CC AA cs 16***/
          /**10 5b 01 5c 16***/ 
          /**68 LL LL 68 CC AA DD    cs 16***/
      /**68 04 04 68 00 99 88 77 98 16***/
            if ((g_sRxBuff[0].m_gBuf[g_uc101BuffHead] == 0x68 && g_sRxBuff[0].m_gBuf[g_uc101BuffHead] == g_sRxBuff[0].m_gBuf[BuffPositionLED(g_uc101BuffHead + 3)]) )  //启动字符68H
                  //端口符合 || unCommPortIndex == COMM_INDEX_232
            {//3 
                if (g_sRxBuff[0].m_gBuf[g_uc101BuffHead] == 0x68)
                {
                    //unAddFlag = 4;
                    unFramLen = g_sRxBuff[0].m_gBuf[BuffPositionLED(g_uc101BuffHead + 1)] + 6;
                }
                
                
                if( BuffCapacity_LED() >= unFramLen)//长度满足要求且地址正确    && (Addr == g_gRunParaJdg[RP_COMM_ADDR])
                {//4.2   
                    unRxLength = unFramLen - 6; //去掉帧头帧尾的长度
                    unCurrentPointer = g_uc101BuffHead + unFramLen - 2;
                    unRxADD = g_sRxBuff[0].m_gBuf[BuffPositionLED(unCurrentPointer)] ;
                    if (unRxADD == AddComChar(unCommPortIndex,unRxLength,0))//校验
                    {//5
                        TakeFrame_LED(unCommPortIndex,unRxLength);  
                        bFrameReceived[unCommPortIndex] = YES;  // set flag: frame received
                        //在TakeFrame0函数中，已经将nBuffHead指到下一帧数据的第一个字节。考虑到
                        //下面代码会有将nBuffHead向后移1位的操作，因此在此需要将nBuffHead的值减1，
                        //在下次循环寻找帧头时，保证nBuffHead指向的是第一个最新数据。                       
                        if(g_uc101BuffHead == 0)
                        {
                            g_uc101BuffHead = 128 - 1;
                        }
                        else
                        {
                            g_uc101BuffHead--;
                        }                           
                    }//5
                }//4.2          
                else//长度不满足要求或子站地址不符合
                {//4.3              
                    if((BuffCapacity_LED() <= unFramLen))//长度不满足 等待后续报文 直接退出
                    {
                        g_sRxBuff[0].m_iBufH = g_uc101BuffHead;
                        return;
                    }
                    else //长度满足而地址错误
                    {
                        ;// increase buff head, continue detecting
                    }
                   
                }//4.3      
            }//3            
        } //2
        else//长度不满足4字节，等待后续报文 直接退出   
        { //2
            g_sRxBuff[0].m_iBufH = g_uc101BuffHead; 
            return;
        } //2
        /*下行代码中将g_uc101BuffTail-1的原因是，当g_uc101BuffHead=127，
        若此时刚好g_uc101BuffTail=128，那么g_uc101BuffHead向后移一位会到buff头，就会变成死循环*/
        if(g_uc101BuffHead != (g_uc101BuffTail-1)) // increase buff head, continue detecting
            g_uc101BuffHead = BuffPositionLED(g_uc101BuffHead+1);   
        
    } while(1);   //1

}

//==============================================================================
//  函数名称   : operate06
//  功能描述   : 功能码06处理函数  遥控重发命令
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate06(unsigned int unCommPortIndex)
{
    if(g_unRX_Buff[unCommPortIndex][1]==0)       //重发
    {
        eight_select=0x8F; //eight_select|=BIT7;                    ////////////置首次发生8脉冲的标志		
        Sign_Repeat(0,0);
    }
    else                    ////////模拟故障
    {
        eight_select=0x8F; //eight_select|=BIT7;                    ////////////置首次发生8脉冲的标志		
        Sign_Repeat(g_unRX_Buff[unCommPortIndex][1],0);
    }
    return 1;
}
//==============================================================================
//  函数名称   : operate02
//  功能描述   : 功能码02处理函数  
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate02(unsigned int unCommPortIndex)
{
    unsigned char i;
    //unsigned char ucSOENUM;
    //unsigned char ucSOEPOS;
    unsigned char unTemp[10];
    if(g_unRX_Buff[unCommPortIndex][1]==0xff)        //////召唤故障总条数
    {
        g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
	g_sTxBuff[0].m_gBuffer[1]=0x0c;
	g_sTxBuff[0].m_gBuffer[2]=0x0c;
	g_sTxBuff[0].m_gBuffer[3]=0x68;
	g_sTxBuff[0].m_gBuffer[4]=0x02;
	g_sTxBuff[0].m_gBuffer[5]=0xff;
	g_sTxBuff[0].m_gBuffer[6]= g_sSoeData.m_ucSoeNum;
	for(i=7;i<=16;i++) 
            g_sTxBuff[0].m_gBuffer[i]=0;
	for(i=4;i<16;i++) 
            g_sTxBuff[0].m_gBuffer[16]+=g_sTxBuff[0].m_gBuffer[i];
        g_sTxBuff[0].m_gBuffer[17]=0x16;
    } 		  
    else   //召唤第几条
    {
 	 //CAT_SpiReadBytes(EEPADD_SOE_NUM , 2, unTemp);	
        i = g_unRX_Buff[unCommPortIndex][1];
        if(g_sSoeData.m_ucSoePos >= i)
            i =  g_sSoeData.m_ucSoePos - i;
        else
            i = 30 + g_sSoeData.m_ucSoePos - i;
        CAT_SpiReadBytes(EEPADD_SOE_DATA + i * 12 , 10, unTemp);
        
	g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
	g_sTxBuff[0].m_gBuffer[1]=0x0c;
	g_sTxBuff[0].m_gBuffer[2]=0x0c;
	g_sTxBuff[0].m_gBuffer[3]=0x68;
	g_sTxBuff[0].m_gBuffer[4]=0x02;
	g_sTxBuff[0].m_gBuffer[5]=g_unRX_Buff[unCommPortIndex][1];
	g_sTxBuff[0].m_gBuffer[6]=unTemp[0];
	g_sTxBuff[0].m_gBuffer[7]=unTemp[1];
	g_sTxBuff[0].m_gBuffer[8]=unTemp[2];
	g_sTxBuff[0].m_gBuffer[9]=unTemp[3];
	g_sTxBuff[0].m_gBuffer[10]=unTemp[4];
	g_sTxBuff[0].m_gBuffer[11]=unTemp[5];
	g_sTxBuff[0].m_gBuffer[12]=unTemp[6];
	g_sTxBuff[0].m_gBuffer[13]=unTemp[7];
	g_sTxBuff[0].m_gBuffer[14]=unTemp[8];
	g_sTxBuff[0].m_gBuffer[15]=unTemp[9];
	g_sTxBuff[0].m_gBuffer[16]=0;
	for(i=4;i<16;i++) 
            g_sTxBuff[0].m_gBuffer[16]+=g_sTxBuff[0].m_gBuffer[i];
	 g_sTxBuff[0].m_gBuffer[17]=0x16; 
			
    }
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,18);
    return 1;
}
//==============================================================================
//  函数名称   : operate07
//  功能描述   : 功能码07处理函数  修改参数命令
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate07(unsigned int unCommPortIndex)
{
    unsigned char i;
    g_gProcCnt[PC_HIGH_P]=g_unRX_Buff[unCommPortIndex][1];     ////////相高电压
    if((g_gProcCnt[PC_HIGH_P]>99)||(g_gProcCnt[PC_HIGH_P]<10))
        g_gProcCnt[PC_HIGH_P]=70;
    
    g_gProcCnt[PC_LOW_P]=g_unRX_Buff[unCommPortIndex][2];
    if((g_gProcCnt[PC_LOW_P]>99)||(g_gProcCnt[PC_LOW_P]<10))
        g_gProcCnt[PC_LOW_P]=51;
    
    g_gProcCnt[PC_HIGH_Z]=g_unRX_Buff[unCommPortIndex][3];
    if((g_gProcCnt[PC_HIGH_Z]>99)||(g_gProcCnt[PC_HIGH_Z]<10))
        g_gProcCnt[PC_HIGH_Z]=25;
    
    g_gProcCnt[PC_LOW_Z]=g_unRX_Buff[unCommPortIndex][4];
    if((g_gProcCnt[PC_LOW_Z]>99)||(g_gProcCnt[PC_LOW_Z]<3))
        g_gProcCnt[PC_LOW_Z]=10;
    
    g_gProcCnt[PC_I0_START]=g_unRX_Buff[unCommPortIndex][5]*13;    /////零序电流门槛值 0.5A/50A*0.8*4095/3.3*1.414*0.9*     
    if((g_gProcCnt[PC_I0_START]>260)||(g_gProcCnt[PC_I0_START]<13))
        g_gProcCnt[PC_I0_START]=26;
    
    g_gProcCnt[PC_T_DELAY]=g_unRX_Buff[unCommPortIndex][6];                    //////故障延时
    if((g_gProcCnt[PC_T_DELAY]>60)||(g_gProcCnt[PC_T_DELAY]<4))
        g_gProcCnt[PC_T_DELAY]=10;
    
    g_gProcCnt[PC_NO_V]=g_unRX_Buff[unCommPortIndex][7];	
    if((g_gProcCnt[PC_NO_V]>30)||(g_gProcCnt[PC_NO_V]<3)) 
        g_gProcCnt[PC_NO_V]=10;
    
    g_gProcCnt[PC_PULSE_VALID]=g_unRX_Buff[unCommPortIndex][8];		
    if((g_gProcCnt[PC_PULSE_VALID]>15)||(g_gProcCnt[PC_PULSE_VALID]<1)) 
        g_gProcCnt[PC_PULSE_VALID]=1;
			
    if(g_gProcCnt[PC_LOW_P]>=g_gProcCnt[PC_HIGH_P])
    {
        g_gProcCnt[PC_HIGH_P]=70;
 	g_gProcCnt[PC_LOW_P]=51;
    }	  	
    if(g_gProcCnt[PC_LOW_Z]>=g_gProcCnt[PC_HIGH_Z])
    {
        g_gProcCnt[PC_HIGH_Z]=25;
        g_gProcCnt[PC_LOW_Z]=10;
    }
    g_gChangFlag[CHNG_PC] = ON;	  //修改定值参数	
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x09;
    g_sTxBuff[0].m_gBuffer[2]=0x09;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x07;
				
    g_sTxBuff[0].m_gBuffer[5]=g_gProcCnt[PC_HIGH_P];
    g_sTxBuff[0].m_gBuffer[6]=g_gProcCnt[PC_LOW_P];
    g_sTxBuff[0].m_gBuffer[7]=g_gProcCnt[PC_HIGH_Z];
    g_sTxBuff[0].m_gBuffer[8]=g_gProcCnt[PC_LOW_Z];
    g_sTxBuff[0].m_gBuffer[9]=g_gProcCnt[PC_I0_START];
    g_sTxBuff[0].m_gBuffer[10]=g_gProcCnt[PC_T_DELAY];
    g_sTxBuff[0].m_gBuffer[11]=g_gProcCnt[PC_NO_V];
    g_sTxBuff[0].m_gBuffer[12]=g_gProcCnt[PC_PULSE_VALID];
    g_sTxBuff[0].m_gBuffer[13]=0;
//#ifdef YN_101S
	g_gProcCnt[PC_HIGH_P]=g_gProcCnt[PC_HIGH_P]*100;
	g_gProcCnt[PC_LOW_P]=g_gProcCnt[PC_LOW_P]*100;
	g_gProcCnt[PC_HIGH_Z]=g_gProcCnt[PC_HIGH_Z]*100;
	g_gProcCnt[PC_LOW_Z]=g_gProcCnt[PC_LOW_Z]*100;
	g_gProcCnt[PC_I0_START]=g_gProcCnt[PC_I0_START]*1000;
	g_gProcCnt[PC_NO_V]=g_gProcCnt[PC_NO_V]*100;
	g_gProcCnt[PC_T_DELAY]=g_gProcCnt[PC_T_DELAY]*1000;
	g_gProcCnt[PC_PULSE_VALID]=g_gProcCnt[PC_PULSE_VALID]*1000;
//#endif   
    for(i=4;i<13;i++)
        g_sTxBuff[0].m_gBuffer[13]+=g_sTxBuff[0].m_gBuffer[i];
    g_sTxBuff[0].m_gBuffer[14]=0x16;
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,15);	
          return 1;
}
//==============================================================================
//  函数名称   : operate08
//  功能描述   : 功能码08处理函数 修改判据命令
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate08(unsigned int unCommPortIndex)
{
    unsigned char i;
    g_gProcCnt[PC_JAG_P]    = g_unRX_Buff[unCommPortIndex][1];
    g_gProcCnt[PC_JAG_Z]    = g_unRX_Buff[unCommPortIndex][2];
    g_gProcCnt[PC_EFS_MODEL]= g_unRX_Buff[unCommPortIndex][3];
	
    if(g_gProcCnt[PC_EFS_MODEL]>1)       ///////0X01 II型信号源   0X00  I型信号源
        g_gProcCnt[PC_EFS_MODEL]=0x00;	 
    if((g_gProcCnt[PC_JAG_P]!=0)&&(g_gProcCnt[PC_JAG_P]!=0x55))           ///////0X55 相判据启动   0X00  不启动
	g_gProcCnt[PC_JAG_P]=0x00;	 	 
    if((g_gProcCnt[PC_JAG_Z]!=0)&&(g_gProcCnt[PC_JAG_Z]!=0x55))             ///////0X55 零序判据启动   0X00  不启动
	g_gProcCnt[PC_JAG_Z]=0x55; 

   // if((g_gProcCnt[PC_JAG_Z]+g_gProcCnt[PC_JAG_P])==0)
	//g_gProcCnt[PC_JAG_Z]=0x55;
    g_gProcCnt[PC_GPRS_MODEL] = g_unRX_Buff[unCommPortIndex][4];
    if((g_gProcCnt[PC_GPRS_MODEL]!=0)&&(g_gProcCnt[PC_GPRS_MODEL]!=0x55))           ///////0X00 GPRS   0X55  SMS
        g_gProcCnt[PC_GPRS_MODEL]=0x00;	
	    
            
    g_gChangFlag[CHNG_PC] = ON;	  //修改定值参数	
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x05;
    g_sTxBuff[0].m_gBuffer[2]=0x05;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x08;
				
    g_sTxBuff[0].m_gBuffer[5]=g_gProcCnt[PC_JAG_P];
    g_sTxBuff[0].m_gBuffer[6]=g_gProcCnt[PC_JAG_Z];
    g_sTxBuff[0].m_gBuffer[7]=g_gProcCnt[PC_EFS_MODEL];
    g_sTxBuff[0].m_gBuffer[8]=g_gProcCnt[PC_GPRS_MODEL];
		
    g_sTxBuff[0].m_gBuffer[9]=0;
    for(i=4;i<9;i++)
        g_sTxBuff[0].m_gBuffer[9]+=g_sTxBuff[0].m_gBuffer[i];
    g_sTxBuff[0].m_gBuffer[10]=0x16;
			
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,11);
          return 1;
}
//==============================================================================
//  函数名称   : operate09
//  功能描述   : 功能码09处理函数  时间设置命令
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate09(unsigned int unCommPortIndex)
{
    unsigned char i;
    unsigned char ucTime[7];
    
    g_sRtcManager.m_gRealTimer[RTC_YEAR]=g_unRX_Buff[unCommPortIndex][1];  	  //year_get;
	 
    g_sRtcManager.m_gRealTimer[RTC_MONTH]=g_unRX_Buff[unCommPortIndex][2];      //month_get;
    g_sRtcManager.m_gRealTimer[RTC_DATE]=g_unRX_Buff[unCommPortIndex][3];      //day_get;
    g_sRtcManager.m_gRealTimer[RTC_HOUR]=g_unRX_Buff[unCommPortIndex][4];      //hour_get;
    g_sRtcManager.m_gRealTimer[RTC_MINUT]=g_unRX_Buff[unCommPortIndex][5];      //minute_get;
    g_sRtcManager.m_gRealTimer[RTC_SEC]=g_unRX_Buff[unCommPortIndex][6];
    
  //  g_sRtcManager.m_ucCounter125 = 0;
  //  g_sRtcManager.m_ucCounter10 = 0;
    
              
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x07;
    g_sTxBuff[0].m_gBuffer[2]=0x07;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x09;
    
    g_sTxBuff[0].m_gBuffer[5]= g_sRtcManager.m_gRealTimer[RTC_YEAR];      //////year
    g_sTxBuff[0].m_gBuffer[6]=g_sRtcManager.m_gRealTimer[RTC_MONTH];      //////month
    g_sTxBuff[0].m_gBuffer[7]=g_sRtcManager.m_gRealTimer[RTC_DATE];      //////day
    g_sTxBuff[0].m_gBuffer[8]=g_sRtcManager.m_gRealTimer[RTC_HOUR];      //////hour
    g_sTxBuff[0].m_gBuffer[9]=g_sRtcManager.m_gRealTimer[RTC_MINUT];      //////monute
    g_sTxBuff[0].m_gBuffer[10]=g_sRtcManager.m_gRealTimer[RTC_SEC];      //////second	
    g_sTxBuff[0].m_gBuffer[11]=0;
    for(i=4;i<11;i++) 
        g_sTxBuff[0].m_gBuffer[11]+=g_sTxBuff[0].m_gBuffer[i];
    g_sTxBuff[0].m_gBuffer[12]=0x16;
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,13);
    
    for(i = 0; i< 6; i++)
        ucTime[i] = (unsigned char)g_sRtcManager.m_gRealTimer[RTC_YEAR + i];
    //////////设置时间
    g_sRtcManager.m_gRealTimer[RTC_YEAR] +=  2000;       
    DS1302_Reset();                 //////////时钟芯片复位			  				  
    DS1302_SettingData(ucTime); 
    
        return 1;
}
//==============================================================================
//  函数名称   : operate0A
//  功能描述   : 功能码0A处理函数   液晶读参数
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate0A(unsigned int unCommPortIndex)
{
    unsigned char i;
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x09;
    g_sTxBuff[0].m_gBuffer[2]=0x09;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x0a;
//#ifdef YN_101S
    g_sTxBuff[0].m_gBuffer[5]=g_gProcCnt[PC_HIGH_P]/100;  //temp8[12]
    g_sTxBuff[0].m_gBuffer[6]=g_gProcCnt[PC_LOW_P]/100; //temp8[13]
    g_sTxBuff[0].m_gBuffer[7]=g_gProcCnt[PC_HIGH_Z]/100; //temp8[14]
    g_sTxBuff[0].m_gBuffer[8]=g_gProcCnt[PC_LOW_Z]/100;  //temp8[15]
    g_sTxBuff[0].m_gBuffer[9]=g_gProcCnt[PC_I0_START]/1000;
    g_sTxBuff[0].m_gBuffer[10]=g_gProcCnt[PC_T_DELAY]/1000; //temp8[17]
    g_sTxBuff[0].m_gBuffer[11]=g_gProcCnt[PC_NO_V]/100; //temp8[16]
    g_sTxBuff[0].m_gBuffer[12]=g_gProcCnt[PC_PULSE_VALID]/1000; //temp8[22]
/*
#else
    g_sTxBuff[0].m_gBuffer[5]=g_gProcCnt[PC_HIGH_P];  //temp8[12]
    g_sTxBuff[0].m_gBuffer[6]=g_gProcCnt[PC_LOW_P]; //temp8[13]
    g_sTxBuff[0].m_gBuffer[7]=g_gProcCnt[PC_HIGH_Z]; //temp8[14]
    g_sTxBuff[0].m_gBuffer[8]=g_gProcCnt[PC_LOW_Z];  //temp8[15]
    g_sTxBuff[0].m_gBuffer[9]=g_gProcCnt[PC_I0_START]/13;
    g_sTxBuff[0].m_gBuffer[10]=g_gProcCnt[PC_T_DELAY]; //temp8[17]
    g_sTxBuff[0].m_gBuffer[11]=g_gProcCnt[PC_NO_V]; //temp8[16]
    g_sTxBuff[0].m_gBuffer[12]=g_gProcCnt[PC_PULSE_VALID]; //temp8[22]
#endif  
*/
    g_sTxBuff[0].m_gBuffer[13]=0;
    for(i=4;i<13;i++) 
        g_sTxBuff[0].m_gBuffer[13]+=g_sTxBuff[0].m_gBuffer[i];
    g_sTxBuff[0].m_gBuffer[14]=0x16;
		//lcd_ctl=3; 
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,15);
         return 1;
}
//==============================================================================
//  函数名称   : operate0B
//  功能描述   : 功能码0B处理函数  液晶读判据
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate0B(unsigned int unCommPortIndex)
{
    unsigned char i;
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x05;
    g_sTxBuff[0].m_gBuffer[2]=0x05;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x0B;
    g_sTxBuff[0].m_gBuffer[5]=g_gProcCnt[PC_JAG_P];  //temp8[18]
    g_sTxBuff[0].m_gBuffer[6]=g_gProcCnt[PC_JAG_Z];   //temp8[19]
    g_sTxBuff[0].m_gBuffer[7]=g_gProcCnt[PC_EFS_MODEL]; //temp8[20]
    g_sTxBuff[0].m_gBuffer[8]=g_gProcCnt[PC_GPRS_MODEL];     //temp8[23]
    g_sTxBuff[0].m_gBuffer[9]=0;
    for(i=4;i<9;i++) 
        g_sTxBuff[0].m_gBuffer[9]+=g_sTxBuff[0].m_gBuffer[i];
    g_sTxBuff[0].m_gBuffer[10]=0x16;
		//lcd_ctl=3; 
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,11);
                return 1;
}
//==============================================================================
//  函数名称   : operate0C
//  功能描述   : 功能码0C处理函数   液晶读时间
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate0C(unsigned int unCommPortIndex)
{
    unsigned char i;
   // DS1302_GetData(rtc);                    //读时间 
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x07;
    g_sTxBuff[0].m_gBuffer[2]=0x07;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x0c;
    g_sTxBuff[0].m_gBuffer[5]= g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;      //////year
    g_sTxBuff[0].m_gBuffer[6]=g_sRtcManager.m_gRealTimer[RTC_MONTH];      //////month
    g_sTxBuff[0].m_gBuffer[7]=g_sRtcManager.m_gRealTimer[RTC_DATE];      //////day
    g_sTxBuff[0].m_gBuffer[8]=g_sRtcManager.m_gRealTimer[RTC_HOUR];      //////hour
    g_sTxBuff[0].m_gBuffer[9]=g_sRtcManager.m_gRealTimer[RTC_MINUT];      //////monute
    g_sTxBuff[0].m_gBuffer[10]=g_sRtcManager.m_gRealTimer[RTC_SEC];      //////second	
    g_sTxBuff[0].m_gBuffer[11]=0;
    for(i=4;i<11;i++) 
        g_sTxBuff[0].m_gBuffer[11]+=g_sTxBuff[0].m_gBuffer[i];
    g_sTxBuff[0].m_gBuffer[12]=0x16;
                
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,13);
        return 1;
     
}
//==============================================================================
//  函数名称   : operate0D
//  功能描述   : 功能码0D处理函数   读地址和版本号
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate0D(unsigned int unCommPortIndex)
{
    //unsigned char i;
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x05;
    g_sTxBuff[0].m_gBuffer[2]=0x05;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x0d;
    g_sTxBuff[0].m_gBuffer[5]=0;
    g_sTxBuff[0].m_gBuffer[6]=VerNum_INLCD;
    g_sTxBuff[0].m_gBuffer[8]=(unsigned char)(g_gRunPara[RP_COMM_ADDR]>>8);//g_gRunPara[RP_COMM_ADDR]
    g_sTxBuff[0].m_gBuffer[7]=(unsigned char)g_gRunPara[RP_COMM_ADDR];//
    g_sTxBuff[0].m_gBuffer[9]=g_sTxBuff[0].m_gBuffer[5]+g_sTxBuff[0].m_gBuffer[6]+g_sTxBuff[0].m_gBuffer[7]+g_sTxBuff[0].m_gBuffer[8]+0x0d;	 
    g_sTxBuff[0].m_gBuffer[10]=0x16;
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,11);
                return 1;
}
//==============================================================================
//  函数名称   : operate12
//  功能描述   : 功能码12处理函数   设置动作相序
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate12(unsigned int unCommPortIndex)
{
    unsigned char i;
    g_gRunPara[RP_PLUSE_TIME]=(unsigned int)(g_unRX_Buff[unCommPortIndex][2] << 8) + g_unRX_Buff[unCommPortIndex][1]; 
    g_gRunPara[RP_PLUSE_TIME] = g_gRunPara[RP_PLUSE_TIME]/10;
    g_gRunPara[RP_SEND_TIME1]=(unsigned int)(g_unRX_Buff[unCommPortIndex][4] << 8) + g_unRX_Buff[unCommPortIndex][3];
    g_gRunPara[RP_SEND_TIME2]=(unsigned int)(g_unRX_Buff[unCommPortIndex][6] << 8) + g_unRX_Buff[unCommPortIndex][5];
    g_gRunPara[RP_JAG_ACT]=(unsigned int)(g_unRX_Buff[unCommPortIndex][8] << 8) + g_unRX_Buff[unCommPortIndex][7];
    g_gRunPara[RP_LACTH_TIME]=(unsigned int)(g_unRX_Buff[unCommPortIndex][10] << 8) + g_unRX_Buff[unCommPortIndex][9];

  /*  g_gProcCnt[PC_JAG_ACT]=g_unRX_Buff[unCommPortIndex][1];
    if((g_gProcCnt[PC_JAG_ACT] ==0)||(g_gProcCnt[PC_JAG_ACT] >= 5))
        g_gProcCnt[PC_JAG_ACT]=0x03;
    
    g_gProcCnt[PC_REV_CURRENT]=g_unRX_Buff[unCommPortIndex][2];    
    if((g_gProcCnt[PC_REV_CURRENT]>35)||(g_gProcCnt[PC_REV_CURRENT]<10))
        g_gProcCnt[PC_REV_CURRENT]=20;
    
    g_gProcCnt[PC_U0_CAL] = g_unRX_Buff[unCommPortIndex][3];
    if((g_gProcCnt[PC_U0_CAL]!=0)&&(g_gProcCnt[PC_U0_CAL]!=0x55))
        g_gProcCnt[PC_U0_CAL]=0;*/
		
            				
    g_ucParaChang |= BIT0;	  //修改定值参数
                
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x0b;
    g_sTxBuff[0].m_gBuffer[2]=0x0b;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x12;
    g_sTxBuff[0].m_gBuffer[5]=(unsigned char)g_gRunPara[RP_PLUSE_TIME];
    g_sTxBuff[0].m_gBuffer[6]=(unsigned char)(g_gRunPara[RP_PLUSE_TIME]>>8);
    
    g_sTxBuff[0].m_gBuffer[7]=(unsigned char)g_gRunPara[RP_SEND_TIME1];//
    g_sTxBuff[0].m_gBuffer[8]=(unsigned char)(g_gRunPara[RP_SEND_TIME1]>>8);//g_gRunPara[RP_COMM_ADDR]

    g_sTxBuff[0].m_gBuffer[9]=(unsigned char)g_gRunPara[RP_SEND_TIME2];
    g_sTxBuff[0].m_gBuffer[10]=(unsigned char)(g_gRunPara[RP_SEND_TIME2]>>8);
    
    g_sTxBuff[0].m_gBuffer[11]=(unsigned char)g_gRunPara[RP_JAG_ACT];//
    g_sTxBuff[0].m_gBuffer[12]=(unsigned char)(g_gRunPara[RP_JAG_ACT]>>8);//g_gRunPara[RP_COMM_ADDR]
    
    g_sTxBuff[0].m_gBuffer[13]=(unsigned char)g_gRunPara[RP_LACTH_TIME];
    g_sTxBuff[0].m_gBuffer[14]=(unsigned char)(g_gRunPara[RP_LACTH_TIME]>>8);
    
    g_sTxBuff[0].m_gBuffer[15]=0;
    for(i=4;i<15;i++)	
        g_sTxBuff[0].m_gBuffer[15]+=g_sTxBuff[0].m_gBuffer[i];
    
    g_sTxBuff[0].m_gBuffer[16]=0x16;
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,17);
        return 1;
}
//==============================================================================
//  函数名称   : operate11
//  功能描述   : 功能码11处理函数   读取动作相序
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate11(unsigned int unCommPortIndex)
{
    unsigned int unTemp = 0;
    unsigned char i;
    
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x0b;
    g_sTxBuff[0].m_gBuffer[2]=0x0b;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x11;
    unTemp = g_gRunPara[RP_PLUSE_TIME] * 10;
    g_sTxBuff[0].m_gBuffer[5]=(unsigned char)unTemp;
    g_sTxBuff[0].m_gBuffer[6]=(unsigned char)(unTemp>>8);
    
    g_sTxBuff[0].m_gBuffer[7]=(unsigned char)g_gRunPara[RP_SEND_TIME1];//
    g_sTxBuff[0].m_gBuffer[8]=(unsigned char)(g_gRunPara[RP_SEND_TIME1]>>8);//g_gRunPara[RP_COMM_ADDR]

    g_sTxBuff[0].m_gBuffer[9]=(unsigned char)g_gRunPara[RP_SEND_TIME2];
    g_sTxBuff[0].m_gBuffer[10]=(unsigned char)(g_gRunPara[RP_SEND_TIME2]>>8);
    
    g_sTxBuff[0].m_gBuffer[11]=(unsigned char)g_gRunPara[RP_JAG_ACT];//
    g_sTxBuff[0].m_gBuffer[12]=(unsigned char)(g_gRunPara[RP_JAG_ACT]>>8);//g_gRunPara[RP_COMM_ADDR]
    
    g_sTxBuff[0].m_gBuffer[13]=(unsigned char)g_gRunPara[RP_LACTH_TIME];
    g_sTxBuff[0].m_gBuffer[14]=(unsigned char)(g_gRunPara[RP_LACTH_TIME]>>8);
    
    g_sTxBuff[0].m_gBuffer[15]=0;
    for(i=4;i<15;i++)	
        g_sTxBuff[0].m_gBuffer[15]+=g_sTxBuff[0].m_gBuffer[i];
    
    g_sTxBuff[0].m_gBuffer[16]=0x16;
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,17);
                return 1;
}
//==============================================================================
//  函数名称   : operate17
//  功能描述   : 功能码17处理函数    读地址
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate17(unsigned int unCommPortIndex)
{
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x03;
    g_sTxBuff[0].m_gBuffer[2]=0x03;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x17;
    g_sTxBuff[0].m_gBuffer[6]=(unsigned char)(g_gRunPara[RP_COMM_ADDR]>>8);///////0X55 滞后   0X00  超前
    g_sTxBuff[0].m_gBuffer[5]=(unsigned char)(g_gRunPara[RP_COMM_ADDR]);//RdCnt(0xF215);///////0X55 滞后   0X00  超前
    g_sTxBuff[0].m_gBuffer[7]=g_sTxBuff[0].m_gBuffer[4]+g_sTxBuff[0].m_gBuffer[5]+g_sTxBuff[0].m_gBuffer[6];
    g_sTxBuff[0].m_gBuffer[8]=0x16;
		//lcd_ctl=3; 
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,9);	
         return 1;
}
//==============================================================================
//  函数名称   : operate14
//  功能描述   : 功能码14处理函数   修改地址
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate14(unsigned int unCommPortIndex)
{
    if(((g_unRX_Buff[unCommPortIndex][1]==0)&&(g_unRX_Buff[unCommPortIndex][2]==0))||((g_unRX_Buff[unCommPortIndex][1]==0xFF)&&(g_unRX_Buff[unCommPortIndex][2]==0xFF)))
    {
	g_unRX_Buff[unCommPortIndex][2]=(unsigned char)(g_gRunPara[RP_COMM_ADDR]>>8);///////0X55 滞后   0X00  超前
	g_unRX_Buff[unCommPortIndex][1]=(unsigned char)(g_gRunPara[RP_COMM_ADDR]);//RdCnt(0xF215);///////0X55 滞后   0X00  超前 
    }
    else
    {
        g_gRunPara[RP_COMM_ADDR]=(unsigned int)(g_unRX_Buff[unCommPortIndex][2] << 8) + g_unRX_Buff[unCommPortIndex][1];     
        g_ucParaChang |= BIT0;    //站名称、站地址修改啊      
    } 	 	 	
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x03;
    g_sTxBuff[0].m_gBuffer[2]=0x03;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x14;
    g_sTxBuff[0].m_gBuffer[6]=(unsigned char)(g_gRunPara[RP_COMM_ADDR]>>8);///////0X55 滞后   0X00  超前
    g_sTxBuff[0].m_gBuffer[5]=(unsigned char)(g_gRunPara[RP_COMM_ADDR]);//RdCnt(0xF215);///////0X55 滞后   0X00  超前
    g_sTxBuff[0].m_gBuffer[7]=g_sTxBuff[0].m_gBuffer[4]+g_sTxBuff[0].m_gBuffer[5]+g_sTxBuff[0].m_gBuffer[6];
    g_sTxBuff[0].m_gBuffer[8]=0x16;
		
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,9);
        return 1;
}
//==============================================================================
//  函数名称   : operate15
//  功能描述   : 功能码15处理函数  读手机号码与参数
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate15(unsigned int unCommPortIndex)
{
    unsigned char i;
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x0E;
    g_sTxBuff[0].m_gBuffer[2]=0x0E;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x15;
    g_sTxBuff[0].m_gBuffer[5]=g_unRX_Buff[unCommPortIndex][1];
    for(i=0;i<11;i++)
       // g_sTxBuff[0].m_gBuffer[i+6]=TEL_NUM[(g_sTxBuff[0].m_gBuffer[5]-1) * 11 + i];	
       g_sTxBuff[0].m_gBuffer[i+6]=g_gSmsPhone[(g_sTxBuff[0].m_gBuffer[5]-1) * 12 + i+1];
	
    g_sTxBuff[0].m_gBuffer[17]= g_gSmsPhone[g_sTxBuff[0].m_gBuffer[5]-1+48];
    
    g_sTxBuff[0].m_gBuffer[18]=0;			          				              
    for(i=4;i<18;i++)	
        g_sTxBuff[0].m_gBuffer[18]+=g_sTxBuff[0].m_gBuffer[i];
    g_sTxBuff[0].m_gBuffer[19]=0x16;
		//lcd_ctl=3; 
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,20);	
         return 1;
}	
//==============================================================================
//  函数名称   : operate16
//  功能描述   : 功能码16处理函数   修改手机号码和参数
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate16(unsigned int unCommPortIndex)
{ 
    unsigned char i;	
    
    for(i=0;i<11;i++)
        //TEL_NUM[(g_unRX_Buff[unCommPortIndex][1]-1) * 11 + i] = g_unRX_Buff[unCommPortIndex][2 + i];
	g_gSmsPhone[(g_unRX_Buff[unCommPortIndex][1]-1) * 12 + i+1] = g_unRX_Buff[unCommPortIndex][2 + i];
	
    //g_ucphone_perm[g_unRX_Buff[unCommPortIndex][1]-1]=  g_unRX_Buff[unCommPortIndex][13]; 
    g_gSmsPhone[g_unRX_Buff[unCommPortIndex][1]-1+48]=  g_unRX_Buff[unCommPortIndex][13]; 
    //g_ucphone_perm[4] = (g_ucphone_perm[0] + g_ucphone_perm[1] + g_ucphone_perm[2] + g_ucphone_perm[3]) % 256;
               
    g_gChangFlag[CHNG_TEL] = ON;       //电话号码发生修改
   
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x0E;
    g_sTxBuff[0].m_gBuffer[2]=0x0E;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x16;
    for(i=5;i<17;i++)
        g_sTxBuff[0].m_gBuffer[i]=g_unRX_Buff[unCommPortIndex][i - 4];	
    g_sTxBuff[0].m_gBuffer[18]=0;			          				              
    for(i=4;i<18;i++)	
        g_sTxBuff[0].m_gBuffer[18]+=g_sTxBuff[0].m_gBuffer[i];
    g_sTxBuff[0].m_gBuffer[19]=0x16;
 
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,20);
         return 1;
}
//==============================================================================
//  函数名称   : operate18
//  功能描述   : 功能码18处理函数    读主动上传间隔
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate18(unsigned int unCommPortIndex)
{    	
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x02;
    g_sTxBuff[0].m_gBuffer[2]=0x02;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x18;
    g_sTxBuff[0].m_gBuffer[5]=g_ucPara_stime;///////0X55 滞后   0X00  超前
    g_sTxBuff[0].m_gBuffer[6]=g_sTxBuff[0].m_gBuffer[4]+g_sTxBuff[0].m_gBuffer[5];
    g_sTxBuff[0].m_gBuffer[7]=0x16;
		//lcd_ctl=3; 
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,8);
          return 1;
}
//==============================================================================
//  函数名称   : operate19
//  功能描述   : 功能码19处理函数   修改主动上传间隔
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate19(unsigned int unCommPortIndex)
{
    g_ucPara_stime = g_unRX_Buff[unCommPortIndex][1];
    if((g_ucPara_stime>48)||(g_ucPara_stime<2))              ////2011-12-13 15:48:08   2-240小时
        g_ucPara_stime = 24;
    g_gChangFlag[CHNG_TEL] = ON;   //修改电话参数
	
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x02;
    g_sTxBuff[0].m_gBuffer[2]=0x02;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x19;
    g_sTxBuff[0].m_gBuffer[5]= g_ucPara_stime;///////0X55 滞后   0X00  超前
    g_sTxBuff[0].m_gBuffer[6]=g_sTxBuff[0].m_gBuffer[4]+g_sTxBuff[0].m_gBuffer[5];
    g_sTxBuff[0].m_gBuffer[7]=0x16;
		//lcd_ctl=3; 
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,8);	
         return 1;
}
//==============================================================================
//  函数名称   : operate19
//  功能描述   : 功能码19处理函数   校准
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operate20(unsigned int unCommPortIndex)
{
    unsigned char i,j;
    int Temp = 0;	
    unsigned int unTemp = 0;
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////信号发送中
    g_sTxBuff[0].m_gBuffer[1]=0x02;
    g_sTxBuff[0].m_gBuffer[2]=0x02;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x20;
    g_sTxBuff[0].m_gBuffer[5]= g_unRX_Buff[unCommPortIndex][1];///////0X55 滞后   0X00  超前
    g_sTxBuff[0].m_gBuffer[6]=g_sTxBuff[0].m_gBuffer[4]+g_sTxBuff[0].m_gBuffer[5];
    g_sTxBuff[0].m_gBuffer[7]=0x16;
    
    if(g_sTxBuff[0].m_gBuffer[5] == 1)
    {
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
    else if(g_sTxBuff[0].m_gBuffer[5] == 2)
    {
        for(j = 0; j < 3; j++)
        {  
            unTemp = g_gProcCnt[j];
            unTemp = (((unsigned long)6000* unTemp) /g_gRmtFilMeas[j + RM_UA]); //张弢 目标校准，上位机下载参数 初始值为电压60V,电流2A
            if(unTemp > 3000 && unTemp < 6000)
                g_gProcCnt[j] = unTemp;                
        }
            unTemp = g_gProcCnt[3];
            unTemp = (((unsigned long)6000 * unTemp) /g_gRmtFilMeas[RM_U0]); //张弢 目标校准，上位机下载参数 初始值为电压60V,电流2A
            if(unTemp > 3000 && unTemp < 6000)
                g_gProcCnt[3] = unTemp;
            unTemp = g_gProcCnt[4];
            unTemp = (((unsigned long)200 * unTemp) /g_gRmtFilMeas[RM_I0]); //张弢 目标校准，上位机下载参数 初始值为电压60V,电流2A
            if(unTemp > 3000 && unTemp < 6000)
                g_gProcCnt[4] = unTemp;
         for(i = 0; i <= 4; i++)
            g_gRunPara[i + RP_UA_ADJ] = g_gProcCnt[i];   
         g_gChangFlag[CHNG_MUBIAO] = ON;   
    }
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,8);	
         return 1;
         

}
//==============================================================================
//  函数名称   : operateFF
//  功能描述   : 功能码06处理函数  间隔上送遥信与遥测
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : 1：成功  0：数据有错误
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned int operateFF(unsigned int unCommPortIndex)
{ 
    unsigned char i;
    
    uart0_event_flag++;
    if(uart0_event_flag==4)
        uart0_event_flag=0;
    if(uart0_event_flag==1)    ////////////  发送状态量
    {
        g_sTxBuff[0].m_gBuffer[5]=0;
 	g_sTxBuff[0].m_gBuffer[6]=0;
 	 	  	 
 	if((phase_copy&BIT0)==BIT0)         ///////A相断线       
 	    g_sTxBuff[0].m_gBuffer[5]|=0x01;
 	if((phase_copy&BIT1)==BIT1)      ///////B相断线
 	    g_sTxBuff[0].m_gBuffer[5]|=0x02;
 	if((phase_copy&BIT2)==BIT2)     ///////C相断线
 	    g_sTxBuff[0].m_gBuffer[5]|=0x04;
    	 	     	 	            	 	 	
        if(eight_pulse_flag>0)    ///////信号发送中
            g_sTxBuff[0].m_gBuffer[5]|=0x08;  
 	if(efslatch_flag>0)          ///////闭锁
 	    g_sTxBuff[0].m_gBuffer[5]|=0x10;  
 	if((efslatch_flag==0)&&(eight_pulse_flag==0))         ///////解锁
 	    g_sTxBuff[0].m_gBuffer[5]|=0x20;
        if(pulse_success==0x55)                     ///////发出有效8脉冲
 	    g_sTxBuff[0].m_gBuffer[5]|=0x40;     	 	  
        if(g_gProcCnt[PC_EFS_MODEL]>0)           ///////////////II型或铁路型
        { 
            if(fault_save>0)         	 	
            {
            //i/////////A相单相接地故障	
                if(g_ucEarthFlg == 1)	
    	            g_sTxBuff[0].m_gBuffer[6]|=0x11; 	
    	      //////////B相单相接地故障	
    	        else if(g_ucEarthFlg == 2)
    	            g_sTxBuff[0].m_gBuffer[6]|=0x12; 	
    	      // /////////C相单相接地故障	
    	        else if(g_ucEarthFlg == 3)
    	            g_sTxBuff[0].m_gBuffer[6]|=0x14;	
    	     }
             else if(fault_save==0)	
              /////////复归   单相接地判据
                 g_sTxBuff[0].m_gBuffer[6]|=0x08;	
        } 
        else
        {
            if(fault_save>0)	
                g_sTxBuff[0].m_gBuffer[6]|=0x10; 	
    	    else if(fault_save==0)       /////////接地故障复归
                g_sTxBuff[0].m_gBuffer[6]|=0x08;   	 	
        }   	 	 	
        g_sTxBuff[0].m_gBuffer[0]=0x68;      
        g_sTxBuff[0].m_gBuffer[1]=0x03;
        g_sTxBuff[0].m_gBuffer[2]=0x03;
        g_sTxBuff[0].m_gBuffer[3]=0x68;
        g_sTxBuff[0].m_gBuffer[4]=0x04;	           
        g_sTxBuff[0].m_gBuffer[7]=g_sTxBuff[0].m_gBuffer[4]+g_sTxBuff[0].m_gBuffer[5]+g_sTxBuff[0].m_gBuffer[6];
        g_sTxBuff[0].m_gBuffer[8]=0x16;
        SendBytes_LED(g_sTxBuff[0].m_gBuffer,9);	
    }
    else                   //////二级数据
    {	              
        g_sTxBuff[0].m_gBuffer[0]=0x68;     
	g_sTxBuff[0].m_gBuffer[1]=0x0b;
	g_sTxBuff[0].m_gBuffer[2]=0x0b;
	g_sTxBuff[0].m_gBuffer[3]=0x68;
	g_sTxBuff[0].m_gBuffer[4]=0x01;
	unsigned long xjj;
	unsigned int  utmpadj;
	xjj=0;
	for(i=0;i<32;i++)
		{
		xjj += g_gRmtMeasPJ[0][i];
		}
	xjj = xjj>>5;	
	if((xjj<2800)&&(xjj>500))
		{
		utmpadj=((unsigned long)(g_gAdjObj[0]*(3000-xjj)))>>12;
		if(xjj>utmpadj)xjj=xjj-utmpadj;
		}
        g_sTxBuff[0].m_gBuffer[5]= (unsigned char)(xjj);  //////链路地址
        g_sTxBuff[0].m_gBuffer[6]= (unsigned char)(xjj >> 8);   //////链路地址
  	xjj=0;
	for(i=0;i<32;i++)
		{
		xjj += g_gRmtMeasPJ[1][i];
		}
	xjj = xjj>>5; 
	if((xjj<2800)&&(xjj>500))
		{
		utmpadj=((unsigned long)(g_gAdjObj[1]*(3000-xjj)))>>12;
		if(xjj>utmpadj)xjj=xjj-utmpadj;
		}
        g_sTxBuff[0].m_gBuffer[7]= (unsigned char)(xjj);//(g_gRmtFilMeas[RM_UB]);  //////链路地址
        g_sTxBuff[0].m_gBuffer[8]= (unsigned char)(xjj>>8);//(g_gRmtFilMeas[RM_UB] >> 8);   //////链路地址
   	xjj=0;
	for(i=0;i<32;i++)
		{
		xjj += g_gRmtMeasPJ[2][i];
		}
	xjj = xjj>>5;   
	if((xjj<2800)&&(xjj>500))
		{
		utmpadj=((unsigned long)(g_gAdjObj[2]*(3000-xjj)))>>12;
		if(xjj>utmpadj)xjj=xjj-utmpadj;
		}
        g_sTxBuff[0].m_gBuffer[9]= (unsigned char)(xjj);//(g_gRmtMeas[RM_UC]);  //////链路地址
        g_sTxBuff[0].m_gBuffer[10]= (unsigned char)(xjj>>8);//(g_gRmtMeas[RM_UC] >> 8);   //////链路地址
          
        g_sTxBuff[0].m_gBuffer[11]= (unsigned char)(g_gRmtFilMeas[RM_U0]);  //////链路地址
        g_sTxBuff[0].m_gBuffer[12]= (unsigned char)(g_gRmtFilMeas[RM_U0] >> 8);   //////链路地址
          
        g_sTxBuff[0].m_gBuffer[13]= (unsigned char)(g_gRmtFilMeas[RM_I0]);  //////链路地址
        g_sTxBuff[0].m_gBuffer[14]= (unsigned char)(g_gRmtFilMeas[RM_I0] >> 8);   //////链路地址
	g_sTxBuff[0].m_gBuffer[15]=0;
	for(i=4;i<15;i++)
	    g_sTxBuff[0].m_gBuffer[15]+=g_sTxBuff[0].m_gBuffer[i];	
	g_sTxBuff[0].m_gBuffer[16]=0x16;
        SendBytes_LED(g_sTxBuff[0].m_gBuffer,17);  
      
    }
     return 1;	
}

//==============================================================================
//  函数名称   : ReadLEDVer
//  功能描述   : 特殊命令，读取液晶的版本号，建议开机读取
//  输入参数   :   
//  输出参数   ：<无>
//  返回值     : 
//  其他说明   : 
//  作者       ：
//==============================================================================
void ReadLEDVer(void)
{
    g_sTxBuff[0].m_gBuffer[0]=0x68;
    g_sTxBuff[0].m_gBuffer[1]=0x03;
    g_sTxBuff[0].m_gBuffer[2]=0x03;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x0e;
    g_sTxBuff[0].m_gBuffer[5]=0x00;
    g_sTxBuff[0].m_gBuffer[6]=0x00;
    g_sTxBuff[0].m_gBuffer[7]=0x0e;		    	
    g_sTxBuff[0].m_gBuffer[8]=0x16;

    //vernum_flag=0;
    VerNumlcd[0]=0;
    VerNumlcd[1]=0;
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,9);
}
//==============================================================================
//  函数名称   : RxProcess_101
//  功能描述   : 解帧收到的数据，进行处理
//  输入参数   : unCommPortIndex：通信端口  
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void RxProcess_LED(unsigned int unCommPortIndex)
{ 
    if(bFrameReceived[unCommPortIndex]==NO)
        return;        // no frame received
    
    bFrameReceived[unCommPortIndex]=NO;//clear flag

    switch (g_unRX_Buff[unCommPortIndex][0])
    {
        case 0x06:  operate06(unCommPortIndex);
                    break;
        case 0x13:  main_reset_flag=0x55;
                    break;
        case 0x02:  operate02(unCommPortIndex);
                    break; 
        case 0x07:  operate07(unCommPortIndex); 
                    break;
        case 0x08:  operate08(unCommPortIndex);
                    break;
        case 0x09:  operate09(unCommPortIndex);
                    break;
        case 0x0A:  operate0A(unCommPortIndex);
                    break;
        case 0x0B:  operate0B(unCommPortIndex);
                    break;
        case 0x0C:  operate0C(unCommPortIndex);
                    break;
        case 0x0D:  operate0D(unCommPortIndex);
                    break;
        case 0x0E:  
		      VerNumlcd[0]=g_unRX_Buff[unCommPortIndex][1];
		      VerNumlcd[1]=g_unRX_Buff[unCommPortIndex][2];	
		      //vernum_flag=1;
                    break;        
        case 0x12:  operate12(unCommPortIndex);
                    break;        
        case 0x11:  operate11(unCommPortIndex);
                    break;        
        case 0x17:  operate17(unCommPortIndex);
                    break;
        case 0x14:  operate14(unCommPortIndex);
                    break;
        case 0x15:  operate15(unCommPortIndex);
                    break; 
        case 0x16:  operate16(unCommPortIndex);
                    break;
        case 0x18:  operate18(unCommPortIndex);
                    break;  
        case 0x19:  operate19(unCommPortIndex);
                    break;
        case 0x20:  operate20(unCommPortIndex);
                    break;
        default:
                 operateFF(unCommPortIndex);     
                  break;  
    }
}
//==============================================================================
//  函数名称   : Comm_GPRS_101
//  功能描述   : GPRS模式101协议下数据传输
//  输入参数   : <无> 
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void Comm_LED_101(void)
{
   // unsigned int unTime_GPRS;  //GPRS间隔时间计时数
    g_uc101BuffTail =  128 - DMA0SZ;
    if (g_uc101BuffHead != g_uc101BuffTail)    

        g_sRxBuff[0].m_iBufT = g_uc101BuffTail;
    if( g_sRxBuff[0].m_iBufH != g_sRxBuff[0].m_iBufT )
    {     
        g_uc101BuffHead = g_sRxBuff[0].m_iBufH; 
        DetectFrame_LED(0);
        RxProcess_LED(0);
    } 

}