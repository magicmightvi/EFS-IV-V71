
#ifndef PrtcOdu_H
#define PrtcOdu_H

#include "..\PUBLIC\Pcolsec.h"

typedef union 
{

    VFrame67 Frame67;
    //VFrameFF Frameff;
    VFrame69 Frame69;//自定义协议 报文结构与101相同，只是报文头为0x69 for 参数读写
} VIecOduFrame; 

class CPrtcOdu : public CProtocol
{
    public:
        BYTE m_OduAddr;//当前ODU的地址(终端内部使用，与实际ODU地址无关)，认定与pOdu使用的串口相连的ODU为1-6路 ODU地址=1；与pOdu1使用的串口相连的ODU为7-12路 ODU地址=2；
		BYTE m_ReadPaTp;//当前读的参数类型 加这个变量的原因是读成功时返回给后台的报文不同
        VIecOduFrame *pReceiveFrame;    //接收帧头指针
        VIecOduFrame *pSendFrame;       //发送帧头指针
        
        BYTE m_ODUCall2Flag;//ODU召二级数据
        BYTE  m_ODUCallFlag;//ODU总召
        BYTE m_ODUCall2Bit;//两个ODU发数据采用同一个定时器，用此变量来区分该给哪个ODU发报文
        BYTE m_ODUCallBit;
		BYTE  m_oduSendBuf[25];//保存ODU需要重传的报文
		WORD  m_oduSendLen;//ODU需要重传的报文长度
		BYTE m_WrPaWaitflag;//ON表示启动重发计时 TRUE表示重发计时到 OFF表示重发结束(重发次数到或收到ODU回复)
		WORD m_WrPaWaitCt;//写ODU参数时重发计时
		WORD m_ReTxNum;//重传次数
		BYTE m_open_gprs;//发给ODU打开GPRS的标志 0x55:关GPRS 0X66:开GPRS
		BYTE m_RcvCloseZdFlag;//ON:收到ODU发送的关闭终端指令
    public:
        CPrtcOdu();
};








#endif



