#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

#include <math.h>
#include <algorithm>
#include <vector>
using namespace std;

// ��ӡ�ڴ�
#ifdef _DEBUG

#include <psapi.h>  
#pragma comment(lib,"psapi.lib")  

#endif // _DEBUG

//////////////////////////////////////////////////////////////////////////////////
//ʱ���ʶ

#define IDI_LOAD_ANDROID_USER		(IDI_MAIN_MODULE_START+1)			//������Ϣ
#define IDI_REPORT_SERVER_INFO		(IDI_MAIN_MODULE_START+2)			//������Ϣ
#define IDI_CONNECT_CORRESPOND		(IDI_MAIN_MODULE_START+3)			//����ʱ��
#define IDI_GAME_SERVICE_PULSE		(IDI_MAIN_MODULE_START+4)			//��������
#define IDI_DISTRIBUTE_ANDROID		(IDI_MAIN_MODULE_START+5)			//�������
#define IDI_DBCORRESPOND_NOTIFY		(IDI_MAIN_MODULE_START+6)			//����֪ͨ
#define IDI_LOAD_SYSTEM_MESSAGE		(IDI_MAIN_MODULE_START+7)			//ϵͳ��Ϣ
#define IDI_SEND_SYSTEM_MESSAGE		(IDI_MAIN_MODULE_START+8)			//ϵͳ��Ϣ
#define IDI_LOAD_SENSITIVE_WORD		(IDI_MAIN_MODULE_START+9)			//�������д�
#define IDI_DISTRIBUTE_USER		    (IDI_MAIN_MODULE_START+10)			//�����û�
#define IDI_ON_ANDROID_SEND_TRUMPT  (IDI_MAIN_MODULE_START+11)
#define IDI_LOAD_BALANCE_SCORE_CURVE (IDI_MAIN_MODULE_START+12)			//����ƽ�������
#define IDI_SQUARE_MATCH_ONTIMER	(IDI_MAIN_MODULE_START+13)			//�㳡����ʱ��

//////////////////////////////////////////////////////////////////////////////////
//ʱ�䶨�� ��

#define TIME_LOAD_ANDROID_USER		600L								//���ػ���
#define TIME_DISTRIBUTE_ANDROID		2L									//�����û�
#define TIME_REPORT_SERVER_INFO		30L									//�ϱ�ʱ��
#define TIME_DBCORRESPOND_NOTIFY	3L									//����֪ͨʱ��
#define TIME_LOAD_SYSTEM_MESSAGE	900L								//ϵͳ��Ϣʱ��
#define TIME_SEND_SYSTEM_MESSAGE	10L								    //ϵͳ��Ϣʱ��
#define TIME_LOAD_SENSITIVE_WORD	5L									//�������д�ʱ��
#define TIME_LOAD_BALANCE_SCORE_CURVE	3L								// ����ƽ�������

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_bCollectUser=false;
	m_bNeekCorrespond=true;
	m_nAndroidUserMessageCount=0;
	m_nAndroidUserSendedCount=0;

	//������
	m_pNormalParameter=NULL;
	m_pAndroidParameter=NULL;

	//״̬����
	m_pInitParameter=NULL;
	m_pGameParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceTableOption = NULL;
	m_cbGameSquareOptionArrayCount=0;
// 	for(BYTE i=0; i<20; i++)memset(&m_GameSquareOptionArray[i],0,sizeof(m_GameSquareOptionArray[i]));

	//�������
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_WaitDistributeList.RemoveAll();

	//��������
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;
	m_pIDBCorrespondManager=NULL;

	//��������
	ZeroMemory(&m_DataConfigColumn,sizeof(m_DataConfigColumn));
	ZeroMemory(&m_DataConfigProperty,sizeof(m_DataConfigProperty));
	for(int i=0; i<20; i++)m_pGameSquareOptionArray[i]=NULL;

	//��������
	m_pIGameMatchServiceManager=NULL;

	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
	//ɾ������
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//ɾ������
	for (size_t i=0;i<m_TableFrameArray.size();i++)
	{
		SafeRelease(m_TableFrameArray[i]);
	}
	m_TableFrameArray.clear();

	//��������
	m_KickUserItemMap.RemoveAll();
	ClearSystemMessageData();
	m_WaitDistributeList.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMainServiceFrame,Guid,dwQueryVer);
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//����Ϣ
	m_pAndroidParameter=new tagBindParameter[MAX_ANDROID];
	ZeroMemory(m_pAndroidParameter,sizeof(tagBindParameter)*MAX_ANDROID);

	//����Ϣ
	m_pNormalParameter=new tagBindParameter[m_pGameServiceOption->wMaxPlayer];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*m_pGameServiceOption->wMaxPlayer);

	//���û���
	if (InitAndroidUser()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (InitTableFrameArray()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//���ù㳡��
	if (InitSquareMatch()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//���ýӿ�
	if (m_ServerUserManager.SetServerUserItemSink(QUERY_ME_INTERFACE(IServerUserItemSink))==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_AndroidUserManager.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf(szIniFile,CountArray(szIniFile),TEXT("%s\\AndroidMessageConfig.ini"),szWorkDir);

	//��ȡ����
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);
	
	int nAndroidMessageTimeCount=10000; 
	nAndroidMessageTimeCount=IniData.ReadInt(TEXT("Time"),TEXT("Elapse"),10000);
	m_nAndroidUserMessageCount=IniData.ReadInt(TEXT("MessageCount"),TEXT("Count"),0);
	m_pITimerEngine->SetTimer(IDI_ON_ANDROID_SEND_TRUMPT,nAndroidMessageTimeCount*1000,TIMES_INFINITY,NULL);


	//����ʱ��
	m_pITimerEngine->SetTimer(IDI_GAME_SERVICE_PULSE,1000L,TIMES_INFINITY,NULL);

#ifdef _DEBUG
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_USER,10000L,TIMES_INFINITY,NULL);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_USER,TIME_LOAD_ANDROID_USER*1000L,TIMES_INFINITY,NULL);
#endif

	m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ANDROID,TIME_DISTRIBUTE_ANDROID*1000L,TIMES_INFINITY,NULL);

	m_pITimerEngine->SetTimer(IDI_DBCORRESPOND_NOTIFY,TIME_DBCORRESPOND_NOTIFY*1000L,TIMES_INFINITY,NULL);

#ifdef _DEBUG
	m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE,15*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,5*1000L,TIMES_INFINITY,NULL);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE,TIME_LOAD_SYSTEM_MESSAGE*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,TIME_SEND_SYSTEM_MESSAGE*1000L,TIMES_INFINITY,NULL);
#endif

	//��ʱ�������д�
	m_pITimerEngine->SetTimer(IDI_LOAD_SENSITIVE_WORD,TIME_LOAD_SENSITIVE_WORD*1000L,TIMES_INFINITY,NULL);

	m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_USER,10*1000,TIMES_INFINITY,NULL);

	// ��ʱ����ƽ�������
	// m_pITimerEngine->SetTimer(IDI_LOAD_BALANCE_SCORE_CURVE,TIME_LOAD_BALANCE_SCORE_CURVE*1000L,TIMES_INFINITY,NULL);

	//�㳡����ʼ��ʱ
	m_pITimerEngine->SetTimer(IDI_SQUARE_MATCH_ONTIMER,1000L,TIMES_INFINITY,NULL);
	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//״̬����
	m_bCollectUser=false;
	m_bNeekCorrespond=true;

	//������Ϣ
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pGameServiceTableOption = NULL;
// 	for(BYTE i=0; i<20; i++)memset(&m_GameSquareOptionArray[i],0,sizeof(m_GameSquareOptionArray[i]));

	//�������
	m_pITimerEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;

	//��������
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;

	//������
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//ɾ������
	for (auto &iter : m_TableFrameArray)
	{
		SafeRelease(iter);
	}

	//ɾ���û�
	m_TableFrameArray.clear();
	m_ServerUserManager.DeleteUserItem();
	m_ServerListManager.ResetServerList();
	m_WaitDistributeList.RemoveAll();
	
	//ֹͣ����
	m_AndroidUserManager.ConcludeService();

	//ֹͣ����
	if(m_pIGameMatchServiceManager!=NULL)
		m_pIGameMatchServiceManager->StopService();

	//�����Ϣ����
	ClearSystemMessageData();

	//��λ�ؼ���
	m_WordsFilter.ResetSensitiveWordArray();

	// ��λƽ�����������
	m_BalanceScoreCurve.ResetBalanceScoreCurve();

	// ���������Ϸ������
	if (m_pIDBCorrespondManager)
	{
		m_pIDBCorrespondManager->PostDataBaseRequest(0L, DBR_GR_RESET_GAME_SCORE_LOCKER, 0L, NULL, 0L);
	}

	//ֹͣ�㳡��
	m_SquareManager.ConcludeService();

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_CONNECT_CORRESPOND:		//����Э��
		{
			//��������
			tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
			m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//������ʾ
			TCHAR szString[512]=TEXT("");
			_sntprintf(szString,CountArray(szString),TEXT("��������Э�������� [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString,TraceLevel_Normal);
			LOGI("CAttemperEngineSink::OnEventControl, "<<szString);

			return true;
		}
	case CT_LOAD_SERVICE_CONFIG:	//��������
		{
			//��������
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_PARAMETER,0L,NULL,0L);

			//�����б�
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_GAME_COLUMN,0L,NULL,0L);

			//���ػ���
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_USER,0L,NULL,0L);

			//���ص���
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_GAME_PROPERTY,0L,NULL,0L);

			//������Ϣ
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_SYSTEM_MESSAGE,0L,NULL,0L);

			// ������Ϸ������
			m_pIDBCorrespondManager->PostDataBaseRequest(0L, DBR_GR_RESET_GAME_SCORE_LOCKER, 0L, NULL, 0L);

			return true;
		}
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

struct TableValue
{
	WORD wTableID;
	int dwScore;
};

bool sortbyscore(TableValue t1, TableValue t2)
{	
	if (t1.dwScore != t2.dwScore)
		return t1.dwScore > t2.dwScore;
	
	return (bool)(rand() % 2);
}

WORD CAttemperEngineSink::GetAndriodTable(IAndroidUserItem * pIAndroidUserItem)
{
//	LOGI("CAttemperEngineSink::GetAndriodTable");
	if (pIAndroidUserItem == NULL)
		return INVALID_TABLE;

	//ÿ�����ӵ���������
	vector<TableValue> valscores;

	bool bAllowDynamicJoin=CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule);
	for (size_t i = 0; i < m_TableFrameArray.size(); i++)
	{
		TableValue tv;
		tv.wTableID = i;
		tv.dwScore = 0;

		//��ȡ����
		CTableFrame * pTableFrame = m_TableFrameArray.at(i);
		if (pTableFrame->IsGameStarted()&&(!bAllowDynamicJoin))
		{
//			LOGI("CAttemperEngineSink::GetAndriodTable, IsGameStarted&&!bAllowDynamicJoin wTableID="<<tv.wTableID<<", dwScore="<<tv.dwScore);
			valscores.push_back(tv);
			continue;
		}

		//����״��
		tagTableUserInfo TableUserInfo;
		WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
		WORD wChairCount=pTableFrame->GetChairCount();

//		LOGI("CAttemperEngineSink::GetAndriodTable, wUserSitCount="<<wUserSitCount<<", wChairCount="<<wChairCount);
		if (wUserSitCount==wChairCount)
		{
//			LOGI("CAttemperEngineSink::GetAndriodTable, wTableID="<<tv.wTableID<<", dwScore="<<tv.dwScore);
			valscores.push_back(tv);
			continue;
		}

		WORD	wAndriodCount = 0;
		SCORE   sScore = 0;
		for (int j = 0; j < wChairCount; j++)
		{
			IServerUserItem* pUserItem = pTableFrame->GetTableUserItem(j);
			if (pUserItem==NULL)
				continue;
			sScore+=pUserItem->GetUserScore();

//			LOGI("CAttemperEngineSink::GetAndriodTable, j="<<j<<", sScore="<<sScore);
			if (pUserItem->IsAndroidUser())
				wAndriodCount++;
		}
//		LOGI("CAttemperEngineSink::GetAndriodTable, wAndriodCount="<<wAndriodCount);
		WORD ScoreVal = 0;  //������������
		if (wUserSitCount != 0)
		{
			sScore /= wUserSitCount;
//			LOGI("CAttemperEngineSink::GetAndriodTable, sScore="<<sScore);
			ScoreVal = (WORD)(log(abs(double(pIAndroidUserItem->GetMeUserItem()->GetUserScore() - sScore)))) * 10;

//			LOGI("CAttemperEngineSink::GetAndriodTable, ScoreVal="<<ScoreVal);
			if (ScoreVal > 5)
				ScoreVal = 5;
			if (ScoreVal <= 0)
				ScoreVal = 0;

			ScoreVal *= 5;
		}

        //����Խ��Խ�л��ᱻ���䵽
		//��λ��ǰԽ�л��ᱻ���䵽
		//������Խ��Խ���ᱻ���䵽
		tv.dwScore = 100 + wUserSitCount * 50 + (m_pGameServiceOption->wTableCount / 2) - wAndriodCount * 20 - ScoreVal;
		if (tv.dwScore < 0)
		{
			ASSERT(0);
			tv.dwScore = 0;
		}
//		LOGI("CAttemperEngineSink::GetAndriodTable, wTableID="<<tv.wTableID<<", dwScore="<<tv.dwScore);
		valscores.push_back(tv);
	}

	int maxscore = -1;
	WORD maxidx = INVALID_TABLE;
	for (int i = 0; i < (int)valscores.size(); i++)
	{
		int score = 0;
		if (valscores[i].dwScore == 0)
			score = 0;
		else
		{
//			score = rand() % valscores[i].dwScore;

			// �˴�����������÷�����ľ�һ���ᰲ�ŵķ���
			score = valscores[i].dwScore; 
		}

		if (score > maxscore)
		{
			maxscore = score;
			maxidx = i;
		}
	}

	//add by wangl 20150303
	//�����������<=0��˵��û���ʺϵ����ӷ���
	if(maxscore<=0)
	{
		maxidx=INVALID_TABLE;
	}

	return maxidx; 
};

int RandIndex(vector<int> all)
{
	if (all.size() == 0)
		return -1;

	if (all.size() == 1)
		return 0;

	int total = 0;
	vector<int> alltmp;
	alltmp.push_back(0); 
	for (size_t i = 0; i < all.size(); i++)
	{
		total += all[i];
		alltmp.push_back(total);
	}

	int rnd = rand() % total;
	for (size_t i = 0; i < alltmp.size() - 1; i++)
	{
		if (rnd >= alltmp[i] && rnd < alltmp[i+1])
		{
			return (int)i;
		}
	}

	return -1;
};

//ʱ���¼�
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
//	LOGI("CAttemperEngineSink::OnEventTimer, dwTimerID=" << dwTimerID);
// #ifdef _DEBUG
// 	HANDLE handle = GetCurrentProcess();
// 	PROCESS_MEMORY_COUNTERS pmc;
// 	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 	LOGD("CAttemperEngineSink::OnEventTimer " << dwTimerID << " Before, WorkingSetSize = " << (pmc.WorkingSetSize));
// #endif
	//����ʱ��
	try
	{
		if ((dwTimerID>=IDI_MAIN_MODULE_START)&&(dwTimerID<=IDI_MAIN_MODULE_FINISH))
		{
			//ʱ�䴦��
			switch (dwTimerID)
			{
			case IDI_LOAD_ANDROID_USER:		//���ػ���
				{
					//���ػ���
					m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_USER,0L,NULL,0L);
// #ifdef _DEBUG
// 					GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 					LOGD("CAttemperEngineSink::OnEventTimer " << dwTimerID << " After, WorkingSetSize = " << (pmc.WorkingSetSize));
// #endif
					return true;
				}
			case IDI_ON_ANDROID_SEND_TRUMPT:
				{
					OnTCPAndroidSendTrumpet();
// #ifdef _DEBUG
// 					GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 					LOGD("CAttemperEngineSink::OnEventTimer " << dwTimerID << " After, WorkingSetSize = " << (pmc.WorkingSetSize));
// #endif
					return true;
				}
			case IDI_REPORT_SERVER_INFO:	//������Ϣ
				{
					//��������
					CMD_CS_C_ServerOnLine ServerOnLine;
					ZeroMemory(&ServerOnLine,sizeof(ServerOnLine));

					//���ñ���
					ServerOnLine.dwOnLineCount=m_ServerUserManager.GetUserItemCount();

					//��������
					m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_C_SERVER_ONLINE,&ServerOnLine,sizeof(ServerOnLine));
// #ifdef _DEBUG
// 					GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 					LOGD("CAttemperEngineSink::OnEventTimer " << dwTimerID << " After, WorkingSetSize = " << (pmc.WorkingSetSize));
// #endif
					return true;
				}
			case IDI_CONNECT_CORRESPOND:	//����Э��
				{
					//��������
					tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
					m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

					//������ʾ
					TCHAR szString[512]=TEXT("");
					_sntprintf(szString,CountArray(szString),TEXT("��������Э�������� [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

					//��ʾ��Ϣ
					CTraceService::TraceString(szString,TraceLevel_Normal);
					LOGI("CAttemperEngineSink::OnEventTimer, "<<szString);
// #ifdef _DEBUG
// 					GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 					LOGD("CAttemperEngineSink::OnEventTimer " << dwTimerID << " After, WorkingSetSize = " << (pmc.WorkingSetSize));
// #endif
					return true;
				}
			case IDI_GAME_SERVICE_PULSE:	//����ά��
				{
					return true;
				}
			case IDI_DISTRIBUTE_ANDROID:	//�������
				{
					//��������
					if (m_AndroidUserManager.GetAndroidCount()>0 && m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
					{
						//��������
						bool bAllowDynamicJoin=CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule);
						bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
						bool bAllowAndroidSimulate=CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);
						bool bAllowAvertCheatMode=(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR));

						//ģ�⴦��
						//���������ռλ
						if (bAllowAndroidSimulate==true && bAllowAvertCheatMode==false)
						{
							//����״̬
							tagAndroidUserInfo AndroidSimulate; 
							m_AndroidUserManager.GetAndroidUserInfo(AndroidSimulate,ANDROID_SIMULATE);

							//��������
							if (AndroidSimulate.wFreeUserCount>0)
							{
 								for (WORD ii=0;ii<1;ii++)   // �����벻��ȷ��˼��ɾ����
 								{
									//������ӣ���ǰ1/4����λѡ��
									WORD wTableID=rand()%(__max(m_pGameServiceOption->wTableCount/3,1));

									//��ȡ����
									CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
									if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

									//����״��
									tagTableUserInfo TableUserInfo;
									WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

									//�����ж�
									if (TableUserInfo.wTableUserCount>0) continue;
									if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) continue;

									vector<int> scores; //��λ������
									for (int i = 0; i <= m_pGameServiceAttrib->wChairCount - TableUserInfo.wMinUserCount; i++)
									{
										int want = m_pGameServiceAttrib->wChairCount - i;
										scores.push_back(want * want);		
									}

									WORD wWantAndroidCount=m_pGameServiceAttrib->wChairCount-RandIndex(scores);
									//�����ж�
									if (AndroidSimulate.wFreeUserCount>=wWantAndroidCount)
									{
										//��������
										WORD wHandleCount=0;

										//���´���
										for (WORD j=0;j<AndroidSimulate.wFreeUserCount;j++)
										{
											//��������
											WORD wChairID=pTableFrame->GetRandNullChairID();

											//��Ч����
											//ASSERT(wChairID!=INVALID_CHAIR);
											if (wChairID==INVALID_CHAIR) continue;

											//�û�����
											IAndroidUserItem * pIAndroidUserItem=AndroidSimulate.pIAndroidUserFree[j];
											if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true)
											{
												//���ñ���
												wHandleCount++;

												//����ж�
												if (wHandleCount>=wWantAndroidCount) 
												{
													return true;
												}
											}
										}

										if(wHandleCount > 0) return true;
									}
								}
							}
						}

						//������������
						if (bAllowAndroidAttend==true)
						{
							LOGI("CAttemperEngineSink::OnEventTimer, IDI_DISTRIBUTE_ANDROID, ANDROID_PASSIVITY");
							//����״̬
							tagAndroidUserInfo AndroidPassivity;
							m_AndroidUserManager.GetAndroidUserInfo(AndroidPassivity,ANDROID_PASSIVITY);

							if(bAllowAvertCheatMode)
							{
								//���´���
								for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
								{
									IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
									if (InsertWaitDistribute(pIAndroidUserItem->GetMeUserItem())==true) return true;
								}
							}
							else
							{
								//��������
								if (AndroidPassivity.wFreeUserCount>0)
								{
									//������Ϸ
									if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
									{
										for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
										{
											//��ȡ����
											CTableFrame * pTableFrame=m_TableFrameArray[i];
											if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false))
											{
												LOGI("CAttemperEngineSink::OnEventTimer, pTableFrame->IsGameStarted()==true&&bAllowDynamicJoin==false");
												continue;
											}

											//����״��
											tagTableUserInfo TableUserInfo;
											WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

											//�����ж�
											if (wUserSitCount>m_pGameServiceAttrib->wChairCount*2/3)
											{
												LOGI("CAttemperEngineSink::OnEventTimer, wFreeUserCount="<<AndroidPassivity.wFreeUserCount<<", wChairCount="<<m_pGameServiceAttrib->wChairCount);
												continue;
											}

											//��������
											IServerUserItem * pIServerUserItem=NULL;
											WORD wChairID=pTableFrame->GetRandNullChairID();

											//��Ч����
											ASSERT(wChairID!=INVALID_CHAIR);
											if (wChairID==INVALID_CHAIR)
											{
												LOGI("CAttemperEngineSink::OnEventTimer, wChairID==INVALID_CHAIR");
												continue;
											}

											//���´���
											for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
											{
												IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
												if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true)
												{
													// ��return ��������
													return true;
												}
											}
										}
									}
									else
									{
										//���´���
										for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
										{
											IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
											if (pIAndroidUserItem == NULL)
												continue;

											WORD wTableID = GetAndriodTable(pIAndroidUserItem);
											if (wTableID == INVALID_TABLE)
											{
												LOGI("CAttemperEngineSink::OnEventTimer, wTableID="<<wTableID);
												continue;
											}

											CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
											if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false))
											{
												LOGI("CAttemperEngineSink::OnEventTimer, pTableFrame->IsGameStarted()==true&&bAllowDynamicJoin==false");
												continue;
											}

											//����״��
											tagTableUserInfo TableUserInfo;
											WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

											//�����ж�
											if (wUserSitCount==0) 
											{
												LOGI("CAttemperEngineSink::OnEventTimer, wUserSitCount==0");
												continue;
											}
											if (TableUserInfo.wTableUserCount==0) continue;
											if ((wUserSitCount>=TableUserInfo.wMinUserCount))
											{
												LOGI("CAttemperEngineSink::OnEventTimer, wUserSitCount=="<<wUserSitCount<<", TableUserInfo.wMinUserCount="<<TableUserInfo.wMinUserCount);
												continue;
											}

											WORD wChairID=pTableFrame->GetRandNullChairID();
											if (wChairID==INVALID_CHAIR)
											{
												LOGI("CAttemperEngineSink::OnEventTimer, wChairID=INVALID_CHAIR");
												continue;
											}

											if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true)
											{
												// �����أ���������
												return true;
//												break;
											}
										}
									}
								}
							}
						}

						//������������
						if (bAllowAndroidAttend==true)
						{
							LOGI("CAttemperEngineSink::OnEventTimer, IDI_DISTRIBUTE_ANDROID, ANDROID_INITIATIVE");
							//����״̬
							tagAndroidUserInfo AndroidInitiative;
							m_AndroidUserManager.GetAndroidUserInfo(AndroidInitiative,ANDROID_INITIATIVE);
							WORD wAllAndroidCount = AndroidInitiative.wFreeUserCount+AndroidInitiative.wPlayUserCount+AndroidInitiative.wSitdownUserCount;

							if(bAllowAvertCheatMode)
							{
								//���´���(������ģʽ)
								for (WORD j=0;j<AndroidInitiative.wFreeUserCount;j++)
								{
									IAndroidUserItem * pIAndroidUserItem=AndroidInitiative.pIAndroidUserFree[j];
									if (InsertWaitDistribute(pIAndroidUserItem->GetMeUserItem())==true) return true;
								}
							}
							else
							{
//								LOGI("CAttemperEngineSink::OnEventTimer, wFreeUserCount="<<AndroidInitiative.wFreeUserCount);
								//���´���
								for (WORD j=0;j<AndroidInitiative.wFreeUserCount;j++)
								{
									IAndroidUserItem * pIAndroidUserItem=AndroidInitiative.pIAndroidUserFree[j];
									if (pIAndroidUserItem == NULL)
									{
										LOGI("CAttemperEngineSink::OnEventTimer, pIAndroidUserItem==NULL");
										continue;
									}

									// ���ݵ�ǰ��ҵ���������,�ж��Ƿ��������
									tagAndroidService* AndroidService=pIAndroidUserItem->GetAndroidService();
									tagAndroidParameter* AndroidParameter=pIAndroidUserItem->GetAndroidParameter();
									if (AndroidService == NULL || AndroidParameter==NULL)
									{
										LOGI("CAttemperEngineSink::OnEventTimer, AndroidService==NULL || AndroidParameter == NULL");
										continue;
									}
									if (AndroidService->dwSitRate > AndroidParameter->dwSitRate)
									{
										LOGI("CAttemperEngineSink::OnEventTimer, AndroidService->dwSitRate="<<AndroidService->dwSitRate<<", AndroidParameter->dwSitRate="<<AndroidParameter->dwSitRate);
										continue;
									}

									WORD wTableID = GetAndriodTable(pIAndroidUserItem);
									if (wTableID == INVALID_TABLE)
									{
										LOGI("CAttemperEngineSink::OnEventTimer, wTableID="<<wTableID);
										continue;
									}

									CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
									if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false))
									{
										LOGI("CAttemperEngineSink::OnEventTimer, pTableFrame->IsGameStarted()==true&&bAllowDynamicJoin==false");
										continue;
									}

									WORD wChairID=pTableFrame->GetRandNullChairID();

									//��Ч����
									ASSERT(wChairID!=INVALID_CHAIR);
									if (wChairID==INVALID_CHAIR)
									{
										LOGI("CAttemperEngineSink::OnEventTimer, wChairID==INVALID_CHAIR");
										continue;
									}

									if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true)
									{
										// �����أ���������
										return true;
//										break;
									}
								}
							}
						}

						//��������(��ʱ����)
// 						WORD wStandUpCount=0;
// 						WORD wRandCount=((rand()%3)+1);
// 						INT_PTR nIndex = rand()%(__max(m_pGameServiceOption->wTableCount,1));
// 						for (INT_PTR i=nIndex;i<m_pGameServiceOption->wTableCount+nIndex;++i)
// 						{
// 							//��ȡ����
// 							INT_PTR nTableIndex=i%m_pGameServiceOption->wTableCount;
// 							CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
// 							if (pTableFrame->IsGameStarted()==true) continue;
// 
// 							//����״��
// 							tagTableUserInfo TableUserInfo;
// 							WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
// 
// 							//�û�����
// 							bool bRand = ((rand()%100)>50);
// 							if (TableUserInfo.wTableAndroidCount==0) continue;
// 							if ((TableUserInfo.wTableUserCount>0)&&(bAllowAndroidAttend==true) && bRand) continue;
// 							if (TableUserInfo.wTableAndroidCount>=TableUserInfo.wMinUserCount && bRand) continue;
// 
// 							//��������
// 							for (WORD j=0;j<pTableFrame->GetChairCount();j++)
// 							{
// 								//��ȡ�û�
// 								IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
// 								if (pIServerUserItem==NULL) continue;
// 
// 								//�û�����
// 								if ((pIServerUserItem->IsAndroidUser()==true))
// 								{
// 									if ( (pIServerUserItem->GetUserStatus()==US_SIT) || (pIServerUserItem->GetUserStatus()==US_READY) || (pIServerUserItem->GetUserStatus()==US_LOOKON))
// 									{
// 										LOGI("CAttemperEngineSink::OnEventTimer PerformStandUpAction, NickName="<<pIServerUserItem->GetNickName());
// 										if (pTableFrame->PerformStandUpAction(pIServerUserItem)==true)
// 										{
// 											wStandUpCount++;
// 											if(wStandUpCount>=wRandCount)
// 												return true;
// 											else
// 												break;
// 										}
// 									}
// 								}
// 							}
// 						}
					}

					return true;
				}
			case IDI_DBCORRESPOND_NOTIFY: //���涨ʱ����
				{
					if (m_pIDBCorrespondManager)
					{
						m_pIDBCorrespondManager->OnTimerNotify();
					}
					return true;
				}
			case IDI_LOAD_SYSTEM_MESSAGE: //ϵͳ��Ϣ
				{
					//�����Ϣ����
					ClearSystemMessageData();

					//������Ϣ
					m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_SYSTEM_MESSAGE,0L,NULL,0L);
					return true;
				}
			case IDI_LOAD_SENSITIVE_WORD:	//�������д�
				{
					//Ͷ������
					m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_SENSITIVE_WORDS,0,NULL,0);	
					return true;
				}
			case IDI_SEND_SYSTEM_MESSAGE: //ϵͳ��Ϣ
				{
					//�����ж�
					if(m_SystemMessageList.GetCount()==0) return true;

					//ʱЧ�ж�
					DWORD dwCurrTime = (DWORD)time(NULL);
					POSITION pos = m_SystemMessageList.GetHeadPosition();
					while(pos != NULL)
					{
						//��ȡ��Ϣ
						POSITION tempPos = pos;
						tagSystemMessage *pTagSystemMessage = m_SystemMessageList.GetNext(pos);

						//ʱЧ�ж�
						if(pTagSystemMessage->SystemMessage.tConcludeTime < dwCurrTime)
						{
							m_SystemMessageList.RemoveAt(tempPos);
							delete pTagSystemMessage;
							continue;
						}

						//����ж�
						if(pTagSystemMessage->dwLastTime+pTagSystemMessage->SystemMessage.dwTimeRate < dwCurrTime)
						{
							//��������
							pTagSystemMessage->dwLastTime=dwCurrTime;

							//������Ϣ
							CMD_GR_SendMessage SendMessage = {};
							SendMessage.cbAllRoom = (pTagSystemMessage->SystemMessage.cbMessageID==100)?TRUE:FALSE;
							SendMessage.cbGame = (pTagSystemMessage->SystemMessage.cbMessageType==1)?TRUE:FALSE;
							SendMessage.cbRoom = (pTagSystemMessage->SystemMessage.cbMessageType==2)?TRUE:FALSE;
							if(pTagSystemMessage->SystemMessage.cbMessageType==3)
							{
								SendMessage.cbGame = TRUE;
								SendMessage.cbRoom = TRUE;
							}
							lstrcpyn(SendMessage.szSystemMessage,pTagSystemMessage->SystemMessage.szSystemMessage,CountArray(SendMessage.szSystemMessage));
							SendMessage.wChatLength = lstrlen(SendMessage.szSystemMessage)+1;

							//������Ϣ
							WORD wSendSize = sizeof(SendMessage)-sizeof(SendMessage.szSystemMessage)+CountStringBuffer(SendMessage.szSystemMessage);
							SendSystemMessage(&SendMessage,wSendSize);
						}
					}
					return true;
				}
			case IDI_DISTRIBUTE_USER: //�����û�
				{
					DistributeUserGame();
					return true;
				}
