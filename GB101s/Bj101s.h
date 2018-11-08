
#ifndef BJ101S_H
#define BJ101S_H

//#include "..\PUBLIC\Pcolsec.h"
#include "..\SMS\PrtcSms.h"
#include "bj101pub.h"

//接收
#define Rec_FCB			0 

#define CALL_YXGRP1		1
#define CALL_YXGRP2		2
#define CALL_YXGRP3		3
#define CALL_YXGRP4		4
#define CALL_YXGRP5		5
#define CALL_YXGRP6		6
#define CALL_DYXGRP1    7
#define CALL_YCGRP1		8
#define CALL_YCGRP2		9
#define CALL_YCGRP3		10
#define CALL_YCGRP4		11
#define CALL_YCGRP5		12

#define CALL_ALLSTOP	13
#define CALL_LINK		14	 //召唤链路
#define CALL_DATA 		15	 //召唤数据


#define DSOEUFLAG    0x01
#define DCOSUFLAG    0x02
#define SSOEUFLAG    0x04
#define SCOSUFLAG    0x08
#define EVSOEUFLAG   0x10
#define EVCOSUFLAG   0x20
#define FAINFOUFLAG  0x40
#define ROUTEUFLAG   0x80



#define FILEOK   0
#define FILEEOF  1


//SendIdle 消息时应发送的帧类别
#define SEND_LINK		1	
#define SEND_CALLALL	2	
#define SEND_CALLALLDD	3	
#define SEND_BALANCE	4	

#define MODE_BALANCE	1
#define MODE_UNBALANCE	2

#define FILE_NAME_LEN	32

#define	RTU_NULL		0
#define	RTU_RECLINK		1
#define	RTU_RESET		2
#define	RTU_INITEND		3

#define	RTU_RECCALL		4
#define	RTU_SENDDATA		5
#define	RTU_CALLEND		6
#define	RTU_SETTIME		7
#define	RTU_EVENT		8


#define VSQ_SQ  		0x80

#define MAX_FRM_RESEND   4//0x7FFF //报文重发次数 目前处理为对端不响应则无限重发

struct FLAG_RECORDER_FILE
{
    unsigned char LIST_flag;//是否在读目录文件的数据
    //unsigned char pRXBuff[50]; //256保存101规约中68长帧的数据
    unsigned char xuchuanflag; 
};

class CBJ101S : public CPrtcSms//CPSecondary
{
	public:
		VIec101Frame *pReceiveFrame; 	//接收帧头指针
		VIec101Frame *pSendFrame;  		//发送帧头指针
		int m_YkWaitCount;//遥控执行命令发给rf或ODU后等待的时间 单位s
		BYTE m_bReceiveControl;
		BYTE m_byLinkAdrShift;
		BYTE m_byTypeIDShift;
		BYTE m_byReasonShift;
		BYTE m_byCommAdrShift;
		BYTE m_byInfoShift;
		BYTE m_ztype;
		BYTE m_byRTUStatus;
                
                BYTE m_HistLoadFlag;//标识召唤历史遥测的步数和Flag，BIT0遥信 BIT1遥测发送 BIT2遥测发送结束 BIT3召唤结束帧
              //  BYTE m_HistLoadDay;//召唤历史遥测的天数
             //   WORD m_PtrSendLoad;//用于控制发送历史遥测
             //   WORD m_HistLoadNum;//召唤历史遥测的总条数，该数值与储存在E2中的数据有关
                                   //该数据随着发送不断减少
                WORD m_HistSoeSEG; //标识读取SOE时的段，读取时从第0段开始，每段512个字节
                BYTE m_PtrSendSOE; //读取段内SOE时指针位置，2个位置，0或者21
                                   //一条SOE12个字节，一段存42个SOE
                BYTE m_HistLuBoFlag;
		
		WORD m_SYxBit;//标识有单点遥信变位
		WORD m_DYxBit;//标识有双点遥信变位
		WORD m_SSoeBit;//标识有单点SOE
		WORD m_DSoeBit;//标识有双点SOE
		//WORD m_NewFltBit;//标识有新故障产生，为提前结束故障重传时间间隔而设置
		WORD m_BeatBit;//定时发心跳
		WORD m_SendYcBit;//定时发送遥测
		
