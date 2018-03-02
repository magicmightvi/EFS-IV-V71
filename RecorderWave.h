
#ifndef _RECORDER_WAVE_H_
#define _RECORDER_WAVE_H_

#ifdef  RECORDER_WAVE_GLOBALS
#define RECORDER_WAVE_EXT
#else
#define RECORDER_WAVE_EXT  extern
#endif
/*�����ļ���Ϣ ��һ��   վ����¼��װ�ñ�š��汾���*/
/*�ڶ���    ͨ�������������*/
/*�����п�ʼ 4��ģ��ͨ������Ϣ*/
/*������, ��1��ģ��ͨ������Ϣ*/
/*�� �����������֣�ͨ����Ϣ*/
/* ������,  ��·Ƶ��*/
/* �ڰ���,  DAT�ļ������õ��Ĳ������ʸ���*/
/* �ھ���,  ������Ϣ*/
/* ��ʮ��,  ���������ļ��е�һ������ֵ��ʱ��*/
/* ��ʮһ��,�ڶ������ڴ������ʱ��*/
/* ��ʮ����,�����ļ���ʽ*/
/* ��ʮ����,ʱ�䱶������������DAT�ļ��е�timestamp����һ��ʹ�ã�timemult��timestamp*/
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
  /*���ͱ�ʶ*/
#define  FILEPREPAR_TYPE 120//�ļ�׼���������ͱ�ʶ
#define  SECTIONPREPAR_TYPE 121//��׼������.�����ļ�����
#define  CALL_TYPE 122//�ٻ�Ŀ¼����ѡ����ѡ���ļ�����ѡ�����ٻ��ļ����ٻ���
#define  LASTSECTION_TYPE 123 //���Ľڣ����Ķ�
#define  APPROVALFILE_TYPE 124 //�Ͽ��ļ����Ͽɽ�
#define  SEGMENT_TYPE 125  //��
#define  CATALOG_TYPE 126  //Ŀ¼

#define  CALL_TYPE_YN 136//�ٻ�Ŀ¼����ѡ��
#define  CALL_FILE_YN 137//�ٻ��ļ����ٻ���
#define  CALL_ONEFRAME_YN 138//���� �����ٻ��ļ��е�һ֡

    /*����ԭ��*/
#define  BREAK_COT  3 //ͻ��(�Է�)
#define  REQUEST_COT  5 //������߱�����
#define  FILETRANS_COT  13 //�ļ�����

#define  SEGMENT_LENGTH     234;// ���Ͷ�����ʱ�ĳ���

#define  DAT_LENGTH     1200*16;// ���������ļ��ĳ��ȣ�ÿ�����1600���㣬ÿ����2���ֽڣ���A/B/C/0���࣬��ѹ������ͬ

#define  CRG_TIME_START 304; //CFG�ļ��е�һ������ֵ��ʱ��
#define  CRG_TIME_END 328; //CFG�ļ��д���ʱ�����ʼλ��
#define  CRG_SAMPLE_FRQUENCY 4000; //CFG�ļ��в���Ƶ��
#define  CRG_SAMPLE_NUM 1200; //CFG�ļ��в�������

struct RECORDER_CFG//�����ļ���¼��Ŀ¼
{
    
    //unsigned char lubo_total_num;//����CFG¼���ļ�ʱ��ǰ�ĸ���������ǰ���Ѿ�����9��¼���ļ������ֵΪ10����Ŀ¼��Ϣʱ��Ҫ��
    unsigned char FileName;
    unsigned int comtrade_time[RTC_NUM];//���Ϸ���ʱ�䣬ָʾ����¼��Ϊ�ն��յ�ָʾ��¼����ʱ��,ͬʱ���ļ�����ʱ��
    unsigned int comtrade_time1[RTC_NUM];
    unsigned int yx_time1[RTC_NUM];	//��Ӧ��ң��ʱ��
    //unsigned int CFG_Leng;//�����ļ��ĳ���
    
