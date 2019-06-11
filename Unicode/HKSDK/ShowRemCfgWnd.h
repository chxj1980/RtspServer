#ifndef SHOWREMOTE_HC
#define SHOWREMOTE_HC

#ifdef SHOW_HCREMOTE_EXPORT
#define NET_SDK_API extern "C"__declspec(dllexport)
#else
#define NET_SDK_API  extern "C"__declspec(dllimport)
#endif

//������ڵ���
#define MAX_MAINNODENUM			15
//����ӽ����
#define MAX_SUBNODENUM			20

//Զ�����ÿ������
#define STYLE_9000				1
#define STYLE_8000				2

//�����ͻ�����Ϣ
#define UM_MSG_REBOOTSOFTWEAR	WM_USER+1100


//��������˵��
#define HC_LANGUAGE_CHI		0
#define HC_LANGUAGE_ENG		1
#define HC_LANGUAGE_TRAD    2
#define HC_LANGUAGE_RUS		3
#define HC_LANGUAGE_PLS		4
#define HC_LANGUAGE_FRCH    5
#define HC_LANGUAGE_SPA		6
#define HC_LANGUAGE_CORA	7
#define HC_LANGUAGE_GMN		8
#define HC_LANGUAGE_ITAL	9

#define HC_LANGUAGE_CZECH	13
#define HC_LANGUAGE_SVK		14
#define HC_LANGUAGE_DUTCH	15


typedef struct _RemConfigPara
{
	LONG lServerID;				//�û�ID
	char sUserName[32];			//�û�����
	int iLanguageType;			//��������
	BOOL bShowIPMode;			//��ʾIPģ������
	HWND hParentWnd;			//Զ�����õĸ�����
	COLORREF crBKColor;			//Զ�����ñ���ɫ(����)
	COLORREF crFontColor;		//Զ������������ɫ(����)
	BYTE byStyle;				//Զ�����ÿ������(����)
	BYTE byDecoder;				//�Ƿ�Ϊ��������0-���ǣ�1-���½�������2-���Ͻ�����	
	BYTE byRes2[32];		
}NET_DVR_REMCONFIGPARA, *LPNET_DVR_REMCONFIGPARA;


//����˳��˵����
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* 
����˳��

	NET_DVR_SetRemConfigNode ->	NET_DVR_ShowRemConfigWnd


	�ڵ���NET_DVR_ShowRemConfigWnd֮ǰ��Ҫ�ȵ���NET_DVR_SetRemConfigNode
	��ȷ����Ҫ���õ�Զ�����õ���������

	eg:����һ����IPģ��ͨ�����õĲ�����������������Զ������

	//��ʾIPͨ�����ý���
	NET_DVR_REMCONFIGPARA stRemConfigPara;
	stRemConfigPara.lServerID   = m_lUserID;
	strcpy(stRemConfigPara.sUserName, m_sUserName);
	stRemConfigPara.iLanguageType = HC_LANGUAGE_CHI;
	stRemConfigPara.bShowIPMode = TRUE;

	//���þ���ȫ�����ݵ�Զ������

	NET_DVR_SetRemConfigNode(0xff, 0xff, TRUE);

	//����Զ������
	NET_DVR_ShowRemConfigWnd(&stRemConfigPara);

                                                                     */
/************************************************************************/


//�ӿ�˵��
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* 
  Function:    		NET_DVR_SetRemConfigNode(int iMainNode, int iSubNode, BOOL bEnable = TRUE)
  
  Description:		����Զ�����õ�������

  Input:			int iMainNode	---		�����������ڵ�����ֵ
					int iSubNode	---		���������ӽڵ�����ֵ
					BOOL bEnable	---		���ñ�ʶ��	

  OutPut:			NULL

  Return:			BOOL		---		����
                                                                     */
/************************************************************************/
NET_SDK_API BOOL __stdcall NET_DVR_SetRemConfigNode(int iMainNode, int iSubNode, BOOL bEnable = TRUE);


/************************************************************************/
/* 
����˵��:

	iMainNode�� 0 ~  11 �ֱ����Զ�����ø������õĸ��ڵ�,0xff����ȫ�����ڵ�
	
	�����豸������ͨ��������������������ڲ����������������쳣�������û�����
		�������á�Զ��������Ӳ�̹���6467ATM����(���豸����ΪDS81XX_AH_S����Ч)
		���������(���Ӿ������ý������������ʱ��Ч)

	iSubNode��1 ~ 19 �ֱ������������ڵ��µ������ýڵ�,0xff����ȫ���ֽڵ�

		��Ҫ����ӽڵ����ã����ڵ����ñ���������״̬
		���磺Ҫ��������õ��ƶ���⹦�ܣ��ƶ���⹦����ͨ�����õ�һ�������ù���
		��ͨ�������������ã�����NET_DVR_SetRemConfog(1, 0, TRUE);
	
	iMainNode = 0,iSubNode = 0
	iSubNode��1 ~ 2	��Ӧ���豸�������豸��Ϣ���汾��Ϣ��

	iMainNode = 1,
	iSubNode��1 ~ 8	��Ӧ��ͨ����������ʾ���á���Ƶ���á�
									¼��ƻ����ƶ���⡢��Ƶ��ʧ��
									�ڵ���������Ƶ�ڸǡ��ַ�����

	iMainNode = 2,
	iSubNode��1 ~ 6	��Ӧ������������������á�PPPOE����
									DDNS���á�NTP���á�NFS���á�
									Email����
	iMainNode = 3,
	iSubNode��1 ~ 2 ��Ӧ�ڴ��ڲ�����RS232���á�RS485����

	iMainNode = 4,
	iSubNode��1 ~ 2 ��Ӧ�ڱ��������ı������롢�������


	ʹ�÷���˵��:
	eg1:ֻ��¼��ƻ���Զ������
	NET_DVR_SetRemConfigNode(1, 0, TRUE);	//����ͨ������
	NET_DVR_SetRemConfigNode(1, 3, TRUE);	//����¼��ƻ�

	eg2:ֻ��ȫ������������õ�Զ������
	NET_DVR_SetRemConfigNode(2, 0xff, TRUE);

	eg3:ֻȱ��ȫ������������õ�Զ������
	NET_DVR_SetRemConfigNode(0xff, 0xff, TRUE);//ȫ������
	NET_DVR_SetRemConfigNode(2, 0xff, FALSE);	//ȥ����������

                                                                     */
/************************************************************************/




/************************************************************************/
/* 
  Function:    		NET_DVR_ShowRemConfigWnd(LPNET_DVR_REMCONFIGPARA pstRemConfigPara);
  
  Description:		����Զ������

  Input:			LPNET_DVR_REMCONFIGPARA pstRemConfigPara	----	Զ�����ò����ṹ��ָ��

  OutPut:			NULL

  Return:			BOOL
                                                                     */
/************************************************************************/
NET_SDK_API BOOL __stdcall NET_DVR_ShowRemConfigWnd(LPNET_DVR_REMCONFIGPARA pstRemConfigPara);
#endif 