		Vgb101para m_guiyuepara;
        VDWASDU m_dwasdu;
        BYTE m_callallflag;
        BYTE m_groupflag;
        BYTE m_YKflag;
        BYTE m_YKSelectAck;
        BYTE m_timeflag;
		BYTE m_timeREQflag;
		BYTE m_delayflag;
		BYTE m_time;
        BYTE m_testflag;
        BYTE m_resetflag;
 		BOOL m_DIR;  /*get DIR */
        BYTE m_PRM;
        BYTE m_QPM;
        BYTE m_acdflag;//表示有一级数据
        BYTE m_recfalg;//接收到一帧读指令报文置1，收到的是确认帧置2，发送完回复帧置0
        BYTE m_fcb;
        BYTE m_sourfaaddr;
        BYTE m_QRP;
        BYTE m_errflag;
        VYKInfo  YKInfo;
        DWORD m_YxYcGrpFlag;
        BYTE m_ZongzhaoYX;
        BYTE m_ZongzhaoYC;
        BYTE  m_BeatFlag;
	 BYTE  m_LuboRe10Flag;	
        WORD time_delay_set;//主站发的延时获得的时间
                
        DWORD m_retxdnum;
        //WORD  m_ReOpenGprsNum;
        BOOL m_YKstop;
        WORD m_ucCosBkNum;//备份正在传输的遥信有几条(<=70)
        //WORD m_ucCosBk[FRM_MAX_COS_NUM];//备份正在传输的遥信序号，用于COS重传
		//BYTE m_comtradeflag;
		BYTE m_comtradeflag_YN;
		//BYTE m_com101flag_YN;//0 录波山东协议 1=录波云南协议
		//读录波及文件传输相关=====================================================
		BYTE m_ulog_directory;//log的标志位
		BYTE m_fixpt_directory;//fixpt的标志位
		BYTE m_soe_directory;//soe的标志位
		BYTE m_lubo_directory;
		BYTE m_comtradeflag;
		BYTE m_fileprocessing;
		BYTE m_ackflag;

		BYTE m_SendListNum;
		FLAG_RECORDER_FILE mRecorder_flag;
		WORD m_BkFlashhead;
		FIXPT_RECORDER mfixpt_recorded;
		SOE_RECORDER   msoe_recorded;
		LOG_RECORDER  mlog_recorded;
		BYTE m_ackRecorder;	
        BYTE m_bEncObject;
		DWORD mSendCFGNum;
		DWORD mSendDATNum;
		//DWORD mSendDATNum;
		DWORD mfile_ID;
		BYTE mfile_flag;//1 //值 1 CFG 2 DAT 3 FIX 4 SOE 5 Ulog
		BYTE m_bSymmetricKeyFlag;
		unsigned char m_wavefix_total;//定点的数量
		unsigned int  m_SendFixNum;
		BYTE m_bCerToolID[8];    //证书管理工具ID
		DWORD m_Fixpt_TOTAL_Leng;
		WORD  m_Fixpt_SECTION_Leng;
		BYTE m_current_section;
		BYTE m_houxu_flag ;
		WORD mwavelog_total;
		DWORD mLog_TOTAL_Leng; 
		BYTE m_logcurrent_section;
		BYTE m_loghouxu_flag;
		BYTE m_lubo_num;
		BYTE m_fixpt_num;
		BYTE mluboYX_flag;
		BYTE m_tra_FI;//转发指示器数据
		BYTE m_bHostCertifyFlag;  //主站认证标志位，认证以后置0x55
		BYTE m_bToolCertifyFlag;  //运维工具证标志位，认证以后置0x55
        BYTE m_bEncDataFlag;     //数据加密标志位
        WORD m_wUpdataFaultFlag;                

	public:
		CBJ101S();
		virtual BOOL Init(WORD wTaskID);
		   void Init_Rec(void);
        virtual void Run(void);
        DWORD GetEqpFlag(DWORD dwFlagNo);
        void SetEqpFlag(DWORD dwFlagNo);
        void ClearEqpFlag(DWORD dwFlagNo);
        DWORD CheckClearEqpFlag(DWORD dwFlagNo);
		void SetBaseCfg(void);
		BOOL DoReceive();
		DWORD SearchOneFrame(BYTE *Buf, WORD Len);
				
