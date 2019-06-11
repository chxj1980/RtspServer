#ifndef RTSPCH_
#define RTSPCH_

#define RTSP_SEVER_PORT 554		// Well-known RTSP server port
#define MAX_STR			65536	// Max string lenget
#define MAX_BUF			1024	// Size of receive buffer
#define MAX_LINE		1024	// No single line in the response should be more than
#define MAX_RESP_SIZE	3072
#define MAX_MEDIA		6		// This many media may be present (all-media, audio, video, text, ...)

// State definitions

#define IDLE		0
#define PLAYING		1
#define PAUSED		2

#define DESCRIBE	1
#define SETUP		2
#define PLAY		3
#define PAUSE		4
#define TEARDOWN	5
#define ANNOUNCE    6
#define OPTIONS     7
#define GET         8

/*********************
WIN32 compatibility */

#ifdef UNIX
#define MB_OK 0
#define SOCKET_ERROR -1
#define SOCKET int
#define stricmp strcasecmp
#define strnicmp strncasecmp
#define ZeroMemory bzero
#define TRUE 1
#define FALSE 0
#endif

enum ErrorCode
{
	ErrNoIniSock = 100,         //Could not initialize sockets; sock�������1��ͷ
	ErrNoSockVersion = 101,     //Could not find WinSock1.1
	ErrNoOpenSocket = 102,      //Could not open the socket!
	ErrNoSendTimeO = 103,       //Could not set SendTimeOut!
	ErrNoRecvTimeO = 104,       //Could not set RecvTimeOut!
	ErrNoConnectServer = 105,   //Could not connect to the server!

	ErrReqDescribe = 300,       //  Failed sending DESCRIBE request!;req�������3��ͷ
	ErrReqSetup = 301,          //  Failed sending Setup request!
	ErrReqPlay = 302,           //  Failed sending Play request!
	ErrReqPause = 303,          // Failed sending Pause request!
	ErrReqTearDown = 304,       // Failed sending TearDown request!

	ErrRecvDescribe = 400,      // Failed receive DESCRIBE response!;recv�������4��ͷ
	ErrRecvSetup = 401,         // Failed receive setup response!;
	ErrRecvPlay = 402,          // Failed receive play response!;
	ErrRecvPause = 403,         // Failed receive play response!;
	ErrRecvTearDown = 404,      // Failed receive pause response!;

	ErrUrlInvalid = 500,       //input invalid Url��Url�������5��ͷ
	ErrUrlBadProtool = 501,     //Bad protool,no RTSP
	ErrUrlNoFile = 502,         // no file specified,Url���ޡ�/��

	ErrNoLocalhost = 601,       //no localhost	
	ErrNoOpneStream = 701,   //HikOpenStream Faild
	ErrHikGetPortError = 702
};

struct CodeStringError
{
	ErrorCode code;
	char ErrorString[200];
};


struct SESSION_MEDIA {
	char setupInfo[MAX_STR];     // :video, :audio etc.
	int uniPort;
	int multiPort;
	int multiTTL;
	char srcAddr[MAX_STR];
	char type[MAX_STR];          // audio, video, application ...
};

struct packetheader
{
	int head;
	int body;
	int tail;
};

#define MAX_BUFFER_SIZE  (2048*2048)
#define MAX_RTP_BUFFER_SIZE  65535

#include "winsock2.h"
#include "ThreadMutex.h"
#include "PlugInLoader.h"
#include "Utilities.h"
#include "Prototypes.h"
#include "clntsink.h"

class MyRTPSession;

class CRTSPclient {
public:
	CRTSPclient();
	~CRTSPclient();

	/////////11.10////////////////////
	BOOL TCPConnect();
	void CloseTCPSocket();
	int ReadTCPData(DWORD DClient);

	/////////11.10////////////////////

	SOCKET		data_socket;					//TCP����ʱ��SOCKET
	int			tcpPort;						//TCP����ʱ�������ݵĶ˿�
	pDataRec	m_pRecFunc;
	int			m_iUserData;

	char		host[MAX_STR];					// DNS host name
	int			serverport;
	char		file[MAX_STR];					// SDP file name, session description
	HWND		hWnd;
	SOCKET		serv_socket;
	char		strURL[MAX_STR];
	char		m_fileHeader[MAX_STR];			//�ļ�ͷsetup�ظ����յ���
	DWORD		m_headerLength;

	char		m_szRemoteFileName[MAX_STR];	//�طŵ��ļ�����
	char		m_szDvrType[10];				//�豸����

	__int64     lfreq;							//��ȡ��ǰʱ��
	__int64     lstart;
	__int64     lstop;
	__int64     lruntime;

	char*		m_PBRecv;//�ط�Զ���ļ�ʱ���յ���RTSP�����е�ֵ����ǰ�����ļ��Ľ���ֵ��ʱ�䳤�ȡ�֡���ȣ�
	DWORD		m_PBRecvLength;
	TCHAR		LastError[1000];                // ��������Ĵ�����Ϣ

