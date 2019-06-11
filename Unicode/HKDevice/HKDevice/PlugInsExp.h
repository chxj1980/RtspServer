#ifdef DFJK_EXPORTS
#define DFJK_API __declspec(dllexport)
#else
#define DFJK_API __declspec(dllimport)
#endif

#define CALL_TYPE   __stdcall

#include "Global.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////
//ͨ�ýӿ���ض���

//�궨��
#define MAX_NAME				64
#define MAX_CHANNEL				128
#define DEF_LEN_64				64
#define DEF_LEN_512				512

//���紫������
enum TRANSTYPE
{
	TT_TCP = 0,              //TCP��ʽֱ��
	TT_UDP = 1,              //UDP��ʽֱ��
	TT_MULTICAST = 2,        //�鲥��ʽֱ��
	TT_TCP_TRANS = 3,        // TCP��ʽ��ý��ת��
	TT_UDP_TRANS = 4,        // UDP��ʽ��ý��ת��
	TT_MULTICAST_TRANS = 5   // �鲥��ʽ��ý��ת��
};

//��������
enum ALARMTYPE
{
	ALARM_IO = 0,			// I/O�ź���������dwChnNo��������ţ���1��ʼ
	ALARM_DISKFULL = 1,		// Ӳ������dwChnNo��Ӳ�̺ţ���1��ʼ
	ALARM_VIDEOLOST = 2,	//��Ƶ��ʧ��dwChnNo����Ƶͨ���ţ���1��ʼ
	ALARM_VIDEOMOTION = 3,	// ��Ƶ�ƶ���⣬dwChnNo����Ƶͨ����
	ALARM_DISKNOTFORMATTED = 4, //Ӳ��δ��ʽ����dwChnNo��Ӳ�̺�
	ALARM_DISKERROR = 5,	//Ӳ�̴���dwChnNo��Ӳ�̺�
	ALARM_VIDEOSHELTER = 6,	// ��Ƶ�ڵ�������dwChnNo����Ƶͨ����
	ALARM_VIDEOFORMAT = 7,	// ��Ƶ��ʽ��ƥ�䣬dwChnNo����Ƶͨ����
	ALARM_ILLEGALCONN = 8,	// �Ƿ������豸
	ALARM_VCA_TRAVERSE_PLANE = 1001,	//��Խ������
	ALARM_VCA_ENTER_AREA = 1002,		//��������
	ALARM_VCA_EXIT_AREA = 1003,			//�뿪����
	ALARM_VCA_INTRUSION = 1004,			//�ܽ�����
	ALARM_VCA_LOITER = 1005,			//�ǻ�
	ALARM_VCA_LEFT_TAKE = 1006,			//���������
	ALARM_VCA_PARKING = 1007,			//ͣ��
	ALARM_VCA_RUN = 1008,				//����
	ALARM_VCA_HIGH_DENSITY = 1009,		//��������Ա�ܶ�
	ALARM_VCA_VIOLENT_MOTION = 1010,	//�����˶����
	ALARM_VCA_REACH_HIGHT = 1011,		//�ʸ߼��
	ALARM_VCA_GET_UP = 1012,			//������
	ALARM_VCA_TARRY = 1013,				//��Ա����
	ALARM_VCA_STICK_UP = 1014,			//��ֽ��
	ALARM_VCA_INSTALL_SCANNER = 1015	//��װ������
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
	CT_STOP = 0,
	CT_TILT_UP = 1,
	CT_TILT_DOWN = 2,
	CT_PAN_LEFT = 3,
	CT_PAN_RIGHT = 4,
	CT_PAN_AUTO = 5,
	CT_ZOOM_IN = 6,
	CT_ZOOM_OUT = 7,
	CT_FOCUS_NEAR = 8,
	CT_FOCUS_FAR = 9,
	CT_IRIS_ENLARGE = 10,
	CT_IRIS_SHRINK = 11,
	CT_CALL_PRESET = 12,
	CT_SET_PRESET = 13,
	CT_CLE_PRESET = 14,
	CT_AUX1_PWRON = 15,
	CT_AUX1_PWROFF = 16,
	CT_AUX2_PWRON = 17,
	CT_AUX2_PWROFF = 18,
	CT_AUX3_PWRON = 19,
	CT_AUX3_PWROFF = 20,
	CT_AUX4_PWRON = 21,
	CT_AUX4_PWROFF = 22,
	CT_TILT_LEFTUP = 23,        //��̨����
	CT_TILT_RIGHTUP = 24,       //��̨����
	CT_TILT_LEFTDOWN= 25,       //��̨����
	CT_TILT_RIGHTDOWN= 26,      //��̨����
	CT_CRUISE_ADD=30,		//Ѳ��������Ԥ�õ�
	CT_CRUISE_DWELL=31,		//Ѳ��������ͣ��ʱ��
	CT_CRUISE_SPEED=32,		//Ѳ��������Ѳ���ٶ�
	CT_CRUISE_REMOVE=33,	//Ѳ������Ѳ��������ɾ��Ԥ�õ�
	CT_CRUISE_RUN=34,		//Ѳ��������
	CT_CRUISE_STOP=35,		//Ѳ����ֹͣ
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
	char description[128];		//ģ������
	char ssoftversion[64];		//����汾
	char smanufactuer[64];		//��������  
	char DeviceTypeName[128];	//�豸�ͺ�
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

//////////////////////////////////////////////////////////////////////////
//  1 ϵͳ��غ���
//////////////////////////////////////////////////////////////////////////
/**************************************************************************
(1-1) SYS_GetLastError
����˵�����ӿڴ�����Ϣ��ȡ
���������nBuffLen - ��������С
���������lpszErrorMsg-���ش�����Ϣ�ַ���������ָ�룬���ַ�����ʾû�д���
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ�� 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE ISYS_GetLastError(LPTSTR lpszErrorMsg, UINT nMsgLen);

/**************************************************************************
(1-2) ISYS_GetInvalidHandleValue
����˵������ȡ��Ч����ú�����ʽ�ṩ
������������ޣ�
���������phHandle-��Ч���ֵָ��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE ISYS_GetInvalidHandleValue(HANDLE *phHandle);

/**************************************************************************
(1-3) ISYS_GetPluginInfo
����˵������ȡ���׽ӿڿ���Ϣ
���������(��)
���������pInfo- PLUGIN_INFO�ṹָ��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE ISYS_GetPluginInfo(PLUGIN_INFO *pInfo);

/**************************************************************************
(1-4) ISYS_Initialize
����˵����ģ���ʼ��������ģ����غ����ȵ���
���������pReserved-����Ϊ��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE ISYS_Initialize(void *pReserved, DWORD dwReserved);

/**************************************************************************
(1-5) ISYS_Uninitialize
����˵����ģ�鷴��ʼ����������ģ��ж��ǰ���á������ͷű�ģ����ص�������Դ��
���������(��)
���������(��)
�������أ�(��)
/**************************************************************************/
void DFJK_API CALL_TYPE ISYS_Uninitialize();

//�ͷ��ڴ�
BOOL DFJK_API CALL_TYPE ISYS_FreeMemory(void* pMem);

//////////////////////////////////////////////////////////////////////////
//  2 �豸��غ���
//////////////////////////////////////////////////////////////////////////

/**************************************************************************
(2-1) IDVR_ConnectDevice
����˵��������վ���豸
���������	lpszRvuIP-Rvu IP��ַ
dwPort-Rvu����˿�
lpszUserName-�û���
lpszPassWord-����
iType-���紫������: 0-TCP, 1:UDP, 2: �鲥, 3: RTP
pExParamIn����չ����
dwExParamInLen����չ��������
���������	hDevHandle-�豸���
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_ConnectDevice(LPCTSTR  lpszRvuIP, 
										   DWORD dwPort,
										   LPCTSTR lpszUserName,
										   LPCTSTR lpszPassWord,
										   TRANSTYPE tType, 
										   HANDLE *hDevHandle,
										   LPCTSTR pExParamIn,
										   DWORD dwExParamInLen); 

/**************************************************************************
(2-2) IDVR_DisConnectDevice
����˵�����Ͽ�վ���豸����
���������hDevHandle-�豸���
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_DisConnectDevice(HANDLE hDevHandle);

/**************************************************************************
(2-3) IDVR_StartPreview
����˵����������ͨ��ͼ��Ԥ��
���������hDevHandle-�豸���
hWnd-Ԥ��ͼ�񴰿�
iChannel-ָ��Ԥ��ͨ��(ͨ���Ŵ�1��ʼ)
iStreamType-�������ͣ�0����������1��������
pExParamIn����չ��������ʽ�������ַ���
dwExParamInLen����չ��������
���������	hChanHandle-�򿪵�ͨ�����
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StartPreview(HANDLE hDevHandle,
										  HWND hWnd,
										  int iChannel,
										  int iStreamType,
										  HANDLE *hChanHandle, 
										  LPCTSTR pExParamIn,
										  DWORD dwExParamInLen);

/**************************************************************************
(2-4) IDVR_StartRealData
����˵����������Ƶͨ��ͼ��ʵʱ���ݻص�����
���������hDevHandle-�豸���
iChannel-ָ��Ԥ��ͨ��(ͨ���Ŵ�1��ʼ)
iStreamType-�������ͣ�0����������1��������
pExParamIn����չ��������ʽ�������ַ���[8]
dwExParamInLen����չ��������
callbackHandler ��ȡʵʱ����Ƶ���Ļص�����
���������phChanHandle-�򿪵�ͨ�����
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/


BOOL DFJK_API  CALL_TYPE IDVR_StartRealData(HANDLE hDevHandle,
											int iChannel,
											int iStreamType,
											HANDLE *phChanHandle,
                                            DWORD dwCookie,
											LPCTSTR pExParamIn,
											DWORD dwExParamInLen,
											RealDataCallBack callbackHandler);
					
/**************************************************************************
(2-5) IDVR_GetRealDataHeader
����˵������ȡ��Ƶͨ��ͼ��ʵʱ���ݵ�����ͷ
ʵ��Ҫ�󣺱���ʵ��
���������hChanHandle -ͨ�����
pBuffer - ��������ͷ������
dwBuffLen -��������ͷ����������
���������pdwHeaderLen ����ʵ������ͷ����
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API  CALL_TYPE IDVR_GetRealDataHeader(HANDLE hChanHandle,
												unsigned char *pBuffer,
												DWORD dwBuffLen,
												DWORD *pdwHeaderLen);

/**************************************************************************
(2-6) IDVR_StopRealData
����˵����ֹͣ��Ƶͨ��ͼ��ʵʱ���ݻص�����
���������hChanHandle -ͨ�����
��������� 
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ�� 
/**************************************************************************/
BOOL DFJK_API  CALL_TYPE IDVR_StopRealData(HANDLE hChanHandle);

/**************************************************************************
(2-7) IDVR_SwitchCamera
����˵����ʵ�ֶ���������л�
ʵ��Ҫ�󣺿�ѡʵ��
���������hDevHandle-�豸���
hChanHandle-ͨ�����
pExParamIn����չ��������ʽ�������ַ���[8]
dwExParamInLen����չ��������
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API  CALL_TYPE IDVR_SwitchCamera(HANDLE hDevHandle, 
										   LPCTSTR lpszProtocol, int nAddress, 
										   int nCamNo, int nMonNo, SWITCHTYPE switchType,
										   COMTYPE comType, DWORD dwParam,
										   LPCTSTR pExParamIn, DWORD dwExParamInLen);

/**************************************************************************
(2-8) IDVR_StopPreview
����˵����ֹͣ��ͨ��ͼ��Ԥ��
���������hChanHandle-ͨ�����
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API  CALL_TYPE IDVR_StopPreview(HANDLE hChanHandle);

/**************************************************************************
(2-9) IDVR_CapturePicture
����˵����ʵʱ������Ƶʱץȡָ��ͨ��ͼƬ
���������hChanHandle - ͨ�����
lpszFileName -- ͼƬ����ָ���ļ���,�100�ַ�
pExParamIn����չ��������ʽ�������ַ���[8]
dwExParamInLen����չ��������
������������ޣ�
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ�� 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_CapturePicture(HANDLE hChanHandle,
											LPCTSTR lpszFileName,
											LPCTSTR pExParamIn,
											DWORD dwExParamInLen);

/**************************************************************************
(2-10) IDVR_StartRecord
����˵��������¼���ڱ��ؽ���¼��
���������hChanHandle - ͨ�����
lpszFileName -- ͼƬ����ָ���ļ���,�100�ַ�
pExParamIn����չ��������ʽ�������ַ���[8]
dwExParamInLen����չ��������
������������ޣ�
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ�� 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StartRecord(HANDLE hChanHandle,
										 LPCTSTR lpszFileName, 
										 LPCTSTR pExParamIn, 
										 DWORD dwExParamInLen);

/**************************************************************************
(2-11) IDVR_GetRecordState
����˵������ȡͨ���ı���¼��״̬
���������hChanHandle - ͨ�����
���������pnState ------ ��ͨ��¼��״̬��0��δ¼�� 1������¼��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetRecordState(HANDLE hChanHandle,int *pnState);

/**************************************************************************
(2-12) IDVR_StopRecord
����˵����ֹͣ¼�� ������¼��
���������hChanHandle - ͨ�����
������������ޣ�
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopRecord(HANDLE hChanHandle);

/**************************************************************************
(2-13) IDVR_SetVideoParams
����˵��������ͨ����Ƶ���� 
ʵ��Ҫ�󣺱���ʵ��
���������hChanHandle-ͨ�����
iBrightness-����0-255
iContrast-�Աȶ�0-255
iSaturation-���Ͷ�0-255
iHue-ɫ��0-255
bDefault����Ϊ1��������ΪĬ�ϲ�������ʱ���ȵ�ֵ��Ч��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetVideoParams(HANDLE hChanHandle, 
											int iBrightness, 
											int iContrast, 
											int iSaturation,
											int iHue, 
											BOOL bDefault);

/**************************************************************************
(2-14) IDVR_GetVideoParams
����˵��������ͨ����Ƶ���� 
ʵ��Ҫ�󣺱���ʵ��
���������hChanHandle-ͨ�����
iBrightness-����0-255
iContrast-�Աȶ�0-255
iSaturation-���Ͷ�0-255
iHue-ɫ��0-255
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetVideoParams(HANDLE hChanHandle,
											int *piBrightness,
											int *piContrast,
											int *piSaturation,
											int *piHue);

/**************************************************************************
(2-15) IDVR_StartSound
����˵������ͨ���������� 
ʵ��Ҫ�󣺿�ѡʵ��
���������hChanHandle-ͨ�����
pExParamIn����չ��������ʽ�������ַ���[8]
dwExParamInLen����չ��������
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StartSound(HANDLE hChanHandle, LPCTSTR pExParamIn, DWORD dwExParamInLen);

/**************************************************************************
(2-16) IDVR_SetVolume
����˵��������ͨ�������������� 
ʵ��Ҫ�󣺿�ѡʵ��
���������hChanHandle-ͨ�����
          lVolume-����ֵ(0 - 100)
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetVolume(HANDLE hChanHandle,LONG lVolume);

/**************************************************************************
(2-17) IDVR_StopSound
����˵����ֹͣͨ���������� 
ʵ��Ҫ�󣺿�ѡʵ��
���������hChanHandle-ͨ�����
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopSound(HANDLE hChanHandle);

/**************************************************************************
(2-18) IDVR_StartAudioPhone
����˵�������������Խ�
ʵ��Ҫ�󣺿�ѡʵ��
���������hDevHandle-�豸���
pExParamIn����չ��������ʽ�������ַ���[8]
dwExParamInLen����չ��������
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StartAudioPhone(HANDLE hDevHandle, LPCTSTR pExParamIn, DWORD dwExParamInLen);

/**************************************************************************
(2-19) IDVR_StopAudioPhone
����˵����ֹͣ�����Խ� 
ʵ��Ҫ�󣺿�ѡʵ��
���������hDevHandle-�豸���
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopAudioPhone(HANDLE hDevHandle);

/**************************************************************************
(2-20) IDVR_PTZControl
����˵������̨�ȸ����豸�Ŀ��Ʋ��� 
ʵ��Ҫ�󣺱���ʵ��
���������command-������������
lpszProtocol-Э��
dwParam1-��̨ˮƽ�ٶȣ���Χ��0-10����Ԥ��λ����ʱΪԤ��λ��
dwParam2-��̨��ֱ�ٶȣ���Χ��0-10��
pExParamIn����չ��������ʽ�������ַ���[8]�������ڴ���PTZ���Ƹ�����Ϣ
��չ�ַ���TAG�ֵ䣺
PROTOCOL     ��̨����Э��
dwExParamInLen����չ��������
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_PTZControl(HANDLE hDevHandle,
										int iChannel,
										CTRLTYPE command,
										LPCTSTR lpszProtocol,
										DWORD dwParam1,
										DWORD dwParam2,
										LPCTSTR pExParamIn, 
										DWORD dwExParamInLen);

/**************************************************************************
(2-??) IDVR_PTZControlByChannel
����˵����������Ƶͨ��Ԥ�����������̨�ȸ����豸 
ʵ��Ҫ�󣺱���ʵ��
���������hChanHandle - ��Ƶͨ�����
          ������������ͬ����IDVR_PTZControl
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_PTZControlByChannel(HANDLE hChanHandle,CTRLTYPE command, DWORD dwParam1, DWORD dwParam2, LPCTSTR pExParamIn, DWORD dwExParamInLen);


/**************************************************************************
(2-21) IDVR_GetDeviceInfo
����˵������ȡ�豸�����Ϣ
���������hDevHandle-�豸���
���������pDeviceInfo-�豸״̬��Ϣ�洢�ṹ
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetDeviceInfo(HANDLE hDevHandle,PDEVICEINFO pDeviceInfo);

/**************************************************************************
(2-22) IDVR_GetChannelInfo
����˵������ȡ�豸ͨ�������Ϣ
���������hDevHandle-�豸���
���������pDeviceInfo-ͨ����Ϣ�洢�ṹ
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetChannelInfo(HANDLE hDevHandle,PCHANNELINFO pChannelInfo);

/**************************************************************************
(2-23) IDVR_SetDeviceTime
����˵����Զ��Уʱվ����Ƶ������DVR��
ʵ��Ҫ�󣺱���ʵ��
���������hDevHandle-�豸���
          pTime-ʱ��ṹָ��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ�� 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetDeviceTime(HANDLE hDevHandle, PDEVTIME pTime);

/**************************************************************************
(2-24) IDVR_StartDeviceRecord
����˵����Զ�������ֳ���Ƶ������DVR��ĳͨ������¼��
ʵ��Ҫ�󣺱���ʵ��
���������hDevHandle-�豸���
lChannel-ͨ���ţ���1��ʼ��
lRecordType����ʱ��Ч
pExParamIn����չ��������ʽ�������ַ���[8]
dwExParamInLen����չ��������
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ�� 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StartDeviceRecord(HANDLE hDevHandle,
											   long lChannel,
											   long lRecordType,
											   LPCTSTR pExParamIn,
											   DWORD dwExParamInLen);

/**************************************************************************
(2-25) IDVR_StopDeviceRecord
����˵����Զ��ֹͣ�ֳ���Ƶ������DVR��ĳͨ��¼��
ʵ��Ҫ�󣺱���ʵ��
���������hDevHandle-�豸���
lChannel-ͨ���ţ���1��ʼ��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ�� 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopDeviceRecord(HANDLE hDevHandle,long lChannel);

/**************************************************************************
(2-26) IDVR_GetDeviceRecordStatus
����˵������ȡ�ֳ���Ƶ������DVR��ĳͨ��¼��״̬
ʵ��Ҫ�󣺿�ѡʵ��
���������hDevHandle-�豸���
lChannel-ͨ���ţ���1��ʼ��
pExParamIn����չ��������ʽ�������ַ���[8]
dwExParamInLen����չ��������
���������piState-״ֵָ̬�루״ֵ̬1Ϊ����¼��0Ϊû��¼��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ�� 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetDeviceRecordStatus(HANDLE hDevHandle,
												   long lChannel,
												   int *piState,
												   LPCTSTR pExParamIn,
												   DWORD dwExParamInLen);

/**************************************************************************
(2-27) IDVR_ShowDeviceSettingDlg
����˵�������豸���öԻ���
ʵ��Ҫ�󣺿�ѡʵ��
���������hDevHandle-�豸���
pExParamIn����չ��������ʽ�������ַ���[8]
dwExParamInLen����չ��������
������������ޣ�
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_ShowDeviceSettingDlg(HANDLE hDevHandle, LPCTSTR pExParamIn, DWORD dwExParamInLen);

/**************************************************************************
(2-28) IDVR_RestartDevice
����˵����Զ������վ���豸
ʵ��Ҫ�󣺱���ʵ��
���������hDevHandle-�豸���
������������ޣ�
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ�� 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_RestartDevice (HANDLE hDevHandle);

/**************************************************************************
(2-29) IDVR_GetDeviceState
����˵������ȡRVU�豸����״̬
ʵ��Ҫ�󣺱���ʵ��
���������hDevHandle-�豸���
���������pStat --- ����RVU����״̬�ṹָ��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetDeviceState (HANDLE hDevHandle, DEV_STAT* pStat);

//////////////////////////////////////////////////////////////////////////
//  3  �����������ຯ��
//////////////////////////////////////////////////////////////////////////

/**************************************************************************
(3-1) IDVR_SetIOStatus
����˵��������վ���豸�˿ڵ�״̬������������ֵ 
ʵ��Ҫ�󣺿�ѡʵ��
���������hDevHandle-�豸���
          iIOPort-����˿�
          iValue-���õ���ֵ
pExParamIn����չ��������ʽ�������ַ���[8]�������ڴ���I/O�豸��ز���
dwExParamInLen����չ��������
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetIOStatus(HANDLE hDevHandle, int iIOPort, int iValue,
										 LPCTSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType,
										 LPCTSTR pExParamIn, DWORD dwExParamInLen);

/**************************************************************************
(3-2) IDVR_GetIOStatus
����˵������ȡվ���豸�˿ڵ�״̬������������ֵ 
ʵ��Ҫ�󣺿�ѡʵ��
���������hDevHandle-�豸���
iIOPort-����˿�
pExParamIn����չ��������ʽ�������ַ���[8]�������ڴ���I/O�豸��ز���
dwExParamInLen����չ��������
���������*piValue-��ȡ����ֵ
ppExParamOut����չ�����������ʽ�������ַ����������ڻ�ȡI/O�豸���ֵ
pdwExParamOutLen����չ�����������
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetIOStatus(HANDLE hDevHandle,
										 int iIOPort,
										 int *piValue,
										 LPCTSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType, int nTimeout,
										 LPCTSTR pExParamIn,
										 DWORD dwExParamInLen,
										 LPTSTR* ppExParamOut,
										 DWORD* pdwExParamOutLen);

/**************************************************************************
(3-3) IDVR_SetIOValue
����˵��������վ���豸�˿ڵ�ģ����ֵ 
ʵ��Ҫ�󣺿�ѡʵ��
���������hDevHandle-�豸���
          iIOPort-����˿�
          fValue-���õ���ֵ
pExParamIn����չ��������ʽ�������ַ���[8]�������ڴ���I/O�豸��ز���
dwExParamInLen����չ��������
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetIOValue(HANDLE hDevHandle,
										int iIOPort,
										float fValue,
										LPCTSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType,
										LPCTSTR pExParamIn,
										DWORD dwExParamInLen);

/**************************************************************************
(3-4) IDVR_GetIOValue
����˵������ȡվ���豸�˿ڵ�ģ����ֵ
ʵ��Ҫ�󣺿�ѡʵ��
���������hDevHandle-�豸���
          IOPort-����˿�
pExParamIn����չ��������ʽ�������ַ���[8]�������ڴ���I/O�豸��ز���
dwExParamInLen����չ��������
���������pfValue-��ȡ����ֵ
ppExParamOut����չ������������ظ�ʽ�������ַ���[8]�������ڻ�ȡI/O�豸���ֵ
pdwExParamOutLen����չ�����������
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetIOValue(HANDLE hDevHandle,
										int iIOPort,
										float *pfValue, 
										LPCTSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType, int nTimeout,
										LPCTSTR pExParamIn, 
										DWORD dwExParamInLen, 
										LPTSTR* ppExParamOut, 
										DWORD* pdwExParamOutLen);

/**************************************************************************
(3-5) IDVR_SetAlarmSubscribe
����˵����Ԥ���ֳ��豸������Ϣ
ʵ��Ҫ�󣺿�ѡʵ��
���������hDevHandle-�豸���
dwUser - �û�����
pExParamIn����չ��������ʽ�������ַ���[8]
dwExParamInLen����չ��������
Cbf-�ص�����������ʵʱ����������Ϣ������Ϊ��
typedef int(CALLBACK *CBF_AlarmInfo)(HANDLE hdevice, DWORD dwUser, DWORD dwAlarmType, DWORD dwChanNo, void *pReserved);
dwUser-���������ݵ��û�����
dwAlarmType-���������ͱ�ţ�����Ϊ��
	0:�ź���������dwChanNo:�������1��ʼ,
	1:Ӳ������ 4:Ӳ��δ��ʽ����5:Ӳ�̳���dwChanNo:Ӳ�̺�,1��ʼ
	2:��Ƶ�źŶ�ʧ��3:�ƶ���⣻6:��Ƶ�ڵ���7:��Ƶ��ʽ��ƥ�䡣dwChanNo:ͨ����,1��ʼ
	8:�Ƿ����ʷ�������
dwChanNo-�����������ͨ�����
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
typedef int(CALLBACK *CBF_AlarmInfo)(HANDLE hdevice, DWORD dwUser, ALARMTYPE alarmType, DWORD dwChanNo, void *pReserved);

BOOL DFJK_API CALL_TYPE IDVR_SetAlarmSubscribe(HANDLE hDevHandle,
											   DWORD dwUser,
											   CBF_AlarmInfo cbf,
											   LPCTSTR pExParamIn,
											   DWORD dwExParamInLen);

/**************************************************************************
(3-6) IDVR_SetAlarmUnsubscribe
����˵����ֹͣԤ���ֳ��豸������Ϣ
ʵ��Ҫ�󣺿�ѡʵ��
���������hDevHandle-�豸���
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
�����汾��V1.0
����˵����(��)
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetAlarmUnsubscribe(HANDLE hDevHandle);

/**************************************************************************
(3-7) IDVR_AlarmSet
����˵�������ֳ��������в����볷��
ʵ��Ҫ�󣺱���ʵ��
���������hDevHandle-�豸���
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_AlarmSet(HANDLE hDevHandle, 
									  int iChannel, 
									  LPCTSTR lpszProtocol, 
									  int nDevAddr, 
									  COMTYPE comType, 
									  BOOL bEnable, 
									  DWORD dwParam);

/**************************************************************************
(3-8) IDVR_GetAlarmSetStatus
����˵������ȡ�ֳ������豸����/����״̬
ʵ��Ҫ�󣺱���ʵ��
���������hDevHandle-�豸���
		  iChannel-��������ţ�ȡֵ��0��1��2������
		  lpszProtocol-�����豸Э�飬��RVU����������Ϊ���ַ�����"EDVR"
		  nDevAddr - �����豸��ַ��1��2��3��......��
		  comType- �����豸�����ӵ�RVUͨѶ��������,����ο�����IDVR_SetIOStatus
		  dwParam - ��������������0
���������pEnabled - TRUE:������FALSE����������iChannel>0ʱ��Ч��
		  ppExParamOut����չ������������ظ�ʽ�������ַ���[8]�����ڻ�ȡ��������Ĳ���/����״̬
		  pdwExParamOutLen����չ�����������
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL CALL_TYPE IDVR_GetAlarmSetStatus(HANDLE hDevHandle, int iChannel, LPCTSTR lpszProtocol, 
									  int nDevAddr, COMTYPE comType, BOOL* pEnabled, 
									  LPTSTR* ppExParamOut, DWORD* pdwExParamOutLen);

/**************************************************************************
(3-9) IDVR_SerialStart
����˵�������豸͸������
ʵ��Ҫ�󣺱���ʵ��
���������  hDevHandle-�豸���
			lSerialPort -���ںţ�1-232 ���ڣ�2-485 ����
			dwUser - �û�����
			fSerialDataCallBack�����ݻص�������
�ص���������˵���� 
	hSerial - ���ھ��
	pRecvDataBuffer����Ž��յ����ݵĻ�����ָ�� 
	dwBufSize���������Ĵ�С 
	dwUser�����洫����û�����
���������phSerial - ����Ĵ��ھ��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
typedef void(CALLBACK *CBF_SerialDataCallBack)(HANDLE hSerial, char *pRecvDataBuffer, DWORD dwBufSize, DWORD dwUser);
BOOL DFJK_API CALL_TYPE IDVR_SerialStart(HANDLE hDevHandle, 
									     LONG lSerialPort,
										 CBF_SerialDataCallBack cbf,
										 DWORD dwUser, 
										 HANDLE* phSerial);

/**************************************************************************
(3-10) IDVR_SerialSend
����˵������͸�����ڷ�������
ʵ��Ҫ�󣺱���ʵ��
���������  hSerial-͸�����ھ��
			lChannel -Ӳ��¼�������Ƶͨ����, �� 485 ����͸��ͨ��ʱ,ָ�����ĸ�ͨ�������ݣ��� 232 ����͸��ͨ��ʱ���ó� 0;
			pSendBuf��Ҫ���͵����ݻ�����ָ�� 
			dwBufSize����������С 
������������ޣ�
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SerialSend(HANDLE hSerial, LONG lChannel, char *pSendBuf, DWORD dwBufSize);

/**************************************************************************
(3-11) IDVR_SerialStop
����˵�����ر�͸������
ʵ��Ҫ�󣺱���ʵ��
���������hSerial-͸�����ھ��
������������ޣ�
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SerialStop(HANDLE hSerial);

//////////////////////////////////////////////////////////////////////////
//  4	Զ�̼������طš����ص�
//////////////////////////////////////////////////////////////////////////

/**************************************************************************
(4-1) IDVR_QueryDevRecord
����˵������ѯRVU�豸�ڵķ���������¼���ļ�
ʵ��Ҫ�󣺱���ʵ��
���������hDevHandle-�豸�����
iChannel-ͨ�����(��1��ʼ)��
RecType-¼�����ͣ�
���������ppExParamOut����չ�������������XML�ַ�����
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_QueryDevRecord(HANDLE hDevHandle,
											int iChannel,
											RECORDTYPE RecType,
											PQUERYTIME ptime,
											LPTSTR* ppExParamOut,
											DWORD* pdwExParamOutLen);

/**************************************************************************
(4-2) IDVR_CancelQueryDevRecord
����˵����ȡ�����ڽ��е�¼���ļ��Ĳ�ѯ����
ʵ��Ҫ�󣺿�ѡʵ��
���������hDevHandle-�豸���
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_CancelQueryDevRecord(HANDLE hDevHandle);

/**************************************************************************
(4-3) IDVR_DownloadFile
����˵��������RVU�е�¼���ļ�������
ʵ��Ҫ�󣺱���ʵ��
���������hDevHandle-�豸���
lpszSrcFileName-ѡ�����ֳ�¼���ļ�����ȫ·������
lpszLocalFileName -���ش洢�ļ�����ȫ·������
���������phDownHandle - ���ؾ��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_DownloadFile(HANDLE hDevHandle, LPCTSTR lpszSrcFileName, LPCTSTR lpszLocalFileName, HANDLE* phDownHandle);

/**************************************************************************
(4-4) IDVR_StopDownloadFile
����˵����ֹͣ�����ֳ��豸�е�¼���ļ�
ʵ��Ҫ�󣺿�ѡʵ��
���������hDownHandle - ���ؾ��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��

/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopDownloadFile(HANDLE hDownHandle);

/**************************************************************************
(4-5) IDVR_GetDownloadPos
����˵������ȡ��ǰ���ؽ���
ʵ��Ҫ�󣺿�ѡʵ��
���������hDownHandle - ���ؾ��
���������piCurPos-��õĵ�ǰ���ؽ���ֵָ�� ��<0:ʧ�ܣ�0��99:���ؽ��ȣ�100:���ؽ�����>100:��������ԭ���DVRæ,�����쳣��ֹ��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetDownloadPos(HANDLE hDownHandle, int *piCurPos);

/**************************************************************************
(4-?) IDVR_DownloadFileByTime
����˵������ʱ�������ֳ��豸�е�¼���ļ�������
ʵ��Ҫ�󣺱���ʵ��
���������hDevHandle-�豸���
iChannel - ��Ƶͨ���ţ�1��ʼ
pTime - ʱ�䷶Χ
lpszLocalFileName -���ش洢�ļ�����ȫ·������
���������phDownHandle - ���ؾ��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_DownloadFileByTime(HANDLE hDevHandle, int iChannel,  PQUERYTIME  pTime, LPCTSTR lpszLocalFileName, HANDLE* phDownHandle);


/**************************************************************************
(4-7) IDVR_RemotePlay
����˵�����ط�Զ��¼���ļ���RVU�ڵ�¼���ļ���
ʵ��Ҫ�󣺱���ʵ��
���������hDevHandle-�豸���
lpszSrcFileName - RVU�ڵ�¼���ļ���
hWnd - �طŴ��ھ��
���������hPlay - �طž��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_RemotePlay (HANDLE hDevHandle, LPCTSTR lpszSrcFileName, HWND hWnd, HANDLE* hPlay);

/**************************************************************************
(4-8) IDVR_RemotePlayByTime
����˵������ʱ��ط�Զ��¼���ļ�
ʵ��Ҫ�󣺱���ʵ��
���������hDevHandle-�豸���
iChannel - ��Ƶͨ���ţ�1��ʼ
pTime - ʱ�䷶Χ
hWnd - �طŴ��ھ��
���������hPlay - �طž��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_RemotePlayByTime (HANDLE hDevHandle, int iChannel, PQUERYTIME pTime, HWND hWnd, HANDLE* hPlay);

/**************************************************************************
(4-9) IDVR_StopRemotePlay
����˵����ֹͣ�ط�Զ��¼���ļ�
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-10) IDVR_PauseRemotePlay
����˵������ͣ�ط�Զ��¼���ļ�
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_PauseRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-11) IDVR_ResumeRemotePlay
����˵�����ָ��ط�Զ��¼���ļ�����ͣ��ָ����ţ�
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_ResumeRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-12) IDVR_StepRemotePlay
����˵������֡�ط�Զ��¼���ļ�
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StepRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-13) IDVR_FastRemotePlay
����˵��������ط�Զ��¼���ļ�
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_FastRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-14) IDVR_SlowRemotePlay
����˵�������ٻط�Զ��¼���ļ�
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SlowRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-15) IDVR_NormalRemotePlay
����˵���������ٶȻط�Զ��¼���ļ������ڿ졢������֡���ź�ָ���
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_NormalRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-16) IDVR_GetRemotePlayPos
����˵������ȡԶ�̻طŽ���
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
���������pfPos - ���ȣ�0.0��1.0��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetRemotePlayPos(HANDLE hPlay, float *pfPos);

/**************************************************************************
(4-17) IDVR_SetRemotePlayPos
����˵��������Զ�̻طŽ��ȣ��϶����ţ�
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
fPos������ֵ��0.0��1.0��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetRemotePlayPos(HANDLE hPlay, float fPos);

/**************************************************************************
(4-18) IDVR_RemotePlayCapturePicture
����˵����Զ�̻ط�ʱץͼ
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
lpszFileName -  ͼƬ����ָ���ļ���,�100�ַ�
pExParamIn����չ��������ʽ�������ַ���[8]
dwExParamInLen����չ��������
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_RemotePlayCapturePicture(HANDLE hPlay,
												  LPCTSTR lpszFileName,
												  LPCTSTR pExParamIn,
												  DWORD dwExParamInLen);

/**************************************************************************
(4-19) IDVR_RemotePlaySaveData
����˵����Զ�̻ط�ʱ��������
ʵ��Ҫ���Ƽ�ʵ��
���������hPlay - �طž��
lpszRecFileName -  �������ݵ�¼���ļ���
pExParamIn����չ��������ʽ�������ַ���[8]
dwExParamInLen����չ��������
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_RemotePlaySaveData(HANDLE hPlay,
												LPCTSTR lpszRecFileName,
												LPCTSTR pExParamIn,
												DWORD dwExParamInLen);

/**************************************************************************
(4-20) IDVR_StopRemotePlaySaveData
����˵����ֹͣԶ�̻ط�ʱ��������
ʵ��Ҫ���Ƽ�ʵ��
���������hPlay - �طž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopRemotePlaySaveData(HANDLE hPlay);

/**************************************************************************
(4-21) IDVR_StopRemotePlaySaveData
����˵��������Զ�̻ط�ʱ�������ص�����
ʵ��Ҫ���Ƽ�ʵ��
���������hPlay - �طž��
		  dwCookie -�ص�������������û���ʶ����
	  	  pExParamIn����չ��������ʽ�������ַ���[8]������ΪNULL
		  dwExParamInLen����չ�������� ������Ϊ0
		  callbackHandler �������ݻص����̣�����κ���IDVR_StartRealData
������������ޣ�
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetRemotePlayDataCallBack (HANDLE hPlay, DWORD dwCookie, LPCTSTR pExParamIn, DWORD dwExParamInLen, RealDataCallBack callbackHandler);

/**************************************************************************
(4-22) IDVR_GetRemoteTotalTime
����˵������ȡԶ�̲���ʱ���ܲ���ʱ��
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
���������pdwTotalTime -�ܲ���ʱ�䣨��λ���룩
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetRemoteTotalTime(HANDLE hPlay, DWORD *pdwTotalTime);

/**************************************************************************
(4-23) IDVR_GetRemotePlayedTime
����˵��������Զ�̻ط�ʱ�������ص�����
ʵ��Ҫ���Ƽ�ʵ��
�书��˵������ȡԶ�̲���ʱ�ĵ�ǰ����ʱ��
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
���������pdwPlayedTime-��ǰ����ʱ�䣨��λ���룩
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetRemotePlayedTime (HANDLE hPlay, DWORD *pdwPlayedTime);

/**************************************************************************
(4-24) IDVR_GetRemoteTotalFrames
����˵������ȡԶ�̻ط�ʱ�ܵ�֡��
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
���������pdwTotalFrames-�ܵĲ���֡��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetRemoteTotalFrames (HANDLE hPlay, DWORD *pdwTotalFrames);

/**************************************************************************
(4-25) IDVR_GetRemotePlayedFrames
����˵������ȡԶ�̻ط�ʱ�ĵ�ǰ����֡��
ʵ��Ҫ�󣺱���ʵ��	
���������hPlay - �طž��
���������pdwPlayedFrames-��ǰ���ŵ�֡��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetRemotePlayedFrames (HANDLE hPlay, DWORD *pdwPlayedFrames);

/**************************************************************************
(4-26) IDVR_GetPlayBackOsdTime
����˵������ȡԶ�̻ط�ʱ����Ƶ����ʱ��
ʵ��Ҫ�󣺱���ʵ��
���������hPlay - �طž��
���������pOsdTime - ��Ƶ����ʱ�䣬����ο�IDVR_SetDeviceTime
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetPlayBackOsdTime(HANDLE hPlay, PDEVTIME pOsdTime);

/**************************************************************************
����˵������̬����һ���ؼ�֡
ʵ��Ҫ�󣺿�ѡʵ��
���������hDevHandle-�豸���
lChannel-ͨ����
streamtype-0Ϊ������ 1Ϊ������
����ֵ��TRUE ��ʾ�ɹ���FALSE ��ʾʧ�ܡ� 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_PrepareStream(HANDLE hDevHandle,long lChannel,int sTreamtype);

/**************************************************************************
����˵�������ý���ʱ���� B ֡�ĸ���, dwNum :0 ����,1-��һ��,2-�� 2 �� 
����˵��: 
hChanHandle-ͨ����� 
m_ dwNum-���� B ֡�ĸ���,ȡֵΪ 0��1��2��0-������1-�� 1 �� B ֡��2-�� 2 �� B ֡���ڶ�
·����ʱ���� B ֡�������Խ��� CPU �������ʣ�������һ·ʱ����ò����� B֡ 
����ֵ��TRUE ��ʾ�ɹ���FALSE ��ʾʧ�ܡ�  
/**************************************************************************/

BOOL  DFJK_API CALL_TYPE IDVR_ThrowBFrame(HANDLE hChanHandle,DWORD dwNum);


/**************************************************************************
����˵���������Խ�
����˵��: 
����ֵ��TRUE ��ʾ�ɹ���FALSE ��ʾʧ�ܡ�  
/**************************************************************************/
typedef void(CALLBACK *CBF_VoiceData)(
  LONG		lVoiceComHandle,
  char     *pRecvDataBuffer,
  DWORD    dwBufSize,
  BYTE     byAudioFlag,
  void     *pUser
);
//�����Խ���֧�ֻص�������IDVR_StopAudioPhone�����Խ�
BOOL DFJK_API CALL_TYPE IDVR_StartAudioPhone_EX(HANDLE hDevHandle, DWORD dwVoiceChan, BOOL bNeedCBNoEncData,
		CBF_VoiceData  cbVoiceData, void *pUser);

//�����㲥
typedef void(CALLBACK *CBF_VoiceDataBroad)(char *pRecvDataBuffer, DWORD dwBufSize, void *pUser);
BOOL DFJK_API CALL_TYPE IDVR_StartAudioBroadcast(CBF_VoiceDataBroad cbVoiceData, void *pUser);
BOOL DFJK_API CALL_TYPE IDVR_AudioBroadcast_AddDev(HANDLE hDevHandle, DWORD dwVoiceChan);
BOOL DFJK_API CALL_TYPE IDVR_AudioBroadcast_DelDev(HANDLE hDevHandle);
BOOL DFJK_API CALL_TYPE IDVR_StopAudioBroadcast();

//�豸�쳣 ��δ��...��
//typedef void (CALLBACK* CBF_ExceptionCallBack)(HANDLE hDevHandle, DWORD dwType, LONG lHandle, void *pUser);
//BOOL DFJK_API CALL_TYPE IDVR_SetExceptionCallBack(CBF_ExceptionCallBack cbException, void *pUser);

//���� (url��ʽDF1500ͨ��url)
BOOL DFJK_API CALL_TYPE IDVR_Decode(HANDLE hDevHandle, DWORD dwDecodeChan, BOOL bStart, LPCTSTR lpszUrl);

//����ͻָ��豸���ò�����bGetorSet--FALSE����ȡ��TRUE�����棩
BOOL DFJK_API CALL_TYPE IDVR_ConfigFile(HANDLE hDevHandle, BOOL bGetorSet, LPCTSTR lpsFileName);


#ifdef __cplusplus
};
#endif
