/*------------------------------------------------------------------------
 Module:			pcolsec.h
 Author:			linxueqin
 Project:			规约平台
 State: 			V1.0
 Creation Date: 	2012-10-8
 Description:		从站方规约基类的声明
------------------------------------------------------------------------*/

#ifndef _PSECONDARY_H
#define _PSECONDARY_H

#include "Protocol.h"

#define ADDR_YX_LO		0x0001
#define ADDR_YX_HI		0x1000

#define ADDR_YC_LO		0x4001
#define ADDR_YC_HI		0x5000

#define ADDR_YK_LO		0x6001//遥控和调压
#define ADDR_YK_HI		0x6200


struct VPtEqpInfo
{
    WORD    wLinkAddress;//链路地址
    WORD    wAddress;  //装置本身地址
    WORD    wSYXNum;
    WORD    wDYXNum;
    WORD    wYCNum;
    WORD    wYTNum;
    WORD    wYKNum;
    WORD    wZJNum;	
   VYKInfo YKInfo;
   // VYTInfo YTInfo;
    WORD    OldYc[IEC_YC_NUM];

    WORD wYxType;  //遥信类型
    WORD wSoeType; //soe类型

};
struct VProtocolBaseCfg
{

    WORD wMaxRetryCount;//链路层报文重发次数
    WORD wBroadcastAddr;
};



class CPSecondary : public CProtocol
{
	public:
		CPSecondary();
		virtual ~CPSecondary(){};
		BYTE ChkSum(BYTE *buf, WORD len);
                BOOL InitEqpInfo(WORD wMinEqpNum);
		BOOL SetEqpInfo(void);

		WORD GetOwnAddr(void);
		BOOL SwitchToAddress(WORD wAddress);
		virtual BOOL DoTimeOut(WORD wTimerID){return FALSE;};
		virtual void CloseTcp(BYTE byReason = 0){return;};
		WORD GetCosNum(WORD HeadPtr,WORD TailPtr,BYTE bSoeBufLen = SOE_EEP_NUM);
		WORD ReadSoeFromFlash(WORD *pHeadPtr,WORD TailPtr,BYTE bSoeMaxNum,BYTE *pSoeBuf);
                virtual void SetBaseCfg(void);
                VPtEqpInfo *pGetEqpInfo(WORD wEqpNo);
                
//			WORD ReadRangeSYXBit(WORD wBeginNo,WORD wNum,BYTE *buf);

		WORD ReadRangeYC(WORD wBeginNo,WORD wNum, short *buf);
		WORD ReadRangePara(BYTE byFlag,WORD wBeginNo,WORD wNum, short *buf);

		WORD SearchSCOS(WORD wNum,struct VDBCOS *buf);
		WORD SearchChangeYC(WORD wNum, VDBYCF *pBuf);
		void CheckChangeYC();
		
		WORD IsOrderFeedBack(DWORD RamBase,WORD wOffset,WORD wYTorYK);
                
                int WriteToComm(DWORD Flag);	
                WORD  GetBroadcastAddr(void);
                WORD  GetMaxRetryCount(void);
		BOOL  GetRetryFlag(void); //获取重发标志
		WORD GetRetryCount(void); //获取重发次数
		void ResetRetryCount(void);   //重发次数清0
		void IncRetryCount(void);	  //重发次数加1
		void DisableRetry(void);   //禁止重发
		BOOL SendRetry(void); //重发报文
                /* Restart */
		void sysRestart(DWORD dwRestartType);
                BOOL SearchCos(WORD wBit,BYTE flag);
				BOOL Init(WORD wMinEqpNum);
public:
        VProtocolBaseCfg *m_pBaseCfg;
		VPtEqpInfo m_VPtEqpInfo;
        WORD m_wRetryCount;  //重发次数统计
        WORD m_wEqpNo;
        WORD m_wEqpNum;
        VPtEqpInfo *m_pEqpInfo;
        BYTE bySendAccidentFlag;
        BYTE byChangeYcInitFlag;
        BYTE byCosInitFlag;
        int m_SoePos;//最新soe在链表中存放的位置
        VProtocolBaseCfg m_VProtocolBaseCfg;
//        VPtEqpInfo *m_pEqpInfo;        
        UINT m_SCosHeadPtr;
        UINT m_DCosHeadPtr;
        UINT m_SSoeHeadPtr;
        UINT m_DSoeHeadPtr;
        UINT m_SSoeHeadPtrBk;
        UINT m_DSoeHeadPtrBk;
	DWORD m_SendSoeAdr;//发送SOE的起始地址
        BYTE m_resendflag;//故障重传计时到，标识可重发故障
        BYTE m_SendYcFlag;//标识有定时遥测需要发送，还未发送
        BYTE m_SendChgYcFlag;//标识有突变遥测需要发送，还未发送
        WORD m_reSendDelay; //故障重传计时
        WORD m_gRmtChgFlag[RMT_WORD_NUM/2];//遥信变化标识，每个标识占1个bit位
	BYTE m_WaitConfTpId;//等待主站确认的类型ID	
	BYTE m_luok;//录波完成 标志；
	BYTE m_luok_filename;//录波完成 标志；
	BYTE m_TxNum_lubo;
	BYTE m_PaWaitflag_lubo;
	BYTE m_PaWaitCt_lubo;
	BYTE m_gprsSendBuf[1];
	BYTE m_gprsSendLen;
};

#endif

