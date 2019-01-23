
#ifndef PrtcRF_H
#define PrtcRF_H

#include "..\PUBLIC\Pcolsec.h"



typedef union 
{
    VFrame55 Frame55;
    VFrameFF Frameff;
	VFrame66 Frame66;
	VFrame69 Frame69;//自定义协议 报文结构与101相同，只是报文头为0x69 for 参数读写
} VIecRfFrame; 
class CPrtcRF : public CProtocol//CPSecondary
{
	public:
		VIecRfFrame *pReceiveFrame; 	//接收帧头指针
		VIecRfFrame *pSendFrame;  		//发送帧头指针
	public:
		CPrtcRF();
};








#endif



