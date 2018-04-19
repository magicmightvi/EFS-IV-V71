//在MSP430中的Long型的数组是低字在前，高字在后

#ifndef _CONSTANT_H
#define _CONSTANT_H 

#define DEBUG
#define GETSOEFROMRAM     //发送的SOE从外部FLASH取，还是从内部取。
//#define ZT_SOE_IN_FLASH //SOE存内部FLASH
//#define ZDSENDADDR //主动给中心站定时发送自身站地址(特殊应用) 
//#define YN_101S      //定义=云南101规约；不定义=标准101规约
//#define CQ_101S      //定义=重庆101规约；不定义=标准101规约
//#define INT_NEST      //张弢测试中断嵌套       0//1=GPRS;0=SMS
//#define DH_101S      //定义=丹华昊博101规约；不定义=标准101规约
#define SD_101S      //定义=丹华昊博101规约；不定义=标准101规约

#define CONDIN_3      //接触器输出3路开合信号  取消信号源异常遥信(辅助开关跳开)


#define         TYPE_EQU                0x1111  //设备代码

#define        INVALIDYC  0xD8F0//-9999
#define        INVALTEYC  0xFF9D//-99
#define        INITYC     0xDD47//-8888
#define        ODUINITYC     0xFFFF//ODU初始化值


 
#define         YES                     0x55
#define         NO                      0xAA
#define         ON                      0x5F
#define         OFF                     0x50
#define         CLOSE                   0x55
#define         TRIP                    0xAA

#define         LuboType_XH      0x55
#define         LuboType_ACT      0xAA

#define  OPERATING_LOGO1 1 //操作标识1 读目录
#define  OPERATING_LOGO2 2//操作标识2 读目录确认
#define  OPERATING_LOGO3 3//读文件激活
#define  OPERATING_LOGO4 4 //读文件激活确认
#define  OPERATING_LOGO5 5 //读文件数据响应
#define  OPERATING_LOGO6 6 //读文件数据响应确认

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

#define KJa1                (P4IN&0x08)//a接触器开关位置信号
#define KJb1                (P4IN&0x10)//
#define KJc1                (P4IN&0x20)//

//===================数据类型定义============================
typedef unsigned char  INT8U;       //无符号8位数
typedef          char  INT8S;       //有符号8位数
typedef unsigned int   INT16U;      //无符号16位数
typedef signed   int   INT16S;      //有符号16位数
typedef unsigned long  INT32U;      //无符号32位数
typedef signed   long  INT32S;      //有符号32位数
typedef float          FP32;        //单精度浮点数
typedef double         FP64;            //双精度浮点数

#define  GETBIT(p,i)   ((p[(i>>4)] & BIT[(i&0x0F)]) && 1)
#define  SETBIT(p,i,v) p[(i>>4)] = (v) ? (p[(i>>4)]|BIT[(i&0x0F)]) : (p[(i>>4)]&~BIT[(i&0x0F)])

#define MAX_ENC_FRAME_LEN                    384
//==============================IEC 101相关参数======================================
#define GPRS_SMS      //       0//1  =GPRS;0=SMS

#define IECP_LINKADDR_NUM    0//链路地址长度 1-2    
#define IECP_TRANSRSN_NUM    1//传输原因长度 1-2   
#define IECP_COMNADDR_NUM    2 //公共地址长度 1-2  
#define IECP_INFO_NUM        3//信息体地址长度 1-3 
#define IECP_COM_MODE        4//0为不平衡时，1为平衡式  默认为1
#define IECP_AUT_MODE        5//主动上传模式，0=主站发送初始化后方可主动上传和总召
                              //1=收到主站任何命令后方可主动上传，任何时候都响应总召命令。
                              //2=主动上传不受任何约束
#define IECP_OBJ_COM0        6 //调试串口波特率，0=9600,1=19200,2=38400,3=57600,4=115200
#define IECP_OBJ_COM1        7
#define IECP_OBJ_COM2        8
#define IECP_OBJ_COM3        9

#define IECP_CRC             10//crc校验

#define IEC_PARA_NUM    11  //IEC运行的参数个数

#define IEC_YC_NUM   16  //允许传给主站的遥测个数上限
#define IEC_YX_NUM   64          //RMT_INFO_NUM允许传给主站的遥信个数上限
#define IEC_YK_NUM  OUT_TYPE_NUM  //允许传给主站的遥控个数上限
#define FRM_MAX_COS_NUM   70 //每帧报文最多传输COS个数，规定为70个
#define IEC_ZJ_NUM   3  //允许传给主站的自检个数上限
//双点
#define RI_ZD_DYX1  		512

//#define RMT_INFO_NUM           524         //总遥信个数
#define RMT_WORD_NUM           RMT_INFO_NUM/8+1          //总遥信占用WORD个数 每个遥信占2个bit，一个WORD保存8个遥信
#define RMT_WDYX_NUM            2                        // 12 双点遥信占的字 12/8+1
#define RMT_WSYX_NUM            1          //RMT_INFO_NUM - 12 单点遥信字个数 (RMT_INFO_NUM - 12)/8 +1

#define RMT_ZSQST_NUM          LINE_NUM * PHASE_NUM    //线路通信状态个数  12*4
#define RI_ZSQST_H             RI_12O_ZSQST           //指示器状态遥信上限
#define DYX_ADDR               RI_ZD_DYX1             //双点遥信起始号
#define RI_FTINFO_NUM           LINE_NUM * PHASE_NUM * FTTPYE_NUM//需要闭锁的遥信个数 短路(5*12)+接地(5*12)+温度(5*12)

//============================  AD采集  =============================================
#define AC_AD_CHAN_NUM                          6            //交流AD采样通道数，三相电流**增加1路零序电压  增加一通道对侧电压UX
#define AC_SAMPLE_DOTS                          16      //每周波采样的点数为32点
#define AC_BUFF_LEN                             32             //交流采集缓冲区保存2个周波，32个点

#define AC_SAM_TAIL_OFFSET                      2               //交流采集放置偏移  由于目前发现有误报的情况，把数据采集下来不立刻送到CPU采集

//交流ADC采样通道号定义
#define CHAN_UA                               0
#define CHAN_UB                               1
#define CHAN_UC                               2
#define CHAN_U0                               3
#define CHAN_I0                               4
#define CHAN_Upt                               5

