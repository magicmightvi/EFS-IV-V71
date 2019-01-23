/*------------------------------------------------------------------------
 Module:            protocol.h
 Author:            linxueqin
 Project:           规约平台
 State:             V1.0
 Creation Date:     2012-10-8
 Description:       规约基类的声明
------------------------------------------------------------------------*/

#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "..\DataStruct.h"
//#include "..\SMS\PrtcSms.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */



//#define   INCLUDE_GB104_S
#define INCLUDE_BJ101_S

#define SOE_BYLEN   6
#define SOE_VALUE_L0             1
#define SOE_VALUE_H0             4000

/*YK*/
#define  MI_YKSELECT  0x01
#define  MI_YKOPRATE  0x02
#define  MI_YKCANCEL  0x03

/*YT*/
#define  MI_YTSELECT  0x06
#define  MI_YTOPRATE  0x07
#define  MI_YTCANCEL  0x08



#define MAX_PUBBUF_LEN      256
#define MAX_RECV_LEN        256
#define MAX_SEND_LEN        256//MAX_PUBBUF_LEN * sizeof(DWORD)
    

//通讯检测位定义
#define FRAME_SHIELD   0xFFFF0000      //屏蔽字
#define FRAME_OK       0x00010000      //检测到一个完整的帧
#define FRAME_ERR      0x00020000      //检测到一个校验错误的帧
#define FRAME_LESS     0x00030000      //检测到一个不完整的帧（还未收齐）


struct VCommBuf 
{
    WORD   wBufSize;        //缓冲区大小
    WORD   wReadPtr;        //读指针
    WORD   wOldReadPtr;     //读指针
    WORD   wWritePtr;       //写指针
    DWORD  dwFlag;          //控制码(主要用于发送)
    BYTE   *pBuf;           //缓冲区
};



typedef struct YtOrder//[数据状态(0xA5A5)+标识(16bit:0xC3)]+地址(16bit)+命令(16bit)+校验(16bit)
{
    WORD wYtNum;
    WORD wYtNo;
    WORD wOrder;
    WORD wCheck;//校验
}YtOrder;

typedef struct YkOrder//[数据状态(0xA5A5)+标识(16bit:0xC3)]+地址(16bit)+命令(16bit)+校验(16bit)
{
    WORD wYkNo;
    WORD wOrder;
    WORD wCheck;//校验
}YkOrder;

typedef struct OrderHead//数据状态(0xA5A5)+标识(16bit:0xC3)+[地址(16bit)+命令(16bit)+校验(16bit)]
{
    WORD wStatus;
    WORD wSign;
}OrderHead;

typedef struct OrderFeedBack//标识(16bit:0xC3)+数据状态(0xA5A5)
{
    WORD wSign;
    WORD wStatus;
}OrderFeedBack;


/******************comm***************************/
typedef struct VSysClock{
  WORD wYear  ;    /*19XX-20XX*/
  BYTE byMonth  ;  /*1-12*/
  BYTE byDay  ;    /*1-31*/
  BYTE byWeek  ;   /*0-6  from sunday*/
  BYTE byHour  ;   /*0-23*/
  BYTE byMinute  ; /*0-59*/
  BYTE bySecond  ; /*0-59*/
  WORD wMSecond  ; /*0-999*/
  WORD pad1; /*armalign add*/
}VSysClock;


typedef struct  VDBSOE{
  BYTE byNol  ;
  BYTE byNoh  ;
  BYTE byValue  ;
  BYTE byMSecondl;
  BYTE byMSecondh;
  BYTE byMinute;
  BYTE byHour;
  BYTE byDay;
  BYTE byMonth;
  BYTE byYear;
  //struct VCalClock Time  ;//加这个变量后，此结构体指针不能正确指向一块内存
}VDBSOE;


typedef struct VDBCOS{
 WORD wNo ;
  BYTE byValue ;
}VDBCOS;

typedef struct VDBYCF{
  WORD wNo ;
  WORD nValue ;
}VDBYCF;

struct VDBYK{
  WORD wID ;       /*遥控号*/
  BYTE byValue ;   /*遥控属性
                  D0~D1
                   00 非法
                   01 合闸继电器
                   10 分闸继电器
                   11 NULL（无需区分合/分闸继电器）
                  D2：1控制对象为开关号（进行开关号查找）（缺省） 0：控制对象为顺序点号（偏移）
                  D3：保留
                  D4-D7:  0－设备自定义脉冲（缺省）
                          1－短脉冲
                          2 长脉冲
                          3 持续输出*/
  BYTE byStatus ;  /*表示请求命令产生的结果*/
};

