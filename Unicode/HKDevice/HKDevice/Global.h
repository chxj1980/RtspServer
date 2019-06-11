 
//
//	Global.h --- ��, �ṹ����, ȫ�ֺ���,��������
//

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <afxtempl.h>
#include "ThreadMutex.h"
#include "ComDev.h"
#include "CommonFunc.h"
//����豸SDKͷ�ļ�
#include "..\..\HKSDK\HCNetSDK.h"
#include "..\..\HKSDK\PlayM4.h"			//ץͼ�ã�
#include "..\..\HKSDK\ShowRemCfgWnd.h"

//-------------------------------------------------------------------------------------
//        �궨��
//-------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//�궨��
#define DFJK_MAXLEN_LASTERROR		1024		//������Ϣ��������С

#define DFJK_INVALID_HANDLE			-1		//��Ч������壨�ǲ���ڲ�ʹ�õģ����ݱ��������޸ģ�

#define DFJK_MAX_CHAN			32		//ÿ̨�豸���ͨ����
#define DFJK_MAX_LINK			50		//ÿ̨�豸�����������ͬһ��ͨ����������Σ�
#define DFJK_MAX_ALARMIN		16		//ÿ̨�豸��౨������
#define DFJK_MAX_ALARMOUT		16		//ÿ̨�豸��౨�����

#define DFJK_MAX_TRANS			8		//��Ƶ���ת������

#define MAXLEN_STR				50
#define MAXLEN_IP				16


//-------------------------------------------------------------------------------------
//        �ṹ����
//-------------------------------------------------------------------------------------


//ͨ�����ࣨ����ʵʱԤ����Զ��¼��طţ�
class CChannelBase
{
public:
	CChannelBase()
	{
		pStreamHeader = NULL;
		dwStreamheaderLen = 0;
		pfnRealDataCallBack = NULL;
		pfnDrawCallBack = NULL;
		hWaitHeaderEvent = NULL;
		dwCookie = 0;
		pUserData = NULL;
	}
	~CChannelBase()
	{
		if (pStreamHeader)
		{
			delete []pStreamHeader;
			pStreamHeader = NULL;
		}
	}

	BYTE*	pStreamHeader;
	DWORD	dwStreamheaderLen;
	RealDataCallBack  pfnRealDataCallBack;
	DrawCallBack pfnDrawCallBack;
	HANDLE hWaitHeaderEvent;
	DWORD	dwCookie;
	void*	pUserData;
};

//��Ƶͨ����
class CChannel: public CChannelBase
{
public:
	CChannel()
	{
		nChanNo			= 0;//ͨ����
		bIsRecording	= FALSE;//�Ƿ�����¼��
		lChanHandle		= DFJK_INVALID_HANDLE;//ͨ�����
		lSoundHandle	= DFJK_INVALID_HANDLE;//��Ƶ���
	}
	~CChannel()
	{
	}

public:
	//char	szChanName[MAXLEN_STR];
	short	nChanNo;			//ͨ����
	long	lChanHandle;		//ͨ�����
	BOOL	bIsRecording;		//�Ƿ�����¼��
	//DWORD	dwRecordTime;		//¼��ʱ��
	//DWORD	dwStartRecordTime;	//��ʼ¼��ʱ��(TickCount)
	//int	iConnectType;		//�����������ͣ�1:TCP, 2:UDP, 3:Multi, 4:RTP
	//CVideoStream* pVideoStream;
	long	lSoundHandle;		//ͨ�����������for DL
};

//����¼���ļ���
// class CRecDownload
// {
// public:
// 	CRecDownload()
// 	{
// 		nChanNo			= 0;
// 		lDownHandle		= DFJK_INVALID_HANDLE;
// 		nDownPercent	= 0;
// 	}
// 	~CRecDownload()
// 	{
// 	}
// 
// public:
// 	short	nChanNo;			//ͨ����
// 	long	lDownHandle;		//���ؾ��
// 	short	nDownPercent;		//���ذٷֱ�
// };

