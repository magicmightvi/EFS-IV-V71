/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  ģ����     : EEPROM����ģ��
  �ļ���     : DriverEEPROM.c
  ����ļ�       : DataStruct.h FuncDef.h Constant.h
  ʵ�ֹ���       : ʵ��EEPROMоƬ����������������д�������Ȳ�����ͨ��I2C���߷���
  ����       :  
  �汾       : 1.0
  
  --------------------------------------------------------------------------------
  ��ע       : -
  --------------------------------------------------------------------------------
  
  �޸ļ�¼       : 
    ��  ��      �汾        �޸���      �޸����� 
    2010/06/01      1.0                  ����

************************************************************************************/
#ifndef _DRIVER_EEPROM

#define _DRIVER_EEPROM
  #include "..\DataStruct.h"
#endif


#define CAT_UNSELECT_CHIP    P9OUT |= BIT0
#define CAT_SELECT_CHIP      P9OUT &= ~BIT0

#define CAT_WP               P8OUT &= ~BIT7
#define CAT_NOWP             P8OUT |= BIT7

#define  WREN   0x06      //ȡ��д����
#define  WRDI   0x04      //ʹ��д����
#define  RDSR   0x05      //��״̬�Ĵ���
#define  WRSR   0x01      //д״̬�Ĵ���
#define  READ   0x03      //������
#define  WRITE  0x02      //д����

//==============================================================================
//  ��������   : BCDtoINT()
//  ��������   : BCD��תʮ����
//  �������   : unsigned char value
//  �������   ��<��>
//  ����ֵ     : ucReturnVal
//  ����       ��
//==============================================================================
unsigned char BCDtoINT(unsigned char ucValue)
{
    unsigned char ucTempH;
    unsigned char ucTempL;
    unsigned char ucReturnVal;
    ucTempL = (ucValue & 0x0F);
    ucTempH = (ucValue & 0xF0) >> 4;  
    ucReturnVal = (ucTempH * 10) + ucTempL;
    return ucReturnVal;
}
/*
//==============================================================================
//  ��������   : EEP_WriteWords
//  ��������   : ʮ����תBCD��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned char INTtoBCD(unsigned char ucValue)// 
{
    unsigned char ucTempH;
    unsigned char ucTempL;
    unsigned char ucReturnVal;
    ucTempL = (ucValue % 10);
    ucTempH = (ucValue / 10); 
    ucReturnVal = ucTempL | (ucTempH << 4);
    return ucReturnVal;
}

*/
//==============================================================================
//  ��������   : UCB0SPIInit
//  ��������   : UCB0SPI���߳�ʼ��������
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void UCB0SPIInit(void)
{

   // P8DIR |= BIT7;            //EEPROMд�������   
 
    UCB0CTL1 |= UCSWRST;                          // **Put state machine in reset** 
    UCB0CTL1 |= UCSSEL_1;                         // ACLK
    UCB0CTL0 |= UCMST + UCSYNC + UCCKPH + UCMSB;   // 3-pin, 8-bit SPI master  + UCCKPL
                                                  // Clock polarity high, MSB
   
    UCB0BR0 = 0x02;                               // /4
    UCB0BR1 = 0;                                  
           
    UCB0CTL1 &= ~UCSWRST;                         // **Initialize USCI state machine**    
  //  CAT_UNSELECT_CHIP;                            //CAT25128Ƭѡ��ʹ��
  //  CAT_WP;                                       //CAT25128д����    
  //  CAT_SpiConcelWP();                            //25128дʹ��
}
//==============================================================================
//  ��������   : UCB0SPIInit
//  ��������   : UCB0SPI���߳�ʼ��������
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void UCB2SPIInit(void)
{

   // P8DIR |= BIT7;            //EEPROMд�������   
 
    UCB2CTL1 |= UCSWRST;                          // **Put state machine in reset**   
    UCB2CTL0 |= UCMST + UCSYNC + UCCKPL + UCMSB;  // 3-pin, 8-bit SPI master
                                                  // Clock polarity high, MSB
    UCB2CTL1 |= UCSSEL_1;                         // ACLK
    UCB2BR0 = 0x04;                               // /4
    UCB2BR1 = 0;                                  
           
    UCB2CTL1 &= ~UCSWRST;                         // **Initialize USCI state machine**    
    CAT_UNSELECT_CHIP;                            //CAT25128Ƭѡ��ʹ��
    CAT_WP;                                       //CAT25128д����    
    CAT_SpiConcelWP();                            //25128дʹ��
    
    Sst_Reset();                                  //��λFlashоƬ
    Sst_Unlock();                                 //����FlashоƬ
}
//==============================================================================
//  ��������   : SPIReadBuf
//  ��������   : SPI����һ�ֽ�����
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
static unsigned char CAT_SPIWriteBuf(unsigned char ucData)
{
    unsigned char i;
    UCB2TXBUF = ucData;
     for( i = 0; i < 255; i++ )
    {
         if((UCB2STAT&UCBUSY)==0)
        {    
            break;
        }
    }
    return UCB2TXBUF;
}
//==============================================================================
//  ��������   : SPIReadBuf
//  ��������   : SPI����һ�ֽ�����
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
static unsigned char CAT_SPIReadBuf(void)
{
    unsigned char i;
    CAT_SPIWriteBuf(0);
    for( i = 0; i < 250; i++ )
    {
        if((UCB2STAT&UCBUSY)==0)
        {    
            break;
        }
    }
    return UCB2RXBUF;
}