//============================ 符合历史记录  =============================================
#define FLOADINFONUM                    4           //
#define FLOAD_TOTALNUM                0            //记录总条数
#define FLOAD_NEW                          1             //即将存储的记录与第一条记录的偏移位置(不是已存储的最新一条记录的位置)
#define FLOAD_OLD                           2             //最老一条记录的位置
#define FLOAD_CS                             3
#define FLOAD_DAY 3
#define FLASH_LOAD_MAXNUM        8192
#define FADDR_LOAD_START           0x750000//750000-790000
#define FLASH_PLOAD_LEN              32//每条负荷记录32字节



//============================  SMS-电话号码        =======================================

#define PHONE_CHARNUM         16//15     //每个电话号码占的字节数/字符数 最大字符数//张弢 0328 ( 1字节电话号码长度 +15字节电话)
//#define PHONE_NUM             4//3      //支持的电话号码个数//张弢 0328 (N最大为8，目前程序定死为4个)

#define PHONE_ENCTL           0 //电话号码使能控制字
#define PHONE1_CHARLEN        1 //号码1长度
#define PHONE1                2
#define PHONE2_CHARLEN        PHONE1 + PHONE_CHARNUM
#define PHONE2                PHONE2_CHARLEN + 1
#define PHONE3_CHARLEN        PHONE2 + PHONE_CHARNUM
#define PHONE3                PHONE3_CHARLEN + 1

#define PHONE_CS              68////张弢 0328 共68字节, 4个电话号码PHONE3 + PHONE_CHARNUM         
#define PHONE_PA_NUM          69//PHONE_CS + 1  //张弢 0328 共69字节, 4个电话号码+4BYTE控制字+1BYTE CS效验

//N(N最大为8，目前程序定死为4个)个电话号码( 1字节电话号码长度 +15字节电话)，N字节短信发送控制字//张弢 0328 
//共136字节//张弢 0328
#define MAX_SMS_NUM 16
#define CnName_NUM   20//张弢 短信用汉字站名地址
#define LBName_NUM   20//张弢 录波文件用英文站名地址
//============================  录波数据        =======================================
#define  REC_AC_CHAN_NUM        5               //录波交流电压电流的通道数，智能永磁驱动器主要是三相电流和零序电流

#define RECAC_UA                               0
#define RECAC_UB                               1
#define RECAC_UC                               2
#define RECAC_U0                               3
#define RECAC_I0                               4
#define REC_AC_BEF_LEN          128              //动做之前保存3个周波的数据
#define REC_AC_LEN              640             //动作后保存5个周波的数据，动作前3个周波的数据，共8周波   

#define         REC_DATA_LEN            10       //每条录波数据的长度

//录波数据的定义 
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

//#define         FADDR_REC_START    0   //Flash地址
//============================  SST25 FLASH地址分配        =======================================
#define         FADDR_REC_START    20480   //录波数据Flash地址起始地址//张弢 SOE存FLASH
#define         REC_LEAF_NUM       16   //每条录波页数 占16页 16*256
#define         MAX_REC_NUM       50 //录波数文件最多32个  
#define         MAX_ACTREC_NUM       10 //录波数文件最多32个  
#define         MAX_XHREC_NUM       10 //录波数文件最多32个  
#define         MAX_ALLREC_NUM       73 //全部录波数文件最多73个MAX_REC_NUM+1+MAX_ACTREC_NUM+1+MAX_XHREC_NUM+1 

#define         FADDR_SOE_START    0   //SOE数据Flash地址起始地址//张弢 SOE存FLASH
#define         MAX_SOE_NUM       512   //SOE最多存512条，每条11字节，占16字节//张弢 SOE存E2ROM
#define         MAX_SOE_BYTE       0x3000 //768*16   //SOE最多存1024条，每条11字节，占16字节//张弢 SOE存FLASH

//#define         REC_LEAF_NUM      9//8       //录波数据占用FLASH页数 由于加了三个通道UA,UB和UC，所以多占了一页
//#define         REC_DISPLAY_NUM      512  //提取给显示的录波条数
//保存在EEPROM中的参数更新标志
#define PARA_NUM        7

#define CHNG_INFO       0       //运行信息
#define CHNG_ADJ        1       //校正信息
#define CHNG_PC         2       //保护定值
#define CHNG_MUBIAO     3       //保护定值组组号
#define CHNG_TEL        4       //运行参数
#define CHNG_GPRS       5       //GPRS参数
#define CHNG_CODE       6       //液晶操作密码

//=================    信息对象地址定义，参考101协议，兼顾Modbus协议 =============================
/*#define         EQU_BASE                                0x0001          //状态量信息起始地址
#define         PC_BASE                                 0x1001          //保护定值起始地址
#define         RI_BASE                                 0x2001          //遥信量信息起始地址
#define         RM_BASE                                 0x4001          //遥测量起始地址
#define         ADJ_BASE                                0x5001          //校正参数起始地址
#define         RC_BASE                                 0x6000          //遥控起始地址
#define         RP_BASE                                 0x7000          //运行参数起始地址
#define         RUN_BASE                                0x7200          //运行信息起始地址*/



//============================  遥测量          =======================================
#define RMT_MEAS_NUM            10


#define RM_U0                   0
#define RM_UA                   1
#define RM_UB                   2
#define RM_UC                   3
#define RM_UAB                 4       //零序电压
#define RM_UBC                 5
#define RM_UCA                 6
#define RM_I0                   	7       //零序电流
#define RM_ACT_NUM    	8
#define RM_UPt                	9

#define RM_CSQ                	10
#define RM_UCAP               	11
#define RM_bk1               	12
#define RM_bk2                	13
#define RM_bk3                	14
#define RM_bk4                	15

#define RM_FILTER_NUM           10      //遥测量的滤波缓冲区深度
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

#define YX_FUN_ON               0  //功能投退
#define YX_MANUAL_ACTION        1  //手动投切
#define YX_ACTION_SUCCESS       2 //最终投切成功
#define YX_EARTH_FAULT          3  //接地告警
#define YX_PHASE_OFF            4  //PT断线告警

#define YX_EFS_OFF              5  //控制器失电
#define YX_U0_HIGH             6 //零序过压报警
#define YX_UAB_HIGH             7 //线电压过压报警
#define YX_U_HIGH              8  //相电压过压告警

