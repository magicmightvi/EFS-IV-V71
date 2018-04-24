/*------------------------------------------------------------------------
 Module:      DataProcess.cpp
 Author:      linxueqin
 Project:     子站平台
 State:       V1.0
 Creation Date:   2014-4-29
 Description:   AD采样初始化，计算子站采集电压，创建和保存SOE，组织遥测遥信发送缓冲区
------------------------------------------------------------------------*/


#ifndef _DATA_PROCESS

    #define _DATA_PROCESS
    #include "DataStruct.h"
    #include "sqrttable.h"
#include "Main.h"
#endif

//==============================================================================
//  函数名称   : InitDataProc
//  功能描述   : 数据处理初始化
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       : 
//==============================================================================
/*void InitDataProc(void)
{
    unsigned int i,j;
    for(i = 0; i < AC_AD_CHAN_NUM;i++)
      for(j = 0; j < AC_BUFF_LEN;j++)
        g_sSampleData[i][j] = 0;
    if(g_gRunPara[Terminal_Type] == AERIAL_TERMINAL)
    {//架空终端的通道状态判断才使用g_sCommStatCount这个数组
      for(i = 0; i < RMT_ZSQST_NUM; i++)
        g_sCommStatCount[i] = g_gRunPara[RP_ZSQCST_T];
    }
    g_sSampleFinish = OFF;
    
    //soe变量初始化
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
 *判断一个unsigned long的非0的最高位数。
 *@param[in] xi 要判断的unsigned long
 *@return 返回这个数的非零的最高位置
 */
