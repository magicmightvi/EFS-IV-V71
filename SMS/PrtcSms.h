
#ifndef PrtcSMS_H
#define PrtcSMS_H
#include "..\PUBLIC\Pcolsec.h"

 
class CPrtcSms : public CPSecondary
{
    public:
        WORD m_SmsSoeBit;
        WORD m_DSmsSoeBit;
        BYTE m_infoaddlen;
        BYTE m_linkaddrlen;
		
		WORD m_SendStatusBit; //״̬�㱨��־λ
		BYTE m_SendStatusFlag;  //״̬�㱨��־λ
		WORD m_wSendYcNum;		//�Ѿ����͵�ң����������ƶ�֡������
		WORD m_wSendYxNum;		//�Ѿ����͵�ң�Ÿ��������ƶ�֡������
		WORD m_wSendDYxNum;		//�Ѿ����͵�ң�Ÿ��������ƶ�֡������
		WORD m_wSendZJNum;//�Ѿ����͵��Լ���������ƶ�֡������
    public:
        CPrtcSms();
	 BOOL RecFrameAA(VFrameAA * pReceiveFrame);	
        BOOL SendSmsFrame(void);
        BOOL SendSmsToXinhao(void);
        BOOL SendRCmdToIHD(unsigned char Cmd,unsigned int Val,CProtocol *pDaSrc);
        BOOL SendWCmdToIHD(unsigned int Val,unsigned char Cmd,BYTE *pDa,CProtocol *pDaSrc);
        BOOL RecIHDPara(VFrameAA *pReceiveFrame);
        BOOL RecIHDWConf(VFrameAA *pReceiveFrame);
        BOOL SendSmsFrameHead(BYTE Command);      
        BOOL TAGContent(BYTE *ptr,BYTE num,BYTE type);		
	 BOOL TLVSContent(BYTE *ptr,unsigned int  num,unsigned int  type);			
};

#endif


