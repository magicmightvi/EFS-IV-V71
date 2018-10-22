
#ifndef BJ101PUB_H
#define BJ101PUB_H



#define ADDR_YC_LO      0x4001
#define ADDR_YC_HI      0x5000
#define ADDR_YCPARA_LO      0x5001
#define ADDR_YCPARA_HI      0x6000

#define ADDR_YK_LO      0x6001
#define ADDR_YK_HI      0x6200

#define ADDR_YT_LO      0x6201
#define ADDR_YT_HI      0x6400
#define ADDR_DD_LO      0x6401
#define ADDR_DD_HI      0x6600


#define M_SP_NA     1   //����ʱ��ĵ�����Ϣ
#define M_SP_TA     2   //����ʱ��ĵ�����Ϣ
#define M_DP_NA     3   //����ʱ���˫����Ϣ
#define M_DP_TA     4     //����ʱ���˫����Ϣ
#define M_ST_NA     5   //��λ����Ϣ
#define M_BO_NA     7   //��վԶ���ն�״̬
#define M_ME_NA     9   //����ֵ ��һ��ֵ
#define M_ME_TA         10  //��ʱ���һ��ֵ
#define M_ME_NB         11  //����ֵ ��Ȼ�ֵ
#define M_ME_TB         12  //��ʱ���Ȼ�ֵ
#define M_ME_NC         13  //����ֵ �̸�����
#define M_ME_TC         14  //��ʱ��̸�����
#define M_IT_NA     15  //�������������
#define M_PS_NA     20  //����״̬��λ����ĳ��鵥����Ϣ
#define M_ME_ND     21  //����Ʒ�������Ĳ���ֵ

#define M_SP_TB   30  //����ʱ��ĵ�����Ϣ
#define M_DP_TB   31  //����ʱ���˫����Ϣ
#define M_ST_TB   32  //����ʱ��Ĳ�λ����Ϣ
#define M_BO_TB   33  //����ʱ���32λλ��
#define M_ME_TD   34  //����ʱ��Ĳ���ֵ����һ��ֵ
#define M_ME_TE   35  //����ʱ��Ĳ���ֵ����Ȼ�ֵ
#define M_ME_TF   36  //����ʱ��Ĳ���ֵ���̸�����
#define M_EI_NA     70  //��ʼ������

#define C_SC_NA     45  //����ң������
#define C_DC_NA     46  //˫��ң������
#define C_RC_NA     47  //��������
#define C_SE_NA     48  //�趨����

#define C_IC_NA     100     //�ٻ�����
#define C_CI_NA     101     //���������ٻ�����
#define C_RD_NA     102     //����������
#define C_CS_NA     103     //ʱ��ͬ������
#define C_TS_NA         104     //��������
#define C_RP_NA     105     //��λ��������
#define C_TS_TA         107     //��ʱ��Ĳ�������
#define P_AC_NA         113     //��������
#define F_FP_FA         210       //�ļ�����
//����ԭ��
#define COT_PERCYC      1   //����/ѭ��
#define COT_BACK        2   //����ɨ��
#define COT_SPONT       3   //ͻ��
#define COT_INIT        4   //��ʼ��
#define COT_REQ         5   //���������
#define COT_ACT         6   //����
#define COT_ACTCON      7   //����ȷ��
#define COT_DEACT       8   //ֹͣ����
#define COT_DEACTCON    9   //ֹͣ����ȷ��
#define COT_ACTTERM     10  //�������
#define COT_RETREM      11  //Զ����������ķ�����Ϣ
#define COT_RETLOC      12  //������������ķ�����Ϣ
#define COT_FILE        13  //�ļ�����

#define COT_INTROGEN    20  //��Ӧ���ٻ�
//#define   COT_INRO1       21  //��Ӧ��1���ٻ�
//#define   COT_INRO2       22  //��Ӧ��2���ٻ�
//#define   COT_INRO3       23  //��Ӧ��3���ٻ�
//#define   COT_INRO4       24  //��Ӧ��4���ٻ�
//#define   COT_INRO5       25  //��Ӧ��5���ٻ�
//#define   COT_INRO6       26  //��Ӧ��6���ٻ�
//#define   COT_INRO7       27  //��Ӧ��7���ٻ�
//#define   COT_INRO8       28  //��Ӧ��8���ٻ�
//#define   COT_INRO9       29  //��Ӧ��9���ٻ�
//#define   COT_INRO10      30  //��Ӧ��10���ٻ�
//#define   COT_INRO11      31  //��Ӧ��11���ٻ�
//#define   COT_INRO12      32  //��Ӧ��12���ٻ�
//#define   COT_INRO13      33  //��Ӧ��13���ٻ�
//#define   COT_INRO14      34  //��Ӧ��14���ٻ�
//#define   COT_INRO15      35  //��Ӧ��15���ٻ�
//#define   COT_INRO16      36  //��Ӧ��16���ٻ�
//#define   COT_REQCOGCN            37  //��Ӧ���������ٻ�
//#define   COT_REQCO1      38  //��Ӧ��1��������ٻ�
//#define   COT_REQCO2      39  //��Ӧ��2��������ٻ�
//#define   COT_REQCO3      40  //��Ӧ��3��������ٻ�
//#define   COT_REQCO4      41  //��Ӧ��4��������ٻ�
#define INIT_OVER       70  //��ʼ������

#define COT_PN_BIT      0x40


#define     PRM_MASTER      0
#define     PRM_SLAVE       1


//֡������
#define     BITS_DIR    0x80    //���䷽��λ
#define     BITS_PRM    0x40    //��������λ 1:����Ϊ���ͻ������� 0:����Ϊȷ�ϱ��Ļ���Ӧ����
#define     BITS_FCB    0x20    //֡����λ
#define     BITS_AVI    0x10    //֡������Чλ
#define     BITS_ACD    0x20
#define     BITS_DFC    0x10
#define     BITS_CODE   0x4F    //��������ռλ

////////////////���Ÿ�ʽ���嶨��
#define     INHAND_A01        0x00   //ӳ嫸�ʽ1
#define     INHAND_A0GS        0x01   //ӳ嫸�ʽ2
#define     HONGDIAN        0x02   //�ݶ�
#define     WANWEI          0x03   //�ݶ�

#define     INHAND_GZ_A01        0x01   //���϶��Ÿ�ʽ1��ӳ�ͨ��      
#define     INHAND_GZ_A0GS       0x02   //���϶��Ÿ�ʽ2��ӳ�ͨ��      
#define     INHAND_HB_A01        0x03   //���Ű��ֽ������ʽ��ӳ�ͨ����0x55��ʽ��״̬�㱨�ô˸�ʽ���
#define     INHAND_HB_A0GS       0x04   //״̬�㱨���Ÿ�ʽ ��ӳ�ͨ�� 
#define     INHAND_DUANXIN       0x05   
#define     INHAND_CHAXUN        0x06





///////////////���ŷ��������ֶ���
#define     TXT_SEND            0x13
#define     TXT_REC             0x13
#define     TXT_SEND_OK         0x17
#define     FIND                0x03
#define     LAND                0x05
#define     RESET               0x12
#define     READ_CONFIG         0x02
#define     READ_SPE_CONFIG     0x16
#define     WRITE_SPE_CONFIG    0x18
#define     READ_SPE_STATE     0x15//��ӳ��ͨģ��״̬
#define     RESET_CONFIG        0x01
#define     ACT                 0x14
#define     READ_STATUS         0x1A
#define     ENG                 0x00   //Ӣ�Ķ���
#define     CHN                 0x01   //���Ķ���

//////////////�ܿն�ңָʾ�������ֶ���
//#define     REVERT            0x01
//#define     VERT              0x03
///////////////���ܱ������ͺ궨��
#define    CONNECT_OBJECT_BIT     0xC0
#define    MASTER_STATION         0x00
#define    SET_TOOL               0x40
#define    GATEWAY                0x80
#define    RES                    0xC0

#define    ENC_TURN_BIT           0x08
#define    ENC_ON                 0x08
#define    ENC_OFF                0x00

#define    SYMMETRIC_KEY_BIT      0x07
#define    MASTER_STATION_KEY     0x01
#define    SET_TOOL_KEY           0x02

#define    GAETWAY_VERIFI         0x20
#define    GAETWAY_VERIFI_ACK     0x21

