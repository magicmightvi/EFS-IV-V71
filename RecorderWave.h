
#ifndef _RECORDER_WAVE_H_
#define _RECORDER_WAVE_H_

#ifdef  RECORDER_WAVE_GLOBALS
#define RECORDER_WAVE_EXT
#else
#define RECORDER_WAVE_EXT  extern
#endif
/*配置文件信息 第一行   站名、录波装置编号、版本年代*/
/*第二行    通道的种类和数量*/
/*第三行开始 4个模拟通道的信息*/
/*第三行, 第1个模拟通道的信息*/
/*无 开关量（数字）通道信息*/
/* 第七行,  线路频率*/
/* 第八行,  DAT文件中所用到的采样速率个数*/
/* 第九行,  采样信息*/
/* 第十行,  用于数据文件中第一个数据值的时间*/
/* 第十一行,第二个用于触发点的时间*/
/* 第十二行,数据文件格式*/
/* 第十三行,时间倍乘因数，它和DAT文件中的timestamp参数一起使用，timemult×timestamp*/
/*
const char ComtrderCfg[]=
"The Recording File of Indicator,108,1999\n\
5,5A,0D\n\
1,Ua,A,,V,1,0,0,0,65535,1,1,S\n\
2,Ub,B,,V,1,0,0,0,65535,1,1,S\n\
3,Uc,C,,V,1,0,0,0,65535,1,1,S\n\
4,U0,0,,V,1,0,0,0,65535,1,1,S\n\
5,I0,0,,A,1,0,0,0,65535,1,1,S\n\
50\n\
1\n\
4000,640\n\
2010/03/15,12:20:27.672\n\
2010/03/15,12:20:27.732\n\
ASCII\n\
1";
*//**/
const char ComtrderCfg[]=
",1999\n\
8,5A,3D\n\
1,Ua,A,,V,8.45,0,0,0,65535,1,1,S\n\
2,Ub,B,,V,8.45,0,0,0,65535,1,1,S\n\
3,Uc,C,,V,8.45,0,0,0,65535,1,1,S\n\
4,U0,0,,V,8.45,0,0,0,65535,1,1,S\n\
5,I0,0,,A,0.0764,0,0,0,65535,1,1,S\n\
1,a,0,0,0\n\
2,b,0,0,0\n\
3,c,0,0,0\n\
50\n\
1\n\
";

const char ComtrderCfg_1[]=
",1999\n\
8,5A,3D\n\
";
const char ComtrderCfg_ua[]=
"1,Ua,A,,V,";
const char ComtrderCfg_ub[]=
"2,Ub,B,,V,";
const char ComtrderCfg_uc[]=
"3,Uc,C,,V,";
const char ComtrderCfg_u0[]=
"4,U0,0,,V,";
const char ComtrderCfg_i0[]=
"5,I0,0,,A,";
const char ComtrderCfg_2[]=
",0,0,0,65535,1,1,S\n\
";
const char ComtrderCfg_3[]=
"1,a,0,0,0\n\
2,b,0,0,0\n\
3,c,0,0,0\n\
50\n\
1\n\
";
#ifdef SD_101S
const char ACTCfg[]=
"800,16000\n\
";

const char RECCfg[]=
"2000,640\n\
";
#else
const char ACTCfg[]=
" 800,16000\n\
";

const char RECCfg[]=
"4000,640\n\
";
#endif
const char CfgEnd[]=
"ASCII\n\
1\n\
";

const char CfgEnd1[]=
"BINARY\n\
1\n\
";

/*
const char ComtrderCfg[]=
"The Recording File of Indicator 108,1999\n\
8,8A,0D\n\
1,Ia,A,,A,1,0,0,0,65535,1,1,S\n\
2,Ib,B,,A,1,0,0,0,65535,1,1,S\n\
3,Ic,C,,A,1,0,0,0,65535,1,1,S\n\
4,I0,0,,A,1,0,0,0,65535,1,1,S\n\
5,Ua,A,,V,1,0,0,0,65535,1,1,S\n\
6,Ub,B,,V,1,0,0,0,65535,1,1,S\n\
7,Uc,C,,V,1,0,0,0,65535,1,1,S\n\
8,U0,0,,V,1,0,0,0,65535,1,1,S\n\
50\n\
1\n\
4000,1200\n\
2010/03/15,12:20:27.672\n\
2010/03/15,12:20:27.772\n\
ASCII\n\
1";
*/
  /*类型标识*/
