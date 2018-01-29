
//************************************************
//*代码升级更新区：0x7c00-0x3E1FF包括中断和复位向量
//*                 0x7c00-0xFFFF （ISR_CODE)中断函数及向量
//*                 0x10000-0x10FFF（APPLICATION）应用程序入口
//*                 0x11000-0x3E1FF（CODE）
//*入口及拷贝程序段：0x5c00-0x7BFF（UPDATECODE）
//*状态存储数据：0x45A00-0x45bff(预留）
//*常量表存储区：0x3E200-0x459FF（TABLE）开方表，反时限表
//*************************************************
//*文件名： main.c
//*功能：1.判断入口，即选择进入更新程序或者应用程序
//*      2.校验外部flash中的数据是否有效
//*      3.将外部flash内的数据写入到内部flash应用区
//*      4.过程中的相应指示
//*      5.完成或者出错之后复位
//**************************************************

#ifndef _BOOT
#define _BOOT
  #include "DataStruct.h"
#endif

#define AppStartAddr    0x7c00//更新段起始地址
//#define AppEndAddr    0x3FFFF
#define AppEndAddr      0x3E1FF
#define AppSegNo        435//擦除段数
//#define UPCOUNTHead     0x45A00//计数更新segment头
//#define UPCOUNT         0x45BF0//更新计数地址超过限制不在远程升级
//0x45BF0内部flash中的数据保存错误更新的计数，每次更新完成，或者jtag连接时的擦除，会对其进行擦除操作
//#define WTD_TOG (P5OUT ^= BIT3)//看门狗信号
#define WTD_TOG (P5OUT ^= BIT3);// p7.5 看门狗 
#define LEDDELAY 20000//led灯闪烁延时

void UPdelay(unsigned int unDelayNum);
//void UpLEDState(unsigned char almled,unsigned char recled);
//void abnormal1(void);//校验过程单帧读取（校验）校验错误
//void abnormal2(void);//打包校验错误
//void abnormal3(void);//写入过程单帧读取（校验）错误
//void abnormal4(void);//写入flash错误
//void abnormal5(void);//异常升级累计超限错误指示

void UPPortInit(void);
void UPSst_Reset(void);
void UPSst_Unlock(void);
void UPWriteFlashWord(unsigned long dst, unsigned char *data);
void UPWriteFlashBytes(unsigned long dst, unsigned char *data,unsigned char length);
unsigned int singleFrameSumCheck(unsigned char *data,unsigned char len,unsigned int tmpcheck);
void UPSst26vf064b_Page_WriteBytes(long unAddr,unsigned char *PcData,unsigned int Size);
void UPSector_Erase(long unBlock);
unsigned int code_copy_crc(unsigned int length,unsigned char *data,unsigned int val);//length:长度，*data：数据指针，val:保存的中间校验值
void WriteUpdate(unsigned int FrameSum,unsigned int SumCheck,unsigned char UpState,unsigned char EraseFlag);
void UPSst26vf064b_Read(long unAddr,unsigned char *PcData,unsigned int Size);
void update(unsigned int FrameSum,unsigned int Sumcheck,unsigned char EraseFlag);
void EraseUpdataFlash(void);
void code_copy(unsigned int frame,unsigned int SumCheck);
void Flash_EraseSegment(unsigned long  pAddr);
void UPWriteFlashByte(unsigned long dst, unsigned char data);
unsigned int singleFrameCheck(unsigned char *data,unsigned int tmpcheck);
void UPUCB0SPIInit(void);
void UpInitClk(void);


