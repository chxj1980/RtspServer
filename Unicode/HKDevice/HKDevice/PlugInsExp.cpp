#include "stdafx.h"
#include "PlugIns.h"
#include "Global.h"
#include "CommonFunc.h"
#include <string>
#include<iostream>

using namespace std;

string CString2string(CString str)
{
	int size = WideCharToMultiByte(CP_ACP, 0, str.GetBuffer(), -1, NULL, 0, NULL, NULL);
	char	*ch = new char[size + 1];

	WideCharToMultiByte(CP_ACP, 0, str.GetBuffer(), -1, ch, size, NULL, NULL);

	string Str = ch;
	delete[]	ch;
	return Str;
}


//���������ע�ͺ궨��󣬼��ɱ�Ϊ���������
//#define PROTECT_DLL

//////////////////////////////////////////////////////////////////////////
//������Ϣ�ص�������
//////////////////////////////////////////////////////////////////////////
CBF_AlarmInfo g_CBF_AlarmInfo = NULL;

CBF_SerialDataCallBack g_CBF_Serial232 = NULL;

CBF_SerialDataCallBack g_CBF_Serial485 = NULL;

/*
static BYTE g_pbHead[] = { 0x34,0x48,0x4b,0x48,0xfe,0xb3,0xd0,0xd6,0x08,0x03,0x04,0x20,0x00,0x00,0x00,0x00,
0x01,0x10,0x01,0x10,0x01,0x10,0x10,0x00,0x80,0x3e,0x00,0x00,0x60,0x01,0x20,0x01,
0x11,0x10,0x00,0x00,0x0e,0x00,0x00,0x00};
*/
// static BYTE g_pbHead[40];
// BYTE pbHead[] = { 0x34,0x48,0x4b,0x48,0xfe,0xb3,0xd0,0xd6,0x08,0x03,0x04,0x20,0x00,0x00,0x00,0x00,
// 0x03,0x10,0x01,0x10,0x01,0x10,0x10,0x00,0x80,0x3e,0x00,0x00,0xc0,0x02,0x40,0x02,
// 0x11,0x10,0x00,0x00,0x05,0x00,0x00,0x00};
// 
// BYTE pbHead_V30[] = { 0x49,0x4D,0x4B,0x48,0x01,0x01,0x00,0x00,0x02,0x00,0x01,0x00,0x21,0x72,0x01,0x10,
// 0x00,0x7D,0x00,0x00,0x00,0x7D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//@�ص�����
BOOL CALLBACK DVSMessageCallback(LONG lCommand,LONG lUserID,char *pBuf,DWORD dwBufLen)
{
	int i=0;
	int nStartChan = 1;	//serverinfo[i].m_lStartChannel;
	DWORD dwVal = 1;
	switch(lCommand) {
	case COMM_ALARM:
		NET_DVR_ALARMINFO alminfo;
		memcpy(&alminfo, pBuf, sizeof(NET_DVR_ALARMINFO));
		switch(alminfo.dwAlarmType)
		{
		case 0:		//�ź�������
			if (g_CBF_AlarmInfo)
			{
				CRvu* pRvu = g_rvuMgr.GetRvuByHandle(lUserID);
				if (pRvu)
				{
					g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_IO, alminfo.dwAlarmInputNumber+1, &dwVal);
				}
			}
			break;
		case 1:		//Ӳ��������
		case 4:		//Ӳ��δ��ʽ������
		case 5:		//Ӳ�̳�����
			if (g_CBF_AlarmInfo)
			{
				CRvu* pRvu = g_rvuMgr.GetRvuByHandle(lUserID);
				if (pRvu)
				{
					for(i=0; i<MAX_DISKNUM; i++)
					{
						if(alminfo.dwDiskNumber[i] == 1)
						{
							if (g_CBF_AlarmInfo)
								g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, (ALARMTYPE)alminfo.dwAlarmType, i+1, &dwVal);
						}
					}
				}
			}
			break;
		case 2:		//�źŶ�ʧ����
		case 3:		//�ƶ���ⱨ��
		case 6:		//�ڵ�����
		case 7:		//��ʽ��ƥ��
			if (g_CBF_AlarmInfo)
			{
				CRvu* pRvu = g_rvuMgr.GetRvuByHandle(lUserID);
				if (pRvu)
				{
					for(i=0; i<MAX_CHANNUM; i++)
					{
						if (alminfo.dwChannel[i] == 1)
						{
							if (g_CBF_AlarmInfo)
								g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, (ALARMTYPE)alminfo.dwAlarmType, i+nStartChan, &dwVal);
						}
					}
				}
			}

			break;
		case 8:			//�Ƿ�����
			if (g_CBF_AlarmInfo)
			{
				CRvu* pRvu = g_rvuMgr.GetRvuByHandle(lUserID);
				if (pRvu)
				{
					g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_ILLEGALCONN, 0, &dwVal);
				}
			}
			break;
		default:
			//strTemp.Format("δ֪���ͱ���������: %d��\n",alminfo.dwAlarmType);
			//TRACE(strTemp);
			break;
		}
		break;
		default:
			break;
	}
	return 0;
}

//��Ϊ������������
void ProcRuleAlarm(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, DWORD dwBufLen, void* pUser)
{
	if (g_CBF_AlarmInfo == NULL)
		return;

	CRvu* pRvu = g_rvuMgr.GetRvuByHandle(pAlarmer->lUserID);
	if (pRvu == NULL)
	{
		TRACE("ProcRuleAlarm: pRvu not found! id=%d\n", pAlarmer->lUserID);
		return;
	}

	NET_VCA_RULE_ALARM  struVcaRuleAlarm;
	memcpy(&struVcaRuleAlarm, pAlarmInfo, sizeof(NET_VCA_RULE_ALARM));

// 	TRACE("��Ϊ����������DevIP:[%s]TargetID[%d]RuleID[%d]RuleName[%s]RelativeTime[%d]AbsTime[%d] piclen[%d]\n",
// 		struVcaRuleAlarm.struDevInfo.struDevIP.sIpV4,struVcaRuleAlarm.struTargetInfo.dwID,
//         struVcaRuleAlarm.struRuleInfo.byRuleID,struVcaRuleAlarm.struRuleInfo.byRuleName,struVcaRuleAlarm.dwRelativeTime,
// 		struVcaRuleAlarm.dwAbsTime, struVcaRuleAlarm.dwPicDataLen);

	ALM_PARAM alm_para;
	ZeroMemory(&alm_para, sizeof(ALM_PARAM));
	alm_para.dwVal = 1;
	strncpy(alm_para.szAlmDevIP, struVcaRuleAlarm.struDevInfo.struDevIP.sIpV4, MAXLEN_IP);
	alm_para.nAlmDevChan = struVcaRuleAlarm.struDevInfo.byChannel;
	alm_para.nRuleID = struVcaRuleAlarm.struRuleInfo.byRuleID;
	strncpy(alm_para.szRuleName, (const char*)struVcaRuleAlarm.struRuleInfo.byRuleName, 32);
	alm_para.dwPicLen = struVcaRuleAlarm.dwPicDataLen;
	alm_para.pPicData = struVcaRuleAlarm.pImage;
	//sprintf(alm_para.szDes, _T("���ܷ�������: ..."));

	switch (struVcaRuleAlarm.struRuleInfo.dwEventType)
	{
	case VCA_TRAVERSE_PLANE://��Խ������
		{
// 			sprintf(alm_para.szDes, _T("��Խ�����汨��������: [%.2f,%.2f,%.2f,%.2f], ����:%d��",
// 				struVcaRuleAlarm.struRuleInfo.uEventParam.struTraversePlane.struPlaneBottom.struStart.fX,
// 				struVcaRuleAlarm.struRuleInfo.uEventParam.struTraversePlane.struPlaneBottom.struStart.fY,
// 				struVcaRuleAlarm.struRuleInfo.uEventParam.struTraversePlane.struPlaneBottom.struEnd.fX,
// 				struVcaRuleAlarm.struRuleInfo.uEventParam.struTraversePlane.struPlaneBottom.struEnd.fY,
// 				struVcaRuleAlarm.struRuleInfo.uEventParam.struTraversePlane.dwCrossDirection
// 				);
//			sprintf(alm_para.szDes, _T("��Խ�����汨��,����:%d"),
//				struVcaRuleAlarm.struRuleInfo.uEventParam.struTraversePlane.dwCrossDirection
			sprintf(alm_para.szDes, "��Խ�����汨��");
			g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_VCA_TRAVERSE_PLANE, struVcaRuleAlarm.struDevInfo.byIvmsChannel , &alm_para);
		}
		break;
	case VCA_ENTER_AREA://Ŀ���������,֧���������
		{
			sprintf(alm_para.szDes, "�������򱨾�");
			g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_VCA_ENTER_AREA, struVcaRuleAlarm.struDevInfo.byIvmsChannel , &alm_para);
		}
		break;
	case VCA_EXIT_AREA://Ŀ���뿪����,֧���������
		{
			sprintf(alm_para.szDes, "�뿪���򱨾�");
			g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_VCA_EXIT_AREA, struVcaRuleAlarm.struDevInfo.byIvmsChannel , &alm_para);
		}
		break;
	case VCA_INTRUSION://�ܽ�����,֧���������
		{
			sprintf(alm_para.szDes, "�ܽ����ֱ���");
			g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_VCA_INTRUSION, struVcaRuleAlarm.struDevInfo.byIvmsChannel , &alm_para);
		}
		break;
	case VCA_LOITER://�ǻ�,֧���������
		{
			sprintf(alm_para.szDes, "�ǻ�����");
			g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_VCA_LOITER, struVcaRuleAlarm.struDevInfo.byIvmsChannel , &alm_para);
		}
		break;
	case VCA_LEFT_TAKE://�������,֧���������
		{
			sprintf(alm_para.szDes, "�����������");
			g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_VCA_LEFT_TAKE, struVcaRuleAlarm.struDevInfo.byIvmsChannel , &alm_para);
		}
		break;
	case VCA_PARKING://ͣ��,֧���������
		{
			sprintf(alm_para.szDes, "�Ƿ�ͣ������");
			g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_VCA_PARKING, struVcaRuleAlarm.struDevInfo.byIvmsChannel , &alm_para);
		}
		break;
	case VCA_RUN://����,֧���������
		{
			sprintf(alm_para.szDes, "���ܱ���");
			g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_VCA_RUN, struVcaRuleAlarm.struDevInfo.byIvmsChannel , &alm_para);
		}
		break;
	case VCA_HIGH_DENSITY://��������Ա�ܶ�,֧���������
		{
			sprintf(alm_para.szDes, "��Ա�ܼ�����");
			g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_VCA_HIGH_DENSITY, struVcaRuleAlarm.struDevInfo.byIvmsChannel , &alm_para);
		}
		break;
	default:
		{
			TCHAR szInfoBuff[512] = {0};
			wsprintf(szInfoBuff, _T("DFHK Plugin: δ�������Ϊ�������������ͣ�%d\n"),struVcaRuleAlarm.struRuleInfo.dwEventType); 
			OutputDebugString(szInfoBuff);
		}
		break;
	}
}



//BOOL CALLBACK DVSMessageCallback(LONG lCommand,LONG lUserID,char *pBuf,DWORD dwBufLen)
void CALLBACK MessageCallback(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, DWORD dwBufLen, void* pUser)
{
	int i=0;
	int nStartChan = 1;	//serverinfo[i].m_lStartChannel;
	DWORD dwVal = 1;
		
	switch(lCommand) {
	case COMM_ALARM:
		NET_DVR_ALARMINFO alminfo;
		memcpy(&alminfo, pAlarmInfo, sizeof(NET_DVR_ALARMINFO));
		switch(alminfo.dwAlarmType)
		{
		case 0:		//�ź�������
			if (g_CBF_AlarmInfo)
			{
				CRvu* pRvu = g_rvuMgr.GetRvuByHandle(pAlarmer->lUserID);
				if (pRvu)
				{
					g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_IO, alminfo.dwAlarmInputNumber+1, &dwVal);
				}
			}
			break;
		case 1:		//Ӳ��������
		case 4:		//Ӳ��δ��ʽ������
		case 5:		//Ӳ�̳�����
			if (g_CBF_AlarmInfo)
			{
				CRvu* pRvu = g_rvuMgr.GetRvuByHandle(pAlarmer->lUserID);
				if (pRvu)
				{
					for(i=0; i<MAX_DISKNUM; i++)
					{
						if(alminfo.dwDiskNumber[i] == 1)
						{
							if (g_CBF_AlarmInfo)
								g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, (ALARMTYPE)alminfo.dwAlarmType, i+1, &dwVal);
						}
					}
				}
			}
			break;
		case 2:		//�źŶ�ʧ����
		case 3:		//�ƶ���ⱨ��
		case 6:		//�ڵ�����
		case 7:		//��ʽ��ƥ��
			if (g_CBF_AlarmInfo)
			{
				CRvu* pRvu = g_rvuMgr.GetRvuByHandle(pAlarmer->lUserID);
				if (pRvu)
				{
					for(i=0; i<MAX_CHANNUM; i++)
					{
						if (alminfo.dwChannel[i] == 1)
						{
							if (g_CBF_AlarmInfo)
								g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, (ALARMTYPE)alminfo.dwAlarmType, i+nStartChan, &dwVal);
						}
					}
				}
			}
			
			break;
		case 8:			//�Ƿ�����
			if (g_CBF_AlarmInfo)
			{
				CRvu* pRvu = g_rvuMgr.GetRvuByHandle(pAlarmer->lUserID);
				if (pRvu)
				{
					g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_ILLEGALCONN, 0, &dwVal);
				}
			}
			break;
		default:
			//strTemp.Format("δ֪���ͱ���������: %d��\n",alminfo.dwAlarmType);
			//TRACE(strTemp);
			break;
		}
		break;
	case COMM_ALARM_V30:
		NET_DVR_ALARMINFO_V30 alminfo_V30;
		memcpy(&alminfo_V30, pAlarmInfo, sizeof(NET_DVR_ALARMINFO_V30));
		switch(alminfo_V30.dwAlarmType)
		{
		case 0:		//�ź�������
			if (g_CBF_AlarmInfo)
			{
				CRvu* pRvu = g_rvuMgr.GetRvuByHandle(pAlarmer->lUserID);
				if (pRvu)
				{
					g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_IO, alminfo_V30.dwAlarmInputNumber+1, &dwVal);
				}
			}
			break;
		case 1:		//Ӳ��������
		case 4:		//Ӳ��δ��ʽ������
		case 5:		//Ӳ�̳�����
			if (g_CBF_AlarmInfo)
			{
				CRvu* pRvu = g_rvuMgr.GetRvuByHandle(pAlarmer->lUserID);
				if (pRvu)
				{
					for(i=0; i<MAX_DISKNUM_V30; i++)
					{
						if(alminfo_V30.byDiskNumber[i] == 1)
						{
							if (g_CBF_AlarmInfo)
								g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, (ALARMTYPE)alminfo_V30.dwAlarmType, i+1, &dwVal);
						}
					}
				}
			}
			break;
		case 2:		//�źŶ�ʧ����
		case 3:		//�ƶ���ⱨ��
		case 6:		//�ڵ�����
		case 7:		//��ʽ��ƥ��
			if (g_CBF_AlarmInfo)
			{
				CRvu* pRvu = g_rvuMgr.GetRvuByHandle(pAlarmer->lUserID);
				if (pRvu)
				{
					for(i=0; i<MAX_CHANNUM_V30; i++)
					{
						if (alminfo_V30.byChannel[i] == 1)
						{
							if (g_CBF_AlarmInfo)
								g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, (ALARMTYPE)alminfo_V30.dwAlarmType, i+nStartChan, &dwVal);
						}
					}
				}
			}
			
			break;
		case 8:			//�Ƿ�����
			if (g_CBF_AlarmInfo)
			{
				CRvu* pRvu = g_rvuMgr.GetRvuByHandle(pAlarmer->lUserID);
				if (pRvu)
				{
					g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_ILLEGALCONN, 0, &dwVal);
				}
			}
			break;
		default:
			//strTemp.Format("δ֪���ͱ���������: %d��\n",alminfo.dwAlarmType);
			//TRACE(strTemp);
			break;
		}
		break;
	case COMM_ALARM_RULE:
		ProcRuleAlarm(lCommand, pAlarmer, pAlarmInfo, dwBufLen, pUser);
		break;
	default:
		break;
	}
	return;
}


//HK DVR ͸�����ڻص�����
CComDevice* g_pCurComDev232 = NULL;
void CALLBACK DVSSerialCallback(LONG lSerialHandle,char *pRecvBuf, DWORD dwBufSize, DWORD dwUser)
{
	TRACE("͸�����ڻ�����ݣ�handle=%d, ����%d, dwUser=%d\n", lSerialHandle, dwBufSize, dwUser);

// 	if (dwUser != 0)	//dwUserΪ�����豸��ʵ��ָ�룬��ô�ÿ���ÿ�ζ���Ҫ��һ��͸�����ڣ�����Ӱ���ٶȡ�������ʱ��ȫ�ֱ�����
// 	{
// 		CComDevice* pComDev = (CComDevice*)dwUser;
// 		pComDev->AddDataToComBuffer((BYTE*)pRecvBuf, dwBufSize)
// 	}

	if (g_pCurComDev232)	//dwUserΪ�����豸��ʵ��ָ�룬��ô�ÿ���ÿ�ζ���Ҫ��һ��͸�����ڣ�����Ӱ���ٶȡ�������ʱ��ȫ�ֱ�����
	{
		g_pCurComDev232->AddDataToComBuffer((BYTE*)pRecvBuf, dwBufSize);
	}
	else if (g_CBF_Serial232)
	{
		g_CBF_Serial232((HANDLE)(lSerialHandle+1), pRecvBuf, dwBufSize, dwUser);
	}
}

//HK DVR ͸��485�ڻص�����
CComDevice* g_pCurComDev485 = NULL;

