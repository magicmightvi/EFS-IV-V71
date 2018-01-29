#ifndef _REMOTE_UPDATA_H_
#define _REMOTE_UPDATA_H_


#ifdef  REMOTE_UPDATA_GLOBALS
#define REMOTE_UPDATA_EXT
#else
#define REMOTE_UPDATA_EXT  extern
#endif



//============================   ��������   ======================================
#define WRITEPACK  232//д������ݸ��� ����ַ��3��+ƫ�Ƶ�ַ��3��+��������1��+�������ݣ�224��+�ۼӺͣ�1��
#define V_OFFSET 3//����汾������VerNum�е�ƫ������ȡ�Ժ�5���ֽ�Ϊ����汾�� 
#define STATE_LOADING        0xAA //����������
#define STATE_ALLCHECK1_YES  0x67//����У��1�ɹ�
#define STATE_ALLCHECK2_YES  0xCD//���У��2�ɹ�
#define STATE_COMPLETE       0x58//�������
#define STATE_FLASHWRITE_NO  0x42//flashд�����
#define STATE_SCHECK_NO      0x49//��֡���ݶ�ȡУ�����
#define STATE_ALLCHECK1_NO   0x45//���У��1����
#define STATE_ALLCHECK2_NO   0x23//���У��2����


//���߸��´洢
//��һ�θ��²���
#define EEPUPDATE_LAST_VERSION     	0x1130 //�ϴθ��°汾��
#define EEPUPDATE_LAST_STATE         	0X1136 //�ϴε�����״̬
#define EEPUPDATE_LAST_SEG             	0X113A //������    �����������������洢
#define EEPUPDATE_LAST_SEGNo       	0X113B //�ϵ�����
#define EEPUPDATE_LAST_LENGTH      	0X113C //���ܳ�
#define EEPUPDATE_LAST_BASE        	0X1140 //�λ�ַ
#define EEPUPDATE_LAST_OFFSET      	0x1144 //ƫ�Ƶ�ַ
#define EEPUPDATE_LAST_COUNTER     	0x1148 //�����
#define EEPUPDATE_LAST_INCOUNTER 	0x114A //���ڰ����
#define EEPUPDATE_LAST_CHECK       	0X114C //���У��ֵ
#define EEPUPDATE_LAST_TOTAL       	0x114E //��������
//����
#define EEPUPDATE_LAST_BKSEG         	0X1170 //������    �����������������洢
#define EEPUPDATE_LAST_BKSEGNo    	0X1171 //�ϵ�����
#define EEPUPDATE_LAST_BKLENGTH   	0X1172 //���ܳ�
#define EEPUPDATE_LAST_BKBASE        0X1176 //�λ�ַ
#define EEPUPDATE_LAST_BKOFFSET   	0x117A //ƫ�Ƶ�ַ
#define EEPUPDATE_LAST_BKCOUNTER 	0x117E //�����
#define EEPUPDATE_LAST_BKINCOUNTER   0x1180 //���ڰ����
#define EEPUPDATE_LAST_BKCHECK       0X1182 //���У��ֵ
#define EEPUPDATE_LAST_BKTOTAL       0x1184 //��������

#define EEPUPDATE_STATE_NO         26//�������ع����е��м�״̬��ʱ����״̬Ϊ����������
#define EEPUPDATE_STATE_SEG         0//������
#define EEPUPDATE_STATE_SEGNo       1//�����
#define EEPUPDATE_STATE_LENGTH1     2//���ܳ����ֽ�
#define EEPUPDATE_STATE_LENGTH2     3
#define EEPUPDATE_STATE_LENGTH3     4//���ܳ����ֽ�
#define EEPUPDATE_STATE_LENGTH4     5//�̶�Ϊ�㣬Ԥ��
#define EEPUPDATE_STATE_BASE1       6//����ַ���ֽ�
#define EEPUPDATE_STATE_BASE2       7
#define EEPUPDATE_STATE_BASE3       8//����ַ���ֽ�
#define EEPUPDATE_STATE_BASE4       9//�̶�Ϊ�㣬Ԥ��
#define EEPUPDATE_STATE_OFFSET1     10//ƫ�Ƶ�ַ���ֽ�
#define EEPUPDATE_STATE_OFFSET2     11//
#define EEPUPDATE_STATE_OFFSET3     12//���ֽ�
#define EEPUPDATE_STATE_OFFSET4     13//�̶�Ϊ�㣬Ԥ��
#define EEPUPDATE_STATE_COUNTER1     14//����ŵ��ֽ�
#define EEPUPDATE_STATE_COUNTER2     15//����Ÿ��ֽ�
#define EEPUPDATE_STATE_INCOUNTER1   16 //���ڰ���ŵ��ֽ�
#define EEPUPDATE_STATE_INCOUNTER2   17 //���ڰ���Ÿ��ֽ�
#define EEPUPDATE_STATE_CHECK1       18 //���У��ֵ���ֽ�
#define EEPUPDATE_STATE_CHECK2       19 //���У��ֵ���ֽ�
#define EEPUPDATE_STATE_TOTAL1       20 //�����������ֽ�
#define EEPUPDATE_STATE_TOTAL2       21 //
#define EEPUPDATE_STATE_TOTAL3       22 //
#define EEPUPDATE_STATE_TOTAL4       23 //�����������ֽ�
#define EEPUPDATE_STATE_CRCL         24 //�����������ֽ�
#define EEPUPDATE_STATE_CRCH         25 //�����������ֽ�

//    FUNC_LOAD
#define Reason_160 200//�ӻ���Ӧ֡ ��������֡
#define Reason_163 203
#define Reason_164 204
#define Reason_165 205
#define Reason_166 206
#define Reason_170 210


REMOTE_UPDATA_EXT unsigned char g_unGPRSTX_Buff[256];          //GPRS���ݷ������� 
//REMOTE_UPDATA_EXT unsigned char g_unRX_Buff[MAX_RXBUFF_LEN];            //���ݽ�������
REMOTE_UPDATA_EXT int g_Cmid;
//REMOTE_UPDATA_EXT int g_flag;


REMOTE_UPDATA_EXT void RemoteUpdata(unsigned char *p , unsigned char len);
REMOTE_UPDATA_EXT void Code_Load(unsigned char *p,unsigned char *pTxBuf);


#endif
