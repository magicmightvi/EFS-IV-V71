
//************************************************
//*����������������0x7c00-0x3E1FF�����жϺ͸�λ����
//*                 0x7c00-0xFFFF ��ISR_CODE)�жϺ���������
//*                 0x10000-0x10FFF��APPLICATION��Ӧ�ó������
//*                 0x11000-0x3E1FF��CODE��
//*��ڼ���������Σ�0x5c00-0x7BFF��UPDATECODE��
//*״̬�洢���ݣ�0x45A00-0x45bff(Ԥ����
//*������洢����0x3E200-0x459FF��TABLE����������ʱ�ޱ�
//*************************************************
//*�ļ����� main.c
//*���ܣ�1.�ж���ڣ���ѡ�������³������Ӧ�ó���
//*      2.У���ⲿflash�е������Ƿ���Ч
//*      3.���ⲿflash�ڵ�����д�뵽�ڲ�flashӦ����
//*      4.�����е���Ӧָʾ
//*      5.��ɻ��߳���֮��λ
//**************************************************

#ifndef _BOOT
#define _BOOT
  #include "DataStruct.h"
#endif

#define AppStartAddr    0x7c00//���¶���ʼ��ַ
//#define AppEndAddr    0x3FFFF
#define AppEndAddr      0x3E1FF
#define AppSegNo        435//��������
//#define UPCOUNTHead     0x45A00//��������segmentͷ
//#define UPCOUNT         0x45BF0//���¼�����ַ�������Ʋ���Զ������
//0x45BF0�ڲ�flash�е����ݱ��������µļ�����ÿ�θ�����ɣ�����jtag����ʱ�Ĳ������������в�������
//#define WTD_TOG (P5OUT ^= BIT3)//���Ź��ź�
#define WTD_TOG P7OUT ^= BIT5;// p7.5 ���Ź� 
#define LEDDELAY 20000//led����˸��ʱ

void UPdelay(unsigned int unDelayNum);
//void UpLEDState(unsigned char almled,unsigned char recled);
//void abnormal1(void);//У����̵�֡��ȡ��У�飩У�����
//void abnormal2(void);//���У�����
//void abnormal3(void);//д����̵�֡��ȡ��У�飩����
//void abnormal4(void);//д��flash����
//void abnormal5(void);//�쳣�����ۼƳ��޴���ָʾ

