
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
		
		WORD m_SendStatusBit; //状态汇报标志位
		BYTE m_SendStatusFlag;  //状态汇报标志位
		WORD m_wSendYcNum;		//已经发送的遥测个数，控制多帧传送用
		WORD m_wSendYxNum;		//已经发送的遥信个数，控制多帧传送用
		WORD m_wSendDYxNum;		//已经发送的遥信个数，控制多帧传送用
		WORD m_wSendZJNum;//已经发送的自检个数，控制多帧传送用
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