unsigned int sqrtinit(unsigned long xi)// 判断一个unsigned long的非0的最高位数
{
    unsigned int x0 = 16;
    int n;
//      unsigned long  xt;
    //xt = xi;
    // x0 = (2^4) +/- (2^3) +/- (2^2) +/- (2^1) +/- (2^0) +(1 or 0)
    // 其中右移为加,左移为减
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
  *开方函数
  *@param[in] 要开方的数(unsigned long)
  *@return 返回开方值(unsigned int)
  *@note 实现方法位牛顿迭代法，现在不用了。
  *
  */
unsigned int ndsqrt(unsigned long x)// 给一个unsigned long开方－－牛顿迭代法
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
    
    r = (xk + x / xk) / 2;// 牛顿迭代过程
    
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

//－－－－－－－－－－－－－－－－－－－－－－－－－
//函数名字:table_sqrt()
//函数功能:查表开方,开方表中数据定标Q10
//参数说明:要开方的unsigned long型数据
//返回值:开方后的unsigned int型数据
//－－－－－－－－－－－－－－－－－－－－－－－－
unsigned int table_sqrt(unsigned long x)//查表法的开方函数,表中数据用Q8定标
{
    unsigned int pos_valid;//the position which the first 1 appears in x in 2s format
    unsigned int Temp_pos1,Temp_pos2;//the two temporary variables are used to determine 
                     
    unsigned long temp_result;
    pos_valid = sqrtinit(x);//确定第一个1的位置（二进制表示）
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
//    result = temp_result >> 10;//表中数据为Q10
    return (unsigned int)temp_result;
}

//==============================================================================
//  函数名称   : AverFilter
//  功能描述   : 中位值平均滤波法,10个值，除去最大和最小的,剩下8个值求平均值
//  输入参数   : unsigned int *p: 要滤波的数组，为一个长度为10的数组，除去最大和最小，剩下8个值求平均值
//  输出参数   : 滤波结果
//  返回值     : <无>
//  其他说明   : 注意该数组的长度需要为10
//  作者       : 林中一
//==============================================================================
unsigned int AverFilter(unsigned int *p)  //中位值平均滤波法,10个值，除去最大和最小的,剩下8个值求平均值
{
    unsigned int i,max,min,temp=0;
    unsigned long sum;
            //关闭全局中断
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
//  函数名称   : InitDataProc
//  功能描述   : 数据处理初始化
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       : 林中一
//==============================================================================
void InitDataProc(void)
{
    unsigned int i,j,temp[8];
    unsigned long ulAddr;
 
    g_sSampleData.m_unAcDataTail = (AC_BUFF_LEN - 1);
 

    //录波模块用到的全局变量初始化

    g_sRecData.m_ucRecSavingFlag = OFF;

    g_sRecData.m_unRecAcLockCnt = 0;
    g_sRecData.m_unRecAcTail = 0;

    g_sRecData.m_ucRecStart = OFF;
    g_sRecData.m_ucFaultRecStart= OFF;	
    g_sRecData.m_ucActRecStart = CLOSE;	
    g_sRecData.m_ucActRecSavingFlag = OFF;  	

    CAT_SpiReadWords(EEPADD_LUBONUM, 6, temp);//读取录波总条数
    if(temp[0]>MAX_REC_NUM) temp[0]=0;//录波总条数>MAX_REC_NUM ，则为0
    if(temp[1]>MAX_REC_NUM) temp[1]=0;//录波总条数>MAX_REC_NUM ，则为0
    if(temp[2]>MAX_ACTREC_NUM) temp[2]=0;//录波总条数>MAX_REC_NUM ，则为0
    if(temp[3]>MAX_ACTREC_NUM) temp[3]=0;//录波总条数>MAX_REC_NUM ，则为0
    if(temp[4]>MAX_XHREC_NUM) temp[4]=0;//录波总条数>MAX_REC_NUM ，则为0
    if(temp[5]>MAX_XHREC_NUM) temp[5]=0;//录波总条数>MAX_REC_NUM ，则为0    
    if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_DEL_LUBO])
    	{
    	temp[0]=0;temp[1]=0;temp[2]=0;temp[3]=0;temp[4]=0;temp[5]=0;
		DelALLSOE();
    	}
    CAT_SpiWriteWords(EEPADD_LUBONUM, 6, temp); 
      g_sRecData.m_gRecANum=temp[0];//录波总条数1~32
      g_sRecData.m_gRecCNum=temp[1];//录波当前位置0~31
      g_sRecData.m_gACTRecANum=temp[2];//录波总条数1~32
      g_sRecData.m_gACTRecCNum=temp[3];//录波当前位置0~31      
      g_sRecData.m_gXHRecANum=temp[4];//录波总条数1~32
      g_sRecData.m_gXHRecCNum=temp[5];//录波当前位置0~31
	  g_sRecData.m_gACTDelay = 0;
	  g_sRecData.m_gXHDelay=0;
//提前擦除flash中，文件目录	
    //long FADDR_RECORDER =FADDR_RECORDER_ACTDATA+ (long)temp[1]*(long)FLINEADDR;	
    //Sector_Erase(FADDR_RECORDER);//预先擦除下一个sector
//提前擦除flash中，录波文件
  ulAddr = FADDR_RECORDER_DATA+(unsigned long)(g_sRecData.m_gRecCNum)*0x2000;//flash地址
  //g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址  
  Sector_Erase(ulAddr);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x1000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  
  ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;  
  g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址  
  Block_Erase(ulAddr);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x10000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x20000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x30000);//ERASE 1个BLOCK 
  delayms(100);WatchDog(); 
  Block_Erase(ulAddr+0x40000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x50000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x60000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Block_Erase(ulAddr+0x70000);//ERASE 1个BLOCK 
  delayms(100);WatchDog(); 
  Block_Erase(ulAddr+0x80000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();  

  ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;//flash地址
  //g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址  
  Sector_Erase(ulAddr);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x1000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x2000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x3000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x4000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x5000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x6000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x7000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  
  FlashReading = 0;
  
//负荷记录初始化
      CAT_SpiReadWords(EEPADD_LOADNUM, FLOADINFONUM, temp);
      if((temp[FLOAD_CS] != temp[FLOAD_TOTALNUM] + temp[FLOAD_NEW] + temp[FLOAD_OLD]) || temp[FLOAD_TOTALNUM] > FLASH_LOAD_MAXNUM || temp[FLOAD_NEW] > FLASH_LOAD_MAXNUM)//如果FLASH地址不在负荷记录保存区域内
      {
          temp[FLOAD_TOTALNUM] = 0; //清空负荷记录总条数
          temp[FLOAD_NEW] = 0; //即将存储的记录与第一条记录的偏移位置(不是已存储的最新一条记录的位置)
          temp[FLOAD_OLD] = 0;//最老一条记录的位置
          temp[FLOAD_CS] = 0;
          CAT_SpiWriteWords(EEPADD_LOADNUM, FLOADINFONUM, temp);
      }
	//g_gSaveload=0;
//LOG记录初始化
	  CAT_SpiReadWords(EEPADD_LOGP , FLOGINFONUM, temp);
	  if((temp[FLOG_CS] != temp[FLOG_TOTALNUM] + temp[FLOG_NEW] + temp[FLOG_OLD])
	  	|| temp[FLOG_TOTALNUM] > FLASH_LOG_MAXNUM || temp[FLOG_NEW] > 0X9000|| temp[FLOG_OLD] > 0X9000
	  	|| ((temp[FLOG_OLD]&0x0fff)!=0))//如果FLASH地址不在负荷记录保存区域内
      	{
      	CAT_SpiReadWords(EEPADDBK_LOGP , FLOGINFONUM, temp);
		if((temp[FLOG_CS] != temp[FLOG_TOTALNUM] + temp[FLOG_NEW] + temp[FLOG_OLD])
	  		|| temp[FLOG_TOTALNUM] > FLASH_LOG_MAXNUM || temp[FLOG_NEW] > 0X9000|| temp[FLOG_OLD] > 0X9000
	  		|| ((temp[FLOG_OLD]&0x0fff)!=0))//如果FLASH地址不在负荷记录保存区域内
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

    for(i = 0; i < AC_AD_CHAN_NUM; i++)     //由于刚开机的时候，交流采集缓冲区的数据没有填满，需要清空该缓冲区，以免均方根累加值不正确
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
    //g_gRmtMeas[RM_ACT_NUM] = 1;//张弢 动作次数
    //soe变量初始化    
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
//  函数名称   : ComputeMax
//  功能描述   : 求三个变量的最大值
//  输入参数   : unsigned long x,y,z;三个值
//  输出参数   : <无>
//  返回值     : unsigned long result;返回最大值
//  其他说明   : 
//  作者       : 林中一
//==============================================================================
unsigned int ComputeMax(unsigned int x,unsigned int y,unsigned int z)//计算最大值
{
    unsigned int result;

    result = x >= y ? x : y;
    result = result >= z ? result : z;

    return result;
}

//==============================================================================
//  函数名称   : ComputeMax
//  功能描述   : 求三个变量的最小值
//  输入参数   : unsigned long x,y,z;三个值
//  输出参数   : <无>
//  返回值     : unsigned long result;返回最小值
//  其他说明   : 
//  作者       : 林中一
//==============================================================================
unsigned int ComputeMin(unsigned int x,unsigned int y,unsigned int z)//计算最小值
{
    unsigned int result;

    result = x <= y ? x : y;
    result = result <= z ? result : z;

    return result;
}
//==============================================================================
//  函数名称   : CalcuProtMeas
//  功能描述   : 均分根计算,计算结果保存到保护电量中(为开方之前的数据)
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       : 林中一
//==============================================================================
/*
void CalcuProtMeas(void)
{

   // unsigned int i;
    long m,n,m1,n1,m2,n2,m3,n3;
   // unsigned long l = 0;
    unsigned int AddIndex, RemoveIndex;
   // static long tempAdc;
    
    //由于校正系数发生变化，可能出现Protmeas[]为原来的值，而AdcData的数据*AdjPara的数据发生变化，导致Protmeas += (m+n)(m-n)的数为负数，出现异常。
    //因此校正的时候，清空AdcData和Protmeas 因此对AdjPara参数的变化必须能够使装置重启
    AddIndex = (g_sSampleData.m_unAcDataTail);      //获得均方根周波的最新的数据在交流采集缓冲区中的位置
    RemoveIndex = (AddIndex - AC_SAMPLE_DOTS) & (AC_BUFF_LEN - 1);  //获得要从该周波去去掉的最老的数据的位置
    //Removedots = (AddIndex - AC_SAMPLE_DOTS_HALF) & (AC_BUFF_LEN - 1);
    //对I0,U0,UA,UB,UC,UAB,UBC,UCA的处理，考虑到执行效率，因此放到for循环外
    m = g_sSampleData.m_gAcAdcData[CHAN_I0][AddIndex];
    n = g_sSampleData.m_gAcAdcData[CHAN_I0][RemoveIndex];
    g_gProcMeas[PM_I0] += (m+n)*(m-n);//m平方-n平方=(m+n)(m-n)
        
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
    g_gProcMeas[PM_U0] += (m+n)*(m-n);//m平方-n平方=(m+n)(m-n)

    g_unRmCaluFlag = ON;    //遥测数据更新了，可以进行遥测量的运算
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
// 功能描述   : 根据傅里叶算法计算获得有效值
//  输入参数   : AdcChannel --AD采样数据的通道，直接从g_sSampleData.m_gAcAdcData[AdcChannel]取对应的数据
//  输出参数   : 根据计算出来的实部和虚部直接获得有效值进行返回
//  返回值     : <无>
//  其他说明   : 
//  作者       : lzy
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
//  函数名称   : CalcuRmtMeas
//  功能描述   : 有效值计算，并更新对应的遥测值
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       : 林中一
//==============================================================================
void CalcuRmtMeas(void)
{
    unsigned int i;
    unsigned int TempRm = 0;

    unsigned long tDft,a,b,c;
    //unsigned int unTemp[3];
    if(g_unRmCaluFlag == OFF)   //如果遥测运算标志仍然为OFF，则说明1.25ms的AD中断没有进去过，g_gProcMeas没有更新，不需要运算
    {
        return;
    }
    else
    {
        g_unRmCaluFlag = OFF;
    }
    //ProtLogic();
    //ProtStart();	
	
    for(i = 0; i < 6/*RMT_MEAS_NUM -1*/ ; i++)//添加了UAB,UBC,UCA三线电压，i的上限=PM_UCA
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
    for(i = 0; i < 8/*RMT_MEAS_NUM-1*/ ; i++)//添加了UAB,UBC,UCA三线电压，i的上限=PM_UCA
    {
        TempRm = AverFilter(g_gRmAcFilt[i]);        //对遥测量的交流量进行滤波        
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
        TempRm = AverFilter(g_gRmAcFilt[9]);        //对UPt遥测量的交流量进行滤波        
        g_gRmtFilMeas[9] = TempRm/25;	//Uo to Upt
        g_gRmtMeas[9] = g_gRmtFilMeas[9];
    g_unFilterIndex++;
    if(g_unFilterIndex == 10)
    {
        g_unFilterIndex = 0;
    }
}

	//==============================================================================
	//	函数名称   : ScanPT
	//	功能描述   : 扫描PT断线遥信
	//	输入参数   : <无>
	//	输出参数   : <无>
	//	返回值	  : <无>
	//	其他说明   : 100ms执行一次，500ms确认
	//	作者		 : 张弢
	//==============================================================================
void ScanPT(void)
{ 
  unsigned int unTemp[3];
  
    if(g_gRmtMeas[RM_U0] >= g_gProcCntJug[PC_HIGH_Z])  //零序过压
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

    unTemp[1] = ComputeMax(g_gRmtMeas[RM_UAB], g_gRmtMeas[RM_UBC], g_gRmtMeas[RM_UCA]);  //求出最大线电压
    if(unTemp[1] >= g_gProcCntJug[PC_I0_START]) 
        g_gRmtInfo[YX_UAB_HIGH] =1;
    else
        g_gRmtInfo[YX_UAB_HIGH] =0;
	
    unTemp[0] = ComputeMax(g_gRmtMeas[RM_UA], g_gRmtMeas[RM_UB], g_gRmtMeas[RM_UC]);      //求出最大相电压
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
//  函数名称   : YCthCross
//  功能描述   :判断遥测值的突变
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       : 林中一
//==============================================================================
void YCthCross(void)
{
//张弢 遥测点表
unsigned int uuTemp[IEC_YC_NUM],i;  
unsigned long ulTemp[7];
unsigned long Templm = 0;

    if(g_gYCYueXian !=0x55)   //张弢 遥测越限
	return;

    for(i = 0; i < IEC_YC_NUM/*RMT_MEAS_NUM*/; i++)
    { 
	 uuTemp[i] = g_gRmtMeas[i] ;//云南遥测要1次值
        if(i==7)
        	{        	
        	if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_CURRENT_PRIMARY])
			uuTemp[i]=uuTemp[i]* g_gRunPara[RP_CT_TRANS]/100;//电流为1次值，单位A
		else
			uuTemp[i]=uuTemp[i]*10;//电流为2次值，单位mA
        	}
    }
//张弢 遥测点表    
    for(i = 0; i < IEC_YC_NUM/*RMT_MEAS_NUM*/; i++)
    {  
         //if(i <= 7)//张弢 遥测点表
            //g_unYcTrans[i] = (g_gRmtMeas[g_ucYCAddr[i] - 1] / 10);//张弢 遥测点表
            g_unYcTrans[i] = uuTemp[g_ucYCAddr[i] - 1] ;//张弢 遥测点表
        //else//张弢 遥测点表
            //g_unYcTrans[i] = g_gRmtMeas[g_ucYCAddr[i] - 1] * 10 ;//张弢 遥测点表
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
			 g_gYCchangData[g_gChangeYCNum + 9] = g_gRmtMeas[i];//昨天云南，遥测要一次值
			 if(i == 7)
			 	{
			 	if(g_gRunPara[RP_CFG_KEY]&BIT[RPCFG_CURRENT_PRIMARY])
					g_gYCchangData[g_gChangeYCNum + 9] =g_gYCchangData[g_gChangeYCNum + 9] * g_gRunPara[RP_CT_TRANS]/100;//电流为1次值，单位A
				else
					g_gYCchangData[g_gChangeYCNum + 9] =g_gYCchangData[g_gChangeYCNum + 9] *10;//电流为2次值，单位mA
			 	}
          	        g_SendChgYcFlag = 1;
		        break;
          	      }		  	
                  } 		
                //g_SendChgYcFlag = 1;
              }            
        }
    }	
    
   if(g_gYCYueXian==0x55)   //张弢 遥测越限
     {
     	for(i = 0; i < IEC_YC_NUM/*RMT_MEAS_NUM*/; i++)
        { 
        g_gRmtMeasBak[i] = g_gRmtMeas[i];	
     	}
        g_gYCYueXian=0;//张弢 遥测越限	
     } 

}
//==============================================================================
//  函数名称   : RecData
//  功能描述   : 录波数据准备，在1.25ms中断中调用。
//                   完成保护元件前的录波数据准备,完成故障动作或操作出口之前的录波数据准备
//                   完成故障动作或操作出口后，在0.125ms中断中未完成的录波数据准备
//                   录波数据准备完毕，置相关的标志
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 0.125ms过程中的录波数据准备不调用该函数，直接在中断中写代码
//  作者       : 林中一
//==============================================================================
/*
void RecData(void)
{
    unsigned int unTemp = 0;
    
    if(g_sRecData.m_ucRecSavingFlag == YES)
    {
    return; //如果录波数据还没有保存结束则不再进行新的录波
    }
    
    
    
    //if(g_sRecData.m_unRecAcLockCnt == 0)    //没有故障出口或者操作出口，则准备动作之前的录波数据
    if(g_sRecData.m_ucRecStart == OFF)
    {

//张弢 录波I0U0        g_sRecData.m_gRecAc[CHAN_I0][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_I0][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[CHAN_UA][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_UA][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[CHAN_UB][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_UB][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[CHAN_UC][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_UC][g_sSampleData.m_unAcDataTail];
        //增加零序电压数据
//张弢 录波I0U0        g_sRecData.m_gRecAc[CHAN_U0][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_U0][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_unRecAcTail++;
        if(g_sRecData.m_unRecAcTail == REC_AC_LEN)
            g_sRecData.m_unRecAcTail = 0;
    }
    else// if(g_sRecData.m_unRecAcLockCnt < 96)//操作动作开始后的录波需要再录波6个周波，录满6个周波，即96个点就停止录波。
    {
        //由于0.125ms中该部分数据可能没有准备完成，需要在高速采样过程中继续准备，准备完成后，置录波数据准备完毕标志

        
        g_sRecData.m_gRecAc[CHAN_UA][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_UA][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[CHAN_UB][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_UB][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_gRecAc[CHAN_UC][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_UC][g_sSampleData.m_unAcDataTail];
        //增加零序电压数据
//张弢 录波I0U0        g_sRecData.m_gRecAc[CHAN_U0][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_U0][g_sSampleData.m_unAcDataTail];
 //张弢 录波I0U0       g_sRecData.m_gRecAc[CHAN_I0][g_sRecData.m_unRecAcTail] = g_sSampleData.m_gAcAdcData[CHAN_I0][g_sSampleData.m_unAcDataTail];
        g_sRecData.m_unRecAcTail++;
        if(g_sRecData.m_unRecAcTail == REC_AC_LEN)
            g_sRecData.m_unRecAcTail = 0;

        g_sRecData.m_unRecAcLockCnt++;  //操作动作开始后的录波需要再录波6个周波，录满6个周波，即96个点就停止录波
        if(g_sRecData.m_unRecAcLockCnt == 96)   //96事故或操作出口发生后的6个周波，录波数据准备完毕
        {
        //  g_sProtLogic.m_ucActStartRec = OFF;  
          g_sRecData.m_ucRecPrepFlag = 2; //录波数据准备完毕，准备写入FLASH中
          if(pulse_phase_flag != 2 ) //B相不录播
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
//  函数名称   : RecData
//  功能描述   : 录波数据准备，在0.25ms中断中调用，4K采样频率。
//                   完成保护元件前的录波数据准备,完成故障动作或操作出口之前的录波数据准备
//                   完成故障动作或操作出口后，在0.125ms中断中未完成的录波数据准备
//                   录波数据准备完毕，置相关的标志
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 0.25ms中断中写代码
//  作者       : 林中一
//==============================================================================
void RecData(void)
{
    //unsigned int unTemp = 0;
	//static unsigned int ab= 0;
    
    if(g_sRecData.m_ucFaultRecStart ==CLOSE)
    {
    return; //如果故障没有恢复。则不再进行新的录波
    }
    if(g_sRecData.m_ucActRecStart  != CLOSE)
    {
    return; //如果录波数据还没有保存结束则不再进行新的录波
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
	  
	  if((g_gKON>0)&&(g_gKON<4))//张弢开关已经闭合//录波增加继电器开关量
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][4] |= (1<<14);//录波增加继电器开关量
	  else	
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][4] &= NBITE;//录波增加继电器开关量
	  if(g_gKON==1) 	
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][0] |= (1<<14);//录波增加继电器开关量
	  else	
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][0] &= NBITE;//录波增加继电器开关量
	  if(g_gKON==2) 	
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][1] |= (1<<14);//录波增加继电器开关量
	  else	
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][1] &= NBITE;//录波增加继电器开关量
	  if(g_gKON==3) 	
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][2] |= (1<<14);//录波增加继电器开关量
	  else	
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][2] &= NBITE;//录波增加继电器开关量
		
        g_sRecData.m_unRecAcTail++;
        if(g_sRecData.m_unRecAcTail == REC_AC_LEN)
            g_sRecData.m_unRecAcTail = 0;    
    

    if(g_sRecData.m_ucFaultRecStart == OFF)
    {
	        g_sRecData.m_unRecAcLockCnt = 0;
    }
    else if(g_sRecData.m_ucFaultRecStart == ON)//开始录波，录1024点结束
    {
        g_sRecData.m_unRecAcLockCnt++; 
#ifdef YN_101S
	 if(g_sRecData.m_unRecAcLockCnt >= 10)   //故障发生后的5个周波记录完毕，记录最后时间，
#else
	 if(g_sRecData.m_unRecAcLockCnt >= 120)   //故障发生后的5个周波记录完毕，记录最后时间，
#endif
        {
            g_sRecData.m_ucFaultRecStart = CLOSE;//录波结束，故障恢复后，恢复OFF
            g_sRecData.m_unRecAcLockCnt = 1000;
            g_sRecData.m_gFaultRecOver[REC_MSL] =  g_sRtcManager.m_gRealTimer[RTC_MICROSEC];
            g_sRecData.m_gFaultRecOver[REC_MSH] =g_sRtcManager.m_gRealTimer[RTC_SEC];
            g_sRecData.m_gFaultRecOver[REC_MINU] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
            g_sRecData.m_gFaultRecOver[REC_HOUR] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
            g_sRecData.m_gFaultRecOver[REC_DAY] = g_sRtcManager.m_gRealTimer[RTC_DATE];
            g_sRecData.m_gFaultRecOver[REC_MONTH] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
            g_sRecData.m_gFaultRecOver[REC_YEAR] = (g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000);
            g_sRecData.m_ucRecSavingFlag = YES; //张弢 录波    //置标志要求保存到FLASH中
        }
    }

}
//==============================================================================
//  函数名称   : RecActData
//  功能描述   : 动作录波数据存入g_sRecData.m_gRecAc，在1.25ms中断中调用，800采样频率。。
//                   m_ucActRecStart 为启动录波标志，ON 开始录波、OFF结束录波、CLOSE结束后存储完成
//                   m_ucActRecSavingFlag 为存储标志，YES存储前320个点、ON存储后320个点、OFF存储结束
//                 
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 0.25ms过程中的故障录波数据准备不调用该函数，在1.25ms中断中调用。
//  作者       : 张弢
//==============================================================================

void RecActData(void)
{
    //static unsigned int abx= 0;
   if(g_sRecData.m_ucFaultRecStart == ON)
    {
    return; //动作录波,从故障开始至存储结束，则不再进行新的录波
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
	  if((g_gKON>0)&&(g_gKON<4))//张弢开关已经闭合//录波增加继电器开关量
	  	{
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][4] |= (1<<14);//录波增加继电器开关量
	  	}
	  else	
	  	{
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][4] &= NBITE;//录波增加继电器开关量
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
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][0] |= (1<<14);//录波增加继电器开关量
	  	}
	  else//if(g_gRmtInfo[YX_P4IN4]==1)	
	  	{
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][0] &= NBITE;//录波增加继电器开关量
	  	}

#ifdef CONDIN_3
	  if(KJb1==kb)
#else	  
	  if((g_gKONBK==2)&&(KJb1==kb))	
#endif	  	
	  	{
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][1] |= (1<<14);//录波增加继电器开关量
	  	}
	  else//if(g_gRmtInfo[YX_P4IN4]==1)		
	  	{
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][1] &= NBITE;//录波增加继电器开关量
	  	}

#ifdef CONDIN_3
	  if(KJc1==kc)
#else	  
	  if((g_gKONBK==3)&& (KJb1==kb))
#endif	  	
	  	{
	  	g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][2] |= (1<<14);//录波增加继电器开关量
	  	}
	  else//if(g_gRmtInfo[YX_P4IN4]==1)
	  	{
		g_sRecData.m_gRecAc[g_sRecData.m_unRecAcTail][2] &= NBITE;//录波增加继电器开关量

	  	}
		
g_test++;
    	if((g_sRecData.m_unRecAcTail == 360))
    		g_sRecData.m_ucActRecSavingFlag = YES; //开始存储前320个点	  
        g_sRecData.m_unRecAcTail++;
        if(g_sRecData.m_unRecAcTail == REC_AC_LEN)
        {
           g_sRecData.m_unRecAcTail = 0;  
	    g_sRecData.m_ucActRecSavingFlag = ON; //开始存储后320个点	 	   
        }
   
    }
	/*
    else if(g_sRecData.m_ucActRecStart == OFF)
    {
    	if((g_sRecData.m_unRecAcTail>319))
    		g_sRecData.m_ucActRecSavingFlag = ON; //开始存储后320个点
    	else
		g_sRecData.m_ucActRecSavingFlag = YES; //开始存储前320个点	
	//g_sRecData.m_ucActRecStart = CLOSE;	
    }
    */
}
/*
//==============================================================================
//  函数名称   : SaveRecData
//  功能描述   : 按照COMTRADE格式整理录波数据，并分批次保存到Flash中
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       : 林中一
//==============================================================================
void SaveRecData(void)
{
    
      
}

//==============================================================================
//  函数名称   : CreatNewSoe
//  功能描述   : 把类型为SOEtype的事故，发生值为SOEvalue的事故写入SOE记录缓冲中，
//                   并在主循环中把缓冲通过SaveSoeData函数写入EEPROM中保存
//                   如果是开关量输入产生的SOE,需要进行一下SOE的时间排序，并把当前发生的时刻提前DIN判断延时时间
//                   缓冲区存放最新的32个SOE记录，FLASH中总共存放256个SOE
//  输入参数   : SOEtype-发生事故类型；
//                   SOEPhase-故障事故发生的相位,a-c相分别对应1-3,ab-bc-ca对应4-6,0为无效
//                   SOEvalue-事故发生时对应的值
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：林中一
//==============================================================================
void CreatNewSoe(unsigned int SoeType,unsigned int SoePhase, unsigned long SoeValue)
{
    
}
*/
//==============================================================================
//  函数名称   : SaveSoeData
//  功能描述   : 根据m_unNewSoeRomNum把所有新的SOE数据都保存到Flash中
//                   为了避免在写SOE的过程中，再次产生新的SOE纪录，则在进入函数时，
//                   先采用临时变量纪录m_unSoeBuffTail和m_unNewSoeRomNum，记住当前需要写入的SOE个数。
//                   如果在这个过程中，产生新的SOE纪录，则在退出该函数后，由于NewSOECount ！= 0，
//                   则会再次进入该函数，进行SOE的写操作。
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       : 
//==============================================================================
void SaveSoeDataRepeat(void)
{
    unsigned int i;
   // unsigned char n,k,j,m,c,b;
   // unsigned char ucTemp[7];
    
    if((fault_save==0x55)&&(flash_write==0))  /////////延时完成，开始存储故障
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
       if(g_gProcCnt[PC_EFS_MODEL]>0)                   //////信号源类型为II型，
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
       CreatNewSMS(FAULT_OCCUR);                      //产生故障短信//张弢
 */      
       
       g_sSoeData.m_ucSoeNum++;      //总条数加
       if(g_sSoeData.m_ucSoeNum>30)
    	    g_sSoeData.m_ucSoeNum = 30;	   
       CAT_SpiWriteBytes(EEPADD_SOE_DATA + g_sSoeData.m_ucSoePos * 12 , 12, g_sSoeData.m_gSoeBuff);
       g_sSoeData.m_ucSoePos++;        //位置加
    	if(g_sSoeData.m_ucSoePos>=30)
    	    g_sSoeData.m_ucSoePos = 0;	
        CAT_SpiWriteByte(EEPADD_SOE_NUM,g_sSoeData.m_ucSoeNum);
        CAT_SpiWriteByte(EEPADD_SOE_POS,g_sSoeData.m_ucSoePos); 
        eight_delay_counter= 200;  ///////emit 8 pulse after 2 seconds
        //g_sRecData.m_ucRecSavingFlag = YES; //张弢 录波    //置标志要求保存到FLASH中
       /* for(i=0;i<WAVE_DATA_LEN_BYTE;i++)
              g_sSoeData.m_gRecBuff[i] = 0;*/
    } 
	/*
    if(g_FirstACTFlag != 0  && g_gProcCnt[PC_JAG_ACT] <= 3)    ////////只要8脉冲延时结束，就会进到这里来。检测到8脉冲电流存故障，判断重发；未检测到8脉冲电流，重发。
    { 
        if(repeat_flag1 == 0)  //投切结束
        {
           if(g_FinishACTFlag == 0x55)
           {
               g_gRmtInfo[0] |= YX_ACTION_SUCCESS;
           }
           g_FinishACTFlag = 0; //清0
           //eight_pulse_flag=0;
           efslatch_flag= g_gProcCntJug[PC_LACTH_TIME];
           uart0_event_flag=0; 
           g_gRmtMeas[RM_ACT_NUM] = 1;
        }
        
        if(g_FirstACTFlag==0x55)
    	{             
              eight_ctl_flag=5;              
              KC1_ON;             ///新电路板                           	 	                   	      
	      sign_RSSI_delay=5;                     //////////////发出有效8脉冲后，置延时，若在延时时间内有子站询问，则认为是故障    
	      pulse_success=0x55; 
                  	
    	     CreatNewSMS(EIGHT_PULSE);                      //产生8脉冲短信 //张弢
			    		     	
    	 
    	    //if((n==0)&&(xinghao_flag==2)&&(repeat_flag1>0))        ///////////说明有电流值小于35A，铁路型信号源，第一次检测到故障
    	    if(repeat_flag1>0)	
		Sign_Repeat(repeat_flag1,1);
	    repeat_flag1=0;	
	}
	else if(g_FirstACTFlag < 3)   //不是有效的脉冲
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



//张弢 SOE存FLASH
void SaveSoeData(void)
{
    //保存单点SOE	
   //Sst26vf064b_Page_WriteBytes(g_unSSoeSaveFlashPtr,g_gSCosBuff[0],11);
   g_unSSoeSaveE2ROMPtr &= 0x3ff0;
   CAT_SpiWriteBytes(g_unSSoeSaveE2ROMPtr,11,g_gSCosBuff[0]); 
   g_unSSoeSaveE2ROMPtr+=16;
   soe_num++;  
   if(soe_num>512)soe_num=512;
   if(g_unSSoeSaveE2ROMPtr>=  EEPADD_SOEENDADR) //写满FLASH，归0重写
   	{
   	g_unSSoeSaveE2ROMPtr = EEPADD_SOESTARTADR;
	soe_num=512;
   	}
   
  return;
}


//==============================================================================
//  函数名称   : ReadSoe
//  功能描述   : 读取一个SEG的SOE
//  输入参数   : pBuf,装读取到的SOE数据，
//                             iSegNo,扇区id，0到SOE_SEG-1
//                            iStartNo,起始序号 ,0到SOE_NUM_SEG-1
//                             iSoeNum,需要读取的SOE个数
//  输出参数   : <无>
//  返回值     : 实际读到的SOE个数
//  其他说明   : 
//  作者       :
//==============================================================================

//张弢 SOE存FLASH
int ReadSoe(unsigned char *pBuf,int iSegNo,int iStartNo,int iSoeNum)
{
  //unsigned char *pAddr;
  int num_cnt = 0,RealNum=0;
  unsigned int j,k,n;
  unsigned char untemp[10];
  WORD YXNo = 0;
    //pAddr = (unsigned char *)(SOE_RECORD_ADD + (iSegNo*512) + 511);    //////检测是否有数据写入
     
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
//张弢 SOE存FLASH



//==============================================================================
//  函数名称   : ReadSoe
//  功能描述   : 读取一个SEG的SOE
//  输入参数   : pBuf,装读取到的SOE数据，
//                             iSegNo,扇区id，0到SOE_SEG-1
//                            iStartNo,起始序号 ,0到SOE_NUM_SEG-1
//                             iSoeNum,需要读取的SOE个数
//  输出参数   : <无>
//  返回值     : 实际读到的SOE个数
//  其他说明   : 
//  作者       :
//==============================================================================

int ReadSoeHistory(unsigned char *pBuf,int iSegNo,int iStartNo,int iSoeNum)
{
  unsigned char *pAddr;
  unsigned char ucTemp[10];
  int num_cnt = 0,RealNum=0;
  int j = 0;
    pAddr = (unsigned char *)(SOE_RECORD_ADD + (iSegNo*512) + 511);    //////检测是否有数据写入
    if(*pAddr == 0x55) // 已有数据写入，发送
    {
      pAddr = (unsigned char *)(SOE_RECORD_ADD + iSegNo*512 + iStartNo*(SOE_DATA_LEN)); 
      for(num_cnt=0;num_cnt<SOE_NUM_SEG;num_cnt++)
      {
        //检测第一个数据是否是有数据
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
          //pAddr++;//每条SOE结尾空一个字节
        }
        else
          break;
       }
   }
  return RealNum;
}
//根据设备实遥信序号得到需要传输的遥信序号
//若返回为0xffff，表示此遥信不在上传点表中
unsigned int RealYxToTsYx(unsigned int RYxNo)
{
  unsigned int TsYxNo = 0xffff;
    for(int m = 0; m < g_ucYxTransNum; m++) //判断该遥信变位是否需要上传
    {
        if(RYxNo == (g_ucYXAddr[m] - 1))    //是需要上传的遥信
        {
            TsYxNo = m; //SOE类型(0-4095
            break;
        }
    }
   return TsYxNo;
}

//==============================================================================
//  函数名称   : 
//  功能描述   : 得到IEC101新的配置点表的遥信信息
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       
//==============================================================================
void SetYxTrans(void)
{
  unsigned int i;//, RIIndex
 // int sYxCount = 0;
  //int dYxCount = 0;

  if((g_ucRefYxYcTs & BIT0) == BIT0)//对遥信根据配置的遥信地址重新排列
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
//  函数名称   : 
//  功能描述   : 得到IEC101新的配置点表的遥测信息
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       
//==============================================================================
void SetYcTrans(void)
{
    unsigned char i;
    if((g_ucRefYxYcTs & BIT1) == BIT1)
    {
      g_ucRefYxYcTs &= NBIT1;
      for(i = 0; i < g_ucYcTransNum; i++)   //计算需要上传的遥测值，按照具体情况进行换算
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
   //       g_sTimer[TM_SENDYC].m_TmFlag = ON;//启动遥测传输
      //  g_FltChgSdYc = 0;
    //  }
    }                                         
}



//==============================================================================
//  函数名称   : RstRMTMeas
//  功能描述   : 复位遥测值
//  输入参数   : 
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       
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
//  函数名称   : RstRMTInfo
//  功能描述   : 复位遥信位
//  输入参数   : 
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       
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
//  函数名称   : CopySoeToFlash
//  功能描述   : 内存中的SOE数据导入外部FLASH
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
/*void CopySoeToFlash(void)
{
     int SaveNum = 0;
      unsigned int BfT;
      unsigned long FSoeAddrOfset;//最新SOE的地址偏移量
      
      
      //保存单点SOE
      if(g_unSSoeSaveFlashPtr == g_unSCosBuffTail) return;

      BfT  = g_unSCosBuffTail;
      if(g_unSSoeSaveFlashPtr <= BfT) 
        SaveNum = BfT - g_unSSoeSaveFlashPtr;
      else                            
        SaveNum = SOE_BUFF_NUM - g_unSSoeSaveFlashPtr + BfT; 
        
      if(SaveNum == 0) return;

      
      //将soe保存到flash
      for(int i = 0; i < SaveNum;i++)
      {
        //判断要不要檫除
        FSoeAddrOfset = g_unSSoeInFlashTail * FSOE_LEN;
        if( !(FSoeAddrOfset % 4096) )//每次檫除都要擦4k空间,256条SOE
        {
          Sector_Erase(FADDR_SOE_START + FSoeAddrOfset);
        }
        Sst26vf064b_Page_WriteBytes(FADDR_SOE_START + FSoeAddrOfset, g_gSCosBuff[g_unSSoeSaveFlashPtr],SOE_DATA_LEN); //把数据保存进入FLASH中
         
        g_unSSoeSaveFlashPtr++;
        g_unSSoeSaveFlashPtr &= (SOE_BUFF_NUM-1);

        g_unSSoeInFlashTail++;
        g_unSSoeInFlashTail &= (FSOE_TOLNUM-1);
        
        //=========判断头指针与尾指针位置================
        //在檫除块时不移动头指针，读SOE记录时判断保存的数据即可
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
      //在E2中保存flash中的SOE尾指针
      CAT_SpiWriteWords(EEPADD_SSOETAIL, 1, &g_unSSoeInFlashTail);
    return;




}
*/
//==============================================================================
//  函数名称   : GetRIStatus
//  功能描述   : 获取遥信信息中的单点状态或双点状态
//  输入参数   : RIIndex:遥信信息表中的全局索引号，具体见constant.h中RMT_INFO部分
//             : RmtInfoFlag 0 =g_gRmtInfo；1=g_unYxTrans；2=g_unDYxTrans
//  输出参数   ：<无>
//  返回值     : 1或0，2,3
//  其他说明   : 每个遥信信息占两个bit位
//  作者       ：
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
    iRtStVal = pRmtInfo[(RIIndex >> 3)] & iStVal;//取出要判断的位 
    iRtStVal = iRtStVal >> ((RIIndex & 7)*2);//将要判断的位右移至最低位
    return iRtStVal;	
}
//==============================================================================
//  函数名称   : SetRIStatus
//  功能描述   : 设置遥信信息中的单点状态，根据实际写入1或者0,2,3
//  输入参数   : RIIndex:遥信信息表中的全局索引号，具体见constant.h中RMT_INFO部分
//                         : Status: 写入状态，1或者0,2,3
//             : RmtInfoFlag 0 =g_gRmtInfo；1=g_unYxTrans；2=g_unDYxTrans
//  输出参数   : <无>
//  返回值     : 
//  其他说明   : 每个遥信信息占两个bit位
//  作者       ：
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
   pRmtInfo[(RIIndex >> 3)] &= ~iStVal;//清除要设置的位
   pRmtInfo[(RIIndex >> 3)] |=  ((Status & 3) << ((RIIndex & 7)*2));//将新状态写入要设置的位
   
}

