
#include "stdafx.h"
#include "Request_Preprocess.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp> 
#include <boost/algorithm/string/classification.hpp>
#include <boost/regex.hpp>
#include "SessionManager.h"
#include "VideoSourceFactory.h"
#include "VideoManager.h"
#include "xlog.h"
#include "Prototypes.h"

using namespace std;
using namespace boost::algorithm; 

namespace RTSP {
	namespace RTSPSrv {

		const static commandMapItem commandMap[] =
		{
			{ methodSetup, "SETUP" },
			{ methodDescribe, "DESCRIBE" },
			{ methodAnnounce, "ANNOUNCE" },
			{ methodPlay, "PLAY" },
			{ methodTeardown, "TEARDOWN" },
			{ methodOptions, "OPTIONS" },
			{ methodGet, "GET" },
			{ methodPost, "POST" }
		};

// 		static BYTE pbHKHead[] = {	0x34,0x48,0x4b,0x48,0xfe,0xb3,0xd0,0xd6,0x08,0x03,0x04,0x20,0x00,0x00,0x00,0x00,
// 			0x01,0x10,0x01,0x10,0x01,0x10,0x10,0x00,0x80,0x3e,0x00,0x00,0x60,0x01,0x20,0x01,
// 			0x11,0x10,0x00,0x00,0x0e,0x00,0x00,0x00};

		static std::string& trim(std::string &s, const std::string& sub)
		{
			if (s.empty())		return s;

			s.erase(0, s.find_first_not_of(sub));
			s.erase(s.find_last_not_of(sub) + 1);
			return s;
		}

		Request_Preprocess::Request_Preprocess()
		{
		}

		CommandType Request_Preprocess::getCommandType(const request& req)
		{
			for (int i = 0; i<DIM(commandMap); i++)
			{
				if (req.method == commandMap[i].method)
					return commandMap[i].methodId;
			}
			return methodUnknown;
		}

		std::string Request_Preprocess::getHeadersItem(const request& req, const std::string& name)
		{
			for (std::size_t i = 0; i < req.headers.size(); ++i)
			{
				const header& h = req.headers[i];
				if (h.name == name)
					return h.value;
			}
			return std::string("");
		}

		bool Request_Preprocess::GetTransportInfo(RTSPSessionItem &item, const std::string &clientport, reply &rep)
		{
			std::string pattern = "RTP/AVP/(TCP|UDP);(destination=([\\d\\.]+);){0,1}unicast;client_port=(\\d{1,5})\\-(\\d{1,5})";
			//std::string pattern = "RTP/AVP/(TCP|UDP);destination=([\\d\\.]+);unicast;client_port=(\\d{1,5})\\-(\\d{1,5})";
			boost::regex expression(pattern);
			boost::cmatch what;

			item.conntype = TT_TCP;

			if (regex_match(clientport.c_str(), what, expression))
			{
				/*for(unsigned int i=0;i<what.size();i++)
				std::cout<<"����:"<<i<<" "<<what[i].str()<<std::endl;*/
				if (what[1].str() == "UDP")
					item.conntype = TT_UDP;
				try	{
					if (!what[3].str().empty())
						item.recvip = what[3].str();

					//clientport
					item.clientport = boost::lexical_cast<int>(what[4].str());
				}
				catch (const boost::bad_lexical_cast& e)
				{
					UNREFERENCED_PARAMETER(e);
					rep.content = "�����ͨѶ����,����ָ��ͨѶЭ��Ͷ˿�";
					return false;
				}
			}
			else
			{
				rep.content = "�����ͨѶ����,����ָ��ͨѶЭ��Ͷ˿�";
				return false;
			}
			return true;
		}