//==============================================================================
//  ��������   : SpiWaitBusy
//  ��������   : ͨ����״̬�Ĵ��������æ״̬
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
static void CAT_SpiWaitBusy(void)
{
    char ucTmp = 0;
    unsigned char i = 0;
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SpiConcelWP();
    CAT_SPIWriteBuf(RDSR);
    while(1)
    {
        ucTmp = CAT_SPIReadBuf();
        if((ucTmp&0x01) == 0 || i >= 250)    //
        break;
        i++;
    }
    CAT_UNSELECT_CHIP;
    CAT_WP;
}
//==============================================================================
//  ��������   : SpiWaitBusy
//  ��������   : ȡ��д����
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void CAT_SpiConcelWP(void)
{
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(WREN);
    CAT_UNSELECT_CHIP;
    CAT_WP;
}

//==============================================================================
//  ��������   : CAT_SpiWriteByte
//  ��������   : �ֽ�д
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================

void CAT_SpiWriteByte(unsigned int unAddr,unsigned char ucData)
{
    CAT_SpiWaitBusy();
    CAT_SpiConcelWP();
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(WRITE);
    CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
    CAT_SPIWriteBuf((unsigned char)unAddr);
    CAT_SPIWriteBuf(ucData);
    CAT_UNSELECT_CHIP;
    CAT_WP;
    delay(13000);    //25128����д����ʱ���5ms
}


void CAT_SpiWriteBytes(unsigned int unAddr,unsigned int unLength,unsigned char *Data)
{
    unsigned char i;
    unsigned int unAddTmp;
   
    while(unLength)
    {   
        unAddTmp   = 64 - (unAddr % 64);
        if(unAddTmp > unLength)
        {
            CAT_SpiWaitBusy();
            CAT_SpiConcelWP();
            CAT_NOWP;
            CAT_SELECT_CHIP;
            CAT_SPIWriteBuf(WRITE);
            CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
            CAT_SPIWriteBuf((unsigned char)unAddr);
            for(i = 0;i < unLength; i++)
            {  
                CAT_SPIWriteBuf(*Data);
                Data++;
            }     
            CAT_UNSELECT_CHIP; 
            CAT_WP;
            unLength = 0;
            delay(13000);    //25128����д����ʱ���5ms
        }
        else
       {
            CAT_SpiWaitBusy();
            CAT_SpiConcelWP();
            CAT_NOWP;
            CAT_SELECT_CHIP;
            CAT_SPIWriteBuf(WRITE);
            CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
            CAT_SPIWriteBuf((unsigned char)unAddr);
            for(i = 0;i < unAddTmp ; i++)
            {  
                CAT_SPIWriteBuf(*Data);
                Data++;
            }
            unAddr  += unAddTmp;
            unLength -= unAddTmp;
            CAT_UNSELECT_CHIP; 
            CAT_WP;
            delay(13000);                 
        }

    }
}

 