#define YX_U_LOW               9 //相电压欠压告警
#define YX_EFS_ABNORMAL         10//控制器异常告警
#define YX_PHASEA_ACT           11 //A相投切
#define YX_PHASEB_ACT           12  //B相投切
#define YX_PHASEC_ACT           13 //C相相投切
//#define YX_PHASEC_OFF           BITD  //最终投切成功
#define YX_EFS_LATCH            14//闭锁 1=闭锁 0=解锁
#define YX_UA_HIGH              15  //A相电压过压告警
#define YX_UA_LOW               16 //A相电压欠压告警
#define YX_UB_HIGH              17  //B相电压过压告警
#define YX_UB_LOW               18  //B相电压欠压告警
#define YX_UC_HIGH              19  //C相电压过压告警
#define YX_UC_LOW               20  //C相电压欠压告警
#define YX_PHASEA_OFF       21  //PT断线告警
#define YX_PHASEB_OFF       22  //PT断线告警
#define YX_PHASEC_OFF       23  //PT断线告警
#define YX_AEARTH_FAULT          24  //接地告警
#define YX_BEARTH_FAULT          25 //接地告警
#define YX_CEARTH_FAULT          26 //接地告警
#define YX_KJC         10 //硬遥信1
#define YX_KJB          32 //硬遥信1
#define YX_KJA          33 //硬遥信2
#define YX_RH_SUCCESS          34 //燃弧成功
#define YX_RH_FAIL         35 //燃弧失败
#define YX_BREAK            36 //断线
#define YX_LBOK              37 //录波完成
#define YX_SBP_OFF        38 //后备电源stand-by power 0：后备电源正常，1：后备电源异常
#define YX_RH_ACT          39//装置熄弧动作	1：熄弧开关投入，0：熄弧开关投入结束
#define YX_EFS_ACT        40//装置特征信号投入	1：处于投切工作状态；0：装置开关投切工作状态结束
#define YX_I0_ACT           41//接地电流反馈	   1：I0>0 ；0：I0=0
#define YX_PHASEA_ERR  42//A相开关位置异常	1：异常状态；0：
#define YX_PHASEB_ERR  43//B相开关位置异常	1：异常状态；0：
#define YX_PHASEC_ERR  44//C相开关位置异常	1：异常状态；0：
#define YX_I0_HIGH         45//I0过流	1：过流状态；0：
#define YX_KJ_TIMEOVER 46//开关闭合超时	1：超时；0：
#define YX_I0_TIMEOVER 47//I0超时	1：超时；0：
#define YX_SYSRESET 63//系统复位
//#define YX_I0_HIGH         48//I0过流	1：过流状态；0：
//#define YX_I0_HIGH         49//I0过流	1：过流状态；0：

//当电流为5A时，保留1个小数点，即实际值为50，则互感器和运放后进入AD为0.8V，对应的采样值为993  1311
#define COEF_AD_I_0                       1562     //5.00/993*4096 =  2062
//#define COEF_AD_I_0                     1100       //5.00/1862*4096 = 
#define COEF_I_0_AD2                    39110    //零序电流的反的系数的平方,由步长为0.01A的实际值换算成采样值的系数的平方 定标Q10，需要在程序中往右移位10 1.986*1.986*1024*10000/225

//电容器电压换算系数计算

#define COEF_AD_U                        33328  //电压系数44000   100.0V/931*4096    100.0V互感器和运放后进入AD为0.750V，对应的采样值为931 1229
#define COEF_U_AD2                      677    //2741=1.157*1.157*2048  0.5748*0.5748*2048= 677

//============================  保护电量        =======================================
#define PROC_MEAS_NUM           8


#define PM_U0                   0
#define PM_UA                   1
#define PM_UB                   2
#define PM_UC                   3
#define PM_UAB                  4       //零序电压
#define PM_UBC                  5
#define PM_UCA                  6
#define PM_I0                   7       //零序电流
//============================  LOG日志        =======================================

#define LOG_INFO_NUM            64//42

#define LOG_RESET               1  //功能投退
#define LOG_101_LINK        	2  //手动投切
#define LOG_101_ERR        		3 //最终投切成功
#define LOG_SELF_CHEK          	4  //接地告警
#define LOG_SOFT_ERR           	5  //PT断线告警
#define LOG_POW_ONF            	6  //控制器失电
#define LOG_UCAP              	7 //零序过压报警
#define LOG_PAR_CHAG          	8 //线电压过压报警
#define LOG_UPT_ERR           	9  //相电压过压告警
#define LOG_EARTH        		10 //最终投切成功
#define LOG_EARTH_TDELAY        11  //接地告警
#define LOG_8FULS_STA           12  //PT断线告警
#define LOG_8FULS_END           13  //控制器失电
#define LOG_LATCH               14 //零序过压报警
#define LOG_I0_ERR             	15 //线电压过压报警
#define LOG_KM_ERR              16  //相电压过压告警
#define LOG_BREAK              	17  //相电压过压告警

//============================  校正参数        =======================================
#define ADJ_PARA_NUM                            7//13   //校正参数包括每个通道的基准源校正和目标值校正，将来有功率计算的话还有相位校正



#define ADJ_REF_CHNUA                         0       //通道5基准源校正值 
#define ADJ_REF_CHNUB                         1       //通道6基准源校正值  
#define ADJ_REF_CHNUC                         2       //通道7基准源校正值  
#define ADJ_REF_CHNU0                         3       //通道4基准源校正值  
#define ADJ_REF_CHNI0                         4       //通道4基准源校正值  
#define ADJ_REF_CHNUPt                       5       //通道4基准源校正值  

#define ADJ_CRC                               6

//============================  运行参数        =======================================

#define RP_CFG_KEY               0      //配置控制字1

#define RP_BEAT_T                1    //定时上传心跳 步长1S
#define RP_SENDYC_T              2    //定时上传遥测时间间隔
#define RP_SENDYX_T               3   //定时上传遥信时间间隔
//#define RP_STATUSRP_INTERVAL     4   //状态汇报间隔
#define RP_FLOAD_T 			 4   //负荷记录存储时间间隔
#define RP_COMM_ADDR        	 5   //子站站地址 
#define RP_SYX_INFADDR    	 6    //单点遥信起始信息体地址
#define RP_DYX_INFADDR     	7    //双点遥信起始信息体?
#define RP_YCLIMIT              	 8   //遥测绝对越限值
#define RP_YCCAP                	 9   //遥测相对越限值 相对指百分比
#define RP_PLUSE_MODFK	   	10  //脉冲宽度修正，实际脉冲宽度为10ms为 RP_PLUSE_TIME -RP_PLUSE_MODFK 单位10ms 默认0
#define RP_PLUSEXH_MODFK	11  //脉冲宽度修正，实际脉冲宽度为10ms为 RP_PLUSE_TIME -RP_PLUSE_MODFK 单位10ms 
//#define RP_DEVICE_TYPE		11     //设备类型 0 = 一遥； 1 = 二遥 ；2=信号源
#define RP_YXRSD_T               12   //遥信重发间隔步长
#define YX_delay                 13   //防抖时间间隔

