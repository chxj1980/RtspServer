// rtspsrv.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "rtspsrv.h"

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <tchar.h>
#include "server.hpp"
#include "SessionManager.h"
#include "EventLogMessage.h"
#include "PlugInLoader.h"
#include "envconfig.h"
#include "xlog.h"
#include <assert.h>
#include <locale.h>
#include <winsvc.h>

#define STRING2(x) #x
#define STRING(x) STRING2(x)
#pragma message("��ǰ�������汾[" STRING(_MSC_VER) "]")

#ifdef USE_LOG4CPLUS
#ifdef _DEBUG
#pragma comment(lib,"log4cplusd")
#else
#pragma comment(lib,"log4cplus")
#endif

#endif

//����ר��
//#define  FOR_TEST_SPEC

#ifdef FOR_TEST_SPEC
#pragma message("����ר����ý�������")
#endif


SERVICE_STATUS          ServiceStatus; 
SERVICE_STATUS_HANDLE   hStatus; 
#define DISPLAY_NAME    _T("��ý��ת������")
#define SERVICE_NAME    _T("RTSPSrv")
BOOL				    g_bService = FALSE;

HRESULT Run(int argc, char* argv[]) throw();

boost::function0<void> console_ctrl_function;

BOOL WINAPI console_ctrl_handler(DWORD ctrl_type)
{
	_set_error_mode(_OUT_TO_STDERR);
	switch (ctrl_type)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		console_ctrl_function();
		return TRUE;
	default:
		return FALSE;
	}
}

/*
void __cdecl LogEvent(LPCTSTR pszFormat, ...) throw()
{
	TCHAR	chMsg[256];
	HANDLE	hEventSource;
	LPTSTR	lpszStrings[1];
	va_list	pArg;

	va_start(pArg, pszFormat);
	_vstprintf(chMsg, pszFormat, pArg);
	va_end(pArg);

	lpszStrings[0] = chMsg;

	if (!g_bService)
	{
		// Not running as a service, so print out the error message 
		// to the console if possible
		_putts(chMsg);		
		return;
	}
	
	hEventSource = RegisterEventSource(NULL, SERVICE_NAME);
	if (hEventSource != NULL)
	{		
		ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, EVENT_LOGGER_MSG, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
		DeregisterEventSource(hEventSource);
	}
}

void LogEventEx(int id, LPCTSTR pszMessage=NULL, WORD type = EVENTLOG_INFORMATION_TYPE) throw()
{
	HANDLE hEventSource;

	hEventSource = RegisterEventSource(NULL, SERVICE_NAME);
	if (hEventSource != NULL)
	{
		ReportEvent(hEventSource, 
				type,
				(WORD)0,
				id,
				NULL,
				(WORD)(pszMessage != NULL ? 1 : 0),
				0,
				pszMessage != NULL ? &pszMessage : NULL,
				NULL);
		DeregisterEventSource(hEventSource);
	}	
}
*/

void ShowUsage(bool install)
{
	printf("\n\nʹ�÷���:\n");
	if (install)	printf("\trtspsrv -install -node �ڵ��� -user �û��� -pass ����\n\n");
	else			printf("\trtspsrv -node �ڵ��� -user �û��� -pass ����\n\n");
}

int InitService() 
{ 	
	int result;
	result = 0;
	X_LOG0(XLOG_TRACE, _T("Monitoring started."));
	return(result);
}

void ControlHandler(DWORD request) 
{ 
	switch(request) 
	{ 
	case SERVICE_CONTROL_STOP: 		
		X_LOG0(XLOG_TRACE,_T("Monitoring stopped."));

		ServiceStatus.dwWin32ExitCode = 0; 
		ServiceStatus.dwCurrentState = SERVICE_STOPPED; 
		SetServiceStatus (hStatus, &ServiceStatus);
		return; 
	case SERVICE_CONTROL_SHUTDOWN: 		
		X_LOG0(XLOG_TRACE,_T("Monitoring stopped."));

		ServiceStatus.dwWin32ExitCode = 0; 
		ServiceStatus.dwCurrentState = SERVICE_STOPPED; 
		SetServiceStatus (hStatus, &ServiceStatus);
		return; 
	default:
		break;
	} 

	// Report current status
	SetServiceStatus (hStatus, &ServiceStatus);
	return; 
}

