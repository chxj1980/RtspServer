// ComDev.cpp : implementation file
//

#include "stdafx.h"
#include "Plugins.h"
#include "ComDev.h"

/////////////////////////////////////////////////////////////////////////////
// CComDevice

CComDevice::CComDevice()
{
	m_nWritePointer = 0;
	m_nReadPointer = 0;
	m_byDeviceType = 0;
	m_hEventGetDataOK = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CComDevice::~CComDevice()
{
	if (m_hEventGetDataOK)
	{
		CloseHandle(m_hEventGetDataOK);
		m_hEventGetDataOK = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CComDevice message handlers

bool CComDevice::Init(HWND hMainWnd, BYTE byCommNo, DWORD dwBaud, BYTE byDataBit, BYTE byParity, BYTE byStopbit)
{
	return true;
}

void CComDevice::Deinit()
{
}

//�򴮿ڻ��������������
void CComDevice::AddDataToComBuffer(BYTE* pBuf, int nLen)
{
	if (COMBUFFER_LEN - m_nWritePointer >= nLen)
	{
		memcpy(m_szComBuffer + m_nWritePointer, pBuf, nLen);
		m_nWritePointer += nLen;
	}
	else	//��������д����
	{
		memcpy(m_szComBuffer + m_nWritePointer, pBuf, COMBUFFER_LEN - m_nWritePointer);
		m_nWritePointer = COMBUFFER_LEN - 1;
	}

	if( m_nWritePointer - m_nReadPointer >= m_byMinFrameLen )		//�յ����ֽ�������һ����( �������֡�� )
	{
		//��������
		SeartchFrame();
	}
}

//������ջ��������ж��Ƿ�Խ���
void CComDevice::NeatenRxdBuff()
{
	int i,j;

	if( m_nReadPointer == 0 )
		return;
	
	//�ݴ���
	if( ( m_nReadPointer >= m_nWritePointer ) || 	//��ָ��>=дָ��Ϊ����
		( m_nWritePointer > COMBUFFER_LEN -1 ))	//дָ��>��������С
	{
		m_nReadPointer = m_nWritePointer = 0;
		return;
    }

	//��ǰ�ƶ�����(����ָ�뵽дָ��������Ƶ�������ͷ��)
	i = 0;
	j = m_nReadPointer;

	while( j < m_nWritePointer )
		m_szComBuffer[i++] = m_szComBuffer[j++];

	m_nReadPointer  = 0;	//���ö�ָ��
	m_nWritePointer = i;	//����дָ��

	return;
}

//���LPCУ����(�ۼӺ�)
BYTE CComDevice::LPC( BYTE* pbyBuf,WORD nLen )
{
	BYTE byResult = 0;

	for( WORD i=0; i<nLen; i++ )
		byResult += pbyBuf[i];	//�ۼӺ�
	return byResult;
}

//////////////////////////////////////////////////////////////////////
// CComDeviceWS302 message handlers
CComDeviceWS302::CComDeviceWS302()
{
	m_byMinFrameLen = 13;
	m_fWendu = 0.0f;
	m_fShidu = 0.0f;

	m_nDataChanCount_Int = 0;
	m_nDataChanCount_Float = 2;
}

CComDeviceWS302::~CComDeviceWS302()
{

}

//�ӻ������в���֡(��WS302Э��)
void CComDeviceWS302::SeartchFrame()
{

LOOP:	NeatenRxdBuff();

//	TRACE("---------pRead=%d,pWrite=%d\n",m_nReadPointer,m_nWritePointer);

	int nTmpPtr = m_nReadPointer;
	int nWriPtr = m_nWritePointer;

	//�Ƿ�С����С֡��(13���ֽ�)?
	if( (nWriPtr - nTmpPtr) < m_byMinFrameLen )	return;

	while( m_szComBuffer[nTmpPtr] != 0x55)
	{
		nTmpPtr++;
		if( nTmpPtr >= nWriPtr )
		{
			//��������������
			m_nReadPointer = nTmpPtr;
			return;
		}
	}

	//�ٴμ�����ݳ����Ƿ�С����С֡��?
	if( (nWriPtr - nTmpPtr) < m_byMinFrameLen )
	{
		m_nReadPointer = nTmpPtr;
		return;
	}

	int nDataLen = m_szComBuffer[nTmpPtr + 4];			//֡�еĵ�5���ֽ������ݳ���
	int nFrameLen = m_byMinFrameLen;
	//������ȷ��֡��
	if ( (nWriPtr - nTmpPtr) < nFrameLen )
	{
		m_nReadPointer = nTmpPtr;
		return;
	}

	//ȷ��֡β��У��λ
	if (m_szComBuffer[nTmpPtr + nFrameLen - 1] == 0xAA &&
		m_szComBuffer[nTmpPtr + nFrameLen - 2] == LPC(m_szComBuffer + nTmpPtr + 1, nFrameLen - 3))
	{
		//�õ�����ȷ��֡!
		TRACE(" ---- WS302: Received a OK frame!\n");

		ProcessData(m_szComBuffer[nTmpPtr+1], m_szComBuffer[nTmpPtr+5], m_szComBuffer[nTmpPtr+6], m_szComBuffer[nTmpPtr+7],
			m_szComBuffer[nTmpPtr+8],m_szComBuffer[nTmpPtr+9],m_szComBuffer[nTmpPtr+10]);

		//���ݴ������
		SetEvent(m_hEventGetDataOK);
	}
	else	//У��ʧ�ܣ�������һ�����ݣ�����
	{
		nFrameLen = 1;
	}


	//���ö�ָ��
	m_nReadPointer = nTmpPtr + nFrameLen;

	//������������
	goto LOOP;

}

void CComDeviceWS302::ProcessData(BYTE nAddr, BYTE c5, BYTE c6, BYTE c7, BYTE c8, BYTE c9, BYTE c10)
{
	BYTE mask;
	mask = 0x80;
	float fVal = 0;
	if(c5 & mask) //�¶ȸ���
	{
		WORD val;
		val = MAKEWORD(c6,c5);
		val = ~val+1;
		fVal = -(val /10.0f);
	}
	else               //�¶�����
	{
		WORD val;
		val = MAKEWORD(c6,c5);
		fVal = val /10.0f;
	}
	m_fWendu = fVal;

	mask = 0x80;
	if(c7 & mask) //ʪ�ȸ���
	{
		WORD val;
		val = MAKEWORD(c8,c7);
		val = ~val+1;
		fVal = -(val /10.0f);
	}
	else              //ʪ������
	{
		WORD val;
		val = MAKEWORD(c8,c7);
		fVal = val /10.0f;
	}
	m_fShidu = fVal;

//	g_WS302[nIndex].byMemFault = (c9 & 0x20) ? 1 : 0;		//�洢������
//	g_WS302[nIndex].byShiduFault = (c9 & 0x40) ? 1 : 0;		//ʪ�Ȳ�������
//	g_WS302[nIndex].byWenduFault = (c9 & 0x80) ? 1 : 0;		//�¶Ȳ�������
}

//��ȡ��ѯ֡����
BOOL CComDeviceWS302::GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen)
{
	if (buff == NULL || nBuffLen < 8)	// WS302��ѯ֡����Ϊ8
	{
		return FALSE;
	}

	buff[0] = 0x55;
	buff[1] = (BYTE)nAddr;
	buff[2] = 0x00;
	buff[3] = 0x01;
	buff[4] = 0x01;
	buff[5] = 0x00;
	buff[6] = 0x00;
	for (int i=1 ; i<6 ; i++)
		buff[6] += buff[i];
	buff[6]  = buff[6] & 0xFF;
	buff[7]	= 0xAA;

	nBuffLen = 8;

	return TRUE;
}

BOOL CComDeviceWS302::ReadIntData(int nChanNo, int& nVal, CString& strAllVal)
{
	return FALSE;
}

BOOL CComDeviceWS302::ReadFloatData(int nChanNo, float& fVal, CString& strAllVal)
{
	if (WaitForSingleObject(m_hEventGetDataOK, 500) == WAIT_TIMEOUT)	//���ȴ�500ms
	{
		//sprintf(g_szLastError, "IDVR_GetIOValue: ��ʱ!");
		return FALSE;
	}

	//��ȡ�ɹ���
	if (nChanNo == 1)
		fVal = m_fWendu;
	else if(nChanNo == 2)
		fVal = m_fShidu;
	else if(nChanNo == 0)
		strAllVal.Format(_T("COUNT=2;VAL=%.2f,%.2f,;"), m_fWendu, m_fShidu);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CComDeviceJWSL message handlers
CComDeviceJWSL::CComDeviceJWSL()
{
	m_byMinFrameLen = 0;
	//m_byDeviceType = DVSCOMDEVTYPE_JWSL;
	m_nDataChanCount_Int = 0;
	m_nDataChanCount_Float = 2;
}

CComDeviceJWSL::~CComDeviceJWSL()
{
}

//�ӻ������в���֡
void CComDeviceJWSL::SeartchFrame()
{

}

BOOL CComDeviceJWSL::ReadIntData(int nChanNo, int& nVal, CString& strAllVal)
{
	//���豸������ֵ���ṩ
	return FALSE;
}

BOOL CComDeviceJWSL::ReadFloatData(int nChanNo, float& fVal, CString& strAllVal)
{
	return FALSE;
}

//��ȡ��ѯ֡����
BOOL CComDeviceJWSL::GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen)
{
	if (buff == NULL || nBuffLen < 8)	// JWSL��ѯ֡����Ϊ8
	{
		return FALSE;
	}

// 	buff[0] = 0x55;
// 	buff[1] = (BYTE)nAddr;
// 	buff[2] = 0x00;
// 	buff[3] = 0x01;
// 	buff[4] = 0x01;
// 	buff[5] = 0x00;
// 	buff[6] = 0x00;
// 	for (int i=1 ; i<6 ; i++) 
// 		buff[6] += buff[i];
// 	buff[6]  = buff[6] & 0xFF;
// 	buff[7]	= 0xAA;
// 	
// 	nBuffLen = 8;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CComDeviceLTM8000 message handlers
CComDeviceLTM8000::CComDeviceLTM8000()
{
	m_byMinFrameLen = 23;
	m_fWendu = 0.0f;
	m_fShidu = 0.0f;

	m_nDataChanCount_Int = 0;
	m_nDataChanCount_Float = 2;
}

CComDeviceLTM8000::~CComDeviceLTM8000()
{

}


BYTE CComDeviceLTM8000::HexStrToVal(BYTE by1, BYTE by2)
{
	BYTE low=0, high=0;
	if (by1 >= 0x30 && by1 <= 0x39)		//0��9
	{
		high = by1-48;
	}
	else if(by1 >= 0x41 && by1 <= 0x46)	//��дA��B
	{
		high = by1-55;
	}
	else if(by1 >= 0x61 && by1 <= 0x66)	//Сдa��b
	{
		high = by1-87;
	}
	
	if (by2 >= 0x30 && by2 <= 0x39)		//0��9
	{
		low = by2-48;
	}
	else if(by2 >= 0x41 && by2 <= 0x46)	//��дA��B
	{
		low = by2-55;
	}
	else if(by2 >= 0x61 && by2 <= 0x66)	//Сдa��b
	{
		low = by2-87;
	}
	
	return (high*16 + low);
}


//���У����(�ۼӺͣ�ȡ����+1)
BYTE CComDeviceLTM8000::CheckSum( BYTE* pbyBuf,WORD nLen )
{
	ASSERT(nLen == 18);
	BYTE temp=0, last=0;
	for (int i=0; i<9; i++)
	{
		temp = HexStrToVal(pbyBuf[2*i], pbyBuf[2*i+1]);
		last += temp;
	}
	last = ~last;
	last += 1;
	return last;
}

//�ӻ������в���֡(��LTM8000Э��)
void CComDeviceLTM8000::SeartchFrame()
{

LOOP:	NeatenRxdBuff();

//	TRACE("---------pRead=%d,pWrite=%d\n",m_nReadPointer,m_nWritePointer);

	int nTmpPtr = m_nReadPointer;
	int nWriPtr = m_nWritePointer;

	//�Ƿ�С����С֡��?
	if( (nWriPtr - nTmpPtr) < m_byMinFrameLen )	return;

	while( m_szComBuffer[nTmpPtr] != 0x3A ||
		m_szComBuffer[21] != 0x0D ||
		m_szComBuffer[22] != 0x0A)
	{
		nTmpPtr++;
		if( nTmpPtr >= nWriPtr )
		{
			//��������������
			m_nReadPointer = nTmpPtr;
			return;
		}
	}

	int nFrameLen = 0;
	
	//�ٴ�ȷ��֡��
	if( (nWriPtr - nTmpPtr) < m_byMinFrameLen )
		return;

	//У��
	if (HexStrToVal(m_szComBuffer[nTmpPtr+19], m_szComBuffer[nTmpPtr+20]) == CheckSum(m_szComBuffer+nTmpPtr+1, 18))
	{
		nFrameLen = m_byMinFrameLen;
		TRACE(" ---- LTM8000: Received a OK frame!\n");

		ProcessData(m_szComBuffer+nTmpPtr);

		//���ݴ������
		SetEvent(m_hEventGetDataOK);
	}
	else	//У��ʧ�ܣ�������һ�����ݣ�����
	{
		nFrameLen = 1;
	}

	//���ö�ָ��
	m_nReadPointer = nTmpPtr + nFrameLen;

	//������������
	goto LOOP;

}

void CComDeviceLTM8000::ProcessData(BYTE* pData)
{
	if (pData == NULL)
		return;

	//��ȡ��ʪ��ֵ
	BYTE TH = HexStrToVal(pData[11], pData[12]);
	BYTE TL = HexStrToVal(pData[13], pData[14]);
	if ((TH & 0x08) == 0)
	{
		m_fWendu = ((TH & 0x07)*256+TL)*0.0625f;		//���¶�
	}
	else
	{
		m_fWendu = -((TH & 0x07)*256+TL)*0.0625f;		//���¶�
	}
	
	BYTE HH = HexStrToVal(pData[15], pData[16]);
	BYTE HL = HexStrToVal(pData[17], pData[18]);
	m_fShidu = (HH * 256 + HL)/ 10.0f;
}

//��ȡ��ѯ֡����
BOOL CComDeviceLTM8000::GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen)
{
	if (buff == NULL || nBuffLen < 8)	// ��ѯ֡����Ϊ17
	{
		return FALSE;
	}

	//֡ͷ
	buff[0] = 0x3A;	//�ֺ�(:)

	char s[2];
	sprintf(s, "%02X", nAddr);
	buff[1] = s[0];
	buff[2] = s[1];

	buff[3] = 0x30;
	buff[4] = 0x34;
	buff[5] = 0x30;
	buff[6] = 0x30;
	buff[7] = 0x38;
	buff[8] = 0x30;
	buff[9] = 0x30;
	buff[10] = 0x30;
	buff[11] = 0x30;
	buff[12] = 0x33;
	
	//0x87 Ϊ byData[3] �� byData[12] ��ASCII��֮�ۼӺͣ������ֽ���һ��16��������
	BYTE CheckSum = ~(0x87 + nAddr) + 1;
	sprintf(s, "%02X", CheckSum);
	buff[13] = s[0];
	buff[14] = s[1];

	//֡β
	buff[15] = 0x0D;
	buff[16] = 0x0A;

	nBuffLen = 17;

	return TRUE;
}

BOOL CComDeviceLTM8000::ReadIntData(int nChanNo, int& nVal, CString& strAllVal)
{
	return FALSE;
}

BOOL CComDeviceLTM8000::ReadFloatData(int nChanNo, float& fVal, CString& strAllVal)
{
	if (WaitForSingleObject(m_hEventGetDataOK, 500) == WAIT_TIMEOUT)	//���ȴ�500ms
	{
		//sprintf(g_szLastError, "IDVR_GetIOValue: ��ʱ!");
		return FALSE;
	}

	//��ȡ�ɹ���
	if (nChanNo == 1)
		fVal = m_fWendu;
	else if(nChanNo == 2)
		fVal = m_fShidu;
	else if(nChanNo == 0)
		strAllVal.Format(_T("COUNT=2;VAL=%.2f,%.2f,;"), m_fWendu, m_fShidu);

	return TRUE;
}