#define RP_UA_ADJ           14       //UA校准系数值
#define RP_UB_ADJ           15       //UB校准系数值
#define RP_UC_ADJ           16               //UC校准系数值
#define RP_U0_ADJ           17               //U0校准系数值
#define RP_I0_ADJ           18               //I0校准系数值
#define RP_I0_START         19              //线电压过压告警定值 //零序电流门槛值
#define RP_HIGH_P           20               //相电压高定值
#define RP_LOW_P            21               //相电压低定值
#define RP_HIGH_Z           22         //零序电压高定值
#define RP_LOW_Z            23          //零序电压低定值
#define RP_NO_V            24      //无压门槛值
#define RP_T_DELAY         25              //故障延时时间
#define RP_JAG_P           26      //相判据标志
#define RP_JAG_Z           27      //零序判据标志
#define RP_EFS_MODEL       28      //信号源型号标识
#define RP_JAG_ACT         29      //动作相序判据
#define RP_PULSE_VALID     30      //8脉冲有效定值
#define RP_GPRS_MODEL      31      //GPRS方式定值
#define RP_REV_CURRENT     32      //翻转电流定值
#define RP_U0_CAL          33      //U0产生方式
/*
#define RP_ETH_MODEL            20      //以太网方式 0：SERVER   1：CLIENT
#define	RP_G_IP_1               21		//网关IP1~4
#define	RP_G_IP_2		22
#define	RP_G_IP_3		23
#define	RP_G_IP_4		24
#define	RP_SUB_MASK_1		25       //子网掩码1~4
#define	RP_SUB_MASK_2		26
#define	RP_SUB_MASK_3		27
#define	RP_SUB_MASK_4		28
#define	RP_MAC_ADDR_1		29       //W5500 MAC地址1~6
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
#define	RP_S0_PORT		43        //Socket0 端口号 
*/
#define RP_CNL_MODEL	        34		//接地功能投退
#define RP_LACTH_TIME           35//信号源闭锁时间，以分钟为单位，默认18分钟
#define RP_PLUSE_TIME           36//脉冲宽度，10ms为单位 默认25
#define RP_SEND_TIME1           37//脉冲发送间隔时间1 ，10ms为单位 默认100
#define RP_SEND_TIME2           38//脉冲发送间隔时间2 ，10ms为单位 默认125

#define RP_PLUSE_NUM       39//脉冲个数，6 ，7，8
#define RP_OVERLOAD_I            40
#define RP_OVERLOAD_T           41
#define RP_SENDSMS_T             42//张弢 遥测起始地址修改运行参数
#define RP_YC_INFADDR            43//张弢 遥测起始地址修改运行参数
#define RP_SENDZJ_T                    44      //定时上传自检时间间隔
#define RP_RHSEND_TIME1           45//燃弧脉冲发送间隔时间1 ，10ms为单位 默认500
#define RP_RHPLUSE_TIME2           46//燃弧脉冲宽度，10ms为单位 默认20
#define RP_RHT_DELAY           47//燃弧遥信延时时间
#define RP_CT_TRANS           48//ct变比
#define RP_KM_TIME           49//接触器开关闭合超时时间，10ms为单位 默认100
#define RP_PLUSE_AMODFK	   	50  //脉冲宽度修正，实际脉冲宽度为10ms为 RP_PLUSE_TIME -RP_PLUSE_MODFK 单位10ms 默认0
#define RP_PLUSE_BMODFK	   	51  //脉冲宽度修正，实际脉冲宽度为10ms为 RP_PLUSE_TIME -RP_PLUSE_MODFK 单位10ms 默认0
#define RP_PLUSE_CMODFK	   	52  //脉冲宽度修正，实际脉冲宽度为10ms为 RP_PLUSE_TIME -RP_PLUSE_MODFK 单位10ms 默认0

#define RP_CRC                   53      //crc校验码//张弢 遥测起始地址修改运行参数
#define RUN_PARA_NUM             54//张弢 遥测起始地址修改运行参数

//控制字具体定义
#define RPCFG_CON_NC              0      //接触器位置0=常开no 1=常闭nc
//AD采集立即上传全遥测
#define RPCFG_SENDLUBOML         1 //云南 是否主动上传录播目录
#define RPCFG_SEND_FTYC             2      //初始化过程发送链接 1=不发送0=发送
#define RPCFG_SEND_CHANGEYC         3     //是否发送遥测越限值 0=不传 1=上传
#define RPCFG_YC_FLOAT             4      //遥测值为浮点数 0=整数 1=浮点数(类型标示)
#define RPCFG_AUTOLINK    5     //是否1=上电后GPRS自动连接101
#define RPCFG_DEL_LUBO              6      //是否删除录波数据 
#define RPCFG_ISSENDINITFIN         7     //0=不发送初始化长帧，1=发送 默认发送
#define RP_COMTRADE_TYPE             8     //0=录波数据为ASCII,1=录波数据二进制，
#define RPCFG_PT_LATCH                        9     // 1=启用PT断线闭锁，
#define RPCFG_I0_LATCH                        10     // 1=启用I0过流闭锁
#define RPCFG_CON_LATCH                     11     // 1=启用开关闭合超时闭锁?#define RPCFG_SEND_RHPLUSE         12     //0=不启动燃弧脉冲，1=发送燃弧脉冲 
#define RPCFG_BREAK_STOP8PUL     13     //0=断线后不停止8脉冲，1=断线后停止8脉冲 
#define RPCFG_CURRENT_PRIMARY   14     //0=电流为2次值，1=电流为1次值，2次值*CT变比
#define RPCFG_ENCRYPT			  15// 1=加密

//================================  定时器变量  ==========================

