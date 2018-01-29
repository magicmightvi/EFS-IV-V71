#ifndef _DS1302

#define  _DS1302
  #include "..\DataStruct.h"
  //#include "ds1302.h"
#endif

#define DS1302_DIR   P6DIR       //����
#define DS1302_IN    P6IN       
#define DS1302_OUT   P6OUT

#define DS1302_RST   BIT3
#define DS1302_SCLK  BIT1
#define DS1302_SDI   BIT2  //����MSP320�Ķ˿�

#define DS1302_RST_LO   DS1302_OUT &= ~DS1302_RST
#define DS1302_RST_HI   DS1302_OUT |= DS1302_RST
#define DS1302_SCLK_LO  DS1302_OUT &= ~DS1302_SCLK
#define DS1302_SCLK_HI  DS1302_OUT |= DS1302_SCLK
#define DS1302_SDI_LO   DS1302_OUT &= ~DS1302_SDI
#define DS1302_SDI_HI   DS1302_OUT |= DS1302_SDI

//��ʱ����
void DS1302_Delay(unsigned int dtime) 
  {
   unsigned int temps;
   for(temps=0;temps<dtime;temps++);
  }

//DS1302��λ
void DS1302_Reset(void) {
 DS1302_DIR |= (DS1302_RST + DS1302_SCLK);
 DS1302_SCLK_LO;
 DS1302_RST_LO;
 DS1302_Delay(6);
 DS1302_SCLK_HI;
}

//��DS1302д��һ���ֽ�
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

//��DS1302�ж�ȡһ���ֽ�
unsigned char DS1302_ReadOneByte(void) { 
 unsigned char tempf,rdata;
 rdata = 0x00;
 
 DS1302_RST_HI;
 DS1302_DIR &= ~DS1302_SDI;
 
 for(tempf=0;tempf<8;tempf++){rdata >>= 1;//����λ�ŵ�ǰ�棬������������һλ
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

//��DS1302��д���ַ��д������
void DS1302_WriteData(unsigned char addr,unsigned char w_dat) {
 DS1302_RST_LO;
 DS1302_SCLK_LO;
 DS1302_RST_HI;
 
 DS1302_WriteOneByte(addr);  //д���ַ
 DS1302_WriteOneByte(w_dat);  //д������
 
 DS1302_SCLK_HI;
 DS1302_RST_LO;
} 

//��DS1302д���ַ��,��DS1302�ж�ȡ����
unsigned char DS1302_ReadData(unsigned char addr) {
 unsigned char r_dat;
 DS1302_RST_LO;
 DS1302_SCLK_LO;
 DS1302_RST_HI;
 
 DS1302_WriteOneByte(addr);  //д���ַ
 r_dat = DS1302_ReadOneByte();  //��������
 
 DS1302_SCLK_LO;
 DS1302_RST_LO;
 
 return(r_dat);
}

//����SettingData����������DS1302��ʱ��
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
 DS1302_WriteData(0x8E,0x00);  //д���������,����д����

 for(tempa=0;tempa<7;tempa++) 
   {
    DS1302_WriteData(addr,temp12[tempa]);
    addr -= 2;
   }
 DS1302_WriteData(0x8E,0x80);  //д���������,����д����
}

//��ȡDS1302ʱ�䵽ReadingData��
void DS1302_GetData(unsigned char *str) {
 unsigned char tempe;
 unsigned char addr = 0x8D;

 for(tempe=0;tempe<7;tempe++) {
  str[tempe] = DS1302_ReadData(addr);//��
  addr -= 2;
 }
}

