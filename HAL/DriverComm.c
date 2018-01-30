/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  ģ����         : ͨ������ģ��
  �ļ���         : DriverComm.c
  ����ļ�       : DataStruct.h FuncDef.h Constant.h DriverComm.c Comm.c
  ʵ�ֹ���       : ���ͨ�Ŵ��ڵĳ�ʼ����
                                                                 ���GPRSģ���RF������Ƶģ���ͨ�������Ϳ��ơ��͹��Ĺ���ȹ���
                                                                 ���232/485��ͨ�Ź���
  ����           : 
  �汾           : 1.0
  
  --------------------------------------------------------------------------------
  ��ע           : -
  --------------------------------------------------------------------------------
  
  �޸ļ�¼       : 
    ��  ��          �汾            �޸���          �޸����� 
    2010/06/01      1.0                              ����

************************************************************************************/

#ifndef _DRIVER_COMM

#define _DRIVER_COMM
  #include "..\DataStruct.h"
#endif


//==============================================================================
//  ��������   : InitSCI
//  ��������   : ͨ��ģ���ʼ��������SCI���ã��������в������ø�������
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ������һ
//==============================================================================
void InitSCI(void)
{
    //Һ������
    P3SEL |= 0x30;                  // P3.4,5 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSWRST;           // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_1;          // ACLK
    UCA0BR0 = 0x82;                // 16MHz 9600 (see User's Guide)
    UCA0BR1 = 0x06;                // 16MHz 9600      
    UCA0CTL1 &= ~UCSWRST;          // **Initialize USCI state machine**
   // UCA0IE |= UCRXIE;              // ʹ�� USCI_A0 RX �ж�+UCTXIE
    
    //���Դ���
    P5SEL |= 0xc0;                  // P5.6,7 = USCI_A0 TXD/RXD
    UCA1CTL1 |= UCSWRST;           // **Put state machine in reset**
    UCA1CTL1 |= UCSSEL_1;          // ACLK

    UCA1BR0 = 0x38;                // 16MHz 19200 (see User's Guide)
    UCA1BR1 = 0x01;                // 16MHz 19200//9600=1666(0x682)  57600=277(x115) 115200=138(8a)
									   //19200=833(x341) 38400=416(x1a0)
    UCA1CTL1 &= ~UCSWRST;          // **Initializ1 RX �ж�+UCTXIE
    UCA1IE |= (UCRXIE + UCTXIE);              // ʹ�� USCI_A0 RX �ж�
    
    //GPRS
    P9SEL |= 0x30;                  // P9.4,5 = USCI_A0 TXD/RXD
    UCA2CTL1 |= UCSWRST;           // **Put state machine in reset**
    UCA2CTL1 |= UCSSEL_1;          // SMCLK
    UCA2BR0 = 0x82;                // 8MHz 38400 (see User's Guide)
    UCA2BR1 = 0x06;                // 8MHz 38400
    UCA2CTL1 &= ~UCSWRST;          // **Initialize USCI state machine**
    UCA2IE |= (UCRXIE + UCTXIE);              // ʹ�� USCI_A0 RX �ж�+UCTXIE
    
    
}



//==============================================================================
//  ��������   : SendBytes
//  ��������   :  ����һ֡���� 
//  �������   : ucRxdlen�����ݳ���  *Data������
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void SendBytes_LED(unsigned char *Data,unsigned char ucRxdlen)
{
  //  unsigned int  i;
    DMA2SZ = ucRxdlen; //����DMA�鴫��size

    //DMA2SA = g_unLEDTX_Buff;                 // Start block address
    DMA2SA = g_sTxBuff[0].m_gBuffer;
//    DMA2SA = g_sTxBuff[COMM_INDEX_232].m_gBuffer;                 // Start block address
    DMA2DA = (unsigned int*)UCA0TXBUF_;                          // Destination block address
    DMA2CTL = DMADT_1+ DMASRCINCR_3 + DMADSTINCR_0 + DMASBDB + DMALEVEL;  // once, inc SRC   
    DMACTL1 |= DMA2TSEL_17; 
    

    DMA2CTL |= DMAEN;                                     //DMA2ʹ��  
    

}

//==============================================================================
//  ��������   : PhysSendByte
//  ��������   :  ������ֽڷ��� 
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void PhysSendByte_GPRS(unsigned int ucByte)
{
    unsigned int  i;
    for(i = 0; i < 1000; i++);                                     //��ʱ
    while((UCA1IFG & UCTXIFG == 0)||(UCA1STAT & UCBUSY == 1));   //�ȴ����ͻ�������
        UCA1TXBUF = ucByte;

}