//==============================================================================
//  ��������   : CAT_SpiWriteWord
//  ��������   : ��д
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================

void CAT_SpiWriteWord(unsigned int unAddr,unsigned int unData)
{ 

    CAT_SpiWaitBusy();
    CAT_SpiConcelWP();
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(WRITE);
    CAT_SPIWriteBuf((unsigned char)((unAddr>>8)));
    CAT_SPIWriteBuf((unsigned char)unAddr);

    CAT_SPIWriteBuf((unsigned char)(unData>>8));
    CAT_SPIWriteBuf((unsigned char)unData);
    CAT_UNSELECT_CHIP;
    CAT_WP;
    delay(13000);    //25128����д����ʱ���5ms
}
//==============================================================================
//  ��������   : CAT_SpiWriteWords
//  ��������   : ����д
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void CAT_SpiWriteWords(unsigned int unAddr,unsigned int unLength,unsigned int *Data)
{
    unsigned char i;
    unsigned int unAddTmp;
    unsigned int unData;
    
    while(unLength)
    {   
        unAddTmp   = 64 - (unAddr % 64);
        if(unAddTmp > (unLength*2))
        {
            CAT_SpiWaitBusy();
            CAT_SpiConcelWP();
            CAT_NOWP;
            CAT_SELECT_CHIP;
            CAT_SPIWriteBuf(WRITE);
            CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
            CAT_SPIWriteBuf((unsigned char)unAddr);
            for(i = 0;i < unLength; i++)
            {  
                unData = *Data;
                unData >>= 8;
                CAT_SPIWriteBuf((unsigned char)unData);
                CAT_SPIWriteBuf((unsigned char)*Data);
                Data++;
            }     
            CAT_UNSELECT_CHIP; 
            CAT_WP;
            unLength = 0;
            delay(13000);    //25128����д����ʱ���5ms
        }
        else
        {
            CAT_SpiWaitBusy();
            CAT_SpiConcelWP();
            CAT_NOWP;
            CAT_SELECT_CHIP;
            CAT_SPIWriteBuf(WRITE);
            CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
            CAT_SPIWriteBuf((unsigned char)unAddr);
            for(i = 0;i < (unAddTmp>>1); i++)
            {  
                unData = *Data;
                unData >>= 8;
                CAT_SPIWriteBuf((unsigned char)unData);
                CAT_SPIWriteBuf((unsigned char)*Data);
                Data++;
            }
            unAddr += unAddTmp;
            unLength  = unLength -(unAddTmp>>1);
            CAT_UNSELECT_CHIP; 
            CAT_WP;
            delay(13000);   //25128����д����ʱ���5ms                
        }
    }
}


//==============================================================================
//  ��������   : CAT_SpiReadByte
//  ��������   : �ֽڶ�
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================

unsigned char CAT_SpiReadByte(unsigned int unAddr)
{ 
    unsigned char ucData; 
    CAT_SpiWaitBusy();
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(READ);
    CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
    CAT_SPIWriteBuf((unsigned char)unAddr);
    ucData = CAT_SPIReadBuf();
    CAT_UNSELECT_CHIP; 
    CAT_WP;
    return(ucData);
}

//==============================================================================
//  ��������   : CAT_SpiReadBytes
//  ��������   : �ֽ�����
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void CAT_SpiReadBytes(unsigned int unAddr,unsigned int unLength,unsigned char *Data)
{
    unsigned char i; 
    CAT_SpiWaitBusy();
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(READ);
    CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
    CAT_SPIWriteBuf((unsigned char)unAddr);
    for(i = 0;i < unLength; i++)
    {  
        *Data = CAT_SPIReadBuf();
        Data++;
    }     
    CAT_UNSELECT_CHIP;
    CAT_WP;
}

//==============================================================================
//  ��������   : CAT_SpiReadWord
//  ��������   : �ֶ�
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================