void CALLBACK DVS485Callback(LONG l485Handle,char *pRecvBuf, DWORD dwBufSize, DWORD dwUser)
{
	TRACE("͸��485�ڻ�����ݣ�handle=%d, ����%d, dwUser=%d\n", l485Handle, dwBufSize, dwUser);

	if (g_pCurComDev485)	//dwUserΪ�����豸��ʵ��ָ�룬��ô�ÿ���ÿ�ζ���Ҫ��һ��͸�����ڣ�����Ӱ���ٶȡ�������ʱ��ȫ�ֱ�����
	{
		g_pCurComDev485->AddDataToComBuffer((BYTE*)pRecvBuf, dwBufSize);
	}
	else if (g_CBF_Serial485)
	{
		g_CBF_Serial485((HANDLE)(l485Handle+1), pRecvBuf, dwBufSize, dwUser);
	}
}
//������̨����Э��
BOOL gf_NWPTZControl(LONG lHandle, int iChannel,CTRLTYPE command, DWORD dwParam1, DWORD dCamaddr)
{
	if (dwParam1 <= 0)
		dwParam1 = 1;
	if (dwParam1 > 7)
		dwParam1 = 7;
	int nSpeed1 = 0;	//�ٶ�
	switch(dwParam1)
	{
	case 1:
		nSpeed1 = 7;break;
	case 2:
		nSpeed1 = 6;break;
	case 3:
		nSpeed1 = 5;break;
	case 4:
		nSpeed1 = 0;break;
	case 5:
		nSpeed1 = 1;break;
	case 6:
		nSpeed1 = 2;break;
	case 7:
		nSpeed1 = 3;break;
	default:
			break;
	}
	static DWORD oldcommand = -1;
	unsigned char buf[6];
	buf[0] = 0xA0;
	buf[1] = (BYTE)dCamaddr;
	BYTE a = 0;
	switch(command)
	{
	case CT_STOP:
		if (oldcommand == CT_TILT_UP||oldcommand ==CT_TILT_DOWN ||oldcommand == CT_PAN_LEFT || oldcommand == CT_PAN_RIGHT)
			buf[2] = 0xFF;
		else buf[2] = 0x0D;
		buf[3] = 0x00;
		buf[4] = 0x00;
		break;
	case CT_TILT_UP:
		buf[2] = 0x04;
		a = 0;
		buf[3] = (BYTE)(a<<7 | nSpeed1);
		buf[4] = 0x00;
		break;
	case CT_TILT_DOWN:
		buf[2] = 0x04;
		a = 1;
		buf[3] = (BYTE)(a<<7 | nSpeed1);
		buf[4] = 0x00;
		break;
	case CT_PAN_LEFT:
		buf[2] = 0x03;
		a = 1;
		buf[3] = (BYTE)(a<<7 | nSpeed1);
		buf[4] = 0x00;
		break;
	case CT_PAN_RIGHT:
		buf[2] = 0x03;
		a = 0;
		buf[3] = (BYTE)(a<<7 | nSpeed1);
		buf[4] = 0x00;
		break;
/*
	case CT_PAN_AUTO:
		break;
*/
	case CT_ZOOM_IN:
		buf[2] = 0x07;
		buf[3] = 0x00;
		buf[4] = 0x00;
		break;
	case CT_ZOOM_OUT:
		buf[2] = 0x09;
		buf[3] = 0x00;
		buf[4] = 0x00;
		break;
	case CT_FOCUS_NEAR:
		buf[2] = 0x0C;
		buf[3] = 0x00;
		buf[4] = 0x00;
		break;
	case CT_FOCUS_FAR:
		buf[2] = 0x0B;
		buf[3] = 0x00;
		buf[4] = 0x00;
		break;
/*
	case CT_IRIS_ENLARGE:
		break;
	case CT_IRIS_SHRINK:
		break;
*/
	case CT_CALL_PRESET:
		buf[2] = 0x06;
		buf[3] = (int)dwParam1;
		buf[4] = 0x00;
		break;
	case CT_SET_PRESET:
		buf[2] = 0x05;
		buf[3] = (int)dwParam1;
		buf[4] = 0x00;
		break;
	case CT_CLE_PRESET:
		buf[2] = 0x1E;
		buf[3] = 0x00;
		buf[4] = 0x00;
		break;
	default:
		wsprintf(g_szLastError, _T("NANWANG PTZControl error:δ֪��֧�ֵĿ�������:%d"), command);
		return FALSE;
	}

	buf[5] = buf[0]+buf[1]+buf[2]+buf[3]+buf[4];

	TRACE("������̨����Э�飺0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);

	BOOL bRet = NET_DVR_TransPTZ_Other(lHandle,iChannel,(char*)buf, 6);
	oldcommand = command;
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("NANWANG PTZControl failed, code =%d "), NET_DVR_GetLastError());
	}
	return bRet;
}

BOOL gF_SplitFromUrl(LPCSTR lpszUrl, LPCSTR lpszName, CString& strRet)
{
	strRet = "";
	CString strUrl = CString(lpszUrl);
	CString strName = CString(lpszName);
	if (strUrl.IsEmpty() || strName.IsEmpty())
		return FALSE;
	
	int nPos1 = strUrl.Find(CString(lpszName), 0);
	if (nPos1 == -1)
		return FALSE;
	
	int nPos2 = strUrl.Find(_T(";"), nPos1);
	if (nPos2 == -1)
		return FALSE;
	
	CString strSub = strUrl.Mid(nPos1, nPos2-nPos1);
	if (strSub.IsEmpty())
		return FALSE;
	
	strRet = strSub.Mid(strName.GetLength()+1);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//1��	ϵͳ�ຯ��
//////////////////////////////////////////////////////////////////////////
//@��ȡ����һ�β������� ������Ϣ ����
extern "C" BOOL CALL_TYPE ISYS_GetLastError(LPTSTR lpszErrorMsg, UINT nMsgLen)
{
	if(lpszErrorMsg)
	{
		wcsncpy(lpszErrorMsg, g_szLastError, nMsgLen-1);
		lpszErrorMsg[nMsgLen-1] = 0;
		return TRUE;
	}
	return FALSE;
}

//@��ȡ��Ч��� ���
extern "C" BOOL CALL_TYPE ISYS_GetInvalidHandleValue(HANDLE *phHandle)
{
	*phHandle = (HANDLE)NULL;
	return TRUE;
}

//@��ȡ�������Ϣ
extern "C" BOOL CALL_TYPE ISYS_GetPluginInfo (PPLUGIN_INFO pInfo)
{
	if (pInfo == NULL)
	{
		wsprintf(g_szLastError, _T("ISYS_GetPluginInfo: �����Ƿ�"));
		return FALSE;
	}
	strcpy(pInfo->DeviceTypeName, "DFHK");
	strcpy(pInfo->smanufactuer, "DFE");
	strcpy(pInfo->ssoftversion, "2.0.1.3");
	strcpy(pInfo->description, "DFHK�豸���");
	return TRUE;
}

//@ģ���ʼ������
extern "C" BOOL CALL_TYPE ISYS_Initialize(void *pReserved, DWORD dwReserved)
{
	//��ʼ��SDK ��������������ǰ��
	NET_DVR_Init();

	//�������ӳ�ʱ
	NET_DVR_SetConnectTime(5000, 1);
	return TRUE;
}

//@�ͷ���Դ
extern "C" void CALL_TYPE ISYS_Uninitialize()
{
	NET_DVR_Cleanup();
}

//@�ͷŲ���ڷ�����ڴ�
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
//2��@	�豸�ຯ��	�����豸 IP �˿� �û��� ���� �������� �豸�������������� ��չ����
//////////////////////////////////////////////////////////////////////////
extern "C" BOOL CALL_TYPE IDVR_ConnectDevice(LPCSTR  lpszRvuIP, DWORD dwPort,LPCSTR lpszUserName, LPCSTR lpszPassword,TRANSTYPE tType, HANDLE *phDevHandle, LPCSTR pExParamIn,DWORD dwExParamInLen)
{
	*phDevHandle = NULL;

	if (lpszRvuIP == NULL || dwPort == 0 || lpszUserName == NULL || lpszPassword == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_ConnectDevice: �����Ƿ�"));
		return FALSE;
	}

	if (strlen(lpszRvuIP) < 7 || strlen(lpszUserName) == 0)
	{
		wsprintf(g_szLastError, _T("IDVR_ConnectDevice: �����Ƿ�"));
		return FALSE;
	}

	if (tType > TT_MULTICAST && pExParamIn == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_ConnectDevice: ��չ�����Ƿ�"));
		return FALSE;
	}

	//�ж��Ƿ��ظ���¼?
	//....

	//��ʼ����Rvu
	CString sTemp;
/*
	NET_DVR_DEVICEINFO DeviceInfo;
*/
	NET_DVR_DEVICEINFO_V30 DeviceInfo;

#ifndef PROTECT_DLL
	//����
	//---------�����豸	IP �˿� �û��� ���� �豸��Ϣ�����������
	long lRvuHandle = NET_DVR_Login_V30((char*)lpszRvuIP, (WORD)dwPort, (char*)lpszUserName, (char*)lpszPassword, &DeviceInfo);

#else
	//���⴦��
	
	//������ܵ��û��������룺admin��12345
	CString strUserName = lpszUserName;
	CString strPassword = lpszPassword;
	if (strUserName != "admin" || strPassword != "12345"))
	{
		wsprintf(g_szLastError, _T("�����豸(%s)ʧ�ܣ��û����������!",lpszRvuIP);
		return FALSE;
	}
	if (dwPort!=8000)
	{
		sTemp.Format("�����豸(%s)ʧ��: ����ԭ���DVRæ!", lpszRvuIP);
		wsprintf(g_szLastError, _T("%s", sTemp);
		return FALSE;
	}
	//�ж�HK SDK�ĺϷ���
	char szSDKDLL[MAX_PATH];
	sprintf(szSDKDLL, _T("%sHCNetSDK.dll"), g_szDLLPath);
	CFile file;
	if (file.Open(szSDKDLL, CFile::modeRead))
	{
		if (file.GetLength() < 300000)
		{
			wsprintf(g_szLastError, _T("IDVR_ConnectDevice error: �����: -2"));
			return FALSE;
		}
		file.Close();
	}
	
	WORD wPort = 8000;
	char sz[31];
	strcpy(sz, "dongfangelectronics05356582503");
	char username[20] = {0};
	sprintf(username, "%c%c%c", sz[0], sz[4], sz[8]);
	username[3] = 0;
	char pass[20] = {0};
	sprintf(pass, "%c%c%c%c", sz[19], sz[23], sz[25], sz[26]);
	pass[4] = 0;
//	long lRvuHandle = NET_DVR_Login((char*)lpszRvuIP, wPort+8, username, pass, &DeviceInfo);
	long lRvuHandle = NET_DVR_Login_V30((char*)lpszRvuIP, wPort+8, username, pass, &DeviceInfo);
	//���⴦�����
#endif

	if(lRvuHandle < 0)
	{
		//-------�����������Ĵ���
		int nErr = NET_DVR_GetLastError();
		if (nErr == NET_DVR_PASSWORD_ERROR)
		{
			sTemp.Format(_T("�����豸(%s)ʧ�ܣ��û����������!"), lpszRvuIP);
			wsprintf(g_szLastError, _T("%s"), sTemp);
		}
		else	//��������ֵ
		{
			sTemp.Format(_T("�����豸(%s)ʧ��: ����ԭ���DVRæ!"), lpszRvuIP);
			wsprintf(g_szLastError, _T("%s"), sTemp);
		}
		return FALSE;
	}

	TRACE("====Login Ruv(IP:%s) �ɹ�, lRvuHandle=%d\n", lpszRvuIP, lRvuHandle);

	//----------������Ϣ�ص�����
	NET_DVR_SetDVRMessageCallBack_V30(MessageCallback, NULL);

	//���ӳɹ��������豸��Ϣ
	CRvu* pRvu = new CRvu();
	if (pRvu == NULL)
	{
		return FALSE;
	}
	strncpy(pRvu->szRvuIP, lpszRvuIP, MAXLEN_IP);
#ifndef PROTECT_DLL
	strncpy(pRvu->szRvuUserName, lpszUserName, MAXLEN_STR);	//����
	strncpy(pRvu->szRvuPassword, lpszPassword, MAXLEN_STR);	//����
#else
	strncpy(pRvu->szRvuUserName, username, MAXLEN_STR);		//���⴦��
	strncpy(pRvu->szRvuPassword, pass, MAXLEN_STR);			//���⴦��
#endif
	pRvu->iConnectType = tType;
	pRvu->wServerType = DeviceInfo.byDVRType;
	pRvu->iChannelNumber = DeviceInfo.byChanNum;
	pRvu->dwAlarmInNum = DeviceInfo.byAlarmInPortNum;
	pRvu->dwAlarmOutNum = DeviceInfo.byAlarmOutPortNum;
	pRvu->lStartChannel = DeviceInfo.byStartChan;
	pRvu->dwDiskNum = DeviceInfo.byDiskNum;

	pRvu->lRvuHandle = lRvuHandle;
	//��ȡ�豸�ͺ���Ϣ
//	NET_DVR_DEVICECFG struDeviceCfg;
//	DWORD dwRetBytes;
// 	if(NET_DVR_GetDVRConfig(lRvuHandle, NET_DVR_GET_DEVICECFG, 0, &struDeviceCfg, sizeof(NET_DVR_DEVICECFG), &dwRetBytes))
// 	{
// 		int iDeviceType = struDeviceCfg.byDVRType;
// 		if (iDeviceType > 70)
// 		{
// 			memcpy(g_pbHead, pbHead_V30, 40);
// 		}
// 		else
// 		{
// 			memcpy(g_pbHead,pbHead,40);
// 		}
// 	}
// 	else{
// 		memcpy(g_pbHead,pbHead,40);
// 		TRACE("NET_DVR_GetDVRConfigʧ��! code=%d"), NET_DVR_GetLastError());
// 	}

	g_rvuMgr.AddRvu(pRvu);

	*phDevHandle = pRvu;

	return TRUE;
}

//@�Ͽ��豸���� �豸���
extern "C" BOOL CALL_TYPE IDVR_DisConnectDevice(HANDLE hDevHandle)
{
	CRvu* pRvu = (CRvu*)hDevHandle;//�豸�������
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_DisConnectDevice error: Invalid Handle"));
		return FALSE;
	}

	//------------�Ͽ�������Ƶ����
	short i;
	//�豸ͨ���б�
	for (i=0; i<pRvu->m_arrayChan.GetSize(); i++)
	{
		CChannel* pChan = (CChannel*)pRvu->m_arrayChan.GetAt(i);
		IDVR_StopPreview((HANDLE)pChan);
	}

	//----------�ر�232��
	if (pRvu->l232Handle != DFJK_INVALID_HANDLE)
		NET_DVR_SerialStop(pRvu->l232Handle);
	//----------�ر�͸��485��
	if (pRvu->l485Handle != DFJK_INVALID_HANDLE)
		NET_DVR_SerialStop(pRvu->l485Handle);

	//----------�رձ�������
	if (pRvu->lAlarmHandle != DFJK_INVALID_HANDLE)
		NET_DVR_CloseAlarmChan(pRvu->lAlarmHandle);

	//----------�ر������Խ�
	if (pRvu->lVoiceComHandle != DFJK_INVALID_HANDLE)
		NET_DVR_StopVoiceCom(pRvu->lVoiceComHandle);

	//-----------�Ͽ��豸
	BOOL bRet = FALSE;
	if (pRvu->lRvuHandle != DFJK_INVALID_HANDLE)
	{
		bRet = NET_DVR_Logout_V30(pRvu->lRvuHandle);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("Logout(%s)ʧ��! code=%d"), pRvu->szRvuIP, NET_DVR_GetLastError());
		}
		else
		{
			TRACE("====Logout Rvu(IP:%s)�ɹ�!\n", pRvu->szRvuIP);
		}
	}

	//ɾ����ʵ��
	g_rvuMgr.RemoveRvu(pRvu);

	return bRet;
}

//@��ʼԤ��	�豸��� Ԥ��ͼ�񴰿� Ԥ��ͨ�� ���� ��ͨ����������������
extern "C" BOOL CALL_TYPE IDVR_StartPreview(HANDLE hDevHandle,HWND hWnd,int iChannel,int iStreamType, HANDLE *phChanHandle, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	*phChanHandle = NULL;

	CRvu* pRvu = (CRvu*)hDevHandle;//�豸����
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartPreview error: Invalid Dev Handle"));
		return FALSE;
	}

	//hk ���ʽDVR ����ͨ���Ŵ���32
// 	if (iChannel <= 0 || iChannel > pRvu->iChannelNumber)
// 	{
// 		wsprintf(g_szLastError, _T("IDVR_StartPreview error: Invalid chan No(1��%d)"), pRvu->iChannelNumber);
// 		return FALSE;
// 	}

	//����Ԥ������ֹͣԤ��
//	if (pRvu->pChan[iChannel-1])
//	{
//		IDVR_StopPreview((HANDLE)pRvu->pChan[iChannel-1]);
//	}

	//Ԥ����Ƶ
	NET_DVR_CLIENTINFO cliInfo;//�ͻ�����Ϣ
	cliInfo.lLinkMode = pRvu->iConnectType;
	if (iStreamType == 1)
 	{
 		cliInfo.lLinkMode |= 0x80000000;		//������������־
 	}

	cliInfo.hPlayWnd = hWnd;//ͼ�񴰿�
	cliInfo.lChannel = iChannel;//ͨ����
	cliInfo.sMultiCastIP = pRvu->szRvuMultiIP;//
/*
	long lChanHandle = NET_DVR_RealPlay(pRvu->lRvuHandle, &cliInfo);
*/
	//---------ʵʱԤ��	��ǰԤ����� Ԥ������ �������ݻص����� �û����� ����������ʽ�Ƿ�������ʽ
	long lChanHandle = NET_DVR_RealPlay_V30(pRvu->lRvuHandle, &cliInfo,NULL,NULL,TRUE);
	if (lChanHandle == DFJK_INVALID_HANDLE)//����-1��ʾʧ��
	{
		wsprintf(g_szLastError, _T("Ԥ��RVU(IP:%s)ͨ��%dʧ��"), pRvu->szRvuIP, iChannel);
		return FALSE;
	}

	//����ͨ��ʵ��----�ɹ�Ԥ�������ͨ��ʵ��
	CChannel* pChan = new CChannel();
	if (pChan == NULL)
	{
		return FALSE;
	}
	pChan->nChanNo = iChannel;
	pChan->lChanHandle = lChanHandle;
	g_rvuMgr.AddChan(pRvu, pChan);//��ĳһ�豸���ͨ��

	*phChanHandle = pChan;

	return TRUE;
}