void main(void)@"UPDATECODE"
{
    
    unsigned char entry_val;
    unsigned char SaveParameter[8];
    unsigned int  FrameSum;
    unsigned int  SumCheck;
    unsigned int  check;//crc校验
    unsigned char updatecount[2];//代码更新计数
    unsigned char eraseflag;//内部flash擦出标志
    //unsigned char ledstate = 0;
    WDTCTL = WDTPW + WDTHOLD;
    _DINT();
    UPPortInit();//端口初始化
    UpInitClk();//初始化时钟
    LED_RUN_ON;//运行灯亮
    UPUCB0SPIInit();//SPI初始化
    UPSst26vf064b_Read(FLASH_UPDATE_COUNTER,SaveParameter,8);//flash读取
    FrameSum = SaveParameter[1];
    FrameSum = (FrameSum<<8)|SaveParameter[0];
    SumCheck = SaveParameter[3];
    SumCheck = (SumCheck<<8)|SaveParameter[2];
    entry_val = SaveParameter[4];
    eraseflag = SaveParameter[5];
    check = SaveParameter[7];
    check = (check<<8)|SaveParameter[6];
    if(check != code_copy_crc(6,SaveParameter,0xFFFF))
    {
        UPSst26vf064b_Read(FLASH_UPDATEBK_COUNTER,SaveParameter,8);//flash读取
        FrameSum = SaveParameter[1];
        FrameSum = (FrameSum<<8)|SaveParameter[0];//帧计数，
        SumCheck = SaveParameter[3];
        SumCheck = (SumCheck<<8)|SaveParameter[2];//校验和，
        entry_val = SaveParameter[4];//更新状态
        eraseflag = SaveParameter[5];
        check = SaveParameter[7];
        check = (check<<8)|SaveParameter[6];
        if(check != code_copy_crc(6,SaveParameter,0xFFFF))
        {
            entry_val = 0;//如果错误，不用改变，直接进入程序
        }
      
    }
    //entry_val = (*(unsigned int*)INFLASH_STATE)&0XFF;//读取入口标志，状态值保存在flash内
    WTD_TOG;
    if((*(unsigned char *)ERASEFLAGADDR == 0xFF)&&(*(unsigned char *)(ERASEFLAGADDR+1) == 0xFF)&&(*(unsigned char *)(ERASEFLAGADDR+2) == 0xFF)&&(*(unsigned char *)(ERASEFLAGADDR+3) == 0xFF)&&(*(unsigned char *)(ERASEFLAGADDR+4) == 0xFF)&&(*(unsigned char *)(ERASEFLAGADDR+5) == 0xFF))//6个字符判断是否为jtag烧写程序并且未进行在线升级
    {
        app();//应用程序入口      
    }
    //不用else，因为APP程序没有出口
    if((entry_val == STATE_ALLCHECK1_YES)||(entry_val == STATE_ALLCHECK2_YES)) //初次校验成功正常进入更新程序，若第二级校验成功则表示原有应用以擦除。
    {
        //updatecount = *(unsigned char *)UPCOUNT+1;//累加在线更新次数
       // Flash_EraseSegment(UPCOUNTHead);//擦除地址0x45A00-0x45BFF第557段
        //UPWriteFlashByte(UPCOUNT,updatecount);//将更新次数存入内部Flash中，在完成更新之后清零（擦除为0xFF），或者在使用jtag下载程序时自动擦除成0xFF
      
        UPSst26vf064b_Read(FLASH_UPCOUNTER,updatecount,2);//flash读取
        if(updatecount[1] != ~updatecount[0])//保证读出数据有效,无效则清零，是升级继续进行
        {
            updatecount[0] = 0;
        }
        if((updatecount[0] <= 0xA)||(updatecount[0] == 0xFF))//为防止jtag下载时擦除为0xff，采用累加和"!="运算，而不是“>="运算
        {
            updatecount[0]++;
            UPSector_Erase(FLASH_UPCOUNTER);
            updatecount[1] = ~updatecount[0];
            UPSst26vf064b_Page_WriteBytes(FLASH_UPCOUNTER,updatecount,2);
            update(FrameSum,SumCheck,eraseflag);//程序更新入口
        }
        else
        {
             unsigned int k;
              while(1)//运行灯慢闪
              {
                WTD_TOG;
                LIGHT_RUN_FLASH();
                for(k = 0; k < 3; k++)
                for(k = 0; k < 65530; k++);
              }
        }
        
    }
    else
    {
        app();//应用程序入口
    }
}

