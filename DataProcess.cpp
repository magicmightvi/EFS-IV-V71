/*------------------------------------------------------------------------
 Module:      DataProcess.cpp
 Author:      linxueqin
 Project:     ��վƽ̨
 State:       V1.0
 Creation Date:   2014-4-29
 Description:   AD������ʼ����������վ�ɼ���ѹ�������ͱ���SOE����֯ң��ң�ŷ��ͻ�����
------------------------------------------------------------------------*/


#ifndef _DATA_PROCESS

    #define _DATA_PROCESS
    #include "DataStruct.h"
    #include "sqrttable.h"
#include "Main.h"
#endif

//==============================================================================
//  ��������   : InitDataProc
//  ��������   : ���ݴ����ʼ��
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       : 
//==============================================================================
/*void InitDataProc(void)
{
    unsigned int i,j;
    for(i = 0; i < AC_AD_CHAN_NUM;i++)
      for(j = 0; j < AC_BUFF_LEN;j++)
        g_sSampleData[i][j] = 0;
    if(g_gRunPara[Terminal_Type] == AERIAL_TERMINAL)
    {//�ܿ��ն˵�ͨ��״̬�жϲ�ʹ��g_sCommStatCount�������
      for(i = 0; i < RMT_ZSQST_NUM; i++)
        g_sCommStatCount[i] = g_gRunPara[RP_ZSQCST_T];
    }
    g_sSampleFinish = OFF;
    
    //soe������ʼ��
    g_unSSoeSavePtr = 0;
    g_unDSoeSavePtr = 0;
    soe_ptr = 0;
    soe_area = 0;
    soe_num = 0;
    g_AdChgSdYc = 0;
    g_FltChgSdYc = 0;
    g_PtLoseFlag = 0;
    g_PtMonitorCnt = 0;
  //g_PtLoseCnt=0;
    if(g_gRunPara[RP_DEVICE_TYPE])
        g_SendYc = 1;
    else
        g_SendYc = 0;
    if(g_gRunPara[RP_STATUSRP_INTERVAL])
        g_DateCount  = g_gRunPara[RP_STATUSRP_INTERVAL];
        
    g_OpenBatFlag = OFF;
    g_SunHighNum  = 0;
    g_Soenum=0;

    
}
*/

/*
 *�ж�һ��unsigned long�ķ�0�����λ����
 *@param[in] xi Ҫ�жϵ�unsigned long
 *@return ����������ķ�������λ��
 */
unsigned int sqrtinit(unsigned long xi)// �ж�һ��unsigned long�ķ�0�����λ��
{
    unsigned int x0 = 16;
    int n;
//      unsigned long  xt;
    //xt = xi;
    // x0 = (2^4) +/- (2^3) +/- (2^2) +/- (2^1) +/- (2^0) +(1 or 0)
    // ��������Ϊ��,����Ϊ��
    for (n = 3;n >= 0; n--)
    {
        if ((xi >> x0) == 0)
            x0 = x0 - BIT[n];               //
        else
            x0 = x0 + BIT[n];               //
        //xt = xi;
    }
    if ((xi >> x0) > 0)
        x0++;
    return x0;
}

/*
  *��������
  *@param[in] Ҫ��������(unsigned long)
  *@return ���ؿ���ֵ(unsigned int)
  *@note ʵ�ַ���λţ�ٵ����������ڲ����ˡ�
  *
  */
unsigned int ndsqrt(unsigned long x)// ��һ��unsigned long��������ţ�ٵ�����
{
    unsigned int xk, x0, r;
    unsigned int zi;
    
    x0 = sqrtinit(x);
    
    if (x0 <= 12) zi = 1;
    else zi = 2;
    
    if (x0 % 2 == 0)
    {
        x0 = x0 / 2 - 1;
        xk = 2 << x0 ;
    }
    else
    {
        x0 = (x0 - 1) / 2;
        xk = 2 << (x0 - 1) ;
    }

    if (xk == 0) xk = 1;
    
    r = (xk + x / xk) / 2;// ţ�ٵ�������
    
    if (r - xk <= zi || xk - r <= zi)
        return r;
    else
    {
        while (r - xk > zi || xk - r > zi)
        {
            xk = r;
            if (xk == 0) xk = 1;
            r = (xk + x / xk) / 2;
            if (xk - r <= zi || r - xk <= zi) break;
        }
        return r;
    }
}

//��������������������������������������������������
//��������:table_sqrt()
//��������:�����,�����������ݶ���Q10
//����˵��:Ҫ������unsigned long������
//����ֵ:�������unsigned int������
//������������������������������������������������
unsigned int table_sqrt(unsigned long x)//����Ŀ�������,����������Q8����
{
    unsigned int pos_valid;//the position which the first 1 appears in x in 2s format
    unsigned int Temp_pos1,Temp_pos2;//the two temporary variables are used to determine 
                     
    unsigned long temp_result;
    pos_valid = sqrtinit(x);//ȷ����һ��1��λ�ã������Ʊ�ʾ��
    if(pos_valid <= 12)//x is less than FFF.
    {
        temp_result = sqrt_table[x] >> 10;
    }       
    else//x is large than FFF
    {
        Temp_pos1 = (pos_valid - 11) >> 1;  //-12 + 1
        Temp_pos2 = (pos_valid - 12) >> 1;

        x = Temp_pos1 > Temp_pos2 ? (x >> (pos_valid - 11)) : (x >> (pos_valid - 12)); 
       
        temp_result = (long)sqrt_table[x] >> (10 - Temp_pos1);//left-shift of result to countact the right-shift of X.

    } 
//    result = temp_result >> 10;//��������ΪQ10
    return (unsigned int)temp_result;
}

//==============================================================================
//  ��������   : AverFilter
//  ��������   : ��λֵƽ���˲���,10��ֵ����ȥ������С��,ʣ��8��ֵ��ƽ��ֵ
//  �������   : unsigned int *p: Ҫ�˲������飬Ϊһ������Ϊ10�����飬��ȥ������С��ʣ��8��ֵ��ƽ��ֵ
//  �������   : �˲����
//  ����ֵ     : <��>
//  ����˵��   : ע�������ĳ�����ҪΪ10
//  ����       : ����һ
//==============================================================================
unsigned int AverFilter(unsigned int *p)  //��λֵƽ���˲���,10��ֵ����ȥ������С��,ʣ��8��ֵ��ƽ��ֵ
{
    unsigned int i,max,min,temp=0;
    unsigned long sum;
            //�ر�ȫ���ж�
    sum = 0;
    max = p[0];
    min = p[0];

    for (i=0;i<RM_FILTER_NUM ;i++)
    {
        temp =p[i];                                
        if (temp > max)
        {
            max=temp;
        }
        if (temp < min)
        {
            min=temp;
        }
        sum +=temp;
    }

    sum = (sum - max - min);
    sum >>= 3; 

    return (unsigned int)sum;
}


//==============================================================================
//  ��������   : InitDataProc
//  ��������   : ���ݴ����ʼ��
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       : ����һ
//==============================================================================
void InitDataProc(void)
{
    unsigned int i,j,temp[8];
    unsigned long ulAddr;
 
    g_sSampleData.m_unAcDataTail = (AC_BUFF_LEN - 1);
 

    //¼��ģ���õ���ȫ�ֱ�����ʼ��

    g_sRecData.m_ucRecSavingFlag = OFF;

    g_sRecData.m_unRecAcLockCnt = 0;
    g_sRecData.m_unRecAcTail = 0;

    g_sRecData.m_ucRecStart = OFF;
    g_sRecData.m_ucFaultRecStart= OFF;	
    g_sRecData.m_ucActRecStart = CLOSE;	
    g_sRecData.m_ucActRecSavingFlag = OFF;  	

    CAT_SpiReadWords(EEPADD_LUBONUM, 6, temp);//��ȡ¼��������
    if(temp[0]>MAX_REC_NUM) temp[0]=0;//¼��������>MAX_REC_NUM ����Ϊ0
    if(temp[1]>MAX_REC_NUM) temp[1]=0;//¼��������>MAX_REC_NUM ����Ϊ0
    if(temp[2]>MAX_ACTREC_NUM) temp[2]=0;//¼��������>MAX_REC_NUM ����Ϊ0
    if(temp[3]>MAX_ACTREC_NUM) temp[3]=0;//¼��������>MAX_REC_NUM ����Ϊ0
    if(temp[4]>MAX_XHREC_NUM) temp[4]=0;//¼��������>MAX_REC_NUM ����Ϊ0
    if(temp[5]>MAX_XHREC_NUM) temp[5]=0;//¼��������>MAX_REC_NUM ����Ϊ0    
    if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_DEL_LUBO])
    	{
    	temp[0]=0;temp[1]=0;temp[2]=0;temp[3]=0;temp[4]=0;temp[5]=0;
		DelALLSOE();
    	}
    CAT_SpiWriteWords(EEPADD_LUBONUM, 6, temp); 
      g_sRecData.m_gRecANum=temp[0];//¼��������1~32
      g_sRecData.m_gRecCNum=temp[1];//¼����ǰλ��0~31
      g_sRecData.m_gACTRecANum=temp[2];//¼��������1~32
      g_sRecData.m_gACTRecCNum=temp[3];//¼����ǰλ��0~31      
      g_sRecData.m_gXHRecANum=temp[4];//¼��������1~32
      g_sRecData.m_gXHRecCNum=temp[5];//¼����ǰλ��0~31
	  g_sRecData.m_gACTDelay = 0;
	  g_sRecData.m_gXHDelay=0;
//��ǰ����flash�У��ļ�Ŀ¼	
    //long FADDR_RECORDER =FADDR_RECORDER_ACTDATA+ (long)temp[1]*(long)FLINEADDR;	
    //Sector_Erase(FADDR_RECORDER);//Ԥ�Ȳ�����һ��sector
//��ǰ����flash�У�¼���ļ�
  ulAddr = FADDR_RECORDER_DATA+(unsigned long)(g_sRecData.m_gRecCNum)*0x2000;//flash��ַ
  //g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ  
  Sector_Erase(ulAddr);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x1000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  
  ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;  
  g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ  
  Block_Erase(ulAddr);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x10000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x20000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x30000);//ERASE 1��BLOCK 
  delayms(100);WatchDog(); 
  Block_Erase(ulAddr+0x40000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x50000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x60000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x70000);//ERASE 1��BLOCK 
  delayms(100);WatchDog(); 
  Block_Erase(ulAddr+0x80000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();  

  ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;//flash��ַ
  //g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ  
  Sector_Erase(ulAddr);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x1000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x2000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x3000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x4000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x5000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x6000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x7000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  
  FlashReading = 0;
  
//���ɼ�¼��ʼ��
      CAT_SpiReadWords(EEPADD_LOADNUM, FLOADINFONUM, temp);
      if((temp[FLOAD_CS] != temp[FLOAD_TOTALNUM] + temp[FLOAD_NEW] + temp[FLOAD_OLD]) || temp[FLOAD_TOTALNUM] > FLASH_LOAD_MAXNUM || temp[FLOAD_NEW] > FLASH_LOAD_MAXNUM)//���FLASH��ַ���ڸ��ɼ�¼����������
      {
          temp[FLOAD_TOTALNUM] = 0; //��ո��ɼ�¼������
          temp[FLOAD_NEW] = 0; //�����洢�ļ�¼���һ����¼��ƫ��λ��(�����Ѵ洢������һ����¼��λ��)
          temp[FLOAD_OLD] = 0;//����һ����¼��λ��
          temp[FLOAD_CS] = 0;
          CAT_SpiWriteWords(EEPADD_LOADNUM, FLOADINFONUM, temp);
      }
	//g_gSaveload=0;
//LOG��¼��ʼ��
	  CAT_SpiReadWords(EEPADD_LOGP , FLOGINFONUM, temp);
	  if((temp[FLOG_CS] != temp[FLOG_TOTALNUM] + temp[FLOG_NEW] + temp[FLOG_OLD])
	  	|| temp[FLOG_TOTALNUM] > FLASH_LOG_MAXNUM || temp[FLOG_NEW] > 0X9000|| temp[FLOG_OLD] > 0X9000
	  	|| ((temp[FLOG_OLD]&0x0fff)!=0))//���FLASH��ַ���ڸ��ɼ�¼����������
      	{
      	CAT_SpiReadWords(EEPADDBK_LOGP , FLOGINFONUM, temp);
		if((temp[FLOG_CS] != temp[FLOG_TOTALNUM] + temp[FLOG_NEW] + temp[FLOG_OLD])
	  		|| temp[FLOG_TOTALNUM] > FLASH_LOG_MAXNUM || temp[FLOG_NEW] > 0X9000|| temp[FLOG_OLD] > 0X9000
	  		|| ((temp[FLOG_OLD]&0x0fff)!=0))//���FLASH��ַ���ڸ��ɼ�¼����������
			{
			temp[FLOG_TOTALNUM]=0;
			temp[FLOG_NEW]=0;
			temp[FLOG_OLD]=0;temp[FLOG_CS]=0;
			log_recorded.log_Flash_count=temp[FLOG_TOTALNUM];
			log_recorded.log_FlashNewPtr=temp[FLOG_NEW]+FADDR_LOG_START;
			log_recorded.log_FlashPtr=temp[FLOG_OLD]+FADDR_LOG_START;		
    		CAT_SpiWriteWords(EEPADD_LOGP , FLOGINFONUM, temp);		
			CAT_SpiWriteWords(EEPADDBK_LOGP, FLOGINFONUM,temp);
			Sector_Erase(log_recorded.log_FlashNewPtr);
	  		}
		else
			{
			log_recorded.log_Flash_count=temp[FLOG_TOTALNUM];
			log_recorded.log_FlashNewPtr=temp[FLOG_NEW]+FADDR_LOG_START;
			log_recorded.log_FlashPtr=temp[FLOG_OLD]+FADDR_LOG_START;		
    		CAT_SpiWriteWords(EEPADD_LOGP , FLOGINFONUM, temp);				
			}
	  	}
	  else
	  	{
	  	log_recorded.log_Flash_count=temp[FLOG_TOTALNUM];
		log_recorded.log_FlashNewPtr=temp[FLOG_NEW]+FADDR_LOG_START;
		log_recorded.log_FlashPtr=temp[FLOG_OLD]+FADDR_LOG_START;
	  	}

    for(i = 0; i < AC_AD_CHAN_NUM; i++)     //���ڸտ�����ʱ�򣬽����ɼ�������������û����������Ҫ��ոû�����������������ۼ�ֵ����ȷ
    {
        for(j = 0; j < AC_BUFF_LEN; j++)
        {
            g_sSampleData.m_gAcAdcData[i][j] = 0;
        }
    } 
   
    for(i = 0; i < PROC_MEAS_NUM; i++)
    {
        g_gProcMeas[i] = 0;
    }
    //g_gRmtMeas[RM_ACT_NUM] = 1;//�ŏ| ��������
    //soe������ʼ��    
    g_unDSoeSavePtr = 0;
    phase_off = 0;
    phase_copy = 0;
    soe_ptr = 0;
    soe_area = 0;
    //soe_num = 0;
    g_gWeekNum = 0;
	log_recorded.log_status_bk=0;log_recorded.log_status=0;

}
//==============================================================================
//  ��������   : ComputeMax
//  ��������   : ���������������ֵ
//  �������   : unsigned long x,y,z;����ֵ
//  �������   : <��>
//  ����ֵ     : unsigned long result;�������ֵ
//  ����˵��   : 
//  ����       : ����һ
//==============================================================================
unsigned int ComputeMax(unsigned int x,unsigned int y,unsigned int z)//�������ֵ
{
    unsigned int result;

    result = x >= y ? x : y;
    result = result >= z ? result : z;

    return result;
}