void WINAPI ServiceMain(int argc, char** argv)
{ 
	int error; 

	ServiceStatus.dwServiceType			= SERVICE_WIN32; 
	ServiceStatus.dwCurrentState		= SERVICE_START_PENDING; 
	ServiceStatus.dwControlsAccepted	= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	ServiceStatus.dwWin32ExitCode = 0; 
	ServiceStatus.dwServiceSpecificExitCode = 0; 
	ServiceStatus.dwCheckPoint = 0; 
	ServiceStatus.dwWaitHint = 0; 

	hStatus = RegisterServiceCtrlHandler(
		SERVICE_NAME, 
		(LPHANDLER_FUNCTION)ControlHandler); 
	if (hStatus == (SERVICE_STATUS_HANDLE)0) 
	{ 
		// Registering Control Handler failed
		return; 
	}  

	SetServiceStatus(hStatus,&ServiceStatus);

	// Initialize Service 
	error = InitService(); 
	if (error) 
	{
		// Initialization failed
		ServiceStatus.dwCurrentState = SERVICE_STOPPED; 
		ServiceStatus.dwWin32ExitCode = -1; 
		SetServiceStatus(hStatus, &ServiceStatus); 
		return; 
	} 

	ServiceStatus.dwWin32ExitCode = S_OK;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;	

	ServiceStatus.dwWin32ExitCode = Run(argc,argv);
	ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	
	SetServiceStatus(hStatus,&ServiceStatus);
	X_LOG0(XLOG_INFO,_T("��ý�������ֹͣ��"));

	return; 
}

#pragma warning(push)
#pragma warning(disable : 4302)	// 'type cast' : truncation from 'LPSTR' to 'TCHAR'

static int WordCmpI(LPCTSTR psz1, LPCTSTR psz2) throw()
{
	TCHAR c1 = (TCHAR)CharUpper((LPTSTR)*psz1);
	TCHAR c2 = (TCHAR)CharUpper((LPTSTR)*psz2);
	while (c1 != NULL && c1 == c2 && c1 != ' ' && c1 != '\t')
	{
		psz1 = CharNext(psz1);
		psz2 = CharNext(psz2);
		c1 = (TCHAR)CharUpper((LPTSTR)*psz1);
		c2 = (TCHAR)CharUpper((LPTSTR)*psz2);
	}
	if ((c1 == NULL || c1 == ' ' || c1 == '\t') && (c2 == NULL || c2 == ' ' || c2 == '\t'))
		return 0;

	return (c1 < c2) ? -1 : 1;
}

#pragma warning (pop)	

static LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2) throw()
{
	while (p1 != NULL && *p1 != NULL)
	{
		LPCTSTR p = p2;
		while (p != NULL && *p != NULL)
		{
			if (*p1 == *p)
				return CharNext(p1);
			p = CharNext(p);
		}
		p1 = CharNext(p1);
	}
	return NULL;
}

BOOL IsInstalled() throw()
{
	BOOL bResult = FALSE;

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM != NULL)
	{
		SC_HANDLE hService = ::OpenService(hSCM, SERVICE_NAME, SERVICE_QUERY_CONFIG);
		if (hService != NULL)
		{
			bResult = TRUE;
			::CloseServiceHandle(hService);
		}
		::CloseServiceHandle(hSCM);
	}
	return bResult;
}

BOOL Install() throw()
{
	if (IsInstalled())
	{
		X_LOG0(XLOG_ERROR,_T("ָ���ķ������Ѿ�����."));
		ShowUsage(true);
		return TRUE;
	}

	// Get the executable file path
	TCHAR szFilePath[MAX_PATH + 2];
	DWORD dwFLen = ::GetModuleFileName(NULL, szFilePath + 1, MAX_PATH);
	if( dwFLen == 0 || dwFLen == MAX_PATH )
		return FALSE;

	//�Է�������ʱ�ı�־
	// Quote the FilePath before calling CreateService
	szFilePath[0] = _T('\"');
	szFilePath[dwFLen + 1] = _T('\"');
	szFilePath[dwFLen + 2] = 0;

	//NOTE:�������������ڵ���Ϣ	
	lstrcat(szFilePath,_T("  -service"));

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		TCHAR szBuf[1024];		
		lstrcpy(szBuf,  _T("�޷���Service Manager"));
		MessageBox(NULL, szBuf, SERVICE_NAME, MB_OK);
		return FALSE;
	}

	SC_HANDLE hService = ::CreateService(
		hSCM, SERVICE_NAME, DISPLAY_NAME,
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		szFilePath, NULL, NULL, NULL, NULL, NULL);

	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		TCHAR szBuf[1024];		
		lstrcpy(szBuf,  _T("�޷���������"));
		MessageBox(NULL, szBuf, SERVICE_NAME, MB_OK);
		return FALSE;
	}
	X_LOG0(XLOG_INFO,_T("�����ѳɹ���װ!"));
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);
	return TRUE;
}