unsigned char ReadArry[WRITEPACK];//读缓存3个基地址，3个偏移地址，1个数据长度，224个数据，1个校验值
//unsigned char WriteArry[200];//写缓存
//******************************************************************
//*函数名：update
//*功能：代码更新程序，对外部flash内的数据进行打包校验，确保内部擦除前外部flash内的数据有效
//*描述：单帧数据校验和总体校验，有错误就记录到flash内，并复位退出升级状态
//*FrameSum 帧总数  SumCheck打包校验值，双字节累加和
//*EraseFlag内部flash是否擦除，YES已擦除，NO未擦除，在未擦除的情况下允许将升级状态改变为错误状态等待重启，
//*         可以进入原有程序继续运行
//*         否则，不允许更改升级状态直接复位，否则改变状态之后可能进入原有被擦除程序就会跑飞
//*         并且无法进行再一次的自动重启升级
//*********************************************************************
void update(unsigned int FrameSum,unsigned int SumCheck,unsigned char EraseFlag)@"UPDATECODE"
{
  unsigned int  tmpcheck = 0;//单帧校验中间结果
  unsigned int  Tcheck = 0;//打包校验中间结果
  unsigned int  tmp;
  unsigned int  FReadTime;//帧读计数，全部读取计数    ,ReadTime
  unsigned char FReadFlag;//帧读标志                  ,ReadFlag
  unsigned int  i;
  unsigned char UpState;
  unsigned long Address;
  unsigned char c = 0;
  
 // while((ReadFlag==0x44)&&(ReadTime<2))
  {
      Address = FLASH_UPDATE_START;//数据起始地址，在每处理完一帧之后进行累加
      for(i=0;i<FrameSum;i++)//打包校验
      {
          c++;
          if(c>20)
          {
              c = 0;
              LIGHT_RUN_FLASH();
          }//指示灯状态

          
          WTD_TOG;
          FReadFlag = NO;
          FReadTime = 0;
          while((FReadFlag==NO)&&(FReadTime<2))
          {
              UPSst26vf064b_Read(Address,ReadArry,WRITEPACK);
              tmp = ReadArry[(7+ReadArry[6])];//校验值
              tmpcheck = singleFrameSumCheck((ReadArry+7),(ReadArry[6]),0);
              tmpcheck &= 0xff;
              if(tmpcheck == tmp)//单帧校验正确则将数据帧加入到打包校验中
              {
                  Tcheck = singleFrameSumCheck((ReadArry+7),(ReadArry[6]),Tcheck);//累加和校验
                  FReadFlag = YES;//帧读取成功标志
              }
              else
              {
                  FReadTime++;
              }
          }
          if(FReadFlag == NO)
          {//错误超限处理
            
            //内部flash未擦除，未擦除的情况下改变状态等待重启，
            //可以进入原有程序继续运行，否则，原有程序被擦除程序就会跑飞
            //并且无法进行再一次的自动重启升级
            if(EraseFlag == NO)
            {
              UpState = STATE_SCHECK_NO;//flash单帧读取校验错误
              WriteUpdate(FrameSum,SumCheck,UpState,EraseFlag);//写入状态
            }         
              _DINT();
              unsigned int k;
              while(1)//运行灯慢闪
              {
                LIGHT_RUN_FLASH();
                for(k = 0; k < 3; k++)
                for(k = 0; k < 65530; k++);
              }
          }
          tmpcheck = 0;//累加和校验初值
          Address += WRITEPACK;//不使用乘法运算
      }
      if(SumCheck == Tcheck)//打包校验
      {
          UpState = STATE_ALLCHECK2_YES;//打包校验正确
          EraseUpdataFlash();
          WriteUpdate(FrameSum,SumCheck,UpState,YES);  //写入校验，计数，状态，flash擦除标志置YES        
          code_copy(FrameSum,SumCheck);//复制操作
      }
      else
      {
          if(EraseFlag == NO)//内部flash未擦除
          {
            UpState = STATE_ALLCHECK2_NO;//打包校验错误
            WriteUpdate(FrameSum,SumCheck,UpState,EraseFlag);//结果写入外部flash
          }
            //Flash_EraseSegment((unsigned long *) INFLASH_COUNTER);
            //UPWriteFlashByte(INFLASH_STATE, 0X23);//写入flash更新错误
            //abnormal2();//指示灯状态
            //ResetCPU();//复位cpu
           _DINT();
           unsigned int k;
              while(1)//运行灯慢闪
              {
                LIGHT_RUN_FLASH();
                for(k = 0; k < 3; k++)
                for(k = 0; k < 65530; k++);
              }
      }
  }
}

unsigned int code_copy_crc(unsigned int length,unsigned char *data,unsigned int val)@"UPDATECODE"//length:长度，*data：数据指针，val:保存的中间校验值
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

unsigned int singleFrameCheck(unsigned char *data,unsigned int tmpcheck)@"UPDATECODE"//读取单帧数据，起始6字节地址、1字节长度、200字节数据，2字节校验码
{
    unsigned char len ;
    unsigned int  res;
    len = *(data+6);//仅校验数据
    //for(i=0;i<len;i++)
    {
        res =  code_copy_crc(len,(data+7),tmpcheck); 
        //(unsigned int length,unsigned char *data,unsigned int val)
    }
    return res;
}
//====================================================================
//累加和校验，配合原来CRC校验预留的两字节空间，将数据类型预留未int型
//  *data 数据  len 长度   tmpcheck 校验初始值用于多帧数据累计校验结果，单帧校验为0
//
//====================================================================
unsigned int singleFrameSumCheck(unsigned char *data,unsigned char len,unsigned int tmpcheck)@"UPDATECODE"
{
    //unsigned char len ;
   // unsigned int  res;
    unsigned char i;
    //len = *(data+6);//仅校验数据
    
    for(i=0;i<len;i++)
    {
        tmpcheck += *data;
        data++;
    }
    tmpcheck &= 0xffff;
    return tmpcheck;
}