//==============================================================================
//  ��������   : ComputeMax
//  ��������   : ��������������Сֵ
//  �������   : unsigned long x,y,z;����ֵ
//  �������   : <��>
//  ����ֵ     : unsigned long result;������Сֵ
//  ����˵��   : 
//  ����       : ����һ
//==============================================================================
unsigned int ComputeMin(unsigned int x,unsigned int y,unsigned int z)//������Сֵ
{
    unsigned int result;

    result = x <= y ? x : y;
    result = result <= z ? result : z;

    return result;
}
//==============================================================================
//  ��������   : CalcuProtMeas
//  ��������   : ���ָ�����,���������浽����������(Ϊ����֮ǰ������)
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       : ����һ
//==============================================================================
/*
void CalcuProtMeas(void)
{

   // unsigned int i;
    long m,n,m1,n1,m2,n2,m3,n3;
   // unsigned long l = 0;
    unsigned int AddIndex, RemoveIndex;
   // static long tempAdc;
    
    //����У��ϵ�������仯�����ܳ���Protmeas[]Ϊԭ����ֵ����AdcData������*AdjPara�����ݷ����仯������Protmeas += (m+n)(m-n)����Ϊ�����������쳣��
    //���У����ʱ�����AdcData��Protmeas ��˶�AdjPara�����ı仯�����ܹ�ʹװ������
    AddIndex = (g_sSampleData.m_unAcDataTail);      //��þ������ܲ������µ������ڽ����ɼ��������е�λ��
    RemoveIndex = (AddIndex - AC_SAMPLE_DOTS) & (AC_BUFF_LEN - 1);  //���Ҫ�Ӹ��ܲ�ȥȥ�������ϵ����ݵ�λ��
    //Removedots = (AddIndex - AC_SAMPLE_DOTS_HALF) & (AC_BUFF_LEN - 1);
    //��I0,U0,UA,UB,UC,UAB,UBC,UCA�Ĵ������ǵ�ִ��Ч�ʣ���˷ŵ�forѭ����
    m = g_sSampleData.m_gAcAdcData[CHAN_I0][AddIndex];
    n = g_sSampleData.m_gAcAdcData[CHAN_I0][RemoveIndex];
    g_gProcMeas[PM_I0] += (m+n)*(m-n);//mƽ��-nƽ��=(m+n)(m-n)
        
    m = g_sSampleData.m_gAcAdcData[CHAN_UA][AddIndex];
    m1 = g_sSampleData.m_gAcAdcData[CHAN_UB][AddIndex];
    m2 = g_sSampleData.m_gAcAdcData[CHAN_UC][AddIndex];
    n = g_sSampleData.m_gAcAdcData[CHAN_UA][RemoveIndex];
    n1 = g_sSampleData.m_gAcAdcData[CHAN_UB][RemoveIndex];
    n2 = g_sSampleData.m_gAcAdcData[CHAN_UC][RemoveIndex];
    g_gProcMeas[PM_UA] += (m+n)*(m-n);
    g_gProcMeas[PM_UB] += (m1+n1)*(m1-n1);
    g_gProcMeas[PM_UC] += (m2+n2)*(m2-n2);
    
    m3 = m - m1;
    n3 = n - n1;
    g_gProcMeas[PM_UAB] += (m3+n3)*(m3-n3);
    m3 = m1 - m2;
    n3 = n1 - n2;
    g_gProcMeas[PM_UBC] += (m3+n3)*(m3-n3);
    m3 = m2 - m;
    n3 = n2 - n;
    g_gProcMeas[PM_UCA] += (m3+n3)*(m3-n3);
  
    m = g_sSampleData.m_gAcAdcData[CHAN_U0][AddIndex];
    n = g_sSampleData.m_gAcAdcData[CHAN_U0][RemoveIndex];
    g_gProcMeas[PM_U0] += (m+n)*(m-n);//mƽ��-nƽ��=(m+n)(m-n)

    g_unRmCaluFlag = ON;    //ң�����ݸ����ˣ����Խ���ң����������
}
*/
const static int real[32]={ 8192,8035,7568,6811,5793,4551,3135,1598,
0,-1598,-3135,-4551,-5793,-6811,-7568,-8035,
-8192,-8035,-7568,-6811,-5793,-4551,-3135,-1598,
0,1598,3135,4551,5793,6811,7568,8035
} ; 

const int img[32]={ 0,-1598,-3135,-4551,-5793,-6811,-7568,-8035,
-8192,-8035,-7568,-6811,-5793,-4551,-3135,-1598,
0,1598,3135,4551,5793,6811,7568,8035,
8192,8035,7568,6811,5793,4551,3135,1598
} ; 

//===============================  
// ��������   : ���ݸ���Ҷ�㷨��������Чֵ
//  �������   : AdcChannel --AD�������ݵ�ͨ����ֱ�Ӵ�g_sSampleData.m_gAcAdcData[AdcChannel]ȡ��Ӧ������
//  �������   : ���ݼ��������ʵ�����鲿ֱ�ӻ����Чֵ���з���
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       : lzy
//===============================  
unsigned long CalcuDft(unsigned char AdcChannel)
{
    unsigned char n;
    int tAdcData;
    long r = 0;
    long i = 0;
    unsigned long result=0; 

    for( n = 0 ; n < AC_SAMPLE_DOTS; n++ )
    {   

        tAdcData = g_sSampleData.m_gAcAdcData[AdcChannel][(g_sSampleData.m_unAcDataTail - n) & 0x1F];
	 r += ((long)real[2*n]) * tAdcData;
        i += (long)img[2*n] * tAdcData;
    }
    r = r >> 13;
    i = i >> 13;
    result = (r * r + i * i);
    
    return result;
    
}

//==============================================================================
//  ��������   : CalcuRmtMeas
//  ��������   : ��Чֵ���㣬�����¶�Ӧ��ң��ֵ
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       : ����һ
//==============================================================================
void CalcuRmtMeas(void)
{
    unsigned int i;
    unsigned int TempRm = 0;

    unsigned long tDft,a,b,c;
    //unsigned int unTemp[3];
    if(g_unRmCaluFlag == OFF)   //���ң�������־��ȻΪOFF����˵��1.25ms��AD�ж�û�н�ȥ����g_gProcMeasû�и��£�����Ҫ����
    {
        return;
    }
    else
    {
        g_unRmCaluFlag = OFF;
    }
    //ProtLogic();
    //ProtStart();	
	
    for(i = 0; i < 6/*RMT_MEAS_NUM -1*/ ; i++)//�����UAB,UBC,UCA���ߵ�ѹ��i������=PM_UCA
    {
        tDft = (CalcuDft(i) >> 3);
       
        if(i == CHAN_I0)//3            
            g_gRmAcFilt[RM_I0][g_unFilterIndex] = (unsigned long)table_sqrt(tDft) * COEF_AD_I_0 >>14; 
        if(i == CHAN_U0)//3
            //g_gRmAcFilt[i][g_unFilterIndex] = (unsigned long)table_sqrt(g_gProcMeas[i]) * COEF_AD_U >>14;  
           g_gRmAcFilt[RM_U0][g_unFilterIndex] = (unsigned long)table_sqrt(tDft) * COEF_AD_U>> 14;
	 if(i == CHAN_UA)//3
           g_gRmAcFilt[RM_UA][g_unFilterIndex] = (unsigned long)table_sqrt(tDft) * COEF_AD_U>> 14;	
	 if(i == CHAN_UB)//3
           g_gRmAcFilt[RM_UB][g_unFilterIndex] = (unsigned long)table_sqrt(tDft) * COEF_AD_U>> 14;
	 if(i == CHAN_UC)//3
           g_gRmAcFilt[RM_UC][g_unFilterIndex] = (unsigned long)table_sqrt(tDft) * COEF_AD_U>> 14;
	 if(i == CHAN_Upt)//3
           g_gRmAcFilt[RM_UPt][g_unFilterIndex] = (unsigned long)table_sqrt(tDft) * COEF_AD_U>> 14;
    }
	 a=(unsigned long)g_gRmAcFilt[RM_UA][g_unFilterIndex];
         b=(unsigned long)g_gRmAcFilt[RM_UB][g_unFilterIndex];
         c=(unsigned long)g_gRmAcFilt[RM_UC][g_unFilterIndex];
	 tDft=(a+b)*(a+b)-a*b;
	 g_gRmAcFilt[RM_UAB][g_unFilterIndex]= (unsigned long)table_sqrt(tDft);// * COEF_AD_U>> 14;

	  tDft=(c+b)*(c+b)-c*b;
	 g_gRmAcFilt[RM_UBC][g_unFilterIndex]= (unsigned long)table_sqrt(tDft);// * COEF_AD_U>> 14;

	  tDft=(a+c)*(a+c)-a*c;
	 g_gRmAcFilt[RM_UCA][g_unFilterIndex]= table_sqrt(tDft);// * COEF_AD_U>> 14;
	 
    	//}	
    for(i = 0; i < 8;i++)
    	{
    	g_gRmtMeas[i] =g_gRmAcFilt[i][g_unFilterIndex];
    	}
    for(i = 0; i < 8/*RMT_MEAS_NUM-1*/ ; i++)//�����UAB,UBC,UCA���ߵ�ѹ��i������=PM_UCA
    {
        TempRm = AverFilter(g_gRmAcFilt[i]);        //��ң�����Ľ����������˲�        
        g_gRmtFilMeas[i] = TempRm;
/*        if((i <= 3)&&(i>=1))
        {
            // if(g_gRmtMeas[i]> 25 && g_gRmtMeas[i]< 80)
            //       g_gRmtMeas[i] -= 20;
            unsigned long x;
            if( g_gRmtMeas[i] <2700)
            	{
			x=(unsigned long)g_gRmtMeas[i] *1049;
                        x=x>>10;                        
			if (x>75)
                        {
                          x=x-75;
			  g_gRmtMeas[i] =(unsigned int)x;//((g_gRmtMeas[i] *1049)>>10)-75;
                        }
            	}
        }     */  
    }
        TempRm = AverFilter(g_gRmAcFilt[9]);        //��UPtң�����Ľ����������˲�        
        g_gRmtFilMeas[9] = TempRm/25;	//Uo to Upt
        g_gRmtMeas[9] = g_gRmtFilMeas[9];
    g_unFilterIndex++;
    if(g_unFilterIndex == 10)
    {
        g_unFilterIndex = 0;
    }
}

	//==============================================================================
	//	��������   : ScanPT
	//	��������   : ɨ��PT����ң��
	//	�������   : <��>
	//	�������   : <��>
	//	����ֵ	  : <��>
	//	����˵��   : 100msִ��һ�Σ�500msȷ��
	//	����		 : �ŏ|
	//==============================================================================
void ScanPT(void)
{ 
  unsigned int unTemp[3];
  
    if(g_gRmtMeas[RM_U0] >= g_gProcCntJug[PC_HIGH_Z])  //�����ѹ
        {
        g_gVErrTimer[0]++;
	if(g_gVErrTimer[0]>5)
		{
        	g_gRmtInfo[YX_U0_HIGH]=1;
			SaveLOG(LOG_UPT_ERR,1);
		 g_gVErrTimer[0]=5;
		}
    	  }
    else
    	{    	
	if(g_gVErrTimer[0]==0)
      		{
      		g_gRmtInfo[YX_U0_HIGH]=0;
			SaveLOG(LOG_UPT_ERR,0);
			}
	else
		g_gVErrTimer[0]--;
    	}

    unTemp[1] = ComputeMax(g_gRmtMeas[RM_UAB], g_gRmtMeas[RM_UBC], g_gRmtMeas[RM_UCA]);  //�������ߵ�ѹ
    if(unTemp[1] >= g_gProcCntJug[PC_I0_START]) 
        g_gRmtInfo[YX_UAB_HIGH] =1;
    else
        g_gRmtInfo[YX_UAB_HIGH] =0;
	
    unTemp[0] = ComputeMax(g_gRmtMeas[RM_UA], g_gRmtMeas[RM_UB], g_gRmtMeas[RM_UC]);      //���������ѹ
    if(unTemp[0] >= g_gProcCntJug[PC_HIGH_P]) 
    	{
    	g_gVErrTimer[2]++;
	if(g_gVErrTimer[2]>5)
		{
        	g_gRmtInfo[YX_U_HIGH]=1;
			SaveLOG(LOG_UPT_ERR,1);
		g_gVErrTimer[2]=5;
		}
    	}
    else
    	{
    	if(g_gVErrTimer[2]==0)
        	{
        	g_gRmtInfo[YX_U_HIGH]=0;
			SaveLOG(LOG_UPT_ERR,0);
    		}
	else
		g_gVErrTimer[2]--;
	}
	
    unTemp[2] = ComputeMin(g_gRmtMeas[RM_UA], g_gRmtMeas[RM_UB], g_gRmtMeas[RM_UC]);
    if(unTemp[2] <= g_gProcCntJug[PC_LOW_P]) 
    	{
    	g_gVErrTimer[3]++;
    	if(g_gVErrTimer[3]>5)
    		{
       	 g_gRmtInfo[YX_U_LOW] =1 ;
		 SaveLOG(LOG_UPT_ERR,1);
		 g_gVErrTimer[3]=5;
    		}
    	}
    else
    	{
    	if(g_gVErrTimer[3]==0)
        	{
        	g_gRmtInfo[YX_U_LOW] =0 ;
			SaveLOG(LOG_UPT_ERR,0);
    		}
	else
		g_gVErrTimer[3]--;
        }

    if(g_gRmtMeas[RM_UA]<= g_gProcCntJug[PC_LOW_P]) 
    	{
    	g_gVErrTimer[4]++;
	if(g_gVErrTimer[4]>5)
		{
        	g_gRmtInfo[YX_UA_LOW] =1 ;
		g_gVErrTimer[4]=5;
		}
    	}
    else
    	{
    	if(g_gVErrTimer[4]==0)
    		g_gRmtInfo[YX_UA_LOW] =0 ;	
	else
		g_gVErrTimer[4]--;
    	}
	
    if(g_gRmtMeas[RM_UB]<= g_gProcCntJug[PC_LOW_P]) 
    	{
    	g_gVErrTimer[5]++;
	if(g_gVErrTimer[5]>5)
		{
        	g_gRmtInfo[YX_UB_LOW] =1 ;
		g_gVErrTimer[5]=5;
		}
    	}
    else
    	{
    	if(g_gVErrTimer[5]==0)
    		g_gRmtInfo[YX_UB_LOW] =0 ;	
	else
		g_gVErrTimer[5]--;
    	}
	
    if(g_gRmtMeas[RM_UC]<= g_gProcCntJug[PC_LOW_P]) 
    	{
    	g_gVErrTimer[6]++;
	if(g_gVErrTimer[6]>5)
		{
        	g_gRmtInfo[YX_UC_LOW] =1 ;
		g_gVErrTimer[6]=5;
		}
    	}
    else
    	{
    	if(g_gVErrTimer[6]==0)
    		g_gRmtInfo[YX_UC_LOW] =0 ;	
	else
		g_gVErrTimer[6]--;
    	}	
	
    if(g_gRmtMeas[RM_UA] > g_gProcCntJug[PC_HIGH_P])
    	{
    	g_gVErrTimer[7]++;
	if(g_gVErrTimer[7]>5)
		{
        	g_gRmtInfo[YX_UA_HIGH]=1;
		g_gVErrTimer[7]=5;
		}
    	}
    else
    	{
	if(g_gVErrTimer[7]==0)
        	g_gRmtInfo[YX_UA_HIGH]=0;	
	else
		g_gVErrTimer[7]--;
    	}
	
    if(g_gRmtMeas[RM_UB] > g_gProcCntJug[PC_HIGH_P])
    	{
    	g_gVErrTimer[8]++;
	if(g_gVErrTimer[8]>5)
		{
        	g_gRmtInfo[YX_UB_HIGH]=1;
		g_gVErrTimer[8]=5;
		}
    	}
    else
    	{
	if(g_gVErrTimer[8]==0)
        	g_gRmtInfo[YX_UB_HIGH]=0;	
	else
		g_gVErrTimer[8]--;
    	}
	
    if(g_gRmtMeas[RM_UC] > g_gProcCntJug[PC_HIGH_P])
    	{
    	g_gVErrTimer[9]++;
	if(g_gVErrTimer[9]>5)
		{
        	g_gRmtInfo[YX_UC_HIGH]=1;
		g_gVErrTimer[9]=5;
		}
    	}
    else
    	{
	if(g_gVErrTimer[9]==0)
        	g_gRmtInfo[YX_UC_HIGH]=0;	
	else
		g_gVErrTimer[9]--;
    	}	
}
//==============================================================================
//  ��������   : YCthCross
//  ��������   :�ж�ң��ֵ��ͻ��
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       : ����һ
//==============================================================================
void YCthCross(void)
{
//�ŏ| ң����
unsigned int uuTemp[IEC_YC_NUM],i;  
unsigned long ulTemp[7];
unsigned long Templm = 0;

    if(g_gYCYueXian !=0x55)   //�ŏ| ң��Խ��
	return;

    for(i = 0; i < IEC_YC_NUM/*RMT_MEAS_NUM*/; i++)
    { 
	 uuTemp[i] = g_gRmtMeas[i] ;//����ң��Ҫ1��ֵ
        if(i==7)
        	{        	
        	if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_CURRENT_PRIMARY])
			uuTemp[i]=uuTemp[i]* g_gRunPara[RP_CT_TRANS]/100;//����Ϊ1��ֵ����λA
		else
			uuTemp[i]=uuTemp[i]*10;//����Ϊ2��ֵ����λmA
        	}
    }