// 			case IDI_LOAD_BALANCE_SCORE_CURVE:	//ƽ�������
// 				{
// 					//Ͷ������
// 					m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_BALANCE_SCORE_CURVE, 0, NULL, 0);		
// 					return true;
// 				}
			case IDI_SQUARE_MATCH_ONTIMER:		//�㳡����ʱ��
				{
					m_SquareManager.OnTimer();
					return true;
				}
			}
		}
	}
	catch(...)
	{
		LOGI("CAttemperEngineSink::OnEventTimer catch, dwTimerID="<<dwTimerID);
	}

	try
	{
		//����ʱ��
		if ((dwTimerID>=IDI_REBOT_MODULE_START)&&(dwTimerID<=IDI_REBOT_MODULE_FINISH))
		{
			//ʱ�䴦��
			m_AndroidUserManager.OnEventTimerPulse(dwTimerID,wBindParam);
			return true;
		}
	}
	catch(...)
	{
		LOGI("CAttemperEngineSink::OnEventTimer catch, dwTimerID="<<dwTimerID);
	}

	try
	{
		//������ʱ��
		if((dwTimerID>=IDI_MATCH_MODULE_START)&&(dwTimerID<IDI_MATCH_MODULE_FINISH))
		{
			if (m_pIGameMatchServiceManager != NULL)
			{
				m_pIGameMatchServiceManager->OnEventTimer(dwTimerID, wBindParam);
			}
			return true;
		}
	}
	catch(...)
	{
		LOGI("CAttemperEngineSink::OnEventTimer catch, dwTimerID="<<dwTimerID);
	}

	
	try
	{
		//����ʱ��(��Ҫ�жϵ�ǰ��ģʽ,��ͬ��ģʽ,���ݸ���ͬ�Ķ�ʱ������)
		if ((dwTimerID>=IDI_TABLE_MODULE_START)&&(dwTimerID<=IDI_TABLE_MODULE_FINISH))
		{
			//���Ӻ���
			DWORD dwTableTimerID=dwTimerID-IDI_TABLE_MODULE_START;
			WORD wTableID=(WORD)(dwTableTimerID/TIME_TABLE_MODULE_RANGE);

			//ʱ��Ч��
			if (wTableID>=(WORD)m_TableFrameArray.size()) 
			{
				ASSERT(FALSE);
				return false;
			}

			//ʱ��֪ͨ
			CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
			bool flag = pTableFrame->OnEventTimer(dwTableTimerID%TIME_TABLE_MODULE_RANGE,wBindParam);
			return flag;
		}
	}
	catch(...)
	{
		LOGI("CAttemperEngineSink::OnEventTimer catch, dwTimerID="<<dwTimerID);
	}

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GR_LOGON_SUCCESS:			//��¼�ɹ�
		{
			return OnDBLogonSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_LOGON_FAILURE:			//��¼ʧ��
		{
			return OnDBLogonFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_PARAMETER:			//��Ϸ����
		{
			return OnDBGameParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_COLUMN_INFO:		//�б���Ϣ
		{
			return OnDBGameColumnInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GR_GAME_ANDROID_INFO:		//������Ϣ
		{
			return OnDBGameAndroidInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_PROPERTY_INFO:		//������Ϣ
		{
			return OnDBGamePropertyInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_INFO:		//������Ϣ
		{
			return OnDBUserInsureInfo(dwContextID,pData,wDataSize);
		}
    case DBR_GR_TRANS_RECORD:
        {
            return OnDBUserTransRecord(dwContextID,pData,wDataSize);
        }
	case DBO_GR_USER_INSURE_SUCCESS:	//���гɹ�
		{
			return OnDBUserInsureSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_FAILURE:	//����ʧ��
		{
			return OnDBUserInsureFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_USER_INFO:  //�û���Ϣ
		{
			return OnDBUserInsureUserInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_PROPERTY_SUCCESS:		//���߳ɹ�
		{
			return OnDBPropertySuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_PROPERTY_FAILURE:		//����ʧ��
		{
			return OnDBPropertyFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SYSTEM_MESSAGE_RESULT:  //ϵͳ��Ϣ
		{
			return OnDBSystemMessage(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SENSITIVE_WORDS:		//�������д�
		{
			return OnDBSensitiveWords(dwContextID,pData,wDataSize);
		}
	case DBO_GR_INSURE_TRANS_RECEIPT:	//ת�˻�ִ
		{
			return OnDBUserTransferReceipt(dwContextID, pData, wDataSize);
		}
	case DBO_GR_OPERATE_SUCCESS:		//�����ɹ�
		{
			return OnDBUserOperateSuccess(dwContextID, pData, wDataSize);
		}
	case DBO_GR_OPERATE_FAILURE:		//����ʧ��
		{	
			return OnDBUserOperateFailure(dwContextID, pData, wDataSize);
		}
	case DBO_GR_BALANCE_SCORE_CURVE:	//ƽ�������
		{
			return OnDBBalanceScoreCurve(dwContextID, pData, wDataSize);
		}
	case DBO_GR_USER_BALANCE_SCORE:		//���ƽ�������
		{
			return OnDBUserBalanceScore(dwContextID, pData, wDataSize);
		}
	case DBO_GR_SQUARE_REWARD:			//�㳡��������Ϣ
		{
			return OnDBSquareReward(dwContextID, pData, wDataSize);
		}
	case DBO_GR_LOAD_SQUARE_REWARD:			//���ع㳡������������Ϣ
		{
			return OnDBLoadSquareReward(dwContextID, pData, wDataSize);
		}
	case DBO_GR_CREATE_CUSTOMTABLE_SUCCESS:	// ����������������
		{
			return OnDBCreateCustomTableSuccess(dwContextID, pData, wDataSize);
		}
	case DBO_GR_CREATE_CUSTOMTABLE_FAILURE:	// ��������ʧ��
		{
			return OnDBCreateCustomTableFailed(dwContextID, pData, wDataSize);
		}
	case DBO_GR_DISMISS_CUSTOMTABLE_SUCCESS:	// ��ɢ���ӳɹ�
		{
			return OnDBDismissCustomTableSuccess(dwContextID, pData, wDataSize);
		}
	case DBO_GR_DISMISS_CUSTOMTABLE_FAILURE:	// ��ɢ����ʧ��
		{
			return OnDBDismissCustomTableFailed(dwContextID, pData, wDataSize);
		}
	case DBO_GR_JOIN_CUSTOMTABLE_SUCCESS:		// �������ӳɹ�
		{
			return OnDBJoinCustomTableSuccess(dwContextID, pData, wDataSize);
		}
	case DBO_GR_JOIN_CUSTOMTABLE_FAILURE:		// �������ӳɹ�
		{	
			return OnDBJoinCustomTableFailed(dwContextID, pData, wDataSize);
		}
	case DBO_GR_USER_ROUNDSCORE_IN_CUSTOMTABLE:	// ��ҷ���
		{
			return OnDBUserRoundScoreInCustomTable(dwContextID, pData, wDataSize);
		}
	}

	//�����¼�
	if(wRequestID>=DBO_GR_MATCH_EVENT_START && wRequestID<=DBO_GR_MATCH_EVENT_END)
	{
		//����Ч��
		if(m_pIGameMatchServiceManager==NULL) return false;

		tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
		IServerUserItem *pIServerUserItem=pBindParameter!=NULL?pBindParameter->pIServerUserItem:NULL;

		//�����ж�
		if ((pBindParameter->pIServerUserItem==NULL)||(pBindParameter->dwSocketID!=dwContextID))
		{
			//�������
			ASSERT(FALSE);
			return true;
		}

		return m_pIGameMatchServiceManager->OnEventDataBase(wRequestID,pIServerUserItem,pData,wDataSize);
	}

	// �û��Խ������¼�


	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//���ñ���
		m_bCollectUser=false;

		//ɾ��ʱ��
		m_pITimerEngine->KillTimer(IDI_REPORT_SERVER_INFO);

		//�����ж�
		if (m_bNeekCorrespond==true)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("��Э�������������ӹر��ˣ�%ld �����������"),m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);
			LOGI(TEXT("CAttemperEngineSink::OnEventTCPSocketShut, ")<<szDescribe);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);
		}

		return true;
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("Э������������ʧ�� [ %ld ]��%ld �����������"),
				nErrorCode,m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);
			LOGI("CAttemperEngineSink::OnEventTCPSocketLink, "<<szDescribe);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڷ�����Ϸ����ע����Ϣ..."),TraceLevel_Normal);
		LOGI("CAttemperEngineSink::OnEventTCPSocketLink, "<<TEXT("���ڷ�����Ϸ����ע����Ϣ..."));

		//��������
		CMD_CS_C_RegisterServer RegisterServer;
		ZeroMemory(&RegisterServer,sizeof(RegisterServer));

		//����˿�
		CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
		//��������
		memcpy(&(RegisterServer.GameServerOption), m_pGameServiceOption, sizeof(RegisterServer.GameServerOption));
		RegisterServer.GameServerOption.wServerPort=pServiceUnits->m_TCPNetworkEngine->GetCurrentPort();
//		RegisterServer.wKindID=m_pGameServiceOption->wKindID;
// 		RegisterServer.wNodeID=m_pGameServiceOption->wNodeID;
// 		RegisterServer.wSortID=m_pGameServiceOption->wSortID;
// 		RegisterServer.wServerID=m_pGameServiceOption->wServerID;
		RegisterServer.dwOnLineCount=m_ServerUserManager.GetUserItemCount();
 		RegisterServer.dwFullCount=m_pGameServiceOption->wMaxPlayer-RESERVE_USER_COUNT;
// 		lstrcpyn(RegisterServer.szServerName,m_pGameServiceOption->szServerName,CountArray(RegisterServer.szServerName));
		lstrcpyn(RegisterServer.szServerAddr,m_pInitParameter->m_ServiceAddress.szAddress,CountArray(RegisterServer.szServerAddr));
		
		//��������
		ASSERT(m_pITCPSocketService!=NULL);
		m_pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_SERVER, &RegisterServer, sizeof(CMD_CS_C_RegisterServer));

		//����ʱ��
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_REPORT_SERVER_INFO,TIME_REPORT_SERVER_INFO*1000L,TIMES_INFINITY,0);

		return true;
	}

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	LOGD("CAttemperEngineSink::OnEventTCPSocketRead, Command.wMainCmdID=" << Command.wMainCmdID << ", Command.wSubCmdID=" << Command.wSubCmdID);
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:		//ע�����
			{
				bool flag = OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
				return flag;
			}
		case MDM_CS_SERVICE_INFO:	//������Ϣ
			{
				bool flag = OnTCPSocketMainServiceInfo(Command.wSubCmdID, pData, wDataSize);
				return flag;
			}
		case MDM_CS_USER_COLLECT:	//�û�����
			{
				bool flag = OnTCPSocketMainUserCollect(Command.wSubCmdID, pData, wDataSize);
				return flag;
			}
		case MDM_CS_REMOTE_SERVICE:	//Զ�̷���
			{
				bool flag = OnTCPSocketMainRemoteService(Command.wSubCmdID, pData, wDataSize);
				return flag;
			}
		case MDM_CS_MANAGER_SERVICE: //�������
			{
				bool flag = OnTCPSocketMainManagerService(Command.wSubCmdID, pData, wDataSize);
				return flag;
			}
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	LOGI("CAttemperEngineSink::OnEventTCPNetworkBind, dwClientAddr="<<dwClientAddr<<", dwSocketID="<<dwSocketID);
#ifdef _DEBUG
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	LOGD("WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
#endif

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//���ñ���
	if (pBindParameter!=NULL)
	{
		pBindParameter->dwSocketID=dwSocketID;
		pBindParameter->dwClientAddr=dwClientAddr;
		pBindParameter->dwActiveTime=(DWORD)time(NULL);

// #ifdef _DEBUG
// 		HANDLE handle = GetCurrentProcess();
// 		PROCESS_MEMORY_COUNTERS pmc;
// 		GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 		LOGD("WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
// #endif
		return true;
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	LOGI("CAttemperEngineSink::OnEventTCPNetworkShut, dwClientAddr="<<dwClientAddr<<", dwSocketID="<<dwSocketID<<", dwActiveTime="<<dwActiveTime);
// #ifdef _DEBUG
// 	HANDLE handle = GetCurrentProcess();
// 	PROCESS_MEMORY_COUNTERS pmc;
// 	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 	LOGD("WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
// #endif
	try
	{
		//��������
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

		//��ȡ�û�
		IServerUserItem * pIServerUserItem=pBindParameter->pIServerUserItem;

		//�û�����
		if (pIServerUserItem!=NULL)
		{
			LOGI("CAttemperEngineSink::OnEventTCPNetworkShut, UserID=" << pIServerUserItem->GetUserID() << ", szNickName=" << pIServerUserItem->GetNickName() << ", UserStatus=" << pIServerUserItem->GetUserStatus());
			if (pIServerUserItem->GetUserStatus() == US_TRUSTEE)
			{
				// ����Ѿ�������״̬����ʱ����ߣ���Ӧ���ٴ��˳�
				pIServerUserItem->DetachBindStatus();
				ZeroMemory(pBindParameter,sizeof(tagBindParameter));
			}
			else
			{
				//��������
				WORD wTableID=pIServerUserItem->GetTableID();

				//���ߴ���
				if (wTableID!=INVALID_TABLE)
				{
					//�����
					pIServerUserItem->DetachBindStatus();

					//����֪ͨ
					ASSERT(wTableID<m_pGameServiceOption->wTableCount);
					m_TableFrameArray[wTableID]->OnEventUserOffLine(pIServerUserItem);
				}
				else
				{
					pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);
				}
			}		
		}

		//�����Ϣ
		ZeroMemory(pBindParameter,sizeof(tagBindParameter));
	}
	catch(...)
	{
		LOGE("CAttemperEngineSink::OnEventTCPNetworkShut, catch");
	}
// #ifdef _DEBUG
// // 	HANDLE handle = GetCurrentProcess();
// // 	PROCESS_MEMORY_COUNTERS pmc;
// 	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 	LOGD("WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
// #endif
	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
//	LOGI("CAttemperEngineSink::OnEventTCPNetworkRead, Command.wMainCmdID="<<Command.wMainCmdID<<", Command.wSubCmdID="<<Command.wSubCmdID);
	switch (Command.wMainCmdID)
	{
	case MDM_GR_USER:		//�û�����
		{
			LOGD("CAttemperEngineSink::OnEventTCPNetworkRead, OnTCPNetworkMainUser , Command.wSubCmdID=" << Command.wSubCmdID);
			bool flag = OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
// #ifdef _DEBUG
// 			HANDLE handle = GetCurrentProcess();
// 			PROCESS_MEMORY_COUNTERS pmc;
// 			GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 			LOGD("WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
// #endif
			return flag;
		}
	case MDM_GR_LOGON:		//��¼����
		{
			LOGD("CAttemperEngineSink::OnEventTCPNetworkRead, OnTCPNetworkMainLogon , Command.wSubCmdID=" << Command.wSubCmdID);
			bool flag = OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
// #ifdef _DEBUG
// 			HANDLE handle = GetCurrentProcess();
// 			PROCESS_MEMORY_COUNTERS pmc;
// 			GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 			LOGD("WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
// #endif
			return flag;
		}
	case MDM_GF_GAME:		//��Ϸ����
		{
			bool flag = OnTCPNetworkMainGame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
// #ifdef _DEBUG
// 			HANDLE handle = GetCurrentProcess();
// 			PROCESS_MEMORY_COUNTERS pmc;
// 			GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 			LOGD("WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
// #endif
			return flag;
		}
	case MDM_GF_FRAME:		//�������
		{
			LOGD("CAttemperEngineSink::OnEventTCPNetworkRead, OnTCPNetworkMainFrame , Command.wSubCmdID=" << Command.wSubCmdID);
			bool flag = OnTCPNetworkMainFrame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
// #ifdef _DEBUG
// 			HANDLE handle = GetCurrentProcess();
// 			PROCESS_MEMORY_COUNTERS pmc;
// 			GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 			LOGD("WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
// #endif
			return flag;
		}
	case MDM_GR_INSURE:		//��������
		{
			LOGD("CAttemperEngineSink::OnEventTCPNetworkRead, OnTCPNetworkMainInsure , Command.wSubCmdID=" << Command.wSubCmdID);
			bool flag = OnTCPNetworkMainInsure(Command.wSubCmdID,pData,wDataSize,dwSocketID);
// #ifdef _DEBUG
// 			HANDLE handle = GetCurrentProcess();
// 			PROCESS_MEMORY_COUNTERS pmc;
// 			GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 			LOGD("WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
// #endif
			return flag;
		}
	case MDM_GR_MANAGE:		//��������
		{
			LOGD("CAttemperEngineSink::OnEventTCPNetworkRead, OnTCPNetworkMainManage , Command.wSubCmdID=" << Command.wSubCmdID);
			bool flag = OnTCPNetworkMainManage(Command.wSubCmdID,pData,wDataSize,dwSocketID);
// #ifdef _DEBUG
// 			HANDLE handle = GetCurrentProcess();
// 			PROCESS_MEMORY_COUNTERS pmc;
// 			GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 			LOGD("WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
// #endif
			return flag;
		}
	case MDM_GR_MATCH:		//��������
		{
			LOGD("CAttemperEngineSink::OnEventTCPNetworkRead, OnTCPNetworkMainMatch , Command.wSubCmdID=" << Command.wSubCmdID);
			bool flag = OnTCPNetworkMainMatch(Command.wSubCmdID,pData,wDataSize,dwSocketID);
// #ifdef _DEBUG
// 			HANDLE handle = GetCurrentProcess();
// 			PROCESS_MEMORY_COUNTERS pmc;
// 			GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
// 			LOGD("WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
// #endif
			return flag;
		}
	case MDM_GR_USERCUSTOM:		// �Խ���������
		{
			LOGD("CAttemperEngineSink::OnEventTCPNetworkRead, OnTCPNeworkUserCustom , Command.wSubCmdID=" << Command.wSubCmdID);
			bool flag = OnTCPNetworkMainUserCustom(Command.wSubCmdID, pData, wDataSize, dwSocketID);
			return flag;
		}
	}

	return false;
}

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_AndroidUserManager.SendDataToClient(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_COMPUTER);

	return true;
}

//��Ϸ��Ϣ
bool CAttemperEngineSink::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_AndroidUserManager.SendDataToClient(MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_COMPUTER);

	return true;
}

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//��������
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//��������
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//�رմ���
			if ((wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0)
			{
				LOGI("CAttemperEngineSink::SendRoomMessage DeleteAndroidUserItem, wType="<<wType<<", wAndroidIndex="<<LOWORD(pBindParameter->dwSocketID)<<", wRoundID="<<HIWORD(pBindParameter->dwSocketID));
				m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID);
			}
		}
		else
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//��Ϸ��Ϣ
bool CAttemperEngineSink::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	if ((pIServerUserItem->GetBindIndex()!=INVALID_WORD)&&(pIServerUserItem->IsClientReady()==true))
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//��������
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//��������
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//�رմ���
			if ((wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0)
			{
				LOGI("CAttemperEngineSink::SendGameMessage DeleteAndroidUserItem, wType="<<wType<<", wAndroidIndex="<<LOWORD(pBindParameter->dwSocketID)<<", wRoundID="<<HIWORD(pBindParameter->dwSocketID));
				m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID);
			}
		}
		else
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	if (bAndroid)
	{
		//�����û�
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}
	else
	{
		//�����û�
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}

	return true;
}

//ϵͳ��Ϣ
bool CAttemperEngineSink::SendSystemMessage(LPCTSTR lpszMessage, WORD wType)
{
	//������Ϣ
	CMD_GR_SendMessage SendMessage = {};
	SendMessage.cbAllRoom = TRUE;
	SendMessage.cbGame = TRUE;
	SendMessage.cbRoom = TRUE;
	lstrcpyn(SendMessage.szSystemMessage,lpszMessage,CountArray(SendMessage.szSystemMessage));
	SendMessage.wChatLength = lstrlen(SendMessage.szSystemMessage)+1;

	//������Ϣ
	WORD wSendSize = sizeof(SendMessage)-sizeof(SendMessage.szSystemMessage)+CountStringBuffer(SendMessage.szSystemMessage);
	SendSystemMessage(&SendMessage,wSendSize);
	return true;
};


//��������
bool CAttemperEngineSink::SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	if ((cbSendMask&BG_COMPUTER)!=0)
	{
		m_AndroidUserManager.SendDataToClient(wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	//�û�����
	m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,cbSendMask);

	return true;
}

//��������
bool CAttemperEngineSink::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	if (LOWORD(dwSocketID)!=INVALID_WORD)
	{
		if (LOWORD(dwSocketID)>=INDEX_ANDROID)
		{
			//�����û�
			m_AndroidUserManager.SendDataToClient(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else 
		{
			//�����û�
			m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}

	return false;
}

//�û�����
bool CAttemperEngineSink::OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	LOGI("CAttemperEngineSink::OnEventUserItemScore, NickName="<<pIServerUserItem->GetNickName()<<", cbReason="<<cbReason);

	//��������
	CMD_GR_UserScore UserScore;
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	//��������
	UserScore.dwUserID=pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;
	UserScore.UserScore.dwUserMedal=pUserInfo->dwUserMedal;
	UserScore.UserScore.dwExperience=pUserInfo->dwExperience;
	UserScore.UserScore.lLoveLiness=pUserInfo->lLoveLiness;

	//�������
	UserScore.UserScore.lGrade=pUserInfo->lGrade;
	UserScore.UserScore.lInsure=pUserInfo->lInsure;

	//�������
	if (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM)
	{
		tagVariationInfo UserRecordInfo;
		pIServerUserItem->QueryRecordInfo(UserRecordInfo);
		UserScore.UserScore.lScore = UserRecordInfo.lScore;
	}
	else
	{
		UserScore.UserScore.lScore = pUserInfo->lScore;
	}
	UserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	UserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();

	//��������
	SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));
//	LOGI("CAttemperEngineSink::OnEventUserItemScore, NickName="<<pIServerUserItem->GetNickName()<<", SendData CMD_GR_UserScore");

	//��������
	CMD_GR_MobileUserScore MobileUserScore;

	//��������
	MobileUserScore.dwUserID=pUserInfo->dwUserID;
	MobileUserScore.UserScore.dwWinCount=pUserInfo->dwWinCount;
	MobileUserScore.UserScore.dwLostCount=pUserInfo->dwLostCount;
	MobileUserScore.UserScore.dwDrawCount=pUserInfo->dwDrawCount;
	MobileUserScore.UserScore.dwFleeCount=pUserInfo->dwFleeCount;
	MobileUserScore.UserScore.dwExperience=pUserInfo->dwExperience;

	//�������
	MobileUserScore.UserScore.lScore=pUserInfo->lScore;
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetTrusteeScore();
	MobileUserScore.UserScore.lScore+=pIServerUserItem->GetFrozenedScore();

	//��������
	SendDataBatchToMobileUser(pIServerUserItem->GetTableID(),MDM_GR_USER,SUB_GR_USER_SCORE,&MobileUserScore,sizeof(MobileUserScore));
//	LOGI("CAttemperEngineSink::OnEventUserItemScore, NickName="<<pIServerUserItem->GetNickName()<<", SendDataBatchToMobileUser CMD_GR_MobileUserScore");

	//��ʱд��
	if ((CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->IsVariation()==true))
	{
//		LOGI("CAttemperEngineSink::OnEventUserItemScore, NickName="<<pIServerUserItem->GetNickName()<<", IsImmediateWriteScore &&  IsVariation");
		//��������
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//�û���Ϣ
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwDBQuestID=pIServerUserItem->GetDBQuestID();
		WriteGameScore.dwClientAddr=pIServerUserItem->GetClientAddr();
		WriteGameScore.dwInoutIndex=pIServerUserItem->GetInoutIndex();

		// ��ȡ���ţ����Ӻ�(�м�¼�ĵط�������Ҫ�����¼)
// 		WriteGameScore.wTableID = pIServerUserItem->GetTableID();
// 		WriteGameScore.wChairID = pIServerUserItem->GetChairID();

		//��ȡ����
		pIServerUserItem->DistillVariation(WriteGameScore.VariationInfo);

        WriteGameScore.VariationInfo.dwExperience=0;
        WriteGameScore.VariationInfo.dwDrawCount=0;
        WriteGameScore.VariationInfo.dwFleeCount=0;
        WriteGameScore.VariationInfo.dwWinCount=0;
        WriteGameScore.VariationInfo.dwLostCount=0;

		// �õ�socketid�������õ��ظ�����Ϣ
		WORD wBindIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
		DWORD dwSocketID = 0;
		if (pBindParameter != NULL)
		{
			dwSocketID = pBindParameter->dwSocketID;
		}
		else
		{
			LOGW("CAttemperEngineSink::OnEventUserItemScore, NickName="<<pIServerUserItem->GetNickName()<<", wBindIndex="<<wBindIndex<<", pBindParameter==NULL");
		}

		//Ͷ������
		m_pIDBCorrespondManager->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,dwSocketID,&WriteGameScore,sizeof(WriteGameScore), TRUE);
	}

	//֪ͨ����
	if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		CTableFrame* pTableFrame = m_TableFrameArray[pIServerUserItem->GetTableID()];
		if (pTableFrame != NULL)
		{
			pTableFrame->OnUserScroeNotify(pIServerUserItem->GetChairID(),pIServerUserItem,cbReason);
		}
		else
		{
			LOGW("CAttemperEngineSink::OnEventUserItemScore, NickName="<<pIServerUserItem->GetNickName()<<", pTableFrame==NULL, GetTableID="<<pIServerUserItem->GetTableID());
		}
	}

	return true;
}

//�û�״̬
bool CAttemperEngineSink::OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID, WORD wOldChairID)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_UserStatus UserStatus;
	ZeroMemory(&UserStatus,sizeof(UserStatus));

	//��������
	UserStatus.dwUserID=pIServerUserItem->GetUserID();
	UserStatus.UserStatus.wTableID=pIServerUserItem->GetTableID();
	UserStatus.UserStatus.wChairID=pIServerUserItem->GetChairID();
	UserStatus.UserStatus.cbUserStatus=pIServerUserItem->GetUserStatus();

	//�޸���Ϣ
	if(pIServerUserItem->GetUserStatus()==US_SIT && pIServerUserItem->IsMobileUser())
	{
		//�������
		WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
		WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();

		WORD wTableCount = wTagerDeskPos+wTagerDeskCount-1;
		if(pIServerUserItem->GetTableID() > wTableCount && wTagerDeskCount>0)
		{
			//�������
			WORD wNewDeskPos = (pIServerUserItem->GetTableID()/wTagerDeskCount)*wTagerDeskCount;
			WORD wMaxDeskPos = m_pGameServiceOption->wTableCount-wTagerDeskCount;

			//����Ч��
			if(wNewDeskPos > wMaxDeskPos) wNewDeskPos = wMaxDeskPos;

			//�޸���Ϣ
			pIServerUserItem->SetMobileUserDeskPos(wNewDeskPos);
		}
	}

	//��������
	SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
	if(pIServerUserItem->GetUserStatus()==US_SIT)
		SendDataBatchToMobileUser(pIServerUserItem->GetTableID(),MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
	else
		SendDataBatchToMobileUser(wOldTableID,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));

	//�뿪�ж�
	if (pIServerUserItem->GetUserStatus()==US_NULL)
	{
		//��ȡ��
		WORD wBindIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

		//�������
		if (pBindParameter!=NULL)
		{
			//�󶨴���
			if (pBindParameter->pIServerUserItem==pIServerUserItem)
			{
				pBindParameter->pIServerUserItem=NULL;
			}

			//�ж�����
			if (pBindParameter->dwSocketID!=0L)
			{
				if (LOWORD(pBindParameter->dwSocketID)>=INDEX_ANDROID)
				{
					LOGI("CAttemperEngineSink::OnEventUserItemStatus, m_wAndroidIndex="<<LOWORD(pBindParameter->dwSocketID)<<", m_wRoundID="<<HIWORD(pBindParameter->dwSocketID));
					m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID);
				}
				else
				{
					m_pITCPNetworkEngine->ShutDownSocket(pBindParameter->dwSocketID);
				}
			}
		}

		//�뿪����
		OnEventUserLogout(pIServerUserItem,0L);
	}
	return true;
}

//�û�Ȩ��
bool CAttemperEngineSink::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,bool bGameRight)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	DBR_GR_ManageUserRight ManageUserRight= {0};
	ManageUserRight.dwUserID = pUserInfo->dwUserID;
	ManageUserRight.dwAddRight = dwAddRight;
	ManageUserRight.dwRemoveRight = dwRemoveRight;
	ManageUserRight.bGameRight=bGameRight;

	//��������
	m_pIDBCorrespondManager->PostDataBaseRequest(ManageUserRight.dwUserID,DBR_GR_MANAGE_USER_RIGHT,0,&ManageUserRight,sizeof(ManageUserRight));

	return true;
}

//��¼�ɹ�
bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wBindIndex=LOWORD(dwContextID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	DBO_GR_LogonSuccess * pDBOLogonSuccess=(DBO_GR_LogonSuccess *)pData;

	//�����ж�
	if ((pBindParameter->pIServerUserItem!=NULL)||(pBindParameter->dwSocketID!=dwContextID))
	{
		//�������
		ASSERT(FALSE);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_NORMAL);

		return true;
	}

	//��������
	bool bAndroidUser=(wBindIndex>=INDEX_ANDROID);
	bool bMobileUser=(pBindParameter->cbClientKind==CLIENT_KIND_MOBILE);

	//�����ж�
	DWORD dwKickTime;
	if(m_KickUserItemMap.Lookup(pDBOLogonSuccess->dwUserID,dwKickTime)==TRUE)
	{
		//ʱЧ�ж�
		DWORD dwCurrTime = (DWORD)time(NULL);
		if(dwKickTime+VALID_TIME_KICK_BY_MANAGER > dwCurrTime)
		{
			//����ʧ��
			SendLogonFailure(TEXT("���ѱ�����Ա�������,1Сʱ֮�ڲ��ܽ��룡"),0,pBindParameter->dwSocketID);

			//�������
			PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

			return true;
		}
		else
		{
			//�Ƴ����
			m_KickUserItemMap.RemoveKey(pDBOLogonSuccess->dwUserID);
		}
	}

	//�л��ж�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	if (pIServerUserItem!=NULL)
	{
		//�л��û�
		SwitchUserItemConnect(pIServerUserItem,pDBOLogonSuccess->szMachineID,wBindIndex,pDBOLogonSuccess->cbDeviceType,pDBOLogonSuccess->wBehaviorFlags,pDBOLogonSuccess->wPageTableCount);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_USER_IMPACT);

		return true;
	}

	//ά���ж�
	if ((CServerRule::IsForfendRoomEnter(m_pGameServiceOption->dwServerRule)==true)&&(pDBOLogonSuccess->cbMasterOrder==0))
	{
		//����ʧ��
		SendLogonFailure(TEXT("��Ǹ������ϵͳά����ԭ�򣬵�ǰ��Ϸ�����ֹ�û����룡"),0,pBindParameter->dwSocketID);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SYSTEM);

		return true;
	}

	//���һ���
	if (bAndroidUser==true)
	{
		//���һ���
		DWORD dwUserID=pDBOLogonSuccess->dwUserID;
		IAndroidUserItem * pIAndroidUserItem=m_AndroidUserManager.SearchAndroidUserItem(dwUserID,dwContextID);

		//�޸Ļ���
		if (pIAndroidUserItem!=NULL)
		{
			//��ȡ����
			tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

			//��������
			if ((pAndroidParameter->lMinTakeScore!=0L)&&(pAndroidParameter->lMaxTakeScore!=0L))
			{
				//��������
				SCORE lMinTakeScore=(SCORE)pAndroidParameter->lMinTakeScore;
				SCORE lMaxTakeScore=(SCORE)__max(pAndroidParameter->lMaxTakeScore,pAndroidParameter->lMinTakeScore);

				//��������
				if ((lMaxTakeScore-lMinTakeScore)>0L)
				{
					SCORE lTakeScore = (lMaxTakeScore-lMinTakeScore)/10;
					pDBOLogonSuccess->lScore=(SCORE)(lMinTakeScore+(rand()%10)*lTakeScore+rand()%lTakeScore);
				}
				else
				{
					pDBOLogonSuccess->lScore=(SCORE)lMaxTakeScore;
				}
			}
		}

		LOGI(TEXT("CAttemperEngineSink::OnDBLogonSuccess, ������"<<dwUserID<<TEXT("��������Ϊ")<<pDBOLogonSuccess->lScore))
	}

	//��ͷ���
	if ((m_pGameServiceOption->lMinEnterScore!=0L)&&(pDBOLogonSuccess->lScore<m_pGameServiceOption->lMinEnterScore))
	{
		//����ʧ��
		TCHAR szMsg[128]=TEXT("");
		_sntprintf(szMsg,CountArray(szMsg), TEXT("��Ǹ��������Ϸ��Ϸ�ҵ��ڵ�ǰ��Ϸ�������ͽ�����Ϸ��%I64d�����ܽ��뵱ǰ��Ϸ���䣡"), m_pGameServiceOption->lMinEnterScore);
		SendLogonFailure(szMsg,0,pBindParameter->dwSocketID);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

		return true;
	}

	//��߷���
	if ((m_pGameServiceOption->lMaxEnterScore!=0L)&&(pDBOLogonSuccess->lScore>m_pGameServiceOption->lMaxEnterScore))
	{
		//����ʧ��
		TCHAR szMsg[128]=TEXT("");
		_sntprintf(szMsg,CountArray(szMsg), TEXT("��Ǹ��������Ϸ��Ϸ�Ҹ��ڵ�ǰ��Ϸ�������߽�����Ϸ��%I64d�����ܽ��뵱ǰ��Ϸ���䣡"), m_pGameServiceOption->lMaxEnterScore);
		SendLogonFailure(szMsg,0,pBindParameter->dwSocketID);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

		return true;
	}

	//��Ա�ж�
	if(m_pGameServiceOption->cbMinEnterMember != 0 && pDBOLogonSuccess->cbMemberOrder < m_pGameServiceOption->cbMinEnterMember &&(pDBOLogonSuccess->cbMasterOrder==0))
	{
		//����ʧ��
		SendLogonFailure(TEXT("��Ǹ�����Ļ�Ա������ڵ�ǰ��Ϸ�������ͽ����Ա���������ܽ��뵱ǰ��Ϸ���䣡"),0,pBindParameter->dwSocketID);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

		return true;
	}

	//��Ա�ж�
	if(m_pGameServiceOption->cbMaxEnterMember != 0 && pDBOLogonSuccess->cbMemberOrder > m_pGameServiceOption->cbMaxEnterMember &&(pDBOLogonSuccess->cbMasterOrder==0))
	{
		//����ʧ��
		SendLogonFailure(TEXT("��Ǹ�����Ļ�Ա������ڵ�ǰ��Ϸ�������߽����Ա���������ܽ��뵱ǰ��Ϸ���䣡"),0,pBindParameter->dwSocketID);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

		return true;
	}

	//�����ж�
	WORD wMaxPlayer=m_pGameServiceOption->wMaxPlayer;
	DWORD dwOnlineCount=m_ServerUserManager.GetUserItemCount();
	if ((pDBOLogonSuccess->cbMemberOrder==0)&&(pDBOLogonSuccess->cbMasterOrder==0)&&(dwOnlineCount>(DWORD)(wMaxPlayer-RESERVE_USER_COUNT)))
	{
		//����ʧ��
		SendLogonFailure(TEXT("��Ǹ�����ڴ˷����Ѿ���������ͨ��Ҳ��ܼ��������ˣ�"),0,pBindParameter->dwSocketID);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_FULL);

		return true;
	}

	//�û�����
	tagUserInfo UserInfo;
	tagUserInfoPlus UserInfoPlus;
	ZeroMemory(&UserInfo,sizeof(UserInfo));
	ZeroMemory(&UserInfoPlus,sizeof(UserInfoPlus));

	//��������
	UserInfo.wFaceID=pDBOLogonSuccess->wFaceID;
	UserInfo.dwUserID=pDBOLogonSuccess->dwUserID;
	UserInfo.dwGameID=pDBOLogonSuccess->dwGameID;
	UserInfo.dwGroupID=pDBOLogonSuccess->dwGroupID;
	UserInfo.dwCustomID=pDBOLogonSuccess->dwCustomID;
	UserInfo.dwAreaID = pDBOLogonSuccess->dwAreaID;
	lstrcpyn(UserInfo.szNickName,pDBOLogonSuccess->szNickName,CountArray(UserInfo.szNickName));

	//�û�����
	UserInfo.cbGender=pDBOLogonSuccess->cbGender;
	UserInfo.cbMemberOrder=pDBOLogonSuccess->cbMemberOrder;
	UserInfo.cbMasterOrder=pDBOLogonSuccess->cbMasterOrder;
	lstrcpyn(UserInfo.szGroupName,pDBOLogonSuccess->szGroupName,CountArray(UserInfo.szGroupName));
	lstrcpyn(UserInfo.szUnderWrite,pDBOLogonSuccess->szUnderWrite,CountArray(UserInfo.szUnderWrite));

	//״̬����
	UserInfo.cbUserStatus=US_FREE;
	UserInfo.wTableID=INVALID_TABLE;
	UserInfo.wChairID=INVALID_CHAIR;

	//������Ϣ
	UserInfo.lScore=pDBOLogonSuccess->lScore;
	UserInfo.lGrade=pDBOLogonSuccess->lGrade;
	UserInfo.lInsure=pDBOLogonSuccess->lInsure;
	UserInfo.dwWinCount=pDBOLogonSuccess->dwWinCount;
	UserInfo.dwLostCount=pDBOLogonSuccess->dwLostCount;
	UserInfo.dwDrawCount=pDBOLogonSuccess->dwDrawCount;
	UserInfo.dwFleeCount=pDBOLogonSuccess->dwFleeCount;
	UserInfo.dwUserMedal=pDBOLogonSuccess->dwUserMedal;
	UserInfo.dwExperience=pDBOLogonSuccess->dwExperience;
	UserInfo.lLoveLiness=pDBOLogonSuccess->lLoveLiness;

	//��¼��Ϣ
	UserInfoPlus.dwLogonTime=(DWORD)time(NULL);
	UserInfoPlus.dwInoutIndex=pDBOLogonSuccess->dwInoutIndex;

	//�û�Ȩ��
	UserInfoPlus.dwUserRight=pDBOLogonSuccess->dwUserRight;
	UserInfoPlus.dwMasterRight=pDBOLogonSuccess->dwMasterRight;

	//��������
	UserInfoPlus.bMobileUser=bMobileUser;
	UserInfoPlus.bAndroidUser=bAndroidUser;
#ifndef _READ_TABLE_OPTION_
	UserInfoPlus.lRestrictScore=m_pGameServiceOption->lRestrictScore;
#else
	// �˴���Ҫ����,��Ҫ�����������ʱ������,ѡ����������
	// ����������ҵ�¼ʱȷ����������
//	UserInfoPlus.lRestrictScore = m_pGameServiceOption->lRestrictScore;
#endif
	lstrcpyn(UserInfoPlus.szPassword,pDBOLogonSuccess->szPassword,CountArray(UserInfoPlus.szPassword));

	//������Ϣ
	UserInfoPlus.wBindIndex=wBindIndex;
	UserInfoPlus.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(UserInfoPlus.szMachineID,pDBOLogonSuccess->szMachineID,CountArray(UserInfoPlus.szMachineID));

	//�����û�
	m_ServerUserManager.InsertUserItem(&pIServerUserItem,UserInfo,UserInfoPlus);

	//�����ж�
	if (pIServerUserItem==NULL)
	{
		//�������
		ASSERT(FALSE);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_FULL);

		//�Ͽ��û�
		if (bAndroidUser==true)
		{
			LOGI("CAttemperEngineSink::OnDBLogonSuccess pIServerUserItem==NULL DeleteAndroidUserItem, m_wAndroidIndex="<<LOWORD(dwContextID)<<", m_wRoundID="<<HIWORD(dwContextID));
			m_AndroidUserManager.DeleteAndroidUserItem(dwContextID);
		}
		else
		{
			m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
		}

		return true;
	}

	// �����������Ӯ
	pIServerUserItem->SetTotalWin(pDBOLogonSuccess->lTotalWin);
	pIServerUserItem->SetTotalLose(pDBOLogonSuccess->lTotalLose);

	// ��������������Լ�buff������


	//�����û�
	pBindParameter->pIServerUserItem=pIServerUserItem;

	//�޸Ĳ���
	if(pIServerUserItem->IsMobileUser())
	{
		SetMobileUserParameter(pIServerUserItem,pDBOLogonSuccess->cbDeviceType,pDBOLogonSuccess->wBehaviorFlags,pDBOLogonSuccess->wPageTableCount);
	}

	//��¼�¼�
	OnEventUserLogon(pIServerUserItem,false);

	//�����û�
	if (m_bCollectUser==true)
	{
		//��������
		CMD_CS_C_UserEnter UserEnter;
		ZeroMemory(&UserEnter,sizeof(UserEnter));

		//���ñ���
		UserEnter.dwUserID=pIServerUserItem->GetUserID();
		UserEnter.dwGameID=pIServerUserItem->GetGameID();
		lstrcpyn(UserEnter.szNickName,pIServerUserItem->GetNickName(),CountArray(UserEnter.szNickName));

		//������Ϣ
		UserEnter.cbGender=pIServerUserItem->GetGender();
		UserEnter.cbMemberOrder=pIServerUserItem->GetMemberOrder();
		UserEnter.cbMasterOrder=pIServerUserItem->GetMasterOrder();

		//������Ϣ
		ASSERT(m_pITCPSocketService!=NULL);
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_ENTER,&UserEnter,sizeof(UserEnter));
	}

	//�㳡����¼
	for (CSquareManager::iterator iter=m_SquareManager.begin(); iter!=m_SquareManager.end(); iter++)
	{
		if(!iter->second.m_bStartSquare)continue;

		if(iter->second.m_dwSquareRemain>=5)
		{
			map<DWORD, tagSQUARERANK>::iterator iterRank = iter->second.m_SquareScoreMap.find(pDBOLogonSuccess->dwUserID);
			if(iterRank==iter->second.m_SquareScoreMap.end())
			{
				tagSQUARERANK SquareRank;
				SquareRank.lScore=0;
				SquareRank.wResult=0;
				SquareRank.dwUserID=pDBOLogonSuccess->dwUserID;
				SquareRank.dwEnterTime=(DWORD)(CTime::GetCurrentTime().GetTime());
				lstrcpyn(SquareRank.szNickName,pIServerUserItem->GetNickName(),CountArray(SquareRank.szNickName));
				iter->second.AddRank(pIServerUserItem->GetUserID(),SquareRank);
			}

			if (!pIServerUserItem->IsAndroidUser())
			{
				iter->second.SendSquareEnter(pIServerUserItem->GetUserID());
				iter->second.SendTopPlayers(pIServerUserItem->GetUserID());
			}
		}

	}

#ifdef _DEBUG
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	LOGD("WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
#endif

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem!=NULL)) return true;

	//���ʹ���
	DBO_GR_LogonFailure * pLogonFailure=(DBO_GR_LogonFailure *)pData;
	SendLogonFailure(pLogonFailure->szDescribeString,pLogonFailure->lResultCode,dwContextID);

	//�Ͽ�����
	if (LOWORD(dwContextID)>=INDEX_ANDROID)
	{
		LOGI("CAttemperEngineSink::OnDBLogonFailure DeleteAndroidUserItem, m_wAndroidIndex="<<LOWORD(dwContextID)<<", m_wRoundID="<<HIWORD(dwContextID));
		m_AndroidUserManager.DeleteAndroidUserItem(dwContextID);
	}
// 	else
// 	{
// 		m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
// 	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBGameParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBO_GR_GameParameter));
	if (wDataSize!=sizeof(DBO_GR_GameParameter)) return false;

	//��������
	DBO_GR_GameParameter * pGameParameter=(DBO_GR_GameParameter *)pData;

	//������Ϣ
	m_pGameParameter->wMedalRate=pGameParameter->wMedalRate;
	m_pGameParameter->wRevenueRate=pGameParameter->wRevenueRate;

	//�汾��Ϣ
	m_pGameParameter->dwClientVersion=pGameParameter->dwClientVersion;
//	m_pGameParameter->dwAppClientVersion = pGameParameter->dwAppClientVersion;
	m_pGameParameter->dwServerVersion=pGameParameter->dwServerVersion;

	//�汾Ч��
	if (VERSION_EFFICACY==TRUE)
	{
		//�汾�ж�
		bool bVersionInvalid=false;
		if (m_pGameParameter->dwClientVersion!=m_pGameServiceAttrib->dwClientVersion) bVersionInvalid=true;
//		if (m_pGameParameter->dwAppClientVersion != m_pGameServiceAttrib->dwAppClientVersion) bVersionInvalid = true;
		if (m_pGameParameter->dwServerVersion!=m_pGameServiceAttrib->dwServerVersion) bVersionInvalid=true;

		//��ʾ��Ϣ
		if (bVersionInvalid==true)
		{
			CTraceService::TraceString(TEXT("ƽ̨���ݿ��������汾ע����Ϣ�뵱ǰ����汾��Ϣ��һ��"),TraceLevel_Warning);
			LOGI( TEXT("CAttemperEngineSink::OnDBGameParameter, ƽ̨���ݿ��������汾ע����Ϣ�뵱ǰ����汾��Ϣ��һ��") );
		}
	}

	return true;
}

//�б���Ϣ
bool CAttemperEngineSink::OnDBGameColumnInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_GameColumnInfo * pGameColumnInfo=(DBO_GR_GameColumnInfo *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameColumnInfo)-sizeof(pGameColumnInfo->ColumnItemInfo);

	//Ч�����
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameColumnInfo->cbColumnCount*sizeof(pGameColumnInfo->ColumnItemInfo[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameColumnInfo->cbColumnCount*sizeof(pGameColumnInfo->ColumnItemInfo[0])))) return false;

	//���ݴ���
	if (pGameColumnInfo->cbColumnCount==0)
	{
		//Ĭ���б�
	}
	else
	{
		//��������
		m_DataConfigColumn.cbColumnCount=pGameColumnInfo->cbColumnCount;
		CopyMemory(m_DataConfigColumn.ColumnItem,pGameColumnInfo->ColumnItemInfo,pGameColumnInfo->cbColumnCount*sizeof(pGameColumnInfo->ColumnItemInfo[0]));
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBGameAndroidInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
//	LOGI("CAttemperEngineSink::OnDBGameAndroidInfo, dwContextID="<<dwContextID);
	if (dwContextID == 0xFFFF)
	{
		m_AndroidUserManager.ResetAndroidStock();
		return true;
	}
	//��������
	DBO_GR_GameAndroidInfo * pGameAndroidInfo=(DBO_GR_GameAndroidInfo *)pData;
//	WORD wHeadSize=sizeof(DBO_GR_GameAndroidInfo)-sizeof(pGameAndroidInfo->AndroidParameter);

	//Ч�����
//	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidParameter[0]))));
//	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidParameter[0])))) return false;

//	LOGI("CAttemperEngineSink::OnDBGameAndroidInfo, lResultCode="<<pGameAndroidInfo->lResultCode);
	//���û���
	if (pGameAndroidInfo->lResultCode==DB_SUCCESS)
	{
//		m_AndroidUserManager.SetAndroidStock(pGameAndroidInfo->AndroidParameter,pGameAndroidInfo->wAndroidCount);
		m_AndroidUserManager.SetAndroidStock(pGameAndroidInfo->AndroidParameter);
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBGamePropertyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_GamePropertyInfo * pGamePropertyInfo=(DBO_GR_GamePropertyInfo *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GamePropertyInfo)-sizeof(pGamePropertyInfo->PropertyInfo);

	//Ч�����
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGamePropertyInfo->cbPropertyCount*sizeof(pGamePropertyInfo->PropertyInfo[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGamePropertyInfo->cbPropertyCount*sizeof(pGamePropertyInfo->PropertyInfo[0])))) return false;

	//��ȡ״̬
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	enServiceStatus ServiceStatus=pServiceUnits->GetServiceStatus();

	//���õ���
	if (pGamePropertyInfo->lResultCode==DB_SUCCESS)
	{
		//���ù���
		m_GamePropertyManager.SetGamePropertyInfo(pGamePropertyInfo->PropertyInfo,pGamePropertyInfo->cbPropertyCount);

		//��������
		m_DataConfigProperty.cbPropertyCount=pGamePropertyInfo->cbPropertyCount;
		CopyMemory(m_DataConfigProperty.PropertyInfo,pGamePropertyInfo->PropertyInfo,pGamePropertyInfo->cbPropertyCount*sizeof(pGamePropertyInfo->PropertyInfo[0]));
	}

	//�¼�֪ͨ
	if (ServiceStatus!=ServiceStatus_Service)
	{
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess=ER_SUCCESS;
		SendUIControlPacket(UI_SERVICE_CONFIG_RESULT,&ControlResult,sizeof(ControlResult));
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_UserInsureInfo * pUserInsureInfo=(DBO_GR_UserInsureInfo *)pData;

	//��������
	CMD_GR_S_UserInsureInfo UserInsureInfo;
	ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

	//��������
	UserInsureInfo.cbActivityGame=pUserInsureInfo->cbActivityGame;
	UserInsureInfo.wRevenueTake=pUserInsureInfo->wRevenueTake;
	UserInsureInfo.wRevenueTransfer=pUserInsureInfo->wRevenueTransfer;
	UserInsureInfo.wServerID=pUserInsureInfo->wServerID;
	UserInsureInfo.lUserInsure=pUserInsureInfo->lUserInsure;
	UserInsureInfo.lUserScore+=pIServerUserItem->GetUserScore();
	UserInsureInfo.lUserScore+=pIServerUserItem->GetTrusteeScore();
	UserInsureInfo.lUserScore+=pIServerUserItem->GetFrozenedScore();
	UserInsureInfo.lTransferPrerequisite=pUserInsureInfo->lTransferPrerequisite;

	//��������
//	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_INSURE,SUB_GR_USER_INSURE_INFO,&UserInsureInfo,sizeof(UserInsureInfo));
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_INSURE_INFO,&UserInsureInfo,sizeof(UserInsureInfo));

	return true;
}
bool CAttemperEngineSink::OnDBUserTransRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
    ASSERT(wDataSize==sizeof(DBR_GR_UserTransRecord));
	if (wDataSize!=sizeof(DBR_GR_UserTransRecord)) return false;

	DBR_GR_UserTransRecord *pTransRecord=(DBR_GR_UserTransRecord *)pData;
    
	CMD_GR_RealTransRecord RealTransRecord;
	ZeroMemory(&RealTransRecord,sizeof(RealTransRecord));
	RealTransRecord.dwGameID = pTransRecord->dwGameID;
	_tcscpy(RealTransRecord.szSourceAccount,pTransRecord->szSourceAccount);
	_tcscpy(RealTransRecord.szTargetAccounts,pTransRecord->szTargetAccounts);
	_tcscpy(RealTransRecord.szTime,pTransRecord->szTime);
	RealTransRecord.lSwapScore = pTransRecord->lSwapScore;
    RealTransRecord.lRevenue=pTransRecord->lRevenue;
    RealTransRecord.bOver = pTransRecord->bOver;
	RealTransRecord.dwTargetID = pTransRecord->dwTargetID;

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_INSURE,SUB_GR_TRANS_RECORD,&RealTransRecord,sizeof(RealTransRecord));
    return true;
}
//���гɹ�
bool CAttemperEngineSink::OnDBUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
//	LOGI(_T("CAttemperEngineSink::OnDBUserInsureSuccess"));
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_UserInsureSuccess * pUserInsureSuccess=(DBO_GR_UserInsureSuccess *)pData;

	//��������
	SCORE lFrozenedScore=pUserInsureSuccess->lFrozenedScore;
	SCORE lInsureRevenue=pUserInsureSuccess->lInsureRevenue;
	SCORE lVariationScore=pUserInsureSuccess->lVariationScore;
	SCORE lVariationInsure=pUserInsureSuccess->lVariationInsure;

	//�ⶳ����
	if ((lFrozenedScore>0L)&&(pIServerUserItem->UnFrozenedUserScore(lFrozenedScore)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//���в���
	if (pIServerUserItem->ModifyUserInsure(lVariationScore,lVariationInsure,lInsureRevenue)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	CMD_GR_S_UserInsureSuccess UserInsureSuccess;
	ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

	//�������
	UserInsureSuccess.cbActivityGame=pUserInsureSuccess->cbActivityGame;
	UserInsureSuccess.lUserScore=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();
	UserInsureSuccess.lUserInsure=pUserInsureSuccess->lSourceInsure+pUserInsureSuccess->lVariationInsure;
	lstrcpyn(UserInsureSuccess.szDescribeString,pUserInsureSuccess->szDescribeString,CountArray(UserInsureSuccess.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(UserInsureSuccess.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_INSURE_SUCCESS,&UserInsureSuccess,wHeadSize+wDescribe);

//	LOGI(_T("CAttemperEngineSink::OnDBUserInsureSuccess end"));
	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//���ʹ���
	DBO_GR_UserInsureFailure * pUserInsureFailure=(DBO_GR_UserInsureFailure *)pData;
	SendInsureFailure(pIServerUserItem,pUserInsureFailure->szDescribeString,pUserInsureFailure->lResultCode,pUserInsureFailure->cbActivityGame);

	//�ⶳ����
	if ((pUserInsureFailure->lFrozenedScore>0L)&&(pIServerUserItem->UnFrozenedUserScore(pUserInsureFailure->lFrozenedScore)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//�û���Ϣ
bool CAttemperEngineSink::OnDBUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_UserTransferUserInfo * pTransferUserInfo=(DBO_GR_UserTransferUserInfo *)pData;

	//��������
	CMD_GR_S_UserTransferUserInfo UserTransferUserInfo;
	ZeroMemory(&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	//�������
	UserTransferUserInfo.cbActivityGame=pTransferUserInfo->cbActivityGame;
	UserTransferUserInfo.dwTargetGameID=pTransferUserInfo->dwGameID;
	lstrcpyn(UserTransferUserInfo.szNickName,pTransferUserInfo->szNickName,CountArray(UserTransferUserInfo.szNickName));

	//��������
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_TRANSFER_USER_INFO,&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	return true;
}

//���߳ɹ�
bool CAttemperEngineSink::OnDBPropertySuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_S_PropertySuccess * pPropertySuccess=(DBO_GR_S_PropertySuccess *)pData;

	//��ȡ�û�
	IServerUserItem * pISourceUserItem=m_ServerUserManager.SearchUserItem(pPropertySuccess->dwSourceUserID);
	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pPropertySuccess->dwTargetUserID);

	//�����û�
	if (pISourceUserItem!=NULL)
	{
		//��������
		SCORE lFrozenedScore=pPropertySuccess->lFrozenedScore;

		//�ⶳ����
		if ((lFrozenedScore>0L)&&(pISourceUserItem->UnFrozenedUserScore(lFrozenedScore)==false))
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//��������
	if (pISourceUserItem!=NULL)
	{
		pISourceUserItem->ModifyUserProperty(0,pPropertySuccess->lSendLoveLiness);
	}

	//��������
	if(pITargetUserItem!=NULL)
	{
		pITargetUserItem->ModifyUserProperty(0,pPropertySuccess->lRecvLoveLiness);
	}

	//��������
	DWORD dwCurrentTime=(DWORD)time(NULL);
	tagUserProperty * pUserProperty = pITargetUserItem->GetUserProperty();

	//���ߴ���	
	switch(pPropertySuccess->wPropertyIndex)
	{
	case PROPERTY_ID_TWO_CARD:       //˫������
		{
			//ʹ���ж�
			if((pUserProperty->wPropertyUseMark&PT_USE_MARK_DOUBLE_SCORE)!=0)
			{
				//��������
				DWORD  dwValidTime=pUserProperty->PropertyInfo[0].wPropertyCount*pUserProperty->PropertyInfo[0].dwValidNum;
				if(pUserProperty->PropertyInfo[0].dwEffectTime+dwValidTime<dwCurrentTime)
				{
					pUserProperty->PropertyInfo[0].dwEffectTime=dwCurrentTime;
					pUserProperty->PropertyInfo[0].wPropertyCount=pPropertySuccess->wItemCount;
					pUserProperty->PropertyInfo[0].dwValidNum=VALID_TIME_DOUBLE_SCORE;
				}
				else
				{
					//��Ŀ�ۼ�
					pUserProperty->PropertyInfo[0].wPropertyCount+=pPropertySuccess->wItemCount;
				}
			}
			else
			{
				//������Ϣ
				pUserProperty->PropertyInfo[0].dwEffectTime=dwCurrentTime;
				pUserProperty->PropertyInfo[0].wPropertyCount=pPropertySuccess->wItemCount;
				pUserProperty->PropertyInfo[0].dwValidNum=VALID_TIME_DOUBLE_SCORE;
				pUserProperty->wPropertyUseMark |= PT_USE_MARK_DOUBLE_SCORE;
			}

			break;
		}
	case PROPERTY_ID_FOUR_CARD:      //�ı�����
		{
			//ʹ���ж�
			if((pUserProperty->wPropertyUseMark&PT_USE_MARK_FOURE_SCORE)!=0)
			{
				//��������
				DWORD  dwValidTime=pUserProperty->PropertyInfo[1].wPropertyCount*pUserProperty->PropertyInfo[1].dwValidNum;
				if(pUserProperty->PropertyInfo[1].dwEffectTime+dwValidTime<dwCurrentTime)
				{
					pUserProperty->PropertyInfo[1].dwEffectTime=dwCurrentTime;
					pUserProperty->PropertyInfo[1].wPropertyCount=pPropertySuccess->wItemCount;
					pUserProperty->PropertyInfo[1].dwValidNum=VALID_TIME_FOUR_SCORE;
				}
				else
				{
					//��Ŀ�ۼ�
					pUserProperty->PropertyInfo[1].wPropertyCount+=pPropertySuccess->wItemCount;
				}
			}
			else
			{
				//������Ϣ
				pUserProperty->PropertyInfo[1].dwEffectTime=dwCurrentTime;
				pUserProperty->PropertyInfo[1].wPropertyCount=pPropertySuccess->wItemCount;
				pUserProperty->PropertyInfo[1].dwValidNum=VALID_TIME_FOUR_SCORE;
				pUserProperty->wPropertyUseMark |= PT_USE_MARK_FOURE_SCORE;
			}
			break;
		}
	case PROPERTY_ID_SCORE_CLEAR:    //��������
		{
			//��������
			SCORE lCurrScore = pITargetUserItem->GetUserScore();
			if ( lCurrScore < 0)
			{
				//�û���Ϣ
				tagUserInfo * pUserInfo = pITargetUserItem->GetUserInfo();
				if(pUserInfo==NULL) return true;

				//�޸Ļ���
				pUserInfo->lScore=0;
			}
			break;
		}
	case PROPERTY_ID_ESCAPE_CLEAR:   //��������
		{
			//�û���Ϣ
			tagUserInfo * pUserInfo = pITargetUserItem->GetUserInfo();
			if(pUserInfo==NULL) return true;

			//�޸�������
			if(pUserInfo->dwFleeCount > 0)
			{
				pUserInfo->dwFleeCount=0;
			}

			break;
		}
	case PROPERTY_ID_GUARDKICK_CARD: //���߿�
		{
			//ʹ���ж�
			if((pUserProperty->wPropertyUseMark&PT_USE_MARK_GUARDKICK_CARD)!=0)
			{
				//��������
				DWORD  dwValidTime=pUserProperty->PropertyInfo[2].wPropertyCount*pUserProperty->PropertyInfo[2].dwValidNum;
				if(pUserProperty->PropertyInfo[2].dwEffectTime+dwValidTime<dwCurrentTime)
				{
					pUserProperty->PropertyInfo[2].dwEffectTime=dwCurrentTime;
					pUserProperty->PropertyInfo[2].wPropertyCount=pPropertySuccess->wItemCount;
					pUserProperty->PropertyInfo[2].dwValidNum=VALID_TIME_GUARDKICK_CARD;
				}
				else
				{
					//��Ŀ�ۼ�
					pUserProperty->PropertyInfo[2].wPropertyCount+=pPropertySuccess->wItemCount;
				}
			}
			else
			{
				//������Ϣ
				pUserProperty->PropertyInfo[2].dwEffectTime=dwCurrentTime;
				pUserProperty->PropertyInfo[2].wPropertyCount=pPropertySuccess->wItemCount;
				pUserProperty->PropertyInfo[2].dwValidNum=VALID_TIME_GUARDKICK_CARD;
				pUserProperty->wPropertyUseMark |= PT_USE_MARK_GUARDKICK_CARD;
			}

			break;
		}
	case PROPERTY_ID_POSSESS:        //�����
		{
			//ʹ���ж�
			if((pUserProperty->wPropertyUseMark&PT_USE_MARK_POSSESS)!=0)
			{
				//��������
				DWORD  dwValidTime=pUserProperty->PropertyInfo[3].wPropertyCount*pUserProperty->PropertyInfo[3].dwValidNum;
				if(pUserProperty->PropertyInfo[3].dwEffectTime+dwValidTime<dwCurrentTime)
				{
					pUserProperty->PropertyInfo[3].dwEffectTime=dwCurrentTime;
					pUserProperty->PropertyInfo[3].wPropertyCount=pPropertySuccess->wItemCount;
					pUserProperty->PropertyInfo[3].dwValidNum=VALID_TIME_POSSESS;
				}
				else
				{
					//��Ŀ�ۼ�
					pUserProperty->PropertyInfo[3].wPropertyCount+=pPropertySuccess->wItemCount;
				}
			}
			else
			{
				//������Ϣ
				pUserProperty->PropertyInfo[3].dwEffectTime=dwCurrentTime;
				pUserProperty->PropertyInfo[3].wPropertyCount=pPropertySuccess->wItemCount;
				pUserProperty->PropertyInfo[3].dwValidNum=VALID_TIME_POSSESS;
				pUserProperty->wPropertyUseMark |= PT_USE_MARK_POSSESS;
			}

			break;
		}
	case PROPERTY_ID_BLUERING_CARD:  //�����Ա
	case PROPERTY_ID_YELLOWRING_CARD://�����Ա
	case PROPERTY_ID_WHITERING_CARD: //�����Ա
	case PROPERTY_ID_REDRING_CARD:   //�����Ա
	case PROPERTY_ID_VIPROOM_CARD:   //VIP��Ա
		{
			//�û���Ϣ
			tagUserInfo * pUserInfo = pITargetUserItem->GetUserInfo();
			if(pUserInfo==NULL) return true;

			//���»�Ա
			pUserInfo->cbMemberOrder=pPropertySuccess->cbMemberOrder;
			
			//�޸�Ȩ��
			pITargetUserItem->ModifyUserRight(pPropertySuccess->dwUserRight,0);

			//������Ϣ
			SendPropertyEffect(pITargetUserItem);

			break;
		}
	default:  // ȫ������
		{			
			break;
		}
	}

	//���ѷ�ʽ
	if(pPropertySuccess->cbConsumeScore==FALSE)
	{
		pISourceUserItem->ModifyUserInsure(0,-pPropertySuccess->lConsumeGold,0);
	}

	//�����ж�
	if(pPropertySuccess->wPropertyIndex==PROPERTY_ID_TRUMPET|| pPropertySuccess->wPropertyIndex==PROPERTY_ID_TYPHON)
		return true;

	//��������
	CMD_GR_S_PropertySuccess PropertySuccess;
	ZeroMemory(&PropertySuccess,sizeof(PropertySuccess));

	//���ñ���
	PropertySuccess.cbRequestArea=pPropertySuccess->cbRequestArea;
	PropertySuccess.wItemCount=pPropertySuccess->wItemCount;
	PropertySuccess.wPropertyIndex=pPropertySuccess->wPropertyIndex;
	PropertySuccess.dwSourceUserID=pPropertySuccess->dwSourceUserID;
	PropertySuccess.dwTargetUserID=pPropertySuccess->dwTargetUserID;

	//������Ϣ
	if (pISourceUserItem!=NULL)
	{
		SendData(pISourceUserItem,MDM_GR_USER,SUB_GR_PROPERTY_SUCCESS,&PropertySuccess,sizeof(PropertySuccess));
	}

	//������Ϣ
	if (pITargetUserItem!=NULL && pITargetUserItem!=pISourceUserItem)
	{
		SendData(pITargetUserItem,MDM_GR_USER,SUB_GR_PROPERTY_SUCCESS,&PropertySuccess,sizeof(PropertySuccess));
	}

	//�㲥����
	if(GetPropertyType(pPropertySuccess->wPropertyIndex)==PT_TYPE_PRESENT)
		SendPropertyMessage(PropertySuccess.dwSourceUserID,PropertySuccess.dwTargetUserID,PropertySuccess.wPropertyIndex,
		PropertySuccess.wItemCount);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBPropertyFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(DBO_GR_PropertyFailure));
	if(wDataSize!=sizeof(DBO_GR_PropertyFailure)) return false;

	//��ȡ����
	DBO_GR_PropertyFailure * pPropertyFailure = (DBO_GR_PropertyFailure *)pData;
	if(pPropertyFailure==NULL) return false;

	//��������
	WORD wBindIndex=LOWORD(dwContextID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	if(pBindParameter==NULL) return false;
	if(pBindParameter->pIServerUserItem==NULL) return false;

	//������Ϣ
	return SendPropertyFailure(pBindParameter->pIServerUserItem,pPropertyFailure->szDescribeString,0L,pPropertyFailure->cbRequestArea);
}

//ϵͳ��Ϣ
bool CAttemperEngineSink::OnDBSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(DBR_GR_SystemMessage));
	if(wDataSize!=sizeof(DBR_GR_SystemMessage)) return false;

	//��ȡ����
	DBR_GR_SystemMessage * pSystemMessage = (DBR_GR_SystemMessage *)pData;
	if(pSystemMessage==NULL) return false;

	//�ظ��ж�
	POSITION pos = m_SystemMessageList.GetHeadPosition();
	while(pos != NULL)
	{
		POSITION tempPos = pos;
		tagSystemMessage *pTagSystemMessage = m_SystemMessageList.GetNext(pos);
		if(pTagSystemMessage->SystemMessage.cbMessageID == pSystemMessage->cbMessageID)
		{
			//��������
			pTagSystemMessage->dwLastTime=0;
			CopyMemory(&pTagSystemMessage->SystemMessage, pSystemMessage, sizeof(DBR_GR_SystemMessage));

			return true;
		}
	}

	//���ڱ���
	tagSystemMessage  *pSendMessage=new tagSystemMessage;
	ZeroMemory(pSendMessage, sizeof(tagSystemMessage));

	//���ñ���
	CopyMemory(&pSendMessage->SystemMessage,pSystemMessage, sizeof(DBR_GR_SystemMessage));

	//��¼��Ϣ
	m_SystemMessageList.AddTail(pSendMessage);

	return true;
}

//�������д�
bool CAttemperEngineSink::OnDBSensitiveWords(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��ʼ����
	if(dwContextID==0xfffe)
	{
		m_WordsFilter.ResetSensitiveWordArray();
		m_pITimerEngine->KillTimer(IDI_LOAD_SENSITIVE_WORD);
		return true;			
	}

	//�������
	if(dwContextID==0xffff)
	{
		m_WordsFilter.FinishAdd();
		return true;
	}

	//�������д�
	const TCHAR *pWords=(const TCHAR*)pData;
	m_WordsFilter.AddSensitiveWords(pWords);
	return true;
}

bool CAttemperEngineSink::OnDBUserTransferReceipt(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_UserTransferReceipt UserTransferReceipt;
	ZeroMemory(&UserTransferReceipt,sizeof(UserTransferReceipt));

	//��������
	CMD_GR_UserTransferReceipt * pUserTransferReceipt=(CMD_GR_UserTransferReceipt *)pData;

	CopyMemory(&UserTransferReceipt, pUserTransferReceipt, wDataSize);

	LOGI("CAttemperEngineSink::OnDBUserTransferReceipt, dwRecordID="<<pUserTransferReceipt->dwRecordID<<", dwSourceUserID="<<pUserTransferReceipt->dwSourceUserID<<\
		", dwTargetUserID="<<pUserTransferReceipt->dwTargetUserID<<", dwSourceGameID="<<pUserTransferReceipt->dwSourceGameID<<", dwTargetGameID="<<pUserTransferReceipt->dwTargetGameID<<\
		", szSourceNickName="<<pUserTransferReceipt->szSourceNickName<<", szTargetNickName="<<pUserTransferReceipt->szTargetNickName<<\
		", lTransferScore="<<pUserTransferReceipt->lTransferScore<<", lRevenueScore="<<pUserTransferReceipt->lRevenueScore);

	//��������
	SendData(pIServerUserItem, MDM_GR_INSURE, SUB_GR_USER_TRANSFER_RECEIPT, &UserTransferReceipt, sizeof(UserTransferReceipt));

	return true;
}

// �����ɹ�
bool CAttemperEngineSink::OnDBUserOperateSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GP_OperateSuccess OperateSuccess;
	ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

	//��������
	DBO_GR_OperateSuccess * pOperateSuccess=(DBO_GR_OperateSuccess *)pData;

	//��������
	OperateSuccess.lResultCode=pOperateSuccess->lResultCode;
	lstrcpyn(OperateSuccess.szDescribeString,pOperateSuccess->szDescribeString,CountArray(OperateSuccess.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(OperateSuccess.szDescribeString);
	WORD wHeadSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString);
	
	SendData(pIServerUserItem, MDM_GR_INSURE, SUB_GP_OPERATE_SUCCESS,&OperateSuccess,wHeadSize+wDescribe);

	return true;
}

// ����ʧ��
bool CAttemperEngineSink::OnDBUserOperateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;


	//��������
	CMD_GP_OperateFailure OperateSuccess;
	ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

	//��������
	DBO_GR_OperateSuccess * pOperateSuccess=(DBO_GR_OperateSuccess *)pData;

	//��������
	OperateSuccess.lResultCode=pOperateSuccess->lResultCode;
	lstrcpyn(OperateSuccess.szDescribeString,pOperateSuccess->szDescribeString,CountArray(OperateSuccess.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(OperateSuccess.szDescribeString);
	WORD wHeadSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString);
	
	SendData(pIServerUserItem,MDM_GR_INSURE, SUB_GP_OPERATE_SUCCESS,&OperateSuccess,wHeadSize+wDescribe);

	return true;
}

// ����ƽ�������
bool CAttemperEngineSink::OnDBBalanceScoreCurve(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��ʼ����
	if(dwContextID==0xfffe)
	{
//		LOGI(TEXT("CAttemperEngineSink::OnDBBalanceScoreCurve ��ʼ��������"));
		m_BalanceScoreCurve.ResetBalanceScoreCurve();
//		m_pITimerEngine->KillTimer(IDI_LOAD_BALANCE_SCORE_CURVE);
		// ��һ�ζ�ȡ���óɹ�����ÿ����Сʱ�ٶ�һ��
//		m_pITimerEngine->SetTimer(IDI_LOAD_BALANCE_SCORE_CURVE,TIME_LOAD_BALANCE_SCORE_CURVE*1000L*6,TIMES_INFINITY,NULL);
		return true;			
	}

	//��������
	DBO_GR_BalanceScoreCurve * pBalanceScoreCurve=(DBO_GR_BalanceScoreCurve *)pData;

	// ��������
//	LOGI(TEXT("CAttemperEngineSink::OnDBBalanceScoreCurve lScore="<<pBalanceScoreCurve->lScore<<", lBalanceScore="<<pBalanceScoreCurve->lBalanceScore));
	m_BalanceScoreCurve.AddBalanceScoreCurve(pBalanceScoreCurve->lScore, pBalanceScoreCurve->lBalanceScore);

	return true;
}

// ���ƽ�������
bool CAttemperEngineSink::OnDBUserBalanceScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
//	LOGI(TEXT("CAttemperEngineSink::OnDBUserBalanceScore"));
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if (pBindParameter == NULL) return true;
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));
	if (pIServerUserItem == NULL)
	{
		return true;
	}

	//��������
	DBO_GR_UserBalanceScore * pUserBalanceScore = (DBO_GR_UserBalanceScore *)pData;

// 	pIServerUserItem->SetUserBalanceScoreBuffTime(pUserBalanceScore->lBuffTime);
// 	pIServerUserItem->SetUserBalanceScoreBuffGames(pUserBalanceScore->lBuffGames);
// 	pIServerUserItem->SetUserBalanceScoreBuffAmount(pUserBalanceScore->lBuffAmount);

	// ����dwBuffTime ���� dwBuffGames ���ж��Ƿ���Ҫ����Buff������BalanceScore
//	LONG lBalanceScore = pUserBalanceScore->lBalanceScore;
//	LOGI(TEXT("CAttemperEngineSink::OnDBUserBalanceScore, szNickName="<<pIServerUserItem->GetNickName()<<", lBalanceScore="<<lBalanceScore));

	// ��������Ӯ������BalanceScore(��һ�ε�½����ʱ����Ҫ������Ӯ�����ڣ�֮�����Ϸ�е����ݣ� ���ǵ��ڹ��ģ�ֱ��set����)
// 	SCORE lTotalWin = 0L;
// 	SCORE lTotalLose = 0L;
// 	if (pUserBalanceScore->bLogon == true)
// 	{
// 		lTotalWin = pIServerUserItem->GetTotalWin();
// 		lTotalLose = pIServerUserItem->GetTotalLose();
// 		lBalanceScore += m_BalanceScoreCurve.CalculateBalanceScore( 0, (lTotalWin+lTotalLose) );
// 	}
// 	pIServerUserItem->SetUserBalanceScore(lBalanceScore);

//	LOGI(TEXT("CAttemperEngineSink::OnDBUserBalanceScore, szNickName="<<pIServerUserItem->GetNickName()<<", lTotalWin="<<lTotalWin<<", lTotalLose="<<lTotalLose<<", BuffTime="<<pUserBalanceScore->lBuffTime<< \
		", BuffGames="<<pUserBalanceScore->lBuffGames<<", BuffAmount="<<pUserBalanceScore->lBuffAmount<<", lBalanceScore="<<lBalanceScore));
	
	return true;
}