#define  FILEPREPAR_TYPE 120//文件准备就绪类型标识
#define  SECTIONPREPAR_TYPE 121//节准备就绪.请求文件传输
#define  CALL_TYPE 122//召唤目录（可选），选择文件（可选），召唤文件，召唤节
#define  LASTSECTION_TYPE 123 //最后的节，最后的段
#define  APPROVALFILE_TYPE 124 //认可文件，认可节
#define  SEGMENT_TYPE 125  //段
#define  CATALOG_TYPE 126  //目录

#define  CALL_TYPE_YN 136//召唤目录（可选）
#define  CALL_FILE_YN 137//召唤文件，召唤节
#define  CALL_ONEFRAME_YN 138//补包 单独召唤文件中的一帧

    /*传送原因*/
#define  BREAK_COT  3 //突发(自发)
#define  REQUEST_COT  5 //请求或者被请求
#define  FILETRANS_COT  13 //文件传输

#define  SEGMENT_LENGTH     234;// 发送段数据时的长度

#define  DAT_LENGTH     1200*16;// 发送数据文件的长度，每相电流1600个点，每个点2个字节，共A/B/C/0四相，电压数量相同

#define  CRG_TIME_START 304; //CFG文件中第一个数据值的时间
#define  CRG_TIME_END 328; //CFG文件中触发时间的起始位置
#define  CRG_SAMPLE_FRQUENCY 4000; //CFG文件中采样频率
#define  CRG_SAMPLE_NUM 1200; //CFG文件中采样个数

struct RECORDER_CFG//配置文件的录波目录
{
    
    //unsigned char lubo_total_num;//传输CFG录波文件时当前的个数，比如前面已经生成9个录波文件，则该值为10，传目录信息时需要用
    unsigned char FileName;
    unsigned int comtrade_time[RTC_NUM];//故障发生时间，指示器的录波为终端收到指示器录波的时间,同时是文件建立时间
    unsigned int comtrade_time1[RTC_NUM];
    unsigned int yx_time1[RTC_NUM];	//对应的遥信时间
    //unsigned int CFG_Leng;//配置文件的长度
    
    DWORD CFG_Leng; 
    unsigned int CFG_Samp;	//采样频率  故障录波=4000，动作录波=800
    unsigned int CFG_EndSamp;	//采样个数  故障录波=640，动作录波=不定产 最长25秒20000条    
    unsigned int CFG_SOF;//配置文件的状态
    //unsigned int comtrade_Stime[RTC_NUM];//
    //unsigned int DATA_SOF;//配置文件的状态
    DWORD DATA_Leng;//data长度
    DWORD TOTAL_Leng; 
	unsigned char lubo_flag;//录波标志位
	//WORD     relative_time1;
	//WORD     relative_time2;
	//WORD     relative_time3;
};

struct FLAG_RECORDER
{
    //unsigned char CFG_flag;//是否在读配置文件的数据
    //unsigned char DAT_flag;//是否在读数据文件的数据
    unsigned char LIST_flag;//是否在读目录文件的数据
    unsigned char pRXBuff[64]; //保存101规约中68长帧的数据
};

struct RE_SEND_REC
{
   unsigned char *res_pTxBuf;
   unsigned char res_leng;
   WORD res_wSecLenPtr;
   int res_segment_leng;
   unsigned char res_timeout;
};
struct SOE_RECORDER
{	  
	  unsigned int Soe_Area;//每段SOE存储起始地址
	  unsigned int Soe_Ptr;//保存SOE存储起始地址，防止阐述过程中SOE增加
	  unsigned int soe_count;////保存soe数量，防止传输过程中SOE增加
	  unsigned char follow_mark;//后续标志
	  unsigned int Soe_Curren_count;//已发送SOE数量
	  unsigned char Soe_count_flag;
	  //char soeBuff[100]; //文件内容数据流
	  //unsigned char check_sum;
	  
};

