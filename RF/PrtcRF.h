
#ifndef PrtcRF_H
#define PrtcRF_H

#include "..\PUBLIC\Pcolsec.h"



typedef union 
{
    VFrame55 Frame55;
    VFrameFF Frameff;
	VFrame66 Frame66;
	VFrame69 Frame69;//�Զ���Э�� ���Ľṹ��101��ͬ��ֻ�Ǳ���ͷΪ0x69 for ������д
} VIecRfFrame; 
class CPrtcRF : public CProtocol//CPSecondary
{
	public:
		VIecRfFrame *pReceiveFrame; 	//����֡ͷָ��
		VIecRfFrame *pSendFrame;  		//����֡ͷָ��
	public:
		CPrtcRF();
};








#endif



