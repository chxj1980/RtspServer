
#pragma once

//////////////////////////////////////////////////////////////////////////
//ͨ�ýӿ���ض���

//�궨��
#define MAX_NAME				64
#define MAX_CHANNEL				128
#define DEF_LEN_64				64
#define DEF_LEN_512				512
#define MAXLEN_IP				16
#define MAXLEN_STR				50

/*
#define NET_DVR_PLAYSTART			101//��ʼ����
#define NET_DVR_PLAYFILELIST		102//��ȡ�ļ��б�
#define NET_DVR_PLAYPAUSE			103//��ͣ����
#define NET_DVR_PLAYRESTART			104//�ָ�����
#define NET_DVR_PLAYFAST			105//���
#define NET_DVR_PLAYSLOW			106//����
#define NET_DVR_PLAYNORMAL			107//�����ٶ�
#define NET_DVR_PLAYFRAME			108//��֡��

#define NET_DVR_PLAYSETPOS			112//�ı��ļ��طŵĽ���
#define NET_DVR_PLAYGETPOS			113//��ȡ�ļ��طŵĽ���
#define NET_DVR_PLAYGETTIME			114//��ȡ��ǰ�Ѿ����ŵ�ʱ��
#define NET_DVR_PLAYGETFRAME		115//��ȡ��ǰ�Ѿ����ŵ�֡��
#define NET_DVR_GETTOTALFRAMES  	116//��ȡ��ǰ�����ļ��ܵ�֡��
#define NET_DVR_GETTOTALTIME    	117//��ȡ��ǰ�����ļ��ܵ�ʱ��
#define NET_DVR_THROWBFRAME			120//��B֡
*/

//���紫������
enum TRANSTYPE
{
	TT_TCP = 0,              //	TCP��ʽֱ��
	TT_UDP = 1,              //	UDP��ʽֱ��
	TT_MULTICAST = 2,        //	�鲥��ʽֱ��
	TT_TCP_TRANS = 3,        //	TCP��ʽ��ý��ת��
	TT_UDP_TRANS = 4,        //	UDP��ʽ��ý��ת��
	TT_MULTICAST_TRANS = 5   //	�鲥��ʽ��ý��ת��
};

//��������
enum ALARMTYPE
{
	ALARM_IO = 0,						//	I/O�ź���������dwChnNo��������ţ���1��ʼ
	ALARM_DISKFULL = 1,					//	Ӳ������dwChnNo��Ӳ�̺ţ���1��ʼ
	ALARM_VIDEOLOST = 2,				//	��Ƶ��ʧ��dwChnNo����Ƶͨ���ţ���1��ʼ
	ALARM_VIDEOMOTION = 3,				//	��Ƶ�ƶ���⣬dwChnNo����Ƶͨ����
	ALARM_DISKNOTFORMATTED = 4,			//	Ӳ��δ��ʽ����dwChnNo��Ӳ�̺�
	ALARM_DISKERROR = 5,				//	Ӳ�̴���dwChnNo��Ӳ�̺�
	ALARM_VIDEOSHELTER = 6,				//	��Ƶ�ڵ�������dwChnNo����Ƶͨ����
	ALARM_VIDEOFORMAT = 7,				//	��Ƶ��ʽ��ƥ�䣬dwChnNo����Ƶͨ����
	ALARM_ILLEGALCONN = 8,				//	�Ƿ������豸
	ALARM_VCA_TRAVERSE_PLANE = 1001,	//	��Խ������
	ALARM_VCA_ENTER_AREA = 1002,		//	��������
	ALARM_VCA_EXIT_AREA = 1003,			//	�뿪����
	ALARM_VCA_INTRUSION = 1004,			//	�ܽ�����
	ALARM_VCA_LOITER = 1005,			//	�ǻ�
	ALARM_VCA_LEFT_TAKE = 1006,			//	���������
	ALARM_VCA_PARKING = 1007,			//	ͣ��
	ALARM_VCA_RUN = 1008,				//	����
	ALARM_VCA_HIGH_DENSITY = 1009,		//	��������Ա�ܶ�
	ALARM_VCA_VIOLENT_MOTION = 1010,	//	�����˶����
	ALARM_VCA_REACH_HIGHT = 1011,		//	�ʸ߼��
	ALARM_VCA_GET_UP = 1012,			//	������
	ALARM_VCA_TARRY = 1013,				//	��Ա����
	ALARM_VCA_STICK_UP = 1014,			//	��ֽ��
	ALARM_VCA_INSTALL_SCANNER = 1015	//	��װ������
};