void Flash_EraseSegment(unsigned long  pAddr)@"UPDATECODE"
{
    volatile unsigned int * ptr;                    //addr为该段的首地址
    unsigned int j;
    
    _DINT();  
    WDTCTL = WDTPW + WDTHOLD;               ////禁止内部看门狗
    //FEED_WATCH_DOG();                             ////清外部看门狗
    WTD_TOG;
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
    //return 1;
}
//==============================================================================
//name:EraseUpdataFlash
//function:擦除flash地址0x7c00-0x3ffff
//         当擦除到复位向量块（66）时将
//         原来向量的内容重新写入，以防
//         止擦除过程中意外复位后无法找
//         到程序入口
//==============================================================================
void EraseUpdataFlash(void)@"UPDATECODE"
{
    unsigned long i = 0;
    unsigned long addr = AppStartAddr;
    unsigned char resval[2];//复位向量值
    LED_RUN_ON;
    resval[0] = *(unsigned char *)0xFFFE;//先保存复位向量值
    resval[1] = *(unsigned char *)0xFFFF; 
    do
    { 
      Flash_EraseSegment(addr);
        addr += 512;
        i++;
        if(i == 66)//擦除复位向量所在块即0xFFFE-0xFFFF
        {//将复位向量内容重新写入到复位复位地址内
            UPWriteFlashByte(0xFFFE, resval[0]);
            UPWriteFlashByte(0xFFFF, resval[1]);
        }
    }
    while(i<AppSegNo);
    LIGHT_RUN_UP();
}
//*************************************************************************
//*函数名:code_copy
//*功能：将外部flash中的代码段复制到内部flash中的应用区
//*描述：先对读进行校验，校验正确再进行写，写完之后再进行校验，若有校验不成功则等待复位并不改变flash内的标志值
//*完成之后再更新并写入flash标志值
//**************************************************************************
void code_copy(unsigned int FrameSum,unsigned int SumCheck)@"UPDATECODE"//framesum：帧总数
{
    //unsigned long unAddr;
    unsigned long Address;//绝对地址
    unsigned long Flash_Addr;
    unsigned long base_addr;
    unsigned long offset_addr;   
    unsigned char frame_no;//帧内数据个数
    unsigned int  check = 0;//重新计算单帧校验值
    unsigned int  tmpcheck = 0;//读回校验值
    unsigned char check_flag,check_time;//校验结果标志，校验错误次数
    unsigned char write_flag,write_time;//写入标志
    unsigned int i,j,k;
    unsigned int c = 0;
    unsigned char ledstate = 0;
    Flash_Addr = FLASH_UPDATE_START;
    for(i=0;i<FrameSum;i++)
    {
        
          c++;
          if(c>20)//异常指示灯慢闪
          {
              ledstate = ~ledstate;
              c = 0;
              LIGHT_RUN_FLASH();
          }//指示灯状态

          
        //从外部flash读取数据  
        check_flag = NO;
        check_time = 0;
        while((check_flag == NO)&&(check_time < 2))//读取并校验数据
        {
            //Sst26vf064b_Read((FLASH_UPDATE_START+i*WRITEPACK),ReadArry,WRITEPACK);
            WTD_TOG;            
            UPSst26vf064b_Read(Flash_Addr,ReadArry,WRITEPACK);
            
            base_addr = ReadArry[2];
            base_addr = (base_addr<<8)|ReadArry[1];
            base_addr = (base_addr<<8)|ReadArry[0];
            base_addr &= 0xFFFFFF;//基地址
            offset_addr = ReadArry[5];
            offset_addr = (offset_addr<<8)|ReadArry[4];
            offset_addr = (offset_addr<<8)|ReadArry[3]; 
            offset_addr &= 0xFFFFFF;//偏移地址
            Address = base_addr+offset_addr;
            frame_no = ReadArry[6];//帧内字节数
            tmpcheck = ReadArry[frame_no+7];//累加和校验仅占一位
            //tmpcheck = (tmpcheck<<8)|ReadArry[207];//读取
            check = singleFrameSumCheck((ReadArry+7),frame_no,0);//读取校验
            check &= 0xff;//单字节
            if(check == tmpcheck)
            {
                check_flag = YES;//单帧校验正确
            }
            else
            {
                check_flag = NO;//单帧校验错误
                check_time++;
            }
        }
        if(check_flag == NO)//读取错误直接复位不改变flash中标志数据，重启后再次进行此操作，
        {
            _DINT();
            unsigned int k;
              while(1)//运行灯慢闪
              {
                LIGHT_RUN_FLASH();
                for(k = 0; k < 3; k++)
                for(k = 0; k < 65530; k++);
              }
        }
        
        //向内部flash写入数据
        write_flag = NO;
        write_time = 0;
        while((write_flag == NO)&&(write_time<2))
        {
            WTD_TOG;            
          //**********************
            //word wirte
            for(k=0;k<frame_no;k+=2)
                UPWriteFlashWord((Address+k), &ReadArry[7+k]);  
            
            //*****块写入***********
            //只能实现long word 写入，无法保证段最后一帧数据量正好是4的倍数写入错误数据
            //UPWriteFlashBytes(Address,&ReadArry[7],frame_no);
              
            for (j=0;j<frame_no;j++)//写入完成之后对比写入结果是否正确
            {
                if(*((unsigned char *)(Address+j)) == ReadArry[7+j])
                {
                    write_flag = YES;
                }
                else
                {
                    write_flag = NO;
                    write_time++;
                    break;
                }
            }//写入校验
        }
        if(write_flag == NO)//写入错误
        {
            _DINT();
            unsigned int k;
            while(1)//运行灯慢闪
            {
              LIGHT_RUN_FLASH();
              for(k = 0; k < 3; k++)
                for(k = 0; k < 65530; k++);
            }
        }
        Flash_Addr += WRITEPACK;//不能使用乘法运算，只能累加地址
    }
    WTD_TOG;
    WriteUpdate(FrameSum,SumCheck,STATE_COMPLETE,NO);//更新完成
    //Flash_EraseSegment(UPCOUNTHead);
    UPSector_Erase(FLASH_UPCOUNTER);//初始化，更新次数计数
    _DINT();
    while(1)//运行灯快闪
    {
      LIGHT_RUN_FLASH();
      for(k = 0; k < 65530; k++);
    }
}

