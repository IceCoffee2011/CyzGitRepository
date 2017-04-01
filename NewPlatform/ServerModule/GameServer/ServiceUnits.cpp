#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

//////////////////////////////////////////////////////////////////////////////////

//��̬����
CServiceUnits *			CServiceUnits::g_pServiceUnits=NULL;			//����ָ��

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceUnits, CWnd)
	ON_MESSAGE(WM_UICONTROL_REQUEST,OnUIControlRequest)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServiceUnits::CServiceUnits()
{
	//�������
	m_ServiceStatus=ServiceStatus_Stop;

	//���ýӿ�
	m_pIServiceUnitsSink=NULL;

	//���ö���
	ASSERT(g_pServiceUnits==NULL);
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	//�������
	ZeroMemory(&m_GameParameter,sizeof(m_GameParameter));
	ZeroMemory(&m_DataBaseParameter,sizeof(m_DataBaseParameter));
	ZeroMemory(&m_GameServiceAttrib,sizeof(m_GameServiceAttrib));
	ZeroMemory(&m_GameServiceOption,sizeof(m_GameServiceOption));
	//ZeroMemory(m_GameSquareOptionArray,sizeof(m_GameSquareOptionArray));
	for(int i=0; i<20; i++)m_pGameSquareOptionArray[i]=NULL;
	m_cbGameSquareOptionArrayCount=0;
	m_vGameSquareOption.clear();
	return;
}

//��������
CServiceUnits::~CServiceUnits()
{
}

//���ýӿ�
bool CServiceUnits::SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)
{
	//���ñ���
	m_pIServiceUnitsSink=pIServiceUnitsSink;

	return true;
}

//Ͷ������
bool CServiceUnits::PostControlRequest(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//״̬�ж�
	ASSERT(IsWindow(m_hWnd));
	if (IsWindow(m_hWnd)==FALSE) return false;

	//�������
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.InsertData(wIdentifier,pData,wDataSize)==false) return false;

	//������Ϣ
	PostMessage(WM_UICONTROL_REQUEST,wIdentifier,wDataSize);

	return true;
}

//��������
bool CServiceUnits::StartService()
{
	//Ч��״̬
	ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//����״̬
	SetServiceStatus(ServiceStatus_Config);

	//��������
	if (m_hWnd==NULL)
	{
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,AfxGetMainWnd(),100);
	}

	//����ģ��
	if (CreateServiceDLL()==false)
	{
		//��ʾ��Ϣ
		CTraceService::TraceString(_T("CServiceUnits::StartService, CreateServiceDLL() false"),TraceLevel_Exception);
		ConcludeService();
		return false;
	}

	//��������
	if (RectifyServiceParameter()==false)
	{
		//��ʾ��Ϣ
		CTraceService::TraceString(_T("CServiceUnits::StartService, RectifyServiceParameter() false"),TraceLevel_Exception);
		ConcludeService();
		return false;
	}

	//���÷���
	if (InitializeService()==false)
	{
		//��ʾ��Ϣ
		CTraceService::TraceString(_T("CServiceUnits::StartService, InitializeService() false"),TraceLevel_Exception);
		ConcludeService();
		return false;
	}

	//�����ں�
	if (StartKernelService()==false)
	{
		//��ʾ��Ϣ
		CTraceService::TraceString(_T("CServiceUnits::StartService, StartKernelService() false"),TraceLevel_Exception);

		ConcludeService();
		return false;
	}

	//��������
	SendControlPacket(CT_LOAD_SERVICE_CONFIG,NULL,0);

	//��������
	if (m_GameMatchServiceManager.GetInterface()!=NULL && m_GameMatchServiceManager->StartService()==false) return false;

	return true;
}

//ֹͣ����
bool CServiceUnits::ConcludeService()
{
	//���ñ���
	SetServiceStatus(ServiceStatus_Stop);

	//�ں����
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPSocketService.GetInterface()!=NULL) m_TCPSocketService->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();

	//��������
	if (m_RecordDataBaseEngine.GetInterface()!=NULL) m_RecordDataBaseEngine->ConcludeService();
	if (m_KernelDataBaseEngine.GetInterface()!=NULL) m_KernelDataBaseEngine->ConcludeService();
	m_DBCorrespondManager.ConcludeService();

	//ע�����
	if (m_GameServiceManager.GetInterface()!=NULL) m_GameServiceManager.CloseInstance();
	if(m_GameMatchServiceManager.GetInterface()!=NULL)m_GameMatchServiceManager.CloseInstance();

	return true;
}

