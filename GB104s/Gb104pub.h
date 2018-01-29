/*------------------------------------------------------------------------
 Module:			gb104pub.h
 Author:			linxueqin
 Project:			��Լƽ̨
 State: 			V1.0
 Creation Date: 	2012-10-8
 Description:		GB104��Լ--��������
------------------------------------------------------------------------*/

#ifndef GB104PUB_H
#define GB104PUB_H

#define	M_SP_NA		1 	//����ʱ��ĵ�����Ϣ
#define	M_SP_TA		2 	//��ʱ��ĵ�����Ϣ(������λλ�������ʱ��)
#define	M_DP_NA		3 	//����ʱ���˫����Ϣ*
#define	M_ST_NA		5 	//��λ����Ϣ*
#define	M_BO_NA		7 	//��վԶ���ն�״̬*
#define	M_ME_NA		9 	//��һ������ֵ
#define	M_ME_NB		11	//��Ȼ�����ֵ
#define M_ME_NC		13	//����ֵ-�̸�����
#define	M_IT_NA		15 	//�������������*
#define	M_PS_NA		20 	//����״̬��λ����ĳ��鵥����Ϣ*
#define	M_ME_ND		21 	//����Ʒ�������Ĺ�һ������ֵ

#define M_SP_TB   30  //����ʱ��ĵ�����Ϣ
#define M_DP_TB   31  //����ʱ���˫����Ϣ*
#define M_ST_TB   32  //����ʱ��Ĳ�λ����Ϣ*
#define M_BO_TB   33  //����ʱ���32λλ��*
#define M_ME_TD   34  //����ʱ��Ĳ���ֵ*

#define	M_EI_NA		70 	//��ʼ������

#define	C_SC_NA		45 	//����ң������
#define	C_DC_NA		46 	//˫��ң������
#define	C_RC_NA		47 	//��������
#define	C_SE_NA		48 	//�趨���� ��һ��ֵ
#define  C_SE_NB      49   //�趨���� ��Ȼ�ֵ

#define	C_IC_NA		100 	//�ٻ�����
#define	C_CI_NA		101 	//���������ٻ�����*
#define	C_RD_NA		102 	//����������
#define	C_CS_NA		103 	//ʱ��ͬ������*
#define C_TS_NA     104   	//��������*
#define	C_RP_NA		105 	//��λ��������
#define C_TS_TA     107   	//��ʱ��Ĳ�������*
#define C_SE_TD     137     //����趨����
#define	C_RD_NA_1		138 	//���������� �����������չ��

#define P_ME_NA     111   	//���� 110=��һ��ֵ 111=��Ȼ�ֵ


#define GB104S_YC_TYPE    M_ME_NB //ң������ѡ�� 9��11,13,21
//#define M_SP_SOE_TYPE     M_SP_TB  //ѡ�����õ���SOE����˫��SOE,���Ƕ�ʱ��ĵ���SOE

//��ʼ��ԭ��
#define COI_NETFT            32 //������ϣ����������
#define COI_PEERCLOSE        33 //�Զ˹ر�����
#define COI_IACKTIMEOUT      34 //����I֡��T1ʱ��δ�յ�ACKȷ�ϣ��Ҷ�����
#define COI_UTIMEOUT         35 //����U֡��T1ʱ��δ�յ��Զ˻ظ����Ҷ�����
#define COI_NETIDLETIMEOUT   36 //����SOCKET��T6ʱ��δ�յ��Զ˱��ģ��Ҷ�����
#define COI_STOPDT_RCVI      37 //�յ��Զ˷���U-STOP��������յ�I֡����Ҷ�����
#define COI_RCVNUMERR        38 //������Ŵ��󣬹Ҷ�����
#define COI_RESETRTU         39 //�յ��Զ˵ĸ�λRTUָ��






