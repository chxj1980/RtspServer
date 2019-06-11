 
//
//	Global.h --- ��, �ṹ����, ȫ�ֺ���,��������
//

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <afxtempl.h>
#include "ThreadMutex.h"
#include "CommonFunc.h"

//����豸SDKͷ�ļ�
#include "PlayM4.h"

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

#define CIF_WIDTH				352
#define	CIF_HEIGHT_PAL			288

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

//////////////////////////////////////////////////////////////////////////
//�ط��ࣨ���豸���������ͬ��
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

//-------------------------------------------------------------------------------------
//        ȫ�ֱ�������
//-------------------------------------------------------------------------------------

extern TCHAR g_szLastError[DFJK_MAXLEN_LASTERROR];	//Last Error

extern TCHAR g_szDLLPath[MAX_PATH];					//DLL ����·��

//-------------------------------------------------------------------------------------
//        ȫ�ֺ�������
//-------------------------------------------------------------------------------------

//����չ������ȡTagֵ
BOOL gF_GetTagValFromExParam(LPCTSTR lpszExParam, LPCTSTR lpszTagName, CString& strVal);

#endif

