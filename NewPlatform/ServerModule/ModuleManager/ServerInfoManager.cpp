#include "StdAfx.h"
#include "ModuleDBParameter.h"
#include "ServerInfoManager.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerInfoBuffer::CServerInfoBuffer()
{
}

//��������
CServerInfoBuffer::~CServerInfoBuffer()
{
	//��������
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		SafeDelete(pGameServerInfo);
	}

	//ɾ������
	for (INT_PTR i=0;i<m_GameServerInfoArray.GetCount();i++)
	{
		pGameServerInfo=m_GameServerInfoArray[i];
		SafeDelete(pGameServerInfo);
	}

	//ɾ������
	m_GameServerInfoMap.RemoveAll();
	m_GameServerInfoArray.RemoveAll();

	return;
}

//��������
bool CServerInfoBuffer::ResetServerInfo()
{
	//��������
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		m_GameServerInfoArray.Add(pGameServerInfo);
	}

	//ɾ������
	m_GameServerInfoMap.RemoveAll();

	return true;
}

//ɾ������
bool CServerInfoBuffer::DeleteServerInfo(WORD wServerID)
{
	//��������
	tagGameServerInfo * pGameServerInfo=NULL;
	if (m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo)==FALSE) return false;

	//ɾ������
	m_GameServerInfoMap.RemoveKey(wServerID);
	m_GameServerInfoArray.Add(pGameServerInfo);

	//���ñ���
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//��������
bool CServerInfoBuffer::InsertServerInfo(tagGameServerInfo * pGameServerInfo)
{
	//Ч�����
	ASSERT(pGameServerInfo!=NULL);
	if (pGameServerInfo==NULL) return false;

	//�����ִ�
	WORD wServerID=pGameServerInfo->wServerID;
	tagGameServerInfo * pGameServerInsert=SearchServerInfo(wServerID);

	//�����ж�
	if (pGameServerInsert==NULL)
	{
		//��������
		pGameServerInsert=CreateServerInfo();

		//����ж�
		if (pGameServerInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//��������
	m_GameServerInfoMap[wServerID]=pGameServerInsert;
	CopyMemory(pGameServerInsert,pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//��ȡ��Ŀ
DWORD CServerInfoBuffer::GetServerInfoCount()
{
	return (DWORD)(m_GameServerInfoMap.GetCount());
}

//��������
tagGameServerInfo * CServerInfoBuffer::SearchServerInfo(WORD wServerID)
{
	tagGameServerInfo * pGameServerInfo=NULL;
	m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo);
	return pGameServerInfo;
}

//��������
tagGameServerInfo * CServerInfoBuffer::CreateServerInfo()
{
	//��������
	tagGameServerInfo * pGameServerInfo=NULL;

	//��������
	try
	{
		INT_PTR nArrayCount=m_GameServerInfoArray.GetCount();
		if (nArrayCount>0)
		{
			pGameServerInfo=m_GameServerInfoArray[nArrayCount-1];
			m_GameServerInfoArray.RemoveAt(nArrayCount-1);
		}
		else
		{
			pGameServerInfo=new tagGameServerInfo;
			if (pGameServerInfo==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//���ñ���
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return pGameServerInfo;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerInfoManager::CServerInfoManager()
{
}

//��������
CServerInfoManager::~CServerInfoManager()
{
}

//���ط���
bool CServerInfoManager::LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], CServerInfoBuffer & ServerInfoBuffer)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	CDataBaseAide UserCustomDBAide;
	CDataBaseHelper UserCustomDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//�������ݿ�
		if (ConnectUserCustomDB(UserCustomDBModule) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());
		UserCustomDBAide.SetDataBase(UserCustomDBModule.GetInterface());

		//�������
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),0);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),szMachineID);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//��ȡ�б�
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameRoomItem"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//����б�
		ServerInfoBuffer.ResetServerInfo();

		//��ȡ�б�
		while (PlatformDBModule->IsRecordsetEnd()==false)
		{
			//��ȡ����
			tagGameServerInfo GameServerResult;
			ReadGameServerInfo(PlatformDBAide,GameServerResult);

			//�����б�
			ServerInfoBuffer.InsertServerInfo(&GameServerResult);

			//�ƶ���¼
			PlatformDBModule->MoveToNext();
		}

		//������ص���������
		//��������
		POSITION Position = ServerInfoBuffer.m_GameServerInfoMap.GetStartPosition();

		//ö��ģ��
		while (Position != NULL)
		{
			//��ȡ����
			WORD wServerID = 0L;
			tagGameServerInfo * pServerInfo = NULL;
			ServerInfoBuffer.m_GameServerInfoMap.GetNextAssoc(Position, wServerID, pServerInfo);

			//������������
			PlatformDBAide.ResetParameter();
			PlatformDBAide.AddParameter(TEXT("@wServerID"), pServerInfo->wServerID);

			//�����Ϣ
			TCHAR szDescribeString[128] = TEXT("");
			PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

			//��ȡ�б�
			if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameRoomItemTable"), true) != DB_SUCCESS)
			{
				//��ȡ��Ϣ
				PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"), szDescribeString, CountArray(szDescribeString));

				//������ʾ
				CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

				//������ʾ
				AfxMessageBox(szDescribeString, MB_ICONERROR);

				return false;
			}

			//��ȡ�б�
			while (PlatformDBModule->IsRecordsetEnd() == false)
			{
				//��ȡ����
				tagDBGameTableInfo GameTableInfo;
				memset(&GameTableInfo, 0, sizeof(tagDBGameTableInfo));
				ReadGameTableInfo(PlatformDBAide, GameTableInfo);
				pServerInfo->TableConfig.push_back(GameTableInfo);

				//�ƶ���¼
				PlatformDBModule->MoveToNext();
			}

			if (pServerInfo->TableConfig.empty())
 			{
 				continue;
 			}

			// ��ȡ�Խ�������Ϣ
			if (pServerInfo->wServerType & GAME_GENRE_USERCUSTOM)
			{
				UserCustomDBAide.ResetParameter();
				UserCustomDBAide.AddParameter(TEXT("@wServerID"), pServerInfo->wServerID);

				//�����Ϣ
				TCHAR szDescribeString[128] = TEXT("");
				UserCustomDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

				//��ȡ�б�
				if (UserCustomDBAide.ExecuteProcess(TEXT("GSP_GR_LoadUserCustomTableInfo"), true) != DB_SUCCESS)
				{
					//��ȡ��Ϣ
					UserCustomDBAide.GetParameter(TEXT("@strErrorDescribe"), szDescribeString, CountArray(szDescribeString));

					//������ʾ
					CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

					//������ʾ
					AfxMessageBox(szDescribeString, MB_ICONERROR);

					return false;
				}

				//��ȡ�б�
				while (UserCustomDBModule->IsRecordsetEnd() == false)
				{
					tagDBUserCustomTableInfo DBUserCustomTableInfo;
					ReadUserCustomTableInfo(UserCustomDBAide, DBUserCustomTableInfo);

					pServerInfo->vectUserCustomTableInfo.push_back(DBUserCustomTableInfo);

					//�ƶ���¼
					UserCustomDBModule->MoveToNext();
				}
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//���ط���
bool CServerInfoManager::LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], WORD wServerID, tagGameServerInfo & GameServerResult)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//��ȡ�б�
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),szMachineID);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//��ȡ�б�
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameRoomItem"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ����
		if (PlatformDBModule->IsRecordsetEnd()==false)
		{
			ReadGameServerInfo(PlatformDBAide,GameServerResult);
		}

		//������������
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"), GameServerResult.wServerID);

		//�����Ϣ
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//��ȡ�б�
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameRoomItemTable"), true) != DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"), szDescribeString, CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		//��ȡ�б�
//		GameServerResult.wTableConfigCount = 0;
//		size_t i = 0;
		while (PlatformDBModule->IsRecordsetEnd() == false)
		{
			// ���ֻ��20��
//			if (GameServerResult.wTableConfigCount < 20)
//			{
				//��ȡ����
			tagDBGameTableInfo GameTableInfo;
			memset(&GameTableInfo, 0, sizeof(tagDBGameTableInfo));
			ReadGameTableInfo(PlatformDBAide, GameTableInfo);
			GameServerResult.TableConfig.push_back(GameTableInfo);
				
			//�ƶ���¼
			PlatformDBModule->MoveToNext();

//				GameServerResult.wTableConfigCount++;
//			}
		}

		if (GameServerResult.TableConfig.size() == 0)
		{
			AfxMessageBox(TEXT("δ��ȡ��������������"), MB_ICONERROR);
			return false;
		}

		// ������Խ�����ģʽ,������Ҵ����ķ���


		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//ɾ������
