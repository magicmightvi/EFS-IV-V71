/*------------------------------------------------------------------------
 Module:            gb104s.h
 Author:            linxueqin
 Project:           规约平台
 State:             V1.0
 Creation Date:     2012-10-8
 Description:       GB104规约--slave函数声明
------------------------------------------------------------------------*/

#ifndef GB104S_H
#define GB104S_H

#include "..\PUBLIC\Pcolsec.h"
#include "Gb104pub.h"


#define  TIMER_MAX   10


//extern void *thread_StartGb104(void* arg);


class CGB104S : public CPSecondary
{
    public:
        Vgb104para                m_guiyuepara;
        DWORD                      m_Cot;
        DWORD                      m_ConAddr;
        DWORD                      m_InfoAdd;
        char*                     m_SvrIP;
        UINT                      m_SvrPort;
        //UINT m_wSocketStatus;
        WORD m_wSendNum;    //Send Counter  
        WORD m_wRecvNum;    //Receive Counter
        WORD m_wAckNum;     //Receive Ack Counter
        WORD m_wAckRecvNum;    //Have Ack Receive Counter
        VIec104Timer m_vTimer[TIMER_MAX];   //iec104 timer for T0 T1 T2 T3
        BOOL m_bDTEnable;       //Data Transfer Enable Flag
        BOOL m_bContinueSend;   //continue send I frame flag about K


        BOOL m_bSendInitEnd;
        BOOL SendInitEnd();


        WORD m_wAckTimeOut[PARA_K];
        int  IsAckTimeout(void);
        WORD m_wAckRetry[PARA_K];       /*标记重链接后需补送的未确认帧*/
        int  SendAckRetry(BYTE id, BYTE reason);    
        WORD m_wYkTimeOut;
        WORD m_wYkNo;
        void SendBack(BYTE Reason); /**<镜像回复*/
        void SetAckRetry();

        DWORD m_dwSendAllDataEqpNo;
        WORD m_wRecAddress;
        WORD m_wChangeYcEqpNo;
        WORD m_wAllDdEqpNo;
        
        VIec104Frame *m_pReceive;
        VIec104Frame *m_pSend;
        BYTE *m_pASDUInfo;
        VASDU *m_pASDU;
        VBackFrame m_vBackFrame[PARA_K];//[PARA_K + 2]
        
        DWORD m_dwYcSendFlag;
        DWORD m_dwYcSendFlagH;
        DWORD m_dwYxSendFlag;
        DWORD m_dwYxSendFlagH;
        
        WORD m_wSendYcNum;
        WORD m_wSendYxNum;
        
        WORD m_wSendChangedYcNum;
        WORD m_wSendCosNum;
        WORD m_wSendSoeNum;


        DWORD m_dwYcGroupFlag;
        DWORD m_dwYxGroupFlag;
        DWORD m_dwDdGroupFlag;
        BYTE m_byYcYxQrp;
        BYTE m_byDdQcc;

        WORD m_PARA_K;
        WORD m_PARA_W;
        BYTE m_YKflag;
        BYTE m_YTflag;
        WORD ASDUID_LEN;
     
    public:
        CGB104S();

        void GetAsduId(void);
        virtual BOOL Init(WORD wTaskID);
        
        virtual void Run(void);
        void StartTimer(BYTE TimerNo);
        void StopTimer(BYTE TimerNo);   
        virtual BOOL DoTimeOut(WORD wTimerID);
        void SendYkReturn(void);
        virtual void DoCommSendIdle(void);
                
        virtual void CloseTcp(BYTE byReason = 0);
        int RcvData(int* iSocket);
        virtual BOOL DoReceive();
        virtual DWORD SearchOneFrame(BYTE *Buf, WORD Len);
        void DoIFrame(void);
        void DoSFrame(void);
        void DoUFrame(void);
        void DoYk(void);
        void DoCallYcYxData(void);
//        void DoCallDd(void);
        void DoReadData(void);
        void DoSetTime(void);

     //   void DoReadMultiData(void);
        void DoTestLink(void);
        void DoReset(void);
        
        void SendSFrame(void);
        void SendUFrame(BYTE byControl);
        void SendFrameHead(BYTE byFrameType);
        BOOL SendFrameTail(void);
        void CycleSendData(void);
        BOOL SendChangeYx(void);
        void SendClassTwoData(void);
        void SendSoe(WORD ReadNum,DWORD RamBase);
        void SendChangeYC9(void);
        void SendChangeYC21(void);
        void SendChangeYC13(void);
        void SendChangeYC11(void);
        void SendYkCancelAck(void);
        void SendAllYcYx(BYTE byFlag);
        void SendSomeYcYx(void);
        void SendStopYcYx(void);
        void SendStopSomeYcYx(void);
        BOOL SendGroupYc11(WORD YcGroupNo,BYTE Reason);
        BOOL SendGroupYcContinue11(WORD YcGroupNo,BYTE Reason);
        BOOL SendGroupYc(WORD YcGroupNo,BYTE Reason);
        BOOL SendGroupYc9(WORD YcGroupNo,BYTE Reason);
        BOOL SendGroupYcContinue9(WORD YcGroupNo,BYTE Reason);
        BOOL SendGroupYc21(WORD YcGroupNo,BYTE Reason);
        BOOL SendGroupYcContinue21(WORD YcGroupNo,BYTE Reason);
        BOOL SendGroupYc13(WORD YcGroupNo,BYTE Reason);
        BOOL SendGroupYcContinue13(WORD YcGroupNo,BYTE Reason);
        BOOL SendGroupYx(WORD YxGroupNo,BYTE Reason);
        BOOL SendGroupYxContinue(WORD YxGroupNo,BYTE Reason);
        void SendCallYcYxAck(void);
        void SendSomeYcYxAck(BYTE byQrp);
        void SendReadYx(WORD wYxNo);
        void SendReadYc(WORD wYcNo);
        void SendNoData(void);
        void SendReadPara(WORD wParaNo,WORD IecAddrL);
        void SendReadMultiPara(WORD wParaNo,WORD IecAddrL,WORD wParaNum);
        
                
        void DelAckFrame(WORD SendNum);
        void SaveSendFrame(WORD SendNum, WORD FrameLen, BYTE *pFrame);
        BOOL CanSendIFrame(void);
    
        BOOL DoYtCommand(void);
        //BOOL DoMultiYtCommand(void);
        void SendYtReturn(WORD wAddL);
        //void SendMultiYtReturn(WORD wYtNum);
        void write_COT(int  data);
        void write_conaddr(int  data);
        void write_infoadd(int  data);
};

#endif

