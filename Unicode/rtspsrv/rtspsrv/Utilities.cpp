#include"stdafx.h"
#include "Utilities.h"
//CLastError* CLastError::m_pInstance =NULL;
#include "request.hpp"
namespace RTSP {
	namespace RTSPSrv {
		std::string headers_to_string(const std::vector<header>& headers)
		{	
			std::stringstream ss;
			for (std::size_t i = 0; i < headers.size(); i++)
			{
				const header& h = headers[i];
				ss <<h.name<<": "<<h.value<<std::endl;				
			}		 
			ss.flush();
			return ss.str();
		}

		//�ַ�����ʽ������
		std::string format(const char *fmt,...) 
		{ 
			std::string strResult="";
			if (NULL != fmt)
			{
				va_list marker = NULL;            
				va_start(marker, fmt);                            //��ʼ���������� 
				size_t nLength = _vscprintf(fmt, marker) + 1;    //��ȡ��ʽ���ַ�������
				std::vector<char> vBuffer(nLength, '\0');        //�������ڴ洢��ʽ���ַ������ַ�����
				int nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, fmt, marker);
				if (nWritten>0)
				{
					strResult = &vBuffer[0];
				}            
				va_end(marker);                                    //���ñ�������
			}
			return strResult; 
		}

		//�ַ�����ʽ������
		std::wstring format(const wchar_t *fmt,...) 
		{ 
			std::wstring strResult=L"";
			if (NULL != fmt)
			{
				va_list marker = NULL;            
				va_start(marker, fmt);                            //��ʼ����������
				size_t nLength = _vscwprintf(fmt, marker) + 1;    //��ȡ��ʽ���ַ�������
				std::vector<wchar_t> vBuffer(nLength, L'\0');    //�������ڴ洢��ʽ���ַ������ַ�����
				int nWritten = _vsnwprintf_s(&vBuffer[0], vBuffer.size(), nLength, fmt, marker); 
				if (nWritten > 0)
				{
					strResult = &vBuffer[0];
				}
				va_end(marker);                                    //���ñ�������
			}
			return strResult; 
		} 
	}
}