unsigned int CAT_SpiReadWord(unsigned int unAddr)
{ 
    unsigned int unHIData; 
    unsigned int unLOData;
    CAT_SpiWaitBusy();
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(READ);
    CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
    CAT_SPIWriteBuf((unsigned char)unAddr);
    unHIData = CAT_SPIReadBuf();
    unHIData = unHIData<<8;
    unLOData = CAT_SPIReadBuf();
    unHIData |= unLOData;
    CAT_UNSELECT_CHIP; 
    CAT_WP;
    return(unHIData);
}

//==============================================================================
//  ��������   : CAT_SpiReadWords
//  ��������   : ���ֶ�
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void CAT_SpiReadWords(unsigned int unAddr,unsigned int unLength,unsigned int *Data)
{
    unsigned char i; 
    unsigned int unData = 0; 
    CAT_SpiWaitBusy();
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(READ);
    CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
    CAT_SPIWriteBuf((unsigned char)unAddr);
    for(i = 0;i < unLength; i++)
    {  
        unData = CAT_SPIReadBuf();
        *Data = CAT_SPIReadBuf(); 
        *Data = (unData <<8)|(*Data);
        Data++;
        unData = 0;
    }     
    CAT_UNSELECT_CHIP; 
    CAT_WP;
}

/*
//==============================================================================
//  ��������   : UCB2IICInit
//  ��������   : UCB2IIC���߳�ʼ��������
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void UCB2IICInit(void)
{
  
  
    P9REN = 0xff;
    IIC_DIR |= IIC_SCL;  //SCL�ܽ�Ϊ���
    IIC_DIR |= IIC_SDA; //SDA�ܽ�Ϊ����
    CLR_SCL;

}
//==============================================================================
//  ��������   : StartBus
//  ��������   : ��������
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================

static void StartBus(void)
{
    SDA_OUT;
    SET_SDA;
    delay(5);
    SET_SCL;
    delay(5);
    CLR_SDA;
    delay(5);
    CLR_SCL;
    delay(5);
}

//==============================================================================
//  ��������   : StopBus
//  ��������   : ���߽���
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================

static void StopBus(void)
{
   // SDA_OUT;
  //  CLR_SCL;
  //  delay(5);
    CLR_SDA;
    delay(5);
    SET_SCL;
    delay(5);
    SET_SDA;
    delay(15);
  //  SDA_OUT;
  //  SET_SDA;
 // while(READ_SDA == 0);
}

//==============================================================================
//  ��������   : AckBus
//  ��������   : ������Ӧ��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================

static void AckBus(void)
{
    delay(5);
    SDA_OUT;
    CLR_SDA;
    delay(5);
    SET_SCL;
    delay(5);
    //SET_SDA;
    CLR_SCL;
 //   SDA_OUT;
  //  SET_SDA;

}

//==============================================================================
//  ��������   : NoAckBus
//  ��������   : ��������Ӧ��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================

static void NoAckBus(void)
{
    SDA_OUT;
    SET_SDA;
    delay(5);
    SET_SCL;
    delay(5);
    CLR_SCL;
    delay(5);
}

//==============================================================================
//  ��������   : TestAckBus
//  ��������   : ��������Ӧ���ź�
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================

static void TestAckBus(void)
{
    unsigned char i = 0;
    SET_SCL;
    
    SDA_IN;
    delay(5);
    while(READ_SDA == 2 && i<255)
      i++;
    delay(5);
    CLR_SCL;
    delay(5);
}
//==============================================================================
//  ��������   :  WriteByte
//  ��������   : ��IIC����дһ���ֽ�
//  �������   : д���һ���ֽ���
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================

static void WriteByte(unsigned char ucInputNum)
{
    unsigned char i;
    unsigned char ucTemp = 0;
    SDA_OUT;
    for(i = 8;i != 0;i--)
    {
    ucTemp = ucInputNum & 0x80;
    if( ucTemp == 0x80 )
        SET_SDA;
    else
        CLR_SDA;
    delay(5);
    SET_SCL;
    delay(5);
    CLR_SCL;
    delay(5);
    ucInputNum = ucInputNum << 1;
    }
     CLR_SDA;
}
//==============================================================================
//  ��������   : ReadByte
//  ��������   : ��IIC������һ���ֽ�
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : ������һ�ֽ���
//  ����˵��   : 
//  ����       ��
//==============================================================================
static unsigned char ReadByte()
{
    unsigned char i;
    unsigned char ucTemp = 0;
    unsigned char ucRbyte = 0; 
    SDA_IN;
    for(i = 8;i != 0;i--)
    {
    SET_SCL;
    delay(5);
    ucRbyte = ucRbyte<<1;
    SDA_IN;
    ucTemp = READ_SDA;
    if(ucTemp == IIC_SDA)
    ucRbyte = ucRbyte|0x01;
    delay(5);
    CLR_SCL;
    delay(5);
    }
    return(ucRbyte);
}
//==============================================================================
//  ��������   : ReadWord
//  ��������   : ��IIC�����ض���ַ����һ�ֽ���
//  �������   : ��ַ�� unAddr  �����ӻ���ַ��unSlaveAddr
//  �������   ��<��>
//  ����ֵ     : ����������
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned char ReadWord(unsigned int  unAddr, unsigned int unSlaveAddr)
{
    unsigned char ucLowAdd  = 0;
    unsigned char ucTempData = 0;
    ucLowAdd  = (unsigned char)unAddr;
    SDA_OUT;
    StartBus();
    WriteByte( unSlaveAddr|0x00);
    TestAckBus();
   // WriteByte(ucHighAdd);
   // TestAckBus();
    WriteByte(ucLowAdd);
    TestAckBus();
    StartBus();
    WriteByte(unSlaveAddr|0x01);
    TestAckBus();
    ucTempData  = ReadByte();
    
    StopBus();
    delay(5);  
    return(ucTempData); 

}

//==============================================================================
//  ��������   : WriteWord
//  ��������   : ��IIC�����ض���ַдһ�ֽ���
//  �������   : ��ַ�� unAddr  �����ӻ���ַ��unSlaveAddr
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void WriteWord(unsigned int unAddr, unsigned int unSlaveAddr, unsigned char ucWriteData)
{
    unsigned char ucLowAdd  = 0;

    ucLowAdd  = (unsigned char)unAddr;
    StartBus();
    WriteByte(unSlaveAddr|0x00);
    TestAckBus();
   // WriteByte(ucHighAdd);
   // TestAckBus();
    WriteByte(ucLowAdd);
    TestAckBus();  
    WriteByte(ucWriteData);
    TestAckBus();
    
   // delay(10);
    StopBus();
    //delay(2000);
}

//==============================================================================
//  ��������   : EEP_ReadBytes
//  ��������   : ��RTC8563�����ض���ַ����һ��8λ����
//  �������   : ��ַ�� unAddr  ���ݳ��ȣ�unLength ���������ݴ��룺*Data
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void EEP_ReadBytes(unsigned int unAddr,unsigned int unLength,unsigned char *Data)
{
    unsigned char i  = 0;
    unsigned char ucLowAdd  = 0;
    ucLowAdd  = (unsigned char)unAddr;
    SDA_OUT;
    StartBus();
    WriteByte(0xa2);
    TestAckBus();
    WriteByte(ucLowAdd);
    TestAckBus();
    StartBus();
    WriteByte(0xa3);
    TestAckBus();
    for(i = 0; i < unLength-1; i++)
    {
        *Data = ReadByte();
        delay(5);
        AckBus();
        Data++;
        delay(10);
    }
    *Data = ReadByte();
    NoAckBus();
    delay(15);
    StopBus();
    delay(15);
 
}
//==============================================================================
//  ��������   : EEP_WriteBytes
//  ��������   : ��RTC�����ض���ַд��һ��8λ����
//  �������   : ��ַ�� unAddr  ���ݳ��ȣ�unLength д������ݣ�*Data
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void EEP_WriteBytes(unsigned int unAddr, unsigned int unLength,unsigned int unSlaveAddr, unsigned char *Data)
{
    unsigned char ucLowAdd  = 0;
    //unsigned char ucHighAdd = 0;
    unsigned char i  = 0;
    ucLowAdd  = (unsigned char)unAddr;
    StartBus();
    WriteByte(unSlaveAddr|0x00);
    TestAckBus();
  //  WriteByte(ucHighAdd);
   // TestAckBus();
    WriteByte(ucLowAdd);
    TestAckBus();
    for(i = 0; i < unLength; i++)
    {  
        WriteByte(*Data);
        TestAckBus();
        Data++;
    }    
   // delay(50);
    StopBus();
    delay(1000);
}

//==============================================================================
//  ��������   : BCDtoINT()
//  ��������   : BCD��תʮ����
//  �������   : unsigned char value
//  �������   ��<��>
//  ����ֵ     : ucReturnVal
//  ����       ��
//==============================================================================
unsigned char BCDtoINT(unsigned char ucValue)
{
    unsigned char ucTempH;
    unsigned char ucTempL;
    unsigned char ucReturnVal;
    ucTempL = (ucValue & 0x0F);
    ucTempH = (ucValue & 0xF0) >> 4;  
    ucReturnVal = (ucTempH * 10) + ucTempL;
    return ucReturnVal;
}
//==============================================================================
//  ��������   : EEP_WriteWords
//  ��������   : ʮ����תBCD��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned char INTtoBCD(unsigned char ucValue)// 
{
    unsigned char ucTempH;
    unsigned char ucTempL;
    unsigned char ucReturnVal;
    ucTempL = (ucValue % 10);
    ucTempH = (ucValue / 10); 
    ucReturnVal = ucTempL | (ucTempH << 4);
    return ucReturnVal;
}


//==============================================================================
//  ��������   : UCB0SPIInit
//  ��������   : UCB0SPI���߳�ʼ��������
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void UCB0SPIInit(void)
{
    P3REN = 0xff;
    P3SEL |= 0x0e;                  // P3.1,2,3, option select
//    P2SEL &= ~BIT0;
//    P2DIR |= BIT0;
    P3SEL &= ~BIT6;
    P3DIR |= BIT6;
    
    P3DIR |= BIT7;
    P7DIR |= BIT3;            //EEPROMд�������   
    P7DIR |= BIT2;            //Flashд�������


  
    UCB0CTL1 |= UCSWRST;                          // **Put state machine in reset**   
    UCB0CTL0 |= UCMST + UCSYNC + UCCKPL + UCMSB;  // 3-pin, 8-bit SPI master
                                                  // Clock polarity high, MSB
//    UCB0CTL1 |= UCSSEL_1;                         // ACLK
//    UCB0BR0 = 0x04;                               // /4
    
    UCB0CTL1 |= UCSSEL_2;                       // MCLK
    UCB0BR0 = 0x02;                     // /2
    UCB0BR1 = 0;                                  
           
    UCB0CTL1 &= ~UCSWRST;                         // **Initialize USCI state machine**
    
    UNSELECT_CHIP;                                //ad45dbƬѡ��ʹ��
    CAT_UNSELECT_CHIP;                            //CAT25128Ƭѡ��ʹ��
    CAT_WP;                                       //CAT25128д����
    AT45DB_NOWP;                                    //AT45DB624д����
    
    CAT_SpiConcelWP();                            //25128дʹ��

	
    Sst_Reset();                                  //��λFlashоƬ
    Sst_Unlock();                                 //����FlashоƬ
}

//==============================================================================
//  ��������   : SPIReadBuf
//  ��������   : SPI����һ�ֽ�����
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
static unsigned char CAT_SPIWriteBuf(unsigned char ucData)
{
    unsigned char i;
    UCB0TXBUF = ucData;
     for( i = 0; i < 255; i++ )
    {
         if((UCB0STAT&UCBUSY)==0)
        {    
            break;
        }
    }
    return UCB0TXBUF;
}
//==============================================================================
//  ��������   : SPIReadBuf
//  ��������   : SPI����һ�ֽ�����
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
static unsigned char CAT_SPIReadBuf(void)
{
    unsigned char i;
    CAT_SPIWriteBuf(0);
    for( i = 0; i < 250; i++ )
    {
        if((UCB0STAT&UCBUSY)==0)
        {    
            break;
        }
    }
    return UCB0RXBUF;
}


//==============================================================================
//  ��������   : SpiWaitBusy
//  ��������   : ͨ����״̬�Ĵ��������æ״̬
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
static void CAT_SpiWaitBusy(void)
{
    char ucTmp = 0;
    unsigned char i = 0;
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SpiConcelWP();
    CAT_SPIWriteBuf(RDSR);
    while(1)
    {
        ucTmp = CAT_SPIReadBuf();
        if((ucTmp&0x01) == 0 || i >= 250)    //
        break;
        i++;
    }
    CAT_UNSELECT_CHIP;
    CAT_WP;
}
//==============================================================================
//  ��������   : SpiWaitBusy
//  ��������   : ȡ��д����
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void CAT_SpiConcelWP(void)
{
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(WREN);
    CAT_UNSELECT_CHIP;
    CAT_WP;
}

//==============================================================================
//  ��������   : CAT_SpiWriteByte
//  ��������   : �ֽ�д
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void CAT_SpiWriteByte(unsigned int unAddr,unsigned char ucData)
{
    CAT_SpiWaitBusy();
    CAT_SpiConcelWP();
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(WRITE);
    CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
    CAT_SPIWriteBuf((unsigned char)unAddr);
    CAT_SPIWriteBuf(ucData);
    CAT_UNSELECT_CHIP;
    CAT_WP;
    delay(13000);    //25128����д����ʱ���5ms
}


void CAT_SpiWriteBytes(unsigned int unAddr,unsigned int unLength,unsigned char *Data)
{
    unsigned char i;
    unsigned int unAddTmp;
   
    while(unLength)
    {   
        unAddTmp   = 64 - (unAddr % 64);
        if(unAddTmp > unLength)
        {
            CAT_SpiWaitBusy();
            CAT_SpiConcelWP();
            CAT_NOWP;
            CAT_SELECT_CHIP;
            CAT_SPIWriteBuf(WRITE);
            CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
            CAT_SPIWriteBuf((unsigned char)unAddr);
            for(i = 0;i < unLength; i++)
            {  
                CAT_SPIWriteBuf(*Data);
                Data++;
            }     
            CAT_UNSELECT_CHIP; 
            CAT_WP;
            unLength = 0;
            delay(13000);    //25128����д����ʱ���5ms
        }
        else
       {
            CAT_SpiWaitBusy();
            CAT_SpiConcelWP();
            CAT_NOWP;
            CAT_SELECT_CHIP;
            CAT_SPIWriteBuf(WRITE);
            CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
            CAT_SPIWriteBuf((unsigned char)unAddr);
            for(i = 0;i < unAddTmp ; i++)
            {  
                CAT_SPIWriteBuf(*Data);
                Data++;
            }
            unAddr  += unAddTmp;
            unLength -= unAddTmp;
            CAT_UNSELECT_CHIP; 
            CAT_WP;
            delay(13000);                 
        }

    }
}

 

//==============================================================================
//  ��������   : CAT_SpiWriteWord
//  ��������   : ��д
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void CAT_SpiWriteWord(unsigned int unAddr,unsigned int unData)
{ 

    CAT_SpiWaitBusy();
    CAT_SpiConcelWP();
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(WRITE);
    CAT_SPIWriteBuf((unsigned char)((unAddr>>8)));
    CAT_SPIWriteBuf((unsigned char)unAddr);

    CAT_SPIWriteBuf((unsigned char)(unData>>8));
    CAT_SPIWriteBuf((unsigned char)unData);
    CAT_UNSELECT_CHIP;
    CAT_WP;
    delay(13000);    //25128����д����ʱ���5ms
}
//==============================================================================
//  ��������   : CAT_SpiWriteWords
//  ��������   : ����д
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void CAT_SpiWriteWords(unsigned int unAddr,unsigned int unLength,unsigned int *Data)
{
    unsigned char i;
    unsigned int unAddTmp;
    unsigned int unData;
    
    while(unLength)
    {   
      unAddTmp   = 64 - (unAddr % 64);
        if(unAddTmp > (unLength*2))
        {
            CAT_SpiWaitBusy();
            CAT_SpiConcelWP();
            CAT_NOWP;
            CAT_SELECT_CHIP;
            CAT_SPIWriteBuf(WRITE);
            CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
            CAT_SPIWriteBuf((unsigned char)unAddr);
            for(i = 0;i < unLength; i++)
            {  
                unData = *Data;
                unData >>= 8;
                CAT_SPIWriteBuf((unsigned char)unData);
                CAT_SPIWriteBuf((unsigned char)*Data);
                Data++;
            }     
            CAT_UNSELECT_CHIP; 
            CAT_WP;
            unLength = 0;
            delay(13000);    //25128����д����ʱ���5ms
        }
        else
        {
            CAT_SpiWaitBusy();
            CAT_SpiConcelWP();
            CAT_NOWP;
            CAT_SELECT_CHIP;
            CAT_SPIWriteBuf(WRITE);
            CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
            CAT_SPIWriteBuf((unsigned char)unAddr);
            for(i = 0;i < (unAddTmp>>1); i++)
            {  
                unData = *Data;
                unData >>= 8;
                CAT_SPIWriteBuf((unsigned char)unData);
                CAT_SPIWriteBuf((unsigned char)*Data);
                Data++;
            }
            unAddr += unAddTmp;
            unLength  = unLength -(unAddTmp>>1);
            CAT_UNSELECT_CHIP; 
            CAT_WP;
            delay(13000);   //25128����д����ʱ���5ms                
        }
    }
}


//==============================================================================
//  ��������   : CAT_SpiReadByte
//  ��������   : �ֽڶ�
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned char CAT_SpiReadByte(unsigned int unAddr)
{ 
    unsigned char ucData; 
    CAT_SpiWaitBusy();
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(READ);
    CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
    CAT_SPIWriteBuf((unsigned char)unAddr);
    ucData = CAT_SPIReadBuf();
    CAT_UNSELECT_CHIP; 
    CAT_WP;
    return(ucData);
}

//==============================================================================
//  ��������   : CAT_SpiReadBytes
//  ��������   : �ֽ�����
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void CAT_SpiReadBytes(unsigned int unAddr,unsigned int unLength,unsigned char *Data)
{
    unsigned char i; 
    CAT_SpiWaitBusy();
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(READ);
    CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
    CAT_SPIWriteBuf((unsigned char)unAddr);
    for(i = 0;i < unLength; i++)
    {  
        *Data = CAT_SPIReadBuf();
        Data++;
    }     
    CAT_UNSELECT_CHIP;
    CAT_WP;
}

//==============================================================================
//  ��������   : CAT_SpiReadWord
//  ��������   : �ֶ�
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned int CAT_SpiReadWord(unsigned int unAddr)
{ 
    unsigned int unHIData; 
    unsigned int unLOData;
    CAT_SpiWaitBusy();
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(READ);
    CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
    CAT_SPIWriteBuf((unsigned char)unAddr);
    unHIData = CAT_SPIReadBuf();
    unHIData = unHIData<<8;
    unLOData = CAT_SPIReadBuf();
    unHIData |= unLOData;
    CAT_UNSELECT_CHIP; 
    CAT_WP;
    return(unHIData);
}

//==============================================================================
//  ��������   : CAT_SpiReadWords
//  ��������   : ���ֶ�
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void CAT_SpiReadWords(unsigned int unAddr,unsigned int unLength,unsigned int *Data)
{
    unsigned char i; 
    unsigned int unData = 0; 
    CAT_SpiWaitBusy();
    CAT_NOWP;
    CAT_SELECT_CHIP;
    CAT_SPIWriteBuf(READ);
    CAT_SPIWriteBuf((unsigned char)(unAddr>>8));
    CAT_SPIWriteBuf((unsigned char)unAddr);
    for(i = 0;i < unLength; i++)
    {  
        unData = CAT_SPIReadBuf();
        *Data = CAT_SPIReadBuf(); 
        *Data = (unData <<8)|(*Data);
        Data++;
        unData = 0;
    }     
    CAT_UNSELECT_CHIP; 
    CAT_WP;
}
*/