struct VDBYT{
  WORD wID ;      /*遥控号*/
  WORD wValue ;   /*遥调值*/
  BYTE byStatus ;
  BYTE pad1[3]; /*armalign add*/
};



struct VMsgHead{
  WORD wLength;
  BYTE byMsgID;    /*D0~D6  MsgID
          D7 请求报文中＝1表示对方回应报文时无需发送紧急事项
           ＝0表示对方不回应报文或回应报文时需发送紧急事项*/
  BYTE byMsgAttr;
  BYTE abyRsv[4];
  BYTE byTypeID;
  WORD wInfoAddr;
  BYTE byDco;
};





struct VYKInfo
{
    VMsgHead Head;
    VDBYK Info;
};

struct VYTInfo
{
    VMsgHead Head;
    VDBYT Info;
};

#define SOE_GRP_NUM    22//每包报文最多包含22条soe 每条SOE占11字节



//=================RF规约和ODU规约==========================================================

#define     R_YCPARA          5
#define     R_BATPARA         6
#define     R_OTHERPARA       7
#define     R_CURRENT         8

#define     CONNECT           21
#define     DISCONNECT        22

#define     REVERT            13
#define     VERT              14
#define     W_YCPARA          15
#define     W_BATPARA         16
#define     W_OTHERPARA       17



#define MAX_FRAME_LEN      255


#define   RF001FTDANUM        17  //每一个指示器的数据蔤占17字节 架空2遥
#define   RF001FTDANUM_A0     9  //一遥每一个指示器的数据蔤占9字节 架空1遥
#define   RF001FTDANUM_3P4O   30  //每一个指示器的数据蔤占30字节 对应电缆3P4O面板指示器


#define   RF003FTDANUM  29  //每一个指示器的数据蔤占29字节


typedef struct sRF001FTDA{
    BYTE byAddr[4];
    BYTE byLinePhase;
    BYTE byGroup[2];
    BYTE byRes;
    BYTE byUH;
    BYTE byIL;
    BYTE byIH;
    BYTE byIMaxL;
    BYTE byIMaxH;
    BYTE byIMinL;
    BYTE byIMinH;
    BYTE byFaultInfo;
    BYTE byUL;  
}RF001FTDA;

typedef struct sRF001FTDA_A0{
    BYTE byAddr[4];
    BYTE byLinePhase;
    BYTE byGroup[2];
    BYTE byRes;
    BYTE byFaultInfo;
}RF001FTDA_A0;

typedef struct sPHASEFTDA_ABC{
    BYTE byExtTmpt;//外部温度
    BYTE byIntTmpt;//内部温度
    BYTE byIL;//电流低字节
    BYTE byIH;//电流高字节
    BYTE byPhFaultInfo;//相故障标志
}PHASEFTDA;

typedef struct sRF001FTDA_3P4O{
    BYTE byAddr[4];
    BYTE byLinePhase;
    BYTE byGroup[2];
    BYTE byDaSt;//传数据状态
    BYTE byFaultOld;//历史故障相
    BYTE byFaultCur;//当前故障相
    BYTE byFaultComm;//通道故障相
    BYTE byPhData[18];//A,B,C 3相故障信息数据 3*5 + O相3字节
    //PHASEFTDA *pPhFtConA;//BYTE byPhData[15];
    //PHASEFTDA *pPhFtConB;
    //PHASEFTDA *pPhFtConC;
    //BYTE byOIL;//O相电流低字节
    //BYTE byOIH;//O相电流高字节
    //BYTE byOFaultInfo;//O相故障标志
    BYTE byFaultInfoRst;//故障信息复位标志
}RF001FTDA_3P4O;

typedef struct sODUFTDA{
    BYTE byLinePhase;//相偏相偏
    BYTE byRes;
    BYTE byFaultVal;//故障标识位
}ODUFTDA;

typedef struct sODUYCDA{
    BYTE byZsqType;//指示器类型
    BYTE byExtTmpt;//外部温度
    BYTE byIntTmpt;//内部温度
    BYTE byIL;//电流低字节
    BYTE byIH;//电流高字节
    BYTE byZsqSt;//指示器状态
}ODUYCDA;

