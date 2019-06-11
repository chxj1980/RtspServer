#include "stdafx.h"
#include "Client.h"
#include "RtspClient.h"
#include <stdio.h>

extern long  g_lLastPort;
extern long g_TCPlLastPort;

TCHAR g_szLastError[1200];
HINSTANCE g_pIntance = NULL;

#define  CHECK_FUNCTION(obj,func)   \
	{						\
	if(obj->pfn##func==NULL)	{	\
		wsprintf(g_szLastError,_T("����û��##func##����!"));\
		GSetLogError(g_szLastError);\
		return 	-1;	\
		}\
} \

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	g_pIntance = (HINSTANCE)hModule;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void rtrim(std::string& s, char c)
{
	if (s.empty())	return;

	std::string::iterator p;
	for (p = s.end(); p != s.begin() && *--p == c;);

	if (*p != c)
		p++;

	s.erase(p, s.end());
}

CLIENT_API  int  CALL_TYPE InitStreamClientLib(void)  //��ʼ��
{
	memset(g_szLastError, 0, 1200);

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)	return -1;

	srand((UINT)time(NULL));
	GLogError(_T("��ʼ����ý�����ɹ���"));

	int randNum = rand();
	if (randNum % 2 == 1)
		randNum = randNum++;

	g_lLastPort = randNum % 1000;
	g_TCPlLastPort = rand() % 1000;
	return 0;
}

extern "C"  int CLIENT_API CALL_TYPE FiniStreamClientLib(void)  //�ر�
{
	WSACleanup();
	CLastError::FreeInstance();
	GLogError(_T("��ʼ�ͷŲ��..."));
	CPlugInLoader::FreeInstance();
	GLogError(_T("�ͷŲ���ɹ���"));
	return 0;
}

//10.26---
extern "C" BOOL CLIENT_API CALL_TYPE HIKS_ISYS_FreeMemory(void* pMem)
{
	if (pMem)
	{
		delete[]pMem;
		pMem = NULL;
		return TRUE;
	}
	return FALSE;
}
//10.26----

extern "C" HSESSION CLIENT_API CALL_TYPE HIKS_CreatePlayer(IHikClientAdviseSink* pSink,
	void* pWndSiteHandle,
	pDataRec pRecFunc,
	LPCTSTR lpszType,
	pMsgBack pMsgFunc,
	int TransMethod)//����Player
{
	if (TransMethod < 0 || TransMethod > 2)
	{
		GSetLogError(_T("zhuanfaleixingcuowu"));
		return -1;
	}
	TCHAR strTemp[300];
	wsprintf(strTemp, _T(" ---HIKS_CreatePlayer return LpszType=%s,pMasgFunc = %d,TransMethod=%d"),
							lpszType, (DWORD)pRecFunc, TransMethod);
	GLogError(strTemp);

	CRTSPclient* RtspClient = new CRTSPclient();
	if (RtspClient == NULL)
	{
		GSetLogError(_T("HIKS_CreatePlayer can't get memory"));
		return -1;
	}

	DWORD hSession = (DWORD)RtspClient;

	RtspClient->hWnd = (HWND)pWndSiteHandle;
	RtspClient->m_pRecFunc = pRecFunc;
	RtspClient->Conntype = TransMethod;   //ת����ʽ

	if (RtspClient->CreatPlayer(lpszType, _T("PLAY")))     //���ò��
	{
		USES_CONVERSION;
		strcpy(RtspClient->m_szDvrType, T2A(lpszType));

		TCHAR strTemp[300];
		wsprintf(strTemp, _T(" ---HIKS_CreatePlayer return HSession = %d, \
						 		LpszType=%s,pMasgFunc = %d,TransMethod=%d"),
								hSession, lpszType, (DWORD)pRecFunc, TransMethod);

		GLogError(strTemp);
		return hSession;
	}
	else
	{
		GSetLogError(lpszType);
	}
	delete RtspClient;
	RtspClient = NULL;
	return -1;
}

