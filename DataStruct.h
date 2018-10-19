#ifndef _DATASTRUCT_H

    #define _DATASTRUCT_H
#ifdef __cplusplus
extern "C"
{
#endif  
  
    #include <string.h>
    #include "stdio.h"
    #include <math.h>
    #include "msp430x54xA.h"
    #include "PUBLIC\mytypes.h"
    #include "Constant.h"
    #include "FuncDef.h"
    #include "RemoteUpdata.h"
    #include "RecorderWave.h"  
//	#include "SMS\PrtcSms.h"
     
// #pragma location=0x0FF00 
 const unsigned char VerNum[]="Ver75.00";
const unsigned char VerNum_INLCD=75;
//================================== LOG��־ģ�� ============================================ 
struct sLOG_DATA//1��LOG��־������
{
	unsigned char m_gLogType;
	unsigned char m_gLogValu;
	unsigned char m_gLogTimer[RTC_NUM+1];//�� �� �� ʱ �� �� ���� ����
	unsigned int m_gRmtMeas[8];//U0 UA UB UC I0 UPT CSQ UCAP  ����8������  
};

//================================== AD�ɼ�ģ�� ============================================
struct sSAMPLE_DATA
  {
    int m_gAcAdcData[AC_AD_CHAN_NUM][AC_BUFF_LEN];//������ѹ������AD�ɼ���������һ�ܲ���16���㣬����2���ܲ������ݻ��壬�����������������Ĺ����ж� 
    unsigned int m_unAcDataTail;
  };


#ifdef _DRIVER_AD
  unsigned int g_gRunInfo[RUN_INFO_NUM+1];//������Ϣ������װ�����еĲ�����Ϣ������Ϸ�բ����__no_init 
  struct sSAMPLE_DATA g_sSampleData;//AD�ɼ�����


#else
  extern unsigned int g_gRunInfo[RUN_INFO_NUM+1];//������Ϣ������װ�����еĲ�����Ϣ������Ϸ�բ����
  extern struct sSAMPLE_DATA g_sSampleData;
#endif
//================================== ���ݴ���ģ�� ============================================
 struct sRMT_BACK
 {
 	unsigned long m_RmtInfoBackNew;//��ң�ű仯ʱ
   unsigned int m_Year;
   unsigned int m_Month;
   unsigned int m_Day;
   unsigned int m_Hour;
   unsigned int m_Minute;
   unsigned int m_Milisecond;
  };
 struct sSOE_DATA  //ȫ����������
  {
    unsigned char m_gSoeBuff[SOE_DATA_LEN_BYTE];//�������е����ݣ���RAM�б���1��
    unsigned char m_gSoeRSTBuff[SOE_DATA_LEN_BYTE];
    unsigned char m_ucSoePos;   //���ϼ�¼��EEP�е�λ��;
    unsigned char m_ucSoeNum;	//���ϼ�¼������
    							
    unsigned char m_gAbnBuff[ABN_DATA_LEN_BYTE];//�������е����ݣ���RAM�б���1��
    unsigned char m_gAbnOFFBuff[ABN_DATA_LEN_BYTE];
    unsigned char m_ucAbnPos;    //
    unsigned char m_ucAbnNum;
    
    unsigned char m_gPowerOFFBuff[OFF_DATA_LEN_BYTE];//�������е����ݣ���RAM�б���1��
    unsigned char m_gPowerONBuff[OFF_DATA_LEN_BYTE];
    unsigned char m_ucPowerOFFPos;//
    unsigned char m_ucPowerOFFNum;
       
    unsigned char m_gBreakBuff[BK_DATA_LEN_BYTE];//�������е����ݣ���RAM�б���1��
    unsigned char m_gBreakRSTBuff[BK_DATA_LEN_BYTE];
    unsigned char m_ucBreakPos;//
    unsigned char m_ucBreakNum;
    
    unsigned char m_gRecBuff[WAVE_DATA_LEN_BYTE];//�������е�¼�����ݣ���RAM�б���1��
    unsigned char m_ucRecPos;//
    unsigned char m_ucRecNum;
  };
  struct sREC_DATA  //¼�����ݽṹ�嶨��
  {

    int m_gRecAc[REC_AC_LEN][REC_AC_CHAN_NUM];//���϶����Ľ�����ѹ�������Σ�������϶���ǰ10���ܲ������϶�����6���ܲ�������¼��ʱֻ��Ҫ���涯��֮ǰ10���ܲ�������֮��6���ܲ�
    //unsigned char m_gRecD[REC_AC_LEN];
    unsigned int m_gFaultRecSOE[REC_DATA_LEN];//�����ù���¼����Ӧ�Ĺ���SOE�����ݰ���ʱ�䣬����Ҫ����CRCУ��
    unsigned int m_gFaultRecOver[7]; //��������ʱ��
	
    //������ֵ��д��FLASHʱ�Ӷ�ֵ�е���д��FLASH
    //���±�ʶע����¼�������������0


//����¼�� 
    unsigned char m_ucRecSavingFlag;//¼��������Flash��������� //������ʱ��fault_save=0x55��  YES ��ʼ�洢//�洢������ OFF 
    unsigned char m_ucFaultRecStart;     //����¼����ʼ��־//fault_begin ��ON //fault_end �� OFF//�洢������ OFF
    unsigned int m_unRecAcTail;//���ݻ����β������,�յ�һ�����ݼӼ�
    unsigned int m_unRecAcLockCnt;//m_ucFaultRecStart=ON����ñ����ۼӣ��ﵽ5���ܲ���ֹͣ�������� 
    unsigned int m_unRecACNum;//Ϊ��ֹ����¼���洢���ݲ���640
    unsigned int m_gRecANum;          //��������10�� 
    unsigned int m_gRecCNum; 	         //���ϵ�ǰ¼��λ�� 
    
    unsigned int m_gACTRecANum;          //��������10�� 
    unsigned int m_gACTRecCNum; 	         //������ǰ¼��λ��
    
    unsigned int m_gXHRecANum;          //Ϩ������10�� 
    unsigned int m_gXHRecCNum; 	         //Ϩ����ǰ¼��λ��    
//����¼��
    unsigned int m_gActRecCNum; 	    //��ǰ¼��λ��
    unsigned int m_gActRecANum; 	    //����10��    
    unsigned char m_ucActRecStart;     
	//����¼����ʼ��־  ON��ʼ OFF����¼��  NO �洢����  CLOSE �洢����������¼�� 
    //unsigned long m_gActRecLen;         //������¼����ÿ��¼����ĵ�����ÿ��5��ģ������ÿ��ģ����2�ֽ�	
    unsigned long m_gActRecAdr;           //����¼����δ洢��FLASH�еĵ�ַ
    unsigned char m_ucActRecSavingFlag;//¼��������Flash��������� ?    									//YES�洢������ǰ320�� 
    									//ON�洢������ǰ320�� 
    									//OFF 320��洢��
    unsigned char m_EraseBlock;//����¼���洢������������һ���洢�ռ� ON
    unsigned char m_LuboType;//¼������ 
    
    unsigned int m_CurWritePageCom;           //����ͨ�ŵ�ҳ��
    unsigned int m_gRecNumCom;              //����ͨ�ŵ�¼������
    unsigned char m_gACTDelay;    //8�����������ʱ2�룬��ֹͣ¼��
    unsigned int m_gXHDelay;    //Ϩ����ʱ ����Ϩ�������Ŀ�ʼ�ͽ��� ��Ϩ��¼���Ľ���
    unsigned char m_ucRecStart;     //����¼����ʼ��־     
  }; 

#ifdef _DATA_PROCESS
    __no_init struct sREC_DATA g_sRecData;//¼������
    __no_init struct sLOG_DATA g_sLogData[MAX_LOG_NUM];//LOG����
    unsigned int g_gRmtLockLB;//¼����־λң�ű���ʱ�����
    unsigned char g_SendZJDingshi = 0; 
    unsigned char 	g_YXLBOKCounter;
    unsigned char g_TQBSCounter = 0; //Ͷ�С�����ָʾ�Ƽ�����  =0 ���� =0x55 ��������  >=1Ͷ����˸	
    unsigned char FlashReading;	// 1=���ڶ�flash������¼��������дsoe
    unsigned char PT_LuBo[8];//U0��,u0�ͣ�
    unsigned int g_unAdcData[7]; //ADCMEM�����ݴ�ŵ���������
    unsigned int g_test;
    //unsigned char g_gFaF[256];//
    unsigned char g_gKON;//�̵���״̬
    unsigned char g_gKONBK;//�̵���״̬
    unsigned int g_gRmtMeas[IEC_YC_NUM/*RMT_MEAS_NUM + 4*/];//ң������
    unsigned int g_gRmtFilMeas[IEC_YC_NUM/*RMT_MEAS_NUM + 4*/];//10���м�ֵ�˲����ң������
    unsigned int g_gVErrTimer[10];	      //��ѹ������ֵȷ��ʱ���ʱ	       
    unsigned int g_gRmtMeasPJ[3][32];
    unsigned int g_gRmtMeasBak[IEC_YC_NUM/*RMT_MEAS_NUM*/]; //����ң����
    unsigned int g_gYCYueXian;//�ŏ| ң��Խ��	
    unsigned int g_gYCchangData[20];
    unsigned int g_gChangeYCNum = 0; //ͻ��ң������ 
    unsigned int g_SendChgYcFlag = 0;  //ͻ��ң���־
    unsigned int g_CosSoeDely = 0;  //ͻ��ң���־
    unsigned int g_gWeekNum = 0;
    unsigned long g_gsendYCTimeNum = 0;
    unsigned long g_gsendZJTimeNum = 0;	
    unsigned char g_SendYcDingshi = 0;
    unsigned long g_gsendYXTimeNum = 0;
    unsigned char g_SendYXDingshi = 0;
    unsigned long g_gBeatNum = 0;
    unsigned char g_SendBeat = 0;
    unsigned char g_SendBeatFailureNum = 0;
    unsigned int g_NolinkReset = 0;	
    unsigned int g_NolinkWifi = 0;		
    unsigned char g_GPRSSendLink = 0;// 1��������һ��101
    unsigned char g_RenZLink = 0;	//���� ��֤��־	
    unsigned char g_gRmtInfo[RMT_INFO_NUM];//ң���� һ��ң����Ϣռ1��bitλ
    unsigned char g_gRmtInfoBak[RMT_INFO_NUM];//ң�������� һ��ң����Ϣռ1��bitλ
    //unsigned char g_gSCosBuff[SOE_BUFF_NUM][SOE_DATA_LEN];
    unsigned char g_gSCosBuff[1][SOE_DATA_LEN];
    unsigned char g_MaichongNum = 0;           //�������
    unsigned char g_I0RmtNum = 0;               //��������
    unsigned char g_I0RmtZeroNum=0;	//���߼���õ�������	
    unsigned char g_FirstACTFlag = 0;           //�״ζ�����־
    unsigned char g_FinishACTFlag = 0;
    
    unsigned char g_ChaxunSOE_STATTime[7];
    unsigned char g_ChaxunSOE_OVERTime[7];
    unsigned int g_gSaveload;//ÿx��洢1�θ��ɼ�¼
    //unsigned char g_SendLuboNum = 0;
  
    unsigned int g_unSCosBuffTail;
	unsigned int g_unSSoeInFlashTail;//�������ⲿflash�е�SOEβָ��
    unsigned int g_gSmsHeadPt;       //����ͷָ��
    unsigned int g_gSmsHeadPtrBk;    //����ͷָ�뱸��
    //unsigned int g_gSmsStartPtr;     //���ŷ��Ϳ�ʼָ��
   // unsigned int g_RecFaultNum;      //���ŷ�����������
  //  unsigned int g_unGroundBuffTail;
    unsigned int g_unDSoeSavePtr;//�������˫��SOEָ��
    unsigned int g_unSSoeSavePtr;//������ĵ���SOEָ��
    unsigned long g_unSSoeSaveFlashHead;//���浽�ⲿflash�ĵ���SOEͷ��ַ
    unsigned int g_unSSoeSaveE2ROMPtr;//�����浽�ⲿflash�ĵ���SOEָ��
    unsigned char   soe_ptr; //����SOE��flash�еĶ�λ��
    unsigned char   soe_area;//����SOE��flash����λ��
    unsigned int    soe_num;//SOE�ܸ���
    unsigned int g_unSoeSendNum;
    unsigned int  g_yxChangeflag;//ң�ű�λ��ʶ BITF��ʶ���±����ϴ�(ң�ż�ң��) BIT0-5,6-11�ֱ��ʶ���Կں�gprs�ڶ�Ӧ��SYX,DYX,SSOE,DSOE,SSMS,DSMS;
   // char g_AdChgSdYc;//��ʶAD�ɼ��Ƿ������ϴ�ON=�����ϴ�
   // char g_FltChgSdYc;//��ʶ����ң���Ƿ������ϴ� 1=�����ϴ�
   // char g_PtLoseFlag;//ptʧѹ��ʶ 0=δʧѹ 1=ʧѹ 2=��ѹ�ظ���
  //  char g_BatLoseFlag;//���ʧѹ��ʶ 0=δʧѹ 1=ʧѹ 2=��ѹ�ظ���   
  //  unsigned int g_PtMonitorCnt;//pt��ѹ�ָ�����
  //  unsigned int g_BatMonitorCnt;//��ص�ѹ�ָ�����
   // char g_SendYc;//0=����ң��������ͣ�1=��������(��ʱ�ϱ���ң�⣬����ң�⣬AD�ɼ�ֵ�仯��ң�⣬Խ��ң��)
  //  unsigned char  g_Soenum;
  //  unsigned int  g_ZsqCmdRBit = 0;
  //  unsigned int  g_ZsqCmdWBit = 0;
  //  unsigned int  g_JdgPwMode = 0;
    
    __no_init struct sSOE_DATA g_sSoeData;//SOE��RAM�еĻ���������

  	//IAR��Է�����ڴ��ڽ���main����֮ǰ������0����Ҫռ�ý϶��ʱ�䣬���¿��Ź���������˶�ռ�ýϴ��ڴ��������"__no_init"��ֹ��0���ŵ�������ע��������г�ʼ��

	//unsigned int g_gRmtMeas[RMT_MEAS_NUM];//ң������������Ҫ�ϴ�����̨��ң������ = ����(g_gProcMeas/16) * 0.6944
	long g_gProcMeas_AC_R[6];
	long g_gProcMeas_AC_I[6];
	unsigned int g_gRmAcFilt[RMT_MEAS_NUM][RM_FILTER_NUM];//ң�����еĽ������������˲� �����3���ߵ�ѹ����ADͨ�������䣬���Դ˴�AC_AD_CHAN_NUM + 3
        unsigned int g_unFilterIndex = 0;   //�������������˲����ݱ����λ��
        volatile unsigned int g_unRmCaluFlag;    //ң�������־, ��1.25ms�ж�����ON����1ms����OFF���ڽ���ң�����㣬����ñ�־ΪON��˵��ң�����ݸ����ˣ����Խ���ң��������
        volatile unsigned int g_unTESTFlag; 
		volatile unsigned int g_unUABCaluFlag;	//ң�������־, ��50ms�ж�����ON���ڴ�ѭ������OFF��
  	//unsigned long g_gProcMeas[PROC_MEAS_NUM];//�����ò����������ڱ����߼��жϵĲ�������Ϊ����ֵ��ƽ��*16 ���ֵΪ(2048*2048/2*16)
        //unsigned int g_gRmtInfo[RMT_INFO_NUM];//ң�����������ڲ�ң����

	unsigned int  g_SendLuboNum;
	unsigned int  g_SendLuboPage;
	unsigned int g_FRecorder_Current_COUNT;//flash���浽�ڼ�����
	unsigned int g_Recorder_Total_COUNT;//�ܵ�����	
     WORD wCfgTotalNum =0;//�ܳ���
#ifdef YN_101S     
     WORD section_current=0;//��ǰ��*/
#else
     WORD section_current=1;//��ǰ��*/
#endif
#else
extern unsigned int g_test;
    extern unsigned int g_gSaveload;//ÿx��洢1�θ��ɼ�¼
    //extern unsigned char g_gFaF[256];//
    extern unsigned char g_YXLBOKCounter;
    extern unsigned int g_gRmtLockLB;//¼����־λң�ű���ʱ�����	
    extern unsigned char g_SendZJDingshi; 
    extern unsigned char g_TQBSCounter; 
    extern unsigned char FlashReading;		
    extern struct sREC_DATA g_sRecData;
    extern unsigned int g_unAdcData[7]; //ADCMEM�����ݴ�ŵ���������
    extern unsigned char g_gKON;//�̵���״̬
    extern unsigned char g_gKONBK;//�̵���״̬
    extern unsigned int g_gRmtMeas[IEC_YC_NUM/*RMT_MEAS_NUM + 4*/];//ң������������Ҫ�ϴ�����̨��ң������ 
    extern unsigned int g_gRmtFilMeas[IEC_YC_NUM/*RMT_MEAS_NUM + 4*/];//10���м�ֵ�˲����ң������
    extern unsigned int g_gRmtMeasPJ[3][32];
    extern unsigned int g_gRmtMeasBak[IEC_YC_NUM/*RMT_MEAS_NUM */]; //����ң����
    extern unsigned int g_gYCYueXian;//�ŏ| ң��Խ��	
    extern unsigned int g_gYCchangData[20];
    extern unsigned int g_gChangeYCNum; //ͻ��ң������
    extern unsigned int g_SendChgYcFlag;  //ͻ��ң���־
    extern unsigned int g_CosSoeDely;  //ͻ��ң���־
    extern unsigned int g_gWeekNum;
    extern unsigned long g_gsendYCTimeNum ;
    extern unsigned char g_SendYcDingshi;
    extern unsigned long g_gsendZJTimeNum ;	 		
    extern unsigned long g_gsendYXTimeNum ;
    extern unsigned char g_SendYXDingshi;
    extern unsigned char g_GPRSSendLink ;
    extern unsigned char g_RenZLink;	//���� ��֤��־
    
    extern unsigned char g_gRmtInfo[RMT_INFO_NUM];//ң���� һ��ң����Ϣռ2��bitλ
    extern unsigned char g_gRmtInfoBak[RMT_INFO_NUM];
    //extern unsigned char g_gSCosBuff[SOE_BUFF_NUM][SOE_DATA_LEN];
    extern unsigned char g_gSCosBuff[1][SOE_DATA_LEN];    
   // extern unsigned char g_gGroundBuff[SOE_BUFF_NUM];
    extern unsigned int g_unSCosBuffTail;
    
    extern unsigned char g_MaichongNum;           //�������
    extern unsigned char g_I0RmtNum;               //��������
    extern unsigned char g_I0RmtZeroNum;	//���߼���õ�������	
    extern unsigned char g_FirstACTFlag;           //�״ζ�����־
    extern unsigned char g_FinishACTFlag;

    extern unsigned int g_unSSoeInFlashTail;//�������ⲿflash�е�SOEβָ��
    extern unsigned int g_gSmsHeadPt;       //����ͷָ��
    extern unsigned int g_gSmsHeadPtrBk;    //����ͷָ�뱸��
   // extern unsigned int g_gSmsStartPtr;     //���ŷ��Ϳ�ʼָ��
   // extern unsigned int g_RecFaultNum;      //���ŷ�����������
    extern unsigned int g_unDSoeSavePtr;//�������˫��SOEָ��
    extern long g_unSSoeSavePtr;//������ĵ���SOEָ��
    extern unsigned long g_unSSoeSaveFlashHead;//���浽�ⲿflash�ĵ���SOEͷ��ַ
    extern unsigned int g_unSSoeSaveE2ROMPtr;//�����浽�ⲿflash�ĵ���SOEָ��
   // extern unsigned int g_unGroundBuffTail;
    extern unsigned char   soe_ptr; //����SOE��flash�еĶ�λ��
    extern unsigned char   soe_area;//����SOE��flash����λ��
    extern unsigned int    soe_num;//SOE�ܸ���
    extern unsigned int g_unSoeSendNum;
    extern unsigned int  g_yxChangeflag;
    extern unsigned char g_ChaxunSOE_STATTime[7];
    extern unsigned char g_ChaxunSOE_OVERTime[7];
    //extern unsigned char g_SendLuboNum;
    extern unsigned long g_gBeatNum;
    extern unsigned char g_SendBeat;
    extern unsigned char g_SendBeatFailureNum;
    extern unsigned int g_NolinkReset;	    
    extern unsigned int g_NolinkWifi;	
    //extern char g_AdChgSdYc;
   // extern char g_FltChgSdYc;
   // extern char g_PtLoseFlag;
   // extern unsigned int g_PtMonitorCnt;
   // extern char g_SendYc;
   // extern unsigned char  g_Soenum;
   // extern unsigned int  g_ZsqCmdRBit;
  //  extern unsigned int  g_ZsqCmdWBit;
   // extern unsigned int  g_JdgPwMode;
    extern struct sSOE_DATA g_sSoeData;    
	extern struct sLOG_DATA g_sLogData[MAX_LOG_NUM];//LOG����
  	//extern unsigned int g_gRmtMeas[RMT_MEAS_NUM];//ң������������Ҫ�ϴ�����̨��ң������
        extern unsigned int g_gRmAcFilt[RMT_MEAS_NUM][RM_FILTER_NUM];//ң�����еĽ������������˲� �����3���ߵ�ѹ����ADͨ�������䣬���Դ˴�AC_AD_CHAN_NUM + 3
	//extern unsigned long g_gProcMeas[PROC_MEAS_NUM];//�����ò����������ڱ����߼��жϵĲ�����
       // extern unsigned int g_gRmtInfo[RMT_INFO_NUM];//ң�����������ڲ�ң����   
       extern volatile unsigned int g_unRmCaluFlag;    //ң�������־, ���ж�����ON���ڴ�ѭ������OFF���ڽ���ң�����㣬����ñ�־ΪON��˵��ң�����ݸ����ˣ����Խ���ң��������
       extern volatile unsigned int g_unTESTFlag;
	   extern volatile unsigned int g_unUABCaluFlag;	//ң�������־, ��50ms�ж�����ON���ڴ�ѭ������OFF��
	extern unsigned int  g_SendLuboNum;
	extern unsigned int  g_SendLuboPage;
    extern WORD wCfgTotalNum ;//�ܳ���
    extern WORD section_current;//��ǰ��*/
    extern unsigned int g_FRecorder_Current_COUNT;//flash���浽�ڼ�����
	extern unsigned int g_Recorder_Total_COUNT;//�ܵ�����    
#endif

//=======================================  ��������ģ��  ===================================
#ifdef _PARA_CFG

        unsigned int g_gRunParaYxRsdTTBk;//���в�����ң���ط�ʱ��������
        unsigned int g_unYxTrans[RMT_WSYX_NUM];  //����ң�ŵĵ�������������ɵ�ң������
        //unsigned int g_unDYxTrans[RMT_WDYX_NUM];//�ϴ���˫��ң��
        unsigned int g_unZJTrans[IEC_ZJ_NUM];    //�ϴ���ң�����ݣ���������ϴ��ĳ���ˢ�����������ϴ�        
        unsigned int g_unYcTrans[IEC_YC_NUM];    //�ϴ���ң�����ݣ���������ϴ��ĳ���ˢ�����������ϴ�
        unsigned char g_ucYcTransNum; //��¼��Ҫ�ϴ���ң��ĸ���
        unsigned int g_ucYxTransNum; //��¼u��Ҫ�ϴ�ң�ŵ��ܸ���(���������˫��)

        unsigned int g_ucSYxTrsStartId;//�ϴ��ĵ���ң���е�һ��������(��g_ucYXAddr������)
        unsigned int g_ucDYxTrsStartId;//�ϴ���˫��ң���е�һ��������(��g_ucYXAddr������)
        unsigned char g_ucYkTransNum;
        unsigned char g_ucParaChang = 0; //101�������������־  BIT0Ϊ 101������BIT1Ϊң����BIT2Ϊң��ϵ����BIT3Ϊң��ƫ������BIT4Ϊң�ŵ�� BIT5Ϊ���в�����//�ŏ| ������վ��BIT6//
        unsigned char g_ucRefYxYcTs = 0;//ң��ң�����ݱ仯��־ BIT0Ϊң�� BIT1Ϊң��
        unsigned char g_ZsqPara = 0;   //zx
        
        unsigned char  g_gSmsPhone[PHONE_PA_NUM];//�����е绰����
        unsigned int g_gCnName[CnName_NUM+1];//�ŏ| ����վ����ַ
        unsigned char g_gLBNameLen;
	 unsigned char  g_gLBName[LBName_NUM+1];//�ŏ| ����վ����ַ
     //   unsigned int g_gRunPara[RUN_PARA_NUM];//���в���������װ�õ�һЩ������Ϣ������ͨ�Ų�����
     	unsigned char g_ucPara101[IEC_PARA_NUM];    //IEC���е����в���
     	//unsigned char g_ucDebug[Debug_PARA_NUM];    //�����ò���
     	//unsigned char g_unParaObjComBk[4];//IEC�����еĶ˿ڶ���4�����ݽ��б��ݣ��б仯ʱ��ӳ���������˿ڵĶ�Ӧ��ϵ
        unsigned char g_ucYCAddr[IEC_YC_NUM + 1];   //ң��ĵ�����԰�ң���е����ݰ���ƫ�����������У���ఴ��20�����ã�Ϊ0�Ĳ��ϴ������Ϊ1-255
        unsigned char g_ucYXAddr[IEC_YX_NUM + 1];   //ң�ŵĵ��Ϊ0�Ĳ��ϴ������Ϊ1-255
    //    unsigned char g_ucYKPa[IEC_YK_NUM + 1];   //ң�ز��� ��ʶָʾ������ƫ��ƫ
		
			
		//unsigned int g_gRunParaFltLkTBk;//���в����й��ϱ���ʱ�䱸�ݣ����ڲ����仯ʱʹ��������Ч
       // unsigned int g_gRunParaZsqStTBk;//���в�����ָʾ��ͨ��״̬������ڱ���
       // unsigned int g_gRunParaYxRsdTTBk;//���в�����ң���ط�ʱ��������
        
       // unsigned char g_open_oducall;//�����ϵ�ʱ5S�ٻ�һ�Σ��ȴ��ɼ���C�ĵ�ѹ�����ж����õ�ػ��ǳ������ݴ�����GPRS
      //  unsigned int Before4HourSdYcTm;
		//unsigned char g_LineFlag[12];    //ODU����·����״̬?0Ϊ��������·�ϳ�1 Ϊ�ϳɵ�ǰһ·2 Ϊ�ϳɵĺ�һ·
        
        extern const unsigned int BIT[16] =   //��õڼ�λΪ1��ֵ��BIT0~BIT15
    	{
                0x01,0x02,0x04,0x08,
        0x10,0x20,0x40,0x80,
        0x100,0x200,0x400,0x800,
        0x1000,0x2000,0x4000,0x8000
    	};
        
        unsigned int g_gRunPara[RUN_PARA_NUM];//���в���������װ�õ�һЩ������Ϣ������ͨ�Ų�����
        BYTE g_gIPPort[13];//  2��IP���˿ڣ�
        BYTE g_gAPN[20];
        BYTE g_gSMSR;//GPRSģ����ն���  0=������ 1=����         
        unsigned int g_gProcCnt[PROC_CNT_NUM];//������ֵ�����汣��������Ҫ�õ��Ķ�ֵ
        //unsigned int g_gETHProcCnt[24];
        unsigned int g_gProcCntJug[27];       //�����жϵļ����ı�����ֵ
        //unsigned long g_gProcCntJug_I0[3];       //���ڹ����жϵļ����ı�����ֵ
        unsigned int g_gOverLoadTimeNum;     //���ڹ��ؼ�ʱ��ʱ����1.25ms��׼
        unsigned int g_gOverLoadFlg;        //���ر�־ ON Ϊ����
        unsigned int g_gAdjObj[ADJ_PARA_NUM];//�ŏ| Ŀ��У׼����λ�����ز��� ��ʼֵΪ��ѹ60V,����2A    
        unsigned char g_gModfk[24];
	unsigned char kat,kbt,kct,Numk;	
  	int g_gAdjPara[ADJ_PARA_NUM];//У���������������ͨ����У������   
  	unsigned int g_gAdjAD[ADJ_PARA_NUM];//����У���������������ADֵ��ƫ����
  	//unsigned int g_gRunInfo[RUN_INFO_NUM];//������Ϣ������װ�����еĲ�����Ϣ������Ϸ�բ����  	
  	unsigned char g_gChangFlag[PARA_NUM];	//�����仯��־����������������ģ�����λ���Ѳ������浽eeprom��
        //unsigned int g_gYXConformTime[2];       //ң��ȷ��ʱ�䣬0���ź�Դ���� 1��
        unsigned char g_gDebugP[Debug_PARA_NUM];//���Դ��ڲ�����
#else
        extern unsigned int g_gRunParaYxRsdTTBk;//���в�����ң���ط�ʱ��������
        extern unsigned int g_unYxTrans[RMT_WSYX_NUM];  //����ң�ŵĵ�������������ɵ�ң������
        //extern unsigned int g_unDYxTrans[RMT_WDYX_NUM];//�ϴ���˫��ң��
        extern unsigned int g_unZJTrans[IEC_ZJ_NUM];       
        extern unsigned int g_unYcTrans[IEC_YC_NUM];    //�ϴ���ң�����ݣ���������ϴ��ĳ���ˢ�����������ϴ�
        extern unsigned char g_ucYcTransNum; //��¼��Ҫ�ϴ���ң��ĸ���
        extern unsigned int g_ucYxTransNum; //��¼u��Ҫ�ϴ�ң�ŵĸ���
        extern unsigned int g_ucSYxTrsStartId;//�ϴ��ĵ���ң���е�һ��������(��g_ucYXAddr������)
        extern unsigned int g_ucDYxTrsStartId;//�ϴ���˫��ң���е�һ��������(��g_ucYXAddr������)
      //  extern unsigned int g_ucDYxTransNum;
        extern unsigned char g_ucYkTransNum;
        extern unsigned char g_ucParaChang; //101�������������־  BIT0Ϊ 101������BIT1Ϊң����BIT2Ϊң��ϵ����BIT3Ϊң��ƫ������BIT4Ϊң�ŵ�� BIT5Ϊ���в�����
        extern unsigned char g_ucRefYxYcTs;//ң��ң�����ݱ仯��־ BIT0Ϊң�� BIT1Ϊң��
                
		extern unsigned char  g_gSmsPhone[PHONE_PA_NUM];//�����е绰����  //�ŏ| 0328
		extern unsigned int g_gCnName[CnName_NUM+1];//�ŏ| ����վ����ַ
		extern unsigned char g_gLBName[LBName_NUM+1];//�ŏ| ����վ����ַ	
		extern unsigned char g_gLBNameLen;
	//	extern unsigned int g_gRunPara[RUN_PARA_NUM];//���в���������װ�õ�һЩ������Ϣ������ͨ�Ų�����
    	extern unsigned char g_ucPara101[IEC_PARA_NUM];    //IEC���е����в���
    	//extern unsigned char g_ucDebug[Debug_PARA_NUM];    //�����ò���
		//extern unsigned char g_unParaObjComBk[4];//IEC�����еĶ˿ڶ���4�����ݽ��б��ݣ��б仯ʱ��ӳ���������˿ڵĶ�Ӧ��ϵ
		extern unsigned char g_ucYCAddr[IEC_YC_NUM + 1];   //ң��ĵ�����԰�ң���е����ݰ���ƫ�����������У���ఴ��20�����ã�Ϊ0�Ĳ��ϴ������Ϊ1-255
        extern unsigned char g_ucYXAddr[IEC_YX_NUM + 1];   //ң�ŵĵ��Ϊ0�Ĳ��ϴ������Ϊ1-255 
     //   extern unsigned char g_ucYKPa[IEC_YK_NUM + 1]; 
		
       //	extern unsigned int g_gRunParaFltLkTBk;//���в����й��ϱ���ʱ�䱸�ݣ����ڲ����仯ʱʹ��������Ч
		//extern unsigned int g_gRunParaZsqStTBk;//���в�����ָʾ��ͨ��״̬������ڱ���
		//extern unsigned int g_gRunParaYxRsdTTBk;//���в�����ң���ط�ʱ��������
        extern const unsigned int BIT[16];
        
        extern unsigned int g_gRunPara[RUN_PARA_NUM];//���в���������װ�õ�һЩ������Ϣ������ͨ�Ų�����
        
		//extern unsigned char g_open_oducall;
		//extern unsigned int Before4HourSdYcTm;
		//extern unsigned char g_LineFlag[12];
                extern unsigned int g_gProcCnt[PROC_CNT_NUM];//������ֵ�����汣��������Ҫ�õ��Ķ�ֵ
        //extern unsigned int g_gETHProcCnt[24];
        extern unsigned int g_gProcCntJug[27];       //�����жϵļ����ı�����ֵ
        //extern unsigned long g_gProcCntJug_I0[3];       //���ڹ����жϵļ����ı�����ֵ
        extern unsigned int g_gOverLoadTimeNum;     //���ڹ��ؼ�ʱ��ʱ����1.25ms��׼
        extern unsigned int g_gOverLoadFlg;        //���ر�־ ON Ϊ����
        extern BYTE g_gIPPort[13];//  2��IP���˿�
        extern BYTE g_gAPN[20];
        extern BYTE g_gSMSR;//GPRSģ����ն���  0=������ 1=����         
	extern int g_gAdjPara[ADJ_PARA_NUM];//У���������������ͨ����У������
	extern unsigned int g_gAdjAD[ADJ_PARA_NUM];//����У���������������ADֵ��ƫ����
	extern unsigned int g_gAdjObj[ADJ_PARA_NUM];//�ŏ| Ŀ��У׼����λ�����ز��� ��ʼֵΪ��ѹ60V,����2A
	extern unsigned char g_gModfk[24];
	extern unsigned char kat,kbt,kct,Numk;	
        extern int g_gAdjParaBuf[ADJ_PARA_NUM];
	
	extern unsigned char g_gChangFlag[PARA_NUM];	//�����仯��־����������������ģ�����λ���Ѳ������浽eeprom��
	//extern unsigned int g_gYXConformTime[2];       //ң��ȷ��ʱ�䣬0���ź�Դ���� 1��
	extern unsigned char g_gDebugP[Debug_PARA_NUM];//���Դ��ڲ�����	
#endif

//=================================== ͨ��ģ�� ==========================================
  struct  sTX_BUFF		   //ͨ�ŷ��ͻ���ṹ��
	{
	  unsigned char m_gBuffer[MAX_RXBUFF_LEN];//�������ݻ���
	  unsigned int m_ucTx_Head;//GPRS�����жϷ������ݵĻ���ͷָ��
      unsigned int m_ucTx_Tail;//GPRS�����жϷ������ݵĻ���βָ��
      unsigned char m_unSendFlag;
	};


  struct  sRX_BUFF         //ͨ�Ž��ջ���ṹ��
  {
    unsigned char m_gBuf[MAX_RXBUFF_LEN];//�������ݻ���
    unsigned int m_iBufT;      //���ݻ���βָ��
    unsigned int m_iBufH;      //���ݻ���ͷָ��
    unsigned int m_iRcvCount; //�����ֽ�ʱ����
    unsigned char m_iRcvFlag;
  };
/*  
    struct  sRX_BUFF_LED         //ͨ�Ž��ջ���ṹ��
  {
    unsigned char m_gBuffer[MAX_RXBUFF_LEN];//�������ݻ���
    unsigned int m_iBufferTail;
    unsigned int m_iBufferHead;
  };
  */
#ifdef _COMM
    __no_init struct  sTX_BUFF g_sTxBuff[COMM_PORT_NUM];
    __no_init struct  sRX_BUFF g_sRxBuff[COMM_PORT_NUM];
             // struct  sRX_BUFF_LED  g_sRX_BUFF_LED;
       // unsigned int g_sCommStatCount[RMT_ZSQST_NUM];//�ж�ָʾ��ͨ��״̬ʱ�����
        char g_YkOrderFlag;//��ʶң���Ƿ�ɹ�

        unsigned char g_GprsPowerSt;//GPRS��Դ״̬ 0=�ر� 1= ��?      
        unsigned char g_batPowerSt;//bat״̬ 0=�ر� 1= ��?		

        unsigned char g_SmsWritePtr;
        unsigned char g_EnSmsSend;  //���ŷ�������
        unsigned char g_SmsDataLen;
        unsigned char g_EnSendToXinhao;
		
        unsigned char g_SmsReSendWaitCount; //�����ط��ȴ�ʱ��
        unsigned char g_SmsReSendTime;   //�����ط�����
    	unsigned char g_SmsSendBuf[55];
		unsigned char g_CmIdRF;//RF����ʹ�õĴ��ں�
		unsigned char  g_CmIdGPRS;
		unsigned char g_CmIdEX;//��չ��ʹ�õĴ��ں�
		unsigned char g_CmIdDBG;//���Կ�ʹ�õĴ��ں�
		unsigned char g_CmIdODU1;//ODU1ʹ�õĴ��ں�
		unsigned char g_CmIdODU2;//ODU2ʹ�õĴ��ں�
		unsigned int g_RstartNum;//ϵͳ��������
                
        //unsigned char g_unTX_Buff[2][256];            //���ݷ�������
    
    unsigned char g_unSMS_Index[MAX_SMS_NUM];              //�洢�²����Ķ��ű�־
    unsigned char g_unFrameFlag_Sms=0;             //������������
    unsigned char g_unFrameFlag_curSms=0;             //������������
    unsigned char g_unSms_s=0;             //  0=���Է��Ͷ���  >0ֹͣ����,�ȴ���ȷ�ظ�
    unsigned char g_unPhoneNo=0;
    unsigned char g_unSms_Count=0;     //���ŷ��ͺ��ʱ����15����޻ظ����ط�
    unsigned char g_unFailSms=0;//���ŷ���ʧ�ܴ���
   // unsigned char g_unGPRSTX_Buff[MAX_TXBUFF_101_LEN];          //GPRS���ݷ������� 
    unsigned char g_unRX_Buff[1][MAX_RXBUFF_LEN];            //���ݽ�������
    //unsigned char g_unLEDTX_Buff[MAX_TXBUFF_101_LEN];         //���ݷ�������
    unsigned char g_uc101BuffHead;   //GPRSͨ�Ž��ջ���������ͷ
    unsigned char g_uc101BuffDataHead;   //GPRSͨ�Ž��ջ���������ͷ    
    unsigned char g_uc101BuffTail;   //GPRSͨ�Ž��ջ���������β
    
    unsigned int bFrameReceived[COMM_PORT_NUM];   //���������������ݱ�־
    //unsigned int g_ucBuffHead[2];   //ͨ�Ž��ջ���������ͷ
    //unsigned int g_ucBuffTail[2];   //ͨ�Ž��ջ���������β
    
    //unsigned char g_ucUpdateTemp[256];   //����ר������
   
    //unsigned int g_unFrameFlag_101[2];  //101Э������֡��־λ


   // const unsigned char VerNum[2]={0,22};   //ע:  �޸������µİ汾  V21
    //unsigned char TEL_NUM[55]={0};   //4��绰����
    //unsigned char g_ucphone_perm[5];         //�绰���ŵĹ���ѡ��
    unsigned char ucTempTEL_NUM[11] = {0};  //��ʱ�洢���룬������վ�Ĳ�ѯ

    unsigned int g_ucGPRSTxlen;         //�����ж����ݳ���
    unsigned int g_ucGPRSTxNum;         //

    unsigned int g_ucGPRSState=GPRSState_IDLE;					//GPRS״̬��0=IDLE,1=GPRS101��2=SMS//�ŏ| 0330 
    unsigned int g_ucGPRSState_TCounter;		//GPRS״̬ʱ��������������ʱ�䲻����0=IDLE״̬����ָ�0=IDLE״̬//�ŏ| 0330
    
    unsigned int g_uc232Txlen;         //�����ж����ݳ���
    unsigned int g_uc232TxNum;         //
    
    unsigned char sign_RSSI_time[6];              ////year month day hour minute second 
    //unsigned char pulse_flag=0;                  ////////�������⵽��־   0x55
    //unsigned char wave_counter=0;                //////// �Է���8����󣬲ɼ����ܲ����м���

    //unsigned char pulse_time_flag;             ////////��һ�η���8������ñ�־��Ϊ��10��������ṩ��־
    unsigned char g_ucPara_stime;  //�����ϴ����ʱ��
    unsigned char eight_pulse_counter;
    unsigned int unFrameNum = 0; //��ʱ�洢�����Ͷ�������
    unsigned char g_uc101Flg = 0;
    
    /*unsigned char g_ucRELConfirm;   //ͨ����ң��ȷ�ϱ�־
    unsigned char g_ucRELUNM;       //ͨ����ң�ؼ̵�����˳���
    unsigned int g_unRELTimeFlg;    //ͨ���ü̵���ѡ��ʱ���־
    unsigned int g_unRELTimeFlg_101;
    
    unsigned int g_unOffset;    //�����ַת��ƫ����*/
    //char  ComtrderCfg1[360];
    char  ComtrderCfg_adj[50];
	
	unsigned char g_EncComNo;		 //�ĸ��������ܼ�������1==GPRS 2 ==DeBug�� ������Ч
	unsigned char g_bEncCerTxt[1];  //����CER֤��
	unsigned char g_bHostRandom[8];  //��վ�����
	unsigned char m_bUnitRandom[8];   //�ն��������������������ά����ͨ��
	int g_wEncCerWPtr;				   //Cer֤���¼λ?
	char g_bUnitID[1];//�ն�ID,24�ֽڵ�һ�����ǽڳ���	 
	unsigned char g_bAppType;//���ܱ���Ӧ������
	//char g_pDevId[30]="12345678901234567890123456789";
#else
    extern struct  sTX_BUFF g_sTxBuff[COMM_PORT_NUM];
    extern struct  sRX_BUFF g_sRxBuff[COMM_PORT_NUM];
    //extern struct  sRX_BUFF_LED  g_sRX_BUFF_LED;
    extern char g_YkOrderFlag;//��ʶң���Ƿ�ɹ�
    extern unsigned char g_GprsPowerSt;//GPRS��Դ״̬ 0=�ر� 1= ��
     extern unsigned char g_batPowerSt;//bat״̬ 0=�ر� 1= ��?    extern int g_ODUYK;//

	extern unsigned char g_SmsWritePtr;
	extern unsigned char g_EnSmsSend;
	extern unsigned char g_SmsDataLen;
    extern unsigned char g_EnSendToXinhao;
    extern unsigned char g_SmsReSendWaitCount;
    extern unsigned char g_SmsReSendTime;
    extern unsigned char g_SmsSendBuf[55];
	extern unsigned char g_CmIdRF;//RF����ʹ�õĴ��ں�
	extern unsigned char  g_CmIdGPRS;
	extern unsigned char g_CmIdEX;//��չ��ʹ�õĴ��ں�
	extern unsigned char g_CmIdDBG;//���Կ�ʹ�õĴ��ں�
	extern unsigned char g_CmIdODU1;//ODU1ʹ�õĴ��ں�
	extern unsigned char g_CmIdODU2;//ODU2ʹ�õĴ��ں�
	
    extern int g_pODU2WaitCount;//ODU��Դ�ر�����ODU2�Ľ��ռ�ʱ
	extern int g_pODUWaitCount;//ODU��Դ�ر�����
	extern unsigned int g_RstartNum;//ϵͳ��������
        
    //extern unsigned char g_unTX_Buff[2][256];
    
    extern unsigned char g_unSMS_Index[MAX_SMS_NUM];              //�洢�²����Ķ��ű�־; 
    extern unsigned char g_unFrameFlag_Sms;             //������������
    extern unsigned char g_unFrameFlag_curSms;             //������������     
    extern unsigned char g_unSms_s;             //  1=���Է��Ͷ���  0=ֹͣ����    
    extern unsigned char g_unPhoneNo;
    extern unsigned char g_unSms_Count;	
    extern unsigned char g_unFailSms;//���ŷ���ʧ�ܴ���
    //extern unsigned char g_unGPRSTX_Buff[MAX_TXBUFF_101_LEN];          //GPRS���ݷ�������
    
    extern unsigned char g_unRX_Buff[1][MAX_RXBUFF_LEN];
    //extern unsigned char g_unLEDTX_Buff[MAX_TXBUFF_101_LEN];         //���ݷ�������
    

    extern unsigned int bFrameReceived[COMM_PORT_NUM];   //���������������ݱ�־

    //extern unsigned int g_ucBuffHead[2];         //ͨ�Ž��ջ���������ͷ
    //extern unsigned int g_ucBuffTail[2];         //ͨ�Ž��ջ���������β
    extern unsigned char g_uc101BuffHead;
    extern unsigned char g_uc101BuffDataHead;   //GPRSͨ�Ž��ջ���������ͷ  	
    extern unsigned char g_uc101BuffTail;
    //extern unsigned char g_ucUpdateTemp[256];   //����ר������
 
    //extern unsigned int g_unFrameFlag_101[2];  //101Э������֡��־λ
  //  extern const unsigned char VerNum[2];   //ע:  �޸������µİ汾  V21
    //extern unsigned char TEL_NUM[55];   //5��绰����
    //extern unsigned char g_ucphone_perm[5];         //�绰���ŵĹ���ѡ��
    extern unsigned char ucTempTEL_NUM[11];  //��ʱ�洢���룬������վ�Ĳ�ѯ

    extern unsigned int g_ucGPRSTxlen;         //�����ж����ݳ���
    extern unsigned int g_ucGPRSTxNum;         //

    extern unsigned int g_ucGPRSState;         //GPRS״̬��0=IDLE,1=GPRS101��2=SMS//�ŏ| 0330 
    extern unsigned int g_ucGPRSState_TCounter;         //GPRS״̬ʱ��������������ʱ�䲻����0=IDLE״̬����ָ�0=IDLE״̬//�ŏ| 0330
	
    
    extern unsigned int g_uc232Txlen;         //�����ж����ݳ���
    extern unsigned int g_uc232TxNum;         //
    
    extern unsigned char sign_RSSI_time[6];              ////year month day hour minute second
    //extern unsigned char pulse_flag;                  ////////�������⵽��־   0x55
    //extern unsigned char wave_counter;                //////// �Է���8����󣬲ɼ����ܲ����м���
 
    extern unsigned char g_ucPara_stime;  //�����ϴ����ʱ��
    extern unsigned char pulse_time_flag;             ////////��һ�η���8������ñ�־��Ϊ��10��������ṩ��־
    extern unsigned char eight_pulse_counter;
    extern unsigned int unFrameNum; //��ʱ�洢�����Ͷ�������
    extern unsigned char g_uc101Flg;
    //extern char  ComtrderCfg1[360];
    extern char  ComtrderCfg_adj[50];
		
	extern unsigned char g_EncComNo;//
	extern unsigned char g_bEncCerTxt[1];
 	extern unsigned char g_bHostRandom[8];//��վ�����
	extern unsigned char m_bUnitRandom[8];//
	extern int g_wEncCerWPtr;//   
	extern char g_bUnitID[1];//�ն�ID,24�ֽڵ�һ�����ǽڳ���   
	extern unsigned char g_bAppType;//���ܱ���Ӧ������ 
	//extern char g_pDevId[30];
#endif
    
//=======================DriverTimer.c--��ʱ������ģ��===============================================
 struct sTIMER_MANAGER
  {
    unsigned int m_TmCount;
    unsigned char m_TmFlag;
	unsigned int m_TmCountBk;//������һ�����õĶ�ʱ���
  };
#ifdef _DRIVER_TIMER
        struct sTIMER_MANAGER g_sTimer[TIMER_NUM];
        unsigned int Time_10ms_T;
        unsigned int g_DateCount;  //״̬�㱨����������
        unsigned char g_StatusTime;
        unsigned char g_SendStatus;
        unsigned char g_SmsSendStatus; //���Ż㱨״̬��־λ
        unsigned char g_SendFault;
        unsigned int TA1OffsetNum = 250;
        unsigned char Powerdown_counter;
	  unsigned char g_STimeout;//���ʱ 	
#else
        extern struct sTIMER_MANAGER g_sTimer[TIMER_NUM];
        extern unsigned int g_DateCount;
        extern unsigned char g_StatusTime;
        extern unsigned char g_SendStatus;
        extern unsigned char g_SmsSendStatus;
	
        extern unsigned char g_SendFault;
        extern unsigned char g_STimeout;//���ʱ
        extern unsigned char Powerdown_counter;
#endif
//=================================== ʱ��ģ�� ==========================================

  struct sRTC_MANAGER
  {
    unsigned char m_ucRtcSynFlag;   //ϵͳʵʱʱ�Ӵ�ʵʱоƬ�ж�ȡ�ı�־��ÿ5���Ӷ�ȡһ��
    unsigned int m_unRtcSynCount;   //ϵͳͬ�����ʱ���ʱ������5���ӣ���m_ucRtcSynFlag��־
    unsigned int m_unStartCount;		//������ʱ���ڸտ�����3S�ڰ����еȳ���
    unsigned int m_gRealTimer[RTC_NUM]; //ϵͳ�ڲ�ʵʱʱ�ӣ��ꡢ�¡��ա�ʱ���֡��롢����
    unsigned char m_ucCounter125;     ///1.25msʱ�������
    unsigned char m_ucCounter10;     ///10msʱ�������
  };
 
  
#ifdef _DRIVER_RTC
  
  struct sRTC_MANAGER g_sRtcManager;  //ϵͳʵʱʱ��
  unsigned int g_gRealTimerbk[RTC_NUM];
#else
  extern struct sRTC_MANAGER g_sRtcManager;
  extern unsigned int g_gRealTimerbk[RTC_NUM];
  

#endif

//================================== IO����ģ�� ===========================================
#ifdef _DRIVER_IO

	unsigned int g_gExtDinStat[2];	//�ⲿ����������ȷ��״̬����һ��Ϊ��һ���ڣ��ڶ���Ϊ��ǰ����
        unsigned int g_gExtDinYesTimer[3];	//�ⲿDIN����ȷ��ʱ���ʱ
	unsigned int g_gExtDinNoTimer[3];		//�ⲿDIN�������ʱ���ʱ
	unsigned char g_gPhasErrTimer[3];	      //����λ�ô���ȷ��ʱ���ʱ
	unsigned int g_gI0OverTimer=0;		//I0��ʱȷ��ʱ���ʱ
	//unsigned char g_gExtDinNoTimer[3];		//�ⲿDIN�������ʱ���ʱ
        //unsigned int g_gDinRtcLock[2][RTC_NUM];
	
        unsigned char g_gucPhaseSQU[3];  //�洢�������������־��
        unsigned char g_gucPhaseSQUTestNum;  //����������������
        //unsigned int rh_counter=0;//ȼ���ɹ�ң�� ��ʱ������
        unsigned int rh_send_ok=0;//ȼ�����巢����� =0x55 ����8���� =0x85 ������ʧ =0        
        unsigned int fault_time=0;            ///////���ϼ���ֵ
        unsigned char fault_begin;               //���ϳ�����ʱ 
        unsigned char fault_pluse;//������λ
        unsigned char fault_end;                 //���ϻָ���ʱ
        
        unsigned char fault_lubo = 0;               //��������������ʱ
        
        unsigned char abnormal_counter=0;              //////װ���쳣������������5�α��쳣
        
        unsigned char save_abnormal;
        unsigned char fault_save;             ///////���ϴ洢��־,ͬʱ�����Ϸ�����־��0���޹��ϣ�0x33���й���

        unsigned char upload_GPRS;
        unsigned char upload_SMS;              ////ÿ��60���ӷ���һ�����š�
        unsigned int   km_timeout; 
        unsigned int   i0_timeout; 		
	 unsigned int   ptoff_timeout; 	
	 unsigned char soft_latch;	
       // unsigned char SMS_delay;               ////��ʱ������ʱÿ��2���ӷ���һ�����š�      
        unsigned char main_reset_flag = 0;//=0x00;            /////////���帴λ��־
        unsigned char flash_write;                 ////////д¼�����ݵ�FLASH

        unsigned char upload_flag;               ///the of upload                3           2         1           0
        unsigned char upload_interval[4];        ///self upload interval        ����    eight_pulse power_down  abnormal  
        unsigned char upload_interval_set = 3;       ///transmit interval parameter default 3 minutes   the range is 3-15 minutes  
//=============================== �����ⲿ���� ===============================///
        
        unsigned char eight_ctl_flag=0;              ///////8����ң�ر�־,��Ч8�����ң�����5��
        unsigned char power_ctl_flag=0;              ///////�ϵ�ң�ر�־
        unsigned char abnormal_ctl_flag=0;           ///////�쳣ң�ر�־
        
        unsigned char phase_off=0x07;               ////////////�ϵ���Ϊ���ֶ���
        unsigned char phase_copy=0x07;              ////////////�ϵ���Ϊ���ֶ���
        
        unsigned char save_power_off=0;             ////////////�洢�ϵ��־ 
        unsigned char power_off_flag=0;           ///////�ϵ��־
        
        unsigned char save_phase_off=0;          //�洢���߱�־
        unsigned char latch_upload_flag=0;         //////////�������߽������ñ�־�������Ϸ�һ֡״̬֡
        
        unsigned int state_counter = 0;             //�ϴ�ʱ�������������Ӽ�ʱ
        
        unsigned char eight_delay_flag=0;              ////////���Ϻ���ʱ8�뷢8�����־            
        unsigned char eight_delay_counter=0;           ////////���Ϻ���ʱ2�뷢8���������
        
        //unsigned char pulse_counter=0;               ////////�Բɼ�������������ͳ�ƣ��������ݴ洢��λ

        unsigned char comm_rec_flag;

        unsigned char phone_perm[4];	//          5        4         3        2       1      0
						                    //      ���ϸ���  �������� �����ϱ� ��Ч8���� ����  ״̬�쳣
						//�ֻ���Ȩ�޿�����
        unsigned char SMS_array[2];      /////////�ȴ����͵Ĺ��ϣ����������4���ֻ��ţ���24����					
    
        unsigned char send_phase;
        unsigned int yc[8];	// 8����ң��ֵ

        //unsigned char yc1[8];	// 8����ң��ֵ
        unsigned char sign_RSSI_time1[6];              ////year month day hour minute second 
        unsigned char eight_select;                  /////�����ڼ���8����

        //unsigned char yc2[8];	// 8����ң��ֵ
        //unsigned char sign_RSSI_time2[6];              ////year month day hour minute second
        
       // unsigned char bais_check;                    //���ƫ�õ�ѹ


        unsigned char pulse_phase_flag=0;              ////////��һ�෢8����ı�־
        unsigned int efslatch_flag;                   ////////����17���ӱ�־
       // unsigned char eight_delay_counter;           ////////���Ϻ���ʱ2�뷢8���������
        unsigned char chongfa;                       ////////�ط���־ 
        unsigned char moniguzhang;                       ////////ģ�����

        unsigned char phase_check_flag = 0x55;//=0x55;          /////////�����λ�쳣����    0x00 ����⣬  0x55׼�����             
        unsigned char phase_check_time = 30;//=24;            /////////�����Ѿ������     ÿ������Сʱ���һ�Σ��ϵ���һ��   
        unsigned char phase_seq_flag;//=0x00;                 ///////// ��ʼ������Ϊ������   0x00������  0x55 ������

///////////////////////////д��FLASH��


        unsigned char VerNumlcd[2]; 

        unsigned char sign_RSSI_delay; 
	 unsigned char newsms_8pluse; 
	 unsigned char newsms_abn; 
        unsigned char fault_point;                ////////����ȷ���Ƕ�ȡ���Ǹ�¼�����߹��ϼ�¼

        unsigned char fault_recovery_flag;//=0;      ///////���ϸ����־
        unsigned char fault_sms_type;            //////////����״̬�������Ϸ���ʱ�������Զ��ϴ��Ķ��ŵ����

        unsigned char pulse_success;                   ////////���巢�ͳɹ�,����Һ����ʾά��17����
        unsigned char eight_pulse_flag;              ////////��ʼ��8����ı�־

        unsigned char repeat_flag1;               //////////��·���ź�Դ�ط���־����⵽��������35A�����ط���

      //  unsigned char phase_copy;              ////////////�ϵ���Ϊ���ֶ���

 
        unsigned char recieve_confirm_flag;   //  ���ھ������ú���Ӧ��ʽ
        unsigned char Process_68_flag;

        unsigned char process_type=0;     //  ��վ���ն�֮��ͨ��ʱ�� �������� 
       // unsigned char process_step=0;     //  ÿ�ֹ��ܵĴ�����
        unsigned char process_pro=0;      //  ���ܲ����Ӧ�����ȼ�����Ҫ��������������ϴ����Ƚ�
        unsigned char command_flag=0;    // ������������ʱ�� ����ԭ��
        unsigned char command_flag2=0;  // ������������ʱ,����һ�����崦������
        unsigned char fault_num;
      //  unsigned char process_pro;
        unsigned char event_pro;
       // unsigned char self_chongfa_flag;
        unsigned char uart0_event_flag;
        unsigned char g_ucEarthFlg = 0;  //�������ʾ
        //unsigned char g_ucPowerOffFlg = 0;  //�����ʾ
       // unsigned char g_ucPTabnormal = 0;  //��ѹ�쳣��ʾ
        unsigned char g_ucDuanxianFlg = 0;
#else
 	//extern unsigned int g_gDinRtcLock[2][RTC_NUM];
        
        extern unsigned char g_gucPhaseSQU[3];  //�洢�������������־��
        extern unsigned char g_gucPhaseSQUTestNum;  //����������������
        //extern unsigned int rh_counter;//ȼ���ɹ�ң�� ��ʱ������  
        extern unsigned int rh_send_ok;//ȼ�����巢����� =1 ����8���� =0          
        extern unsigned int fault_time;            ///////���ϼ���ֵ
        extern unsigned char fault_begin;               //���ϳ�����ʱ              
        extern unsigned char fault_end;                 //���ϻָ���ʱ
        extern unsigned char fault_pluse;//������λ        
        extern unsigned char fault_lubo;
        
        extern unsigned char abnormal_counter;              //////װ���쳣������������5�α��쳣

        extern unsigned char save_abnormal;
        extern unsigned char fault_save;             ///////���ϴ洢��־,ͬʱ�����Ϸ�����־��0���޹��ϣ�0x33���й���

        extern unsigned char upload_GPRS;
        extern  unsigned char upload_SMS;              ////ÿ��60���ӷ���һ�����š�
        extern unsigned char SMS_delay;               ////��ʱ������ʱÿ��2���ӷ���һ�����š�   
        //extern unsigned int   km_timeout; 
        //extern unsigned int   i0_timeout; 		
	 //extern unsigned int   ptoff_timeout;	
	 extern unsigned char soft_latch;	
        extern unsigned char main_reset_flag;//=0x00;            /////////���帴λ��־
        extern unsigned char flash_write;                 ////////д¼�����ݵ�FLASH

        extern unsigned char upload_flag;               ///the of upload                3           2         1           0
        extern unsigned char upload_interval[4];        ///self upload interval        ����    eight_pulse power_down  abnormal  
        extern unsigned char upload_interval_set;       ///transmit interval parameter default 3 minutes   the range is 3-15 minutes  

//ע:2013-2-14 ����ȫ�ֲ�������,����ͨ��
       // extern unsigned char g_ucNamePARA[44];

//=============================== �����ⲿ���� ===============================///
        
        extern unsigned char eight_ctl_flag;              ///////8����ң�ر�־,��Ч8�����ң�����5��
        extern unsigned char power_ctl_flag;              ///////�ϵ�ң�ر�־
        extern unsigned char abnormal_ctl_flag;           ///////�쳣ң�ر�־
        
        extern unsigned char phase_off;               ////////////�ϵ���Ϊ���ֶ���
        extern unsigned char phase_copy;              ////////////�ϵ���Ϊ���ֶ���
        
        extern unsigned char save_power_off;             ////////////�洢�ϵ��־ 
        extern unsigned char power_off_flag;           ///////�ϵ��־
        
        extern unsigned char save_phase_off;          //�洢���߱�־
        extern unsigned char latch_upload_flag;         //////////�������߽������ñ�־�������Ϸ�һ֡״̬֡
        
        extern unsigned int state_counter;             //�ϴ�ʱ�������������Ӽ�ʱ
        
        extern unsigned char eight_delay_flag;              ////////���Ϻ���ʱ8�뷢8�����־            
        extern unsigned char eight_delay_counter;           ////////���Ϻ���ʱ2�뷢8���������
        
        //extern unsigned char pulse_counter;               ////////�Բɼ�������������ͳ�ƣ��������ݴ洢��λ
       
        extern unsigned char comm_rec_flag;
        extern unsigned char phone_perm[4];	//          5        4         3        2       1      0						                    //      ���ϸ���  �������� �����ϱ� ��Ч8���� ����  ״̬��						//�ֻ���Ȩ�޿�����
        extern unsigned char SMS_array[2];
     
        extern unsigned char send_phase;
        extern unsigned int yc[8];	// 8����ң��ֵ

        //extern unsigned char yc1[8];	// 8����ң��ֵ
        extern unsigned char sign_RSSI_time1[6];              ////year month day hour minute second 
        extern unsigned char eight_select;                  /////�����ڼ���8����

        //extern unsigned char yc2[8];	// 8����ң��ֵ
        //extern unsigned char sign_RSSI_time2[6];              ////year month day hour minute second

        extern unsigned char pulse_phase_flag;              ////////��һ�෢8����ı�־
        extern unsigned int efslatch_flag;                   ////////����17���ӱ�־
        extern unsigned char eight_delay_counter;           ////////���Ϻ���ʱ2�뷢8���������
        extern unsigned char chongfa;                       ////////�ط���־ 
        extern unsigned char moniguzhang;                       ////////ģ�����
        extern unsigned char phase_check_flag;//=0x55;          /////////�����λ�쳣����    0x00 ����⣬  0x55׼�����             
        extern unsigned char phase_check_time;//=24;            /////////�����Ѿ������     ÿ������Сʱ���һ�Σ��ϵ���һ��   
        extern unsigned char phase_seq_flag;//=0x00;                 ///////// ��ʼ������Ϊ������   0x00������  0x55 ������

       // extern unsigned char lcd_ctl;                       /////////Һ���Ƿ��ڷ�������


///////////////////////////д��FLASH��
       // extern unsigned char fault_delay_time;        //������ʱʱ��    
        //extern unsigned int cal_value[5];

        extern unsigned char VerNumlcd[2]; 


        extern unsigned char sign_RSSI_delay; 
	 extern unsigned char newsms_8pluse; 
         extern unsigned char newsms_abn; 
        //extern unsigned char fault_point;                ////////����ȷ���Ƕ�ȡ���Ǹ�¼�����߹��ϼ�¼

        extern unsigned char fault_recovery_flag;//=0;      ///////���ϸ����־
        extern unsigned char fault_sms_type;            //////////����״̬�������Ϸ���ʱ�������Զ��ϴ��Ķ��ŵ����

        extern unsigned char pulse_success;                   ////////���巢�ͳɹ�,����Һ����ʾά��17����
        extern unsigned char eight_pulse_flag;              ////////��ʼ��8����ı�־


        extern unsigned char repeat_flag1;               //////////��·���ź�Դ�ط���־����⵽��������35A�����ط���

        extern unsigned char phase_copy;              ////////////�ϵ���Ϊ���ֶ���

 
        extern unsigned char recieve_confirm_flag;   //  ���ھ������ú���Ӧ��ʽ
        extern unsigned char Process_68_flag;
       //extern  unsigned char Note_Flag;

       extern  unsigned char process_type;     //  ��վ���ն�֮��ͨ��ʱ�� �������� 
        //extern unsigned char process_step;     //  ÿ�ֹ��ܵĴ�����
        extern unsigned char process_pro;      //  ���ܲ����Ӧ�����ȼ�����Ҫ��������������ϴ����Ƚ�
       extern  unsigned char command_flag;    // ������������ʱ�� ����ԭ��
       extern  unsigned char command_flag2;  // ������������ʱ,����һ�����崦������
       extern  unsigned char fault_num;
        extern unsigned char process_pro;
       extern  unsigned char event_pro;
      //extern  unsigned char self_chongfa_flag;
       extern  unsigned char uart0_event_flag;
       extern  unsigned char g_ucEarthFlg;  //�������ʾ
       //extern  unsigned char g_ucPowerOffFlg;  //�����ʾ
      // extern  unsigned char g_ucPTabnormal;  //��ѹ�쳣��ʾ
        extern unsigned char g_ucDuanxianFlg;
#endif


#ifdef __cplusplus
}
#endif

#endif  //end of file