BOOL Uninstall() throw()
{
	if (!IsInstalled())
	{
		X_LOG0(XLOG_ERROR,_T("δ����ָ���ķ�����."));
		return TRUE;
	}

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM == NULL)
	{
		TCHAR szBuf[1024];
		lstrcpy(szBuf,  _T("�޷���Service Manager"));
		MessageBox(NULL, szBuf, SERVICE_NAME, MB_OK);
		return FALSE;
	}

	SC_HANDLE hService = ::OpenService(hSCM, SERVICE_NAME, SERVICE_STOP | DELETE);

	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		TCHAR szBuf[1024];		
		lstrcpy(szBuf,  _T("�޷��򿪷���"));
		MessageBox(NULL, szBuf, SERVICE_NAME, MB_OK);
		return FALSE;
	}
	SERVICE_STATUS status;
	BOOL bRet = ::ControlService(hService, SERVICE_CONTROL_STOP, &status);
	if (!bRet)
	{
		DWORD dwError = GetLastError();
		if (!((dwError == ERROR_SERVICE_NOT_ACTIVE) ||  (dwError == ERROR_SERVICE_CANNOT_ACCEPT_CTRL && status.dwCurrentState == SERVICE_STOP_PENDING)))
		{
			TCHAR szBuf[1024];			
			lstrcpy(szBuf,  _T("�޷�ֹͣ����"));
			MessageBox(NULL, szBuf, SERVICE_NAME, MB_OK);
		}
	}

	BOOL bDelete = ::DeleteService(hService);
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	if (bDelete)
	{
		X_LOG0(XLOG_INFO,_T("�����Ѿ��ɹ�ж��."));
		return TRUE;
	}

	TCHAR szBuf[1024];
	
	lstrcpy(szBuf,  _T("�޷�ɾ������"));
	MessageBox(NULL, szBuf, SERVICE_NAME, MB_OK);
	return FALSE;
}

bool  GetParam(LPCTSTR lpszToken,LPCTSTR lpszName,std::wstring& param)
{
	if (WordCmpI(lpszToken, lpszName) == 0)
	{
		LPTSTR lpStr = (LPTSTR)lpszToken + wcslen(lpszName);

		//ȥ��ǰ���ո�			
		while (('\n' == *lpStr || '\t' == *lpStr || ' ' == *lpStr) && *lpStr != '\0')
		{
			lpStr++;
		}

		//��ȡ����
		//��һ������Ϊ"-"��ֹȡ����һ������
		if ((lpStr - lpszToken)>0 && *lpStr != '-')
		{
			while ('/' != *lpStr&&
				'\n' != *lpStr&&
				'\t' != *lpStr&&
				' ' != *lpStr&&
				*lpStr != '\0')
			{
				param.append(1, *lpStr);
				lpStr++;
			}
			X_LOG2(XLOG_DEBUG, _T("��������:[%s]=[%s]"), lpszName, param.c_str());

		}
		if (param.empty())
			X_LOG1(XLOG_WARNING, _T("����:��������[%s]Ϊ�գ���"), lpszName);

		return true;
	}
	return false;
}

bool ParseCommandLine(LPCTSTR lpCmdLine, HRESULT* pnRetCode) throw()
{
	*pnRetCode = S_OK;

	TCHAR szTokens[] = _T("-/");
	bool bRet=true;

	LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);

	//���ȴ���Node����
	//while(lpszToken != NULL)
	//{		
	//	GetParam(lpszToken,_T("node"),g_strNode);
	//	GetParam(lpszToken,_T("user"),g_strUserName);
	//	GetParam(lpszToken,_T("pass"),g_strPassword);				
	//	lpszToken = FindOneOf(lpszToken, szTokens);
	//}

	lpszToken = FindOneOf(lpCmdLine, szTokens);
	while (lpszToken != NULL)
	{				
		if (WordCmpI(lpszToken, _T("uninstall"))==0)
		{
			Uninstall();
			*pnRetCode =0;			
			bRet = false;
			return false;
		}

		// Register as Local Server
		if (WordCmpI(lpszToken, _T("install"))==0)
		{			
			Install();
			bRet = false;
			*pnRetCode = 0;						
		}

		if (WordCmpI(lpszToken, _T("service"))==0)
		{
			g_bService = TRUE;
			*pnRetCode = 0;						
		}

		lpszToken = FindOneOf(lpszToken, szTokens);
	}

	return bRet;
}

