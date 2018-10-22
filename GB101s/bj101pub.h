
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


#define M_SP_NA     1   //不带时标的单点信息
#define M_SP_TA     2   //带短时标的单点信息
#define M_DP_NA     3   //不带时标的双点信息
#define M_DP_TA     4     //带短时标的双点信息
#define M_ST_NA     5   //步位置信息
#define M_BO_NA     7   //子站远动终端状态
#define M_ME_NA     9   //测量值 归一化值
#define M_ME_TA         10  //带时标归一化值
#define M_ME_NB         11  //测量值 标度化值
#define M_ME_TB         12  //带时标标度化值
#define M_ME_NC         13  //测量值 短浮点数
#define M_ME_TC         14  //带时标短浮点数
#define M_IT_NA     15  //电能脉冲记数量
#define M_PS_NA     20  //具有状态变位检出的成组单点信息
#define M_ME_ND     21  //不带品质描述的测量值

#define M_SP_TB   30  //带长时标的单点信息
#define M_DP_TB   31  //带长时标的双点信息
#define M_ST_TB   32  //带长时标的步位置信息
#define M_BO_TB   33  //带长时标的32位位串
#define M_ME_TD   34  //带长时标的测量值，归一化值
#define M_ME_TE   35  //带长时标的测量值，标度化值
#define M_ME_TF   36  //带长时标的测量值，短浮点数
#define M_EI_NA     70  //初始化结束

#define C_SC_NA     45  //单点遥控命令
#define C_DC_NA     46  //双点遥控命令
#define C_RC_NA     47  //升降命令
#define C_SE_NA     48  //设定命令

#define C_IC_NA     100     //召唤命令
#define C_CI_NA     101     //电能脉冲召唤命令
#define C_RD_NA     102     //读数据命令
#define C_CS_NA     103     //时钟同步命令
#define C_TS_NA         104     //测试命令
#define C_RP_NA     105     //复位进程命令
#define C_TS_TA         107     //带时标的测试命令
#define P_AC_NA         113     //参数激活
#define F_FP_FA         210       //文件传输
//传送原因：
#define COT_PERCYC      1   //周期/循环
#define COT_BACK        2   //背景扫描
#define COT_SPONT       3   //突发
#define COT_INIT        4   //初始化
#define COT_REQ         5   //请求或被请求
#define COT_ACT         6   //激活
#define COT_ACTCON      7   //激活确认
#define COT_DEACT       8   //停止激活
#define COT_DEACTCON    9   //停止激活确认
#define COT_ACTTERM     10  //激活结束
#define COT_RETREM      11  //远程命令引起的返送信息
#define COT_RETLOC      12  //当地命令引起的返送信息
#define COT_FILE        13  //文件传送

#define COT_INTROGEN    20  //响应总召唤
//#define   COT_INRO1       21  //响应第1组召唤
//#define   COT_INRO2       22  //响应第2组召唤
//#define   COT_INRO3       23  //响应第3组召唤
//#define   COT_INRO4       24  //响应第4组召唤
//#define   COT_INRO5       25  //响应第5组召唤
//#define   COT_INRO6       26  //响应第6组召唤
//#define   COT_INRO7       27  //响应第7组召唤
//#define   COT_INRO8       28  //响应第8组召唤
//#define   COT_INRO9       29  //响应第9组召唤
//#define   COT_INRO10      30  //响应第10组召唤
//#define   COT_INRO11      31  //响应第11组召唤
//#define   COT_INRO12      32  //响应第12组召唤
//#define   COT_INRO13      33  //响应第13组召唤
//#define   COT_INRO14      34  //响应第14组召唤
//#define   COT_INRO15      35  //响应第15组召唤
//#define   COT_INRO16      36  //响应第16组召唤
//#define   COT_REQCOGCN            37  //响应计数量总召唤
//#define   COT_REQCO1      38  //响应第1组计数量召唤
//#define   COT_REQCO2      39  //响应第2组计数量召唤
//#define   COT_REQCO3      40  //响应第3组计数量召唤
//#define   COT_REQCO4      41  //响应第4组计数量召唤
#define INIT_OVER       70  //初始化结束

#define COT_PN_BIT      0x40


#define     PRM_MASTER      0
#define     PRM_SLAVE       1


//帧控制域
#define     BITS_DIR    0x80    //传输方向位
#define     BITS_PRM    0x40    //启动报文位 1:报文为发送或请求报文 0:报文为确认报文或响应报文
#define     BITS_FCB    0x20    //帧计数位
#define     BITS_AVI    0x10    //帧计数有效位
#define     BITS_ACD    0x20
#define     BITS_DFC    0x10
#define     BITS_CODE   0x4F    //功能码所占位

////////////////短信格式定义定义
#define     INHAND_A01        0x00   //映瀚格式1
#define     INHAND_A0GS        0x01   //映瀚格式2
#define     HONGDIAN        0x02   //暂定
#define     WANWEI          0x03   //暂定