//�ŏ| ң����    
    for(i = 0; i < IEC_YC_NUM/*RMT_MEAS_NUM*/; i++)
    {  
         //if(i <= 7)//�ŏ| ң����
            //g_unYcTrans[i] = (g_gRmtMeas[g_ucYCAddr[i] - 1] / 10);//�ŏ| ң����
            g_unYcTrans[i] = uuTemp[g_ucYCAddr[i] - 1] ;//�ŏ| ң����
        //else//�ŏ| ң����
            //g_unYcTrans[i] = g_gRmtMeas[g_ucYCAddr[i] - 1] * 10 ;//�ŏ| ң����
    }
	
     
    for(i = 0; i < IEC_YC_NUM/*RMT_MEAS_NUM*/; i++)
    { 
        if(g_gRmtMeasBak[i] != g_gRmtMeas[i] && i != 7)
        {
            if(g_gRmtMeas[i] >= g_gRmtMeasBak[i])
                Templm = g_gRmtMeas[i] - g_gRmtMeasBak[i];
            else
                Templm = g_gRmtMeasBak[i] - g_gRmtMeas[i];
            ulTemp[i] = g_gRmtMeasBak[i];
            if(Templm >= g_gRunPara[RP_YCLIMIT] || (((Templm* 100) > (ulTemp[i] * g_gRunPara[RP_YCCAP])) && (g_gRmtMeasBak[i] >= 1000)))
            {/*
                if(g_gChangeYCNum < 9)
                    g_gChangeYCNum++;
                g_gYCchangData[g_gChangeYCNum - 1] = i;
                if(i <= 7)
                    g_gYCchangData[g_gChangeYCNum + 9] = g_gRmtMeas[i]/10;
                else
                    g_gYCchangData[g_gChangeYCNum + 9] = g_gRmtMeas[i] * 10;
		*/ 
		char k;
		  for(k= 0;k< g_ucYcTransNum;k++)
                  {
                   if  (g_ucYCAddr[k]==(i+1))
          	     {
          	     	if(g_gChangeYCNum < 9)
                    	  g_gChangeYCNum++;
			g_gYCchangData[g_gChangeYCNum - 1] = i;				
			 g_gYCchangData[g_gChangeYCNum + 9] = g_gRmtMeas[i];//�������ϣ�ң��Ҫһ��ֵ
			 if(i == 7)
			 	{
			 	if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_CURRENT_PRIMARY])
					g_gYCchangData[g_gChangeYCNum + 9] =g_gYCchangData[g_gChangeYCNum + 9] * g_gRunPara[RP_CT_TRANS]/100;//����Ϊ1��ֵ����λA
				else
					g_gYCchangData[g_gChangeYCNum + 9] =g_gYCchangData[g_gChangeYCNum + 9] *10;//����Ϊ2��ֵ����λmA
			 	}
          	        g_SendChgYcFlag = 1;
		        break;
          	      }		  	
                  } 		
                //g_SendChgYcFlag = 1;
              }            
        }
    }	
    
   if(g_gYCYueXian==0x55)   //�ŏ| ң��Խ��
     {
     	for(i = 0; i < IEC_YC_NUM/*RMT_MEAS_NUM*/; i++)
        { 
        g_gRmtMeasBak[i] = g_gRmtMeas[i];	
     	}
        g_gYCYueXian=0;//�ŏ| ң��Խ��	
     } 

}
//==============================================================================
//  ��������   : RecData
//  ��������   : ¼������׼������1.25ms�ж��е��á�
//                   ��ɱ���Ԫ��ǰ��¼������׼��,��ɹ��϶������������֮ǰ��¼������׼��
//                   ��ɹ��϶�����������ں���0.125ms�ж���δ��ɵ�¼������׼��
//                   ¼������׼����ϣ�����صı�־
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 0.125ms�����е�¼������׼�������øú�����ֱ�����ж���д����
//  ����       : ����һ
//==============================================================================
/*
void RecData(void)
{
    unsigned int unTemp = 0;
    
    if(g_sRecData.m_ucRecSavingFlag == YES)
    {
    return; //���¼�����ݻ�û�б���������ٽ����µ�¼��
    }
    
    
    
    //if(g_sRecData.m_unRecAcLockCnt == 0)    //û�й��ϳ��ڻ��߲������ڣ���׼������֮ǰ��¼������
    if(g_sRecData.m_ucRecStart == OFF)
    {

//�ŏ| ¼��I0U0        g_sRecData.m_gRecAc[CHAN_I0][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_I0][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[CHAN_UA][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_UA][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[CHAN_UB][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_UB][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[CHAN_UC][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_UC][g_sSampleData.m_unAcDataTail];
        //���������ѹ����
//�ŏ| ¼��I0U0        g_sRecData.m_gRecAc[CHAN_U0][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_U0][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_unRecAcTail++;
        if(g_sRecData.m_unRecAcTail == REC_AC_LEN)
            g_sRecData.m_unRecAcTail = 0;
    }
    else// if(g_sRecData.m_unRecAcLockCnt < 96)//����������ʼ���¼����Ҫ��¼��6���ܲ���¼��6���ܲ�����96�����ֹͣ¼����
    {
        //����0.125ms�иò������ݿ���û��׼����ɣ���Ҫ�ڸ��ٲ��������м���׼����׼����ɺ���¼������׼����ϱ�־

        
        g_sRecData.m_gRecAc[CHAN_UA][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_UA][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[CHAN_UB][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_UB][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[CHAN_UC][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_UC][g_sSampleData.m_unAcDataTail];
        //���������ѹ����
//�ŏ| ¼��I0U0        g_sRecData.m_gRecAc[CHAN_U0][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_U0][g_sSampleData.m_unAcDataTail];
 //�ŏ| ¼��I0U0       g_sRecData.m_gRecAc[CHAN_I0][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_I0][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_unRecAcTail++;
        if(g_sRecData.m_unRecAcTail == REC_AC_LEN)
            g_sRecData.m_unRecAcTail = 0;

        g_sRecData.m_unRecAcLockCnt++;  //����������ʼ���¼����Ҫ��¼��6���ܲ���¼��6���ܲ�����96�����ֹͣ¼��
        if(g_sRecData.m_unRecAcLockCnt == 96)   //96�¹ʻ�������ڷ������6���ܲ���¼������׼�����
        {
        //  g_sProtLogic.m_ucActStartRec = OFF;  
          g_sRecData.m_ucRecPrepFlag = 2; //¼������׼����ϣ�׼��д��FLASH��
          if(pulse_phase_flag != 2 ) //B�಻¼��
          {
            
          unTemp = (g_sRtcManager.m_gRealTimer[RTC_SEC]*1000 + g_sRtcManager.m_gRealTimer[RTC_MICROSEC]); 
          g_sRecData.m_gFaultRecOver[REC_MSL] = unTemp;
          g_sRecData.m_gFaultRecOver[REC_MSH] = (unTemp>>8);
          g_sRecData.m_gFaultRecOver[REC_MINU] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
          g_sRecData.m_gFaultRecOver[REC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
          g_sRecData.m_gFaultRecOver[REC_DAY] = g_sRtcManager.m_gRealTimer[RTC_DATE];
          g_sRecData.m_gFaultRecOver[REC_MONTH] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
          g_sRecData.m_gFaultRecOver[REC_YEAR] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000);
          }
        }
    }

}

*/
//==============================================================================
//  ��������   : RecData
//  ��������   : ¼������׼������0.25ms�ж��е��ã�4K����Ƶ�ʡ�
//                   ��ɱ���Ԫ��ǰ��¼������׼��,��ɹ��϶������������֮ǰ��¼������׼��
//                   ��ɹ��϶�����������ں���0.125ms�ж���δ��ɵ�¼������׼��
//                   ¼������׼����ϣ�����صı�־
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 0.25ms�ж���д����
//  ����       : ����һ
//==============================================================================
void RecData(void)
{
    //unsigned int unTemp = 0;
	//static unsigned int ab= 0;
    
    if(g_sRecData.m_ucFaultRecStart ==CLOSE)
    {
    return; //�������û�лָ������ٽ����µ�¼��
    }
    if(g_sRecData.m_ucActRecStart  != CLOSE)
    {
    return; //���¼�����ݻ�û�б���������ٽ����µ�¼��
    }
	
        g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][0] =g_unAdcData[CHAN_UA]-g_gAdjAD[CHAN_UA];// g_sSampleData.m_gAcAdcData[CHAN_UA][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][1] =g_unAdcData[CHAN_UB]-g_gAdjAD[CHAN_UB];// g_sSampleData.m_gAcAdcData[CHAN_UB][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][2] =g_unAdcData[CHAN_UC]-g_gAdjAD[CHAN_UC];// g_sSampleData.m_gAcAdcData[CHAN_UC][g_sSampleData.m_unAcDataTail];
        if(g_gProcCnt[PC_U0_CAL] == 0)  //
              g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][3] = g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][0]+g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][1]+g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][2]; 
	 else
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][3] =g_unAdcData[CHAN_U0]-g_gAdjAD[CHAN_U0];
	  g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][4] =g_unAdcData[CHAN_I0]-g_gAdjAD[CHAN_I0];

		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][3] = g_gRmtMeas[RM_UA];
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][4] = g_gRmtMeas[RM_UB];
	  
	  if((g_gKON>0)&&(g_gKON<4))//�ŏ|�����Ѿ��պ�//¼�����Ӽ̵���������
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][4] |= (1<<14);//¼�����Ӽ̵���������
	  else	
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][4] &= NBITE;//¼�����Ӽ̵���������
	  if(g_gKON==1) 	
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][0] |= (1<<14);//¼�����Ӽ̵���������
	  else	
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][0] &= NBITE;//¼�����Ӽ̵���������
	  if(g_gKON==2) 	
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][1] |= (1<<14);//¼�����Ӽ̵���������
	  else	
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][1] &= NBITE;//¼�����Ӽ̵���������
	  if(g_gKON==3) 	
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][2] |= (1<<14);//¼�����Ӽ̵���������
	  else	
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][2] &= NBITE;//¼�����Ӽ̵���������
		
        g_sRecData.m_unRecAcTail++;
        if(g_sRecData.m_unRecAcTail == REC_AC_LEN)
            g_sRecData.m_unRecAcTail = 0;    
    

    if(g_sRecData.m_ucFaultRecStart == OFF)
    {
	        g_sRecData.m_unRecAcLockCnt = 0;
    }
    else if(g_sRecData.m_ucFaultRecStart == ON)//��ʼ¼����¼1024�����
    {
        g_sRecData.m_unRecAcLockCnt++; 
#ifdef YN_101S
	 if(g_sRecData.m_unRecAcLockCnt >= 10)   //���Ϸ������5���ܲ���¼��ϣ���¼���ʱ�䣬
#else
	 if(g_sRecData.m_unRecAcLockCnt >= 120)   //���Ϸ������5���ܲ���¼��ϣ���¼���ʱ�䣬
#endif
        {
            g_sRecData.m_ucFaultRecStart = CLOSE;//¼�����������ϻָ��󣬻ָ�OFF
            g_sRecData.m_unRecAcLockCnt = 1000;
            g_sRecData.m_gFaultRecOver[REC_MSL] =  g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
            g_sRecData.m_gFaultRecOver[REC_MSH] =g_sRtcManager.m_gRealTimer[RTC_SEC];
            g_sRecData.m_gFaultRecOver[REC_MINU] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
            g_sRecData.m_gFaultRecOver[REC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
            g_sRecData.m_gFaultRecOver[REC_DAY] = g_sRtcManager.m_gRealTimer[RTC_DATE];
            g_sRecData.m_gFaultRecOver[REC_MONTH] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
            g_sRecData.m_gFaultRecOver[REC_YEAR] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000);
            g_sRecData.m_ucRecSavingFlag = YES; //�ŏ| ¼��    //�ñ�־Ҫ�󱣴浽FLASH��
        }
    }

}
//==============================================================================
//  ��������   : RecActData
//  ��������   : ����¼�����ݴ���g_sRecData.m_gRecAc����1.25ms�ж��е��ã�800����Ƶ�ʡ���
//                   m_ucActRecStart Ϊ����¼����־��ON ��ʼ¼����OFF����¼����CLOSE������洢���
//                   m_ucActRecSavingFlag Ϊ�洢��־��YES�洢ǰ320���㡢ON�洢��320���㡢OFF�洢����
//                 
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 0.25ms�����еĹ���¼������׼�������øú�������1.25ms�ж��е��á�
//  ����       : �ŏ|
//==============================================================================

