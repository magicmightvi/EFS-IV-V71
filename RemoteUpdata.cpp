
#ifndef REMOTE_UPDATA_GLOBALS

#define REMOTE_UPDATA_GLOBALS
  #include "DataStruct.h"
#endif

unsigned int update_crc(unsigned int length,unsigned char *data,unsigned int val);
unsigned int UpdateSumcheck(unsigned char *data,unsigned char length);
void WriteFlashByteR(unsigned long dst,unsigned int data);
char DataFlash_EraseSegment(unsigned long *pAddr);
extern struct LOCPARAMETER Locparameter;

union Item2
{
    unsigned int  bit16;
    unsigned char bit8[2];
};
union Item4
{
    unsigned long bit32;
    unsigned char bit8[4];
};
union Item2    UpPackNo;//升级数据包序号
union Item2    UpInPackNo;//升级段内数据包序号
union Item2    SumCheck;//打包校验结果
union Item2    SFrameCounter;//从机帧计数
unsigned char UpState;//升级状态
unsigned char SegNo;//总段数
unsigned char SegBreakNo;//断点段序号
unsigned char WriteTime,WriteFlag;//写入次数，写入标志0x55：失败，0x66：成功
unsigned char FrameTotal_No;//单帧数据总数
union   Item4 SegLength;//断点段长度
union   Item4 SegBaseAdd;//断点段基地址
union   Item4 SegOffsetAdd;//断点段偏移地址
union   Item4 Total_No;//数据总长，仅有效数据

//在确认升级之后进行再次清零，达到计数范围之后再次清零，每次有效帧都会累加
unsigned char FrameCount = 0;//数据帧计数，用于达到几次之后进行对EEPROM的更新
unsigned int  AddSize = 0;//累计数据量
unsigned int  AddCheck = 0;//累加校验


unsigned char AddByte(unsigned char *p, unsigned int length)
{
    unsigned int AddValue = 0;
    unsigned int i;
    for(i = 0; i < length; i++, p++)
    {
        AddValue += *p;
    }
    return (unsigned char)AddValue;    
}
//响应更新flash指令
void RspRefreshFlash(unsigned char *pTXBuff)
{
    unsigned char i = 0;
    unsigned char j = 0;
    pTXBuff[i++] = 0x69;
    i += 2; /*length*/
    pTXBuff[i++] = 0x69;
    pTXBuff[i++] = 0x80;
    pTXBuff[i++] = LOBYTE(g_gRunPara[RP_COMM_ADDR]);
    pTXBuff[i++] = HIBYTE(g_gRunPara[RP_COMM_ADDR]);     
    pTXBuff[i++] = 165;
    pTXBuff[i++] = 0;
    
    pTXBuff[i++] = 0;
    FEED_WATCH_DOG();
    while(i<(12+7))
    {
        pTXBuff[i++] = 0;
    }

    pTXBuff[1] = i - 4;
    pTXBuff[2] = i - 4;
    
    for(j=4;j<i;j++) 
    {
        pTXBuff[i] += pTXBuff[j];
    }

    i++;

    pTXBuff[i] = 0x16;

    CommSendData(pTXBuff,i + 1,g_Cmid);
}

