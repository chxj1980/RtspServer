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
//  5	�����ط���
//////////////////////////////////////////////////////////////////////////


/**************************************************************************
(5-1) IPLY_OpenFile
����˵�����򿪱���¼���ļ�
ʵ��Ҫ�󣺱���ʵ��
���������lpszFileName -¼���ļ�����ȫ·������
���������phFileHandle-�ļ��򿪾��ָ��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_OpenFile(LPCTSTR lpszFileName, HANDLE *phFileHandle);

/**************************************************************************
(5-2) IPLY_CloseFile
����˵�����ر��Ѵ򿪵ı���¼���ļ�
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-¼���ļ���� 
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_CloseFile(HANDLE hFileHandle);

/**************************************************************************
(5-3) IPLY_Play
����˵��������¼���ļ�
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-�򿪵�¼���ļ����
          hWnd-��ʾ���ھ��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_Play(HANDLE hFileHandle, HWND hWnd);

/**************************************************************************
(5-4) IPLY_Stop
����˵����ֹͣ����¼���ļ�
ʵ��Ҫ�󣺱���ʵ��
��������� hFileHandle-�ļ��򿪾��
������������ޣ�
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_Stop(HANDLE hFileHandle);

/**************************************************************************
(4-10) IPLY_CapturePicture
����˵�����ط�ʱץȡͼƬ
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-¼�񲥷ž��
lpszBmpFileName -ͼƬ�����ļ�����ȫ·������
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_CapturePicture(HANDLE hFileHandle,LPCTSTR lpszBmpFileName);

/**************************************************************************
(4-11) IPLY_RefreshPlay
����˵����ˢ����ʾ���Ŵ���
ʵ��Ҫ�󣺿�ѡʵ��
���������hFileHandle-¼�񲥷ž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_RefreshPlay(HANDLE hFileHandle);

/**************************************************************************
(4-12) IPLY_Pause
����˵������ͣ����¼��
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-¼�񲥷ž��
bPause-TRUE:��ͣ���ţ�FALSE:��������
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_Pause(HANDLE hFileHandle,BOOL bPause);

/**************************************************************************
(4-13) IPLY_Fast
����˵�����������
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-¼�񲥷ž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_Fast(HANDLE hFileHandle);

/**************************************************************************
(4-14) IPLY_Slow
����˵�������ٲ���
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-¼�񲥷ž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_Slow(HANDLE hFileHandle);

/**************************************************************************
(4-15) IPLY_FastBack
����˵�������ٺ��˲���
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-¼�񲥷ž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_FastBack(HANDLE hFileHandle);

/**************************************************************************
(4-16) IPLY_SlowBack
����˵�������ٺ��˲���
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-¼�񲥷ž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_SlowBack(HANDLE hFileHandle);

/**************************************************************************
(4-17) IPLY_Step
����˵������֡����
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-¼�񲥷ž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_Step(HANDLE hFileHandle);

/**************************************************************************
(4-??) IPLY_StepBack
����˵������֡���ű���¼���ļ�
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-¼�񲥷ž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_StepBack(HANDLE hFileHandle);


/**************************************************************************
(4-18) IPLY_SetPlayPos
����˵���������ļ�����λ��
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-¼�񲥷ž��
          fRelaPos-��Բ���λ�ã�0.0��1.0��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_SetPlayPos(HANDLE hFileHandle, float fRelaPos);

/**************************************************************************
(4-19) IPLY_GetPlayPos
����˵������ȡ��ǰ����λ��
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-¼�񲥷ž��
���������pfRelaPos-��ǰ����λ��ָ��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_GetPlayPos(HANDLE hFileHandle, float *pfRelaPos);

/**************************************************************************
(4-20) IPLY_GetTotalTime
����˵������ȡ�ļ��ܵĲ���ʱ��
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-¼�񲥷ž��
pdwFileTime-�ļ��ܵĲ���ʱ�䣨��λ���룩
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_GetTotalTime(HANDLE hFileHandle, DWORD *pdwFileTime);

/**************************************************************************
(4-21) IPLY_GetPlayedTime
����˵������ȡ¼���ļ���ǰ����ʱ��
ʵ��Ҫ�󣺱���ʵ��
���������hFileHandle-¼�񲥷ž��
pdwPlayedTime-¼���ļ���ǰ����ʱ�䣨��λ���룩
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_GetPlayedTime(HANDLE hFileHandle,DWORD *pdwPlayedTime);

/**************************************************************************
(4-22) IPLY_GetTotalFrames
����˵������ȡ¼���ļ��ܵ�֡��
ʵ��Ҫ�󣺿�ѡʵ��
���������hFileHandle-¼�񲥷ž��
pdwTotalFrames-��ȡ�ļ��ܵĲ���֡��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_GetTotalFrames(HANDLE hFileHandle, DWORD *pdwTotalFrames);

/**************************************************************************
(4-23) IPLY_GetPlayedFrames
����˵������ȡ¼���ļ���ǰ����֡��
ʵ��Ҫ�󣺿�ѡʵ��
���������hFileHandle-¼�񲥷ž��
pdwPlayedFrames-��ǰ���ŵ�֡��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_GetPlayedFrames(HANDLE hFileHandle, DWORD *pdwPlayedFrames);

/**************************************************************************
(4-24) IPLY_StartSound
����˵�����ط�¼���ļ�ʱ��������
ʵ��Ҫ�󣺿�ѡʵ��
���������hFileHandle-¼�񲥷ž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_StartSound(HANDLE hFileHandle);

/**************************************************************************
(4-25) IPLY_StopSound
����˵�����ط�¼���ļ�ʱֹͣ��������
ʵ��Ҫ�󣺿�ѡʵ��
���������hFileHandle-¼�񲥷ž��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_StopSound(HANDLE hFileHandle);

/**************************************************************************
(4-26) IPLY_SetVolume
����˵�����ط�ʱ������������
ʵ��Ҫ�󣺿�ѡʵ��
���������hFileHandle-¼�񲥷ž��
dwVolume-����ֵ��0-100��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_SetVolume(HANDLE hFileHandle, DWORD dwVolume);

/**************************************************************************
(4-27) IPLY_SetVideoParams
����˵�������ûط�ʱ��ͼ�����
ʵ��Ҫ�󣺿�ѡʵ��
���������hFileHandle-¼�񲥷ž��
iBrightness-����0-255
iContrast-�Աȶ�0-255
iSaturation-���Ͷ�0-255
iHue-ɫ��0-255
bDefault - ��Ϊ1��������ΪĬ�ϲ�������ʱ���ȵ�ֵ��Ч��
���������(��)
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_SetVideoParams(HANDLE hFileHandle,
											int iBrightness,
											int iContrast,
											int iSaturation,
											int iHue,
											BOOL bDefault);

/**************************************************************************
(4-28) IPLY_GetVideoParams
����˵������ȡ�ط�ʱ��ͼ�����
ʵ��Ҫ�󣺿�ѡʵ��
���������hFileHandle-¼�񲥷ž��
���������piBrightness-����0-255
piContrast-�Աȶ�0-255
piSaturation-���Ͷ�0-255
piHue-ɫ��0-255
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_GetVideoParams(HANDLE hFileHandle,
											int *piBrightness,
											int *piContrast,
											int *piSaturation,
											int *piHue);

/**************************************************************************
(4-29) IPLY_OpenPlayStream
����˵����������ģʽ���Ż�����
ʵ��Ҫ�󣺱���ʵ��
��������� 
pStreamHeader - ����������ͷ
dwHeaderLen   -  ����������ͷ����
dwStreamLen   -  �����ݻ���������
dwParam1	    - ��������
���������
phStreamHandle - ���ؽ������ľ��
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_OpenPlayStream(BYTE* pStreamHeader,
											DWORD dwHeaderLen,
											DWORD dwStreamLen,
											DWORD dwParam1,
											HANDLE *phStreamHandle);

/**************************************************************************
(4-30) IPLY_InputStreamData
����˵��������ģʽ���Ż������м�����յ�ʵʱ����
ʵ��Ҫ�󣺱���ʵ��
��������� 
hStreamHandle- �����
pBuffer -  ����ʵʱ����
dwStreamLen   -  ����ʵʱ���ݳ���
dwParam1	    - ��������
dwParam2		- ��������
���������
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_InputStreamData(HANDLE hStreamHandle,
											 BYTE* pBuffer,
											 DWORD dwStreamLen,
											 DWORD* dwParam1,
											 DWORD dwParam2);

/**************************************************************************
(4-31) IPLY_StreamPlay
����˵��������ģʽ����
ʵ��Ҫ�󣺱���ʵ��
��������� 
hStreamHandle- �����
pBuffer -  ����ʵʱ����
dwStreamLen   -  ����ʵʱ���ݳ���
dwParam1	    - ��������
dwParam2		- ��������
���������
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_StreamPlay(HANDLE hStreamHandle,HWND hWnd);

/**************************************************************************
(4-32) IPLY_StreamPause
����˵��������ģʽ����ʱ��ͣ��ָ�
ʵ��Ҫ�󣺿�ѡʵ��
��������� 
hStreamHandle- �����
pBuffer -  ����ʵʱ����
dwStreamLen   -  ����ʵʱ���ݳ���
dwParam1	    - ��������
dwParam2		- ��������
���������
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_StreamPause(HANDLE hStreamHandle,BOOL bPause);

/**************************************************************************
(4-33) IPLY_ResetStreamBuffer
����˵������λ��ģʽ�������ݻ�����
ʵ��Ҫ�󣺱���ʵ��
��������� 
hStreamHandle- �����
���������
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_ResetStreamBuffer(HANDLE hStreamHandle);

/**************************************************************************
(4-34) IPLY_StreamRefreshPlay
����˵��������ģʽˢ�²��Ŵ���
ʵ��Ҫ�󣺱���ʵ��
��������� 
hStreamHandle- �����
���������
�������أ�TRUE��ִ�гɹ� / FALSE��ִ��ʧ��
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_StreamRefreshPlay( HANDLE hStreamHandle);


BOOL DFJK_API  CALL_TYPE IPLY_StreamStop(HANDLE hStreamHandle);


BOOL DFJK_API  CALL_TYPE IPLY_CloseStream(HANDLE hStreamHandle);


//�û��Զ��庯��
//BOOL DFJK_API CALL_TYPE ISYS_CustomFunction(LPCTSTR pParamIn, DWORD dwParamInLen, LPTSTR *ppParamOut, DWORD* pdwParamOutLen);

//���ӷŴ�
BOOL DFJK_API  CALL_TYPE IPLY_SetDisplayRegion(HANDLE hPlayHandle,DWORD nRegionNum, RECT *pSrcRect, HWND hDestWnd, BOOL bEnable);
BOOL DFJK_API  CALL_TYPE IPLY_RefreshPlayEx(HANDLE hPlayHandle, DWORD nRegionNum);


//ע���ͼ�ص�����
//��ͼ�ص�����
typedef void(CALLBACK *DrawCallBack)(HANDLE hPlayHandle, HDC hDC, void* pUserData);
BOOL DFJK_API CALL_TYPE IPLY_RegisterDrawFun(HANDLE hPlayHandle, DrawCallBack callbackDraw, void* pUserData);

#ifdef __cplusplus
};
#endif