//��������
enum COMTYPE
{
	COM_NONE = 0,       //��
	COM_232 = 1,        //͸��232����
	COM_485 = 2,        //485��
};

//������л�����
enum SWITCHTYPE
{
	SWITCH_MANUAL = 0,     //�ֶ��л�
	SWITCH_AUTO = 1,       //ͨ���Զ��л�
	SWITCH_SYN = 2,        //ͬ���л�
	SWITCH_GROUPAUTO = 3,  //Ⱥ���Զ��л�
};

//I/O����
enum IOTYPE
{
	IO_IN = 0,     //����
	IO_OUT = 1,    //���
};

//��̨��ͷ��������
enum CTRLTYPE
{
	CT_STOP = 0,				//����ֹͣ
	CT_TILT_UP = 1,				//��̨����
	CT_TILT_DOWN = 2,			//��̨�¸�
	CT_PAN_LEFT = 3,			//��̨��ת
	CT_PAN_RIGHT = 4,			//��̨��ת
	CT_PAN_AUTO = 5,			//��̨�Զ�
	CT_ZOOM_IN = 6,				//���ʱ��
	CT_ZOOM_OUT = 7,			//���ʱ�С
	CT_FOCUS_NEAR = 8,			//����ǰ��
	CT_FOCUS_FAR = 9,			//������
	CT_IRIS_ENLARGE = 10,		//��Ȧ����
	CT_IRIS_SHRINK = 11,		//��Ȧ��С
	CT_CALL_PRESET = 12,		//ת��Ԥ��
	CT_SET_PRESET = 13,			//����Ԥ��
	CT_CLE_PRESET = 14,			//���Ԥ��
	CT_AUX1_PWRON = 15,			//��������1��
	CT_AUX1_PWROFF = 16,		//��������1��
	CT_AUX2_PWRON = 17,			//��������2��
	CT_AUX2_PWROFF = 18,		//��������2��
	CT_AUX3_PWRON = 19,			//��������3��
	CT_AUX3_PWROFF = 20,		//��������3��
	CT_AUX4_PWRON = 21,			//��������4��
	CT_AUX4_PWROFF = 22,		//��������4��
	CT_TILT_LEFTUP = 23,		//��̨����
	CT_TILT_RIGHTUP = 24,		//��̨����
	CT_TILT_LEFTDOWN= 25,		//��̨����
	CT_TILT_RIGHTDOWN= 26,		//��̨����
	CT_CRUISE_ADD=30,			//Ѳ��������Ԥ�õ�
	CT_CRUISE_DWELL=31,			//Ѳ��������ͣ��ʱ��
	CT_CRUISE_SPEED=32,			//Ѳ��������Ѳ���ٶ�
	CT_CRUISE_REMOVE=33,		//Ѳ������Ѳ��������ɾ��Ԥ�õ�
	CT_CRUISE_RUN=34,			//Ѳ��������
	CT_CRUISE_STOP=35,			//Ѳ����ֹͣ
	CT_TRACK_STARTREC=40,		//�켣����ʼ��¼
	CT_TRACK_STOPREC=41,		//�켣��ֹͣ��¼
	CT_TRACK_RUN=42,			//�켣������
	CT_RECTZOOM=50				//�������ţ�3D��λ��
};

//¼������
enum RECORDTYPE
{
	MANUAL_REC = 0, //�ֶ�¼��
	PLAN_REC = 1,   //�ƻ�¼��
	ALARM_REC = 2,  //����¼��
	ALL_REC = 3,    //ȫ��¼��
};

//¼��ʱ������
typedef struct tagQueryTime
{
	unsigned int ufromyear;		// ��ʼ��
	unsigned int ufrommonth;	// ��ʼ��
	unsigned int ufromday;		// ��ʼ��
	unsigned int ufromhour;		// ��ʼʱ
	unsigned int ufromminute;	// ��ʼ��
	unsigned int ufromsecond;	// ��ʼ��
	unsigned int utoyear;		// ��ֹ��
	unsigned int utomonth;		// ��ֹ��
	unsigned int utoday;		// ��ֹ��
	unsigned int utohour;		// ��ֹʱ
	unsigned int utominute;		// ��ֹ��
	unsigned int utosecond;		// ��ֹ��
}QUERYTIME, *PQUERYTIME;