#define     INHAND_GZ_A01        0x01   //故障短信格式1（映瀚通）      
#define     INHAND_GZ_A0GS       0x02   //故障短信格式2（映瀚通）      
#define     INHAND_HB_A01        0x03   //短信按字节输出格式（映瀚通），0x55格式的状态汇报用此格式输出
#define     INHAND_HB_A0GS       0x04   //状态汇报短信格式 （映瀚通） 
#define     INHAND_DUANXIN       0x05   
#define     INHAND_CHAXUN        0x06





///////////////短信发送命令字定义
#define     TXT_SEND            0x13
#define     TXT_REC             0x13
#define     TXT_SEND_OK         0x17
#define     FIND                0x03
#define     LAND                0x05
#define     RESET               0x12
#define     READ_CONFIG         0x02
#define     READ_SPE_CONFIG     0x16
#define     WRITE_SPE_CONFIG    0x18
#define     READ_SPE_STATE     0x15//读映翰通模块状态
#define     RESET_CONFIG        0x01
#define     ACT                 0x14
#define     READ_STATUS         0x1A
#define     ENG                 0x00   //英文短信
#define     CHN                 0x01   //中文短信

//////////////架空二遥指示器命令字定义
//#define     REVERT            0x01
//#define     VERT              0x03
///////////////加密报文类型宏定义
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

/////////////////给加密芯片发送的数据字符串定义
#define GET_CHIP_ID                  "00B0990500020008"
#define GET_RRI_KEY                  "801A00000000"
#define GET_RANDOM                   "008400080000"                  
#define GET_RANDOM_SIGN              "801600800008"
#define VERIFY_SIGN                  "801800"
#define ENCRYPT_DATA                 "80266001"   //加密
#define DECRYPT_DATA                 "802C6001"   //解密
#define VERIFY_DATA_SIGN             "800800"
#define UPDATA_CERTIFICATE           "8022"
#define LOAD_CERTIFICATE             "80240000"
#define READ_CERTIFICATE_LEN         "00B0810000020002"
#define READ_TEST_CERTIFICATE        "00B081020002"
#define READ_REAL_CERTIFICATE        "803001000000"
#define RECOV_PRIVATE_KEY            "801C01"
#define UPDATA_PRIVATE_KEY           "801C00"
//运维工具 (   在与主站网关通讯的基础上增加的命令)
#define TOOL_VERIFY_CER               "801E0000"
#define TOOL_VERIFY_SIGN             "802000000040"
#define TOOL_READ_PUBK               "803000000000"
#define TOOL_ENCRYPT_DATA            "80266202"   //加密
#define TOOL_DECRYPT_DATA            "802C6202"   //解密
#define TOOL_GET_SIGN                "800A0080"  
#define TOOL_RECOV_PRIVATE_KEY       "801C0200"
#define TOOL_LOAD_CER                "8024"

#define MAX_ENC_CMD_BYTES            9 //最长命令字节数 

#define CHINP_ID_BYTES                8
#define RANDOM_BYTES                  8
#define SIGN_BYTES                    64 
//各种数据在扩展区中的位置
#define FRAME_HEAD                   0
#define FRAME_BYTES                  3
#define ENC_CHIP_DATA                4

#define HOST_RANDOM                  2
#define CHIP_ID                      4
#define KEY_VER                      4

//各种信息在加密数据-应用数据中的位置

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
    VFrame69 Frame69;//自定义协议 报文结构与101相同，只是报文头为0x69 for 参数读写 
    VFrameAA FrameAA;
    VFrameEB FrameEB;//EB加密帧	
} VIec101Frame; 


typedef struct{
    BYTE linkaddrlen;/*链路地址长度*/
    BYTE typeidlen;/*信息类型长度*/
    BYTE conaddrlen;/*应用层公共地址长度*/
    BYTE VSQlen;/*可变结构限定词长度*/
    BYTE COTlen;/*传送原因长度*/
    BYTE infoaddlen;/*信息地址长度*/
    BYTE mode;/*传输模式*///1=平衡式 0=非平衡
    BYTE yxtype;/*1.不带时标；2.带时标；3带CP56Time2a时标*/
    BYTE yctype;/*9.测量值, 规一化值;10.带时标的测量值, 规一化值11.测量值, 标度化值12.测量值, 带时标的标度化值
                  13.测量值, 短浮点数14.测量值, 带时标的短浮点数21.测量值, 不带品质描述词的规一化值
                   34.带CP56Time2a时标的测量值, 规一化值35带CP56Time2a时标的测量值, 标度化值
      36带CP56Time2a时标的测量值, 短浮点数 */
    WORD baseyear;/*1900或2000*/
    
}Vgb101para;/*101规约参数结构*/


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
