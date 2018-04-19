
#ifndef _FUNCDEF_H

  #define _FUNCDEF_H
#ifdef __cplusplus
extern "C"
{
#endif
//======================================================================
//                main.c--主函数
//======================================================================
#ifdef _MAIN
        void app();

        void WatchDog(void);
        void InitSys(void); //完成系统初始化的工作，包括系统时钟设置，外设寄存器初始化，
      //  void HandleTimerEvent(void);
    	bool TimeOut(int flag);
		void RefreshPara(void);
		//bool CheckInLPM3(void);
		void InitCommObj(void);
                void InitGPRS(void);		
              //  void CloseGPRS(BYTE bDa = 0,BYTE Close_Type = 0);//第一个参数区别是GPRS打开5分钟后关还是进入准实时模式关
                                                                   //第二个参数说明时遥控关还是普通的关用以在关闭GPRS模块时检测是否是实时模式
				//void OpenGPRS(BYTE bDa = 0);
				//BOOL CheckHaveDataToSd();
        void RmInfoChk(void);
      //  void RstRMTInfo(unsigned int SoeType);
#else
        
        extern void WatchDog(void);
    	extern bool TimeOut(int flag);
		extern void RefreshPara(void);
		extern void InitCommObj(void);
                extern void InitGPRS(void);		
        extern void CloseGPRS(BYTE bDa = 0,BYTE Close_Type = 0);
		extern void OpenGPRS(BYTE bDa = 0);
		//extern BOOL CheckHaveDataToSd();
                extern void app();
                extern void RmInfoChk(void);
       // extern void RstRMTInfo(unsigned int SoeType);
       

#endif

//======================================================================
//                DataProcess.c--数据处理模块
//======================================================================
#ifdef _DATA_PROCESS
        void InitDataProc(void);
        void CalcuRmtMeas(void);
	 void YCthCross(void);	
        void SaveSoeData(void);
        void SaveSoeDataRepeat(void);
        void RECParaPro(void);
	void DelALLSOE(void);
	void DelALLLOG(void);
	
        void SaveRecData(void);
	 void SaveActRecData(void);	
	 void SaveActRecDataCFG(void);		 
        void CreatNewSoe(unsigned int SOEtype,unsigned long SOEvalue,unsigned char Flag = 0);
    	int ReadSoe(unsigned char *pBuf,int iSegNo,int iStartNo,int iSoeNum);
        int ReadSoeHistory(unsigned char *pBuf,int iSegNo,int iStartNo,int iSoeNum);
	void SaveLoad(void);
	void SaveLOG(char   logtype,char logvalue);
	void SaveFlashLOG(void);
        void RstRMTInfo(unsigned int SoeType);
        void RstRMTMeas();
        void SetYxTrans(void);
        void SetYcTrans(void);
        unsigned int RealYxToTsYx(unsigned int RYxNo);
        void read_ptr(void);

		unsigned int GetRIStatus( unsigned int RIIndex , BYTE RmtInfoFlag = 0);
		void SetRIStatus( unsigned int RIIndex , unsigned int Status , BYTE RmtInfoFlag = 0);

		void CopySoeToFlash(void);
        void CalcuProtMeas(void); 
	unsigned int table_sqrt(unsigned long x);	
        void RecData(void);
        void RecActData(void);		
#else
        extern void InitDataProc(void);
        extern void CalcuRmtMeas(void);
	 extern void YCthCross(void);	
        extern void SaveSoeData(void);
        extern void SaveSoeDataRepeat(void);
        extern void RECParaPro(void);
	 extern void DelALLSOE(void);
	 extern void DelALLLOG(void);

        extern void SaveRecData(void);
	 extern void SaveActRecData(void);	
	 extern void SaveActRecDataCFG(void);		 
        extern void CreatNewSoe(unsigned int SOEtype, unsigned long SOEvalue,unsigned char Flag = 0);
        extern int ReadSoe(unsigned char *pBuf,int iSegNo,int iStartNo,int iSoeNum);
        extern int ReadSoeHistory(unsigned char *pBuf,int iSegNo,int iStartNo,int iSoeNum);
        extern void SaveLoad(void);
		extern void SaveLOG(char   logtype,char logvalue);
		extern void SaveFlashLOG(void);
        extern void RstRMTInfo(unsigned int SoeType);
        extern void RstRMTMeas();
        extern void SetYxTrans(void);
        extern void SetYcTrans(void);
        extern unsigned int RealYxToTsYx(unsigned int RYxNo);
        extern void read_ptr(void);

		extern unsigned int GetRIStatus( unsigned int RIIndex , BYTE RmtInfoFlag = 0);
		extern void SetRIStatus( unsigned int RIIndex , unsigned int Status, BYTE RmtInfoFlag = 0 );

		extern void CopySoeToFlash(void);
        extern void CalcuProtMeas(void); 
	extern unsigned int table_sqrt(unsigned long x);
        extern void RecData(void);
        extern void RecActData(void);			
#endif
//======================================================================
//                  ProtectLogic.c--逻辑判断模块                
//======================================================================
#ifdef _PROTECT_LOGIC
        void InitProt(void);

        void ProtStart(void);
        void ProtLogic(void);


        void Sign_Repeat(unsigned char repeat_flag,unsigned char rev_flag);
                
#else 
        extern void InitProt(void);

        extern void ProtStart(void);
        extern void ProtLogic(void);


        extern void Sign_Repeat(unsigned char repeat_flag,unsigned char rev_flag);
        
#endif
        
//======================================================================
//                  ParaCfg.c--参数配置模块                
//======================================================================
#ifdef _PARA_CFG
        void InitcfgPara(void);
        void SaveCfgPara(void); 
        void CheckCfgPara(void);
		void CheckCfgERR(void);
    	void InitTsYxInfo(void);
        void InitTsYcInfo(void);
        void RstRunPara(void);
    	void RstIEC101Para(void );
        void RstIEC101YxAddr(void);
        void RstIEC101YcAddr(void);
        void RstIEC101YkAddr(void);
        void RstIEC101YcSiqu(void);
		void RstSmsPhoneInfo(void);
		void RstCnName(void);
		void RstLBName(void);                
        void InitPara(void);
        void CheckProtCnt(void);

        void CheckAdjPara(void);
        void CheckRunInfo(void);

        void CheckRECPara(void); 

        void CheckParaData(void);// 检查内存中的数据是否合法
        void CheckPara(void);

        void RstProcPara(void);
        void CheckTELNUMPara(void);
	 void TELNUMTOggSmsPhone(void);

        void CalcProtCnt(void);
                
#else 
        extern void InitcfgPara(void);   
        extern void SaveCfgPara(void);
        extern void CheckCfgPara(void);
		extern void CheckCfgERR(void);
        extern void InitTsYxInfo(void);
        extern void InitTsYcInfo(void);
        extern void RstRunPara(void);
        extern void RstIEC101Para(void );
        extern void RstIEC101YxAddr(void);
        extern void RstIEC101YcAddr(void);
        extern void RstIEC101YkAddr(void);
        extern void RstIEC101YcSiqu(void);
		extern void RstSmsPhoneInfo(void);
		extern void RstCnName(void);
              extern void RstLBName(void);  
        extern void InitPara(void);
        extern void CheckProtCnt(void);

        extern void CheckAdjPara(void);
        extern void CheckRunInfo(void);

        extern void CheckRECPara(void); 

        extern void CheckParaData(void);// 检查内存中的数据是否合法
        extern void CheckPara(void);

        extern void RstProcPara(void);
        extern void CheckTELNUMPara(void);
	 extern void TELNUMTOggSmsPhone(void);
        extern void CalcProtCnt(void);
        
#endif


//======================================================================
//                  Comm.c--通信模块                
//======================================================================
#ifdef _COMM
        void InitComm(void);
        void InitCommProgram(void);
        int CommRcvData(unsigned char *pBuf,int MaxLen,int uartId);
        int CommSendData(unsigned char *pBuf,int BufLen,int uartId);

		unsigned int YcIdToYxId(unsigned int RYxNo);
        unsigned int CrcCount(unsigned int *p, unsigned int Length);
        unsigned char AddChar(unsigned char *p, unsigned int length);
		unsigned int AddInt(unsigned int *p, unsigned int length);
	unsigned short Fcs16(unsigned short fcs, unsigned char *cp, unsigned short len);
        unsigned short ByAddCrc16(unsigned char *cp, unsigned short len);
		void MyPrintf(char *ch);
		void MyPrintfD(BYTE *p,WORD wLen);
        
      //   void InitComm(void);

      //  unsigned int CrcCount(unsigned int *p, unsigned int Length);
      //  unsigned int AddCount(unsigned int *p, unsigned int length);
        unsigned char AddComChar(unsigned char unCommPortIndex, unsigned char length,unsigned char ucFlag);
      //  void Comm_LED_101(void);
       // unsigned int BuffPositionLED(unsigned int unpointer);
       // void ReadLEDVer(void);
		
                
#else 
        extern void InitComm(void); 
        extern void InitCommProgram(void);

		extern unsigned int YcIdToYxId(unsigned int RYxNo);
        extern int CommRcvData(unsigned char *pBuf,int MaxLen,int uartId);
        extern int CommSendData(unsigned char *pBuf,int BufLen,int uartId);
        extern unsigned int CrcCount(unsigned int *p, unsigned int Length);
       extern unsigned char AddChar(unsigned char *p, unsigned int length);
	   unsigned int AddInt(unsigned int *p, unsigned int length);
       extern unsigned short Fcs16(unsigned short fcs, unsigned char *cp, unsigned short len);
        extern unsigned short ByAddCrc16(unsigned char *cp, unsigned short len);
		extern void MyPrintf(char *ch);
		extern void MyPrintfD(BYTE *p,WORD wLen);
       // extern void InitComm(void);

      //  extern unsigned int CrcCount(unsigned int *p, unsigned int Length);
      //  extern unsigned int AddCount(unsigned int *p, unsigned int length);
        extern unsigned char AddComChar(unsigned char unCommPortIndex, unsigned char length,unsigned char ucFlag);
       // extern void Comm_LED_101(void);
       // extern unsigned int BuffPositionLED(unsigned int unpointer);
       // extern void ReadLEDVer(void);

        
#endif
        
//======================================================================
//                  Comm.c--通信模块                
//======================================================================
#ifdef _COMM_LED
       // void InitComm(void);

       // unsigned int CrcCount(unsigned int *p, unsigned int Length);
        unsigned int AddCount(unsigned int *p, unsigned int length);
       // unsigned char AddChar(unsigned char unCommPortIndex, unsigned char length,unsigned char ucFlag);
        void Comm_LED_101(void);
        unsigned int BuffPositionLED(unsigned int unpointer);
        void ReadLEDVer(void);
                
#else 
       // extern void InitComm(void);

       // extern unsigned int CrcCount(unsigned int *p, unsigned int Length);
        extern unsigned int AddCount(unsigned int *p, unsigned int length);
       // extern unsigned char AddChar(unsigned char unCommPortIndex, unsigned char length,unsigned char ucFlag);
        extern void Comm_LED_101(void);
        extern unsigned int BuffPositionLED(unsigned int unpointer);
        extern void ReadLEDVer(void);
        
#endif

     
    
//======================================================================
//                  DriverTimer.c--定时器驱动模块                
//======================================================================

#ifdef _DRIVER_TIMER
        void InitTimer(void);
        void OpenTimer(int TmId,unsigned int TmVal = 0);
		void RefreshTmCount(unsigned int TmId);
       
        
#else
        extern void InitTimer(void);
        extern void OpenTimer(int TmId,unsigned int TmVal = 0);
		extern void RefreshTmCount(unsigned int TmId);
        
        
#endif

//======================================================================
//                  DriveDMA.c--系统时钟管理模块                
//======================================================================
#ifdef _DRIVE_DMA
        void InitDMA(void);

#else
        extern void InitDMA(void);
        
#endif                
//======================================================================
//                  DriverAD.c--通信驱动模块                
//======================================================================
#ifdef _DRIVER_AD
        void InitAdc(void);

        
#else
        extern void InitAdc(void);

        
#endif

//======================================================================
//                  DriverComm.c--通信驱动模块                
//======================================================================
#ifdef _DRIVER_COMM
        void InitSCI(void);
       // void SendBytes_232(unsigned char *Data, int ucRxdlen);
       // void SendBytes_GPRS(unsigned char *Data,int ucRxdlen);
       // void SendBytes_RF(unsigned char *Data, int ucRxdlen);
        void SendBytes_UART2(unsigned char *Data,int ucRxdlen);
        
        void SendBytes_LED(unsigned char *Data,unsigned char ucRxdlen);
       // void SendBytes_GPRS(unsigned int *Data,unsigned char ucRxdlen);
        void PhysSendByte_GPRS(unsigned int ucByte);
        
#else
        extern void InitSCI(void);
     //   extern void InitSCI0(void);
       // extern void SendBytes_232(unsigned char *Data,int ucRxdlen);
     //   extern void SendBytes_GPRS(unsigned char *Data,int ucRxdlen);
      //  extern void SendBytes_RF(unsigned char *Data,int ucRxdlen);
        extern void SendBytes_UART2(unsigned char *Data,int ucRxdlen);
        
        extern  void SendBytes_LED(unsigned char *Data,unsigned char ucRxdlen);
       // void SendBytes_GPRS(unsigned int *Data,unsigned char ucRxdlen);
        extern  void PhysSendByte_GPRS(unsigned int ucByte);
#endif

//======================================================================
//                  DriverEEprom.c--通信驱动模块                
//======================================================================
#ifdef _DRIVER_EEPROM
      //  void UCB2IICInit(void);
        unsigned char ReadWord(unsigned int  unAddr, unsigned int unSlaveAddr);
        void WriteWord(unsigned int unAddr, unsigned int unSlaveAddr, unsigned char ucWriteData);

        void EEP_ReadBytes(unsigned int unAddr,unsigned int unLength,unsigned char *Data);
        void EEP_WriteBytes(unsigned int unAddr, unsigned int unLength,unsigned int unSlaveAddr, unsigned char *Data);

        unsigned char BCDtoINT(unsigned char ucValue);
        unsigned char INTtoBCD(unsigned char ucValue);
        void UCB0SPIInit(void);
        void UCB2SPIInit(void);
        
        void CAT_SpiConcelWP(void);
        void CAT_SpiWriteByte(unsigned int unAddr,unsigned char ucData);
        void CAT_SpiWriteBytes(unsigned int unAddr,unsigned int unLength,unsigned char *Data);
        unsigned char CAT_SpiReadByte(unsigned int unAddr);
        void CAT_SpiReadBytes(unsigned int unAddr,unsigned int unLength,unsigned char *Data);
        void CAT_SpiWriteWord(unsigned int unAddr,unsigned int unData);
        void CAT_SpiWriteWords(unsigned int unAddr,unsigned int unLength,unsigned int *Data);
        unsigned int CAT_SpiReadWord(unsigned int unAddr);
        void CAT_SpiReadWords(unsigned int unAddr,unsigned int unLength,unsigned int *Data);

        
#else
        extern void UCB2SPIInit(void);

        extern unsigned char ReadWord(unsigned int  unAddr, unsigned int unSlaveAddr);
        extern void WriteWord(unsigned int unAddr, unsigned int unSlaveAddr, unsigned char ucWriteData);

        extern void EEP_ReadBytes(unsigned int unAddr,unsigned int unLength,unsigned char *Data);
        extern void EEP_WriteBytes(unsigned int unAddr, unsigned int unLength,unsigned int unSlaveAddr, unsigned char *Data);

        extern unsigned char BCDtoINT(unsigned char ucValue);
        extern unsigned char INTtoBCD(unsigned char ucValue);
        extern void UCB0SPIInit(void);
        extern void CAT_SpiConcelWP(void);
        extern void CAT_SpiWriteByte(unsigned int unAddr,unsigned char ucData);
        extern void CAT_SpiWriteBytes(unsigned int unAddr,unsigned int unLength,unsigned char *Data);
        extern unsigned char CAT_SpiReadByte(unsigned int unAddr);
        extern void CAT_SpiReadBytes(unsigned int unAddr,unsigned int unLength,unsigned char *Data);
        extern void CAT_SpiWriteWord(unsigned int unAddr,unsigned int unData);
        extern void CAT_SpiWriteWords(unsigned int unAddr,unsigned int unLength,unsigned int *Data);
        extern unsigned int CAT_SpiReadWord(unsigned int unAddr);
        extern void CAT_SpiReadWords(unsigned int unAddr,unsigned int unLength,unsigned int *Data);

#endif

//======================================================================
//                  DriverFlash.c--通信驱动模块                
//======================================================================
#ifdef _DRIVER_FLASH
        //内部flash
        void EraseFlash(INT32U addr);
        void WriteFlashNByte(INT32U dst, INT16U src, INT8U len);
        void WriteFlashStru(INT32U dst);
        void WriteFlashBuff(INT32U dst, INT8U *data, INT8U len);
        void WriteFlashBuff1(INT32U dst, INT8U *data, INT8U len);
        void Read_para(long *src, char *des,int count);
        void WriteFlashBuffWord(INT32U dst, INT16S *data, INT8U len);
        void WriteFlashBuffLongWord(INT32U dst, INT32S *data, INT8U len);
        void WriteFlashByte(INT32U dst, INT8U data);
        void WriteFlashWord(INT32U dst, INT16U data);
        
        //外部flash

        void PageErase(unsigned int unPage);
        void ReadContinuePage(unsigned long ulAddr,unsigned char *PcBuf, unsigned int unsize);
        void DBWriteBytes(unsigned long ulAddr,unsigned char *PcBuf,unsigned int unSize);
        void DBWriteWords(unsigned long ulAddr,unsigned int *PcBuf,unsigned int unSize);

		
        void Sst26vf064b_Page_WriteBytes(long unAddr,unsigned char *PcData,unsigned int Size);
        void Sst26vf064b_Page_WriteWords(long unAddr,unsigned int *PcData,unsigned int Size);
        void Sst26vf064b_Read(long unAddr,unsigned char *PcData,unsigned int Size);
        void Sst_Reset(void);
        void Sst_Unlock(void);
        void Sector_Erase(long unBlock);
        void Sst_ReadID(unsigned char *PcData,unsigned int Size);
        void Sst_ReadJEDECID(unsigned char *PcData,unsigned int Size);		
        void Block_Erase(long unBlock);
        void SpiWaitBusy(void);

        
#else
        //内部flash
        extern  void EraseFlash(INT32U addr);
        extern  void WriteFlashNByte(INT32U dst, INT16U src, INT8U len);
        extern  void WriteFlashStru(INT32U dst);
        extern  void WriteFlashBuff(INT32U dst, INT8U *data, INT8U len);
        extern  void WriteFlashBuff1(INT32U dst, INT8U *data, INT8U len);
        extern  void Read_para(long *src, char *des,int count);
        extern  void WriteFlashBuffWord(INT32U dst, INT16S *data, INT8U len);
        extern  void WriteFlashBuffLongWord(INT32U dst, INT32S *data, INT8U len);
        extern  void WriteFlashByte(INT32U dst, INT8U data);
        extern  void WriteFlashWord(INT32U dst, INT16U data);
        //外部flash
        extern void ReadContinuePage(unsigned long ulAddr,unsigned char *PcBuf, unsigned int unsize);
        extern void DBWriteBytes(unsigned long ulAddr,unsigned char *PcBuf,unsigned int unSize);
        extern void DBWriteWords(unsigned long ulAddr,unsigned int *PcBuf,unsigned int unSize);
        extern void DBWriteBytes_ForLoad(unsigned long ulAddr,unsigned char *PcBuf,unsigned int unSize);

		
        extern void Sst26vf064b_Page_WriteBytes(long unAddr,unsigned char *PcData,unsigned int Size);
        extern void Sst26vf064b_Page_WriteWords(long unAddr,unsigned int *PcData,unsigned int Size);
        extern void Sst26vf064b_Read(long unAddr,unsigned char *PcData,unsigned int Size);
        extern void Sst_Reset(void);
        extern void Sst_Unlock(void);
        extern void Sector_Erase(long unBlock);
        extern void Sst_ReadID(unsigned char *PcData,unsigned int Size);
        extern void Sst_ReadJEDECID(unsigned char *PcData,unsigned int Size);		
        extern void Block_Erase(long unBlock);
        extern void SpiWaitBusy(void);


#endif
//======================================================================
//                  DriverIO.c--IO驱动模块                
//======================================================================
#ifdef _DRIVER_IO
        void InitClk(void);
        void IoInit(void);
        void InitDin(void);
        void ScanDin(void);
        void ScanOut(void);
        void ScanDinYX(void);
	 void ScanSoftLacth(void);
        void ActOut(unsigned int OutType, unsigned int OutValue);
        void ScanClsTrip(void);
        bool WatchWire(void);               //测试无线模块喂狗信号
        
        void InitPort(void);
        void ClkChange(void);  //切换XT2时钟
#else
        extern void InitClk(void);
        extern void IoInit(void);
        extern void InitDin(void);
        extern void ScanDin(void);
        extern void ScanOut(void);
        extern void ScanDinYX(void);
	 extern void ScanSoftLacth(void);	
        extern void ActOut(unsigned int OutType, unsigned int OutValue);
        extern void ScanClsTrip(void);
        extern bool WatchWire(void); 
        
        extern void InitPort(void);
        extern void ClkChange(void);  //切换XT2时钟
#endif

//======================================================================
//                  DriverRTC.c--时钟芯片驱动模块                
//======================================================================
#ifdef _DRIVER_RTC
        void ReadRealTime(void);
        void WriteRealTime(unsigned int *RealTime);
        void InitRtc(void);
        
#else
        extern void ReadRealTime(void);
        extern void WriteRealTime(unsigned int *RealTime);
        extern void InitRtc(void);
        
#endif
//======================================================================
//                  DriveDMA.c--系统时钟管理模块                
//======================================================================
#ifdef _DRIVE_DMA
        void InitDMA(void);

#else
        extern void InitDMA(void);
        
#endif
//======================================================================
//                  Delay.c--时钟芯片驱动模块                
//======================================================================
#ifdef _DELAY
        void delay(unsigned int unDelayNum);
        void delayms(unsigned int unMSNum);
        void delay_1ms(INT16U dat);
        
#else
        extern void delay(unsigned int unDelayNum);
        extern void delayms(unsigned int unMSNum);
        extern void delay_1ms(INT16U dat);

#endif
        
//======================================================================
//                  Prtc101.c--101通信协议驱动模块                
//======================================================================
#ifdef _PRTC101
        void GPRSSendCommand(unsigned char *Ptr);
        unsigned int GPRSinit(void);
        void Comm_GPRS(void);
        void Comm_GPRS_101(void);
        unsigned int BuffCapacity_GPRS(unsigned int unCommPortIndex);
        
#else
        extern void GPRSSendCommand(unsigned char *Ptr);
        extern unsigned int GPRSinit(void);
        extern void Comm_GPRS(void);
        extern void Comm_GPRS_101(void);
        extern unsigned int BuffCapacity_GPRS(unsigned int unCommPortIndex);

#endif

//======================================================================
//                  PrtcSms.c--短信上报驱动模块                
//======================================================================
#ifdef _PRTC_SMS
       void CreatNewSMS(unsigned char ucSMSType);
       void Comm_GPRS_SMS(void);
       void Send_GPRS_SMS(unsigned char ucSMSType,unsigned char ucPhoneNo);
        
        
#else
       extern void CreatNewSMS(unsigned char ucSMSType);
       extern void Comm_GPRS_SMS(void);
       extern void Send_GPRS_SMS(unsigned char ucSMSType,unsigned char ucPhoneNo);

#endif
        
#ifdef  _DS1302
        void DS1302_Delay(unsigned int dtime);
        void DS1302_Reset(void);                       
        void DS1302_WriteOneByte(unsigned char w_dat);
        void DS1302_WriteData(unsigned char addr,unsigned char w_dat);
        void DS1302_SettingData(unsigned char *point);
        void DS1302_GetData(unsigned char *str);
        unsigned char DS1302_ReadOneByte(void);
        unsigned char DS1302_ReadData(unsigned char addr);
        
#else
        extern  void DS1302_SettingData(unsigned char *point);
        extern  void DS1302_GetData(unsigned char *str);
        extern  void DS1302_Reset(void);
#endif 

#ifdef _PRTC_COMM
        unsigned char AddByte( unsigned int length,unsigned int StartIndex,unsigned int unCommPortIndex,unsigned int Rxflag);
        unsigned int BuffCapacity_GPRS(unsigned int unCommPortIndex);
        void TakeFrame_101(unsigned int unCommPortIndex, unsigned int unRxLength);
        void DetectFrame_101 (unsigned int unCommPortIndex);
        void RxProcess_101(unsigned int unCommPortIndex);
        unsigned int BuffPosition(unsigned int unpointer);
        void Comm_232(void);
        
#else
        extern unsigned char AddByte( unsigned int length,unsigned int StartIndex,unsigned int unCommPortIndex,unsigned int Rxflag);
        extern unsigned int BuffCapacity_GPRS(unsigned int unCommPortIndex);
        extern void TakeFrame_101(unsigned int unCommPortIndex, unsigned int unRxLength);
        extern void DetectFrame_101 (unsigned int unCommPortIndex);
        extern void RxProcess_101(unsigned int unCommPortIndex);
        extern unsigned int BuffPosition(unsigned int unpointer);
        extern void Comm_232(void);

#endif        
        
//======================================================================
//                  智芯微加密芯片驱动模块                
//======================================================================
#ifdef _DRIVER_ENCRYPT
        unsigned char CheckLrc (unsigned char *ptr,int num);
        int SendCmdToEncChip(const char *sCmd,BYTE *EncDate,WORD DataLen);
        int RecBytesFromEsam(unsigned char *pRecDataTemp);
        
        
#else
        extern unsigned char CheckLrc (unsigned char *ptr,int num);
        extern int SendCmdToEncChip(const char *sCmd,BYTE *EncDate,WORD DataLen);
        extern int RecBytesFromEsam(unsigned char *pRecDataTemp);
		
#endif
//================================== MD5校验 ===========================================
                
//#define MD5_LONG unsigned int
#define MD5_LONG unsigned long int


#define MD5_CBLOCK	64
#define MD5_LBLOCK	(MD5_CBLOCK/4)
#define MD5_DIGEST_LENGTH 16

typedef struct MD5state_st
{
	MD5_LONG state[5];
	MD5_LONG count[2];
	unsigned char buffer[MD5_CBLOCK];
} MD5_CTX;
#ifdef _MD5_DGST_
    void MD5_Init(MD5_CTX *ctx);
    void MD5_Update(MD5_CTX *ctx, const unsigned char *data, size_t len);
    void MD5_Final(MD5_CTX *ctx, unsigned char *md);
    void MD5(const unsigned char *d, size_t n, unsigned char *md);
#else
    extern void MD5_Init(MD5_CTX *ctx);
    extern void MD5_Update(MD5_CTX *ctx, const unsigned char *data, size_t len);
    extern void MD5_Final(MD5_CTX *ctx, unsigned char *md);
    extern void MD5(const unsigned char *d, size_t n, unsigned char *md);
#endif

#ifdef __cplusplus
}
#endif

//======================================================================
//                  PrtcSms.c--短信上报驱动模块                
//======================================================================


#endif

