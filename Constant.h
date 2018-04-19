//��MSP430�е�Long�͵������ǵ�����ǰ�������ں�

#ifndef _CONSTANT_H
#define _CONSTANT_H 

#define DEBUG
#define GETSOEFROMRAM     //���͵�SOE���ⲿFLASHȡ�����Ǵ��ڲ�ȡ��
//#define ZT_SOE_IN_FLASH //SOE���ڲ�FLASH
//#define ZDSENDADDR //����������վ��ʱ��������վ��ַ(����Ӧ��) 
//#define YN_101S      //����=����101��Լ��������=��׼101��Լ
//#define CQ_101S      //����=����101��Լ��������=��׼101��Լ
//#define INT_NEST      //�ŏ|�����ж�Ƕ��       0//1=GPRS;0=SMS
//#define DH_101S      //����=����껲�101��Լ��������=��׼101��Լ
#define SD_101S      //����=����껲�101��Լ��������=��׼101��Լ

#define CONDIN_3      //�Ӵ������3·�����ź�  ȡ���ź�Դ�쳣ң��(������������)


#define         TYPE_EQU                0x1111  //�豸����

#define        INVALIDYC  0xD8F0//-9999
#define        INVALTEYC  0xFF9D//-99
#define        INITYC     0xDD47//-8888
#define        ODUINITYC     0xFFFF//ODU��ʼ��ֵ


 
#define         YES                     0x55
#define         NO                      0xAA
#define         ON                      0x5F
#define         OFF                     0x50
#define         CLOSE                   0x55
#define         TRIP                    0xAA

#define         LuboType_XH      0x55
#define         LuboType_ACT      0xAA

#define  OPERATING_LOGO1 1 //������ʶ1 ��Ŀ¼
#define  OPERATING_LOGO2 2//������ʶ2 ��Ŀ¼ȷ��
#define  OPERATING_LOGO3 3//���ļ�����
#define  OPERATING_LOGO4 4 //���ļ�����ȷ��
#define  OPERATING_LOGO5 5 //���ļ�������Ӧ
#define  OPERATING_LOGO6 6 //���ļ�������Ӧȷ��

/*-------------------------------------------------------------------------
 *   Standard Bits
 *-------------------------------------------------------------------------*/
#define NBIT0                (0xFFFE)
#define NBIT1                (0xFFFD)
#define NBIT2                (0xFFFB)
#define NBIT3                (0xFFF7)
#define NBIT4                (0xFFEF)
#define NBIT5                (0xFFDF)
#define NBIT6                (0xFFBF)
#define NBIT7                (0xFF7F)
#define NBIT8                (0xFEFF)
#define NBIT9                (0xFDFF)
#define NBITA                (0xFBFF)
#define NBITB                (0xF7FF)
#define NBITC                (0xEFFF)
#define NBITD                (0xDFFF)
#define NBITE                (0xBFFF)
#define NBITF                (0x7FFF)

#define KJa1                (P4IN&0x08)//a�Ӵ�������λ���ź�
#define KJb1                (P4IN&0x10)//
#define KJc1                (P4IN&0x20)//

//===================�������Ͷ���============================
typedef unsigned char  INT8U;       //�޷���8λ��
typedef          char  INT8S;       //�з���8λ��
typedef unsigned int   INT16U;      //�޷���16λ��
typedef signed   int   INT16S;      //�з���16λ��
typedef unsigned long  INT32U;      //�޷���32λ��
typedef signed   long  INT32S;      //�з���32λ��
typedef float          FP32;        //�����ȸ�����
typedef double         FP64;            //˫���ȸ�����

#define  GETBIT(p,i)   ((p[(i>>4)] & BIT[(i&0x0F)]) && 1)
#define  SETBIT(p,i,v) p[(i>>4)] = (v) ? (p[(i>>4)]|BIT[(i&0x0F)]) : (p[(i>>4)]&~BIT[(i&0x0F)])

#define MAX_ENC_FRAME_LEN                    384
//==============================IEC 101��ز���======================================
#define GPRS_SMS      //       0//1  =GPRS;0=SMS

#define IECP_LINKADDR_NUM    0//��·��ַ���� 1-2    
#define IECP_TRANSRSN_NUM    1//����ԭ�򳤶� 1-2   
#define IECP_COMNADDR_NUM    2 //������ַ���� 1-2  
#define IECP_INFO_NUM        3//��Ϣ���ַ���� 1-3 
#define IECP_COM_MODE        4//0Ϊ��ƽ��ʱ��1Ϊƽ��ʽ  Ĭ��Ϊ1
#define IECP_AUT_MODE        5//�����ϴ�ģʽ��0=��վ���ͳ�ʼ���󷽿������ϴ�������
                              //1=�յ���վ�κ�����󷽿������ϴ����κ�ʱ����Ӧ�������
                              //2=�����ϴ������κ�Լ��
#define IECP_OBJ_COM0        6 //���Դ��ڲ����ʣ�0=9600,1=19200,2=38400,3=57600,4=115200
#define IECP_OBJ_COM1        7
#define IECP_OBJ_COM2        8
#define IECP_OBJ_COM3        9

#define IECP_CRC             10//crcУ��

#define IEC_PARA_NUM    11  //IEC���еĲ�������

#define IEC_YC_NUM   16  //��������վ��ң���������
#define IEC_YX_NUM   64          //RMT_INFO_NUM��������վ��ң�Ÿ�������
#define IEC_YK_NUM  OUT_TYPE_NUM  //��������վ��ң�ظ�������
#define FRM_MAX_COS_NUM   70 //ÿ֡������ഫ��COS�������涨Ϊ70��
#define IEC_ZJ_NUM   3  //��������վ���Լ��������
//˫��
#define RI_ZD_DYX1  		512

//#define RMT_INFO_NUM           524         //��ң�Ÿ���
#define RMT_WORD_NUM           RMT_INFO_NUM/8+1          //��ң��ռ��WORD���� ÿ��ң��ռ2��bit��һ��WORD����8��ң��
#define RMT_WDYX_NUM            2                        // 12 ˫��ң��ռ���� 12/8+1
#define RMT_WSYX_NUM            1          //RMT_INFO_NUM - 12 ����ң���ָ��� (RMT_INFO_NUM - 12)/8 +1

#define RMT_ZSQST_NUM          LINE_NUM * PHASE_NUM    //��·ͨ��״̬����  12*4
#define RI_ZSQST_H             RI_12O_ZSQST           //ָʾ��״̬ң������
#define DYX_ADDR               RI_ZD_DYX1             //˫��ң����ʼ��
#define RI_FTINFO_NUM           LINE_NUM * PHASE_NUM * FTTPYE_NUM//��Ҫ������ң�Ÿ��� ��·(5*12)+�ӵ�(5*12)+�¶�(5*12)

//============================  AD�ɼ�  =============================================
#define AC_AD_CHAN_NUM                          6            //����AD����ͨ�������������**����1·�����ѹ  ����һͨ���Բ��ѹUX
#define AC_SAMPLE_DOTS                          16      //ÿ�ܲ������ĵ���Ϊ32��
#define AC_BUFF_LEN                             32             //�����ɼ�����������2���ܲ���32����

#define AC_SAM_TAIL_OFFSET                      2               //�����ɼ�����ƫ��  ����Ŀǰ�������󱨵�����������ݲɼ������������͵�CPU�ɼ�

//����ADC����ͨ���Ŷ���
#define CHAN_UA                               0
#define CHAN_UB                               1
#define CHAN_UC                               2
#define CHAN_U0                               3
#define CHAN_I0                               4
#define CHAN_Upt                               5

