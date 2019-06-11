// ComDev.h : header file
//

#if !defined(_COMDEV_)
#define _COMDEV_

//////////////////////////////////////////////////////////////////////////
//�궨��
#define		COMBUFFER_LEN				1024			//���ڽ������ݻ�������С
#define		MAX_DATA_NUM				32				//��ౣ���������
#define		MAX_QUERYDATA				100				//���ѯ���ݳ���

/////////////////////////////////////////////////////////////////////////////
// CComDevice

class CComDevice
{
protected:
	BYTE	LPC( BYTE* pbyBuf,WORD nLen );
	void	NeatenRxdBuff();
	virtual void SeartchFrame(){};
	BYTE	CRC16Hi( BYTE * puchMsg, short usDataLen);
	BYTE	CRC16Lo( BYTE * puchMsg, short usDataLen);

public:
	CComDevice();
	virtual ~CComDevice();

	bool	Init(HWND hMainWnd, BYTE byCommNo, DWORD dwBaud, BYTE byDataBit = 8,
				BYTE byParity = NOPARITY, BYTE byStopbit = ONESTOPBIT);
	void	Deinit();

	void	AddDataToComBuffer(BYTE* pBuf, int nLen);

	//��ȡ�������ڵ����ݣ�һ��������ȡ��ŵ��������ڣ���nIndex��1��ʼ
	BOOL GetIntValue(int nIndex, int& nVal)
	{
		if(nIndex > 0 &&  nIndex <= m_nDataChanCount_Int && nIndex <= MAX_DATA_NUM)
		{
			nVal = m_nDataBufferInt[nIndex-1];
			return TRUE;
		}
		return FALSE;
	}

	BOOL GetFloatValue(int nIndex, float& fVal)
	{
		if(nIndex > 0 &&  nIndex <= m_nDataChanCount_Float && nIndex <= MAX_DATA_NUM)
		{
			fVal = m_fDataBufferFloat[nIndex-1];
			return TRUE;
		}
		return FALSE;
	}

	int GetDataChanCount_Int()
	{
		return m_nDataChanCount_Int;
	}
	int GetDataChanCount_Float()
	{
		return m_nDataChanCount_Float;
	}

	void SetTimeout(DWORD dwTimeout)
	{
		m_dwTimeout = dwTimeout;
	}
	DWORD GetTimeout()
	{
		return m_dwTimeout;
	}

	//��ȡ��ѯ����
	virtual BOOL GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen){return FALSE;};

	//ͨ�����ڶ�ȡ���ݣ�nChanNo = 0��������ȡ��1��n����ȡ�������ݣ�
	virtual BOOL ReadIntData(int nChanNo, int& nVal, CString& strAllVal){return FALSE;};
	virtual BOOL ReadFloatData(int nChanNo, float& fVal, CString& strAllVal){return FALSE;};

protected:
	int		m_nWritePointer;					//���ڽ������ݻ�����дָ��(0��COMBUFFER_LEN-1)
	int		m_nReadPointer;						//���ڽ������ݻ�������ָ��(0��COMBUFFER_LEN-1)
	BYTE	m_szComBuffer[COMBUFFER_LEN];		//���ڽ������ݻ�����

	BYTE	m_byMinFrameLen;					//��������
	int		m_nDataChanCount_Int;				//�豸����ֵͨ����
	int		m_nDataChanCount_Float;				//�豸����ֵͨ����

	HANDLE	m_hEventGetDataOK;					//�Ƿ�����ȷ������

	int		m_nDataBufferInt[MAX_DATA_NUM];
	float	m_fDataBufferFloat[MAX_DATA_NUM];

	DWORD	m_dwTimeout;						//��ȡ��ʱ����λΪms
};

/////////////////////////////////////////////////////////////////////////////
// CComDeviceWS302
class CComDeviceWS302 : public CComDevice
{
protected:
	float m_fWendu;
	float m_fShidu;

	virtual void SeartchFrame();

public:
	CComDeviceWS302();
	virtual ~CComDeviceWS302();

	//��ȡ��ѯ֡����
	virtual BOOL GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen);

	//ͨ�����ڶ�ȡ���ݣ�nChanNo = 0��������ȡ��1��n����ȡ�������ݣ�
	virtual BOOL ReadIntData(int nChanNo, int& nVal, CString& strAllVal);
	virtual BOOL ReadFloatData(int nChanNo, float& fVal, CString& strAllVal);

	float GetWendu(){return m_fWendu;}
	float GetShidu(){return m_fShidu;}

protected:
	void ProcessData(BYTE nAddr, BYTE c5, BYTE c6, BYTE c7, BYTE c8, BYTE c9, BYTE c10);

};

/////////////////////////////////////////////////////////////////////////////
// CComDeviceJWSL
class CComDeviceJWSL : public CComDevice
{
protected:
	float m_fWendu;
	float m_fShidu;

	virtual void SeartchFrame();
	void ProcessData(BYTE* pData);

public:
	CComDeviceJWSL();
	virtual ~CComDeviceJWSL();

	//��ȡ��ѯ֡����
	virtual BOOL GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen);

	//ͨ�����ڶ�ȡ���ݣ�nChanNo = 0��������ȡ��1��n����ȡ�������ݣ�
	virtual BOOL ReadIntData(int nChanNo, int& nVal, CString& strAllVal);
	virtual BOOL ReadFloatData(int nChanNo, float& fVal, CString& strAllVal);

	float GetWendu(){return m_fWendu;}
	float GetShidu(){return m_fShidu;}
};

/////////////////////////////////////////////////////////////////////////////
// CComDeviceLTM8000
class CComDeviceLTM8000 : public CComDevice
{
protected:
	float m_fWendu;
	float m_fShidu;

	virtual void SeartchFrame();

public:
	CComDeviceLTM8000();
	virtual ~CComDeviceLTM8000();

	//��ȡ��ѯ֡����
	virtual BOOL GetQueryData(int nAddr, int nType, BYTE* buff, int& nBuffLen);
	
	//ͨ�����ڶ�ȡ���ݣ�nChanNo = 0��������ȡ��1��n����ȡ�������ݣ�
	virtual BOOL ReadIntData(int nChanNo, int& nVal, CString& strAllVal);
	virtual BOOL ReadFloatData(int nChanNo, float& fVal, CString& strAllVal);

	float GetWendu(){return m_fWendu;}
	float GetShidu(){return m_fShidu;}

protected:
	BYTE HexStrToVal(BYTE by1, BYTE by2);
	BYTE CheckSum( BYTE* pbyBuf,WORD nLen );
	void ProcessData(BYTE* pData);
};

#endif