//@ֹͣԤ��	ͨ�����
extern "C" BOOL CALL_TYPE IDVR_StopPreview(HANDLE hChanHandle)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopPreview error: Invalid Handle!"));
		return FALSE;
	}
	//ֹͣ¼��
	if (pChan->bIsRecording)
	{
		IDVR_StopRecord(hChanHandle);
	}

	//ֹͣԤ��
	BOOL bRet = FALSE;
	if (pChan->lChanHandle != DFJK_INVALID_HANDLE)
	{
		//ֹͣԤ�� ͨ����� ����TRUE FASLE
		bRet = NET_DVR_StopRealPlay(pChan->lChanHandle);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("ֹͣԤ��RVU ͨ��%dʧ��"), pChan->nChanNo);
		}
	}

	//ɾ��ͨ����ʵ��
	g_rvuMgr.RemoveChan(pChan);

	return TRUE;
}


void CALLBACK RealdataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,DWORD dwUser)
{
//	printf("Realdata: handle=%d, type=%d, size=%d\n", lRealHandle,dwDataType,dwBufSize);
	if(dwUser != NULL)
	{
		CChannel* pChannel = (CChannel*)dwUser;
		if(pChannel->pfnRealDataCallBack!=NULL)
			pChannel->pfnRealDataCallBack( (HANDLE) pChannel, pBuffer, dwBufSize,
										pChannel->dwCookie,	dwDataType);
		if (dwDataType == 1)
		{
			//����֡ͷ
			pChannel->pStreamHeader = new BYTE[dwBufSize];
			memcpy(pChannel->pStreamHeader, pBuffer, dwBufSize);
			pChannel->dwStreamheaderLen = dwBufSize;
			SetEvent(pChannel->hWaitHeaderEvent);
		}
	}
}

void CALLBACK RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void *pUser)
{
	if(pUser != NULL)
	{
		CChannel* pChannel = (CChannel*)pUser;
		if(pChannel->pfnRealDataCallBack!=NULL)
			pChannel->pfnRealDataCallBack( (HANDLE) pChannel, pBuffer, dwBufSize,
										pChannel->dwCookie,	dwDataType);

		if (dwDataType == 1)
		{
			//����֡ͷ
			pChannel->pStreamHeader = new BYTE[dwBufSize];
			memcpy(pChannel->pStreamHeader, pBuffer, dwBufSize);
			pChannel->dwStreamheaderLen = dwBufSize;
			SetEvent(pChannel->hWaitHeaderEvent);
		}
	}

}
//
extern "C" BOOL CALL_TYPE IDVR_StartRealData(HANDLE hDevHandle, int iChannel, int iStreamType,  HANDLE *phChanHandle,
								  DWORD dwCookie,  LPCSTR pExParamIn, DWORD dwExParamInLen, RealDataCallBack callbackHandler)
{
    *phChanHandle = NULL;

	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartRealData error: Invalid Dev Handle"));
		return FALSE;
	}
	wprintf(_T("���뺣����̬��!\r\n"));
// 	if (iChannel <= 0 || iChannel > pRvu->iChannelNumber)
// 	{
// 		wsprintf(g_szLastError, _T("IDVR_StartRealData error: Invalid chan No(1��%d)", pRvu->iChannelNumber);
// 		return FALSE;
// 	}
	
	//Ԥ����Ƶ���޴��ڷ�ʽ��
	NET_DVR_CLIENTINFO cliInfo;
	cliInfo.lLinkMode = pRvu->iConnectType;
 	if (iStreamType == 1)
 	{
 		cliInfo.lLinkMode |= 0x80000000;		//������������־
 	}
	cliInfo.hPlayWnd = NULL;
	cliInfo.lChannel = iChannel;
	cliInfo.sMultiCastIP = pRvu->szRvuMultiIP;
/*
	long lChanHandle = NET_DVR_RealPlay(pRvu->lRvuHandle, &cliInfo);    

	if (lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_StartRealData: Ԥ��RVU(IP:%s)ͨ��%dʧ��", pRvu->szRvuIP, iChannel);
		return FALSE;
	}
*/
	CChannel* pChan = new CChannel();
	pChan->nChanNo = iChannel;
// 	pChan->lChanHandle = lChanHandle;
	pChan->dwCookie = dwCookie;
	pChan->pfnRealDataCallBack = callbackHandler;
	pChan->hWaitHeaderEvent = ::CreateEvent(NULL,FALSE, FALSE, NULL);
	g_rvuMgr.AddChan(pRvu, pChan);
/*
	NET_DVR_SetRealDataCallBack(lChanHandle, RealdataCallBack, (DWORD)pChan);
*/
	long lChanHandle = NET_DVR_RealPlay_V30(pRvu->lRvuHandle, &cliInfo,RealDataCallBack_V30, pChan,TRUE);//trueλtcp��ʽȡ��
	wprintf(_T("���뺣����̬��,NET_DVR_RealPlay_V30:%d!\r\n"),lChanHandle);
	if (lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_StartRealData: Ԥ��RVU(IP:%s)ͨ��%dʧ��"), pRvu->szRvuIP, iChannel);

		CloseHandle(pChan->hWaitHeaderEvent);
		g_rvuMgr.RemoveChan(pChan);		
		return FALSE;
	}
	pChan->lChanHandle = lChanHandle;
	if(WaitForSingleObject(pChan->hWaitHeaderEvent,10000)==WAIT_TIMEOUT)//5000MS�½��������޷�������Ӧ
	{
		CloseHandle(pChan->hWaitHeaderEvent);
		return FALSE;
	}

	*phChanHandle = pChan;

	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_GetRealDataHeader(HANDLE hChanHandle,unsigned char *pBuffer,DWORD dwBuffLen,DWORD *pdwHeaderLen)
{
	*pdwHeaderLen = 0;


//	CChannel* pChan = (CChannel*)hChanHandle;				//ԭ���ģ�ֻ������ʵʱ�ط�
	CChannelBase* pChan = (CChannelBase*)hChanHandle;		//��ӦʵʱԤ�����ص���ʽԶ�̻ط�  wyz 2010.8.31
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRealDataHeader error: Invalid Handle!"));
		return FALSE;
	}

	if (pChan->pStreamHeader == NULL || pChan->dwStreamheaderLen == 0)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRealDataHeader error: ֡ͷ��δ��ȡ�����ȵ�IDVR_StartRealData������ʱ200ms!"));
		return FALSE;
	}

	if (pChan->dwStreamheaderLen > dwBuffLen)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRealDataHeader error: ���������Ȳ��㣬��Ҫ %d"), pChan->dwStreamheaderLen);
		*pdwHeaderLen = pChan->dwStreamheaderLen;
		return FALSE;
	}

	memcpy(pBuffer, pChan->pStreamHeader, pChan->dwStreamheaderLen);
	*pdwHeaderLen = pChan->dwStreamheaderLen;

	
// 	//��ͷֱ��ȡд����
// 	if (pBuffer == NULL)
// 	{
// 		wsprintf(g_szLastError, _T("IDVR_GetRealDataHeader error: pBuffer is null!")));
// 		return FALSE;
// 	}
// 
// 	if (dwBuffLen < 40)
// 	{
// 		wsprintf(g_szLastError, _T("IDVR_GetRealDataHeader error: ���������Ȳ��㣬��Ҫ %d"), 40);
// 		*pdwHeaderLen = 40;
// 		return FALSE;
// 	}
// 
// 	memcpy(pBuffer, g_pbHead, 40);
// 	*pdwHeaderLen = 40;

	return TRUE;
}

//@ֹͣԤ�� ͨ��
extern "C" BOOL CALL_TYPE IDVR_StopRealData(HANDLE hChanHandle)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopRealData error: Invalid Handle!"));
		return FALSE;
	}
	BOOL bRet = FALSE;//����
	if (pChan->lChanHandle != DFJK_INVALID_HANDLE)
	{
		//-----�ص����� ֹͣ�������� ĳһͨ�� �������ݻص����� �û����� 
		NET_DVR_SetRealDataCallBack(pChan->lChanHandle, NULL, 0);

		//-----ֹͣԤ�� ĳһͨ��
		bRet = NET_DVR_StopRealPlay(pChan->lChanHandle);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("IDVR_StopRealData:ֹͣԤ��RVU ͨ��%dʧ��"), pChan->nChanNo);
		}
	}

	//ɾ��ͨ����ʵ��
	g_rvuMgr.RemoveChan(pChan);

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_SwitchCamera(HANDLE hDevHandle, 
								LPCSTR lpszProtocol, int nAddress, 
								int nCamNo, int nMonNo, SWITCHTYPE switchType,
								COMTYPE comType, DWORD dwParam,
								LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	wsprintf(g_szLastError, _T("IDVR_SwitchCamera error: ��δʵ��!"));
	return FALSE;
}

//@ץͼ ͨ����� ͼƬ����ָ���ļ��� ��չ���� ��չ��������
extern "C" BOOL CALL_TYPE IDVR_CapturePicture(HANDLE hChanHandle,LPCSTR lpszFileName, LPCSTR pExParamIn,DWORD dwExParamInLen)
{
	CChannel* pChan = (CChannel*)hChanHandle;//ͨ������
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_CapturePicture error: Invalid Handle!"));
		return FALSE;
	}
	if (lpszFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_CapturePicture error: file name is NULL"));
		return FALSE;
	}
	
	if (strlen(lpszFileName) <= 0)
	{
		wsprintf(g_szLastError, _T("IDVR_CapturePicture error: file name is empty"));
		return FALSE;
	}
	
	if (pChan->lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_CapturePicture error: Invalid Chan handle\n"));
		return FALSE;
	}

	if (strlen(lpszFileName) > 100)		//HK SDK����
	{
		wsprintf(g_szLastError, _T("IDVR_CapturePicture error: ͼƬ�ļ������Ȳ��ܳ���100�ֽ�"));
		return FALSE;
	}

	BOOL bRet = FALSE;

	CString strFileName = CString(lpszFileName);
	if (strFileName.Right(3).CompareNoCase(_T("bmp")) == 0)//�����λ�Ƿ���bmp
	{
		bRet = NET_DVR_CapturePicture(pChan->lChanHandle, (char*)lpszFileName);
	}
	else// if (strFileName.Right(3).CompareNoCase("jpg")) == 0)
	{
		CRvu* pRvu = g_rvuMgr.GetRvuByChan(pChan);
		if (pRvu)
		{
			NET_DVR_JPEGPARA jp;
			jp.wPicQuality = 0;//ͼ������
			jp.wPicSize = 2;
			//ץ�ĳ�JPEG��ʽͼ��
			bRet = NET_DVR_CaptureJPEGPicture(pRvu->lRvuHandle, pChan->nChanNo, &jp, (char*)lpszFileName);
		}
	}
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_CapturePicture failed! Error code = %d"), NET_DVR_GetLastError());

	return bRet;
}

//@��ʼ¼�� ͨ����� �洢�ļ� ��չ���� ��չ��������
extern "C" BOOL CALL_TYPE IDVR_StartRecord(HANDLE hChanHandle,LPCSTR lpszFileName, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartRecord error: Invalid Handle!"));
		return FALSE;
	}
	if (lpszFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartRecord error: file name is NULL"));
		return FALSE;
	}

	if (strlen(lpszFileName) <= 0)
	{
		wsprintf(g_szLastError, _T("IDVR_StartRecord error: file name is empty"));
		return FALSE;
	}

	if (pChan->lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_StartRecord error: Invalid Chan handle\n"));
		pChan->bIsRecording = FALSE;
		return FALSE;
	}

	//��ʼ¼��
	BOOL bRet = FALSE;
	//bRet = NET_DVR_SaveRealData(pChan->lChanHandle, (char*)lpszFileName);
	//ͨ����� �ļ����� �洢�ļ���
	bRet = NET_DVR_SaveRealData_V30(pChan->lChanHandle,0, (char*)lpszFileName);//0-HIK,1-PS,2-3GP
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_StartRecord failed! Error code = %d"), NET_DVR_GetLastError());

	//���±�־
	pChan->bIsRecording = bRet;

	return bRet;
}

//@ֹͣ¼�� ͨ�����
extern "C" BOOL CALL_TYPE IDVR_StopRecord(HANDLE hChanHandle)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopRecord error: Invalid Handle!"));
		return FALSE;
	}

	if (pChan->lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_StopRecord error: Invalid Chan handle\n"));
		pChan->bIsRecording = FALSE;
		return FALSE;
	}

	//ֹͣ¼��
	BOOL bRet = FALSE;
	bRet = NET_DVR_StopSaveRealData(pChan->lChanHandle);
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_StopRecord failed!: Error code = %d"), NET_DVR_GetLastError());

	//���±�־
	pChan->bIsRecording = FALSE;

	return bRet;
}

//@��ȡ¼��״̬ ͨ����� ״̬
extern "C" BOOL CALL_TYPE IDVR_GetRecordState(HANDLE hChanHandle,int *pnState)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRecordState error: Invalid Handle!"));
		return FALSE;
	}
	*pnState = pChan->bIsRecording;
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_SetVideoParams(HANDLE hChanHandle, int iBrightness, int iContrast, int iSaturation,int iHue, BOOL bDefault)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetVideoParams error: Invalid Handle!"));
		return FALSE;
	}

	if (pChan->lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_SetVideoParams error: Invalid Chan handle\n"));
		pChan->bIsRecording = FALSE;
		return FALSE;
	}
	
	DWORD dwB, dwC, dwS, dwH;
	if (bDefault)	//Ĭ��ֵ
	{
		dwB = 6;
		dwC = 6;
		dwS = 6;
		dwH = 6;
	}
	else	//��������Ҫ��Χ��0��255��SDKΪ0��10����Ҫת��
	{
		dwB = iBrightness * 10 / 255;
		dwC = iContrast * 10 / 255;
		dwS = iSaturation * 10 / 255;
		dwH = iHue * 10 / 255;
	}

	BOOL bRet = FALSE;
	bRet = NET_DVR_ClientSetVideoEffect(pChan->lChanHandle, dwB, dwC, dwS, dwH);
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_SetVideoParams failed, Error code = %d"), NET_DVR_GetLastError());
	
	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_GetVideoParams(HANDLE hChanHandle,int *piBrightness,int *piContrast,int *piSaturation,int *piHue)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetVideoParams error: Invalid Handle!"));
		return FALSE;
	}
	
	if (pChan->lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_GetVideoParams error: Invalid Chan handle\n"));
		pChan->bIsRecording = FALSE;
		return FALSE;
	}

	DWORD dwB, dwC, dwS, dwH;
	BOOL bRet = NET_DVR_ClientGetVideoEffect(pChan->lChanHandle, &dwB, &dwC, &dwS, &dwH);
	if (bRet)
	{
		//��������Ҫ��Χ��0��255��SDKΪ0��10����Ҫת��
		*piBrightness = dwB * 255 / 10;
		*piContrast = dwC * 255 / 10;
		*piSaturation = dwS * 255 / 10;
		*piHue = dwH * 255 / 10;
	}
	else
		wsprintf(g_szLastError, _T("IDVR_GetVideoParams failed, Error code = %d"), NET_DVR_GetLastError());

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_StartSound(HANDLE hChanHandle, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartSound error: Invalid Handle!"));
		return FALSE;
	}

	BOOL bRet = FALSE;
	bRet = NET_DVR_OpenSound(pChan->lChanHandle);
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_StartSound failed: Error code = %d"), NET_DVR_GetLastError());

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_StopSound(HANDLE hChanHandle)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopSound error: Invalid Handle!"));
		return FALSE;
	}

	BOOL bRet = FALSE;
	bRet = NET_DVR_CloseSound();
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_StopSound failed: Error code = %d"), NET_DVR_GetLastError());

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_SetVolume(HANDLE hChanHandle,LONG lVolume)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetVolume error: Invalid Handle!"));
		return FALSE;
	}

	if (lVolume < 0)
		lVolume = 0;
	if (lVolume > 100)
		lVolume = 100;

	WORD wVol = (WORD)(lVolume * 0xFFFF / 100);		//HK SDK������ΧΪ0��65535

	BOOL bRet = FALSE;
	bRet = NET_DVR_Volume(pChan->lChanHandle, wVol);
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_SetVolume failed: Error code = %d"), NET_DVR_GetLastError());

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_StartAudioPhone(HANDLE hDevHandle, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartAudioPhone error: Invalid Dev Handle"));
		return FALSE;
	}

	//LONG lRet = NET_DVR_StartVoiceCom((LONG)hDevHandle, fVoiceDataCallBack, 88);
//	LONG lRet = NET_DVR_StartVoiceCom(pRvu->lRvuHandle, NULL, 0);
	//V30��Ҫ����ͨ���ţ�1-16���Լ��ص������������ͣ�0���������������ݣ�1������ǰ��PCMԭʼ���ݣ�
	LONG lRet = NET_DVR_StartVoiceCom_V30(pRvu->lRvuHandle,1,true, NULL, 0);
	if (lRet == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("�����Խ�ʧ��, Error code=%d"), NET_DVR_GetLastError());
		return FALSE;
	}
	else
	{
		//������
		pRvu->lVoiceComHandle = lRet;
	}
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_StopAudioPhone(HANDLE hDevHandle)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopAudioPhone error: Invalid Dev Handle"));
		return FALSE;
	}

	BOOL bRet = FALSE;
	if (pRvu->lVoiceComHandle != DFJK_INVALID_HANDLE)
	{
		bRet = NET_DVR_StopVoiceCom(pRvu->lVoiceComHandle);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("ֹͣ�����Խ�ʧ��, Error code=%d"), NET_DVR_GetLastError());
		}
	}

	//��λ���
	pRvu->lVoiceComHandle = DFJK_INVALID_HANDLE;

	return bRet;
}