//============================ ������ʷ��¼  =============================================
#define FLOADINFONUM                    4           //
#define FLOAD_TOTALNUM                0            //��¼������
#define FLOAD_NEW                          1             //�����洢�ļ�¼���һ����¼��ƫ��λ��(�����Ѵ洢������һ����¼��λ��)
#define FLOAD_OLD                           2             //����һ����¼��λ��
#define FLOAD_CS                             3
#define FLOAD_DAY 3
#define FLASH_LOAD_MAXNUM        8192
#define FADDR_LOAD_START           0x750000//750000-790000
#define FLASH_PLOAD_LEN              32//ÿ�����ɼ�¼32�ֽ�



//============================  SMS-�绰����        =======================================

#define PHONE_CHARNUM         16//15     //ÿ���绰����ռ���ֽ���/�ַ��� ����ַ���//�ŏ| 0328 ( 1�ֽڵ绰���볤�� +15�ֽڵ绰)
//#define PHONE_NUM             4//3      //֧�ֵĵ绰�������//�ŏ| 0328 (N���Ϊ8��Ŀǰ������Ϊ4��)

#define PHONE_ENCTL           0 //�绰����ʹ�ܿ�����
#define PHONE1_CHARLEN        1 //����1����
#define PHONE1                2
#define PHONE2_CHARLEN        PHONE1 + PHONE_CHARNUM
#define PHONE2                PHONE2_CHARLEN + 1
#define PHONE3_CHARLEN        PHONE2 + PHONE_CHARNUM
#define PHONE3                PHONE3_CHARLEN + 1

#define PHONE_CS              68////�ŏ| 0328 ��68�ֽ�, 4���绰����PHONE3 + PHONE_CHARNUM         
#define PHONE_PA_NUM          69//PHONE_CS + 1  //�ŏ| 0328 ��69�ֽ�, 4���绰����+4BYTE������+1BYTE CSЧ��

//N(N���Ϊ8��Ŀǰ������Ϊ4��)���绰����( 1�ֽڵ绰���볤�� +15�ֽڵ绰)��N�ֽڶ��ŷ��Ϳ�����//�ŏ| 0328 
//��136�ֽ�//�ŏ| 0328
#define MAX_SMS_NUM 16
#define CnName_NUM   20//�ŏ| �����ú���վ����ַ
#define LBName_NUM   20//�ŏ| ¼���ļ���Ӣ��վ����ַ
//============================  ¼������        =======================================
#define  REC_AC_CHAN_NUM        5               //¼��������ѹ������ͨ����������������������Ҫ������������������

#define RECAC_UA                               0
#define RECAC_UB                               1
#define RECAC_UC                               2
#define RECAC_U0                               3
#define RECAC_I0                               4
#define REC_AC_BEF_LEN          128              //����֮ǰ����3���ܲ�������
#define REC_AC_LEN              640             //�����󱣴�5���ܲ������ݣ�����ǰ3���ܲ������ݣ���8�ܲ�   

#define         REC_DATA_LEN            10       //ÿ��¼�����ݵĳ���

//¼�����ݵĶ��� 
#define         REC_MSL           0 
#define         REC_MSH           1
#define         REC_MINU          2 
#define         REC_HOUR          3
#define         REC_DAY           4
#define         REC_MONTH         5  
#define         REC_YEAR          6 
#define         REC_TYPE          7
#define         REC_TOTAL_LENL     8
#define         REC_TOTAL_LENH     9

//#define         FADDR_REC_START    0   //Flash��ַ
//============================  SST25 FLASH��ַ����        =======================================
#define         FADDR_REC_START    20480   //¼������Flash��ַ��ʼ��ַ//�ŏ| SOE��FLASH
#define         REC_LEAF_NUM       16   //ÿ��¼��ҳ�� ռ16ҳ 16*256
#define         MAX_REC_NUM       50 //¼�����ļ����32��  
#define         MAX_ACTREC_NUM       10 //¼�����ļ����32��  
#define         MAX_XHREC_NUM       10 //¼�����ļ����32��  
#define         MAX_ALLREC_NUM       73 //ȫ��¼�����ļ����73��MAX_REC_NUM+1+MAX_ACTREC_NUM+1+MAX_XHREC_NUM+1 

#define         FADDR_SOE_START    0   //SOE����Flash��ַ��ʼ��ַ//�ŏ| SOE��FLASH
#define         MAX_SOE_NUM       512   //SOE����512����ÿ��11�ֽڣ�ռ16�ֽ�//�ŏ| SOE��E2ROM
#define         MAX_SOE_BYTE       0x3000 //768*16   //SOE����1024����ÿ��11�ֽڣ�ռ16�ֽ�//�ŏ| SOE��FLASH

//#define         REC_LEAF_NUM      9//8       //¼������ռ��FLASHҳ�� ���ڼ�������ͨ��UA,UB��UC�����Զ�ռ��һҳ
//#define         REC_DISPLAY_NUM      512  //��ȡ����ʾ��¼������
//������EEPROM�еĲ������±�־
#define PARA_NUM        7

#define CHNG_INFO       0       //������Ϣ
#define CHNG_ADJ        1       //У����Ϣ
#define CHNG_PC         2       //������ֵ
#define CHNG_MUBIAO     3       //������ֵ�����
#define CHNG_TEL        4       //���в���
#define CHNG_GPRS       5       //GPRS����
#define CHNG_CODE       6       //Һ����������

//=================    ��Ϣ�����ַ���壬�ο�101Э�飬���ModbusЭ�� =============================
/*#define         EQU_BASE                                0x0001          //״̬����Ϣ��ʼ��ַ
#define         PC_BASE                                 0x1001          //������ֵ��ʼ��ַ
#define         RI_BASE                                 0x2001          //ң������Ϣ��ʼ��ַ
#define         RM_BASE                                 0x4001          //ң������ʼ��ַ
#define         ADJ_BASE                                0x5001          //У��������ʼ��ַ
#define         RC_BASE                                 0x6000          //ң����ʼ��ַ
#define         RP_BASE                                 0x7000          //���в�����ʼ��ַ
#define         RUN_BASE                                0x7200          //������Ϣ��ʼ��ַ*/



//============================  ң����          =======================================
#define RMT_MEAS_NUM            10


#define RM_U0                   0
#define RM_UA                   1
#define RM_UB                   2
#define RM_UC                   3
#define RM_UAB                 4       //�����ѹ
#define RM_UBC                 5
#define RM_UCA                 6
#define RM_I0                   	7       //�������
#define RM_ACT_NUM    	8
#define RM_UPt                	9

#define RM_CSQ                	10
#define RM_UCAP               	11
#define RM_bk1               	12
#define RM_bk2                	13
#define RM_bk3                	14
#define RM_bk4                	15

#define RM_FILTER_NUM           10      //ң�������˲����������
/*
#define RMT_INFO_NUM            14

#define YX_A_ACTION             BIT0
#define YX_B_ACTION             BIT1
#define YX_C_ACTION             BIT2
#define YX_EFS_OFF              BIT3

#define YX_EFS_ABNORMAL         BIT4
#define YX_EFS_LATCH            BIT5 
#define YX_EIGHT_PULSE          BIT6
#define YX_PHASEA_FAULT         BIT7 

#define YX_PHASEB_FAULT         BIT8 
#define YX_PHASEC_FAULT         BIT9
#define YX_EARTH_FAULT          BITA
#define YX_PHASEA_OFF           BITB 

#define YX_PHASEB_OFF           BITC 
#define YX_PHASEC_OFF           BITD 
*/
#define RMT_INFO_NUM            64//42

#define YX_FUN_ON               0  //����Ͷ��
#define YX_MANUAL_ACTION        1  //�ֶ�Ͷ��
#define YX_ACTION_SUCCESS       2 //����Ͷ�гɹ�
#define YX_EARTH_FAULT          3  //�ӵظ澯
#define YX_PHASE_OFF            4  //PT���߸澯