//��Ϸ����
bool CServiceUnits::CollocateService(LPCTSTR pszGameModule, tagGameServiceOption & GameServiceOption, tagGameServiceTableOption& GameServiceTableOption)
{
	//Ч��״̬
	ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//����ģ��
	m_GameServiceOption=GameServiceOption;
	m_GameServiceTableOption = GameServiceTableOption;
	m_GameServiceManager.SetModuleCreateInfo(pszGameModule,GAME_SERVICE_CREATE_NAME);

	return true;
}

//�㳡������
bool CServiceUnits::CollocateSquareMatch(tagGameSquareOption GameSquareOptionArray[], BYTE cbGameSquareOptionArrayCount)
{
 	//Ч��״̬
 	ASSERT(m_ServiceStatus==ServiceStatus_Stop);
 	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//����ģ��
	//m_cbGameSquareOptionArrayCount=cbGameServiceOptionArrayCount;
	//for(BYTE i=0; i<cbGameServiceOptionArrayCount; i++)m_GameSquareOptionArray[i]=GameServiceOptionArray[i];
	m_vGameSquareOption.clear();
	for(BYTE i=0; i<cbGameSquareOptionArrayCount; i++)
	{
		m_vGameSquareOption.push_back(GameSquareOptionArray[i]);
	}

	m_cbGameSquareOptionArrayCount=cbGameSquareOptionArrayCount;
	for(BYTE i=0; i<cbGameSquareOptionArrayCount;i++)
	{
		m_pGameSquareOptionArray[i]=&GameSquareOptionArray[i];
	}

	return true;
}