void RecActData(void)
{
    //static unsigned int abx= 0;
   if(g_sRecData.m_ucFaultRecStart == ON)
    {
    return; //����¼��,�ӹ��Ͽ�ʼ���洢���������ٽ����µ�¼��
    }   	
    if((g_sRecData.m_ucActRecStart == ON)||(g_sRecData.m_ucActRecStart == OFF))
    {
        g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][0] =g_unAdcData[CHAN_UA]-g_gAdjAD[CHAN_UA];// g_sSampleData.m_gAcAdcData[CHAN_UA][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][1] =g_unAdcData[CHAN_UB]-g_gAdjAD[CHAN_UB];// g_sSampleData.m_gAcAdcData[CHAN_UB][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][2] =g_unAdcData[CHAN_UC]-g_gAdjAD[CHAN_UC];// g_sSampleData.m_gAcAdcData[CHAN_UC][g_sSampleData.m_unAcDataTail];	
	if(g_gProcCnt[PC_U0_CAL] == 0)  //
              g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][3] = g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][0]+g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][1]+g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][2]; 
	 else
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][3] =g_unAdcData[CHAN_U0]-g_gAdjAD[CHAN_U0];
	 g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][3]=g_gRmtMeas[RM_I0];
	  g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][4] =g_unAdcData[CHAN_I0]-g_gAdjAD[CHAN_I0];//g_gRmtMeas[RM_I0]
	  if((g_gKON>0)&&(g_gKON<4))//�ŏ|�����Ѿ��պ�//¼�����Ӽ̵���������
	  	{
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][4] |= (1<<14);//¼�����Ӽ̵���������
	  	}
	  else	
	  	{
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][4] &= NBITE;//¼�����Ӽ̵���������
	  	}
	unsigned char ka,kb,kc;
	if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_CON_NC])
		{//0=nc
		ka=0x08;kb=0x10;kc=0x20;
		}
	else
		{//0=no
		ka=0;kb=0;kc=0;
		}
#ifdef CONDIN_3
	  if(KJa1==ka)
#else
	  if((g_gKONBK==1)&& (KJb1==kb))
#endif	  	
	  	{
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][0] |= (1<<14);//¼�����Ӽ̵���������
	  	}
	  else//if(g_gRmtInfo[YX_P4IN4]==1)	
	  	{
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][0] &= NBITE;//¼�����Ӽ̵���������
	  	}

#ifdef CONDIN_3
	  if(KJb1==kb)
#else	  
	  if((g_gKONBK==2)&&(KJb1==kb))	
#endif	  	
	  	{
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][1] |= (1<<14);//¼�����Ӽ̵���������
	  	}
	  else//if(g_gRmtInfo[YX_P4IN4]==1)		
	  	{
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][1] &= NBITE;//¼�����Ӽ̵���������
	  	}

#ifdef CONDIN_3
	  if(KJc1==kc)
#else	  
	  if((g_gKONBK==3)&& (KJb1==kb))
#endif	  	
	  	{
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][2] |= (1<<14);//¼�����Ӽ̵���������
	  	}
	  else//if(g_gRmtInfo[YX_P4IN4]==1)
	  	{
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][2] &= NBITE;//¼�����Ӽ̵���������

	  	}
		