//@��̨�ȸ����豸�Ŀ��Ʋ���    �豸���    ͨ����    ������������    ��̨����Э�� ��̨ˮƽ�ٶ�  ��̨��ֱ�ٶ�   ��չ����        ��չ��������
//2016.12.13 ������  �������û���õ� ���õ��� IDVR_PTZControlByChannel ����
extern "C" BOOL CALL_TYPE IDVR_PTZControl(HANDLE hDevHandle,int iChannel,CTRLTYPE command, LPCSTR lpszProtocol, DWORD dwParam1, DWORD dwParam2, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CRvu* pRvu = (CRvu*)hDevHandle;//�豸����
	printf("sdfsdf");
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_PTZControl error: Invalid Handle"));
		return FALSE;
	}

	CString strProtocol = CString(lpszProtocol);//����Э������

	WORD wSpeed, wCamAddr;
	wSpeed = LOWORD(dwParam2);//ˮƽ�ٶ�
	wCamAddr = HIWORD(dwParam2);//��ֱ�ٶ�

	BOOL bRet = FALSE;
	if (strProtocol == _T("EDVR") || strProtocol == _T(""))
	{
		static DWORD olddwPTZCmd = -1;
		DWORD dwPTZCmd = 0;
		DWORD dwStop = 0;
		BOOL bPreset = FALSE;
		bRet = FALSE;
		switch(command)//����
		{
		case CT_STOP://����ֹͣ
			dwPTZCmd = olddwPTZCmd;
			dwStop = 1;
			break;
		case CT_TILT_UP://��̨����
			dwPTZCmd = TILT_UP;
			break;
		case CT_TILT_DOWN://��̨�¸�
			dwPTZCmd = TILT_DOWN;
			break;
		case CT_PAN_LEFT://��̨��ת
			dwPTZCmd = PAN_LEFT;
			break;
		case CT_PAN_RIGHT://��̨��ת
			dwPTZCmd = PAN_RIGHT;
			break;
		case CT_PAN_AUTO://��̨�Զ�
			dwPTZCmd = PAN_AUTO;
			break;
		case CT_ZOOM_IN://���ʱ��
			dwPTZCmd = ZOOM_IN;
			break;
		case CT_ZOOM_OUT://���ʱ�С
			dwPTZCmd = ZOOM_OUT;
			break;
		case CT_FOCUS_NEAR://����ǰ��
			dwPTZCmd = FOCUS_NEAR;
			break;
		case CT_FOCUS_FAR://������
			dwPTZCmd = FOCUS_FAR;
			break;
		case CT_IRIS_ENLARGE://��Ȧ���
			dwPTZCmd = IRIS_OPEN;
			break;
		case CT_IRIS_SHRINK://��Ȧ��С
			dwPTZCmd = IRIS_CLOSE;
			break;
		case CT_CALL_PRESET://ת��Ԥ��
			dwPTZCmd = GOTO_PRESET;
			bPreset = TRUE;
			break;
		case CT_SET_PRESET://����Ԥ��
			dwPTZCmd = SET_PRESET;
			bPreset = TRUE;
			break;
		case CT_CLE_PRESET://���Ԥ��
			dwPTZCmd = CLE_PRESET;
			bPreset = TRUE;
			break;
		case CT_AUX1_PWRON:	//��������1������������: ��������궨��Ķ�Ӧ��ϵ����DVS������ΪPelco-dЭ��ʱ���Ժ�õ���
			dwPTZCmd = WIPER_PWRON;
			dwStop = 0;
			break;
		case CT_AUX1_PWROFF://��������1��
			dwPTZCmd = WIPER_PWRON;
			dwStop = 1;
			break;
		case CT_AUX2_PWRON://��������2��
			dwPTZCmd = LIGHT_PWRON;
			dwStop = 0;
			break;
		case CT_AUX2_PWROFF://��������2��
			dwPTZCmd = LIGHT_PWRON;
			dwStop = 1;
			break;
		case CT_AUX3_PWRON://��������3��
			dwPTZCmd = AUX_PWRON1;
			dwStop = 0;
			break;
		case CT_AUX3_PWROFF://��������3��
			dwPTZCmd = AUX_PWRON1;
			dwStop = 1;
			break;
		case CT_AUX4_PWRON://��������4��
			dwPTZCmd = AUX_PWRON2;
			dwStop = 0;
			break;
		case CT_AUX4_PWROFF://��������4��
			dwPTZCmd = AUX_PWRON2;
			dwStop = 1;
			break;
		case CT_TILT_LEFTUP://��̨����
			dwPTZCmd = UP_LEFT;
			break;
		case CT_TILT_RIGHTUP://��̨����
			dwPTZCmd = UP_RIGHT;
			break;
		case CT_TILT_LEFTDOWN://��̨����
			dwPTZCmd = DOWN_LEFT;
			break;
		case CT_TILT_RIGHTDOWN://��̨����
			dwPTZCmd = DOWN_RIGHT;
			break;
		case CT_CRUISE_ADD:			//Ѳ��������Ԥ�õ�
		case CT_CRUISE_DWELL:		//Ѳ��������ͣ��ʱ��
		case CT_CRUISE_SPEED:		//Ѳ��������Ѳ���ٶ�
		case CT_CRUISE_REMOVE:		//Ѳ������Ѳ��������ɾ��Ԥ�õ�
		case CT_CRUISE_RUN:			//Ѳ��������
		case CT_CRUISE_STOP:		//Ѳ����ֹͣ
			{
				DWORD dwCmd = command;	//ת��ΪHK SDK����
				if (command == CT_CRUISE_RUN)
					dwCmd = RUN_SEQ;
				else if (command == CT_CRUISE_STOP)
					dwCmd = STOP_SEQ;
				BYTE byCruiseRoute  = (BYTE)HIWORD(dwParam1);
				BYTE byCruisePoint  = (BYTE)LOWORD(dwParam1);
				WORD wInput = (WORD)dwParam2;
				if (!NET_DVR_PTZCruise_Other(pRvu->lRvuHandle, iChannel, dwCmd, byCruiseRoute, byCruisePoint, wInput))
				{
					wsprintf(g_szLastError, _T("NET_DVR_PTZCruise_Other failed: Error code = %d"), NET_DVR_GetLastError());
					return FALSE;
				}
				else
					return TRUE;
			}
			break;
		case CT_TRACK_STARTREC:		//�켣����ʼ��¼
		case CT_TRACK_STOPREC:		//�켣��ֹͣ��¼
		case CT_TRACK_RUN:			//�켣������
			{
				DWORD dwCmd = 0;	//ת��ΪHK SDK����
				if (command == CT_TRACK_STARTREC)
					dwCmd = STA_MEM_CRUISE;
				else if (command == CT_TRACK_STOPREC)
					dwCmd = STO_MEM_CRUISE;
				else
					dwCmd = RUN_CRUISE;
				if (!NET_DVR_PTZTrack_Other(pRvu->lRvuHandle, iChannel, dwCmd))
				{
					wsprintf(g_szLastError, _T("NET_DVR_PTZTrack_Other failed: Error code = %d"), NET_DVR_GetLastError());
					return FALSE;
				}
				else
					return TRUE;				
			}
			break;
		case CT_RECTZOOM:	//3D��λ��HK IP�����й��ܣ�
			{
				NET_DVR_POINT_FRAME frm;
				frm.xTop = LOWORD(dwParam1);
				frm.yTop = HIWORD(dwParam1);
				frm.xBottom = LOWORD(dwParam2);
				frm.yBottom = HIWORD(dwParam2);
				frm.bCounter = 0;	//��������
				if (!NET_DVR_PTZSelZoomIn_EX(pRvu->lRvuHandle, iChannel, &frm))
				{
					wsprintf(g_szLastError, _T("NET_DVR_PTZSelZoomIn_EX failed: Error code = %d"), NET_DVR_GetLastError());
					return FALSE;
				}
				else
					return TRUE;			
			}
			break;
		default:
			wsprintf(g_szLastError, _T("IDVR_PTZControl error:δ֪��֧�ֵĿ�������:%d"), command);
			return FALSE;
		}

		if (!bPreset)	//��Ԥ��λ����
		{
			//HK SDK���ƣ��ٶȷ�Χ��1��7��
			if (dwParam1 <= 0)
				dwParam1 = 1;
			if (dwParam1 > 7) 
				dwParam1 = 7;

			//�ȷ�һ��ֹͣ���˵����Readme.txt��
			//NET_DVR_PTZControlWithSpeed_Other(pRvu->lRvuHandle, iChannel, dwPTZCmd, 1, 0);

			//bRet = NET_DVR_PTZControl_EX((LONG)g_infoDEV[iIndex].infoChn[iChannel-1].hChannel, dwPTZCmd, dwStop);
			//���ٶȵ���̨���Ʋ��� �豸��� ͨ���� �������� ��̨����ֹͣ��ʼ �ٶ� 
			//dwPTZCmd = 2Ϊ��ͨ�ƹ��Դ
			//std::cout<<"������̨ʧ��,���صĴ����:"<<dwPTZCmd<<std::endl;
			//���µ�����ͷ�����Դ��ƹ�ģ�����ͨ������NET_DVR_PTZControlWithSpeed_Other���Դ��ĵƹ�
			//������Щ����ͷ����û�еƹ⣬��ô���ҿ����Լ����죬ռ�ñ��������0�ţ������Ƶƹ⡣
			bRet = NET_DVR_PTZControlWithSpeed_Other(pRvu->lRvuHandle, iChannel, dwPTZCmd, dwStop, dwParam1);
			if (!bRet)
			{
				if(dwPTZCmd == 2)
				{
					if(dwStop == 0)
					{
						//��0�ű��������
						bRet = NET_DVR_SetAlarmOut(pRvu->lRvuHandle,0,1);
						if(bRet<=0)	TRACE("��0�ű��������ʧ�ܣ�");
					}
					else
					{
						//�ر�0�ű��������
						bRet = NET_DVR_SetAlarmOut(pRvu->lRvuHandle,0,0);
						if(bRet<=0)	TRACE("��0�ű��������ʧ�ܣ�");
					}
				}
				else
				{
					//�������Ͻӿ�
					bRet = NET_DVR_PTZControl_Other(pRvu->lRvuHandle, iChannel, dwPTZCmd, dwStop);
				}
			}
		}
		else	//Ԥ��λ�й�����
		{
			//��㷢һ��ֹͣ����
			//NET_DVR_PTZControlWithSpeed_Other(pRvu->lRvuHandle, iChannel, TILT_UP, 1, 0);

			//bRet = NET_DVR_PTZPreset_EX((LONG)g_infoDEV[iIndex].infoChn[iChannel-1].hChannel, dwPTZCmd, dwParam1);
			//��̨Ԥ�õ���� �豸��� ͨ���� Ԥ�õ����� Ԥ�õ����
			bRet = NET_DVR_PTZPreset_Other(pRvu->lRvuHandle, iChannel, dwPTZCmd, dwParam1);
		}

		if (!bRet)
			wsprintf(g_szLastError, _T("IDVR_PTZControl failed: Error code = %d"), NET_DVR_GetLastError());

		olddwPTZCmd = dwPTZCmd;
	}
	//������̨����Э��
	else if (strProtocol == _T("NANWANG"))
	{
// 		if (iChannel <= 0 || iChannel > pRvu->iChannelNumber)
// 		{
// 			wsprintf(g_szLastError, _T("IDVR_PTZControl error: Invalid chan No(1��%d)", pRvu->iChannelNumber);
// 			return FALSE;
// 		}
		
		TRACE("hDevHandle= %d,iChannel=%d,command=%d,dwParam1=%d,CamAddr=%d\n",(LONG)hDevHandle,iChannel,command,dwParam1,wCamAddr);
		return gf_NWPTZControl(pRvu->lRvuHandle,iChannel, command, dwParam1, wCamAddr);
	}
	else
	{
		wsprintf(g_szLastError, _T("IDVR_PTZControl error: ��֧�ֵ�PTZЭ�飺%s"), strProtocol);
		return FALSE;
	}

	return bRet;
}
/**
	��̨����ʵ�ֺ���
**/
//@���Ƽ�ʵ�֣��õ�HK SDK��ͬ��������Ƶͨ��Ԥ�����������̨�ȸ��� ͨ����� ��������
extern "C" BOOL CALL_TYPE IDVR_PTZControlByChannel(HANDLE hChanHandle,CTRLTYPE command, DWORD dwParam1, DWORD dwParam2, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_PTZControlByChannel error: Invalid Handle"));
		return FALSE;
	}

	BOOL bRet = FALSE;

		//HK SDK���ƣ��ٶȷ�Χ��1��7��
		if (dwParam1 <= 0)
			dwParam1 = 1;
		if (dwParam1 > 7)
		dwParam1 = 7;

		static DWORD olddwPTZCmd = -1;
		DWORD dwPTZCmd = 0;
		DWORD dwStop = 0;
		BOOL bPreset = FALSE;
		bRet = FALSE;
		switch(command)
		{
		case CT_STOP:
			dwPTZCmd = olddwPTZCmd;
			dwStop = 1;
			break;
		case CT_TILT_LEFTUP:
			dwPTZCmd = UP_LEFT;
			break;
		case CT_TILT_RIGHTUP:
			dwPTZCmd = UP_RIGHT;
			break;
		case CT_TILT_LEFTDOWN:
			dwPTZCmd = DOWN_LEFT;
			break;
		case CT_TILT_RIGHTDOWN:
			dwPTZCmd = DOWN_RIGHT;
			break;
		case CT_TILT_UP:
			dwPTZCmd = TILT_UP;
			break;
		case CT_TILT_DOWN:
			dwPTZCmd = TILT_DOWN;
			break;
		case CT_PAN_LEFT:
			dwPTZCmd = PAN_LEFT;
			break;
		case CT_PAN_RIGHT:
			dwPTZCmd = PAN_RIGHT;
			break;
		case CT_PAN_AUTO:
			dwPTZCmd = PAN_AUTO;
			break;
		case CT_ZOOM_IN:
			dwPTZCmd = ZOOM_IN;
			break;
		case CT_ZOOM_OUT:
			dwPTZCmd = ZOOM_OUT;
			break;
		case CT_FOCUS_NEAR:
			dwPTZCmd = FOCUS_NEAR;
			break;
		case CT_FOCUS_FAR:
			dwPTZCmd = FOCUS_FAR;
			break;
		case CT_IRIS_ENLARGE:
			dwPTZCmd = IRIS_OPEN;
			break;
		case CT_IRIS_SHRINK:
			dwPTZCmd = IRIS_CLOSE;
			break;
		case CT_CALL_PRESET:
			dwPTZCmd = GOTO_PRESET;
			bPreset = TRUE;
			break;
		case CT_SET_PRESET:
			dwPTZCmd = SET_PRESET;
			bPreset = TRUE;
			break;
		case CT_CLE_PRESET:
			dwPTZCmd = CLE_PRESET;
			bPreset = TRUE;
			break;
		case CT_AUX1_PWRON:	//��������: ��������궨��Ķ�Ӧ��ϵ����DVS������ΪPelco-dЭ��ʱ���Ժ�õ���
			dwPTZCmd = WIPER_PWRON;
			dwStop = 0;
			break;
		case CT_AUX1_PWROFF:
			dwPTZCmd = WIPER_PWRON;
			dwStop = 1;
			break;
		case CT_AUX2_PWRON:				//���������1����--��
			dwPTZCmd = LIGHT_PWRON;
			dwStop = 0;
			break;
		case CT_AUX2_PWROFF:			//���������1����--��
			dwPTZCmd = LIGHT_PWRON;
			dwStop = 1;
			break;
		case CT_AUX3_PWRON:
			dwPTZCmd = AUX_PWRON1;
			dwStop = 0;
			break;
		case CT_AUX3_PWROFF:
			dwPTZCmd = AUX_PWRON1;
			dwStop = 1;
			break;
		default:
			wsprintf(g_szLastError, _T("IDVR_PTZControl error:δ֪��֧�ֵĿ�������:%d"), command);
			return FALSE;
		}
		//std::cout << "************HKDevice ��̬����ã���̨���Ʊ��:" << dwPTZCmd << std::endl;
		if (!bPreset)	//��Ԥ��λ����
		{
			if(dwPTZCmd == 2)
			{
				if(dwStop == 0)
				{
					//��0�ű��������
					bRet = NET_DVR_SetAlarmOut(pChan->lChanHandle,0,1);
					std::cout << "HKDevice ��̬����ã�����0�ű�������ڣ���̨���Ʊ��:" << dwPTZCmd << "����ֵ:" << bRet << std::endl;
				}
				else
				{
					//�ر�0�ű��������
					bRet = NET_DVR_SetAlarmOut(pChan->lChanHandle,0,0);
					std::cout << "HKDevice ��̬����ã��ر�0�ű�������ڣ���̨���Ʊ��:" << dwPTZCmd << "����ֵ:" << bRet << std::endl;
				}
			}
			else
			{
				bRet = NET_DVR_PTZControlWithSpeed(pChan->lChanHandle, dwPTZCmd, dwStop, dwParam1);
				if(!bRet)
				{
					//�������Ͻӿ�
					bRet = NET_DVR_PTZControl(pChan->lChanHandle, dwPTZCmd, dwStop);
					std::cout << "HKDevice ��̬����ã���ͨ��̨����ʧ�ܣ���̨���Ʊ��:" << dwPTZCmd << "����ֵ:" << bRet << std::endl;
				}
				else
				{
					std::cout << "HKDevice ��̬����ã���ͨ��̨���Ƴɹ�����̨���Ʊ��:" << dwPTZCmd << std::endl;
				}
			}
			
		}
		else	//Ԥ��λ�й�����
		{
			bRet = NET_DVR_PTZPreset(pChan->lChanHandle, dwPTZCmd, dwParam1);
			std::cout << "HKDevice ��̬����ã�Ԥ��λ�й������̨���Ʊ��:" << dwPTZCmd << std::endl;
		}

		if (!bRet)
			wsprintf(g_szLastError, _T("IDVR_PTZControl failed: Error code = %d"), NET_DVR_GetLastError());

		olddwPTZCmd = dwPTZCmd;

	return bRet;
}

//@����豸��Ϣ �豸��� ������������豸��Ϣ
extern "C" BOOL CALL_TYPE IDVR_GetDeviceInfo(HANDLE hDevHandle,PDEVICEINFO pDeviceInfo)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetDeviceInfo error: Invalid Handle"));
		return FALSE;
	}

	if (pDeviceInfo == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetDeviceInfo: Invalid Params!"));
		return FALSE;
	}

	pDeviceInfo->nchannelnum =pRvu->iChannelNumber;//ͨ����
	pDeviceInfo->ndisknumber = pRvu->dwDiskNum;//Ӳ����
	pDeviceInfo->nioinputnum = pRvu->dwAlarmInNum;//����������
	pDeviceInfo->niooutputnum = pRvu->dwAlarmOutNum;//���������
	sprintf(pDeviceInfo->sdevicetype, "DF1500-EDVR");//�豸����
	sprintf(pDeviceInfo->smanufactuer, "DongFang");//����
	sprintf(pDeviceInfo->ssoftversion, "V1.0");//�汾

	return TRUE;
}