/*
typedef struct sRF003FTDA{
    BYTE byAddr[4];
    BYTE byGroup[4];//组地址1，组地址2，保留字节，组地址3
    BYTE byLinePhase;//线偏，相偏
    BYTE byDevType;//0x1x报警类型，0x2x中继类型，0x4x终端类型
    BYTE byIMaxH;
    BYTE byIMaxL;
    BYTE byIMinH;
    BYTE byIMinL;
    BYTE byIAve[2];//平均电流 高位在前
    BYTE byUAve[2];//平均电压
    BYTE byTmp[2];//温度1，温度2
    BYTE byUBat;//电池电压，系数=0.1
    BYTE byStInfo;//状态信息
    BYTE byFaultInfo;//故障信息
    BYTE byTrsDevTp;//转发设备类型
    BYTE byRouteNum;//路由次数
    BYTE byTime[4];//包含：年、月、日、时、分、秒
    
}RF003FTDA;

*/
typedef struct sRFASDU1{
    BYTE bySubType;
    BYTE byInfoNum;
    BYTE byContent[MAX_FRAME_LEN-6];
}RFASDUSUBTP;
typedef struct sRFASDU2{
    BYTE byInfoNum;
    BYTE byContent[MAX_FRAME_LEN-5];
}RFASDU;
typedef struct sRFASDU3{  //指示器上传参数
    BYTE SubType1;
    BYTE IPAddres[5];
    BYTE SubType2;
    BYTE Frame1;
    BYTE ByContenLen;
    BYTE ByConten[16];
}RF001ASDU;

typedef struct 
{
    BYTE Start; 
    BYTE Control; 
    BYTE Data[4];//BYTE Address; 
}VFrame10; 

typedef struct 
{
    BYTE Start1; 
    BYTE Length1; 
    BYTE Length2; 
    BYTE Start2; 
    BYTE Control; 
    BYTE Data[MAX_FRAME_LEN-5];//BYTE LinkAddress; 
}VFrame68;



typedef struct 
{
    BYTE Start1; 
    BYTE Start2;
    BYTE FrameLen;//include frame head and tail
    BYTE Type;
    union{
        RFASDUSUBTP RfAsduSubTp;
        RFASDU RfAsdu;
                RF001ASDU Rf001Asdu;
        }RFASDU;
}VFrame55;

typedef struct 
{
    BYTE Start[4]; 
    BYTE Type;
    BYTE FrameLen[2];
    BYTE byContent[MAX_FRAME_LEN-7];
}VFrameAA;

typedef struct 
{
    BYTE Start0;
    BYTE Start1; 
    BYTE Start2;
    BYTE FrameLen;//include frame head and tail
    BYTE Type;
    union{
        RFASDUSUBTP RfAsduSubTp;
        RFASDU RfAsdu;
        }RFASDU;
}VFrameFF;

typedef struct 
{
    BYTE Start1; 
    BYTE Length1; 
    BYTE Length2; 
    BYTE Start2; 
    BYTE Control;
    BYTE Addr_odu;
    BYTE Type_odu;
    BYTE VSQ_odu;
    BYTE COT_odu;
    BYTE Data[MAX_FRAME_LEN-9];//BYTE LinkAddress; 
}VFrame67; 
typedef struct 
{
    BYTE Start1; 
    BYTE Length1; 
    BYTE Length2; 
    BYTE Start2; 
    BYTE Control; 
	BYTE LinkAddr[2];
    BYTE Type;
    BYTE VSQ;
    BYTE COT[2];
	BYTE ConAddr[2];
	BYTE InfoAddr[2];
    BYTE Data[MAX_FRAME_LEN-15];//从文件名开始
}VFrame69;//69报文的链路地址，传输原因，公共地址和信息体地址长度都为两字节
typedef struct 
{
    BYTE Start1; 
    BYTE Length1; 
    BYTE Length2; 
    BYTE Start2; 
    BYTE Control; 
	BYTE LinkAddr[2];
    BYTE Type;
    BYTE VSQ;
    BYTE COT[2];
	BYTE ConAddr[2];
	BYTE InfoAddr[2];
    BYTE Data[MAX_FRAME_LEN-15];//从文件名开始
}VFrame66;//66报文的链路地址，传输原因，公共地址和信息体地址长度都为两字节

typedef struct 
{
    BYTE Start1; 
    BYTE LengthH; 
    BYTE LengthL; 
    BYTE Start2; 
    BYTE FrameType[2];  //报文类型,高位在前，高位为0
    BYTE AppType;       //应用类型
    BYTE Data[MAX_ENC_FRAME_LEN-8];//
}VFrameEB;//EB加密报文的


