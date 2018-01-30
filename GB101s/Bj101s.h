
#ifndef BJ101S_H
#define BJ101S_H

//#include "..\PUBLIC\Pcolsec.h"
#include "..\SMS\PrtcSms.h"
#include "bj101pub.h"

//����
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
#define CALL_LINK		14	 //�ٻ���·
#define CALL_DATA 		15	 //�ٻ�����


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


//SendIdle ��ϢʱӦ���͵�֡���
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

#define MAX_FRM_RESEND   4//0x7FFF //�����ط����� Ŀǰ����Ϊ�Զ˲���Ӧ�������ط�



class CBJ101S : public CPrtcSms//CPSecondary
{
	public:
		VIec101Frame *pReceiveFrame; 	//����֡ͷָ��
		VIec101Frame *pSendFrame;  		//����֡ͷָ��
		int m_YkWaitCount;//ң��ִ�������rf��ODU��ȴ���ʱ�� ��λs
		BYTE m_bReceiveControl;
		BYTE m_byLinkAdrShift;
		BYTE m_byTypeIDShift;
		BYTE m_byReasonShift;
		BYTE m_byCommAdrShift;
		BYTE m_byInfoShift;
		BYTE m_ztype;
		BYTE m_byRTUStatus;
                
                BYTE m_HistLoadFlag;//��ʶ�ٻ���ʷң��Ĳ�����Flag��BIT0ң�� BIT1ң�ⷢ�� BIT2ң�ⷢ�ͽ��� BIT3�ٻ�����֡
              //  BYTE m_HistLoadDay;//�ٻ���ʷң�������
             //   WORD m_PtrSendLoad;//���ڿ��Ʒ�����ʷң��
             //   WORD m_HistLoadNum;//�ٻ���ʷң���������������ֵ�봢����E2�е������й�
                                   //���������ŷ��Ͳ��ϼ���
                WORD m_HistSoeSEG; //��ʶ��ȡSOEʱ�ĶΣ���ȡʱ�ӵ�0�ο�ʼ��ÿ��512���ֽ�
                BYTE m_PtrSendSOE; //��ȡ����SOEʱָ��λ�ã�2��λ�ã�0����21
                                   //һ��SOE12���ֽڣ�һ�δ�42��SOE
                BYTE m_HistLuBoFlag;
		
		WORD m_SYxBit;//��ʶ�е���ң�ű�λ
		WORD m_DYxBit;//��ʶ��˫��ң�ű�λ
		WORD m_SSoeBit;//��ʶ�е���SOE
		WORD m_DSoeBit;//��ʶ��˫��SOE
		//WORD m_NewFltBit;//��ʶ���¹��ϲ�����Ϊ��ǰ���������ش�ʱ����������
		WORD m_BeatBit;//��ʱ������
		WORD m_SendYcBit;//��ʱ����ң��
		
		Vgb101para m_guiyuepara;
        VDWASDU m_dwasdu;
        BYTE m_callallflag;
        BYTE m_groupflag;
        BYTE m_YKflag;
        BYTE m_YKSelectAck;
        BYTE m_timeflag;
	 BYTE m_timeREQflag;
		BYTE m_delayflag;
        BYTE m_testflag;
        BYTE m_resetflag;
 		BOOL m_DIR;  /*get DIR */
        BYTE m_PRM;
        BYTE m_QPM;
        BYTE m_acdflag;//��ʾ��һ������
        BYTE m_recfalg;//���յ�һ֡��ָ�����1���յ�����ȷ��֡��2��������ظ�֡��0
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
        WORD time_delay_set;//��վ������ʱ��õ�ʱ��
                
        DWORD m_retxdnum;
        //WORD  m_ReOpenGprsNum;
        BOOL m_YKstop;
        WORD m_ucCosBkNum;//�������ڴ����ң���м���(<=70)
        //WORD m_ucCosBk[FRM_MAX_COS_NUM];//�������ڴ����ң����ţ�����COS�ش�
		BYTE m_comtradeflag;
	public:
		CBJ101S();
		virtual BOOL Init(WORD wTaskID);
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
//			BOOL RecFrame69(void);
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
		BOOL SendYCGroupContinue(WORD GroupNo, BYTE Reason);
		BOOL SendAllStop(void);
		BOOL SendYXGroup(WORD GroupNo, BYTE Reason, BYTE bType);
//			BOOL SendYXGroupContinue(WORD GroupNo, BYTE Reason);
		BOOL SendZJGroup(WORD GroupNo, BYTE Reason, BYTE bType);

		BOOL SendTsetLinkAck(void);

		BOOL SendSoe(void);
		BOOL SendChangeYC(void);
		BOOL SendCallAllStartAck(void);
		BOOL SendYKSetAck(void);
		BOOL ExecuteYKOrder(unsigned int YkNo,unsigned int Val);
		BOOL SendYKstop(void) ;
		void SendInitFinish(void) ;//add by lqh 20081103
		void SendAck(void);//add by lqh 20081103
                void RecReadData(void);  //����
                BOOL RecYSCommand(void); //����
                BOOL SendCallHistLoad(void); //����
                BOOL SendHistSOE(void); //����
                BOOL SendCallHistLoadStop(void); //����
                
                BOOL SendCallHistLuBo(void);
                BOOL SendHistLuBo1(void);
                BOOL SendHistLuBo2(void); 
                BOOL SendCallHistLuBoStop(void); //����
                BOOL SendCallHistLuBoStartAck(void); //����
                BOOL SendCallHistLuBoCFGAck(void); //����
		
		BYTE GetCtrCode(BYTE PRM, BYTE dwCode,BYTE fcv);
		BOOL SendBaseFrame(BYTE PRM, BYTE dwCode);
		BOOL SendResetLink(BYTE PRM);
		BOOL SendReqLink(void);
		BOOL SearchClass1(void);
		BOOL SearchCos(WORD wBit,BYTE flag);
		BOOL SendCos(void);
		BOOL SendLBML(void);		
		BOOL SendFrameHead(BYTE Style, BYTE Reason);
		BOOL SendFrameTail(BYTE PRM, BYTE dwCode, BYTE Num);
		BOOL SendReadYCAck(WORD YCNo);
		BOOL SendReadYXAck(WORD YXNo);
		BOOL SendNoData(void);
        BOOL DoTimeOut(WORD wTimerID); /*BJ101���ٱ仯ң�ź�ң���ϴ��ٶ�*/
		void DoCommSendIdle();
	
		DWORD GetAddress(void);
		virtual void initpara(BYTE flag=0);
		void getasdu(void);
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
		BYTE QDS(BYTE data);
		BYTE SIQ(BYTE data);
		BYTE DIQ(BYTE data1,BYTE data2);
		void write_time();
		void write_time3();
		BOOL RecCallAllStop(void);
		BOOL SendDYXGroup(WORD GroupNo, BYTE Reason);
		BOOL SendtimeAck(void);
		BOOL SendtimeReq(void);
		BOOL SenddelayeAck(void);
		BOOL SendDCos(void) ;
		BOOL SendDSoe(void) ;
		
		void write_10linkaddr(int  data);
		BOOL SendresetAck(void);
		BOOL SenderrtypeAck(void) ;

		BOOL SendLinktesetFrame(BYTE PRM,BYTE dwCode);
		BOOL SendRetry(void);//��д101�����ط�����
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