//�㳡��������Ϣ
bool CAttemperEngineSink::OnDBSquareReward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_Square_Reward * pSquareReward=(DBO_GR_Square_Reward *)pData;

	CSquareManager::iterator iter=m_SquareManager.find(pSquareReward->wMatchID);
	if(iter!=m_SquareManager.end())
	{
		//�㳡�������Ѿ�����
		if(iter->second.m_bStartSquare)
		{
			LOGE("�㳡��<" << iter->second.m_pGameSquareOption->wMatchID <<  "|" << iter->second.m_pGameSquareOption->szSquareName << "> ��û�н���!");
			return true;
		}
		
		iter->second.m_StrSelfRankBonusMap[pSquareReward->dwUserID]=pSquareReward->szAwardInfo;

		if(pSquareReward->cbMemberOrder||pSquareReward->dwUserRight)
		{
			//�޸��û���Ա����
			IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pSquareReward->dwUserID);

			//�û���Ϣ
			tagUserInfo * pUserInfo = pITargetUserItem->GetUserInfo();
			if(pUserInfo==NULL) return true;

			//���»�Ա
			pUserInfo->cbMemberOrder=pSquareReward->cbMemberOrder;

			//�޸�Ȩ��
			pITargetUserItem->ModifyUserRight(pSquareReward->dwUserRight,0);

			//������Ϣ
			SendPropertyEffect(pITargetUserItem);
		}

		LOGI("<" << iter->second.m_pGameSquareOption->wMatchID <<  "|" << iter->second.m_pGameSquareOption->szSquareName\
			<< "> CAttemperEngineSink::OnDBSquareReward dwUserID= " << pSquareReward->dwUserID << " wRank= " << pSquareReward->wRank\
			<< " szAwardInfo= " << pSquareReward->szAwardInfo << " dwContextID= " << dwContextID);
		//������ҽ������������ͽ�������
		if(dwContextID==0xFFFF)iter->second.SendSquareEnd(20);
	}

	return true;
}

//���ع㳡������������Ϣ
bool CAttemperEngineSink::OnDBLoadSquareReward(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_Load_Square_Reward * pLoadSquareReward=(DBO_GR_Load_Square_Reward *)pData;

	CSquareManager::iterator iter=m_SquareManager.find(pLoadSquareReward->wMatchID);
	if(iter!=m_SquareManager.end())
	{
		memset(iter->second.m_pGameSquareOption->RankBonus, 0, sizeof(tagRANKBONUS)*20);
		iter->second.m_pGameSquareOption->cbRankBonus=pLoadSquareReward->cbRankBonus;

		LOGI("<" << iter->second.m_pGameSquareOption->wMatchID <<  "|" << iter->second.m_pGameSquareOption->szSquareName\
			<< "> CAttemperEngineSink::OnDBLoadSquareReward ���ع㳡���������� RankBonusCount= " << iter->second.m_pGameSquareOption->cbRankBonus);

		for(int i=0; i<pLoadSquareReward->cbRankBonus; i++)
		{
			iter->second.m_pGameSquareOption->RankBonus[i].wFirst=pLoadSquareReward->RankBonus[i].wFirst;
			iter->second.m_pGameSquareOption->RankBonus[i].wLast=pLoadSquareReward->RankBonus[i].wLast;
			iter->second.m_pGameSquareOption->RankBonus[i].cbPropertyCount=pLoadSquareReward->RankBonus[i].cbPropertyCount;

			LOGI("<" << iter->second.m_pGameSquareOption->wMatchID <<  "|" << iter->second.m_pGameSquareOption->szSquareName\
				<< "> CAttemperEngineSink::OnDBLoadSquareReward i= " << i << " First= " << iter->second.m_pGameSquareOption->RankBonus[i].wFirst\
				<< " Last= " << iter->second.m_pGameSquareOption->RankBonus[i].wLast\
				<< " PropertyCount= " << iter->second.m_pGameSquareOption->RankBonus[i].cbPropertyCount);

			for(int j=0; j<pLoadSquareReward->RankBonus[i].cbPropertyCount; j++)
			{
				iter->second.m_pGameSquareOption->RankBonus[i].wPropertyID[j]=pLoadSquareReward->RankBonus[i].wPropertyID[j];

				LOGI("<" << iter->second.m_pGameSquareOption->wMatchID <<  "|" << iter->second.m_pGameSquareOption->szSquareName\
					<< "> CAttemperEngineSink::OnDBLoadSquareReward j= " << j << " PropertyID= " << iter->second.m_pGameSquareOption->RankBonus[i].wPropertyID[j]);
			}
		}
	}

	return true;
}

// �Խ�����ɹ�
bool CAttemperEngineSink::OnDBCreateCustomTableSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_UserCustomTableSuccess * pUserCustomTableSuccess = (DBO_GR_UserCustomTableSuccess *)pData;

	//���Ӳ���
	tagTableFrameParameter TableFrameParameter;
	ZeroMemory(&TableFrameParameter, sizeof(TableFrameParameter));

	//�ں����
	TableFrameParameter.pITimerEngine = m_pITimerEngine;
	TableFrameParameter.pIKernelDataBaseEngine = m_pIKernelDataBaseEngine;
	TableFrameParameter.pIRecordDataBaseEngine = m_pIRecordDataBaseEngine;

	//�������
	TableFrameParameter.pIMainServiceFrame = this;
	TableFrameParameter.pIAndroidUserManager = &m_AndroidUserManager;
	TableFrameParameter.pIGameServiceManager = m_pIGameServiceManager;

	//���ò���
	TableFrameParameter.pGameParameter = m_pGameParameter;
	TableFrameParameter.pGameServiceAttrib = m_pGameServiceAttrib;
	TableFrameParameter.pGameServiceOption = m_pGameServiceOption;

	if (m_pIGameMatchServiceManager != NULL)
		TableFrameParameter.pIGameMatchServiceManager = QUERY_OBJECT_PTR_INTERFACE(m_pIGameMatchServiceManager, IUnknownEx);


	// �ж��Ƿ��п��ظ����õ���
	// �еĻ��������е�����,û�еĻ�˵�����Ӷ��Ѿ�������,���û�����������Ϣ
	CTableFrame* pTableFrame = nullptr;
	if (pUserCustomTableSuccess->wTableID < m_TableFrameArray.size())
	{
		pTableFrame = m_TableFrameArray.at(pUserCustomTableSuccess->wTableID);
	}
	if (pTableFrame == nullptr)
	{
		return true;
	}

	//��������
	// �����Զ������
	TableFrameParameter.pTableOption = &(m_pGameServiceTableOption->TableOption[pUserCustomTableSuccess->wTableID]);
	memcpy(TableFrameParameter.pTableOption->cbCustomRule, pUserCustomTableSuccess->cbCustomRule, sizeof(TableFrameParameter.pTableOption->cbCustomRule));
	TableFrameParameter.pTableOption->dwCreatUserID = pUserCustomTableSuccess->dwCreateUserID;
	TableFrameParameter.pTableOption->dwRoundCount = pUserCustomTableSuccess->dwRoundCount;
	TableFrameParameter.pTableOption->TableCreateTime = pUserCustomTableSuccess->tmCreateCustomTable;
	lstrcpyn(TableFrameParameter.pTableOption->szSecret, pUserCustomTableSuccess->szSecret, sizeof(TableFrameParameter.pTableOption->szSecret));
	
	// �������ӱ���
	if (pTableFrame->InitializationFrame(pUserCustomTableSuccess->wTableID, TableFrameParameter) == false)
	{
		return true;
	}

	// ��������
	if (m_pIGameServiceManager)
	{
		m_pIGameServiceManager->RectifyParameter(*m_pGameServiceOption, m_pGameServiceTableOption->TableOption[pUserCustomTableSuccess->wTableID]);
	}

	// ���سɹ���Ϣ
	CMD_CS_C_UserCustomTableSuccess UserCustomTableSuccess;
	memset(&UserCustomTableSuccess, 0, sizeof(UserCustomTableSuccess));
	UserCustomTableSuccess.wKindID = pUserCustomTableSuccess->wKindID;
	UserCustomTableSuccess.wServerID = pUserCustomTableSuccess->wServerID;
	lstrcpyn(UserCustomTableSuccess.szSecret, pUserCustomTableSuccess->szSecret, CountArray(UserCustomTableSuccess.szSecret));
	UserCustomTableSuccess.dwSocketID = dwContextID;
	UserCustomTableSuccess.dwPropertyCount = pUserCustomTableSuccess->dwPropertyCount;
	m_pITCPSocketService->SendData(MDM_CS_REMOTE_SERVICE, SUB_CS_C_USER_CUSTOM_TABLE_SUCCESS, &UserCustomTableSuccess, sizeof(UserCustomTableSuccess));
	
	return true;
}

// �Խ�(����)����ʧ��
bool CAttemperEngineSink::OnDBCreateCustomTableFailed(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_UserCustomTableFailure * pUserCustomTableFailure = (DBO_GR_UserCustomTableFailure *)pData;

	// �������
	CTableFrame* pTableFrame = nullptr;
	if (pUserCustomTableFailure->wTableID < m_TableFrameArray.size())
	{
		pTableFrame = m_TableFrameArray.at(pUserCustomTableFailure->wTableID);
	}
	if (pTableFrame == nullptr)
	{
		return true;
	}
	// �������
	pTableFrame->SetLockForCustom(false);

	CMD_CS_C_UserCustomTableFailed UserCustomTableFailed;
	UserCustomTableFailed.lErrorCode = pUserCustomTableFailure->lResultCode;
	lstrcpyn(UserCustomTableFailed.szDescribeString, pUserCustomTableFailure->szDescribeString, CountArray(UserCustomTableFailed.szDescribeString));
	UserCustomTableFailed.dwSocketID = dwContextID;
	m_pITCPSocketService->SendData(MDM_CS_REMOTE_SERVICE, SUB_CS_C_USER_CUSTOM_TABLE_FAILED, &UserCustomTableFailed, sizeof(UserCustomTableFailed));

	return true;
}

// ��ɢ���ӳɹ�
bool CAttemperEngineSink::OnDBDismissCustomTableSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_DismissCustomTableSuccess * pDismissCustomTableSuccess = (DBO_GR_DismissCustomTableSuccess *)pData;

	CMD_GF_S_ConludeTable ConludeTable;
	memset(&ConludeTable, 0, sizeof(ConludeTable));
	ConludeTable.wConcludeType = pDismissCustomTableSuccess->wConcludeType;
	ConludeTable.dwTableCreater = pDismissCustomTableSuccess->dwTableCreater;											// ���Ӵ�����
	ConludeTable.dwDiamondCount = pDismissCustomTableSuccess->dwPropertyCount;
	switch (pDismissCustomTableSuccess->wConcludeType)
	{
		case CONCLUDE_TYPE_NORMAL:				// 1:��������,��������;
		{
			break;
		}
		case CONCLUDE_TYPE_CREATER_DISMISS:
		{
//			SendGameMessage(TEXT("��Ϸ�ѱ�������ɢ.��Ϸδ��ʼ,��ɢ���䲻��۳���ʯ."), SMT_CHAT | SMT_EJECT | SMT_CLOSE_GAME);
			_sntprintf(ConludeTable.szMessage, sizeof(ConludeTable.szMessage), TEXT("��Ϸ�ѱ�������ɢ.��Ϸδ��ʼ,��ɢ���䲻��۳���ʯ."));
			break;
		}
		case CONCLUDE_TYPE_TIMEOUT_NOSTART:
		{
			// ��ʾ��ɢ
//			SendGameMessage(TEXT("��Ϸ��ʱ��δ��ʼ,�ѱ���ɢ.��Ϸδ��ʼ,��ɢ���䲻��۳���ʯ."), SMT_CHAT | SMT_EJECT | SMT_CLOSE_GAME);
			_sntprintf(ConludeTable.szMessage, sizeof(ConludeTable.szMessage), TEXT("��Ϸ��ʱ��δ��ʼ,�ѱ���ɢ.��Ϸδ��ʼ,��ɢ���䲻��۳���ʯ."));
			break;
		}
		case CONCLUDE_TYPE_PLAYER_DISMISS:
		{
//			SendGameMessage(TEXT("�������ͬ���ɢ��Ϸ,��Ϸ�ѱ���ɢ."), SMT_CHAT | SMT_EJECT | SMT_CLOSE_GAME);
			_sntprintf(ConludeTable.szMessage, sizeof(ConludeTable.szMessage), TEXT("�������ͬ���ɢ��Ϸ,��Ϸ�ѱ���ɢ."));
			break;
		}
		case CONCLUDE_TYPE_TIMEOUT_START:
		{
			_sntprintf(ConludeTable.szMessage, sizeof(ConludeTable.szMessage), TEXT("�Խ����䳤ʱ��δ����,�ѱ���ɢ."));
			break;
		}
		default:
		{
			break;
		}
	}

	// ����������Ϣ
	CTableFrame* pTableFrame = nullptr;
	if (pDismissCustomTableSuccess->wTableID < m_TableFrameArray.size())
	{
		pTableFrame = m_TableFrameArray.at(pDismissCustomTableSuccess->wTableID);
	}
	if (pTableFrame)
	{
		// ���վ��
		for (WORD i = 0; i < pTableFrame->GetChairCount(); i++)
		{
			IServerUserItem* pTableUserItem = pTableFrame->GetTableUserItem(i);
			if (pTableUserItem == nullptr)
			{
				continue;
			}
			pTableFrame->SendTableData(i, SUB_GF_S_CONLUDETABLE, &ConludeTable, sizeof(ConludeTable), MDM_GF_FRAME);
		}

		// ��ֹ��Ϸ
		if (pTableFrame->IsGameStarted())
		{
			// ��Ϸ��ʼ,��ֹ��Ϸ
			pTableFrame->DismissGame();
		}
		else if (pTableFrame->IsTableStarted())
		{
			// ��Ϸû��ʼ,�������ӿ�ʼ,˵�����������м�
			// ��ɢ����
			switch (pDismissCustomTableSuccess->wConcludeType)
			{
				case CONCLUDE_TYPE_NORMAL:
				case CONCLUDE_TYPE_PLAYER_DISMISS:
				case CONCLUDE_TYPE_TIMEOUT_START:
				{
					pTableFrame->ConcludeTable();
				}
			}
		}

		// ���վ��
		for (WORD i = 0; i < pTableFrame->GetChairCount(); i++)
		{
			IServerUserItem* pTableUserItem = pTableFrame->GetTableUserItem(i);
			if (pTableUserItem == nullptr)
			{
				continue;
			}
			pTableFrame->PerformStandUpAction(pTableUserItem);
		}

		pTableFrame->Reset();
	}

	return true;
}

// ��ɢ����ʧ��
bool CAttemperEngineSink::OnDBDismissCustomTableFailed(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_DismissCustomTableFailure * pDismissCustomTableFailure = (DBO_GR_DismissCustomTableFailure *)pData;
	LOGE("CAttemperEngineSink::OnDBDismissCustomTableFailed, TableID=" << pDismissCustomTableFailure->wTableID << ", Reason=" << pDismissCustomTableFailure->szDescribeString);
	
	CTableFrame* pTableFrame = nullptr;
	if (pDismissCustomTableFailure->wTableID < m_TableFrameArray.size())
	{
		pTableFrame = m_TableFrameArray.at(pDismissCustomTableFailure->wTableID);
	}
	if (pTableFrame)
	{
		// ��ɢʧ��
		pTableFrame->SetLockForDismiss(false);
	}
	return true;
}

// �������ӳɹ�
bool CAttemperEngineSink::OnDBJoinCustomTableSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	LOGI("CAttemperEngineSink::OnDBJoinCustomTableSuccess");
	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem = GetBindUserItem(wBindIndex);
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

	DBO_GR_UserJoinCustomTableSuccess * pUserJoinCustomTableSuccess = (DBO_GR_UserJoinCustomTableSuccess *)pData;
	CTableFrame* pTableFrame = nullptr;
	if (pUserJoinCustomTableSuccess->wTableID < m_TableFrameArray.size())
	{
		pTableFrame = m_TableFrameArray.at(pUserJoinCustomTableSuccess->wTableID);
	}

	if (pTableFrame == nullptr)
	{
		SendRequestFailure(pIServerUserItem, TEXT("������ķ��䲻����,��ȷ�Ϻ��ټ��룡"), 0);
		return false;
	}

	// �������Ƿ��Ѿ�����λ����,�ڵĻ�ֱ�ӾͲ����ظ�����
	if (pIServerUserItem->GetTableID() != INVALID_TABLE && pIServerUserItem->GetTableID() != INVALID_CHAIR)
	{
		return true;
	}

	// ����Ƿ��п���λ,��������
	WORD wRequestChairID = INVALID_CHAIR;
	if (pUserJoinCustomTableSuccess->wChairID == INVALID_CHAIR)
	{
		bool bCustomTableCreateIn = false;
		for (WORD i = 0; i < pTableFrame->GetChairCount(); i++)
		{
			IServerUserItem* pIServerUserItem = pTableFrame->GetTableUserItem(i);
			if (pIServerUserItem == nullptr)
			{
				continue;
			}
			if (pIServerUserItem->GetUserID() == pTableFrame->GetGameTableOption()->dwCreatUserID)
			{
				// �������Ѿ�����λ��
				bCustomTableCreateIn = true;
				break;
			}
		}

		// ����Ƿ��п�λ��
		if (pTableFrame->GetNullChairCount() == 1 && bCustomTableCreateIn == false && pIServerUserItem->GetUserID() != pTableFrame->GetGameTableOption()->dwCreatUserID)
		{
			// ֻʣһ����λ,���ҿ�����û����λ��
			LOGW("CAttemperEngineSink::OnTCPNetworkMainUserCustom SUB_GR_USER_JOIN_TABLE ֻʣһ��λ��,�ҿ�����û����λ��");
			SendRequestFailure(pIServerUserItem, TEXT("���ڴ���Ϸ����ʱû�п������������λ���ˣ���ѡ���������Ϸ����"), 0);
			return true;
		}
		else
		{
			// û��������,���������λ
			wRequestChairID = pTableFrame->GetRandNullChairID();
		}
	}
	else
	{
		wRequestChairID = pUserJoinCustomTableSuccess->wChairID;
	}

	//����ж�
	if (wRequestChairID == INVALID_CHAIR)
	{
		LOGW("CAttemperEngineSink::OnTCPNetworkMainUserCustom SUB_GR_USER_JOIN_TABLE ����û�п�λ����");
		SendRequestFailure(pIServerUserItem, TEXT("���ڴ���Ϸ����ʱû�п������������λ���ˣ���ѡ���������Ϸ����"), 0);
		return true;
	}

	// ����ͬ����Ϣ
	for (WORD j = 0; j < pTableFrame->GetChairCount(); j++)
	{
		IServerUserItem* pTableUserItem = pTableFrame->GetTableUserItem(j);
		if (pTableUserItem == nullptr || pTableUserItem == pIServerUserItem)
		{
			continue;
		}

		// ͬ������Ϣ���͸��Լ�
		SendUserInfoPacket(pTableUserItem, dwContextID);

		// �Լ�����Ϣ���͸�ͬ��
		tagBindParameter * pBindParameter = GetBindParameter(pTableUserItem->GetBindIndex());
		if (pBindParameter != nullptr)
		{
			SendUserInfoPacket(pIServerUserItem, pBindParameter->dwSocketID);
		}
	}

	// ��������
	if ( pTableFrame->PerformSitDownAction(wRequestChairID, pIServerUserItem) == false )
	{
//		SendRequestFailure(pIServerUserItem, TEXT("λ�ô���,���뷿��ʧ�ܣ�"), 0);
		return true;
	}
	else
	{
		// ���³ɹ�
	}
	return true;
}

// ��������ʧ��
bool CAttemperEngineSink::OnDBJoinCustomTableFailed(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_UserJoinCustomTableFailure* pUserRoundScoreInCustomTable = (DBO_GR_UserJoinCustomTableFailure*)pData;

	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem = GetBindUserItem(wBindIndex);
	SendRequestFailure(pIServerUserItem, pUserRoundScoreInCustomTable->szDescribeString, 0);

	return true;
}

// ������Զ��������ϵķ���
bool CAttemperEngineSink::OnDBUserRoundScoreInCustomTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	DBO_GR_UserRoundScoreInCustomTable* pUserRoundScoreInCustomTable = (DBO_GR_UserRoundScoreInCustomTable*)pData;

	if (pUserRoundScoreInCustomTable)
	{
		IServerUserItem* pServerUserItem = m_ServerUserManager.SearchUserItem(pUserRoundScoreInCustomTable->dwUserID);
		if (pServerUserItem)
		{
			tagVariationInfo RecordInfo;
			pServerUserItem->QueryRecordInfo(RecordInfo);
			RecordInfo.lScore += pUserRoundScoreInCustomTable->lRoundScore;
			pServerUserItem->SetRecordInfo(RecordInfo);
		}
	}
	return true;
}

//ע���¼�
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//��������
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;

			//Ч�����
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���
			m_bNeekCorrespond=false;
			m_pITCPSocketService->CloseSocket();

			//��ʾ��Ϣ
			if (lstrlen(pRegisterFailure->szDescribeString)>0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString,TraceLevel_Exception);
				LOGI("CAttemperEngineSink::OnTCPSocketMainRegister, "<<pRegisterFailure->szDescribeString);
			}

			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//�б��¼�
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVER_INFO:		//������Ϣ
		{
			//�����б�
			m_ServerListManager.DisuseServerItem();

			return true;
		}
	case SUB_CS_S_SERVER_ONLINE:	//��������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerOnLine));
			if (wDataSize!=sizeof(CMD_CS_S_ServerOnLine)) return false;

			//��������
			CMD_CS_S_ServerOnLine * pServerOnLine=(CMD_CS_S_ServerOnLine *)pData;

			//���ҷ���
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerOnLine->wServerID);

			//��������
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->m_GameServer.dwOnLineCount=pServerOnLine->dwOnLineCount;
			}

			return true;
		}
	case SUB_CS_S_SERVER_INSERT:	//�������
		{
			//Ч�����
			ASSERT(wDataSize%sizeof(tagGameServer)==0);
			if (wDataSize%sizeof(tagGameServer)!=0) return false;

			//��������
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;

			//��������
			for (WORD i=0;i<wItemCount;i++)
			{
				m_ServerListManager.InsertGameServer(pGameServer++);
			}

			return true;
		}
	case SUB_CS_S_SERVER_MODIFY:	//�����޸�
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerModify));
			if (wDataSize!=sizeof(CMD_CS_S_ServerModify)) return false;

			//��������
			CMD_CS_S_ServerModify * pServerModify=(CMD_CS_S_ServerModify *)pData;

			//���ҷ���
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerModify->wServerID);

			//���÷���
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->m_GameServer.wNodeID=pServerModify->wNodeID;
				pGameServerItem->m_GameServer.wSortID=pServerModify->wSortID;
				pGameServerItem->m_GameServer.wServerPort=pServerModify->wServerPort;
				pGameServerItem->m_GameServer.dwOnLineCount=pServerModify->dwOnLineCount;
				pGameServerItem->m_GameServer.dwFullCount=pServerModify->dwFullCount;
				lstrcpyn(pGameServerItem->m_GameServer.szServerName,pServerModify->szServerName,CountArray(pGameServerItem->m_GameServer.szServerName));
				lstrcpyn(pGameServerItem->m_GameServer.szServerAddr,pServerModify->szServerAddr,CountArray(pGameServerItem->m_GameServer.szServerAddr));
			}

			return true;
		}
	case SUB_CS_S_SERVER_REMOVE:	//����ɾ��
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerRemove));
			if (wDataSize!=sizeof(CMD_CS_S_ServerRemove)) return false;

			//��������
			CMD_CS_S_ServerRemove * pServerRemove=(CMD_CS_S_ServerRemove *)pData;

			//��������
			m_ServerListManager.DeleteGameServer(pServerRemove->wServerID);

			return true;
		}
	case SUB_CS_S_SERVER_FINISH:	//�������
		{
			//�����б�
			m_ServerListManager.CleanServerItem();

			//�¼�����
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_COLLECT_REQUEST:	//�û�����
		{
			//��������
			CMD_CS_C_UserEnter UserEnter;
			ZeroMemory(&UserEnter,sizeof(UserEnter));

			//�����û�
			WORD wIndex=0;
			do
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=m_ServerUserManager.EnumUserItem(wIndex++);
				if (pIServerUserItem==NULL) break;

				//���ñ���
				UserEnter.dwUserID=pIServerUserItem->GetUserID();
				UserEnter.dwGameID=pIServerUserItem->GetGameID();
				lstrcpyn(UserEnter.szNickName,pIServerUserItem->GetNickName(),CountArray(UserEnter.szNickName));

				//������Ϣ
				UserEnter.cbGender=pIServerUserItem->GetGender();
				UserEnter.cbMemberOrder=pIServerUserItem->GetMemberOrder();
				UserEnter.cbMasterOrder=pIServerUserItem->GetMasterOrder();

				//��������
				ASSERT(m_pITCPSocketService!=NULL);
				m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_ENTER,&UserEnter,sizeof(UserEnter));

			} while (true);

			//�㱨���
			m_bCollectUser=true;
			m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_FINISH);

			return true;
		}
	}

	return true;
}

bool CAttemperEngineSink::OnTCPSocketMainRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
		case SUB_CS_S_USER_CUSTOM_TABLE:	// ��������
		{
			return OnTCPSocketUserCreateCustomTable(pData,wDataSize);
		}
		default:
			break;
	}
	return true;
}

//�������
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SYSTEM_MESSAGE:	//ϵͳ��Ϣ
		{
			//��Ϣ����
			SendSystemMessage((CMD_GR_SendMessage *)pData, wDataSize);

			return true;
		}
	case SUB_CS_S_PROPERTY_TRUMPET:  //������Ϣ
		{
			if (pData == NULL)
			{
				return true;
			}
			// ������Ϣ,�õ������ߵ�����ID
			CMD_CS_S_SendTrumpet* pSendTrumpet = (CMD_CS_S_SendTrumpet*)pData;
			CMD_GR_S_SendTrumpet SendTrumpet;
			SendTrumpet.wPropertyIndex = pSendTrumpet->wPropertyIndex;							//�������� 
			SendTrumpet.dwSendUserID = pSendTrumpet->dwSendUserID;								//�û� I D
			SendTrumpet.TrumpetColor = pSendTrumpet->TrumpetColor;								//������ɫ
			_tcsncpy(SendTrumpet.szSendNickName, pSendTrumpet->szSendNickName, sizeof(SendTrumpet.szSendNickName));
			_tcsncpy(SendTrumpet.szTrumpetContent, pSendTrumpet->szTrumpetContent, sizeof(SendTrumpet.szTrumpetContent));
			// �������
			DWORD dwAreaID = pSendTrumpet->dwSendUserAreaID;
			WORD wEnumIndex = 0;
			while (wEnumIndex < m_ServerUserManager.GetUserItemCount())
			{
				IServerUserItem *pIUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
				if (pIUserItem == NULL || ( dwAreaID!=0 && pIUserItem->GetAreaID() != dwAreaID))
				{
					continue;
				}
				WORD wBindIndex = pIUserItem->GetBindIndex();
				tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);
				if (pBindParameter)
				{
					m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_USER, SUB_GR_PROPERTY_TRUMPET, &SendTrumpet, sizeof(SendTrumpet));
				}
			}
			//��������
			//m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_PROPERTY_TRUMPET,pData,wDataSize,BG_COMPUTER);

			return true;
		}
	}

	return true;
}