//***********************************************************************************
//*函数名：Code_Load
//*功能：对相应的ASDU报文做出响应
//*描述：远程升级报文包括起始，写入，校验，确认几个类型
//***********************************************************************************
void Code_Load(unsigned char *pRxBuf,unsigned char *pTXBuff)//传输原因作为单字节处理
{//1
    unsigned char *pTxBuf = pTXBuff;//重新对发送缓存写入数据
    unsigned char pRXBuff[256] = {0};
    unsigned long address;
    unsigned int i,tmpcheck;
    unsigned char tmp,tmp1,tmp2;
    unsigned char ReadBack[256] = {0};//写入flash后重新读取回来
    unsigned char *pCSPos;
    unsigned char write_c;//写入标志0x55未写入，0x44成功写入
    unsigned char write_T;//写入错误计数
    unsigned char SaveParameter[8],SaveBack1[8],SaveBack2[8];
    unsigned long addr;
    unsigned int PageSize;//当前页面剩余空间
    unsigned char PackInfo[EEPUPDATE_STATE_NO]={0};//接收数据帧，帧状态信息
    unsigned char checkright; //eeprom 中存储的原数据区校验是否正确YES：正确，数据有效，NO：错误，数据无效使用备份去数据
    unsigned char updatecount[2];
    unsigned char reason;
    static unsigned char chRcvEnd = 0;
    FEED_WATCH_DOG();

        memcpy(pRXBuff,pRxBuf,6+pRxBuf[1]);
        reason = pRXBuff[9];//传输原因在第10个字节
        tmp = 6;//linkaddr_num;
        *pTxBuf++ = 0x69;
        *pTxBuf++ = 0;
        *pTxBuf++ = 0;
        *pTxBuf++ = 0x69;
        pCSPos = pTxBuf;
        *pTxBuf++ = 83;//控制域:子站->主站 数据确认帧
        *pTxBuf++ = LOBYTE(g_gRunPara[RP_COMM_ADDR]);
        *pTxBuf++ = HIBYTE(g_gRunPara[RP_COMM_ADDR]); 
        *pTxBuf++ = 200;//帧类型
        *pTxBuf++ = 0;//限定词
    
    switch(reason)
    {//2
    case Reason_160://从传输原因开始赋值
                    //返回当前系统的各种状态
      *pTxBuf++ = 160;//传输原因
      *pTxBuf++ = VerNum[V_OFFSET];//版本号
      *pTxBuf++ = VerNum[V_OFFSET+1];
      *pTxBuf++ = VerNum[V_OFFSET+2];
      *pTxBuf++ = VerNum[V_OFFSET+3]; 
      *pTxBuf++ = VerNum[V_OFFSET+4];      
      //*pTxBuf++ = 0;//版本号高
      //*pTxBuf++ = 0;//版本号低
      *pTxBuf++ = 0;//芯片号高
      *pTxBuf++ = 0;//芯片号低    
      *pTxBuf++ = 0x00;//程序升级用空间低字节400段200k，用十六进制表示十进制数
      *pTxBuf++ = 0x04;//程序升级用空间高字节  segment（512B)     
      //*pTxBuf++ = ReadWord(EEPUPDATE_LAST_STATE, 0);//上次的更新状态
      Sst26vf064b_Read(FLASH_UPDATE_COUNTER,SaveBack1,8);//上次更新状态保存于外部flash内
      tmpcheck = SaveBack1[7];
      tmpcheck = (tmpcheck<<8)|SaveBack1[6];
      if(tmpcheck != update_crc(6,SaveBack1,0xFFFF))
      {
          Sst26vf064b_Read(FLASH_UPDATEBK_COUNTER,SaveBack1,8);//flash备份读取
          tmpcheck = SaveBack1[7];
          tmpcheck = (tmpcheck<<8)|SaveBack1[6];
          if(tmpcheck != update_crc(6,SaveBack1,0xFFFF))
          {
              
              *pTxBuf++ = STATE_ALLCHECK1_NO;//状态值校验不正确归为校验错误
          }
          else
          {
              *pTxBuf++ = SaveBack1[4];
          }
      
      }
      else
      {
          *pTxBuf++ = SaveBack1[4];
      }
      
      CAT_SpiReadBytes(EEPUPDATE_LAST_VERSION,5,pTxBuf);//上次升级版本号
      pTxBuf += 5;
      *pTxBuf++ = CAT_SpiReadByte(EEPUPDATE_LAST_SEG);//段总数
      *pTxBuf++ = CAT_SpiReadByte(EEPUPDATE_LAST_SEGNo);//段序号
      CAT_SpiReadBytes(EEPUPDATE_LAST_LENGTH,3,pTxBuf);//段总长
      pTxBuf += 3;
      CAT_SpiReadBytes(EEPUPDATE_LAST_BASE,3,pTxBuf);//段基地址
      pTxBuf += 3;
      CAT_SpiReadBytes(EEPUPDATE_LAST_OFFSET,3,pTxBuf);//段偏移地址
      pTxBuf += 3;
      CAT_SpiReadBytes(EEPUPDATE_LAST_INCOUNTER,2,pTxBuf);//段内包计数
      pTxBuf += 2;
      CAT_SpiReadBytes(EEPUPDATE_LAST_COUNTER,2,pTxBuf);//包计数
      pTxBuf += 2;      
      for(i=0;i<5;i++)//预留
      {
          *pTxBuf++ = 0;
      }
      
      break;
    case Reason_163://升级确认
        *pTxBuf++ = 163;//传输原因 
        FrameCount = 0;
        AddSize = 0;
        AddCheck = 0;        
        CAT_SpiWriteBytes(EEPUPDATE_LAST_VERSION,5,&pRXBuff[tmp+5]);
        if(pRXBuff[tmp+4] == 0xAA)//起始位置更新
        {

            address = FLASH_UPDATE_START;
            for(i=0;i<0x4;i++)//0x4 * 0x10000 = 0x40000
            {
                Block_Erase(address);        //确认并进行擦除操作，响应，
                address += 0x10000;  
                SpiWaitBusy();
            }
            //测试用
            //Sst26vf064b_Read(FLASH_UPDATE_START,ReadBack,WRITEPACK);
            //初始化保存状态
            //SumCheck.bit16 = 0xFFFF; //CRC校验初始化
            SumCheck.bit16 = 0;//累加和校验初始化 
            CAT_SpiWriteBytes(EEPUPDATE_LAST_CHECK,2,SumCheck.bit8);//初始化校验值
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BKCHECK,2,SumCheck.bit8);//初始化校验值
            SFrameCounter.bit16 = 0;
            CAT_SpiWriteBytes(EEPUPDATE_LAST_COUNTER,2,SFrameCounter.bit8);//从机帧计数初始化            
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BKCOUNTER,2,SFrameCounter.bit8);//从机帧计数初始化            
            //CAT_SpiReadBytes(EEPUPDATE_LAST_COUNTER,2,SFrameCounter.bit8);
            UpInPackNo.bit16 = 0;
            CAT_SpiWriteBytes(EEPUPDATE_LAST_INCOUNTER,2,UpInPackNo.bit8);
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BKINCOUNTER,2,UpInPackNo.bit8);
                //CAT_SpiWriteByte(EPPUPDATE_UPDATE_IN_INCOUNTER,1,0, &UpPackNo);  
            SegNo = 0;
            CAT_SpiWriteBytes(EEPUPDATE_LAST_SEG,1,&SegNo);
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BKSEG,1,&SegNo);
            SegBreakNo = 0;
            CAT_SpiWriteBytes(EEPUPDATE_LAST_SEGNo,1,&SegBreakNo);
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BKSEGNo,1,&SegBreakNo);
            SegLength.bit32 = 0;
            CAT_SpiWriteBytes(EEPUPDATE_LAST_LENGTH,3,SegLength.bit8);
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BKLENGTH,3,SegLength.bit8);
            SegBaseAdd.bit32 = 0;
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BASE,3,SegBaseAdd.bit8);
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BKBASE,3,SegBaseAdd.bit8);
            SegOffsetAdd.bit32 = 0;
            CAT_SpiWriteBytes(EEPUPDATE_LAST_OFFSET,3,SegOffsetAdd.bit8);//保存量需要从接收报文中提取出来10.15 16:29
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BKOFFSET,3,SegOffsetAdd.bit8);//保存量需要从接收报文中提取出来10.15 16:29
            Total_No.bit32 = 0;
            CAT_SpiWriteBytes(EEPUPDATE_LAST_TOTAL,4,Total_No.bit8);
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BKTOTAL,4,Total_No.bit8);
            Sector_Erase(FLASH_UPDATE_COUNTER);//擦除入口标志数据
            *pTxBuf++ = 0x66;//确认升级，0x55为否认升级         
        }
        else if(pRXBuff[tmp+4] == 0xBB)//断点位置更新
        {
            //address = FLASH_UPDATE_START;
            CAT_SpiReadBytes(EEPUPDATE_LAST_COUNTER,2,SFrameCounter.bit8);//包计数
            *pTxBuf++ = 0x66;//确认升级，0x55为否认升级         
        
        }
        else
        {
            *pTxBuf++ = 0x55;//0x55为否认升级 
        }

        for(i=0;i<6;i++)
            *pTxBuf++ = 0; 
            
      break;
    case Reason_164://校验帧
          tmpcheck = pRXBuff[tmp+5];
          tmpcheck = (tmpcheck<<8)|pRXBuff[tmp+4];
          CAT_SpiReadBytes(EEPUPDATE_LAST_CHECK,2,SumCheck.bit8);          //打包校验对比
          CAT_SpiReadBytes(EEPUPDATE_LAST_COUNTER,2,UpPackNo.bit8);//包计数
          *pTxBuf++ = 164;
          *pTxBuf++ = SumCheck.bit8[0];
          *pTxBuf++ = SumCheck.bit8[1];//本身校验
          *pTxBuf++ = pRXBuff[tmp+6];
          *pTxBuf++ = pRXBuff[tmp+7];
          *pTxBuf++ = pRXBuff[tmp+8]; //程序大小，直接使用上位机数据  
          if(tmpcheck == SumCheck.bit16)
          {
              //*pTxBuf++ = 0x32;//校验结果是否相同放到其他写入操作完成之后确定
              //UpState = 0x67;
              UpState = STATE_ALLCHECK1_YES;
              //CAT_SpiWriteByte(EPPUPDATE_LAST_STATE,1,0, &UpState);
          }
          else
          {
              //*pTxBuf++ = 0x45;//校验结果不同
              //UpState = 0x45;
              UpState = STATE_ALLCHECK1_NO;
              //CAT_SpiWriteByte(EPPUPDATE_LAST_STATE,1,0, &UpState);
          }
          
          SaveParameter[0] = UpPackNo.bit8[0];//入口保存数据整理
          SaveParameter[1] = UpPackNo.bit8[1];
          SaveParameter[2] = SumCheck.bit8[0];
          SaveParameter[3] = SumCheck.bit8[1];
          SaveParameter[4] = UpState;
          SaveParameter[5] = NO;//下载校验完成，将内部flash擦出标志 置NO
          tmpcheck = update_crc(6,SaveParameter,0xFFFF);
          SaveParameter[6] = tmpcheck&0x00ff;
          SaveParameter[7] = (tmpcheck>>8)&0x00ff;   
          
          if(UpState == STATE_ALLCHECK1_YES)  //更新标志
          {
              //SavePerameter[8] = {0};  

              write_c = NO;
              write_T = 0;
              while((write_c == NO)&&(write_T < 6))//flash入口判断标志数据写入确认
              {
                      Sector_Erase(FLASH_UPDATE_COUNTER);//擦除与写入之间时间太短无法正常写入SPI分频调节为0时出错
                      //delay(ERASE_DELAY);
                      Sst26vf064b_Page_WriteBytes(FLASH_UPDATE_COUNTER,SaveParameter,8);
                      Sst26vf064b_Page_WriteBytes(FLASH_UPDATEBK_COUNTER,SaveParameter,8); 
                      Sst26vf064b_Read(FLASH_UPDATE_COUNTER,SaveBack1,8);//检测写入是否正确
                      Sst26vf064b_Read(FLASH_UPDATEBK_COUNTER,SaveBack2,8);
                      for(i=0;i<8;i++)
                      {
                        if((SaveParameter[i] == SaveBack1[i])&&(SaveParameter[i] == SaveBack2[i]))
                          {
                              if(i==7)//最后一个数据判断完成
                                write_c = YES;//写入成功
                          }
                          else
                          {
                              write_T++;//写入次数累加
                              break;
                          }
                      }
               }
              if(write_c == NO)//标志写入错误归入到校验错误
              {
                  UpState = STATE_ALLCHECK1_NO;
                  SaveParameter[4] = UpState;//入口标志写入错误则认为整体校验错误，需改变即将存储的写入标志值
                  /*DataFlash_EraseSegment((unsigned long *)INFLASH_COUNTER);
                  WriteFlashByte(INFLASH_STATE, UpState); */
                  Sector_Erase(FLASH_UPDATE_COUNTER);
                  //delay(ERASE_DELAY);
                  Sst26vf064b_Page_WriteBytes(FLASH_UPDATE_COUNTER,SaveParameter,8);
                  Sst26vf064b_Page_WriteBytes(FLASH_UPDATEBK_COUNTER,SaveParameter,8); 
                 // *pTxBuf++ = 0x45;
                  *pTxBuf++ = NO;
              } 
              else
              {
                  //*pTxBuf++ = 0x32;
                  *pTxBuf++ = YES;//写入成功
              }              
          }
          else//校验失败重新写入口标志
          {
                  //UpState = STATE_ALLCHECK1_NO;
                  SaveParameter[4] = UpState;//失败信息不再确认是否写入成功
                  Sector_Erase(FLASH_UPDATE_COUNTER);
                  //delay(ERASE_DELAY);
                  Sst26vf064b_Page_WriteBytes(FLASH_UPDATE_COUNTER,SaveParameter,8);
                  Sst26vf064b_Page_WriteBytes(FLASH_UPDATEBK_COUNTER,SaveParameter,8); 
                 // *pTxBuf++ = 0x45;
                  *pTxBuf++ = NO;              
          }
         // CAT_SpiWriteBytes(EEPUPDATE_LAST_STATE,1,&UpState);
          for(i=0;i<4;i++)//保留值
          {
              *pTxBuf++ = 0;
          }
          if(UpState == STATE_ALLCHECK1_YES)
          {//成功则进行响应且等待复位
              pTXBuff[1] = pTxBuf - pCSPos;//LL
              pTXBuff[2] = pTxBuf - pCSPos;//LL
              *pTxBuf++ = AddByte(&(pTXBuff[4]),pTXBuff[1]);
              *pTxBuf++ = 0x16;
              chRcvEnd = YES;
              CommSendData(pTXBuff,pTXBuff[1]+6,g_Cmid);
              //Send_101(unTxlength,4,COMM_INDEX_GPRS);
             // DataFlash_EraseSegment((unsigned long *)ERASEFLAGADDR);//不用擦除，jtag烧写已经擦除过，即使二次升级也没有必要擦除
              for(i=0;i<6;i++)
              {
                //ucTestRam[i]=11;//改变重启标志，使在线升级重启状态认为是掉电重启
	        WriteFlashByteR((ERASEFLAGADDR+i),0x55);//需要升级时将jtag擦除标志置位，以判断其不是jtag程序下载
              }
              
              //程序下载完成之后将flash的数据清除，初始化，减少对flash的操作次数，和减少开机耗时
              Sst26vf064b_Read(FLASH_UPCOUNTER,updatecount,2);//
              if(updatecount[0] != 0xFF)//无效并且非0xFF，则擦除flash
              {
                  Sector_Erase(FLASH_UPCOUNTER);
              }
                
//              delayms(500);
//              _DINT();
//              unsigned int k;
//              while(1)//运行灯快闪
//              {
//                LIGHT_RUN_FLASH();
//                for(k = 0; k < 65530; k++);
//              }
          }
        return;  
      //break;
    case Reason_165:
      RspRefreshFlash(pTXBuff);
      
        if(chRcvEnd == YES)
        {//等待重启
              chRcvEnd = NO;
              delayms(500);
              _DINT();
              unsigned int k;
              while(1)//运行灯快闪
              {
                LIGHT_RUN_FLASH();
                for(k = 0; k < 65530; k++);
              }
        }
      break;
    case Reason_166:  //返回当前软件版本
                      
        *pTxBuf++ = 166;//传输原因
        *pTxBuf++ = 0;//版本号高
        *pTxBuf++ = 0;//版本号低
      break;  
    case Reason_170://数据传输
        //UpState = 0xAA;
        *pTxBuf++ = 170;//传输原因
        //unsigned char FrameCount;//数据帧计数，用于达到几次之后进行对EEPROM的更新

        UpPackNo.bit16 = pRXBuff[tmp+5];
        UpPackNo.bit16 = UpPackNo.bit16<<8;
        UpPackNo.bit16 += pRXBuff[tmp+4];//包序号
        CAT_SpiReadBytes(EEPUPDATE_LAST_SEG,EEPUPDATE_STATE_NO,PackInfo);//读取升级中间量
        tmpcheck = PackInfo[EEPUPDATE_STATE_CRCH];
        tmpcheck <<= 8;
        tmpcheck |= PackInfo[EEPUPDATE_STATE_CRCL];
        if(tmpcheck == update_crc((EEPUPDATE_STATE_NO-2),PackInfo,0xFFFF))
        { 
            //CAT_SpiReadBytes(EEPUPDATE_LAST_COUNTER,2,SFrameCounter.bit8);//包计数
            checkright = YES;
        }
        else
        {
            CAT_SpiReadBytes(EEPUPDATE_LAST_BKSEG,EEPUPDATE_STATE_NO,PackInfo);//备份区包计数
            checkright = NO;
        }//若同时无效除非前4帧，则认为EEPROM出问题
        SFrameCounter.bit8[0] = PackInfo[EEPUPDATE_STATE_COUNTER1];
        SFrameCounter.bit8[1] = PackInfo[EEPUPDATE_STATE_COUNTER2];//包计数
        Total_No.bit8[0] = PackInfo[EEPUPDATE_STATE_TOTAL1];
        Total_No.bit8[1] = PackInfo[EEPUPDATE_STATE_TOTAL2];
        Total_No.bit8[2] = PackInfo[EEPUPDATE_STATE_TOTAL3];
        Total_No.bit8[3] = PackInfo[EEPUPDATE_STATE_TOTAL4];//数据总数
        
        SumCheck.bit8[0] = PackInfo[EEPUPDATE_STATE_CHECK1];
        SumCheck.bit8[1] = PackInfo[EEPUPDATE_STATE_CHECK2];//打包校验值
        if((SFrameCounter.bit16+(FrameCount&0x03)) == (UpPackNo.bit16-1))//接受包总是比当前已接受的包计数多1，使用EEPROM中的数据加上当前累积帧数
        {
          
            FrameCount++;//收到一有效帧累加一次
            
            SegNo = pRXBuff[tmp+8]; //段总数

            
            UpInPackNo.bit16 = pRXBuff[tmp+10];
            UpInPackNo.bit16 = (UpInPackNo.bit16<<8)|pRXBuff[tmp+9];//段内包序号
            

            
            SegBreakNo = pRXBuff[tmp+11]; //段序号
 
            
            
            FrameTotal_No = pRXBuff[tmp+21];//单帧数据总数
            
            
            WriteFlag = NO;//
            WriteTime = 0;
            addr = UpPackNo.bit16-1;
            addr *= WRITEPACK;
            addr += FLASH_UPDATE_START;//总地址 
            while((WriteFlag == NO)&&(WriteTime <= 2))//写入校验
            {

                PageSize = 0x100 - (addr%0x100);//当前页内剩余空间
                if(PageSize < WRITEPACK)//当前页面剩余空间小于要写入数据量
                {
                    Sst26vf064b_Page_WriteBytes(addr,&pRXBuff[tmp+15],PageSize);//每个包长度定义为相同，未达到长度的包，使用包长度，在读取时区分有效数据                    
                    Sst26vf064b_Page_WriteBytes(((addr&0xffffff00) + 0x100),&pRXBuff[tmp+15+PageSize],(WRITEPACK-PageSize)); //下一页的起始地址               
                    
                }
                else
                {
                    Sst26vf064b_Page_WriteBytes(addr,&pRXBuff[tmp+15],WRITEPACK);//每个包长度定义为相同，未达到长度的包，使用包长度，在读取时区分有效数据
                }
                Sst26vf064b_Read(addr,ReadBack,WRITEPACK);//读取刚写入数据
                BYTE bCheckFrmLen = 8 + FrameTotal_No;//基地址（3）+偏移地址（3）+数据量（1）+数据内容（224）+累加和（1）
               // for(i=0;i<WRITEPACK;i++)
                for(i=0;i<bCheckFrmLen;i++)//判断写入和读出数据是否相同时只能判断有用的数据，无用的数据不能判断(比如101报文的校验)
                {
                  if(pRXBuff[tmp+15+i] != ReadBack[i])
                  {
                    WriteTime++;
                      break;                 
                  }
                  else
                  {
                      //if(i==WRITEPACK-1)
                      if(i==bCheckFrmLen-1)
                          WriteFlag = YES;//写入成功
                  }
                }
            }
            if(WriteFlag == YES)//写入正确则保存
            {
                //*pTxBuf++ = 0x66;
                //UpState = 0xAA;
                UpState = STATE_LOADING;
         //unsigned int  AddSize;//累计数据量
        //unsigned int  AddCheck;//累加校验
                AddSize += FrameTotal_No;//累加数据量
                AddCheck += UpdateSumcheck(&pRXBuff[tmp+22],FrameTotal_No);//累加校验
                
                if(((FrameCount & 0x03) == 0)||((SegBreakNo==SegNo-1)&&(UpInPackNo.bit16==0xFFFF))||(UpPackNo.bit16 == 0x01))//计数每4次进行一次eeprom内容更新或者在程序下载最后一段的最后一包数据或者第一包数据
                {
                    
                      FrameCount = 0;
                      PackInfo[EEPUPDATE_STATE_SEG] =pRXBuff[tmp+8]; //段总数  
                      
                      PackInfo[EEPUPDATE_STATE_INCOUNTER1] = pRXBuff[tmp+9];//段内包序号
                      PackInfo[EEPUPDATE_STATE_INCOUNTER2] = pRXBuff[tmp+10];
                      
                      PackInfo[EEPUPDATE_STATE_SEGNo] =pRXBuff[tmp+11]; //段序号
                      
                      PackInfo[EEPUPDATE_STATE_COUNTER1] = pRXBuff[tmp+4];//包序号
                      PackInfo[EEPUPDATE_STATE_COUNTER2] = pRXBuff[tmp+5];
            
                      SegLength.bit32 = pRXBuff[tmp+14];//段总长
                      SegLength.bit32 = (SegLength.bit32<<8)|pRXBuff[tmp+13];
                      SegLength.bit32 = (SegLength.bit32<<8)|pRXBuff[tmp+12]; 
                      SegLength.bit32 &= 0xFFFFFF;            
                      PackInfo[EEPUPDATE_STATE_LENGTH1] = pRXBuff[tmp+12];
                      PackInfo[EEPUPDATE_STATE_LENGTH2] = pRXBuff[tmp+13];
                      PackInfo[EEPUPDATE_STATE_LENGTH3] = pRXBuff[tmp+14];
                      PackInfo[EEPUPDATE_STATE_LENGTH4] = 0;
        
                      SegBaseAdd.bit32 = pRXBuff[tmp+17];//基地址
                      SegBaseAdd.bit32 = (SegBaseAdd.bit32<<8)|pRXBuff[tmp+16];
                      SegBaseAdd.bit32 = (SegBaseAdd.bit32<<8)|pRXBuff[tmp+15];
                      SegBaseAdd.bit32 &= 0xFFFFFF;
                      PackInfo[EEPUPDATE_STATE_BASE1] = pRXBuff[tmp+15];
                      PackInfo[EEPUPDATE_STATE_BASE2] = pRXBuff[tmp+16];
                      PackInfo[EEPUPDATE_STATE_BASE3] = pRXBuff[tmp+17];
                      PackInfo[EEPUPDATE_STATE_BASE4] = 0;
            
                      SegOffsetAdd.bit32 = pRXBuff[tmp+20];//偏移地址
                      SegOffsetAdd.bit32 = (SegOffsetAdd.bit32<<8)|pRXBuff[tmp+19];
                      SegOffsetAdd.bit32 = (SegOffsetAdd.bit32<<8)|pRXBuff[tmp+18];        
                      SegOffsetAdd.bit32 &= 0xFFFFFF;
                      PackInfo[EEPUPDATE_STATE_OFFSET1] = pRXBuff[tmp+18];
                      PackInfo[EEPUPDATE_STATE_OFFSET2] = pRXBuff[tmp+19];
                      PackInfo[EEPUPDATE_STATE_OFFSET3] = pRXBuff[tmp+20];
                      PackInfo[EEPUPDATE_STATE_OFFSET4] = 0;

                    //if(checkright == YES)//原始数据有效则使用，否则使用备份去数据
                   // {
                   //   CAT_SpiReadBytes(EEPUPDATE_LAST_TOTAL,4,Total_No.bit8);
                   //   CAT_SpiReadBytes(EEPUPDATE_LAST_CHECK,2,SumCheck.bit8);//读取上次保存校验结果
                    //}
                    //else
                   // {
 
                    //  CAT_SpiReadBytes(EEPUPDATE_LAST_BKTOTAL,4,Total_No.bit8);
                   //   CAT_SpiReadBytes(EEPUPDATE_LAST_BKCHECK,2,SumCheck.bit8);//读取上次保存校验结果
                   // }
                    Total_No.bit32 += AddSize;
                    PackInfo[EEPUPDATE_STATE_TOTAL1] = Total_No.bit8[0];
                    PackInfo[EEPUPDATE_STATE_TOTAL2] = Total_No.bit8[1];
                    PackInfo[EEPUPDATE_STATE_TOTAL3] = Total_No.bit8[2];
                    PackInfo[EEPUPDATE_STATE_TOTAL4] = Total_No.bit8[3];
                    /*CAT_SpiWriteBytes(EEPUPDATE_LAST_TOTAL,4,Total_No.bit8);*/
                
                    //SumCheck.bit16 = update_crc((unsigned int)FrameTotal_No,&pRXBuff[tmp+22],SumCheck.bit16);//存储校验结果
                    SumCheck.bit16 += AddCheck;//更新校验结果并重新保存
                    SumCheck.bit16 &= 0xffff;//暂时保留原来的16位校验空间
                    PackInfo[EEPUPDATE_STATE_CHECK1] = SumCheck.bit8[0];
                    PackInfo[EEPUPDATE_STATE_CHECK2] = SumCheck.bit8[1];
                    AddSize = 0;
                    AddCheck = 0;
                /*CAT_SpiWriteBytes(EEPUPDATE_LAST_CHECK,2,SumCheck.bit8);*/
                
                //CAT_SpiWriteBytes(EEPUPDATE_LAST_COUNTER,2,UpPackNo.bit8);//包序号
                //CAT_SpiWriteBytes(EEPUPDATE_LAST_INCOUNTER,2,UpInPackNo.bit8);//段内包序号
                //CAT_SpiWriteByte(EPPUPDATE_UPDATE_IN_INCOUNTER,1,0, &UpPackNo); 
                //if(SegBreakNo != CAT_SpiReadByte(EEPUPDATE_LAST_SEGNo))//加入段序号判断，只有当段序号变化之后才对段序号、总段数、段总长、段基地址进行更新以节省时间
                //{
                  //CAT_SpiWriteBytes(EEPUPDATE_LAST_SEG,1,&SegNo);
                  //CAT_SpiWriteBytes(EEPUPDATE_LAST_SEGNo,1,&SegBreakNo);
                  //CAT_SpiWriteBytes(EEPUPDATE_LAST_LENGTH,3,SegLength.bit8);
                  //CAT_SpiWriteBytes(EEPUPDATE_LAST_BASE,3,SegBaseAdd.bit8);
                //}
                //CAT_SpiWriteBytes(EEPUPDATE_LAST_OFFSET,3,SegOffsetAdd.bit8);//保存量需要从接收报文中提取出来10.15 16:29
                    tmpcheck = update_crc((EEPUPDATE_STATE_NO-2),PackInfo,0xFFFF);
                    PackInfo[EEPUPDATE_STATE_CRCL] = tmpcheck&0x00ff;
                    PackInfo[EEPUPDATE_STATE_CRCH] = (tmpcheck>>8)&0x00ff;
                    CAT_SpiWriteBytes(EEPUPDATE_LAST_SEG,EEPUPDATE_STATE_NO,PackInfo);
                    CAT_SpiWriteBytes(EEPUPDATE_LAST_BKSEG,EEPUPDATE_STATE_NO,PackInfo);
                //CAT_SpiReadBytes(EEPUPDATE_LAST_SEG,EEPUPDATE_STATE_NO,PackInfo);
                //重新读取，查看是否写入正确
                //CAT_SpiReadBytes(EEPUPDATE_LAST_SEG,1,&SegNo);
                //CAT_SpiReadBytes(EEPUPDATE_LAST_CHECK,2,SumCheck.bit8);//读取上次保存校验结果
                //CAT_SpiReadBytes(EEPUPDATE_LAST_TOTAL,4,Total_No.bit8);
                }
                
            }
            else
            {
                WriteFlag = NO;//*pTxBuf++ = 0x55;//写入错误
                UpState = STATE_FLASHWRITE_NO;
            }
            //CAT_SpiReadBytes(EEPUPDATE_LAST_STATE,1,&tmp2);
            //if(tmp2 != UpState)
                //CAT_SpiWriteBytes(EEPUPDATE_LAST_STATE,1,&UpState);//更新EEPROM中状态若改变则保存
            *pTxBuf++ = UpPackNo.bit16&0xff;
            *pTxBuf++ = (UpPackNo.bit16>>8)&0xff;//帧计数更新 
            for(i=6;i<22;i++)//响应和接收的相应位内容相同
            {
              *pTxBuf++ = *(pRxBuf+tmp+i);
              //pRxBuf++;
            }
        }
        else//帧计数不对应，返回从机接收帧计数
        {
            WriteFlag = 0x77;
            FrameCount = 0;//单片机返回接收断点时需要将原有的可能的数值清零，以避免造成帧计数不对应
            AddSize = 0; //累计数据量清零
            AddCheck = 0; //累加校验清零
            UpState = STATE_LOADING;
            *pTxBuf++ = SFrameCounter.bit8[0]&0xff;
            *pTxBuf++ = SFrameCounter.bit8[1]&0xff;//帧计数不变
            *pTxBuf++ = *(pRxBuf+tmp+6);//版本号
            *pTxBuf++ = *(pRxBuf+tmp+7);//版本号
            *pTxBuf++ = *(pRxBuf+tmp+8);//段总数


            //判断区域内存储的数据是否有效，有效则读取，否则使用备份区数据
            if(checkright == YES)
            {
                //CAT_SpiReadBytes(EEPUPDATE_LAST_INCOUNTER,2,UpInPackNo.bit8);//段内包计数
                //*pTxBuf++ = UpInPackNo.bit8[0];
                //*pTxBuf++ = UpInPackNo.bit8[1];            
                CAT_SpiReadBytes(EEPUPDATE_LAST_INCOUNTER,2,pTxBuf);
                pTxBuf += 2;

                //CAT_SpiReadBytes(EEPUPDATE_LAST_SEGNo,1,&SegBreakNo);//段序号
                //*pTxBuf++ = SegBreakNo;
                CAT_SpiReadBytes(EEPUPDATE_LAST_SEGNo,1,pTxBuf);
                pTxBuf += 1;
            
                //CAT_SpiReadBytes(EEPUPDATE_LAST_LENGTH,3,SegLength.bit8);//段总长
                //*pTxBuf++ = SegLength.bit8[0];
                //*pTxBuf++ = SegLength.bit8[1];
                //*pTxBuf++ = SegLength.bit8[2];
                CAT_SpiReadBytes(EEPUPDATE_LAST_LENGTH,3,pTxBuf);
                pTxBuf += 3;
            
                //CAT_SpiReadBytes(EEPUPDATE_LAST_BASE,3,SegBaseAdd.bit8);//段基址
                //*pTxBuf++ = SegBaseAdd.bit8[0];
                //*pTxBuf++ = SegBaseAdd.bit8[1];
                //*pTxBuf++ = SegBaseAdd.bit8[2];
                CAT_SpiReadBytes(EEPUPDATE_LAST_BASE,3,pTxBuf);
                pTxBuf += 3;    
            
                //CAT_SpiReadBytes(EEPUPDATE_LAST_OFFSET,3,SegOffsetAdd.bit8);//段偏移地址
                //*pTxBuf++ = SegOffsetAdd.bit8[0];
                //*pTxBuf++ = SegOffsetAdd.bit8[1];
                //*pTxBuf++ = SegOffsetAdd.bit8[2];
                CAT_SpiReadBytes(EEPUPDATE_LAST_OFFSET,3,pTxBuf);
                pTxBuf += 3; 
            }
            else
            {
                //CAT_SpiReadBytes(EEPUPDATE_LAST_INCOUNTER,2,UpInPackNo.bit8);//段内包计数
                //*pTxBuf++ = UpInPackNo.bit8[0];
                //*pTxBuf++ = UpInPackNo.bit8[1];            
                CAT_SpiReadBytes(EEPUPDATE_LAST_BKINCOUNTER,2,pTxBuf);
                pTxBuf += 2;

                //CAT_SpiReadBytes(EEPUPDATE_LAST_SEGNo,1,&SegBreakNo);//段序号
                //*pTxBuf++ = SegBreakNo;
                CAT_SpiReadBytes(EEPUPDATE_LAST_BKSEGNo,1,pTxBuf);
                pTxBuf += 1;
            
                //CAT_SpiReadBytes(EEPUPDATE_LAST_LENGTH,3,SegLength.bit8);//段总长
                //*pTxBuf++ = SegLength.bit8[0];
                //*pTxBuf++ = SegLength.bit8[1];
                //*pTxBuf++ = SegLength.bit8[2];
                CAT_SpiReadBytes(EEPUPDATE_LAST_BKLENGTH,3,pTxBuf);
                pTxBuf += 3;
            
                //CAT_SpiReadBytes(EEPUPDATE_LAST_BASE,3,SegBaseAdd.bit8);//段基址
                //*pTxBuf++ = SegBaseAdd.bit8[0];
                //*pTxBuf++ = SegBaseAdd.bit8[1];
                //*pTxBuf++ = SegBaseAdd.bit8[2];
                CAT_SpiReadBytes(EEPUPDATE_LAST_BKBASE,3,pTxBuf);
                pTxBuf += 3;    
            
                //CAT_SpiReadBytes(EEPUPDATE_LAST_OFFSET,3,SegOffsetAdd.bit8);//段偏移地址
                //*pTxBuf++ = SegOffsetAdd.bit8[0];
                //*pTxBuf++ = SegOffsetAdd.bit8[1];
                //*pTxBuf++ = SegOffsetAdd.bit8[2];
                CAT_SpiReadBytes(EEPUPDATE_LAST_BKOFFSET,3,pTxBuf);
                pTxBuf += 3; 
            }              
            
            *pTxBuf++ = pRXBuff[tmp+21];//包内数据量，没用
        }
        
        Sst26vf064b_Read(FLASH_UPDATE_STATE,&tmp2,1);
        Sst26vf064b_Read(FLASH_UPDATEBK_STATE,&tmp1,1);//读取升级状态
        if((UpState != tmp2)&&(UpState != tmp1))//状态不同则重新保存 ，加入判断防止在写入过程中，每写一次就更新一次
        {
              SaveParameter[0] = UpPackNo.bit8[0];//入口保存数据整理
              SaveParameter[1] = UpPackNo.bit8[1];
              SaveParameter[2] = SumCheck.bit8[0];
              SaveParameter[3] = SumCheck.bit8[1];
              SaveParameter[4] = UpState;
              SaveParameter[5] = 0;
              tmpcheck = update_crc(6,SaveParameter,0xFFFF);
              SaveParameter[6] = tmpcheck&0x00ff;
              SaveParameter[7] = (tmpcheck>>8)&0x00ff;   
              
              Sector_Erase(FLASH_UPDATE_COUNTER);
              //delay(ERASE_DELAY);
              Sst26vf064b_Page_WriteBytes(FLASH_UPDATE_COUNTER,SaveParameter,8);
              Sst26vf064b_Page_WriteBytes(FLASH_UPDATEBK_COUNTER,SaveParameter,8); 
        }
        *pTxBuf++ = WriteFlag;//flash数据写入情况
            
      break;       
    }//end 2

    pTXBuff[1] = pTxBuf - pCSPos;//LL
    pTXBuff[2] = pTxBuf - pCSPos;//LL
    *pTxBuf++ = AddByte(&(pTXBuff[4]),pTXBuff[1]);
    *pTxBuf++ = 0x16;
    CommSendData(pTXBuff,pTXBuff[1]+6,g_Cmid);
    //return pTxBuf;
}//end 1



