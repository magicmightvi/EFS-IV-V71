/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  ģ����     : Һ�����ݽ���ģ��
  �ļ���     : Comm.c
  ����ļ�       : DataStruct.h FuncDef.h Constant.h DriverComm.c Main.c 
  ʵ�ֹ���       : 
  ����       : 
  �汾       : 1.0
  
  --------------------------------------------------------------------------------
  ��ע       : -
  --------------------------------------------------------------------------------
  
  �޸ļ�¼       : 
    ��  ��      �汾        �޸���      �޸����� 


************************************************************************************/

#ifndef _COMM_LED

#define _COMM_LED
  #include "DataStruct.h"
#endif
 struct  check{                   //ʵʱʱ�ӽṹ �оݲ���
	unsigned char high_phase;
	unsigned char low_phase;
	unsigned char high_zero;
	unsigned char low_zero;
	unsigned char voltage_zero;
}compare; 
//==============================================================================
//  ��������   : InitComm
//  ��������   : ͨ��ģ���ʼ���������Ի������ĳ�ʼ��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       �� 
//==============================================================================
/*void InitComm(void)
{
    InitSCI();
}*/
//==============================================================================
//  ��������   : CrcCount
//  ��������   : ���ݴ��ݽ���������ָ��p�����㳤��Ϊlength�ֵ�CRCУ����
//  �������   : p����У�����ݵ�ͷָ��
//           length����У�����ݵĳ���
//  �������   :<��>
//  ����ֵ     : crcvalue:У���õ���CRCУ����(16λ)
//  ����˵��   : 
//  ����       �� 
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
            if((AL & 0x01) == 1)    // AL���λΪ1
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
    return ((unsigned int)(AL * 0x100 + AH));       //��λ��ǰ����λ�ں�

}*/


//==============================================================================
//  ��������   : AddChar
//  ��������   : ���ݴ��ݽ���������ָ��p�����㳤��Ϊlength�ļӺ�У����
//  �������   : p����У�����ݵ�ͷָ��
//           length����У�����ݵĳ���
//  �������   :<��>
//  ����ֵ     : AddValue:У���õ��ļӺ�У����(ȡ8λ�洢)
//  ����˵��   : 
//  ����       ��
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
//  ��������   : BuffCapacity_LED
//  ��������   : ���㻺�������յ��������ݸ���
//  �������   : <��>
//  �������   ��unCapacity��һ֡���ݵĸ��� 
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
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
//  ��������   : BuffPositionLED
//  ��������   : ��������λ�õļ���
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int BuffPositionLED(unsigned int unpointer)
{
    // if Rx buff reach its end, restart from the beginning.
    if( unpointer >= 128)
        unpointer %= 128;

    return unpointer;
}
//==============================================================================
//  ��������   : TakeFrame101
//  ��������   : �����ջ�����������֡��ȡ����������
//  �������   : unCommPortIndex��ͨ�Ŷ˿� unRxLength������֡����(ȥ��֡ͷ֡β)
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��

//==============================================================================
void TakeFrame_LED(unsigned int unCommPortIndex, unsigned int unRxLength)
{
    unsigned int unCounter = 0;
    /**10 CC AA cs 16***/
    /**68 LL LL 68 CC AA DD    cs 16***/
     if(g_sRxBuff[0].m_gBuf[g_uc101BuffHead] == 0x68) //�����ַ�Ϊ68H
    {
        //g_unFrameFlag_101 = NUM_68H;
        for(unCounter = 0;unCounter < unRxLength ;unCounter++)
            g_unRX_Buff[unCommPortIndex][unCounter] = g_sRxBuff[0].m_gBuf[BuffPositionLED(g_uc101BuffHead + unCounter + 4)];
        
        g_sRxBuff[0].m_iBufH=BuffPositionLED(g_uc101BuffHead + unRxLength + 6);// ��������������ͷָ��λ��
    }
    g_uc101BuffHead =  g_sRxBuff[0].m_iBufH;

}
//==============================================================================
//  ��������   : DetectFrame
//  ��������   : ����µ�����֡
//  �������   : unCommPortIndex��ͨ�Ŷ˿� 
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
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
        if(BuffCapacity_LED()>=5) //����֡�ֽںϷ�
        {//2  
          /**10 CC AA cs 16***/
          /**10 5b 01 5c 16***/ 
          /**68 LL LL 68 CC AA DD    cs 16***/
      /**68 04 04 68 00 99 88 77 98 16***/
            if ((g_sRxBuff[0].m_gBuf[g_uc101BuffHead] == 0x68 && g_sRxBuff[0].m_gBuf[g_uc101BuffHead] == g_sRxBuff[0].m_gBuf[BuffPositionLED(g_uc101BuffHead + 3)]) )  //�����ַ�68H
                  //�˿ڷ��� || unCommPortIndex == COMM_INDEX_232
            {//3 
                if (g_sRxBuff[0].m_gBuf[g_uc101BuffHead] == 0x68)
                {
                    //unAddFlag = 4;
                    unFramLen = g_sRxBuff[0].m_gBuf[BuffPositionLED(g_uc101BuffHead + 1)] + 6;
                }
                
                
                if( BuffCapacity_LED() >= unFramLen)//��������Ҫ���ҵ�ַ��ȷ    && (Addr == g_gRunParaJdg[RP_COMM_ADDR])
                {//4.2   
                    unRxLength = unFramLen - 6; //ȥ��֡ͷ֡β�ĳ���
                    unCurrentPointer = g_uc101BuffHead + unFramLen - 2;
                    unRxADD = g_sRxBuff[0].m_gBuf[BuffPositionLED(unCurrentPointer)] ;
                    if (unRxADD == AddComChar(unCommPortIndex,unRxLength,0))//У��
                    {//5
                        TakeFrame_LED(unCommPortIndex,unRxLength);  
                        bFrameReceived[unCommPortIndex] = YES;  // set flag: frame received
                        //��TakeFrame0�����У��Ѿ���nBuffHeadָ����һ֡���ݵĵ�һ���ֽڡ����ǵ�
                        //���������н�nBuffHead�����1λ�Ĳ���������ڴ���Ҫ��nBuffHead��ֵ��1��
                        //���´�ѭ��Ѱ��֡ͷʱ����֤nBuffHeadָ����ǵ�һ���������ݡ�                       
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
                else//���Ȳ�����Ҫ�����վ��ַ������
                {//4.3              
                    if((BuffCapacity_LED() <= unFramLen))//���Ȳ����� �ȴ��������� ֱ���˳�
                    {
                        g_sRxBuff[0].m_iBufH = g_uc101BuffHead;
                        return;
                    }
                    else //�����������ַ����
                    {
                        ;// increase buff head, continue detecting
                    }
                   
                }//4.3      
            }//3            
        } //2
        else//���Ȳ�����4�ֽڣ��ȴ��������� ֱ���˳�   
        { //2
            g_sRxBuff[0].m_iBufH = g_uc101BuffHead; 
            return;
        } //2
        /*���д����н�g_uc101BuffTail-1��ԭ���ǣ���g_uc101BuffHead=127��
        ����ʱ�պ�g_uc101BuffTail=128����ôg_uc101BuffHead�����һλ�ᵽbuffͷ���ͻ�����ѭ��*/
        if(g_uc101BuffHead != (g_uc101BuffTail-1)) // increase buff head, continue detecting
            g_uc101BuffHead = BuffPositionLED(g_uc101BuffHead+1);   
        
    } while(1);   //1

}