    DWORD CFG_Leng; 
    unsigned int CFG_Samp;	//����Ƶ��  ����¼��=4000������¼��=800
    unsigned int CFG_EndSamp;	//��������  ����¼��=640������¼��=������ �25��20000��    
    unsigned int CFG_SOF;//�����ļ���״̬
    //unsigned int comtrade_Stime[RTC_NUM];//
    //unsigned int DATA_SOF;//�����ļ���״̬
    DWORD DATA_Leng;//data����
    DWORD TOTAL_Leng; 
	unsigned char lubo_flag;//¼����־λ
	//WORD     relative_time1;
	//WORD     relative_time2;
	//WORD     relative_time3;
};

struct FLAG_RECORDER
{
    //unsigned char CFG_flag;//�Ƿ��ڶ������ļ�������
    //unsigned char DAT_flag;//�Ƿ��ڶ������ļ�������
    unsigned char LIST_flag;//�Ƿ��ڶ�Ŀ¼�ļ�������
    unsigned char pRXBuff[64]; //����101��Լ��68��֡������
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
	  unsigned int Soe_Area;//ÿ��SOE�洢��ʼ��ַ
	  unsigned int Soe_Ptr;//����SOE�洢��ʼ��ַ����ֹ����������SOE����
	  unsigned int soe_count;////����soe��������ֹ���������SOE����
	  unsigned char follow_mark;//������־
	  unsigned int Soe_Curren_count;//�ѷ���SOE����
	  unsigned char Soe_count_flag;
	  //char soeBuff[100]; //�ļ�����������
	  //unsigned char check_sum;
	  
};

struct FIXPT_RECORDER
{	  
	  unsigned char fixpt_Ptr;
	  unsigned char fixpt_count;//�ļ����ݵ�ƫ��ָ��ֵ��
	  unsigned char fixpt_Curren_count;
	  unsigned char fixpt_count_flag;
	  //char soeBuff[100]; //�ļ�����������
	  //unsigned char check_sum;	  
};
struct LOG_RECORDER
{	  
	unsigned char log_Ptr;
	unsigned char log_count;//�ļ����ݵ�ƫ��ָ��ֵ��
	unsigned char log_Curren_count;
	unsigned char log_count_flag;
	//unsigned char check_sum;	  
};