//���¹㳡������
bool CServiceUnits::UpdateSquareMatch(tagGameSquareOption GameSquareOptionArray[], BYTE cbGameSquareOptionArrayCount)
{
	//Ч��״̬
	ASSERT(m_ServiceStatus!=ServiceStatus_Config);
	if (m_ServiceStatus==ServiceStatus_Config) return false;

	if(cbGameSquareOptionArrayCount<=0)return true;

	/*
	//���¹㳡������
	for(int i=0; i<cbGameSquareOptionArrayCount; i++)
	{
		if(i>=20)break;
		bool bAdd=true;
		for(int j=0; j<m_vGameSquareOption.size();j++)
		{
			if(j>=20)break;
			if(GameSquareOptionArray[i].wMatchID==m_vGameSquareOption[j].wMatchID)
			{//�Ѵ��ڹ㳡��
				bAdd=false;

				//ʣ��ʱ��
				CTime tmLocalTime=CTime::GetCurrentTime();
				CTime tmStartTime(m_vGameSquareOption[j].StartTime);
				CTime tmEndTime(m_vGameSquareOption[j].EndTime);

				if(tmLocalTime>=tmStartTime&&tmLocalTime<tmEndTime)
				{
					LOGW("CServiceUnits::UpdateSquareMatch ���ع㳡��ʧ��, �㳡���ѿ�ʼ wMatch= "\
						<< m_vGameSquareOption[j].wMatchID << " SquareName= "\
						<< m_vGameSquareOption[j].szSquareName);
					break;
				}

				m_vGameSquareOption[j]=GameSquareOptionArray[i];
// 				memcpy(&(m_vGameSquareOption[j]), &(DlgServerItem.m_ModuleInitParameter.GameSquareOptionArray[i]),sizeof(tagGameSquareOption));
				break;
			}
		}

		if(bAdd)
		{
			m_vGameSquareOption.push_back(GameSquareOptionArray[i]);
		}
			//memcpy(&(m_ModuleInitParameter.GameSquareOptionArray[m_ModuleInitParameter.cbGameSquareOptionArrayCount++]), &(DlgServerItem.m_ModuleInitParameter.GameSquareOptionArray[i]),sizeof(tagGameSquareOption));		
	}

	m_AttemperEngineSink.m_cbGameSquareOptionArrayCount=(BYTE)m_vGameSquareOption.size();
 	for(BYTE i=0; i<m_vGameSquareOption.size(); i++)m_AttemperEngineSink.m_pGameSquareOptionArray[i]=&m_vGameSquareOption[i];*/

	//���¹㳡������
	for(int i=0; i<cbGameSquareOptionArrayCount; i++)
	{
		if(i>=20)break;

		if(GameSquareOptionArray[i].wMatchID<=0)continue;

		CTime tmLocalTime=CTime::GetCurrentTime();
		CTime tmStartTime(GameSquareOptionArray[i].StartTime);
		CTime tmEndTime(GameSquareOptionArray[i].EndTime);
		if(tmLocalTime>=tmEndTime)
		{
			LOGW("CServiceUnits::UpdateSquareMatch ���ع㳡������, �˹㳡���ѽ��� wMatch= "\
				<< GameSquareOptionArray[i].wMatchID << " SquareName= "\
				<< GameSquareOptionArray[i].szSquareName);
			continue;			
		}

		bool bAdd=true;
		for(int j=0; j<m_cbGameSquareOptionArrayCount;j++)
		{
			if(j>=20)break;
			if(GameSquareOptionArray[i].wMatchID==m_pGameSquareOptionArray[j]->wMatchID)
			{//�Ѵ��ڹ㳡��
				bAdd=false;

				//ʣ��ʱ��
				CTime tmLocalTime=CTime::GetCurrentTime();
				CTime tmStartTime(m_pGameSquareOptionArray[j]->StartTime);
				CTime tmEndTime(m_pGameSquareOptionArray[j]->EndTime);

				if(tmLocalTime>=tmStartTime&&tmLocalTime<tmEndTime)
				{
					LOGW("CServiceUnits::UpdateSquareMatch ���ع㳡��ʧ��, �㳡���ѿ�ʼ wMatch= "\
						<< m_pGameSquareOptionArray[j]->wMatchID << " SquareName= " << m_pGameSquareOptionArray[j]->szSquareName);
					break;
				}

				memcpy(m_pGameSquareOptionArray[j],&GameSquareOptionArray[i],sizeof(tagGameSquareOption));
				break;
			}
		}

		if(bAdd)
		{
			m_pGameSquareOptionArray[m_cbGameSquareOptionArrayCount++]=&GameSquareOptionArray[i];
		}
	}

	m_AttemperEngineSink.m_cbGameSquareOptionArrayCount=m_cbGameSquareOptionArrayCount;
	for(BYTE i=0; i<m_cbGameSquareOptionArrayCount; i++)m_AttemperEngineSink.m_pGameSquareOptionArray[i]=m_pGameSquareOptionArray[i];


	for(BYTE i=0; i<m_AttemperEngineSink.m_cbGameSquareOptionArrayCount; i++)
	{
		m_AttemperEngineSink.UpdateSquareMatch(m_AttemperEngineSink.m_pGameSquareOptionArray[i]);

// 		LOGI("CServiceUnits::UpdateSquareMatch ���ع㳡�� wMatch= "\
// 			<< m_AttemperEngineSink.m_pGameSquareOptionArray[i]->wMatchID << " SquareName= "\
// 			<< m_AttemperEngineSink.m_pGameSquareOptionArray[i]->szSquareName);
	}

	for(map<WORD,CSquare>::iterator iter=m_AttemperEngineSink.m_SquareManager.begin(); iter!=m_AttemperEngineSink.m_SquareManager.end(); iter++)
	{
		TCHAR szStartToEndTime[1024]=TEXT("");
		_sntprintf(szStartToEndTime,CountArray(szStartToEndTime),\
			TEXT("%d-%02d-%02d %02d:%02d:%02d - %d-%02d-%02d %02d:%02d:%02d"),\
			iter->second.m_pGameSquareOption->StartTime.wYear,iter->second.m_pGameSquareOption->StartTime.wMonth,iter->second.m_pGameSquareOption->StartTime.wDay,\
			iter->second.m_pGameSquareOption->StartTime.wHour,iter->second.m_pGameSquareOption->StartTime.wMinute,iter->second.m_pGameSquareOption->StartTime.wSecond,\
			iter->second.m_pGameSquareOption->EndTime.wYear,iter->second.m_pGameSquareOption->EndTime.wMonth,iter->second.m_pGameSquareOption->EndTime.wDay,\
			iter->second.m_pGameSquareOption->EndTime.wHour,iter->second.m_pGameSquareOption->EndTime.wMinute,iter->second.m_pGameSquareOption->EndTime.wSecond);
		LOGI("CServiceUnits::UpdateSquareMatch ���¹㳡������: MatchID= " << iter->second.m_pGameSquareOption->wMatchID\
			<< " SquareName= " << iter->second.m_pGameSquareOption->szSquareName << " ServerID= " << iter->second.m_pGameSquareOption->wServerID\
			<< " GameID= " << iter->second.m_pGameSquareOption->wGameID << " StartToEndTime= " << szStartToEndTime << " Interval= " << iter->second.m_pGameSquareOption->wInterval\
			<< " RankNumber= " << iter->second.m_pGameSquareOption->wRankNumber << " SquareDesc= " << iter->second.m_pGameSquareOption->szSquareDesc\
			<< " SquareBroadcast= " << iter->second.m_pGameSquareOption->szSquareBroadcast << " RankBonusCount= " << iter->second.m_pGameSquareOption->cbRankBonus\
			<< " RankBonusByResultCount= " << iter->second.m_pGameSquareOption->cbRankBonusByResult);

		LOGI("CServiceUnits::UpdateSquareMatch �㳡���������� RankBonusCount= " << iter->second.m_pGameSquareOption->cbRankBonus);

		for(int i=0; i<iter->second.m_pGameSquareOption->cbRankBonus; i++)
		{
			LOGI("CServiceUnits::UpdateSquareMatch i= " << i << " First= " << iter->second.m_pGameSquareOption->RankBonus[i].wFirst\
				<< " Last= " << iter->second.m_pGameSquareOption->RankBonus[i].wLast\
				<< " PropertyCount= " << iter->second.m_pGameSquareOption->RankBonus[i].cbPropertyCount);

			for(int j=0; j<iter->second.m_pGameSquareOption->RankBonus[i].cbPropertyCount; j++)
			{
				LOGI("CServiceUnits::UpdateSquareMatch j= " << j << " PropertyID= " << iter->second.m_pGameSquareOption->RankBonus[i].wPropertyID[j]);
			}
		}
	}



	return true;
}