void UPPortInit(void);
void UPSst_Reset(void);
void UPSst_Unlock(void);
void UPWriteFlashWord(unsigned long dst, unsigned char *data);
void UPWriteFlashBytes(unsigned long dst, unsigned char *data,unsigned char length);
unsigned int singleFrameSumCheck(unsigned char *data,unsigned char len,unsigned int tmpcheck);
void UPSst26vf064b_Page_WriteBytes(long unAddr,unsigned char *PcData,unsigned int Size);
void UPSector_Erase(long unBlock);
unsigned int code_copy_crc(unsigned int length,unsigned char *data,unsigned int val);//length:���ȣ�*data������ָ�룬val:������м�У��ֵ
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
    
   /* unsigned char entry_val;
    unsigned char SaveParameter[8];
    unsigned int  FrameSum;
    unsigned int  SumCheck;
    unsigned int  check;//crcУ��
    unsigned char updatecount[2];//������¼���
    unsigned char eraseflag;//�ڲ�flash������־
    */
    WDTCTL = WDTPW + WDTHOLD;
    _DINT();
   /* UPPortInit();//�˿ڳ�ʼ��
    UpInitClk();//��ʼ��ʱ��
    LIGHT_RUN_DOWN();//���е���
    UPUCB0SPIInit();//SPI��ʼ��
    UPSst26vf064b_Read(FLASH_UPDATE_COUNTER,SaveParameter,8);//flash��ȡ
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
        UPSst26vf064b_Read(FLASH_UPDATEBK_COUNTER,SaveParameter,8);//flash��ȡ
        FrameSum = SaveParameter[1];
        FrameSum = (FrameSum<<8)|SaveParameter[0];//֡������
        SumCheck = SaveParameter[3];
        SumCheck = (SumCheck<<8)|SaveParameter[2];//У��ͣ�
        entry_val = SaveParameter[4];//����״̬
        eraseflag = SaveParameter[5];
        check = SaveParameter[7];
        check = (check<<8)|SaveParameter[6];
        if(check != code_copy_crc(6,SaveParameter,0xFFFF))
        {
            entry_val = 0;//������󣬲��øı䣬ֱ�ӽ������
        }
      
    }
    //entry_val = (*(unsigned int*)INFLASH_STATE)&0XFF;//��ȡ��ڱ�־��״ֵ̬������flash��
    WTD_TOG;
    if((*(unsigned char *)ERASEFLAGADDR == 0xFF)&&(*(unsigned char *)(ERASEFLAGADDR+1) == 0xFF)&&(*(unsigned char *)(ERASEFLAGADDR+2) == 0xFF)&&(*(unsigned char *)(ERASEFLAGADDR+3) == 0xFF)&&(*(unsigned char *)(ERASEFLAGADDR+4) == 0xFF)&&(*(unsigned char *)(ERASEFLAGADDR+5) == 0xFF))//6���ַ��ж��Ƿ�Ϊjtag��д������δ������������
    {*/
        app();//Ӧ�ó������      
   // }
    //����else����ΪAPP����û�г���
   /* if((entry_val == STATE_ALLCHECK1_YES)||(entry_val == STATE_ALLCHECK2_YES)) //����У��ɹ�����������³������ڶ���У��ɹ����ʾԭ��Ӧ���Բ�����
    {
        //updatecount = *(unsigned char *)UPCOUNT+1;//�ۼ����߸��´���
       // Flash_EraseSegment(UPCOUNTHead);//������ַ0x45A00-0x45BFF��557��
        //UPWriteFlashByte(UPCOUNT,updatecount);//�����´��������ڲ�Flash�У�����ɸ���֮�����㣨����Ϊ0xFF����������ʹ��jtag���س���ʱ�Զ�������0xFF
      
        UPSst26vf064b_Read(FLASH_UPCOUNTER,updatecount,2);//flash��ȡ
        if(updatecount[1] != ~updatecount[0])//��֤����������Ч,��Ч�����㣬��������������
        {
            updatecount[0] = 0;
        }
        if((updatecount[0] <= 0xA)||(updatecount[0] == 0xFF))//Ϊ��ֹjtag����ʱ����Ϊ0xff�������ۼӺ�"!="���㣬�����ǡ�>="����
        {
            updatecount[0]++;
            UPSector_Erase(FLASH_UPCOUNTER);
            updatecount[1] = ~updatecount[0];
            UPSst26vf064b_Page_WriteBytes(FLASH_UPCOUNTER,updatecount,2);
            update(FrameSum,SumCheck,eraseflag);//����������
        }
        else
        {
             unsigned int k;
              while(1)//���е�����
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
        app();//Ӧ�ó������
    }*/
}

