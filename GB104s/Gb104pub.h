/*------------------------------------------------------------------------
 Module:			gb104pub.h
 Author:			linxueqin
 Project:			规约平台
 State: 			V1.0
 Creation Date: 	2012-10-8
 Description:		GB104规约--公共声明
------------------------------------------------------------------------*/

#ifndef GB104PUB_H
#define GB104PUB_H

#define	M_SP_NA		1 	//不带时标的单点信息
#define	M_SP_TA		2 	//带时标的单点信息(三个八位位组二进制时间)
#define	M_DP_NA		3 	//不带时标的双点信息*
#define	M_ST_NA		5 	//步位置信息*
#define	M_BO_NA		7 	//子站远动终端状态*
#define	M_ME_NA		9 	//归一化测量值
#define	M_ME_NB		11	//标度化测量值
#define M_ME_NC		13	//测量值-短浮点数
#define	M_IT_NA		15 	//电能脉冲记数量*
#define	M_PS_NA		20 	//具有状态变位检出的成组单点信息*
#define	M_ME_ND		21 	//不带品质描述的归一化测量值

#define M_SP_TB   30  //带长时标的单点信息
#define M_DP_TB   31  //带长时标的双点信息*
#define M_ST_TB   32  //带长时标的步位置信息*
#define M_BO_TB   33  //带长时标的32位位串*
#define M_ME_TD   34  //带长时标的测量值*

#define	M_EI_NA		70 	//初始化结束

#define	C_SC_NA		45 	//单点遥控命令
#define	C_DC_NA		46 	//双点遥控命令
#define	C_RC_NA		47 	//升降命令
#define	C_SE_NA		48 	//设定命令 规一化值
#define  C_SE_NB      49   //设定命令 标度化值

#define	C_IC_NA		100 	//召唤命令
#define	C_CI_NA		101 	//电能脉冲召唤命令*
#define	C_RD_NA		102 	//读数据命令
#define	C_CS_NA		103 	//时钟同步命令*
#define C_TS_NA     104   	//测试命令*
#define	C_RP_NA		105 	//复位进程命令
#define C_TS_TA     107   	//带时标的测试命令*
#define C_SE_TD     137     //多点设定命令
#define	C_RD_NA_1		138 	//读数据命令 多个参数（扩展）

#define P_ME_NA     111   	//参数 110=归一化值 111=标度化值


#define GB104S_YC_TYPE    M_ME_NB //遥测类型选择 9，11,13,21
//#define M_SP_SOE_TYPE     M_SP_TB  //选择是用单点SOE还是双点SOE,还是短时标的单点SOE

//初始化原因
#define COI_NETFT            32 //网络故障，比如掉网线
#define COI_PEERCLOSE        33 //对端关闭连接
#define COI_IACKTIMEOUT      34 //发了I帧后T1时刻未收到ACK确认，挂断连接
#define COI_UTIMEOUT         35 //发了U帧后T1时刻未收到对端回复，挂断连接
#define COI_NETIDLETIMEOUT   36 //建立SOCKET后T6时间未收到对端报文，挂断连接
#define COI_STOPDT_RCVI      37 //收到对端发的U-STOP命令后又收到I帧，则挂断连接
#define COI_RCVNUMERR        38 //接收序号错误，挂断连接
#define COI_RESETRTU         39 //收到对端的复位RTU指令






//传送原因：
#define	COT_PERCYC	  	1 	//周期/循环
#define	COT_BACK		2 	//背景扫描
#define	COT_SPONT		3 	//突发
#define	COT_INIT	  	4 	//初始化
#define	COT_REQ		    5 	//请求或被请求
#define	COT_ACT		    6 	//激活
#define	COT_ACTCON		7 	//激活确认
#define	COT_DEACT		8 	//停止激活
#define	COT_DEACTCON	9 	//停止激活确认
#define	COT_ACTTERM		10 	//激活结束
#define	COT_RETREM		11 	//远程命令引起的返送信息
#define	COT_RETLOC		12 	//当地命令引起的返送信息
#define	COT_FILE		13 	//文件传送

#define	COT_INTROGEN	20 	//响应总召唤
#define	COT_INRO1		21 	//响应第1组召唤
#define	COT_INRO2		22 	//响应第2组召唤
#define	COT_INRO3		23 	//响应第3组召唤
#define	COT_INRO4		24 	//响应第4组召唤
#define	COT_INRO5		25 	//响应第5组召唤
#define	COT_INRO6		26 	//响应第6组召唤
#define	COT_INRO7		27 	//响应第7组召唤
#define	COT_INRO8		28 	//响应第8组召唤
#define	COT_INRO9		29 	//响应第9组召唤  yc
#define	COT_INRO10		30  //响应第10组召唤
#define	COT_INRO11		31 	//响应第11组召唤
#define	COT_INRO12		32 	//响应第12组召唤
#define	COT_INRO13		33 	//响应第13组召唤
#define	COT_INRO14		34 	//响应第14组召唤
#define	COT_INRO15		35 	//响应第15组召唤
#define	COT_INRO16		36  //响应第16组召唤
#define	COT_REQCOGCN	37  //响应计数量总召唤
#define	COT_REQCO1		38 	//响应第1组计数量召唤
#define	COT_REQCO2		39 	//响应第2组计数量召唤
#define	COT_REQCO3		40 	//响应第3组计数量召唤
#define	COT_REQCO4		41 	//响应第4组计数量召唤