		BOOL RecFrame10(void);
		BOOL RecResetLink(void);
		BOOL RecReqLink(void);
		BOOL RecCallClass1(void);
		BOOL RecCallClass2(void);
		void  DoRecAck(void);//add by lqh 20081103
		
		BOOL RecFrame68(void);
		BOOL RecFrame69(void);
		BOOL RecYKCommand(void);
		BOOL RecYKSet(void);

		BOOL RecCallSomeGroup(void);
		BOOL RecCallAllCommand(void);
		BOOL RecCallAllStart(void);

		BOOL RecSetClock(void);
		BOOL RecReadClock(void);		
		BOOL RecDelaytime(void);
		BOOL RecDelaytrans(void);
		BOOL RecTestLink(void);
		BOOL RecResetRTU(void);		
		BOOL RecYTCommand(void);
		
		BOOL SendCallAll(void);
		BOOL SendYCGroup(WORD GroupNo, BYTE Reason, BYTE bType);
		BOOL SendYCGroup_ZS(WORD GroupNo, BYTE Reason, BYTE bType);
		BOOL SendYCGroupContinue(WORD GroupNo, BYTE Reason);
		BOOL SendAllStop(void);
		BOOL SendAllStop_ZS(void);
		BOOL SendYXGroup(WORD GroupNo, BYTE Reason, BYTE bType);
		BOOL SendYXGroup_ZS(WORD GroupNo, BYTE Reason, BYTE bType);
//			BOOL SendYXGroupContinue(WORD GroupNo, BYTE Reason);
		BOOL SendZJGroup(WORD GroupNo, BYTE Reason, BYTE bType);

		BOOL SendTsetLinkAck(void);
		BOOL SendSoe_ZS(void);
		BOOL SendSoe(void);
		BOOL SendChangeYC(void);
		BOOL SendCallAllStartAck(void);
		BOOL SendYKSetAck(void);
		BOOL ExecuteYKOrder(unsigned int YkNo,unsigned int Val);
		BOOL SendYKstop(void) ;
		void SendInitFinish(void) ;//add by lqh 20081103
		void SendAck(void);//add by lqh 20081103
                //void RecReadData(void);  //云南
                BOOL RecYSCommand(void); //云南
                BOOL RecSetClock_zs(void);//舟山，主站对时
                BOOL SendCallAllStartAck_ZS(void);//舟山，总召应答                
                BOOL RecYSPassWord(void);//舟山 收到修改密码报文
				BOOL RecYSCiPHer(void);//舟山 收到修改密文报文
				BOOL SendERRPassWord(void);//舟山 收到密码错误报文，上报错误				
				BOOL SendReqTime(void);//舟山，请求对时
                BOOL SendCallHistLoad(void); //云南
                BOOL SendHistSOE(void); //云南
                BOOL SendCallHistLoadStop(void); //云南
                
                BOOL SendCallHistLuBo(void);
                BOOL SendHistLuBo1(void);
                BOOL SendHistLuBo2(void); 
                BOOL SendCallHistLuBoStop(void); //云南
                BOOL SendCallHistLuBoStartAck(void); //云南
                BOOL SendCallHistLuBoCFGAck(void); //云南
		