//��¼����
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_LOGON_USERID:		//I D ��¼
		{
			return OnTCPNetworkSubLogonUserID(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_LOGON_MOBILE:		//�ֻ���¼
		{
			return OnTCPNetworkSubLogonMobile(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_LOGON_ACCOUNTS:		//�ʺŵ�¼
		{
			return OnTCPNetworkSubLogonAccounts(pData,wDataSize,dwSocketID);
		}
	}

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_RULE:			//�û�����
		{
			return OnTCPNetworkSubUserRule(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_LOOKON:		//�û��Թ�
		{
			return OnTCPNetworkSubUserLookon(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_SITDOWN:		//�û�����
		{
			return OnTCPNetworkSubUserSitDown(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_STANDUP:		//�û�����
		{
			return OnTCPNetworkSubUserStandUp(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAT:			//�û�����
		{
			return OnTCPNetworkSubUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_EXPRESSION:	//�û�����
		{
			return OnTCPNetworkSubUserExpression(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_WISPER_CHAT:		//�û�˽��
		{
			return OnTCPNetworkSubWisperChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_WISPER_EXPRESSION:	//˽�ı���
		{
			return OnTCPNetworkSubWisperExpression(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_PROPERTY_BUY:		//�������
		{
			return OnTCPNetworkSubPropertyBuy(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_PROPERTY_TRUMPET:   //���ȵ���
		{
			return OnTCPNetwordSubSendTrumpet(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_INVITE_REQ:    //�����û�
		{
			return OnTCPNetworkSubUserInviteReq(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_REPULSE_SIT:   //�ܾ�����
		{
			return OnTCPNetworkSubUserRepulseSit(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_KICK_USER:    //�߳��û�
		{
			return OnTCPNetworkSubMemberKickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_INFO_REQ:     //�����û���Ϣ
		{
			return OnTCPNetworkSubUserInfoReq(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAIR_REQ:    //�������λ��
		{
			return OnTCPNetworkSubUserChairReq(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAIR_INFO_REQ: //���������û���Ϣ
		{
			return OnTCPNetworkSubChairUserInfoReq(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//���д���
bool CAttemperEngineSink::OnTCPNetworkMainInsure(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
//	LOGI(_T("CAttemperEngineSink::OnTCPNetworkMainInsure, wSubCmdID="<<wSubCmdID));
	switch (wSubCmdID)
	{
	case SUB_GR_QUERY_INSURE_INFO:		//���в�ѯ
		{
			return OnTCPNetworkSubQueryInsureInfo(pData,wDataSize,dwSocketID);
		}
    case SUB_GR_QUERY_TRANSRECORD:
        {
            return OnTCPNetworkSubQueryTransRecord(pData,wDataSize,dwSocketID);
        }
	case SUB_GR_SAVE_SCORE_REQUEST:		//�������
		{
			return OnTCPNetworkSubSaveScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TAKE_SCORE_REQUEST:		//ȡ������
		{
			return OnTCPNetworkSubTakeScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TRANSFER_SCORE_REQUEST:	//ת������
		{
			return OnTCPNetworkSubTransferScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_USER_INFO_REQUEST:	//��ѯ�û�
		{
			return OnTCPNetworkSubQueryUserInfoRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_NICKNAME_BY_GAMEID:	// ����GameID��ѯ�ǳ�
		{
			return OnTCPNetworkSubQueryNickNameByGameID(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_VERIFY_INSURE_PASSWORD:		// ��֤��������
		{
			return OnTCPNetworkSubVerifyInsurePassword(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_MODIFY_INSURE_PASS:			// ������������
		{
			return OnTCPNetworkSubModifyInsurePassword(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//������
bool CAttemperEngineSink::OnTCPNetworkMainManage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_QUERY_OPTION:		//��ѯ����
		{
			return OnTCPNetworkSubQueryOption(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_OPTION_SERVER:		//��������
		{
			return OnTCPNetworkSubOptionServer(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_KILL_USER:          //�߳��û�
		{
			return OnTCPNetworkSubManagerKickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_LIMIT_USER_CHAT:	//��������
		{
			return OnTCPNetworkSubLimitUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_KICK_ALL_USER:		//�߳��û�
		{
			return OnTCPNetworkSubKickAllUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_SEND_MESSAGE:		//������Ϣ
		{
			return OnTCPNetworkSubSendMessage(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_DISMISSGAME:        //��ɢ��Ϸ
		{
			return OnTCPNetworkSubDismissGame(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//Ч��ӿ�
	ASSERT(m_pIGameMatchServiceManager!=NULL);
	if (m_pIGameMatchServiceManager==NULL) return false;

	//��Ϣ����
	return m_pIGameMatchServiceManager->OnEventSocketMatch(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID);
}

// �û��Խ���������
bool CAttemperEngineSink::OnTCPNetworkMainUserCustom(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_JOIN_TABLE:
	{
		return OnTCPNetworkUserJoinCustomTable(pData, wDataSize, dwSocketID);
	}
	default:
		break;
	}

// 	//Ч��ӿ�
// 	ASSERT(m_pIGameMatchServiceManager != NULL);
// 	if (m_pIGameMatchServiceManager == NULL) return false;
// 
// 	//��Ϣ����
// 	return m_pIGameMatchServiceManager->OnEventSocketUserCustom(wSubCmdID, pData, wDataSize, pIServerUserItem, dwSocketID);
	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//��ܴ���
bool CAttemperEngineSink::OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//I D ��¼
bool CAttemperEngineSink::OnTCPNetworkSubLogonUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonUserID));
	if (wDataSize<sizeof(CMD_GR_LogonUserID)) return false;

	//������Ϣ
	CMD_GR_LogonUserID * pLogonUserID=(CMD_GR_LogonUserID *)pData;
	pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
	pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;

	//����Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIBindUserItem=GetBindUserItem(wBindIndex);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//�ظ��ж�
	if ((pBindParameter==NULL)||(pIBindUserItem!=NULL))
	{ 
		ASSERT(FALSE);
		return false;
	}

	//�����ж�
	if(pLogonUserID->wKindID != m_pGameServiceOption->wKindID)
	{
		//����ʧ��
		SendLogonFailure(TEXT("�ܱ�Ǹ������Ϸ�����Ѿ��ر��ˣ�������������룡"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
		return true;
	}

	//�����˺����˲�����
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLogonUserID->dwUserID);
	if (pIServerUserItem!=NULL)
	{
		if((pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr!=0L))
			|| (!pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr==0L)))
		{
			SendRoomMessage(dwSocketID, TEXT("���˺����ڴ˷�����Ϸ���Ҳ������߳�������ѯ����Ա��"), SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
			return true;
		}
	}

	//�汾��Ϣ
	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonUserID->dwPlazaVersion;
	pBindParameter->dwFrameVersion=pLogonUserID->dwFrameVersion;
	pBindParameter->dwProcessVersion=pLogonUserID->dwProcessVersion;

	//�����汾
	DWORD dwPlazaVersion=pLogonUserID->dwPlazaVersion;
	DWORD dwFrameVersion=pLogonUserID->dwFrameVersion;
	DWORD dwClientVersion=pLogonUserID->dwProcessVersion;
	if (PerformCheckVersion(dwPlazaVersion, dwFrameVersion, dwClientVersion, dwSocketID) == false)
	{
		LOGI("CAttemperEngineSink::OnTCPNetworkSubLogonUserID, PerformCheckVersion==false");
		return true;
	}

	//�л��ж�
	if((pIServerUserItem!=NULL)&&(pIServerUserItem->ContrastLogonPass(pLogonUserID->szPassword)==true))
	{
		SwitchUserItemConnect(pIServerUserItem,pLogonUserID->szMachineID,wBindIndex);
		return true;
	}

	if(m_pIGameMatchServiceManager!=NULL&&m_pIGameMatchServiceManager->OnEventEnterMatch(dwSocketID,(VOID*)pLogonUserID, pBindParameter->dwClientAddr))
	{
		//SendRoomMessage(dwSocketID, szPrint, SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
		return true;
	}
	//��������
	DBR_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//��������
	LogonUserID.dwUserID=pLogonUserID->dwUserID;
	LogonUserID.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));

#ifdef _DEBUG
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	LOGD("WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
#endif

	return true;
}

//�ֻ���¼
bool CAttemperEngineSink::OnTCPNetworkSubLogonMobile(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonMobile));
	if (wDataSize<sizeof(CMD_GR_LogonMobile)) return false;

	//������Ϣ
	CMD_GR_LogonMobile * pLogonMobile=(CMD_GR_LogonMobile *)pData;
	pLogonMobile->szPassword[CountArray(pLogonMobile->szPassword)-1]=0;
	pLogonMobile->szMachineID[CountArray(pLogonMobile->szMachineID)-1]=0;

	//����Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIBindUserItem=GetBindUserItem(wBindIndex);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//�ظ��ж�
	if ((pBindParameter==NULL)||(pIBindUserItem!=NULL))
	{ 
		ASSERT(FALSE);
		return false;
	}

	//�����˺����˲�����
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLogonMobile->dwUserID);
	if (pIServerUserItem!=NULL)
	{
		if((pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr!=0L))
			|| (!pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr==0L)))
		{
			SendRoomMessage(dwSocketID, TEXT("���˺����ڴ˷�����Ϸ���Ҳ������߳�������ѯ����Ա��"), SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
			return false;
		}
	}

	//�汾��Ϣ
	pBindParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pBindParameter->dwProcessVersion=pLogonMobile->dwProcessVersion;

	//�����汾(��ʱ��ȥ���汾)
//	DWORD dwClientVersion=pLogonMobile->dwProcessVersion;
//	if (PerformCheckVersion(0L,0L,dwClientVersion,dwSocketID, true)==false) return true;

	//�л��ж�
	if ((pIServerUserItem!=NULL)&&(pIServerUserItem->ContrastLogonPass(pLogonMobile->szPassword)==true))
	{
		SwitchUserItemConnect(pIServerUserItem,pLogonMobile->szMachineID,wBindIndex,pLogonMobile->cbDeviceType,pLogonMobile->wBehaviorFlags,pLogonMobile->wPageTableCount);
		return true;
	}

	//��������
	DBR_GR_LogonMobile LogonMobile;
	ZeroMemory(&LogonMobile,sizeof(LogonMobile));

	//��������
	LogonMobile.dwUserID=pLogonMobile->dwUserID;
	LogonMobile.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(LogonMobile.szPassword,pLogonMobile->szPassword,CountArray(LogonMobile.szPassword));
	lstrcpyn(LogonMobile.szMachineID,pLogonMobile->szMachineID,CountArray(LogonMobile.szMachineID));
	LogonMobile.cbDeviceType=pLogonMobile->cbDeviceType;
	LogonMobile.wBehaviorFlags=pLogonMobile->wBehaviorFlags;
	LogonMobile.wPageTableCount=pLogonMobile->wPageTableCount;

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(LogonMobile.dwUserID,DBR_GR_LOGON_MOBILE,dwSocketID,&LogonMobile,sizeof(LogonMobile));

	return true;
}

//�ʺŵ�¼
bool CAttemperEngineSink::OnTCPNetworkSubLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonAccounts));
	if (wDataSize<=sizeof(CMD_GR_LogonAccounts)) return false;

	//������Ϣ
	CMD_GR_LogonAccounts * pLogonAccounts=(CMD_GR_LogonAccounts *)pData;
	pLogonAccounts->szPassword[CountArray(pLogonAccounts->szPassword)-1]=0;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szMachineID[CountArray(pLogonAccounts->szMachineID)-1]=0;

	//����Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIBindUserItem=GetBindUserItem(wBindIndex);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//�ظ��ж�
	if (pIBindUserItem!=NULL)
	{ 
		ASSERT(FALSE);
		return false;
	}

	//�����˺����˲�����
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLogonAccounts->szAccounts);
	if (pIServerUserItem!=NULL)
	{
		if((pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr!=0L))
			|| (!pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr==0L)))
		{
			SendRoomMessage(dwSocketID, TEXT("���˺����ڴ˷�����Ϸ���Ҳ������߳�������ѯ����Ա��"), SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
			return false;
		}
	}

	//�汾��Ϣ
	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonAccounts->dwPlazaVersion;
	pBindParameter->dwFrameVersion=pLogonAccounts->dwFrameVersion;
	pBindParameter->dwProcessVersion=pLogonAccounts->dwProcessVersion;

	//�����汾
	DWORD dwPlazaVersion=pLogonAccounts->dwPlazaVersion;
	DWORD dwFrameVersion=pLogonAccounts->dwFrameVersion;
	DWORD dwClientVersion=pLogonAccounts->dwProcessVersion;
	if (PerformCheckVersion(dwPlazaVersion, dwFrameVersion, dwClientVersion, dwSocketID) == false)
	{
		LOGI("CAttemperEngineSink::OnTCPNetworkSubLogonAccounts, PerformCheckVersion==false");
		return true;
	}

	//�л��ж�
	if ((pIServerUserItem!=NULL)&&(pIServerUserItem->ContrastLogonPass(pLogonAccounts->szPassword)==true))
	{
		SwitchUserItemConnect(pIServerUserItem,pLogonAccounts->szMachineID,wBindIndex);
		return true;
	}

	//��������
	DBR_GR_LogonAccounts LogonAccounts;
	ZeroMemory(&LogonAccounts,sizeof(LogonAccounts));

	//��������
	LogonAccounts.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(LogonAccounts.szAccounts,pLogonAccounts->szAccounts,CountArray(LogonAccounts.szAccounts));
	lstrcpyn(LogonAccounts.szPassword,pLogonAccounts->szPassword,CountArray(LogonAccounts.szPassword));
	lstrcpyn(LogonAccounts.szMachineID,pLogonAccounts->szMachineID,CountArray(LogonAccounts.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOGON_ACCOUNTS,dwSocketID,&LogonAccounts,sizeof(LogonAccounts));

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserRule(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_UserRule));
	if (wDataSize<sizeof(CMD_GR_UserRule)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�����ж�
	//ASSERT(CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==false);
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//��Ϣ����
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	CMD_GR_UserRule * pCMDUserRule=(CMD_GR_UserRule *)pData;

	//�����־
	pUserRule->bLimitSameIP=((pCMDUserRule->cbRuleMask&UR_LIMIT_SAME_IP)>0);
	pUserRule->bLimitWinRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_WIN_RATE)>0);
	pUserRule->bLimitFleeRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_FLEE_RATE)>0);
	pUserRule->bLimitGameScore=((pCMDUserRule->cbRuleMask&UR_LIMIT_GAME_SCORE)>0);

	//��������
	pUserRule->szPassword[0]=0;
	pUserRule->wMinWinRate=pCMDUserRule->wMinWinRate;
	pUserRule->wMaxFleeRate=pCMDUserRule->wMaxFleeRate;
	pUserRule->lMaxGameScore=pCMDUserRule->lMaxGameScore;
	pUserRule->lMinGameScore=pCMDUserRule->lMinGameScore;

	//��������
	if (wDataSize>sizeof(CMD_GR_UserRule))
	{
		//��������
		VOID * pDataBuffer=NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pCMDUserRule+1,wDataSize-sizeof(CMD_GR_UserRule));

		//��ȡ����
		while (true)
		{
			//��ȡ����
			pDataBuffer=RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe==DTP_NULL) break;

			//���ݷ���
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_GR_TABLE_PASSWORD:		//��������
				{
					//Ч������
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(pUserRule->szPassword));

					//�����ж�
					ASSERT(CServerRule::IsForfendLockTable(m_pGameServiceOption->dwServerRule)==false);
					if (CServerRule::IsForfendLockTable(m_pGameServiceOption->dwServerRule)==true) break;

					//��������
					if (DataDescribe.wDataSize<=sizeof(pUserRule->szPassword))
					{
						CopyMemory(&pUserRule->szPassword,pDataBuffer,DataDescribe.wDataSize);
						pUserRule->szPassword[CountArray(pUserRule->szPassword)-1]=0;
					}

					break;
				}
			}
		}
	}

	return true;
}

//�û��Թ�
bool CAttemperEngineSink::OnTCPNetworkSubUserLookon(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserLookon));
	if (wDataSize!=sizeof(CMD_GR_UserLookon)) return false;

	//Ч������
	CMD_GR_UserLookon * pUserLookon=(CMD_GR_UserLookon *)pData;
	if (pUserLookon->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserLookon->wTableID>=(WORD)m_TableFrameArray.size()) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��Ϣ����
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	if ((wTableID==pUserLookon->wTableID)&&(wChairID==pUserLookon->wChairID)&&(cbUserStatus==US_LOOKON)) return true;

	//�û��ж�
	if (cbUserStatus==US_TRUSTEE)
	{
		SendRequestFailure(pIServerUserItem,TEXT("�������йܴ��򣬲�����Ч����ȴ���ǰ��Ϸ������"),0);
		return true;
	}
	if (cbUserStatus == US_OFFLINE)
	{
		SendRequestFailure(pIServerUserItem, TEXT("�������ڶ���״̬, ������Ч, ��ȴ���ǰ��Ϸ������"), 0);
		return true;
	}
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),0);
		return true;
	}

	//�뿪����
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		LOGI("CAttemperEngineSink::OnTCPNetworkSubUserLookon PerformStandUpAction, NickName="<<pIServerUserItem->GetNickName());
		if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	}

	//���´���
	CTableFrame * pTableFrame=m_TableFrameArray[pUserLookon->wTableID];
	pTableFrame->PerformLookonAction(pUserLookon->wChairID,pIServerUserItem,pUserLookon->szPassword);

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserSitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserSitDown));
	if (wDataSize!=sizeof(CMD_GR_UserSitDown)) return false;

	//Ч������
	CMD_GR_UserSitDown * pUserSitDown=(CMD_GR_UserSitDown *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��Ϣ����
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//�ظ��ж�
	if ((pUserSitDown->wTableID<m_pGameServiceOption->wTableCount)&&(pUserSitDown->wChairID<m_pGameServiceAttrib->wChairCount))
	{
		CTableFrame * pTableFrame=m_TableFrameArray[pUserSitDown->wTableID];
		if (pTableFrame->GetTableUserItem(pUserSitDown->wChairID)==pIServerUserItem) return true;
	}

	//�û��ж�
	if (cbUserStatus==US_TRUSTEE)
	{
		SendRequestFailure(pIServerUserItem,TEXT("�������йܴ��򣬲�����Ч����ȴ���ǰ��Ϸ������"),0);
		return true;
	}
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),0);
		return true;
	}

	//�������
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		//����FALSEΪ ��ʱ����
		if(InsertDistribute(pIServerUserItem))
			return true;
	}

	//�뿪����
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		LOGI("CAttemperEngineSink::OnTCPNetworkSubUserSitDown PerformStandUpAction, NickName="<<pIServerUserItem->GetNickName());
		if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	}

	//������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
	{
		if(m_TableFrameArray[0]->EfficacyEnterTableScoreRule(0, pIServerUserItem))
			InsertWaitDistribute(pIServerUserItem);

		return true;
	}

	//�������
	WORD wRequestTableID=pUserSitDown->wTableID;
	WORD wRequestChairID=pUserSitDown->wChairID;

	//���ӵ���
	if (wRequestTableID>=m_TableFrameArray.size())
	{
		//��ʼ����
		WORD wStartTableID=0;
		DWORD dwServerRule=m_pGameServiceOption->dwServerRule;
		if ((CServerRule::IsAllowAvertCheatMode(dwServerRule)==true)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) wStartTableID=1;

		//��̬����
		bool bDynamicJoin=true;
		if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
		if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

		//Ѱ��λ��
		for (size_t i=wStartTableID;i<m_TableFrameArray.size();i++)
		{
			//��Ϸ״̬
			if ((m_TableFrameArray[i]->IsGameStarted()==true)&&(bDynamicJoin==false))continue;

			//��ȡ��λ
			WORD wNullChairID=m_TableFrameArray[i]->GetNullChairID();

			//�������
			if (wNullChairID!=INVALID_CHAIR)
			{
				//���ñ���
				wRequestTableID=i;
				wRequestChairID=wNullChairID;

				break;
			}
		}

		//����ж�
		if ((wRequestTableID==INVALID_CHAIR)||(wRequestChairID==INVALID_CHAIR))
		{
			SendRequestFailure(pIServerUserItem,TEXT("��ǰ��Ϸ�����Ѿ�����Ϊ���ˣ���ʱû�п������������λ�ã����Ժ����ԣ�"),0);
			return true;
		}
	}

	//���ӵ���
	if (wRequestChairID>=m_pGameServiceAttrib->wChairCount)
	{
		//Ч�����
		ASSERT(wRequestTableID<m_TableFrameArray.size());
		if (wRequestTableID>=m_TableFrameArray.size()) return false;

		//���ҿ�λ
		wRequestChairID=m_TableFrameArray[wRequestTableID]->GetNullChairID();

		//����ж�
		if (wRequestChairID==INVALID_CHAIR)
		{
			SendRequestFailure(pIServerUserItem,TEXT("���ڴ���Ϸ����ʱû�п������������λ���ˣ���ѡ���������Ϸ����"),0);
			return true;
		}
	}

	//���´���
	CTableFrame * pTableFrame=m_TableFrameArray[wRequestTableID];
	pTableFrame->PerformSitDownAction(wRequestChairID,pIServerUserItem,pUserSitDown->szPassword);

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserStandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserStandUp));
	if (wDataSize!=sizeof(CMD_GR_UserStandUp)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	CMD_GR_UserStandUp * pUserStandUp=(CMD_GR_UserStandUp *)pData;

	//ȡ������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
	{
		POSITION pos=m_WaitDistributeList.GetHeadPosition();
		while(pos != NULL)
		{
			POSITION tempPos=pos;
			IServerUserItem *pUserItem = m_WaitDistributeList.GetNext(pos);
			if(pUserItem->GetUserID()==pIServerUserItem->GetUserID())
			{
				m_WaitDistributeList.RemoveAt(tempPos);
				break;
			}
		}

		if(pUserStandUp->wTableID==INVALID_TABLE) return true;
	}

	//Ч������
	if (pUserStandUp->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserStandUp->wTableID>=(WORD)m_TableFrameArray.size()) return false;

	//��Ϣ����
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID!=pUserStandUp->wTableID)||(wChairID!=pUserStandUp->wChairID)) return true;

	//�û��ж�
	if (pIServerUserItem->GetUserStatus()==US_TRUSTEE)
	{
		SendRequestFailure(pIServerUserItem,TEXT("�������йܴ��򣬲�����Ч����ȴ���ǰ��Ϸ������"),0);
		return true;
	}
	if ((pUserStandUp->cbForceLeave==FALSE)&&(pIServerUserItem->GetUserStatus()==US_PLAYING))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),0);
		return true;
	}

	//�뿪����
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		LOGI("CAttemperEngineSink::OnTCPNetworkSubUserStandUp PerformStandUpAction, NickName="<<pIServerUserItem->GetNickName());
		if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	}

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��������
	CMD_GR_C_UserChat * pUserChat=(CMD_GR_C_UserChat *)pData;

	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GR_C_UserChat));
	ASSERT(wDataSize>=(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0])));

	//Ч�����
	if (wDataSize>sizeof(CMD_GR_C_UserChat)) return false;
	if (wDataSize<(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0]))) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=NULL;
	if (pUserChat->dwTargetUserID!=0)
	{
		pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserChat->dwTargetUserID);
		if (pIRecvUserItem==NULL) return true;
	}

	//״̬�ж�
	if ((CServerRule::IsForfendRoomChat(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����ǰ����Ϸ�����ֹ�û��������죡"),SMT_CHAT);
		return true;
	}

	//Ȩ���ж�
	if (CUserRight::CanRoomChat(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����û�д������Ե�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//������Ϣ
	CMD_GR_S_UserChat UserChat;
	ZeroMemory(&UserChat,sizeof(UserChat));

	//�ַ�����
	SensitiveWordFilter(pUserChat->szChatString,UserChat.szChatString,CountArray(UserChat.szChatString));
	
	//��������
	UserChat.dwChatColor=pUserChat->dwChatColor;
	UserChat.wChatLength=pUserChat->wChatLength;
	UserChat.dwSendUserID=pISendUserItem->GetUserID();
	UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

	//ת����Ϣ
	WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
	SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_CHAT,&UserChat,wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]));

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_UserExpression));
	if (wDataSize!=sizeof(CMD_GR_C_UserExpression)) return false;

	//��������
	CMD_GR_C_UserExpression * pUserExpression=(CMD_GR_C_UserExpression *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=NULL;
	if (pUserExpression->dwTargetUserID!=0)
	{
		pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserExpression->dwTargetUserID);
		if (pIRecvUserItem==NULL) return true;
	}

	//״̬�ж�
	if ((CServerRule::IsForfendRoomChat(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����ǰ����Ϸ�����ֹ�û��������죡"),SMT_CHAT);
		return true;
	}

	//Ȩ���ж�
	if (CUserRight::CanRoomChat(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����û�д������Ե�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//������Ϣ
	CMD_GR_S_UserExpression UserExpression;
	ZeroMemory(&UserExpression,sizeof(UserExpression));

	//��������
	UserExpression.wItemIndex=pUserExpression->wItemIndex;
	UserExpression.dwSendUserID=pISendUserItem->GetUserID();
	UserExpression.dwTargetUserID=pUserExpression->dwTargetUserID;

	//ת����Ϣ
	SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));

	return true;
}

//�û�˽��
bool CAttemperEngineSink::OnTCPNetworkSubWisperChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��������
	CMD_GR_C_WisperChat * pWisperChat=(CMD_GR_C_WisperChat *)pData;

	//Ч�����
	ASSERT(wDataSize>=(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)+pWisperChat->wChatLength*sizeof(pWisperChat->szChatString[0])));

	//Ч�����
	if (wDataSize<(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)+pWisperChat->wChatLength*sizeof(pWisperChat->szChatString[0]))) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=m_ServerUserManager.SearchUserItem(pWisperChat->dwTargetUserID);
	if (pIRecvUserItem==NULL) return true;

	//״̬�ж�
	if ((CServerRule::IsForfendWisperChat(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����ǰ����Ϸ�����ֹ�û�˽�ģ�"),SMT_CHAT);
		return true;
	}

	//ͬ���ж�
	if ((CServerRule::IsForfendWisperOnGame(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		//��������
		bool bForfend=true;
		WORD wTableIDSend=pISendUserItem->GetTableID();
		WORD wTableIDRecv=pIRecvUserItem->GetTableID();

		//�����ж�
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pISendUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&((wTableIDSend==INVALID_TABLE)||(wTableIDSend!=wTableIDRecv))) bForfend=false;

		//��ʾ��Ϣ
		if (bForfend==true)
		{
			SendRoomMessage(pISendUserItem,TEXT("��Ǹ������Ϸ���䲻��������Ϸ����ͬ�������˽�ģ�"),SMT_EJECT|SMT_CHAT);
			return true;
		}
	}

	//Ȩ���ж�
	if (CUserRight::CanWisper(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����û�з���˽�ĵ�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//��������
	CMD_GR_S_WisperChat WisperChat;
	ZeroMemory(&WisperChat,sizeof(WisperChat));

	//�ַ�����
	SensitiveWordFilter(pWisperChat->szChatString,WisperChat.szChatString,CountArray(WisperChat.szChatString));

	//��������
	WisperChat.dwChatColor=pWisperChat->dwChatColor;
	WisperChat.wChatLength=pWisperChat->wChatLength;
	WisperChat.dwSendUserID=pISendUserItem->GetUserID();
	WisperChat.dwTargetUserID=pIRecvUserItem->GetUserID();
	WisperChat.wChatLength=CountStringBuffer(WisperChat.szChatString);

	//ת����Ϣ
	WORD wHeadSize=sizeof(WisperChat)-sizeof(WisperChat.szChatString);
	SendData(pISendUserItem,MDM_GR_USER,SUB_GR_WISPER_CHAT,&WisperChat,wHeadSize+WisperChat.wChatLength*sizeof(WisperChat.szChatString[0]));
	SendData(pIRecvUserItem,MDM_GR_USER,SUB_GR_WISPER_CHAT,&WisperChat,wHeadSize+WisperChat.wChatLength*sizeof(WisperChat.szChatString[0]));

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubWisperExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_WisperExpression));
	if (wDataSize!=sizeof(CMD_GR_C_WisperExpression)) return false;

	//��������
	CMD_GR_C_WisperExpression * pWisperExpression=(CMD_GR_C_WisperExpression *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=m_ServerUserManager.SearchUserItem(pWisperExpression->dwTargetUserID);
	if (pIRecvUserItem==NULL) return true;

	//״̬�ж�
	if ((CServerRule::IsForfendWisperChat(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����ǰ����Ϸ�����ֹ�û�˽�ģ�"),SMT_CHAT);
		return true;
	}

	//ͬ���ж�
	if ((CServerRule::IsForfendWisperOnGame(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		//��������
		bool bForfend=true;
		WORD wTableIDSend=pISendUserItem->GetTableID();
		WORD wTableIDRecv=pIRecvUserItem->GetTableID();

		//�����ж�
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pISendUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&((wTableIDSend==INVALID_TABLE)||(wTableIDSend!=wTableIDRecv))) bForfend=false;

		//��ʾ��Ϣ
		if (bForfend==true)
		{
			SendRoomMessage(pISendUserItem,TEXT("��Ǹ������Ϸ���䲻��������Ϸ����ͬ�������˽�ģ�"),SMT_EJECT|SMT_CHAT);
			return true;
		}
	}

	//Ȩ���ж�
	if (CUserRight::CanWisper(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����û�з���˽�ĵ�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//��������
	CMD_GR_S_WisperExpression WisperExpression;
	ZeroMemory(&WisperExpression,sizeof(WisperExpression));

	//��������
	WisperExpression.wItemIndex=pWisperExpression->wItemIndex;
	WisperExpression.dwSendUserID=pISendUserItem->GetUserID();
	WisperExpression.dwTargetUserID=pWisperExpression->dwTargetUserID;

	//ת����Ϣ
	SendData(pISendUserItem,MDM_GR_USER,SUB_GR_WISPER_EXPRESSION,&WisperExpression,sizeof(WisperExpression));
	SendData(pIRecvUserItem,MDM_GR_USER,SUB_GR_WISPER_EXPRESSION,&WisperExpression,sizeof(WisperExpression));

	return true;
}

//�������
bool CAttemperEngineSink::OnTCPNetworkSubPropertyBuy(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//�¼�����
	int cbResult = OnPropertyBuy(pData, wDataSize, dwSocketID);

	//����ж�
	if(cbResult == RESULT_ERROR) return false;
	if(cbResult == RESULT_FAIL) return true;
	if(cbResult == RESULT_SUCCESS) return true;

	return true;
}

//�������
int CAttemperEngineSink::OnPropertyBuy(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_PropertyBuy));
	if (wDataSize!=sizeof(CMD_GR_C_PropertyBuy)) return RESULT_ERROR;

	//��������
	CMD_GR_C_PropertyBuy * pPropertyBuy=(CMD_GR_C_PropertyBuy *)pData;

	//����Ч��
	ASSERT(pPropertyBuy->wItemCount>0);
	if (pPropertyBuy->wItemCount==0) return RESULT_ERROR;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return RESULT_ERROR;

	//Ŀ���û�
	IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pPropertyBuy->dwTargetUserID);
	if (pITargerUserItem==NULL) return RESULT_ERROR;

	//��������
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		//������Ϣ
		SendPropertyFailure(pIServerUserItem,TEXT("�������䲻����ʹ�ô˹��ܣ�"),0L,pPropertyBuy->cbRequestArea);

		return RESULT_FAIL;
	}

	//��ϰ����
	if (m_pGameServiceOption->wServerType==GAME_GENRE_EDUCATE)
	{
		SendPropertyFailure(pIServerUserItem,TEXT("��ϰ���䲻����ʹ�ô˹��ܣ�"),0L,pPropertyBuy->cbRequestArea);
		return RESULT_FAIL;
	}

	//��ǰ�¼�
	if(OnEventPropertyBuyPrep(pPropertyBuy->cbRequestArea,pPropertyBuy->wPropertyIndex,pIServerUserItem,pITargerUserItem)==false)
		return RESULT_FAIL;

	//��������
	DBR_GR_PropertyRequest PropertyRequest;
	ZeroMemory(&PropertyRequest,sizeof(PropertyRequest));

	//���ҵ���
	tagPropertyInfo * pPropertyInfo=m_GamePropertyManager.SearchPropertyItem(pPropertyBuy->wPropertyIndex);
	if (pPropertyInfo==NULL)
	{
		SendPropertyFailure(pIServerUserItem,TEXT("������ĵ��߲����ڻ���ά���У��������Ա��ϵ��"),0L,pPropertyBuy->cbRequestArea);
		return RESULT_FAIL;
	}

	//���ѷ�ʽ
	if (pPropertyBuy->cbConsumeScore==TRUE)
	{
		//�����ж�
		ASSERT((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0);
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return RESULT_FAIL;		

		//��������
		PropertyRequest.cbConsumeScore=TRUE;
		PropertyRequest.lFrozenedScore=pPropertyInfo->lPropertyGold*pPropertyBuy->wItemCount;

		//��Ա�ۿ�
		if (pIServerUserItem->GetMemberOrder()>0)
		{
			PropertyRequest.lFrozenedScore = PropertyRequest.lFrozenedScore*pPropertyInfo->wDiscount/100L;
		}

		//��������
		if (pIServerUserItem->FrozenedUserScore(PropertyRequest.lFrozenedScore)==false)
		{
			SendPropertyFailure(pIServerUserItem,TEXT("������Ϸ�����㣬���߹���ʧ�ܣ�"),0L,pPropertyBuy->cbRequestArea);
			return RESULT_FAIL;
		}
	}
	else
	{
		//���п۷�
		PropertyRequest.lFrozenedScore=0L;
		PropertyRequest.cbConsumeScore=FALSE;

		//��������
		SCORE lInsure = pIServerUserItem->GetUserInsure();
		SCORE lConsumeScore = pPropertyInfo->lPropertyGold*pPropertyBuy->wItemCount;

		//��Ա�ۿ�
		if (pIServerUserItem->GetMemberOrder()>0)
		{
			lConsumeScore = lConsumeScore*pPropertyInfo->wDiscount/100L;
		}

		//����У��
		if(lInsure < lConsumeScore)
		{
			SendPropertyFailure(pIServerUserItem,TEXT("���ı��չ����㣬������ٴι���"),0L,pPropertyBuy->cbRequestArea);
			return RESULT_FAIL;
		}		
	}

	//������Ϣ
	PropertyRequest.cbRequestArea=pPropertyBuy->cbRequestArea;
	PropertyRequest.wItemCount=pPropertyBuy->wItemCount;
	PropertyRequest.wPropertyIndex=pPropertyBuy->wPropertyIndex;
	PropertyRequest.dwSourceUserID=pIServerUserItem->GetUserID();
	PropertyRequest.dwTargetUserID=pITargerUserItem->GetUserID();

	//����Ȩ��
	if(PropertyRequest.wPropertyIndex==PROPERTY_ID_VIPROOM_CARD)
		PropertyRequest.dwUserRight |= UR_GAME_KICK_OUT_USER|UR_GAME_ENTER_VIP_ROOM;
	else if(PropertyRequest.wPropertyIndex>=PROPERTY_ID_BLUERING_CARD && PropertyRequest.wPropertyIndex <=PROPERTY_ID_REDRING_CARD)
		PropertyRequest.dwUserRight |= UR_GAME_KICK_OUT_USER;
	else
		PropertyRequest.dwUserRight=0;

	//ϵͳ��Ϣ
	PropertyRequest.wTableID=INVALID_TABLE;
	PropertyRequest.dwInoutIndex=pIServerUserItem->GetInoutIndex();
	PropertyRequest.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(PropertyRequest.szMachineID,pIServerUserItem->GetMachineID(),CountArray(PropertyRequest.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_PROPERTY_REQUEST,dwSocketID,&PropertyRequest,sizeof(PropertyRequest));

	return RESULT_SUCCESS;
}

//ʹ�õ���
bool CAttemperEngineSink::OnTCPNetwordSubSendTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_SendTrumpet));
	if (wDataSize!=sizeof(CMD_GR_C_SendTrumpet)) return false;

	//��������
	CMD_GR_C_SendTrumpet * pSendTrumpet=(CMD_GR_C_SendTrumpet *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	CMD_GR_C_PropertyBuy  PropertyBuy;
	PropertyBuy.cbRequestArea=pSendTrumpet->cbRequestArea;
	PropertyBuy.dwTargetUserID=pIServerUserItem->GetUserID();
	PropertyBuy.wPropertyIndex=pSendTrumpet->wPropertyIndex;
	PropertyBuy.cbConsumeScore=FALSE;
	PropertyBuy.wItemCount=1;
    
	//����
	int cbResult = OnPropertyBuy((void *)&PropertyBuy,sizeof(PropertyBuy),dwSocketID);

	//����ж�
	if(cbResult == RESULT_ERROR) return false;
	if(cbResult != RESULT_SUCCESS) return true;

	//��ȡ����
	tagUserProperty * pUserProperty = pIServerUserItem->GetUserProperty();

	//��������
	BYTE cbIndex=(pSendTrumpet->wPropertyIndex==PROPERTY_ID_TRUMPET)?2:3;

	//����ṹ
	CMD_GR_S_SendTrumpet  SendTrumpet;
	SendTrumpet.dwSendUserID=pIServerUserItem->GetUserID();
	SendTrumpet.wPropertyIndex=pSendTrumpet->wPropertyIndex;
	SendTrumpet.TrumpetColor=pSendTrumpet->TrumpetColor;
	ZeroMemory(SendTrumpet.szTrumpetContent,sizeof(SendTrumpet.szTrumpetContent));
	CopyMemory(SendTrumpet.szSendNickName,pIServerUserItem->GetNickName(),sizeof(SendTrumpet.szSendNickName));

	//�ַ�����
	SensitiveWordFilter(pSendTrumpet->szTrumpetContent,SendTrumpet.szTrumpetContent,CountArray(SendTrumpet.szTrumpetContent));

	DWORD dwSendUserAreaID = pIServerUserItem->GetAreaID();
    //����ת��
	if(cbIndex==3)
	{
		//�㲥����
		WORD wUserIndex=0;
//		CMD_CS_S_SendTrumpet * pSendTrumpet = (CMD_CS_S_SendTrumpet *)&SendTrumpet;
		CMD_CS_S_SendTrumpet CSSendTrumpet;
		CSSendTrumpet.wPropertyIndex = SendTrumpet.wPropertyIndex;							//�������� 
		CSSendTrumpet.dwSendUserID = SendTrumpet.dwSendUserID;								//�û� I D
		CSSendTrumpet.dwSendUserAreaID = dwSendUserAreaID;						//�����û�������ID
		CSSendTrumpet.TrumpetColor = SendTrumpet.TrumpetColor;								//������ɫ
		_tcsncpy(CSSendTrumpet.szSendNickName, SendTrumpet.szSendNickName, sizeof(CSSendTrumpet.szSendNickName));
		_tcsncpy(CSSendTrumpet.szTrumpetContent, SendTrumpet.szTrumpetContent, sizeof(CSSendTrumpet.szTrumpetContent));
		if(m_pITCPSocketService)
		{
			m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE, SUB_CS_C_PROPERTY_TRUMPET, &CSSendTrumpet, sizeof(CSSendTrumpet));
		}
	}

	//��Ϸת��
	if (cbIndex == 2)
	{
		//��������
		WORD wEnumIndex = 0;
		while (wEnumIndex < m_ServerUserManager.GetUserItemCount())
		{
			IServerUserItem* pIUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
			DWORD dwAreaID = pIUserItem->GetAreaID();
			if (pIUserItem == NULL)
			{
				continue;
			}
			if (dwSendUserAreaID == 0 || dwAreaID == dwSendUserAreaID)
			{
				WORD wBindIndex = pIUserItem->GetBindIndex();
				tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);
				if (pBindParameter)
				{
					m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_USER, SUB_GR_PROPERTY_TRUMPET, &SendTrumpet, sizeof(SendTrumpet));
				}
			}
		}
	}

	return true;
}

bool CAttemperEngineSink::OnTCPAndroidSendTrumpet()
{
	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf(szIniFile,CountArray(szIniFile),TEXT("%s\\AndroidMessageConfig.ini"),szWorkDir);

	//��ȡ����
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);
	if(m_nAndroidUserMessageCount!=0)
		m_nAndroidUserSendedCount=rand()%m_nAndroidUserMessageCount;
	TCHAR szMessage[255];
	TCHAR szName[32];
	CString str;
	str.Format(TEXT("Msg%d"),m_nAndroidUserSendedCount);
	IniData.ReadString(TEXT("Message"),str,NULL,szMessage,CountArray(szMessage));
	str.Format(TEXT("Name%d"),m_nAndroidUserSendedCount);
	IniData.ReadString(TEXT("NickName"),str,NULL,szName,CountArray(szName));
	//����ṹ
	CMD_GR_S_SendTrumpet  SendTrumpet;
	SendTrumpet.dwSendUserID=201;
	SendTrumpet.wPropertyIndex=19;
		if((rand()%3)==1)
			SendTrumpet.TrumpetColor=RGB(255,0,0);
		else if((rand()%3)==2)
			SendTrumpet.TrumpetColor=RGB(51,169,57);
		else if((rand()%3)==0)
			SendTrumpet.TrumpetColor=RGB(168,117,132);
	ZeroMemory(SendTrumpet.szTrumpetContent,sizeof(SendTrumpet.szTrumpetContent));
	CopyMemory(SendTrumpet.szSendNickName,szName,sizeof(SendTrumpet.szSendNickName));
	//�ַ�����
	SensitiveWordFilter(szMessage,SendTrumpet.szTrumpetContent,CountArray(szMessage));

	m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_PROPERTY_TRUMPET,&SendTrumpet,sizeof(SendTrumpet),BG_COMPUTER);
	//m_nAndroidUserSendedCount++;
	return true;

}

//�����û�
bool CAttemperEngineSink::OnTCPNetworkSubUserInviteReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_UserInviteReq));
	if (wDataSize!=sizeof(CMD_GR_UserInviteReq)) return false;

	//��Ϣ����
	CMD_GR_UserInviteReq * pUserInviteReq=(CMD_GR_UserInviteReq *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ч��״̬
	if (pUserInviteReq->wTableID==INVALID_TABLE) return true;
	if (pIServerUserItem->GetTableID()!=pUserInviteReq->wTableID) return true;
	if (pIServerUserItem->GetUserStatus()==US_PLAYING) return true;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE) return true;

	//Ŀ���û�
	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pUserInviteReq->dwUserID);
	if (pITargetUserItem==NULL) return true;
	if (pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//������Ϣ
	CMD_GR_UserInvite UserInvite;
	memset(&UserInvite,0,sizeof(UserInvite));
	UserInvite.wTableID=pUserInviteReq->wTableID;
	UserInvite.dwUserID=pIServerUserItem->GetUserID();
	SendData(pITargetUserItem,MDM_GR_USER,SUB_GR_USER_INVITE,&UserInvite,sizeof(UserInvite));

	return true;
}

//�ܾ�����
bool CAttemperEngineSink::OnTCPNetworkSubUserRepulseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_UserRepulseSit));
	if (wDataSize!=sizeof(CMD_GR_UserRepulseSit)) return false;

	//��Ϣ����
	CMD_GR_UserRepulseSit * pUserRepulseSit=(CMD_GR_UserRepulseSit *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CTableFrame * pTableFrame=m_TableFrameArray[pUserRepulseSit->wTableID];
	if (pTableFrame->IsGameStarted()==true) return true;

	//��ȡ�û�
	IServerUserItem * pRepulseIServerUserItem = pTableFrame->GetTableUserItem(pUserRepulseSit->wChairID);
	if (pRepulseIServerUserItem==NULL) return true;
	if(pRepulseIServerUserItem->GetUserID() != pUserRepulseSit->dwRepulseUserID)return true;

	//������Ϣ
	TCHAR szDescribe[256]=TEXT("");
	lstrcpyn(szDescribe,TEXT("��������������˲�����ͬ����Ϸ��"),CountArray(szDescribe));
	SendRoomMessage(pRepulseIServerUserItem,szDescribe,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

	//�������
	LOGI("CAttemperEngineSink::OnTCPNetworkSubUserRepulseSit PerformStandUpAction, NickName="<<pRepulseIServerUserItem->GetNickName());
	pTableFrame->PerformStandUpAction(pRepulseIServerUserItem);

	return true;
}

//�߳�����
bool CAttemperEngineSink::OnTCPNetworkSubMemberKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//��������
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//Ŀ���û�
	IServerUserItem * pITargetUserItem = m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMemberOrder()>pITargetUserItem->GetMemberOrder()));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMemberOrder()<=pITargetUserItem->GetMemberOrder())) return false;

	//Ȩ���ж�
	ASSERT(CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==true);
	if (CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==false) return false;

	//������Ϸ
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
	{
		//������Ϣ
		SendRoomMessage(pIServerUserItem,TEXT("�ܱ�Ǹ��������Ϸ�������ˣ�"),SMT_EJECT);
		return true;
	}

	//�û�״̬
	if(pITargetUserItem->GetUserStatus()==US_PLAYING)
	{
		//��������
		TCHAR szMessage[256]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("������� [ %s ] ������Ϸ��,�����ܽ����߳���Ϸ��"),pITargetUserItem->GetNickName());

		//������Ϣ
		SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);
		return true;
	}

	//�����ж�
	if((pITargetUserItem->GetUserProperty()->wPropertyUseMark&PT_USE_MARK_GUARDKICK_CARD)!=0)
	{
		//��������
		DWORD dwCurrentTime=(DWORD)time(NULL);
		tagUserProperty * pUserProperty = pITargetUserItem->GetUserProperty();

		//ʱЧ�ж�
		DWORD dwValidTime=pUserProperty->PropertyInfo[2].wPropertyCount*pUserProperty->PropertyInfo[2].dwValidNum;
		if(pUserProperty->PropertyInfo[2].dwEffectTime+dwValidTime>dwCurrentTime)
		{
			//��������
			TCHAR szMessage[256]=TEXT("");
			_sntprintf(szMessage,CountArray(szMessage),TEXT("������� [ %s ] ����ʹ�÷��߿�,���޷������߳���Ϸ��"),pITargetUserItem->GetNickName());

			//������Ϣ
			SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);

			return true; 
		}
		else
			pUserProperty->wPropertyUseMark &= ~PT_USE_MARK_GUARDKICK_CARD;
	}

	//��������
	WORD wTargerTableID = pITargetUserItem->GetTableID();
	if(wTargerTableID != INVALID_TABLE)
	{
		//�������
		TCHAR szMessage[64]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("���ѱ�%s�������ӣ�"),pIServerUserItem->GetNickName());

		//������Ϣ
		SendGameMessage(pITargetUserItem,szMessage,SMT_CHAT|SMT_CLOSE_GAME);

		CTableFrame * pTableFrame=m_TableFrameArray[wTargerTableID];
		LOGI("CAttemperEngineSink::OnTCPNetworkSubMemberKickUser PerformStandUpAction, NickName="<<pITargetUserItem->GetNickName());
		if (pTableFrame->PerformStandUpAction(pITargetUserItem)==false) return true;
	}

	return true;
}