HRESULT Start(int argc, char* argv[]) throw()
{
	if (g_bService)
	{
		SERVICE_TABLE_ENTRY st[2];
		st[0].lpServiceName = SERVICE_NAME;
		st[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;

		st[1].lpServiceName = NULL;
		st[1].lpServiceProc = NULL;

		if (::StartServiceCtrlDispatcher(st) == 0)
			ServiceStatus.dwWin32ExitCode = GetLastError();
		return ServiceStatus.dwWin32ExitCode;
	}
	// local server - call Run() directly, rather than
	// from ServiceMain()
	ServiceStatus.dwWin32ExitCode = Run(argc,argv);
	return ServiceStatus.dwWin32ExitCode;
}


//�ܵ����
HRESULT Run(int argc, char* argv[]) throw()
{
	_set_error_mode(_OUT_TO_STDERR);//���ô�����ʱ�Ķ��������ǵ����Ի������ֱ���˳�

	try	{
		// Initialise server.
		//��ȡ�������� NUMBER_OF_PROCESSORS ��ֵ����Ҫ�ڻ��������������ø�ֵ
		std::size_t num_threads = boost::lexical_cast<std::size_t>(getenv("NUMBER_OF_PROCESSORS"));
		num_threads = num_threads * 2 + 2;

#ifdef    FOR_TEST_SPEC
		RTSP::RTSPSrv::server s("127.0.0.1", "554", "12000", ".", num_threads);
#else 
		RTSP::RTSPSrv::server s("0.0.0.0", "554", "12000", ".", num_threads);
#endif
		X_LOG1(XLOG_INFO, _T("ϵͳ����˿�:554��rtsp����˿�:12000,�����߳���:%d"), num_threads);
		//::wprintf(_T("ϵͳ����˿�:554��rtsp����˿�:12000,�����߳���:%d \r\n"), num_threads);
		// Set console control handler to allow server to be stopped.
		console_ctrl_function = boost::bind(&RTSP::RTSPSrv::server::stop, &s);
		SetConsoleCtrlHandler(console_ctrl_handler, TRUE);

		if (g_bService)
		{
			ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			SetServiceStatus(hStatus, &ServiceStatus);
		}

		X_LOG0(XLOG_INFO, _T("��ý�����������������������Ƶ���롣"));

		// Run the server until stopped.
		s.run();
	}
	catch (boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> >& ex2)    {
		X_LOG1(XLOG_FATAL, _T("boost�ڵڶ�λ�÷����쳣��%s"), ex2.what());
		throw; //Okay, re-throwing the original exception object.    
	}
	catch (std::exception& e)
	{
		X_LOG1(XLOG_FATAL, _T("δ֪���쳣:%s.�����˳�."), e.what());
		//RTSP::RTSPSrv::RTSPSessionManagerSingleton::instance()->StopHeartBeatCheck();				
	}
	return S_OK;
}

int main(int argc, char* argv[])
{
#ifdef    FOR_TEST_SPEC
	__time64_t ltime;
	struct tm *today;
	_tzset();

	_time64(&ltime);
	today = _localtime64(&ltime);
	//std::cout<<today->tm_year+1900<<"-"<<today->tm_mon+1<<"-"<<today->tm_mday<<std::endl;
	std::cout << "******************************************************" << std::endl;
	std::cout << "*   �����е����ר����ý�����ˡ�                   *"<<std::endl;
	std::cout << "******************************************************"<<std::endl;
	int year = today->tm_year+1900;
	int mon = today->tm_mon+1;
	int day = today->tm_mday;

	if(!(year<=2011&&mon<=12)) 
	{
		X_LOG0(XLOG_ERROR,"Runtime error -1001,abnormal exit!");
		return E_FAIL;
	}
#endif

	setlocale(LC_ALL, "");

#ifdef USE_LOG4CPLUS 	
	init_log(CString(CEnvConfig::Instance()->GetHomeDir()) + _T("/log.config")); 
#endif 

	X_LOG0(XLOG_INFO,_T("������Ƶ������..."));

	LPTSTR lpCmdLine = GetCommandLine();
	HRESULT hr = S_OK;

	if (ParseCommandLine(lpCmdLine, &hr))
	{
		if (!g_bService)
		{
			X_LOG0(XLOG_INFO, _T("dfe��ý����ƽ̨...."));	//���е���ģʽ...
			Run(argc, argv);
		}
		else
		{
			X_LOG0(XLOG_INFO, _T("rtsp�������Է���ķ�ʽ����..."));
			Start(argc, argv);
		}
	}
	try{
		X_LOG0(XLOG_INFO, _T("��ʼ�ͷ����в��..."));
		CPlugInLoader::GetInstance()->FreePlugIn();
		X_LOG0(XLOG_INFO, _T("�����ͷ����в����"));
		CPlugInLoader::FreeInstance();
		CEnvConfig::FreeInstance();
	}
	catch (...)
	{
		X_LOG0(XLOG_FATAL, _T("�ͷŲ��ʱ�����쳣��"));
	}

	X_LOG0(XLOG_INFO, _T("������ֹͣ��"));
}