		BYTE GetCtrCode(BYTE PRM, BYTE dwCode,BYTE fcv);
		BOOL SendBaseFrame(BYTE PRM, BYTE dwCode);
		BOOL SendBaseFrame_ZS(BYTE PRM, BYTE dwCode);
		BOOL SendResetLink(BYTE PRM);
		BOOL SendReqLink(void);
		BOOL SearchClass1(void);
		BOOL SearchCos(WORD wBit,BYTE flag);
		BOOL SendCos(void);
		BOOL SendLBML(void);		
		BOOL SendFrameHead(BYTE Style, BYTE Reason);
		BOOL SendFrameTail(BYTE PRM, BYTE dwCode, BYTE Num,BYTE EncType);
		BOOL SendFrameHead_ZS(BYTE Style, BYTE Reason);
		BOOL SendFrameTail_ZS(BYTE PRM, BYTE dwCode, BYTE Num,BYTE EncType);
		BOOL SendReadYCAck(WORD YCNo);
		BOOL SendReadYXAck(WORD YXNo);
		BOOL SendNoData(void);
        BOOL DoTimeOut(WORD wTimerID); /*BJ101加速变化遥信和遥测上传速度*/
		void DoCommSendIdle();
	   void SendlbRetry();
		DWORD GetAddress(void);
		virtual void initpara(BYTE flag=0);
		void getasdu(void);
		void getasdu_ZS(void);
		void write_linkaddr(int  data);
		void write_typeid(int  data);
		void write_VSQ(int  data);
		void write_COT(int  data);
		void write_conaddr(int  data);
		void write_infoadd(int  data);
		BOOL SendCallgroup(void);
		BOOL SendgroupStop(void);
		BOOL RecACK(void)	;
		void SendNOAck(void);
		void Initlink(void) ;
		void Initlink_ZS(void) ;
		BYTE QDS(BYTE data);
		BYTE SIQ(BYTE data);
		BYTE DIQ(BYTE data1,BYTE data2);
		void write_time();
		void write_time3();
		BOOL RecCallAllStop(void);
		BOOL SendDYXGroup(WORD GroupNo, BYTE Reason);
		BOOL SendtimeAck_ZS(void);
		BOOL SendtimeAck(void);
		BOOL SendtimeReq(void);
		BOOL SenddelayeAck(void);
		BOOL SendDCos(void) ;
		BOOL SendDSoe(void) ;

		WORD GetDataFromeEncChip(BYTE *pRecData);
        BOOL GetChipID(BYTE *pChipID);
        BOOL GetChipKeyVer(BYTE *Key);
        BOOL GetChipRandom(BYTE *pRandom);
        BOOL GetChipRandomSign(BYTE*pHostRandom,BYTE *pRecSign);
        BOOL VerifySign(BYTE KeyIndex,BYTE*Sign);
        WORD EncryptData(BYTE *pClearData,WORD DataLen,BYTE *pEncData);
        WORD DecryptData(BYTE *EncData,WORD EncDataLen,BYTE *ClearData);
        BOOL VerifyDataSign(BYTE asKey,BYTE* pData,WORD Datalen,BYTE* pSign,WORD SignLen);
        BOOL UpdataCer(BYTE CerID, BYTE* CerTxt, WORD CerTxtLen);
        BOOL LoadCer(BYTE* CerTxt, WORD CerTxtLen);
        BOOL ReadCerLen(BYTE *CerLen);
        BOOL ReadTestCer(BYTE *CerLen,BYTE *CerTxt);
        WORD ReadRealCer(BYTE *CerTxt);
        BOOL UpdataPrivateKey(BYTE asKID,BYTE NewKeyVer,BYTE* keySignData,WORD dataLen);
        BOOL RecovPrivateKey(BYTE asKID,BYTE* keySignData,WORD dataLen);
        BOOL VerifyToolCer(BYTE *pCerTxt,WORD CerLen);
        BOOL VerifyToolSignData(BYTE *pSignTxt,BYTE SignLen);
        BOOL ReadPubKey(BYTE *pPubKey);
        WORD EncryptToolData(BYTE *pOriginalData, WORD  OriginalDataLen,BYTE *pEncData);
        WORD DecryptToolData(BYTE *pEncData, WORD  EncDataLen,BYTE *pClearData);
        BOOL RecovToolKey(BYTE *pKeyData, BYTE KeyDataLen);
        WORD SignData(BYTE *pOriginalData,WORD OriginalDataLen,BYTE *pSrReqData);
        BOOL SendFrameEBHead(WORD FrameType,BYTE AppType);
        BOOL SendFrameEBAppData(BYTE AppDataBytes,BYTE *pAppData);
        BOOL SendFrameEBEixampleData(BYTE EixampleDataBytes,BYTE *pEixampleData);
        BOOL SendEncData(BYTE EncDataLen,BYTE *pEncData);
        BOOL SendAppData(BYTE EixampleDataBytes,BYTE *pEixampleData);
        BOOL SendFrameEBTail(void);
        BOOL SendEncFrameAck(WORD Ack,WORD FrameType,BYTE AppType);
        BOOL RecCipherTxt(BYTE *EncData,WORD EncDataLen,BYTE *pClearData);
        BOOL SendGatewayVerify(void);
        BOOL SendGatewayVerifyAck(void);
        BOOL SendHostVerify(void);
        BOOL SendHostVerifyAck(void);
        BOOL SendEncChipID(BYTE type);
        BOOL SendChipKeyVer(BYTE type);
		void RecReadParaData(BYTE Style,BYTE VSQ);
        BOOL SendUpdataKeyAck(void);
        BOOL SendRecovKeyAck(void);
        BOOL SendCerUpdataAck(void);  //远程证书更新
        BOOL SendCerDownloadAck(void);  //证书下载
        BOOL SendCer(BYTE Type);  //读取芯片证书
        BOOL SendToolVerifyAck(void);
        BOOL VerifyToolSign(void);
        BOOL SendUnitID(void);
        BOOL SendEncPubKey(void);
        BOOL SendSignCerTxt(void);
        BOOL SendLoadCerAck(void);
        BOOL SendReWriteCerAck(void);
        BOOL SendToolRecovKey(void);
        BOOL RecClearTxt(void);
        BOOL RecFrameEB(WORD ProcBytes);
        WORD SearchEncFrame(void);
        BOOL RcvEncData(void);

		
		void write_10linkaddr(int  data);
		BOOL SendresetAck(void);
		BOOL SenderrtypeAck(void) ;

