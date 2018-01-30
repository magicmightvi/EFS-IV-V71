/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  模块名         : 通信驱动模块
  文件名         : DriverComm.c
  相关文件       : DataStruct.h FuncDef.h Constant.h DriverComm.c Comm.c
  实现功能       : 完成通信串口的初始化，
                                                                 完成GPRS模块和RF无线射频模块的通信驱动和控制、低功耗管理等功能
                                                                 完成232/485口通信管理
  作者           : 
  版本           : 1.0
  
  --------------------------------------------------------------------------------
  备注           : -
  --------------------------------------------------------------------------------
  
  修改记录       : 
    日  期          版本            修改人          修改内容 
    2010/06/01      1.0                              创建

************************************************************************************/

#ifndef _DRIVER_COMM

#define _DRIVER_COMM
  #include "..\DataStruct.h"
#endif


//==============================================================================
//  函数名称   : InitSCI
//  功能描述   : 通信模块初始化，包括SCI设置，根据运行参数设置各波特率
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：林中一
//==============================================================================
void InitSCI(void)
{
    //液晶串口
    P3SEL |= 0x30;                  // P3.4,5 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSWRST;           // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_1;          // ACLK
    UCA0BR0 = 0x82;                // 16MHz 9600 (see User's Guide)
    UCA0BR1 = 0x06;                // 16MHz 9600      
    UCA0CTL1 &= ~UCSWRST;          // **Initialize USCI state machine**
   // UCA0IE |= UCRXIE;              // 使能 USCI_A0 RX 中断+UCTXIE
    
    //调试串口
    P5SEL |= 0xc0;                  // P5.6,7 = USCI_A0 TXD/RXD
    UCA1CTL1 |= UCSWRST;           // **Put state machine in reset**
    UCA1CTL1 |= UCSSEL_1;          // ACLK

    UCA1BR0 = 0x38;                // 16MHz 19200 (see User's Guide)
    UCA1BR1 = 0x01;                // 16MHz 19200//9600=1666(0x682)  57600=277(x115) 115200=138(8a)
									   //19200=833(x341) 38400=416(x1a0)
    UCA1CTL1 &= ~UCSWRST;          // **Initializ1 RX 中断+UCTXIE
    UCA1IE |= (UCRXIE + UCTXIE);              // 使能 USCI_A0 RX 中断
    
    //GPRS
    P9SEL |= 0x30;                  // P9.4,5 = USCI_A0 TXD/RXD
    UCA2CTL1 |= UCSWRST;           // **Put state machine in reset**
    UCA2CTL1 |= UCSSEL_1;          // SMCLK
    UCA2BR0 = 0x82;                // 8MHz 38400 (see User's Guide)
    UCA2BR1 = 0x06;                // 8MHz 38400
    UCA2CTL1 &= ~UCSWRST;          // **Initialize USCI state machine**
    UCA2IE |= (UCRXIE + UCTXIE);              // 使能 USCI_A0 RX 中断+UCTXIE
    
    
}



//==============================================================================
//  函数名称   : SendBytes
//  功能描述   :  发送一帧数据 
//  输入参数   : ucRxdlen：数据长度  *Data：数据
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void SendBytes_LED(unsigned char *Data,unsigned char ucRxdlen)
{
  //  unsigned int  i;
    DMA2SZ = ucRxdlen; //设置DMA块传输size

    //DMA2SA = g_unLEDTX_Buff;                 // Start block address
    DMA2SA = g_sTxBuff[0].m_gBuffer;
//    DMA2SA = g_sTxBuff[COMM_INDEX_232].m_gBuffer;                 // Start block address
    DMA2DA = (unsigned int*)UCA0TXBUF_;                          // Destination block address
    DMA2CTL = DMADT_1+ DMASRCINCR_3 + DMADSTINCR_0 + DMASBDB + DMALEVEL;  // once, inc SRC   
    DMACTL1 |= DMA2TSEL_17; 
    

    DMA2CTL |= DMAEN;                                     //DMA2使能  
    

}