g_test++;
    	if((g_sRecData.m_unRecAcTail == 360))
    		g_sRecData.m_ucActRecSavingFlag = YES; //��ʼ�洢ǰ320����	  
        g_sRecData.m_unRecAcTail++;
        if(g_sRecData.m_unRecAcTail == REC_AC_LEN)
        {
           g_sRecData.m_unRecAcTail = 0;  
	    g_sRecData.m_ucActRecSavingFlag = ON; //��ʼ�洢��320����	 	   
        }
   
    }
	/*
    else if(g_sRecData.m_ucActRecStart == OFF)
    {
    	if((g_sRecData.m_unRecAcTail>319))
    		g_sRecData.m_ucActRecSavingFlag = ON; //��ʼ�洢��320����
    	else
		g_sRecData.m_ucActRecSavingFlag = YES; //��ʼ�洢ǰ320����	
	//g_sRecData.m_ucActRecStart = CLOSE;	
    }
    */
}
/*
//==============================================================================
//  ��������   : SaveRecData
//  ��������   : ����COMTRADE��ʽ����¼�����ݣ��������α��浽Flash��
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       : ����һ
//==============================================================================
void SaveRecData(void)
{
    
      
}

//==============================================================================
//  ��������   : CreatNewSoe
//  ��������   : ������ΪSOEtype���¹ʣ�����ֵΪSOEvalue���¹�д��SOE��¼�����У�
//                   ������ѭ���аѻ���ͨ��SaveSoeData����д��EEPROM�б���
//                   ����ǿ��������������SOE,��Ҫ����һ��SOE��ʱ�����򣬲��ѵ�ǰ������ʱ����ǰDIN�ж���ʱʱ��
//                   ������������µ�32��SOE��¼��FLASH���ܹ����256��SOE
//  �������   : SOEtype-�����¹����ͣ�
//                   SOEPhase-�����¹ʷ�������λ,a-c��ֱ��Ӧ1-3,ab-bc-ca��Ӧ4-6,0Ϊ��Ч
//                   SOEvalue-�¹ʷ���ʱ��Ӧ��ֵ
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ������һ
//==============================================================================
void CreatNewSoe(unsigned int SoeType,unsigned int SoePhase, unsigned long SoeValue)
{
    
}
*/
//==============================================================================
//  ��������   : SaveSoeData
//  ��������   : ����m_unNewSoeRomNum�������µ�SOE���ݶ����浽Flash��
//                   Ϊ�˱�����дSOE�Ĺ����У��ٴβ����µ�SOE��¼�����ڽ��뺯��ʱ��
//                   �Ȳ�����ʱ������¼m_unSoeBuffTail��m_unNewSoeRomNum����ס��ǰ��Ҫд���SOE������
//                   �������������У������µ�SOE��¼�������˳��ú���������NewSOECount ��= 0��
//                   ����ٴν���ú���������SOE��д������
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       : 
//==============================================================================
void SaveSoeDataRepeat(void)
{
    unsigned int i;
   // unsigned char n,k,j,m,c,b;
   // unsigned char ucTemp[7];
    
    if((fault_save==0x55)&&(flash_write==0))  /////////��ʱ��ɣ���ʼ�洢����
    {
    	
       fault_save=0x33;     	   
       for(i=0;i<6;i++)
       {
           g_sSoeData.m_gSoeBuff[i] = g_sRtcManager.m_gRealTimer[i];
       }
       g_sSoeData.m_gSoeBuff[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
       i=(unsigned char)(g_gRmtMeas[RM_UA]/100);
       g_sSoeData.m_gSoeBuff[6]=i;
       i=(unsigned char)(g_gRmtMeas[RM_UB]/100);
       g_sSoeData.m_gSoeBuff[7]=i;
       i=(unsigned char)(g_gRmtMeas[RM_UC]/100);
       g_sSoeData.m_gSoeBuff[8]=i;
       i=(unsigned char)(g_gRmtMeas[RM_U0]/100);
       g_sSoeData.m_gSoeBuff[9]=i;
       g_sSoeData.m_gSoeBuff[10]=phase_seq_flag;
       g_sSoeData.m_gSoeBuff[11]=g_gProcCnt[PC_EFS_MODEL]; 
/*       
       if(g_gProcCnt[PC_EFS_MODEL]>0)                   //////�ź�Դ����ΪII�ͣ�
       	 {
       	 	if(g_ucEarthFlg == 1)                               //(mm[0]<=mm[1])&&(mm[0]<=mm[2])
       	 		{fault_sms_type=1;}
       	 	else if(g_ucEarthFlg == 2)//else if((mm[1]<=mm[0])&&(mm[1]<=mm[2]))
       	 		{fault_sms_type=2;}
       	        else if(g_ucEarthFlg == 3)  //else if((mm[2]<=mm[1])&&(mm[2]<=mm[0]))			
       	 	  {fault_sms_type=3;}
       	 }   
       else
           fault_sms_type=4;          
       CreatNewSMS(FAULT_OCCUR);                      //�������϶���//�ŏ|
 */      
       
       g_sSoeData.m_ucSoeNum++;      //��������
       if(g_sSoeData.m_ucSoeNum>30)
    	    g_sSoeData.m_ucSoeNum = 30;	   
       CAT_SpiWriteBytes(EEPADD_SOE_DATA + g_sSoeData.m_ucSoePos * 12 , 12, g_sSoeData.m_gSoeBuff);
       g_sSoeData.m_ucSoePos++;        //λ�ü�
    	if(g_sSoeData.m_ucSoePos>=30)
    	    g_sSoeData.m_ucSoePos = 0;	
        CAT_SpiWriteByte(EEPADD_SOE_NUM,g_sSoeData.m_ucSoeNum);
        CAT_SpiWriteByte(EEPADD_SOE_POS,g_sSoeData.m_ucSoePos); 
        eight_delay_counter= 200;  ///////emit 8 pulse after 2 seconds
        //g_sRecData.m_ucRecSavingFlag = YES; //�ŏ| ¼��    //�ñ�־Ҫ�󱣴浽FLASH��
       /* for(i=0;i<WAVE_DATA_LEN_BYTE;i++)
              g_sSoeData.m_gRecBuff[i] = 0;*/
    } 
	/*
    if(g_FirstACTFlag != 0  && g_gProcCnt[PC_JAG_ACT] <= 3)    ////////ֻҪ8������ʱ�������ͻ��������������⵽8�����������ϣ��ж��ط���δ��⵽8����������ط���
    { 
        if(repeat_flag1 == 0)  //Ͷ�н���
        {
           if(g_FinishACTFlag == 0x55)
           {
               g_gRmtInfo[0] |= YX_ACTION_SUCCESS;
           }
           g_FinishACTFlag = 0; //��0
           //eight_pulse_flag=0;
           efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];
           uart0_event_flag=0; 
           g_gRmtMeas[RM_ACT_NUM] = 1;
        }
        
        if(g_FirstACTFlag==0x55)
    	{             
              eight_ctl_flag=5;              
              KC1_ON;             ///�µ�·��                           	 	                   	      
	      sign_RSSI_delay=5;                     //////////////������Ч8���������ʱ��������ʱʱ��������վѯ�ʣ�����Ϊ�ǹ���    
	      pulse_success=0x55; 
                  	
    	     CreatNewSMS(EIGHT_PULSE);                      //����8������� //�ŏ|
			    		     	
    	 
    	    //if((n==0)&&(xinghao_flag==2)&&(repeat_flag1>0))        ///////////˵���е���ֵС��35A����·���ź�Դ����һ�μ�⵽����
    	    if(repeat_flag1>0)	
		Sign_Repeat(repeat_flag1,1);
	    repeat_flag1=0;	
	}
	else if(g_FirstACTFlag < 3)   //������Ч������
	{
	  //  flash_write=0; 
	  //  pulse_counter=0;	
	    if(repeat_flag1>0)
		(repeat_flag1,1);
	    repeat_flag1=0;	
	} 
        g_FirstACTFlag = 0;
        
    }
    */
}



//�ŏ| SOE��FLASH
void SaveSoeData(void)
{
    //���浥��SOE	
   //Sst26vf064b_Page_WriteBytes(g_unSSoeSaveFlashPtr,g_gSCosBuff[0],11);
   g_unSSoeSaveE2ROMPtr &= 0x3ff0;
   CAT_SpiWriteBytes(g_unSSoeSaveE2ROMPtr,11,g_gSCosBuff[0]); 
   g_unSSoeSaveE2ROMPtr+=16;
   soe_num++;  
   if(soe_num>512)soe_num=512;
   if(g_unSSoeSaveE2ROMPtr>=  EEPADD_SOEENDADR) //д��FLASH����0��д
   	{
   	g_unSSoeSaveE2ROMPtr = EEPADD_SOESTARTADR;
	soe_num=512;
   	}
   
  return;
}


//==============================================================================
//  ��������   : ReadSoe
//  ��������   : ��ȡһ��SEG��SOE
//  �������   : pBuf,װ��ȡ����SOE���ݣ�
//                             iSegNo,����id��0��SOE_SEG-1
//                            iStartNo,��ʼ��� ,0��SOE_NUM_SEG-1
//                             iSoeNum,��Ҫ��ȡ��SOE����
//  �������   : <��>
//  ����ֵ     : ʵ�ʶ�����SOE����
//  ����˵��   : 
//  ����       :
//==============================================================================

//�ŏ| SOE��FLASH
int ReadSoe(unsigned char *pBuf,int iSegNo,int iStartNo,int iSoeNum)
{
  //unsigned char *pAddr;
  int num_cnt = 0,RealNum=0;
  unsigned int j,k,n;
  unsigned char untemp[10];
  WORD YXNo = 0;
    //pAddr = (unsigned char *)(SOE_RECORD_ADD + (iSegNo*512) + 511);    //////����Ƿ�������д��
     
    	for(j=iStartNo;j<=iSoeNum;j++) 
      {
      	//soe_num
      	RealNum =(j-1)*16+1+EEPADD_SOESTARTADR;
      	if(RealNum>=EEPADD_SOEENDADR)
      		RealNum=RealNum-EEPADD_SOESTARTADR;
	CAT_SpiReadBytes(RealNum,10,untemp);
	
	YXNo = MAKEWORD(untemp[0],untemp[1]);
		for(k = 0; k < g_ucYxTransNum;k++)
               {
                   if((g_ucYXAddr[k]+g_gRunPara[RP_SYX_INFADDR]-2)==YXNo)
                   {
                       YXNo =k+g_gRunPara[RP_SYX_INFADDR];
			  untemp[0]=(unsigned char)(YXNo);
			untemp[1]=(unsigned char)(YXNo>>8);
			for(n = 0; n < 10;n++)
			{
				*pBuf=untemp[n];
				pBuf++;
			}
      			//pBuf+=10;
			num_cnt++;		   
                       break;					   
                   }
               }
		
    	}
   
  return num_cnt;
}
//�ŏ| SOE��FLASH



//==============================================================================
//  ��������   : ReadSoe
//  ��������   : ��ȡһ��SEG��SOE
//  �������   : pBuf,װ��ȡ����SOE���ݣ�
//                             iSegNo,����id��0��SOE_SEG-1
//                            iStartNo,��ʼ��� ,0��SOE_NUM_SEG-1
//                             iSoeNum,��Ҫ��ȡ��SOE����
//  �������   : <��>
//  ����ֵ     : ʵ�ʶ�����SOE����
//  ����˵��   : 
//  ����       :
//==============================================================================

int ReadSoeHistory(unsigned char *pBuf,int iSegNo,int iStartNo,int iSoeNum)
{
  unsigned char *pAddr;
  unsigned char ucTemp[10];
  int num_cnt = 0,RealNum=0;
  int j = 0;
    pAddr = (unsigned char *)(SOE_RECORD_ADD + (iSegNo*512) + 511);    //////����Ƿ�������д��
    if(*pAddr == 0x55) // ��������д�룬����
    {
      pAddr = (unsigned char *)(SOE_RECORD_ADD + iSegNo*512 + iStartNo*(SOE_DATA_LEN)); 
      for(num_cnt=0;num_cnt<SOE_NUM_SEG;num_cnt++)
      {
        //����һ�������Ƿ���������
        if(*pAddr==0x55)     //valid data
        {
          pAddr++; 
          for(j=0;j<SOE_SENDDA_LEN;j++) 
          {
            ucTemp[j] = *pAddr++;
          }
          if((ucTemp[7] == g_ChaxunSOE_STATTime[4]  && ucTemp[8] == g_ChaxunSOE_STATTime[5])
             || (ucTemp[7] > g_ChaxunSOE_STATTime[4]  && ucTemp[8] <= g_ChaxunSOE_OVERTime[5])
             || (ucTemp[7] < g_ChaxunSOE_OVERTime[4]  && ucTemp[8] <= g_ChaxunSOE_OVERTime[5])
             || (ucTemp[9] > g_ChaxunSOE_STATTime[6]  && ucTemp[9] <= g_ChaxunSOE_OVERTime[6]))
          {
              for(j=0;j<SOE_SENDDA_LEN;j++) 
              {
                *pBuf++ = ucTemp[j];
              }
              RealNum++;
          }
          if(RealNum == iSoeNum)
            break;
          //pAddr++;//ÿ��SOE��β��һ���ֽ�
        }
        else
          break;
       }
   }
  return RealNum;
}
//�����豸ʵң����ŵõ���Ҫ�����ң�����
//������Ϊ0xffff����ʾ��ң�Ų����ϴ������
unsigned int RealYxToTsYx(unsigned int RYxNo)
{
  unsigned int TsYxNo = 0xffff;
    for(int m = 0; m < g_ucYxTransNum; m++) //�жϸ�ң�ű�λ�Ƿ���Ҫ�ϴ�
    {
        if(RYxNo == (g_ucYXAddr[m] - 1))    //����Ҫ�ϴ���ң��
        {
            TsYxNo = m; //SOE����(0-4095
            break;
        }
    }
   return TsYxNo;
}

//==============================================================================
//  ��������   : 
//  ��������   : �õ�IEC101�µ����õ���ң����Ϣ
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       
//==============================================================================
void SetYxTrans(void)
{
  unsigned int i;//, RIIndex
 // int sYxCount = 0;
  //int dYxCount = 0;

  if((g_ucRefYxYcTs & BIT0) == BIT0)//��ң�Ÿ������õ�ң�ŵ�ַ��������
    {
      g_ucRefYxYcTs &= NBIT0;
      for(i = 0; i < g_ucYxTransNum; i++)
      {
    /*    RIIndex = (g_ucYXAddr[i] - 1);
        if(RIIndex < DYX_ADDR)
          SetRIStatus(sYxCount++,GetRIStatus(RIIndex),1);
       else if(RIIndex == 0xfffe)
          SetRIStatus(sYxCount++,0,1);
       else if(RIIndex == 0xfffd)
          SetRIStatus(sYxCount++,1,1);
        else
          SetRIStatus(dYxCount++,GetRIStatus(RIIndex),2);*/
      }
                             
    }               
                                              
}

//==============================================================================
//  ��������   : 
//  ��������   : �õ�IEC101�µ����õ���ң����Ϣ
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       
//==============================================================================
void SetYcTrans(void)
{
    unsigned char i;
    if((g_ucRefYxYcTs & BIT1) == BIT1)
    {
      g_ucRefYxYcTs &= NBIT1;
      for(i = 0; i < g_ucYcTransNum; i++)   //������Ҫ�ϴ���ң��ֵ�����վ���������л���
       {
      if(g_ucYCAddr[i]<255)
      {
          g_unYcTrans[i] = g_gRmtMeas[g_ucYCAddr[i] - 1];
      }
      else
        g_unYcTrans[i] = 0;
       }
     //  if(g_FltChgSdYc)
    //  {
   //     if(g_SendYc && (g_gRunPara[RP_CFG_KEY] & BIT[RPCFG_SEND_FTYC]))
   //       g_sTimer[TM_SENDYC].m_TmFlag = ON;//����ң�⴫��
      //  g_FltChgSdYc = 0;
    //  }
    }                                         
}



//==============================================================================
//  ��������   : RstRMTMeas
//  ��������   : ��λң��ֵ
//  �������   : 
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       
//==============================================================================
void RstRMTMeas()
{
    unsigned int i;

    for(i = 0; i < IEC_YC_NUM/*RMT_MEAS_NUM*/; i++)
    {
      g_gRmtMeas[i] = 0;
    }   
  
}



//==============================================================================
//  ��������   : RstRMTInfo
//  ��������   : ��λң��λ
//  �������   : 
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       
//==============================================================================
void RstRMTInfo(unsigned int SoeType)
{
    unsigned int i;
    for(i = 0; i < RMT_INFO_NUM; i++)
    {
        g_gRmtInfo[i] = 0;
		g_gRmtInfoBak[i]=0;
    }
}

//==============================================================================
//  ��������   : CopySoeToFlash
//  ��������   : �ڴ��е�SOE���ݵ����ⲿFLASH
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
/*void CopySoeToFlash(void)
{
     int SaveNum = 0;
      unsigned int BfT;
      unsigned long FSoeAddrOfset;//����SOE�ĵ�ַƫ����
      
      
      //���浥��SOE
      if(g_unSSoeSaveFlashPtr == g_unSCosBuffTail) return;

      BfT  = g_unSCosBuffTail;
      if(g_unSSoeSaveFlashPtr <= BfT) 
        SaveNum = BfT - g_unSSoeSaveFlashPtr;
      else                            
        SaveNum = SOE_BUFF_NUM - g_unSSoeSaveFlashPtr + BfT; 
        
      if(SaveNum == 0) return;

      
      //��soe���浽flash
      for(int i = 0; i < SaveNum;i++)
      {
        //�ж�Ҫ��Ҫ�߳�
        FSoeAddrOfset = g_unSSoeInFlashTail * FSOE_LEN;
        if( !(FSoeAddrOfset % 4096) )//ÿ���߳���Ҫ��4k�ռ�,256��SOE
        {
          Sector_Erase(FADDR_SOE_START + FSoeAddrOfset);
        }
        Sst26vf064b_Page_WriteBytes(FADDR_SOE_START + FSoeAddrOfset, g_gSCosBuff[g_unSSoeSaveFlashPtr],SOE_DATA_LEN); //�����ݱ������FLASH��
         
        g_unSSoeSaveFlashPtr++;
        g_unSSoeSaveFlashPtr &= (SOE_BUFF_NUM-1);

        g_unSSoeInFlashTail++;
        g_unSSoeInFlashTail &= (FSOE_TOLNUM-1);
        
        //=========�ж�ͷָ����βָ��λ��================
        //���߳���ʱ���ƶ�ͷָ�룬��SOE��¼ʱ�жϱ�������ݼ���
        if(pDbg != null)
        {
          if(pDbg->m_SSoeHeadPtr == g_unSSoeInFlashTail)
          {
            pDbg->m_SSoeHeadPtr ++;
            pDbg->m_SSoeHeadPtr &= (FSOE_TOLNUM - 1);
            CAT_SpiWriteWords(EEPADD_SSOEHEAD1, 1, &(pDbg->m_SSoeHeadPtr));
          }
         }

        if(pGprs != null)
        {
          if(pGprs->m_SSoeHeadPtr == g_unSSoeInFlashTail) 
          {
            pGprs->m_SSoeHeadPtr++;
            pGprs->m_SSoeHeadPtr &= (FSOE_TOLNUM - 1);
            CAT_SpiWriteWords(EEPADD_SSOEHEAD2, 1, &(pGprs->m_SSoeHeadPtr));
          }
         }
      }
      //��E2�б���flash�е�SOEβָ��
      CAT_SpiWriteWords(EEPADD_SSOETAIL, 1, &g_unSSoeInFlashTail);
    return;




}
*/
//==============================================================================
//  ��������   : GetRIStatus
//  ��������   : ��ȡң����Ϣ�еĵ���״̬��˫��״̬
//  �������   : RIIndex:ң����Ϣ���е�ȫ�������ţ������constant.h��RMT_INFO����
//             : RmtInfoFlag 0 =g_gRmtInfo��1=g_unYxTrans��2=g_unDYxTrans
//  �������   ��<��>
//  ����ֵ     : 1��0��2,3
//  ����˵��   : ÿ��ң����Ϣռ����bitλ
//  ����       ��
//==============================================================================

unsigned int GetRIStatus( unsigned int RIIndex, BYTE RmtInfoFlag)
{
    unsigned int iRtStVal = 0;
    unsigned int iStVal = 0;
    unsigned int *pRmtInfo = null;
    //unsigned long *pRmtInfolong = null; 

    if(RmtInfoFlag == 1)       pRmtInfo = g_unYxTrans;
    else if(RmtInfoFlag == 2)  pRmtInfo = g_unYxTrans;//g_unDYxTrans;
    else          return  g_gRmtInfo[RIIndex];

    iStVal = 3 << ((RIIndex & 7)*2);
    iRtStVal = pRmtInfo[(RIIndex >> 3)] & iStVal;//ȡ��Ҫ�жϵ�λ 
    iRtStVal = iRtStVal >> ((RIIndex & 7)*2);//��Ҫ�жϵ�λ���������λ
    return iRtStVal;	
}
//==============================================================================
//  ��������   : SetRIStatus
//  ��������   : ����ң����Ϣ�еĵ���״̬������ʵ��д��1����0,2,3
//  �������   : RIIndex:ң����Ϣ���е�ȫ�������ţ������constant.h��RMT_INFO����
//                         : Status: д��״̬��1����0,2,3
//             : RmtInfoFlag 0 =g_gRmtInfo��1=g_unYxTrans��2=g_unDYxTrans
//  �������   : <��>
//  ����ֵ     : 
//  ����˵��   : ÿ��ң����Ϣռ����bitλ
//  ����       ��
//==============================================================================

void SetRIStatus( unsigned int RIIndex , unsigned int Status, BYTE RmtInfoFlag)
{
   unsigned int iStVal = 0;
   unsigned int *pRmtInfo = null;
    //unsigned long *pRmtInfolong = null; 
	
    if(RmtInfoFlag == 1)       pRmtInfo = g_unYxTrans;
    else if(RmtInfoFlag == 2)  pRmtInfo = g_unYxTrans;//g_unDYxTrans;
    else pRmtInfo = g_gRmtInfo;

   iStVal = 3 << ((RIIndex & 7)*2);
   pRmtInfo[(RIIndex >> 3)] &= ~iStVal;//���Ҫ���õ�λ
   pRmtInfo[(RIIndex >> 3)] |=  ((Status & 3) << ((RIIndex & 7)*2));//����״̬д��Ҫ���õ�λ
   
}

//Flag = 1 ��ʾɨ��Ӳң�ų��������� 2 ��ʾң�ŶԵ������SOE(�����ϳ�)
void CreatNewSoe(unsigned int SoeType,unsigned long SoeValue,unsigned char Flag)
{//SoeType:ʵң��
    unsigned int TsSoeType;//,TsSoeDNum,TsSoeNum;
    //unsigned int iDinID = 0;

      
      if(pGprs != null) SETBIT(pGprs->m_gRmtChgFlag,SoeType,1);
      if(pDbg != null)  SETBIT(pDbg->m_gRmtChgFlag,SoeType,1);
      //����SOE֮ǰ�����ֵ����˫�㣬��Ϊ�����soe���Ͳ���ң�ŵ���Ϣ���ַ
      //������Э���д���soe�Ͳ����ٷֱ�����Ϣ���ַ

      TsSoeType = g_gRunPara[RP_SYX_INFADDR] + SoeType - 1;   //- 1 + TsSoeType - g_ucSYxTrsStartId
        
 
      g_yxChangeflag = 0xffff;   
     // g_ucRefYxYcTs |= BIT0;
      //int iDinID = SoeType;
      unsigned int *pchSoeTm;
      unsigned char *pchSoeBuf;
      
      if((int)SoeType >= 0 && SoeType < RMT_INFO_NUM + 2)  //ע������2·Ӳң��
      {
         
          pchSoeTm = g_sRtcManager.m_gRealTimer;
          pchSoeBuf = g_gSCosBuff[0];//�ŏ| SOE��FLASH
          pchSoeBuf[SOE_FLAG] = SOEVALID;
          pchSoeBuf[SOE_TYPEL] = TsSoeType; //SOE����(0-4095
          pchSoeBuf[SOE_TYPEH] = TsSoeType >> 8;
          pchSoeBuf[SOE_STVAL] = SoeValue;
          pchSoeBuf[SOE_MSL] = (pchSoeTm[RTC_MICROSEC]+pchSoeTm[RTC_SEC]*1000)&0XFF;
          pchSoeBuf[SOE_MSH] = (pchSoeTm[RTC_MICROSEC]+pchSoeTm[RTC_SEC]*1000)>>8;
          pchSoeBuf[SOE_HOUR] = pchSoeTm[RTC_HOUR];
          pchSoeBuf[SOE_MINU] = pchSoeTm[RTC_MINUT];
          pchSoeBuf[SOE_DAY] =  pchSoeTm[RTC_DATE];
          pchSoeBuf[SOE_MONTH] = pchSoeTm[RTC_MONTH];
          pchSoeBuf[SOE_YEAR] = pchSoeTm[RTC_YEAR]-2000; 
       
      }
    //����soeβָ��
    		SaveSoeData();//��1�����ŏ| SOE��FLASH
    		/*
		if(pDbg != null)
              {
              	pDbg->m_SSoeHeadPtr=g_unSSoeSaveFlashHead;                
              }
    
              if(pGprs != null)
              {             
                	pGprs->m_SSoeHeadPtr=g_unSSoeSaveFlashHead;           
              }*/
		unsigned int untemp[4];
		g_unSSoeSaveE2ROMPtr &= 0xfff0;
		untemp[0]=g_unSSoeSaveE2ROMPtr;
		untemp[1]=g_unSSoeSaveE2ROMPtr;
		untemp[2]=soe_num;
		untemp[3]=soe_num;			
             	CAT_SpiWriteWords(EEPADD_SOE_E2ROMADR, 4, untemp);  //���浽EEPROM��
        	CAT_SpiWriteWords(EEPADDBK_SOE_E2ROMADR, 4, untemp);    //���浽EEPROM�ı�������

}
//==============================================================================
//  ��������   : DelALLSOE
//  ��������   : ɾ������SOE
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void DelALLSOE(void)
{
		unsigned int untemp[4];

		untemp[0]=EEPADD_SOESTARTADR;//(unsigned int)g_unSSoeSaveFlashPtr;
		untemp[1]=EEPADD_SOESTARTADR;//(unsigned int)(g_unSSoeSaveFlashPtr>>16);
		untemp[2]=0;untemp[3]=0;
             	CAT_SpiWriteWords(EEPADD_SOE_E2ROMADR,4, untemp);  //���浽EEPROM��
        	CAT_SpiWriteWords(EEPADDBK_SOE_E2ROMADR, 4, untemp);    //���浽EEPROM�ı�������
        	g_unSSoeSaveE2ROMPtr=EEPADD_SOESTARTADR;soe_num =0;			
		if(pDbg != null)
			pDbg->m_SSoeHeadPtr=EEPADD_SOESTARTADR;
		if(pGprs!= null)
			pGprs->m_SSoeHeadPtr=EEPADD_SOESTARTADR;
		untemp[2]=EEPADD_SOESTARTADR;//(unsigned int)g_unSSoeSaveFlashPtr;
		untemp[3]=EEPADD_SOESTARTADR;//(unsigned int)(g_unSSoeSaveFlashPtr>>16);		
		CAT_SpiWriteWords(EEPADD_SOESEND_E2ROMADR,4, untemp);
}

void DelALLLOG(void)
{
		unsigned int temp[4];

		temp[FLOG_TOTALNUM]=0;
		temp[FLOG_NEW]=0;
		temp[FLOG_OLD]=0;
		temp[FLOG_CS]=0;
		log_recorded.log_MemNewPtr=0;
		log_recorded.log_MemPtr=0;
		log_recorded.log_Flash_count=temp[FLOG_TOTALNUM];
		log_recorded.log_FlashNewPtr=temp[FLOG_NEW]+FADDR_LOG_START;
		log_recorded.log_FlashPtr=temp[FLOG_OLD]+FADDR_LOG_START;		
    	CAT_SpiWriteWords(EEPADD_LOGP , FLOGINFONUM, temp);		
		CAT_SpiWriteWords(EEPADDBK_LOGP, FLOGINFONUM,temp);
		Sector_Erase(log_recorded.log_FlashNewPtr);
}

//==============================================================================
//  ��������   : CheckRECPara
//  ��������   : ��EEPROM�ж�ȡ¼��������¼�����ڵ�ǰҳ
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ��
//==============================================================================
void RECParaPro(void)
{

}


//==============================================================================
//  ��������   : SaveRecData
//  ��������   : ����COMTRADE��ʽ�������¼�����ݣ��������α��浽Flash��
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   :Sst26vf064��126��BLOCK��ÿ��BLOCK��64kByte 
//                  FADDR_RECORDER_DATA ��ʼ��ַ 
//                  ÿ��¼��640����¼��ÿ��5��ң��ֵ��ÿ��ң��ֵ2�ֽ� 
//                  ��6400�ֽ� ռ2��SECTOR��8192(0x2000)�ֽ�
//                  �洢50����¼
//  ����       : �ŏ|
//==============================================================================

void SaveRecData(void)
{
  unsigned char *PTemp;   //��ʱ��������
  unsigned int j,ulen,uRect,temp[4],unum;//
  unsigned long ulAddr;

  
  //���ڹ���¼����640�㣬ÿ��5��ң��ֵ��ÿ��ң��ֵ2�ֽ�
  //��640*5*2=6400�ֽ�
  //Sst26vf064ÿpage��256�ֽڣ���25ҳ  
  //ulAddr = (unsigned long)(FADDR_RECORDER_DATA+g_sRecData.m_gRecCNum*4)<<16;//flash��ַ
  ulAddr = FADDR_RECORDER_DATA+(unsigned long)(g_sRecData.m_gRecCNum)*0x2000;//flash��ַ
  uRect = g_sRecData.m_unRecAcTail;//m_gRecAc�������ְ//1�����д256�ֽ�
  PTemp = (unsigned char *)&(g_sRecData.m_gRecAc[uRect][0]);
  //PTemp = (unsigned char *)&(g_sRecData.m_gRecAc[0][0]);
  unum = uRect*10;
  for(j=0;j<6400;)//���ڹ���
  {
    if((unum+256)<6400)
    {
         ulen = 256;	  
	  Sst26vf064b_Page_WriteBytes(ulAddr,PTemp,ulen);
	  PTemp+=ulen;
	  unum+=256;
	  ulAddr += 256;
    }
   else
    {
      	ulen = 6400-unum;	
	Sst26vf064b_Page_WriteBytes(ulAddr,PTemp,ulen);
	ulAddr +=ulen;
	PTemp = (unsigned char *)&(g_sRecData.m_gRecAc[0][0]);
	ulen = 256-ulen;
	if(ulen>0)
	{
		Sst26vf064b_Page_WriteBytes(ulAddr,PTemp,ulen);
		ulAddr += ulen;
		PTemp+=ulen;
		unum = 0;
	}
     }
/*  */   
    
    j += 256;
	
  }

//�ŏ| ¼�� ��Ҫд��CFG�ļ�
  
//�ŏ| ¼�� ��Ҫд�ļ�Ŀ¼  
  temp[1]=g_sRecData.m_gRecCNum;
  gRecorder_filecfg.FileName=temp[1];//��Ϊ�ļ���
  gRecorder_filecfg.CFG_Leng=330;//strlen(ComtrderCfg1);//
  gRecorder_filecfg.TOTAL_Leng=64;//gRecorder_filecfg.CFG_Leng+6400;//
  gRecorder_filecfg.CFG_Samp=4000;	//����Ƶ�� 
#ifdef SD_101S
  gRecorder_filecfg.CFG_Samp=2000;	//����Ƶ�� 
#endif
  gRecorder_filecfg.CFG_EndSamp=640;//�������� 
  gRecorder_filecfg.TOTAL_Leng=gRecorder_filecfg.CFG_EndSamp;//gRecorder_filecfg.CFG_Leng+6400;//  
  gRecorder_filecfg.comtrade_time[RTC_MICROSEC]=g_sRecData.m_gFaultRecOver[REC_MSL] ;
  gRecorder_filecfg.comtrade_time[RTC_SEC]=g_sRecData.m_gFaultRecOver[REC_MSH];
  gRecorder_filecfg.comtrade_time[RTC_MINUT]=g_sRecData.m_gFaultRecOver[REC_MINU] ;
  gRecorder_filecfg.comtrade_time[RTC_HOUR]=g_sRecData.m_gFaultRecOver[REC_HOUR] ;
  gRecorder_filecfg.comtrade_time[RTC_DATE]=g_sRecData.m_gFaultRecOver[REC_DAY] ;
  gRecorder_filecfg.comtrade_time[RTC_MONTH]=g_sRecData.m_gFaultRecOver[REC_MONTH];
  gRecorder_filecfg.comtrade_time[RTC_YEAR]=g_sRecData.m_gFaultRecOver[REC_YEAR]+2000; 
  ulAddr =FADDR_RECORDER_DATA+(unsigned long)(g_sRecData.m_gRecCNum+1)*0x2000-256;	
  Sst26vf064b_Page_WriteBytes(ulAddr,(unsigned char *)&gRecorder_filecfg,sizeof(gRecorder_filecfg)); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ����� 

//��ǰ������2��ʹ�õ�SECTOR 
if(pDbg != null)pDbg->m_luok_filename=g_sRecData.m_gRecCNum;
if(pGprs != null)pGprs->m_luok_filename=g_sRecData.m_gRecCNum;
  g_sRecData.m_gRecCNum++;
  if(g_sRecData.m_gRecCNum>MAX_REC_NUM)g_sRecData.m_gRecCNum=0;
  //ulAddr = (unsigned long)(FADDR_RECORDER_DATA+g_sRecData.m_gRecCNum*4)<<16;//flash��ַ 
  ulAddr = FADDR_RECORDER_DATA+(unsigned long)(g_sRecData.m_gRecCNum)*0x2000;//flash��ַ
  //g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ  
  Sector_Erase(ulAddr);//ERASE 1��BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x1000);//ERASE 1��BLOCK 
  delayms(100);WatchDog();

// д��EEPROM,��¼�������ͽ�д��ĵ�ַ
  g_sRecData.m_gRecANum++;
  if(g_sRecData.m_gRecANum>MAX_REC_NUM)g_sRecData.m_gRecANum=MAX_REC_NUM;
  temp[0]=g_sRecData.m_gRecANum;//¼��������1~32
  temp[1]=g_sRecData.m_gRecCNum;//¼����ǰλ��0~31
  CAT_SpiWriteWords(EEPADD_LUBONUM, 2, temp);   
//��ǰ������1��ʹ�õ�SECTOR,�ļ�Ŀ¼��    
  //ulAddr =FADDR_RECORDER_INFO+ (long)temp[1]*(long)FLINEADDR;  
  //Sector_Erase(ulAddr);//Ԥ�Ȳ�����һ��sector
  //g_gRmtInfo[YX_LBOK]=1;//¼�����ң����λ
  if(pDbg != null) pDbg->m_luok=1;
  if(pGprs != null) pGprs->m_luok=1;  
  //g_YXLBOKCounter=60;//¼�����ң�ų���60s

}
//==============================================================================
//  ��������   : SaveActRecData
//  ��������   : ������¼�����ݣ��������α��浽Flash��
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   :Sst26vf064��126��BLOCK��ÿ��BLOCK��64kByte 
//              0x300000~0x34FFFF
//              0x350000~0x39FFFF
//              ������������
//              
//              0x2F0000~0x2FFFFF  ÿ5��BLOCK��һ�ζ���¼��
//  ����       : �ŏ|
//==============================================================================

void SaveActRecData(void)
{
  unsigned char *PTemp;   //��ʱ��������
  unsigned int j,ulen,pagelen;//,unum=0;//
  unsigned long ulAddr;
  unsigned int temp[8];
  static unsigned int save_page_num=0;

  if(g_sRecData.m_ucActRecSavingFlag == OFF)
  	return;
  //���ڶ���¼����ÿ��5��ң��ֵ��ÿ��ң��ֵ2�ֽ�
  //���������25��
  //ÿ���ܲ��ɼ�16��
  //g_sRecData.m_gRecAc ���Դ�640�㣬��6400�ֽ�
  //��2�δ棬�ȴ�12x256=3072�ֽڣ��ٴ�13x256=3328
  PTemp = (unsigned char *)&(g_sRecData.m_gRecAc[0][0]);//��ǰ320��
  if(g_sRecData.m_ucActRecSavingFlag == YES)
  {
  	pagelen=12;
  }
  if(g_sRecData.m_ucActRecSavingFlag == ON)
  {
  	PTemp += 3072;
	pagelen=13;
  }
  g_sRecData.m_ucActRecSavingFlag = OFF;  
  WatchDog();
  for(j=0;j<pagelen;j++)//����
  { 
         ulen = 256;	  
	  Sst26vf064b_Page_WriteBytes(g_sRecData.m_gActRecAdr,PTemp,ulen);
	  PTemp+=256;          
	 g_sRecData.m_gActRecAdr += 256;//m_gActRecAdr ��¼��FLASH�еĵ�ַ
  }
 WatchDog();
 save_page_num = save_page_num+pagelen;
 if(g_sRecData.m_LuboType == LuboType_XH)
 	{
 	if(save_page_num>125)//Ϩ��¼���洢�ռ����Ϊ32kB����128page
 		g_sRecData.m_ucActRecStart = OFF;
 	}
else if(g_sRecData.m_LuboType == LuboType_ACT)	
	{
 	if(save_page_num>2300)//����¼���洢�ռ����Ϊ9*64kB����2304page
 		g_sRecData.m_ucActRecStart = OFF;	
	}

if((g_sRecData.m_ucActRecStart == OFF))
{
	save_page_num =0;
	g_sRecData.m_ucActRecStart = NO;
//�ŏ| ¼�� ��Ҫд��CFG�ļ�  
//�ŏ| ¼�� ��Ҫд�ļ�Ŀ¼  
  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	temp[1]=g_sRecData.m_gXHRecCNum;
	gRecorder_filecfg.FileName=temp[1]+MAX_REC_NUM+MAX_ACTREC_NUM+2;//��Ϊ�ļ���
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{
  	temp[1]=g_sRecData.m_gACTRecCNum;
	gRecorder_filecfg.FileName=temp[1]+MAX_REC_NUM+1;//��Ϊ�ļ���
	}  
  gRecorder_filecfg.CFG_Leng=330;//strlen(ComtrderCfg1);//
  gRecorder_filecfg.CFG_Samp=800;	//����Ƶ�� 
#ifdef SD_101S
  gRecorder_filecfg.CFG_Samp=800;	//����Ƶ�� 
#endif  
  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;//flash��ַ  g_sRecData.m_gActRecAdr/10
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{  	
    	ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;//flash��ַ  g_sRecData.m_gActRecAdr/10
  	}
  gRecorder_filecfg.CFG_EndSamp=(g_sRecData.m_gActRecAdr-ulAddr)/10;//�������� 
  gRecorder_filecfg.TOTAL_Leng=gRecorder_filecfg.CFG_EndSamp;//gRecorder_filecfg.CFG_Leng+6400;//
  gRecorder_filecfg.comtrade_time[RTC_MICROSEC]=g_sRecData.m_gFaultRecSOE[REC_MSL] ;
  gRecorder_filecfg.comtrade_time[RTC_SEC]=g_sRecData.m_gFaultRecSOE[REC_MSH];
  gRecorder_filecfg.comtrade_time[RTC_MINUT]=g_sRecData.m_gFaultRecSOE[REC_MINU] ;
  gRecorder_filecfg.comtrade_time[RTC_HOUR]=g_sRecData.m_gFaultRecSOE[REC_HOUR] ;
  gRecorder_filecfg.comtrade_time[RTC_DATE]=g_sRecData.m_gFaultRecSOE[REC_DAY] ;
  gRecorder_filecfg.comtrade_time[RTC_MONTH]=g_sRecData.m_gFaultRecSOE[REC_MONTH];
  gRecorder_filecfg.comtrade_time[RTC_YEAR]=g_sRecData.m_gFaultRecSOE[REC_YEAR]+2000; 

   gRecorder_filecfg.comtrade_time1[RTC_MICROSEC]=g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
   gRecorder_filecfg.comtrade_time1[RTC_SEC] = g_sRtcManager.m_gRealTimer[RTC_SEC];
   gRecorder_filecfg.comtrade_time1[RTC_MINUT] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
   gRecorder_filecfg.comtrade_time1[RTC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
   gRecorder_filecfg.comtrade_time1[RTC_DATE] = g_sRtcManager.m_gRealTimer[RTC_DATE];
   gRecorder_filecfg.comtrade_time1[RTC_MONTH]= g_sRtcManager.m_gRealTimer[RTC_MONTH];
  gRecorder_filecfg.comtrade_time1[RTC_YEAR] = g_sRtcManager.m_gRealTimer[RTC_YEAR] ;

  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	ulAddr = FADDR_RECORDER_XHDATA-256+(unsigned long)(g_sRecData.m_gXHRecCNum+1)*0x8000;
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{  	
    	ulAddr = FADDR_RECORDER_ACTDATA-256+(unsigned long)(g_sRecData.m_gACTRecCNum+1)*0x90000;
  	}   
  Sst26vf064b_Page_WriteBytes(ulAddr,(unsigned char *)&gRecorder_filecfg,sizeof(gRecorder_filecfg)); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ����� 

//��ǰ������9��ʹ�õ�BLOCK 
  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	if(pDbg != null)pDbg->m_luok_filename=g_sRecData.m_gXHRecCNum;
	if(pGprs != null)pGprs->m_luok_filename=g_sRecData.m_gXHRecCNum;
  	g_sRecData.m_gXHRecCNum++;
 	 if(g_sRecData.m_gXHRecCNum>MAX_XHREC_NUM)g_sRecData.m_gXHRecCNum=0;
  	ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;  
  	g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ  
  	g_sRecData.m_EraseBlock = YES;
// д��EEPROM,��¼�������ͽ�д��ĵ�ַ
  	g_sRecData.m_gXHRecANum++;
  	if(g_sRecData.m_gXHRecANum>MAX_XHREC_NUM)g_sRecData.m_gXHRecANum=MAX_XHREC_NUM;
  	temp[0]=g_sRecData.m_gXHRecANum;//¼��������1~32
  	temp[1]=g_sRecData.m_gXHRecCNum;//¼����ǰλ��0~31
  	CAT_SpiWriteWords((EEPADD_LUBONUM+8), 2, temp); 
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{  
  	if(pDbg != null)pDbg->m_luok_filename=g_sRecData.m_gACTRecCNum;
	if(pGprs != null)pGprs->m_luok_filename=g_sRecData.m_gACTRecCNum;
  	g_sRecData.m_gACTRecCNum++;
 	 if(g_sRecData.m_gACTRecCNum>MAX_ACTREC_NUM)g_sRecData.m_gACTRecCNum=0;
  	ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;  
  	g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ  
  	g_sRecData.m_EraseBlock = ON;
// д��EEPROM,��¼�������ͽ�д��ĵ�ַ
  	g_sRecData.m_gACTRecANum++;
  	if(g_sRecData.m_gACTRecANum>MAX_ACTREC_NUM)g_sRecData.m_gACTRecANum=MAX_ACTREC_NUM;
  	temp[0]=g_sRecData.m_gACTRecANum;//¼��������1~32
  	temp[1]=g_sRecData.m_gACTRecCNum;//¼����ǰλ��0~31
  	CAT_SpiWriteWords((EEPADD_LUBONUM+4), 2, temp); 
  	}
//��ǰ������1��ʹ�õ�SECTOR,�ļ�Ŀ¼��    
  //ulAddr =FADDR_RECORDER_INFO+ (long)temp[1]*(long)FLINEADDR;  
  //Sector_Erase(ulAddr);//Ԥ�Ȳ�����һ��sector
    	
  g_gRmtInfo[YX_LBOK]=1;//¼�����ң����λ
  if(pDbg != null) pDbg->m_luok=1;
  if(pGprs != null) pGprs->m_luok=1; 
  g_YXLBOKCounter=1;//¼�����ң�ų���60s
  g_sRecData.m_ucActRecStart = CLOSE;
  
}
 
}

void SaveActRecDataCFG(void)
{
 unsigned int temp[8];
 unsigned long ulAddr;
if((g_sRecData.m_ucActRecStart == OFF))
{
	//save_page_num =0;
//�ŏ| ¼�� ��Ҫд��CFG�ļ�  
//�ŏ| ¼�� ��Ҫд�ļ�Ŀ¼  
  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	temp[1]=g_sRecData.m_gXHRecCNum;
	gRecorder_filecfg.FileName=temp[1]+MAX_REC_NUM+MAX_ACTREC_NUM+2;//��Ϊ�ļ���
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{
  	temp[1]=g_sRecData.m_gACTRecCNum;
	gRecorder_filecfg.FileName=temp[1]+MAX_REC_NUM+1;//��Ϊ�ļ���
	}  
  gRecorder_filecfg.CFG_Leng=330;//strlen(ComtrderCfg1);//
  gRecorder_filecfg.CFG_Samp=800;	//����Ƶ�� 
#ifdef SD_101S
  gRecorder_filecfg.CFG_Samp=800;	//����Ƶ�� 
#endif  
  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;//flash��ַ  g_sRecData.m_gActRecAdr/10
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{  	
    	ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;//flash��ַ  g_sRecData.m_gActRecAdr/10
  	}
  gRecorder_filecfg.CFG_EndSamp=(g_sRecData.m_gActRecAdr-ulAddr)/10;//�������� 
  gRecorder_filecfg.TOTAL_Leng=gRecorder_filecfg.CFG_EndSamp;//gRecorder_filecfg.CFG_Leng+6400;//
  gRecorder_filecfg.comtrade_time[RTC_MICROSEC]=g_sRecData.m_gFaultRecSOE[REC_MSL] ;
  gRecorder_filecfg.comtrade_time[RTC_SEC]=g_sRecData.m_gFaultRecSOE[REC_MSH];
  gRecorder_filecfg.comtrade_time[RTC_MINUT]=g_sRecData.m_gFaultRecSOE[REC_MINU] ;
  gRecorder_filecfg.comtrade_time[RTC_HOUR]=g_sRecData.m_gFaultRecSOE[REC_HOUR] ;
  gRecorder_filecfg.comtrade_time[RTC_DATE]=g_sRecData.m_gFaultRecSOE[REC_DAY] ;
  gRecorder_filecfg.comtrade_time[RTC_MONTH]=g_sRecData.m_gFaultRecSOE[REC_MONTH];
  gRecorder_filecfg.comtrade_time[RTC_YEAR]=g_sRecData.m_gFaultRecSOE[REC_YEAR]+2000; 

   gRecorder_filecfg.comtrade_time1[RTC_MICROSEC]=g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
   gRecorder_filecfg.comtrade_time1[RTC_SEC] = g_sRtcManager.m_gRealTimer[RTC_SEC];
   gRecorder_filecfg.comtrade_time1[RTC_MINUT] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
   gRecorder_filecfg.comtrade_time1[RTC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
   gRecorder_filecfg.comtrade_time1[RTC_DATE] = g_sRtcManager.m_gRealTimer[RTC_DATE];
   gRecorder_filecfg.comtrade_time1[RTC_MONTH]= g_sRtcManager.m_gRealTimer[RTC_MONTH];
  gRecorder_filecfg.comtrade_time1[RTC_YEAR] = g_sRtcManager.m_gRealTimer[RTC_YEAR] ;

  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	ulAddr = FADDR_RECORDER_XHDATA-256+(unsigned long)(g_sRecData.m_gXHRecCNum+1)*0x8000;
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{  	
    	ulAddr = FADDR_RECORDER_ACTDATA-256+(unsigned long)(g_sRecData.m_gACTRecCNum+1)*0x90000;
  	}   
  Sst26vf064b_Page_WriteBytes(ulAddr,(unsigned char *)&gRecorder_filecfg,sizeof(gRecorder_filecfg)); //�������ﱣ��gRecorder_cfg��ֵ����Ϊ�����¼����һ�����ܴ����� 

//��ǰ������9��ʹ�õ�BLOCK 
  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	g_sRecData.m_gXHRecCNum++;
 	 if(g_sRecData.m_gXHRecCNum>MAX_XHREC_NUM)g_sRecData.m_gXHRecCNum=0;
  	ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;  
  	g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ  
  	g_sRecData.m_EraseBlock = YES;
// д��EEPROM,��¼�������ͽ�д��ĵ�ַ
  	g_sRecData.m_gXHRecANum++;
  	if(g_sRecData.m_gXHRecANum>MAX_XHREC_NUM)g_sRecData.m_gXHRecANum=MAX_XHREC_NUM;
  	temp[0]=g_sRecData.m_gXHRecANum;//¼��������1~32
  	temp[1]=g_sRecData.m_gXHRecCNum;//¼����ǰλ��0~31
  	CAT_SpiWriteWords((EEPADD_LUBONUM+8), 2, temp); 
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{   
  	g_sRecData.m_gACTRecCNum++;
 	 if(g_sRecData.m_gACTRecCNum>MAX_ACTREC_NUM)g_sRecData.m_gACTRecCNum=0;
  	ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;  
  	g_sRecData.m_gActRecAdr = ulAddr;//����flash��ַ  
  	g_sRecData.m_EraseBlock = ON;
// д��EEPROM,��¼�������ͽ�д��ĵ�ַ
  	g_sRecData.m_gACTRecANum++;
  	if(g_sRecData.m_gACTRecANum>MAX_ACTREC_NUM)g_sRecData.m_gACTRecANum=MAX_ACTREC_NUM;
  	temp[0]=g_sRecData.m_gACTRecANum;//¼��������1~32
  	temp[1]=g_sRecData.m_gACTRecCNum;//¼����ǰλ��0~31
  	CAT_SpiWriteWords((EEPADD_LUBONUM+4), 2, temp); 
  	}
//��ǰ������1��ʹ�õ�SECTOR,�ļ�Ŀ¼��    
  //ulAddr =FADDR_RECORDER_INFO+ (long)temp[1]*(long)FLINEADDR;  
  //Sector_Erase(ulAddr);//Ԥ�Ȳ�����һ��sector
    	
  g_gRmtInfo[YX_LBOK]=1;//¼�����ң����λ
  if(pDbg != null) pDbg->m_luok=1;
  if(pGprs != null) pGprs->m_luok=1; 
  g_YXLBOKCounter=1;//¼�����ң�ų���60s
  g_sRecData.m_ucActRecStart = CLOSE;
}
}
/*
void SaveRecData(void)
{
    unsigned long ulAddr;       //AT45DB��ַ����
    unsigned long ulTempAddr;   //��ʱ����Flashҳ��
  //  unsigned int i = 0;
    //unsigned int j;
   // unsigned int unAddCount;    //EEPROM�洢���ݼӺ�У��ֵ
    unsigned int *PTemp;   //��ʱ��������  
    unsigned int PunTempData[2];
    //unsigned int unTemp[6];
    static unsigned char ucSaveFlg[5];    //��ʱ�����ͨ���洢��־
    if((g_sRecData.m_CurWritePage - FADDR_REC_START) >= 16384)         //����1024��¼����¼12288
    g_sRecData.m_CurWritePage = FADDR_REC_START;
    //Ϊ��Ӧ�Կ��ܷ����Ķ�ʱ��β���Ӱ��¼�����ݣ����ݴ洢˳�����Ϊ�ȴ洢�Ϸ�բ��������
    ulTempAddr = ((g_sRecData.m_CurWritePage - FADDR_REC_START) % REC_LEAF_NUM);
    ulAddr = ((unsigned long)g_sRecData.m_CurWritePage)*256;  //�洢������Ե�ַ
    switch(ulTempAddr)
    {
    case 0x00:  
          //�洢������¼����SOE ����(SOE_DATA_LEN - 1)
          Sector_Erase(ulAddr);                          //�洢֮ǰ�Ȳ������飨4K��
          g_sRecData.m_gFaultRecSOE[REC_TOTAL_LENL] =  0xCA;   //ÿ��¼������10 + 224*5*2 = 2250  0x08CA
          g_sRecData.m_gFaultRecSOE[REC_TOTAL_LENH] =  0x08;
          
          if(g_sRecData.m_gRecNum % 1024 == 0)           // �洢512�����������
          {
              //g_sRecData.m_gRecNum = 0;
              g_sRecData.m_CurWritePage = FADDR_REC_START;
          }
          
          g_sRecData.m_gRecNum += 1;              //¼��������1
          PunTempData[0] = g_sRecData.m_gRecNum;
          PunTempData[1] = g_sRecData.m_CurWritePage;
          //CAT_SpiWriteWord(EEPADD_REC_NUM,g_sRecData.m_gRecNum);  //�洢¼������

         // CAT_SpiWriteWord(EEPADD_REC_POS,g_sRecData.m_CurWritePage); //�洢����¼���������ڵ�at45ҳ����EEPROM
          CAT_SpiWriteWords(EEPADD_REC_NUM,2,PunTempData);  //����¼������������¼���������ڵ�ַҳ��

        //  unAddCount = PunTempData[0] + PunTempData[1];   //���ݵļӺ�У��ֵ
        //  CAT_SpiWriteWord(EEPADD_REC_DATA,unAddCount);          //�洢���ݵļӺ�У��ֵ��EEPROM
          
          
          Sst26vf064b_Page_WriteBytes(ulAddr,g_sRecData.m_gFaultRecSOE,7);  // ռ10�ֽ�    
          Sst26vf064b_Page_WriteBytes(ulAddr + 7,g_sRecData.m_gFaultRecOver,7);
          ulAddr += 30;                 //Ԥ����15�ֿռ�  
          g_sRecData.m_CurWritePage++;                   //Flashҳ������
          break;
     case 0x01:
          PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[0][g_sRecData.m_unRecAcTail]);        //�洢���涯��ǰ10���ܲ�������6���ܲ��Ľ����źŲ���          
          if(g_sRecData.m_unRecAcTail > 112)  //ʣ�µ����ݲ���һҳ����ȫд��ȥ
          {
              ucSaveFlg[0] = 0xAA;
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(REC_AC_LEN-g_sRecData.m_unRecAcTail));   //�洢��һ��ͨ�� 
              ulAddr += ((REC_AC_LEN-g_sRecData.m_unRecAcTail)*2);
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[0][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 + g_sRecData.m_unRecAcTail - REC_AC_LEN));
         
          }
          else
          {
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //�洢��һ��ͨ�� 
              ucSaveFlg[0] = 0x55;
          }
          g_sRecData.m_CurWritePage++;                   //Flashҳ������
          break;
          
    case 0x02:
          if(ucSaveFlg[0] == 0xAA)  //�Ѿ�ת�ص�����ͷ
          {
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[0][112 + g_sRecData.m_unRecAcTail - REC_AC_LEN]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //�洢��һ��ͨ�� 
          }
          else if(ucSaveFlg[0] == 0x55)
          {
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[0][g_sRecData.m_unRecAcTail + 112]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 - g_sRecData.m_unRecAcTail));   //�洢��һ��ͨ��
              ulAddr += ((112 - g_sRecData.m_unRecAcTail)*2);
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[0][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(g_sRecData.m_unRecAcTail));              
          }
          ucSaveFlg[0] = 0;

          g_sRecData.m_CurWritePage++;                   //Flashҳ������
          break;
     case 0x03:
          PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[1][g_sRecData.m_unRecAcTail]);        //�洢���涯��ǰ10���ܲ�������6���ܲ��Ľ����źŲ���          
          if(g_sRecData.m_unRecAcTail > 112)  //ʣ�µ����ݲ���һҳ����ȫд��ȥ
          {
              ucSaveFlg[1] = 0xAA;
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(REC_AC_LEN-g_sRecData.m_unRecAcTail));   //�洢��һ��ͨ�� 
              ulAddr += ((REC_AC_LEN-g_sRecData.m_unRecAcTail)*2);
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[1][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 + g_sRecData.m_unRecAcTail - REC_AC_LEN));
         
          }
          else
          {
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //�洢��һ��ͨ�� 
              ucSaveFlg[1] = 0x55;
          }

          g_sRecData.m_CurWritePage++;                   //Flashҳ������
          break;          
    case 0x04:
          if(ucSaveFlg[1] == 0xAA)  //�Ѿ�ת�ص�����ͷ
          {
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[1][112 + g_sRecData.m_unRecAcTail - REC_AC_LEN]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //�洢��һ��ͨ�� 
          }
          else if(ucSaveFlg[1] == 0x55)
          {
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[1][g_sRecData.m_unRecAcTail + 112]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 - g_sRecData.m_unRecAcTail));   //�洢��һ��ͨ��
              ulAddr += ((112 - g_sRecData.m_unRecAcTail)*2);
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[1][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(g_sRecData.m_unRecAcTail));              
          }
          ucSaveFlg[1] = 0;

          g_sRecData.m_CurWritePage++;                   //Flashҳ������
          break;
    case 0x05:
          PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[2][g_sRecData.m_unRecAcTail]);        //�洢���涯��ǰ10���ܲ�������6���ܲ��Ľ����źŲ���          
          if(g_sRecData.m_unRecAcTail > 112)  //ʣ�µ����ݲ���һҳ����ȫд��ȥ
          {
              ucSaveFlg[2] = 0xAA;
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(REC_AC_LEN-g_sRecData.m_unRecAcTail));   //�洢��һ��ͨ�� 
              ulAddr += ((REC_AC_LEN-g_sRecData.m_unRecAcTail)*2);
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[2][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 + g_sRecData.m_unRecAcTail - REC_AC_LEN));
         
          }
          else
          {
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //�洢��һ��ͨ�� 
              ucSaveFlg[2] = 0x55;
          }

          g_sRecData.m_CurWritePage++;                   //Flashҳ������
          break;          
    case 0x06:
          if(ucSaveFlg[2] == 0xAA)  //�Ѿ�ת�ص�����ͷ
          {
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[2][112 + g_sRecData.m_unRecAcTail - REC_AC_LEN]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //�洢��һ��ͨ�� 
          }
          else if(ucSaveFlg[2] == 0x55)
          {
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[2][g_sRecData.m_unRecAcTail + 112]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 - g_sRecData.m_unRecAcTail));   //�洢��һ��ͨ��
              ulAddr += ((112 - g_sRecData.m_unRecAcTail)*2);
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[2][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(g_sRecData.m_unRecAcTail));              
          }
          ucSaveFlg[2] = 0;

          g_sRecData.m_CurWritePage++;                   //Flashҳ������
          break;

    case 0x07:
 //�ŏ| ¼��I0U0         PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[3][g_sRecData.m_unRecAcTail]);        //�洢���涯��ǰ10���ܲ�������6���ܲ��Ľ����źŲ���          
          if(g_sRecData.m_unRecAcTail > 112)  //ʣ�µ����ݲ���һҳ����ȫд��ȥ
          {
              ucSaveFlg[3] = 0xAA;
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(REC_AC_LEN-g_sRecData.m_unRecAcTail));   //�洢��һ��ͨ�� 
              ulAddr += ((REC_AC_LEN-g_sRecData.m_unRecAcTail)*2);
 //�ŏ| ¼��I0U0             PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[3][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 + g_sRecData.m_unRecAcTail - REC_AC_LEN));
         
          }
          else
          {
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //�洢��һ��ͨ�� 
              ucSaveFlg[3] = 0x55;
          }
    
          g_sRecData.m_CurWritePage++;                   //Flashҳ������
          break;          
    case 0x08:
          if(ucSaveFlg[3] == 0xAA)  //�Ѿ�ת�ص�����ͷ
          {
 //�ŏ| ¼��I0U0             PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[3][112 + g_sRecData.m_unRecAcTail - REC_AC_LEN]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //�洢��һ��ͨ�� 
          }
          else if(ucSaveFlg[3] == 0x55)
          {
//�ŏ| ¼��I0U0              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[3][g_sRecData.m_unRecAcTail + 112]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 - g_sRecData.m_unRecAcTail));   //�洢��һ��ͨ��
              ulAddr += ((112 - g_sRecData.m_unRecAcTail)*2);
//�ŏ| ¼��I0U0              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[3][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(g_sRecData.m_unRecAcTail));              
          }
          ucSaveFlg[3] = 0;
     
          g_sRecData.m_CurWritePage++;                   //Flashҳ������
          break;
    case 0x09:
 //�ŏ| ¼��I0U0         PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[4][g_sRecData.m_unRecAcTail]);        //�洢���涯��ǰ10���ܲ�������6���ܲ��Ľ����źŲ���          
          if(g_sRecData.m_unRecAcTail > 112)  //ʣ�µ����ݲ���һҳ����ȫд��ȥ
          {
              ucSaveFlg[4] = 0xAA;
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(REC_AC_LEN-g_sRecData.m_unRecAcTail));   //�洢��һ��ͨ�� 
              ulAddr += ((REC_AC_LEN-g_sRecData.m_unRecAcTail)*2);
 //�ŏ| ¼��I0U0             PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[4][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 + g_sRecData.m_unRecAcTail - REC_AC_LEN));
         
          }
          else
          {
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //�洢��һ��ͨ�� 
              ucSaveFlg[4] = 0x55;
          }

          g_sRecData.m_CurWritePage++;                   //Flashҳ������
          break;          
    case 0x0a:
          if(ucSaveFlg[4] == 0xAA)  //�Ѿ�ת�ص�����ͷ
          {
 //�ŏ| ¼��I0U0             PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[4][112 + g_sRecData.m_unRecAcTail - REC_AC_LEN]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //�洢��һ��ͨ�� 
          }
          else if(ucSaveFlg[4] == 0x55)
          {
 //�ŏ| ¼��I0U0             PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[4][g_sRecData.m_unRecAcTail + 112]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 - g_sRecData.m_unRecAcTail));   //�洢��һ��ͨ��
              ulAddr += ((112 - g_sRecData.m_unRecAcTail)*2);
 //�ŏ| ¼��I0U0             PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[4][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(g_sRecData.m_unRecAcTail));              
          }
          ucSaveFlg[4] = 0;

          g_sRecData.m_CurWritePage += 6;                   //Flashҳ������
          //���ݱ����������ձ�־
         // g_sRecData.m_ucOperRecFlag = 0;
          g_sRecData.m_ucRecSavingFlag = OFF;

          g_sRecData.m_unRecAcTail = 0;
       //   g_sRecData.m_unRecAcBefTail = 0;
          g_sRecData.m_unRecAcLockCnt = 0;
          g_sRecData.m_ucRecStart = OFF;
//		  
          if(g_I0RmtNum >= 8)     //��⵽��Ч����
          {
              g_I0RmtNum = 0;
              if(g_MaichongNum < 8)
                  g_MaichongNum++;
              
          }
          else
              g_I0RmtNum = 0;
      

          break;
       default:
          break;
    }

    //��EEPROM�н�������
      
}
*/

//==============================================================================

//  ��������   : SaveLoad

//  ��������   : �Ѹ���ͳ�Ƶ����ݱ��浽flash��

//  �������   : <��>

//  �������   : <��>

//  ����ֵ     : <��>

//  ����˵��   : ��¼���㸺�����ݣ������ʽΪ

//                   ��ʼ��(1)  ��¼�ֽ���(1)   ʱ��(5)     ����(X)

//                   0xEA         L             YYMMDDhhmm     xx   

//  ����       : 

//==============================================================================

//unsigned char ucLOADData[64][7]={0};//for test

//unsigned char min = 0;//for test

void SaveLoad(void)
{
    unsigned char LoadData[32];
    unsigned char LoadDataPtr=0;
    unsigned long FLoadAddr;
    unsigned int FLoadInfo[FLOADINFONUM];      //��EEPROM�ж�ȡ������FLASH�б��渺�ɼ�¼�������Ϣ��������+�����洢��¼��λ��+����һ����¼��λ��+У��
    LoadData[LoadDataPtr++] = 0xEA;     //��ʼ��
    LoadData[LoadDataPtr++] = 5 + 2*RMT_MEAS_NUM;//RM_LOADI_NUM*2; //�������ɼ�¼�ĳ���Ϊ5�ֽ�ʱ����ϸ��ɵ�������
    //ʱ�� ��¼�ꡢ�¡��ա�ʱ����
    LoadData[LoadDataPtr++] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
    LoadData[LoadDataPtr++] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
    LoadData[LoadDataPtr++] = g_sRtcManager.m_gRealTimer[RTC_DATE];
    LoadData[LoadDataPtr++] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
    LoadData[LoadDataPtr++] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
    memcpy(&LoadData[LoadDataPtr],&g_gRmtMeas[0],2*RMT_MEAS_NUM);
    CAT_SpiReadWords(EEPADD_LOADNUM, FLOADINFONUM, FLoadInfo);
    if((FLoadInfo[FLOAD_CS] != FLoadInfo[FLOAD_TOTALNUM] + FLoadInfo[FLOAD_NEW] + FLoadInfo[FLOAD_OLD]) || FLoadInfo[FLOAD_TOTALNUM] > FLASH_LOAD_MAXNUM || FLoadInfo[FLOAD_NEW] > FLASH_LOAD_MAXNUM)//���FLASH��ַ���ڸ��ɼ�¼����������
    {
        FLoadInfo[FLOAD_TOTALNUM] = 0; //��ո��ɼ�¼������
        FLoadInfo[FLOAD_NEW] = 0; //�����洢�ļ�¼���һ����¼��ƫ��λ��(�����Ѵ洢������һ����¼��λ��)
        FLoadInfo[FLOAD_OLD] = 0;//����һ����¼��λ��
    }
    FLoadAddr = FADDR_LOAD_START + (FLoadInfo[FLOAD_NEW] * FLASH_PLOAD_LEN);
    if(FLoadAddr == FADDR_LOAD_START || ((FLoadInfo[FLOAD_NEW] * FLASH_PLOAD_LEN) >= 4096 && (FLoadInfo[1] * FLASH_PLOAD_LEN) % 4096 == 0))
    {
        if(FLoadInfo[FLOAD_TOTALNUM] == FLASH_LOAD_MAXNUM)
        {
          FLoadInfo[FLOAD_TOTALNUM] -= (4096/FLASH_PLOAD_LEN);
          FLoadInfo[FLOAD_OLD] += (4096/FLASH_PLOAD_LEN);
          if(FLoadInfo[FLOAD_OLD] >= FLASH_LOAD_MAXNUM)
            FLoadInfo[FLOAD_OLD] = 0;
        }
        Sector_Erase(FLoadAddr);
    }
    if(FLoadInfo[FLOAD_NEW] == FLASH_LOAD_MAXNUM)
        FLoadAddr = FADDR_LOAD_START;
    Sst26vf064b_Page_WriteBytes(FLoadAddr, LoadData, 32);//�����ݱ��浽FLASH��   

    if(FLoadInfo[FLOAD_TOTALNUM] < FLASH_LOAD_MAXNUM)//���û�д�������FLASH�б���ĸ��ɼ�¼��������
    {
        FLoadInfo[FLOAD_TOTALNUM]++;
    }
    FLoadInfo[FLOAD_NEW]++;
    if(FLoadInfo[FLOAD_NEW] >= FLASH_LOAD_MAXNUM)
    {
        FLoadInfo[FLOAD_NEW] = 0;
    }
    FLoadInfo[FLOAD_CS] = FLoadInfo[FLOAD_NEW] + FLoadInfo[FLOAD_OLD] + FLoadInfo[FLOAD_TOTALNUM];
    //��EEPROM�м�¼����һ�����ɼ�¼��λ��
    CAT_SpiWriteWords(EEPADD_LOADNUM, FLOADINFONUM, FLoadInfo);
}
//==============================================================================
//  ��������   : SaveLOG
//  ��������   : ��LOG���ݴ����ڴ��е�sLOG_DATA g_sLogData[MAX_LOG_NUM] ����FLASH
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       : ZT
//==============================================================================
void SaveLOG(char   logtype,char logvalue )
{
	unsigned long temp;
	if(logtype>31)
		return;
	temp=0;
	if(logtype>=16)
		temp = (((DWORD)((WORD)(1<<(logtype-16))))<< 16);
	else
		temp = ((DWORD)((WORD)(1<<logtype)));
	if(logvalue==1)
		log_recorded.log_status |= temp;
	else if(logvalue==0)
		log_recorded.log_status &=(~temp);

	if(log_recorded.log_status>0xffff)
		{
		temp = 0;
		}
	return;
}
void ScanLOG()
{
	char i,olds,news;
	unsigned long temp;
	if(log_recorded.log_Mem_Flag != 0)
		return;
	
	for(i=0;i<32;i++)
		{
		olds = (log_recorded.log_status_bk>>i)&0x01;
		news = (log_recorded.log_status>>i)&0x01;
		if(olds!=news)
			SaveMEMLOG(i,news);
		}
	temp = (long)(1<<LOG_RESET)|(1<<LOG_101_LINK)|(1<<LOG_PAR_CHAG);
	temp = ~temp;
	log_recorded.log_status &= temp;
	log_recorded.log_status_bk=log_recorded.log_status;
}

void SaveMEMLOG(char   logtype,char logvalue )
{
    char i;	
	
	g_sLogData[log_recorded.log_MemNewPtr].m_gLogType=logtype;
	g_sLogData[log_recorded.log_MemNewPtr].m_gLogValu=logvalue;
	for(i=0;i<6;i++)
    	g_sLogData[log_recorded.log_MemNewPtr].m_gLogTimer[i] = g_sRtcManager.m_gRealTimer[i];
   	g_sLogData[log_recorded.log_MemNewPtr].m_gLogTimer[0] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
	g_sLogData[log_recorded.log_MemNewPtr].m_gLogTimer[6] = LOBYTE(g_sRtcManager.m_gRealTimer[RTC_MICROSEC]);
	g_sLogData[log_recorded.log_MemNewPtr].m_gLogTimer[7] = HIBYTE(g_sRtcManager.m_gRealTimer[RTC_MICROSEC]);
	if(((logtype==LOG_8FULS_STA)&&(logvalue==1))||(logtype==LOG_BREAK))
		{
		for(i=0;i<8;i++)
			g_sLogData[log_recorded.log_MemNewPtr].m_gRmtMeas[i]=yc[i];
		}
	else
		{
		g_sLogData[log_recorded.log_MemNewPtr].m_gRmtMeas[RM_U0] =g_gRmtMeas[RM_U0];
		g_sLogData[log_recorded.log_MemNewPtr].m_gRmtMeas[RM_UA] =g_gRmtMeas[RM_UA];
		g_sLogData[log_recorded.log_MemNewPtr].m_gRmtMeas[RM_UB] =g_gRmtMeas[RM_UB];
		g_sLogData[log_recorded.log_MemNewPtr].m_gRmtMeas[RM_UC] =g_gRmtMeas[RM_UC];
		g_sLogData[log_recorded.log_MemNewPtr].m_gRmtMeas[4] =g_gRmtMeas[RM_UPt];
		g_sLogData[log_recorded.log_MemNewPtr].m_gRmtMeas[5] =g_gRmtMeas[RM_UCAP];
		g_sLogData[log_recorded.log_MemNewPtr].m_gRmtMeas[6] =g_gRmtMeas[RM_CSQ];
		}
	log_recorded.log_MemNewPtr++;
	if(log_recorded.log_MemNewPtr >= MAX_LOG_NUM)
		log_recorded.log_MemNewPtr = 0;
	return;
}

//==============================================================================
//  ��������   : SaveFlashLOG
//  ��������   : ���ڴ��е�sLOG_DATA g_sLogData[MAX_LOG_NUM] ����FLASH
//  �������   : <��>
//  �������   : <��>
//  ����ֵ     : <��>
//  ����˵��   : �����ַ FADDR_LOG_DATA      0x720000-0x729000;32�ֽ� 1152����LOG_RECORDER log_recorded
//  ����       : ZT
//==============================================================================

void SaveFlashLOG(void)
{
    unsigned char saveflag=0;
	long fLoadAddr;
    //unsigned char LoadDataPtr=0;
    //unsigned long FLoadAddr;
    unsigned int FLogInfo[FLOGINFONUM];      //��EEPROM�ж�ȡ������FLASH�б��渺�ɼ�¼�������Ϣ��������+�����洢��¼��λ��+����һ����¼��λ��+У��
	
	if(log_recorded.log_MemNewPtr==log_recorded.log_MemPtr)
		{
		log_recorded.log_Mem_Flag=0;
		return;
		}
	log_recorded.log_Mem_Flag=0x55;
	do
	{	
	fLoadAddr = log_recorded.log_FlashNewPtr;	
	Sst26vf064b_Page_WriteBytes(fLoadAddr,(unsigned char*)&g_sLogData[log_recorded.log_MemPtr],32);
	
	log_recorded.log_FlashNewPtr += 32;	
	if(log_recorded.log_FlashNewPtr >= FADDR_LOG_END)
		log_recorded.log_FlashNewPtr = FADDR_LOG_START;	
	if((log_recorded.log_FlashNewPtr & 0x0fff)==0)		
		Sector_Erase(log_recorded.log_FlashNewPtr);
	
	log_recorded.log_FlashPtr &= 0xfffff000;
	if(log_recorded.log_FlashNewPtr == log_recorded.log_FlashPtr)
		log_recorded.log_FlashPtr += 0x1000;
	if(log_recorded.log_FlashPtr>=FADDR_LOG_END)
		log_recorded.log_FlashPtr = FADDR_LOG_START;		
	if(log_recorded.log_FlashNewPtr >= FADDR_LOG_END)		
		log_recorded.log_FlashNewPtr =FADDR_LOG_START;
	
	log_recorded.log_MemPtr++;
	if(log_recorded.log_MemPtr>=MAX_LOG_NUM)	
		log_recorded.log_MemPtr=0;
	log_recorded.log_Flash_count++;
	if(log_recorded.log_Flash_count>=1152)
		log_recorded.log_Flash_count=1024;
	saveflag=0x55;
	}
	while(log_recorded.log_MemNewPtr!=log_recorded.log_MemPtr);
			
	if(saveflag == 0x55)
		{
		FLogInfo[FLOG_TOTALNUM]=log_recorded.log_Flash_count;
		FLogInfo[FLOG_NEW]=log_recorded.log_FlashNewPtr;
		FLogInfo[FLOG_OLD]=log_recorded.log_FlashPtr;
		FLogInfo[FLOG_CS]=FLogInfo[FLOG_TOTALNUM]+FLogInfo[FLOG_NEW]+FLogInfo[FLOG_OLD];
    	CAT_SpiWriteWords(EEPADD_LOGP , FLOGINFONUM, FLogInfo);		
		CAT_SpiWriteWords(EEPADDBK_LOGP, FLOGINFONUM,FLogInfo);
		}
	log_recorded.log_Mem_Flag=0;
	return;
}