//=====================¼������===========================================
/*#define RECORDER_TOTALNUM  0//��FLASH�б����¼����¼������
#define RECORDER_NEW       1//�����洢��¼�����һ��¼����ƫ��λ�ã�
#define RECORDER_OLD       2//����һ��¼���洢λ��
#define RECORDER_CS        3//�����������ݼӺ�У��

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
//RECORDER_WAVE_EXT int wave_total;//¼��������
RECORDER_WAVE_EXT void cfg_dat_length(int file_current_num);  //�����ļ����ȵĺ���
RECORDER_WAVE_EXT struct FLAG_RECORDER gRecorder_flag;  //������ʶ�Ƿ��ڶ�¼������
RECORDER_WAVE_EXT struct RE_SEND_REC gRes_rec;  //������ʶ�Ƿ��ڶ�¼������
//RECORDER_WAVE_EXT char ComtrderCfg[1000];
//BYTE ChkSum(BYTE *buf, WORD len);
RECORDER_WAVE_EXT BOOL SendCFGGroup(WORD CFGGroup,WORD GroupNo);
RECORDER_WAVE_EXT BYTE GLubodat_Sum;//�����ļ���У�飬�ڶ��ڵ�У��
RECORDER_WAVE_EXT BYTE GLubocfg_Sum;//�����ļ���У�飬��һ�ڵ�У��
RECORDER_WAVE_EXT BYTE GLubo_Sum;//�����ļ���У��
RECORDER_WAVE_EXT BYTE g_lubo_erase;
RECORDER_WAVE_EXT  unsigned int  wSendLISTNum ;////= 0�Ѿ����͵�Ŀ¼����������෢18��
RECORDER_WAVE_EXT  unsigned int  BK_FRecorder_Current_COUNT ;//�ӵ�ǰ��ָ�뷢��¼������

RECORDER_WAVE_EXT unsigned char * FileDirectory(unsigned char *pTxBuf, unsigned char leng,unsigned int wave_total);
RECORDER_WAVE_EXT unsigned char * FileDirectory_YN(unsigned char *pTxBuf, unsigned char leng,unsigned int wave_total);
RECORDER_WAVE_EXT unsigned char * FileDataCfg(unsigned char *pTxBuf, unsigned char leng,RECORDER_CFG *pgRecorder_cfg,int segment_leng);//CFG�ļ�WORD wSecLenPtr,
RECORDER_WAVE_EXT unsigned char * FileDatadat(unsigned char pTxBuf,unsigned char leng,WORD wSecLenPtr,int segment_leng);//dat�ļ�
RECORDER_WAVE_EXT unsigned char * ApprovalFile(unsigned char *pTxBuf,unsigned char leng,RECORDER_CFG *pgRecorder_cfg,WORD wSecLenPtr);//�Ͽ��ļ����ڵ����̴���
RECORDER_WAVE_EXT unsigned char * SectionPrepareFile(unsigned char *pTxBuf,unsigned char leng,RECORDER_CFG *pgRecorder_cfg);//�����ļ�����

RECORDER_WAVE_EXT unsigned char *  RecCallFile_YN(unsigned char *pTxBuf, unsigned char leng,unsigned char filename);//����¼��Э�� ��Ӧ�ļ��ٻ�
RECORDER_WAVE_EXT unsigned char * FileDataCfg_YN(unsigned char *pTxBuf, unsigned char leng,RECORDER_CFG *pgRecorder_cfg,int segment_leng);//CFG�ļ�WORD wSecLenPtr,
RECORDER_WAVE_EXT unsigned char * FileDatadat_YN(unsigned char pTxBuf,unsigned char leng,WORD wSecLenPtr,int segment_leng);//dat�ļ�
RECORDER_WAVE_EXT unsigned char * FileCfgOneFrame_YN(unsigned char *pTxBuf, unsigned char leng,RECORDER_CFG *pgRecorder_cfg,int framenum);//CFG�ļ�WORD wSecLenPtr,
RECORDER_WAVE_EXT unsigned char * FileDatOneFrame_YN(unsigned char pTxBuf,unsigned char leng,WORD wSecLenPtr,int framenum);//dat�ļ�
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
//RECORDER_WAVE_EXT WORD m_wSendCFGNum;////�Ѿ����͵������ļ����ֽ��������ƶ�֡������


/*const unsigned char Station_Name[]="The Recording File of Indicator";//�64���ַ�
const unsigned char Dev_ID[]="108";//64
const unsigned char  Ver_Year[]="1999";//4

  �ڶ���    ͨ�������������

const unsigned char Channel_Num[]="4";//7
const unsigned char AChannel_Num[]="4A";//7
const unsigned char DChannel_Num[]="0D";//7

�����п�ʼ 4��ģ��ͨ������Ϣ
������, ��1��ģ��ͨ������Ϣ
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
������, ��2��ģ��ͨ������Ϣ
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

������, ��3��ģ��ͨ������Ϣ
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
������, ��4��ģ��ͨ������Ϣ
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

�� �����������֣�ͨ����Ϣ
 ������,  ��·Ƶ��
const unsigned char line_frequency[]="50.0";//32
 �ڰ���,    DAT�ļ������õ��Ĳ������ʸ���
const unsigned char Num_rates[]="1";//3
 �ھ���,    ������Ϣ
const unsigned char Sample_Frquency[]="1000";//32
const unsigned char Sample_Num[]="200";//10

 ��ʮ��,    ������Ϣ
const unsigned char Sample_Frquency[]="1000";//32
const unsigned char Sample_Num[]="200";//10
 ��ʮһ��,  ������Ϣ*/
