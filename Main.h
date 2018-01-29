
#ifndef _MAIN_H

  #define _MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif
//#include "GB104s\Gb104s.h"
#include "GB101s\Bj101s.h"
#include "ODU\PrtcOdu.h"
#include "RF\PrtcRF.h"
#include "SMS\PrtcSms.h"
  
       
#ifdef __cplusplus
}
#endif

#ifdef _MAIN
	 CPSecondary *pDbg;
	 CPSecondary *pGprs;

     CProtocol *pRf;
     CProtocol *pOdu;
     CProtocol *pOdu1;
#else

    extern CPSecondary *pDbg;
    extern CPSecondary *pGprs;
    extern CProtocol *pRf;
    extern CProtocol *pOdu;
    extern CProtocol *pOdu1;
#endif
    

#endif