		bool Request_Preprocess::getUriInfo(const string& uri, RTSPSessionItem& session)
		{
			//rtsp://172.20.43.49:554/172.20.43.41:8000:HIK-DS8000HC:0:0:admin:12345/av_stream/real RTSP/1.0
			//TODO:�ط�¼���ļ�
			//rtsp://172.20.43.49:554/172.20.43.41:8000:HIK-DS8000HC:0:0:admin:12345/av_stream/file/e:\20090103100711-20090103235959-172.20.43.41-03-jans.mp4 RTSP/1.0
			//TODO:�ط�Ӳ��¼���¼���ļ�
			//rtsp://172.20.43.49:554/172.20.43.41:8000:HIK-DS8000HC:0:0:admin:12345/av_stream/vod/20090103100711-20090103235959.mp4 RTSP/1.0	
			std::size_t rtsp_pos = uri.find("rtsp://");

			if (rtsp_pos == string::npos)	return false;

			//�������̷���֧��,�޸���DVRIP��ַ������,֧����ͨ���ַ�������
			//	std::string pattern = "rtsp://(([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}(:\\d{1,5}){0,1}/)*)([\\w\\d.\\(\\)]+):([0-9]{1,5}):([\\w\\d-]{1,}):(\\d{1,5}):(\\d{1,5}):([\\w\\d]{1,10}):([\\w\\d]{1,10})/av_stream((/real)|(/file/([\\w&:\\\\\\-\\d.]{1,})))";				
			std::string pattern = "rtsp://(([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}(:\\d{1,5}){0,1}/)*)([\\w\\d.\\(\\)]+):([0-9]{1,5}):([\\w\\d-]{1,}):(\\d{1,5}):(\\d{1,5}):([\\w\\d]{1,10}):([\\w\\d]{1,15})/av_stream((/real)|(/file))";
			boost::regex expression(pattern);
			boost::cmatch what;
			if (boost::regex_match(uri.c_str(), what, expression))
			{
				//for(int i=0;i<what.size();i++)
				//	std::cout<<"����:"<<i<<" "<<what[i].str()<<std::endl;
				try	{
					//���ֻ��һ�����򲻽���ת��
					if (what[1].str().empty() || what[1].str().size() <= what[2].str().size())
						session.proxy.clear();
					else
					{
						//std::cout<<"ת��·��1��"<<what[1].str()<<std::endl;
						size_t pos = what[1].str().find_first_of("/");
						session.proxy = what[1].str().substr(pos + 1);
						trim(session.proxy, "/");
						//std::cout<<"ת��·��2��"<<session.proxy<<std::endl;
					}

					session.dvrip = what[4].str();
					session.port = boost::lexical_cast<unsigned short>(what[5].str());
					session.devtype = what[6].str();
					session.channel = boost::lexical_cast<short>(what[7].str());
					session.streamtype = boost::lexical_cast<short>(what[8].str());
					session.username = what[9].str();
					session.password = what[10].str();
					session.file = what[14].str();

					return true;
				}
				catch (const boost::bad_lexical_cast& e)
				{
					UNREFERENCED_PARAMETER(e);
					X_LOG1(XLOG_ERROR, _T("������������:%s"), CString(uri.c_str()));
					return false;
				}
			}
			else
				X_LOG1(XLOG_DEBUG, _T("�Ƿ������ʽ:%s"), CString(uri.c_str()));

			return false;
		}