#define YX_EFS_OFF              5  //������ʧ��
#define YX_U0_HIGH             6 //�����ѹ����
#define YX_UAB_HIGH             7 //�ߵ�ѹ��ѹ����
#define YX_U_HIGH              8  //���ѹ��ѹ�澯

#define YX_U_LOW               9 //���ѹǷѹ�澯
#define YX_EFS_ABNORMAL         10//�������쳣�澯
#define YX_PHASEA_ACT           11 //A��Ͷ��
#define YX_PHASEB_ACT           12  //B��Ͷ��
#define YX_PHASEC_ACT           13 //C����Ͷ��
//#define YX_PHASEC_OFF           BITD  //����Ͷ�гɹ�
#define YX_EFS_LATCH            14//���� 1=���� 0=����
#define YX_UA_HIGH              15  //A���ѹ��ѹ�澯
#define YX_UA_LOW               16 //A���ѹǷѹ�澯
#define YX_UB_HIGH              17  //B���ѹ��ѹ�澯
#define YX_UB_LOW               18  //B���ѹǷѹ�澯
#define YX_UC_HIGH              19  //C���ѹ��ѹ�澯
#define YX_UC_LOW               20  //C���ѹǷѹ�澯
#define YX_PHASEA_OFF       21  //PT���߸澯
#define YX_PHASEB_OFF       22  //PT���߸澯
#define YX_PHASEC_OFF       23  //PT���߸澯
#define YX_AEARTH_FAULT          24  //�ӵظ澯
#define YX_BEARTH_FAULT          25 //�ӵظ澯
#define YX_CEARTH_FAULT          26 //�ӵظ澯
#define YX_KJC         10 //Ӳң��1
#define YX_KJB          32 //Ӳң��1
#define YX_KJA          33 //Ӳң��2
#define YX_RH_SUCCESS          34 //ȼ���ɹ�
#define YX_RH_FAIL         35 //ȼ��ʧ��
#define YX_BREAK            36 //����
#define YX_LBOK              37 //¼�����
#define YX_SBP_OFF        38 //�󱸵�Դstand-by power 0���󱸵�Դ������1���󱸵�Դ�쳣
#define YX_RH_ACT          39//װ��Ϩ������	1��Ϩ������Ͷ�룬0��Ϩ������Ͷ�����
#define YX_EFS_ACT        40//װ�������ź�Ͷ��	1������Ͷ�й���״̬��0��װ�ÿ���Ͷ�й���״̬����
#define YX_I0_ACT           41//�ӵص�������	   1��I0>0 ��0��I0=0
#define YX_PHASEA_ERR  42//A�࿪��λ���쳣	1���쳣״̬��0��
#define YX_PHASEB_ERR  43//B�࿪��λ���쳣	1���쳣״̬��0��
#define YX_PHASEC_ERR  44//C�࿪��λ���쳣	1���쳣״̬��0��
#define YX_I0_HIGH         45//I0����	1������״̬��0��
#define YX_KJ_TIMEOVER 46//���رպϳ�ʱ	1����ʱ��0��
#define YX_I0_TIMEOVER 47//I0��ʱ	1����ʱ��0��
#define YX_SYSRESET 63//ϵͳ��λ
//#define YX_I0_HIGH         48//I0����	1������״̬��0��
//#define YX_I0_HIGH         49//I0����	1������״̬��0��

//������Ϊ5Aʱ������1��С���㣬��ʵ��ֵΪ50���򻥸������˷ź����ADΪ0.8V����Ӧ�Ĳ���ֵΪ993  1311
#define COEF_AD_I_0                       1562     //5.00/993*4096 =  2062
//#define COEF_AD_I_0                     1100       //5.00/1862*4096 = 
#define COEF_I_0_AD2                    39110    //��������ķ���ϵ����ƽ��,�ɲ���Ϊ0.01A��ʵ��ֵ����ɲ���ֵ��ϵ����ƽ�� ����Q10����Ҫ�ڳ�����������λ10 1.986*1.986*1024*10000/225

//��������ѹ����ϵ������

#define COEF_AD_U                        33328  //��ѹϵ��44000   100.0V/931*4096    100.0V���������˷ź����ADΪ0.750V����Ӧ�Ĳ���ֵΪ931 1229
#define COEF_U_AD2                      677    //2741=1.157*1.157*2048  0.5748*0.5748*2048= 677

//============================  ��������        =======================================
#define PROC_MEAS_NUM           8


#define PM_U0                   0
#define PM_UA                   1
#define PM_UB                   2
#define PM_UC                   3
#define PM_UAB                  4       //�����ѹ
#define PM_UBC                  5
#define PM_UCA                  6
#define PM_I0                   7       //�������
//============================  LOG��־        =======================================

#define LOG_INFO_NUM            64//42

#define LOG_RESET               1  //����Ͷ��
#define LOG_101_LINK        	2  //�ֶ�Ͷ��
#define LOG_101_ERR        		3 //����Ͷ�гɹ�
#define LOG_SELF_CHEK          	4  //�ӵظ澯
#define LOG_SOFT_ERR           	5  //PT���߸澯
#define LOG_POW_ONF            	6  //������ʧ��
#define LOG_UCAP              	7 //�����ѹ����
#define LOG_PAR_CHAG          	8 //�ߵ�ѹ��ѹ����
#define LOG_UPT_ERR           	9  //���ѹ��ѹ�澯
#define LOG_EARTH        		10 //����Ͷ�гɹ�
#define LOG_EARTH_TDELAY        11  //�ӵظ澯
#define LOG_8FULS_STA           12  //PT���߸澯
#define LOG_8FULS_END           13  //������ʧ��
#define LOG_LATCH               14 //�����ѹ����
#define LOG_I0_ERR             	15 //�ߵ�ѹ��ѹ����
#define LOG_KM_ERR              16  //���ѹ��ѹ�澯
#define LOG_BREAK              	17  //���ѹ��ѹ�澯

//============================  У������        =======================================
#define ADJ_PARA_NUM                            7//13   //У����������ÿ��ͨ���Ļ�׼ԴУ����Ŀ��ֵУ���������й��ʼ���Ļ�������λУ��



#define ADJ_REF_CHNUA                         0       //ͨ��5��׼ԴУ��ֵ 
#define ADJ_REF_CHNUB                         1       //ͨ��6��׼ԴУ��ֵ  
#define ADJ_REF_CHNUC                         2       //ͨ��7��׼ԴУ��ֵ  
#define ADJ_REF_CHNU0                         3       //ͨ��4��׼ԴУ��ֵ  
#define ADJ_REF_CHNI0                         4       //ͨ��4��׼ԴУ��ֵ  
#define ADJ_REF_CHNUPt                       5       //ͨ��4��׼ԴУ��ֵ  

#define ADJ_CRC                               6

//============================  ���в���        =======================================

#define RP_CFG_KEY               0      //���ÿ�����1

