
//
//	Global.cpp --- ȫ�ֺ�������, ��������
//
#include "StdAfx.h"
#include "Global.h"


//�����豸SDK lib
#pragma comment(lib, "PlayCtrl.lib")

//-------------------------------------------------------------------------------------
//        ȫ�ֱ���
//-------------------------------------------------------------------------------------

TCHAR g_szLastError[DFJK_MAXLEN_LASTERROR] = { 0 };

TCHAR g_szDLLPath[MAX_PATH] = { 0 };

//-------------------------------------------------------------------------------------
//        ȫ�ֺ���
//-------------------------------------------------------------------------------------

//����չ������ȡTagֵ
BOOL gF_GetTagValFromExParam(LPCTSTR lpszExParam, LPCTSTR lpszTagName, CString& strVal)
{
	strVal = "";
	CString strUrl = lpszExParam;
	CString strName = lpszTagName;
	if (strUrl.IsEmpty() || strName.IsEmpty())
		return FALSE;

	int nPos1 = strUrl.Find(lpszTagName, 0);
	if (nPos1 == -1)
		return FALSE;

	int nPos2 = strUrl.Find(_T(";"), nPos1);
	if (nPos2 == -1)
		return FALSE;

	CString strSub = strUrl.Mid(nPos1, nPos2-nPos1);
	if (strSub.IsEmpty())
		return FALSE;

	strVal = strSub.Mid(strName.GetLength()+1);

	return TRUE;
}