struct FIXPT_RECORDER
{	  
	  unsigned char fixpt_Ptr;
	  unsigned char fixpt_count;//文件内容的偏移指针值。
	  unsigned char fixpt_Curren_count;
	  unsigned char fixpt_count_flag;
	  //char soeBuff[100]; //文件内容数据流
	  //unsigned char check_sum;	  
};
struct LOG_RECORDER
{	  
	unsigned char log_Ptr;
	unsigned char log_count;//文件内容的偏移指针值。
	unsigned char log_Curren_count;
	unsigned char log_count_flag;
	//unsigned char check_sum;	  
};

//=====================录波数据===========================================
/*#define RECORDER_TOTALNUM  0//在FLASH中保存的录波记录的条数
#define RECORDER_NEW       1//即将存储的录波与第一条录波的偏移位置，
#define RECORDER_OLD       2//最老一条录波存储位置
#define RECORDER_CS        3//以上三个数据加和校验

#define RECORDERNUM  4*/



//";
//RECORDER_WAVE_EXT unsigned long FLbAddrStart;
//RECORDER_WAVE_EXT unsigned long FLbAddrEnd;
//const char  ComtrderCfg2[]=
//"//
RECORDER_WAVE_EXT DWORD TOTAL_Leng; 
RECORDER_WAVE_EXT DWORD Fixpt_TOTAL_Leng;
RECORDER_WAVE_EXT WORD Fixpt_SECTION_Leng;

RECORDER_WAVE_EXT DWORD Log_TOTAL_Leng; 
RECORDER_WAVE_EXT unsigned char LuboGetCtrCode(BYTE control);
RECORDER_WAVE_EXT void Code_Lubo(unsigned char *p=null,unsigned char *pTxBuf=null);
RECORDER_WAVE_EXT void Code_Lubo_YN(unsigned char *p=null,unsigned char *pTxBuf=null);
//#ifdef YN_101S
RECORDER_WAVE_EXT struct RECORDER_CFG gRecorder_cfg[2/*FRECORDER_TOLNUM*/];
//#endif
RECORDER_WAVE_EXT struct RECORDER_CFG gRecorder_filecfg;
RECORDER_WAVE_EXT struct RECORDER_CFG gRecorder_Readfilecfg;
//RECORDER_WAVE_EXT int wave_total;//录波总条数
RECORDER_WAVE_EXT void cfg_dat_length(int file_current_num);  //计算文件长度的函数
RECORDER_WAVE_EXT struct FLAG_RECORDER gRecorder_flag;  //用来标识是否在读录波数据
RECORDER_WAVE_EXT struct RE_SEND_REC gRes_rec;  //用来标识是否在读录波数据
//RECORDER_WAVE_EXT char ComtrderCfg[1000];
//BYTE ChkSum(BYTE *buf, WORD len);
RECORDER_WAVE_EXT BOOL SendCFGGroup(WORD CFGGroup,WORD GroupNo);
RECORDER_WAVE_EXT BYTE GLubodat_Sum;//数据文件的校验，第二节的校验
RECORDER_WAVE_EXT BYTE GLubocfg_Sum;//配置文件的校验，第一节的校验
RECORDER_WAVE_EXT BYTE GLubo_Sum;//两个文件的校验
RECORDER_WAVE_EXT BYTE g_lubo_erase;
RECORDER_WAVE_EXT  unsigned int  wSendLISTNum ;////= 0已经发送的目录的条数，最多发18条
RECORDER_WAVE_EXT  unsigned int  BK_FRecorder_Current_COUNT ;//从当前的指针发送录波数据

RECORDER_WAVE_EXT unsigned char * FileDirectory(unsigned char *pTxBuf, unsigned char leng,unsigned int wave_total);
RECORDER_WAVE_EXT unsigned char * FileDirectory_YN(unsigned char *pTxBuf, unsigned char leng,unsigned int wave_total);
RECORDER_WAVE_EXT unsigned char * FileDataCfg(unsigned char *pTxBuf, unsigned char leng,RECORDER_CFG *pgRecorder_cfg,int segment_leng);//CFG文件WORD wSecLenPtr,
RECORDER_WAVE_EXT unsigned char * FileDatadat(unsigned char pTxBuf,unsigned char leng,WORD wSecLenPtr,int segment_leng);//dat文件
RECORDER_WAVE_EXT unsigned char * ApprovalFile(unsigned char *pTxBuf,unsigned char leng,RECORDER_CFG *pgRecorder_cfg,WORD wSecLenPtr);//认可文件、节的流程处理
RECORDER_WAVE_EXT unsigned char * SectionPrepareFile(unsigned char *pTxBuf,unsigned char leng,RECORDER_CFG *pgRecorder_cfg);//请求文件传输