//@�õ�ͨ����Ϣ �豸��� �����������ͨ����Ϣ
extern "C" BOOL CALL_TYPE IDVR_GetChannelInfo(HANDLE hDevHandle, PCHANNELINFO pChannelInfo)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetChannelInfo error: Invalid Handle"));
		return FALSE;
	}

	if (pChannelInfo == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetChannelInfo: Invalid Params!"));
		return FALSE;
	}

	pChannelInfo->uchannels = pRvu->iChannelNumber;
	NET_DVR_PICCFG pc;
	DWORD dwRetBytes;
	for (UINT i=0; i<pChannelInfo->uchannels; i++)
	{
		//��ȡ�豸��Ϣ �豸��� ���������ͣ���ȡͼ����� ͨ���� �������ݵĻ���ָ�� ����
		if(NET_DVR_GetDVRConfig(pRvu->lRvuHandle, NET_DVR_GET_PICCFG, i+1, &pc, sizeof(NET_DVR_PICCFG), &dwRetBytes))
		{
			sprintf(pChannelInfo->schannode[i].sname, "%s", pc.sChanName);
		}
		else
			pChannelInfo->schannode[i].sname[0] = 0;
	}

	return TRUE;
}

//@�����豸ʱ�� �豸��� ʱ��
extern "C" BOOL CALL_TYPE IDVR_SetDeviceTime(HANDLE hDevHandle, PDEVTIME pTime)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetDeviceTime error: Invalid Handle"));
		return FALSE;
	}

	if (pTime == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetDeviceTime: Invalid Params!"));
		return FALSE;
	}

	NET_DVR_TIME CurTime;
	CurTime.dwYear = pTime->ufromyear;
	CurTime.dwMonth = pTime->ufrommonth;
	CurTime.dwDay = pTime->ufromday;
	CurTime.dwHour = pTime->ufromhour;
	CurTime.dwMinute = pTime->ufromminute;
	CurTime.dwSecond = pTime->ufromsecond;
	//�����豸������Ϣ �豸��� ���������ͣ�����DVRʱ��
	if (!NET_DVR_SetDVRConfig(pRvu->lRvuHandle, NET_DVR_SET_TIMECFG, 0, &CurTime, sizeof(NET_DVR_TIME)))
	{
		wsprintf(g_szLastError, _T("IDVR_SetDeviceTime failed, err=%d"), NET_DVR_GetLastError());
		return FALSE;
	}

	return TRUE;
}

//@Զ������DVRĳͨ������¼�� �豸��� ͨ�� ¼������ ��չ���� ��չ��������
extern "C" BOOL CALL_TYPE IDVR_StartDeviceRecord(HANDLE hDevHandle, long lChannel, long lRecordType, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartDeviceRecord error: Invalid Dev Handle"));
		return FALSE;
	}
	
	if (lChannel <= 0 || lChannel > DFJK_MAX_CHAN)
	{
		wsprintf(g_szLastError, _T("IDVR_StartDeviceRecord error: Invalid Params!"));
		return FALSE;
	}

	//Զ���ֶ������豸¼�� �豸��� ͨ�� ¼������
	BOOL bRet = NET_DVR_StartDVRRecord(pRvu->lRvuHandle, lChannel, lRecordType);
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_StartDeviceRecord failed: Err=%d"), NET_DVR_GetLastError());

	return bRet;
}

//@Զ��ֹͣDVRĳͨ��¼�� �豸��� ͨ��
extern "C" BOOL CALL_TYPE IDVR_StopDeviceRecord(HANDLE hDevHandle,long lChannel)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopDeviceRecord error: Invalid Dev Handle"));
		return FALSE;
	}

	if (lChannel <= 0 || lChannel > DFJK_MAX_CHAN)
	{
		wsprintf(g_szLastError, _T("IDVR_StopDeviceRecord error: Invalid Params!"));
		return FALSE;
	}

	//Զ���ֶ�ֹͣ¼�� �豸��� ͨ��
	BOOL bRet = NET_DVR_StopDVRRecord(pRvu->lRvuHandle, lChannel);
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_StopDeviceRecord failed: Err=%d"), NET_DVR_GetLastError());

	return bRet;
}

//@Զ�̻�ȡDVRĳͨ��¼��״̬ �豸��� ͨ�� ״̬ ��չ���� ��չ��������
extern "C" BOOL CALL_TYPE IDVR_GetDeviceRecordStatus(HANDLE hDevHandle,long lChannel, int *piState, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	*piState = 0;

	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetDeviceRecordStatus error: Invalid Dev Handle"));
		return FALSE;
	}

	if (lChannel <= 0 || lChannel > pRvu->iChannelNumber)
	{
		wsprintf(g_szLastError, _T("IDVR_GetDeviceRecordStatus error: Invalid chan No(1��%d)"), pRvu->iChannelNumber);
		return FALSE;
	}

/*	NET_DVR_WORKSTATE WorkState;
	if (!NET_DVR_GetDVRWorkState(pRvu->lRvuHandle, &WorkState))
*/
	NET_DVR_WORKSTATE_V30 WorkState;
	//Զ�̻�ȡDVRͨ��¼��״̬
	if (!NET_DVR_GetDVRWorkState_V30(pRvu->lRvuHandle, &WorkState))
	{
		wsprintf(g_szLastError, _T("IDVR_GetDeviceRecordState failed: %d"), NET_DVR_GetLastError());
		return FALSE;
	}
	//ͨ����0��ʼ
	*piState = (int)WorkState.struChanStatic[lChannel-1].byRecordStatic;

	return TRUE;
}

/*
//���ص����öԻ����е��û����ü��Ժ��TABҳ
UINT ThreadConfigDlg( LPVOID pParam )
{
	BOOL bOK = FALSE;
	int nCount = 0;
	while (1)
	{
		HWND hWnd = FindWindow(NULL, _T("Զ�̲�������"));
		if (hWnd)
		{
			HWND hChild = GetWindow(hWnd, GW_CHILD);
			if (hChild)
			{
				char buff[100] = {0};
				GetWindowText(hChild, buff, 100);
				if (strcmp(buff, _T("Tab1")) == 0)
				{
					CTabCtrl* pTab = (CTabCtrl*)(CWnd::FromHandle(hChild));
					if (pTab)
					{
						pTab->DeleteItem(7);
						pTab->DeleteItem(6);
						pTab->DeleteItem(5);
					}
					bOK = TRUE;
					break;
				}
			}
		}
		nCount ++;
		if (bOK || nCount > 50)
			break;
		else
			Sleep(100);
	}
	//TRACE("----SetDlg thread exit, Count=%d\n", nCount);
	return 999;
}

typedef BOOL (FAR _stdcall * LPShowDlg)(long lServerID,CString strUserName,int iLanguageType,BOOL bCenter,int x,int y);
*/

//@Զ�̴��豸���ô��� �豸��� ��չ���� ��չ��������
extern "C" BOOL CALL_TYPE IDVR_ShowDeviceSettingDlg(HANDLE hDevHandle, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_ShowDeviceSettingDlg error: Invalid Dev Handle"));
		return FALSE;
	}

	BOOL bRet = FALSE;
/*	LPShowDlg fShowDlg = NULL;
	char szDLL[MAX_PATH];

	sprintf(szDLL, _T("%sShowHCRemCfgWnd.dll", g_szDLLPath);

	HINSTANCE  hDLL = ::LoadLibrary(szDLL);
	if (hDLL)
	{

		fShowDlg = (LPShowDlg)::GetProcAddress(hDLL, _T("NET_DVR_ShowHCRemoteWindow"));
		if (fShowDlg)
		{
			AfxBeginThread(ThreadConfigDlg, NULL);

			CString str = pRvu->szRvuUserName;
			fShowDlg(pRvu->lRvuHandle, str, 0, TRUE, 0, 0);
			bRet = TRUE;
		}
		FreeLibrary(hDLL);
	}
*/
	NET_DVR_REMCONFIGPARA stRemConfigPara;//�豸
	stRemConfigPara.lServerID   = pRvu->lRvuHandle;
	strcpy(stRemConfigPara.sUserName, pRvu->szRvuUserName);
	stRemConfigPara.iLanguageType = HC_LANGUAGE_CHI;
	stRemConfigPara.bShowIPMode = FALSE;
	stRemConfigPara.byStyle = STYLE_8000;
	
	//���þ���ȫ�����ݵ�Զ������
	
	NET_DVR_SetRemConfigNode(0xff, 0xff, TRUE);
	//�����û�����֮�������
	NET_DVR_SetRemConfigNode(6, 0xff, FALSE);
	NET_DVR_SetRemConfigNode(7, 0xff, FALSE);
	NET_DVR_SetRemConfigNode(8, 0xff, FALSE);
	NET_DVR_SetRemConfigNode(9, 0xff, FALSE);
	NET_DVR_SetRemConfigNode(10, 0xff, FALSE);
	NET_DVR_SetRemConfigNode(11, 0xff, FALSE);
	
	//����Զ������
	bRet = NET_DVR_ShowRemConfigWnd(&stRemConfigPara);

	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_ShowDeviceSettingDlg failed, Check ShowHCRemCfgWnd.dll"));
	}

	return bRet;
}

//@Զ�������豸 �豸���
extern "C" BOOL CALL_TYPE IDVR_RestartDevice(HANDLE hDevHandle)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RestartDevice error: Invalid Dev Handle"));
		return FALSE;
	}
	//Զ�������豸 �豸���
	if (!NET_DVR_RebootDVR(pRvu->lRvuHandle))
	{
		wsprintf(g_szLastError, _T("IDVR_RestartDevice failed: %d"), NET_DVR_GetLastError());
		return FALSE;
	}
	return TRUE;
}

//@����豸״̬ �豸��� �豸״̬
extern "C" BOOL CALL_TYPE IDVR_GetDeviceState(HANDLE hDevHandle, DEV_STAT* pStat)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetChannelInfo error: Invalid Handle"));
		return FALSE;
	}
	//��ȡ�豸״̬
/*	
	NET_DVR_WORKSTATE WorkState;
	UINT i;
	if(!NET_DVR_GetDVRWorkState(pRvu->lRvuHandle, &WorkState))
*/
	NET_DVR_WORKSTATE_V30 WorkState;//�豸����״̬ ����ͨ����������������״̬
	UINT i;
	//���DVR����״̬ �豸��� ����״̬
	if(!NET_DVR_GetDVRWorkState_V30(pRvu->lRvuHandle, &WorkState))
	{
		wsprintf(g_szLastError, _T("IDVR_GetDeviceState failed: %d"), NET_DVR_GetLastError());
		return FALSE;
	}
	for (i=0;i < (UINT)pRvu->iChannelNumber;i++)//ͨ��״̬
	{
		pStat->struChanState[i].byRecordStatic = WorkState.struChanStatic[i].byRecordStatic;
		pStat->struChanState[i].bySignalStatic = WorkState.struChanStatic[i].bySignalStatic;
		pStat->struChanState[i].dwBitRate = WorkState.struChanStatic[i].dwBitRate;
		pStat->struChanState[i].dwLinkNum = WorkState.struChanStatic[i].dwLinkNum;
	}
	//	��ȡӲ����Ϣ
	for (i=0; i<MAX_DISKNUM; i++)
	{
		int j=0;
		if (WorkState.struHardDiskStatic[i].dwVolume > 0 && j < (int)pRvu->dwDiskNum)
		{
			pStat->struDiskState[j].dwVolume = WorkState.struHardDiskStatic[i].dwVolume;
			pStat->struDiskState[j].dwFreeSpace = WorkState.struHardDiskStatic[i].dwFreeSpace;
			pStat->struDiskState[j].dwHardDiskStatic = WorkState.struHardDiskStatic[i].dwHardDiskStatic;
			j=j+1;
		}
	}

	for (i=0;i < pRvu->dwAlarmInNum;i++)//��������״̬
	{
		pStat->byAlarmInStatus[i] = WorkState.byAlarmInStatic[i];
	}

	for (i=0;i < pRvu->dwAlarmOutNum;i++)//�������״̬
	{
		pStat->byAlarmOutStatus[i] = WorkState.byAlarmOutStatic[i];
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//3��	�����������ຯ��	20
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//@����վ���豸�˿ڵ�״̬�� �豸��� ����˿� ���õ���ֵ �豸Э�� �豸��ַ ���� ͨѶ��������
extern "C" BOOL CALL_TYPE IDVR_SetIOStatus( HANDLE hDevHandle, int iIOPort, int iValue,
								 LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType,
								 LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetIOStatus error: Invalid Dev Handle"));
		return FALSE;
	}

	//��ȡЭ��
	CString	strProtocol = CString(lpszProtocol);
	BOOL	bRet = FALSE;
	if (strProtocol == "EDVR" || strProtocol == "")
	{
		if (ioType != IO_OUT)
		{
			wsprintf(g_szLastError, _T("IDVR_SetIOStatus error: ֻ�ܿ���״̬�����"));
			return FALSE;
		}
		if(iIOPort <= 0 || iIOPort > (int)pRvu->dwAlarmOutNum)
		{
			wsprintf(g_szLastError, _T("IDVR_SetIOStatus: IO����Ч����Χ:1-%d"), pRvu->dwAlarmOutNum);
			return FALSE;
		}

		bRet = NET_DVR_SetAlarmOut(pRvu->lRvuHandle, iIOPort-1, iValue);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("IDVR_SetIOStatus Failed, err = %d"), NET_DVR_GetLastError());
		}
	}
	else if (strProtocol == "PTZAUX")		//���ƽ������������أ�Address��Ϊ�����ͨ����portΪ�������غţ�
	{
		//TRACE("IDVR_SetIOStatus(PTZAUX): hDevHandle= %lX,chan=%d, iPort=%d,value=%d\n",(LONG)hDevHandle,nAddress,iIOPort,iValue);
		DWORD dwCmd;
/*		//��Address��ʾ��Ƶͨ���ţ���iIOPort��ʾ�������غ�
		if (iIOPort == 1)		dwCmd = WIPER_PWRON;
		else if (iIOPort == 2)	dwCmd = LIGHT_PWRON;
		else if (iIOPort == 3)	dwCmd = AUX_PWRON1;
		else if (iIOPort == 4)	dwCmd = AUX_PWRON2;
		else
		{
			wsprintf(g_szLastError, _T("IDVR_SetIOStatus(PTZAUX) error: IO port(%d) out of range(1-4)", iIOPort);
			return FALSE;
		}
		if (nAddress < 1 || nAddress > pRvu->iChannelNumber)
		{
			wsprintf(g_szLastError, _T("IDVR_SetIOStatus(PTZAUX) error: ��ַ(%d)Ӧ����Ƶͨ������ͬ(��Χ: 1-%d)", nAddress, pRvu->iChannelNumber);
			return FALSE;
		}
*/
		//��iIOPort��ʾ��Ƶͨ���ţ���Address��ʾ�������غ�
		if (nAddress == 1)		dwCmd = WIPER_PWRON;
		else if (nAddress == 2)	dwCmd = LIGHT_PWRON;
		else if (nAddress == 3)	dwCmd = AUX_PWRON1;
		else if (nAddress == 4)	dwCmd = AUX_PWRON2;
		else
		{
			wsprintf(g_szLastError, _T("IDVR_SetIOStatus(PTZAUX) error: Address(%d) out of range(1-4)"), nAddress);
			return FALSE;
		}

		if (iIOPort < 1 || iIOPort > pRvu->iChannelNumber)
		{
			wsprintf(g_szLastError, _T("IDVR_SetIOStatus(PTZAUX) error: ͨ����(%d)Ӧ����Ƶͨ������ͬ(��Χ: 1-%d)"), iIOPort, pRvu->iChannelNumber);
			return FALSE;
		}

		DWORD dwStop = 1;	//��
		if (iValue == 1)
			dwStop = 0;		//��

		bRet = NET_DVR_PTZControlWithSpeed_Other(pRvu->lRvuHandle, iIOPort, dwCmd, dwStop, 0);
		if (!bRet)
		{
			//�������Ͻӿ�
			bRet = NET_DVR_PTZControl_Other(pRvu->lRvuHandle, iIOPort, dwCmd, dwStop);
		}
	}
	else
	{
		wsprintf(g_szLastError, _T("IDVR_SetIOStatus error: Unknown protocol: %s"), strProtocol);
	}

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_GetIOStatus(HANDLE hDevHandle,int iIOPort,int *piValue, LPCSTR lpszProtocol,
								int nAddress, IOTYPE ioType, COMTYPE comType, int nTimeout, LPCSTR pExParamIn,
								DWORD dwExParamInLen, LPSTR* ppExParamOut, DWORD* pdwExParamOutLen)
{
	*piValue = 0;
	*ppExParamOut = NULL;
	*pdwExParamOutLen = 0;

	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetIOStatus error: Invalid Dev Handle"));
		return FALSE;
	}

	//��ȡЭ��
	CString strProtocol = CString(lpszProtocol);
	BOOL bRet = FALSE;
	if (strProtocol == _T("EDVR") || strProtocol == _T(""))
	{
		int nCount = (ioType == IO_IN ? pRvu->dwAlarmInNum : pRvu->dwAlarmOutNum);
		if(iIOPort < 0 || iIOPort > nCount)
		{
			wsprintf(g_szLastError, _T("IDVR_GetIOStatus: IO����Ч����Χ:1-%d"), nCount);
			return FALSE;
		}

	/*	NET_DVR_WORKSTATE WorkState;
		bRet = NET_DVR_GetDVRWorkState(pRvu->lRvuHandle, &WorkState);
	*/
		NET_DVR_WORKSTATE_V30 WorkState;
		bRet = NET_DVR_GetDVRWorkState_V30(pRvu->lRvuHandle, &WorkState);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("IDVR_GetIOStatus Fail: %d"), NET_DVR_GetLastError());
			return FALSE;
		}

		if (iIOPort == 0)	//������ȡ
		{
			*piValue = -1;	//��������һ��ֵ
			CString strExtOut;
			CString strTemp;
			strExtOut.Format(_T("COUNT=%d;VAL="), nCount);
			for (int i=0; i<nCount; i++)
			{
				if (ioType == IO_OUT)
					strTemp.Format(_T("%d,"), WorkState.byAlarmOutStatic[i]);
				else
					strTemp.Format(_T("%d,"), WorkState.byAlarmInStatic[i]);

				strExtOut += strTemp;
			}
			strExtOut += _T(";");
			DWORD dwLen = strExtOut.GetLength();
			*ppExParamOut = new char[dwLen+1];
			if (*ppExParamOut == NULL)
			{
				wsprintf(g_szLastError, _T("IDVR_GetIOStatus Fail: �ڴ治��"));
				return FALSE;
			}
			strcpy(*ppExParamOut, CString2string(strExtOut).c_str());
			*pdwExParamOutLen = dwLen;
		}
		else
		{
			if (ioType == IO_OUT)
				*piValue = WorkState.byAlarmOutStatic[iIOPort-1];
			else
				*piValue = WorkState.byAlarmInStatic[iIOPort-1];
		}
	}
	else
	{
		wsprintf(g_szLastError, _T("IDVR_GetIOStatus error: Unknown protocol: %s"), strProtocol);
	}

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_SetIOValue(HANDLE hDevHandle,int iIOPort,float fValue, 
							   LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType,
							   LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	wsprintf(g_szLastError, _T("IDVR_SetIOValue error: ��δʵ�ִ˺���"));
	return FALSE;
}