unsigned char ReadArry[WRITEPACK];//������3������ַ��3��ƫ�Ƶ�ַ��1�����ݳ��ȣ�224�����ݣ�1��У��ֵ
//unsigned char WriteArry[200];//д����
//******************************************************************
//*��������update
//*���ܣ�������³��򣬶��ⲿflash�ڵ����ݽ��д��У�飬ȷ���ڲ�����ǰ�ⲿflash�ڵ�������Ч
//*��������֡����У�������У�飬�д���ͼ�¼��flash�ڣ�����λ�˳�����״̬
//*FrameSum ֡����  SumCheck���У��ֵ��˫�ֽ��ۼӺ�
//*EraseFlag�ڲ�flash�Ƿ������YES�Ѳ�����NOδ��������δ�������������������״̬�ı�Ϊ����״̬�ȴ�������
//*         ���Խ���ԭ�г����������
//*         ���򣬲������������״ֱ̬�Ӹ�λ������ı�״̬֮����ܽ���ԭ�б���������ͻ��ܷ�
//*         �����޷�������һ�ε��Զ���������
//*********************************************************************
void update(unsigned int FrameSum,unsigned int SumCheck,unsigned char EraseFlag)@"UPDATECODE"
{
 /* unsigned int  tmpcheck = 0;//��֡У���м���
  unsigned int  Tcheck = 0;//���У���м���
  unsigned int  tmp;
  unsigned int  FReadTime;//֡��������ȫ����ȡ����    ,ReadTime
  unsigned char FReadFlag;//֡����־                  ,ReadFlag
  unsigned int  i;
  unsigned char UpState;
  unsigned long Address;
  unsigned char c = 0;
  
 // while((ReadFlag==0x44)&&(ReadTime<2))
  {
      Address = FLASH_UPDATE_START;//������ʼ��ַ����ÿ������һ֮֡������ۼ�
      for(i=0;i<FrameSum;i++)//���У��
      {
          c++;
          if(c>20)
          {
              c = 0;
              LIGHT_RUN_FLASH();
          }//ָʾ��״̬

          
          WTD_TOG;
          FReadFlag = NO;
          FReadTime = 0;
          while((FReadFlag==NO)&&(FReadTime<2))
          {
              UPSst26vf064b_Read(Address,ReadArry,WRITEPACK);
              tmp = ReadArry[(7+ReadArry[6])];//У��ֵ
              tmpcheck = singleFrameSumCheck((ReadArry+7),(ReadArry[6]),0);
              tmpcheck &= 0xff;
              if(tmpcheck == tmp)//��֡У����ȷ������֡���뵽���У����
              {
                  Tcheck = singleFrameSumCheck((ReadArry+7),(ReadArry[6]),Tcheck);//�ۼӺ�У��
                  FReadFlag = YES;//֡��ȡ�ɹ���־
              }
              else
              {
                  FReadTime++;
              }
          }
          if(FReadFlag == NO)
          {//�����޴���
            
            //�ڲ�flashδ������δ����������¸ı�״̬�ȴ�������
            //���Խ���ԭ�г���������У�����ԭ�г��򱻲�������ͻ��ܷ�
            //�����޷�������һ�ε��Զ���������
            if(EraseFlag == NO)
            {
              UpState = STATE_SCHECK_NO;//flash��֡��ȡУ�����
              WriteUpdate(FrameSum,SumCheck,UpState,EraseFlag);//д��״̬
            }         
              _DINT();
              unsigned int k;
              while(1)//���е�����
              {
                LIGHT_RUN_FLASH();
                for(k = 0; k < 3; k++)
                for(k = 0; k < 65530; k++);
              }
          }
          tmpcheck = 0;//�ۼӺ�У���ֵ
          Address += WRITEPACK;//��ʹ�ó˷�����
      }
      if(SumCheck == Tcheck)//���У��
      {
          UpState = STATE_ALLCHECK2_YES;//���У����ȷ
          EraseUpdataFlash();
          WriteUpdate(FrameSum,SumCheck,UpState,YES);  //д��У�飬������״̬��flash������־��YES        
          code_copy(FrameSum,SumCheck);//���Ʋ���
      }
      else
      {
          if(EraseFlag == NO)//�ڲ�flashδ����
          {
            UpState = STATE_ALLCHECK2_NO;//���У�����
            WriteUpdate(FrameSum,SumCheck,UpState,EraseFlag);//���д���ⲿflash
          }
            //Flash_EraseSegment((unsigned long *) INFLASH_COUNTER);
            //UPWriteFlashByte(INFLASH_STATE, 0X23);//д��flash���´���
            //abnormal2();//ָʾ��״̬
            //ResetCPU();//��λcpu
           _DINT();
           unsigned int k;
              while(1)//���е�����
              {
                LIGHT_RUN_FLASH();
                for(k = 0; k < 3; k++)
                for(k = 0; k < 65530; k++);
              }
      }
  }*/
}

unsigned int code_copy_crc(unsigned int length,unsigned char *data,unsigned int val)@"UPDATECODE"//length:���ȣ�*data������ָ�룬val:������м�У��ֵ
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
            if(AL & 0x01 == 1)      // AL���λΪ1
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
    crcvalue = (unsigned int)(AH<<8) + (AL&0x0ff);  //��λ��ǰ?

    return crcvalue;
}

