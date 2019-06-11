
//
//	Global.cpp --- ȫ�ֺ�������, ��������
//
#include "StdAfx.h"
#include "Global.h"


//�����豸SDK lib

#pragma comment(lib, "dhplay.lib")
#pragma comment(lib, "dhnetsdk.lib")

//-------------------------------------------------------------------------------------
//        ȫ�ֱ���
//-------------------------------------------------------------------------------------

CRvuManager g_rvuMgr;

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

//-------------------------------------------------------------------------------------
//        CRvu
//-------------------------------------------------------------------------------------

CRvu::CRvu()
{
	lRvuHandle		= DFJK_INVALID_HANDLE;
	lAlarmHandle	= DFJK_INVALID_HANDLE;
	lSerialHandle	= DFJK_INVALID_HANDLE;
	l485Handle		= DFJK_INVALID_HANDLE;
	lVoiceComHandle	= DFJK_INVALID_HANDLE;
	dwAlarmUserData = 0;
	ZeroMemory(bAlarmIn, sizeof(bAlarmIn));
	ZeroMemory(bAlarmVideoMotion, sizeof(bAlarmVideoMotion));
}

CRvu::~CRvu()
{

}

//-------------------------------------------------------------------------------------
//        CRvuManager
//-------------------------------------------------------------------------------------

//����һ��Rvuʵ��
BOOL CRvuManager::AddRvu(CRvu* pRvu)
{
	if (!pRvu)
		return FALSE;

	CGuard<CThreadMutex> guard(m_mutex);
	m_arrayRvu.Add(pRvu);
	TRACE("====Add new RVU (%X)\n", pRvu);
	return TRUE;
}

//ɾ��һ��Rvuʵ��
BOOL CRvuManager::RemoveRvu(CRvu* pRvu)
{
	if (!pRvu)
		return FALSE;

	TRACE("====Remove the RVU (%X): ", pRvu);

	CGuard<CThreadMutex> guard(m_mutex);
	short i=0;
	for (i=0; i<m_arrayRvu.GetSize(); i++)
	{
		if (m_arrayRvu.GetAt(i) == pRvu)
		{
			m_arrayRvu.RemoveAt(i);
			delete pRvu;
			pRvu = NULL;
			TRACE("Success!\n");
			return TRUE;
		}
	}

	delete pRvu;
	pRvu = NULL;

	TRACE("Failed! (Not found)\n");
	return FALSE;
}

//ɾ��һ��Rvuʵ��������hRvu��
BOOL CRvuManager::RemoveRvuByHandle(HANDLE hRvu)
{
	return FALSE;
}

//����Rvu�����ȡRvuʵ��
CRvu* CRvuManager::GetRvuByHandle(long lRvuHandle)
{
	if (lRvuHandle == DFJK_INVALID_HANDLE)
		return NULL;

	CGuard<CThreadMutex> guard(m_mutex);
	for (short i=0; i<m_arrayRvu.GetSize(); i++)
	{
		CRvu* pRvu = (CRvu*)m_arrayRvu.GetAt(i);
		if (pRvu)
		{
			if (pRvu->lRvuHandle == lRvuHandle)
			{
				return pRvu;
			}
		}
	}
	return NULL;
}

BOOL CRvuManager::AddChan(CRvu* pRvu, CChannel* pChan)
{
	if (!pRvu || !pChan)
		return FALSE;

	CGuard<CThreadMutex> guard(m_mutex);
	pRvu->m_arrayChan.Add(pChan);
	TRACE("====Add new Chan (%X)\n", pChan);
	return TRUE;
}

BOOL CRvuManager::RemoveChan(CChannel* pChan)
{
	if (!pChan)
		return FALSE;
	TRACE("====Remove the Chan (%X): ", pChan);
	CGuard<CThreadMutex> guard(m_mutex);
	for (short i=0; i<m_arrayRvu.GetSize(); i++)
	{
		CRvu* pRvu = (CRvu*)m_arrayRvu.GetAt(i);
		if (pRvu)
		{
			for (short j=0; j<pRvu->m_arrayChan.GetSize(); j++)
			{
				if (pRvu->m_arrayChan.GetAt(j) == pChan)
				{
					delete pChan;
					pRvu->m_arrayChan.RemoveAt(j);
					TRACE("Success!\n");
					return TRUE;
				}
			}
		}
	}

	delete pChan;
	TRACE("Failed! (Not found)\n");
	return FALSE;
}

//����ĳͨ���������ȡ����Rvuʵ��
CRvu* CRvuManager::GetRvuByChanHandle(long lChanHandle)
{
	if (lChanHandle == DFJK_INVALID_HANDLE)
		return NULL;

	CGuard<CThreadMutex> guard(m_mutex);
	for (short i=0; i<m_arrayRvu.GetSize(); i++)
	{
		CRvu* pRvu = (CRvu*)m_arrayRvu.GetAt(i);
		if (pRvu)
		{
			for (short j=0; j<pRvu->m_arrayChan.GetSize(); j++)
			{
				CChannel* pChan = (CChannel*)pRvu->m_arrayChan.GetAt(j);
				if (pChan)
				{
					if (pChan->lChanHandle == lChanHandle)
					{
						return pRvu;
					}
				}
			}
		}
	}
	return NULL;
}

//����Rvu�����ȡͨ����ʵ��
CChannel* CRvuManager::GetChan(HANDLE hRvu, short nChan)
{
	return NULL;
}

//�ͷ�һ��
void CRvuManager::ClearAll()
{

	TRACE("----ClearAll: array size = %d\n", m_arrayRvu.GetSize());
	CGuard<CThreadMutex> guard(m_mutex);
	for (short i=0; i<m_arrayRvu.GetSize(); i++)
	{
		CRvu* pRvu = (CRvu*)m_arrayRvu.GetAt(i);
		if (pRvu)
		{
			for (short j=0; j<pRvu->m_arrayChan.GetSize(); j++)
			{
				CChannel* pChan = (CChannel*)pRvu->m_arrayChan.GetAt(j);
				if (pChan)
				{
					delete pChan;
				}
			}
			pRvu->m_arrayChan.RemoveAll();
			delete pRvu;
		}
	}
	m_arrayRvu.RemoveAll();
}
