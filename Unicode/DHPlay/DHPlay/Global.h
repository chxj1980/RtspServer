 
//
//	Global.h --- ��, �ṹ����, ȫ�ֺ���,��������
//

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <afxtempl.h>
#include "ThreadMutex.h"

//����豸SDKͷ�ļ�
#include "dhplay.h"

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


//-------------------------------------------------------------------------------------
//        �ṹ����
//-------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//�ط���
class CRecPlay
{
public:
	CRecPlay()
	{
		lPlayHandle		= DFJK_INVALID_HANDLE;
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
	//BOOL	bIsPlaying;			//�Ƿ����ڲ���
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