#define COT_E_TYPE		44 /**<未知的类型标识*/
#define COT_E_REASON	45 /**<未知的传送原因*/
#define COT_E_CADDR	46 /**<未知的ASDU公共地址*/
#define COT_E_IADDR	47 /**<未知的信息对象地址*/


#define	COT_PN_BIT		0x40


	#define PARA_K		3//12
	#define PARA_W		2//8

#define PARA_T0		60 //1min
#define PARA_T1		12//ack and u-test u-start
#define PARA_T2		5
#define PARA_T3		15

#define PARA_T4      60  //for all yc
#define PARA_T5      5  //judge net status
#define PARA_T6      60  //socket idle time overtime

#define FRM_I		0
#define FRM_S		1
#define FRM_U		3

#define  STARTDT_ACT 0x7
#define  STARTDT_CON 0x0B
#define  STOPDT_ACT  0x13
#define  STOPDT_CON  0x23
#define  TESTFR_ACT  0x43
#define  TESTFR_CON  0x83
#define MAX_FRAME_COUNTER   0x7fff

#define START_CODE          0x68

#define MIN_RECEIVE_LEN  	6    //最小接收帧长
#define MAX_FRAME_LEN  		255
  
#define APCI_LEN            6
#define CONTROL_LEN			4

#define VSQ_NUM 		0x7f
#define VSQ_SQ  		0x80

#define YC_GRP_NUM		80//128 modify by lxq
#define YX_GRP_NUM		127//128 modify by lxq
#define DD_GRP_NUM		32	//dd num per group

#define YC_FRM_NUM		80	//max yc num = 120 per frame
#define YX_FRM_NUM		127	//max yx num = 127 per frame because of the VSQ bit8 not use
#define DD_FRM_NUM		20	//max dd num = 20 per frame because each dd len = 3(no)+4(val)+1(serial) = 8

#define REQ_SCOS_NUM		60	//max req scos num = 60 (ASDUINFO_LEN/4)
#define REQ_SSOE_NUM		22	//max req ssoe num = 22 (ASDUINFO_LEN/11)
#define REQ_CHANGEYC_NUM	48	//max req changeyc num = 48 (ASDUINFO_LEN/5)





	#define SCOS_LEN		4
	#define SSOE_LONG_LEN	11
	#define CHANGE_YC_LEN	5
	#define	INFO_ADR_LEN	3		


#define ETHERNET_COMM_ON	1
#define ETHERNET_COMM_OFF	0

struct VIec104Timer
{
	BOOL bRun;
	WORD wCounter;
	WORD wInitVal; 	
};

struct VIec104Frame
{
	BYTE byStartCode;
	BYTE byAPDULen;
	BYTE byControl1;
	BYTE byControl2;
	BYTE byControl3;
	BYTE byControl4;
	BYTE byASDU[MAX_FRAME_LEN-6];	
};


	struct VASDU
	{
		BYTE byTypeID;	   
		BYTE byQualifier;
		BYTE byInfo[MAX_FRAME_LEN - 8];	   
	};

struct VBackFrame
{
	WORD wSendNum;
	WORD wFrameLen;
	BYTE byBuf[MAX_FRAME_LEN];	
};

typedef struct{
	BYTE conaddrlen;/*应用层公共地址长度*/
	BYTE COTlen;/*传送原因长度*/
	BYTE infoaddlen;/*信息地址长度*/
	BYTE yxtype;/*1.不带时标；2.带时标；3带CP56Time2a时标*/
	BYTE yctype;/*9.测量值, 规一化值;10.带时标的测量值, 规一化值11.测量值, 标度化值12.测量值, 带时标的标度化值
	              13.测量值, 短浮点数14.测量值, 带时标的短浮点数21.测量值, 不带品质描述词的规一化值
	               34.带CP56Time2a时标的测量值, 规一化值35带CP56Time2a时标的测量值, 标度化值
	  36带CP56Time2a时标的测量值, 短浮点数 */
	//BYTE ddtype;/*15.累计量16带时标的累计量37带CP56Time2a时标的累计量*/
	WORD baseyear;/*1900或2000*/
	
}Vgb104para;/*104规约参数结构*/


#endif