		bool Request_Preprocess::process(const request& req, reply& rep)
		{
			string  strSquence = getHeadersItem(req,"CSeq");
			int sequence = -1;
			if(!strSquence.empty())
				sequence = boost::lexical_cast<int>(strSquence);
			string  request_type = getHeadersItem(req,"File");

			if(request_type.empty())	request_type = "real";			
			else						request_type = "file/"+request_type;

			switch(getCommandType(req))
			{
			case methodDescribe:
				{
					string sdp = 
						"v=0\r\n" 
//						"o=StreamingServer 3443478079 1109641162000 IN IP4 172.20.48.240\r\n"  
						"s=Dongfang Electric stream session\r\n"  
						"u=rtsp://www.dongfang-china.com\r\n"  
						"e=webscada@163.com\r\n"  
						"c=IN IP4 "+req.server_addr.to_string()+"\r\n"  
						"b=AS:94\r\n"  
						"t=0 0\r\n"  
						"a=control:*\r\n"  
						"a=range:npt=0-  70.00000\r\n"  
						"m=video 0 RTP/AVP 96\r\n"  
						"b=AS:79\r\n"  
						"a=rtpmap:96 X-SV3V-ES/90000\r\n"
						"a=control:"+request_type+"\r\n"
						"a=x-bufferdelay:4.97\r\n";			

					//���ûظ����
					rep.status = reply::ok;
					rep.content = sdp;					
					rep.headers.resize(6);
					rep.headers[0].name="CSeq";
					rep.headers[0].value= boost::lexical_cast<string>(sequence);	
					rep.headers[1].name="Cache-Control";
					rep.headers[1].value="must-revalidate";
					rep.headers[2].name="Content-length";
					rep.headers[2].value=boost::lexical_cast<string>((DWORD)sdp.size());	
					rep.headers[3].name="Content-Type";
					rep.headers[3].value="application/sdp";
					rep.headers[4].name="x-Accept-Retransmit";
					rep.headers[4].value="our-retransmit";
					rep.headers[5].name="x-Accept-Dynamic-Rate";
					rep.headers[5].value="1";
					//rep.headers[6].name="Content-Base";
					//rep.headers[6].value="rtsp://172.20.48.238/1.mov/";					
					return true;
				}
				break;
			case methodSetup:
				{
					RTSPSessionManager* pMgr = RTSPSessionManagerSingleton::instance();
					RTSPSessionItem item;					
					item.file.clear();
					item.proxy.clear();
					item.hSession = INVALID_DEV_HANDLE;					
					item.clientip = req.peer.address().to_string();		

					if(!pMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "�û��Ự��������ʼ���쳣";
						X_LOG0(XLOG_ERROR,_T("�û��Ự��������ʼ������."));						
						return true;
					}
					
					if(!getUriInfo(req.uri,item))		return false;
					
					//ͨ������1��ʼ
					if(item.channel<1)
					{
						rep.content = "�����ͨ����,ͨ������1��ʼ��";
						return false;
					}

					item.dwSeqenceNumber = sequence;					
					item.sessionid = pMgr->createSessionId();					

					//���ûظ����
					rep.status = reply::ok;			
					rep.content.clear();
					rep.headers.resize(3);
					rep.headers[0].name="CSeq";
					rep.headers[0].value= boost::lexical_cast<string>(sequence);					
					rep.headers[1].name="Session";
					rep.headers[1].value=boost::lexical_cast<string>(item.sessionid);
					rep.headers[2].name="Transport";

					string clientport = getHeadersItem(req,"Transport");		

					if(!GetTransportInfo(item, clientport, rep))
						return true;//��ֹ����400 Bad Request����

					VideoSourceItem vitem;
					
					//��ȡ��
					if(!GetVideoStream(vitem,item, rep))
						return true; //��ֹ����400 Bad Request����

					std::string strTransport;
					long lPortbase;

					if(item.conntype==TT_UDP)
					{												
						VideoStreamManager *pVideoMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();
						lPortbase = pVideoMgr->GetSenderPort(item,vitem);
						strTransport = format("source=%s;server_port=%d-%d",req.server_addr.to_string().c_str(),lPortbase,lPortbase+1);						
					}
					else if(item.conntype==TT_TCP)
					{
						//TODO:ʹ�ù̶�����Ƶ�˿�12000
						strTransport = format("source=%s;server_port=%d-%d",req.server_addr.to_string().c_str(),12000,12001);						
					}

					rep.headers[2].value=getHeadersItem(req,"Transport")+";"+strTransport;										
					
					item.portbase = (WORD)lPortbase;	//������Udp����ʱ��	
					if(item.recvip.empty())
							item.recvip=item.clientip;
					item.pUserData = boost::shared_ptr<ObserverInfo>(new ObserverInfo(item.recvip.c_str(),item.clientport,item.conntype));
					item.status = reply::ok;			

					time(&item.timestamp);
					item.hSession = (HANDLE)vitem.sessionid;	//8.15 setupʱ�ͽ�vitem��sessID��ֵ��item
					pMgr->AddSession(item);						
					return true;
				}
				break;
			case methodAnnounce:
				{					
					string session = getHeadersItem(req,"Session");
					int sessionid;

					try{
						sessionid = boost::lexical_cast<int>(session);
					}
					catch(const boost::bad_lexical_cast& e)
					{
						UNREFERENCED_PARAMETER(e);
						return false;
					}

					RTSPSessionItem item;
					RTSPSessionManager* pMgr = RTSPSessionManagerSingleton::instance();					
					if(!pMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "�û��Ự��������ʼ���쳣";
						X_LOG0(XLOG_ERROR,_T("�û��Ự��������ʼ������."));
						return true;
					}

					if(!pMgr->GetSessionByHandle(sessionid,item))
						return false;
					
					time(&item.timestamp);

					if(!pMgr->SetSessionByHandle(sessionid,item))
						return false;

					//���ûظ����
					rep.status = item.status;
					rep.content.clear();
					rep.headers.resize(2);
					rep.headers[0].name="CSeq";
					rep.headers[0].value= boost::lexical_cast<string>(sequence);	
					rep.headers[1].name="Session";
					rep.headers[1].value= boost::lexical_cast<string>(sessionid);						
					return true;
				}
				break;
			case methodPlay:
				{
					string session = getHeadersItem(req,"Session");
					int sessionid;					
					
					try{
						sessionid = boost::lexical_cast<int>(session);
					}
					catch(const boost::bad_lexical_cast& e)
					{
						UNREFERENCED_PARAMETER(e);
						return false;
					}	

					RTSPSessionItem item;
					RTSPSessionManager* pMgr = RTSPSessionManagerSingleton::instance();
					if(!pMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "�û��Ự��������ʼ���쳣";
						X_LOG0(XLOG_ERROR,_T("�û��Ự��������ʼ������."));
						return true;
					}

					if(!pMgr->GetSessionByHandle(sessionid,item))
						return false;

					VideoSourceItem vitem;
					//8.15 add	
					VideoStreamManager *pVideoMgr = (VideoStreamManager *)VideoStreamManagerSingleton::instance();
					if (!pVideoMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "��ƵԴ��������ʼ���쳣��";
						X_LOG0(XLOG_FATAL, _T("��ƵԴ��������ʼ���쳣��"));
						return false;
					}

					if (!pVideoMgr->getStreamByID((int)item.hSession, vitem))
					{
						//������ʧ�ܣ����ߴ���������ʧ��
						rep.status = reply::connect_time_out;
						rep.content = "��ȡ��Ƶ�������÷��Ͷ˿�ʱʧ��!";
						return true; //��ֹ����400 Bad Request����
					}
					////8.15 add

					//8.15 ע�͵����������
//   				   if(!GetVideoStream(vitem,item, rep))
// 				   {
// 					   //������ʧ�ܣ����ߴ���������ʧ��
// 					   rep.status = reply::connect_time_out;
// 					   rep.content = "��ȡ��Ƶ�������÷��Ͷ˿�ʱʧ��!";					   
// 					   return true; //��ֹ����400 Bad Request����
// 				   }										
// 					
// 					item.hSession = (HANDLE)vitem.sessionid;	

					X_LOG4(XLOG_DEBUG,_T("���ݽ��տͻ���(session=%d,hsession=%d)��ַ[%s:%d]"),sessionid,vitem.sessionid,CString(item.recvip.c_str()),item.clientport);

					if(item.pUserData==NULL)
						item.pUserData = boost::shared_ptr<ObserverInfo>(new ObserverInfo(item.clientip.c_str(),item.clientport,item.conntype));

					pMgr->SetSessionByHandle(sessionid,item);				

					if(vitem.sender)
					{											
						//����RTP�Ự  8.15 ע��					
// 						VideoStreamManager *pVideoMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();										
// 						if(!pVideoMgr)
// 						{
// 							rep.status = reply::internal_server_error;
// 							rep.content = "��ƵԴ��������ʼ���쳣��";
// 							X_LOG0(XLOG_FATAL,_T("��ƵԴ��������ʼ���쳣��"));
// 							return false;
// 						}	

						vitem.sender->addObserver(item.pUserData);
						pVideoMgr->SetSessionByHandle(vitem.sessionid,vitem);
						//cout<<"item.channel:"<<item.channel<<" item.streamtype:"<<item.streamtype<<std::endl;
						vitem.pSource->PrepareStream(item.channel,item.streamtype);

						rep.status = reply::ok;																													
						
						//û�л�ȡ����-1
						if(vitem.nHeaderLength<0)
						{
							//2016.9.17 zxl ȡ����ӡ
							X_LOG1(XLOG_DEBUG,_T("�Ӳ����ȡ�ļ�ͷ,�������:[%s]"),CString(item.devtype.c_str()));

							BOOL bRet=FALSE;
							//û���ļ�ͷ�Ļ�����3��
							int nRetrys = 3;
							do
							{
								//�ȴ���󴫳�
								unsigned long nBufferLength = 512;	
								bRet = pVideoMgr->GetFileHeader(vitem.sessionid,vitem.header,nBufferLength);
								if(!bRet)
								{
									Sleep(1000);
									nRetrys--;
								}
								else
								{
									//�ɹ�����������
									vitem.nHeaderLength = nBufferLength;
									pVideoMgr->SetSessionByHandle(vitem.sessionid,vitem);
									break;
								}
							} while(nRetrys>0&&!bRet);

							if(!bRet)
								X_LOG1(XLOG_WARNING,_T("�������[%s]��ȡ�ļ�ͷʧ��!"),CString(item.devtype.c_str()));
						}
						else
							X_LOG1(XLOG_DEBUG,_T("�ӻ�����ֱ��ȡ�ļ�ͷ,�������:[%s]"),CString(item.devtype.c_str()));

						if(vitem.nHeaderLength>0)
								rep.content.append((const char*)vitem.header,vitem.nHeaderLength);
						else	rep.content.clear();

						rep.headers.resize(2);
						rep.headers[0].name="CSeq";
						rep.headers[0].value= boost::lexical_cast<string>(sequence);	
						rep.headers[1].name="Content-Length";
						rep.headers[1].value=boost::lexical_cast<string>(vitem.nHeaderLength);							
						return true;
					}
					else
					{
						item.pUserData.reset();
						pMgr->SetSessionByHandle(sessionid,item);
					}
				}
				break;
			case methodTeardown:
				//TODO:ֹͣ���ͣ��رջỰ,������Դ,
				//���ûظ����
				{					
					string session = getHeadersItem(req,"Session");
					int sessionid = -1;
					if(session.empty())
					{
						session = getHeadersItem(req,"User-agent");
					}										
					try{
							sessionid = boost::lexical_cast<int>(session);
					}
					catch(const boost::bad_lexical_cast& e)
					{
						UNREFERENCED_PARAMETER(e);
						X_LOG1(XLOG_FATAL,_T("�����Ự����:%s"),CString(session.c_str()));
						return false;
					}

					RTSPSessionItem item;					
					RTSPSessionManager* pMgr = RTSPSessionManagerSingleton::instance();
					if(!pMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "�û��Ự��������ʼ���쳣";
						X_LOG0(XLOG_ERROR,_T("�û��Ự��������ʼ������."));
						return true;
					}

					if(!pMgr->GetSessionByHandle(sessionid,item))
					{
						X_LOG1(XLOG_ERROR,_T("��Ч��Session[%d]��"),sessionid);
						return false;
					}
					rep.status = reply::ok;
					rep.content.clear();
					rep.headers.resize(1);
					rep.headers[0].name="CSeq";
					rep.headers[0].value= boost::lexical_cast<string>(sequence);
					VideoStreamManager *pVideoMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();
					if(!pVideoMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "��ƵԴ��������ʼ���쳣��";
						X_LOG0(XLOG_FATAL,_T("��ƵԴ��������ʼ���쳣��"));
						return true;
					}			

					pMgr->CleanSession(sessionid);
					return true;
				}
								
				break;
			case methodOptions:
				{					
					string  command= getHeadersItem(req,"command");
					string  session= getHeadersItem(req,"Session");
					string  stop= getHeadersItem(req,"param1");
					string  speed= getHeadersItem(req,"param2");
					string  param3= getHeadersItem(req,"param3");
					string  param4= getHeadersItem(req,"param4");					
					string  param5 = getHeadersItem(req,"param5");					
					
					DWORD dwCommand;
					DWORD dwStop;
					DWORD dwSpeed;
					DWORD dwParam3;
					DWORD dwParam4;
					long  lSessionId;
					vector<DWORD> params;

					try{
						dwCommand = boost::lexical_cast<DWORD>(command);
						lSessionId = boost::lexical_cast<long>(session);	
						dwStop = boost::lexical_cast<DWORD>(stop);
						dwSpeed = boost::lexical_cast<DWORD>(speed);
						dwParam3 = boost::lexical_cast<DWORD>(param3);
						dwParam4 = boost::lexical_cast<DWORD>(param4);											
						params.push_back(dwStop);
						params.push_back(dwSpeed);					
						params.push_back(dwParam3);					
						params.push_back(dwParam4);					
					}
					catch(const boost::bad_lexical_cast& e)
					{
						UNREFERENCED_PARAMETER(e);
						return false;
					}
												
					RTSPSessionItem item;
					RTSPSessionManager* pMgr = RTSPSessionManagerSingleton::instance();
					if(!pMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "�û��Ự��������ʼ���쳣";
						X_LOG0(XLOG_ERROR,_T("�û��Ự��������ʼ������."));
						return true;
					}

					if(!pMgr->GetSessionByHandle(lSessionId,item))
						return false;
					
					//cout<<"command:"<<dwCommand<<"("<<dwStop<<","<<dwSpeed<<","<<param3<<","<<param4<<")"<<std::endl;					

					VideoStreamManager *pVideoMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();					
					if(!pVideoMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "��ƵԴ��������ʼ���쳣��";
						X_LOG0(XLOG_FATAL,_T("��ƵԴ��������ʼ���쳣��"));
						return true;
					}
					
					VideoSourceItem vitem;
					if(!pVideoMgr->GetSessionByHandle((DWORD)item.hSession,vitem))
					{
						X_LOG3(XLOG_ERROR, _T("�޷���ȡ���ӵ���Ƶ����Ϣ.IP:%s,ͨ��:%d,�ļ�:%s"), CString(item.dvrip.c_str()), item.channel, CString(item.file.c_str()));
						return false;
					}	 

					if( dwCommand >= NET_DVR_PLAYPAUSE && dwCommand <= NET_DVR_GETTOTALTIME )
					{
						vector<header> result;
						if(pVideoMgr->DoExtendCommand(item,vitem,dwCommand,params,param5,result))	
						{							
							rep.status = reply::ok;							
							if(!result.empty())
							{
								rep.content.clear();
								rep.headers.resize(2);
								rep.headers[0].name="CSeq";
								rep.headers[0].value= boost::lexical_cast<string>(sequence);
								rep.headers[1].name="Content-Length";
								rep.headers[1].value=boost::lexical_cast<string>(result[0].value.length());
								rep.content.append(result[0].value);
							}
							return true;
						}
						else
						{
							rep.status = reply::unsupported; 
							rep.content.clear();
							rep.headers.resize(1);
							rep.headers[0].name="CSeq";
							rep.headers[0].value= boost::lexical_cast<string>(sequence);
							return true;
						}
					}
					else if(pVideoMgr->DoPTZCommand(item,vitem,dwCommand,params,param5))					
					{
						rep.status = reply::ok;
						rep.content.clear();
						rep.headers.resize(1);
						rep.headers[0].name = "CSeq";
						rep.headers[0].value = boost::lexical_cast<string>(sequence);
						return true;
					}
					else
					{
						rep.status = reply::unsupported;
						rep.content.clear();
						rep.headers.resize(1);
						rep.headers[0].name="CSeq";
						rep.headers[0].value= boost::lexical_cast<string>(sequence);
						return true;
					}
				}
				break;				
			case methodGet:
				{
					RTSPSessionItem item;						
					
					if(!getUriInfo(req.uri,item))	return false;

					if(item.file!="QUERYLIST")
					{
						rep.status = reply::unsupported;
						return true;
					}
					else
					{
						VideoStreamManager *pVideoMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();
						VideoSourceItem vitem;
						vitem.pSource = NULL;						
						
						if(!pVideoMgr->CreateVideoSource(item,&vitem.pSource))						
						{	
							rep.status = reply::connect_time_out;
							return true;
						}

						vector<header> result;
						vector<DWORD> params;
						DWORD dwRecType = ALL_REC;
						string  strRecType = getHeadersItem(req,"RecordType");
						string  strDateRange = getHeadersItem(req,"DateRange");
						if(!strRecType.empty())
							dwRecType = boost::lexical_cast<DWORD>(strRecType);
						if(strDateRange.empty())
							strDateRange = "20091001000000-29991231235959";
						params.push_back(dwRecType);

						if(pVideoMgr->DoExtendCommand(item,vitem,NET_DVR_PLAYFILELIST,params,strDateRange,result))
						{														
							if(!result.empty())
							{
								rep.status =reply::ok;
								rep.content.clear();
								rep.headers.resize(1);								
								rep.headers[0].name="Content-Length";
								rep.headers[0].value=boost::lexical_cast<string>(result[0].value.length());
								rep.content = result[0].value;								
							}
							else	rep.status = reply::not_found;
						}
						else	rep.status = reply::service_unavailable;

						if(vitem.pSource)
						{
							vitem.pSource->DisconnectDevice();
							delete vitem.pSource;
						}
						return true;
					}
				}
				break;
			default:				
				return false;
			}
			return false;
		}