extern "C" BOOL CALL_TYPE IDVR_GetIOValue(HANDLE hDevHandle,int iIOPort,float *pfValue, 
							   LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType, int nTimeout,
							   LPCSTR pExParamIn, DWORD dwExParamInLen, LPSTR* ppExParamOut, DWORD* pdwExParamOutLen)
{
	*ppExParamOut = NULL;
	*pdwExParamOutLen = 0;

	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetIOValue error: Invalid Dev Handle"));
		return FALSE;
	}

	BYTE byAddr = BYTE(nAddress);

	//�жϴ�������, COMNO: 0-��,1-232,2-485
	long lSerial = DFJK_INVALID_HANDLE;
	long lComChan = 0;
	if (comType == COM_232)
	{
		if (g_pCurComDev232)
		{
			wsprintf(g_szLastError, _T("IDVR_GetIOValue error: ͸��������æ���Ժ��ٲ�ѯ"));
			return FALSE;
		}
		//��͸������δ�򿪣������ϴ�
		if (pRvu->l232Handle == DFJK_INVALID_HANDLE)
		{
			pRvu->l232Handle = NET_DVR_SerialStart(pRvu->lRvuHandle, 1, DVSSerialCallback, 0);
			if (pRvu->l232Handle == DFJK_INVALID_HANDLE)
			{
				wsprintf(g_szLastError, _T("IDVR_GetIOValue: RVU͸�����ڴ�ʧ��"));
				return FALSE;
			}
		}

		lSerial = pRvu->l232Handle;
		lComChan = 0;
	}
	else if (comType == COM_485)
	{
		if (g_pCurComDev485)
		{
			wsprintf(g_szLastError, _T("IDVR_GetIOValue error: ͸��485����æ���Ժ��ٲ�ѯ"));
			return FALSE;
		}

		//��͸��485��δ�򿪣������ϴ�
		if (pRvu->l485Handle == DFJK_INVALID_HANDLE)
		{
			pRvu->l485Handle = NET_DVR_SerialStart(pRvu->lRvuHandle, 2, DVS485Callback, 0);
			if (pRvu->l485Handle == DFJK_INVALID_HANDLE)
			{
				wsprintf(g_szLastError, _T("IDVR_GetIOValue: RVU͸��485�ڴ�ʧ��"));
				return FALSE;
			}
		}

		lSerial = pRvu->l485Handle;
		lComChan = 1;	//Ĭ����ͨ��1����
	}
	else
	{
		wsprintf(g_szLastError, _T("IDVR_GetIOValue: ��Ч��������:%d"), comType);
		return FALSE;
	}

	//����Э���ȡ�豸����
	CString strProtocol = CString(lpszProtocol);
	CComDevice* pComDev = NULL;
	if (strProtocol == _T("WS302"))
	{
		pComDev = new CComDeviceWS302();
	}
	else if (strProtocol == _T("LTM8000"))
	{
		pComDev = new CComDeviceLTM8000();
	}
	else if (strProtocol == _T("JWSL"))
	{
		pComDev = new CComDeviceJWSL();
	}
	else
	{
		wsprintf(g_szLastError, _T("IDVR_GetIOValue error: ��֧�ֵ�Э��:%s"), strProtocol);
		return FALSE;
	}

	if (pComDev == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetIOValue error: �ڴ治��"));
		return FALSE;
	}

	BOOL bRet = FALSE;
	pComDev->SetTimeout(nTimeout);

	//�ж�I/Oͨ������
	if ( iIOPort < 0 || iIOPort > pComDev->GetDataChanCount_Float())
	{
		wsprintf(g_szLastError, _T("IDVR_GetIOValue error: I/O�˿ںų���Χ(0~%d)"), pComDev->GetDataChanCount_Float());
	}
	else
	{
		BYTE byBuff[100];
		int nBuffLen = 100;
		pComDev->GetQueryData(byAddr, 0, byBuff, nBuffLen);

		if (comType == COM_232)
			g_pCurComDev232 = pComDev;
		else if (comType == COM_485)
			g_pCurComDev485 = pComDev;

		if (NET_DVR_SerialSend(lSerial, lComChan, (char*)byBuff, nBuffLen))
		{
			float fVal = 0.0f;
			CString strAllVal;
			if (!pComDev->ReadFloatData(iIOPort, fVal, strAllVal))		//��ͬ����ʽ��ȡ����
			{
				wsprintf(g_szLastError, _T("IDVR_GetIOValue: ��ʱ!"));
			}
			else
			{
				bRet = TRUE;
				if (iIOPort == 0)
				{
					DWORD dwLen = strAllVal.GetLength();
					*ppExParamOut = new char[dwLen+1];
					if (*ppExParamOut == NULL)
					{
						wsprintf(g_szLastError, _T("IDVR_GetIOValue Fail: �ڴ治��"));
						bRet = FALSE;
					}
					else
					{
						strcpy(*ppExParamOut, CString2string(strAllVal).c_str());
						*pdwExParamOutLen = dwLen;
					}
				}
				else
				{
					*pfValue = fVal;
				}
			}
		}

		if (comType == COM_232)
			g_pCurComDev232 = NULL;
		else if (comType == COM_485)
			g_pCurComDev485 = NULL;
	}

	delete pComDev;

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_SetAlarmSubscribe(HANDLE hDevHandle, DWORD dwUser, CBF_AlarmInfo cbf, LPCSTR pExParamIn,DWORD dwExParamInLen)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetAlarmSubscribe error: Invalid Dev Handle"));
		return FALSE;
	}
	
//	LONG lAlarmHandle = NET_DVR_SetupAlarmChan(pRvu->lRvuHandle);
	LONG lAlarmHandle = NET_DVR_SetupAlarmChan_V30(pRvu->lRvuHandle);
	if (lAlarmHandle == -1)
	{
		wsprintf(g_szLastError, _T("IDVR_SetAlarmSubscribe failed, Error code=%d"), NET_DVR_GetLastError());
		return FALSE;
	}

	g_CBF_AlarmInfo = cbf;
	pRvu->lAlarmHandle = lAlarmHandle;
	pRvu->dwAlarmUserData = dwUser;

	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_SetAlarmUnsubscribe(HANDLE hDevHandle)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetAlarmUnsubscribe error: Invalid Dev Handle"));
		return FALSE;
	}

//	BOOL bRet = NET_DVR_CloseAlarmChan(pRvu->lAlarmHandle);
	BOOL bRet = NET_DVR_CloseAlarmChan_V30(pRvu->lAlarmHandle);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_SetAlarmUnsubscribe failed, Error code=%d"), NET_DVR_GetLastError());
		return FALSE;
	}

	pRvu->lAlarmHandle = DFJK_INVALID_HANDLE;

	return TRUE;
}


//��������/����
extern "C" BOOL CALL_TYPE IDVR_AlarmSet(HANDLE hDevHandle, int iChannel, LPCSTR lpszProtocol, int nDevAddr, COMTYPE comType, BOOL bEnable, DWORD dwParam)
{
	CString str = CString(lpszProtocol);
	if (!str.IsEmpty() && str != "EDVR")
	{
		wsprintf(g_szLastError, _T("IDVR_AlarmSet error: ��֧��Э��%s"), lpszProtocol);
		return FALSE;
	}
	
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_AlarmSet error: Invalid Handle"));
		return FALSE;
	}
	
	BOOL bRet = FALSE;
	//��������/����,NET_DVR_GetDVRConfig����ͨ����Ŵ�0��ʼ
	NET_DVR_ALARMINCFG_V30 ac;
	DWORD dwRetBytes;
	if(NET_DVR_GetDVRConfig(pRvu->lRvuHandle, NET_DVR_GET_ALARMINCFG_V30,
		iChannel-1, &ac, sizeof(NET_DVR_ALARMINCFG_V30), &dwRetBytes))
	{
		ac.byAlarmInHandle = bEnable ? 1 : 0;
		bRet = NET_DVR_SetDVRConfig(pRvu->lRvuHandle, NET_DVR_SET_ALARMINCFG_V30,
			iChannel-1, &ac, sizeof(NET_DVR_ALARMINCFG_V30));
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("IDVR_AlarmSet error, code=%d"), NET_DVR_GetLastError());
		}
	}
	return bRet;
}