//写入内部Flash，dst地址，data数据,将数据改写到外部flash后不用
void WriteFlashByteR(unsigned long dst,unsigned int data)
{
    unsigned int i;
    unsigned char *ptr;
    
    _DINT();
    WDTCTL = WDTPW + WDTHOLD;               ////禁止内部看门狗
    //FEED_WATCH_DOG();                             ////清外部看门狗
    ptr  = (unsigned char *)dst;
    for(i = 0; i < 1000; i++)
    {
      if((FCTL3 & BUSY) != BUSY)              //不忙则跳出,完成擦除
      break;
    }
    FCTL3 = FWKEY;                            //  LOCK=0
    FCTL1 = FWKEY+WRT;                        //  WRT=1
    *ptr = data;
	//FEED_WATCH_DOG();
    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
    _EINT();
}

char DataFlash_EraseSegment(unsigned long *pAddr)
{
    volatile unsigned int * ptr;                    //addr为该段的首地址
    unsigned int j;
    
    _DINT();  
    WDTCTL = WDTPW + WDTHOLD;               ////禁止内部看门狗
    //FEED_WATCH_DOG();                             ////清外部看门狗
    
    for(j = 0; j < 1000; j++)
    {
      if((FCTL3 & BUSY) != BUSY)              //不忙则跳出,完成擦除
      break;
    }      
    ptr = (unsigned int *)pAddr;
    FCTL1          = 0xA502;                  //ERASE =1
    FCTL3          = 0xA500;                  //LOCK=0
    *ptr           = 0;
    FCTL1          = FWKEY;                   // Clear WRT bit
    FCTL3          = FWKEY + LOCK;            // Set LOCK bit    
    _EINT();
   // return OK;
    return 1;
}

unsigned int update_crc(unsigned int length,unsigned char *data,unsigned int val)//length:长度，*data：数据指针，val:保存的中间校验值
{
    unsigned int crcvalue;
    unsigned char AH , AL ;
    unsigned char GH = 0xA0, GL = 0x01;
    unsigned char i, j; 
    unsigned int point;
    
    AH = (val>>8)&0xff;
    AL = val&0xff;
    
    for(i = 0;i < length;i++)
    {
        point = *(data+i);
        AL = point ^ AL;
        for(j = 0;j < 8;j++)
        {
            AL = AL&0xff;
            AH = AH&0xFF;
            if(AL & 0x01 == 1)      // AL最低位为1
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
                if(AH & 0x01 == 1)
                    AL = AL | 0x80;
                AH = AH >> 1;
            }
        }
    }
    crcvalue = (unsigned int)(AH<<8) + (AL&0x0ff);  //高位在前?

    return crcvalue;
}
//累加和校验
unsigned int UpdateSumcheck(unsigned char *data,unsigned char length)
{
    unsigned int val = 0;
    unsigned char i;
    for(i=0;i<length;i++)
    {
        val += *data;
        data++;
    }
    val &= 0xffff;
    return val;    
}