bool CServerInfoManager::DeleteGameServer(WORD wServerID)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//�������
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ������
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_DeleteGameRoom"),false)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		// ɾ����������
		//�������
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"), wServerID);

		//�����Ϣ
		memset(szDescribeString, 0, sizeof(szDescribeString));
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ������
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_DeleteGameRoomTable"), false) != DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"), szDescribeString, CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//���뷿��
bool CServerInfoManager::InsertGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult)
{
	LOGI("CServerInfoManager::InsertGameServer")

	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//��չ����
		TCHAR szCustomRule[CountArray(pGameServerCreate->cbCustomRule)*2+1]=TEXT("");
		for (INT i=0;i<CountArray(pGameServerCreate->cbCustomRule);i++) _stprintf(&szCustomRule[i*2],TEXT("%02X"),pGameServerCreate->cbCustomRule[i]);

		//�������
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wGameID"),pGameServerCreate->wGameID);
		PlatformDBAide.AddParameter(TEXT("@wKindID"),pGameServerCreate->wKindID);
		PlatformDBAide.AddParameter(TEXT("@wNodeID"),pGameServerCreate->wNodeID);
		PlatformDBAide.AddParameter(TEXT("@wSortID"),pGameServerCreate->wSortID);
		PlatformDBAide.AddParameter(TEXT("@lCellScore"),pGameServerCreate->lCellScore);
		PlatformDBAide.AddParameter(TEXT("@wRevenueRatio"),pGameServerCreate->wRevenueRatio);
		PlatformDBAide.AddParameter(TEXT("@lServiceScore"),pGameServerCreate->lServiceScore);
		PlatformDBAide.AddParameter(TEXT("@lRestrictScore"),pGameServerCreate->lRestrictScore);
		PlatformDBAide.AddParameter(TEXT("@lMinTableScore"),pGameServerCreate->lMinTableScore);
		PlatformDBAide.AddParameter(TEXT("@lMinEnterScore"),pGameServerCreate->lMinEnterScore);
		PlatformDBAide.AddParameter(TEXT("@lMaxEnterScore"),pGameServerCreate->lMaxEnterScore);
		PlatformDBAide.AddParameter(TEXT("@cbMinEnterMember"),pGameServerCreate->cbMinEnterMember);
		PlatformDBAide.AddParameter(TEXT("@cbMaxEnterMember"),pGameServerCreate->cbMaxEnterMember);
		PlatformDBAide.AddParameter(TEXT("@dwServerRule"),pGameServerCreate->dwServerRule);
		PlatformDBAide.AddParameter(TEXT("@dwAttachUserRight"),pGameServerCreate->dwAttachUserRight);
		PlatformDBAide.AddParameter(TEXT("@wMaxPlayer"),pGameServerCreate->wMaxPlayer);
		PlatformDBAide.AddParameter(TEXT("@wTableCount"),pGameServerCreate->wTableCount);
		PlatformDBAide.AddParameter(TEXT("@wServerPort"),pGameServerCreate->wServerPort);
		PlatformDBAide.AddParameter(TEXT("@wServerType"),pGameServerCreate->wServerType);
		PlatformDBAide.AddParameter(TEXT("@strServerName"),pGameServerCreate->szServerName);
		PlatformDBAide.AddParameter(TEXT("@cbDistributeRule"),pGameServerCreate->cbDistributeRule);
		PlatformDBAide.AddParameter(TEXT("@wMinDistributeUser"),pGameServerCreate->wMinDistributeUser);
		PlatformDBAide.AddParameter(TEXT("@wMaxDistributeUser"),pGameServerCreate->wMaxDistributeUser);
		PlatformDBAide.AddParameter(TEXT("@wDistributeTimeSpace"),pGameServerCreate->wDistributeTimeSpace);
		PlatformDBAide.AddParameter(TEXT("@wDistributeDrawCount"),pGameServerCreate->wDistributeDrawCount);
		PlatformDBAide.AddParameter(TEXT("@wDistributeStartDelay"),pGameServerCreate->wDistributeStartDelay);
		PlatformDBAide.AddParameter(TEXT("@strDataBaseName"),pGameServerCreate->szDataBaseName);
		PlatformDBAide.AddParameter(TEXT("@strDataBaseAddr"),pGameServerCreate->szDataBaseAddr);
		PlatformDBAide.AddParameter(TEXT("@strCustomRule"),szCustomRule);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),pGameServerCreate->szServiceMachine);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ������
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_InsertGameRoom"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ����
		ReadGameServerInfo(PlatformDBAide,GameServerResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//�޸ķ���
bool CServerInfoManager::ModifyGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//��չ����
		TCHAR szCustomRule[CountArray(pGameServerCreate->cbCustomRule)*2+1]=TEXT("");
		for (INT i=0;i<CountArray(pGameServerCreate->cbCustomRule);i++) _stprintf(&szCustomRule[i*2],TEXT("%02X"),pGameServerCreate->cbCustomRule[i]);

		//�������
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),pGameServerCreate->wServerID);
		PlatformDBAide.AddParameter(TEXT("@wKindID"),pGameServerCreate->wKindID);
		PlatformDBAide.AddParameter(TEXT("@wNodeID"),pGameServerCreate->wNodeID);
		PlatformDBAide.AddParameter(TEXT("@wSortID"),pGameServerCreate->wSortID);
		PlatformDBAide.AddParameter(TEXT("@lCellScore"),pGameServerCreate->lCellScore);
		PlatformDBAide.AddParameter(TEXT("@wRevenueRatio"),pGameServerCreate->wRevenueRatio);
		PlatformDBAide.AddParameter(TEXT("@lServiceScore"),pGameServerCreate->lServiceScore);
		PlatformDBAide.AddParameter(TEXT("@lRestrictScore"),pGameServerCreate->lRestrictScore);
		PlatformDBAide.AddParameter(TEXT("@lMinTableScore"),pGameServerCreate->lMinTableScore);
		PlatformDBAide.AddParameter(TEXT("@lMinEnterScore"),pGameServerCreate->lMinEnterScore);
		PlatformDBAide.AddParameter(TEXT("@lMaxEnterScore"),pGameServerCreate->lMaxEnterScore);
		PlatformDBAide.AddParameter(TEXT("@cbMinEnterMember"),pGameServerCreate->cbMinEnterMember);
		PlatformDBAide.AddParameter(TEXT("@cbMaxEnterMember"),pGameServerCreate->cbMaxEnterMember);
		PlatformDBAide.AddParameter(TEXT("@dwServerRule"),pGameServerCreate->dwServerRule);
		PlatformDBAide.AddParameter(TEXT("@dwAttachUserRight"),pGameServerCreate->dwAttachUserRight);
		PlatformDBAide.AddParameter(TEXT("@wMaxPlayer"),pGameServerCreate->wMaxPlayer);
		PlatformDBAide.AddParameter(TEXT("@wTableCount"),pGameServerCreate->wTableCount);
		PlatformDBAide.AddParameter(TEXT("@wServerPort"),pGameServerCreate->wServerPort);
		PlatformDBAide.AddParameter(TEXT("@wServerType"),pGameServerCreate->wServerType);
		PlatformDBAide.AddParameter(TEXT("@strServerName"),pGameServerCreate->szServerName);
		PlatformDBAide.AddParameter(TEXT("@cbDistributeRule"),pGameServerCreate->cbDistributeRule);
		PlatformDBAide.AddParameter(TEXT("@wMinDistributeUser"),pGameServerCreate->wMinDistributeUser);
		PlatformDBAide.AddParameter(TEXT("@wMaxDistributeUser"),pGameServerCreate->wMaxDistributeUser);
		PlatformDBAide.AddParameter(TEXT("@wDistributeTimeSpace"),pGameServerCreate->wDistributeTimeSpace);
		PlatformDBAide.AddParameter(TEXT("@wDistributeDrawCount"),pGameServerCreate->wDistributeDrawCount);
		PlatformDBAide.AddParameter(TEXT("@wDistributeStartDelay"),pGameServerCreate->wDistributeStartDelay);
		PlatformDBAide.AddParameter(TEXT("@strDataBaseName"),pGameServerCreate->szDataBaseName);
		PlatformDBAide.AddParameter(TEXT("@strDataBaseAddr"),pGameServerCreate->szDataBaseAddr);
		PlatformDBAide.AddParameter(TEXT("@strCustomRule"),szCustomRule);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),pGameServerCreate->szServiceMachine);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ������
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_ModifyGameRoom"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ����
		ReadGameServerInfo(PlatformDBAide,GameServerResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//����㳡��
bool CServerInfoManager::InsertSquareMatch(tagGameSquareOption * pSquareOptionCreate, tagGameSquareOption & SquareOptionResult)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//�������
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wGameID"),pSquareOptionCreate->wGameID);
		PlatformDBAide.AddParameter(TEXT("@wServerID"),pSquareOptionCreate->wServerID);
		PlatformDBAide.AddParameter(TEXT("@strServerName"),pSquareOptionCreate->szServerName);
		PlatformDBAide.AddParameter(TEXT("@StartTime"),pSquareOptionCreate->StartTime);
		PlatformDBAide.AddParameter(TEXT("@EndTime"),pSquareOptionCreate->EndTime);
		PlatformDBAide.AddParameter(TEXT("@wInterval"),pSquareOptionCreate->wInterval);
		PlatformDBAide.AddParameter(TEXT("@wRankNumber"),pSquareOptionCreate->wRankNumber);
		PlatformDBAide.AddParameter(TEXT("@strSquareName"),pSquareOptionCreate->szSquareName);
		PlatformDBAide.AddParameter(TEXT("@strSquareDesc"),pSquareOptionCreate->szSquareDesc);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ������
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_InsertSquareMatch"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ����
		ReadSquareMatchInfo(PlatformDBAide,SquareOptionResult);

		//�������
		PlatformDBAide.ResetParameter();
// 		PlatformDBAide.AddParameter(TEXT("@wServerID"),SquareOptionResult.wServerID);
		PlatformDBAide.AddParameter(TEXT("@wMatchID"),SquareOptionResult.wMatchID);

		//�����Ϣ
		ZeroMemory(szDescribeString,sizeof(szDescribeString));
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ������
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadSquareMatchRankBonus"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		tagRANKBONUS RankBonus[20];
		ZeroMemory(RankBonus,sizeof(RankBonus));
		//��ȡ�б�
		BYTE cbRankIndex=0;
		BYTE cbPropIDIndex=0;
		while (PlatformDBModule->IsRecordsetEnd()==false)
		{
			//����
			if(cbRankIndex>=20)break;
			if(cbPropIDIndex>=5)
			{
				cbRankIndex++;
				cbPropIDIndex=0;
				PlatformDBModule->MoveToNext();
				continue;
			}

			WORD wFirstRank=PlatformDBAide.GetValue_WORD(TEXT("FirstRank"));
			if(cbRankIndex==0&&cbPropIDIndex==0)
			{//�״����
				SquareOptionResult.RankBonus[cbRankIndex].wFirst=wFirstRank;
				SquareOptionResult.RankBonus[cbRankIndex].wLast=PlatformDBAide.GetValue_WORD(TEXT("EndRank"));
				SquareOptionResult.RankBonus[cbRankIndex].wPropertyID[cbPropIDIndex++]=PlatformDBAide.GetValue_WORD(TEXT("AwardID"));
			}
			else
			{
				if(wFirstRank==SquareOptionResult.RankBonus[cbRankIndex].wFirst)
				{//ͬ������Χ�ڵĵ���
					SquareOptionResult.RankBonus[cbRankIndex].wFirst=wFirstRank;
					SquareOptionResult.RankBonus[cbRankIndex].wLast=PlatformDBAide.GetValue_WORD(TEXT("EndRank"));
					SquareOptionResult.RankBonus[cbRankIndex].wPropertyID[cbPropIDIndex++]=PlatformDBAide.GetValue_WORD(TEXT("AwardID"));
				}
				else
				{//��һ��������Χ�ڵĵ���
					cbRankIndex++;
					cbPropIDIndex=0;
					SquareOptionResult.RankBonus[cbRankIndex].wFirst=wFirstRank;
					SquareOptionResult.RankBonus[cbRankIndex].wLast=PlatformDBAide.GetValue_WORD(TEXT("EndRank"));
					SquareOptionResult.RankBonus[cbRankIndex].wPropertyID[cbPropIDIndex++]=PlatformDBAide.GetValue_WORD(TEXT("AwardID"));
				}
			}

			PlatformDBModule->MoveToNext();
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//���ع㳡��
bool CServerInfoManager::LoadSquareMatch(WORD wServerID, tagGameSquareOption SquareOptionResultArray[], BYTE & cbGameSquareOpitonArrayCount)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;
	cbGameSquareOpitonArrayCount=0;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//�������
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ������
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadSquareMatch"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		while (PlatformDBModule->IsRecordsetEnd()==false)
		{
			if(cbGameSquareOpitonArrayCount>=20)break;

			tagGameSquareOption* pSquareOption=&SquareOptionResultArray[cbGameSquareOpitonArrayCount++];
			//��ȡ����
			ReadSquareMatchInfo(PlatformDBAide,*pSquareOption);

			//���ص����㳡������
			LoadSquareMatchOption(/*pSquareOption->wServerID,*/pSquareOption->wMatchID,*pSquareOption);
			PlatformDBModule->MoveToNext();
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//���ع㳡������
bool CServerInfoManager::LoadSquareMatchOption(/*WORD wServerID, */WORD wMatchID, tagGameSquareOption & SquareOptionResult)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		/*
		//�������
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);
		PlatformDBAide.AddParameter(TEXT("@wMatchID"),wMatchID);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ������
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadSquareMatchOption"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ����
		ReadSquareMatchInfo(PlatformDBAide,SquareOptionResult);
		*/

		//�������
		PlatformDBAide.ResetParameter();
// 		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);
		PlatformDBAide.AddParameter(TEXT("@wMatchID"),wMatchID);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ������
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadSquareMatchRankBonus"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		tagRANKBONUS RankBonus[20];
		ZeroMemory(RankBonus,sizeof(RankBonus));
		//��ȡ�б�
		BYTE cbRankIndex=0;
		BYTE cbPropIDIndex=0;
		while (PlatformDBModule->IsRecordsetEnd()==false)
		{
			//����
			if(cbRankIndex>=20)break;
			if(cbPropIDIndex>=5)
			{
				cbRankIndex++;
				cbPropIDIndex=0;
				PlatformDBModule->MoveToNext();
				continue;
			}

			WORD wFirstRank=PlatformDBAide.GetValue_WORD(TEXT("FirstRank"));
			if(cbRankIndex==0&&cbPropIDIndex==0)
			{//�״����
				SquareOptionResult.RankBonus[cbRankIndex].wFirst=wFirstRank;
				SquareOptionResult.RankBonus[cbRankIndex].wLast=PlatformDBAide.GetValue_WORD(TEXT("EndRank"));
				SquareOptionResult.RankBonus[cbRankIndex].wPropertyID[cbPropIDIndex++]=PlatformDBAide.GetValue_WORD(TEXT("AwardID"));
				SquareOptionResult.RankBonus[cbRankIndex].cbPropertyCount++;
				SquareOptionResult.cbRankBonus++;
			}
			else
			{
				if(wFirstRank==SquareOptionResult.RankBonus[cbRankIndex].wFirst)
				{//ͬ������Χ�ڵĵ���
					SquareOptionResult.RankBonus[cbRankIndex].wFirst=wFirstRank;
					SquareOptionResult.RankBonus[cbRankIndex].wLast=PlatformDBAide.GetValue_WORD(TEXT("EndRank"));
					SquareOptionResult.RankBonus[cbRankIndex].wPropertyID[cbPropIDIndex++]=PlatformDBAide.GetValue_WORD(TEXT("AwardID"));
					SquareOptionResult.RankBonus[cbRankIndex].cbPropertyCount++;
				}
				else
				{//��һ��������Χ�ڵĵ���
					cbRankIndex++;
					cbPropIDIndex=0;
					SquareOptionResult.RankBonus[cbRankIndex].wFirst=wFirstRank;
					SquareOptionResult.RankBonus[cbRankIndex].wLast=PlatformDBAide.GetValue_WORD(TEXT("EndRank"));
					SquareOptionResult.RankBonus[cbRankIndex].wPropertyID[cbPropIDIndex++]=PlatformDBAide.GetValue_WORD(TEXT("AwardID"));
					SquareOptionResult.RankBonus[cbRankIndex].cbPropertyCount++;
					SquareOptionResult.cbRankBonus++;
				}
			}

			PlatformDBModule->MoveToNext();
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//�������������б�
//bool CServerInfoManager::InsertRoomTable(WORD wServerID, tagGameTableInfo* pGameTableInfo)
bool CServerInfoManager::InsertRoomTable(WORD wServerID, vector<tagDBGameTableInfo>* pGameTableInfo)
{
	//����ȫ�����µķ�ʽ

	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);

		//�����Ϣ
		TCHAR szDescribeString[128] = TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ������
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_DeleteGameRoomTable"), true) != DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"), szDescribeString, CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		//�������
		for (size_t i = 0; i < pGameTableInfo->size(); i++)
		{
			PlatformDBAide.ResetParameter();
//			tagGameTableInfo* info = pGameTableInfo + i;
			tagDBGameTableInfo GameTableInfo = pGameTableInfo->at(i);

			if (GameTableInfo.wStartTableID == 0 || GameTableInfo.wEndTableID == 0)
				break;

			PlatformDBAide.AddParameter(TEXT("@wServerID"), wServerID);
			PlatformDBAide.AddParameter(TEXT("@wStartTableID"), GameTableInfo.wStartTableID);
			PlatformDBAide.AddParameter(TEXT("@wEndTableID"), GameTableInfo.wEndTableID);
			PlatformDBAide.AddParameter(TEXT("@strTableName"), GameTableInfo.TableOption.szTableName);
			PlatformDBAide.AddParameter(TEXT("@lCellScore"), GameTableInfo.TableOption.lCellScore);
			PlatformDBAide.AddParameter(TEXT("@wRevenueRatio"), GameTableInfo.TableOption.wRevenueRatio);
			PlatformDBAide.AddParameter(TEXT("@lServiceScore"), GameTableInfo.TableOption.lServiceScore);
			PlatformDBAide.AddParameter(TEXT("@lRestrictScore"), GameTableInfo.TableOption.lRestrictScore);
			PlatformDBAide.AddParameter(TEXT("@lMinTableScore"), GameTableInfo.TableOption.lMinTableScore);
			PlatformDBAide.AddParameter(TEXT("@lMaxTableScore"), GameTableInfo.TableOption.lMaxTableScore);
			PlatformDBAide.AddParameter(TEXT("@cbMinTableMember"), (int)GameTableInfo.TableOption.cbMinTableEnterMember);
			PlatformDBAide.AddParameter(TEXT("@cbMaxTableMember"), (int)GameTableInfo.TableOption.cbMaxTableEnterMember);

			PlatformDBAide.AddParameter(TEXT("@wMaxGameTime"), GameTableInfo.TableOption.nMaxGameTime);

			PlatformDBAide.AddParameter(TEXT("@wTableNormalPicID"), GameTableInfo.TableOption.nNormalPicID);
			PlatformDBAide.AddParameter(TEXT("@wTablePlayingPicID"), GameTableInfo.TableOption.nPlayingPicID);

			TCHAR szCustomRule[CountArray(GameTableInfo.TableOption.cbCustomRule) * 2 + 1] = TEXT("");
			for (INT i = 0; i < CountArray(GameTableInfo.TableOption.cbCustomRule); i++)
 			{
				_stprintf(&szCustomRule[i * 2], TEXT("%02X"), GameTableInfo.TableOption.cbCustomRule[i]);
 			}
 			PlatformDBAide.AddParameter(TEXT("@strCustomRule"), szCustomRule);

			//�����Ϣ
			TCHAR szDescribeString[128] = TEXT("");
			PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

			//ִ������
			if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_InsertGameRoomTable"), true) != DB_SUCCESS)
			{
				//��ȡ��Ϣ
				PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"), szDescribeString, CountArray(szDescribeString));

				//������ʾ
				CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

				//������ʾ
				AfxMessageBox(szDescribeString, MB_ICONERROR);

				return false;
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe, TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe, MB_ICONERROR);

		return false;
	}

	return false;
};

//�������������б�
bool CServerInfoManager::LoadRoomTable()
{
	return false;
};

//��������
bool CServerInfoManager::ConnectPlatformDB(CDataBaseHelper & PlatformDBModule)
{
	//��ȡ����
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

	//��������
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("CreateGameServer ���� PlatformDBModule ����ʧ��"),MB_ICONERROR);
		return false;
	}

	//��������
	PlatformDBModule->SetConnectionInfo(pDataBaseParameter->szDataBaseAddr,pDataBaseParameter->wDataBasePort,
		pDataBaseParameter->szDataBaseName,pDataBaseParameter->szDataBaseUser,pDataBaseParameter->szDataBasePass);

	//��������
	PlatformDBModule->OpenConnection();

	return true;
}