//��ȡ��������/����״̬ (iChannel = 0ʱ��ȡ���б����Ĳ���״̬)
extern "C" BOOL CALL_TYPE IDVR_GetAlarmSetStatus(HANDLE hDevHandle, int iChannel, LPCSTR lpszProtocol, int nDevAddr, COMTYPE comType, BOOL* pEnabled, LPSTR* ppExParamOut, DWORD* pdwExParamOutLen)
{
//	*ppExParamOut = NULL;		//��Ҫ��ֵ����Ϊ����ָ�������Ч��wyz 2010.10.25
//	*pdwExParamOutLen = 0;		//��Ҫ��ֵ����Ϊ����ָ�������Ч��wyz 2010.10.25

	CString str = CString(lpszProtocol);
	if (!str.IsEmpty() && str != _T("EDVR"))
	{
		wsprintf(g_szLastError, _T("IDVR_GetAlarmSetStatus error: ��֧��Э��%s"), lpszProtocol);
		return FALSE;
	}
	
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetAlarmSetStatus error: Invalid Handle"));
		return FALSE;
	}
	
	if (iChannel < 0 || iChannel > pRvu->iChannelNumber)
	{
		wsprintf(g_szLastError, _T("IDVR_GetAlarmSetStatus error: Invalid chan No %d(0��%d)"), iChannel, pRvu->iChannelNumber);
		return FALSE;
	}
	
	BOOL bRet = FALSE;
	//��������/����,NET_DVR_GetDVRConfig����ͨ����Ŵ�0��ʼ
	NET_DVR_ALARMINCFG_V30 ac;
	DWORD dwRetBytes;
	if (iChannel > 0)	//��ȡ����ͨ������״̬
	{
		bRet = NET_DVR_GetDVRConfig(pRvu->lRvuHandle, NET_DVR_GET_ALARMINCFG_V30,
			iChannel-1, &ac, sizeof(NET_DVR_ALARMINCFG_V30), &dwRetBytes);
		if (bRet)
		{
			*pEnabled = (BOOL)ac.byAlarmInHandle;
		}
		else
		{
			wsprintf(g_szLastError, _T("IDVR_GetAlarmSetStatus failed, code=%d"), NET_DVR_GetLastError());
		}
	}
	else	//��ȡ����ͨ������״̬
	{
		CString strExtOut;
		CString strTemp;
		BOOL bOk = FALSE;
		strExtOut.Format(_T("COUNT=%d;VAL="), pRvu->dwAlarmInNum);
		for (int i=0; i<(int)(pRvu->dwAlarmInNum); i++)
		{
			if (NET_DVR_GetDVRConfig(pRvu->lRvuHandle, NET_DVR_GET_ALARMINCFG_V30,
				i, &ac, sizeof(NET_DVR_ALARMINCFG_V30), &dwRetBytes))
			{
				bOk = TRUE;		//ֻҪ�ɹ�һ����������óɹ�
				strTemp.Format(_T("%d,"), ac.byAlarmInHandle);
			}
			else
				strTemp = _T("0,");
			
			strExtOut += strTemp;
		}
		if (!bOk)
		{
			wsprintf(g_szLastError, _T("IDVR_GetAlarmSetStatus failed, code=%d"), NET_DVR_GetLastError());
			return FALSE;
		}
		strExtOut += _T(";");
		DWORD dwLen = strExtOut.GetLength();
		*ppExParamOut = new char[dwLen+1];
		if (*ppExParamOut == NULL)
		{
			wsprintf(g_szLastError, _T("IDVR_GetAlarmSetStatus Fail: �ڴ治��"));
			return FALSE;
		}
		strcpy(*ppExParamOut, CString2string(strExtOut).c_str());
		*pdwExParamOutLen = dwLen;
		bRet = TRUE;
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_SerialStart(HANDLE hDevHandle, LONG lSerialPort, CBF_SerialDataCallBack cbf, DWORD dwUser, HANDLE* phSerial)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetAlarmSetStatus error: Invalid Handle"));
		return FALSE;
	}
	LONG lSerialHandle;
	if (lSerialPort == 1)
	{
		g_CBF_Serial232 = cbf;
		if (pRvu->l232Handle != DFJK_INVALID_HANDLE)
		{
			NET_DVR_SerialStop(pRvu->l232Handle);
			pRvu->l232Handle = DFJK_INVALID_HANDLE;
			g_pCurComDev232 = NULL;
		}
		lSerialHandle = NET_DVR_SerialStart(pRvu->lRvuHandle, lSerialPort, DVSSerialCallback, dwUser);
		pRvu->l232Handle = lSerialHandle;
	}
	else if (lSerialPort == 2)
	{
		g_CBF_Serial485 = cbf;
		if (pRvu->l485Handle != DFJK_INVALID_HANDLE)
		{
			NET_DVR_SerialStop(pRvu->l485Handle);
			pRvu->l485Handle = DFJK_INVALID_HANDLE;
			g_pCurComDev485 = NULL;
		}
		lSerialHandle = NET_DVR_SerialStart(pRvu->lRvuHandle, lSerialPort, DVS485Callback, dwUser);
		pRvu->l485Handle = lSerialHandle;
	}
	if (lSerialHandle == -1)
	{
		wsprintf(g_szLastError, _T("IDVR_SerialStart failed, code=%d"), NET_DVR_GetLastError());
		return FALSE;
	}

	*phSerial = (HANDLE)(lSerialHandle + 1);

	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_SerialSend(HANDLE hSerial, LONG lChannel, char *pSendBuf, DWORD dwBufSize)
{
	if (!hSerial)
	{
		wsprintf(g_szLastError, _T("IDVR_SerialSend error: Invalid Handle"));
		return FALSE;
	}
	if (!NET_DVR_SerialSend((LONG)hSerial-1,lChannel,pSendBuf,dwBufSize))
	{
		wsprintf(g_szLastError, _T("IDVR_SerialSend failed, code=%d"), NET_DVR_GetLastError());
		return FALSE;
	}
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_SerialStop(HANDLE hSerial)
{
	if (!hSerial)
	{
		wsprintf(g_szLastError, _T("IDVR_SerialStop error: Invalid Handle"));
		return FALSE;
	}
	int nType = 0;
	CRvu* pRvu = g_rvuMgr.GetRvuBySerial((LONG)hSerial-1, nType);

	if(!NET_DVR_SerialStop((LONG)hSerial-1))
	{
		wsprintf(g_szLastError, _T("NET_DVR_SerialStop failed, code=%d"), NET_DVR_GetLastError());
		return FALSE;
	}
	if (pRvu)
	{
		if (nType ==1)
			pRvu->l232Handle = DFJK_INVALID_HANDLE;
		else if (nType ==2)
			pRvu->l485Handle = DFJK_INVALID_HANDLE;
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//4��	�ط��ຯ��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//@��ѯDVR����������¼���ļ�
extern "C" BOOL CALL_TYPE IDVR_QueryDevRecord(HANDLE hDevHandle,//�豸���
								   int iChannel,//ͨ��
								   RECORDTYPE RecType,//¼������
								   PQUERYTIME ptime, //ʱ�䷶Χ
								   LPSTR* ppExParamOut, //�������������XML�ַ���������¼����Ϣ
								   DWORD* pdwExParamOutLen)//��չ�����������
{
    *ppExParamOut = NULL;
	*pdwExParamOutLen = 0;

	if (hDevHandle==NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_QueryDevRecord error: Invalid Handle"));
		return FALSE;
	}

   	CRvu* pRvu = (CRvu*)hDevHandle;//�豸ָ��
	
// 	if (iChannel <= 0 || iChannel > pRvu->iChannelNumber)
// 	{
// 		wsprintf(g_szLastError, _T("IDVR_QueryDevRecord error: Invalid chan No(1��%d)", pRvu->iChannelNumber);
// 		return FALSE;
// 	}

	long lFileHandle = -1;
	//��ʼʱ��
	NET_DVR_TIME lStartTime = {ptime->ufromyear,
		ptime->ufrommonth,
		ptime->ufromday,
		ptime->ufromhour,
		ptime->ufromminute,
		ptime->ufromsecond};
	
	//����ʱ��
	NET_DVR_TIME lEndTime = {ptime->utoyear,
		ptime->utomonth,
		ptime->utoday,
		ptime->utohour,
		ptime->utominute,
		ptime->utosecond};
	
	DWORD dwFileType = 0xff;
	/*
	�������Ų���ʱ���ͣ�0xff��ȫ����0����ʱ¼��1-�ƶ���⣬2������������3-�����������ƶ���⣬4-�����������ƶ���⣬5-�������6-�ֶ�¼��7-����¼��
	�����Ų���ʱ���ͣ�0xff��ȫ����0����ʱ¼��1���ƶ���⣬2���ӽ�������3������������4������������5���������6���ֶ�¼��7���𶯱���v
	*/
	switch (RecType)
	{
	case MANUAL_REC://�ֶ�¼��
		dwFileType = 6;
		break;
	case ALARM_REC://����¼��
		dwFileType = 0xff;	//??
		break;
	case PLAN_REC://�ƻ�¼��
		dwFileType = 0;
		break;
	default:
		dwFileType = 0xff;
	}	
	NET_DVR_FILECOND lFindCond;//¼���ļ���������
	lFindCond.lChannel = iChannel;//ͨ��
	lFindCond.dwIsLocked = 0xff;	//0-δ�����ļ���1-�����ļ���0xff��ʾ�����ļ�������������δ������
	lFindCond.dwUseCardNo = 0;		//�Ƿ�����Ų��� 
	lFindCond.dwFileType = dwFileType;//¼���ļ�����
	lFindCond.struStartTime = lStartTime;//��ʼʱ��
	lFindCond.struStopTime = lEndTime;//����ʱ��

	//�����ļ����͡�ʱ������豸¼���ļ� �豸��� ��������
	lFileHandle = NET_DVR_FindFile_V30(pRvu->lRvuHandle,&lFindCond);
	
	if (lFileHandle < 0)
	{
		wsprintf(g_szLastError, _T("IDVR_QueryDevRecord failed,û���ҵ��ļ�" )); 
		return FALSE;
	}
	
	long bRet = -1;
	NET_DVR_FINDDATA_V30 findData;//¼���ļ�����
	CString strXmlHeader;
	strXmlHeader.Format(_T("<?xml version='1.0' ?>"));
	
	CString strXmlFileBody = _T("");
	CString strTemp= _T("");
	int scount = 0;
	
	do 
	{
		//�����ȡ���ҵ����ļ���Ϣ �ļ���� �����ļ���Ϣָ��
		bRet = NET_DVR_FindNextFile_V30(lFileHandle,&findData);
		if (bRet == NET_DVR_FILE_SUCCESS)
		{
			strTemp.Format(_T("<File name='%s' length='%d' starttime='%04d-%02d-%02d %02d:%02d:%02d' endtime='%04d-%02d-%02d %02d:%02d:%02d' type='%d'/>"),
				CString(findData.sFileName),
				findData.dwFileSize / 1024,
				findData.struStartTime.dwYear,
				findData.struStartTime.dwMonth,
				findData.struStartTime.dwDay,
				findData.struStartTime.dwHour,
				findData.struStartTime.dwMinute,
				findData.struStartTime.dwSecond,
				findData.struStopTime.dwYear,
				findData.struStopTime.dwMonth,
				findData.struStopTime.dwDay,
				findData.struStopTime.dwHour,
				findData.struStopTime.dwMinute,
				findData.struStopTime.dwSecond,
				dwFileType
				);
			strXmlFileBody=strXmlFileBody+strTemp;
			scount++;
		}
		
	} while(!( 
		bRet == NET_DVR_FILE_NOFIND || 
			   	 bRet == NET_DVR_FILE_EXCEPTION|| 
				 bRet == NET_DVR_NOMOREFILE|| 
				 bRet == -1)//û���ļ� �����ļ��쳣 �����ļ�ʱû�и�����ļ�
				 );

	if (bRet == -1||bRet == NET_DVR_FILE_EXCEPTION)//û���ļ�
	{
       wsprintf(g_szLastError, _T("IDVR_QueryDevRecord failed,�����ļ�ʧ��" ));
	   return FALSE;
	}

	if (bRet == NET_DVR_FILE_NOFIND)
	{
		 wsprintf(g_szLastError, _T("IDVR_QueryDevRecord failed,û����Ҫ���ҵ��ļ�"));
		 return FALSE;
	}

    strTemp.Format(_T("<Result Count='%d'>"),scount);
	strXmlHeader = strXmlHeader + strTemp + strXmlFileBody;
	strTemp.Format(_T("</Result>"));
	strXmlHeader = strXmlHeader + strTemp;
	
	const long lLength = strXmlHeader.GetLength();
	*pdwExParamOutLen = lLength;

    *ppExParamOut = new char[lLength+1];
	strcpy(*ppExParamOut,CString2string(strXmlHeader).c_str());

	 NET_DVR_FindClose_V30(lFileHandle);

	return TRUE;
}

//@ȡ������ ��δʵ��
extern "C" BOOL CALL_TYPE IDVR_CancelQueryDevRecord(HANDLE hDevHandle)
{
	wsprintf(g_szLastError, _T("IDVR_CancelQueryDevRecord error: ��δʵ��"));
	return FALSE;
}

//@����DVR�豸�е�¼���ļ������� �豸��� �豸¼���ļ��� ���ش洢�ļ��� ���ؾ��
extern "C" BOOL CALL_TYPE IDVR_DownloadFile(HANDLE hDevHandle, LPCSTR lpszSrcFileName, LPCSTR lpszLocalFileName, HANDLE* phDownHandle)
{
	*phDownHandle = NULL;
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_DownloadFile error: Invalid Handle"));
		return FALSE;
	}

	if (lpszSrcFileName == NULL || lpszLocalFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_DownloadFile error: Invalid param!"));
		return FALSE;
	}
	//�豸��� �豸¼���ļ��� ���ش洢�ļ���
	long lDownloadHandle = NET_DVR_GetFileByName(pRvu->lRvuHandle, (char*)lpszSrcFileName, (char*)(LPCSTR)lpszLocalFileName);
	if (lDownloadHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_DownloadFile failed, error code=%d"), NET_DVR_GetLastError());
		return FALSE;
	}
	//����¼��ط�״̬ ����¼��ط�״̬���� ���õĲ��� ��ȡ�Ĳ���
	NET_DVR_PlayBackControl(lDownloadHandle, NET_DVR_PLAYSTART, 0, NULL);

	*phDownHandle = (HANDLE)(lDownloadHandle + 1);	//HK lDownloadHandle ��0��ʼ

	return TRUE;
}

//@��ʱ�������豸�е�¼���ļ� �豸��� ͨ�� ʱ������ ���ش洢�ļ�
extern "C" BOOL CALL_TYPE IDVR_DownloadFileByTime(HANDLE hDevHandle, int iChannel,  PQUERYTIME  pTime, LPCSTR lpszLocalFileName, HANDLE* phDownHandle)
{
	*phDownHandle = NULL;
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_DownloadFileByTime error: Invalid Handle"));
		return FALSE;
	}

	if (lpszLocalFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_DownloadFileByTime error: Invalid param!"));
		return FALSE;
	}

// 	if (iChannel <= 0 || iChannel > pRvu->iChannelNumber)
// 	{
// 		wsprintf(g_szLastError, _T("IDVR_DownloadFileByTime error: Invalid chan No(1��%d)", pRvu->iChannelNumber);
// 		return FALSE;
// 	}

	NET_DVR_TIME ntmStart, ntmEnd;
	ntmStart.dwYear = pTime->ufromyear;
	ntmStart.dwMonth = pTime->ufrommonth;
	ntmStart.dwDay = pTime->ufromday;
	ntmStart.dwHour = pTime->ufromhour;
	ntmStart.dwMinute = pTime->ufromminute;
	ntmStart.dwSecond = pTime->ufromsecond;
	
	ntmEnd.dwYear = pTime->utoyear;
	ntmEnd.dwMonth = pTime->utomonth;
	ntmEnd.dwDay = pTime->utoday;
	ntmEnd.dwHour = pTime->utohour;
	ntmEnd.dwMinute = pTime->utominute;
	ntmEnd.dwSecond = pTime->utosecond;
	long lDownloadHandle = NET_DVR_GetFileByTime(pRvu->lRvuHandle, iChannel, &ntmStart, &ntmEnd, (char *)lpszLocalFileName);
	if (lDownloadHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_DownloadFileByTime failed, error code=%d"), NET_DVR_GetLastError());
		return FALSE;
	}
	*phDownHandle = (HANDLE)(lDownloadHandle + 1);	//HK lDownloadHandle ��0��ʼ

	NET_DVR_PlayBackControl(lDownloadHandle, NET_DVR_PLAYSTART, 0, NULL);

	return TRUE;
}

//@ֹͣ�����豸�е��ļ� ���ؾ��
extern "C" BOOL CALL_TYPE IDVR_StopDownloadFile(HANDLE hDownHandle)
{
	if (hDownHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopDownloadFile Error: invalid handle"));
		return FALSE;
	}
	//ֹͣ����¼���ļ�
	BOOL bRet = NET_DVR_StopGetFile((long)hDownHandle - 1);//HK lDownloadHandle ��0��ʼ
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_StopDownloadFile failed, err code=%d"), NET_DVR_GetLastError());

	return bRet;
}

//@��ȡ��ǰ���ؽ��� ���ؾ�� ��ȡ�ĵ�ǰ���ؽ��ȵ�ָ��
extern "C" BOOL CALL_TYPE IDVR_GetDownloadPos(HANDLE hDownHandle, int *piCurPos)
{
	*piCurPos = 0;
	if (hDownHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetDownloadPos error, invalid id"));
		return FALSE;
	}

	//��ȡ��ǰ���ؽ���0-100֮ǰ ���󷵻�200
	*piCurPos = NET_DVR_GetDownloadPos((long)hDownHandle - 1);

	return TRUE;
}

//@�ط�Զ��¼���ļ� �豸��� DVR�ڵ�¼���ļ� �طŴ��ھ�� �طž��
extern "C" BOOL CALL_TYPE IDVR_RemotePlay (HANDLE hDevHandle, LPCSTR lpszSrcFileName, HWND hWnd, HANDLE* hPlay)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RemotePlay error: Invalid Handle"));
		return FALSE;
	}

	if (lpszSrcFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RemotePlay error: Invalid param!"));
		return FALSE;
	}
	BOOL bCallBack=FALSE;
	HWND hWndTemp = NULL;//��ʱ���ھ��
	if (hWnd == NULL)
	{
		hWndTemp = ::CreateWindow(_T("static"), NULL, 0, 0,0,0,0, NULL,NULL, NULL, 0);//������ʱ����
		hWnd = hWndTemp;
		bCallBack = TRUE;
	}
	//���ļ�������¼���ļ�
	long lPlayHandle = NET_DVR_PlayBackByName(pRvu->lRvuHandle, (char*)lpszSrcFileName, hWnd);
	if (lPlayHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_RemotePlay failed: err code = %d"), NET_DVR_GetLastError());
		return FALSE;
	}
	CRecPlay* pPlay = new CRecPlay();
	if (pPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RemotePlay failed: �ڴ治��"));
		//ֹͣ����
		NET_DVR_StopPlayBack(lPlayHandle);
		return FALSE;
	}
	//����״̬����
	NET_DVR_PlayBackControl(lPlayHandle, NET_DVR_PLAYSTART, 0, NULL);
	pPlay->lPlayHandle = lPlayHandle;
	pPlay->nPlayerIndex = NET_DVR_GetPlayBackPlayerIndex(lPlayHandle);
TRACE("dev plug: IDVR_RemotePlay: lPlayHandle = %d, PlayerIndex = %d\n", lPlayHandle, pPlay->nPlayerIndex);
	pPlay->bIsRemotePlay = TRUE;
	*hPlay = (HANDLE)pPlay;
	if (bCallBack)
		pPlay->hVirtualPlayWnd = hWnd;

	return TRUE;
}

//@��ʱ��ط�¼���ļ� �豸��� ͨ�� ʱ������ ���Ŵ��� ���ž��
extern "C" BOOL CALL_TYPE IDVR_RemotePlayByTime(HANDLE hDevHandle, int iChannel, PQUERYTIME pTime, HWND hWnd, HANDLE* hPlay)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_PlayBackByTime error: Invalid Handle"));
		return FALSE;
	}
	if (pTime == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_PlayBackByTime error: Invalid Params"));
		return FALSE;
	}

// 	if (iChannel <= 0 || iChannel > pRvu->iChannelNumber)
// 	{
// 		wsprintf(g_szLastError, _T("IDVR_PlayBackByTime error: Invalid chan No(1��%d)", pRvu->iChannelNumber);
// 		return FALSE;
// 	}

	NET_DVR_TIME ntmStart, ntmEnd;
	ntmStart.dwYear = pTime->ufromyear;
	ntmStart.dwMonth = pTime->ufrommonth;
	ntmStart.dwDay = pTime->ufromday;
	ntmStart.dwHour = pTime->ufromhour;
	ntmStart.dwMinute = pTime->ufromminute;
	ntmStart.dwSecond = pTime->ufromsecond;

	ntmEnd.dwYear = pTime->utoyear;
	ntmEnd.dwMonth = pTime->utomonth;
	ntmEnd.dwDay = pTime->utoday;
	ntmEnd.dwHour = pTime->utohour;
	ntmEnd.dwMinute = pTime->utominute;
	ntmEnd.dwSecond = pTime->utosecond;

	BOOL bCallBack=FALSE;
	HWND hWndTemp = NULL;
	if (hWnd == NULL)
	{
		hWndTemp = ::CreateWindow(_T("static"), NULL, 0, 0,0,0,0, NULL,NULL, NULL, 0);
		hWnd = hWndTemp;
		bCallBack = TRUE;
	}

	long lPlayHandle = NET_DVR_PlayBackByTime(pRvu->lRvuHandle, iChannel, &ntmStart, &ntmEnd, hWnd);
	if (lPlayHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_PlayBackByTime failed: err code = %d"), NET_DVR_GetLastError());
		return FALSE;
	}

	CRecPlay* pPlay = new CRecPlay();
	if (pPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_PlayBackByTime failed: �ڴ治��"));
		NET_DVR_StopPlayBack(lPlayHandle);//ֹͣ����
		return FALSE;
	}
	//���Ʋ���״̬
	NET_DVR_PlayBackControl(lPlayHandle, NET_DVR_PLAYSTART, 0, NULL);
	pPlay->lPlayHandle = lPlayHandle;
	pPlay->nPlayerIndex = NET_DVR_GetPlayBackPlayerIndex(lPlayHandle);
	pPlay->bRemotePlayByTime = TRUE;
	pPlay->bIsRemotePlay = TRUE;
	*hPlay = (HANDLE)pPlay;
	if (bCallBack)
		pPlay->hVirtualPlayWnd = hWnd;

	return TRUE;
}

//@ֹͣ����¼���ļ� ���ž��
extern "C" BOOL CALL_TYPE IDVR_StopRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopRemotePlay error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	BOOL bRet = NET_DVR_StopPlayBack(pPlay->lPlayHandle);//ֹͣ����
	if (pPlay->hVirtualPlayWnd)
	{
		::DestroyWindow(pPlay->hVirtualPlayWnd);
		pPlay->hVirtualPlayWnd = NULL;
	}
	delete pPlay;
	
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_StopRemotePlay failed: err code = %d"), NET_DVR_GetLastError());
	}
	return bRet;
}

//��ͣ����¼���ļ� ���ž��
extern "C" BOOL CALL_TYPE IDVR_PauseRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_PauseRemotePlay error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	//�ط�״̬���� ���ž�� ��ͣ
	BOOL bRet = NET_DVR_PlayBackControl(pPlay->lPlayHandle, NET_DVR_PLAYPAUSE, 0, NULL);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_PauseRemotePlay failed: err code = %d"), NET_DVR_GetLastError());
	}
	return bRet;
}

//@�ָ�����¼���ļ� ���ž��
extern "C" BOOL CALL_TYPE IDVR_ResumeRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_ResumeRemotePlay error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	//��ͣ��ָ�����¼���ļ�
	BOOL bRet = NET_DVR_PlayBackControl(pPlay->lPlayHandle, NET_DVR_PLAYRESTART, 0, NULL);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_ResumeRemotePlay failed: err code = %d"), NET_DVR_GetLastError());
	}
	return bRet;
}

//@��֡����¼���ļ�
extern "C" BOOL CALL_TYPE IDVR_StepRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StepRemotePlay error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	//��֡����¼���ļ�
	BOOL bRet = NET_DVR_PlayBackControl(pPlay->lPlayHandle, NET_DVR_PLAYFRAME, 0, NULL);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_StepRemotePlay failed: err code = %d"), NET_DVR_GetLastError());
	}
	return bRet;
}

//@�������¼���ļ�
extern "C" BOOL CALL_TYPE IDVR_FastRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_FastRemotePlay error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	BOOL bRet = NET_DVR_PlayBackControl(pPlay->lPlayHandle, NET_DVR_PLAYFAST, 0, NULL);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_FastRemotePlay failed: err code = %d"), NET_DVR_GetLastError());
	}
	return bRet;
}

//@���ٲ���¼���ļ�
extern "C" BOOL CALL_TYPE IDVR_SlowRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SlowRemotePlay error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	BOOL bRet = NET_DVR_PlayBackControl(pPlay->lPlayHandle, NET_DVR_PLAYSLOW, 0, NULL);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_SlowRemotePlay failed: err code = %d"), NET_DVR_GetLastError());
	}
	return bRet;
}

//@�����ٶȲ���¼���ļ�
extern "C" BOOL CALL_TYPE IDVR_NormalRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_NormalRemotePlay error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	BOOL bRet = NET_DVR_PlayBackControl(pPlay->lPlayHandle, NET_DVR_PLAYNORMAL, 0, NULL);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_NormalRemotePlay failed: err code = %d"), NET_DVR_GetLastError());
	}
	return bRet;
}

//@��ȡԶ�̲��Ž��� ����λ��
extern "C" BOOL CALL_TYPE IDVR_GetRemotePlayPos(HANDLE hPlay, float *pfPos)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRemotePlayPos error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	if (pPlay->bRemotePlayByTime)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRemotePlayPos error: Զ�̰�ʱ��طŲ�֧�ִ˹���"));
		return FALSE;
	}
	DWORD dwPos = 0;
	BOOL bRet = NET_DVR_PlayBackControl(pPlay->lPlayHandle, NET_DVR_PLAYGETPOS, 0, &dwPos);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRemotePlayPos failed: err code = %d"), NET_DVR_GetLastError());
	}
	*pfPos = dwPos/100.0f;
	return bRet;
}

//@���ò����ļ����� ���ž�� λ��
extern "C" BOOL CALL_TYPE IDVR_SetRemotePlayPos(HANDLE hPlay, float fPos)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetRemotePlayPos error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	if (pPlay->bRemotePlayByTime)
	{
		wsprintf(g_szLastError, _T("IDVR_SetRemotePlayPos error: Զ�̰�ʱ��طŲ�֧�ִ˹���"));
		return FALSE;
	}
	if (fPos < 0)
		fPos = 0.0f;
	if (fPos > 1.0f)
		fPos = 1.0f;
	DWORD dwPos = (DWORD)(fPos*100);
	BOOL bRet = NET_DVR_PlayBackControl(pPlay->lPlayHandle, NET_DVR_PLAYSETPOS, dwPos, NULL);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_SetRemotePlayPos failed: err code = %d"), NET_DVR_GetLastError());
	}
	return bRet;
}