#define RP_BEAT_T                1    //��ʱ�ϴ����� ����1S
#define RP_SENDYC_T              2    //��ʱ�ϴ�ң��ʱ����
#define RP_SENDYX_T               3   //��ʱ�ϴ�ң��ʱ����
//#define RP_STATUSRP_INTERVAL     4   //״̬�㱨���
#define RP_FLOAD_T 			 4   //���ɼ�¼�洢ʱ����
#define RP_COMM_ADDR        	 5   //��վվ��ַ 
#define RP_SYX_INFADDR    	 6    //����ң����ʼ��Ϣ���ַ
#define RP_DYX_INFADDR     	7    //˫��ң����ʼ��Ϣ��?
#define RP_YCLIMIT              	 8   //ң�����Խ��ֵ
#define RP_YCCAP                	 9   //ң�����Խ��ֵ ���ָ�ٷֱ�
#define RP_PLUSE_MODFK	   	10  //������������ʵ��������Ϊ10msΪ RP_PLUSE_TIME -RP_PLUSE_MODFK ��λ10ms Ĭ��0
#define RP_PLUSEXH_MODFK	11  //������������ʵ��������Ϊ10msΪ RP_PLUSE_TIME -RP_PLUSE_MODFK ��λ10ms 
//#define RP_DEVICE_TYPE		11     //�豸���� 0 = һң�� 1 = ��ң ��2=�ź�Դ
#define RP_YXRSD_T               12   //ң���ط��������
#define YX_delay                 13   //����ʱ����

#define RP_UA_ADJ           14       //UAУ׼ϵ��ֵ
#define RP_UB_ADJ           15       //UBУ׼ϵ��ֵ
#define RP_UC_ADJ           16               //UCУ׼ϵ��ֵ
#define RP_U0_ADJ           17               //U0У׼ϵ��ֵ
#define RP_I0_ADJ           18               //I0У׼ϵ��ֵ
#define RP_I0_START         19              //�ߵ�ѹ��ѹ�澯��ֵ //��������ż�ֵ
#define RP_HIGH_P           20               //���ѹ�߶�ֵ
#define RP_LOW_P            21               //���ѹ�Ͷ�ֵ
#define RP_HIGH_Z           22         //�����ѹ�߶�ֵ
#define RP_LOW_Z            23          //�����ѹ�Ͷ�ֵ
#define RP_NO_V            24      //��ѹ�ż�ֵ
#define RP_T_DELAY         25              //������ʱʱ��
#define RP_JAG_P           26      //���оݱ�־
#define RP_JAG_Z           27      //�����оݱ�־
#define RP_EFS_MODEL       28      //�ź�Դ�ͺű�ʶ
#define RP_JAG_ACT         29      //���������о�
#define RP_PULSE_VALID     30      //8������Ч��ֵ
#define RP_GPRS_MODEL      31      //GPRS��ʽ��ֵ
#define RP_REV_CURRENT     32      //��ת������ֵ
#define RP_U0_CAL          33      //U0������ʽ
/*
#define RP_ETH_MODEL            20      //��̫����ʽ 0��SERVER   1��CLIENT
#define	RP_G_IP_1               21		//����IP1~4
#define	RP_G_IP_2		22
#define	RP_G_IP_3		23
#define	RP_G_IP_4		24
#define	RP_SUB_MASK_1		25       //��������1~4
#define	RP_SUB_MASK_2		26
#define	RP_SUB_MASK_3		27
#define	RP_SUB_MASK_4		28
#define	RP_MAC_ADDR_1		29       //W5500 MAC��ַ1~6
#define	RP_MAC_ADDR_2		30
#define	RP_MAC_ADDR_3		31
#define	RP_MAC_ADDR_4		32
#define	RP_MAC_ADDR_5		33
#define	RP_MAC_ADDR_6		34
#define	RP_W_IP_1		35        //W5500IP1~4
#define	RP_W_IP_2		36
#define	RP_W_IP_3		37
#define	RP_W_IP_4		38
#define	RP_D_IP_1		39        //W5500IP1~4
#define	RP_D_IP_2		40
#define	RP_D_IP_3		41
#define	RP_D_IP_4		42
#define	RP_S0_PORT		43        //Socket0 �˿ں� 
*/
#define RP_CNL_MODEL	        34		//�ӵع���Ͷ��
#define RP_LACTH_TIME           35//�ź�Դ����ʱ�䣬�Է���Ϊ��λ��Ĭ��18����
#define RP_PLUSE_TIME           36//�����ȣ�10msΪ��λ Ĭ��25
#define RP_SEND_TIME1           37//���巢�ͼ��ʱ��1 ��10msΪ��λ Ĭ��100
#define RP_SEND_TIME2           38//���巢�ͼ��ʱ��2 ��10msΪ��λ Ĭ��125

#define RP_PLUSE_NUM       39//���������6 ��7��8
#define RP_OVERLOAD_I            40
#define RP_OVERLOAD_T           41
#define RP_SENDSMS_T             42//�ŏ| ң����ʼ��ַ�޸����в���
#define RP_YC_INFADDR            43//�ŏ| ң����ʼ��ַ�޸����в���
#define RP_SENDZJ_T                    44      //��ʱ�ϴ��Լ�ʱ����
#define RP_RHSEND_TIME1           45//ȼ�����巢�ͼ��ʱ��1 ��10msΪ��λ Ĭ��500
#define RP_RHPLUSE_TIME2           46//ȼ�������ȣ�10msΪ��λ Ĭ��20
#define RP_RHT_DELAY           47//ȼ��ң����ʱʱ��
#define RP_CT_TRANS           48//ct���
#define RP_KM_TIME           49//�Ӵ������رպϳ�ʱʱ�䣬10msΪ��λ Ĭ��100
#define RP_PLUSE_AMODFK	   	50  //������������ʵ��������Ϊ10msΪ RP_PLUSE_TIME -RP_PLUSE_MODFK ��λ10ms Ĭ��0
#define RP_PLUSE_BMODFK	   	51  //������������ʵ��������Ϊ10msΪ RP_PLUSE_TIME -RP_PLUSE_MODFK ��λ10ms Ĭ��0
#define RP_PLUSE_CMODFK	   	52  //������������ʵ��������Ϊ10msΪ RP_PLUSE_TIME -RP_PLUSE_MODFK ��λ10ms Ĭ��0

#define RP_CRC                   53      //crcУ����//�ŏ| ң����ʼ��ַ�޸����в���
#define RUN_PARA_NUM             54//�ŏ| ң����ʼ��ַ�޸����в���

//�����־��嶨��
#define RPCFG_CON_NC              0      //�Ӵ���λ��0=����no 1=����nc
//AD�ɼ������ϴ�ȫң��
#define RPCFG_SENDLUBOML         1 //���� �Ƿ������ϴ�¼��Ŀ¼
#define RPCFG_SEND_FTYC             2      //��ʼ�����̷������� 1=������0=����
#define RPCFG_SEND_CHANGEYC         3     //�Ƿ���ң��Խ��ֵ 0=���� 1=�ϴ�
#define RPCFG_YC_FLOAT             4      //ң��ֵΪ������ 0=���� 1=������(���ͱ�ʾ)
#define RPCFG_AUTOLINK    5     //�Ƿ�1=�ϵ��GPRS�Զ�����101
#define RPCFG_DEL_LUBO              6      //�Ƿ�ɾ��¼������ 
#define RPCFG_ISSENDINITFIN         7     //0=�����ͳ�ʼ����֡��1=���� Ĭ�Ϸ���
#define RP_COMTRADE_TYPE             8     //0=¼������ΪASCII,1=¼�����ݶ����ƣ�
#define RPCFG_PT_LATCH                        9     // 1=����PT���߱�����
#define RPCFG_I0_LATCH                        10     // 1=����I0��������
#define RPCFG_CON_LATCH                     11     // 1=���ÿ��رպϳ�ʱ����?#define RPCFG_SEND_RHPLUSE         12     //0=������ȼ�����壬1=����ȼ������ 
#define RPCFG_BREAK_STOP8PUL     13     //0=���ߺ�ֹͣ8���壬1=���ߺ�ֹͣ8���� 
#define RPCFG_CURRENT_PRIMARY   14     //0=����Ϊ2��ֵ��1=����Ϊ1��ֵ��2��ֵ*CT���
#define RPCFG_ENCRYPT			  15// 1=����

//================================  ��ʱ������  ==========================

#define TIMER_NUM            19  //��ʱ������ 1sΪ��λ
#define TM_BEAT               0     //��ʱ�ϴ����� GPRSͨ������ʱ�� ����1S
#define TM_ADCOLT             1   //AD�ɼ�ʱ���� ��ʱ��AD�ɼ�λ��1s��ʹ��AD�ɼ�
#define TM_SENDYC             2   //��ʱ�ϴ�ң��ʱ����
#define TM_CLOSECHARGE        3   //�رճ��ʱ��
#define TM_GPRSPWOERDOWN      4   //GPRS����ʱ�䣬����׼ʵʱ����ģʽ
#define TM_SAVELOAD           5   //���渺�ɵ���
#define TM_ODU_call2          6   //�ٻ�ODU�Ķ�������
#define TM_ODU_call           7   //�ٻ�ODU����
#define TM_BATPWOERDOWN       8   //﮵�عض�ʱ��
#define TM_SMSSENDALLOW       9  //���ŷ����ӳ�
#define TM_WAITXINHAO         10  //���ŷ������ź�Դ�ĵȴ�ʱ��
#define TM_ADBAT              11  //﮵�ز���ʱ��?#define TM_ADBATTEN      11  //﮵�ص�ʹ��A�Ĳ���
#define TM_Volt               12  //��ѹ����ʱ������Դģʽ��ʱ����
#define TM_SENDSTATUSDELAY    13  //״̬�㱨��ʱ
#define TM_SENDADDR           14  //��վ��ַ������ʱ
#define TM_SENDFAULTDELAY     15  //�������������ӳ� ����������ģʽ
#define TM_SENDSTATE          16  // ����ODU��״̬�㱨����
#define TM_SNTP_TIME          17  //��ʱSNTP��ʱ���
#define TM_GPRS_RW            18  //GPRS��д��ʱ
                                    

//============================  У׼ϵ��������       =======================================
#define PROC_CNT_NUM            28

#define PC_UA_ADJ           0       //UAУ׼ϵ��ֵ
#define PC_UB_ADJ           1       //UBУ׼ϵ��ֵ
#define PC_UC_ADJ           2               //UCУ׼ϵ��ֵ
#define PC_U0_ADJ           3               //U0У׼ϵ��ֵ
#define PC_I0_ADJ           4               //I0У׼ϵ��ֵ
#define PC_I0_START         5               //�ߵ�ѹ�߶�ֵ//��������ż�ֵ
#define PC_HIGH_P           6               //���ѹ�߶�ֵ
#define PC_LOW_P            7               //���ѹ�Ͷ�ֵ
#define PC_HIGH_Z           8         //�����ѹ�߶�ֵ
#define PC_LOW_Z            9          //�����ѹ�Ͷ�ֵ
#define PC_NO_V            10      //��ѹ�ż�ֵ
#define PC_T_DELAY         11              //������ʱʱ��
#define PC_JAG_P           12      //���оݱ�־
#define PC_JAG_Z           13      //�����оݱ�־
#define PC_EFS_MODEL       14      //�ź�Դ�ͺű�ʶ
#define PC_JAG_ACT         15      //���������о�
#define PC_PULSE_VALID     16      //8������Ч��ֵ
#define PC_GPRS_MODEL      17      //GPRS��ʽ��ֵ
#define PC_REV_CURRENT     18      //��ת������ֵ
#define PC_U0_CAL          19      //U0������ʽ

#define PC_CNL_MODEL	        20		//���Ʒ�ʽ
#define PC_LACTH_TIME           21//�ź�Դ����ʱ�䣬�Է���Ϊ��λ��Ĭ��18����
#define PC_PLUSE_TIME           22//�����ȣ�10msΪ��λ Ĭ��25 
#define PC_SEND_TIME1           23//���巢�ͼ��ʱ��1 ��10msΪ��λ Ĭ��100
#define PC_SEND_TIME2           24//���巢�ͼ��ʱ��2 ��10msΪ��λ Ĭ��125

#define PC_OVERLOAD_CNT         25
#define PC_OVERLOAD_I           26
#define PC_OVERLOAD_T           27

/*
#define RP_ETH_MODEL            20      //��̫����ʽ 0��SERVER   1��CLIENT
#define	RP_G_IP_1               21		//����IP1~4
#define	RP_G_IP_2		22
#define	RP_G_IP_3		23
#define	RP_G_IP_4		24
#define	RP_SUB_MASK_1		25       //��������1~4
#define	RP_SUB_MASK_2		26
#define	RP_SUB_MASK_3		27
#define	RP_SUB_MASK_4		28
#define	RP_MAC_ADDR_1		29       //W5500 MAC��ַ1~6
#define	RP_MAC_ADDR_2		30
#define	RP_MAC_ADDR_3		31
#define	RP_MAC_ADDR_4		32
#define	RP_MAC_ADDR_5		33
#define	RP_MAC_ADDR_6		34
#define	RP_W_IP_1		35        //W5500IP1~4
#define	RP_W_IP_2		36
#define	RP_W_IP_3		37
#define	RP_W_IP_4		38
#define	RP_D_IP_1		39        //W5500IP1~4
#define	RP_D_IP_2		40
#define	RP_D_IP_3		41
#define	RP_D_IP_4		42
#define	RP_S0_PORT		43        //Socket0 �˿ں� 
*/


//#define RUN_INFO_NUM            22        //����վ������վ��ַ��21����
#define RUN_INFO_NUM            20        //����վ������վ��ַ��20����
#define INF_CRC                 20
//============================  ����������  ==================================
#define DIN_NUM                 3     //�ܹ��Ŀ������������

#define DIN_CON_TIME           160     //ң��ȷ��ʱ�䣬�ݶ�200ms 
//============================  ʵʱʱ��        ==================================
#define RTC_NUM                         7

#define RTC_YEAR                        0       //��
#define RTC_MONTH                       1       //��
#define RTC_DATE                        2       //��
#define RTC_HOUR                        3       //ʱ
#define RTC_MINUT                       4       //��
#define RTC_SEC                         5       //��
#define RTC_MICROSEC                    6       //����

//=============================   ң������������� =========================

#define TYPE_RMTDO_NUM		5      //ͨ�ŵ�������

#define RMTDO_MONI_A			0   //ģ��A�����
#define RMTDO_MONI_B		        1   //ģ��B�����
#define RMTDO_MONI_C			2   //ģ��C�����
     
#define RMTDO_CLEAR_SOE			3    //���SOE��¼

#define RMTDO_RESET			4    //Զ����λ

//============================  ����ʱ���¼ ===========================================
#define         BK_EEP_NUM              25      //��EEPROM�б�������� 25��
#define         BK_DATA_LEN_BYTE        9      //ÿ��SOE������ռ���ֽ���
//============================  ����ʱ���¼ ===========================================
#define         OFF_EEP_NUM              30      //��EEPROM�б�������� 30��
#define         OFF_DATA_LEN_BYTE        7      //ÿ��SOE������ռ���ֽ���
//============================  ״̬�쳣ʱ���¼ ===========================================
#define         ABN_EEP_NUM              30      //��EEPROM�б�������� 30��
#define         ABN_DATA_LEN_BYTE        7      //ÿ��SOE������ռ���ֽ���
//============================  ���ϼ�¼ ===========================================
#define         SOE_EEP_NUM              30      //��EEPROM�б�������� 30��
#define         SOE_DATA_LEN_BYTE        12     //ÿ��SOE������ռ���ֽ���
//============================  ¼�����ݼ�¼ ===========================================
#define         WAVE_EEP_NUM              30      //��EEPROM�б�������� 30��
#define         WAVE_DATA_LEN_BYTE        1     //ÿ��SOE������ռ���ֽ���

