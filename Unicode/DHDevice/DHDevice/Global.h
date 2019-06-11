 
//
//	Global.h --- ��, �ṹ����, ȫ�ֺ���,��������
//

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <afxtempl.h>
#include "ThreadMutex.h"
#include "ComDev.h"
//����豸SDKͷ�ļ�
#include "dhnetsdk.h"
#include "dhplay.h"


//#include "SDK\ShowHCRemoteWnd.h"

//-------------------------------------------------------------------------------------
//        �궨��
//-------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//�궨��
#define DFJK_MAXLEN_LASTERROR		1024		//������Ϣ��������С

#define DFJK_INVALID_HANDLE			0		//��Ч������壨�ǲ���ڲ�ʹ�õģ����ݱ��������޸ģ�

#define DFJK_MAX_CHAN			32		//ÿ̨�豸���ͨ����
#define DFJK_MAX_LINK			50		//ÿ̨�豸�����������ͬһ��ͨ����������Σ�
#define DFJK_MAX_ALARMIN		16		//ÿ̨�豸��౨������
#define DFJK_MAX_ALARMOUT		16		//ÿ̨�豸��౨�����

#define DFJK_MAX_TRANS			8		//��Ƶ���ת������

#define MAXLEN_STR				50
#define MAXLEN_IP				16
#define nPort    1


//-------------------------------------------------------------------------------------
//        �ṹ����
//-------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////

//��Ƶͨ����
typedef void(CALLBACK *RealDataCallBack)(HANDLE hChanHandle, 										 
										 BYTE *pBuffer,
										DWORD dwBufSize,
										DWORD dwCookie, 
										DWORD dwParam2);
class CChannel
{
public:
	CChannel()
	{
		nChanNo			= 0;
		bIsRecording	= FALSE;
		lChanHandle		= DFJK_INVALID_HANDLE;
		lSoundHandle	= DFJK_INVALID_HANDLE;
		pfnRealDataCallBack = NULL;
		pStreamHeader = NULL;
		dwStreamheaderLen = 0;
	}
	~CChannel()
	{
		if (pStreamHeader)
		{
			delete []pStreamHeader;
			pStreamHeader = NULL;
			dwStreamheaderLen = 0;
		}
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
	RealDataCallBack  pfnRealDataCallBack;
	DWORD	dwCookie;
	BYTE*	pStreamHeader;
	DWORD	dwStreamheaderLen;
};


//�ط���
class CRecPlay
{
public:
	CRecPlay()
	{
		lPlayHandle	= DFJK_INVALID_HANDLE;

		dwTotalSize		= 0;
		dwPlayedSize	= 0;
		byPlayedPercent	= 0;

		cStr[0] = 0;
		flag = FALSE;
		
	}
	~CRecPlay()
	{
	}

public:
	long lPlayHandle;		//���ž��
	char  cStr[128];
    BOOL flag;  //0:���ļ����ţ�1����ʱ�䲥��

	
	DWORD	dwTotalSize;		//�ļ��ܴ�С
	DWORD	dwPlayedSize;		//��ǰ�Ѿ����ŵĴ�С
	BYTE	byPlayedPercent;	//���Űٷֱ�
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
	long	l485Handle;					//͸��485�ھ��
	long	lVoiceComHandle;			//�����Խ����
	CPtrArray m_arrayChan;				//ͨ���б�
//	CChannel* pChan[DFJK_MAX_CHAN];		//ͨ����ʵ��ָ��
	BYTE	bAlarmIn[DFJK_MAX_ALARMIN];	//DVS��������״̬
	BYTE	bAlarmVideoMotion[DFJK_MAX_ALARMIN];		//�ƶ���ⱨ��״̬
	BYTE    bAlarmVideoLost[DFJK_MAX_ALARMIN];
	BYTE	bAlarmVideoShelter[DFJK_MAX_ALARMIN];
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
	CRvu* GetRvuByChanHandle(long lChanHandle);		//����ĳͨ���������ȡ����Rvuʵ��
	CChannel* GetChan(HANDLE hRvu, short nChan);	//����Rvu�����ȡͨ����ʵ��

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