//�����û���Ϣ
bool CAttemperEngineSink::OnTCPNetworkSubUserInfoReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserInfoReq));
	if (wDataSize!=sizeof(CMD_GR_UserInfoReq)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_UserInfoReq * pUserInfoReq = (CMD_GR_UserInfoReq *)pData;
	WORD wCurDeskPos = pIServerUserItem->GetMobileUserDeskPos();
	WORD wMaxDeskPos = m_pGameServiceOption->wTableCount-pIServerUserItem->GetMobileUserDeskCount();

	//����Ч��
	if(pUserInfoReq->wTablePos > wMaxDeskPos) pUserInfoReq->wTablePos = wMaxDeskPos;

	//������Ϣ
	pIServerUserItem->SetMobileUserDeskPos(pUserInfoReq->wTablePos);
	
	//������Ϣ
	SendViewTableUserInfoPacketToMobileUser(pIServerUserItem,pUserInfoReq->dwUserIDReq);

	return true;
}

//�������λ��
bool CAttemperEngineSink::OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�û�״̬
	if (pIServerUserItem->GetUserStatus() == US_TRUSTEE)
	{
		SendRequestFailure(pIServerUserItem,TEXT("�������йܴ��򣬲�����Ч����ȴ���ǰ��Ϸ������"),0);
		return true;
	}
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		//ʧ��
		m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	//��������
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//����ͬ��
		if(i == pIServerUserItem->GetTableID())continue;

		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[i];
		if (pTableFrame->IsGameStarted()==true) continue;
		if(pTableFrame->IsTableLocked()) continue;

		//��Ч����
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//�뿪����
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			LOGI("CAttemperEngineSink::OnTCPNetworkSubUserChairReq PerformStandUpAction, NickName="<<pIServerUserItem->GetNickName());
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}

		//�������
		WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
		WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();

		//������Ϣ
		if((i < wTagerDeskPos) ||(i > (wTagerDeskPos+wTagerDeskCount-1)))
		{
			pIServerUserItem->SetMobileUserDeskPos(i/wTagerDeskCount);
		}

		//�û�����
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//����ͬ��
	if(pIServerUserItem->GetTableID() != INVALID_TABLE)
	{
		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
		if (pTableFrame->IsGameStarted()==false && pTableFrame->IsTableLocked()==false)
		{
			//��Ч����
			WORD wChairID=pTableFrame->GetRandNullChairID();
			if (wChairID!=INVALID_CHAIR)
			{
				//�뿪����
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
					LOGI("CAttemperEngineSink::OnTCPNetworkSubUserChairReq PerformStandUpAction, (pIServerUserItem->GetTableID() != INVALID_TABLE), NickName="<<pIServerUserItem->GetNickName());
					if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
				}

				//�û�����
				pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
				return true;
			}
		}
	}

	//ʧ��
	m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("û�ҵ��ɽ������Ϸ����"),REQUEST_FAILURE_NORMAL);
	return true;
}

//���������û���Ϣ
bool CAttemperEngineSink::OnTCPNetworkSubChairUserInfoReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_ChairUserInfoReq));
	if (wDataSize!=sizeof(CMD_GR_ChairUserInfoReq)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_ChairUserInfoReq * pUserInfoReq = (CMD_GR_ChairUserInfoReq *)pData;
	if(pUserInfoReq->wTableID == INVALID_TABLE) return true;
	if(pUserInfoReq->wTableID >= m_pGameServiceOption->wTableCount)return true;

	//������Ϣ
	WORD wChairCout = m_TableFrameArray[pUserInfoReq->wTableID]->GetChairCount();
	for(WORD wIndex = 0; wIndex < wChairCout; wIndex++)
	{
		//��ȡ�û�
		if(pUserInfoReq->wChairID != INVALID_CHAIR && wIndex != pUserInfoReq->wChairID)continue;
		IServerUserItem * pTagerIServerUserItem=m_TableFrameArray[pUserInfoReq->wTableID]->GetTableUserItem(wIndex);
		if(pTagerIServerUserItem==NULL)continue;

		//��������
		BYTE cbBuffer[SOCKET_TCP_PACKET]={0};
		tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)cbBuffer;
		CSendPacketHelper SendPacket(cbBuffer+sizeof(tagMobileUserInfoHead),sizeof(cbBuffer)-sizeof(tagMobileUserInfoHead));
		tagUserInfo *pUserInfo = pTagerIServerUserItem->GetUserInfo();

		//�û�����
		pUserInfoHead->wFaceID=pUserInfo->wFaceID;
		pUserInfoHead->dwGameID=pUserInfo->dwGameID;
		pUserInfoHead->dwUserID=pUserInfo->dwUserID;
		pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;

		//�û�����
		pUserInfoHead->cbGender=pUserInfo->cbGender;
		pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;

		//�û�״̬
		pUserInfoHead->wTableID=pUserInfo->wTableID;
		pUserInfoHead->wChairID=pUserInfo->wChairID;
		pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

		//�û�����
		pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
		pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
		pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
		pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
		pUserInfoHead->dwExperience=pUserInfo->dwExperience;

		//�û���Ϸ��
		pUserInfoHead->lScore=pUserInfo->lScore;
		pUserInfoHead->lScore+=pIServerUserItem->GetTrusteeScore();
		pUserInfoHead->lScore+=pIServerUserItem->GetFrozenedScore();

		//������Ϣ
		SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);

		//������Ϣ
		WORD wHeadSize=sizeof(tagMobileUserInfoHead);
		SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}

	return true;
}

//��ѯ����
bool CAttemperEngineSink::OnTCPNetworkSubQueryInsureInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_QueryInsureInfoRequest));
	if (wDataSize!=sizeof(CMD_GR_C_QueryInsureInfoRequest)) return false;

	//�����ж�
	ASSERT((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)||(m_pGameServiceOption->wServerType&GAME_GENRE_MATCH));
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0&&(m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_C_QueryInsureInfoRequest * pQueryInsureInfoRequest = (CMD_GR_C_QueryInsureInfoRequest *)pData;

	//��������
	DBR_GR_QueryInsureInfo QueryInsureInfo;
	ZeroMemory(&QueryInsureInfo,sizeof(QueryInsureInfo));

	//��������
	QueryInsureInfo.cbActivityGame=pQueryInsureInfoRequest->cbActivityGame;
	QueryInsureInfo.dwUserID=pIServerUserItem->GetUserID();
	QueryInsureInfo.dwClientAddr=pIServerUserItem->GetClientAddr();

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(QueryInsureInfo.dwUserID,DBR_GR_QUERY_INSURE_INFO,dwSocketID,&QueryInsureInfo,sizeof(QueryInsureInfo));

	return true;
}
bool CAttemperEngineSink::OnTCPNetworkSubQueryTransRecord(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
    //Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_QueryInsureInfoRequest));
	if (wDataSize!=sizeof(CMD_GR_C_QueryInsureInfoRequest)) return false;

	//�����ж�
	ASSERT((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_C_QueryInsureInfoRequest * pQueryInsureInfoRequest = (CMD_GR_C_QueryInsureInfoRequest *)pData;

	//��������
	DBR_GR_QueryInsureInfo QueryInsureInfo;
	ZeroMemory(&QueryInsureInfo,sizeof(QueryInsureInfo));

	//��������
	QueryInsureInfo.cbActivityGame=pQueryInsureInfoRequest->cbActivityGame;
	QueryInsureInfo.dwUserID=pIServerUserItem->GetUserID();
	QueryInsureInfo.dwClientAddr=pIServerUserItem->GetClientAddr();

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(QueryInsureInfo.dwUserID,DBR_GR_QUERY_TRANSRECORD,dwSocketID,&QueryInsureInfo,sizeof(QueryInsureInfo));
    return true;
}
//�������
bool CAttemperEngineSink::OnTCPNetworkSubSaveScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_SaveScoreRequest));
	if (wDataSize!=sizeof(CMD_GR_C_SaveScoreRequest)) return false;

	//�����ж�
	ASSERT((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)||(m_pGameServiceOption->wServerType&GAME_GENRE_MATCH));
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0&&(m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0) return false;

	//��������
	CMD_GR_C_SaveScoreRequest * pSaveScoreRequest=(CMD_GR_C_SaveScoreRequest *)pData;

	//Ч�����
	ASSERT(pSaveScoreRequest->lSaveScore>0L);
	if (pSaveScoreRequest->lSaveScore<=0L) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�����ж�
	if(pSaveScoreRequest->cbActivityGame == FALSE && CServerRule::IsForfendSaveInRoom(m_pGameServiceOption->dwServerRule))
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ������������ʧ�ܣ�"),0L,pSaveScoreRequest->cbActivityGame);
		return true;
	}

	if (pIServerUserItem->GetUserStatus() == US_TRUSTEE)
	{
		SendInsureFailure(pIServerUserItem,TEXT("�����������йܴ���״̬�����ܽ��д��������ʧ�ܣ�"),0L,pSaveScoreRequest->cbActivityGame);
		return true;
	}

	// ��������ϣ��������Ǯ
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE && CServerRule::IsForfendSaveInGame(m_pGameServiceOption->dwServerRule) )
	{
		SendInsureFailure(pIServerUserItem,TEXT("������������Ϸ�У����ܽ��д��������ʧ�ܣ�"),0L,pSaveScoreRequest->cbActivityGame);
		return true;
	}

	//�����ж�
	if(pSaveScoreRequest->cbActivityGame == TRUE && CServerRule::IsForfendSaveInGame(m_pGameServiceOption->dwServerRule))
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ��Ϸ���������ʧ�ܣ�"),0L,pSaveScoreRequest->cbActivityGame);
		return true;
	}

	//��������
	SCORE lConsumeQuota=0L;
	SCORE lUserWholeScore=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();

	//��ȡ�޶�
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		WORD wTableID=pIServerUserItem->GetTableID();
		lConsumeQuota=m_TableFrameArray[wTableID]->QueryConsumeQuota(pIServerUserItem);
	}
	else
	{
		lConsumeQuota=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();
	}

	//�޶��ж�
	if (pSaveScoreRequest->lSaveScore>lConsumeQuota)
	{
		if (lConsumeQuota<lUserWholeScore)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("������������Ϸ�У���Ϸ�ҿɴ�����Ϊ %I64d���������ʧ�ܣ�"),lConsumeQuota);

			//��������
			SendInsureFailure(pIServerUserItem,szDescribe,0L,pSaveScoreRequest->cbActivityGame);
		}
		else
		{
			//��������
			SendInsureFailure(pIServerUserItem,TEXT("������Ϸ�����㣬�������ʧ�ܣ�"),0L,pSaveScoreRequest->cbActivityGame);
		}

		return true;
	}

	//��������
	if (pIServerUserItem->FrozenedUserScore(pSaveScoreRequest->lSaveScore)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	DBR_GR_UserSaveScore UserSaveScore;
	ZeroMemory(&UserSaveScore,sizeof(UserSaveScore));

	//��������
	UserSaveScore.cbActivityGame=pSaveScoreRequest->cbActivityGame;
	UserSaveScore.dwUserID=pIServerUserItem->GetUserID();
	UserSaveScore.lSaveScore=pSaveScoreRequest->lSaveScore;
	UserSaveScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(UserSaveScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserSaveScore.szMachineID));

//	LOGI(_T("CAttemperEngineSink::OnTCPNetworkSubSaveScoreRequest"));
	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_SAVE_SCORE,dwSocketID,&UserSaveScore,sizeof(UserSaveScore));

	return true;
}

//ȡ������
bool CAttemperEngineSink::OnTCPNetworkSubTakeScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_TakeScoreRequest));
	if (wDataSize!=sizeof(CMD_GR_C_TakeScoreRequest)) return false;

	//�����ж�
	ASSERT((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)||(m_pGameServiceOption->wServerType&GAME_GENRE_MATCH));
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0&&(m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0) return false;

	//��������
	CMD_GR_C_TakeScoreRequest * pTakeScoreRequest=(CMD_GR_C_TakeScoreRequest *)pData;
	pTakeScoreRequest->szInsurePass[CountArray(pTakeScoreRequest->szInsurePass)-1]=0;

	//Ч�����
	ASSERT(pTakeScoreRequest->lTakeScore>0L);
	if (pTakeScoreRequest->lTakeScore<=0L) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�����ж�
	if(pTakeScoreRequest->cbActivityGame == FALSE && CServerRule::IsForfendTakeInRoom(m_pGameServiceOption->dwServerRule))
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ����ȡ�ȡ������ʧ�ܣ�"),0L,pTakeScoreRequest->cbActivityGame);
		return true;
	}

	if (pIServerUserItem->GetUserStatus() == US_TRUSTEE)
	{
		SendInsureFailure(pIServerUserItem,TEXT("�����������йܴ���״̬�����ܽ���ȡ�ȡ������ʧ�ܣ�"), 0L, pTakeScoreRequest->cbActivityGame);
		return true;
	}

	// ��������ϣ��������Ǯ
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE && CServerRule::IsForfendSaveInGame(m_pGameServiceOption->dwServerRule) )
	{
		SendInsureFailure(pIServerUserItem,TEXT("������������Ϸ�У����ܽ���ȡ�ȡ������ʧ�ܣ�"),0L,pTakeScoreRequest->cbActivityGame);
		return true;
	}

	//�����ж�
	if(pTakeScoreRequest->cbActivityGame == TRUE && CServerRule::IsForfendTakeInGame(m_pGameServiceOption->dwServerRule))
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ��Ϸȡ�ȡ������ʧ�ܣ�"),0L,pTakeScoreRequest->cbActivityGame);
		return true;
	}

	//��������
	DBR_GR_UserTakeScore UserTakeScore;
	ZeroMemory(&UserTakeScore,sizeof(UserTakeScore));

	//��������
	UserTakeScore.cbActivityGame=pTakeScoreRequest->cbActivityGame;
	UserTakeScore.dwUserID=pIServerUserItem->GetUserID();
	UserTakeScore.lTakeScore=pTakeScoreRequest->lTakeScore;
	UserTakeScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(UserTakeScore.szPassword,pTakeScoreRequest->szInsurePass,CountArray(UserTakeScore.szPassword));
	lstrcpyn(UserTakeScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserTakeScore.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_TAKE_SCORE,dwSocketID,&UserTakeScore,sizeof(UserTakeScore));

	return true;
}

//ת������
bool CAttemperEngineSink::OnTCPNetworkSubTransferScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GP_C_TransferScoreRequest));
	if (wDataSize!=sizeof(CMD_GP_C_TransferScoreRequest)) return false;

	//�����ж�
	ASSERT((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)||(m_pGameServiceOption->wServerType&GAME_GENRE_MATCH));
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0&&(m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0) return false;

	//��������
	CMD_GP_C_TransferScoreRequest * pTransferScoreRequest=(CMD_GP_C_TransferScoreRequest *)pData;
	pTransferScoreRequest->szNickName[CountArray(pTransferScoreRequest->szNickName)-1]=0;
	pTransferScoreRequest->szInsurePass[CountArray(pTransferScoreRequest->szInsurePass)-1]=0;

	//Ч�����
	ASSERT(pTransferScoreRequest->lTransferScore>0L);
	if (pTransferScoreRequest->lTransferScore<=0L) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	// ��Ϸ���޷�ת�ˣ�������Ҳ�޷�ת��
	if ( pTransferScoreRequest->cbActivityGame==FALSE && CServerRule::IsForfendTransferInRoom(m_pGameServiceOption->dwServerRule) )
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ����ת�ˣ�ת�˲���ʧ�ܣ�"),0L,pTransferScoreRequest->cbActivityGame);
		return true;
	}

	if (pIServerUserItem->GetUserStatus() == US_TRUSTEE)
	{
		SendInsureFailure(pIServerUserItem,TEXT("�����������йܴ���״̬�����ܽ���ת�ˣ�ת�˲���ʧ�ܣ�"),0L,pTransferScoreRequest->cbActivityGame);
		return true;
	}

	// ��������ϣ��������Ǯ
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE && CServerRule::IsForfendSaveInGame(m_pGameServiceOption->dwServerRule) )
	{
		SendInsureFailure(pIServerUserItem,TEXT("������������Ϸ�У����ܽ���ת�ˣ�ת�˲���ʧ�ܣ�"),0L,pTransferScoreRequest->cbActivityGame);
		return true;
	}

	if ( pTransferScoreRequest->cbActivityGame==TRUE && CServerRule::IsForfendTransferInGame(m_pGameServiceOption->dwServerRule) )
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ��Ϸת�ˣ�ת�˲���ʧ�ܣ�"),0L,pTransferScoreRequest->cbActivityGame);
		return true;
	}

	//��������
	DBR_GR_UserTransferScore UserTransferScore;
	ZeroMemory(&UserTransferScore,sizeof(UserTransferScore));

	//��������
	UserTransferScore.cbActivityGame=pTransferScoreRequest->cbActivityGame;
	UserTransferScore.dwUserID=pIServerUserItem->GetUserID();
	UserTransferScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	UserTransferScore.cbByNickName=pTransferScoreRequest->cbByNickName;
	UserTransferScore.lTransferScore=pTransferScoreRequest->lTransferScore;
	lstrcpyn(UserTransferScore.szNickName,pTransferScoreRequest->szNickName,CountArray(UserTransferScore.szNickName));
	lstrcpyn(UserTransferScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserTransferScore.szMachineID));
	lstrcpyn(UserTransferScore.szPassword,pTransferScoreRequest->szInsurePass,CountArray(UserTransferScore.szPassword));

//	LOGI(_T("CAttemperEngineSink::OnTCPNetworkSubTransferScoreRequest"));
	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_TRANSFER_SCORE,dwSocketID,&UserTransferScore,sizeof(UserTransferScore));

	return true;
}

//��ѯ�û�����
bool CAttemperEngineSink::OnTCPNetworkSubQueryUserInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_QueryUserInfoRequest));
	if (wDataSize!=sizeof(CMD_GR_C_QueryUserInfoRequest)) return false;

	//�����ж�
	ASSERT((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��������
	CMD_GR_C_QueryUserInfoRequest * pQueryUserInfoRequest=(CMD_GR_C_QueryUserInfoRequest *)pData;
	pQueryUserInfoRequest->szNickName[CountArray(pQueryUserInfoRequest->szNickName)-1]=0;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	DBR_GR_QueryTransferUserInfo QueryTransferUserInfo;
	ZeroMemory(&QueryTransferUserInfo,sizeof(QueryTransferUserInfo));

	//��������
	QueryTransferUserInfo.cbActivityGame=pQueryUserInfoRequest->cbActivityGame;
	QueryTransferUserInfo.cbByNickName=pQueryUserInfoRequest->cbByNickName;
	lstrcpyn(QueryTransferUserInfo.szNickName,pQueryUserInfoRequest->szNickName,CountArray(QueryTransferUserInfo.szNickName));
	QueryTransferUserInfo.dwUserID=pIServerUserItem->GetUserID();

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_QUERY_TRANSFER_USER_INFO,dwSocketID,&QueryTransferUserInfo,sizeof(QueryTransferUserInfo));

	return true;
}

// ����GameID��ѯ�ǳ�
bool CAttemperEngineSink::OnTCPNetworkSubQueryNickNameByGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GP_QueryNickNameByGameID));
	if (wDataSize!=sizeof(CMD_GP_QueryNickNameByGameID)) return false;

	//������Ϣ
	CMD_GP_QueryNickNameByGameID* pQueryNickNameByGameID=(CMD_GP_QueryNickNameByGameID *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//��������
	DBR_GR_QueryNickNameByGameID QueryNickNameByGameID;
	ZeroMemory(&QueryNickNameByGameID,sizeof(QueryNickNameByGameID));


	//��������
	QueryNickNameByGameID.dwUserID=pIServerUserItem->GetUserID();
	QueryNickNameByGameID.dwGameID=pQueryNickNameByGameID->dwGameID;

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(), DBR_GR_QUERY_NICKNAME_BY_GAMEID,dwSocketID,&QueryNickNameByGameID,sizeof(QueryNickNameByGameID));
	return true;
}

// ��֤��������
bool CAttemperEngineSink::OnTCPNetworkSubVerifyInsurePassword(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_VerifyUserInsurePassword));
	if (wDataSize!=sizeof(CMD_GR_VerifyUserInsurePassword)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	
	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//������Ϣ
	CMD_GR_VerifyUserInsurePassword* pVerifyUserInsurePassword=(CMD_GR_VerifyUserInsurePassword *)pData;

	//��������
	DBR_GR_VerifyInsurePassword VerifyInsurePassword;
	ZeroMemory(&VerifyInsurePassword,sizeof(VerifyInsurePassword));

	//��������
	VerifyInsurePassword.dwUserID=pVerifyUserInsurePassword->dwUserID;
	lstrcpyn(VerifyInsurePassword.szInsurePassword,pVerifyUserInsurePassword->szInsurePassword,CountArray(VerifyInsurePassword.szInsurePassword));

	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(), DBR_GR_VERIFY_INSURE_PASSWORD,dwSocketID,&VerifyInsurePassword,sizeof(VerifyInsurePassword));
	return true;
}

// ������������
bool CAttemperEngineSink::OnTCPNetworkSubModifyInsurePassword(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GP_ModifyInsurePass));
	if (wDataSize!=sizeof(CMD_GP_ModifyInsurePass)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//������Ϣ
	CMD_GP_ModifyInsurePass * pModifyInsurePass=(CMD_GP_ModifyInsurePass *)pData;
	pModifyInsurePass->szDesPassword[CountArray(pModifyInsurePass->szDesPassword)-1]=0;
	pModifyInsurePass->szScrPassword[CountArray(pModifyInsurePass->szScrPassword)-1]=0;

	//��������
	DBR_GR_ModifyInsurePass ModifyInsurePass;
	ZeroMemory(&ModifyInsurePass,sizeof(ModifyInsurePass));

	//��������
	ModifyInsurePass.dwUserID=pModifyInsurePass->dwUserID;
	ModifyInsurePass.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(ModifyInsurePass.szDesPassword,pModifyInsurePass->szDesPassword,CountArray(ModifyInsurePass.szDesPassword));
	lstrcpyn(ModifyInsurePass.szScrPassword,pModifyInsurePass->szScrPassword,CountArray(ModifyInsurePass.szScrPassword));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(), DBR_GR_MODIFY_INSURE_PASSWORD,dwSocketID,&ModifyInsurePass,sizeof(ModifyInsurePass));

	return true;
}

//��ѯ����
bool CAttemperEngineSink::OnTCPNetworkSubQueryOption(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//��������
	CMD_GR_OptionCurrent OptionCurrent;
	ZeroMemory(&OptionCurrent,sizeof(OptionCurrent));

	//�ҽ�����
	OptionCurrent.ServerOptionInfo.wKindID=m_pGameServiceOption->wKindID;
	OptionCurrent.ServerOptionInfo.wNodeID=m_pGameServiceOption->wNodeID;
	OptionCurrent.ServerOptionInfo.wSortID=m_pGameServiceOption->wSortID;

	//��������
#ifndef _READ_TABLE_OPTION_
	OptionCurrent.ServerOptionInfo.wRevenueRatio=m_pGameServiceOption->wRevenueRatio;
	OptionCurrent.ServerOptionInfo.lServiceScore=m_pGameServiceOption->lServiceScore;
	OptionCurrent.ServerOptionInfo.lRestrictScore=m_pGameServiceOption->lRestrictScore;
	OptionCurrent.ServerOptionInfo.lMinTableScore=m_pGameServiceOption->lMinTableScore;
#else
	// anjay !!
	// GM������ʱʧЧ,��Ҫ�޸���ϢЭ��
#endif
	OptionCurrent.ServerOptionInfo.lMinEnterScore=m_pGameServiceOption->lMinEnterScore;
	OptionCurrent.ServerOptionInfo.lMaxEnterScore=m_pGameServiceOption->lMaxEnterScore;

	//��Ա����
	OptionCurrent.ServerOptionInfo.cbMinEnterMember=m_pGameServiceOption->cbMinEnterMember;
	OptionCurrent.ServerOptionInfo.cbMaxEnterMember=m_pGameServiceOption->cbMaxEnterMember;

	//��������
	OptionCurrent.ServerOptionInfo.dwServerRule=m_pGameServiceOption->dwServerRule;
	lstrcpyn(OptionCurrent.ServerOptionInfo.szServerName,m_pGameServiceOption->szServerName,CountArray(OptionCurrent.ServerOptionInfo.szServerName));

	//�������
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_CHAT;
	OptionCurrent.dwRuleMask|=SR_FORFEND_ROOM_CHAT;
	OptionCurrent.dwRuleMask|=SR_FORFEND_WISPER_CHAT;
	OptionCurrent.dwRuleMask|=SR_FORFEND_WISPER_ON_GAME;

	//�������
	OptionCurrent.dwRuleMask|=SR_FORFEND_ROOM_ENTER;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_ENTER;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_LOOKON;

	//���й���
	OptionCurrent.dwRuleMask|=SR_FORFEND_TAKE_IN_ROOM;
	OptionCurrent.dwRuleMask|=SR_FORFEND_TAKE_IN_GAME;
	OptionCurrent.dwRuleMask|=SR_FORFEND_SAVE_IN_ROOM;
	OptionCurrent.dwRuleMask|=SR_FORFEND_SAVE_IN_GAME;

	//��������
	//OptionCurrent.dwRuleMask|=SR_RECORD_GAME_TRACK;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_RULE;
	OptionCurrent.dwRuleMask|=SR_FORFEND_LOCK_TABLE;
	OptionCurrent.dwRuleMask|=SR_ALLOW_ANDROID_SIMULATE;

	//�������
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_DYNAMIC_JOIN;
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_ANDROID_ATTEND;
	//if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_OFFLINE_TRUSTEE;

	//ģʽ����
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0) OptionCurrent.dwRuleMask|=SR_RECORD_GAME_SCORE;
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0) OptionCurrent.dwRuleMask|=SR_IMMEDIATE_WRITE_SCORE;

	//��������
	SendData(pIServerUserItem,MDM_GR_MANAGE,SUB_GR_OPTION_CURRENT,&OptionCurrent,sizeof(OptionCurrent));

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubOptionServer(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_ServerOption));
	if (wDataSize!=sizeof(CMD_GR_ServerOption)) return false;

	//��������
	CMD_GR_ServerOption * pServerOption=(CMD_GR_ServerOption *)pData;
	tagServerOptionInfo * pServerOptionInfo=&pServerOption->ServerOptionInfo;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//��������
	bool bModifyServer=false;

	//�ҽӽڵ�
	if (m_pGameServiceOption->wNodeID!=pServerOptionInfo->wNodeID)
	{
		bModifyServer=true;
		m_pGameServiceOption->wNodeID=pServerOptionInfo->wNodeID;
	}

	//�ҽ�����
	if ((pServerOptionInfo->wKindID!=0)&&(m_pGameServiceOption->wKindID!=pServerOptionInfo->wKindID))
	{
		bModifyServer=true;
		m_pGameServiceOption->wKindID=pServerOptionInfo->wKindID;
	}
	
	//�ҽ�����
	if ((pServerOptionInfo->wSortID!=0)&&(m_pGameServiceOption->wSortID!=pServerOptionInfo->wSortID))
	{
		bModifyServer=true;
		m_pGameServiceOption->wSortID=pServerOptionInfo->wSortID;
	}

	//��������
	if ((pServerOptionInfo->szServerName[0]!=0)&&(lstrcmp(m_pGameServiceOption->szServerName,pServerOptionInfo->szServerName)!=0))
	{
		bModifyServer=true;
		lstrcpyn(m_pGameServiceOption->szServerName,pServerOptionInfo->szServerName,CountArray(m_pGameServiceOption->szServerName));
	}

#ifndef _READ_TABLE_OPTION_
	//˰������
	m_pGameServiceOption->wRevenueRatio=pServerOptionInfo->wRevenueRatio;
	m_pGameServiceOption->lServiceScore=pServerOptionInfo->lServiceScore;

	//��������
	m_pGameServiceOption->lRestrictScore=pServerOptionInfo->lRestrictScore;
	m_pGameServiceOption->lMinTableScore=pServerOptionInfo->lMinTableScore;
#else
	// anjay !!
	// GM������ʱʧЧ,��Ҫ�޸�Э��Ϳͻ���
#endif
	m_pGameServiceOption->lMinEnterScore=pServerOptionInfo->lMinEnterScore;
	m_pGameServiceOption->lMaxEnterScore=pServerOptionInfo->lMaxEnterScore;

	//��Ա����
	m_pGameServiceOption->cbMinEnterMember=pServerOptionInfo->cbMinEnterMember;
	m_pGameServiceOption->cbMaxEnterMember=pServerOptionInfo->cbMaxEnterMember;

	//�������
	CServerRule::SetForfendGameChat(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameChat(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendRoomChat(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendRoomChat(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendWisperChat(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendWisperChat(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendWisperOnGame(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendWisperOnGame(pServerOptionInfo->dwServerRule));

	//�������
	CServerRule::SetForfendRoomEnter(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendRoomEnter(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameEnter(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameEnter(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameLookon(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameLookon(pServerOptionInfo->dwServerRule));

	//���й���
	CServerRule::SetForfendTakeInRoom(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendTakeInRoom(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendTakeInGame(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendTakeInGame(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendSaveInRoom(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendSaveInRoom(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendSaveInGame(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendSaveInGame(pServerOptionInfo->dwServerRule));

	//��������
	CServerRule::SetRecordGameTrack(m_pGameServiceOption->dwServerRule,CServerRule::IsRecordGameTrack(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameRule(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameRule(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendLockTable(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendLockTable(pServerOptionInfo->dwServerRule));

	//��̬����
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE)
	{
		CServerRule::SetAllowDynamicJoin(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowDynamicJoin(pServerOptionInfo->dwServerRule));
	}

	//��������
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE)
	{
		CServerRule::SetAllowAndroidAttend(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowAndroidAttend(pServerOptionInfo->dwServerRule));
	}

	//�����й�
	if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE)
	{
		CServerRule::SetAllowOffLineTrustee(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowOffLineTrustee(pServerOptionInfo->dwServerRule));
	}

	//��¼��Ϸ��
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0)
	{
		CServerRule::SetRecordGameScore(m_pGameServiceOption->dwServerRule,CServerRule::IsRecordGameScore(pServerOptionInfo->dwServerRule));
	}

	//����д��
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0)
	{
		CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,CServerRule::IsImmediateWriteScore(pServerOptionInfo->dwServerRule));
	}

	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->RectifyServiceParameter();

	//�����޸�
	if (bModifyServer==true)
	{
		//��������
		CMD_CS_C_ServerModify ServerModify;
		ZeroMemory(&ServerModify,sizeof(ServerModify));

		//����˿�
		ServerModify.wServerPort=pServiceUnits->m_TCPNetworkEngine->GetCurrentPort();

		//������Ϣ
		ServerModify.wKindID=m_pGameServiceOption->wKindID;
		ServerModify.wNodeID=m_pGameServiceOption->wNodeID;
		ServerModify.wSortID=m_pGameServiceOption->wSortID;
		ServerModify.dwOnLineCount=m_ServerUserManager.GetUserItemCount();
		ServerModify.dwFullCount=m_pGameServiceOption->wMaxPlayer-RESERVE_USER_COUNT;
		lstrcpyn(ServerModify.szServerName,m_pGameServiceOption->szServerName,CountArray(ServerModify.szServerName));
		lstrcpyn(ServerModify.szServerAddr,m_pInitParameter->m_ServiceAddress.szAddress,CountArray(ServerModify.szServerAddr));

		//��������
		m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_C_SERVER_MODIFY,&ServerModify,sizeof(ServerModify));
	}

	//������Ϣ
	SendRoomMessage(pIServerUserItem,TEXT("��ǰ��Ϸ����������ġ�����ֵ��״̬���������޸ĳɹ�"),SMT_CHAT|SMT_EJECT);

	//�����Ϣ
	TCHAR szBuffer[128]=TEXT("");
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("Զ���޸ķ�������֪ͨ ����Ա %s [ %ld ]"),pIServerUserItem->GetNickName(),pIServerUserItem->GetUserID());

	//�����Ϣ
	CTraceService::TraceString(szBuffer,TraceLevel_Info);
	LOGI("CAttemperEngineSink::OnTCPNetworkSubOptionServer, "<<szBuffer)

	return true;
}

//�߳��û�
bool CAttemperEngineSink::OnTCPNetworkSubManagerKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//��������
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

	//Ŀ���û�
	IServerUserItem * pITargetUserItem = m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//�û�״̬
	if(pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//�߳���¼
	m_KickUserItemMap[pITargetUserItem->GetUserID()]=(DWORD)time(NULL);

	//��������
	WORD wTargerTableID = pITargetUserItem->GetTableID();
	if(wTargerTableID != INVALID_TABLE)
	{
		//������Ϣ
		SendGameMessage(pITargetUserItem,TEXT("���ѱ�����Ա�������ӣ�"),SMT_CHAT|SMT_CLOSE_GAME);

		CTableFrame * pTableFrame=m_TableFrameArray[wTargerTableID];
		LOGI("CAttemperEngineSink::OnTCPNetworkSubManagerKickUser PerformStandUpAction, NickName="<<pITargetUserItem->GetNickName());
		if (pTableFrame->PerformStandUpAction(pITargetUserItem)==false) return true;
	}

	//����֪ͨ
	LPCTSTR pszMessage=TEXT("���ѱ�����Ա�������Ϸ���䣡");
	SendRoomMessage(pITargetUserItem,pszMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

	pITargetUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubLimitUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_LimitUserChat));
	if (wDataSize!=sizeof(CMD_GR_LimitUserChat)) return false;

	//��Ϣ����
	CMD_GR_LimitUserChat * pLimitUserChat=(CMD_GR_LimitUserChat *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ȩ���ж�
	//ASSERT(CMasterRight::CanLimitUserChat(pIServerUserItem->GetMasterRight())==true);
	//if (CMasterRight::CanLimitUserChat(pIServerUserItem->GetMasterRight())==false) return false;

	//Ŀ���û�
	IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pLimitUserChat->dwTargetUserID);
	if (pITargerUserItem==NULL) return true;

	//��������
	DWORD dwAddRight = 0, dwRemoveRight = 0;

	//��������
	if (pLimitUserChat->cbLimitFlags==OSF_ROOM_CHAT)
	{
		if (CMasterRight::CanLimitRoomChat(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_ROOM_CHAT;
		else
			dwRemoveRight |= UR_CANNOT_ROOM_CHAT;
	}

	//��Ϸ����
	if (pLimitUserChat->cbLimitFlags==OSF_GAME_CHAT)
	{
		if (CMasterRight::CanLimitGameChat(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_GAME_CHAT;
		else
			dwRemoveRight |= UR_CANNOT_GAME_CHAT;
	}

	//����˽��
	if (pLimitUserChat->cbLimitFlags==OSF_ROOM_WISPER)
	{
		if (CMasterRight::CanLimitWisper(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_WISPER;
		else
			dwRemoveRight |= UR_CANNOT_WISPER;
	}

	//��������
	if(pLimitUserChat->cbLimitFlags==OSF_SEND_BUGLE)
	{
		if (CMasterRight::CanLimitUserChat(pIServerUserItem->GetMasterRight())==false) return false;

		if(pLimitUserChat->cbLimitValue == TRUE)
			dwAddRight |= UR_CANNOT_BUGLE;
		else
			dwRemoveRight |= UR_CANNOT_BUGLE;
	}

	if( dwAddRight != 0 || dwRemoveRight != 0 )
	{
		pITargerUserItem->ModifyUserRight(dwAddRight,dwRemoveRight);

		//����֪ͨ
		CMD_GR_ConfigUserRight cur = {0};
		cur.dwUserRight = pITargerUserItem->GetUserRight();

		SendData( pITargerUserItem,MDM_GR_CONFIG,SUB_GR_CONFIG_USER_RIGHT,&cur,sizeof(cur) );

		//������Ϣ
		SendRoomMessage(pIServerUserItem,TEXT("�û�����Ȩ�����óɹ���"),SMT_CHAT);
	}
	else return false;

	return true;
}

//�߳������û�
bool CAttemperEngineSink::OnTCPNetworkSubKickAllUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��Ϣ����
	CMD_GR_KickAllUser * pKillAllUser=(CMD_GR_KickAllUser *)pData;

	//Ч������
	ASSERT(wDataSize<=sizeof(CMD_GR_KickAllUser));
	if( wDataSize > sizeof(CMD_GR_KickAllUser) ) return false;
	ASSERT(wDataSize==CountStringBuffer(pKillAllUser->szKickMessage));
	if (wDataSize!=CountStringBuffer(pKillAllUser->szKickMessage)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

	//��ɢ������Ϸ
	for (size_t i=0;i<m_TableFrameArray.size();i++)
	{
		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[i];
		if ( !pTableFrame->IsGameStarted() ) continue;

		pTableFrame->DismissGame();
	}

	tagBindParameter *pBindParameter = m_pNormalParameter;
	for( INT i = 0; i < m_pGameServiceOption->wMaxPlayer; i++ )
	{
		//Ŀ¼�û�
		IServerUserItem * pITargerUserItem= pBindParameter->pIServerUserItem;
		if (pITargerUserItem==NULL || pITargerUserItem==pIServerUserItem ) 
		{
			pBindParameter++;
			continue;
		}

		//������Ϣ
		SendRoomMessage(pITargerUserItem,pKillAllUser->szKickMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_LINK|SMT_CLOSE_ROOM);

		pBindParameter++;
	} 

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnTCPNetworkSubSendMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==false) return false;

	//��������
	CMD_GR_SendMessage *pSysMessage = (CMD_GR_SendMessage *)pData;

	if(pSysMessage->cbLoop==TRUE)
	{
		//���ڱ���
		tagSystemMessage  *pSendMessage=new tagSystemMessage;
		ZeroMemory(pSendMessage, sizeof(tagSystemMessage));

		//������Ϣ
		pSendMessage->dwLastTime=(DWORD)time(NULL);
		pSendMessage->SystemMessage.cbMessageID=100;
		pSendMessage->SystemMessage.cbAllRoom=pSysMessage->cbAllRoom;
		if(pSysMessage->cbRoom==TRUE && pSysMessage->cbGame==TRUE)
			pSendMessage->SystemMessage.cbMessageType=3;
		else if(pSysMessage->cbRoom==TRUE)
			pSendMessage->SystemMessage.cbMessageType=2;
		else if(pSysMessage->cbGame==TRUE)
			pSendMessage->SystemMessage.cbMessageType=1;
		pSendMessage->SystemMessage.dwTimeRate=pSysMessage->dwTimeRate;
		pSendMessage->SystemMessage.tConcludeTime=pSysMessage->tConcludeTime;
		lstrcpyn(pSendMessage->SystemMessage.szSystemMessage, pSysMessage->szSystemMessage, CountArray(pSendMessage->SystemMessage.szSystemMessage));

		//��¼��Ϣ
		m_SystemMessageList.AddTail(pSendMessage);
	}


	//��Ϣ����
	return SendSystemMessage((CMD_GR_SendMessage *)pData, wDataSize);
}

//��ɢ��Ϸ
bool CAttemperEngineSink::OnTCPNetworkSubDismissGame(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_DismissGame));
	if (wDataSize!=sizeof(CMD_GR_DismissGame)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==false) return false;

	//��Ϣ����
	CMD_GR_DismissGame * pDismissGame=(CMD_GR_DismissGame *)pData;

	//Ч������
	if(pDismissGame->wDismissTableNum >= m_TableFrameArray.size()) return true;

	//��ɢ��Ϸ
	CTableFrame *pTableFrame=m_TableFrameArray[pDismissGame->wDismissTableNum];
	if(pTableFrame)
	{
		if(pTableFrame->IsGameStarted()) pTableFrame->DismissGame();
	}

	return true;
}

// �Խ�����
bool CAttemperEngineSink::OnTCPSocketUserCreateCustomTable(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_CS_C_UserCustomTable));
	if (wDataSize != sizeof(CMD_CS_C_UserCustomTable)) return false;

	CMD_CS_C_UserCustomTable* pUserCustomTable = (CMD_CS_C_UserCustomTable*)pData;

	if (pUserCustomTable == nullptr)
	{
		return true;
	}

	// �ж��Ƿ��п����ӿ��������Զ������
	CTableFrame* pTableFrame = nullptr;
	for (size_t i = 0; i < m_TableFrameArray.size(); i++)
	{
		pTableFrame = m_TableFrameArray.at(i);
		if (pTableFrame == nullptr || pTableFrame->IsLockedForCustom() == true || pTableFrame->IsTableStarted() == true)
		{
			continue;
		}

		LOGI("CAttemperEngineSink::OnTCPSocketUserCreateCustomTable, ��������["<<i+1<<"], ׼�������Զ������");
		// ��������׼������
		pTableFrame->SetLockForCustom(true);

		// ���ݿ����
		DBR_GR_UserCreateCustomTable DBUserCustomTable;
		DBUserCustomTable.dwUserID = pUserCustomTable->dwUserID;
		DBUserCustomTable.wCardCousumeCount = pUserCustomTable->wCardCousumeCount;
		DBUserCustomTable.wKindID = pUserCustomTable->wKindID;
		DBUserCustomTable.wServerID = m_pGameServiceOption->wServerID;
		DBUserCustomTable.wTableID = i;
		memcpy(DBUserCustomTable.cbCustomRule, pUserCustomTable->cbCustomRule, sizeof(DBUserCustomTable.cbCustomRule));

		m_pIDBCorrespondManager->PostDataBaseRequest(0L, DBR_GR_USER_CREATE_CUSTOMTABLE, pUserCustomTable->dwSocketID, &DBUserCustomTable, sizeof(DBR_GR_UserCreateCustomTable));

		return true;
	}

	// û�п�������,���ش�����Ϣ
	CMD_CS_C_UserCustomTableFailed UserCustomTableFailed;
	UserCustomTableFailed.lErrorCode = 1;
	lstrcpyn(UserCustomTableFailed.szDescribeString, TEXT("��������Ա,��ʱ�޷���������,���Ժ�����!������������ͷ���ϵ"), CountArray(UserCustomTableFailed.szDescribeString));
	UserCustomTableFailed.dwSocketID = pUserCustomTable->dwSocketID;
	m_pITCPSocketService->SendData(MDM_CS_REMOTE_SERVICE, SUB_CS_C_USER_CUSTOM_TABLE_FAILED, &UserCustomTableFailed, sizeof(UserCustomTableFailed));

	return true;
}

// �����Խ�����
bool CAttemperEngineSink::OnTCPNetworkUserJoinCustomTable(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize >= sizeof(CMD_MB_JoinCustomTable));
	if (wDataSize < sizeof(CMD_MB_JoinCustomTable)) return false;

	//������Ϣ
	CMD_MB_JoinCustomTable * pJoinCustomTable = (CMD_MB_JoinCustomTable *)pData;

	// ȥ���ݿ��ѯ������ǵ�һ�μ���,����֮ǰ�Ѿ��������,��Ҫ���ŵ�ԭ����λ��
	DBR_GR_UserJoinCustomTable UserJoinCustomTable;
	UserJoinCustomTable.dwUserID = pJoinCustomTable->dwUserID;
	lstrcpyn(UserJoinCustomTable.szSecret, pJoinCustomTable->szSecret, sizeof(UserJoinCustomTable.szSecret));

	m_pIDBCorrespondManager->PostDataBaseRequest(pJoinCustomTable->dwUserID, DBR_GR_USER_JOIN_CUSTOMTABLE, dwSocketID, &UserJoinCustomTable, sizeof(DBR_GR_UserJoinCustomTable));

	return true;
}

//�û���¼
VOID CAttemperEngineSink::OnEventUserLogon(IServerUserItem * pIServerUserItem, bool bAlreadyOnLine)
{
	//��ȡ����
	WORD wBindIndex=pIServerUserItem->GetBindIndex();
	bool bAndroidUser=pIServerUserItem->IsAndroidUser();
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//��¼����(PC���̺��ֻ���������һ��)
	//��������
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	CMD_GR_LogonSuccess LogonSuccess;
	CMD_GR_ConfigServer* pConfigServer = (CMD_GR_ConfigServer *)cbDataBuffer;
	ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));

	WORD wHeadSize = sizeof(CMD_GR_ConfigServer);
	CSendPacketHelper SendPacket(cbDataBuffer + wHeadSize, sizeof(cbDataBuffer) - wHeadSize);
	ZeroMemory(pConfigServer, sizeof(CMD_GR_ConfigServer));

	//��¼�ɹ�
	LogonSuccess.dwUserRight=pIServerUserItem->GetUserRight();
	LogonSuccess.dwMasterRight=pIServerUserItem->GetMasterRight();
	SendData(pBindParameter->dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_SUCCESS,&LogonSuccess,sizeof(LogonSuccess));

	//��������
	pConfigServer->wTableCount = m_pGameServiceOption->wTableCount;
	pConfigServer->wChairCount = m_pGameServiceAttrib->wChairCount;
	pConfigServer->wServerType = m_pGameServiceOption->wServerType;
	pConfigServer->dwServerRule = m_pGameServiceOption->dwServerRule;

	// ���ӷ�������
	if ( (m_pGameServiceOption->wServerType&GAME_GENRE_USERCUSTOM) == 0)
	{
		// �Խ����Ӳ��·�������Ϣ
		DTP_GR_TableInfo DTPTableInfo;
		ZeroMemory(&DTPTableInfo, sizeof(DTPTableInfo));
		for (int i = 0; i < m_pGameServiceOption->wTableCount; i++)
		{
			int j = CountArray(DTPTableInfo.szTableName);
			if (i >= CountArray(DTPTableInfo.szTableName))
			{
				continue;
			}
			_tcsnccpy(DTPTableInfo.szTableName[i], m_pGameServiceTableOption->TableOption[i].szTableName, LEN_TABLENAME);
			DTPTableInfo.cTableNormalPicID[i] = (unsigned char)m_pGameServiceTableOption->TableOption[i].nNormalPicID;
			DTPTableInfo.cTablePlayingPicID[i] = (unsigned char)m_pGameServiceTableOption->TableOption[i].nPlayingPicID;
		}
		SendPacket.AddPacket(&DTPTableInfo, sizeof(DTPTableInfo), DTP_GR_TABLE_INFO);
	}

	WORD wSendSize = SendPacket.GetDataSize() + sizeof(CMD_GR_ConfigServer);
	SendData(pBindParameter->dwSocketID, MDM_GR_CONFIG, SUB_GR_CONFIG_SERVER, &cbDataBuffer, wSendSize);

	//�б�����
	WORD wConfigColumnHead=sizeof(m_DataConfigColumn)-sizeof(m_DataConfigColumn.ColumnItem);
	WORD wConfigColumnInfo=m_DataConfigColumn.cbColumnCount*sizeof(m_DataConfigColumn.ColumnItem[0]);
	SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_COLUMN,&m_DataConfigColumn,wConfigColumnHead+wConfigColumnInfo);

	//��������
	WORD wConfigPropertyHead=sizeof(m_DataConfigProperty)-sizeof(m_DataConfigProperty.PropertyInfo);
	WORD wConfigPropertyInfo=m_DataConfigProperty.cbPropertyCount*sizeof(m_DataConfigProperty.PropertyInfo[0]);
	SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_PROPERTY,&m_DataConfigProperty,wConfigPropertyHead+wConfigPropertyInfo);

	//�������
	SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_FINISH,NULL,0);

	//�������
	SendUserInfoPacket(pIServerUserItem,pBindParameter->dwSocketID);

	//�����û�
	if (m_pGameServiceOption->wServerType&GAME_GENRE_USERCUSTOM)
	{
		// �����ʱ������λ��,�·�ͬ�������Ϣ
		if (pIServerUserItem->GetTableID() != INVALID_TABLE )
		{
			WORD wUserIndex = 0;
			IServerUserItem * pIServerUserItemSend = NULL;
			while (true)
			{
				pIServerUserItemSend = m_ServerUserManager.EnumUserItem(wUserIndex++);
				if (pIServerUserItemSend == nullptr)
				{
					break;
				}
				if (pIServerUserItemSend == pIServerUserItem)
				{
					continue;
				}
				if (pIServerUserItemSend->GetTableID() == pIServerUserItem->GetTableID())
				{
					SendUserInfoPacket(pIServerUserItemSend, pBindParameter->dwSocketID);
				}
			}
		}
	}
	else
	{
		// �Խ���������,���·�������������ҵ���Ϣ
		WORD wUserIndex = 0;
		IServerUserItem * pIServerUserItemSend = NULL;
		while (true)
		{
			pIServerUserItemSend = m_ServerUserManager.EnumUserItem(wUserIndex++);
			if (pIServerUserItemSend == NULL) break;
			if (pIServerUserItemSend == pIServerUserItem) continue;
			SendUserInfoPacket(pIServerUserItemSend, pBindParameter->dwSocketID);
		}
	}

	//����״̬
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_USERCUSTOM) == 0)
	{
		// �Խ���������,���·����������е�����״̬
		CMD_GR_TableInfo TableInfo;
		TableInfo.wTableCount = (WORD)m_TableFrameArray.size();
		ASSERT(TableInfo.wTableCount < CountArray(TableInfo.TableStatusArray));
		for (WORD i = 0; i < TableInfo.wTableCount; i++)
		{
			CTableFrame * pTableFrame = m_TableFrameArray[i];
			TableInfo.TableStatusArray[i].cbTableLock = pTableFrame->IsTableLocked() ? TRUE : FALSE;
			TableInfo.TableStatusArray[i].cbPlayStatus = pTableFrame->IsTableStarted() ? TRUE : FALSE;
		}

		//����״̬
		wHeadSize = sizeof(TableInfo) - sizeof(TableInfo.TableStatusArray);
		wSendSize = wHeadSize + TableInfo.wTableCount*sizeof(TableInfo.TableStatusArray[0]);
		SendData(pBindParameter->dwSocketID, MDM_GR_STATUS, SUB_GR_TABLE_INFO, &TableInfo, wSendSize);
	}

	//��һ�ν���,����֪ͨ,�ͻ�����Ӹ������Ϣ
	if (bAlreadyOnLine==false)
	{
		if (m_pGameServiceOption->wServerType&GAME_GENRE_USERCUSTOM)
		{
			// �Լ���Ϣ���͸�ͬ���������
			CTableFrame* pTableFrame = nullptr;
			if (pIServerUserItem->GetTableID() < m_TableFrameArray.size())
			{
				pTableFrame = m_TableFrameArray.at(pIServerUserItem->GetTableID());
			}

			if (pTableFrame)
			{
				// ����ͬ����Ϣ
				for (WORD j = 0; j < pTableFrame->GetChairCount(); j++)
				{
					IServerUserItem* pTableUserItem = pTableFrame->GetTableUserItem(j);
					if (pTableUserItem == nullptr || pTableUserItem == pIServerUserItem)
					{
						continue;
					}

					// �Լ�����Ϣ���͸�ͬ��
					tagBindParameter * pBindParameter = GetBindParameter(pTableUserItem->GetBindIndex());
					if (pBindParameter != nullptr)
					{
						SendUserInfoPacket(pIServerUserItem, pBindParameter->dwSocketID);
					}
				}
			}
		}
		else
		{
			// �Լ���Ϣ���͸������������
			SendUserInfoPacket(pIServerUserItem, INVALID_DWORD);
		}
	}

	//��¼���
	SendData(pBindParameter->dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FINISH,NULL,0);

	//��ӭ��Ϣ
	if (bAndroidUser==false)
	{
		//������ʾ
		TCHAR szMessage[128]=TEXT("");
		_sntprintf(szMessage,CountArray(szMessage),TEXT("��ӭ�����롰%s-%s����Ϸ���䣬ף����Ϸ��죡"),m_pGameServiceAttrib->szGameName,m_pGameServiceOption->szServerName);

		//������Ϣ
		SendRoomMessage(pIServerUserItem,szMessage,SMT_CHAT);
	}

	//��������
	if (bAndroidUser==false)
	{
		if (pBindParameter->cbClientKind==CLIENT_KIND_MOBILE)
		{
			m_pITCPNetworkEngine->AllowBatchSend(pBindParameter->dwSocketID,true,BG_MOBILE);
		}
		else
		{
			m_pITCPNetworkEngine->AllowBatchSend(pBindParameter->dwSocketID,true,BG_COMPUTER);
		}
	}

	if(m_pIGameMatchServiceManager!=NULL)
	{
		m_pIGameMatchServiceManager->SendMatchInfo(pIServerUserItem);
	}
	return;
}

//�û��뿪
VOID CAttemperEngineSink::OnEventUserLogout(IServerUserItem * pIServerUserItem, DWORD dwLeaveReason)
{
	//��������
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//��ȡ��Ϸ��
	pIServerUserItem->QueryRecordInfo(LeaveGameServer.RecordInfo);
	pIServerUserItem->DistillVariation(LeaveGameServer.VariationInfo);

    LeaveGameServer.VariationInfo.dwDrawCount=0;
    LeaveGameServer.VariationInfo.dwExperience=0;
    LeaveGameServer.VariationInfo.dwFleeCount=0;
    LeaveGameServer.VariationInfo.dwLostCount=0;
    LeaveGameServer.VariationInfo.dwWinCount=0;

	//�û���Ϣ
	LeaveGameServer.dwLeaveReason=dwLeaveReason;
	LeaveGameServer.dwUserID=pIServerUserItem->GetUserID();
	LeaveGameServer.dwInoutIndex=pIServerUserItem->GetInoutIndex();
	LeaveGameServer.dwOnLineTimeCount=(DWORD)(time(NULL))-pIServerUserItem->GetLogonTime();

	//������Ϣ
	LeaveGameServer.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(LeaveGameServer.szMachineID,pIServerUserItem->GetMachineID(),CountArray(LeaveGameServer.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer), TRUE);
	
	//�����û�
	if (m_bCollectUser==true)
	{
		//��������
		CMD_CS_C_UserLeave UserLeave;
		ZeroMemory(&UserLeave,sizeof(UserLeave));

		//���ñ���
		UserLeave.dwUserID=pIServerUserItem->GetUserID();

		//������Ϣ
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_LEAVE,&UserLeave,sizeof(UserLeave));
	}

	//֪�����������˳���Ϸ
	if(m_pIGameMatchServiceManager!=NULL)m_pIGameMatchServiceManager->OnUserQuitGame(pIServerUserItem, 0);

	//����ɾ��
	DeleteWaitDistribute(pIServerUserItem);

	//ɾ���û�
	m_ServerUserManager.DeleteUserItem(pIServerUserItem);

	return;
}

//������Ϸ��
bool CAttemperEngineSink::PerformUnlockScore(DWORD dwUserID, DWORD dwInoutIndex, DWORD dwLeaveReason)
{
	//��������
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//���ñ���
	LeaveGameServer.dwUserID=dwUserID;
	LeaveGameServer.dwInoutIndex=dwInoutIndex;
	LeaveGameServer.dwLeaveReason=dwLeaveReason;

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(dwUserID,DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer));

	return true;
}

//�汾���
bool CAttemperEngineSink::PerformCheckVersion(DWORD dwPlazaVersion, DWORD dwFrameVersion, DWORD dwClientVersion, DWORD dwSocketID, bool bMobileClient/* = false*/)
{
	//��������
	bool bMustUpdateClient=false;
	bool bAdviceUpdateClient=false;


	//��Ϸ�汾
	if (bMobileClient == false)
	{
		if (VERSION_EFFICACY == TRUE)
		{
			if (GetSubVer(dwClientVersion) < GetSubVer(m_pGameServiceAttrib->dwClientVersion)) bAdviceUpdateClient = true;
			if (GetMainVer(dwClientVersion) != GetMainVer(m_pGameServiceAttrib->dwClientVersion)) bMustUpdateClient = true;
			if (GetProductVer(dwClientVersion) != GetProductVer(m_pGameServiceAttrib->dwClientVersion)) bMustUpdateClient = true;
		}
		else
		{
			if (GetSubVer(dwClientVersion) < GetSubVer(m_pGameParameter->dwClientVersion)) bAdviceUpdateClient = true;
			if (GetMainVer(dwClientVersion) != GetMainVer(m_pGameParameter->dwClientVersion)) bMustUpdateClient = true;
			if (GetProductVer(dwClientVersion) != GetProductVer(m_pGameParameter->dwClientVersion)) bMustUpdateClient = true;
		}
	}
	else
	{
// 		if (VERSION_EFFICACY == TRUE)
// 		{
// 			if (GetSubVer(dwClientVersion) < GetSubVer(m_pGameServiceAttrib->dwAppClientVersion)) bAdviceUpdateClient = true;
// 			if (GetMainVer(dwClientVersion) != GetMainVer(m_pGameServiceAttrib->dwAppClientVersion)) bMustUpdateClient = true;
// 			if (GetProductVer(dwClientVersion) != GetProductVer(m_pGameServiceAttrib->dwAppClientVersion)) bMustUpdateClient = true;
// 		}
// 		else
// 		{
// 			if (GetSubVer(dwClientVersion) < GetSubVer(m_pGameParameter->dwAppClientVersion)) bAdviceUpdateClient = true;
//  			if (GetMainVer(dwClientVersion) != GetMainVer(m_pGameParameter->dwAppClientVersion)) bMustUpdateClient = true;
//  			if (GetProductVer(dwClientVersion) != GetProductVer(m_pGameParameter->dwAppClientVersion)) bMustUpdateClient = true;
// 		}
	}


	//����֪ͨ
	if ((bMustUpdateClient==true)||(bAdviceUpdateClient==true))
	{
		//��������
		CMD_GR_UpdateNotify UpdateNotify;
		ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));

		//��������
		UpdateNotify.cbMustUpdatePlaza=false;
		UpdateNotify.cbMustUpdateClient=bMustUpdateClient;
		UpdateNotify.cbAdviceUpdateClient=bAdviceUpdateClient;

		//��ǰ�汾
		UpdateNotify.dwCurrentPlazaVersion=VERSION_PLAZA;
		UpdateNotify.dwCurrentFrameVersion=VERSION_FRAME;
		if (bMobileClient == false)
		{
			UpdateNotify.dwCurrentClientVersion = m_pGameServiceAttrib->dwClientVersion;
		}
		else
		{
//			UpdateNotify.dwCurrentClientVersion = m_pGameServiceAttrib->dwAppClientVersion;
		}

		//������Ϣ
		SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_UPDATE_NOTIFY,&UpdateNotify,sizeof(UpdateNotify));

		//��ֹ�ж�
		if (bMustUpdateClient==true)
		{
//			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
			return false;
		}
	}

	return true;
}

//�л�����
bool CAttemperEngineSink::SwitchUserItemConnect(IServerUserItem * pIServerUserItem, TCHAR szMachineID[LEN_MACHINE_ID], WORD wTargetIndex,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount)
{
	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(wTargetIndex!=INVALID_WORD));
	if ((pIServerUserItem==NULL)||(wTargetIndex==INVALID_WORD)) return false;

	//�Ͽ��û�
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		//����֪ͨ
		LPCTSTR pszMessage=TEXT("��ע�⣬�����ʺ�����һ�ط������˴���Ϸ���䣬�������뿪��");
		SendRoomMessage(pIServerUserItem,pszMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

		//�󶨲���
		WORD wSourceIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pSourceParameter=GetBindParameter(wSourceIndex);

		//�����
		ASSERT((pSourceParameter!=NULL)&&(pSourceParameter->pIServerUserItem==pIServerUserItem));
		if ((pSourceParameter!=NULL)&&(pSourceParameter->pIServerUserItem==pIServerUserItem)) pSourceParameter->pIServerUserItem=NULL;

		//�Ͽ��û�
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			LOGI("CAttemperEngineSink::SwitchUserItemConnect DeleteAndroidUserItem, wAndroidIndex="<<LOWORD(pSourceParameter->dwSocketID)<<", wRoundID="<<HIWORD(pSourceParameter->dwSocketID));
			m_AndroidUserManager.DeleteAndroidUserItem(pSourceParameter->dwSocketID);
		}
		else
		{
			m_pITCPNetworkEngine->ShutDownSocket(pSourceParameter->dwSocketID);
		}
	}

	//״̬�л�
	bool bIsOffLine=false;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
	{
		//��������
		WORD wTableID=pIServerUserItem->GetTableID();
		WORD wChairID=pIServerUserItem->GetChairID();

		//����״̬
		bIsOffLine=true;
		pIServerUserItem->SetUserStatus(US_PLAYING,wTableID,wChairID);
	}
	if (pIServerUserItem->GetUserStatus() == US_TRUSTEE && (m_pGameServiceOption->wServerType&GAME_GENRE_USERCUSTOM))
	{
		//��������
		WORD wTableID = pIServerUserItem->GetTableID();
		WORD wChairID = pIServerUserItem->GetChairID();

		//����״̬
		bIsOffLine = true;
		pIServerUserItem->SetUserStatus(US_PLAYING, wTableID, wChairID);
	}

	//�����ж�
	LPCTSTR pszMachineID=pIServerUserItem->GetMachineID();
	bool bSameMachineID=(lstrcmp(pszMachineID,szMachineID)==0);

	//��������
	bool bAndroidUser=(wTargetIndex>=INDEX_ANDROID);
	tagBindParameter * pTargetParameter=GetBindParameter(wTargetIndex);

	//�����û�
	pTargetParameter->pIServerUserItem=pIServerUserItem;
	pIServerUserItem->SetUserParameter(pTargetParameter->dwClientAddr,wTargetIndex,szMachineID,bAndroidUser,false);

	//�ֻ���ʶ
	if(pTargetParameter->cbClientKind==CLIENT_KIND_MOBILE)
	{
		pIServerUserItem->SetMobileUser(true);
		SetMobileUserParameter(pIServerUserItem,cbDeviceType,wBehaviorFlags,wPageTableCount);
	}

	//��¼�¼�
	OnEventUserLogon(pIServerUserItem,true);

	//��ȫ��ʾ
	if ((bAndroidUser==false)&&(bIsOffLine==false)&&(bSameMachineID==false))
	{
		SendRoomMessage(pIServerUserItem,TEXT("��ע�⣬�����ʺ�����һ�ط������˴���Ϸ���䣬�Է������뿪��"),SMT_EJECT|SMT_CHAT|SMT_GLOBAL);
	}

	if (m_pGameServiceOption->wServerType&GAME_GENRE_USERCUSTOM)
	{
		//��������
		WORD wTableID = pIServerUserItem->GetTableID();
		WORD wChairID = pIServerUserItem->GetChairID();
		CTableFrame* pTableFrame = nullptr;
		if (wTableID < m_TableFrameArray.size())
		{
			pTableFrame = m_TableFrameArray.at(wTableID);
		}

		if (pTableFrame)
		{
			// ����ͬ����Ϣ
			for (WORD j = 0; j < pTableFrame->GetChairCount(); j++)
			{
 				IServerUserItem* pTableUserItem = pTableFrame->GetTableUserItem(j);
 				if (pTableUserItem == nullptr || pTableUserItem == pIServerUserItem)
 				{
 					continue;
 				}
 
 				// ͬ������Ϣ���͸��Լ�
				SendUserInfoPacket(pTableUserItem, pTargetParameter->dwSocketID);

				// �Լ�����Ϣ���͸�ͬ��
				tagBindParameter * pBindParameter = GetBindParameter(pTableUserItem->GetBindIndex());
				if (pBindParameter != nullptr)
				{
					SendUserInfoPacket(pIServerUserItem, pBindParameter->dwSocketID);
				}
			}
		}
	}

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID)
{
	//��������
	CMD_GR_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));

	//��������
	LogonFailure.lErrorCode=lErrorCode;
	lstrcpyn(LogonFailure.szDescribeString,pszString,CountArray(LogonFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);

	//��������
	SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FAILURE,&LogonFailure,wHeadSize+wDataSize);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::SendInsureFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszString, LONG lErrorCode,BYTE cbActivityGame)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_S_UserInsureFailure UserInsureFailure;
	ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

	//��������
	UserInsureFailure.cbActivityGame=cbActivityGame;
	UserInsureFailure.lErrorCode=lErrorCode;
	lstrcpyn(UserInsureFailure.szDescribeString,pszString,CountArray(UserInsureFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);

	//��������
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_INSURE_FAILURE,&UserInsureFailure,wHeadSize+wDataSize);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//��������
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//���ñ���
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(RequestFailure.szDescribeString);
	WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wHeadSize+wDataSize);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::SendPropertyFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode,WORD wRequestArea)
{
	//��������
	CMD_GR_PropertyFailure PropertyFailure;
	ZeroMemory(&PropertyFailure,sizeof(PropertyFailure));

	//���ñ���
	PropertyFailure.lErrorCode=lErrorCode;
	PropertyFailure.wRequestArea=wRequestArea;
	lstrcpyn(PropertyFailure.szDescribeString,pszDescribe,CountArray(PropertyFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(PropertyFailure.szDescribeString);
	WORD wHeadSize=sizeof(PropertyFailure)-sizeof(PropertyFailure.szDescribeString);
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_PROPERTY_FAILURE,&PropertyFailure,wHeadSize+wDataSize);

	return true;
}

//�����û�
bool CAttemperEngineSink::SendUserInfoPacket(IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	BYTE cbBuffer[SOCKET_TCP_PACKET];
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)cbBuffer;
	CSendPacketHelper SendPacket(cbBuffer+sizeof(tagUserInfoHead),sizeof(cbBuffer)-sizeof(tagUserInfoHead));

	//�û�����
	pUserInfoHead->wFaceID=pUserInfo->wFaceID;
	pUserInfoHead->dwGameID=pUserInfo->dwGameID;
	pUserInfoHead->dwUserID=pUserInfo->dwUserID;
	pUserInfoHead->dwGroupID=pUserInfo->dwGroupID;
	pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;

	//�û�����
	pUserInfoHead->cbGender=pUserInfo->cbGender;
	pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;
	pUserInfoHead->cbMasterOrder=pUserInfo->cbMasterOrder;

	//�û�״̬
	pUserInfoHead->wTableID=pUserInfo->wTableID;
	pUserInfoHead->wChairID=pUserInfo->wChairID;
	pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

	//�û�����
	pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
	pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
	pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
	pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
	pUserInfoHead->dwUserMedal=pUserInfo->dwUserMedal;
	pUserInfoHead->dwExperience=pUserInfo->dwExperience;
	pUserInfoHead->lLoveLiness=pUserInfo->lLoveLiness;

	//�û�����
	pUserInfoHead->lGrade=pUserInfo->lGrade;
	pUserInfoHead->lInsure=pUserInfo->lInsure;

	//�û���Ϸ��
	if (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM)
	{
		pUserInfoHead->lScore = pUserInfo->lScore - CUSTOMTABLE_INIT_SCORE;
	}
	else
	{
		pUserInfoHead->lScore = pUserInfo->lScore;
	}
	pUserInfoHead->lScore+=pIServerUserItem->GetTrusteeScore();
	pUserInfoHead->lScore+=pIServerUserItem->GetFrozenedScore();

	//������Ϣ
	SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);
	SendPacket.AddPacket(pUserInfo->szGroupName,DTP_GR_GROUP_NAME);
	SendPacket.AddPacket(pUserInfo->szUnderWrite,DTP_GR_UNDER_WRITE);

	//��������
	if (dwSocketID==INVALID_DWORD)
	{
		WORD wHeadSize=sizeof(tagUserInfoHead);
		SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}
	else
	{
		WORD wHeadSize=sizeof(tagUserInfoHead);
		SendData(dwSocketID,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}

	return true;
}