#define TIMER_NUM            19  //定时器个数 1s为单位
#define TM_BEAT               0     //定时上传心跳 GPRS通信心跳时间 步长1S
#define TM_ADCOLT             1   //AD采集时间间隔 此时置AD采集位，1s后使能AD采集
#define TM_SENDYC             2   //定时上传遥测时间间隔
#define TM_CLOSECHARGE        3   //关闭充电时间
#define TM_GPRSPWOERDOWN      4   //GPRS掉电时间，用于准实时在线模式
#define TM_SAVELOAD           5   //保存负荷电流
#define TM_ODU_call2          6   //召唤ODU的二级数据
#define TM_ODU_call           7   //召唤ODU数据
#define TM_BATPWOERDOWN       8   //锂电池关断时间
#define TM_SMSSENDALLOW       9  //短信发送延迟
#define TM_WAITXINHAO         10  //短信发送至信号源的等待时间
#define TM_ADBAT              11  //锂电池采样时间?#define TM_ADBATTEN      11  //锂电池的使能AＤ采样
#define TM_Volt               12  //电压不稳时，检测电源模式的时间间隔
#define TM_SENDSTATUSDELAY    13  //状态汇报延时
#define TM_SENDADDR           14  //子站地址发送延时
#define TM_SENDFAULTDELAY     15  //故障主动发送延迟 用于无限制模式
#define TM_SENDSTATE          16  // 发送ODU的状态汇报周期
#define TM_SNTP_TIME          17  //定时SNTP对时间隔
#define TM_GPRS_RW            18  //GPRS读写超时
                                    

//============================  校准系数及参数       =======================================
#define PROC_CNT_NUM            28

#define PC_UA_ADJ           0       //UA校准系数值
#define PC_UB_ADJ           1       //UB校准系数值
#define PC_UC_ADJ           2               //UC校准系数值
#define PC_U0_ADJ           3               //U0校准系数值
#define PC_I0_ADJ           4               //I0校准系数值
#define PC_I0_START         5               //线电压高定值//零序电流门槛值
#define PC_HIGH_P           6               //相电压高定值
#define PC_LOW_P            7               //相电压低定值
#define PC_HIGH_Z           8         //零序电压高定值
#define PC_LOW_Z            9          //零序电压低定值
#define PC_NO_V            10      //无压门槛值
#define PC_T_DELAY         11              //故障延时时间
#define PC_JAG_P           12      //相判据标志
#define PC_JAG_Z           13      //零序判据标志
#define PC_EFS_MODEL       14      //信号源型号标识
#define PC_JAG_ACT         15      //动作相序判据
#define PC_PULSE_VALID     16      //8脉冲有效定值
#define PC_GPRS_MODEL      17      //GPRS方式定值
#define PC_REV_CURRENT     18      //翻转电流定值
#define PC_U0_CAL          19      //U0产生方式

#define PC_CNL_MODEL	        20		//控制方式
#define PC_LACTH_TIME           21//信号源闭锁时间，以分钟为单位，默认18分钟
#define PC_PLUSE_TIME           22//脉冲宽度，10ms为单位 默认25 
#define PC_SEND_TIME1           23//脉冲发送间隔时间1 ，10ms为单位 默认100
#define PC_SEND_TIME2           24//脉冲发送间隔时间2 ，10ms为单位 默认125

#define PC_OVERLOAD_CNT         25
#define PC_OVERLOAD_I           26
#define PC_OVERLOAD_T           27

/*
#define RP_ETH_MODEL            20      //以太网方式 0：SERVER   1：CLIENT
#define	RP_G_IP_1               21		//网关IP1~4
#define	RP_G_IP_2		22
#define	RP_G_IP_3		23
#define	RP_G_IP_4		24
#define	RP_SUB_MASK_1		25       //子网掩码1~4
#define	RP_SUB_MASK_2		26
#define	RP_SUB_MASK_3		27
#define	RP_SUB_MASK_4		28
#define	RP_MAC_ADDR_1		29       //W5500 MAC地址1~6
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
#define	RP_S0_PORT		43        //Socket0 端口号 
*/


//#define RUN_INFO_NUM            22        //包括站名称与站地址共21个字
#define RUN_INFO_NUM            20        //包括站名称与站地址共20个字
#define INF_CRC                 20
//============================  开关量输入  ==================================
#define DIN_NUM                 3     //总共的开关量输入个数

#define DIN_CON_TIME           160     //遥信确认时间，暂定200ms 
//============================  实时时钟        ==================================
#define RTC_NUM                         7

#define RTC_YEAR                        0       //年
#define RTC_MONTH                       1       //月
#define RTC_DATE                        2       //日
#define RTC_HOUR                        3       //时
#define RTC_MINUT                       4       //分
#define RTC_SEC                         5       //秒
#define RTC_MICROSEC                    6       //毫秒

//=============================   遥控物理出口类型 =========================

#define TYPE_RMTDO_NUM		5      //通信点表的配置

#define RMTDO_MONI_A			0   //模拟A相故障
#define RMTDO_MONI_B		        1   //模拟B相故障
#define RMTDO_MONI_C			2   //模拟C相故障
     
#define RMTDO_CLEAR_SOE			3    //清除SOE记录

#define RMTDO_RESET			4    //远动复位

//============================  断线时间记录 ===========================================
#define         BK_EEP_NUM              25      //在EEPROM中保存的数量 25条
#define         BK_DATA_LEN_BYTE        9      //每条SOE数据所占的字节数
//============================  掉电时间记录 ===========================================
#define         OFF_EEP_NUM              30      //在EEPROM中保存的数量 30条
#define         OFF_DATA_LEN_BYTE        7      //每条SOE数据所占的字节数
//============================  状态异常时间记录 ===========================================
#define         ABN_EEP_NUM              30      //在EEPROM中保存的数量 30条
#define         ABN_DATA_LEN_BYTE        7      //每条SOE数据所占的字节数
//============================  故障记录 ===========================================
#define         SOE_EEP_NUM              30      //在EEPROM中保存的数量 30条
#define         SOE_DATA_LEN_BYTE        12     //每条SOE数据所占的字节数
//============================  录波数据记录 ===========================================
#define         WAVE_EEP_NUM              30      //在EEPROM中保存的数量 30条
#define         WAVE_DATA_LEN_BYTE        1     //每条SOE数据所占的字节数

//SOE数据的定义

//===============================  COMM模块  =============================
#define COMM_PORT_NUM           3//物理通信端口个数


#define COMM_INDEX_000          0
#define COMM_INDEX_232          1
#define COMM_INDEX_GPRS         2



