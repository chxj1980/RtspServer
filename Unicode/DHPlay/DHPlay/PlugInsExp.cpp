#include "stdafx.h"
#include "PlugIns.h"
#include "Global.h"
#include "CommonFunc.h"

//////////////////////////////////////////////////////////////////////////
//ʵʱ����Ƶ���Ļص�����

static BYTE pbHead[] = { 0x34,0x48,0x4b,0x48,0xfe,0xb3,0xd0,0xd6,0x08,0x03,0x04,0x20,0x00,0x00,0x00,0x00,
0x01,0x10,0x01,0x10,0x01,0x10,0x10,0x00,0x80,0x3e,0x00,0x00,0x60,0x01,0x20,0x01,
0x11,0x10,0x00,0x00,0x0e,0x00,0x00,0x00};

long g_lPortSeed = 1;

//////////////////////////////////////////////////////////////////////////
//1��	ϵͳ�ຯ��	5
extern "C" BOOL CALL_TYPE ISYS_GetLastError(LPTSTR lpszErrorMsg, UINT nMsgLen)
{
	if((nMsgLen <0)||(nMsgLen>DFJK_MAXLEN_LASTERROR))
		return FALSE;
	else
	{
	    if((g_szLastError)&&(sizeof(g_szLastError))<= nMsgLen)
		 {
			wsprintf(lpszErrorMsg,g_szLastError);
			lpszErrorMsg[nMsgLen-1] = 0;
	        return TRUE;
		 }
	    else
		return FALSE;
	}
}

extern "C" BOOL CALL_TYPE ISYS_GetInvalidHandleValue(HANDLE *phHandle)
{
	*phHandle = (HANDLE)DFJK_INVALID_HANDLE;
	return TRUE;
}

extern "C" BOOL CALL_TYPE ISYS_GetPluginInfo (PPLUGIN_INFO pInfo)
{
	if (pInfo == NULL)
	{
		wsprintf(g_szLastError, _T("ISYS_GetPluginInfo: �����Ƿ�"));
		return FALSE;
	}
	strcpy(pInfo->DeviceTypeName, "DH");
	strcpy(pInfo->smanufactuer, "DFDH");
	strcpy(pInfo->ssoftversion, "1.0.0.1");
	strcpy(pInfo->description, "DH DVR");
	return TRUE;
}

extern "C" BOOL CALL_TYPE ISYS_Initialize(void *pReserved, DWORD dwReserved)
{
//	CLIENT_Init(NULL,(DWORD)100);
	return TRUE;
}

extern "C" void CALL_TYPE ISYS_Uninitialize()
{
//	CLIENT_Cleanup();
}