		BOOL SendLinktesetFrame(BYTE PRM,BYTE dwCode);
		BOOL SendRetry(void);//重写101报文重发函数
//Rec.cpp
		void RecFIUpdata(BYTE Type);
		//void RecReadData(void);
		void RecWriteData(BYTE Style,BYTE mQos,BYTE VSQ);//BYTE cpypara,		
		BOOL Recfileprocessing(unsigned char *pRxBuf);
		void lubo_directory_confirm(WORD InfoAddr,DWORD Directory_ID,BYTE call_sign,DWORD start_minute_time,DWORD start_date_time,DWORD end_minute_time,DWORD end_date_time);
		void fixpt_directory_confirm(WORD InfoAddr,DWORD Directory_ID,BYTE call_sign,DWORD start_minute_time,DWORD start_date_time,DWORD end_minute_time,DWORD end_date_time);
		void soe_directory_confirm(WORD InfoAddr,DWORD Directory_ID);
		void ulog_directory_confirm(WORD InfoAddr,DWORD Directory_ID);
		void Send_ReadFile_Confirm(WORD File_ID,BYTE File_TYPE,WORD InfoAddr,unsigned char *pRxBuf,unsigned char length);
		BOOL FileRemoteupdata(unsigned char *pRxBuf);
		void Send_ReadFile_Data(WORD File_ID,BYTE File_TYPE,WORD InfoAddr,unsigned int Packet_Num);
		void ReadFileDataCfg(RECORDER_CFG *pgRecorder_cfg);
		void ReadFileDataDat(WORD FileName,RECORDER_CFG *pgRecorder_cfg);
		void ReadFileDataFixpt(WORD FileName,BYTE section_current,BYTE flag);
		void ReadFileDataSoe(WORD FileName);
		void ReadFileDataSoe_Flash(WORD FileName);
		void ReadFileDataUlog(WORD FileName,BYTE section_current,BYTE flag);
		unsigned char Get_time_read(WORD wave_total,DWORD start_minute,DWORD start_date,DWORD end_minute,DWORD end_date);


//			BOOL RecReadFile();
//			BOOL RecWriteFile();
//			void SendWrPaSuc();
//			void SendReadPa(WORD FileName,BYTE SecName);
//			
//			void SendWrPaAck(WORD wFile,BYTE bySec);
//			void SendSavedSoe(BYTE bySec,WORD wStartNo,WORD wSOENum);
//			void SendLoadData(BYTE bySec,WORD wStartNo,WORD wLoadNum,BYTE byQds=0);
//			void SendOneLoadData(WORD wCurLoadNum,WORD wFlag=0);
//			void SendSoeSavedStu();
//			void SendTrigAck();
		

		
		
};

#endif