RECORDER_WAVE_EXT unsigned char *  RecCallFile_YN(unsigned char *pTxBuf, unsigned char leng,unsigned char filename);//云南录波协议 响应文件召唤
RECORDER_WAVE_EXT unsigned char * FileDataCfg_YN(unsigned char *pTxBuf, unsigned char leng,RECORDER_CFG *pgRecorder_cfg,int segment_leng);//CFG文件WORD wSecLenPtr,
RECORDER_WAVE_EXT unsigned char * FileDatadat_YN(unsigned char pTxBuf,unsigned char leng,WORD wSecLenPtr,int segment_leng);//dat文件
RECORDER_WAVE_EXT unsigned char * FileCfgOneFrame_YN(unsigned char *pTxBuf, unsigned char leng,RECORDER_CFG *pgRecorder_cfg,int framenum);//CFG文件WORD wSecLenPtr,
RECORDER_WAVE_EXT unsigned char * FileDatOneFrame_YN(unsigned char pTxBuf,unsigned char leng,WORD wSecLenPtr,int framenum);//dat文件
//RECORDER_WAVE_EXT 
RECORDER_WAVE_EXT unsigned char  check;
RECORDER_WAVE_EXT unsigned char *SectionPrepareSoe(unsigned char *pTxBuf,unsigned char leng);
RECORDER_WAVE_EXT unsigned char *SectionPrepareFixpt(unsigned char *pTxBuf,unsigned char leng);
RECORDER_WAVE_EXT unsigned char *FixptData(unsigned char *pTxBuf,unsigned char leng,int segment_leng);
RECORDER_WAVE_EXT unsigned char * SoeData(unsigned char *pTxBuf,unsigned char leng,int segment_leng);
RECORDER_WAVE_EXT unsigned char *SectionPrepareLog(unsigned char *pTxBuf,unsigned char leng);
RECORDER_WAVE_EXT unsigned char *LogData(unsigned char *pTxBuf,unsigned char leng,int segment_leng);
RECORDER_WAVE_EXT WORD  ReadNumSoeFromFlash(WORD *pHeadPtr,WORD TailPtr);

RECORDER_WAVE_EXT void LuBoGetNum(unsigned int wave_total);//unsigned int

RECORDER_WAVE_EXT unsigned char* SoeDirectory(unsigned char *pTxBuf, unsigned char leng,unsigned int wave_total);
RECORDER_WAVE_EXT unsigned char* FixDirectory(unsigned char *pTxBuf, unsigned char leng);
RECORDER_WAVE_EXT  SOE_RECORDER soe_recorded;
RECORDER_WAVE_EXT  unsigned char soe_sum;
RECORDER_WAVE_EXT unsigned char* LogDirectory(unsigned char *pTxBuf, unsigned char leng);

RECORDER_WAVE_EXT  FIXPT_RECORDER fixpt_recorded;
RECORDER_WAVE_EXT  LOG_RECORDER log_recorded;


RECORDER_WAVE_EXT  BYTE fix_YC_NUM;

RECORDER_WAVE_EXT  unsigned char fixpt_sum[96];
RECORDER_WAVE_EXT  BYTE lubo_valid[64];
RECORDER_WAVE_EXT  BYTE lubonum_valid[64];

//RECORDER_WAVE_EXT  BYTE bySoeDa[512];
RECORDER_WAVE_EXT unsigned char *  ApprovalSoeFile(unsigned char *pTxBuf,unsigned char leng,WORD wSecLenPtr);
RECORDER_WAVE_EXT unsigned char wavefix_total;
RECORDER_WAVE_EXT unsigned char wavelog_total;
RECORDER_WAVE_EXT void LuBoNum(unsigned int wave_total);
RECORDER_WAVE_EXT unsigned char *  ApprovalFixFile(unsigned char *pTxBuf,unsigned char leng,WORD wSecLenPtr);
RECORDER_WAVE_EXT  unsigned char log_sum_section;
RECORDER_WAVE_EXT  unsigned char log_sum_total;
RECORDER_WAVE_EXT unsigned char *  ApprovalLogFile(unsigned char *pTxBuf,unsigned char leng,WORD wSecLenPtr);
//RECORDER_WAVE_EXT  char *strtemp ; 
RECORDER_WAVE_EXT unsigned int  BkFlashhead;
//RECORDER_WAVE_EXT WORD m_wSendCFGNum;////已经发送的配置文件的字节数，控制多帧传送用