//����ԭ��
#define	COT_PERCYC	  	1 	//����/ѭ��
#define	COT_BACK		2 	//����ɨ��
#define	COT_SPONT		3 	//ͻ��
#define	COT_INIT	  	4 	//��ʼ��
#define	COT_REQ		    5 	//���������
#define	COT_ACT		    6 	//����
#define	COT_ACTCON		7 	//����ȷ��
#define	COT_DEACT		8 	//ֹͣ����
#define	COT_DEACTCON	9 	//ֹͣ����ȷ��
#define	COT_ACTTERM		10 	//�������
#define	COT_RETREM		11 	//Զ����������ķ�����Ϣ
#define	COT_RETLOC		12 	//������������ķ�����Ϣ
#define	COT_FILE		13 	//�ļ�����

#define	COT_INTROGEN	20 	//��Ӧ���ٻ�
#define	COT_INRO1		21 	//��Ӧ��1���ٻ�
#define	COT_INRO2		22 	//��Ӧ��2���ٻ�
#define	COT_INRO3		23 	//��Ӧ��3���ٻ�
#define	COT_INRO4		24 	//��Ӧ��4���ٻ�
#define	COT_INRO5		25 	//��Ӧ��5���ٻ�
#define	COT_INRO6		26 	//��Ӧ��6���ٻ�
#define	COT_INRO7		27 	//��Ӧ��7���ٻ�
#define	COT_INRO8		28 	//��Ӧ��8���ٻ�
#define	COT_INRO9		29 	//��Ӧ��9���ٻ�  yc
#define	COT_INRO10		30  //��Ӧ��10���ٻ�
#define	COT_INRO11		31 	//��Ӧ��11���ٻ�
#define	COT_INRO12		32 	//��Ӧ��12���ٻ�
#define	COT_INRO13		33 	//��Ӧ��13���ٻ�
#define	COT_INRO14		34 	//��Ӧ��14���ٻ�
#define	COT_INRO15		35 	//��Ӧ��15���ٻ�
#define	COT_INRO16		36  //��Ӧ��16���ٻ�
#define	COT_REQCOGCN	37  //��Ӧ���������ٻ�
#define	COT_REQCO1		38 	//��Ӧ��1��������ٻ�
#define	COT_REQCO2		39 	//��Ӧ��2��������ٻ�
#define	COT_REQCO3		40 	//��Ӧ��3��������ٻ�
#define	COT_REQCO4		41 	//��Ӧ��4��������ٻ�

#define COT_E_TYPE		44 /**<δ֪�����ͱ�ʶ*/
#define COT_E_REASON	45 /**<δ֪�Ĵ���ԭ��*/
#define COT_E_CADDR	46 /**<δ֪��ASDU������ַ*/
#define COT_E_IADDR	47 /**<δ֪����Ϣ�����ַ*/


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

#define MIN_RECEIVE_LEN  	6    //��С����֡��
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
	BYTE conaddrlen;/*Ӧ�ò㹫����ַ����*/
	BYTE COTlen;/*����ԭ�򳤶�*/
	BYTE infoaddlen;/*��Ϣ��ַ����*/
	BYTE yxtype;/*1.����ʱ�ꣻ2.��ʱ�ꣻ3��CP56Time2aʱ��*/
	BYTE yctype;/*9.����ֵ, ��һ��ֵ;10.��ʱ��Ĳ���ֵ, ��һ��ֵ11.����ֵ, ��Ȼ�ֵ12.����ֵ, ��ʱ��ı�Ȼ�ֵ
	              13.����ֵ, �̸�����14.����ֵ, ��ʱ��Ķ̸�����21.����ֵ, ����Ʒ�������ʵĹ�һ��ֵ
	               34.��CP56Time2aʱ��Ĳ���ֵ, ��һ��ֵ35��CP56Time2aʱ��Ĳ���ֵ, ��Ȼ�ֵ
	  36��CP56Time2aʱ��Ĳ���ֵ, �̸����� */
	//BYTE ddtype;/*15.�ۼ���16��ʱ����ۼ���37��CP56Time2aʱ����ۼ���*/
	WORD baseyear;/*1900��2000*/
	
}Vgb104para;/*104��Լ�����ṹ*/


#endif