//==============================================================================
//  函数名称   : PhysSendByte
//  功能描述   :  物理层字节发送 
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void PhysSendByte_GPRS(unsigned int ucByte)
{
    unsigned int  i;
    for(i = 0; i < 1000; i++);                                     //延时
    while((UCA1IFG & UCTXIFG == 0)||(UCA1STAT & UCBUSY == 1));   //等待发送缓冲区空
        UCA1TXBUF = ucByte;

}

//==============================================================================
//  函数名称   : SendBytes
//  功能描述   :  发送一帧数据 
//  输入参数   : ucRxdlen：数据长度  *Data：数据
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void SendBytes_GPRS(unsigned char *Data,int ucRxdlen)
{
  int i = 0;
  for(i = 0;i < ucRxdlen;i++)
    PhysSendByte_GPRS(Data[i]); 
}
//==============================================================================
//  函数名称   : PhysSendByte
//  功能描述   :  物理层字节发送 
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void PhysSendByte_UART2(unsigned char ucByte)
{
    //unsigned int  i;
    //for(i = 0; i < 1000; i++);                                     //延时
    while((UCA2IFG & UCTXIFG == 0)||(UCA2STAT & UCBUSY == 1));   //等待发送缓冲区空
        UCA2TXBUF = ucByte;

}
//==============================================================================
//  函数名称   : SendBytes
//  功能描述   :  发送一帧数据 
//  输入参数   : ucRxdlen：数据长度  *Data：数据
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void SendBytes_UART2(unsigned char *Data,int ucRxdlen)
{
  int i = 0;
  for(i = 0;i < ucRxdlen;i++)
    PhysSendByte_UART2(Data[i]);
}

//**********************************************************************************
// **                                   串口 1
// ** 软件配置 协议同GPRS通讯  
//**********************************************************************************

#pragma vector=USCI_A1_VECTOR           
__interrupt void UsartA1_ISR()          //发送数据
{    
unsigned char  ucRxdReg;
  unsigned char cCmId;
  cCmId = 1;
  
 switch(__even_in_range(UCA1IV,4))
   {
    case 0x04://发送 
    	
	    if(g_sTxBuff[cCmId].m_ucTx_Head != g_sTxBuff[cCmId].m_ucTx_Tail)
	    {
			g_sTxBuff[cCmId].m_unSendFlag = 1;
			UCA1TXBUF = g_sTxBuff[cCmId].m_gBuffer[g_sTxBuff[cCmId].m_ucTx_Head];
			g_sTxBuff[cCmId].m_ucTx_Head++;
	        g_sTxBuff[cCmId].m_ucTx_Head &= (MAX_RXBUFF_LEN - 1);
		}
		else
		{
			g_sTxBuff[cCmId].m_unSendFlag = 0;			
		}
         break;
     case 0x02://接收
           // if((g_CmIdGPRS == cCmId) && !g_GprsPowerSt)//for test 模拟GPRS关闭状态，关闭子站应该收不到数据
             // break;
            ucRxdReg = UCA1RXBUF;
            g_sRxBuff[cCmId].m_iRcvCount = 30;
             g_sRxBuff[cCmId].m_iBufT &= (MAX_RXBUFF_LEN - 1);
            g_sRxBuff[cCmId].m_gBuf[g_sRxBuff[cCmId].m_iBufT] = ucRxdReg;   //保存数据到接收缓冲器 
            g_sRxBuff[cCmId].m_iBufT++;
           break;
   default:break;
   }        
}



//**********************************************************************************
// **                                   串口 2
// ** GPRS
//**********************************************************************************
#ifdef GPRS_SMS //张弢 0324

