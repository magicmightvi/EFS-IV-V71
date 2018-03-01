/*------------------------------------------------------------------------
 Module:			pcolsec.h
 Author:			linxueqin
 Project:			��Լƽ̨
 State: 			V1.0
 Creation Date: 	2012-10-8
 Description:		��վ����Լ���������
------------------------------------------------------------------------*/

#ifndef _PSECONDARY_H
#define _PSECONDARY_H

#include "Protocol.h"

#define ADDR_YX_LO		0x0001
#define ADDR_YX_HI		0x1000

#define ADDR_YC_LO		0x4001
#define ADDR_YC_HI		0x5000

#define ADDR_YK_LO		0x6001//ң�غ͵�ѹ
#define ADDR_YK_HI		0x6200


struct VPtEqpInfo
{
    WORD    wLinkAddress;//��·��ַ
    WORD    wAddress;  //װ�ñ����ַ
    WORD    wSYXNum;
    WORD    wDYXNum;
    WORD    wYCNum;
    WORD    wYTNum;
    WORD    wYKNum;
    WORD    wZJNum;	
   VYKInfo YKInfo;
   // VYTInfo YTInfo;
    WORD    OldYc[IEC_YC_NUM];

    WORD wYxType;  //ң������
    WORD wSoeType; //soe����

};
struct VProtocolBaseCfg
{

    WORD wMaxRetryCount;//��·�㱨���ط�����
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
		BOOL  GetRetryFlag(void); //��ȡ�ط���־
		WORD GetRetryCount(void); //��ȡ�ط�����
		void ResetRetryCount(void);   //�ط�������0
		void IncRetryCount(void);	  //�ط�������1
		void DisableRetry(void);   //��ֹ�ط�
		BOOL SendRetry(void); //�ط�����
                /* Restart */
		void sysRestart(DWORD dwRestartType);
                BOOL SearchCos(WORD wBit,BYTE flag);
				BOOL Init(WORD wMinEqpNum);
public:
        VProtocolBaseCfg *m_pBaseCfg;
		VPtEqpInfo m_VPtEqpInfo;
        WORD m_wRetryCount;  //�ط�����ͳ��
        WORD m_wEqpNo;
        WORD m_wEqpNum;
        VPtEqpInfo *m_pEqpInfo;
        BYTE bySendAccidentFlag;
        BYTE byChangeYcInitFlag;
        BYTE byCosInitFlag;
        int m_SoePos;//����soe�������д�ŵ�λ��
        VProtocolBaseCfg m_VProtocolBaseCfg;
//        VPtEqpInfo *m_pEqpInfo;        
        UINT m_SCosHeadPtr;
        UINT m_DCosHeadPtr;
        UINT m_SSoeHeadPtr;
        UINT m_DSoeHeadPtr;
        UINT m_SSoeHeadPtrBk;
        UINT m_DSoeHeadPtrBk;
	DWORD m_SendSoeAdr;//����SOE����ʼ��ַ
        BYTE m_resendflag;//�����ش���ʱ������ʶ���ط�����
        BYTE m_SendYcFlag;//��ʶ�ж�ʱң����Ҫ���ͣ���δ����
        BYTE m_SendChgYcFlag;//��ʶ��ͻ��ң����Ҫ���ͣ���δ����
        WORD m_reSendDelay; //�����ش���ʱ
        WORD m_gRmtChgFlag[RMT_WORD_NUM/2];//ң�ű仯��ʶ��ÿ����ʶռ1��bitλ
	BYTE m_WaitConfTpId;//�ȴ���վȷ�ϵ�����ID	
	BYTE m_luok;//¼����� ��־��
	BYTE m_luok_filename;//¼����� ��־��
	BYTE m_TxNum_lubo;
	BYTE m_PaWaitflag_lubo;
	BYTE m_PaWaitCt_lubo;
	BYTE m_gprsSendBuf[1];
	BYTE m_gprsSendLen;
};

#endif

