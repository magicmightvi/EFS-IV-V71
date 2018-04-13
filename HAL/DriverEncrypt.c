/*------------------------------------------------------------------------
 Module:            PrtENCRYPT.cpp
 Author:            
 Project:           
 State:             
 Creation Date:     2017-3-916
 Description:       与北京智芯微加密芯片的通信规约
------------------------------------------------------------------------*/

#ifndef _DRIVER_ENCRYPT

  #define _DRIVER_ENCRYPT
#include "..\DataStruct.h"

#endif


//T-ESAM
#define CAT_SELECT_TESAM     P3OUT &= ~BIT0  //加密芯片片选信号
#define CAT_UNSELECT_TESAM   P3OUT |= BIT0

#define TESAM_POWER_ON       P1OUT &= ~BIT6  //加密芯片电源开//P6OUT &= ~BIT4  //加密芯片电源开
#define TESAM_POWER_OFF      P1OUT |= BIT6
#define STEP1                1
#define STEP2                2
#define STEP3                3
#define STEP4                4




//应用类型定义
//#define 

//==============================================================================
//  函数名称   : CheckLrc
//  功能描述   : 异或校验
//  输入参数   : *ptr 校验数据头指针
//               num需要校验的数据长度
//  输出参数   ：<无>
//  返回值     : 返回校验值
//  其他说明   : 
//  作者       ：zx
//==============================================================================
unsigned char CheckLrc (unsigned char *ptr,int num)
{
    unsigned char result;
    result = *ptr++;
    for(int i = 0; i < (num-1); i++)
    {
        result ^= *ptr++;
    }
    result = ~result;
    return result;
}

//==============================================================================
//  函数名称   : SPIWriteBuf
//  功能描述   : SPI发送一字节数据
//  输入参数   : ucData: 待写入数据
//  输出参数   ：<无>
//  返回值     : UCB0TXBUF寄存器数据
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned char SPIWriteBufE(unsigned char ucData)
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
//  函数名称   : InitSPI
//  功能描述   : SPI接收一字节数据
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : UCB0RXBUF寄存器数据
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned char SPIReadBufE(void)
{
    unsigned char i;
    SPIWriteBufE(0);
    for( i = 0; i < 250; i++ )
    {
    if((UCB0STAT&UCBUSY)==0)
    {    
        break;
    }
    }
    return UCB0RXBUF;
}
/*
////==============================================================================
////  函数名称   : SPIReadBuf
////  功能描述   : SPI发送一字节数据
////  输入参数   : <无>
////  输出参数   ：<无>
////  返回值     : <无>
////  其他说明   : 
////  作者       ：
////==============================================================================
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
*/
//==============================================================================
//  函数名称   : SendBytesToEncChip
//  功能描述   : 给加密芯片发送数据
//  输入参数   : 
//  输出参数   ：<无>
//  返回值     : 

//  其他说明   : 
//  作者       ：zx
//==============================================================================
void SendBytesToEncChip(unsigned char *pdata,unsigned char num)
{
    //检测总线先否忙碌
    CAT_SELECT_TESAM;
    delayms(1);
    SPIWriteBufE(0x55);  
    for(unsigned char i = 0; i < num; i++)
    {
        SPIWriteBufE(pdata[i]);
    }
    SPIWriteBufE(CheckLrc(pdata,num));  //写校验
    CAT_UNSELECT_TESAM;
    delayms(200);
    //return true;
}

unsigned long StrToHex(char const *SrcStr,BYTE len)
{
    unsigned long DestTemp;
    unsigned long value = 0;
    if(len > 8)
    {
        return -1;
    }
    for(BYTE i = 0; i < len; i++)
    {
        value = value << 4;
        if((SrcStr[i] >='0') && (SrcStr[i] <= '9'))
        {
            DestTemp = SrcStr[i] - '1' + 1;
        }
        else if((SrcStr[i] >='a') && (SrcStr[i] <= 'f'))
        {
            DestTemp = SrcStr[i] - 'a' + 10;
        }
        else if((SrcStr[i] >='A') && (SrcStr[i] <= 'F'))
        {
            DestTemp = SrcStr[i] - 'A' + 10;
        }
        else
        {
            return -1;
        }
        value |= DestTemp;
    }
    return value;
}


int SendCmdToEncChip(const char *sCmd,BYTE *EncDate,WORD DataLen)
{
    char sDataStr[9] = {0};
    //char sDataStr[256] = {0};
    WORD count = 0;
    //BYTE Check;
    BYTE Check;
    //sDataStr[count++] = 0x55;
    if(strlen(sCmd) >= 17)
    {
        return -1;
    }   
    for(BYTE i = 0; i < strlen(sCmd);)
    {
        sDataStr[count++] = (char)StrToHex(&sCmd[i],2);
        i += 2;
    }  
    CAT_SELECT_TESAM;
    delayms(1);
    SPIWriteBufE(0x55);
    SPIWriteBufE(sDataStr[0]);  
    Check = sDataStr[0];//异或校验赋初值
    for(unsigned char i = 1; i < count; i++)
    {
        SPIWriteBufE(sDataStr[i]);
        Check ^= sDataStr[i];
    } 
    if((DataLen != 0) && (EncDate != NULL))
    {
        for(WORD i = 0; i < DataLen; i++)
        {
            SPIWriteBufE(EncDate[i]);
            Check ^= EncDate[i];
        }
        count += DataLen;
    } 
    Check = ~Check;
    SPIWriteBufE(Check);
    delayms(1);
    CAT_UNSELECT_TESAM;
    delayms(200);
    //SendBytesToEncChip((BYTE*)sDataStr,count);
    return count;  //给加密芯片下发的数据计数不包括头（0x55和校验)
}