extern "C"  int CLIENT_API CALL_TYPE HIKS_OpenURL(HSESSION hSession,
	const char* pszURL,
	int iusrdata,
	const char* lpszRemoteFileName)//��URL  //suc 1,fail -1
{
	//	rtsp://192.0.1.179/192.0.1.173:8000:HIK-DS8000HC:0:0:admin:12345/av_stream  //��ʱ�ȿ���һ����ý�������ת�������
	/*
	TCHAR strTemp[500];
	wsprintf(strTemp, _T("HIKS_OpenURL hSession = %d,pszURL = %s,iusrdata = %d"), hSession, CString(pszURL), iusrdata);

	GLogError(strTemp);
	AfxMessageBox(strTemp);
	if (hSession == -1)
	{
		wcscpy(g_szLastError, _T("HIKS_OpenURL hSession ��Ч"));
		GSetLogError(g_szLastError);
		return -1;
	}*/

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
	if (RtspClient)
	{
		if (lpszRemoteFileName != NULL&&strlen(lpszRemoteFileName)>1)
		{
			strcpy(RtspClient->m_szRemoteFileName, lpszRemoteFileName);

			std::string strTemp(RtspClient->m_szRemoteFileName);
			rtrim(strTemp, '\n');
			rtrim(strTemp, '\r');
			strcpy(RtspClient->m_szRemoteFileName, strTemp.c_str());
		}

		strcpy(RtspClient->strURL, pszURL);
		RtspClient->m_iUserData = iusrdata;
		if ((RtspClient->ParseInput(pszURL)) == 0)
		{
			if (RtspClient->ServConnect() != 0)
			{
				wsprintf(g_szLastError, _T("��ý�����������ʧ�ܣ�%s"), GGetError());
				GSetError(g_szLastError);
				return -1;
			}
			if (RtspClient->Connect() == 0)
			{
				GLogError(_T("Open URL success-----"));
				return 1;
			}
			if (lpszRemoteFileName != NULL&&strlen(lpszRemoteFileName)>1)
			{
				wsprintf(g_szLastError, _T("������ϻ��ļ�û���ҵ���%s"), GGetError());
				GSetError(g_szLastError);
				return -1;
			}

			wsprintf(g_szLastError, _T("��Ƶ����ʧ�ܣ�%s"), GGetError());
			GSetError(g_szLastError);
			return -1;
		}
		else
		{
			wsprintf(g_szLastError, _T("URL�ַ�����ʽ����"));
			GSetLogError(g_szLastError);
		}
	}

	return -1;
}

