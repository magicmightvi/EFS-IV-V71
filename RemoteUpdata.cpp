
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
union Item2    UpPackNo;//�������ݰ����
union Item2    UpInPackNo;//�����������ݰ����
union Item2    SumCheck;//���У����
union Item2    SFrameCounter;//�ӻ�֡����
unsigned char UpState;//����״̬
unsigned char SegNo;//�ܶ���
unsigned char SegBreakNo;//�ϵ�����
unsigned char WriteTime,WriteFlag;//д�������д���־0x55��ʧ�ܣ�0x66���ɹ�
unsigned char FrameTotal_No;//��֡��������
union   Item4 SegLength;//�ϵ�γ���
union   Item4 SegBaseAdd;//�ϵ�λ���ַ
union   Item4 SegOffsetAdd;//�ϵ��ƫ�Ƶ�ַ
union   Item4 Total_No;//�����ܳ�������Ч����

//��ȷ������֮������ٴ����㣬�ﵽ������Χ֮���ٴ����㣬ÿ����Ч֡�����ۼ�
unsigned char FrameCount = 0;//����֡���������ڴﵽ����֮����ж�EEPROM�ĸ���
unsigned int  AddSize = 0;//�ۼ�������
unsigned int  AddCheck = 0;//�ۼ�У��


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
//��Ӧ����flashָ��
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
//*��������Code_Load
//*���ܣ�����Ӧ��ASDU����������Ӧ
//*������Զ���������İ�����ʼ��д�룬У�飬ȷ�ϼ�������
//***********************************************************************************
void Code_Load(unsigned char *pRxBuf,unsigned char *pTXBuff)//����ԭ����Ϊ���ֽڴ���
{//1
    unsigned char *pTxBuf = pTXBuff;//���¶Է��ͻ���д������
    unsigned char pRXBuff[256] = {0};
    unsigned long address;
    unsigned int i,tmpcheck;
    unsigned char tmp,tmp1,tmp2;
    unsigned char ReadBack[256] = {0};//д��flash�����¶�ȡ����
    unsigned char *pCSPos;
    unsigned char write_c;//д���־0x55δд�룬0x44�ɹ�д��
    unsigned char write_T;//д��������
    unsigned char SaveParameter[8],SaveBack1[8],SaveBack2[8];
    unsigned long addr;
    unsigned int PageSize;//��ǰҳ��ʣ��ռ�
    unsigned char PackInfo[EEPUPDATE_STATE_NO]={0};//��������֡��֡״̬��Ϣ
    unsigned char checkright; //eeprom �д洢��ԭ������У���Ƿ���ȷYES����ȷ��������Ч��NO������������Чʹ�ñ���ȥ����
    unsigned char updatecount[2];
    unsigned char reason;
    static unsigned char chRcvEnd = 0;
    FEED_WATCH_DOG();

        memcpy(pRXBuff,pRxBuf,6+pRxBuf[1]);
        reason = pRXBuff[9];//����ԭ���ڵ�10���ֽ�
        tmp = 6;//linkaddr_num;
        *pTxBuf++ = 0x69;
        *pTxBuf++ = 0;
        *pTxBuf++ = 0;
        *pTxBuf++ = 0x69;
        pCSPos = pTxBuf;
        *pTxBuf++ = 83;//������:��վ->��վ ����ȷ��֡
        *pTxBuf++ = LOBYTE(g_gRunPara[RP_COMM_ADDR]);
        *pTxBuf++ = HIBYTE(g_gRunPara[RP_COMM_ADDR]); 
        *pTxBuf++ = 200;//֡����
        *pTxBuf++ = 0;//�޶���
    
    switch(reason)
    {//2
    case Reason_160://�Ӵ���ԭ��ʼ��ֵ
                    //���ص�ǰϵͳ�ĸ���״̬
      *pTxBuf++ = 160;//����ԭ��
      *pTxBuf++ = VerNum[V_OFFSET];//�汾��
      *pTxBuf++ = VerNum[V_OFFSET+1];
      *pTxBuf++ = VerNum[V_OFFSET+2];
      *pTxBuf++ = VerNum[V_OFFSET+3]; 
      *pTxBuf++ = VerNum[V_OFFSET+4];      
      //*pTxBuf++ = 0;//�汾�Ÿ�
      //*pTxBuf++ = 0;//�汾�ŵ�
      *pTxBuf++ = 0;//оƬ�Ÿ�
      *pTxBuf++ = 0;//оƬ�ŵ�    
      *pTxBuf++ = 0x00;//���������ÿռ���ֽ�400��200k����ʮ�����Ʊ�ʾʮ������
      *pTxBuf++ = 0x04;//���������ÿռ���ֽ�  segment��512B)     
      //*pTxBuf++ = ReadWord(EEPUPDATE_LAST_STATE, 0);//�ϴεĸ���״̬
      Sst26vf064b_Read(FLASH_UPDATE_COUNTER,SaveBack1,8);//�ϴθ���״̬�������ⲿflash��
      tmpcheck = SaveBack1[7];
      tmpcheck = (tmpcheck<<8)|SaveBack1[6];
      if(tmpcheck != update_crc(6,SaveBack1,0xFFFF))
      {
          Sst26vf064b_Read(FLASH_UPDATEBK_COUNTER,SaveBack1,8);//flash���ݶ�ȡ
          tmpcheck = SaveBack1[7];
          tmpcheck = (tmpcheck<<8)|SaveBack1[6];
          if(tmpcheck != update_crc(6,SaveBack1,0xFFFF))
          {
              
              *pTxBuf++ = STATE_ALLCHECK1_NO;//״ֵ̬У�鲻��ȷ��ΪУ�����
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
      
      CAT_SpiReadBytes(EEPUPDATE_LAST_VERSION,5,pTxBuf);//�ϴ������汾��
      pTxBuf += 5;
      *pTxBuf++ = CAT_SpiReadByte(EEPUPDATE_LAST_SEG);//������
      *pTxBuf++ = CAT_SpiReadByte(EEPUPDATE_LAST_SEGNo);//�����
      CAT_SpiReadBytes(EEPUPDATE_LAST_LENGTH,3,pTxBuf);//���ܳ�
      pTxBuf += 3;
      CAT_SpiReadBytes(EEPUPDATE_LAST_BASE,3,pTxBuf);//�λ���ַ
      pTxBuf += 3;
      CAT_SpiReadBytes(EEPUPDATE_LAST_OFFSET,3,pTxBuf);//��ƫ�Ƶ�ַ
      pTxBuf += 3;
      CAT_SpiReadBytes(EEPUPDATE_LAST_INCOUNTER,2,pTxBuf);//���ڰ�����
      pTxBuf += 2;
      CAT_SpiReadBytes(EEPUPDATE_LAST_COUNTER,2,pTxBuf);//������
      pTxBuf += 2;      
      for(i=0;i<5;i++)//Ԥ��
      {
          *pTxBuf++ = 0;
      }
      
      break;
    case Reason_163://����ȷ��
        *pTxBuf++ = 163;//����ԭ�� 
        FrameCount = 0;
        AddSize = 0;
        AddCheck = 0;        
        CAT_SpiWriteBytes(EEPUPDATE_LAST_VERSION,5,&pRXBuff[tmp+5]);
        if(pRXBuff[tmp+4] == 0xAA)//��ʼλ�ø���
        {

            address = FLASH_UPDATE_START;
            for(i=0;i<0x4;i++)//0x4 * 0x10000 = 0x40000
            {
                Block_Erase(address);        //ȷ�ϲ����в�����������Ӧ��
                address += 0x10000;  
                SpiWaitBusy();
            }
            //������
            //Sst26vf064b_Read(FLASH_UPDATE_START,ReadBack,WRITEPACK);
            //��ʼ������״̬
            //SumCheck.bit16 = 0xFFFF; //CRCУ���ʼ��
            SumCheck.bit16 = 0;//�ۼӺ�У���ʼ�� 
            CAT_SpiWriteBytes(EEPUPDATE_LAST_CHECK,2,SumCheck.bit8);//��ʼ��У��ֵ
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BKCHECK,2,SumCheck.bit8);//��ʼ��У��ֵ
            SFrameCounter.bit16 = 0;
            CAT_SpiWriteBytes(EEPUPDATE_LAST_COUNTER,2,SFrameCounter.bit8);//�ӻ�֡������ʼ��            
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BKCOUNTER,2,SFrameCounter.bit8);//�ӻ�֡������ʼ��            
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
            CAT_SpiWriteBytes(EEPUPDATE_LAST_OFFSET,3,SegOffsetAdd.bit8);//��������Ҫ�ӽ��ձ�������ȡ����10.15 16:29
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BKOFFSET,3,SegOffsetAdd.bit8);//��������Ҫ�ӽ��ձ�������ȡ����10.15 16:29
            Total_No.bit32 = 0;
            CAT_SpiWriteBytes(EEPUPDATE_LAST_TOTAL,4,Total_No.bit8);
            CAT_SpiWriteBytes(EEPUPDATE_LAST_BKTOTAL,4,Total_No.bit8);
            Sector_Erase(FLASH_UPDATE_COUNTER);//������ڱ�־����
            *pTxBuf++ = 0x66;//ȷ��������0x55Ϊ��������         
        }
        else if(pRXBuff[tmp+4] == 0xBB)//�ϵ�λ�ø���
        {
            //address = FLASH_UPDATE_START;
            CAT_SpiReadBytes(EEPUPDATE_LAST_COUNTER,2,SFrameCounter.bit8);//������
            *pTxBuf++ = 0x66;//ȷ��������0x55Ϊ��������         
        
        }
        else
        {
            *pTxBuf++ = 0x55;//0x55Ϊ�������� 
        }

        for(i=0;i<6;i++)
            *pTxBuf++ = 0; 
            
      break;
    case Reason_164://У��֡
          tmpcheck = pRXBuff[tmp+5];
          tmpcheck = (tmpcheck<<8)|pRXBuff[tmp+4];
          CAT_SpiReadBytes(EEPUPDATE_LAST_CHECK,2,SumCheck.bit8);          //���У��Ա�
          CAT_SpiReadBytes(EEPUPDATE_LAST_COUNTER,2,UpPackNo.bit8);//������
          *pTxBuf++ = 164;
          *pTxBuf++ = SumCheck.bit8[0];
          *pTxBuf++ = SumCheck.bit8[1];//����У��
          *pTxBuf++ = pRXBuff[tmp+6];
          *pTxBuf++ = pRXBuff[tmp+7];
          *pTxBuf++ = pRXBuff[tmp+8]; //�����С��ֱ��ʹ����λ������  
          if(tmpcheck == SumCheck.bit16)
          {
              //*pTxBuf++ = 0x32;//У�����Ƿ���ͬ�ŵ�����д��������֮��ȷ��
              //UpState = 0x67;
              UpState = STATE_ALLCHECK1_YES;
              //CAT_SpiWriteByte(EPPUPDATE_LAST_STATE,1,0, &UpState);
          }
          else
          {
              //*pTxBuf++ = 0x45;//У������ͬ
              //UpState = 0x45;
              UpState = STATE_ALLCHECK1_NO;
              //CAT_SpiWriteByte(EPPUPDATE_LAST_STATE,1,0, &UpState);
          }
          
          SaveParameter[0] = UpPackNo.bit8[0];//��ڱ�����������
          SaveParameter[1] = UpPackNo.bit8[1];
          SaveParameter[2] = SumCheck.bit8[0];
          SaveParameter[3] = SumCheck.bit8[1];
          SaveParameter[4] = UpState;
          SaveParameter[5] = NO;//����У����ɣ����ڲ�flash������־ ��NO
          tmpcheck = update_crc(6,SaveParameter,0xFFFF);
          SaveParameter[6] = tmpcheck&0x00ff;
          SaveParameter[7] = (tmpcheck>>8)&0x00ff;   
          
          if(UpState == STATE_ALLCHECK1_YES)  //���±�־
          {
              //SavePerameter[8] = {0};  

              write_c = NO;
              write_T = 0;
              while((write_c == NO)&&(write_T < 6))//flash����жϱ�־����д��ȷ��
              {
                      Sector_Erase(FLASH_UPDATE_COUNTER);//������д��֮��ʱ��̫���޷�����д��SPI��Ƶ����Ϊ0ʱ����
                      //delay(ERASE_DELAY);
                      Sst26vf064b_Page_WriteBytes(FLASH_UPDATE_COUNTER,SaveParameter,8);
                      Sst26vf064b_Page_WriteBytes(FLASH_UPDATEBK_COUNTER,SaveParameter,8); 
                      Sst26vf064b_Read(FLASH_UPDATE_COUNTER,SaveBack1,8);//���д���Ƿ���ȷ
                      Sst26vf064b_Read(FLASH_UPDATEBK_COUNTER,SaveBack2,8);
                      for(i=0;i<8;i++)
                      {
                        if((SaveParameter[i] == SaveBack1[i])&&(SaveParameter[i] == SaveBack2[i]))
                          {
                              if(i==7)//���һ�������ж����
                                write_c = YES;//д��ɹ�
                          }
                          else
                          {
                              write_T++;//д������ۼ�
                              break;
                          }
                      }
               }
              if(write_c == NO)//��־д�������뵽У�����
              {
                  UpState = STATE_ALLCHECK1_NO;
                  SaveParameter[4] = UpState;//��ڱ�־д���������Ϊ����У�������ı伴���洢��д���־ֵ
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
                  *pTxBuf++ = YES;//д��ɹ�
              }              
          }
          else//У��ʧ������д��ڱ�־
          {
                  //UpState = STATE_ALLCHECK1_NO;
                  SaveParameter[4] = UpState;//ʧ����Ϣ����ȷ���Ƿ�д��ɹ�
                  Sector_Erase(FLASH_UPDATE_COUNTER);
                  //delay(ERASE_DELAY);
                  Sst26vf064b_Page_WriteBytes(FLASH_UPDATE_COUNTER,SaveParameter,8);
                  Sst26vf064b_Page_WriteBytes(FLASH_UPDATEBK_COUNTER,SaveParameter,8); 
                 // *pTxBuf++ = 0x45;
                  *pTxBuf++ = NO;              
          }
         // CAT_SpiWriteBytes(EEPUPDATE_LAST_STATE,1,&UpState);
          for(i=0;i<4;i++)//����ֵ
          {
              *pTxBuf++ = 0;
          }
          if(UpState == STATE_ALLCHECK1_YES)
          {//�ɹ��������Ӧ�ҵȴ���λ
              pTXBuff[1] = pTxBuf - pCSPos;//LL
              pTXBuff[2] = pTxBuf - pCSPos;//LL
              *pTxBuf++ = AddByte(&(pTXBuff[4]),pTXBuff[1]);
              *pTxBuf++ = 0x16;
              chRcvEnd = YES;
              CommSendData(pTXBuff,pTXBuff[1]+6,g_Cmid);
              //Send_101(unTxlength,4,COMM_INDEX_GPRS);
             // DataFlash_EraseSegment((unsigned long *)ERASEFLAGADDR);//���ò�����jtag��д�Ѿ�����������ʹ��������Ҳû�б�Ҫ����
              for(i=0;i<6;i++)
              {
                //ucTestRam[i]=11;//�ı�������־��ʹ������������״̬��Ϊ�ǵ�������
	        WriteFlashByteR((ERASEFLAGADDR+i),0x55);//��Ҫ����ʱ��jtag������־��λ�����ж��䲻��jtag��������
              }
              
              //�����������֮��flash�������������ʼ�������ٶ�flash�Ĳ����������ͼ��ٿ�����ʱ
              Sst26vf064b_Read(FLASH_UPCOUNTER,updatecount,2);//
              if(updatecount[0] != 0xFF)//��Ч���ҷ�0xFF�������flash
              {
                  Sector_Erase(FLASH_UPCOUNTER);
              }
                
//              delayms(500);
//              _DINT();
//              unsigned int k;
//              while(1)//���еƿ���
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
        {//�ȴ�����
              chRcvEnd = NO;
              delayms(500);
              _DINT();
              unsigned int k;
              while(1)//���еƿ���
              {
                LIGHT_RUN_FLASH();
                for(k = 0; k < 65530; k++);
              }
        }
      break;
    case Reason_166:  //���ص�ǰ����汾
                      
        *pTxBuf++ = 166;//����ԭ��
        *pTxBuf++ = 0;//�汾�Ÿ�
        *pTxBuf++ = 0;//�汾�ŵ�
      break;  
    case Reason_170://���ݴ���
        //UpState = 0xAA;
        *pTxBuf++ = 170;//����ԭ��
        //unsigned char FrameCount;//����֡���������ڴﵽ����֮����ж�EEPROM�ĸ���

        UpPackNo.bit16 = pRXBuff[tmp+5];
        UpPackNo.bit16 = UpPackNo.bit16<<8;
        UpPackNo.bit16 += pRXBuff[tmp+4];//�����
        CAT_SpiReadBytes(EEPUPDATE_LAST_SEG,EEPUPDATE_STATE_NO,PackInfo);//��ȡ�����м���
        tmpcheck = PackInfo[EEPUPDATE_STATE_CRCH];
        tmpcheck <<= 8;
        tmpcheck |= PackInfo[EEPUPDATE_STATE_CRCL];
        if(tmpcheck == update_crc((EEPUPDATE_STATE_NO-2),PackInfo,0xFFFF))
        { 
            //CAT_SpiReadBytes(EEPUPDATE_LAST_COUNTER,2,SFrameCounter.bit8);//������
            checkright = YES;
        }
        else
        {
            CAT_SpiReadBytes(EEPUPDATE_LAST_BKSEG,EEPUPDATE_STATE_NO,PackInfo);//������������
            checkright = NO;
        }//��ͬʱ��Ч����ǰ4֡������ΪEEPROM������
        SFrameCounter.bit8[0] = PackInfo[EEPUPDATE_STATE_COUNTER1];
        SFrameCounter.bit8[1] = PackInfo[EEPUPDATE_STATE_COUNTER2];//������
        Total_No.bit8[0] = PackInfo[EEPUPDATE_STATE_TOTAL1];
        Total_No.bit8[1] = PackInfo[EEPUPDATE_STATE_TOTAL2];
        Total_No.bit8[2] = PackInfo[EEPUPDATE_STATE_TOTAL3];
        Total_No.bit8[3] = PackInfo[EEPUPDATE_STATE_TOTAL4];//��������
        
        SumCheck.bit8[0] = PackInfo[EEPUPDATE_STATE_CHECK1];
        SumCheck.bit8[1] = PackInfo[EEPUPDATE_STATE_CHECK2];//���У��ֵ
        if((SFrameCounter.bit16+(FrameCount&0x03)) == (UpPackNo.bit16-1))//���ܰ����Ǳȵ�ǰ�ѽ��ܵİ�������1��ʹ��EEPROM�е����ݼ��ϵ�ǰ�ۻ�֡��
        {
          
            FrameCount++;//�յ�һ��Ч֡�ۼ�һ��
            
            SegNo = pRXBuff[tmp+8]; //������

            
            UpInPackNo.bit16 = pRXBuff[tmp+10];
            UpInPackNo.bit16 = (UpInPackNo.bit16<<8)|pRXBuff[tmp+9];//���ڰ����
            

            
            SegBreakNo = pRXBuff[tmp+11]; //�����
 
            
            
            FrameTotal_No = pRXBuff[tmp+21];//��֡��������
            
            
            WriteFlag = NO;//
            WriteTime = 0;
            addr = UpPackNo.bit16-1;
            addr *= WRITEPACK;
            addr += FLASH_UPDATE_START;//�ܵ�ַ 
            while((WriteFlag == NO)&&(WriteTime <= 2))//д��У��
            {

                PageSize = 0x100 - (addr%0x100);//��ǰҳ��ʣ��ռ�
                if(PageSize < WRITEPACK)//��ǰҳ��ʣ��ռ�С��Ҫд��������
                {
                    Sst26vf064b_Page_WriteBytes(addr,&pRXBuff[tmp+15],PageSize);//ÿ�������ȶ���Ϊ��ͬ��δ�ﵽ���ȵİ���ʹ�ð����ȣ��ڶ�ȡʱ������Ч����                    
                    Sst26vf064b_Page_WriteBytes(((addr&0xffffff00) + 0x100),&pRXBuff[tmp+15+PageSize],(WRITEPACK-PageSize)); //��һҳ����ʼ��ַ               
                    
                }
                else
                {
                    Sst26vf064b_Page_WriteBytes(addr,&pRXBuff[tmp+15],WRITEPACK);//ÿ�������ȶ���Ϊ��ͬ��δ�ﵽ���ȵİ���ʹ�ð����ȣ��ڶ�ȡʱ������Ч����
                }
                Sst26vf064b_Read(addr,ReadBack,WRITEPACK);//��ȡ��д������
                BYTE bCheckFrmLen = 8 + FrameTotal_No;//����ַ��3��+ƫ�Ƶ�ַ��3��+��������1��+�������ݣ�224��+�ۼӺͣ�1��
               // for(i=0;i<WRITEPACK;i++)
                for(i=0;i<bCheckFrmLen;i++)//�ж�д��Ͷ��������Ƿ���ͬʱֻ���ж����õ����ݣ����õ����ݲ����ж�(����101���ĵ�У��)
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
                          WriteFlag = YES;//д��ɹ�
                  }
                }
            }
            if(WriteFlag == YES)//д����ȷ�򱣴�
            {
                //*pTxBuf++ = 0x66;
                //UpState = 0xAA;
                UpState = STATE_LOADING;
         //unsigned int  AddSize;//�ۼ�������
        //unsigned int  AddCheck;//�ۼ�У��
                AddSize += FrameTotal_No;//�ۼ�������
                AddCheck += UpdateSumcheck(&pRXBuff[tmp+22],FrameTotal_No);//�ۼ�У��
                
                if(((FrameCount & 0x03) == 0)||((SegBreakNo==SegNo-1)&&(UpInPackNo.bit16==0xFFFF))||(UpPackNo.bit16 == 0x01))//����ÿ4�ν���һ��eeprom���ݸ��»����ڳ����������һ�ε����һ�����ݻ��ߵ�һ������
                {
                    
                      FrameCount = 0;
                      PackInfo[EEPUPDATE_STATE_SEG] =pRXBuff[tmp+8]; //������  
                      
                      PackInfo[EEPUPDATE_STATE_INCOUNTER1] = pRXBuff[tmp+9];//���ڰ����
                      PackInfo[EEPUPDATE_STATE_INCOUNTER2] = pRXBuff[tmp+10];
                      
                      PackInfo[EEPUPDATE_STATE_SEGNo] =pRXBuff[tmp+11]; //�����
                      
                      PackInfo[EEPUPDATE_STATE_COUNTER1] = pRXBuff[tmp+4];//�����
                      PackInfo[EEPUPDATE_STATE_COUNTER2] = pRXBuff[tmp+5];
            
                      SegLength.bit32 = pRXBuff[tmp+14];//���ܳ�
                      SegLength.bit32 = (SegLength.bit32<<8)|pRXBuff[tmp+13];
                      SegLength.bit32 = (SegLength.bit32<<8)|pRXBuff[tmp+12]; 
                      SegLength.bit32 &= 0xFFFFFF;            
                      PackInfo[EEPUPDATE_STATE_LENGTH1] = pRXBuff[tmp+12];
                      PackInfo[EEPUPDATE_STATE_LENGTH2] = pRXBuff[tmp+13];
                      PackInfo[EEPUPDATE_STATE_LENGTH3] = pRXBuff[tmp+14];
                      PackInfo[EEPUPDATE_STATE_LENGTH4] = 0;
        
                      SegBaseAdd.bit32 = pRXBuff[tmp+17];//����ַ
                      SegBaseAdd.bit32 = (SegBaseAdd.bit32<<8)|pRXBuff[tmp+16];
                      SegBaseAdd.bit32 = (SegBaseAdd.bit32<<8)|pRXBuff[tmp+15];
                      SegBaseAdd.bit32 &= 0xFFFFFF;
                      PackInfo[EEPUPDATE_STATE_BASE1] = pRXBuff[tmp+15];
                      PackInfo[EEPUPDATE_STATE_BASE2] = pRXBuff[tmp+16];
                      PackInfo[EEPUPDATE_STATE_BASE3] = pRXBuff[tmp+17];
                      PackInfo[EEPUPDATE_STATE_BASE4] = 0;
            
                      SegOffsetAdd.bit32 = pRXBuff[tmp+20];//ƫ�Ƶ�ַ
                      SegOffsetAdd.bit32 = (SegOffsetAdd.bit32<<8)|pRXBuff[tmp+19];
                      SegOffsetAdd.bit32 = (SegOffsetAdd.bit32<<8)|pRXBuff[tmp+18];        
                      SegOffsetAdd.bit32 &= 0xFFFFFF;
                      PackInfo[EEPUPDATE_STATE_OFFSET1] = pRXBuff[tmp+18];
                      PackInfo[EEPUPDATE_STATE_OFFSET2] = pRXBuff[tmp+19];
                      PackInfo[EEPUPDATE_STATE_OFFSET3] = pRXBuff[tmp+20];
                      PackInfo[EEPUPDATE_STATE_OFFSET4] = 0;

                    //if(checkright == YES)//ԭʼ������Ч��ʹ�ã�����ʹ�ñ���ȥ����
                   // {
                   //   CAT_SpiReadBytes(EEPUPDATE_LAST_TOTAL,4,Total_No.bit8);
                   //   CAT_SpiReadBytes(EEPUPDATE_LAST_CHECK,2,SumCheck.bit8);//��ȡ�ϴα���У����
                    //}
                    //else
                   // {
 
                    //  CAT_SpiReadBytes(EEPUPDATE_LAST_BKTOTAL,4,Total_No.bit8);
                   //   CAT_SpiReadBytes(EEPUPDATE_LAST_BKCHECK,2,SumCheck.bit8);//��ȡ�ϴα���У����
                   // }
                    Total_No.bit32 += AddSize;
                    PackInfo[EEPUPDATE_STATE_TOTAL1] = Total_No.bit8[0];
                    PackInfo[EEPUPDATE_STATE_TOTAL2] = Total_No.bit8[1];
                    PackInfo[EEPUPDATE_STATE_TOTAL3] = Total_No.bit8[2];
                    PackInfo[EEPUPDATE_STATE_TOTAL4] = Total_No.bit8[3];
                    /*CAT_SpiWriteBytes(EEPUPDATE_LAST_TOTAL,4,Total_No.bit8);*/
                
                    //SumCheck.bit16 = update_crc((unsigned int)FrameTotal_No,&pRXBuff[tmp+22],SumCheck.bit16);//�洢У����
                    SumCheck.bit16 += AddCheck;//����У���������±���
                    SumCheck.bit16 &= 0xffff;//��ʱ����ԭ����16λУ��ռ�
                    PackInfo[EEPUPDATE_STATE_CHECK1] = SumCheck.bit8[0];
                    PackInfo[EEPUPDATE_STATE_CHECK2] = SumCheck.bit8[1];
                    AddSize = 0;
                    AddCheck = 0;
                /*CAT_SpiWriteBytes(EEPUPDATE_LAST_CHECK,2,SumCheck.bit8);*/
                
                //CAT_SpiWriteBytes(EEPUPDATE_LAST_COUNTER,2,UpPackNo.bit8);//�����
                //CAT_SpiWriteBytes(EEPUPDATE_LAST_INCOUNTER,2,UpInPackNo.bit8);//���ڰ����
                //CAT_SpiWriteByte(EPPUPDATE_UPDATE_IN_INCOUNTER,1,0, &UpPackNo); 
                //if(SegBreakNo != CAT_SpiReadByte(EEPUPDATE_LAST_SEGNo))//���������жϣ�ֻ�е�����ű仯֮��ŶԶ���š��ܶ��������ܳ����λ���ַ���и����Խ�ʡʱ��
                //{
                  //CAT_SpiWriteBytes(EEPUPDATE_LAST_SEG,1,&SegNo);
                  //CAT_SpiWriteBytes(EEPUPDATE_LAST_SEGNo,1,&SegBreakNo);
                  //CAT_SpiWriteBytes(EEPUPDATE_LAST_LENGTH,3,SegLength.bit8);
                  //CAT_SpiWriteBytes(EEPUPDATE_LAST_BASE,3,SegBaseAdd.bit8);
                //}
                //CAT_SpiWriteBytes(EEPUPDATE_LAST_OFFSET,3,SegOffsetAdd.bit8);//��������Ҫ�ӽ��ձ�������ȡ����10.15 16:29
                    tmpcheck = update_crc((EEPUPDATE_STATE_NO-2),PackInfo,0xFFFF);
                    PackInfo[EEPUPDATE_STATE_CRCL] = tmpcheck&0x00ff;
                    PackInfo[EEPUPDATE_STATE_CRCH] = (tmpcheck>>8)&0x00ff;
                    CAT_SpiWriteBytes(EEPUPDATE_LAST_SEG,EEPUPDATE_STATE_NO,PackInfo);
                    CAT_SpiWriteBytes(EEPUPDATE_LAST_BKSEG,EEPUPDATE_STATE_NO,PackInfo);
                //CAT_SpiReadBytes(EEPUPDATE_LAST_SEG,EEPUPDATE_STATE_NO,PackInfo);
                //���¶�ȡ���鿴�Ƿ�д����ȷ
                //CAT_SpiReadBytes(EEPUPDATE_LAST_SEG,1,&SegNo);
                //CAT_SpiReadBytes(EEPUPDATE_LAST_CHECK,2,SumCheck.bit8);//��ȡ�ϴα���У����
                //CAT_SpiReadBytes(EEPUPDATE_LAST_TOTAL,4,Total_No.bit8);
                }
                
            }
            else
            {
                WriteFlag = NO;//*pTxBuf++ = 0x55;//д�����
                UpState = STATE_FLASHWRITE_NO;
            }
            //CAT_SpiReadBytes(EEPUPDATE_LAST_STATE,1,&tmp2);
            //if(tmp2 != UpState)
                //CAT_SpiWriteBytes(EEPUPDATE_LAST_STATE,1,&UpState);//����EEPROM��״̬���ı��򱣴�
            *pTxBuf++ = UpPackNo.bit16&0xff;
            *pTxBuf++ = (UpPackNo.bit16>>8)&0xff;//֡�������� 
            for(i=6;i<22;i++)//��Ӧ�ͽ��յ���Ӧλ������ͬ
            {
              *pTxBuf++ = *(pRxBuf+tmp+i);
              //pRxBuf++;
            }
        }
        else//֡��������Ӧ�����شӻ�����֡����
        {
            WriteFlag = 0x77;
            FrameCount = 0;//��Ƭ�����ؽ��նϵ�ʱ��Ҫ��ԭ�еĿ��ܵ���ֵ���㣬�Ա������֡��������Ӧ
            AddSize = 0; //�ۼ�����������
            AddCheck = 0; //�ۼ�У������
            UpState = STATE_LOADING;
            *pTxBuf++ = SFrameCounter.bit8[0]&0xff;
            *pTxBuf++ = SFrameCounter.bit8[1]&0xff;//֡��������
            *pTxBuf++ = *(pRxBuf+tmp+6);//�汾��
            *pTxBuf++ = *(pRxBuf+tmp+7);//�汾��
            *pTxBuf++ = *(pRxBuf+tmp+8);//������


            //�ж������ڴ洢�������Ƿ���Ч����Ч���ȡ������ʹ�ñ���������
            if(checkright == YES)
            {
                //CAT_SpiReadBytes(EEPUPDATE_LAST_INCOUNTER,2,UpInPackNo.bit8);//���ڰ�����
                //*pTxBuf++ = UpInPackNo.bit8[0];
                //*pTxBuf++ = UpInPackNo.bit8[1];            
                CAT_SpiReadBytes(EEPUPDATE_LAST_INCOUNTER,2,pTxBuf);
                pTxBuf += 2;

                //CAT_SpiReadBytes(EEPUPDATE_LAST_SEGNo,1,&SegBreakNo);//�����
                //*pTxBuf++ = SegBreakNo;
                CAT_SpiReadBytes(EEPUPDATE_LAST_SEGNo,1,pTxBuf);
                pTxBuf += 1;
            
                //CAT_SpiReadBytes(EEPUPDATE_LAST_LENGTH,3,SegLength.bit8);//���ܳ�
                //*pTxBuf++ = SegLength.bit8[0];
                //*pTxBuf++ = SegLength.bit8[1];
                //*pTxBuf++ = SegLength.bit8[2];
                CAT_SpiReadBytes(EEPUPDATE_LAST_LENGTH,3,pTxBuf);
                pTxBuf += 3;
            
                //CAT_SpiReadBytes(EEPUPDATE_LAST_BASE,3,SegBaseAdd.bit8);//�λ�ַ
                //*pTxBuf++ = SegBaseAdd.bit8[0];
                //*pTxBuf++ = SegBaseAdd.bit8[1];
                //*pTxBuf++ = SegBaseAdd.bit8[2];
                CAT_SpiReadBytes(EEPUPDATE_LAST_BASE,3,pTxBuf);
                pTxBuf += 3;    
            
                //CAT_SpiReadBytes(EEPUPDATE_LAST_OFFSET,3,SegOffsetAdd.bit8);//��ƫ�Ƶ�ַ
                //*pTxBuf++ = SegOffsetAdd.bit8[0];
                //*pTxBuf++ = SegOffsetAdd.bit8[1];
                //*pTxBuf++ = SegOffsetAdd.bit8[2];
                CAT_SpiReadBytes(EEPUPDATE_LAST_OFFSET,3,pTxBuf);
                pTxBuf += 3; 
            }
            else
            {
                //CAT_SpiReadBytes(EEPUPDATE_LAST_INCOUNTER,2,UpInPackNo.bit8);//���ڰ�����
                //*pTxBuf++ = UpInPackNo.bit8[0];
                //*pTxBuf++ = UpInPackNo.bit8[1];            
                CAT_SpiReadBytes(EEPUPDATE_LAST_BKINCOUNTER,2,pTxBuf);
                pTxBuf += 2;

                //CAT_SpiReadBytes(EEPUPDATE_LAST_SEGNo,1,&SegBreakNo);//�����
                //*pTxBuf++ = SegBreakNo;
                CAT_SpiReadBytes(EEPUPDATE_LAST_BKSEGNo,1,pTxBuf);
                pTxBuf += 1;
            
                //CAT_SpiReadBytes(EEPUPDATE_LAST_LENGTH,3,SegLength.bit8);//���ܳ�
                //*pTxBuf++ = SegLength.bit8[0];
                //*pTxBuf++ = SegLength.bit8[1];
                //*pTxBuf++ = SegLength.bit8[2];
                CAT_SpiReadBytes(EEPUPDATE_LAST_BKLENGTH,3,pTxBuf);
                pTxBuf += 3;
            
                //CAT_SpiReadBytes(EEPUPDATE_LAST_BASE,3,SegBaseAdd.bit8);//�λ�ַ
                //*pTxBuf++ = SegBaseAdd.bit8[0];
                //*pTxBuf++ = SegBaseAdd.bit8[1];
                //*pTxBuf++ = SegBaseAdd.bit8[2];
                CAT_SpiReadBytes(EEPUPDATE_LAST_BKBASE,3,pTxBuf);
                pTxBuf += 3;    
            
                //CAT_SpiReadBytes(EEPUPDATE_LAST_OFFSET,3,SegOffsetAdd.bit8);//��ƫ�Ƶ�ַ
                //*pTxBuf++ = SegOffsetAdd.bit8[0];
                //*pTxBuf++ = SegOffsetAdd.bit8[1];
                //*pTxBuf++ = SegOffsetAdd.bit8[2];
                CAT_SpiReadBytes(EEPUPDATE_LAST_BKOFFSET,3,pTxBuf);
                pTxBuf += 3; 
            }              
            
            *pTxBuf++ = pRXBuff[tmp+21];//������������û��
        }
        
        Sst26vf064b_Read(FLASH_UPDATE_STATE,&tmp2,1);
        Sst26vf064b_Read(FLASH_UPDATEBK_STATE,&tmp1,1);//��ȡ����״̬
        if((UpState != tmp2)&&(UpState != tmp1))//״̬��ͬ�����±��� �������жϷ�ֹ��д������У�ÿдһ�ξ͸���һ��
        {
              SaveParameter[0] = UpPackNo.bit8[0];//��ڱ�����������
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
        *pTxBuf++ = WriteFlag;//flash����д�����
            
      break;       
    }//end 2

    pTXBuff[1] = pTxBuf - pCSPos;//LL
    pTXBuff[2] = pTxBuf - pCSPos;//LL
    *pTxBuf++ = AddByte(&(pTXBuff[4]),pTXBuff[1]);
    *pTxBuf++ = 0x16;
    CommSendData(pTXBuff,pTXBuff[1]+6,g_Cmid);
    //return pTxBuf;
}//end 1



//д���ڲ�Flash��dst��ַ��data����,�����ݸ�д���ⲿflash����
void WriteFlashByteR(unsigned long dst,unsigned int data)
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

char DataFlash_EraseSegment(unsigned long *pAddr)
{
    volatile unsigned int * ptr;                    //addrΪ�öε��׵�ַ
    unsigned int j;
    
    _DINT();  
    WDTCTL = WDTPW + WDTHOLD;               ////��ֹ�ڲ����Ź�
    //FEED_WATCH_DOG();                             ////���ⲿ���Ź�
    
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
   // return OK;
    return 1;
}

unsigned int update_crc(unsigned int length,unsigned char *data,unsigned int val)//length:���ȣ�*data������ָ�룬val:������м�У��ֵ
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
//�ۼӺ�У��
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