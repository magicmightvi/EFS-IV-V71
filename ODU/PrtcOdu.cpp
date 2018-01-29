/*------------------------------------------------------------------------
 Module:            PrtcOdu.cpp
 Author:            linxueqin
 Project:           规约平台
 State:             V1.0
 Creation Date:     2015-3-16
 Description:       与ODU通信--primary
------------------------------------------------------------------------*/




#ifndef _PRTC_ODU

  #define _PRTC_ODU
  #include "PrtcOdu.h"
#include "..\Main.h"
//构造函数
CPrtcOdu::CPrtcOdu() : CProtocol()
{
  m_OduAddr = 0;
  m_open_gprs = 0;
}



#endif



