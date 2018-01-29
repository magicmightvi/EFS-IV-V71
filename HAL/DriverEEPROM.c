/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  模块名     : EEPROM驱动模块
  文件名     : DriverEEPROM.c
  相关文件       : DataStruct.h FuncDef.h Constant.h
  实现功能       : 实现EEPROM芯片的驱动，包括读、写、擦除等操作，通过I2C总线访问
  作者       :  
  版本       : 1.0
  
  --------------------------------------------------------------------------------
  备注       : -
  --------------------------------------------------------------------------------
  
  修改记录       : 
    日  期      版本        修改人      修改内容 
    2010/06/01      1.0                  创建

************************************************************************************/
#ifndef _DRIVER_EEPROM

#define _DRIVER_EEPROM
  #include "..\DataStruct.h"
#endif


#define CAT_UNSELECT_CHIP    P9OUT |= BIT0
#define CAT_SELECT_CHIP      P9OUT &= ~BIT0

#define CAT_WP               P8OUT &= ~BIT7
#define CAT_NOWP             P8OUT |= BIT7

#define  WREN   0x06      //取消写保护
#define  WRDI   0x04      //使能写保护
#define  RDSR   0x05      //读状态寄存器
#define  WRSR   0x01      //写状态寄存器
#define  READ   0x03      //读数据
#define  WRITE  0x02      //写数据

//==============================================================================
//  函数名称   : BCDtoINT()
//  功能描述   : BCD码转十进制
//  输入参数   : unsigned char value
//  输出参数   ：<无>
//  返回值     : ucReturnVal
//  作者       ：
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
//  函数名称   : EEP_WriteWords
//  功能描述   : 十进制转BCD码
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : UCB0SPIInit
//  功能描述   : UCB0SPI总线初始化工作，
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void UCB0SPIInit(void)
{

   // P8DIR |= BIT7;            //EEPROM写保护输出   
 
    UCB0CTL1 |= UCSWRST;                          // **Put state machine in reset** 
    UCB0CTL1 |= UCSSEL_1;                         // ACLK
    UCB0CTL0 |= UCMST + UCSYNC + UCCKPH + UCMSB;   // 3-pin, 8-bit SPI master  + UCCKPL
                                                  // Clock polarity high, MSB
   
    UCB0BR0 = 0x02;                               // /4
    UCB0BR1 = 0;                                  
           
    UCB0CTL1 &= ~UCSWRST;                         // **Initialize USCI state machine**    
  //  CAT_UNSELECT_CHIP;                            //CAT25128片选不使能
  //  CAT_WP;                                       //CAT25128写保护    
  //  CAT_SpiConcelWP();                            //25128写使能
}
//==============================================================================
//  函数名称   : UCB0SPIInit
//  功能描述   : UCB0SPI总线初始化工作，
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void UCB2SPIInit(void)
{

   // P8DIR |= BIT7;            //EEPROM写保护输出   
 
    UCB2CTL1 |= UCSWRST;                          // **Put state machine in reset**   
    UCB2CTL0 |= UCMST + UCSYNC + UCCKPL + UCMSB;  // 3-pin, 8-bit SPI master
                                                  // Clock polarity high, MSB
    UCB2CTL1 |= UCSSEL_1;                         // ACLK
    UCB2BR0 = 0x04;                               // /4
    UCB2BR1 = 0;                                  
           
    UCB2CTL1 &= ~UCSWRST;                         // **Initialize USCI state machine**    
    CAT_UNSELECT_CHIP;                            //CAT25128片选不使能
    CAT_WP;                                       //CAT25128写保护    
    CAT_SpiConcelWP();                            //25128写使能
    
    Sst_Reset();                                  //复位Flash芯片
    Sst_Unlock();                                 //解锁Flash芯片
}
//==============================================================================
//  函数名称   : SPIReadBuf
//  功能描述   : SPI发送一字节数据
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : SPIReadBuf
//  功能描述   : SPI接收一字节数据
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : SpiWaitBusy
//  功能描述   : 通过读状态寄存器来检测忙状态
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : SpiWaitBusy
//  功能描述   : 取消写保护
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : CAT_SpiWriteByte
//  功能描述   : 字节写
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
    delay(13000);    //25128数据写入延时最大5ms
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
            delay(13000);    //25128数据写入延时最大5ms
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
//  函数名称   : CAT_SpiWriteWord
//  功能描述   : 字写
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
    delay(13000);    //25128数据写入延时最大5ms
}
//==============================================================================
//  函数名称   : CAT_SpiWriteWords
//  功能描述   : 多字写
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
            delay(13000);    //25128数据写入延时最大5ms
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
            delay(13000);   //25128数据写入延时最大5ms                
        }
    }
}