/////////////////������оƬ���͵������ַ�������
#define GET_CHIP_ID                  "00B0990500020008"
#define GET_RRI_KEY                  "801A00000000"
#define GET_RANDOM                   "008400080000"                  
#define GET_RANDOM_SIGN              "801600800008"
#define VERIFY_SIGN                  "801800"
#define ENCRYPT_DATA                 "80266001"   //����
#define DECRYPT_DATA                 "802C6001"   //����
#define VERIFY_DATA_SIGN             "800800"
#define UPDATA_CERTIFICATE           "8022"
#define LOAD_CERTIFICATE             "80240000"
#define READ_CERTIFICATE_LEN         "00B0810000020002"
#define READ_TEST_CERTIFICATE        "00B081020002"
#define READ_REAL_CERTIFICATE        "803001000000"
#define RECOV_PRIVATE_KEY            "801C01"
#define UPDATA_PRIVATE_KEY           "801C00"
//��ά���� (   ������վ����ͨѶ�Ļ��������ӵ�����)
#define TOOL_VERIFY_CER               "801E0000"
#define TOOL_VERIFY_SIGN             "802000000040"
#define TOOL_READ_PUBK               "803000000000"
#define TOOL_ENCRYPT_DATA            "80266202"   //����
#define TOOL_DECRYPT_DATA            "802C6202"   //����
#define TOOL_GET_SIGN                "800A0080"  
#define TOOL_RECOV_PRIVATE_KEY       "801C0200"
#define TOOL_LOAD_CER                "8024"

#define MAX_ENC_CMD_BYTES            9 //������ֽ��� 

#define CHINP_ID_BYTES                8
#define RANDOM_BYTES                  8
#define SIGN_BYTES                    64 
//������������չ���е�λ��
#define FRAME_HEAD                   0
#define FRAME_BYTES                  3
#define ENC_CHIP_DATA                4

#define HOST_RANDOM                  2
#define CHIP_ID                      4
#define KEY_VER                      4

//������Ϣ�ڼ�������-Ӧ�������е�λ��

#define APPDATA_LEN                 0
#define APPDATA                     1


#define NEW_KEY_VER                 2
#define UPDATA_KEY_DATA             3
#define SIGN_KEY_DATA               183
#define S_KEY_ID                    247

typedef union 
{
    VFrame10 Frame10;
    VFrame68 Frame68;
    VFrame69 Frame69;//�Զ���Э�� ���Ľṹ��101��ͬ��ֻ�Ǳ���ͷΪ0x69 for ������д 
    VFrameAA FrameAA;
    VFrameEB FrameEB;//EB����֡	
} VIec101Frame; 


typedef struct{
    BYTE linkaddrlen;/*��·��ַ����*/
    BYTE typeidlen;/*��Ϣ���ͳ���*/
    BYTE conaddrlen;/*Ӧ�ò㹫����ַ����*/
    BYTE VSQlen;/*�ɱ�ṹ�޶��ʳ���*/
    BYTE COTlen;/*����ԭ�򳤶�*/
    BYTE infoaddlen;/*��Ϣ��ַ����*/
    BYTE mode;/*����ģʽ*///1=ƽ��ʽ 0=��ƽ��
    BYTE yxtype;/*1.����ʱ�ꣻ2.��ʱ�ꣻ3��CP56Time2aʱ��*/
    BYTE yctype;/*9.����ֵ, ��һ��ֵ;10.��ʱ��Ĳ���ֵ, ��һ��ֵ11.����ֵ, ��Ȼ�ֵ12.����ֵ, ��ʱ��ı�Ȼ�ֵ
                  13.����ֵ, �̸�����14.����ֵ, ��ʱ��Ķ̸�����21.����ֵ, ����Ʒ�������ʵĹ�һ��ֵ
                   34.��CP56Time2aʱ��Ĳ���ֵ, ��һ��ֵ35��CP56Time2aʱ��Ĳ���ֵ, ��Ȼ�ֵ
      36��CP56Time2aʱ��Ĳ���ֵ, �̸����� */
    WORD baseyear;/*1900��2000*/
    
}Vgb101para;/*101��Լ�����ṹ*/


typedef struct {
    DWORD LinkAddr;    
    DWORD TypeID;      
    DWORD VSQ;
    DWORD COT;  
    DWORD Address;  
    DWORD Info;    
    BYTE  Infooff;     
} VDWASDU;

#endif /*BJ101PUB_H*/