int RecBytesFromEsam(unsigned char *pRecDataTemp)
{
    CAT_SELECT_TESAM;
    delayms(1);
    int RecDataNum = 0;
    int ReadBytes;
    int DataLen;
    unsigned char Temp;
    unsigned char CommFlag = 0;
    //unsigned char RecData[255] = {0};
    for(BYTE times = 0;times < 4; times++)     //重发次数4
    {
        for(ReadBytes = 0; ReadBytes < 1024; ReadBytes++) 
        {
//            for(int i = 0; i < 255; i++)
//            {
//                pRecDataTemp[i] = CAT_SPIReadBuf();
//            }
            if(CommFlag <= 1)
                Temp = SPIReadBufE();
            if(Temp == 0xFF)
                break;           
            if((Temp == 0x55) && (CommFlag == 0))
            {
                CommFlag = 1;
                continue;
            }
            if(CommFlag == 1) 
            {
                switch(Temp)
                {
                    case 0x90:
                        pRecDataTemp[RecDataNum++] = Temp;
                        for(int i = 0; i < 3; i++)
                        {
                            pRecDataTemp[RecDataNum++] = SPIReadBufE();
                        }                      
                        CommFlag = 2;
                        continue;
                    case 0x63:
                    case 0x65:
                    case 0x67:
                    case 0x69:
                    case 0x6A:
                    case 0x6D:
                    case 0x6E:
                        pRecDataTemp[RecDataNum++] = Temp;
                        for(int i = 0; i < 3; i++)
                        {
                            pRecDataTemp[RecDataNum++] = SPIReadBufE();
                        }
                        return 0;
                    default:
                        break;
                }
//                if(Temp == 0x90)
//                {
//                    pRecDataTemp[RecDataNum++] = Temp;
//                    for(int i = 0; i < 3; i++)
//                    {
//                        pRecDataTemp[RecDataNum++] = CAT_SPIReadBuf();
//                    }
//                    
//                    continue;
//                }
//                else if(Temp == 0x69)
//                {
//                    for(int i = 0; i < 3; i++)
//                    {
//                        pRecDataTemp[RecDataNum++] = CAT_SPIReadBuf();
//                    }
//                    return 0;
//                }
                //else if()
            }
            if(CommFlag == 2)
            {
                if((pRecDataTemp[0] == 0x90) && (pRecDataTemp[1] == 0x00))
                {
                    DataLen = MAKEWORD(pRecDataTemp[3],pRecDataTemp[2]);
                }
                else
                {
                    return 0;
                }
                if(DataLen < 1024)
                {
                    RecDataNum = 0;
                    for(int i = 0; i < DataLen; i++)
                        pRecDataTemp[RecDataNum++] = SPIReadBufE();
                    CAT_UNSELECT_TESAM;
                    delayms(50);
                    if(DataLen == 0)
                    {
                        return 2048;
                    }
                    return DataLen;
                }
            }
        }
    }
    return 0;
}

////获取芯片ID
//void AskEncChipID(void)                      
//{
//    SendCmdToEncChip(GET_CHIP_ID,NULL,0);
//}

//获取私钥
//void AskEncChipPrivateKey(void)
//{
//    SendCmdToEncChip(GET_RRI_KEY,NULL,0);
//}
//void AskEncChipRandom(void)
//{
//    //SendCmdToEncChip(GET_RRI_KEY_STEP1,NULL,0);
//}
//获取随机数



//void GetEncChipPriKey(BYTE step)
//{
//    BYTE sDataStr[60] = {0};
//    switch(step)
//    {
//    case STEP1:
//        BYTE DataSrcStr[] = "801a00000000";
//        for(BYTE i = 0,j = 0; i < strlen(DataSrcStr); j++)
//        {
//            sDataStr[j] = (char)StrToHex((BYTE*)&DataSrcStr[i],2);
//            i += 2;
//        }
//        SendBytesToEncChip((BYTE*)sDataStr,(BYTE)strlen(sDataStr));
//        break;
//    case STEP2:
//        BYTE DataSrcStr[] = "008400080000";
//        for(BYTE i = 0,j = 0; i < strlen(DataSrcStr); j++)
//        {
//            sDataStr[j] = (char)StrToHex((BYTE*)&DataSrcStr[i],2);
//            i += 2;
//        }
//        SendBytesToEncChip((BYTE*)sDataStr,(BYTE)strlen(sDataStr));      
//        break;
//    default:
//        break;
//    }
//}

////身份双向认证
//void EncChipIdent(BYTE step)
//{
//    BYTE DataStr[60] = {0};
//    BYTE *DataPtr,DataSrcPtr;
//    switch(step)
//    {
//    case STEP1:
//        BYTE DataSrcStr[] = "008400080000";
//        StrToHex(DataSrcStr,DataStr,strlen(DataSrcStr));
//        SendDataToEncChip(DataStr,strlen(DataStr));
//        break;
//    case STEP2:
//        BYTE DataSrcStr[] = "008400080000";
//        StrToHex(DataSrcStr,DataStr,strlen(DataSrcStr));
//        SendDataToEncChip(DataStr,strlen(DataStr));        
//        break;
//    default:
//        break;
//    }
//}