//#define MAX_TXBUFF_LEN       128//无论使用哪一种协议，最后发送时使用的缓冲区相同，因此缓冲区长度取最大值。
#define MAX_RXBUFF_LEN       256//无论使用哪一种协议，最后发送时使用的缓冲区相同，因此缓冲区长度取最大值。

#define MAX_TXBUFF_101_LEN       256//101协议发送缓冲区长度
#define MAX_SOE_TXNUM            8//16//单次上送SOE条数

//============================  SOE记录 ===========================================
#define         SOE_BUFF_NUM            32      //SOE内存缓冲区中SOE的条数

#define         SOE_DATA_LEN            11       //每条SOE数据的长度
#define         SOEVALID               0X55
#define         SOE_SENDDA_LEN         11//SOE_DATA_LEN-1
//SOE数据的定义
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

//============================  内部flash存储  ===============================

#define SOE_RECORD_ADD   0x42000     //SOE记录地址区

#define SOE_NUM_SEG   4//42
#define SOE_SEG       24 

#define SOE_RECORD_LEN  0x10        //每条记录16字节
#define SOE_RECORD_NUM  32          //32条记录

//====================  EEPROM 地址分配  CAT25C128 (0~0x3FFF 字节) 16K字节 ======================
//由于CAT25128的页为64字节，则分配空间的起始地址最好为页起始
//EEPROM中保存配置区的参数      分配1K字节的空间
#define EEPADD_BIAS_V               0x0000    //偏置电压系数等10字节   占16字节
#define EEPADD_COM                  0x0010    //站名称、站地址等42字节 占48字节
#define EEPADD_PHONE_NUM            0x0040    //4个手机号码（每个13位）及手机短信属性57字节  占64字节
#define EEPADD_PHONE                0x0040	//张弢 0328 为和林雪琴程序兼容，==EEPADD_PHONE_NUM 
//N(N最大为8，目前程序定死为4个)个电话号码( 1字节电话号码长度 +15字节电话)，N字节短信发送控制字//张弢 0328 
//共136字节//张弢 0328 
#define EEPADD_SOE_E2ROMADR            0x0100//0x0080//张弢 头4字节，尾4字节 长度2字节
#define EEPADD_SOESEND_E2ROMADR   0x0110
#define EEPADD_CNNAME 			 0x0140//张弢 汉字站名42字节
#define EEPADD_ADJOBJ                   0x0180//张弢 目标校准，上位机下载参数10字节 初始值为电压60V,电流2A
#define EEPADD_VER_CPU                0x01a0          //程序版本号 2字节
//EEPROM中IEC参数保存
#define EEPADD_IECYXADDR        0x01b0 //遥信地址  32字节
#define EEPADD_IECPARA             0x0200 //IEC参数   32byte
#define EEPADD_IECYCADDR        0x0240 //遥测参数  100byte 
#define EEPADD_IECYKADDR        0x0280 //遥控参数  100byte
#define EEPADD_RP                      0x02c0
#define EEPADD_LBNAME             0x03c0
#define EEPADD_LBNAMELEN       0x03e0
//EEPROM中参数的备份区  分配1K字节的空间
#define EEPADDBK_BIAS_V               0x0400    //偏置电压系数等10字节   占16字节
#define EEPADDBK_COM                    0x0410    //站名称、站地址等42字节 占48字节
#define EEPADDBK_PHONE_NUM       0x0440    //4个手机号码（每个13位）及手机短信属性57字节  占64字节
#define EEPADDBK_PHONE                0x0440
#define EEPADDBK_SOE_E2ROMADR         0x0500//0x0080//张弢 头4字节，尾4字节 长度2字节 
#define EEPADDBK_CNNAME 		  0x540//张弢 汉字站名42字节
#define EEPADDBK_ADJOBJ               0x0580//张弢 目标校准，上位机下载参数10字节 初始值为电压60V,电流2A
#define EEPADDBK_IECYXADDR        0x05b0
#define EEPADDBK_IECPARA            0x0600
#define EEPADDBK_IECYCADDR        0x0640
#define EEPADDBK_IECYKADDR        0x0680
#define EEPADDBK_RP                      0x06c0

#define EEPADD_LOADNUM               0x0720   
#define EEPADDBK_LOADNUM           0x0730

#define EEPADD_LOGP               0x0740   
#define EEPADDBK_LOGP           0x0750


//EEPROM中断线、掉电、异常（保护开关跳开）时间记录  分配1K字节的空间
#define EEPADD_TIME_BK_NUM        0x0800          //断线时间总条数  1
#define EEPADD_TIME_BK_POS        0x0801          //断线时间位置数  1
#define EEPADD_TIME_BK            0x0802          //断线时间值     225 字节  占256字节
#define EEPADD_TIME_OFF_NUM       0x0900          //掉电时间总条数  1
#define EEPADD_TIME_OFF_POS       0x0901          //掉电时间位置数  1
#define EEPADD_TIME_OFF           0x0902          //掉电时间值 212 字节  占256字节
#define EEPADD_TIME_ABN_NUM       0x0a00          //状态异常时间总条数  1
#define EEPADD_TIME_ABN_POS       0x0a01          //状态异常时间位置数  1
#define EEPADD_TIME_ABN           0x0a02          //状态异常时间值 212 字节  占256字节

//EEPROM中故障记录  分配1K字节的空间
#define EEPADD_SOE_NUM          0x0c00          //故障记录总条数  1
#define EEPADD_SOE_POS          0x0c01          //故障记录位置数  1
#define EEPADD_SOE_DATA         0x0c02          //故障记录 360字节


//EEPROM中录波数据  分配8K字节空间
#define EEPADD_REC_NUM          0x0d00           //录波数据的总条数
#define EEPADD_REC_POS          0x0d02           //最新录波数据所在的相对位置
#define EEPADD_REC_DATA         0x0d04           //录波数据校验


//E2中保存录波的个数
#define EEPADD_LUBONUM          0x0E98 
#define EEPADD_ALEN             0x0E9A //A相的偏移量
#define EEPADD_BLEN             0x0E9C //B相的偏移量
#define EEPADD_CLEN             0x0E9E //C相的偏移量
#define EEPADD_COMLEN           0x0EA0 //最大偏移量