#pragma vector=USCI_A2_VECTOR
__interrupt void UsartA2_ISR()          //发送数据
{
  unsigned char  ucRxdReg;
  unsigned char cCmId;
  cCmId = 2;
  
 switch(__even_in_range(UCA2IV,4))
   {
    case 0x04://发送
       switch(g_ucGPRSState)
       {
       case GPRSState_GPRS101:
    		if(g_sTxBuff[cCmId].m_ucTx_Head != g_sTxBuff[cCmId].m_ucTx_Tail)
	    	{
			g_sTxBuff[cCmId].m_unSendFlag = 1;
			UCA2TXBUF = g_sTxBuff[cCmId].m_gBuffer[g_sTxBuff[cCmId].m_ucTx_Head];
			g_sTxBuff[cCmId].m_ucTx_Head++;
	        	g_sTxBuff[cCmId].m_ucTx_Head &= (MAX_RXBUFF_LEN - 1);
		}
		else
		{
			g_sTxBuff[cCmId].m_unSendFlag = 0;
			g_ucGPRSState_TCounter = 0;					//张弢0330 
			g_ucGPRSState = GPRSState_IDLE;//张弢0330 串口在空闲状态
		}
	  	break;
	 case GPRSState_SMS:	
	 	 if(g_ucGPRSTxNum != g_ucGPRSTxlen)
                  {
                     // if(unFrameNum == 0)
                          //UCA2TXBUF = g_unTX_Buff[COMM_INDEX_GPRS][g_ucGPRSTxNum];
                      //else
                          UCA2TXBUF = g_unGPRSTX_Buff[g_ucGPRSTxNum];
                      g_ucGPRSTxNum++; 
                  }
                  else
                  {
                      g_ucGPRSTxNum = 0;// g_unSendFlag_GPRS = 0;
                      g_ucGPRSTxlen = 0;
			 g_ucGPRSState_TCounter = 0;					//张弢0330 			  
			 g_ucGPRSState = GPRSState_IDLE;//张弢0330 串口在空闲状态		  
                  }
	 	break;
	  default:
	  	g_ucGPRSState_TCounter = 0;					//张弢0330 
	  	g_ucGPRSState = GPRSState_IDLE;
	  	break;	
         }
         break;
     case 0x02://接收
            ucRxdReg = UCA2RXBUF;
            g_sRxBuff[cCmId].m_iRcvCount = 200;
             g_sRxBuff[cCmId].m_iBufT &= (MAX_RXBUFF_LEN - 1);
            g_sRxBuff[cCmId].m_gBuf[g_sRxBuff[cCmId].m_iBufT] = ucRxdReg;   //保存数据到接收缓冲器 
            g_sRxBuff[cCmId].m_iBufT++;
           break;
   default:break;
   }        
}

//**********************************************************************************
// **                                   串口 2
// ** 短信
//**********************************************************************************
#else //张弢 0324
#pragma vector=USCI_A2_VECTOR
__interrupt void USCI_A2_ISR(void)
{
    unsigned char  ucRxdReg;
    switch(__even_in_range(UCA2IV,4))
    {
        case 2:                                                           // Vector 2 - RXIFG    
            ucRxdReg = UCA2RXBUF; 
            /* //张弢 0324
            if(g_ucBuffTail[COMM_INDEX_GPRS] == 256)
                g_ucBuffTail[COMM_INDEX_GPRS] = 0;
            g_sRxBuff[COMM_INDEX_GPRS].m_gBuffer[g_ucBuffTail[COMM_INDEX_GPRS]] = ucRxdReg;   //保存数据到接收缓冲器 
            g_ucBuffTail[COMM_INDEX_GPRS]++;
            *///张弢 0324
            break;
        case 0x04:              
                  if(g_ucGPRSTxNum != g_ucGPRSTxlen)
                  {
                      //if(unFrameNum == 0)
                          //UCA2TXBUF = g_unTX_Buff[COMM_INDEX_GPRS][g_ucGPRSTxNum];
                      //else
                          UCA2TXBUF = g_unGPRSTX_Buff[g_ucGPRSTxNum];
                      g_ucGPRSTxNum++; 
                  }
                  else
                  {
                      g_ucGPRSTxNum = 0;// g_unSendFlag_GPRS = 0;
                      g_ucGPRSTxlen = 0;
			 g_ucGPRSState = GPRSState_IDLE;//张弢0330 串口在空闲状态		  
                  }
              
                
           break;    
        default:
            break;
    }
    
}
#endif  //张弢 0324