	CPlugInLoader	*m_pPlugloader;
	CPluginInfo*	m_pPluginfo;

	int		m_hPort;
	int		Conntype;								//ת����ʽ0:tcp 1:UDP

	unsigned char Rtp_Buffer[MAX_RTP_BUFFER_SIZE];	//rtp��ȡ���ݻ�����
	unsigned char TCP_Buffer[MAX_BUFFER_SIZE];

	int Rtp_Buffer_plength;//Rtp_Buffer�ĵ�ǰ����ָ��


	int Connect();
	int Stop();
	int Resume();
	int Disconnect();
	const char* GetTitle();

	int GetState()
	{
		CGuard<CThreadMutex> lock(*m_mutex);
		return m_nState;
	}

	void SetState(int state)
	{
		CGuard<CThreadMutex> lock(*m_mutex);
		m_nState = state;
	}

	MyRTPSession *GetRTPSession()
	{
		return m_pRTPSession;
	}

	int ReadRTPData(DWORD DClient, MyRTPSession* pRTPSession);
	int InitRTPSession(int nPortBase, MyRTPSession** ppRTPSession);
	int UninitRTPSession();

	int sendAnnounce();
	int sendOptions(unsigned int ucommand,
		int iparam1,
		int iparam2,
		int iparam3,
		int iparam4,
		char* lpszParm5);   //��̨���� +��������ŵ�

	int sendGets(const char* strURL,
		int RecType,
		const char* strTime);//���͵õ��ļ��б�


	//���ò�� 

	BOOL LoadPlugin(LPCTSTR lpszType, LPCTSTR lpszSubType);
	BOOL intialize(void *pvReserved, DWORD reserved);
	BOOL FreePlugin();
	BOOL GetFunction();

	IPLY_StreamPlay			pfnIPLY_StreamPlay;
	IPLY_OpenPlayStream		pfnIPLY_OpenPlayStream;
	IPLY_StreamStop			pfnIPLY_StreamStop;
	IPLY_CloseStream		pfnIPLY_CloseStream;
	IPLY_InputStreamData	pfnIPLY_InputStreamData;        //����RefreshPLAY  readrealheader 
	ISYS_GetLastError		pfnISYS_GetLastError;
	IPLY_StreamRefreshPlay	pfnIPLY_StreamRefreshPlay;
	IPLY_CapturePicture		pfnIPLY_CapturePicture;
	IPLY_SetVideoParams		pfnIPLY_SetVideoParams;
	IPLY_GetVideoParams		pfnIPLY_GetVideoParams;

	IPLY_StartSound			pfnIPLY_StartSound;
	IPLY_StopSound			pfnIPLY_StopSound;
	IPLY_SetVolume			pfnIPLY_SetVolume;

	//10.21
	IPLY_Pause				pfnIPLY_Pause;
	IPLY_Fast				pfnIPLY_Fast;
	IPLY_Slow				pfnIPLY_Slow;
	IPLY_Play				pfnIPLY_Play;

	//------------------------------------------------------------------------------------------------------------------------------------	
	//2012-07-04 lyq
	//���ӷŴ�begin
	IPLY_SetDisplayRegion	pfnIPLY_SetDisplayRegion;
	IPLY_RefreshPlayEx		pfnIPLY_RefreshPlayEx;
	IPLY_RegisterDrawFun	pfnIPLY_RegisterDrawFun;
	//���ӷŴ�end
	//2012-07-04 lyq
	//------------------------------------------------------------------------------------------------------------------------------------	

	void CloseSocket();
	void CloseThread();
	int RecvResponse(int type, int mediaCtrl);
	BOOL CreatPlayer(LPCTSTR lpszType, LPCTSTR lpszSubType);
	BOOL StreamPlay();
	int ParseInput(const char * serverURL);

	int CloseStream();
	int ServConnect();

	int recvTCPData(const int datalength, char* pBuffer);

private:

	int m_nState;                        // ״̬����

	char session[MAX_STR];               // Unique RTSP session identifier
	char sessName[MAX_STR];              // Session name extracted from SDP

	SESSION_MEDIA media[MAX_MEDIA];      // We support up to MAX_MEDIA descriptions

	int		cSeq;	                         // Increased by 1, always
	int		m_StreamNum;                     // ... and we actually have this many     

	MyRTPSession *m_pRTPSession;

	HANDLE	m_hThread;
	DWORD	dwThreadId;

	HANDLE	m_sleepThread;
	DWORD	dwSleepThreadId;

	CThreadMutex *m_mutex;

	int TCPSocketBind();               //TCP��ʽ��������ʱ 11.10

	int GetSomePort();

	int sendDescribe();
	int sendSetup(int mediaCtrl);
	int sendPlay();
	int sendTeardown();
	int addHeader(char * reqString);
	int sendv(char* send_p, int sendplen);
	void addSession(char * reqString);

	int receive(char* respstr);
};

const char* GetErrorMessage(ErrorCode code);
#endif