bool CServerInfoManager::ConnectUserCustomDB(CDataBaseHelper & UserCustomDBModule)
{
	//��ȡ����
	CModuleDBParameter * pModuleDBParameter = CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter = pModuleDBParameter->GetUserCustomDBParameter();

	//��������
	if ((UserCustomDBModule.GetInterface() == NULL) && (UserCustomDBModule.CreateInstance() == false))
	{
		AfxMessageBox(TEXT("CreateGameServer ���� UserCustomDBModule ����ʧ��"), MB_ICONERROR);
		return false;
	}

	//��������
	UserCustomDBModule->SetConnectionInfo(pDataBaseParameter->szDataBaseAddr, pDataBaseParameter->wDataBasePort,
		pDataBaseParameter->szDataBaseName, pDataBaseParameter->szDataBaseUser, pDataBaseParameter->szDataBasePass);

	//��������
	UserCustomDBModule->OpenConnection();

	return true;
}

//��ȡ��������
bool CServerInfoManager::ReadGameTableInfo(CDataBaseAide & PlatformDBAide, tagDBGameTableInfo & TableInfo)
{
	//���ñ���
	ZeroMemory(&TableInfo, sizeof(TableInfo));

	TableInfo.wServerID = PlatformDBAide.GetValue_WORD(TEXT("ServerID"));

	PlatformDBAide.GetValue_String(TEXT("TableName"), TableInfo.TableOption.szTableName, CountArray(TableInfo.TableOption.szTableName));

	TableInfo.wStartTableID = PlatformDBAide.GetValue_WORD(TEXT("StartTableID"));
	TableInfo.wEndTableID = PlatformDBAide.GetValue_WORD(TEXT("EndTableID"));

	TableInfo.TableOption.lCellScore = PlatformDBAide.GetValue_DWORD(TEXT("CellScore"));
	TableInfo.TableOption.wRevenueRatio = PlatformDBAide.GetValue_DWORD(TEXT("RevenueRatio"));
	TableInfo.TableOption.lServiceScore = PlatformDBAide.GetValue_DWORD(TEXT("ServiceScore"));

	TableInfo.TableOption.lRestrictScore = PlatformDBAide.GetValue_DWORD(TEXT("RestrictScore"));
	TableInfo.TableOption.lMinTableScore = PlatformDBAide.GetValue_DWORD(TEXT("MinTableScore"));
	TableInfo.TableOption.lMaxTableScore = PlatformDBAide.GetValue_DWORD(TEXT("MaxTableScore"));

	TableInfo.TableOption.cbMinTableEnterMember = PlatformDBAide.GetValue_DWORD(TEXT("MinTableMember"));
	TableInfo.TableOption.cbMaxTableEnterMember = PlatformDBAide.GetValue_DWORD(TEXT("MaxTableMember"));

	TableInfo.TableOption.nMaxGameTime = PlatformDBAide.GetValue_DWORD(TEXT("MaxGameTime"));

	TableInfo.TableOption.nNormalPicID = PlatformDBAide.GetValue_DWORD(TEXT("NormalPicID"));
	TableInfo.TableOption.nPlayingPicID = PlatformDBAide.GetValue_DWORD(TEXT("PlayingPicID"));

	TCHAR szCustomRule[CountArray(TableInfo.TableOption.cbCustomRule) * 2 + 1];
	PlatformDBAide.GetValue_String(TEXT("CustomRule"), szCustomRule, CountArray(szCustomRule));
	//��չ����
	if (szCustomRule[0] != 0)
	{
		//��ȡ����
		ASSERT(lstrlen(szCustomRule) % 2 == 0);
		INT nCustomRuleSize = lstrlen(szCustomRule) / 2;

		// ת���ַ�
		for (INT i = 0; i < nCustomRuleSize; i++)
		{
			//��ȡ�ַ�
			TCHAR cbChar1 = szCustomRule[i * 2];
			TCHAR cbChar2 = szCustomRule[i * 2 + 1];

			//Ч���ַ�
			ASSERT((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9')) || (cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F')));
			ASSERT((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9')) || (cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F')));

			//���ɽ��
			if ((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9'))) TableInfo.TableOption.cbCustomRule[i] += (cbChar2 - TEXT('0'));
			if ((cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F'))) TableInfo.TableOption.cbCustomRule[i] += (cbChar2 - TEXT('A') + 0x0A);

			//���ɽ��
			if ((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9'))) TableInfo.TableOption.cbCustomRule[i] += (cbChar1 - TEXT('0')) * 0x10;
			if ((cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F'))) TableInfo.TableOption.cbCustomRule[i] += (cbChar1 - TEXT('A') + 0x0A) * 0x10;
		}
	}

	return true;
}

//��ȡ����
bool CServerInfoManager::ReadGameServerInfo(CDataBaseAide & PlatformDBAide, tagGameServerInfo & GameServerResult)
{
	//���ñ���
	ZeroMemory(&GameServerResult,sizeof(GameServerResult));

	//��������
	GameServerResult.wGameID=PlatformDBAide.GetValue_WORD(TEXT("GameID"));
	GameServerResult.wServerID=PlatformDBAide.GetValue_WORD(TEXT("ServerID"));

	//�ҽ�����
	GameServerResult.wKindID=PlatformDBAide.GetValue_WORD(TEXT("KindID"));
	GameServerResult.wNodeID=PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
	GameServerResult.wSortID=PlatformDBAide.GetValue_WORD(TEXT("SortID"));

	//�汾��Ϣ
	GameServerResult.dwClientVersion=PlatformDBAide.GetValue_DWORD(TEXT("ClientVersion"));
//	GameServerResult.dwAppClientVersion = PlatformDBAide.GetValue_DWORD(TEXT("AppClientVersion"));
	GameServerResult.dwServerVersion=PlatformDBAide.GetValue_DWORD(TEXT("ServerVersion"));

	//ģ������
	PlatformDBAide.GetValue_String(TEXT("GameName"),GameServerResult.szGameName,CountArray(GameServerResult.szGameName));
	PlatformDBAide.GetValue_String(TEXT("ServerDLLName"),GameServerResult.szServerDLLName,CountArray(GameServerResult.szServerDLLName));
	PlatformDBAide.GetValue_String(TEXT("ClientEXEName"),GameServerResult.szClientEXEName,CountArray(GameServerResult.szClientEXEName));

	//˰������
	GameServerResult.lCellScore=PlatformDBAide.GetValue_LONG(TEXT("CellScore"));
	GameServerResult.wRevenueRatio=PlatformDBAide.GetValue_WORD(TEXT("RevenueRatio"));
	GameServerResult.lServiceScore=PlatformDBAide.GetValue_LONGLONG(TEXT("ServiceScore"));

	//��������
	GameServerResult.lRestrictScore=PlatformDBAide.GetValue_LONGLONG(TEXT("RestrictScore"));
	GameServerResult.lMinTableScore=PlatformDBAide.GetValue_LONGLONG(TEXT("MinTableScore"));
	GameServerResult.lMinEnterScore=PlatformDBAide.GetValue_LONGLONG(TEXT("MinEnterScore"));
	GameServerResult.lMaxEnterScore=PlatformDBAide.GetValue_LONGLONG(TEXT("MaxEnterScore"));

	//��Ա����
	GameServerResult.cbMinEnterMember=PlatformDBAide.GetValue_BYTE(TEXT("MinEnterMember"));
	GameServerResult.cbMaxEnterMember=PlatformDBAide.GetValue_BYTE(TEXT("MaxEnterMember"));

	//��������
	GameServerResult.dwServerRule=PlatformDBAide.GetValue_DWORD(TEXT("ServerRule"));
	GameServerResult.dwAttachUserRight=PlatformDBAide.GetValue_DWORD(TEXT("AttachUserRight"));

	//��������
	GameServerResult.wMaxPlayer=PlatformDBAide.GetValue_WORD(TEXT("MaxPlayer"));
	GameServerResult.wTableCount=PlatformDBAide.GetValue_WORD(TEXT("TableCount"));
	GameServerResult.wServerPort=PlatformDBAide.GetValue_WORD(TEXT("ServerPort"));
	GameServerResult.wServerType=PlatformDBAide.GetValue_WORD(TEXT("ServerType"));
	PlatformDBAide.GetValue_String(TEXT("ServerName"),GameServerResult.szServerName,CountArray(GameServerResult.szServerName));

	//��������
	GameServerResult.cbDistributeRule=PlatformDBAide.GetValue_BYTE(TEXT("DistributeRule"));
	GameServerResult.wMinDistributeUser=PlatformDBAide.GetValue_WORD(TEXT("MinDistributeUser"));
	GameServerResult.wMaxDistributeUser=PlatformDBAide.GetValue_WORD(TEXT("MaxDistributeUser"));
	GameServerResult.wDistributeTimeSpace=PlatformDBAide.GetValue_WORD(TEXT("DistributeTimeSpace"));
	GameServerResult.wDistributeDrawCount=PlatformDBAide.GetValue_WORD(TEXT("DistributeDrawCount"));
	GameServerResult.wDistributeStartDelay=PlatformDBAide.GetValue_WORD(TEXT("DistributeStartDelay"));

	//��������
	PlatformDBAide.GetValue_String(TEXT("DataBaseName"),GameServerResult.szDataBaseName,CountArray(GameServerResult.szDataBaseName));
	PlatformDBAide.GetValue_String(TEXT("DataBaseAddr"),GameServerResult.szDataBaseAddr,CountArray(GameServerResult.szDataBaseAddr));

	//��չ����
//	TCHAR szCustomRule[CountArray(GameServerResult.cbCustomRule)*2+1];
//	PlatformDBAide.GetValue_String(TEXT("CustomRule"),szCustomRule,CountArray(szCustomRule));
	PlatformDBAide.GetValue_String(TEXT("ServiceMachine"),GameServerResult.szServiceMachine,CountArray(GameServerResult.szServiceMachine));


	//���ذ汾
	LPCTSTR pszServerDLLName=GameServerResult.szServerDLLName;
	CWHService::GetModuleVersion(pszServerDLLName,GameServerResult.dwNativeVersion);

	return true;
}

//��ȡ�㳡��
bool CServerInfoManager::ReadSquareMatchInfo(CDataBaseAide & PlatformDBAide, tagGameSquareOption & GameSquareResult)
{
	//���ñ���
	ZeroMemory(&GameSquareResult,sizeof(GameSquareResult));

	//��������
	GameSquareResult.wGameID=PlatformDBAide.GetValue_WORD(TEXT("GameID"));
	GameSquareResult.wMatchID=PlatformDBAide.GetValue_WORD(TEXT("MatchID"));

	//��������
	GameSquareResult.wServerID=PlatformDBAide.GetValue_WORD(TEXT("ServerID"));
	PlatformDBAide.GetValue_String(TEXT("ServerName"),GameSquareResult.szServerName,CountArray(GameSquareResult.szServerName));

	//ʱ������
	PlatformDBAide.GetValue_SystemTime(TEXT("StartAt"),GameSquareResult.StartTime);
	PlatformDBAide.GetValue_SystemTime(TEXT("EndAt"),GameSquareResult.EndTime);
	GameSquareResult.wInterval=PlatformDBAide.GetValue_WORD(TEXT("Interval"));

	//����������
	GameSquareResult.wRankNumber=PlatformDBAide.GetValue_WORD(TEXT("RankNumber"));
	PlatformDBAide.GetValue_String(TEXT("SquareName"),GameSquareResult.szSquareName,CountArray(GameSquareResult.szSquareName));
	PlatformDBAide.GetValue_String(TEXT("SquareDesc"),GameSquareResult.szSquareDesc,CountArray(GameSquareResult.szSquareDesc));

	return true;
}

// ��ȡ�Խ���������
bool CServerInfoManager::ReadUserCustomTableInfo(CDataBaseAide & UserCustomDBAide, tagDBUserCustomTableInfo & DBUserCustomTableInfo)
{
	DBUserCustomTableInfo.dwUserID = UserCustomDBAide.GetValue_DWORD(TEXT("UserID"));
	DBUserCustomTableInfo.wTableID = UserCustomDBAide.GetValue_WORD(TEXT("TableID"));
	UserCustomDBAide.GetValue_String(TEXT("Secret"), DBUserCustomTableInfo.szSecret, CountArray(DBUserCustomTableInfo.szSecret));
	TCHAR szCustomRule[CountArray(DBUserCustomTableInfo.cbCustomRule) * 2 + 1];
	UserCustomDBAide.GetValue_String(TEXT("CustomRule"), szCustomRule, CountArray(szCustomRule));

	DBUserCustomTableInfo.dwRoundCount = UserCustomDBAide.GetValue_DWORD(TEXT("RoundCount"));
	UserCustomDBAide.GetValue_SystemTime(TEXT("CreateDateTime"), DBUserCustomTableInfo.TableCreateTime);
	DBUserCustomTableInfo.dwPlayRoundCount = UserCustomDBAide.GetValue_DWORD(TEXT("PlayRoundCount"));

	// �ַ���ת�����ڴ�
	if (szCustomRule[0] != 0)
	{
		//��ȡ����
		ASSERT(lstrlen(szCustomRule) % 2 == 0);
		INT nCustomRuleSize = lstrlen(szCustomRule) / 2;

		//ת���ַ�
		for (INT i = 0; i < nCustomRuleSize; i++)
		{
			//��ȡ�ַ�
			TCHAR cbChar1 = szCustomRule[i * 2];
			TCHAR cbChar2 = szCustomRule[i * 2 + 1];

			//Ч���ַ�
			ASSERT((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9')) || (cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F')));
			ASSERT((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9')) || (cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F')));

			//���ɽ��
			if ((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9'))) DBUserCustomTableInfo.cbCustomRule[i] += (cbChar2 - TEXT('0'));
			if ((cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F'))) DBUserCustomTableInfo.cbCustomRule[i] += (cbChar2 - TEXT('A') + 0x0A);

			//���ɽ��
			if ((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9'))) DBUserCustomTableInfo.cbCustomRule[i] += (cbChar1 - TEXT('0')) * 0x10;
			if ((cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F'))) DBUserCustomTableInfo.cbCustomRule[i] += (cbChar1 - TEXT('A') + 0x0A) * 0x10;
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
