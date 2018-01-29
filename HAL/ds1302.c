#ifndef _DS1302

#define  _DS1302
  #include "..\DataStruct.h"
  //#include "ds1302.h"
#endif

#define DS1302_DIR   P6DIR       //方向
#define DS1302_IN    P6IN       
#define DS1302_OUT   P6OUT

#define DS1302_RST   BIT3
#define DS1302_SCLK  BIT1
#define DS1302_SDI   BIT2  //定义MSP320的端口

#define DS1302_RST_LO   DS1302_OUT &= ~DS1302_RST
#define DS1302_RST_HI   DS1302_OUT |= DS1302_RST
#define DS1302_SCLK_LO  DS1302_OUT &= ~DS1302_SCLK
#define DS1302_SCLK_HI  DS1302_OUT |= DS1302_SCLK
#define DS1302_SDI_LO   DS1302_OUT &= ~DS1302_SDI
#define DS1302_SDI_HI   DS1302_OUT |= DS1302_SDI

//延时函数
void DS1302_Delay(unsigned int dtime) 
  {
   unsigned int temps;
   for(temps=0;temps<dtime;temps++);
  }

//DS1302复位
void DS1302_Reset(void) {
 DS1302_DIR |= (DS1302_RST + DS1302_SCLK);
 DS1302_SCLK_LO;
 DS1302_RST_LO;
 DS1302_Delay(6);
 DS1302_SCLK_HI;
}

//向DS1302写入一个字节
void DS1302_WriteOneByte(unsigned char w_dat) {
 unsigned char tempa;
 
 DS1302_RST_HI;
 DS1302_DIR |= DS1302_SDI;
 
 for(tempa=8;tempa>0;tempa--) {
  DS1302_SDI_LO;
  if(w_dat&BIT0) DS1302_SDI_HI;
  DS1302_SCLK_LO;
  DS1302_Delay(6);
  DS1302_SCLK_HI;
  DS1302_Delay(6);
  w_dat >>=1;
 } 
}

//从DS1302中读取一个字节
unsigned char DS1302_ReadOneByte(void) { 
 unsigned char tempf,rdata;
 rdata = 0x00;
 
 DS1302_RST_HI;
 DS1302_DIR &= ~DS1302_SDI;
 
 for(tempf=0;tempf<8;tempf++){rdata >>= 1;//将移位放到前面，否则读不到最高一位
  DS1302_SCLK_HI;
  DS1302_Delay(6);
  DS1302_SCLK_LO;
  DS1302_Delay(6);
  if((DS1302_IN&DS1302_SDI)==DS1302_SDI)
   rdata |= BIT7;
  //rdata >>= 1;
 }
 
 return(rdata); 
}

//向DS1302中写入地址后写入数据
void DS1302_WriteData(unsigned char addr,unsigned char w_dat) {
 DS1302_RST_LO;
 DS1302_SCLK_LO;
 DS1302_RST_HI;
 
 DS1302_WriteOneByte(addr);  //写入地址
 DS1302_WriteOneByte(w_dat);  //写入数据
 
 DS1302_SCLK_HI;
 DS1302_RST_LO;
} 

//向DS1302写入地址后,从DS1302中读取数据
unsigned char DS1302_ReadData(unsigned char addr) {
 unsigned char r_dat;
 DS1302_RST_LO;
 DS1302_SCLK_LO;
 DS1302_RST_HI;
 
 DS1302_WriteOneByte(addr);  //写入地址
 r_dat = DS1302_ReadOneByte();  //读出数据
 
 DS1302_SCLK_LO;
 DS1302_RST_LO;
 
 return(r_dat);
}

//按照SettingData的设置设置DS1302的时间
void DS1302_SettingData(unsigned char *point) {
 unsigned char tempa;
 unsigned char addr = 0x8C;
 unsigned char temp12[7];
 for(tempa=0;tempa<7;tempa++) 
 {
     if(tempa == 1)
         temp12[tempa] = 1;
     else
     {
         temp12[tempa]=*point++;
         temp12[tempa]=((temp12[tempa]/10)<<4)+(temp12[tempa]%10);
     }
 }
 DS1302_WriteData(0x8E,0x00);  //写入控制命令,禁用写保护

 for(tempa=0;tempa<7;tempa++) 
   {
    DS1302_WriteData(addr,temp12[tempa]);
    addr -= 2;
   }
 DS1302_WriteData(0x8E,0x80);  //写入控制命令,启用写保护
}

//读取DS1302时间到ReadingData中
void DS1302_GetData(unsigned char *str) {
 unsigned char tempe;
 unsigned char addr = 0x8D;

 for(tempe=0;tempe<7;tempe++) {
  str[tempe] = DS1302_ReadData(addr);//年
  addr -= 2;
 }
}