#define EEPADD_VerNum            0x1104 // 当前录波的总个数,占用8个字节，SV01.033
#define EEPADD_LOGNUM           0x1110  //在FLASH中保存的负荷记录的条数 占用2字节
#define EEPADD_LOGADDR_NEW      0x1112  //即将存储的记录与第一条记录的偏移位置，为(0----FLASH_LOAD_MAXNUM-1) 占用2字节
#define EEPADD_LOGADDR_OLD      0x1114  //最老一条记录存储位置
#define EEPADD_LOG_CHK          0x1116  //以上两个数据加和校验
#define EEPADD_DAYNUM           0x1200 //定点负荷记录共存31天，每天最多存96条，实际存的条数
#define EEPADD_VALIDITY         0x1300 //有效期，64条录波目录，每条录波目录的有效期为2个字节，共2*64个字节
#define EEPADD_FIXNUM           0x1400 //通过文件名保存定点的索引，共31*20个字节
#define EEPADD_CFG              0x1700 //保存CFG文件，目前CFG文件最多占用700个字节，没有写入FLASH的原因是:每次发送该文件时，都需要对CFG文件的某几个地方做修改，方便操作
#define EEPADD_DEGBF101            0x1A00 // DEG口 读写报文时，备份当前进行的命令。读写文件时，若被其它的进程包括总召、对时、主动上报等命令打断时，可以继续传输
#define EEPADD_GPRSBF101           0x1B00 // GPRS口 读写报文时，备份当前进行的命令。读写文件时，若被其它的进程包括总召、对时、主动上报等命令打断时，可以继续传输

#define EEPADD_SOESTARTADR       0x2000 //最大偏移量
#define EEPADD_SOEENDADR       0x4000
//============================   GPIO对应LED定义  ===============================
#define WDG_SET         (P5OUT |= BIT3)         //看门狗置位    同时指示运行灯闪烁
#define WDG_CLR         (P5OUT &= NBIT3)        //看门狗清零

#define LED_RUN_ON      (P6OUT |= BIT0)        //LED00 运行灯 P6.0
#define LED_RUN_OFF     (P6OUT &= NBIT0) 
#define LED_RUN_TOGG    (P6OUT ^= BIT0)


//============================   GPIO对应DO定义  ===============================
#define KA1_OFF     (P4OUT &= NBIT6)                        //异常状态恢复
#define KA1_ON      (P4OUT |= BIT6)                         //异常状态报警
#define KB1_OFF     (P4OUT &= NBIT7)                        //信号源掉电恢复
#define KB1_ON      (P4OUT |= BIT7)                         //信号源掉电报警
#define KC1_OFF     (P5OUT &= NBIT4)                        //没有有效8脉冲
#define KC1_ON      (P5OUT |= BIT4)                         //发出有效8脉冲

#define KA0_OFF     (P7OUT &= NBIT2)                        //A相接触器截止
#define KA0_ON      (P7OUT |= BIT2)                         //A相接触器导通
#define KB0_OFF     (P7OUT &= NBIT3)                        //B相接触器截止
#define KB0_ON      (P7OUT |= BIT3)                         //B相接触器导通
#define KC0_OFF     (P8OUT &= NBIT0)                        //C相接触器截止
#define KC0_ON      (P8OUT |= BIT0)                         //C相接触器导通

#define   ABN_CHECK           BIT0          //abnormal self checked
#define   POWER_OFF           BIT1          //power down
#define   EIGHT_PULSE         BIT2          //eight pulse omit    
#define   SEND_AUTO           BIT3          //upload regulary
#define   FAULT_RST           BIT5          //fault recovery
#define   FAULT_OCCUR         BIT4          //fault occcur
#define   PHASE_BREAK         BIT6          //断线发生
#define   ZIZHAN_XUN          0x80          //子站询问
#define   PHONE_XUN          0xc0          //子站询问

#define NUM_10H  0x10
#define NUM_68H  0x68

#define   FEED_WATCH_DOG()            P5OUT ^= BIT3;

#define   LIGHT_RUN_UP()              P6OUT |= BIT0;
#define   LIGHT_RUN_DOWN()            P6OUT &= NBIT0;
#define   LIGHT_RUN_FLASH()           P6OUT ^= BIT0;

#pragma location=0xFF00
const unsigned char ucVerNum[]="Ver02.02";

//****************** command_flag *************************
//************* 终端主动发送数据时 的发送原因*****************//>0x68:可变帧。>0x10：固定帧
#define ZONGZHAO_CONFIRM  0x71    //    总召-确认
#define ZUZHAO_CONFIRM    0x72    //    组招-确认
#define ZONGZHAO_STOP     0x73    //    总召-停止
#define ZUZHAO_STOP       0x74    //    组招-确认
#define CESHI_CONFIRM     0x75    //    测试-确认
#define FUWEI_CONFIRM     0x76    //    复位-确认
#define FUWEI_STOP        0x77    //    复位-停止

#define YANSHI_CONFIRM    0x79    //    延时-确认
#define TONGBU_CONFIRM    0x7A    //    时钟同步-确认
#define TONGBU_STOP       0X7B    //    发送对时之前的时钟

#define CHUSHIHUA_FINISH  0xA0    //    初始化-完成

#define YAOXIN_DAN_ALL    0X7C    //    以全部 单点-遥信 响应
#define YAOCE_ALL         0X7D    //    以 全部遥测 响应
#define YAOXIN_SHUANG_ALL 0x7E    //    以全部 双点-遥信 响应

#define CHUSHIHUA_JIESHU  0x8E    //    初始化结束-确认
#define GGDIZHI_RONGCUO   0x90    //    公共地址容错
#define COT_RONGCUO       0x91    //    未知 传送原因
#define TYP_RONGCUO       0x92    //    未知 类型标记

#define QINGQIULIANLU     0x11    //    请求链路
#define FUWEIYUANFANG     0x12    //    复位远方

#define LINK_START      0x13    //    链路启动
#define LINK_TEST       0x14    //    链路测试
#define LINK_STOP       0x15    //    链路停止
#define LINK_ACK        0x16    //    肯定确认
#define LINK_TEST_SEND  0x18    //    链路测试 发送
#define LINK_STOP_SEND  0x19    //    链路停止 发送
#define LINK_START_SEND 0x17    //    链路启动 发送

//****************** process_type *************************
//************** 终端与主站之间 功能类型 ******************
#define CHUSHIHUA         0X01    //  初始化
#define ZONGZHAO_INIT     0X02    //  总召（初始化）
#define YANSHI            0X03    //  延时
#define ZONGZHAO          0X04    //  总召
#define TONGBU            0X05    //  时钟同步
#define CESHI             0X06    //  测试
#define FUWEI             0X07    //  复位  
#define ZUZHAO_YX         0X08    //  组招_遥信
#define ZUZHAO_YC         0X09    //  组招_遥测
#define ZUZHAO_YXD        0X0A    //  组招_双点遥信