//@�ط�¼���ļ�ʱץͼ ���ž�� �洢�ļ��� ��չ���� ��չ��������
extern "C" BOOL CALL_TYPE IDVR_RemotePlayCapturePicture(HANDLE hPlay, LPCSTR lpszFileName, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RemotePlayCapturePicture error: Invalid Handle"));
		return FALSE;
	}
	if (lpszFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RemotePlayCapturePicture error: file name is NULL"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hPlay;


	CString strFileName = CString(lpszFileName);
	
	BOOL bRet = FALSE;
	if (strFileName.Right(3).CompareNoCase(_T("bmp")) == 0)
	{
		//�ط�¼���ļ�ʱץͼ ���ž�� �洢�ļ���
		bRet = NET_DVR_PlayBackCaptureFile(pPlay->lPlayHandle, (char*)lpszFileName);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("IDVR_RemotePlayCapturePicture failed: err code = %d"), NET_DVR_GetLastError());
		}
	}
	else// if (strFileName.Right(3).CompareNoCase("jpg")) == 0)
	{
		PBYTE	pImage		= NULL;
		DWORD   nBufSize	= 704 * 576* 5;  // ��֤�㹻�󼴿�(����С��һ��bmp����jpegͼ���С)
		DWORD   pImageSize	= 0;
		try
		{
			pImage = new BYTE[nBufSize];
			if(NULL == pImage)
			{
				throw 0;
			}
			if (strFileName.Right(3).CompareNoCase(_T("jpg")) == 0)
			{
				int nIndex = NET_DVR_GetPlayBackPlayerIndex(pPlay->lPlayHandle);
				if (PlayM4_GetJPEG(nIndex, pImage, nBufSize, &pImageSize))
				{
					CFile picfile;
					if(picfile.Open(CString(lpszFileName), CFile::modeCreate|CFile::modeWrite))
					{
						picfile.Write(pImage, pImageSize);
						picfile.Close();
						bRet = TRUE;
					}
				}
			}
		}
		catch (CFileException* e)
		{
			//e->ReportError();
			e->Delete();
		}
		catch(...)
		{
		}

		if(pImage != NULL)
		{
			delete []pImage;
			pImage = NULL;
		}
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("IDVR_RemotePlayCapturePicture failed(jpg): ץͼʧ��"));
		}
	}

	return bRet;
}
//@Զ�̻ط�ʱ��������
extern "C" BOOL CALL_TYPE IDVR_RemotePlaySaveData(HANDLE hPlay, LPCSTR lpszRecFileName, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RemotePlaySaveData error: Invalid Handle"));
		return FALSE;
	}
	if (lpszRecFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RemotePlaySaveData error: file name is NULL"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hPlay;
	BOOL bRet = NET_DVR_PlayBackSaveData(pPlay->lPlayHandle, (char*)lpszRecFileName);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_RemotePlaySaveData failed: err code = %d"), NET_DVR_GetLastError());
	}
	return bRet;
}

//@Զ�̻ط�ʱֹͣ��������
extern "C" BOOL CALL_TYPE IDVR_StopRemotePlaySaveData(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopRemotePlaySaveData error: Invalid Handle"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hPlay;
	BOOL bRet = NET_DVR_StopPlayBackSave(pPlay->lPlayHandle);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_StopRemotePlaySaveData failed: err code = %d"), NET_DVR_GetLastError());
	}
	return bRet;
}

//�ص�����
extern "C" void CALLBACK RemotePlayCallBack(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,DWORD dwUser)
{
	if(dwUser != NULL)// && dwDataType == 2)
	{
		CRecPlay* pPlay = (CRecPlay*)dwUser;
		if(pPlay->pfnRealDataCallBack != NULL)
			pPlay->pfnRealDataCallBack((HANDLE)pPlay, pBuffer, dwBufSize,
										pPlay->dwCookie, dwDataType);
//		TRACE("RemotePlayCallBack: type = %d, data size=%d\n", dwDataType, dwBufSize);

		
		if (dwDataType == 1)
		{
			//����֡ͷ
			pPlay->pStreamHeader = new BYTE[dwBufSize];
			memcpy(pPlay->pStreamHeader, pBuffer, dwBufSize);
			pPlay->dwStreamheaderLen = dwBufSize;
			SetEvent(pPlay->hWaitHeaderEvent);
		}
	}
}

//����Զ�̻ط�ʱ�������ص�����
extern "C" BOOL CALL_TYPE IDVR_SetRemotePlayDataCallBack (HANDLE hPlay, DWORD dwCookie, LPCSTR pExParamIn, DWORD dwExParamInLen, RealDataCallBack callbackHandler)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetRemotePlayDataCallBack error: Invalid Handle"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hPlay;
	pPlay->dwCookie = dwCookie;
	pPlay->pfnRealDataCallBack = callbackHandler;

	pPlay->hWaitHeaderEvent = ::CreateEvent(NULL,FALSE, FALSE, NULL);

	BOOL bRet = NET_DVR_SetPlayDataCallBack(pPlay->lPlayHandle, RemotePlayCallBack, DWORD(pPlay));
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_SetRemotePlayDataCallBack failed: err code = %d"), NET_DVR_GetLastError());
	}

	if(WaitForSingleObject(pPlay->hWaitHeaderEvent,5000)==WAIT_TIMEOUT)
	{
		CloseHandle(pPlay->hWaitHeaderEvent);
		return FALSE;
	}
	
	return bRet;
}

//@��ȡ��ǰ�����ļ�����ʱ�� ���ž�� ��ʱ��
extern "C" BOOL CALL_TYPE IDVR_GetRemoteTotalTime(HANDLE hPlay, DWORD *pdwTotalTime)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRemoteTotalTime error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	DWORD dwTotalTime = 0;
	BOOL bRet = NET_DVR_PlayBackControl(pPlay->lPlayHandle, NET_DVR_GETTOTALTIME, 0, &dwTotalTime);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRemoteTotalTime failed: err code = %d"), NET_DVR_GetLastError());
	}
	*pdwTotalTime = dwTotalTime;
	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_GetRemotePlayedTime (HANDLE hPlay, DWORD *pdwPlayedTime)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRemotePlayedTime error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	DWORD dwPlayedTime = 0;
	BOOL bRet = NET_DVR_PlayBackControl(pPlay->lPlayHandle, NET_DVR_PLAYGETTIME, 0, &dwPlayedTime);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRemotePlayedTime failed: err code = %d"), NET_DVR_GetLastError());
	}
	*pdwPlayedTime = dwPlayedTime;
	return bRet;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_GetRemoteTotalFrames (HANDLE hPlay, DWORD *pdwTotalFrames)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRemoteTotalFrames error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	DWORD dwTotalFrames= 0;
	BOOL bRet = NET_DVR_PlayBackControl(pPlay->lPlayHandle, NET_DVR_GETTOTALFRAMES, 0, &dwTotalFrames);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRemoteTotalFrames failed: err code = %d"), NET_DVR_GetLastError());
	}
	*pdwTotalFrames = dwTotalFrames;

	return bRet;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_GetRemotePlayedFrames (HANDLE hPlay, DWORD *pdwPlayedFrames)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRemotePlayedFrames error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	DWORD dwPlayedFrames=0;
	BOOL bRet = NET_DVR_PlayBackControl(pPlay->lPlayHandle, NET_DVR_PLAYGETFRAME, 0, &dwPlayedFrames);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRemotePlayedFrames failed: err code = %d"), NET_DVR_GetLastError());
	}
	*pdwPlayedFrames = dwPlayedFrames;

	return bRet;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_GetPlayBackOsdTime(HANDLE hPlay, PDEVTIME pOsdTime)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetPlayBackOsdTime error: Invalid Handle"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hPlay;
	NET_DVR_TIME OsdTime;
	BOOL bRet = NET_DVR_GetPlayBackOsdTime(pPlay->lPlayHandle, &OsdTime);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_GetPlayBackOsdTime failed: err code = %d"), NET_DVR_GetLastError());
	}
	

	pOsdTime->ufromyear = OsdTime.dwYear;
	pOsdTime->ufrommonth = OsdTime.dwMonth;
	pOsdTime->ufromday = OsdTime.dwDay;
	pOsdTime->ufromhour = OsdTime.dwHour;
	pOsdTime->ufromminute = OsdTime.dwMinute;
	pOsdTime->ufromsecond = OsdTime.dwSecond;
	return bRet;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_PrepareStream(HANDLE hDevHandle,long lChannel,int sTreamtype)
{

	CRvu* pRvu = (CRvu*)hDevHandle;
	BOOL bRet = FALSE;

	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartPreview error: Invalid Dev Handle"));
		return FALSE;
	}

	if (sTreamtype == 0)
	{
		bRet = NET_DVR_MakeKeyFrame((long)pRvu->lRvuHandle,lChannel);
		if(!bRet)
		{
		   wsprintf(g_szLastError, _T("NET_DVR_MakeKeyFrame failed: err code = %d"), NET_DVR_GetLastError());
		}

	}
	else
	{
		bRet = NET_DVR_MakeKeyFrameSub((long)pRvu->lRvuHandle,lChannel);
        if(!bRet)
		{
			wsprintf(g_szLastError, _T("NET_DVR_MakeKeyFrameSub failed: err code = %d"), NET_DVR_GetLastError());
			
		}
	}

	return bRet;	

}

extern "C" BOOL  DFJK_API CALL_TYPE IDVR_ThrowBFrame(HANDLE hChanHandle,DWORD dwNum)
{
	if (hChanHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_PrepareStream error: Invalid Handle"));
		return FALSE;		
	}

	BOOL bRet = NET_DVR_ThrowBFrame((long)hChanHandle,(DWORD)dwNum);
	if(!bRet)
	{
		wsprintf(g_szLastError, _T("NET_DVR_ThrowBFrame failed: err code = %d"), NET_DVR_GetLastError());
	}

	return bRet;
}

//BOOL CALL_TYPE ISYS_CustomFunction(LPCSTR pParamIn, DWORD dwParamInLen, LPSTR *ppParamOut, DWORD* pdwParamOutLen)
//{
//	return FALSE;
//}

//�����Խ���֧�ֻص���
extern "C" BOOL CALL_TYPE IDVR_StartAudioPhone_EX(HANDLE hDevHandle, DWORD dwVoiceChan, BOOL bNeedCBNoEncData,CBF_VoiceData  cbVoiceData, void *pUser)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartAudioPhone_EX error: Invalid Dev Handle"));
		return FALSE;
	}
	if (pRvu->lVoiceComHandle != DFJK_INVALID_HANDLE)
		NET_DVR_StopVoiceCom(pRvu->lVoiceComHandle);

	LONG lRet = NET_DVR_StartVoiceCom_V30(pRvu->lRvuHandle,dwVoiceChan,bNeedCBNoEncData, cbVoiceData, pUser);
	if (lRet == DFJK_INVALID_HANDLE)
	{
		DWORD dwErr = NET_DVR_GetLastError();
		CString str;
		if (dwErr == NET_DVR_NOSUPPORT)
			str = _T("�豸��֧�ֶԽ�");
		else if (dwErr == NET_DVR_DVROPRATEFAILED || dwErr == NET_DVR_DVRVOICEOPENED)
			str = _T("�豸�������ܱ�ռ��");
		else
			str.Format(_T("Error code=%d"), dwErr);
		wsprintf(g_szLastError, _T("IDVR_StartAudioPhone_EX failed: %s"), str);
		return FALSE;
	}
	else
	{
		//������
		pRvu->lVoiceComHandle = lRet;
	}
	g_szLastError[0] = 0;
	return TRUE;
}

//�����㲥
extern "C" BOOL CALL_TYPE IDVR_StartAudioBroadcast(CBF_VoiceDataBroad cbVoiceData, void *pUser)
{
	if (!NET_DVR_ClientAudioStart_V30(cbVoiceData, pUser))
	{
		wsprintf(g_szLastError, _T("IDVR_StartAudioBroadcast failed, Error code=%d"), NET_DVR_GetLastError());
		return FALSE;
	}
	g_szLastError[0] = 0;
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_AudioBroadcast_AddDev(HANDLE hDevHandle, DWORD dwVoiceChan)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_AudioBroadcast_AddDev error: Invalid Dev Handle"));
		return FALSE;
	}
//	if (pRvu->lVoiceComHandle != DFJK_INVALID_HANDLE)
//		NET_DVR_StopVoiceCom(pRvu->lVoiceComHandle);

	pRvu->lVoiceComHandle = NET_DVR_AddDVR_V30(pRvu->lRvuHandle, dwVoiceChan);
	if (pRvu->lVoiceComHandle == -1)
	{
		DWORD dwErr = NET_DVR_GetLastError();
		CString str;
		if (dwErr == NET_DVR_NOSUPPORT)
			str = _T("�豸��֧�ֶԽ�");
		else if (dwErr == NET_DVR_DVROPRATEFAILED || dwErr == NET_DVR_DVRVOICEOPENED)
			str = _T("�豸�������ܱ�ռ��");
		else
			str.Format(_T("Error code=%d"), dwErr);
		wsprintf(g_szLastError, _T("IDVR_AudioBroadcast_AddDev failed: %s"), str);
		return FALSE;
	}
	g_szLastError[0] = 0;
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_AudioBroadcast_DelDev(HANDLE hDevHandle)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_AudioBroadcast_DelDev error: Invalid Dev Handle"));
		return FALSE;
	}
	BOOL bRet = NET_DVR_DelDVR_V30(pRvu->lVoiceComHandle);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_AudioBroadcast_DelDev failed, Error code=%d"), NET_DVR_GetLastError());
	}
	else
		g_szLastError[0] = 0;
	return bRet;	
}

extern "C" BOOL CALL_TYPE IDVR_StopAudioBroadcast()
{
	if (!NET_DVR_ClientAudioStop())
	{
		wsprintf(g_szLastError, _T("IDVR_StopAudioBroadcast failed, Error code=%d"), NET_DVR_GetLastError());
		return FALSE;
	}
	g_szLastError[0] = 0;
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_Decode(HANDLE hDevHandle, DWORD dwDecodeChan, BOOL bStart, LPCSTR lpszUrl)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_Decode error: Invalid Dev Handle"));
		return FALSE;
	}
	CString strUrl = CString(lpszUrl);
	if (dwDecodeChan == 0 || strUrl.IsEmpty())
	{
		wsprintf(g_szLastError, _T("IDVR_Decode error: Invalid param"));
		return FALSE;
	}

	BOOL bRet = FALSE;
	if(bStart)
	{
		NET_DVR_PU_STREAM_CFG dt;
		ZeroMemory(&dt, sizeof(dt));
		dt.dwSize = sizeof(NET_DVR_PU_STREAM_CFG);
		dt.struDevChanInfo.byTransMode = 0;		//0--��������1-������
		dt.struDevChanInfo.byTransProtocol = 0;	//0-tcp, 1-udp
		dt.struDevChanInfo.byFactoryType = 0;

		CString strTemp;
		gF_SplitFromUrl(lpszUrl, "ADDRESS", strTemp);
		strncpy(dt.struDevChanInfo.struIP.sIpV4, CString2string(strTemp).c_str(), 16);
		gF_SplitFromUrl(lpszUrl, "PORT", strTemp);
		dt.struDevChanInfo.wDVRPort = atoi(CString2string(strTemp).c_str());
		gF_SplitFromUrl(lpszUrl, "USERNAME", strTemp);
		strncpy((char *)dt.struDevChanInfo.sUserName, CString2string(strTemp).c_str(), 32);
		gF_SplitFromUrl(lpszUrl, "PASSWORD", strTemp);
		strncpy((char *)dt.struDevChanInfo.sPassword, CString2string(strTemp).c_str(), 16);
		gF_SplitFromUrl(lpszUrl, "CHANNEL", strTemp);
		dt.struDevChanInfo.byChannel = _wtoi(strTemp);

		gF_SplitFromUrl(lpszUrl, "CONN_PROXY_INFO", strTemp);
		int nPos = strTemp.Find(_T(":"), 0);
		if (nPos != -1)
		{
			strncpy(dt.struStreamMediaSvrCfg.struDevIP.sIpV4, CString2string(strTemp.Left(nPos)).c_str(), 16);

			dt.struStreamMediaSvrCfg.byValid = 1;
			dt.struStreamMediaSvrCfg.wDevPort = _wtoi(strTemp.Mid(nPos+1));

			gF_SplitFromUrl(lpszUrl, "CONN_TYPE", strTemp);
			int nConnType = _wtoi(strTemp);
			if (nConnType > 1)
				nConnType = 0;
			dt.struStreamMediaSvrCfg.byTransmitType = nConnType;	//0-tcp, 1-udp
		}

		//NET_DVR_MatrixStopDynamic(lUserID, 1);
		//Sleep(100);

		bRet = NET_DVR_MatrixStartDynamic_V30(pRvu->lRvuHandle, dwDecodeChan, &dt);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("NET_DVR_MatrixStartDynamic_V30 failed, Error code=%d"), NET_DVR_GetLastError());
		}
		else
			g_szLastError[0] = 0;
	}
	else
	{
		bRet = NET_DVR_MatrixStopDynamic(pRvu->lRvuHandle, dwDecodeChan);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("NET_DVR_MatrixStopDynamic failed, Error code=%d"), NET_DVR_GetLastError());
		}
		else
			g_szLastError[0] = 0;
	}

	return bRet;
}

//����ͻָ��豸���ò�����bGetorSet--FALSE����ȡ��TRUE�����棩
extern "C" BOOL CALL_TYPE IDVR_ConfigFile(HANDLE hDevHandle, BOOL bGetorSet, LPCSTR lpsFileName)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_ConfigFile error: Invalid Dev Handle"));
		return FALSE;
	}
	CString strFileName = CString(lpsFileName);
	if (strFileName.IsEmpty())
	{
		wsprintf(g_szLastError, _T("IDVR_ConfigFile error: Invalid file name"));
		return FALSE;
	}

	BOOL bRet = FALSE;
	if (bGetorSet)
		bRet =	NET_DVR_SetConfigFile(pRvu->lRvuHandle, (char*)lpsFileName);
	else
		bRet =	NET_DVR_GetConfigFile(pRvu->lRvuHandle, (char*)lpsFileName);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_ConfigFile failed, Error code=%d"), NET_DVR_GetLastError());
	}
	else
		g_szLastError[0] = 0;

	return bRet;
}