		bool Request_Preprocess::GetVideoStream(VideoSourceItem &vitem, const RTSPSessionItem &item, reply &rep) 
		{
			//����RTP�Ự					
			VideoStreamManager *pVideoMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();										
			if(!pVideoMgr)
			{
				rep.status = reply::internal_server_error;
				rep.content = "��ƵԴ��������ʼ���쳣��";
				X_LOG0(XLOG_FATAL,_T("��ƵԴ��������ʼ���쳣��"));
				return false;
			}	

			//std::cout<<"\n���ŵ���:"<<(item.file.empty()?"ʵʱ����\n":item.file)<<std::endl;

//			if(!pVideoMgr->GetLiveStreamByDVR(item.dvrip.c_str(),item.channel,item.file,vitem))										
			//7.28  ����sessionID�Ĵ���
			//2012-10-26 ���Ӷ˿ںŲ���
			if(!pVideoMgr->GetLiveStreamByDVR(item.streamtype,item.sessionid,item.dvrip.c_str(),item.port,item.channel,item.file,vitem))										
			{
				try{
					if(!pVideoMgr->CreateVideoStream(item,vitem))						
					{
						rep.status = reply::connect_time_out;
						rep.content = "�޷������豸��";
						return false;
					}
				}
				catch(...)
				{
					X_LOG3(XLOG_FATAL, _T("������Ƶ��ʱ�����쳣!�豸����:%s,IP:%sͨ��:%d"), CString(item.devtype.c_str()), CString(item.dvrip.c_str()), item.channel);
					rep.status = reply::service_unavailable;
					return false;
				}						
			}
			else
			{
				LOG4CPLUS_DEBUG(logger, _T("��ȡ������,IP:") << CString(item.dvrip.c_str()) << _T(" ͨ��:") << item.channel << std::endl);
//				S_LOG(DEBUG, _T("��ȡ������,IP:") << CString(item.dvrip) << _T(" ͨ��:") << item.channel << std::endl);
			}
			return true;
		}
	}
}