typedef union 
{
  VFrame69 Frame69;//为扩展才定义此共用体
  VFrame66 Frame66;//为扩展才定义此共用体
  VFrameAA FrameAA;
} VParaFrame; 

//====================================================================




/*------------------------------------------------------------------------
 规约基类定义
------------------------------------------------------------------------*/
class CProtocol
{
    public:

        CProtocol();//构造函数
        virtual ~CProtocol(){};
        virtual BOOL Init(WORD wMinEqpNum);
        virtual void initpara(BYTE flag = 0){return;};
        virtual BOOL SetEqpInfo(void){return 1;};                      
        virtual DWORD SearchOneFrame(BYTE *Buf,WORD Len){return FRAME_OK|Len;};
        virtual BOOL DoReceive(void){ return TRUE; };
        virtual BOOL DoSend(void){ return TRUE; };
		virtual WORD GetOwnAddr(void);
		virtual void DisableRetry(void){return;};   //禁止重发
        virtual void DoCommSendIdle(void){DoSend();};
        virtual BOOL DoTimeOut(WORD wTimerID){return FALSE;};
        virtual void CloseTcp(BYTE byReason = 0){return;};
		virtual void Run(void){return;}; 
	virtual BOOL SendFrameHead(BYTE Style, BYTE Reason){return TRUE;};
	virtual BOOL SendFrameHead_ZS(BYTE Style, BYTE Reason){return TRUE;};
	virtual BOOL SendFrameTail(BYTE PRM, BYTE dwCode, BYTE Num,BYTE EncType){return TRUE;};
	virtual BOOL SendFrameTail_ZS(BYTE PRM, BYTE dwCode, BYTE Num,BYTE EncType){return TRUE;};
	virtual void write_infoadd(int  data){return;};
               
        void NeatenCommBuf(VCommBuf *pCommIO);
        BOOL SearchFrame(void);
        WORD SearchHead(BYTE *Buf, WORD Len,short Offset1,BYTE Key1);  
        int RcvData(int* iSocket);  
        int CommWrite(char* pbuf, int buflen, DWORD flags=0);
        BYTE ChkSum(BYTE *buf, WORD len);
		BOOL SendFrameHeadForPa(BYTE Style, BYTE Reason);
		BOOL SendFrameTailForPa(void);

		//参数读写部分
		BOOL RecFrame69(void);
		BOOL RecFrame66(void);
		//BOOL RecFrameAA(void);//张弢 0404 短信接收
		BOOL RecReadFile();
		BOOL RecWriteFile();
		BOOL RecSendSMSOK();
		BOOL RecSMS();
		void SendWrPaSuc();
		void SendReadPa(WORD FileName,BYTE SecName);
                void SendReadRecData(WORD FileName,WORD SecName);  //读取录波和索引
                
		
		void SendWrPaAck(WORD wFile,BYTE bySec);
		void SendSavedSoe(BYTE bySec,WORD wStartNo,WORD wSOENum);
		void SendLoadData(BYTE bySec,WORD wStartNo,WORD wLoadNum,BYTE byQds=0);
		void SendOneLoadData(WORD wCurLoadNum,WORD wFlag=0);
		void SendSoeSavedStu();
		void SendTrigAck();
		void ChangeIP(BYTE *pDa);  		

public:
		VParaFrame *pReceiveFrame; 	//接收帧头指针		
		WORD m_wSendPaNum;		//已经发送的参数个数，控制多帧传送用
		

        VCommBuf m_RecBuf;              
        VCommBuf m_RecFrame;          
        VCommBuf m_SendBuf; 
        int m_uartId;
		CProtocol *m_RWPaSrcObj;//正在读/写rf或ODU参数的上行通道

        BYTE m_initflag_ZS;//初始化=0；10C9后为ON；接收到1000后为OFF，m_linkflag=1 
        BYTE m_SBuf[256];
        BYTE m_RBuf[256];
        BYTE m_initflag;//初始值为0111，请求远方链路状态后变为0110，复位远方链路后变为0100，发初始化结束帧后为0000
        BYTE m_linkflag; //1=链路初始化完成
        BYTE m_zdflag;//主动上传标志，主动上传后置1，收到主站回复后置0
		BYTE m_initfirstflag;//初始化为1，只在上电后的第一次发送初始化
	BYTE g_gGprsCXip;
	BYTE g_gGprsSETip;
	BYTE g_gGprsCXVSQ;
	BYTE m_GprsVSQ;		
};




#endif