//==============================================================================
//  ��������   : operate06
//  ��������   : ������06������  ң���ط�����
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate06(unsigned int unCommPortIndex)
{
    if(g_unRX_Buff[unCommPortIndex][1]==0)       //�ط�
    {
        eight_select=0x8F; //eight_select|=BIT7;                    ////////////���״η���8����ı�־		
        Sign_Repeat(0,0);
    }
    else                    ////////ģ�����
    {
        eight_select=0x8F; //eight_select|=BIT7;                    ////////////���״η���8����ı�־		
        Sign_Repeat(g_unRX_Buff[unCommPortIndex][1],0);
    }
    return 1;
}
//==============================================================================
//  ��������   : operate02
//  ��������   : ������02������  
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate02(unsigned int unCommPortIndex)
{
    unsigned char i;
    //unsigned char ucSOENUM;
    //unsigned char ucSOEPOS;
    unsigned char unTemp[10];
    if(g_unRX_Buff[unCommPortIndex][1]==0xff)        //////�ٻ�����������
    {
        g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
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
    else   //�ٻ��ڼ���
    {
 	 //CAT_SpiReadBytes(EEPADD_SOE_NUM , 2, unTemp);	
        i = g_unRX_Buff[unCommPortIndex][1];
        if(g_sSoeData.m_ucSoePos >= i)
            i =  g_sSoeData.m_ucSoePos - i;
        else
            i = 30 + g_sSoeData.m_ucSoePos - i;
        CAT_SpiReadBytes(EEPADD_SOE_DATA + i * 12 , 10, unTemp);
        
	g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
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
//  ��������   : operate07
//  ��������   : ������07������  �޸Ĳ�������
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate07(unsigned int unCommPortIndex)
{
    unsigned char i;
    g_gProcCnt[PC_HIGH_P]=g_unRX_Buff[unCommPortIndex][1];     ////////��ߵ�ѹ
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
    
    g_gProcCnt[PC_I0_START]=g_unRX_Buff[unCommPortIndex][5]*13;    /////��������ż�ֵ 0.5A/50A*0.8*4095/3.3*1.414*0.9*     
    if((g_gProcCnt[PC_I0_START]>260)||(g_gProcCnt[PC_I0_START]<13))
        g_gProcCnt[PC_I0_START]=26;
    
    g_gProcCnt[PC_T_DELAY]=g_unRX_Buff[unCommPortIndex][6];                    //////������ʱ
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
    g_gChangFlag[CHNG_PC] = ON;	  //�޸Ķ�ֵ����	
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
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
//  ��������   : operate08
//  ��������   : ������08������ �޸��о�����
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate08(unsigned int unCommPortIndex)
{
    unsigned char i;
    g_gProcCnt[PC_JAG_P]    = g_unRX_Buff[unCommPortIndex][1];
    g_gProcCnt[PC_JAG_Z]    = g_unRX_Buff[unCommPortIndex][2];
    g_gProcCnt[PC_EFS_MODEL]= g_unRX_Buff[unCommPortIndex][3];
	
    if(g_gProcCnt[PC_EFS_MODEL]>1)       ///////0X01 II���ź�Դ   0X00  I���ź�Դ
        g_gProcCnt[PC_EFS_MODEL]=0x00;	 
    if((g_gProcCnt[PC_JAG_P]!=0)&&(g_gProcCnt[PC_JAG_P]!=0x55))           ///////0X55 ���о�����   0X00  ������
	g_gProcCnt[PC_JAG_P]=0x00;	 	 
    if((g_gProcCnt[PC_JAG_Z]!=0)&&(g_gProcCnt[PC_JAG_Z]!=0x55))             ///////0X55 �����о�����   0X00  ������
	g_gProcCnt[PC_JAG_Z]=0x55; 

   // if((g_gProcCnt[PC_JAG_Z]+g_gProcCnt[PC_JAG_P])==0)
	//g_gProcCnt[PC_JAG_Z]=0x55;
    g_gProcCnt[PC_GPRS_MODEL] = g_unRX_Buff[unCommPortIndex][4];
    if((g_gProcCnt[PC_GPRS_MODEL]!=0)&&(g_gProcCnt[PC_GPRS_MODEL]!=0x55))           ///////0X00 GPRS   0X55  SMS
        g_gProcCnt[PC_GPRS_MODEL]=0x00;	
	    
            
    g_gChangFlag[CHNG_PC] = ON;	  //�޸Ķ�ֵ����	
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
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
//  ��������   : operate09
//  ��������   : ������09������  ʱ����������
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
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
    
              
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
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
    //////////����ʱ��
    g_sRtcManager.m_gRealTimer[RTC_YEAR] +=  2000;       
    DS1302_Reset();                 //////////ʱ��оƬ��λ			  				  
    DS1302_SettingData(ucTime); 
    
        return 1;
}
//==============================================================================
//  ��������   : operate0A
//  ��������   : ������0A������   Һ��������
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate0A(unsigned int unCommPortIndex)
{
    unsigned char i;
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
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
//  ��������   : operate0B
//  ��������   : ������0B������  Һ�����о�
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate0B(unsigned int unCommPortIndex)
{
    unsigned char i;
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
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
//  ��������   : operate0C
//  ��������   : ������0C������   Һ����ʱ��
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate0C(unsigned int unCommPortIndex)
{
    unsigned char i;
   // DS1302_GetData(rtc);                    //��ʱ�� 
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
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
//  ��������   : operate0D
//  ��������   : ������0D������   ����ַ�Ͱ汾��
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate0D(unsigned int unCommPortIndex)
{
    //unsigned char i;
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
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
//  ��������   : operate12
//  ��������   : ������12������   ���ö�������
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
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
		
            				
    g_ucParaChang |= BIT0;	  //�޸Ķ�ֵ����
                
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
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
//  ��������   : operate11
//  ��������   : ������11������   ��ȡ��������
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate11(unsigned int unCommPortIndex)
{
    unsigned int unTemp = 0;
    unsigned char i;
    
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
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
//  ��������   : operate17
//  ��������   : ������17������    ����ַ
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate17(unsigned int unCommPortIndex)
{
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
    g_sTxBuff[0].m_gBuffer[1]=0x03;
    g_sTxBuff[0].m_gBuffer[2]=0x03;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x17;
    g_sTxBuff[0].m_gBuffer[6]=(unsigned char)(g_gRunPara[RP_COMM_ADDR]>>8);///////0X55 �ͺ�   0X00  ��ǰ
    g_sTxBuff[0].m_gBuffer[5]=(unsigned char)(g_gRunPara[RP_COMM_ADDR]);//RdCnt(0xF215);///////0X55 �ͺ�   0X00  ��ǰ
    g_sTxBuff[0].m_gBuffer[7]=g_sTxBuff[0].m_gBuffer[4]+g_sTxBuff[0].m_gBuffer[5]+g_sTxBuff[0].m_gBuffer[6];
    g_sTxBuff[0].m_gBuffer[8]=0x16;
		//lcd_ctl=3; 
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,9);	
         return 1;
}
//==============================================================================
//  ��������   : operate14
//  ��������   : ������14������   �޸ĵ�ַ
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate14(unsigned int unCommPortIndex)
{
    if(((g_unRX_Buff[unCommPortIndex][1]==0)&&(g_unRX_Buff[unCommPortIndex][2]==0))||((g_unRX_Buff[unCommPortIndex][1]==0xFF)&&(g_unRX_Buff[unCommPortIndex][2]==0xFF)))
    {
	g_unRX_Buff[unCommPortIndex][2]=(unsigned char)(g_gRunPara[RP_COMM_ADDR]>>8);///////0X55 �ͺ�   0X00  ��ǰ
	g_unRX_Buff[unCommPortIndex][1]=(unsigned char)(g_gRunPara[RP_COMM_ADDR]);//RdCnt(0xF215);///////0X55 �ͺ�   0X00  ��ǰ 
    }
    else
    {
        g_gRunPara[RP_COMM_ADDR]=(unsigned int)(g_unRX_Buff[unCommPortIndex][2] << 8) + g_unRX_Buff[unCommPortIndex][1];     
        g_ucParaChang |= BIT0;    //վ���ơ�վ��ַ�޸İ�      
    } 	 	 	
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
    g_sTxBuff[0].m_gBuffer[1]=0x03;
    g_sTxBuff[0].m_gBuffer[2]=0x03;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x14;
    g_sTxBuff[0].m_gBuffer[6]=(unsigned char)(g_gRunPara[RP_COMM_ADDR]>>8);///////0X55 �ͺ�   0X00  ��ǰ
    g_sTxBuff[0].m_gBuffer[5]=(unsigned char)(g_gRunPara[RP_COMM_ADDR]);//RdCnt(0xF215);///////0X55 �ͺ�   0X00  ��ǰ
    g_sTxBuff[0].m_gBuffer[7]=g_sTxBuff[0].m_gBuffer[4]+g_sTxBuff[0].m_gBuffer[5]+g_sTxBuff[0].m_gBuffer[6];
    g_sTxBuff[0].m_gBuffer[8]=0x16;
		
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,9);
        return 1;
}
//==============================================================================
//  ��������   : operate15
//  ��������   : ������15������  ���ֻ����������
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate15(unsigned int unCommPortIndex)
{
    unsigned char i;
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
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
//  ��������   : operate16
//  ��������   : ������16������   �޸��ֻ�����Ͳ���
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
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
               
    g_gChangFlag[CHNG_TEL] = ON;       //�绰���뷢���޸�
   
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
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
//  ��������   : operate18
//  ��������   : ������18������    �������ϴ����
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate18(unsigned int unCommPortIndex)
{    	
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
    g_sTxBuff[0].m_gBuffer[1]=0x02;
    g_sTxBuff[0].m_gBuffer[2]=0x02;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x18;
    g_sTxBuff[0].m_gBuffer[5]=g_ucPara_stime;///////0X55 �ͺ�   0X00  ��ǰ
    g_sTxBuff[0].m_gBuffer[6]=g_sTxBuff[0].m_gBuffer[4]+g_sTxBuff[0].m_gBuffer[5];
    g_sTxBuff[0].m_gBuffer[7]=0x16;
		//lcd_ctl=3; 
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,8);
          return 1;
}
//==============================================================================
//  ��������   : operate19
//  ��������   : ������19������   �޸������ϴ����
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate19(unsigned int unCommPortIndex)
{
    g_ucPara_stime = g_unRX_Buff[unCommPortIndex][1];
    if((g_ucPara_stime>48)||(g_ucPara_stime<2))              ////2011-12-13 15:48:08   2-240Сʱ
        g_ucPara_stime = 24;
    g_gChangFlag[CHNG_TEL] = ON;   //�޸ĵ绰����
	
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
    g_sTxBuff[0].m_gBuffer[1]=0x02;
    g_sTxBuff[0].m_gBuffer[2]=0x02;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x19;
    g_sTxBuff[0].m_gBuffer[5]= g_ucPara_stime;///////0X55 �ͺ�   0X00  ��ǰ
    g_sTxBuff[0].m_gBuffer[6]=g_sTxBuff[0].m_gBuffer[4]+g_sTxBuff[0].m_gBuffer[5];
    g_sTxBuff[0].m_gBuffer[7]=0x16;
		//lcd_ctl=3; 
    SendBytes_LED(g_sTxBuff[0].m_gBuffer,8);	
         return 1;
}
//==============================================================================
//  ��������   : operate19
//  ��������   : ������19������   У׼
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operate20(unsigned int unCommPortIndex)
{
    unsigned char i,j;
    int Temp = 0;	
    unsigned int unTemp = 0;
    g_sTxBuff[0].m_gBuffer[0]=0x68;      ////////�źŷ�����
    g_sTxBuff[0].m_gBuffer[1]=0x02;
    g_sTxBuff[0].m_gBuffer[2]=0x02;
    g_sTxBuff[0].m_gBuffer[3]=0x68;
    g_sTxBuff[0].m_gBuffer[4]=0x20;
    g_sTxBuff[0].m_gBuffer[5]= g_unRX_Buff[unCommPortIndex][1];///////0X55 �ͺ�   0X00  ��ǰ
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
        g_gChangFlag[CHNG_ADJ] = ON;  //��У��ϵ���޸ı�־��У����������
    }
    else if(g_sTxBuff[0].m_gBuffer[5] == 2)
    {
        for(j = 0; j < 3; j++)
        {  
            unTemp = g_gProcCnt[j];
            unTemp = (((unsigned long)6000* unTemp) /g_gRmtFilMeas[j + RM_UA]); //�ŏ| Ŀ��У׼����λ�����ز��� ��ʼֵΪ��ѹ60V,����2A
            if(unTemp > 3000 && unTemp < 6000)
                g_gProcCnt[j] = unTemp;                
        }
            unTemp = g_gProcCnt[3];
            unTemp = (((unsigned long)6000 * unTemp) /g_gRmtFilMeas[RM_U0]); //�ŏ| Ŀ��У׼����λ�����ز��� ��ʼֵΪ��ѹ60V,����2A
            if(unTemp > 3000 && unTemp < 6000)
                g_gProcCnt[3] = unTemp;
            unTemp = g_gProcCnt[4];
            unTemp = (((unsigned long)200 * unTemp) /g_gRmtFilMeas[RM_I0]); //�ŏ| Ŀ��У׼����λ�����ز��� ��ʼֵΪ��ѹ60V,����2A
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
//  ��������   : operateFF
//  ��������   : ������06������  �������ң����ң��
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : 1���ɹ�  0�������д���
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int operateFF(unsigned int unCommPortIndex)
{ 
    unsigned char i;
    
    uart0_event_flag++;
    if(uart0_event_flag==4)
        uart0_event_flag=0;
    if(uart0_event_flag==1)    ////////////  ����״̬��
    {
        g_sTxBuff[0].m_gBuffer[5]=0;
 	g_sTxBuff[0].m_gBuffer[6]=0;
 	 	  	 
 	if((phase_copy&BIT0)==BIT0)         ///////A�����       
 	    g_sTxBuff[0].m_gBuffer[5]|=0x01;
 	if((phase_copy&BIT1)==BIT1)      ///////B�����
 	    g_sTxBuff[0].m_gBuffer[5]|=0x02;
 	if((phase_copy&BIT2)==BIT2)     ///////C�����
 	    g_sTxBuff[0].m_gBuffer[5]|=0x04;
    	 	     	 	            	 	 	
        if(eight_pulse_flag>0)    ///////�źŷ�����
            g_sTxBuff[0].m_gBuffer[5]|=0x08;  
 	if(efslatch_flag>0)          ///////����
 	    g_sTxBuff[0].m_gBuffer[5]|=0x10;  
 	if((efslatch_flag==0)&&(eight_pulse_flag==0))         ///////����
 	    g_sTxBuff[0].m_gBuffer[5]|=0x20;
        if(pulse_success==0x55)                     ///////������Ч8����
 	    g_sTxBuff[0].m_gBuffer[5]|=0x40;     	 	  
        if(g_gProcCnt[PC_EFS_MODEL]>0)           ///////////////II�ͻ���·��
        { 
            if(fault_save>0)         	 	
            {
            //i/////////A�൥��ӵع���	
                if(g_ucEarthFlg == 1)	
    	            g_sTxBuff[0].m_gBuffer[6]|=0x11; 	
    	      //////////B�൥��ӵع���	
    	        else if(g_ucEarthFlg == 2)
    	            g_sTxBuff[0].m_gBuffer[6]|=0x12; 	
    	      // /////////C�൥��ӵع���	
    	        else if(g_ucEarthFlg == 3)
    	            g_sTxBuff[0].m_gBuffer[6]|=0x14;	
    	     }
             else if(fault_save==0)	
              /////////����   ����ӵ��о�
                 g_sTxBuff[0].m_gBuffer[6]|=0x08;	
        } 
        else
        {
            if(fault_save>0)	
                g_sTxBuff[0].m_gBuffer[6]|=0x10; 	
    	    else if(fault_save==0)       /////////�ӵع��ϸ���
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
    else                   //////��������
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
        g_sTxBuff[0].m_gBuffer[5]= (unsigned char)(xjj);  //////��·��ַ
        g_sTxBuff[0].m_gBuffer[6]= (unsigned char)(xjj >> 8);   //////��·��ַ
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
        g_sTxBuff[0].m_gBuffer[7]= (unsigned char)(xjj);//(g_gRmtFilMeas[RM_UB]);  //////��·��ַ
        g_sTxBuff[0].m_gBuffer[8]= (unsigned char)(xjj>>8);//(g_gRmtFilMeas[RM_UB] >> 8);   //////��·��ַ
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
        g_sTxBuff[0].m_gBuffer[9]= (unsigned char)(xjj);//(g_gRmtMeas[RM_UC]);  //////��·��ַ
        g_sTxBuff[0].m_gBuffer[10]= (unsigned char)(xjj>>8);//(g_gRmtMeas[RM_UC] >> 8);   //////��·��ַ
          
        g_sTxBuff[0].m_gBuffer[11]= (unsigned char)(g_gRmtFilMeas[RM_U0]);  //////��·��ַ
        g_sTxBuff[0].m_gBuffer[12]= (unsigned char)(g_gRmtFilMeas[RM_U0] >> 8);   //////��·��ַ
          
        g_sTxBuff[0].m_gBuffer[13]= (unsigned char)(g_gRmtFilMeas[RM_I0]);  //////��·��ַ
        g_sTxBuff[0].m_gBuffer[14]= (unsigned char)(g_gRmtFilMeas[RM_I0] >> 8);   //////��·��ַ
	g_sTxBuff[0].m_gBuffer[15]=0;
	for(i=4;i<15;i++)
	    g_sTxBuff[0].m_gBuffer[15]+=g_sTxBuff[0].m_gBuffer[i];	
	g_sTxBuff[0].m_gBuffer[16]=0x16;
        SendBytes_LED(g_sTxBuff[0].m_gBuffer,17);  
      
    }
     return 1;	
}

//==============================================================================
//  ��������   : ReadLEDVer
//  ��������   : ���������ȡҺ���İ汾�ţ����鿪����ȡ
//  �������   :   
//  �������   ��<��>
//  ����ֵ     : 
//  ����˵��   : 
//  ����       ��
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
//  ��������   : RxProcess_101
//  ��������   : ��֡�յ������ݣ����д���
//  �������   : unCommPortIndex��ͨ�Ŷ˿�  
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
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
//  ��������   : Comm_GPRS_101
//  ��������   : GPRSģʽ101Э�������ݴ���
//  �������   : <��> 
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void Comm_LED_101(void)
{
   // unsigned int unTime_GPRS;  //GPRS���ʱ���ʱ��
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