//����ģ��
bool CServiceUnits::CreateServiceDLL()
{
	//ʱ������
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TimerEngine.GetErrorDescribe(),TraceLevel_Exception);
		LOGI("CServiceUnits::CreateServiceDLL, m_TimerEngine.GetErrorDescribe, "<<m_TimerEngine.GetErrorDescribe());
		return false;
	}

	//��������
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_AttemperEngine.GetErrorDescribe(),TraceLevel_Exception);
		LOGI("CServiceUnits::CreateServiceDLL, m_AttemperEngine.GetErrorDescribe, "<<m_AttemperEngine.GetErrorDescribe());
		return false;
	}

	//�������
	if ((m_TCPSocketService.GetInterface()==NULL)&&(m_TCPSocketService.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPSocketService.GetErrorDescribe(),TraceLevel_Exception);
		LOGI("CServiceUnits::CreateServiceDLL, m_TCPSocketService.GetErrorDescribe, "<<m_TCPSocketService.GetErrorDescribe());
		return false;
	}

	//��������
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPNetworkEngine.GetErrorDescribe(),TraceLevel_Exception);
		LOGI("CServiceUnits::CreateServiceDLL, m_TCPNetworkEngine.GetErrorDescribe, "<<m_TCPNetworkEngine.GetErrorDescribe());
		return false;
	}

	//�������
	if ((m_KernelDataBaseEngine.GetInterface()==NULL)&&(m_KernelDataBaseEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_KernelDataBaseEngine.GetErrorDescribe(),TraceLevel_Exception);
		LOGI("CServiceUnits::CreateServiceDLL, m_KernelDataBaseEngine.GetErrorDescribe, "<<m_KernelDataBaseEngine.GetErrorDescribe());
		return false;
	}

	//�������
	if ((m_RecordDataBaseEngine.GetInterface()==NULL)&&(m_RecordDataBaseEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_RecordDataBaseEngine.GetErrorDescribe(),TraceLevel_Exception);
		LOGI("CServiceUnits::CreateServiceDLL, m_RecordDataBaseEngine.GetErrorDescribe, "<<m_RecordDataBaseEngine.GetErrorDescribe());
		return false;
	}

	//��Ϸģ��
	if ((m_GameServiceManager.GetInterface()==NULL)&&(m_GameServiceManager.CreateInstance()==false))
	{
		CTraceService::TraceString(m_GameServiceManager.GetErrorDescribe(),TraceLevel_Exception);
		LOGI("CServiceUnits::CreateServiceDLL, m_GameServiceManager.GetErrorDescribe, "<<m_GameServiceManager.GetErrorDescribe());
		return false;
	}

	//����ģ��
	if ((m_GameServiceOption.wServerType&GAME_GENRE_MATCH)!=0)
	{
		if ((m_GameMatchServiceManager.GetInterface()==NULL)&&(m_GameMatchServiceManager.CreateInstance()==false))
		{
			CTraceService::TraceString(m_GameMatchServiceManager.GetErrorDescribe(),TraceLevel_Exception);
			LOGI("CServiceUnits::CreateServiceDLL, m_GameMatchServiceManager.GetErrorDescribe, "<<m_GameMatchServiceManager.GetErrorDescribe());
			return false;
		}
	}

	return true;
}

