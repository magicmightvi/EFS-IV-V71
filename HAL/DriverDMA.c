/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  ģ����       : ��������
  �ļ���       : .c
  ����ļ�     : DataStruct.h FuncDef.h Constant.h
  ʵ�ֹ���     : 
  ����         : ����һ
  �汾         : 1.0
  
  --------------------------------------------------------------------------------
  ��ע         : -
  --------------------------------------------------------------------------------
  
  �޸ļ�¼     : 
    ��  ��        �汾        �޸���        �޸����� 
    2010/06/01    1.0         ����һ        ����

************************************************************************************/
#ifndef _DRIVER_DMA

#define _DRIVER_DMA
  #include "..\DataStruct.h"
#endif

//==============================================================================
//  ��������   : InitDMA
//  ��������   : DMA0��ΪGPRS���ݽ���
//               DMA1��Ϊ232���ݽ���
//               DMA1��Ϊ232��GPRS���ݷ��͹��ã��ڷ��ͺ����г�ʼ��
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����     :
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