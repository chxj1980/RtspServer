
// TestRtspClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestRtspClientApp:
// �йش����ʵ�֣������ TestRtspClient.cpp
//

class CTestRtspClientApp : public CWinApp
{
public:
	CTestRtspClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestRtspClientApp theApp;