//�������
bool CServiceUnits::InitializeService()
{
	//���ز���
	m_InitParameter.LoadInitParameter();

	//���ò���
	m_GameParameter.wMedalRate=1L;
	m_GameParameter.wRevenueRate=1L;

	//������Ϣ
	LPCTSTR pszDataBaseAddr=m_GameServiceOption.szDataBaseAddr;
	LPCTSTR pszDataBaseName=m_GameServiceOption.szDataBaseName;
	if (LoadDataBaseParameter(pszDataBaseAddr,pszDataBaseName,m_DataBaseParameter)==false) return false;

	//����ӿ�
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//��������
	IUnknownEx * pIDataBaseEngineRecordSink[CountArray(m_RecordDataBaseSink)];
	IUnknownEx * pIDataBaseEngineKernelSink[CountArray(m_KernelDataBaseSink)];
	for (WORD i=0;i<CountArray(pIDataBaseEngineRecordSink);i++) pIDataBaseEngineRecordSink[i]=QUERY_OBJECT_INTERFACE(m_RecordDataBaseSink[i],IUnknownEx);
	for (WORD i=0;i<CountArray(pIDataBaseEngineKernelSink);i++) pIDataBaseEngineKernelSink[i]=QUERY_OBJECT_INTERFACE(m_KernelDataBaseSink[i],IUnknownEx);

	//�󶨽ӿ�
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;
	if (m_RecordDataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineRecordSink,CountArray(pIDataBaseEngineRecordSink))==false) return false;
	if (m_KernelDataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineKernelSink,CountArray(pIDataBaseEngineKernelSink))==false) return false;

	//�ں����
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false;
	if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine)==false) return false;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;

	//Э������
	if (m_TCPSocketService->SetServiceID(NETWORK_CORRESPOND)==false) return false;
	if (m_TCPSocketService->SetTCPSocketEvent(pIAttemperEngine)==false) return false;

	//����Э��
	m_DBCorrespondManager.InitDBCorrespondManager(m_KernelDataBaseEngine.GetInterface());

	//���Ȼص�
	m_AttemperEngineSink.m_pInitParameter=&m_InitParameter;
	m_AttemperEngineSink.m_pGameParameter=&m_GameParameter;
	m_AttemperEngineSink.m_pGameServiceAttrib=&m_GameServiceAttrib;
	m_AttemperEngineSink.m_pGameServiceOption = &m_GameServiceOption;
	m_AttemperEngineSink.m_pGameServiceTableOption = &m_GameServiceTableOption;
	//m_AttemperEngineSink.m_cbGameSquareOptionArrayCount=(BYTE)m_vGameSquareOption.size();
	//for(BYTE i=0; i<m_vGameSquareOption.size(); i++)m_AttemperEngineSink.m_pGameSquareOptionArray[i]=&m_vGameSquareOption[i];
	m_AttemperEngineSink.m_cbGameSquareOptionArrayCount=m_cbGameSquareOptionArrayCount;
	for(BYTE i=0; i<m_cbGameSquareOptionArrayCount; i++)m_AttemperEngineSink.m_pGameSquareOptionArray[i]=m_pGameSquareOptionArray[i];

	//���Ȼص�
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pIAttemperEngine=m_AttemperEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketService=m_TCPSocketService.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	m_AttemperEngineSink.m_pIGameServiceManager=m_GameServiceManager.GetInterface();
	m_AttemperEngineSink.m_pIRecordDataBaseEngine=m_RecordDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIKernelDataBaseEngine=m_KernelDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIGameMatchServiceManager=m_GameMatchServiceManager.GetInterface();
	m_AttemperEngineSink.m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);

	//���ݻص�
	for (INT i=0;i<CountArray(m_RecordDataBaseSink);i++)
	{
		m_RecordDataBaseSink[i].m_pInitParameter=&m_InitParameter;
		m_RecordDataBaseSink[i].m_pGameParameter=&m_GameParameter;
		m_RecordDataBaseSink[i].m_pDataBaseParameter=&m_DataBaseParameter;
		m_RecordDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_RecordDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_RecordDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
		m_RecordDataBaseSink[i].m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);
	}

	//���ݻص�
	for (INT i=0;i<CountArray(m_KernelDataBaseSink);i++)
	{
		m_KernelDataBaseSink[i].m_pInitParameter=&m_InitParameter;
		m_KernelDataBaseSink[i].m_pGameParameter=&m_GameParameter;
		m_KernelDataBaseSink[i].m_pDataBaseParameter=&m_DataBaseParameter;
		m_KernelDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_KernelDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_KernelDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
		m_KernelDataBaseSink[i].m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);
		m_KernelDataBaseSink[i].m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);
	}

	//��������
	m_TCPNetworkEngine->SetServiceParameter(m_GameServiceOption.wServerPort,m_GameServiceOption.wMaxPlayer,szCompilation);

	return true;
}