unsigned int singleFrameCheck(unsigned char *data,unsigned int tmpcheck)@"UPDATECODE"//��ȡ��֡���ݣ���ʼ6�ֽڵ�ַ��1�ֽڳ��ȡ�200�ֽ����ݣ�2�ֽ�У����
{
    unsigned char len ;
    unsigned int  res;
    len = *(data+6);//��У������
    //for(i=0;i<len;i++)
    {
        res =  code_copy_crc(len,(data+7),tmpcheck); 
        //(unsigned int length,unsigned char *data,unsigned int val)
    }
    return res;
}
//====================================================================
//�ۼӺ�У�飬���ԭ��CRCУ��Ԥ�������ֽڿռ䣬����������Ԥ��δint��
//  *data ����  len ����   tmpcheck У���ʼֵ���ڶ�֡�����ۼ�У��������֡У��Ϊ0
//
//====================================================================
unsigned int singleFrameSumCheck(unsigned char *data,unsigned char len,unsigned int tmpcheck)@"UPDATECODE"
{
    //unsigned char len ;
   // unsigned int  res;
    unsigned char i;
    //len = *(data+6);//��У������
    
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
    volatile unsigned int * ptr;                    //addrΪ�öε��׵�ַ
    unsigned int j;
    
    _DINT();  
    WDTCTL = WDTPW + WDTHOLD;               ////��ֹ�ڲ����Ź�
    //FEED_WATCH_DOG();                             ////���ⲿ���Ź�
    WTD_TOG;
    for(j = 0; j < 1000; j++)
    {
      if((FCTL3 & BUSY) != BUSY)              //��æ������,��ɲ���
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
//function:����flash��ַ0x7c00-0x3ffff
//         ����������λ�����飨66��ʱ��
//         ԭ����������������д�룬�Է�
//         ֹ�������������⸴λ���޷���
//         ���������
//==============================================================================
void EraseUpdataFlash(void)@"UPDATECODE"
{
    unsigned long i = 0;
    unsigned long addr = AppStartAddr;
    unsigned char resval[2];//��λ����ֵ
    LIGHT_RUN_DOWN();
    resval[0] = *(unsigned char *)0xFFFE;//�ȱ��渴λ����ֵ
    resval[1] = *(unsigned char *)0xFFFF; 
    do
    { 
      Flash_EraseSegment(addr);
        addr += 512;
        i++;
        if(i == 66)//������λ�������ڿ鼴0xFFFE-0xFFFF
        {//����λ������������д�뵽��λ��λ��ַ��
            UPWriteFlashByte(0xFFFE, resval[0]);
            UPWriteFlashByte(0xFFFF, resval[1]);
        }
    }
    while(i<AppSegNo);
    LIGHT_RUN_UP();
}
//*************************************************************************
//*������:code_copy
//*���ܣ����ⲿflash�еĴ���θ��Ƶ��ڲ�flash�е�Ӧ����
//*�������ȶԶ�����У�飬У����ȷ�ٽ���д��д��֮���ٽ���У�飬����У�鲻�ɹ���ȴ���λ�����ı�flash�ڵı�־ֵ
//*���֮���ٸ��²�д��flash��־ֵ
//**************************************************************************
void code_copy(unsigned int FrameSum,unsigned int SumCheck)@"UPDATECODE"//framesum��֡����
{
    //unsigned long unAddr;
   /* unsigned long Address;//���Ե�ַ
    unsigned long Flash_Addr;
    unsigned long base_addr;
    unsigned long offset_addr;   
    unsigned char frame_no;//֡�����ݸ���
    unsigned int  check = 0;//���¼��㵥֡У��ֵ
    unsigned int  tmpcheck = 0;//����У��ֵ
    unsigned char check_flag,check_time;//У������־��У��������
    unsigned char write_flag,write_time;//д���־
    unsigned int i,j,k;
    unsigned int c = 0;
    unsigned char ledstate = 0;
    Flash_Addr = FLASH_UPDATE_START;
    for(i=0;i<FrameSum;i++)
    {
        
          c++;
          if(c>20)//�쳣ָʾ������
          {
              ledstate = ~ledstate;
              c = 0;
              LIGHT_RUN_FLASH();
          }//ָʾ��״̬

          
        //���ⲿflash��ȡ����  
        check_flag = NO;
        check_time = 0;
        while((check_flag == NO)&&(check_time < 2))//��ȡ��У������
        {
            //Sst26vf064b_Read((FLASH_UPDATE_START+i*WRITEPACK),ReadArry,WRITEPACK);
            WTD_TOG;            
            UPSst26vf064b_Read(Flash_Addr,ReadArry,WRITEPACK);
            
            base_addr = ReadArry[2];
            base_addr = (base_addr<<8)|ReadArry[1];
            base_addr = (base_addr<<8)|ReadArry[0];
            base_addr &= 0xFFFFFF;//����ַ
            offset_addr = ReadArry[5];
            offset_addr = (offset_addr<<8)|ReadArry[4];
            offset_addr = (offset_addr<<8)|ReadArry[3]; 
            offset_addr &= 0xFFFFFF;//ƫ�Ƶ�ַ
            Address = base_addr+offset_addr;
            frame_no = ReadArry[6];//֡���ֽ���
            tmpcheck = ReadArry[frame_no+7];//�ۼӺ�У���ռһλ
            //tmpcheck = (tmpcheck<<8)|ReadArry[207];//��ȡ
            check = singleFrameSumCheck((ReadArry+7),frame_no,0);//��ȡУ��
            check &= 0xff;//���ֽ�
            if(check == tmpcheck)
            {
                check_flag = YES;//��֡У����ȷ
            }
            else
            {
                check_flag = NO;//��֡У�����
                check_time++;
            }
        }
        if(check_flag == NO)//��ȡ����ֱ�Ӹ�λ���ı�flash�б�־���ݣ��������ٴν��д˲�����
        {
            _DINT();
            unsigned int k;
              while(1)//���е�����
              {
                LIGHT_RUN_FLASH();
                for(k = 0; k < 3; k++)
                for(k = 0; k < 65530; k++);
              }
        }
        
        //���ڲ�flashд������
        write_flag = NO;
        write_time = 0;
        while((write_flag == NO)&&(write_time<2))
        {
            WTD_TOG;            
          **********************
            //word wirte
            for(k=0;k<frame_no;k+=2)
                UPWriteFlashWord((Address+k), &ReadArry[7+k]);  
            
            *****��д��***********
            //ֻ��ʵ��long word д�룬�޷���֤�����һ֡������������4�ı���д���������
            //UPWriteFlashBytes(Address,&ReadArry[7],frame_no);
              
            for (j=0;j<frame_no;j++)//д�����֮��Ա�д�����Ƿ���ȷ
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
            }//д��У��
        }
        if(write_flag == NO)//д�����
        {
            _DINT();
            unsigned int k;
            while(1)//���е�����
            {
              LIGHT_RUN_FLASH();
              for(k = 0; k < 3; k++)
                for(k = 0; k < 65530; k++);
            }
        }
        Flash_Addr += WRITEPACK;//����ʹ�ó˷����㣬ֻ���ۼӵ�ַ
    }
    WTD_TOG;
    WriteUpdate(FrameSum,SumCheck,STATE_COMPLETE,NO);//�������
    //Flash_EraseSegment(UPCOUNTHead);
    UPSector_Erase(FLASH_UPCOUNTER);//��ʼ�������´�������
    _DINT();
    while(1)//���еƿ���
    {
      LIGHT_RUN_FLASH();
      for(k = 0; k < 65530; k++);
    }*/
}

//***************************************************************
//��������WriteUpdate
//���ܣ��ⲿflash��ڱ�־д�뺯��
//˵����FrameSum ,SumCheck,UpSate,EraseFlagһ���У��д�룬д��ǰ��Ҫ��������
//      EraseFlag:�ڲ�flash�Ƿ񱻲�����YES���Բ�����ԭ�г�����Ч��NO��δ����ԭ�г�����Ч��
//        �˱�־Ϊ�˷�ֹ�ڲ���ԭ�г�������ʱ��У������п�����ɵĸ�������״̬���Ѱ��
//        ԭ�г�����ڵ�����,���ڲ�flash��������YES����ɸ��»����������֮����NO
//***************************************************************
void WriteUpdate(unsigned int FrameSum,unsigned int SumCheck,unsigned char UpState,unsigned char EraseFlag)@"UPDATECODE"
{
    /*  unsigned char  SaveParameter[8];   
      unsigned int   tmpcheck;
      SaveParameter[0] = FrameSum&0x00FF;//��ڱ�����������
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
      UPSst26vf064b_Page_WriteBytes(FLASH_UPDATEBK_COUNTER,SaveParameter,8);//������*/
}

//***************************************************************
//*��������ResetFun
//*���ܣ�������������֮�󽫸�λ��ַ����д�뵽0xFFFE-0xFFFF��
//***************************************************************
void ResetFun(void)@"UPDATECODE"
{
    
}


//�ֽ�д���ڲ�flash
void UPWriteFlashByte(unsigned long dst, unsigned char data)@"UPDATECODE"
{
    unsigned int i;
    unsigned char *ptr;
    
    _DINT();
    WDTCTL = WDTPW + WDTHOLD;               ////��ֹ�ڲ����Ź�
    //FEED_WATCH_DOG();                             ////���ⲿ���Ź�
    ptr  = (unsigned char *)dst;
    for(i = 0; i < 1000; i++)
    {
      if((FCTL3 & BUSY) != BUSY)              //��æ������,��ɲ���
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

//��д��flash
void UPWriteFlashWord(unsigned long dst, unsigned char *data)@"UPDATECODE"
{
    unsigned int i;
    unsigned char *ptr;
    
    _DINT();
    WDTCTL = WDTPW + WDTHOLD;               ////��ֹ�ڲ����Ź�
    //FEED_WATCH_DOG();                             ////���ⲿ���Ź�
    ptr  = (unsigned char *)dst;
    for(i = 0; i < 1000; i++)
    {
      if((FCTL3 & BUSY) != BUSY)              //��æ������,��ɲ���
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
//д���ڲ�flash
//dst flash����ַ   *data:����ָ��   length������
//��д����Ҫ���ڴ�������
//=================================================
void UPWriteFlashBytes(unsigned long dst, unsigned char *data,unsigned char length)@"UPDATECODE"
{
    unsigned int i;
    unsigned char *ptr;
    
    _DINT();
    WDTCTL = WDTPW + WDTHOLD;               ////��ֹ�ڲ����Ź�
    //FEED_WATCH_DOG();                             ////���ⲿ���Ź�
    ptr  = (unsigned char *)dst;
    for(i = 0; i < 1000; i++)
    {
      if((FCTL3 & BUSY) != BUSY)              //��æ������,��ɲ���
      break;
    }
    FCTL3 = FWKEY;                            //  LOCK=0
    FCTL1 = FWKEY+WRT+BLKWRT;                        // BLKWRT= WRT=1 block write
    for(i = 0;i <length;i++)
    {
      *ptr++ = *data++;
      while(!(FCTL3 & WAIT));//�ȴ�WAITλΪ1�������һ���ݵ�д��
        
    }
    for(i = 0; i < 1000; i++)
    {
      if((FCTL3 & BUSY) != BUSY)              //��æ������,��ɲ���
      break;
    }	
    //FEED_WATCH_DOG();
    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
    _EINT();
}

//==============================================================================
//  ��������   : InitSPI
//  ��������   : SPI����һ�ֽ�����
//  �������   : ucData: ��д������
//  �������   ��<��>
//  ����ֵ     : UCB0TXBUF�Ĵ�������
//  ����˵��   : 
//  ����       ��
//==============================================================================
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


//==============================================================================
//  ��������   : UPUCB0SPIInit
//  ��������   : UPUCB0SPI���߳�ʼ��������
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void UPUCB0SPIInit(void)@"UPDATECODE"
{
    P3REN = 0xff;
    P3SEL |= 0x0e;                  // P3.1,2,3, option select
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
    UPSst_Reset();                                  //��λFlashоƬ
    UPSst_Unlock();                                 //����FlashоƬ
    
//    P3REN = 0xff;
//    P3SEL |= 0x0e;                  // P3.1,2,3, option select
//    P2SEL &= ~BIT0;
//    P2DIR |= BIT0;
//    P3DIR |= BIT7;
//    P7DIR |= BIT3;            //EEPROMд�������   
//    P7DIR |= BIT2;            //Flashд�������
//
//
//  
//    UCB0CTL1 |= UCSWRST;                          // **Put state machine in reset**   
//    UCB0CTL0 |= UCMST + UCSYNC + UCCKPL + UCMSB;  // 3-pin, 8-bit SPI master 
//                                                  // Clock polarity high, MSB
//    UCB0CTL1 |= UCSSEL_1;                         // ACLK + UCCKPH
//    UCB0BR0 = 0;                               // /4
//    UCB0BR1 = 0;                                  
//           
//    UCB0CTL1 &= ~UCSWRST;                         // **Initialize USCI state machine**
//    
//    //UPUNSELECT_CHIP;                                //ad45dbƬѡ��ʹ��
//    //CAT_UNSELECT_CHIP;                            //CAT25128Ƭѡ��ʹ��
//    //CAT_WP;                                       //CAT25128д����
//    //AT45DB_NOWP;                                    //AT45DB624д����
//    
// //   CAT_SpiConcelWP();                            //25128дʹ��
//    UPSst_Reset();                                  //��λFlashоƬ
//    UPSst_Unlock();                                 //����FlashоƬ
//    
}


#define UPSELECT_CHIP      P3OUT &= ~BIT7
#define UPUNSELECT_CHIP    P3OUT |= BIT7
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
//  ��������   : UPInitSPI
//  ��������   : SPI����һ�ֽ�����
//  �������   : ucData: ��д������
//  �������   ��<��>
//  ����ֵ     : UCB0TXBUF�Ĵ�������
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned char UPSPIWriteBuf(unsigned char ucData)@"UPDATECODE"
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
//  ��������   : UPInitSPI
//  ��������   : SPI����һ�ֽ�����
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : UCB0RXBUF�Ĵ�������
//  ����˵��   : 
//  ����       ��
//==============================================================================
unsigned char UPSPIReadBuf(void)@"UPDATECODE"
{
    unsigned char i;
    UPSPIWriteBuf(0);
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
//  ��������   : UPSpiWaitBusy
//  ��������   : ͨ����״̬�Ĵ������æ״̬
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   :
//  ����       ��
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
 //  ��������   : Sst26vf064b_Read
 //  ��������   : ������
 //  �������   : unAddr�����������Ե�ַ �� PcData��Ŀ�����ݴ洢ָ�� ��Size����ȡ���ݸ����
 //  �������   ��<��>
 //  ����ֵ     : 	<��>
 //  ����˵��   :
 //  ����       ��
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
 //  ��������   : Sst26vf064b_Page_Write
 //  ��������   : д����
 //  �������   : unAddr�����������Ե�ַ��  PcData��Ŀ�����ݴ洢ָ�룻 Size��д�����ݸ���(<=256��ע���ҳ)
 //  �������   ��<��>
 //  ����ֵ     : <��>
 //  ����˵��   :
 //  ����       ��
 //==============================================================================

 void UPSst26vf064b_Page_WriteBytes(long unAddr,unsigned char *PcData,unsigned int Size)@"UPDATECODE"
 {
     unsigned int i;
     UPSpiWaitBusy();

     UPSELECT_CHIP;
    // delay(1);
     UPSPIWriteBuf(UPWREN);							//����д
    // Flash_cs=1;
    // delay(1);
     UPUNSELECT_CHIP;

    // Flash_cs=0;
     UPSELECT_CHIP;
     UPSPIWriteBuf(UPSST_WRITE);						//дָ��
     UPSPIWriteBuf((unsigned char)(unAddr >> 16));	//��ַ
     UPSPIWriteBuf((unsigned char)(unAddr >> 8));
     UPSPIWriteBuf((unsigned char) unAddr);
     for(i = 0;i < Size;i++)					//����
     {
    	UPSPIWriteBuf(*PcData);
    	PcData++;
     }
     UPdelay(20);
     UPUNSELECT_CHIP;

     UPSELECT_CHIP;
  //   delay(1);
     UPSPIWriteBuf(UPWRDI);							//��ֹд
     //Flash_cs=1;
  //   delay(1);
     UPUNSELECT_CHIP;
 }

 //==============================================================================
 //  ��������   :Sector_Erase
 //  ��������   : ����� (4k Bytes)
 //  �������   : ���ַ����12λ��Ч
 //  �������   ��<��>
 //  ����ֵ     : <��>
 //  ����˵��   :
 //  ����       ��
 //==============================================================================
 void UPSector_Erase(long unBlock)@"UPDATECODE"
 {
    /* UPSpiWaitBusy();

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
     
     UPdelay(ERASE_DELAY);//���������֮����ʱһ��ʱ��*/
 }

//==============================================================================
//  ��������   : Sst_Reset
//  ��������   : ��λоƬ
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
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
 //  ��������   : Sst_Unlock
 //  ��������   : ����оƬ
 //  �������   : <��>
 //  �������   ��<��>
 //  ����ֵ     : <��>
 //  ����˵��   :
 //  ����       ��
 //==============================================================================
void UPSst_Unlock(void)@"UPDATECODE"
{
     UPSELECT_CHIP;
     UPSPIWriteBuf(UPWREN);
     UPUNSELECT_CHIP;
     UPSELECT_CHIP;
     UPSPIWriteBuf(UPULBPR);
     UPUNSELECT_CHIP;
     UPSELECT_CHIP;         //sst25�ϵ���Ҫ�ֶ�����鱣��״̬
     UPSPIWriteBuf(UPEWSR);
     UPUNSELECT_CHIP;
     UPSELECT_CHIP;
     UPSPIWriteBuf(UPWRSR);
     UPSPIWriteBuf(0x00);  //����鱣��
     UPUNSELECT_CHIP;
}
//==============================================================================
//  ��������   : UpInitClk
//  ��������   : ϵͳ�õ���ʱ�ӽ��г�ʼ��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void UpInitClk(void)@"UPDATECODE"
{ 

    UCSCTL3 = SELREF_2;                                        //FLL = REFCLK  32768kHz
    UCSCTL4 = SELS__DCOCLK + SELA__XT1CLK + SELM__DCOCLK;
    __bis_SR_register(SCG0);
    UCSCTL0 = 0;
    UCSCTL1 = DCORSEL_5;
    UCSCTL2 = FLLD_1 + 249;   
    P7SEL |= 0x03;                            // Select XT1
    UCSCTL6 &= (~XT1OFF);//8MHz
    UCSCTL6 |= XCAP_3; 
    __bic_SR_register(SCG0);   
   do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

  UCSCTL6 &= ~(XT1DRIVE_3);                 // Xtal is now stable, reduce drive
                                            // strength     
}
void UPPortInit(void)@"UPDATECODE"
{
  
      P1SEL = 0x00;   //1.0-1.7�ɼ�����ң�ţ�ȫ������Ϊ����
    P1DIR = 0x00;
    P2SEL = 0x00;   //2.0-2.7�ɼ�����ң�ţ�ȫ������Ϊ����
    P2DIR = 0x00;
    
    
    P3SEL = 0x3E;  //3.0- flash hold 3.1-SIMO 3.2-SOMI 3.3-CLK;3.4-TX0 3.5-RX0 3.6 CS1;3.7 CS0
    P3DIR = 0xDB;
    P3OUT = 0x01; //flash hold ��Ϊ�ߵ�ƽ
     
    P4SEL = 0x00;  //4.0-RF��Դ 4.1-RF��λ 4.2-RF����(in) 4.3-GPRS��Դ  4.4-4.5 DI���� 4.6-BAT��� 4.7-BAT�ŵ�
    P4DIR = 0xCB;
    P4OUT = 0x00;

    P5SEL = 0xC0;  //P56 TXD1  P57 RXD1 ��GPRSͨѶ  5.4 DI���� 5.5-��ػ���
    P5DIR = 0x6F;
    P5OUT = 0x00;
    
    P6SEL = 0x0F;  //P60-P63�ɼ�AD�� P60-BATFU-AD0 P61-SAM_C P62-SAM_SUN P63-SAM_BAT P64-��λGPRSģ��  P65-GPRSģ������  P66-G_ON/OFF ��/�ر� P67 ��ظ���ѹ����
    P6DIR = 0xFF;
    P6OUT = 0x00;
     
    P7SEL = 0x00; //7.2-flash_wp 7.3-eeprom_wp 7.5-WD 7.4-RUN LED
    P7DIR = 0xFF;
    P7OUT = 0x00;    

    P8SEL = 0x00; //8.0-8.2  8.4-8.5 ң������ P8.3 ʱ���ж� P8.6 Ӳң�ŵ�Դ״̬���  P8.7 �̵����������
    P8DIR = 0x80; 
    P8OUT = 0x80;    

    P9SEL = 0x30; //P94 TXD2  P95 RXD2 9.1-9.2 I2C 9.0 �̵���������� 9.3 �ܿ�24V��Դ���
    P9DIR = 0xDF;
    P9OUT = 0x01;    

    P10SEL = 0x30;  //P10.0-10.2 AD�������� P10.3 Ӳң�ŵ�Դ���� P10.4 TXD3  P10.5 RXD3 ���ÿ�ʹ��  
    P10DIR = 0xDF;
    P10OUT = 0x00; 
//    P5DIR |= BIT3;
//    P5OUT &= NBIT3;//���Ź�����
//    
//    //LED�˿�
//    P8SEL &= ~(BIT5+BIT6+BIT7);
//    P8DIR |= BIT5+BIT6+BIT7;
//    P8OUT = 0;
//    
//    P9DIR = BIT0;
//    P9OUT = 0;
//    
//    P11DIR = BIT2;
//    P11OUT = 0;
}


//ʹ��˵����
//*ָʾ��״̬˵����
//*1.��������״̬�£����е�����
//*2.�������ع����У����е����� 6min
//*3.�������֮��λ���г�����º����еƿ���
//***���У������쳣�������غϵ����� 1min
//***�ڲ�flash ����ʱ�쳣�Ⱥ��غϵƾ���
//***���븴�ƽ׶��쳣���������غϵ����� 1.5min
//*4.��ɸ��²�����֮�����е�����
//*5.�쳣״ָ̬ʾ�쳣�ƺ��غϵ�ͬ������
//ע�����
//1.ȷ�����´����������������ܣ��ܹ����г����ٴ�����
//2.ȷ�����´������ݽ�����0x7c00-0x3e200֮��Ĵ���
//3.��������֮ǰ��Ҫ����豸״̬
//������֮��ͨ���쳵�豸״̬�鿴�����������û����Ӧ��δ��ɻ�ʧ�ܣ�������Ӧ�򿴳���汾ʱ����£��������������������δ����

//����˵��
//�����ļ���ʹ��Ĭ�����ã������Զ����ļ�
//cstartup�ļ�Ҳ��Ҫ���չ��ܽ����޸