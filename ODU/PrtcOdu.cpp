/*------------------------------------------------------------------------
 Module:            PrtcOdu.cpp
 Author:            linxueqin
 Project:           ��Լƽ̨
 State:             V1.0
 Creation Date:     2015-3-16
 Description:       ��ODUͨ��--primary
------------------------------------------------------------------------*/




#ifndef _PRTC_ODU

  #define _PRTC_ODU
  #include "PrtcOdu.h"
#include "..\Main.h"
//���캯��
CPrtcOdu::CPrtcOdu() : CProtocol()
{
  m_OduAddr = 0;
  m_open_gprs = 0;
}



#endif