//�㲥����
bool CAttemperEngineSink::SendPropertyMessage(DWORD dwSourceID,DWORD dwTargerID,WORD wPropertyIndex,WORD wPropertyCount)
{
	//����ṹ
	CMD_GR_S_PropertyMessage  PropertyMessage;
	PropertyMessage.wPropertyIndex=wPropertyIndex;
	PropertyMessage.dwSourceUserID=dwSourceID;
	PropertyMessage.dwTargerUserID=dwTargerID;
	PropertyMessage.wPropertyCount=wPropertyCount;

	//�����û�
	WORD wUserIndex=0;
	IServerUserItem * pIServerUserItemSend=NULL;
	while (true)
	{
		pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
		if (pIServerUserItemSend==NULL) break;
		SendData(pIServerUserItemSend,MDM_GR_USER,SUB_GR_PROPERTY_MESSAGE,&PropertyMessage,sizeof(PropertyMessage));
	}

	return true;
}

//����ЧӦ
bool CAttemperEngineSink::SendPropertyEffect(IServerUserItem * pIServerUserItem)
{
	//����У��
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	CMD_GR_S_PropertyEffect  PropertyEffect;
	PropertyEffect.wUserID =pIServerUserItem->GetUserID();
	PropertyEffect.cbMemberOrder=pIServerUserItem->GetMemberOrder();

	//�����û�
	WORD wUserIndex=0;
	IServerUserItem * pIServerUserItemSend=NULL;
	while (true)
	{
		pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
		if (pIServerUserItemSend==NULL) break;
		SendData(pIServerUserItemSend,MDM_GR_USER,SUB_GR_PROPERTY_EFFECT,&PropertyEffect,sizeof(PropertyEffect));
	}

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventPropertyBuyPrep(WORD cbRequestArea,WORD wPropertyIndex,IServerUserItem *pISourceUserItem,IServerUserItem *pTargetUserItem)
{
	//Ŀ�����
	if ( pTargetUserItem == NULL )
	{
		//������Ϣ
		SendPropertyFailure(pISourceUserItem,TEXT("����ʧ�ܣ���Ҫ���͵�����Ѿ��뿪��"), 0L,cbRequestArea);

		return false;
	}

	//�����ж�
	if ( (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD || m_pGameServiceOption->wServerType == GAME_GENRE_MATCH ) && 
		(wPropertyIndex== PROPERTY_ID_SCORE_CLEAR||wPropertyIndex==PROPERTY_ID_TWO_CARD||wPropertyIndex == PROPERTY_ID_FOUR_CARD||wPropertyIndex == PROPERTY_ID_POSSESS) )
	{
		//������Ϣ
		SendPropertyFailure(pISourceUserItem,TEXT("�˷��䲻����ʹ�ô˵���,����ʧ��"), 0L,cbRequestArea);

		return false;
	}

	//���ҵ���
	tagPropertyInfo * pPropertyInfo=m_GamePropertyManager.SearchPropertyItem(wPropertyIndex);

	//��ЧЧ��
	if(pPropertyInfo==NULL)
	{
		//������Ϣ
		SendPropertyFailure(pISourceUserItem,TEXT("�˵��߻�δ����,����ʧ�ܣ�"), 0L,cbRequestArea);

		return false;
	}

	//�Լ�ʹ��
    if((pPropertyInfo->wIssueArea&PT_SERVICE_AREA_MESELF)==0 && pISourceUserItem==pTargetUserItem) 
	{
		//������Ϣ
		SendPropertyFailure(pISourceUserItem,TEXT("�˵��߲����Լ�ʹ��,����ʧ�ܣ�"), 0L,cbRequestArea);

		return false;
	}

	//���ʹ��
	if((pPropertyInfo->wIssueArea&PT_SERVICE_AREA_PLAYER)==0 && pISourceUserItem!=pTargetUserItem) 
	{
		//������Ϣ
		SendPropertyFailure(pISourceUserItem,TEXT("�˵��߲������͸����,ֻ���Լ�ʹ��,����ʧ�ܣ�"), 0L,cbRequestArea);

		return false;
	}

	//�Թ۷�Χ
	if((pPropertyInfo->wIssueArea&PT_SERVICE_AREA_LOOKON)==0)  
	{
		//��������
		WORD wTableID = pTargetUserItem->GetTableID();
		if(wTableID!=INVALID_TABLE)
		{
			//��������
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//��ȡ����
			CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	
			//ö���û�
			do
			{
				//��ȡ�û�
				pIServerUserItem=pTableFrame->EnumLookonUserItem(wEnumIndex++);
				if( pIServerUserItem==NULL) break;
				if( pIServerUserItem==pTargetUserItem )
				{
					//������Ϣ
					SendPropertyFailure(pISourceUserItem,TEXT("�˵��߲������͸��Թ��û�,����ʧ�ܣ�"), 0L,cbRequestArea);

					return false;
				}
			} while (true);
		}
	}

	//�����ж�
	switch(wPropertyIndex)
	{
	case PROPERTY_ID_SCORE_CLEAR :			//��������
		{
			//��������
			SCORE lCurrScore = pTargetUserItem->GetUserScore();
			if( lCurrScore >= 0)
			{
				//��������
				TCHAR szMessage[128]=TEXT("");
				if ( pISourceUserItem==pTargetUserItem ) 
					_sntprintf(szMessage,CountArray(szMessage),TEXT("�����ڵĻ����Ѿ��ǷǸ���������Ҫʹ�ø���������ߣ�"));
				else
					_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ]���ڵĻ����Ѿ��ǷǸ���������Ҫʹ�ø���������ߣ�"), pTargetUserItem->GetNickName());

				//������Ϣ
				SendPropertyFailure(pISourceUserItem,szMessage, 0L,cbRequestArea);

				return false;
			}
			break;
		}
	case PROPERTY_ID_ESCAPE_CLEAR :			 //��������
		{
			//��������
			DWORD dwCurrFleeCount = pTargetUserItem->GetUserInfo()->dwFleeCount;
			if ( dwCurrFleeCount==0 )
			{
				//��������
				TCHAR szMessage[128]=TEXT("");		
				if ( pISourceUserItem == pTargetUserItem ) 
					_sntprintf(szMessage,CountArray(szMessage),TEXT("�����ڵ��������Ѿ�Ϊ0������Ҫʹ������������ߣ�"));
				else
					_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ]���ڵ��������Ѿ�Ϊ0������Ҫʹ������������ߣ�"), pTargetUserItem->GetNickName());

				//������Ϣ
				SendPropertyFailure(pISourceUserItem,szMessage,0L,cbRequestArea);

				return false;
			}
			break;
		}
	}

	return true;
}

//���û�
IServerUserItem * CAttemperEngineSink::GetBindUserItem(WORD wBindIndex)
{
	//��ȡ����
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//��ȡ�û�
	if (pBindParameter!=NULL)
	{
		return pBindParameter->pIServerUserItem;
	}

	//�������
	ASSERT(FALSE);

	return NULL;
}

//�󶨲���
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//��Ч����
	if (wBindIndex==INVALID_WORD) return NULL;

	//��������
	if (wBindIndex<m_pGameServiceOption->wMaxPlayer)
	{
		return m_pNormalParameter+wBindIndex;
	}

	//��������
	if ((wBindIndex>=INDEX_ANDROID)&&(wBindIndex<(INDEX_ANDROID+MAX_ANDROID)))
	{
		return m_pAndroidParameter+(wBindIndex-INDEX_ANDROID);
	}

	//�������
	ASSERT(FALSE);

	return NULL;
}

//��������
WORD CAttemperEngineSink::GetPropertyType(WORD wPropertyIndex)
{
	switch(wPropertyIndex)
	{
	case PROPERTY_ID_CAR:	case PROPERTY_ID_EGG: 	case PROPERTY_ID_CLAP: 	case PROPERTY_ID_KISS: 	case PROPERTY_ID_BEER:
	case PROPERTY_ID_CAKE: 	case PROPERTY_ID_RING:  case PROPERTY_ID_BEAT: 	case PROPERTY_ID_BOMB:  case PROPERTY_ID_SMOKE:
	case PROPERTY_ID_VILLA: case PROPERTY_ID_BRICK: case PROPERTY_ID_FLOWER: 
		{
			return PT_TYPE_PRESENT;
		};
    case PROPERTY_ID_TWO_CARD: 	case PROPERTY_ID_FOUR_CARD:  case PROPERTY_ID_SCORE_CLEAR:     case PROPERTY_ID_ESCAPE_CLEAR:
	case PROPERTY_ID_TRUMPET:	case PROPERTY_ID_TYPHON:     case PROPERTY_ID_GUARDKICK_CARD:  case PROPERTY_ID_POSSESS:
	case PROPERTY_ID_BLUERING_CARD: case PROPERTY_ID_YELLOWRING_CARD: case PROPERTY_ID_WHITERING_CARD: case PROPERTY_ID_REDRING_CARD:
	case PROPERTY_ID_VIPROOM_CARD: 
		{
			return PT_TYPE_PROPERTY;
		};
	}

	ASSERT(false);

	return PT_TYPE_ERROR;
}

//���û���
bool CAttemperEngineSink::InitAndroidUser()
{
	//��������
	tagAndroidUserParameter AndroidUserParameter;
	ZeroMemory(&AndroidUserParameter,sizeof(AndroidUserParameter));

	//���ò���
	AndroidUserParameter.pGameParameter=m_pGameParameter;
	AndroidUserParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	AndroidUserParameter.pGameServiceOption=m_pGameServiceOption;
	AndroidUserParameter.pGameServiceTableOption = m_pGameServiceTableOption;

	//�������
	AndroidUserParameter.pITimerEngine=m_pITimerEngine;
	AndroidUserParameter.pIServerUserManager=&m_ServerUserManager;
	AndroidUserParameter.pIGameServiceManager=m_pIGameServiceManager;
	AndroidUserParameter.pITCPNetworkEngineEvent=QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent);

	//���ö���
	if (m_AndroidUserManager.InitAndroidUser(AndroidUserParameter)==false)
	{
		return false;
	}

	return true;
}

//��������
bool CAttemperEngineSink::InitTableFrameArray()
{
	//���Ӳ���
	tagTableFrameParameter TableFrameParameter;
	ZeroMemory(&TableFrameParameter,sizeof(TableFrameParameter));

	//�ں����
	TableFrameParameter.pITimerEngine=m_pITimerEngine;
	TableFrameParameter.pIKernelDataBaseEngine=m_pIKernelDataBaseEngine;
	TableFrameParameter.pIRecordDataBaseEngine=m_pIRecordDataBaseEngine;

	//�������
	TableFrameParameter.pIMainServiceFrame=this;
	TableFrameParameter.pIAndroidUserManager=&m_AndroidUserManager;
	TableFrameParameter.pIGameServiceManager=m_pIGameServiceManager;

	//���ò���
	TableFrameParameter.pGameParameter=m_pGameParameter;
	TableFrameParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	TableFrameParameter.pGameServiceOption=m_pGameServiceOption;

	if(m_pIGameMatchServiceManager!=NULL)
		TableFrameParameter.pIGameMatchServiceManager=QUERY_OBJECT_PTR_INTERFACE(m_pIGameMatchServiceManager,IUnknownEx);

	//��������
	m_TableFrameArray.resize(m_pGameServiceOption->wTableCount);

	//��������
	for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		//��������
		m_TableFrameArray[i]=new CTableFrame;

		//��������
		TableFrameParameter.pTableOption = &(m_pGameServiceTableOption->TableOption[i]);
		if (m_TableFrameArray[i]->InitializationFrame(i,TableFrameParameter)==false)
		{
			return false;
		}

		if(m_pIGameMatchServiceManager!=NULL)
			m_pIGameMatchServiceManager->InitTableFrame(QUERY_OBJECT_PTR_INTERFACE((m_TableFrameArray[i]),ITableFrame),i);
	}

	if(m_pIGameMatchServiceManager!=NULL)
	{
		if (m_pIGameMatchServiceManager->InitMatchInterface(QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent),m_pIKernelDataBaseEngine,
				(IServerUserManager*)QUERY_OBJECT_INTERFACE(m_ServerUserManager,IServerUserManager),this,this,m_pITimerEngine,&m_AndroidUserManager)==false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return true;
}

//���ù㳡��
bool CAttemperEngineSink::InitSquareMatch()
{
	LOGI("CAttemperEngineSink::InitSquareMatch ��ʼ���㳡��");
	//���Ӳ���
	tagSquareMatchParameter SquareMatchParameter;
	ZeroMemory(&SquareMatchParameter,sizeof(SquareMatchParameter));

	//�ں����
	SquareMatchParameter.pITimerEngine=m_pITimerEngine;
	SquareMatchParameter.pIKernelDataBaseEngine=m_pIKernelDataBaseEngine;
	SquareMatchParameter.pIRecordDataBaseEngine=m_pIRecordDataBaseEngine;

	//�������
	SquareMatchParameter.pIMainServiceFrame=this;
	SquareMatchParameter.pIServerUserManager=&m_ServerUserManager;

	//���ò���
	SquareMatchParameter.pGameParameter=m_pGameParameter;
	SquareMatchParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	SquareMatchParameter.pGameServiceOption=m_pGameServiceOption;
	//for(BYTE i=0; i<m_cbGameSquareOptionArrayCount; i++)
	//{
	//	SquareMatchParameter.pGameSquareOption=m_pGameSquareOptionArray[i];

	//	CSquare Square;
	//	Square.InitSquareMatch(SquareMatchParameter);
	//	m_SquareManager[m_pGameSquareOptionArray[i]->wMatchID]=Square;
	//}
	for(BYTE i=0; i<m_cbGameSquareOptionArrayCount; i++)
	{
		SquareMatchParameter.pGameSquareOption=m_pGameSquareOptionArray[i];

		CSquare Square;
		Square.InitSquareMatch(SquareMatchParameter);
		m_SquareManager[m_pGameSquareOptionArray[i]->wMatchID]=Square;
	}

	for(map<WORD,CSquare>::iterator iter=m_SquareManager.begin(); iter!=m_SquareManager.end(); iter++)
	{
		CSquare Square=iter->second;

		TCHAR szStartToEndTime[1024]=TEXT("");
		_sntprintf(szStartToEndTime,CountArray(szStartToEndTime),\
			TEXT("%d-%02d-%02d %02d:%02d:%02d - %d-%02d-%02d %02d:%02d:%02d"),\
			iter->second.m_pGameSquareOption->StartTime.wYear,iter->second.m_pGameSquareOption->StartTime.wMonth,iter->second.m_pGameSquareOption->StartTime.wDay,\
			iter->second.m_pGameSquareOption->StartTime.wHour,iter->second.m_pGameSquareOption->StartTime.wMinute,iter->second.m_pGameSquareOption->StartTime.wSecond,\
			iter->second.m_pGameSquareOption->EndTime.wYear,iter->second.m_pGameSquareOption->EndTime.wMonth,iter->second.m_pGameSquareOption->EndTime.wDay,\
			iter->second.m_pGameSquareOption->EndTime.wHour,iter->second.m_pGameSquareOption->EndTime.wMinute,iter->second.m_pGameSquareOption->EndTime.wSecond);
		LOGI("CAttemperEngineSink::InitSquareMatch �㳡������: MatchID= " << iter->second.m_pGameSquareOption->wMatchID\
			<< " SquareName= " << iter->second.m_pGameSquareOption->szSquareName << " ServerID= " << iter->second.m_pGameSquareOption->wServerID\
			<< " GameID= " << iter->second.m_pGameSquareOption->wGameID << " StartToEndTime= " << szStartToEndTime << " Interval= " << iter->second.m_pGameSquareOption->wInterval\
			<< " RankNumber= " << iter->second.m_pGameSquareOption->wRankNumber << " SquareDesc= " << iter->second.m_pGameSquareOption->szSquareDesc\
			<< " SquareBroadcast= " << iter->second.m_pGameSquareOption->szSquareBroadcast << " RankBonusCount= " << iter->second.m_pGameSquareOption->cbRankBonus\
			<< " RankBonusByResultCount= " << iter->second.m_pGameSquareOption->cbRankBonusByResult);

		LOGI("CAttemperEngineSink::InitSquareMatch �㳡���������� RankBonusCount= " << iter->second.m_pGameSquareOption->cbRankBonus);

		for(int i=0; i<iter->second.m_pGameSquareOption->cbRankBonus; i++)
		{
			LOGI("CAttemperEngineSink::InitSquareMatch i= " << i << " First= " << iter->second.m_pGameSquareOption->RankBonus[i].wFirst\
				<< " Last= " << iter->second.m_pGameSquareOption->RankBonus[i].wLast\
				<< " PropertyCount= " << iter->second.m_pGameSquareOption->RankBonus[i].cbPropertyCount);

			for(int j=0; j<iter->second.m_pGameSquareOption->RankBonus[i].cbPropertyCount; j++)
			{
				LOGI("CAttemperEngineSink::InitSquareMatch j= " << j << " PropertyID= " << iter->second.m_pGameSquareOption->RankBonus[i].wPropertyID[j]);
			}
		}
	}

	return true;
}

//���¹㳡��
bool CAttemperEngineSink::UpdateSquareMatch(tagGameSquareOption* pGameSquareOption)
{
	if(pGameSquareOption->wMatchID<=0)return false;

 	map<WORD,CSquare>::iterator iter=m_SquareManager.find(pGameSquareOption->wMatchID);
 	if(iter!=m_SquareManager.end())
 	{
 		if(iter->second.m_bStartSquare)
 		{
 			LOGE("CAttemperEngineSink::UpdateSquareMatch ���¹㳡��ʧ��, �㳡���ѿ�ʼ wMatch= " << pGameSquareOption->wMatchID << " SquareName= " << iter->second.m_pGameSquareOption->szSquareName);
 			return false;
 		}
 		memcpy(iter->second.m_pGameSquareOption, pGameSquareOption, sizeof(tagGameSquareOption));

		//������ʾ
		TCHAR szString[256]=TEXT("");
		LPCTSTR pszServerName=iter->second.m_pGameSquareOption->szSquareName;
		_sntprintf(szString,CountArray(szString),TEXT("[ %s ] �㳡���������Ǽ��سɹ�"),pszServerName);

		//�����Ϣ
		CTraceService::TraceString(szString,TraceLevel_Normal);
 	}
 	else
 	{
		//���Ӳ���
		tagSquareMatchParameter SquareMatchParameter;
		ZeroMemory(&SquareMatchParameter,sizeof(SquareMatchParameter));

		//�ں����
		SquareMatchParameter.pITimerEngine=m_pITimerEngine;
		SquareMatchParameter.pIKernelDataBaseEngine=m_pIKernelDataBaseEngine;
		SquareMatchParameter.pIRecordDataBaseEngine=m_pIRecordDataBaseEngine;

		//�������
		SquareMatchParameter.pIMainServiceFrame=this;
		SquareMatchParameter.pIServerUserManager=&m_ServerUserManager;

		//���ò���
		SquareMatchParameter.pGameParameter=m_pGameParameter;
		SquareMatchParameter.pGameServiceAttrib=m_pGameServiceAttrib;
		SquareMatchParameter.pGameServiceOption=m_pGameServiceOption;
		SquareMatchParameter.pGameSquareOption=pGameSquareOption;

		CSquare Square;
		Square.InitSquareMatch(SquareMatchParameter);
		m_SquareManager[pGameSquareOption->wMatchID]=Square;

		//������ʾ
		TCHAR szString[256]=TEXT("");
		LPCTSTR pszServerName=pGameSquareOption->szSquareName;
		_sntprintf(szString,CountArray(szString),TEXT("[ %s ] �㳡��������Ӽ��سɹ�"),pszServerName);

		//�����Ϣ
		CTraceService::TraceString(szString,TraceLevel_Normal);
	}

// 	for(map<WORD,CSquare>::iterator iter=m_SquareManager.begin(); iter!=m_SquareManager.end(); iter++)
// 	{
// 		CSquare Square=iter->second;
// 
// 		LOGI("CAttemperEngineSink::UpdateSquareMatch ���¹㳡�� wMatch= "\
// 			<< iter->second.m_pGameSquareOption->wMatchID << " SquareName= "\
// 			<< iter->second.m_pGameSquareOption->szSquareName);
// 	}

}

//��������
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//�������
bool CAttemperEngineSink::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//״̬�ж�
	ASSERT(pIServerUserItem->GetUserStatus()<US_PLAYING);
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	bool bReturn=false;
	if(m_pIGameMatchServiceManager!=NULL) bReturn=m_pIGameMatchServiceManager->OnUserJoinGame(pIServerUserItem,0);
	return bReturn;
}

//���дʹ���
void CAttemperEngineSink::SensitiveWordFilter(LPCTSTR pMsg, LPTSTR pszFiltered, int nMaxLen)
{
	m_WordsFilter.Filtrate(pMsg,pszFiltered,nMaxLen);
}

// ����ƽ��ֵ���(added by anjay )
LONG CAttemperEngineSink::CalculateBalanceScore(SCORE lOldScore, SCORE lNewScore)
{
	// �жϵ�ǰ�����Ƿ���������ֵ�������õĻ��������м���
	if (CServerRule::IsForfendCalculateBalanceScore(m_pGameServiceOption->dwServerRule))
	{
		return 0;
	}
	else
	{
		return m_BalanceScoreCurve.CalculateBalanceScore(lOldScore, lNewScore);
	}
}

//����㳡������
BYTE CAttemperEngineSink::ResultSquareMatchScore(ITableFrame* pITableFrame, tagScoreInfo ScoreInfoArray[], WORD wScoreCount)
{
	//�㳡���Ʒ�
	//for (size_t stSquareIndex=0; stSquareIndex<m_SquareManager.size(); stSquareIndex++)
	//{
	//	if (m_SquareManager[stSquareIndex].m_bStartSquare)
	//		m_SquareManager[stSquareIndex].ResultToRank(pITableFrame, ScoreInfoArray, wScoreCount);
	//}
	for (map<WORD,CSquare>::iterator iter=m_SquareManager.begin(); iter!=m_SquareManager.end(); iter++)
	{
		if(iter->second.m_bStartSquare)iter->second.ResultToRank(pITableFrame, ScoreInfoArray, wScoreCount);
	}
	return 0;
}

//����㳡������
BYTE CAttemperEngineSink::ResultSquareMatchScore(ITableFrame* pITableFrame, WORD wChairID, tagScoreInfo ScoreInfo)
{
	//�㳡���Ʒ�
	//for (size_t stSquareIndex=0; stSquareIndex<m_SquareManager.size(); stSquareIndex++)
	//{
	//	if (m_SquareManager[stSquareIndex].m_bStartSquare)
	//		m_SquareManager[stSquareIndex].ResultToRank(pITableFrame, ScoreInfoArray, wScoreCount);
	//}
	for (map<WORD,CSquare>::iterator iter=m_SquareManager.begin(); iter!=m_SquareManager.end(); iter++)
	{
		if(iter->second.m_bStartSquare)iter->second.ResultToRank(pITableFrame, wChairID, ScoreInfo);
	}
	return 0;
}

//�����û�
bool CAttemperEngineSink::InsertWaitDistribute(IServerUserItem * pIServerUserItem)
{
	//����
	bool bAdd=true;
	POSITION pos=m_WaitDistributeList.GetHeadPosition();
	while(pos != NULL)
	{
		IServerUserItem *pUserItem = m_WaitDistributeList.GetNext(pos);
		if(pUserItem->GetUserID()==pIServerUserItem->GetUserID())
		{
			bAdd=false;
			break;
		}
	}

	//����
	if(bAdd) m_WaitDistributeList.AddTail(pIServerUserItem);

	//֪ͨ
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_WAIT_DISTRIBUTE,NULL,0);

	return bAdd;
}

//ɾ���û�
bool CAttemperEngineSink::DeleteWaitDistribute(IServerUserItem * pIServerUserItem)
{
	//����
	POSITION pos=m_WaitDistributeList.GetHeadPosition();
	while(pos != NULL)
	{
		POSITION tempPos = pos;
		IServerUserItem *pUserItem = m_WaitDistributeList.GetNext(pos);
		if(pUserItem->GetUserID()==pIServerUserItem->GetUserID())
		{
			m_WaitDistributeList.RemoveAt(tempPos);
			break;
		}
	}

	return true;
}

//�����û�
bool CAttemperEngineSink::DistributeUserGame()
{
	//�û����ж�
	WORD wNeedUserCount=__max(m_pGameServiceAttrib->wChairCount, m_pGameServiceOption->wMinDistributeUser);
	if(m_pGameServiceAttrib->cbDynamicJoin==TRUE)wNeedUserCount=2;
	if(m_WaitDistributeList.GetCount()<wNeedUserCount)return true;

	//�������
	for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[i];
		if ((pTableFrame->IsGameStarted()==true)) continue;

		//����״��
		tagTableUserInfo TableUserInfo;
		WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

		//�����ж�
		if (wUserSitCount!=0) continue;

		//�����ж�
		if (m_WaitDistributeList.GetCount()>=TableUserInfo.wMinUserCount)
		{
			//��������
			WORD wHandleCount=0;
			WORD wWantUserCount=TableUserInfo.wMinUserCount;
			WORD wFreeUserCount=(WORD)m_WaitDistributeList.GetCount();

			//���ݵ���
			if (m_pGameServiceAttrib->wChairCount>TableUserInfo.wMinUserCount)
			{
				WORD wChairCount=m_pGameServiceAttrib->wChairCount;
				WORD wOffUserCount=__min(wChairCount,wFreeUserCount)-TableUserInfo.wMinUserCount;
				wWantUserCount+=(wOffUserCount > 0) ? (rand()%wOffUserCount) : 0;
			}

			//���´���
			POSITION pos=m_WaitDistributeList.GetHeadPosition();
			while(pos!=NULL)
			{
				//��������
				WORD wChairID=pTableFrame->GetRandNullChairID();

				//��Ч����
				if (wChairID==INVALID_CHAIR) continue;

				//���´���
				POSITION tempPos=pos;
				IServerUserItem *pUserItem = m_WaitDistributeList.GetNext(pos);
				if(pUserItem && pUserItem->GetUserStatus()==US_FREE && (pTableFrame->PerformSitDownAction(wChairID,pUserItem)==true))
				{
					//ɾ���Ŷ�
					m_WaitDistributeList.RemoveAt(tempPos);

					//���ñ���
					wHandleCount++;

					//����ж�
					if (wHandleCount>=wWantUserCount) 
					{
						return true;
					}
				}
			}

			if(wHandleCount > 0) return true;
		}
	}

	return true;
}

//���ò���
void CAttemperEngineSink::SetMobileUserParameter(IServerUserItem * pIServerUserItem,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount)
{
	if(wPageTableCount > m_pGameServiceOption->wTableCount)wPageTableCount=m_pGameServiceOption->wTableCount;
	pIServerUserItem->SetMobileUserDeskCount(wPageTableCount);
	pIServerUserItem->SetMobileUserRule(wBehaviorFlags);
}

//Ⱥ������
bool CAttemperEngineSink::SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//ö���û�
	WORD wEnumIndex=0;
	while(wEnumIndex<m_ServerUserManager.GetUserItemCount())
	{
		//�����û�
		IServerUserItem *pIServerUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if(pIServerUserItem==NULL) continue;
		if(!pIServerUserItem->IsMobileUser()) continue;

		//�������
		WORD wMobileUserRule = pIServerUserItem->GetMobileUserRule();
		WORD wTagerTableID = pIServerUserItem->GetTableID();
		bool bViewModeAll = ((wMobileUserRule&VIEW_MODE_ALL)!=0);
		bool bRecviceGameChat = ((wMobileUserRule&RECVICE_GAME_CHAT)!=0);
		bool bRecviceRoomChat = ((wMobileUserRule&RECVICE_ROOM_CHAT)!=0);
		bool bRecviceRoomWhisper = ((wMobileUserRule&RECVICE_ROOM_WHISPER)!=0);

		//״̬����
		if(pIServerUserItem->GetUserStatus() >= US_SIT)
		{
			if(wTagerTableID != wCmdTable)continue;
		}

		//�������
		if(wSubCmdID==SUB_GR_USER_CHAT || wSubCmdID==SUB_GR_USER_EXPRESSION)
		{
			if(!bRecviceGameChat || !bRecviceRoomChat) continue;
		}
		if(wSubCmdID==SUB_GR_WISPER_CHAT || wSubCmdID==SUB_GR_WISPER_EXPRESSION)
		{
			if(!bRecviceRoomWhisper) continue;
		}

		//���ֿ���
		if(!bViewModeAll)
		{
			//��Ϣ����
			if(wSubCmdID==SUB_GR_USER_ENTER && wCmdTable==INVALID_TABLE) continue;
			if(wSubCmdID==SUB_GR_USER_SCORE && pIServerUserItem->GetUserStatus() < US_SIT) continue;
			
			//�������
			WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
			WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();

			//״̬��Ϣ����
			if(wCmdTable < wTagerDeskPos) continue;
			if(wCmdTable > (wTagerDeskPos+wTagerDeskCount-1)) continue;
		}

		//������Ϣ
		SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	return true;
}

//Ⱥ���û���Ϣ
bool CAttemperEngineSink::SendUserInfoPacketBatchToMobileUser(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	BYTE cbBuffer[SOCKET_TCP_PACKET];
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)cbBuffer;
	CSendPacketHelper SendPacket(cbBuffer+sizeof(tagMobileUserInfoHead),sizeof(cbBuffer)-sizeof(tagMobileUserInfoHead));

	//�û�����
	pUserInfoHead->wFaceID=pUserInfo->wFaceID;
	pUserInfoHead->dwGameID=pUserInfo->dwGameID;
	pUserInfoHead->dwUserID=pUserInfo->dwUserID;
	pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;

	//�û�����
	pUserInfoHead->cbGender=pUserInfo->cbGender;
	pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;

	//�û�״̬
	pUserInfoHead->wTableID=pUserInfo->wTableID;
	pUserInfoHead->wChairID=pUserInfo->wChairID;
	pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

	//�û�����
	pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
	pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
	pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
	pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
	pUserInfoHead->dwExperience=pUserInfo->dwExperience;

	//�û���Ϸ��
	pUserInfoHead->lScore=pUserInfo->lScore;
	pUserInfoHead->lScore+=pIServerUserItem->GetTrusteeScore();
	pUserInfoHead->lScore+=pIServerUserItem->GetFrozenedScore();

	//������Ϣ
	SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);

	//��������
	WORD wHeadSize=sizeof(tagMobileUserInfoHead);
	SendDataBatchToMobileUser(pUserInfoHead->wTableID,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());

	return true;
}

//�������û���Ϣ(����ĳ����ҵ���Ϣ�����������е����)
bool CAttemperEngineSink::SendViewTableUserInfoPacketToMobileUser(IServerUserItem * pIServerUserItem,DWORD dwUserIDReq)
{
	//��������
	BYTE cbBuffer[SOCKET_TCP_PACKET];
	tagMobileUserInfoHead * pUserInfoHead=(tagMobileUserInfoHead *)cbBuffer;
	WORD wMobileUserRule = pIServerUserItem->GetMobileUserRule();
	WORD wTagerTableID = pIServerUserItem->GetTableID();
	WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
	WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();
	bool bViewModeAll = ((wMobileUserRule&VIEW_MODE_ALL)!=0);
	if(wTagerDeskCount==0) wTagerDeskCount=1;

	//ö���û�
	WORD wEnumIndex=0;
	while(wEnumIndex<m_ServerUserManager.GetUserItemCount())
	{
		//�����û�
		IServerUserItem *pIUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if(pIUserItem==NULL || (dwUserIDReq==INVALID_CHAIR && pIUserItem==pIServerUserItem)) continue;
		if(dwUserIDReq != INVALID_CHAIR && pIUserItem->GetUserID() != dwUserIDReq) continue;

		//���ֿ���
		if(dwUserIDReq==INVALID_CHAIR && !bViewModeAll)
		{
			if(pIUserItem->GetTableID() < wTagerDeskPos) continue;
			if(pIUserItem->GetTableID() > (wTagerDeskPos+wTagerDeskCount-1)) continue;
		}

		//�������
		tagUserInfo * pUserInfo=pIUserItem->GetUserInfo();
		ZeroMemory(cbBuffer,sizeof(cbBuffer));
		CSendPacketHelper SendPacket(cbBuffer+sizeof(tagMobileUserInfoHead),sizeof(cbBuffer)-sizeof(tagMobileUserInfoHead));

		//�û�����
		pUserInfoHead->wFaceID=pUserInfo->wFaceID;
		pUserInfoHead->dwGameID=pUserInfo->dwGameID;
		pUserInfoHead->dwUserID=pUserInfo->dwUserID;
		pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;

		//�û�����
		pUserInfoHead->cbGender=pUserInfo->cbGender;
		pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;

		//�û�״̬
		pUserInfoHead->wTableID=pUserInfo->wTableID;
		pUserInfoHead->wChairID=pUserInfo->wChairID;
		pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

		//�û�����
		pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
		pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
		pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
		pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
		pUserInfoHead->dwExperience=pUserInfo->dwExperience;

		//�û���Ϸ��
		pUserInfoHead->lScore=pUserInfo->lScore;
		pUserInfoHead->lScore+=pIUserItem->GetTrusteeScore();
		pUserInfoHead->lScore+=pIUserItem->GetFrozenedScore();

		//������Ϣ
		SendPacket.AddPacket(pUserInfo->szNickName,DTP_GR_NICK_NAME);

		//��������
		WORD wHeadSize=sizeof(tagMobileUserInfoHead);
		SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}

	return true;
}

//�ֻ�������¼
bool CAttemperEngineSink::MobileUserImmediately(IServerUserItem * pIServerUserItem)
{
	//��������
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[i];
		if (pTableFrame->IsGameStarted()==true) continue;
		if(pTableFrame->IsTableLocked()) continue;

		//��Ч����
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//�û�����
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//ʧ��
	m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("û�ҵ��ɽ������Ϸ����"),REQUEST_FAILURE_NORMAL);
	return true;
}

//����ϵͳ��Ϣ
bool CAttemperEngineSink::SendSystemMessage(CMD_GR_SendMessage * pSendMessage, WORD wDataSize)
{
	//��Ϣ����
	ASSERT(pSendMessage!=NULL);

	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+sizeof(TCHAR)*pSendMessage->wChatLength);
	if (wDataSize!=sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+sizeof(TCHAR)*pSendMessage->wChatLength) 
		return false;

	//���з���
	if(pSendMessage->cbAllRoom == TRUE)
	{
		pSendMessage->cbAllRoom=FALSE;
		m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_SYSTEM_MESSAGE,pSendMessage,wDataSize);

//		CTraceService::TraceString(TEXT("run sendsystemmessage��"),TraceLevel_Exception);
	}
	else
	{
		//����ϵͳ��Ϣ
		if(pSendMessage->cbGame == TRUE)
			SendGameMessage(pSendMessage->szSystemMessage, SMT_CHAT | SMT_TABLE_ROLL);
		if(pSendMessage->cbRoom == TRUE)
			SendRoomMessage(pSendMessage->szSystemMessage, SMT_CHAT | SMT_TABLE_ROLL);
	}

	return true;
}

//�����Ϣ����
void CAttemperEngineSink::ClearSystemMessageData()
{
	while(m_SystemMessageList.GetCount() > 0)
	{
		tagSystemMessage * pRqHead = m_SystemMessageList.RemoveHead();
		if(pRqHead)
		{
			delete [] ((BYTE*)pRqHead);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