//==============================================================================
//  函数名称   : CAT_SpiReadByte
//  功能描述   : 字节读
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : CAT_SpiReadBytes
//  功能描述   : 字节流读
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : CAT_SpiReadWord
//  功能描述   : 字读
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : CAT_SpiReadWords
//  功能描述   : 多字读
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : UCB2IICInit
//  功能描述   : UCB2IIC总线初始化工作，
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void UCB2IICInit(void)
{
  
  
    P9REN = 0xff;
    IIC_DIR |= IIC_SCL;  //SCL管脚为输出
    IIC_DIR |= IIC_SDA; //SDA管脚为输入
    CLR_SCL;

}
//==============================================================================
//  函数名称   : StartBus
//  功能描述   : 总线启动
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : StopBus
//  功能描述   : 总线结束
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : AckBus
//  功能描述   : 主器件应答
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : NoAckBus
//  功能描述   : 主器件不应答
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : TestAckBus
//  功能描述   : 检测从器件应答信号
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   :  WriteByte
//  功能描述   : 向IIC器件写一个字节
//  输入参数   : 写入的一个字节数
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : ReadByte
//  功能描述   : 向IIC器件读一个字节
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : 读出的一字节数
//  其他说明   : 
//  作者       ：
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
//  函数名称   : ReadWord
//  功能描述   : 从IIC器件特定地址读出一字节数
//  输入参数   : 地址： unAddr  器件从机地址：unSlaveAddr
//  输出参数   ：<无>
//  返回值     : 读出的数据
//  其他说明   : 
//  作者       ：
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
//  函数名称   : WriteWord
//  功能描述   : 向IIC器件特定地址写一字节数
//  输入参数   : 地址： unAddr  器件从机地址：unSlaveAddr
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : EEP_ReadBytes
//  功能描述   : 从RTC8563器件特定地址读出一组8位数据
//  输入参数   : 地址： unAddr  数据长度：unLength 读出的数据存入：*Data
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : EEP_WriteBytes
//  功能描述   : 向RTC器件特定地址写入一组8位数据
//  输入参数   : 地址： unAddr  数据长度：unLength 写入的数据：*Data
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : BCDtoINT()
//  功能描述   : BCD码转十进制
//  输入参数   : unsigned char value
//  输出参数   ：<无>
//  返回值     : ucReturnVal
//  作者       ：
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
//  函数名称   : EEP_WriteWords
//  功能描述   : 十进制转BCD码
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : UCB0SPIInit
//  功能描述   : UCB0SPI总线初始化工作，
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
    P7DIR |= BIT3;            //EEPROM写保护输出   
    P7DIR |= BIT2;            //Flash写保护输出


  
    UCB0CTL1 |= UCSWRST;                          // **Put state machine in reset**   
    UCB0CTL0 |= UCMST + UCSYNC + UCCKPL + UCMSB;  // 3-pin, 8-bit SPI master
                                                  // Clock polarity high, MSB
//    UCB0CTL1 |= UCSSEL_1;                         // ACLK
//    UCB0BR0 = 0x04;                               // /4
    
    UCB0CTL1 |= UCSSEL_2;                       // MCLK
    UCB0BR0 = 0x02;                     // /2
    UCB0BR1 = 0;                                  
           
    UCB0CTL1 &= ~UCSWRST;                         // **Initialize USCI state machine**
    
    UNSELECT_CHIP;                                //ad45db片选不使能
    CAT_UNSELECT_CHIP;                            //CAT25128片选不使能
    CAT_WP;                                       //CAT25128写保护
    AT45DB_NOWP;                                    //AT45DB624写保护
    
    CAT_SpiConcelWP();                            //25128写使能

	
    Sst_Reset();                                  //复位Flash芯片
    Sst_Unlock();                                 //解锁Flash芯片
}

//==============================================================================
//  函数名称   : SPIReadBuf
//  功能描述   : SPI发送一字节数据
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : SPIReadBuf
//  功能描述   : SPI接收一字节数据
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : SpiWaitBusy
//  功能描述   : 通过读状态寄存器来检测忙状态
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : SpiWaitBusy
//  功能描述   : 取消写保护
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : CAT_SpiWriteByte
//  功能描述   : 字节写
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
    delay(13000);    //25128数据写入延时最大5ms
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
            delay(13000);    //25128数据写入延时最大5ms
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
//  函数名称   : CAT_SpiWriteWord
//  功能描述   : 字写
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
    delay(13000);    //25128数据写入延时最大5ms
}
//==============================================================================
//  函数名称   : CAT_SpiWriteWords
//  功能描述   : 多字写
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
            delay(13000);    //25128数据写入延时最大5ms
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
            delay(13000);   //25128数据写入延时最大5ms                
        }
    }
}


//==============================================================================
//  函数名称   : CAT_SpiReadByte
//  功能描述   : 字节读
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : CAT_SpiReadBytes
//  功能描述   : 字节流读
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : CAT_SpiReadWord
//  功能描述   : 字读
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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
//  函数名称   : CAT_SpiReadWords
//  功能描述   : 多字读
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
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