//SOE���ݵĶ���

//===============================  COMMģ��  =============================
#define COMM_PORT_NUM           3//����ͨ�Ŷ˿ڸ���


#define COMM_INDEX_000          0
#define COMM_INDEX_232          1
#define COMM_INDEX_GPRS         2



//#define MAX_TXBUFF_LEN       128//����ʹ����һ��Э�飬�����ʱʹ�õĻ�������ͬ����˻���������ȡ���ֵ��
#define MAX_RXBUFF_LEN       256//����ʹ����һ��Э�飬�����ʱʹ�õĻ�������ͬ����˻���������ȡ���ֵ��

#define MAX_TXBUFF_101_LEN       256//101Э�鷢�ͻ���������
#define MAX_SOE_TXNUM            8//16//��������SOE����

//============================  SOE��¼ ===========================================
#define         SOE_BUFF_NUM            32      //SOE�ڴ滺������SOE������

#define         SOE_DATA_LEN            11       //ÿ��SOE���ݵĳ���
#define         SOEVALID               0X55
#define         SOE_SENDDA_LEN         11//SOE_DATA_LEN-1
//SOE���ݵĶ���
#define         SOE_FLAG    0  
#define         SOE_TYPEL   1 
#define         SOE_TYPEH   2 
#define         SOE_STVAL   3 
#define         SOE_MSL     4 
#define         SOE_MSH     5
#define         SOE_MINU    6 
#define         SOE_HOUR     7
#define         SOE_DAY     8
#define         SOE_MONTH   9  
#define         SOE_YEAR    10 
/*#define         SOE_UA      11
#define         SOE_UB      12
#define         SOE_UC      13
#define         SOE_U0      14*/

//============================  �ڲ�flash�洢  ===============================

#define SOE_RECORD_ADD   0x42000     //SOE��¼��ַ��

#define SOE_NUM_SEG   4//42
#define SOE_SEG       24 

#define SOE_RECORD_LEN  0x10        //ÿ����¼16�ֽ�
#define SOE_RECORD_NUM  32          //32����¼

//====================  EEPROM ��ַ����  CAT25C128 (0~0x3FFF �ֽ�) 16K�ֽ� ======================
//����CAT25128��ҳΪ64�ֽڣ������ռ����ʼ��ַ���Ϊҳ��ʼ
//EEPROM�б����������Ĳ���      ����1K�ֽڵĿռ�
#define EEPADD_BIAS_V               0x0000    //ƫ�õ�ѹϵ����10�ֽ�   ռ16�ֽ�
#define EEPADD_COM                  0x0010    //վ���ơ�վ��ַ��42�ֽ� ռ48�ֽ�
#define EEPADD_PHONE_NUM            0x0040    //4���ֻ����루ÿ��13λ�����ֻ���������57�ֽ�  ռ64�ֽ�
#define EEPADD_PHONE                0x0040	//�ŏ| 0328 Ϊ����ѩ�ٳ�����ݣ�==EEPADD_PHONE_NUM 
//N(N���Ϊ8��Ŀǰ������Ϊ4��)���绰����( 1�ֽڵ绰���볤�� +15�ֽڵ绰)��N�ֽڶ��ŷ��Ϳ�����//�ŏ| 0328 
//��136�ֽ�//�ŏ| 0328 
#define EEPADD_SOE_E2ROMADR            0x0100//0x0080//�ŏ| ͷ4�ֽڣ�β4�ֽ� ����2�ֽ�
#define EEPADD_SOESEND_E2ROMADR   0x0110
#define EEPADD_CNNAME 			 0x0140//�ŏ| ����վ��42�ֽ�
#define EEPADD_ADJOBJ                   0x0180//�ŏ| Ŀ��У׼����λ�����ز���10�ֽ� ��ʼֵΪ��ѹ60V,����2A
#define EEPADD_VER_CPU                0x01a0          //����汾�� 2�ֽ�
//EEPROM��IEC��������
#define EEPADD_IECYXADDR        0x01b0 //ң�ŵ�ַ  32�ֽ�
#define EEPADD_IECPARA             0x0200 //IEC����   32byte
#define EEPADD_IECYCADDR        0x0240 //ң�����  100byte 
#define EEPADD_IECYKADDR        0x0280 //ң�ز���  100byte
#define EEPADD_RP                      0x02c0
#define EEPADD_LBNAME             0x03c0
#define EEPADD_LBNAMELEN       0x03e0
//EEPROM�в����ı�����  ����1K�ֽڵĿռ�
#define EEPADDBK_BIAS_V               0x0400    //ƫ�õ�ѹϵ����10�ֽ�   ռ16�ֽ�
#define EEPADDBK_COM                    0x0410    //վ���ơ�վ��ַ��42�ֽ� ռ48�ֽ�
#define EEPADDBK_PHONE_NUM       0x0440    //4���ֻ����루ÿ��13λ�����ֻ���������57�ֽ�  ռ64�ֽ�
#define EEPADDBK_PHONE                0x0440
#define EEPADDBK_SOE_E2ROMADR         0x0500//0x0080//�ŏ| ͷ4�ֽڣ�β4�ֽ� ����2�ֽ� 
#define EEPADDBK_CNNAME 		  0x540//�ŏ| ����վ��42�ֽ�
#define EEPADDBK_ADJOBJ               0x0580//�ŏ| Ŀ��У׼����λ�����ز���10�ֽ� ��ʼֵΪ��ѹ60V,����2A
#define EEPADDBK_IECYXADDR        0x05b0
#define EEPADDBK_IECPARA            0x0600
#define EEPADDBK_IECYCADDR        0x0640
#define EEPADDBK_IECYKADDR        0x0680
#define EEPADDBK_RP                      0x06c0

#define EEPADD_LOADNUM               0x0720   
#define EEPADDBK_LOADNUM           0x0730

#define EEPADD_LOGP               0x0740   
#define EEPADDBK_LOGP           0x0750


//EEPROM�ж��ߡ����硢�쳣����������������ʱ���¼  ����1K�ֽڵĿռ�
#define EEPADD_TIME_BK_NUM        0x0800          //����ʱ��������  1
#define EEPADD_TIME_BK_POS        0x0801          //����ʱ��λ����  1
#define EEPADD_TIME_BK            0x0802          //����ʱ��ֵ     225 �ֽ�  ռ256�ֽ�
#define EEPADD_TIME_OFF_NUM       0x0900          //����ʱ��������  1
#define EEPADD_TIME_OFF_POS       0x0901          //����ʱ��λ����  1
#define EEPADD_TIME_OFF           0x0902          //����ʱ��ֵ 212 �ֽ�  ռ256�ֽ�
#define EEPADD_TIME_ABN_NUM       0x0a00          //״̬�쳣ʱ��������  1
#define EEPADD_TIME_ABN_POS       0x0a01          //״̬�쳣ʱ��λ����  1
#define EEPADD_TIME_ABN           0x0a02          //״̬�쳣ʱ��ֵ 212 �ֽ�  ռ256�ֽ�

//EEPROM�й��ϼ�¼  ����1K�ֽڵĿռ�
#define EEPADD_SOE_NUM          0x0c00          //���ϼ�¼������  1
#define EEPADD_SOE_POS          0x0c01          //���ϼ�¼λ����  1
#define EEPADD_SOE_DATA         0x0c02          //���ϼ�¼ 360�ֽ�


//EEPROM��¼������  ����8K�ֽڿռ�
#define EEPADD_REC_NUM          0x0d00           //¼�����ݵ�������
#define EEPADD_REC_POS          0x0d02           //����¼���������ڵ����λ��
#define EEPADD_REC_DATA         0x0d04           //¼������У��


//E2�б���¼���ĸ���
#define EEPADD_LUBONUM          0x0E98 
#define EEPADD_ALEN             0x0E9A //A���ƫ����
#define EEPADD_BLEN             0x0E9C //B���ƫ����
#define EEPADD_CLEN             0x0E9E //C���ƫ����
#define EEPADD_COMLEN           0x0EA0 //���ƫ����


#define EEPADD_VerNum            0x1104 // ��ǰ¼�����ܸ���,ռ��8���ֽڣ�SV01.033
#define EEPADD_LOGNUM           0x1110  //��FLASH�б���ĸ��ɼ�¼������ ռ��2�ֽ�
#define EEPADD_LOGADDR_NEW      0x1112  //�����洢�ļ�¼���һ����¼��ƫ��λ�ã�Ϊ(0----FLASH_LOAD_MAXNUM-1) ռ��2�ֽ�
#define EEPADD_LOGADDR_OLD      0x1114  //����һ����¼�洢λ��
#define EEPADD_LOG_CHK          0x1116  //�����������ݼӺ�У��
#define EEPADD_DAYNUM           0x1200 //���㸺�ɼ�¼����31�죬ÿ������96����ʵ�ʴ������
#define EEPADD_VALIDITY         0x1300 //��Ч�ڣ�64��¼��Ŀ¼��ÿ��¼��Ŀ¼����Ч��Ϊ2���ֽڣ���2*64���ֽ�
#define EEPADD_FIXNUM           0x1400 //ͨ���ļ������涨�����������31*20���ֽ�
#define EEPADD_CFG              0x1700 //����CFG�ļ���ĿǰCFG�ļ����ռ��700���ֽڣ�û��д��FLASH��ԭ����:ÿ�η��͸��ļ�ʱ������Ҫ��CFG�ļ���ĳ�����ط����޸ģ��������
#define EEPADD_DEGBF101            0x1A00 // DEG�� ��д����ʱ�����ݵ�ǰ���е������д�ļ�ʱ�����������Ľ��̰������١���ʱ�������ϱ���������ʱ�����Լ�������
#define EEPADD_GPRSBF101           0x1B00 // GPRS�� ��д����ʱ�����ݵ�ǰ���е������д�ļ�ʱ�����������Ľ��̰������١���ʱ�������ϱ���������ʱ�����Լ�������

#define EEPADD_SOESTARTADR       0x2000 //���ƫ����
#define EEPADD_SOEENDADR       0x4000
//============================   GPIO��ӦLED����  ===============================
#define WDG_SET         (P5OUT |= BIT3)         //���Ź���λ    ͬʱָʾ���е���˸
#define WDG_CLR         (P5OUT &= NBIT3)        //���Ź�����

#define LED_RUN_ON      (P6OUT |= BIT0)        //LED00 ���е� P6.0
#define LED_RUN_OFF     (P6OUT &= NBIT0) 
#define LED_RUN_TOGG    (P6OUT ^= BIT0)


//============================   GPIO��ӦDO����  ===============================
#define KA1_OFF     (P4OUT &= NBIT6)                        //�쳣״̬�ָ�
#define KA1_ON      (P4OUT |= BIT6)                         //�쳣״̬����
#define KB1_OFF     (P4OUT &= NBIT7)                        //�ź�Դ����ָ�
#define KB1_ON      (P4OUT |= BIT7)                         //�ź�Դ���籨��
#define KC1_OFF     (P5OUT &= NBIT4)                        //û����Ч8����
#define KC1_ON      (P5OUT |= BIT4)                         //������Ч8����

#define KA0_OFF     (P7OUT &= NBIT2)                        //A��Ӵ�����ֹ
#define KA0_ON      (P7OUT |= BIT2)                         //A��Ӵ�����ͨ
#define KB0_OFF     (P7OUT &= NBIT3)                        //B��Ӵ�����ֹ
#define KB0_ON      (P7OUT |= BIT3)                         //B��Ӵ�����ͨ
#define KC0_OFF     (P8OUT &= NBIT0)                        //C��Ӵ�����ֹ
#define KC0_ON      (P8OUT |= BIT0)                         //C��Ӵ�����ͨ

#define   ABN_CHECK           BIT0          //abnormal self checked
#define   POWER_OFF           BIT1          //power down
#define   EIGHT_PULSE         BIT2          //eight pulse omit    
#define   SEND_AUTO           BIT3          //upload regulary
#define   FAULT_RST           BIT5          //fault recovery
#define   FAULT_OCCUR         BIT4          //fault occcur
#define   PHASE_BREAK         BIT6          //���߷���
#define   ZIZHAN_XUN          0x80          //��վѯ��
#define   PHONE_XUN          0xc0          //��վѯ��

#define NUM_10H  0x10
#define NUM_68H  0x68

#define   FEED_WATCH_DOG()            P5OUT ^= BIT3;

#define   LIGHT_RUN_UP()              P6OUT |= BIT0;
#define   LIGHT_RUN_DOWN()            P6OUT &= NBIT0;
#define   LIGHT_RUN_FLASH()           P6OUT ^= BIT0;

#pragma location=0xFF00
const unsigned char ucVerNum[]="Ver02.02";

//****************** command_flag *************************
//************* �ն�������������ʱ �ķ���ԭ��*****************//>0x68:�ɱ�֡��>0x10���̶�֡
#define ZONGZHAO_CONFIRM  0x71    //    ����-ȷ��
#define ZUZHAO_CONFIRM    0x72    //    ����-ȷ��
#define ZONGZHAO_STOP     0x73    //    ����-ֹͣ
#define ZUZHAO_STOP       0x74    //    ����-ȷ��
#define CESHI_CONFIRM     0x75    //    ����-ȷ��
#define FUWEI_CONFIRM     0x76    //    ��λ-ȷ��
#define FUWEI_STOP        0x77    //    ��λ-ֹͣ

#define YANSHI_CONFIRM    0x79    //    ��ʱ-ȷ��
#define TONGBU_CONFIRM    0x7A    //    ʱ��ͬ��-ȷ��
#define TONGBU_STOP       0X7B    //    ���Ͷ�ʱ֮ǰ��ʱ��

#define CHUSHIHUA_FINISH  0xA0    //    ��ʼ��-���

#define YAOXIN_DAN_ALL    0X7C    //    ��ȫ�� ����-ң�� ��Ӧ
#define YAOCE_ALL         0X7D    //    �� ȫ��ң�� ��Ӧ
#define YAOXIN_SHUANG_ALL 0x7E    //    ��ȫ�� ˫��-ң�� ��Ӧ

#define CHUSHIHUA_JIESHU  0x8E    //    ��ʼ������-ȷ��
#define GGDIZHI_RONGCUO   0x90    //    ������ַ�ݴ�
#define COT_RONGCUO       0x91    //    δ֪ ����ԭ��
#define TYP_RONGCUO       0x92    //    δ֪ ���ͱ��

#define QINGQIULIANLU     0x11    //    ������·
#define FUWEIYUANFANG     0x12    //    ��λԶ��

#define LINK_START      0x13    //    ��·����
#define LINK_TEST       0x14    //    ��·����
#define LINK_STOP       0x15    //    ��·ֹͣ
#define LINK_ACK        0x16    //    �϶�ȷ��
#define LINK_TEST_SEND  0x18    //    ��·���� ����
#define LINK_STOP_SEND  0x19    //    ��·ֹͣ ����
#define LINK_START_SEND 0x17    //    ��·���� ����

//****************** process_type *************************
//************** �ն�����վ֮�� �������� ******************
#define CHUSHIHUA         0X01    //  ��ʼ��
#define ZONGZHAO_INIT     0X02    //  ���٣���ʼ����
#define YANSHI            0X03    //  ��ʱ
#define ZONGZHAO          0X04    //  ����
#define TONGBU            0X05    //  ʱ��ͬ��
#define CESHI             0X06    //  ����
#define FUWEI             0X07    //  ��λ  
#define ZUZHAO_YX         0X08    //  ����_ң��
#define ZUZHAO_YC         0X09    //  ����_ң��
#define ZUZHAO_YXD        0X0A    //  ����_˫��ң��


//****************** ���ȼ����� *************************
#define INIT_PRIORITY         0x38  //  ��ʼ��
#define ZONGZHAO_I_PRIORITY   0x36  //  ���٣���ʼ����

#define YAOXIN_PRIORITY       0x30  //  ң��
#define YANSHI_PRIORITY       0x28  //  ��ʱ ���
#define ZONGZHAO_PRIORITY     0x26  //  ����

#define YXSOE_PRIORITY        0x20   // ң��SOE

#define YC_PRIORITY           0x10   // ң��ֵ
#define TONGBU_PRIORITY       0x09  //  ʱ��ͬ��
#define CESHI_PRIORITY        0X08  //  ����

#define FUWEI_PRIORITY        0X06  //  ��λ

#define ZUZHAO_YX_PRIORITY    0X04  //  ���� ң��
#define ZUZHAO_YC_PRIORITY    0x04  //  ����  ң��

#define GGDIZHI_PRIORITY      0x02  //  ������ַ�ݴ�
#define COT_PRIORITY          0x02  //  δ֪ ����ԭ��
#define TYP_PRIORITY          0x02  //  δ֪ ���ͱ��

//****************** GPRS101 �� SMS ״̬�л�*************************
#define GPRSState_IDLE           0x00
#define GPRSState_GPRS101    0x01
#define GPRSState_SMS            0x02
//FLASH��¼������        �ӵ�1200ҳ��ʼ������640ҳ ÿ16ҳ(3840)��һָֻʾ����һ��¼�����ݣ�16*4=64ҳ��һ����������¼�����ݣ��ɴ�10��������¼������

#define FADDR_RECORDER_START         0x4B000   //����¼����ʼ��ַ         

#define FRECORDER_TOLNUM      21  //�����ⲿflash��¼��������20��������¼��
//#define FRECORDER_PNUM        16    //ÿ16ҳ��һ��¼������
#define WRITEPECORDER  228//д������ݸ��� ����ַ��3��+ƫ�Ƶ�ַ��3��+��������1��+�������ݣ�224��+�ۼӺͣ�1��
//#define FLINEADDR  (64*256)
#define FLINEADDR  (16*256)
#define FPHASEADDR (16*256)

#define FADDR_RECORDER_STARTV        0x9C000   //��ѹ¼����ʼֵ��ַ 
#define RECORDER  (80*15)

#define FADDR_RECORDER_INFO       0x10000   //¼��Ŀ¼��Ϣ A20*100A
#define FADDR_RECORDER_DATA      0x4000   //����¼������0x4000
#define FADDR_RECORDER_ACTDATA      0x70000   //����¼������0x70000
#define FADDR_RECORDER_XHDATA      0x6C0000   //Ϩ��¼������0x6C0000
#define FADDR_LOG_START      0x720000   //LOG����0x720000-7290000
#define FADDR_LOG_END      0x729000  
#define MAX_LOG_NUM       4//�ڴ���LOG������

/*
��������7AF000
���ɼ�¼//750000-790000   ÿ��32�ֽ� ��8192��
Ϩ��¼��//6C0000-710000  ÿ��2���ֽڡ�0.5��BLOCK����10��
����¼��//70000-6B0000  ÿ��60���ֽڡ�9��BLOCK����10��
����¼��//4000-68000  ÿ��6400�ֽڡ�2��Sector(0x2000)����50��
soe//0-3000 ÿ��16�ֽڹ�768��
*/
/*
0x0         -  0x3000     �洢soe��ÿ��soe16�ֽڣ���512��
0x10000  -  0x30000   ��gRecorder_cfg������¼���ļ�����¼���ļ����ȣ�¼��ʱ�䣬ÿ��ռ1��
sector-4096�ֽ�

0x100000-  0x300000   �����¼����5��ң�⣬8���ܲ���ÿ�ܲ�80�㣬��640�㣬6400�ֽڣ�ÿ��ռ4��block-0x10000�ֽ�
                                   �涯��¼����5��ң�⣬10-25�룬ÿ��800�㣬���20000�㣬20���ֽڣ�ÿ��4��block-0x50000
*/
#define RECORDER_INFO_LENGTH      100      

//#define FADDR_RECORDER_VINFO       0xF4000   //¼��Ŀ¼��Ϣ 20*100
//#define FADDR_RECORDER_VINFOB       0xF5000   //¼��Ŀ¼��Ϣ 20*100
//#define FADDR_RECORDER_VINFOC       0xF6000   //¼��Ŀ¼��Ϣ 20*100
//#define FADDR_RECORDER_VINFO0       0xF7000   //¼��Ŀ¼��Ϣ 20*100



#define RECORDER_MULU_LENGTH      13  //�ظ�ÿ��¼��Ŀ¼�ĳ���

#define FADDR_RECORDER_START_BK         0x100000   //����¼����ʼ��ַ 
#define FADDR_RECORDER_STARTV_BK        0x151000   //��ѹ¼����ʼֵ��ַ 


//#define FSOE_LEN         16    //û��SOEռ16�ֽ� �ڴ���ÿ��ռ11�ֽڣ�����Ϊ��flash�����״����Ԥ����5�ֽ�

//===============================��ڱ�־���ݼ�����У��洢��================
#define FLASH_UPDATE_COUNTER     0x7AF000 //������
#define FLASH_UPDATE_CHECK       0x7AF002 //У��ֵ
#define FLASH_UPDATE_STATE       0x7AF004 //����״̬
#define FLASH_UPDATE_EARASE       0x7AF005//�ڲ�flash������ʾ���ڱ�ʾ�����������Ƿ��Ѿ����ڲ�flash�������Է�ֹ�ڲ����Ժ�ı�����״̬��������ȥѰ��ԭ�г������
#define FLASH_UPDATE_CCHECK       0x7AF006 //У��
//����
#define FLASH_UPDATEBK_COUNTER   0x7AF008 //������
#define FLASH_UPDATEBK_CHECK     0x7AF00A //У��ֵ
#define FLASH_UPDATEBK_STATE     0x7AF00C //����״̬
#define FLASH_UPDATEBK_EARASE     0x7AF00C //flash������־
#define FLASH_UPDATEBK_CCHECK     0x7AF00D //У��

//===================================Ӧ�ó���洢��===========================
#define FLASH_UPDATE_START      0X7B0000       //
#define FLASH_UPDATE_END        0X7EFFFF       //
#define FLASH_UPCOUNTER          0x7F0000//��Ч��������      


#define ERASE_DELAY 30000//������д������ʱ��̫С������д�����ȫ1��
#define ERASEFLAGADDR   0x45A00//�ڲ�flash��ַ��jtag��д�����־�׵�ַ,ʹ��6���ֽڱ�ʾ���ڳ����������֮��ȫ��дΪ0x55

#endif

/*
��������7AF000
���ɼ�¼//750000-790000   ÿ��32�ֽ� ��8192��
Ϩ��¼��//6C0000-710000  ÿ��2���ֽڡ�0.5��BLOCK����10��
����¼��//70000-6B0000  ÿ��60���ֽڡ�9��BLOCK����10��
����¼��//4000-68000  ÿ��6400�ֽڡ�2��Sector����50��
soe//0-3000 ÿ��16�ֽڹ�768��
*/