/*const unsigned char Station_Name[]="The Recording File of Indicator";//最长64个字符
const unsigned char Dev_ID[]="108";//64
const unsigned char  Ver_Year[]="1999";//4

  第二行    通道的种类和数量

const unsigned char Channel_Num[]="4";//7
const unsigned char AChannel_Num[]="4A";//7
const unsigned char DChannel_Num[]="0D";//7

第三行开始 4个模拟通道的信息
第三行, 第1个模拟通道的信息
const unsigned char AChannel_Index[]="1";//6
const unsigned char AChannel_ID[]="Ia";//64
const unsigned char AChannel_PHASE[]="A"//2
const unsigned char AChannel_CIRCUITRY[]="";//64
const unsigned char AChannel_UNIT[]="A";//32
const unsigned char AChannel_MULTI[]="1";//32
const unsigned char AChannel_OFFSET[]="0";//32
const unsigned char AChannel_SKEW[]="0";//32
const unsigned char AChannel_MIN[]="0"//6
const unsigned char AChannel_MAX[]="65535"//6
const unsigned char AChannel_PRIMARY[]="1";//32
const unsigned char AChannel_SECONDARY[]="1";//32
const unsigned char AChannel_PS="S";
第四行, 第2个模拟通道的信息
const unsigned char AChannel_Index[]="2";//6
const unsigned char AChannel_ID[]="Ib";//64
const unsigned char AChannel_PHASE[]="B"//2
const unsigned char AChannel_CIRCUITRY[]="";//64
const unsigned char AChannel_UNIT[]="A";//32
const unsigned char AChannel_MULTI[]="1";//32
const unsigned char AChannel_OFFSET[]="0";//32
const unsigned char AChannel_SKEW[]="0";//32
const unsigned char AChannel_MIN[]="0"//6
const unsigned char AChannel_MAX[]="65535"//6
const unsigned char AChannel_PRIMARY[]="1";//32
const unsigned char AChannel_SECONDARY[]="1";//32
const unsigned char AChannel_PS="S";

第五行, 第3个模拟通道的信息
const unsigned char AChannel_Index[]="3";
const unsigned char AChannel_ID[]="Ic";
const unsigned char AChannel_PHASE[]="C"
const unsigned char AChannel_CIRCUITRY[]="";
const unsigned char AChannel_UNIT[]="A";
const unsigned char AChannel_MULTI[]="1";
const unsigned char AChannel_OFFSET[]="0";
const unsigned char AChannel_SKEW[]="0";
const unsigned char AChannel_MIN[]="0"
const unsigned char AChannel_MAX[]="65535"
const unsigned char AChannel_PRIMARY[]="1";
const unsigned char AChannel_SECONDARY[]="1";
const unsigned char AChannel_PS="S";
第六行, 第4个模拟通道的信息
const unsigned char AChannel_Index[]="4";
const unsigned char AChannel_ID[]="I0";
const unsigned char AChannel_PHASE[]="0"
const unsigned char AChannel_CIRCUITRY[]="";
const unsigned char AChannel_UNIT[]="A";
const unsigned char AChannel_MULTI[]="1";
const unsigned char AChannel_OFFSET[]="0";
const unsigned char AChannel_SKEW[]="0";
const unsigned char AChannel_MIN[]="0"
const unsigned char AChannel_MAX[]="65535"
const unsigned char AChannel_PRIMARY[]="1";
const unsigned char AChannel_SECONDARY[]="1";
const unsigned char AChannel_PS="S";

无 开关量（数字）通道信息
 第七行,  线路频率
const unsigned char line_frequency[]="50.0";//32
 第八行,    DAT文件中所用到的采样速率个数
const unsigned char Num_rates[]="1";//3
 第九行,    采样信息
const unsigned char Sample_Frquency[]="1000";//32
const unsigned char Sample_Num[]="200";//10

 第十行,    采样信息
const unsigned char Sample_Frquency[]="1000";//32
const unsigned char Sample_Num[]="200";//10
 第十一行,  采样信息*/