//***************************************************************
//函数名：WriteUpdate
//功能：外部flash入口标志写入函数
//说明；FrameSum ,SumCheck,UpSate,EraseFlag一起加校验写入，写入前需要擦除操作
//      EraseFlag:内部flash是否被擦除，YES：以擦除，原有程序无效，NO：未擦除原有程序有效，
//        此标志为了防止在擦除原有程序并重启时的校验过程中可能造成的更改升级状态造成寻找
//        原有程序入口的问题,在内部flash擦出是置YES，完成更新或者下载完成之后置NO
//***************************************************************
void WriteUpdate(unsigned int FrameSum,unsigned int SumCheck,unsigned char UpState,unsigned char EraseFlag)@"UPDATECODE"
{
      unsigned char  SaveParameter[8];   
      unsigned int   tmpcheck;
      SaveParameter[0] = FrameSum&0x00FF;//入口保存数据整理
      SaveParameter[1] = (FrameSum>>8)&0x00FF;
      SaveParameter[2] = SumCheck&0x00FF;
      SaveParameter[3] = (SumCheck>>8)&0x00FF;
      SaveParameter[4] = UpState;
      SaveParameter[5] = EraseFlag;
      tmpcheck = code_copy_crc(6,SaveParameter,0xFFFF);
      SaveParameter[6] = tmpcheck&0x00ff;
      SaveParameter[7] = (tmpcheck>>8)&0x00ff;   
              
      UPSector_Erase(FLASH_UPDATE_COUNTER);
     
      UPSst26vf064b_Page_WriteBytes(FLASH_UPDATE_COUNTER,SaveParameter,8);
      UPSst26vf064b_Page_WriteBytes(FLASH_UPDATEBK_COUNTER,SaveParameter,8);//备份区
}

//***************************************************************
//*函数名：ResetFun
//*功能：在完成整体擦除之后将复位地址内容写入到0xFFFE-0xFFFF中
//***************************************************************
void ResetFun(void)@"UPDATECODE"
{
    
}


//字节写入内部flash
void UPWriteFlashByte(unsigned long dst, unsigned char data)@"UPDATECODE"
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

//字写入flash
void UPWriteFlashWord(unsigned long dst, unsigned char *data)@"UPDATECODE"
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
    *ptr++ = *data++;
    *ptr = *data;
	//FEED_WATCH_DOG();
    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
    _EINT();
}
//=================================================
//写入内部flash
//dst flash：地址   *data:数据指针   length：长度
//块写入需要在内存中运行
//=================================================
void UPWriteFlashBytes(unsigned long dst, unsigned char *data,unsigned char length)@"UPDATECODE"
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
    FCTL1 = FWKEY+WRT+BLKWRT;                        // BLKWRT= WRT=1 block write
    for(i = 0;i <length;i++)
    {
      *ptr++ = *data++;
      while(!(FCTL3 & WAIT));//等待WAIT位为1后进行下一数据的写入
        
    }
    for(i = 0; i < 1000; i++)
    {
      if((FCTL3 & BUSY) != BUSY)              //不忙则跳出,完成擦除
      break;
    }	
    //FEED_WATCH_DOG();
    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
    _EINT();
}

//==============================================================================
//  函数名称   : InitSPI
//  功能描述   : SPI发送一字节数据
//  输入参数   : ucData: 待写入数据
//  输出参数   ：<无>
//  返回值     : UCB0TXBUF寄存器数据
//  其他说明   : 
//  作者       ：
//==============================================================================
/*
unsigned char UpSPIWriteBuf(unsigned char ucData)@"UPDATECODE"
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
*/