//�����ں�
bool CServiceUnits::StartKernelService()
{
	//ʱ������
	if (m_TimerEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_AttemperEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//Э������
	if (m_TCPSocketService->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_RecordDataBaseEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_KernelDataBaseEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//����Э��
	if (m_DBCorrespondManager.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//��������
bool CServiceUnits::StartNetworkService()
{
	//��������
	if (m_TCPNetworkEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//��������
bool CServiceUnits::RectifyServiceParameter()
{
	//��ȡ����
	m_GameServiceManager->GetServiceAttrib(m_GameServiceAttrib);
	if (lstrcmp(m_GameServiceAttrib.szServerDLLName,m_GameServiceManager.m_szModuleDllName)!=0)
	{
		CTraceService::TraceString(TEXT("��Ϸģ��İ� DLL ����������ģ�� DLL ���ֲ�һ��"),TraceLevel_Exception);
		return false;
	}

	//��������
	for (size_t i = 0; i < m_GameServiceTableOption.TableOption.size(); i++)
	{
		if (m_GameServiceManager->RectifyParameter(m_GameServiceOption, m_GameServiceTableOption.TableOption.at(i)) == false)
		{
			CTraceService::TraceString(TEXT("��Ϸģ��������ò���ʧ��"), TraceLevel_Exception);
			return false;
		}
	}


	//��������
	if ((m_GameServiceOption.wServerType&m_GameServiceAttrib.wSupporType)==0)
	{
		CTraceService::TraceString(TEXT("��Ϸģ�鲻֧��������ָ���ķ���ģʽ����"),TraceLevel_Exception);
		return false;
	}

	//ռλ����
	if (m_GameServiceAttrib.wChairCount==MAX_CHAIR)
	{
		CServerRule::SetAllowAndroidSimulate(m_GameServiceOption.dwServerRule,false);
	}

	//����ģʽ
	if ((m_GameServiceOption.cbDistributeRule&DISTRIBUTE_ALLOW)!=0)
	{
		//��������
		CServerRule::SetAllowAvertCheatMode(m_GameServiceOption.dwServerRule,true);

		//��������
		m_GameServiceOption.wMinDistributeUser=__max(m_GameServiceAttrib.wChairCount,m_GameServiceOption.wMinDistributeUser);

		//�������
		if (m_GameServiceOption.wMaxDistributeUser!=0)
		{
			m_GameServiceOption.wMaxDistributeUser=__max(m_GameServiceOption.wMaxDistributeUser,m_GameServiceOption.wMinDistributeUser);
		}
	}

	//��Ϸ��¼
	if (m_GameServiceOption.wServerType&(GAME_GENRE_GOLD/*|GAME_GENRE_MATCH*/))
	{
		CServerRule::SetRecordGameScore(m_GameServiceOption.dwServerRule,true);
	}

	//��ʱд��
	if (m_GameServiceOption.wServerType&(GAME_GENRE_GOLD/*|GAME_GENRE_MATCH*/))
	{
		CServerRule::SetImmediateWriteScore(m_GameServiceOption.dwServerRule,true);
	}

	//�ҽ�����
	if (m_GameServiceOption.wSortID==0) m_GameServiceOption.wSortID=500;
	if (m_GameServiceOption.wKindID==0) m_GameServiceOption.wKindID=m_GameServiceAttrib.wKindID;

	//�������
	WORD wMaxPlayer=m_GameServiceOption.wTableCount*m_GameServiceAttrib.wChairCount;
	m_GameServiceOption.wMaxPlayer=__max(m_GameServiceOption.wMaxPlayer,wMaxPlayer+RESERVE_USER_COUNT);

	//��С����
	if (m_GameServiceOption.wServerType&GAME_GENRE_GOLD)
	{
#ifndef _READ_TABLE_OPTION_
		m_GameServiceOption.lMinTableScore+=m_GameServiceOption.lServiceScore;
		m_GameServiceOption.lMinTableScore=__max(m_GameServiceOption.lMinTableScore,m_GameServiceOption.lServiceScore);
#else
// 		for (int i = 0; i < CountArray(m_GameServiceOption.TableOption); i++)
// 		{
// 			m_GameServiceOption.TableOption[i].lMinTableScore += m_GameServiceOption.TableOption[i].lServiceScore;
// 			m_GameServiceOption.TableOption[i].lMinTableScore = __max(m_GameServiceOption.TableOption[i].lMinTableScore, m_GameServiceOption.TableOption[i].lServiceScore);
// 		}
		for (int i = 0; i < CountArray(m_GameServiceTableOption.TableOption); i++)
		{
			m_GameServiceTableOption.TableOption[i].lMinTableScore += m_GameServiceTableOption.TableOption[i].lServiceScore;
			m_GameServiceTableOption.TableOption[i].lMinTableScore = __max(m_GameServiceTableOption.TableOption[i].lMinTableScore, m_GameServiceTableOption.TableOption[i].lServiceScore);
		}
#endif
	}

	//���Ƶ���
	if (m_GameServiceOption.lMaxEnterScore!=0L)
	{
#ifndef _READ_TABLE_OPTION_
		m_GameServiceOption.lMaxEnterScore=__max(m_GameServiceOption.lMaxEnterScore,m_GameServiceOption.lMinTableScore);
#else
// 		for (int i = 0; i < CountArray(m_GameServiceOption.TableOption); i++)
// 		{
// 			m_GameServiceOption.lMaxEnterScore = __max(m_GameServiceOption.lMaxEnterScore, m_GameServiceOption.TableOption[i].lMinTableScore);
// 		}
		for (int i = 0; i < CountArray(m_GameServiceTableOption.TableOption); i++)
		{
			m_GameServiceOption.lMaxEnterScore = __max(m_GameServiceOption.lMaxEnterScore, m_GameServiceTableOption.TableOption[i].lMinTableScore);
		}
#endif
	}

	//��������
	if (m_GameMatchServiceManager.GetInterface()!=NULL)
	{
		if (m_GameMatchServiceManager->RectifyServiceOption(&m_GameServiceOption, &m_GameServiceAttrib) == false)
		{
			CTraceService::TraceString(TEXT("����������������쳣"), TraceLevel_Exception);
			return false;
		}
	}

	return true;
}

//����״̬
bool CServiceUnits::SetServiceStatus(enServiceStatus ServiceStatus)
{
	//״̬�ж�
	if (m_ServiceStatus!=ServiceStatus)
	{
		//����֪ͨ
		if ((m_ServiceStatus!=ServiceStatus_Service)&&(ServiceStatus==ServiceStatus_Stop))
		{
			LPCTSTR pszString=TEXT("��������ʧ��");
			CTraceService::TraceString(pszString,TraceLevel_Exception);
		}

		//���ñ���
		m_ServiceStatus=ServiceStatus;

		//״̬֪ͨ
		ASSERT(m_pIServiceUnitsSink!=NULL);
		if (m_pIServiceUnitsSink!=NULL) m_pIServiceUnitsSink->OnServiceUnitsStatus(m_ServiceStatus);
	}

	return true;
}

//���Ϳ���
bool CServiceUnits::SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	ASSERT(m_AttemperEngine.GetInterface()!=NULL);
	if (m_AttemperEngine.GetInterface()==NULL) return false;

	//���Ϳ���
	m_AttemperEngine->OnEventControl(wControlID,pData,wDataSize);

	return true;
}

//������Ϣ
bool CServiceUnits::LoadDataBaseParameter(LPCTSTR pszDataBaseAddr, LPCTSTR pszDataBaseName, tagDataBaseParameter & DataBaseParameter)
{
	LOGI("CServiceUnits::LoadDataBaseParameter, pszDataBaseAddr="<<pszDataBaseAddr<<", pszDataBaseName="<<pszDataBaseName);
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//��������
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	tagDataBaseParameter * pPlatformDBParameter=&m_InitParameter.m_PlatformDBParameter;

	//��������
	PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
		pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

	//��ȡ��Ϣ
	try
	{
		//��������
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//������Ϣ
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@strDataBaseAddr"),pszDataBaseAddr);

		//ִ�в�ѯ
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadDataBaseInfo"),true)!=DB_SUCCESS)
		{
			//������Ϣ
			TCHAR szErrorDescribe[128]=TEXT("");
			PlatformDBAide.GetValue_String(TEXT("ErrorDescribe"),szErrorDescribe,CountArray(szErrorDescribe));

			//��ʾ��Ϣ
			CTraceService::TraceString(szErrorDescribe,TraceLevel_Exception);
			LOGI("CServiceUnits::LoadDataBaseParameter, GSP_GS_LoadDataBaseInfo, "<<szErrorDescribe);

			return false;
		}

		//��ȡ����
		TCHAR szDBUserRead[512]=TEXT(""),szDBPassRead[512]=TEXT("");
		PlatformDBAide.GetValue_String(TEXT("DBUser"),szDBUserRead,CountArray(szDBUserRead));
		PlatformDBAide.GetValue_String(TEXT("DBPassword"),szDBPassRead,CountArray(szDBPassRead));

		//��ȡ��Ϣ
		DataBaseParameter.wDataBasePort=PlatformDBAide.GetValue_WORD(TEXT("DBPort"));
		lstrcpyn(DataBaseParameter.szDataBaseAddr,pszDataBaseAddr,CountArray(DataBaseParameter.szDataBaseAddr));
		lstrcpyn(DataBaseParameter.szDataBaseName,pszDataBaseName,CountArray(DataBaseParameter.szDataBaseName));

		//��������
		TCHAR szDataBaseUser[32]=TEXT(""),szDataBasePass[32]=TEXT("");
		CWHEncrypt::XorCrevasse(szDBUserRead,DataBaseParameter.szDataBaseUser,CountArray(DataBaseParameter.szDataBaseUser));
		CWHEncrypt::XorCrevasse(szDBPassRead,DataBaseParameter.szDataBasePass,CountArray(DataBaseParameter.szDataBasePass));
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
		LOGI("CServiceUnits::LoadDataBaseParameter, "<<pszDescribe);

		return false;
	}

	return true;
}

//������Ϣ
LRESULT CServiceUnits::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//��������
	tagDataHead DataHead;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];

	//��ȡ����
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.DistillData(DataHead,cbBuffer,sizeof(cbBuffer))==false)
	{
		ASSERT(FALSE);
		return NULL;
	}

	//���ݴ���
	switch (DataHead.wIdentifier)
	{
	case UI_CORRESPOND_RESULT:		//Э���ɹ�
		{
			//Ч����Ϣ
			ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//��������
			CP_ControlResult * pControlResult=(CP_ControlResult *)cbBuffer;

			//ʧ�ܴ���
			if ((m_ServiceStatus!=ServiceStatus_Service)&&(pControlResult->cbSuccess==ER_FAILURE))
			{
				ConcludeService();
				return 0;
			}

			//�ɹ�����
			if ((m_ServiceStatus!=ServiceStatus_Service)&&(pControlResult->cbSuccess==ER_SUCCESS))
			{
				//����״̬
				SetServiceStatus(ServiceStatus_Service);
			}

			return 0;
		}
	case UI_SERVICE_CONFIG_RESULT:	//���ý��
		{
			//Ч����Ϣ
			ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//��������
			CP_ControlResult * pControlResult=(CP_ControlResult *)cbBuffer;

			//ʧ�ܴ���
			if ((m_ServiceStatus!=ServiceStatus_Service)&&(pControlResult->cbSuccess==ER_FAILURE))
			{
				ConcludeService();
				return 0;
			}

			//�ɹ�����
			if ((m_ServiceStatus!=ServiceStatus_Service)&&(pControlResult->cbSuccess==ER_SUCCESS))
			{
				//��������
				if (StartNetworkService()==false)
				{
					ConcludeService();
					return 0;
				}

				//����Э��
				SendControlPacket(CT_CONNECT_CORRESPOND,NULL,0);
			}

			return 0;
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
