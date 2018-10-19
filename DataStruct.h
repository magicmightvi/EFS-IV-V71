#ifndef _DATASTRUCT_H

    #define _DATASTRUCT_H
#ifdef __cplusplus
extern "C"
{
#endif  
  
    #include <string.h>
    #include "stdio.h"
    #include <math.h>
    #include "msp430x54xA.h"
    #include "PUBLIC\mytypes.h"
    #include "Constant.h"
    #include "FuncDef.h"
    #include "RemoteUpdata.h"
    #include "RecorderWave.h"  
//	#include "SMS\PrtcSms.h"
     
// #pragma location=0x0FF00 
 const unsigned char VerNum[]="Ver75.00";
const unsigned char VerNum_INLCD=75;
//================================== LOG日志模块 ============================================ 
struct sLOG_DATA//1条LOG日志的内容
{
	unsigned char m_gLogType;
	unsigned char m_gLogValu;
	unsigned char m_gLogTimer[RTC_NUM+1];//年 月 日 时 分 秒 毫秒 毫秒
	unsigned int m_gRmtMeas[8];//U0 UA UB UC I0 UPT CSQ UCAP  或者8个电流  
};

//================================== AD采集模块 ============================================
struct sSAMPLE_DATA
  {
    int m_gAcAdcData[AC_AD_CHAN_NUM][AC_BUFF_LEN];//交流电压电流的AD采集缓冲区，一周波采16个点，保存2个周波的数据缓冲，用于永磁驱动结束的故障判断 
    unsigned int m_unAcDataTail;
  };


#ifdef _DRIVER_AD
  unsigned int g_gRunInfo[RUN_INFO_NUM+1];//运行信息，保存装置运行的部分信息，比如合分闸次数__no_init 
  struct sSAMPLE_DATA g_sSampleData;//AD采集数据


#else
  extern unsigned int g_gRunInfo[RUN_INFO_NUM+1];//运行信息，保存装置运行的部分信息，比如合分闸次数
  extern struct sSAMPLE_DATA g_sSampleData;
#endif
//================================== 数据处理模块 ============================================
 struct sRMT_BACK
 {
 	unsigned long m_RmtInfoBackNew;//有遥信变化时
   unsigned int m_Year;
   unsigned int m_Month;
   unsigned int m_Day;
   unsigned int m_Hour;
   unsigned int m_Minute;
   unsigned int m_Milisecond;
  };
 struct sSOE_DATA  //全部故障数据
  {
    unsigned char m_gSoeBuff[SOE_DATA_LEN_BYTE];//缓冲区中的数据，在RAM中保存1条
    unsigned char m_gSoeRSTBuff[SOE_DATA_LEN_BYTE];
    unsigned char m_ucSoePos;   //故障记录在EEP中的位置;
    unsigned char m_ucSoeNum;	//故障记录总条数
    							
    unsigned char m_gAbnBuff[ABN_DATA_LEN_BYTE];//缓冲区中的数据，在RAM中保存1条
    unsigned char m_gAbnOFFBuff[ABN_DATA_LEN_BYTE];
    unsigned char m_ucAbnPos;    //
    unsigned char m_ucAbnNum;
    
    unsigned char m_gPowerOFFBuff[OFF_DATA_LEN_BYTE];//缓冲区中的数据，在RAM中保存1条
    unsigned char m_gPowerONBuff[OFF_DATA_LEN_BYTE];
    unsigned char m_ucPowerOFFPos;//
    unsigned char m_ucPowerOFFNum;
       
    unsigned char m_gBreakBuff[BK_DATA_LEN_BYTE];//缓冲区中的数据，在RAM中保存1条
    unsigned char m_gBreakRSTBuff[BK_DATA_LEN_BYTE];
    unsigned char m_ucBreakPos;//
    unsigned char m_ucBreakNum;
    
    unsigned char m_gRecBuff[WAVE_DATA_LEN_BYTE];//缓冲区中的录波数据，在RAM中保存1条
    unsigned char m_ucRecPos;//
    unsigned char m_ucRecNum;
  };
  struct sREC_DATA  //录波数据结构体定义
  {

    int m_gRecAc[REC_AC_LEN][REC_AC_CHAN_NUM];//故障动作的交流电压电流波形，保存故障动作前10个周波，故障动作后6个周波。操作录波时只需要保存动作之前10个周波，动作之后6个周波
    //unsigned char m_gRecD[REC_AC_LEN];
    unsigned int m_gFaultRecSOE[REC_DATA_LEN];//启动该故障录波对应的故障SOE，内容包括时间，不需要最后的CRC校验
    unsigned int m_gFaultRecOver[7]; //采样结束时间
	
    //保护定值在写进FLASH时从定值中调出写入FLASH
    //以下标识注意在录波保存结束后清0


//故障录波 
    unsigned char m_ucRecSavingFlag;//录波数据往Flash保存过程中 //故障延时到fault_save=0x55则  YES 开始存储//存储结束则 OFF 
    unsigned char m_ucFaultRecStart;     //故障录波开始标志//fault_begin 则ON //fault_end 则 OFF//存储结束则 OFF
    unsigned int m_unRecAcTail;//数据缓存的尾部索引,收到一个数据加加
    unsigned int m_unRecAcLockCnt;//m_ucFaultRecStart=ON，则该变量累加，达到5个周波后，停止继续存数 
    unsigned int m_unRecACNum;//为防止故障录波存储数据不足640
    unsigned int m_gRecANum;          //故障最多存10条 
    unsigned int m_gRecCNum; 	         //故障当前录波位置 
    
    unsigned int m_gACTRecANum;          //动作最多存10条 
    unsigned int m_gACTRecCNum; 	         //动作当前录波位置
    
    unsigned int m_gXHRecANum;          //熄弧最多存10条 
    unsigned int m_gXHRecCNum; 	         //熄弧当前录波位置    
//动作录波
    unsigned int m_gActRecCNum; 	    //当前录波位置
    unsigned int m_gActRecANum; 	    //最多存10条    
    unsigned char m_ucActRecStart;     
	//动作录波开始标志  ON开始 OFF结束录波  NO 存储数据  CLOSE 存储结束，不再录波 
    //unsigned long m_gActRecLen;         //不定长录波，每条录波存的点数，每点5个模拟量，每个模拟量2字节	
    unsigned long m_gActRecAdr;           //动作录波多次存储，FLASH中的地址
    unsigned char m_ucActRecSavingFlag;//录波数据往Flash保存过程中 ?    									//YES存储缓存中前320点 
    									//ON存储缓存中前320点 
    									//OFF 320点存储完
    unsigned char m_EraseBlock;//动作录波存储结束，擦除下一个存储空间 ON
    unsigned char m_LuboType;//录波类型 
    
    unsigned int m_CurWritePageCom;           //用于通信的页数
    unsigned int m_gRecNumCom;              //用于通信的录波次数
    unsigned char m_gACTDelay;    //8脉冲结束后，延时2秒，再停止录波
    unsigned int m_gXHDelay;    //熄弧延时 用于熄弧动作的开始和结束 及熄弧录波的结束
    unsigned char m_ucRecStart;     //动作录波开始标志     
  }; 

#ifdef _DATA_PROCESS
    __no_init struct sREC_DATA g_sRecData;//录波数据
    __no_init struct sLOG_DATA g_sLogData[MAX_LOG_NUM];//LOG数据
    unsigned int g_gRmtLockLB;//录波标志位遥信闭锁时间计数
    unsigned char g_SendZJDingshi = 0; 
    unsigned char 	g_YXLBOKCounter;
    unsigned char g_TQBSCounter = 0; //投切、闭锁指示灯计数器  =0 是灭 =0x55 闭锁常亮  >=1投切闪烁	
    unsigned char FlashReading;	// 1=正在读flash，不能录波，不能写soe
    unsigned char PT_LuBo[8];//U0高,u0低，
    unsigned int g_unAdcData[7]; //ADCMEM的数据存放到该数组中
    unsigned int g_test;
    //unsigned char g_gFaF[256];//
    unsigned char g_gKON;//继电器状态
    unsigned char g_gKONBK;//继电器状态
    unsigned int g_gRmtMeas[IEC_YC_NUM/*RMT_MEAS_NUM + 4*/];//遥测量，
    unsigned int g_gRmtFilMeas[IEC_YC_NUM/*RMT_MEAS_NUM + 4*/];//10次中间值滤波后的遥测量，
    unsigned int g_gVErrTimer[10];	      //电压超过阈值确认时间计时	       
    unsigned int g_gRmtMeasPJ[3][32];
    unsigned int g_gRmtMeasBak[IEC_YC_NUM/*RMT_MEAS_NUM*/]; //备份遥测量
    unsigned int g_gYCYueXian;//张弢 遥测越限	
    unsigned int g_gYCchangData[20];
    unsigned int g_gChangeYCNum = 0; //突变遥测数量 
    unsigned int g_SendChgYcFlag = 0;  //突变遥测标志
    unsigned int g_CosSoeDely = 0;  //突变遥测标志
    unsigned int g_gWeekNum = 0;
    unsigned long g_gsendYCTimeNum = 0;
    unsigned long g_gsendZJTimeNum = 0;	
    unsigned char g_SendYcDingshi = 0;
    unsigned long g_gsendYXTimeNum = 0;
    unsigned char g_SendYXDingshi = 0;
    unsigned long g_gBeatNum = 0;
    unsigned char g_SendBeat = 0;
    unsigned char g_SendBeatFailureNum = 0;
    unsigned int g_NolinkReset = 0;	
    unsigned int g_NolinkWifi = 0;		
    unsigned char g_GPRSSendLink = 0;// 1分钟重练一次101
    unsigned char g_RenZLink = 0;	//重庆 认证标志	
    unsigned char g_gRmtInfo[RMT_INFO_NUM];//遥信量 一个遥信信息占1个bit位
    unsigned char g_gRmtInfoBak[RMT_INFO_NUM];//遥信量备份 一个遥信信息占1个bit位
    //unsigned char g_gSCosBuff[SOE_BUFF_NUM][SOE_DATA_LEN];
    unsigned char g_gSCosBuff[1][SOE_DATA_LEN];
    unsigned char g_MaichongNum = 0;           //脉冲计数
    unsigned char g_I0RmtNum = 0;               //电流计数
    unsigned char g_I0RmtZeroNum=0;	//断线检测用电流计数	
    unsigned char g_FirstACTFlag = 0;           //首次动作标志
    unsigned char g_FinishACTFlag = 0;
    
    unsigned char g_ChaxunSOE_STATTime[7];
    unsigned char g_ChaxunSOE_OVERTime[7];
    unsigned int g_gSaveload;//每x秒存储1次负荷记录
    //unsigned char g_SendLuboNum = 0;
  
    unsigned int g_unSCosBuffTail;
	unsigned int g_unSSoeInFlashTail;//保存在外部flash中的SOE尾指针
    unsigned int g_gSmsHeadPt;       //短信头指针
    unsigned int g_gSmsHeadPtrBk;    //短信头指针备份
    //unsigned int g_gSmsStartPtr;     //短信发送开始指针
   // unsigned int g_RecFaultNum;      //短信发送条数备份
  //  unsigned int g_unGroundBuffTail;
    unsigned int g_unDSoeSavePtr;//待保存的双点SOE指针
    unsigned int g_unSSoeSavePtr;//待保存的单点SOE指针
    unsigned long g_unSSoeSaveFlashHead;//保存到外部flash的单点SOE头地址
    unsigned int g_unSSoeSaveE2ROMPtr;//待保存到外部flash的单点SOE指针
    unsigned char   soe_ptr; //最新SOE在flash中的段位置
    unsigned char   soe_area;//最新SOE在flash段中位置
    unsigned int    soe_num;//SOE总个数
    unsigned int g_unSoeSendNum;
    unsigned int  g_yxChangeflag;//遥信变位标识 BITF标识有新报文上传(遥信及遥测) BIT0-5,6-11分别标识调试口和gprs口对应的SYX,DYX,SSOE,DSOE,SSMS,DSMS;
   // char g_AdChgSdYc;//标识AD采集是否立即上传ON=立即上传
   // char g_FltChgSdYc;//标识故障遥测是否立即上传 1=立即上传
   // char g_PtLoseFlag;//pt失压标识 0=未失压 1=失压 2=电压回复中
  //  char g_BatLoseFlag;//电池失压标识 0=未失压 1=失压 2=电压回复中   
  //  unsigned int g_PtMonitorCnt;//pt电压恢复计数
  //  unsigned int g_BatMonitorCnt;//电池电压恢复计数
   // char g_SendYc;//0=所有遥测均不发送，1=正常发送(定时上报的遥测，故障遥测，AD采集值变化的遥测，越限遥测)
  //  unsigned char  g_Soenum;
  //  unsigned int  g_ZsqCmdRBit = 0;
  //  unsigned int  g_ZsqCmdWBit = 0;
  //  unsigned int  g_JdgPwMode = 0;
    
    __no_init struct sSOE_DATA g_sSoeData;//SOE在RAM中的缓冲区数据

  	//IAR会对分配的内存在进入main函数之前进行清0，需要占用较多的时间，导致看门狗启动，因此对占用较大内存的数组用"__no_init"禁止清0，放到程序中注意对它进行初始化

	//unsigned int g_gRmtMeas[RMT_MEAS_NUM];//遥测量，保存需要上传到后台的遥测数据 = 开方(g_gProcMeas/16) * 0.6944
	long g_gProcMeas_AC_R[6];
	long g_gProcMeas_AC_I[6];
	unsigned int g_gRmAcFilt[RMT_MEAS_NUM][RM_FILTER_NUM];//遥测量中的交流测量数据滤波 添加了3个线电压，但AD通道数不变，所以此处AC_AD_CHAN_NUM + 3
        unsigned int g_unFilterIndex = 0;   //交流测量数据滤波数据保存的位置
        volatile unsigned int g_unRmCaluFlag;    //遥测运算标志, 在1.25ms中断中置ON，在1ms中置OFF，在进行遥测运算，如果该标志为ON，说明遥测数据更新了，可以进行遥测量运算
        volatile unsigned int g_unTESTFlag; 
		volatile unsigned int g_unUABCaluFlag;	//遥测运算标志, 在50ms中断中置ON，在大循环中置OFF，
  	//unsigned long g_gProcMeas[PROC_MEAS_NUM];//保护用测量量，用于保护逻辑判断的测量量，为采样值的平方*16 最大值为(2048*2048/2*16)
        //unsigned int g_gRmtInfo[RMT_INFO_NUM];//遥信量，包括内部遥信量

	unsigned int  g_SendLuboNum;
	unsigned int  g_SendLuboPage;
	unsigned int g_FRecorder_Current_COUNT;//flash保存到第几条了
	unsigned int g_Recorder_Total_COUNT;//总的条数	
     WORD wCfgTotalNum =0;//总长度
#ifdef YN_101S     
     WORD section_current=0;//当前节*/
#else
     WORD section_current=1;//当前节*/
#endif
#else
extern unsigned int g_test;
    extern unsigned int g_gSaveload;//每x秒存储1次负荷记录
    //extern unsigned char g_gFaF[256];//
    extern unsigned char g_YXLBOKCounter;
    extern unsigned int g_gRmtLockLB;//录波标志位遥信闭锁时间计数	
    extern unsigned char g_SendZJDingshi; 
    extern unsigned char g_TQBSCounter; 
    extern unsigned char FlashReading;		
    extern struct sREC_DATA g_sRecData;
    extern unsigned int g_unAdcData[7]; //ADCMEM的数据存放到该数组中
    extern unsigned char g_gKON;//继电器状态
    extern unsigned char g_gKONBK;//继电器状态
    extern unsigned int g_gRmtMeas[IEC_YC_NUM/*RMT_MEAS_NUM + 4*/];//遥测量，保存需要上传到后台的遥测数据 
    extern unsigned int g_gRmtFilMeas[IEC_YC_NUM/*RMT_MEAS_NUM + 4*/];//10次中间值滤波后的遥测量，
    extern unsigned int g_gRmtMeasPJ[3][32];
    extern unsigned int g_gRmtMeasBak[IEC_YC_NUM/*RMT_MEAS_NUM */]; //备份遥测量
    extern unsigned int g_gYCYueXian;//张弢 遥测越限	
    extern unsigned int g_gYCchangData[20];
    extern unsigned int g_gChangeYCNum; //突变遥测数量
    extern unsigned int g_SendChgYcFlag;  //突变遥测标志
    extern unsigned int g_CosSoeDely;  //突变遥测标志
    extern unsigned int g_gWeekNum;
    extern unsigned long g_gsendYCTimeNum ;
    extern unsigned char g_SendYcDingshi;
    extern unsigned long g_gsendZJTimeNum ;	 		
    extern unsigned long g_gsendYXTimeNum ;
    extern unsigned char g_SendYXDingshi;
    extern unsigned char g_GPRSSendLink ;
    extern unsigned char g_RenZLink;	//重庆 认证标志
    
    extern unsigned char g_gRmtInfo[RMT_INFO_NUM];//遥信量 一个遥信信息占2个bit位
    extern unsigned char g_gRmtInfoBak[RMT_INFO_NUM];
    //extern unsigned char g_gSCosBuff[SOE_BUFF_NUM][SOE_DATA_LEN];
    extern unsigned char g_gSCosBuff[1][SOE_DATA_LEN];    
   // extern unsigned char g_gGroundBuff[SOE_BUFF_NUM];
    extern unsigned int g_unSCosBuffTail;
    
    extern unsigned char g_MaichongNum;           //脉冲计数
    extern unsigned char g_I0RmtNum;               //电流计数
    extern unsigned char g_I0RmtZeroNum;	//断线检测用电流计数	
    extern unsigned char g_FirstACTFlag;           //首次动作标志
    extern unsigned char g_FinishACTFlag;

    extern unsigned int g_unSSoeInFlashTail;//保存在外部flash中的SOE尾指针
    extern unsigned int g_gSmsHeadPt;       //短信头指针
    extern unsigned int g_gSmsHeadPtrBk;    //短信头指针备份
   // extern unsigned int g_gSmsStartPtr;     //短信发送开始指针
   // extern unsigned int g_RecFaultNum;      //短信发送条数备份
    extern unsigned int g_unDSoeSavePtr;//待保存的双点SOE指针
    extern long g_unSSoeSavePtr;//待保存的单点SOE指针
    extern unsigned long g_unSSoeSaveFlashHead;//保存到外部flash的单点SOE头地址
    extern unsigned int g_unSSoeSaveE2ROMPtr;//待保存到外部flash的单点SOE指针
   // extern unsigned int g_unGroundBuffTail;
    extern unsigned char   soe_ptr; //最新SOE在flash中的段位置
    extern unsigned char   soe_area;//最新SOE在flash段中位置
    extern unsigned int    soe_num;//SOE总个数
    extern unsigned int g_unSoeSendNum;
    extern unsigned int  g_yxChangeflag;
    extern unsigned char g_ChaxunSOE_STATTime[7];
    extern unsigned char g_ChaxunSOE_OVERTime[7];
    //extern unsigned char g_SendLuboNum;
    extern unsigned long g_gBeatNum;
    extern unsigned char g_SendBeat;
    extern unsigned char g_SendBeatFailureNum;
    extern unsigned int g_NolinkReset;	    
    extern unsigned int g_NolinkWifi;	
    //extern char g_AdChgSdYc;
   // extern char g_FltChgSdYc;
   // extern char g_PtLoseFlag;
   // extern unsigned int g_PtMonitorCnt;
   // extern char g_SendYc;
   // extern unsigned char  g_Soenum;
   // extern unsigned int  g_ZsqCmdRBit;
  //  extern unsigned int  g_ZsqCmdWBit;
   // extern unsigned int  g_JdgPwMode;
    extern struct sSOE_DATA g_sSoeData;    
	extern struct sLOG_DATA g_sLogData[MAX_LOG_NUM];//LOG数据
  	//extern unsigned int g_gRmtMeas[RMT_MEAS_NUM];//遥测量，保存需要上传到后台的遥测数据
        extern unsigned int g_gRmAcFilt[RMT_MEAS_NUM][RM_FILTER_NUM];//遥测量中的交流测量数据滤波 添加了3个线电压，但AD通道数不变，所以此处AC_AD_CHAN_NUM + 3
	//extern unsigned long g_gProcMeas[PROC_MEAS_NUM];//保护用测量量，用于保护逻辑判断的测量量
       // extern unsigned int g_gRmtInfo[RMT_INFO_NUM];//遥信量，包括内部遥信量   
       extern volatile unsigned int g_unRmCaluFlag;    //遥测运算标志, 在中断中置ON，在大循环中置OFF，在进行遥测运算，如果该标志为ON，说明遥测数据更新了，可以进行遥测量运算
       extern volatile unsigned int g_unTESTFlag;
	   extern volatile unsigned int g_unUABCaluFlag;	//遥测运算标志, 在50ms中断中置ON，在大循环中置OFF，
	extern unsigned int  g_SendLuboNum;
	extern unsigned int  g_SendLuboPage;
    extern WORD wCfgTotalNum ;//总长度
    extern WORD section_current;//当前节*/
    extern unsigned int g_FRecorder_Current_COUNT;//flash保存到第几条了
	extern unsigned int g_Recorder_Total_COUNT;//总的条数    
#endif

//=======================================  参数配置模块  ===================================
#ifdef _PARA_CFG

        unsigned int g_gRunParaYxRsdTTBk;//运行参数中遥信重发时间间隔备份
        unsigned int g_unYxTrans[RMT_WSYX_NUM];  //根据遥信的点表，重新配置生成的遥信内容
        //unsigned int g_unDYxTrans[RMT_WDYX_NUM];//上传的双点遥信
        unsigned int g_unZJTrans[IEC_ZJ_NUM];    //上传的遥测数据，如果发生上传的超过刷新死区，则上传        
        unsigned int g_unYcTrans[IEC_YC_NUM];    //上传的遥测数据，如果发生上传的超过刷新死区，则上传
        unsigned char g_ucYcTransNum; //记录需要上传的遥测的个数
        unsigned int g_ucYxTransNum; //记录u需要上传遥信的总个数(包括单点和双点)

        unsigned int g_ucSYxTrsStartId;//上传的单点遥信中第一个点的序号(在g_ucYXAddr数组中)
        unsigned int g_ucDYxTrsStartId;//上传的双点遥信中第一个点的序号(在g_ucYXAddr数组中)
        unsigned char g_ucYkTransNum;
        unsigned char g_ucParaChang = 0; //101参数发生变更标志  BIT0为 101参数，BIT1为遥测点表，BIT2为遥测系数，BIT3为遥测偏移量，BIT4为遥信点表 BIT5为运行参数表//张弢 读汉字站名BIT6//
        unsigned char g_ucRefYxYcTs = 0;//遥信遥测数据变化标志 BIT0为遥信 BIT1为遥测
        unsigned char g_ZsqPara = 0;   //zx
        
        unsigned char  g_gSmsPhone[PHONE_PA_NUM];//短信中电话号码
        unsigned int g_gCnName[CnName_NUM+1];//张弢 汉字站名地址
        unsigned char g_gLBNameLen;
	 unsigned char  g_gLBName[LBName_NUM+1];//张弢 汉字站名地址
     //   unsigned int g_gRunPara[RUN_PARA_NUM];//运行参数，保存装置的一些配置信息，包括通信参数等
     	unsigned char g_ucPara101[IEC_PARA_NUM];    //IEC运行的运行参数
     	//unsigned char g_ucDebug[Debug_PARA_NUM];    //调试用参数
     	//unsigned char g_unParaObjComBk[4];//IEC参数中的端口对象4个数据进行备份，有变化时重映射类对象与端口的对应关系
        unsigned char g_ucYCAddr[IEC_YC_NUM + 1];   //遥测的点表，可以把遥测中的数据按照偏移量重新排列，最多按照20个配置，为0的不上传，点号为1-255
        unsigned char g_ucYXAddr[IEC_YX_NUM + 1];   //遥信的点表，为0的不上传，点号为1-255
    //    unsigned char g_ucYKPa[IEC_YK_NUM + 1];   //遥控参数 标识指示器的线偏相偏
		
			
		//unsigned int g_gRunParaFltLkTBk;//运行参数中故障闭锁时间备份，用于参数变化时使其立即生效
       // unsigned int g_gRunParaZsqStTBk;//运行参数中指示器通信状态检测周期备份
       // unsigned int g_gRunParaYxRsdTTBk;//运行参数中遥信重发时间间隔备份
        
       // unsigned char g_open_oducall;//程序上电时5S召唤一次，等待采集的C的电压，以判断是用电池还是超级电容打开邋邋GPRS
      //  unsigned int Before4HourSdYcTm;
		//unsigned char g_LineFlag[12];    //ODU所有路数的状态?0为不和其它路合成1 为合成的前一路2 为合成的后一路
        
        extern const unsigned int BIT[16] =   //获得第几位为1的值，BIT0~BIT15
    	{
                0x01,0x02,0x04,0x08,
        0x10,0x20,0x40,0x80,
        0x100,0x200,0x400,0x800,
        0x1000,0x2000,0x4000,0x8000
    	};
        
        unsigned int g_gRunPara[RUN_PARA_NUM];//运行参数，保存装置的一些配置信息，包括通信参数等
        BYTE g_gIPPort[13];//  2个IP及端口，
        BYTE g_gAPN[20];
        BYTE g_gSMSR;//GPRS模块接收短信  0=不接收 1=接收         
        unsigned int g_gProcCnt[PROC_CNT_NUM];//保护定值，保存保护动作需要用到的定值
        //unsigned int g_gETHProcCnt[24];
        unsigned int g_gProcCntJug[27];       //用于判断的计算后的保护定值
        //unsigned long g_gProcCntJug_I0[3];       //用于过载判断的计算后的保护定值
        unsigned int g_gOverLoadTimeNum;     //用于过载计时，时间以1.25ms基准
        unsigned int g_gOverLoadFlg;        //过载标志 ON 为过载
        unsigned int g_gAdjObj[ADJ_PARA_NUM];//张弢 目标校准，上位机下载参数 初始值为电压60V,电流2A    
        unsigned char g_gModfk[24];
	unsigned char kat,kbt,kct,Numk;	
  	int g_gAdjPara[ADJ_PARA_NUM];//校正参数，保存各个通道的校正参数   
  	unsigned int g_gAdjAD[ADJ_PARA_NUM];//根据校正参数，计算出的AD值的偏移量
  	//unsigned int g_gRunInfo[RUN_INFO_NUM];//运行信息，保存装置运行的部分信息，比如合分闸次数  	
  	unsigned char g_gChangFlag[PARA_NUM];	//参数变化标志，如果发生参数更改，则置位，把参数保存到eeprom中
        //unsigned int g_gYXConformTime[2];       //遥信确认时间，0：信号源断线 1：
        unsigned char g_gDebugP[Debug_PARA_NUM];//调试串口波特率
#else
        extern unsigned int g_gRunParaYxRsdTTBk;//运行参数中遥信重发时间间隔备份
        extern unsigned int g_unYxTrans[RMT_WSYX_NUM];  //根据遥信的点表，重新配置生成的遥信内容
        //extern unsigned int g_unDYxTrans[RMT_WDYX_NUM];//上传的双点遥信
        extern unsigned int g_unZJTrans[IEC_ZJ_NUM];       
        extern unsigned int g_unYcTrans[IEC_YC_NUM];    //上传的遥测数据，如果发生上传的超过刷新死区，则上传
        extern unsigned char g_ucYcTransNum; //记录需要上传的遥测的个数
        extern unsigned int g_ucYxTransNum; //记录u需要上传遥信的个数
        extern unsigned int g_ucSYxTrsStartId;//上传的单点遥信中第一个点的序号(在g_ucYXAddr数组中)
        extern unsigned int g_ucDYxTrsStartId;//上传的双点遥信中第一个点的序号(在g_ucYXAddr数组中)
      //  extern unsigned int g_ucDYxTransNum;
        extern unsigned char g_ucYkTransNum;
        extern unsigned char g_ucParaChang; //101参数发生变更标志  BIT0为 101参数，BIT1为遥测点表，BIT2为遥测系数，BIT3为遥测偏移量，BIT4为遥信点表 BIT5为运行参数表
        extern unsigned char g_ucRefYxYcTs;//遥信遥测数据变化标志 BIT0为遥信 BIT1为遥测
                
		extern unsigned char  g_gSmsPhone[PHONE_PA_NUM];//短信中电话号码  //张弢 0328
		extern unsigned int g_gCnName[CnName_NUM+1];//张弢 汉字站名地址
		extern unsigned char g_gLBName[LBName_NUM+1];//张弢 汉字站名地址	
		extern unsigned char g_gLBNameLen;
	//	extern unsigned int g_gRunPara[RUN_PARA_NUM];//运行参数，保存装置的一些配置信息，包括通信参数等
    	extern unsigned char g_ucPara101[IEC_PARA_NUM];    //IEC运行的运行参数
    	//extern unsigned char g_ucDebug[Debug_PARA_NUM];    //调试用参数
		//extern unsigned char g_unParaObjComBk[4];//IEC参数中的端口对象4个数据进行备份，有变化时重映射类对象与端口的对应关系
		extern unsigned char g_ucYCAddr[IEC_YC_NUM + 1];   //遥测的点表，可以把遥测中的数据按照偏移量重新排列，最多按照20个配置，为0的不上传，点号为1-255
        extern unsigned char g_ucYXAddr[IEC_YX_NUM + 1];   //遥信的点表，为0的不上传，点号为1-255 
     //   extern unsigned char g_ucYKPa[IEC_YK_NUM + 1]; 
		
       //	extern unsigned int g_gRunParaFltLkTBk;//运行参数中故障闭锁时间备份，用于参数变化时使其立即生效
		//extern unsigned int g_gRunParaZsqStTBk;//运行参数中指示器通信状态检测周期备份
		//extern unsigned int g_gRunParaYxRsdTTBk;//运行参数中遥信重发时间间隔备份
        extern const unsigned int BIT[16];
        
        extern unsigned int g_gRunPara[RUN_PARA_NUM];//运行参数，保存装置的一些配置信息，包括通信参数等
        
		//extern unsigned char g_open_oducall;
		//extern unsigned int Before4HourSdYcTm;
		//extern unsigned char g_LineFlag[12];
                extern unsigned int g_gProcCnt[PROC_CNT_NUM];//保护定值，保存保护动作需要用到的定值
        //extern unsigned int g_gETHProcCnt[24];
        extern unsigned int g_gProcCntJug[27];       //用于判断的计算后的保护定值
        //extern unsigned long g_gProcCntJug_I0[3];       //用于过载判断的计算后的保护定值
        extern unsigned int g_gOverLoadTimeNum;     //用于过载计时，时间以1.25ms基准
        extern unsigned int g_gOverLoadFlg;        //过载标志 ON 为过载
        extern BYTE g_gIPPort[13];//  2个IP及端口
        extern BYTE g_gAPN[20];
        extern BYTE g_gSMSR;//GPRS模块接收短信  0=不接收 1=接收         
	extern int g_gAdjPara[ADJ_PARA_NUM];//校正参数，保存各个通道的校正参数
	extern unsigned int g_gAdjAD[ADJ_PARA_NUM];//根据校正参数，计算出的AD值的偏移量
	extern unsigned int g_gAdjObj[ADJ_PARA_NUM];//张弢 目标校准，上位机下载参数 初始值为电压60V,电流2A
	extern unsigned char g_gModfk[24];
	extern unsigned char kat,kbt,kct,Numk;	
        extern int g_gAdjParaBuf[ADJ_PARA_NUM];
	
	extern unsigned char g_gChangFlag[PARA_NUM];	//参数变化标志，如果发生参数更改，则置位，把参数保存到eeprom中
	//extern unsigned int g_gYXConformTime[2];       //遥信确认时间，0：信号源断线 1：
	extern unsigned char g_gDebugP[Debug_PARA_NUM];//调试串口波特率	
#endif

//=================================== 通信模块 ==========================================
  struct  sTX_BUFF		   //通信发送缓冲结构体
	{
	  unsigned char m_gBuffer[MAX_RXBUFF_LEN];//发送数据缓冲
	  unsigned int m_ucTx_Head;//GPRS串口中断发送数据的缓存头指针
      unsigned int m_ucTx_Tail;//GPRS串口中断发送数据的缓存尾指针
      unsigned char m_unSendFlag;
	};


  struct  sRX_BUFF         //通信接收缓冲结构体
  {
    unsigned char m_gBuf[MAX_RXBUFF_LEN];//接收数据缓冲
    unsigned int m_iBufT;      //数据缓冲尾指针
    unsigned int m_iBufH;      //数据缓冲头指针
    unsigned int m_iRcvCount; //接收字节时间间隔
    unsigned char m_iRcvFlag;
  };
/*  
    struct  sRX_BUFF_LED         //通信接收缓冲结构体
  {
    unsigned char m_gBuffer[MAX_RXBUFF_LEN];//接收数据缓冲
    unsigned int m_iBufferTail;
    unsigned int m_iBufferHead;
  };
  */
#ifdef _COMM
    __no_init struct  sTX_BUFF g_sTxBuff[COMM_PORT_NUM];
    __no_init struct  sRX_BUFF g_sRxBuff[COMM_PORT_NUM];
             // struct  sRX_BUFF_LED  g_sRX_BUFF_LED;
       // unsigned int g_sCommStatCount[RMT_ZSQST_NUM];//判断指示器通信状态时间计数
        char g_YkOrderFlag;//标识遥控是否成功

        unsigned char g_GprsPowerSt;//GPRS电源状态 0=关闭 1= 打?      
        unsigned char g_batPowerSt;//bat状态 0=关闭 1= 打?		

        unsigned char g_SmsWritePtr;
        unsigned char g_EnSmsSend;  //短信发送允许
        unsigned char g_SmsDataLen;
        unsigned char g_EnSendToXinhao;
		
        unsigned char g_SmsReSendWaitCount; //短信重发等待时间
        unsigned char g_SmsReSendTime;   //短信重发次数
    	unsigned char g_SmsSendBuf[55];
		unsigned char g_CmIdRF;//RF对象使用的串口号
		unsigned char  g_CmIdGPRS;
		unsigned char g_CmIdEX;//扩展口使用的串口号
		unsigned char g_CmIdDBG;//调试口使用的串口号
		unsigned char g_CmIdODU1;//ODU1使用的串口号
		unsigned char g_CmIdODU2;//ODU2使用的串口号
		unsigned int g_RstartNum;//系统重启次数
                
        //unsigned char g_unTX_Buff[2][256];            //数据发送数组
    
    unsigned char g_unSMS_Index[MAX_SMS_NUM];              //存储新产生的短信标志
    unsigned char g_unFrameFlag_Sms=0;             //产生短信数量
    unsigned char g_unFrameFlag_curSms=0;             //产生短信数量
    unsigned char g_unSms_s=0;             //  0=可以发送短信  >0停止发送,等待正确回复
    unsigned char g_unPhoneNo=0;
    unsigned char g_unSms_Count=0;     //短信发送后计时，若15秒后无回复，重发
    unsigned char g_unFailSms=0;//短信发送失败次数
   // unsigned char g_unGPRSTX_Buff[MAX_TXBUFF_101_LEN];          //GPRS数据发送数组 
    unsigned char g_unRX_Buff[1][MAX_RXBUFF_LEN];            //数据接收数组
    //unsigned char g_unLEDTX_Buff[MAX_TXBUFF_101_LEN];         //数据发送数组
    unsigned char g_uc101BuffHead;   //GPRS通信接收缓冲区数据头
    unsigned char g_uc101BuffDataHead;   //GPRS通信接收缓冲区数据头    
    unsigned char g_uc101BuffTail;   //GPRS通信接收缓冲区数据尾
    
    unsigned int bFrameReceived[COMM_PORT_NUM];   //缓冲区接收新数据标志
    //unsigned int g_ucBuffHead[2];   //通信接收缓冲区数据头
    //unsigned int g_ucBuffTail[2];   //通信接收缓冲区数据尾
    
    //unsigned char g_ucUpdateTemp[256];   //升级专用数组
   
    //unsigned int g_unFrameFlag_101[2];  //101协议数据帧标志位


   // const unsigned char VerNum[2]={0,22};   //注:  修改生产新的版本  V21
    //unsigned char TEL_NUM[55]={0};   //4组电话号码
    //unsigned char g_ucphone_perm[5];         //电话短信的功能选择
    unsigned char ucTempTEL_NUM[11] = {0};  //临时存储号码，用于子站的查询

    unsigned int g_ucGPRSTxlen;         //发送中断数据长度
    unsigned int g_ucGPRSTxNum;         //

    unsigned int g_ucGPRSState=GPRSState_IDLE;					//GPRS状态，0=IDLE,1=GPRS101，2=SMS//张弢 0330 
    unsigned int g_ucGPRSState_TCounter;		//GPRS状态时间计数器，如果长时间不处于0=IDLE状态，则恢复0=IDLE状态//张弢 0330
    
    unsigned int g_uc232Txlen;         //发送中断数据长度
    unsigned int g_uc232TxNum;         //
    
    unsigned char sign_RSSI_time[6];              ////year month day hour minute second 
    //unsigned char pulse_flag=0;                  ////////八脉冲检测到标志   0x55
    //unsigned char wave_counter=0;                //////// 对发现8脉冲后，采集的周波进行计数

    //unsigned char pulse_time_flag;             ////////第一次发现8脉冲后，置标志，为在10秒内完成提供标志
    unsigned char g_ucPara_stime;  //主动上传间隔时间
    unsigned char eight_pulse_counter;
    unsigned int unFrameNum = 0; //临时存储待发送短信条数
    unsigned char g_uc101Flg = 0;
    
    /*unsigned char g_ucRELConfirm;   //通信用遥控确认标志
    unsigned char g_ucRELUNM;       //通信用遥控继电器的顺序号
    unsigned int g_unRELTimeFlg;    //通信用继电器选择时间标志
    unsigned int g_unRELTimeFlg_101;
    
    unsigned int g_unOffset;    //物理地址转换偏移量*/
    //char  ComtrderCfg1[360];
    char  ComtrderCfg_adj[50];
	
	unsigned char g_EncComNo;		 //哪个串口在跑加密数据1==GPRS 2 ==DeBug， 其余无效
	unsigned char g_bEncCerTxt[1];  //加密CER证书
	unsigned char g_bHostRandom[8];  //主站随机数
	unsigned char m_bUnitRandom[8];   //终端自身随机数，用于与运维工具通信
	int g_wEncCerWPtr;				   //Cer证书记录位?
	char g_bUnitID[1];//终端ID,24字节第一个字是节长度	 
	unsigned char g_bAppType;//加密报文应用类型
	//char g_pDevId[30]="12345678901234567890123456789";
#else
    extern struct  sTX_BUFF g_sTxBuff[COMM_PORT_NUM];
    extern struct  sRX_BUFF g_sRxBuff[COMM_PORT_NUM];
    //extern struct  sRX_BUFF_LED  g_sRX_BUFF_LED;
    extern char g_YkOrderFlag;//标识遥控是否成功
    extern unsigned char g_GprsPowerSt;//GPRS电源状态 0=关闭 1= 打开
     extern unsigned char g_batPowerSt;//bat状态 0=关闭 1= 打?    extern int g_ODUYK;//

	extern unsigned char g_SmsWritePtr;
	extern unsigned char g_EnSmsSend;
	extern unsigned char g_SmsDataLen;
    extern unsigned char g_EnSendToXinhao;
    extern unsigned char g_SmsReSendWaitCount;
    extern unsigned char g_SmsReSendTime;
    extern unsigned char g_SmsSendBuf[55];
	extern unsigned char g_CmIdRF;//RF对象使用的串口号
	extern unsigned char  g_CmIdGPRS;
	extern unsigned char g_CmIdEX;//扩展口使用的串口号
	extern unsigned char g_CmIdDBG;//调试口使用的串口号
	extern unsigned char g_CmIdODU1;//ODU1使用的串口号
	extern unsigned char g_CmIdODU2;//ODU2使用的串口号
	
    extern int g_pODU2WaitCount;//ODU电源关闭请求ODU2的接收计时
	extern int g_pODUWaitCount;//ODU电源关闭请求
	extern unsigned int g_RstartNum;//系统重启次数
        
    //extern unsigned char g_unTX_Buff[2][256];
    
    extern unsigned char g_unSMS_Index[MAX_SMS_NUM];              //存储新产生的短信标志; 
    extern unsigned char g_unFrameFlag_Sms;             //产生短信数量
    extern unsigned char g_unFrameFlag_curSms;             //产生短信数量     
    extern unsigned char g_unSms_s;             //  1=可以发送短信  0=停止发送    
    extern unsigned char g_unPhoneNo;
    extern unsigned char g_unSms_Count;	
    extern unsigned char g_unFailSms;//短信发送失败次数
    //extern unsigned char g_unGPRSTX_Buff[MAX_TXBUFF_101_LEN];          //GPRS数据发送数组
    
    extern unsigned char g_unRX_Buff[1][MAX_RXBUFF_LEN];
    //extern unsigned char g_unLEDTX_Buff[MAX_TXBUFF_101_LEN];         //数据发送数组
    

    extern unsigned int bFrameReceived[COMM_PORT_NUM];   //缓冲区接收新数据标志

    //extern unsigned int g_ucBuffHead[2];         //通信接收缓冲区数据头
    //extern unsigned int g_ucBuffTail[2];         //通信接收缓冲区数据尾
    extern unsigned char g_uc101BuffHead;
    extern unsigned char g_uc101BuffDataHead;   //GPRS通信接收缓冲区数据头  	
    extern unsigned char g_uc101BuffTail;
    //extern unsigned char g_ucUpdateTemp[256];   //升级专用数组
 
    //extern unsigned int g_unFrameFlag_101[2];  //101协议数据帧标志位
  //  extern const unsigned char VerNum[2];   //注:  修改生产新的版本  V21
    //extern unsigned char TEL_NUM[55];   //5组电话号码
    //extern unsigned char g_ucphone_perm[5];         //电话短信的功能选择
    extern unsigned char ucTempTEL_NUM[11];  //临时存储号码，用于子站的查询

    extern unsigned int g_ucGPRSTxlen;         //发送中断数据长度
    extern unsigned int g_ucGPRSTxNum;         //

    extern unsigned int g_ucGPRSState;         //GPRS状态，0=IDLE,1=GPRS101，2=SMS//张弢 0330 
    extern unsigned int g_ucGPRSState_TCounter;         //GPRS状态时间计数器，如果长时间不处于0=IDLE状态，则恢复0=IDLE状态//张弢 0330
	
    
    extern unsigned int g_uc232Txlen;         //发送中断数据长度
    extern unsigned int g_uc232TxNum;         //
    
    extern unsigned char sign_RSSI_time[6];              ////year month day hour minute second
    //extern unsigned char pulse_flag;                  ////////八脉冲检测到标志   0x55
    //extern unsigned char wave_counter;                //////// 对发现8脉冲后，采集的周波进行计数
 
    extern unsigned char g_ucPara_stime;  //主动上传间隔时间
    extern unsigned char pulse_time_flag;             ////////第一次发现8脉冲后，置标志，为在10秒内完成提供标志
    extern unsigned char eight_pulse_counter;
    extern unsigned int unFrameNum; //临时存储待发送短信条数
    extern unsigned char g_uc101Flg;
    //extern char  ComtrderCfg1[360];
    extern char  ComtrderCfg_adj[50];
		
	extern unsigned char g_EncComNo;//
	extern unsigned char g_bEncCerTxt[1];
 	extern unsigned char g_bHostRandom[8];//主站随机数
	extern unsigned char m_bUnitRandom[8];//
	extern int g_wEncCerWPtr;//   
	extern char g_bUnitID[1];//终端ID,24字节第一个字是节长度   
	extern unsigned char g_bAppType;//加密报文应用类型 
	//extern char g_pDevId[30];
#endif
    
//=======================DriverTimer.c--定时器驱动模块===============================================
 struct sTIMER_MANAGER
  {
    unsigned int m_TmCount;
    unsigned char m_TmFlag;
	unsigned int m_TmCountBk;//保存上一次设置的定时间隔
  };
#ifdef _DRIVER_TIMER
        struct sTIMER_MANAGER g_sTimer[TIMER_NUM];
        unsigned int Time_10ms_T;
        unsigned int g_DateCount;  //状态汇报天数计数器
        unsigned char g_StatusTime;
        unsigned char g_SendStatus;
        unsigned char g_SmsSendStatus; //短信汇报状态标志位
        unsigned char g_SendFault;
        unsigned int TA1OffsetNum = 250;
        unsigned char Powerdown_counter;
	  unsigned char g_STimeout;//秒计时 	
#else
        extern struct sTIMER_MANAGER g_sTimer[TIMER_NUM];
        extern unsigned int g_DateCount;
        extern unsigned char g_StatusTime;
        extern unsigned char g_SendStatus;
        extern unsigned char g_SmsSendStatus;
	
        extern unsigned char g_SendFault;
        extern unsigned char g_STimeout;//秒计时
        extern unsigned char Powerdown_counter;
#endif
//=================================== 时钟模块 ==========================================

  struct sRTC_MANAGER
  {
    unsigned char m_ucRtcSynFlag;   //系统实时时钟从实时芯片中读取的标志，每5分钟读取一次
    unsigned int m_unRtcSynCount;   //系统同步间隔时间计时，计满5分钟，置m_ucRtcSynFlag标志
    unsigned int m_unStartCount;		//开机计时，在刚开机的3S内把运行等常亮
    unsigned int m_gRealTimer[RTC_NUM]; //系统内部实时时钟，年、月、日、时、分、秒、毫秒
    unsigned char m_ucCounter125;     ///1.25ms时间计数器
    unsigned char m_ucCounter10;     ///10ms时间计数器
  };
 
  
#ifdef _DRIVER_RTC
  
  struct sRTC_MANAGER g_sRtcManager;  //系统实时时钟
  unsigned int g_gRealTimerbk[RTC_NUM];
#else
  extern struct sRTC_MANAGER g_sRtcManager;
  extern unsigned int g_gRealTimerbk[RTC_NUM];
  

#endif

//================================== IO驱动模块 ===========================================
#ifdef _DRIVER_IO

	unsigned int g_gExtDinStat[2];	//外部开关量输入确认状态，第一个为上一周期，第二个为当前周期
        unsigned int g_gExtDinYesTimer[3];	//外部DIN输入确认时间计时
	unsigned int g_gExtDinNoTimer[3];		//外部DIN输入清除时间计时
	unsigned char g_gPhasErrTimer[3];	      //开关位置错误确认时间计时
	unsigned int g_gI0OverTimer=0;		//I0超时确认时间计时
	//unsigned char g_gExtDinNoTimer[3];		//外部DIN输入清除时间计时
        //unsigned int g_gDinRtcLock[2][RTC_NUM];
	
        unsigned char g_gucPhaseSQU[3];  //存储相序检测各相过零标志数
        unsigned char g_gucPhaseSQUTestNum;  //相序检测各相过零点计数
        //unsigned int rh_counter=0;//燃弧成功遥信 延时计数器
        unsigned int rh_send_ok=0;//燃弧脉冲发送完成 =0x55 发出8脉冲 =0x85 故障消失 =0        
        unsigned int fault_time=0;            ///////故障计数值
        unsigned char fault_begin;               //故障出现延时 
        unsigned char fault_pluse;//故障相位
        unsigned char fault_end;                 //故障恢复延时
        
        unsigned char fault_lubo = 0;               //故障启动出现延时
        
        unsigned char abnormal_counter=0;              //////装置异常计数器，机构5次报异常
        
        unsigned char save_abnormal;
        unsigned char fault_save;             ///////故障存储标志,同时做故障发生标志，0：无故障，0x33：有故障

        unsigned char upload_GPRS;
        unsigned char upload_SMS;              ////每隔60秒钟发送一条短信。
        unsigned int   km_timeout; 
        unsigned int   i0_timeout; 		
	 unsigned int   ptoff_timeout; 	
	 unsigned char soft_latch;	
       // unsigned char SMS_delay;               ////延时短信延时每隔2秒钟发送一条短信。      
        unsigned char main_reset_flag = 0;//=0x00;            /////////主板复位标志
        unsigned char flash_write;                 ////////写录波数据到FLASH

        unsigned char upload_flag;               ///the of upload                3           2         1           0
        unsigned char upload_interval[4];        ///self upload interval        断线    eight_pulse power_down  abnormal  
        unsigned char upload_interval_set = 3;       ///transmit interval parameter default 3 minutes   the range is 3-15 minutes  
//=============================== 调用外部参数 ===============================///
        
        unsigned char eight_ctl_flag=0;              ///////8脉冲遥控标志,有效8脉冲后，遥控输出5秒
        unsigned char power_ctl_flag=0;              ///////断电遥控标志
        unsigned char abnormal_ctl_flag=0;           ///////异常遥控标志
        
        unsigned char phase_off=0x07;               ////////////上电认为出现断线
        unsigned char phase_copy=0x07;              ////////////上电认为出现断线
        
        unsigned char save_power_off=0;             ////////////存储断电标志 
        unsigned char power_off_flag=0;           ///////断电标志
        
        unsigned char save_phase_off=0;          //存储断线标志
        unsigned char latch_upload_flag=0;         //////////闭锁或者解锁后置标志，主动上发一帧状态帧
        
        unsigned int state_counter = 0;             //上传时间间隔计数，分钟计时
        
        unsigned char eight_delay_flag=0;              ////////故障后延时8秒发8脉冲标志            
        unsigned char eight_delay_counter=0;           ////////故障后延时2秒发8脉冲计数器
        
        //unsigned char pulse_counter=0;               ////////对采集的脉冲数进行统计，用作数据存储定位

        unsigned char comm_rec_flag;

        unsigned char phone_perm[4];	//          5        4         3        2       1      0
						                    //      故障复归  发生故障 主动上报 有效8脉冲 掉电  状态异常
						//手机的权限控制字
        unsigned char SMS_array[2];      /////////等待发送的故障，六种情况，4个手机号，共24个；					
    
        unsigned char send_phase;
        unsigned int yc[8];	// 8脉冲遥测值

        //unsigned char yc1[8];	// 8脉冲遥测值
        unsigned char sign_RSSI_time1[6];              ////year month day hour minute second 
        unsigned char eight_select;                  /////传动第几次8脉冲

        //unsigned char yc2[8];	// 8脉冲遥测值
        //unsigned char sign_RSSI_time2[6];              ////year month day hour minute second
        
       // unsigned char bais_check;                    //检测偏置电压


        unsigned char pulse_phase_flag=0;              ////////哪一相发8脉冲的标志
        unsigned int efslatch_flag;                   ////////闭锁17分钟标志
       // unsigned char eight_delay_counter;           ////////故障后延时2秒发8脉冲计数器
        unsigned char chongfa;                       ////////重发标志 
        unsigned char moniguzhang;                       ////////模拟故障

        unsigned char phase_check_flag = 0x55;//=0x55;          /////////检测相位异常标致    0x00 不检测，  0x55准备检测             
        unsigned char phase_check_time = 30;//=24;            /////////故障已经检测完     每隔两个小时检测一次，上电检测一次   
        unsigned char phase_seq_flag;//=0x00;                 ///////// 初始化设置为正相序   0x00正相许  0x55 逆相序

///////////////////////////写入FLASH中


        unsigned char VerNumlcd[2]; 

        unsigned char sign_RSSI_delay; 
	 unsigned char newsms_8pluse; 
	 unsigned char newsms_abn; 
        unsigned char fault_point;                ////////用来确定是读取的那个录波或者故障记录

        unsigned char fault_recovery_flag;//=0;      ///////故障复归标志
        unsigned char fault_sms_type;            //////////故障状态，当故障发生时，放在自动上传的短信的最后

        unsigned char pulse_success;                   ////////脉冲发送成功,用于液晶显示维持17分钟
        unsigned char eight_pulse_flag;              ////////开始发8脉冲的标志

        unsigned char repeat_flag1;               //////////铁路型信号源重发标志，检测到电流超过35A，就重发。

      //  unsigned char phase_copy;              ////////////上电认为出现断线

 
        unsigned char recieve_confirm_flag;   //  用于决定采用何种应答方式
        unsigned char Process_68_flag;

        unsigned char process_type=0;     //  主站与终端之间通信时的 功能类型 
       // unsigned char process_step=0;     //  每种功能的处理步骤
        unsigned char process_pro=0;      //  功能步骤对应的优先级，主要用于与故障主动上传做比较
        unsigned char command_flag=0;    // 主动发送数据时的 发送原因
        unsigned char command_flag2=0;  // 主动发送数据时,增加一个缓冲处理任务
        unsigned char fault_num;
      //  unsigned char process_pro;
        unsigned char event_pro;
       // unsigned char self_chongfa_flag;
        unsigned char uart0_event_flag;
        unsigned char g_ucEarthFlg = 0;  //故障相标示
        //unsigned char g_ucPowerOffFlg = 0;  //掉电标示
       // unsigned char g_ucPTabnormal = 0;  //电压异常标示
        unsigned char g_ucDuanxianFlg = 0;
#else
 	//extern unsigned int g_gDinRtcLock[2][RTC_NUM];
        
        extern unsigned char g_gucPhaseSQU[3];  //存储相序检测各相过零标志数
        extern unsigned char g_gucPhaseSQUTestNum;  //相序检测各相过零点计数
        //extern unsigned int rh_counter;//燃弧成功遥信 延时计数器  
        extern unsigned int rh_send_ok;//燃弧脉冲发送完成 =1 发出8脉冲 =0          
        extern unsigned int fault_time;            ///////故障计数值
        extern unsigned char fault_begin;               //故障出现延时              
        extern unsigned char fault_end;                 //故障恢复延时
        extern unsigned char fault_pluse;//故障相位        
        extern unsigned char fault_lubo;
        
        extern unsigned char abnormal_counter;              //////装置异常计数器，机构5次报异常

        extern unsigned char save_abnormal;
        extern unsigned char fault_save;             ///////故障存储标志,同时做故障发生标志，0：无故障，0x33：有故障

        extern unsigned char upload_GPRS;
        extern  unsigned char upload_SMS;              ////每隔60秒钟发送一条短信。
        extern unsigned char SMS_delay;               ////延时短信延时每隔2秒钟发送一条短信。   
        //extern unsigned int   km_timeout; 
        //extern unsigned int   i0_timeout; 		
	 //extern unsigned int   ptoff_timeout;	
	 extern unsigned char soft_latch;	
        extern unsigned char main_reset_flag;//=0x00;            /////////主板复位标志
        extern unsigned char flash_write;                 ////////写录波数据到FLASH

        extern unsigned char upload_flag;               ///the of upload                3           2         1           0
        extern unsigned char upload_interval[4];        ///self upload interval        断线    eight_pulse power_down  abnormal  
        extern unsigned char upload_interval_set;       ///transmit interval parameter default 3 minutes   the range is 3-15 minutes  

//注:2013-2-14 增加全局参数数组,方便通信
       // extern unsigned char g_ucNamePARA[44];

//=============================== 调用外部参数 ===============================///
        
        extern unsigned char eight_ctl_flag;              ///////8脉冲遥控标志,有效8脉冲后，遥控输出5秒
        extern unsigned char power_ctl_flag;              ///////断电遥控标志
        extern unsigned char abnormal_ctl_flag;           ///////异常遥控标志
        
        extern unsigned char phase_off;               ////////////上电认为出现断线
        extern unsigned char phase_copy;              ////////////上电认为出现断线
        
        extern unsigned char save_power_off;             ////////////存储断电标志 
        extern unsigned char power_off_flag;           ///////断电标志
        
        extern unsigned char save_phase_off;          //存储断线标志
        extern unsigned char latch_upload_flag;         //////////闭锁或者解锁后置标志，主动上发一帧状态帧
        
        extern unsigned int state_counter;             //上传时间间隔计数，分钟计时
        
        extern unsigned char eight_delay_flag;              ////////故障后延时8秒发8脉冲标志            
        extern unsigned char eight_delay_counter;           ////////故障后延时2秒发8脉冲计数器
        
        //extern unsigned char pulse_counter;               ////////对采集的脉冲数进行统计，用作数据存储定位
       
        extern unsigned char comm_rec_flag;
        extern unsigned char phone_perm[4];	//          5        4         3        2       1      0						                    //      故障复归  发生故障 主动上报 有效8脉冲 掉电  状态异						//手机的权限控制字
        extern unsigned char SMS_array[2];
     
        extern unsigned char send_phase;
        extern unsigned int yc[8];	// 8脉冲遥测值

        //extern unsigned char yc1[8];	// 8脉冲遥测值
        extern unsigned char sign_RSSI_time1[6];              ////year month day hour minute second 
        extern unsigned char eight_select;                  /////传动第几次8脉冲

        //extern unsigned char yc2[8];	// 8脉冲遥测值
        //extern unsigned char sign_RSSI_time2[6];              ////year month day hour minute second

        extern unsigned char pulse_phase_flag;              ////////哪一相发8脉冲的标志
        extern unsigned int efslatch_flag;                   ////////闭锁17分钟标志
        extern unsigned char eight_delay_counter;           ////////故障后延时2秒发8脉冲计数器
        extern unsigned char chongfa;                       ////////重发标志 
        extern unsigned char moniguzhang;                       ////////模拟故障
        extern unsigned char phase_check_flag;//=0x55;          /////////检测相位异常标致    0x00 不检测，  0x55准备检测             
        extern unsigned char phase_check_time;//=24;            /////////故障已经检测完     每隔两个小时检测一次，上电检测一次   
        extern unsigned char phase_seq_flag;//=0x00;                 ///////// 初始化设置为正相序   0x00正相许  0x55 逆相序

       // extern unsigned char lcd_ctl;                       /////////液晶是否在发送数据


///////////////////////////写入FLASH中
       // extern unsigned char fault_delay_time;        //故障延时时间    
        //extern unsigned int cal_value[5];

        extern unsigned char VerNumlcd[2]; 


        extern unsigned char sign_RSSI_delay; 
	 extern unsigned char newsms_8pluse; 
         extern unsigned char newsms_abn; 
        //extern unsigned char fault_point;                ////////用来确定是读取的那个录波或者故障记录

        extern unsigned char fault_recovery_flag;//=0;      ///////故障复归标志
        extern unsigned char fault_sms_type;            //////////故障状态，当故障发生时，放在自动上传的短信的最后

        extern unsigned char pulse_success;                   ////////脉冲发送成功,用于液晶显示维持17分钟
        extern unsigned char eight_pulse_flag;              ////////开始发8脉冲的标志


        extern unsigned char repeat_flag1;               //////////铁路型信号源重发标志，检测到电流超过35A，就重发。

        extern unsigned char phase_copy;              ////////////上电认为出现断线

 
        extern unsigned char recieve_confirm_flag;   //  用于决定采用何种应答方式
        extern unsigned char Process_68_flag;
       //extern  unsigned char Note_Flag;

       extern  unsigned char process_type;     //  主站与终端之间通信时的 功能类型 
        //extern unsigned char process_step;     //  每种功能的处理步骤
        extern unsigned char process_pro;      //  功能步骤对应的优先级，主要用于与故障主动上传做比较
       extern  unsigned char command_flag;    // 主动发送数据时的 发送原因
       extern  unsigned char command_flag2;  // 主动发送数据时,增加一个缓冲处理任务
       extern  unsigned char fault_num;
        extern unsigned char process_pro;
       extern  unsigned char event_pro;
      //extern  unsigned char self_chongfa_flag;
       extern  unsigned char uart0_event_flag;
       extern  unsigned char g_ucEarthFlg;  //故障相标示
       //extern  unsigned char g_ucPowerOffFlg;  //掉电标示
      // extern  unsigned char g_ucPTabnormal;  //电压异常标示
        extern unsigned char g_ucDuanxianFlg;
#endif


#ifdef __cplusplus
}
#endif

#endif  //end of file