//==============================================================================
//  函数名称   : UPUCB0SPIInit
//  功能描述   : UPUCB0SPI总线初始化工作，
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void UPUCB0SPIInit(void)@"UPDATECODE"
{
    UCB0CTL1 |= UCSWRST;                          // **Put state machine in reset** 
    UCB0CTL1 |= UCSSEL_1;                         // ACLK
    UCB0CTL0 |= UCMST + UCSYNC + UCCKPH + UCMSB;   // 3-pin, 8-bit SPI master  + UCCKPL
                                                  // Clock polarity high, MSB
   
    UCB0BR0 = 0x02;                               // /4
    UCB0BR1 = 0;                                  
           
    UCB0CTL1 &= ~UCSWRST;                         // **Initialize USCI state machine**    

 
    UCB2CTL1 |= UCSWRST;                          // **Put state machine in reset**   
    UCB2CTL0 |= UCMST + UCSYNC + UCCKPL + UCMSB;  // 3-pin, 8-bit SPI master
                                                  // Clock polarity high, MSB
    UCB2CTL1 |= UCSSEL_1;                         // ACLK
    UCB2BR0 = 0x04;                               // /4
    UCB2BR1 = 0;                                  
           
    UCB2CTL1 &= ~UCSWRST;                         // **Initialize USCI state machine**    
    //CAT_UNSELECT_CHIP;                            //CAT25128片选不使能
    //CAT_WP;                                       //CAT25128写保护    
    //CAT_SpiConcelWP();                            //25128写使能
    
    //Sst_Reset();                                  //复位Flash芯片
    //Sst_Unlock();                                 //解锁Flash芯片


    UPSst_Reset();                                  //复位Flash芯片
    UPSst_Unlock();                                 //解锁Flash芯片
    

}


#define UPSELECT_CHIP      P8OUT &= ~BIT1
#define UPUNSELECT_CHIP    P8OUT |= BIT1
#define UPSST_READ		   (0x03)
#define UPSST_WRITE		   (0x02)
#define UPSTATUS_REGISTER            (0x05)
#define UPWREN		  	   (0x06)
#define UPWRDI			   (0x04)
#define UPSECTOR_ERASE		   (0x20)
#define UPRSTEN		       	   (0x66)
#define UPRST			   (0x99)
#define UPULBPR			   (0x98)
#define UPEWSR			   (0x50)
#define UPWRSR			   (0x01)