//ģ����Ϣ
typedef struct { 
	char	description[128];		//ģ������
	char	ssoftversion[64];		//����汾
	char	smanufactuer[64];		//��������  
	char	DeviceTypeName[128];	//�豸�ͺ�
	long invalid_handle_value;		//Ĭ��-1
}PLUGIN_INFO,*PPLUGIN_INFO;

//�豸��Ϣ
typedef struct tagDeviceInfo
{
	char sdevicetype[DEF_LEN_64];	//�豸�ͺ�
	char ssoftversion[DEF_LEN_64];	//����汾
	char smanufactuer[DEF_LEN_64];	//��������
	int nchannelnum;				//��ͨ������
	int ndisknumber;				//��������
	int nioinputnum;				//IO��������
	int niooutputnum;				//IO�������
}SDEVICEINFO,*PDEVICEINFO;

//ͨ����Ϣ
typedef struct tagChannelNode
{
	char sname[MAX_NAME];			//ͨ������
	DWORD dwReserve1;
}SCHANNODE,*PCHANNODE;

typedef struct tagChannelInfo
{
	unsigned int uchannels;			//ͨ������
	SCHANNODE schannode[MAX_CHANNEL];
}CHANNELINFO,*PCHANNELINFO;

typedef struct
{
	char szProxySvrIP[16];	//ת��������IP��ַ
	WORD wProxySvrPort;		//ת���������˿ں�
	DWORD dwReserved;		//��������
}VIDEOPROXY, *PVIDEOPROXY;

//ʱ��ṹָ��
typedef struct tagDevTime
{
	unsigned int ufromyear;		// ��
	unsigned int ufrommonth;	// ��
	unsigned int ufromday;		// ��
	unsigned int ufromhour;		// ʱ
	unsigned int ufromminute;	// ��
	unsigned int ufromsecond;	// ��
}DEVTIME, *PDEVTIME;

//�豸ͨ��״̬�ṹָ��
typedef struct{
	BYTE byRecordStatic;		 //ͨ���Ƿ���¼��,0-��¼��,1-¼��
	BYTE bySignalStatic;		 //���ӵ��ź�״̬,0-����,1-�źŶ�ʧ
	DWORD dwBitRate;			 //ʵ������
	DWORD dwLinkNum;			 //�ͻ������ӵĸ���
}DEV_CHANNELSTATE,*LPDEV_CHANNELSTATE;

typedef struct{
	DWORD dwVolume;				 //Ӳ�̵�����
	DWORD dwFreeSpace;			 //Ӳ�̵�ʣ��ռ�
	DWORD dwHardDiskStatic;		 //Ӳ�̵�״̬,����,�,��������,1-����,2-������,3-����Ӳ�̳���
}DEV_DISKSTATE,*LPDEV_DISKSTATE;

typedef struct{
	DEV_DISKSTATE  struDiskState[16];	//Ӳ��״̬
	DEV_CHANNELSTATE struChanState[32];	//ͨ����״̬
	BYTE  byAlarmInStatus[32];			//�����˿ڵ�״̬,0-û�б���,1-�б���
	BYTE  byAlarmOutStatus[16];			//��������˿ڵ�״̬,0-û�����,1-�б������
}DEV_STAT,*LPDEV_STAT;

typedef struct
{
	DWORD dwVal;					//������־��1��������0��������ʧ��--- Ϊ�������ڰ汾����
	char  szAlmDevIP[MAXLEN_IP];	//�����豸ip
	int	  nAlmDevChan;				//�����豸ͨ����,1��ʼ
	int	  nRuleID;					//HK iVms����ţ�0��7
	char  szRuleName[32];			//HK iVms��������
	char  szDes[DEF_LEN_512];		//��������
	DWORD dwPicLen;					//����ץ��ͼƬ��С
	BYTE* pPicData;					//����ץ��ͼƬ����(jpg)
}ALM_PARAM, *PALM_PARAM;