//==============================================================================
//  ��������   : SendBytes
//  ��������   :  ����һ֡���� 
//  �������   : ucRxdlen�����ݳ���  *Data������
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void SendBytes_GPRS(unsigned char *Data,int ucRxdlen)
{
  int i = 0;
  for(i = 0;i < ucRxdlen;i++)
    PhysSendByte_GPRS(Data[i]); 
}
//==============================================================================
//  ��������   : PhysSendByte
//  ��������   :  ������ֽڷ��� 
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void PhysSendByte_UART2(unsigned char ucByte)
{
    //unsigned int  i;
    //for(i = 0; i < 1000; i++);                                     //��ʱ
    while((UCA2IFG & UCTXIFG == 0)||(UCA2STAT & UCBUSY == 1));   //�ȴ����ͻ�������
        UCA2TXBUF = ucByte;

}
//==============================================================================
//  ��������   : SendBytes
//  ��������   :  ����һ֡���� 
//  �������   : ucRxdlen�����ݳ���  *Data������
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void SendBytes_UART2(unsigned char *Data,int ucRxdlen)
{
  int i = 0;
  for(i = 0;i < ucRxdlen;i++)
    PhysSendByte_UART2(Data[i]);
}

//**********************************************************************************
// **                                   ���� 1
// ** ������� Э��ͬGPRSͨѶ  
//**********************************************************************************

#pragma vector=USCI_A1_VECTOR           
__interrupt void UsartA1_ISR()          //��������
{    
unsigned char  ucRxdReg;
  unsigned char cCmId;
  cCmId = 1;
  
 switch(__even_in_range(UCA1IV,4))
   {
    case 0x04://���� 
    	
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
     case 0x02://����
           // if((g_CmIdGPRS == cCmId) && !g_GprsPowerSt)//for test ģ��GPRS�ر�״̬���ر���վӦ���ղ�������
             // break;
            ucRxdReg = UCA1RXBUF;
            g_sRxBuff[cCmId].m_iRcvCount = 30;
             g_sRxBuff[cCmId].m_iBufT &= (MAX_RXBUFF_LEN - 1);
            g_sRxBuff[cCmId].m_gBuf[g_sRxBuff[cCmId].m_iBufT] = ucRxdReg;   //�������ݵ����ջ����� 
            g_sRxBuff[cCmId].m_iBufT++;
           break;
   default:break;
   }        
}



//**********************************************************************************
// **                                   ���� 2
// ** GPRS
//**********************************************************************************
#ifdef GPRS_SMS //�ŏ| 0324

#pragma vector=USCI_A2_VECTOR
__interrupt void UsartA2_ISR()          //��������
{
  unsigned char  ucRxdReg;
  unsigned char cCmId;
  cCmId = 2;
  
 switch(__even_in_range(UCA2IV,4))
   {
    case 0x04://����
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
			g_ucGPRSState_TCounter = 0;					//�ŏ|0330 
			g_ucGPRSState = GPRSState_IDLE;//�ŏ|0330 �����ڿ���״̬
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
			 g_ucGPRSState_TCounter = 0;					//�ŏ|0330 			  
			 g_ucGPRSState = GPRSState_IDLE;//�ŏ|0330 �����ڿ���״̬		  
                  }
	 	break;
	  default:
	  	g_ucGPRSState_TCounter = 0;					//�ŏ|0330 
	  	g_ucGPRSState = GPRSState_IDLE;
	  	break;	
         }
         break;
     case 0x02://����
            ucRxdReg = UCA2RXBUF;
            g_sRxBuff[cCmId].m_iRcvCount = 200;
             g_sRxBuff[cCmId].m_iBufT &= (MAX_RXBUFF_LEN - 1);
            g_sRxBuff[cCmId].m_gBuf[g_sRxBuff[cCmId].m_iBufT] = ucRxdReg;   //�������ݵ����ջ����� 
            g_sRxBuff[cCmId].m_iBufT++;
           break;
   default:break;
   }        
}

//**********************************************************************************
// **                                   ���� 2
// ** ����
//**********************************************************************************
#else //�ŏ| 0324
#pragma vector=USCI_A2_VECTOR
__interrupt void USCI_A2_ISR(void)
{
    unsigned char  ucRxdReg;
    switch(__even_in_range(UCA2IV,4))
    {
        case 2:                                                           // Vector 2 - RXIFG    
            ucRxdReg = UCA2RXBUF; 
            /* //�ŏ| 0324
            if(g_ucBuffTail[COMM_INDEX_GPRS] == 256)
                g_ucBuffTail[COMM_INDEX_GPRS] = 0;
            g_sRxBuff[COMM_INDEX_GPRS].m_gBuffer[g_ucBuffTail[COMM_INDEX_GPRS]] = ucRxdReg;   //�������ݵ����ջ����� 
            g_ucBuffTail[COMM_INDEX_GPRS]++;
            *///�ŏ| 0324
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
			 g_ucGPRSState = GPRSState_IDLE;//�ŏ|0330 �����ڿ���״̬		  
                  }
              
                
           break;    
        default:
            break;
    }
    
}
#endif  //�ŏ| 0324