//�ط��ࣨ��طŲ��������ͬ��
class CRecPlay: public CChannelBase
{
public:
	CRecPlay()
	{
		lPlayHandle		= DFJK_INVALID_HANDLE;
		nPlayerIndex	= DFJK_INVALID_HANDLE;
		lWidth			= 0;
		lHeight			= 0;
		bRemotePlayByTime = FALSE;
		bIsRemotePlay	= FALSE;
		hVirtualPlayWnd	= NULL;
	}
	~CRecPlay()
	{
	}

public:
	long	lPlayHandle;		//���ž��
	int		nPlayerIndex;		//���ž��
	long	lWidth;				//ͼ����
	long	lHeight;			//ͼ��߶�
	BOOL	bIsRemotePlay;		//�Ƿ�ΪԶ�̻ط�
	BOOL	bRemotePlayByTime;	//�Ƿ�ΪԶ�̰�ʱ��ط�
	HWND	hVirtualPlayWnd;	//�ص���ʽԤ����
	HANDLE  hWaitHeaderEvent;	//�ȴ��ص�
};

//RVU��
class CRvu
{
public:
	CRvu();
	~CRvu();

//	BOOL AddDownload(CRecDownload* pDown);		//����һ��������ʵ��
//	BOOL RemoveDownload(CRecDownload* pDown);		//����һ��������ʵ��

public:
	char	szRvuName[MAXLEN_STR];
	char	szRvuIP[MAXLEN_IP];
	char	szRvuMultiIP[MAXLEN_IP];
	char	szRvuUserName[MAXLEN_STR];
	char	szRvuPassword[MAXLEN_STR];
	long	lRvuHandle;					//RVU���
	long	lStartChannel;				//��ʼͨ����
	int		iChannelNumber;				//ͨ����
	WORD	wServerPort;				//�������˿ں�
	WORD	wServerType;				//DVR����
	DWORD	dwAlarmInNum;				//�����������
	DWORD	dwAlarmOutNum;				//�����������
	DWORD	dwDiskNum;					//Ӳ�̸���
	long	lAlarmHandle;				//�������
	long	lSerialHandle;				//͸�����ھ��
	long	l232Handle;					//͸��232�ھ��
	long	l485Handle;					//͸��485�ھ��
	long	lVoiceComHandle;			//�����Խ����
	CPtrArray m_arrayChan;				//ͨ���б�
//	CChannel* pChan[DFJK_MAX_CHAN];		//ͨ����ʵ��ָ��
	//BOOL	bAlarmIn[DFJK_MAX_ALARMIN][2];	//DVS��������״̬��[][0]Ϊ����״̬��[][1]Ϊ�ڲ�ʹ�ñ���
	int		iConnectType;				//�����������ͣ�1:TCP, 2:UDP, 3:Multi
	//MRPARAM mrParam[DFJK_MAX_TRANS];
//	CPtrArray m_arrayDownload;			//�����б�
	DWORD	dwAlarmUserData;			//�����ص������û�����
};

class CRvuManager
{
public:
	CThreadMutex m_mutex;
	CPtrArray m_arrayRvu;

public:
	BOOL AddRvu(CRvu* pRvu);					//����һ��Rvuʵ��
	BOOL RemoveRvu(CRvu* pRvu);					//ɾ��һ��Rvuʵ��
	BOOL RemoveRvuByHandle(HANDLE hRvu);		//ɾ��һ��Rvuʵ��������hRvu��

	BOOL AddChan(CRvu* pRvu, CChannel* pChan);	//����һ��Channelʵ��
	BOOL RemoveChan(CChannel* pChan);			//ɾ��һ��Channelʵ��

	CRvu* GetRvuByHandle(long lRvuHandle);		//����Rvu�����ȡRvuʵ��
	CRvu* GetRvuByChan(CChannel* pChan);		//����ĳͨ��ʵ������ȡ����Rvuʵ��
	CRvu* GetRvuByChanHandle(long lChanHandle);		//����ĳͨ���������ȡ����Rvuʵ��
	CChannel* GetChan(HANDLE hRvu, short nChan);	//����Rvu�����ȡͨ����ʵ��

	CRvu* GetRvuBySerial(LONG lSerial, int & nType);

	void ClearAll();							//�ͷ�һ��
};

//-------------------------------------------------------------------------------------
//        ȫ�ֱ�������
//-------------------------------------------------------------------------------------

extern CRvuManager g_rvuMgr;

extern TCHAR g_szLastError[DFJK_MAXLEN_LASTERROR];	//Last Error

extern TCHAR g_szDLLPath[MAX_PATH];					//DLL ����·��

//-------------------------------------------------------------------------------------
//        ȫ�ֺ�������
//-------------------------------------------------------------------------------------

//����չ������ȡTagֵ
BOOL gF_GetTagValFromExParam(LPCTSTR lpszExParam, LPCTSTR lpszTagName, CString& strVal);

#endif