/*const unsigned char Sample_Frquency[32]="1000";
const unsigned char Sample_Num[10]="200";

typedef struct 
{
  unsigned char *pStation_Name; 
  unsigned char *PDev_ID; 
  unsigned char *PVer_Year; 
}Station_Info;    //站名、录波装置编号、版本年代


typedef struct 
{
  unsigned char PChannel_Num; 
  unsigned char PAChannel_Num; 
  unsigned char PDChannel_Num; 
}Channel_Info; //   通道的种类和数量

typedef struct 
{
  unsigned char pAChannel_Index; //[6];
  unsigned char pAChannel_ID; //[64];
  unsigned char pAChannel_PHASE; //[2];
  unsigned char pAChannel_CIRCUITRY; //[64];//电路组件
  unsigned char pAChannel_UNIT; //[32]; 
  unsigned char pAChannel_MULTI; //[32]; 
  unsigned char pAChannel_OFFSET; //[32]; 
  unsigned char pAChannel_SKEW; //[32]; //采样中通道间的时间差
  unsigned char pAChannel_MIN; //[6]; 
  unsigned char pAChannel_MAX; //[6]; 
  unsigned char pAChannel_PRIMARY; //[32]; 
  unsigned char pAChannel_SECONDARY; //[32];
  unsigned char pAChannel_PS;//P、p、S、s 
} AChannel_Info; //         模拟通道信息

AChannel_Info    achannel_info[Channel_Info.PAChannel_Num];

typedef struct 
{
  unsigned char DChannel_Index[6];
  unsigned char DChannel_ID[64];
  unsigned char AChannel_PHASE[2];
  unsigned char AChannel_CIRCUITRY[64];//电路组件
  unsigned char DChannel_STATE; 
  
} DChannel_Info; // 开关量（数字）通道信息

typedef struct 
{
  unsigned char Sample_Frquency[32];//采样频率
  unsigned char Sample_Num[10];
  
}Sample_Info; // 采样信息

typedef struct 
{
  unsigned char StartTime_date[2];//采样频率
  unsigned char StartTime_month[2];
  unsigned char StartTime_year[4];
  unsigned char StartTime_hour[2];
  unsigned char StartTime_minute[2];
  unsigned char StartTime_Second[9];
} StartTime_Info; // 采样信息

typedef struct 
{
  unsigned char End_date[2];//采样频率
  unsigned char End_month[2];
  unsigned char End_year[4];
  unsigned char End_hour[2];
  unsigned char Ende_minute[2];
  unsigned char End_Second[9];
} EndTime_Info; // 采样信息

*****************CFG***************************/
/*typedef struct ComtrderCfg
{
  Station_Info   station_info;   //     站名、录波装置编号、版本年代
  Channel_Info   channel_info;  //1-12
  AChannel_Info  achannel_info;
  DChannel_Info  dchannel_info; 
  unsigned char  line_frequency[32];//线路频率
  unsigned char  Num_rates[3];//有几个采样频率
  Sample_Info    sample_info;
  StartTime_Info starttime_info;
  EndTime_Info   endtime_info;
  unsigned char  Date_Type[6];//ASCII、ascii、BINARY或者binary
  unsigned char  TimeMult[32];//    时间倍乘因数
}ComtrderCfg;*/


#define  FADDR_LOG_START 				2
#define  FLASH_DAYLOAD_LEN 				2

#define  RM_1A_LUBO 				2
#define  RTC_WEEK 				2
#define  FSOE_LEN 2
#define  FSOE_TOLNUM 2
#define  FLASH_PLOG_LEN 2


//=====================日志记录ulog===========================================
#define FLOG_TOTALNUM  0//在FLASH中保存的日志记录的条数
#define FLOG_NEW       1//即将存储的记录与第一条记录的偏移位置，
#define FLOG_OLD       2//最老一条记录存储位置
#define FLOG_CS        3//以上三个数据加和校验
#define FLOGINFONUM  4

#endif






