#ifndef __CLIENT_H__
#define __CLIENT_H__

#ifdef CLIENT_EXPORTS
#define CLIENT_API __declspec(dllexport)
#else
#define CLIENT_API __declspec(dllimport)
#endif

#define CALL_TYPE   WINAPI //�����ΪPASCAL 
#include "clntsink.h"
#include "Prototypes.h"


#define NET_DVR_PLAYSTART			101//��ʼ����
#define NET_DVR_GETPLAYLIST			102//��ȡ�ļ��б�
#define NET_DVR_PLAYPAUSE			103//��ͣ����
#define NET_DVR_PLAYRESTART			104//�ָ�����
#define NET_DVR_PLAYFAST			105//���
#define NET_DVR_PLAYSLOW			106//����
#define NET_DVR_PLAYNORMAL			107//�����ٶ�
#define NET_DVR_PLAYFRAME			108//��֡��

#define NET_DVR_PLAYSETPOS			112//�ı��ļ��طŵĽ���
#define NET_DVR_PLAYGETPOS			113//��ȡ�ļ��طŵĽ���
#define NET_DVR_PLAYGETTIME			114//��ȡ��ǰ�Ѿ����ŵ�ʱ��
#define NET_DVR_PLAYGETFRAME		115//��ȡ��ǰ�Ѿ����ŵ�֡��
#define NET_DVR_GETTOTALFRAMES  	116//��ȡ��ǰ�����ļ��ܵ�֡��
#define NET_DVR_GETTOTALTIME    	117//��ȡ��ǰ�����ļ��ܵ�ʱ��
#define NET_DVR_THROWBFRAME			120//��B֡


#ifdef __cplusplus
extern "C" {
#endif


typedef DWORD HSESSION;

/*
  192.0.0.145����ý���������IP��ַ
  rtsp://192.0.0.145/stream.264      //VOD ���·��
  rtsp://192.0.0.145/d:/��ý��/hikstream/files/stream.264      //VOD ����·��
  rtsp://192.0.0.12/192.0.1.200:8000:DS-8000HC:0:0:admin:12345/av_stream //ʵʱ��
*/

 
int CLIENT_API CALL_TYPE  InitStreamClientLib(void);  //��ʼ��ACE
int CLIENT_API CALL_TYPE  FiniStreamClientLib(void);  //�ر�ACE

HSESSION CLIENT_API CALL_TYPE  HIKS_CreatePlayer(IHikClientAdviseSink* pSink, 
												 void* pWndSiteHandle, 
												 pDataRec pRecFunc, 
												 LPCTSTR lpszType,
												 pMsgBack pMsgFunc=0,
												 int TransMethod=0
												 );//����Player

int CLIENT_API CALL_TYPE  HIKS_OpenURL(HSESSION hSession,
									   const char* pszURL,
									   int iusrdata,
									   const char* oPenfilename);//��URL

int CLIENT_API CALL_TYPE  HIKS_Play(HSESSION hSession);//����

int CLIENT_API CALL_TYPE  HIKS_RandomPlay(HSESSION hSession,unsigned long timepos);//���λ�ò���,������Ƶ��ǰ���϶�

int CLIENT_API CALL_TYPE  HIKS_Pause(HSESSION hSession); //��ͣ

int CLIENT_API CALL_TYPE  HIKS_Resume(HSESSION hSession);//�ָ�

int CLIENT_API CALL_TYPE  HIKS_Stop(HSESSION hSession);//ֹͣ,����Player

int CLIENT_API CALL_TYPE  HIKS_GetCurTime(HSESSION hSession,unsigned long *utime);//��ȡ��ǰ�Ĳ���ʱ�䣬1/64��Ϊ��λ, Ŀǰ���ض���0

int CLIENT_API CALL_TYPE  HIKS_ChangeRate(HSESSION hSession,int scale);//�ı䲥������,�ڿ����������ʱ����

int CLIENT_API CALL_TYPE  HIKS_Destroy(HSESSION hSession);//����Player

int CLIENT_API CALL_TYPE  HIKS_GetVideoParams(HSESSION hSession, 
											  int *piBrightness, 
											  int *piContrast, 
											  int *piSaturation, 
											  int *piHue);

int CLIENT_API CALL_TYPE  HIKS_SetVideoParams(HSESSION hSession,  
											  int iBrightness,
											  int iContrast, 
											  int iSaturation, 
											  int iHue,
											  BOOL bDefault);

int CLIENT_API CALL_TYPE  HIKS_PTZControl(HSESSION hSession, 
										  unsigned int ucommand, 
										  int iparam1,
										  int iparam2, 
										  int iparam3, 
										  int iparam4,
										  LPCSTR lpszParam5);

int CLIENT_API CALL_TYPE  HIKS_SetVolume(HSESSION hSession,
										 unsigned short volume);//�������� ����volume�ķ�Χ��0-65535

int CLIENT_API CALL_TYPE  HIKS_OpenSound(HSESSION hSession, 
										 bool bExclusive=false);

int CLIENT_API CALL_TYPE  HIKS_CloseSound(HSESSION hSession);

int CLIENT_API CALL_TYPE  HIKS_ThrowBFrameNum(HSESSION hSession,
											  unsigned int nNum);//��B��

int CLIENT_API CALL_TYPE  HIKS_GrabPic(HSESSION hSession,
									   const char* szPicFileName, 
									   unsigned short byPicType);

CLIENT_API  LPCTSTR CALL_TYPE HIKS_GetLastError();

int CLIENT_API CALL_TYPE  HIKS_GetRealDataHeader(HSESSION hSession,											
												char *pBuffer,
												DWORD dwBuffLen,
												DWORD *pdwHeaderLen);//�õ��ļ�ͷ

int CLIENT_API CALL_TYPE HIKS_PlayBackControl(HSESSION hSession,
											  DWORD dwControlCode,
											  DWORD dwInValue,
											  DWORD *lpOutValue);//Զ�̻طſ��ƺ���
//dwControlCode ���ն�����
//�� dwControlCode=104ʱ��dwInValueΪ���ھ����

int CLIENT_API CALL_TYPE HIKS_QueryDevRecord(const char* strURL,
											 int RecType,
											 char* pTime,
// 											 DWORD dwbufferLen,
// 											 char* ppExParamOut,
                                             LPSTR* ppExParamOut,
											 DWORD* pdwExParamOutLen   
											 );//��ȡԶ���ļ��б�

BOOL CLIENT_API CALL_TYPE HIKS_ISYS_FreeMemory(void* pMem);

//------------------------------------------------------------------------------------------------------------------------------------	
//2012-07-04 lyq	
//���ӷŴ�begin
BOOL CLIENT_API  CALL_TYPE HIKS_SetDisplayRegion(HSESSION hSession,
												 DWORD nRegionNum, 
												 RECT *pSrcRect, 
												 HWND hDestWnd, 
												 BOOL bEnable);

BOOL CLIENT_API  CALL_TYPE HIKS_RefreshPlayEx(HSESSION hSession,
											  DWORD nRegionNum);


BOOL CLIENT_API CALL_TYPE HIKS_RegisterDrawFun(HSESSION hSession, 
											   DrawCallBack callbackDraw,
											   void* pUserData);
//���ӷŴ�end
//2012-07-04 lyq
//------------------------------------------------------------------------------------------------------------------------------------	
	
#ifdef __cplusplus
}
#endif

#endif