//****************** 优先级定义 *************************
#define INIT_PRIORITY         0x38  //  初始化
#define ZONGZHAO_I_PRIORITY   0x36  //  总召（初始化）

#define YAOXIN_PRIORITY       0x30  //  遥信
#define YANSHI_PRIORITY       0x28  //  延时 获得
#define ZONGZHAO_PRIORITY     0x26  //  总召

#define YXSOE_PRIORITY        0x20   // 遥信SOE

#define YC_PRIORITY           0x10   // 遥测值
#define TONGBU_PRIORITY       0x09  //  时钟同步
#define CESHI_PRIORITY        0X08  //  测试

#define FUWEI_PRIORITY        0X06  //  复位

#define ZUZHAO_YX_PRIORITY    0X04  //  组招 遥信
#define ZUZHAO_YC_PRIORITY    0x04  //  组招  遥测

#define GGDIZHI_PRIORITY      0x02  //  公共地址容错
#define COT_PRIORITY          0x02  //  未知 传送原因
#define TYP_PRIORITY          0x02  //  未知 类型标记

//****************** GPRS101 和 SMS 状态切换*************************
#define GPRSState_IDLE           0x00
#define GPRSState_GPRS101    0x01
#define GPRSState_SMS            0x02
//FLASH中录波数据        从第1200页开始，保存640页 每16页(3840)存一只指示器的一条录波数据，16*4=64页存一整条完整的录波数据，可存10条完整的录波数据

#define FADDR_RECORDER_START         0x4B000   //电流录波起始地址         

#define FRECORDER_TOLNUM      21  //存入外部flash的录波总数。20条完整的录波
//#define FRECORDER_PNUM        16    //每16页存一条录波数据
#define WRITEPECORDER  228//写入包数据个数 基地址（3）+偏移地址（3）+数据量（1）+数据内容（224）+累加和（1）
//#define FLINEADDR  (64*256)
#define FLINEADDR  (16*256)
#define FPHASEADDR (16*256)

#define FADDR_RECORDER_STARTV        0x9C000   //电压录波起始值地址 
#define RECORDER  (80*15)

#define FADDR_RECORDER_INFO       0x10000   //录波目录信息 A20*100A
#define FADDR_RECORDER_DATA      0x4000   //故障录波数据0x4000
#define FADDR_RECORDER_ACTDATA      0x70000   //动作录波数据0x70000
#define FADDR_RECORDER_XHDATA      0x6C0000   //熄弧录波数据0x6C0000
#define FADDR_LOG_START      0x720000   //LOG数据0x720000-7290000
#define FADDR_LOG_END      0x729000  
#define MAX_LOG_NUM       4//内存中LOG的数量

/*
程序升级7AF000
负荷记录//750000-790000   每条32字节 共8192条
熄弧录波//6C0000-710000  每条2万字节、0.5个BLOCK，共10条
动作录波//70000-6B0000  每条60万字节、9个BLOCK，共10条
故障录波//4000-68000  每条6400字节、2个Sector(0x2000)，共50条
soe//0-3000 每条16字节共768条
*/
/*
0x0         -  0x3000     存储soe，每条soe16字节，存512条
0x10000  -  0x30000   存gRecorder_cfg，包括录波文件名，录波文件长度，录波时间，每条占1个
sector-4096字节

0x100000-  0x300000   存故障录波，5个遥测，8个周波，每周波80点，共640点，6400字节，每条占4个block-0x10000字节
                                   存动作录波，5个遥测，10-25秒，每秒800点，最多20000点，20万字节，每条4个block-0x50000
*/
#define RECORDER_INFO_LENGTH      100      

//#define FADDR_RECORDER_VINFO       0xF4000   //录波目录信息 20*100
//#define FADDR_RECORDER_VINFOB       0xF5000   //录波目录信息 20*100
//#define FADDR_RECORDER_VINFOC       0xF6000   //录波目录信息 20*100
//#define FADDR_RECORDER_VINFO0       0xF7000   //录波目录信息 20*100



#define RECORDER_MULU_LENGTH      13  //回复每条录波目录的长度

#define FADDR_RECORDER_START_BK         0x100000   //电流录波起始地址 
#define FADDR_RECORDER_STARTV_BK        0x151000   //电压录波起始值地址 


//#define FSOE_LEN         16    //没条SOE占16字节 内存中每条占11字节，这里为了flash中容易处理才预留出5字节

//===============================入口标志数据及备份校验存储区================
#define FLASH_UPDATE_COUNTER     0x7AF000 //包总数
#define FLASH_UPDATE_CHECK       0x7AF002 //校验值
#define FLASH_UPDATE_STATE       0x7AF004 //升级状态
#define FLASH_UPDATE_EARASE       0x7AF005//内部flash擦除表示用于表示升级过程中是否已经将内部flash擦除，以防止在擦出以后改变升级状态而重启后去寻找原有程序入口
#define FLASH_UPDATE_CCHECK       0x7AF006 //校验
//备份
#define FLASH_UPDATEBK_COUNTER   0x7AF008 //包总数
#define FLASH_UPDATEBK_CHECK     0x7AF00A //校验值
#define FLASH_UPDATEBK_STATE     0x7AF00C //升级状态
#define FLASH_UPDATEBK_EARASE     0x7AF00C //flash擦除标志
#define FLASH_UPDATEBK_CCHECK     0x7AF00D //校验

//===================================应用程序存储区===========================
#define FLASH_UPDATE_START      0X7B0000       //
#define FLASH_UPDATE_END        0X7EFFFF       //
#define FLASH_UPCOUNTER          0x7F0000//无效升级次数      


#define ERASE_DELAY 30000//擦除与写入间隔延时，太小则会造成写入错误（全1）
#define ERASEFLAGADDR   0x45A00//内部flash地址，jtag烧写程序标志首地址,使用6个字节表示，在程序下载完成之后全部写为0x55

#endif

/*
程序升级7AF000
负荷记录//750000-790000   每条32字节 共8192条
熄弧录波//6C0000-710000  每条2万字节、0.5个BLOCK，共10条
动作录波//70000-6B0000  每条60万字节、9个BLOCK，共10条
故障录波//4000-68000  每条6400字节、2个Sector，共50条
soe//0-3000 每条16字节共768条
*/