extern "C" int CLIENT_API CALL_TYPE HIKS_Play(HSESSION hSession)//����
{
	TCHAR strTemp[40];
	wsprintf(strTemp, _T("HIKS_Play(%d)"), hSession);
	GLogError(strTemp);

	if (hSession == -1)
	{
		wcscpy(g_szLastError, _T("HIKS_Play hSession ��Ч"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	if (RtspClient)
	{
		if (RtspClient->StreamPlay())
		{
			GLogError(_T("play success----"));
			return 1;
		}
	}
	return -1;
}

extern "C" int  CLIENT_API CALL_TYPE HIKS_Stop(HSESSION hSession)//ֹͣ,����Player
{
	TCHAR strTemp[40];
	wsprintf(strTemp, _T("HIKS_Stop(%d)"), hSession);
	GLogError(strTemp);

	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("HIKS_Stop hSession ��Ч"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
	if (RtspClient != NULL)
	{
		if (RtspClient->Disconnect() == 0)
		{
			delete RtspClient;
			RtspClient = NULL;
			return 0;
		}
	}

	return -1;
}

LPCTSTR  CALL_TYPE HIKS_GetLastError()
{
	wsprintf(g_szLastError, GGetError());
	return g_szLastError;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_Destroy(HSESSION hSession)//����Player���� 4.13
{
//	if (hSession == -1)
// 	{
//		sprintf(g_szLastError," HIKS_Destroy hSession ��Ч" ); 
//		return -1;
//	}
// 
//	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
//
//	if (RtspClient)
//	{
//		if (RtspClient ->CloseStream() == 0)
// 		   return 0;
// 	}
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_GetRealDataHeader(HSESSION hSession,
	char *pBuffer,
	DWORD dwBuffLen,
	DWORD *pdwHeaderLen)//�õ��ļ�ͷ
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_GetFileHeader FAILED!"));
		GLogError(g_szLastError);
		GSetError(_T("���ú�������ֵ����"));
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
	if (RtspClient)
	{
		if (dwBuffLen < RtspClient->m_headerLength)
		{
			wsprintf(g_szLastError, _T(" HIKS_GetRealDataHeader failed,����������̫С��"));
			GLogError(g_szLastError);
			GSetError(_T("���뻺��������̫С��"));
			return -1;
		}

		*pdwHeaderLen = RtspClient->m_headerLength;
		if (RtspClient->m_headerLength >0)
		{
			memcpy(pBuffer, RtspClient->m_fileHeader, RtspClient->m_headerLength);
			return 1;
		}
	}

	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_GrabPic(HSESSION hSession,
	const char* szPicFileName,
	unsigned short byPicType)
	//success 1,fail -1;  unsigned short byPicTypeû��
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_GrabPic FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
	CHECK_FUNCTION(RtspClient, IPLY_CapturePicture);

	if (RtspClient->pfnIPLY_CapturePicture((HANDLE)RtspClient->m_hPort, szPicFileName))
	{
		wsprintf(g_szLastError, _T("HIKS_GrabPic(%d,%s)success"), hSession, szPicFileName);
		GLogError(g_szLastError);
		return 1;
	}

	TCHAR bufString[300];
	RtspClient->pfnISYS_GetLastError(bufString, 300);

	wsprintf(g_szLastError, _T("HIKS_GrabPic(%d,%s)failed,error infor= %s"), hSession, szPicFileName, bufString);
	GLogError(g_szLastError);
	GSetError(_T("�ײ�������"));
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_GetVideoParams(HSESSION hSession,
	int *piBrightness,
	int *piContrast,
	int *piSaturation,
	int *piHue)
{

	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_GrabPic FAILED!"));
		GSetError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
	if (strnicmp(RtspClient->m_szDvrType, "DFDH", 4) == 0)
	{
		GSetError(_T("�󻪵Ĳ����֧�� GetVideoParams!"));
		return -1;
	}

	CHECK_FUNCTION(RtspClient, IPLY_GetVideoParams);
	if (RtspClient->pfnIPLY_GetVideoParams((HANDLE)RtspClient->m_hPort, piBrightness, piContrast, piSaturation, piHue))
		return 1;

	TCHAR bufString[300];
	RtspClient->pfnISYS_GetLastError(bufString, 300);

	wsprintf(g_szLastError, _T("HIKS_GetVideoParams failed,error infor= %s"), bufString);
	GSetError(g_szLastError);
	GLogError(_T("�ײ�������"));
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_SetVideoParams(HSESSION hSession,
	int iBrightness,
	int iContrast,
	int iSaturation,
	int iHue,
	BOOL bDefault)
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_GrabPic FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	if (strnicmp(RtspClient->m_szDvrType, "DFDH", 4) == 0)
	{
		GSetError(_T("�󻪵Ĳ����֧��SetVideoParams !"));
		return -1;
	}

	CHECK_FUNCTION(RtspClient, IPLY_SetVideoParams);
	if (RtspClient->pfnIPLY_SetVideoParams((HANDLE)RtspClient->m_hPort, iBrightness, iContrast, iSaturation, iHue, bDefault))
	{
		wsprintf(g_szLastError, _T("HIKS_SetVideoParams successes"));
		GLogError(g_szLastError);
		return 1;
	}

	TCHAR bufString[300];
	RtspClient->pfnISYS_GetLastError(bufString, 300);

	wsprintf(g_szLastError, _T("HIKS_SetVideoParams failed,error infor= %s"), bufString);
	GSetError(g_szLastError);
	GLogError(g_szLastError);
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE HIKS_PTZControl(HSESSION hSession,            //�ɹ�Ϊ1��ʧ��Ϊ0-1��
	unsigned int ucommand,
	int iparam1,
	int iparam2,
	int iparam3,
	int iparam4,
	LPCSTR lpszParam5)             //����ӵ�Э���ַ���lpszParam5
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_GrabPic FAILED!"));
		GSetError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	if (RtspClient)
	{
		RtspClient->sendOptions(ucommand, iparam1, iparam2, iparam3, iparam4, (char*)lpszParam5);
		if (RtspClient->RecvResponse(OPTIONS, 0) == 0)
			return 1;

		return -1;
	}
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_RandomPlay(HSESSION hSession, unsigned long timepos)
{
	GSetLogError(_T("HIKS_RandomPlay ��δʵ��"));
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_Pause(HSESSION hSession)
{
	GSetLogError(_T("HIKS_Pause ��δʵ��"));
	return -1;
}

extern "C"int CLIENT_API CALL_TYPE  HIKS_Resume(HSESSION hSession)//�ָ�
{
	GSetLogError(_T("HIKS_Resume ��δʵ��"));
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_GetCurTime(HSESSION hSession, unsigned long *utime)//��ȡ��ǰ�Ĳ���ʱ�䣬1/64��Ϊ��λ, Ŀǰ���ض���0
{
	GSetLogError(_T("HIKS_GetCurTime ��δʵ��"));
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_ChangeRate(HSESSION hSession, int scale)//�ı䲥������,�ڿ����������ʱ����
{
	GSetLogError(_T("HIKS_ChangeRate ��δʵ��"));
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_SetVolume(HSESSION hSession, unsigned short volume)//�������� ����volume�ķ�Χ��0-65535
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1  HIKS_SetVolume FAILED!"));
		GSetError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	CHECK_FUNCTION(RtspClient, IPLY_SetVolume);
	BOOL bret = RtspClient->pfnIPLY_SetVolume((HANDLE)RtspClient->m_hPort, volume);
	if (!bret)
	{
		TCHAR bufString[300];
		RtspClient->pfnISYS_GetLastError(bufString, 300);

		wsprintf(g_szLastError, _T("IPLY_SetVolume failed,error infor= %s"), bufString);
		GSetError(g_szLastError);
		GLogError(g_szLastError);
		return -1;
	}

	return 0;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_OpenSound(HSESSION hSession, bool bExclusive)   //10.15
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_OpenSound FAILED!"));
		GSetError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
	CHECK_FUNCTION(RtspClient, IPLY_StartSound);

	BOOL bret = RtspClient->pfnIPLY_StartSound((HANDLE)RtspClient->m_hPort);
	if (!bret)
	{
		TCHAR bufString[300];
		RtspClient->pfnISYS_GetLastError(bufString, 300);

		wsprintf(g_szLastError, _T("IPLY_StartSound failed,error infor= %s"), bufString);
		GSetError(g_szLastError);
		GLogError(g_szLastError);
		return -1;
	}

//	GLogError(_T("OpenSound Success"));
	return  1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_CloseSound(HSESSION hSession)
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_CloseSound FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	CHECK_FUNCTION(RtspClient, IPLY_StopSound);
	BOOL bret = (RtspClient->pfnIPLY_StopSound((HANDLE)RtspClient->m_hPort));
	if (bret)	return 0;

	TCHAR bufString[300];
	RtspClient->pfnISYS_GetLastError(bufString, 300);

	wsprintf(g_szLastError, _T("IPLY_StopSound failed,error infor= %s"), bufString);
	GSetError(g_szLastError);
	GLogError(g_szLastError);
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_ThrowBFrameNum(HSESSION hSession, unsigned int nNum)//��B��
{
	GSetLogError(_T("HIKS_ThrowBFrameNum ��δʵ��"));
	return  -1;
}

extern "C" int CLIENT_API CALL_TYPE HIKS_PlayBackControl(HSESSION hSession,
	DWORD dwControlCode,
	DWORD dwInValue,
	DWORD *lpOutValue)//Զ�̻طſ��ƺ���
{
	//	GLogError(_T("Enter HIKS_PlayBackControl---");
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_PlayBackControl FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	CHECK_FUNCTION(RtspClient, IPLY_Fast);
	CHECK_FUNCTION(RtspClient, IPLY_Slow);
	CHECK_FUNCTION(RtspClient, IPLY_Play);

	if (dwControlCode == NET_DVR_PLAYFAST)
	{
		if (RtspClient->hWnd != NULL)
		{
			BOOL bret = RtspClient->pfnIPLY_Fast((HANDLE)RtspClient->m_hPort);
			if (!bret)
			{
				TCHAR bufString[300];
				RtspClient->pfnISYS_GetLastError(bufString, 300);

				wsprintf(g_szLastError, _T("IPLY_Fast FAILED!,error infor= %s"), bufString);
				GSetLogError(g_szLastError);
				//	return -1;
			}
		}

		if (dwControlCode == NET_DVR_PLAYSLOW)
		{
			BOOL bret = RtspClient->pfnIPLY_Slow((HANDLE)RtspClient->m_hPort);
			if (!bret)
			{
				TCHAR bufString[300];
				RtspClient->pfnISYS_GetLastError(bufString, 300);

				wsprintf(g_szLastError, _T("IPLY_Slow failed,error infor= %s"), bufString);
				GSetLogError(g_szLastError);
				//	return -1;
			}
		}

		if (dwControlCode == NET_DVR_PLAYNORMAL)
		{
			BOOL bret = RtspClient->pfnIPLY_Play((HANDLE)RtspClient->m_hPort, RtspClient->hWnd);
			if (!bret)
			{
				TCHAR bufString[300];
				RtspClient->pfnISYS_GetLastError(bufString, 300);

				wsprintf(g_szLastError, _T("IPLY_Play failed,error infor= %s"), bufString);
				//	GSetLogError(g_szLastError);
				return -1;
			}
		}
	}

	RtspClient->sendOptions(dwControlCode, dwInValue, 0, 0, 0, "");
	if (RtspClient->RecvResponse(OPTIONS, 0) == 0)
	{
		if (lpOutValue != NULL)
		{
			if (RtspClient->m_PBRecvLength != 0)
			{
				*lpOutValue = atoi(RtspClient->m_PBRecv);
				delete[] RtspClient->m_PBRecv;
				RtspClient->m_PBRecv = NULL;
				wsprintf(g_szLastError, _T("server return play back = %d"), RtspClient->m_PBRecv);
				GLogError(g_szLastError);
			}
			else	return -1;
		}
		return 1;
	}
	return -1;
}

extern "C"  int CLIENT_API CALL_TYPE HIKS_QueryDevRecord(const char* strURL,
	int RecType,
	char* pTime,
	// 														 DWORD dwbufferLen,
	// 														 char* ppExParamOut,
	LPSTR* ppExParamOut,
	DWORD* pdwExParamOutLen
	)//��ȡԶ���ļ��б�
{
	CRTSPclient* RtspClient = new CRTSPclient();

	if (RtspClient == NULL)
	{
		GSetLogError(_T("HIKS_QueryDevRecord can't get memory"));
		return -1;
	}

	strcpy(RtspClient->strURL, strURL);
	if ((RtspClient->ParseInput(strURL)) == 0)
	{
		if (RtspClient->ServConnect() == 0)
		{
			GLogError(_T("ServConnect success"));
		}
		else
		{
			wsprintf(g_szLastError, _T("��ý�����������ʧ�ܣ�%s"), GGetError());
			GSetError(g_szLastError);
			return -1;
		}
	}
	else
	{
		wsprintf(g_szLastError, _T("URL�ַ�����ʽ����"));
		GSetLogError(g_szLastError);
	}

	RtspClient->sendGets(strURL, RecType, pTime);
	int iRet = RtspClient->RecvResponse(GET, 0);

	RtspClient->CloseSocket();

	if (iRet == 0)
	{
		*pdwExParamOutLen = RtspClient->m_PBRecvLength;

		if (RtspClient->m_PBRecvLength != 0)
		{
			*ppExParamOut = new char[RtspClient->m_PBRecvLength + 1];
			if (*ppExParamOut == NULL)
			{
				delete[] RtspClient->m_PBRecv;
				RtspClient->m_PBRecv = NULL;
				delete RtspClient;
				RtspClient = NULL;

				GSetLogError(_T("IDVR_QueryDevRecord failed. get filelistmemory failed"));
				return -1;
			}

			strcpy(*ppExParamOut, RtspClient->m_PBRecv);

			delete[] RtspClient->m_PBRecv;
			RtspClient->m_PBRecv = NULL;
		}

		if (RtspClient->m_PBRecvLength == 0)
		{
			GSetError(_T("���ʱ��û���ļ���"));
			return -1;
		}

		delete RtspClient;
		RtspClient = NULL;
		return 1;
	}
	else
	{
		wsprintf(g_szLastError, _T("û����Ҫ���ҵ��ļ���%s"), GGetError());
		GSetError(g_szLastError);

		if (RtspClient->m_PBRecv != NULL && RtspClient->m_PBRecvLength != 0)
		{
			delete[] RtspClient->m_PBRecv;
			RtspClient->m_PBRecv = NULL;
		}

		delete RtspClient;
		RtspClient = NULL;
		return -1;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------	
//2012-07-04 lyq														 
//���ӷŴ�begin
extern "C" BOOL CLIENT_API  CALL_TYPE HIKS_SetDisplayRegion(HSESSION hSession,
	DWORD nRegionNum,
	RECT *pSrcRect,
	HWND hDestWnd,
	BOOL bEnable)
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_SetDisplayRegion FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	if (RtspClient->pfnIPLY_SetDisplayRegion == NULL)
		return -1;

	if (RtspClient->pfnIPLY_SetDisplayRegion((HANDLE)RtspClient->m_hPort,
		nRegionNum,
		pSrcRect,
		hDestWnd,
		bEnable))
	{
		wsprintf(g_szLastError, _T("HIKS_SetDisplayRegion(%d,%d,%d)success"), hSession, nRegionNum, bEnable);
		GLogError(g_szLastError);
		return TRUE;
	}
	else
	{
		TCHAR bufString[300];
		RtspClient->pfnISYS_GetLastError(bufString, 300);

		wsprintf(g_szLastError, _T("HIKS_SetDisplayRegion(%d,%d,%d)failed,error info= %s"), hSession, nRegionNum, bEnable, bufString);
		GLogError(g_szLastError);
		GSetError(_T("�ײ�������"));
		return FALSE;
	}
}

extern "C" BOOL CLIENT_API  CALL_TYPE HIKS_RefreshPlayEx(HSESSION hSession,
	DWORD nRegionNum)
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_RefreshPlayEx FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	if (RtspClient->pfnIPLY_RefreshPlayEx == NULL)
		return -1;

	if (RtspClient->pfnIPLY_RefreshPlayEx((HANDLE)RtspClient->m_hPort, nRegionNum))
	{
		wsprintf(g_szLastError, _T("HIKS_RefreshPlayEx(%d,%d)success"), hSession, nRegionNum);
		GLogError(g_szLastError);
		return TRUE;
	}
	else
	{
		TCHAR bufString[300];
		RtspClient->pfnISYS_GetLastError(bufString, 300);

		wsprintf(g_szLastError, _T("HIKS_RefreshPlayEx(%d,%d)failed,error info= %s"), hSession, nRegionNum, bufString);
		GLogError(g_szLastError);
		GSetError(_T("�ײ�������"));
		return FALSE;
	}
}

//ע���ͼ�ص�����
extern "C" BOOL CLIENT_API CALL_TYPE HIKS_RegisterDrawFun(HSESSION hSession,
	DrawCallBack callbackDraw,
	void* pUserData)
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_RegisterDrawFun FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	if (RtspClient->pfnIPLY_RegisterDrawFun == NULL)	return -1;

	if (RtspClient->pfnIPLY_RegisterDrawFun((HANDLE)(RtspClient->m_hPort), callbackDraw, pUserData))
	{
		wsprintf(g_szLastError, _T("HIKS_RegisterDrawFun(%d)success"), hSession);
		GLogError(g_szLastError);
		return TRUE;
	}
	else
	{
		TCHAR bufString[300];
		RtspClient->pfnISYS_GetLastError(bufString, 300);

		wsprintf(g_szLastError, _T("HIKS_RegisterDrawFun(%d)failed,error info= %s"), hSession, bufString);
		GLogError(g_szLastError);
		GSetError(_T("�ײ�������"));
		return FALSE;
	}
}

//���ӷŴ�end
//2012-07-04 lyq
//------------------------------------------------------------------------------------------------------------------------------------	