//==============================================================================
//  函数名称   : UPInitSPI
//  功能描述   : SPI发送一字节数据
//  输入参数   : ucData: 待写入数据
//  输出参数   ：<无>
//  返回值     : UCB0TXBUF寄存器数据
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned char UPSPIWriteBuf(unsigned char ucData)@"UPDATECODE"
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
//  函数名称   : UPInitSPI
//  功能描述   : SPI接收一字节数据
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : UCB0RXBUF寄存器数据
//  其他说明   : 
//  作者       ：
//==============================================================================
unsigned char UPSPIReadBuf(void)@"UPDATECODE"
{
    unsigned char i;
    UPSPIWriteBuf(0);
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
//  函数名称   : UPSpiWaitBusy
//  功能描述   : 通过读状态寄存器检测忙状态
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   :
//  作者       ：
//==============================================================================
 void UPSpiWaitBusy(void)@"UPDATECODE"
{
    unsigned char temp = 0;
    unsigned int a = 0;
	
	UPSELECT_CHIP;
        UPSPIWriteBuf(UPSTATUS_REGISTER);
        //SPIWriteBuf(0x35);

	for(a=0;a<2000;a++)
	{
		temp = UPSPIReadBuf();
		temp&=0x81;
		if(temp==0)
		{
		    break;
		}
	}
	UPUNSELECT_CHIP;
}

 //==============================================================================
 //  函数名称   : Sst26vf064b_Read
 //  功能描述   : 读数据
 //  输入参数   : unAddr：缓冲区绝对地址 ； PcData：目标数据存储指针 ；Size：读取数据个数最；
 //  输出参数   ：<无>
 //  返回值     : 	<无>
 //  其他说明   :
 //  作者       ：
 //==============================================================================
 void UPSst26vf064b_Read(long unAddr,unsigned char *PcData,unsigned int Size)@"UPDATECODE"
 {
     unsigned int i;
     UPSpiWaitBusy();

 	UPSELECT_CHIP;
 	UPSPIWriteBuf(UPSST_READ);
 	UPSPIWriteBuf((unsigned char)(unAddr >> 16));
 	UPSPIWriteBuf((unsigned char)(unAddr >> 8));
        UPSPIWriteBuf((unsigned char) unAddr);
    for(i = 0;i < Size;i++)
    {  	
      *PcData= UPSPIReadBuf();  
     	PcData++;
    }
 	UPUNSELECT_CHIP;
 }

void UPdelay(unsigned int unDelayNum)@"UPDATECODE"
{
    unsigned int i;
    for(i = 0;i < unDelayNum;i++)
    {
       ;
    }
}

 //==============================================================================
 //  函数名称   : Sst26vf064b_Page_Write
 //  功能描述   : 写数据
 //  输入参数   : unAddr：缓冲区绝对地址；  PcData：目标数据存储指针； Size：写入数据个数(<=256，注意跨页)
 //  输出参数   ：<无>
 //  返回值     : <无>
 //  其他说明   :
 //  作者       ：
 //==============================================================================

 void UPSst26vf064b_Page_WriteBytes(long unAddr,unsigned char *PcData,unsigned int Size)@"UPDATECODE"
 {
     unsigned int i;
     UPSpiWaitBusy();

     UPSELECT_CHIP;
    // delay(1);
     UPSPIWriteBuf(UPWREN);							//允许写
    // Flash_cs=1;
    // delay(1);
     UPUNSELECT_CHIP;

    // Flash_cs=0;
     UPSELECT_CHIP;
     UPSPIWriteBuf(UPSST_WRITE);						//写指令
     UPSPIWriteBuf((unsigned char)(unAddr >> 16));	//地址
     UPSPIWriteBuf((unsigned char)(unAddr >> 8));
     UPSPIWriteBuf((unsigned char) unAddr);
     for(i = 0;i < Size;i++)					//数据
     {
    	UPSPIWriteBuf(*PcData);
    	PcData++;
     }
     UPdelay(20);
     UPUNSELECT_CHIP;

     UPSELECT_CHIP;
  //   delay(1);
     UPSPIWriteBuf(UPWRDI);							//禁止写
     //Flash_cs=1;
  //   delay(1);
     UPUNSELECT_CHIP;
 }

 //==============================================================================
 //  函数名称   :Sector_Erase
 //  功能描述   : 块擦除 (4k Bytes)
 //  输入参数   : 块地址，低12位无效
 //  输出参数   ：<无>
 //  返回值     : <无>
 //  其他说明   :
 //  作者       ：
 //==============================================================================
 void UPSector_Erase(long unBlock)@"UPDATECODE"
 {
     UPSpiWaitBusy();

     UPSELECT_CHIP;
     UPSPIWriteBuf(UPWREN);
     UPUNSELECT_CHIP;

     UPSELECT_CHIP;
     UPSPIWriteBuf(UPSECTOR_ERASE);
     UPSPIWriteBuf((unsigned char)(unBlock >> 16));
     UPSPIWriteBuf((unsigned char)(unBlock >> 8));
     UPSPIWriteBuf((unsigned char)unBlock);
     UPUNSELECT_CHIP;

     UPSELECT_CHIP;
     UPSPIWriteBuf(UPWRDI);
     UPUNSELECT_CHIP;
     
     UPdelay(ERASE_DELAY);//擦除命令发出之后延时一段时间
 }

//==============================================================================
//  函数名称   : Sst_Reset
//  功能描述   : 复位芯片
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
 void UPSst_Reset(void)@"UPDATECODE"
{
    UPSELECT_CHIP;
    UPSPIWriteBuf(UPRSTEN);
    UPUNSELECT_CHIP;
    UPSELECT_CHIP;
    UPSPIWriteBuf(UPRST);
    UPUNSELECT_CHIP;
}
 //==============================================================================
 //  函数名称   : Sst_Unlock
 //  功能描述   : 解锁芯片
 //  输入参数   : <无>
 //  输出参数   ：<无>
 //  返回值     : <无>
 //  其他说明   :
 //  作者       ：
 //==============================================================================
void UPSst_Unlock(void)@"UPDATECODE"
{
     UPSELECT_CHIP;
     UPSPIWriteBuf(UPWREN);
     UPUNSELECT_CHIP;
     UPSELECT_CHIP;
     UPSPIWriteBuf(UPULBPR);
     UPUNSELECT_CHIP;
     UPSELECT_CHIP;         //sst25上电需要手动清除块保护状态
     UPSPIWriteBuf(UPEWSR);
     UPUNSELECT_CHIP;
     UPSELECT_CHIP;
     UPSPIWriteBuf(UPWRSR);
     UPSPIWriteBuf(0x00);  //解除块保护
     UPUNSELECT_CHIP;
}
//==============================================================================
//  函数名称   : UpInitClk
//  功能描述   : 系统用到的时钟进行初始化
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void UpInitClk(void)@"UPDATECODE"
{ 
    unsigned int i;
    PMMCTL0_H = 0xA5; // Open PMM module
    PMMCTL0 = 0xA500 + PMMCOREV_3; // Set VCore
    SVSMLCTL = SVMLE + (PMMCOREV_3 * SVSMLRRL0);// Set SVM new Level 
    while ((PMMIFG & SVSMLDLYIFG) == 0); // Wait till SVM is settled (Delay)
    PMMIFG &= ~(SVMLVLRIFG + SVMLIFG); // Clear already set flags
    if ((PMMIFG & SVMLIFG)) 
     while ((PMMIFG & SVMLVLRIFG) == 0); // Wait till level is reached
    PMMCTL0_H = 0x00; // Lock PMM module registers 
    //InitPort();//IO端口初始化
    P5SEL |= 0x04;                            // Port select XT2 
    UCSCTL0 = 0x1F00;                           // Set lowest possible DCOx, MODx   *注：DCO =31 配合DCO更改为24M
    
    UCSCTL1 = DCORSEL_5;                      // Select range for 18MHz operation
    UCSCTL2 = 732;                            // Set DCO Multiplier for 16MHz
                                            // (N + 1) * FLLRef = Fdco
                                            // (733 + 1) * 32768 = 24MHz
    UCSCTL6 &= ~XT2OFF;                       // Enable XT2 even if not used
 //发现XT1晶振报错，由于晶振未使用，加程序软件关闭XT1  
    UCSCTL6 |= XT1OFF;                     
    UCSCTL3 |= SELREF_2;                     // FLLref = REFO
                                            // Since LFXT1 is not used,
                                            // sourcing FLL with LFXT1 can cause
                                            // XT1OFFG flag to set
    UCSCTL4 |= SELA_3;                        // ACLK=REFO,SMCLK=DCO,MCLK=DCO
    UCSCTL6 |= XT2BYPASS;
    UCSCTL6 &= ~(XT2DRIVE_2);
  // Loop until XT2 & DCO stabilize
    do
    {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
        SFRIFG1 &= ~OFIFG;                      // Clear fault flags
        for(i=0;i<0xFFFF;i++);                  // Delay for Osc to stabilize
    }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
    UCSCTL4 |= SELS_4 + SELM_4+SELA_5;               // SMCLK=MCLK=DCO  ACLK = XT2    
}
void UPPortInit(void)@"UPDATECODE"
{

    P1SEL = 0x00;
    P2SEL = 0x00;
    P3SEL |= 0x3e;        //P3.1-2 SPI0引脚，P3.4-5 串口0引脚

    P4SEL = 0x00;        
    P5SEL |= 0xc7;        //P5.0-1 Vref+,-   P5.2 x2clkIN   P5.6-7 串口1
    P6SEL |= 0xf0;        //AD输入4-7 Ua/Ub/Uc/U0

    P7SEL |= 0x10;        //  P7.4 AD输入I0
    P8SEL = 0x00;
    P9SEL |= 0x3e;        //P9.1-3 SPI2  4-5串口2 

    P10SEL |= 0x30;       //P10.4-5 串口3引脚
    P11SEL = 0x00;
    
    P1DIR = 0x00;         //
    P2DIR |= 0x04;        //P2.2 W5500RST
    P3DIR |= 0x98;        //P3.4 TXD0,P3.3 SPI0 CLK, P3.0 W5500INT, P3.7 SPI0 CS0

    P4DIR |= 0xc0;        //P4.3-5 KJA1-KJC1输入  P4.6-7 Ka1/Kb1
    P4OUT = 0;
    P5DIR |= 0x59;        //P5.0 输出VREF+ P5.2晶振输入 P5.3 WDG_CLR P5.4 Kc1输出 P5.5 P-V输入 P5.6 TXD1
    P6DIR = 0x0f;         //AD输入4-7  P6.0 运行灯RUN P6.1-3 时钟芯片IIC SCLK/DATA/CE

    P7DIR |= 0x0d;        //P7.2 Ka0, P7.3 Kb0
    P7OUT = 0;

    P8DIR = 0x83;   //P8.0 Kc0  P8.7 EEPROM_WP    P8.1FlashCS0
    P8OUT = 0;
        
    P9DIR |= 0x19;        //P9.0 SCS P9.3 CLK , P9.4 TXD2
}


//使用说明：
//*指示灯状态说明：
//*1.正常工作状态下，运行灯慢闪
//*2.程序下载过程中，运行灯慢闪 6min
//*3.下载完成之后复位进行程序更新后，运行灯快闪
//***打包校验过程异常灯亮，重合灯慢闪 1min
//***内部flash 擦除时异常等和重合灯均灭
//***代码复制阶段异常灯慢闪，重合灯慢闪 1.5min
//*4.完成更新并重启之后，运行灯慢闪
//*5.异常状态指示异常灯和重合灯同步快闪
//注意事项：
//1.确定更新代码符合在线升级框架，能够进行程序再次升级
//2.确定更新代码内容仅包括0x7c00-0x3e200之间的代码
//3.启动升级之前需要检查设备状态
//检查完成之后通过检车设备状态查看升级情况：若没有响应则未完成或失败，正常响应则看程序版本时候更新，更新则完成升级，否则未升级

//编译说明
//链接文件不使用默认设置，而是自定义文件
//cstartup文件也需要按照功能进行修改