extern "C" BOOL DFJK_API CALL_TYPE ISYS_FreeMemory(void* pMem)
{
	if (pMem)
	{
		delete []pMem;
		pMem = NULL;
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

extern "C" BOOL CALL_TYPE IPLY_OpenFile(LPCSTR lpszFileName, HANDLE *phFileHandle)
{
	*phFileHandle = NULL;
	if (lpszFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_OpenFile: ��Ч����"));
		return FALSE;
	}
	long m_lport = 99;
	BOOL bRet = PLAY_OpenFile(m_lport,(char *)lpszFileName);
	if(!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_OpenFile:���ļ�ʧ��"));
		return FALSE;
	}

	DWORD m_dwMaxFileTime =PLAY_GetFileTime(m_lport);
	if (m_dwMaxFileTime == 0)
	{
	    wsprintf(g_szLastError, _T("�ļ�����Ϊ0,�ļ���Ϊ�� %s"),lpszFileName);
		return FALSE;
	}
	CRecPlay* pPlay = new CRecPlay();
	pPlay->lPlayHandle = m_lport;

	*phFileHandle = (HANDLE)pPlay;
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_CloseFile(HANDLE hFileHandle)
{
	if ( hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_CloseFile: �ļ���δ��"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	PLAY_Stop(pPlay->lPlayHandle);
	
	if(PLAY_CloseFile(pPlay->lPlayHandle))
	{
		delete pPlay;
		return TRUE;
	}
	return FALSE;
}

extern "C" BOOL CALL_TYPE IPLY_Play(HANDLE hFileHandle, HWND hWnd)
{
	if( hFileHandle == NULL )
	{
		wsprintf(g_szLastError, _T("IPLY_Play error:��ЧHANDLE"));
		return FALSE;
	}
	if (hWnd == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_Play error: Invalid Params!"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hFileHandle;

	BOOL bRet = FALSE;
//	PLAY_SetOverlayMode(pPlay->lPlayHandle,TRUE,RGB(255,0,255));   //����ģʽ���ú󱾵ز��Ż�������ʾ
    bRet = PLAY_Play(pPlay->lPlayHandle,hWnd);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_Stop(HANDLE hFileHandle)
{
	if( hFileHandle == NULL )
	{
		wsprintf(g_szLastError, _T("IPLY_Play:��Ч����"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PLAY_Stop(pPlay->lPlayHandle);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_CapturePicture(HANDLE hFileHandle,LPCSTR lpszBmpFileName)
{
	if (lpszBmpFileName == NULL || hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_CapturePicture: ��Ч����"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet=PLAY_CatchPic(pPlay->lPlayHandle,(char *)lpszBmpFileName);
	if(!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_CapturePicture: ץͼʧ��"));
		return FALSE;
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_RefreshPlay(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_RefreshPlay error: Invalid handle!"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PLAY_RefreshPlay(pPlay->lPlayHandle);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_Pause(HANDLE hFileHandle,BOOL bPause)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_Pause error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PLAY_Pause(pPlay->lPlayHandle, bPause);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_Fast(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_Fast error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PLAY_Fast(pPlay->lPlayHandle);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_Slow(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_Slow error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PLAY_Slow(pPlay->lPlayHandle);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_FastBack(HANDLE hFileHandle)
{
	wsprintf(g_szLastError, _T("IPLY_FastBack error: ��δʵ��!"));
	return FALSE;
}

extern "C" BOOL CALL_TYPE IPLY_SlowBack(HANDLE hFileHandle)
{
	wsprintf(g_szLastError, _T("IPLY_SlowBack error: ��δʵ��!"));
	return FALSE;
}

extern "C" BOOL CALL_TYPE IPLY_Step(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_Step error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PLAY_OneByOne(pPlay->lPlayHandle);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_StepBack(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_StepBack error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PLAY_OneByOneBack(pPlay->lPlayHandle);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_SetPlayPos(HANDLE hFileHandle, float fRelaPos)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_SetPlayPos error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PLAY_SetPlayPos(pPlay->lPlayHandle, fRelaPos);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_GetPlayPos(HANDLE hFileHandle, float *pfRelaPos)
{
	*pfRelaPos = 0;
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_GetPlayPos error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	float fCurPos = PLAY_GetPlayPos(pPlay->lPlayHandle);
	*pfRelaPos = fCurPos;
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_GetTotalTime(HANDLE hFileHandle, DWORD *pdwFileTime)
{
	*pdwFileTime = 0;
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_GetTotalTime error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	*pdwFileTime = PLAY_GetFileTime(pPlay->lPlayHandle);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_GetPlayedTime(HANDLE hFileHandle,DWORD *pdwPlayedTime)
{
	*pdwPlayedTime = 0;
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_GetPlayedTime error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	*pdwPlayedTime = PLAY_GetPlayedTime(pPlay->lPlayHandle);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_GetTotalFrames(HANDLE hFileHandle, DWORD *pdwTotalFrames)
{
	*pdwTotalFrames = 0;
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_GetTotalFrames error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	*pdwTotalFrames = PLAY_GetFileTotalFrames(pPlay->lPlayHandle);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_GetPlayedFrames(HANDLE hFileHandle, DWORD *pdwPlayedFrames)
{
	*pdwPlayedFrames = 0;
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_GetPlayedFrames error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	*pdwPlayedFrames = PLAY_GetPlayedFrames(pPlay->lPlayHandle);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_StartSound(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_StartSound error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PLAY_PlaySoundShare((pPlay->lPlayHandle));
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_StopSound(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_StartSound error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PLAY_StopSoundShare(pPlay->lPlayHandle);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_SetVolume(HANDLE hFileHandle, DWORD dwVolume)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_SetVolume error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	if (dwVolume < 0)
		dwVolume = 0;
	if (dwVolume > 100)
		dwVolume = 100;
	
	WORD wVol = (WORD)( dwVolume * 0xFFFF / 100 );	//SDK��Χ��0��0xFFFF, ���ӿ�Ҫ��0��100����Ҫת��

	bRet = PLAY_SetVolume(pPlay->lPlayHandle, wVol);

	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_SetVideoParams(HANDLE hFileHandle,int iBrightness,int iContrast, int iSaturation, int iHue, BOOL bDefault)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_SetVideoParams error: Invalid handle!"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PLAY_SetColor(pPlay->lPlayHandle,0,iBrightness*128/255,iContrast*128/255,iSaturation*128/255,iHue*128/255);
	return bRet;	
}

extern "C" BOOL CALL_TYPE IPLY_GetVideoParams(HANDLE hFileHandle, int *piBrightness, int *piContrast, int *piSaturation, int *piHue)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_GetVideoParams error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	int iB,iC,iS,iH;
	bRet = PLAY_GetColor(pPlay->lPlayHandle,0,&iB,&iC,&iS,&iH);
	*piBrightness = iB*255/128;
	*piContrast = iC*255/128;
	*piSaturation = iS*255/128;
	*piHue = iH*255/128;
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_OpenPlayStream(BYTE* pStreamHeader,
								   DWORD dwHeaderLen,
								   DWORD dwStreamLen,
								   DWORD dwParam1,
								   HANDLE *phStreamHandle)
{


	*phStreamHandle = NULL;
	if(g_lPortSeed>98)
	{
		g_lPortSeed = 0;
	}
	InterlockedIncrement(&g_lPortSeed);
	

	BOOL bRet=PLAY_OpenStream(g_lPortSeed,NULL,0,900*1024);
	if(!bRet)
	{
		TRACE("%d_%d_%d\n",PLAY_GetLastError((LONG)*phStreamHandle),bRet,g_lPortSeed);
	}
	
	CRecPlay *pStream = new CRecPlay();
	pStream->lPlayHandle = g_lPortSeed;
	*phStreamHandle = pStream;

	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_InputStreamData(HANDLE hStreamHandle,BYTE* pBuffer,DWORD dwStreamLen,DWORD* dwParam1,DWORD dwParam2)
{
	CRecPlay * pStream = (CRecPlay*)hStreamHandle;

    BOOL bRet=PLAY_InputData(pStream->lPlayHandle,pBuffer,dwStreamLen);

	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_StreamPlay(HANDLE hStreamHandle,HWND hWnd)
{
	CRecPlay * pStream = (CRecPlay*)hStreamHandle;
	BOOL bRet=PLAY_Play(pStream->lPlayHandle,hWnd);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_StreamPause(HANDLE hStreamHandle,BOOL bPause)
{
	CRecPlay * pStream = (CRecPlay*)hStreamHandle;
    PLAY_Pause(pStream->lPlayHandle,bPause);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_ResetStreamBuffer(HANDLE hStreamHandle)
{
	CRecPlay * pStream = (CRecPlay*)hStreamHandle;
	BOOL bRet=PLAY_ResetSourceBuffer(pStream->lPlayHandle);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_StreamRefreshPlay( HANDLE hStreamHandle)
{
	CRecPlay * pStream = (CRecPlay*)hStreamHandle;
	BOOL bRet=PLAY_RefreshPlay(pStream->lPlayHandle);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_StreamStop(HANDLE hStreamHandle)
{
	CRecPlay * pStream = (CRecPlay*)hStreamHandle;
	BOOL bRet = PLAY_Stop(pStream->lPlayHandle);
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_CloseStream(HANDLE hStreamHandle)
{	
	CRecPlay *pStream = (CRecPlay*)hStreamHandle;
	return PLAY_CloseStream(pStream->lPlayHandle);
}

//extern "C" BOOL CALL_TYPE ISYS_CustomFunction(LPCSTR pParamIn, DWORD dwParamInLen, LPTSTR *ppParamOut, DWORD* pdwParamOutLen)
//{
//	return FALSE;
//}


//���ӷŴ�
extern "C" BOOL CALL_TYPE IPLY_SetDisplayRegion(HANDLE hPlayHandle,DWORD nRegionNum, RECT *pSrcRect, HWND hDestWnd, BOOL bEnable)
{
// 	if (hPlayHandle == NULL)
// 	{
// 		wsprintf(g_szLastError, _T("IPLY_SetDisplayRegion error: Invalid handle!");
// 		return FALSE;
// 	}
// 
// 	CRecPlay * pPlay = (CRecPlay*)hPlayHandle;
	BOOL bRet = FALSE;
// 	if (pPlay)
// 	{
// 		if (pSrcRect)
// 		{
// 			long lW,lH;
// 			PlayM4_GetPictureSize(pPlay->lPlayHandle,&lW,&lH);	//������pPlay->lWidth��lHeight����׼ȷ
// 					//PlayM4_GetPictureSize���벥��֮����ò�׼ȷ��
// 
// 			CRect rc = pSrcRect;
// 			RECT rcLast;
// 			rc.NormalizeRect();
// 			rcLast.left = rc.left*lW/ 255;
// 			rcLast.right = rc.right*lW / 255;
// 			rcLast.top = rc.top*lH/ 255;
// 			rcLast.bottom = rc.bottom*lH / 255;
// 
// 			bRet = PlayM4_SetDisplayRegion(pPlay->lPlayHandle, 0, &rcLast, hDestWnd, bEnable);
// 		}
// 		else
// 			bRet = PlayM4_SetDisplayRegion(pPlay->lPlayHandle, 0, pSrcRect, hDestWnd, bEnable);
// 	}
// 	if (!bRet)
// 	{
// 		wsprintf(g_szLastError, _T("IPLY_SetDisplayRegion failed, err = %d", PlayM4_GetLastError(pPlay->lPlayHandle));
// 	}

	return bRet;
}

//ˢ����ʾ���ڣ���IPLY_SetDisplayRegionˢ�µĴ��ڣ�
extern "C" BOOL CALL_TYPE IPLY_RefreshPlayEx(HANDLE hPlayHandle, DWORD nRegionNum)
{
// 	if (hPlayHandle == NULL)
// 	{
// 		wsprintf(g_szLastError, _T("IPLY_RefreshPlayEx error: Invalid handle!");
// 		return FALSE;
// 	}
// 
// 	CRecPlay * pPlay = (CRecPlay*)hPlayHandle;
	BOOL bRet = FALSE;
// 	if (pPlay)
// 	{
// 		bRet = PlayM4_RefreshPlayEx(pPlay->lPlayHandle, nRegionNum);
// 	}
// 	if (!bRet)
// 	{
// 		wsprintf(g_szLastError, _T("IPLY_RefreshPlayEx failed, err = %d", PlayM4_GetLastError(pPlay->lPlayHandle));
// 	}

	return bRet;
}

//��ͼ�ص�����
void CALLBACK DrawFun(long nPort,HDC hDc,LONG nUser)
{
// 	if(nUser != 0)
// 	{
// 		CRecPlay * pPlay = (CRecPlay*)nUser;
// 		if(pPlay->pfnDrawCallBack != NULL)
// 		{
// 			pPlay->pfnDrawCallBack( (HANDLE) pPlay, hDc, pPlay->pUserData);
// 		}
// 	}
}

//ע���ͼ�ص�����
extern "C" BOOL CALL_TYPE IPLY_RegisterDrawFun(HANDLE hPlayHandle, DrawCallBack callbackDraw, void* pUserData)
{
// 	if (hPlayHandle == NULL)
// 	{
// 		wsprintf(g_szLastError, _T("IPLY_RegisterDrawFun error: Invalid handle!");
// 		return FALSE;
// 	}
// 
// 	CRecPlay * pPlay = (CRecPlay*)hPlayHandle;
	BOOL bRet = FALSE;
// 	if (pPlay)
// 	{
// 		pPlay->pUserData = pUserData;
// 		pPlay->pfnDrawCallBack = callbackDraw;
// 		bRet = PlayM4_RegisterDrawFun(pPlay->lPlayHandle, DrawFun, (LONG)pPlay);
// 	}
// 	if (!bRet)
// 	{
// 		wsprintf(g_szLastError, _T("IPLY_RegisterDrawFun failed, err = %d", PlayM4_GetLastError(pPlay->lPlayHandle));
// 	}

	return bRet;
}