/*const unsigned char Sample_Frquency[32]="1000";
const unsigned char Sample_Num[10]="200";

typedef struct 
{
  unsigned char *pStation_Name; 
  unsigned char *PDev_ID; 
  unsigned char *PVer_Year; 
}Station_Info;    //վ����¼��װ�ñ�š��汾���


typedef struct 
{
  unsigned char PChannel_Num; 
  unsigned char PAChannel_Num; 
  unsigned char PDChannel_Num; 
}Channel_Info; //   ͨ�������������

typedef struct 
{
  unsigned char pAChannel_Index; //[6];
  unsigned char pAChannel_ID; //[64];
  unsigned char pAChannel_PHASE; //[2];
  unsigned char pAChannel_CIRCUITRY; //[64];//��·���
  unsigned char pAChannel_UNIT; //[32]; 
  unsigned char pAChannel_MULTI; //[32]; 
  unsigned char pAChannel_OFFSET; //[32]; 
  unsigned char pAChannel_SKEW; //[32]; //������ͨ�����ʱ���
  unsigned char pAChannel_MIN; //[6]; 
  unsigned char pAChannel_MAX; //[6]; 
  unsigned char pAChannel_PRIMARY; //[32]; 
  unsigned char pAChannel_SECONDARY; //[32];
  unsigned char pAChannel_PS;//P��p��S��s 
} AChannel_Info; //         ģ��ͨ����Ϣ

AChannel_Info    achannel_info[Channel_Info.PAChannel_Num];

typedef struct 
{
  unsigned char DChannel_Index[6];
  unsigned char DChannel_ID[64];
  unsigned char AChannel_PHASE[2];
  unsigned char AChannel_CIRCUITRY[64];//��·���
  unsigned char DChannel_STATE; 
  
} DChannel_Info; // �����������֣�ͨ����Ϣ

typedef struct 
{
  unsigned char Sample_Frquency[32];//����Ƶ��
  unsigned char Sample_Num[10];
  
}Sample_Info; // ������Ϣ

typedef struct 
{
  unsigned char StartTime_date[2];//����Ƶ��
  unsigned char StartTime_month[2];
  unsigned char StartTime_year[4];
  unsigned char StartTime_hour[2];
  unsigned char StartTime_minute[2];
  unsigned char StartTime_Second[9];
} StartTime_Info; // ������Ϣ

typedef struct 
{
  unsigned char End_date[2];//����Ƶ��
  unsigned char End_month[2];
  unsigned char End_year[4];
  unsigned char End_hour[2];
  unsigned char Ende_minute[2];
  unsigned char End_Second[9];
} EndTime_Info; // ������Ϣ

*****************CFG***************************/
/*typedef struct ComtrderCfg
{
  Station_Info   station_info;   //     վ����¼��װ�ñ�š��汾���
  Channel_Info   channel_info;  //1-12
  AChannel_Info  achannel_info;
  DChannel_Info  dchannel_info; 
  unsigned char  line_frequency[32];//��·Ƶ��
  unsigned char  Num_rates[3];//�м�������Ƶ��
  Sample_Info    sample_info;
  StartTime_Info starttime_info;
  EndTime_Info   endtime_info;
  unsigned char  Date_Type[6];//ASCII��ascii��BINARY����binary
  unsigned char  TimeMult[32];//    ʱ�䱶������
}ComtrderCfg;*/


#define  FADDR_LOG_START 				2
#define  FLASH_DAYLOAD_LEN 				2

#define  RM_1A_LUBO 				2
#define  RTC_WEEK 				2
#define  FSOE_LEN 2
#define  FSOE_TOLNUM 2
#define  FLASH_PLOG_LEN 2


//=====================��־��¼ulog===========================================
#define FLOG_TOTALNUM  0//��FLASH�б������־��¼������
#define FLOG_NEW       1//�����洢�ļ�¼���һ����¼��ƫ��λ�ã�
#define FLOG_OLD       2//����һ����¼�洢λ��
#define FLOG_CS        3//�����������ݼӺ�У��
#define FLOGINFONUM  4

#endif






