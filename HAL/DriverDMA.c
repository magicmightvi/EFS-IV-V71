/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  模块名       : 驱动程序
  文件名       : .c
  相关文件     : DataStruct.h FuncDef.h Constant.h
  实现功能     : 
  作者         : 林中一
  版本         : 1.0
  
  --------------------------------------------------------------------------------
  备注         : -
  --------------------------------------------------------------------------------
  
  修改记录     : 
    日  期        版本        修改人        修改内容 
    2010/06/01    1.0         林中一        创建

************************************************************************************/
#ifndef _DRIVER_DMA

#define _DRIVER_DMA
  #include "..\DataStruct.h"
#endif

//==============================================================================
//  函数名称   : InitDMA
//  功能描述   : DMA0作为GPRS数据接收
//               DMA1作为232数据接收
//               DMA1作为232、GPRS数据发送共用，在发送函数中初始化
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者     :
//==============================================================================
void InitDMA(void)
{    
    DMACTL0 |= DMA0TSEL_16;
    DMA0SA =  (unsigned int*)UCA0RXBUF_ ;                        //  Start block address  
    //DMA0DA =  g_sRxBuff[COMM_INDEX_RF].m_gBuffer;               // Destination block address0
    //DMA0DA =  g_sRX_BUFF_LED.m_gBuffer;
    DMA0DA =  g_sRxBuff[COMM_INDEX_000].m_gBuf;
    DMA0SZ =  128;                                               // Block size
    DMA0CTL = DMADT_4 + DMASRCINCR_0 +DMADSTINCR_3 + DMASBDB  + DMALEVEL ; // Rpt, inc DST   
    DMA0CTL |= DMAEN;

}