//Flag = 1 表示扫描硬遥信出来的数据 2 表示遥信对点产生的SOE(不做合成)
void CreatNewSoe(unsigned int SoeType,unsigned long SoeValue,unsigned char Flag)
{//SoeType:实遥信
    unsigned int TsSoeType;//,TsSoeDNum,TsSoeNum;
    //unsigned int iDinID = 0;

      
      if(pGprs != null) SETBIT(pGprs->m_gRmtChgFlag,SoeType,1);
      if(pDbg != null)  SETBIT(pDbg->m_gRmtChgFlag,SoeType,1);
      //保存SOE之前就区分单点和双点，因为保存的soe类型采用遥信的信息体地址
      //这样在协议中传输soe就不用再分别处理信息体地址

      TsSoeType = g_gRunPara[RP_SYX_INFADDR] + SoeType - 1;   //- 1 + TsSoeType - g_ucSYxTrsStartId
        
 
      g_yxChangeflag = 0xffff;   
     // g_ucRefYxYcTs |= BIT0;
      //int iDinID = SoeType;
      unsigned int *pchSoeTm;
      unsigned char *pchSoeBuf;
      
      if((int)SoeType >= 0 && SoeType < RMT_INFO_NUM + 2)  //注：增加2路硬遥信
      {
         
          pchSoeTm = g_sRtcManager.m_gRealTimer;
          pchSoeBuf = g_gSCosBuff[0];//张弢 SOE存FLASH
          pchSoeBuf[SOE_FLAG] = SOEVALID;
          pchSoeBuf[SOE_TYPEL] = TsSoeType; //SOE类型(0-4095
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
    //处理soe尾指针
    		SaveSoeData();//存1条，张弢 SOE存FLASH
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
             	CAT_SpiWriteWords(EEPADD_SOE_E2ROMADR, 4, untemp);  //保存到EEPROM中
        	CAT_SpiWriteWords(EEPADDBK_SOE_E2ROMADR, 4, untemp);    //保存到EEPROM的备份区中

}
//==============================================================================
//  函数名称   : DelALLSOE
//  功能描述   : 删除所有SOE
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void DelALLSOE(void)
{
		unsigned int untemp[4];

		untemp[0]=EEPADD_SOESTARTADR;//(unsigned int)g_unSSoeSaveFlashPtr;
		untemp[1]=EEPADD_SOESTARTADR;//(unsigned int)(g_unSSoeSaveFlashPtr>>16);
		untemp[2]=0;untemp[3]=0;
             	CAT_SpiWriteWords(EEPADD_SOE_E2ROMADR,4, untemp);  //保存到EEPROM中
        	CAT_SpiWriteWords(EEPADDBK_SOE_E2ROMADR, 4, untemp);    //保存到EEPROM的备份区中
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
//  函数名称   : CheckRECPara
//  功能描述   : 从EEPROM中读取录波条数及录波所在当前页
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：
//==============================================================================
void RECParaPro(void)
{

}


//==============================================================================
//  函数名称   : SaveRecData
//  功能描述   : 按照COMTRADE格式整理故障录波数据，并分批次保存到Flash中
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   :Sst26vf064，126个BLOCK，每个BLOCK有64kByte 
//                  FADDR_RECORDER_DATA 起始地址 
//                  每个录波640条记录，每条5个遥测值，每个遥测值2字节 
//                  共6400字节 占2个SECTOR，8192(0x2000)字节
//                  存储50条记录
//  作者       : 张弢
//==============================================================================

void SaveRecData(void)
{
  unsigned char *PTemp;   //临时变量数组
  unsigned int j,ulen,uRect,temp[4],unum;//
  unsigned long ulAddr;

  
  //对于故障录波，640点，每点5个遥测值，每个遥测值2字节
  //共640*5*2=6400字节
  //Sst26vf064每page有256字节，共25页  
  //ulAddr = (unsigned long)(FADDR_RECORDER_DATA+g_sRecData.m_gRecCNum*4)<<16;//flash地址
  ulAddr = FADDR_RECORDER_DATA+(unsigned long)(g_sRecData.m_gRecCNum)*0x2000;//flash地址
  uRect = g_sRecData.m_unRecAcTail;//m_gRecAc数组的文职//1次最多写256字节
  PTemp = (unsigned char *)&(g_sRecData.m_gRecAc[uRect][0]);
  //PTemp = (unsigned char *)&(g_sRecData.m_gRecAc[0][0]);
  unum = uRect*10;
  for(j=0;j<6400;)//对于鼓掌
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

//张弢 录波 需要写入CFG文件
  
//张弢 录波 需要写文件目录  
  temp[1]=g_sRecData.m_gRecCNum;
  gRecorder_filecfg.FileName=temp[1];//设为文件名
  gRecorder_filecfg.CFG_Leng=330;//strlen(ComtrderCfg1);//
  gRecorder_filecfg.TOTAL_Leng=64;//gRecorder_filecfg.CFG_Leng+6400;//
  gRecorder_filecfg.CFG_Samp=4000;	//采样频率 
#ifdef SD_101S
  gRecorder_filecfg.CFG_Samp=2000;	//采样频率 
#endif
  gRecorder_filecfg.CFG_EndSamp=640;//采样个数 
  gRecorder_filecfg.TOTAL_Leng=gRecorder_filecfg.CFG_EndSamp;//gRecorder_filecfg.CFG_Leng+6400;//  
  gRecorder_filecfg.comtrade_time[RTC_MICROSEC]=g_sRecData.m_gFaultRecOver[REC_MSL] ;
  gRecorder_filecfg.comtrade_time[RTC_SEC]=g_sRecData.m_gFaultRecOver[REC_MSH];
  gRecorder_filecfg.comtrade_time[RTC_MINUT]=g_sRecData.m_gFaultRecOver[REC_MINU] ;
  gRecorder_filecfg.comtrade_time[RTC_HOUR]=g_sRecData.m_gFaultRecOver[REC_HOUR] ;
  gRecorder_filecfg.comtrade_time[RTC_DATE]=g_sRecData.m_gFaultRecOver[REC_DAY] ;
  gRecorder_filecfg.comtrade_time[RTC_MONTH]=g_sRecData.m_gFaultRecOver[REC_MONTH];
  gRecorder_filecfg.comtrade_time[RTC_YEAR]=g_sRecData.m_gFaultRecOver[REC_YEAR]+2000; 
  ulAddr =FADDR_RECORDER_DATA+(unsigned long)(g_sRecData.m_gRecCNum+1)*0x2000-256;	
  Sst26vf064b_Page_WriteBytes(ulAddr,(unsigned char *)&gRecorder_filecfg,sizeof(gRecorder_filecfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来 

//提前擦除下2个使用的SECTOR 
if(pDbg != null)pDbg->m_luok_filename=g_sRecData.m_gRecCNum;
if(pGprs != null)pGprs->m_luok_filename=g_sRecData.m_gRecCNum;
  g_sRecData.m_gRecCNum++;
  if(g_sRecData.m_gRecCNum>MAX_REC_NUM)g_sRecData.m_gRecCNum=0;
  //ulAddr = (unsigned long)(FADDR_RECORDER_DATA+g_sRecData.m_gRecCNum*4)<<16;//flash地址 
  ulAddr = FADDR_RECORDER_DATA+(unsigned long)(g_sRecData.m_gRecCNum)*0x2000;//flash地址
  //g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址  
  Sector_Erase(ulAddr);//ERASE 1个BLOCK 
  delayms(100);WatchDog();
  Sector_Erase(ulAddr+0x1000);//ERASE 1个BLOCK 
  delayms(100);WatchDog();

// 写入EEPROM,总录波条数和将写入的地址
  g_sRecData.m_gRecANum++;
  if(g_sRecData.m_gRecANum>MAX_REC_NUM)g_sRecData.m_gRecANum=MAX_REC_NUM;
  temp[0]=g_sRecData.m_gRecANum;//录波总条数1~32
  temp[1]=g_sRecData.m_gRecCNum;//录波当前位置0~31
  CAT_SpiWriteWords(EEPADD_LUBONUM, 2, temp);   
//提前擦除下1个使用的SECTOR,文件目录用    
  //ulAddr =FADDR_RECORDER_INFO+ (long)temp[1]*(long)FLINEADDR;  
  //Sector_Erase(ulAddr);//预先擦除下一个sector
  //g_gRmtInfo[YX_LBOK]=1;//录波完成遥信置位
  if(pDbg != null) pDbg->m_luok=1;
  if(pGprs != null) pGprs->m_luok=1;  
  //g_YXLBOKCounter=60;//录波完成遥信持续60s

}
//==============================================================================
//  函数名称   : SaveActRecData
//  功能描述   : 整理动作录波数据，并分批次保存到Flash中
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   :Sst26vf064，126个BLOCK，每个BLOCK有64kByte 
//              0x300000~0x34FFFF
//              0x350000~0x39FFFF
//              ………………
//              
//              0x2F0000~0x2FFFFF  每5个BLOCK存一次动作录波
//  作者       : 张弢
//==============================================================================

void SaveActRecData(void)
{
  unsigned char *PTemp;   //临时变量数组
  unsigned int j,ulen,pagelen;//,unum=0;//
  unsigned long ulAddr;
  unsigned int temp[8];
  static unsigned int save_page_num=0;

  if(g_sRecData.m_ucActRecSavingFlag == OFF)
  	return;
  //对于动作录波，每点5个遥测值，每个遥测值2字节
  //不定长最多25秒
  //每个周波采集16点
  //g_sRecData.m_gRecAc 可以存640点，共6400字节
  //分2次存，先存12x256=3072字节，再存13x256=3328
  PTemp = (unsigned char *)&(g_sRecData.m_gRecAc[0][0]);//存前320点
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
  for(j=0;j<pagelen;j++)//对于
  { 
         ulen = 256;	  
	  Sst26vf064b_Page_WriteBytes(g_sRecData.m_gActRecAdr,PTemp,ulen);
	  PTemp+=256;          
	 g_sRecData.m_gActRecAdr += 256;//m_gActRecAdr 记录在FLASH中的地址
  }
 WatchDog();
 save_page_num = save_page_num+pagelen;
 if(g_sRecData.m_LuboType == LuboType_XH)
 	{
 	if(save_page_num>125)//熄弧录波存储空间最大为32kB，共128page
 		g_sRecData.m_ucActRecStart = OFF;
 	}
else if(g_sRecData.m_LuboType == LuboType_ACT)	
	{
 	if(save_page_num>2300)//动作录波存储空间最大为9*64kB，共2304page
 		g_sRecData.m_ucActRecStart = OFF;	
	}

if((g_sRecData.m_ucActRecStart == OFF))
{
	save_page_num =0;
	g_sRecData.m_ucActRecStart = NO;
//张弢 录波 需要写入CFG文件  
//张弢 录波 需要写文件目录  
  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	temp[1]=g_sRecData.m_gXHRecCNum;
	gRecorder_filecfg.FileName=temp[1]+MAX_REC_NUM+MAX_ACTREC_NUM+2;//设为文件名
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{
  	temp[1]=g_sRecData.m_gACTRecCNum;
	gRecorder_filecfg.FileName=temp[1]+MAX_REC_NUM+1;//设为文件名
	}  
  gRecorder_filecfg.CFG_Leng=330;//strlen(ComtrderCfg1);//
  gRecorder_filecfg.CFG_Samp=800;	//采样频率 
#ifdef SD_101S
  gRecorder_filecfg.CFG_Samp=800;	//采样频率 
#endif  
  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;//flash地址  g_sRecData.m_gActRecAdr/10
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{  	
    	ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;//flash地址  g_sRecData.m_gActRecAdr/10
  	}
  gRecorder_filecfg.CFG_EndSamp=(g_sRecData.m_gActRecAdr-ulAddr)/10;//采样个数 
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
  Sst26vf064b_Page_WriteBytes(ulAddr,(unsigned char *)&gRecorder_filecfg,sizeof(gRecorder_filecfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来 

//提前擦除下9个使用的BLOCK 
  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	if(pDbg != null)pDbg->m_luok_filename=g_sRecData.m_gXHRecCNum;
	if(pGprs != null)pGprs->m_luok_filename=g_sRecData.m_gXHRecCNum;
  	g_sRecData.m_gXHRecCNum++;
 	 if(g_sRecData.m_gXHRecCNum>MAX_XHREC_NUM)g_sRecData.m_gXHRecCNum=0;
  	ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;  
  	g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址  
  	g_sRecData.m_EraseBlock = YES;
// 写入EEPROM,总录波条数和将写入的地址
  	g_sRecData.m_gXHRecANum++;
  	if(g_sRecData.m_gXHRecANum>MAX_XHREC_NUM)g_sRecData.m_gXHRecANum=MAX_XHREC_NUM;
  	temp[0]=g_sRecData.m_gXHRecANum;//录波总条数1~32
  	temp[1]=g_sRecData.m_gXHRecCNum;//录波当前位置0~31
  	CAT_SpiWriteWords((EEPADD_LUBONUM+8), 2, temp); 
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{  
  	if(pDbg != null)pDbg->m_luok_filename=g_sRecData.m_gACTRecCNum;
	if(pGprs != null)pGprs->m_luok_filename=g_sRecData.m_gACTRecCNum;
  	g_sRecData.m_gACTRecCNum++;
 	 if(g_sRecData.m_gACTRecCNum>MAX_ACTREC_NUM)g_sRecData.m_gACTRecCNum=0;
  	ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;  
  	g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址  
  	g_sRecData.m_EraseBlock = ON;
// 写入EEPROM,总录波条数和将写入的地址
  	g_sRecData.m_gACTRecANum++;
  	if(g_sRecData.m_gACTRecANum>MAX_ACTREC_NUM)g_sRecData.m_gACTRecANum=MAX_ACTREC_NUM;
  	temp[0]=g_sRecData.m_gACTRecANum;//录波总条数1~32
  	temp[1]=g_sRecData.m_gACTRecCNum;//录波当前位置0~31
  	CAT_SpiWriteWords((EEPADD_LUBONUM+4), 2, temp); 
  	}
//提前擦除下1个使用的SECTOR,文件目录用    
  //ulAddr =FADDR_RECORDER_INFO+ (long)temp[1]*(long)FLINEADDR;  
  //Sector_Erase(ulAddr);//预先擦除下一个sector
    	
  g_gRmtInfo[YX_LBOK]=1;//录波完成遥信置位
  if(pDbg != null) pDbg->m_luok=1;
  if(pGprs != null) pGprs->m_luok=1; 
  g_YXLBOKCounter=1;//录波完成遥信持续60s
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
//张弢 录波 需要写入CFG文件  
//张弢 录波 需要写文件目录  
  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	temp[1]=g_sRecData.m_gXHRecCNum;
	gRecorder_filecfg.FileName=temp[1]+MAX_REC_NUM+MAX_ACTREC_NUM+2;//设为文件名
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{
  	temp[1]=g_sRecData.m_gACTRecCNum;
	gRecorder_filecfg.FileName=temp[1]+MAX_REC_NUM+1;//设为文件名
	}  
  gRecorder_filecfg.CFG_Leng=330;//strlen(ComtrderCfg1);//
  gRecorder_filecfg.CFG_Samp=800;	//采样频率 
#ifdef SD_101S
  gRecorder_filecfg.CFG_Samp=800;	//采样频率 
#endif  
  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;//flash地址  g_sRecData.m_gActRecAdr/10
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{  	
    	ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;//flash地址  g_sRecData.m_gActRecAdr/10
  	}
  gRecorder_filecfg.CFG_EndSamp=(g_sRecData.m_gActRecAdr-ulAddr)/10;//采样个数 
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
  Sst26vf064b_Page_WriteBytes(ulAddr,(unsigned char *)&gRecorder_filecfg,sizeof(gRecorder_filecfg)); //不在这里保存gRecorder_cfg的值是因为三相的录波不一定都能传上来 

//提前擦除下9个使用的BLOCK 
  if(g_sRecData.m_LuboType == LuboType_XH)
  	{
  	g_sRecData.m_gXHRecCNum++;
 	 if(g_sRecData.m_gXHRecCNum>MAX_XHREC_NUM)g_sRecData.m_gXHRecCNum=0;
  	ulAddr = FADDR_RECORDER_XHDATA+(unsigned long)(g_sRecData.m_gXHRecCNum)*0x8000;  
  	g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址  
  	g_sRecData.m_EraseBlock = YES;
// 写入EEPROM,总录波条数和将写入的地址
  	g_sRecData.m_gXHRecANum++;
  	if(g_sRecData.m_gXHRecANum>MAX_XHREC_NUM)g_sRecData.m_gXHRecANum=MAX_XHREC_NUM;
  	temp[0]=g_sRecData.m_gXHRecANum;//录波总条数1~32
  	temp[1]=g_sRecData.m_gXHRecCNum;//录波当前位置0~31
  	CAT_SpiWriteWords((EEPADD_LUBONUM+8), 2, temp); 
	}
  else if(g_sRecData.m_LuboType == LuboType_ACT)	
  	{   
  	g_sRecData.m_gACTRecCNum++;
 	 if(g_sRecData.m_gACTRecCNum>MAX_ACTREC_NUM)g_sRecData.m_gACTRecCNum=0;
  	ulAddr = FADDR_RECORDER_ACTDATA+(unsigned long)(g_sRecData.m_gACTRecCNum)*0x90000;  
  	g_sRecData.m_gActRecAdr = ulAddr;//更新flash地址  
  	g_sRecData.m_EraseBlock = ON;
// 写入EEPROM,总录波条数和将写入的地址
  	g_sRecData.m_gACTRecANum++;
  	if(g_sRecData.m_gACTRecANum>MAX_ACTREC_NUM)g_sRecData.m_gACTRecANum=MAX_ACTREC_NUM;
  	temp[0]=g_sRecData.m_gACTRecANum;//录波总条数1~32
  	temp[1]=g_sRecData.m_gACTRecCNum;//录波当前位置0~31
  	CAT_SpiWriteWords((EEPADD_LUBONUM+4), 2, temp); 
  	}
//提前擦除下1个使用的SECTOR,文件目录用    
  //ulAddr =FADDR_RECORDER_INFO+ (long)temp[1]*(long)FLINEADDR;  
  //Sector_Erase(ulAddr);//预先擦除下一个sector
    	
  g_gRmtInfo[YX_LBOK]=1;//录波完成遥信置位
  if(pDbg != null) pDbg->m_luok=1;
  if(pGprs != null) pGprs->m_luok=1; 
  g_YXLBOKCounter=1;//录波完成遥信持续60s
  g_sRecData.m_ucActRecStart = CLOSE;
}
}
/*
void SaveRecData(void)
{
    unsigned long ulAddr;       //AT45DB地址变量
    unsigned long ulTempAddr;   //临时保存Flash页数
  //  unsigned int i = 0;
    //unsigned int j;
   // unsigned int unAddCount;    //EEPROM存储数据加和校验值
    unsigned int *PTemp;   //临时变量数组  
    unsigned int PunTempData[2];
    //unsigned int unTemp[6];
    static unsigned char ucSaveFlg[5];    //临时保存各通道存储标志
    if((g_sRecData.m_CurWritePage - FADDR_REC_START) >= 16384)         //保存1024条录波记录12288
    g_sRecData.m_CurWritePage = FADDR_REC_START;
    //为了应对可能发生的短时多次操作影响录波数据，数据存储顺序调整为先存储合分闸操作数据
    ulTempAddr = ((g_sRecData.m_CurWritePage - FADDR_REC_START) % REC_LEAF_NUM);
    ulAddr = ((unsigned long)g_sRecData.m_CurWritePage)*256;  //存储器变绝对地址
    switch(ulTempAddr)
    {
    case 0x00:  
          //存储启动该录波的SOE 长度(SOE_DATA_LEN - 1)
          Sector_Erase(ulAddr);                          //存储之前先擦除整块（4K）
          g_sRecData.m_gFaultRecSOE[REC_TOTAL_LENL] =  0xCA;   //每条录波数据10 + 224*5*2 = 2250  0x08CA
          g_sRecData.m_gFaultRecSOE[REC_TOTAL_LENH] =  0x08;
          
          if(g_sRecData.m_gRecNum % 1024 == 0)           // 存储512条视情况而定
          {
              //g_sRecData.m_gRecNum = 0;
              g_sRecData.m_CurWritePage = FADDR_REC_START;
          }
          
          g_sRecData.m_gRecNum += 1;              //录波条数加1
          PunTempData[0] = g_sRecData.m_gRecNum;
          PunTempData[1] = g_sRecData.m_CurWritePage;
          //CAT_SpiWriteWord(EEPADD_REC_NUM,g_sRecData.m_gRecNum);  //存储录波条数

         // CAT_SpiWriteWord(EEPADD_REC_POS,g_sRecData.m_CurWritePage); //存储最新录波数据所在的at45页数到EEPROM
          CAT_SpiWriteWords(EEPADD_REC_NUM,2,PunTempData);  //读出录波条数及最新录波数据所在地址页数

        //  unAddCount = PunTempData[0] + PunTempData[1];   //数据的加和校验值
        //  CAT_SpiWriteWord(EEPADD_REC_DATA,unAddCount);          //存储数据的加和校验值到EEPROM
          
          
          Sst26vf064b_Page_WriteBytes(ulAddr,g_sRecData.m_gFaultRecSOE,7);  // 占10字节    
          Sst26vf064b_Page_WriteBytes(ulAddr + 7,g_sRecData.m_gFaultRecOver,7);
          ulAddr += 30;                 //预留够15字空间  
          g_sRecData.m_CurWritePage++;                   //Flash页数增加
          break;
     case 0x01:
          PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[0][g_sRecData.m_unRecAcTail]);        //存储保存动作前10个周波动作后6个周波的交流信号波形          
          if(g_sRecData.m_unRecAcTail > 112)  //剩下的数据不够一页，先全写进去
          {
              ucSaveFlg[0] = 0xAA;
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(REC_AC_LEN-g_sRecData.m_unRecAcTail));   //存储第一个通道 
              ulAddr += ((REC_AC_LEN-g_sRecData.m_unRecAcTail)*2);
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[0][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 + g_sRecData.m_unRecAcTail - REC_AC_LEN));
         
          }
          else
          {
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //存储第一个通道 
              ucSaveFlg[0] = 0x55;
          }
          g_sRecData.m_CurWritePage++;                   //Flash页数增加
          break;
          
    case 0x02:
          if(ucSaveFlg[0] == 0xAA)  //已经转回到数据头
          {
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[0][112 + g_sRecData.m_unRecAcTail - REC_AC_LEN]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //存储第一个通道 
          }
          else if(ucSaveFlg[0] == 0x55)
          {
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[0][g_sRecData.m_unRecAcTail + 112]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 - g_sRecData.m_unRecAcTail));   //存储第一个通道
              ulAddr += ((112 - g_sRecData.m_unRecAcTail)*2);
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[0][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(g_sRecData.m_unRecAcTail));              
          }
          ucSaveFlg[0] = 0;

          g_sRecData.m_CurWritePage++;                   //Flash页数增加
          break;
     case 0x03:
          PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[1][g_sRecData.m_unRecAcTail]);        //存储保存动作前10个周波动作后6个周波的交流信号波形          
          if(g_sRecData.m_unRecAcTail > 112)  //剩下的数据不够一页，先全写进去
          {
              ucSaveFlg[1] = 0xAA;
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(REC_AC_LEN-g_sRecData.m_unRecAcTail));   //存储第一个通道 
              ulAddr += ((REC_AC_LEN-g_sRecData.m_unRecAcTail)*2);
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[1][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 + g_sRecData.m_unRecAcTail - REC_AC_LEN));
         
          }
          else
          {
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //存储第一个通道 
              ucSaveFlg[1] = 0x55;
          }

          g_sRecData.m_CurWritePage++;                   //Flash页数增加
          break;          
    case 0x04:
          if(ucSaveFlg[1] == 0xAA)  //已经转回到数据头
          {
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[1][112 + g_sRecData.m_unRecAcTail - REC_AC_LEN]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //存储第一个通道 
          }
          else if(ucSaveFlg[1] == 0x55)
          {
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[1][g_sRecData.m_unRecAcTail + 112]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 - g_sRecData.m_unRecAcTail));   //存储第一个通道
              ulAddr += ((112 - g_sRecData.m_unRecAcTail)*2);
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[1][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(g_sRecData.m_unRecAcTail));              
          }
          ucSaveFlg[1] = 0;

          g_sRecData.m_CurWritePage++;                   //Flash页数增加
          break;
    case 0x05:
          PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[2][g_sRecData.m_unRecAcTail]);        //存储保存动作前10个周波动作后6个周波的交流信号波形          
          if(g_sRecData.m_unRecAcTail > 112)  //剩下的数据不够一页，先全写进去
          {
              ucSaveFlg[2] = 0xAA;
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(REC_AC_LEN-g_sRecData.m_unRecAcTail));   //存储第一个通道 
              ulAddr += ((REC_AC_LEN-g_sRecData.m_unRecAcTail)*2);
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[2][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 + g_sRecData.m_unRecAcTail - REC_AC_LEN));
         
          }
          else
          {
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //存储第一个通道 
              ucSaveFlg[2] = 0x55;
          }

          g_sRecData.m_CurWritePage++;                   //Flash页数增加
          break;          
    case 0x06:
          if(ucSaveFlg[2] == 0xAA)  //已经转回到数据头
          {
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[2][112 + g_sRecData.m_unRecAcTail - REC_AC_LEN]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //存储第一个通道 
          }
          else if(ucSaveFlg[2] == 0x55)
          {
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[2][g_sRecData.m_unRecAcTail + 112]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 - g_sRecData.m_unRecAcTail));   //存储第一个通道
              ulAddr += ((112 - g_sRecData.m_unRecAcTail)*2);
              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[2][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(g_sRecData.m_unRecAcTail));              
          }
          ucSaveFlg[2] = 0;

          g_sRecData.m_CurWritePage++;                   //Flash页数增加
          break;

    case 0x07:
 //张弢 录波I0U0         PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[3][g_sRecData.m_unRecAcTail]);        //存储保存动作前10个周波动作后6个周波的交流信号波形          
          if(g_sRecData.m_unRecAcTail > 112)  //剩下的数据不够一页，先全写进去
          {
              ucSaveFlg[3] = 0xAA;
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(REC_AC_LEN-g_sRecData.m_unRecAcTail));   //存储第一个通道 
              ulAddr += ((REC_AC_LEN-g_sRecData.m_unRecAcTail)*2);
 //张弢 录波I0U0             PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[3][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 + g_sRecData.m_unRecAcTail - REC_AC_LEN));
         
          }
          else
          {
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //存储第一个通道 
              ucSaveFlg[3] = 0x55;
          }
    
          g_sRecData.m_CurWritePage++;                   //Flash页数增加
          break;          
    case 0x08:
          if(ucSaveFlg[3] == 0xAA)  //已经转回到数据头
          {
 //张弢 录波I0U0             PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[3][112 + g_sRecData.m_unRecAcTail - REC_AC_LEN]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //存储第一个通道 
          }
          else if(ucSaveFlg[3] == 0x55)
          {
//张弢 录波I0U0              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[3][g_sRecData.m_unRecAcTail + 112]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 - g_sRecData.m_unRecAcTail));   //存储第一个通道
              ulAddr += ((112 - g_sRecData.m_unRecAcTail)*2);
//张弢 录波I0U0              PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[3][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(g_sRecData.m_unRecAcTail));              
          }
          ucSaveFlg[3] = 0;
     
          g_sRecData.m_CurWritePage++;                   //Flash页数增加
          break;
    case 0x09:
 //张弢 录波I0U0         PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[4][g_sRecData.m_unRecAcTail]);        //存储保存动作前10个周波动作后6个周波的交流信号波形          
          if(g_sRecData.m_unRecAcTail > 112)  //剩下的数据不够一页，先全写进去
          {
              ucSaveFlg[4] = 0xAA;
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(REC_AC_LEN-g_sRecData.m_unRecAcTail));   //存储第一个通道 
              ulAddr += ((REC_AC_LEN-g_sRecData.m_unRecAcTail)*2);
 //张弢 录波I0U0             PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[4][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 + g_sRecData.m_unRecAcTail - REC_AC_LEN));
         
          }
          else
          {
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //存储第一个通道 
              ucSaveFlg[4] = 0x55;
          }

          g_sRecData.m_CurWritePage++;                   //Flash页数增加
          break;          
    case 0x0a:
          if(ucSaveFlg[4] == 0xAA)  //已经转回到数据头
          {
 //张弢 录波I0U0             PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[4][112 + g_sRecData.m_unRecAcTail - REC_AC_LEN]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,112);   //存储第一个通道 
          }
          else if(ucSaveFlg[4] == 0x55)
          {
 //张弢 录波I0U0             PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[4][g_sRecData.m_unRecAcTail + 112]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(112 - g_sRecData.m_unRecAcTail));   //存储第一个通道
              ulAddr += ((112 - g_sRecData.m_unRecAcTail)*2);
 //张弢 录波I0U0             PTemp = (unsigned int*)&(g_sRecData.m_gRecAc[4][0]);
              Sst26vf064b_Page_WriteWords(ulAddr,(unsigned int*)PTemp,(g_sRecData.m_unRecAcTail));              
          }
          ucSaveFlg[4] = 0;

          g_sRecData.m_CurWritePage += 6;                   //Flash页数增加
          //数据保存完毕则清空标志
         // g_sRecData.m_ucOperRecFlag = 0;
          g_sRecData.m_ucRecSavingFlag = OFF;

          g_sRecData.m_unRecAcTail = 0;
       //   g_sRecData.m_unRecAcBefTail = 0;
          g_sRecData.m_unRecAcLockCnt = 0;
          g_sRecData.m_ucRecStart = OFF;
//		  
          if(g_I0RmtNum >= 8)     //检测到有效电流
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

    //在EEPROM中建立索引
      
}
*/

//==============================================================================

//  函数名称   : SaveLoad

//  功能描述   : 把负荷统计的数据保存到flash中

//  输入参数   : <无>

//  输出参数   : <无>

//  返回值     : <无>

//  其他说明   : 记录整点负荷数据，保存格式为

//                   起始码(1)  记录字节数(1)   时间(5)     内容(X)

//                   0xEA         L             YYMMDDhhmm     xx   

//  作者       : 

//==============================================================================

//unsigned char ucLOADData[64][7]={0};//for test

//unsigned char min = 0;//for test

void SaveLoad(void)
{
    unsigned char LoadData[32];
    unsigned char LoadDataPtr=0;
    unsigned long FLoadAddr;
    unsigned int FLoadInfo[FLOADINFONUM];      //从EEPROM中读取出来的FLASH中保存负荷记录的相关信息，总条数+即将存储记录的位置+最老一条记录的位置+校验
    LoadData[LoadDataPtr++] = 0xEA;     //起始码
    LoadData[LoadDataPtr++] = 5 + 2*RMT_MEAS_NUM;//RM_LOADI_NUM*2; //单条负荷记录的长度为5字节时间加上负荷电流数据
    //时间 记录年、月、日、时、分
    LoadData[LoadDataPtr++] = g_sRtcManager.m_gRealTimer[RTC_YEAR] - 2000;
    LoadData[LoadDataPtr++] = g_sRtcManager.m_gRealTimer[RTC_MONTH];
    LoadData[LoadDataPtr++] = g_sRtcManager.m_gRealTimer[RTC_DATE];
    LoadData[LoadDataPtr++] = g_sRtcManager.m_gRealTimer[RTC_HOUR];
    LoadData[LoadDataPtr++] = g_sRtcManager.m_gRealTimer[RTC_MINUT];
    memcpy(&LoadData[LoadDataPtr],&g_gRmtMeas[0],2*RMT_MEAS_NUM);
    CAT_SpiReadWords(EEPADD_LOADNUM, FLOADINFONUM, FLoadInfo);
    if((FLoadInfo[FLOAD_CS] != FLoadInfo[FLOAD_TOTALNUM] + FLoadInfo[FLOAD_NEW] + FLoadInfo[FLOAD_OLD]) || FLoadInfo[FLOAD_TOTALNUM] > FLASH_LOAD_MAXNUM || FLoadInfo[FLOAD_NEW] > FLASH_LOAD_MAXNUM)//如果FLASH地址不在负荷记录保存区域内
    {
        FLoadInfo[FLOAD_TOTALNUM] = 0; //清空负荷记录总条数
        FLoadInfo[FLOAD_NEW] = 0; //即将存储的记录与第一条记录的偏移位置(不是已存储的最新一条记录的位置)
        FLoadInfo[FLOAD_OLD] = 0;//最老一条记录的位置
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
    Sst26vf064b_Page_WriteBytes(FLoadAddr, LoadData, 32);//把数据保存到FLASH中   

    if(FLoadInfo[FLOAD_TOTALNUM] < FLASH_LOAD_MAXNUM)//如果没有存满，则FLASH中保存的负荷记录条数增加
    {
        FLoadInfo[FLOAD_TOTALNUM]++;
    }
    FLoadInfo[FLOAD_NEW]++;
    if(FLoadInfo[FLOAD_NEW] >= FLASH_LOAD_MAXNUM)
    {
        FLoadInfo[FLOAD_NEW] = 0;
    }
    FLoadInfo[FLOAD_CS] = FLoadInfo[FLOAD_NEW] + FLoadInfo[FLOAD_OLD] + FLoadInfo[FLOAD_TOTALNUM];
    //在EEPROM中记录最新一条负荷记录的位置
    CAT_SpiWriteWords(EEPADD_LOADNUM, FLOADINFONUM, FLoadInfo);
}
//==============================================================================
//  函数名称   : SaveLOG
//  功能描述   : 将LOG数据存入内存中的sLOG_DATA g_sLogData[MAX_LOG_NUM] 存入FLASH
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 
//  作者       : ZT
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
//  函数名称   : SaveFlashLOG
//  功能描述   : 将内存中的sLOG_DATA g_sLogData[MAX_LOG_NUM] 存入FLASH
//  输入参数   : <无>
//  输出参数   : <无>
//  返回值     : <无>
//  其他说明   : 存入地址 FADDR_LOG_DATA      0x720000-0x729000;32字节 1152条；LOG_RECORDER log_recorded
//  作者       : ZT
//==============================================================================

void SaveFlashLOG(void)
{
    unsigned char saveflag=0;
	long fLoadAddr;
    //unsigned char LoadDataPtr=0;
    //unsigned long FLoadAddr;
    unsigned int FLogInfo[FLOGINFONUM];      //从EEPROM中读取出来的FLASH中保存负荷记录的相关信息，总条数+即将存储记录的位置+最老一条记录的位置+校验
	
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

