#include "StdAfx.h"
#include "ModuleDBParameter.h"
#include "ServerInfoManager.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerInfoBuffer::CServerInfoBuffer()
{
}

//析构函数
CServerInfoBuffer::~CServerInfoBuffer()
{
	//变量定义
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//删除索引
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		SafeDelete(pGameServerInfo);
	}

	//删除数组
	for (INT_PTR i=0;i<m_GameServerInfoArray.GetCount();i++)
	{
		pGameServerInfo=m_GameServerInfoArray[i];
		SafeDelete(pGameServerInfo);
	}

	//删除引用
	m_GameServerInfoMap.RemoveAll();
	m_GameServerInfoArray.RemoveAll();

	return;
}

//重置数据
bool CServerInfoBuffer::ResetServerInfo()
{
	//变量定义
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//删除对象
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		m_GameServerInfoArray.Add(pGameServerInfo);
	}

	//删除索引
	m_GameServerInfoMap.RemoveAll();

	return true;
}

//删除数据
bool CServerInfoBuffer::DeleteServerInfo(WORD wServerID)
{
	//查找类型
	tagGameServerInfo * pGameServerInfo=NULL;
	if (m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo)==FALSE) return false;

	//删除数据
	m_GameServerInfoMap.RemoveKey(wServerID);
	m_GameServerInfoArray.Add(pGameServerInfo);

	//设置变量
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//插入数据
bool CServerInfoBuffer::InsertServerInfo(tagGameServerInfo * pGameServerInfo)
{
	//效验参数
	ASSERT(pGameServerInfo!=NULL);
	if (pGameServerInfo==NULL) return false;

	//查找现存
	WORD wServerID=pGameServerInfo->wServerID;
	tagGameServerInfo * pGameServerInsert=SearchServerInfo(wServerID);

	//创建判断
	if (pGameServerInsert==NULL)
	{
		//创建对象
		pGameServerInsert=CreateServerInfo();

		//结果判断
		if (pGameServerInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置数据
	m_GameServerInfoMap[wServerID]=pGameServerInsert;
	CopyMemory(pGameServerInsert,pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//获取数目
DWORD CServerInfoBuffer::GetServerInfoCount()
{
	return (DWORD)(m_GameServerInfoMap.GetCount());
}

//查找数据
tagGameServerInfo * CServerInfoBuffer::SearchServerInfo(WORD wServerID)
{
	tagGameServerInfo * pGameServerInfo=NULL;
	m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo);
	return pGameServerInfo;
}

//创建对象
tagGameServerInfo * CServerInfoBuffer::CreateServerInfo()
{
	//变量定义
	tagGameServerInfo * pGameServerInfo=NULL;

	//创建对象
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

	//设置变量
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return pGameServerInfo;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerInfoManager::CServerInfoManager()
{
}

//析构函数
CServerInfoManager::~CServerInfoManager()
{
}

//加载房间
bool CServerInfoManager::LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], CServerInfoBuffer & ServerInfoBuffer)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	CDataBaseAide UserCustomDBAide;
	CDataBaseHelper UserCustomDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//连接数据库
		if (ConnectUserCustomDB(UserCustomDBModule) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());
		UserCustomDBAide.SetDataBase(UserCustomDBModule.GetInterface());

		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),0);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),szMachineID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//读取列表
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameRoomItem"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//清空列表
		ServerInfoBuffer.ResetServerInfo();

		//读取列表
		while (PlatformDBModule->IsRecordsetEnd()==false)
		{
			//读取数据
			tagGameServerInfo GameServerResult;
			ReadGameServerInfo(PlatformDBAide,GameServerResult);

			//插入列表
			ServerInfoBuffer.InsertServerInfo(&GameServerResult);

			//移动记录
			PlatformDBModule->MoveToNext();
		}

		//读出相关的桌子配置
		//变量定义
		POSITION Position = ServerInfoBuffer.m_GameServerInfoMap.GetStartPosition();

		//枚举模块
		while (Position != NULL)
		{
			//获取对象
			WORD wServerID = 0L;
			tagGameServerInfo * pServerInfo = NULL;
			ServerInfoBuffer.m_GameServerInfoMap.GetNextAssoc(Position, wServerID, pServerInfo);

			//读出桌子配置
			PlatformDBAide.ResetParameter();
			PlatformDBAide.AddParameter(TEXT("@wServerID"), pServerInfo->wServerID);

			//输出信息
			TCHAR szDescribeString[128] = TEXT("");
			PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

			//读取列表
			if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameRoomItemTable"), true) != DB_SUCCESS)
			{
				//获取信息
				PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"), szDescribeString, CountArray(szDescribeString));

				//错误提示
				CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

				//错误提示
				AfxMessageBox(szDescribeString, MB_ICONERROR);

				return false;
			}

			//读取列表
			while (PlatformDBModule->IsRecordsetEnd() == false)
			{
				//读取数据
				tagDBGameTableInfo GameTableInfo;
				memset(&GameTableInfo, 0, sizeof(tagDBGameTableInfo));
				ReadGameTableInfo(PlatformDBAide, GameTableInfo);
				pServerInfo->TableConfig.push_back(GameTableInfo);

				//移动记录
				PlatformDBModule->MoveToNext();
			}

			if (pServerInfo->TableConfig.empty())
 			{
 				continue;
 			}

			// 读取自建桌子信息
			if (pServerInfo->wServerType & GAME_GENRE_USERCUSTOM)
			{
				UserCustomDBAide.ResetParameter();
				UserCustomDBAide.AddParameter(TEXT("@wServerID"), pServerInfo->wServerID);

				//输出信息
				TCHAR szDescribeString[128] = TEXT("");
				UserCustomDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

				//读取列表
				if (UserCustomDBAide.ExecuteProcess(TEXT("GSP_GR_LoadUserCustomTableInfo"), true) != DB_SUCCESS)
				{
					//获取信息
					UserCustomDBAide.GetParameter(TEXT("@strErrorDescribe"), szDescribeString, CountArray(szDescribeString));

					//错误提示
					CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

					//错误提示
					AfxMessageBox(szDescribeString, MB_ICONERROR);

					return false;
				}

				//读取列表
				while (UserCustomDBModule->IsRecordsetEnd() == false)
				{
					tagDBUserCustomTableInfo DBUserCustomTableInfo;
					ReadUserCustomTableInfo(UserCustomDBAide, DBUserCustomTableInfo);

					pServerInfo->vectUserCustomTableInfo.push_back(DBUserCustomTableInfo);

					//移动记录
					UserCustomDBModule->MoveToNext();
				}
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//加载房间
bool CServerInfoManager::LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], WORD wServerID, tagGameServerInfo & GameServerResult)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//读取列表
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),szMachineID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//读取列表
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameRoomItem"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取数据
		if (PlatformDBModule->IsRecordsetEnd()==false)
		{
			ReadGameServerInfo(PlatformDBAide,GameServerResult);
		}

		//读出桌子配置
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"), GameServerResult.wServerID);

		//输出信息
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//读取列表
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameRoomItemTable"), true) != DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"), szDescribeString, CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		//读取列表
//		GameServerResult.wTableConfigCount = 0;
//		size_t i = 0;
		while (PlatformDBModule->IsRecordsetEnd() == false)
		{
			// 最多只读20条
//			if (GameServerResult.wTableConfigCount < 20)
//			{
				//读取数据
			tagDBGameTableInfo GameTableInfo;
			memset(&GameTableInfo, 0, sizeof(tagDBGameTableInfo));
			ReadGameTableInfo(PlatformDBAide, GameTableInfo);
			GameServerResult.TableConfig.push_back(GameTableInfo);
				
			//移动记录
			PlatformDBModule->MoveToNext();

//				GameServerResult.wTableConfigCount++;
//			}
		}

		if (GameServerResult.TableConfig.size() == 0)
		{
			AfxMessageBox(TEXT("未读取到房间桌子配置"), MB_ICONERROR);
			return false;
		}

		// 如果是自建桌子模式,载入玩家创建的房间


		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//删除房间
bool CServerInfoManager::DeleteGameServer(WORD wServerID)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_DeleteGameRoom"),false)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		// 删除桌子配置
		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"), wServerID);

		//输出信息
		memset(szDescribeString, 0, sizeof(szDescribeString));
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_DeleteGameRoomTable"), false) != DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"), szDescribeString, CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//插入房间
bool CServerInfoManager::InsertGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult)
{
	LOGI("CServerInfoManager::InsertGameServer")

	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//扩展配置
		TCHAR szCustomRule[CountArray(pGameServerCreate->cbCustomRule)*2+1]=TEXT("");
		for (INT i=0;i<CountArray(pGameServerCreate->cbCustomRule);i++) _stprintf(&szCustomRule[i*2],TEXT("%02X"),pGameServerCreate->cbCustomRule[i]);

		//插入参数
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

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_InsertGameRoom"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取资料
		ReadGameServerInfo(PlatformDBAide,GameServerResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//修改房间
bool CServerInfoManager::ModifyGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//扩展配置
		TCHAR szCustomRule[CountArray(pGameServerCreate->cbCustomRule)*2+1]=TEXT("");
		for (INT i=0;i<CountArray(pGameServerCreate->cbCustomRule);i++) _stprintf(&szCustomRule[i*2],TEXT("%02X"),pGameServerCreate->cbCustomRule[i]);

		//插入参数
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

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_ModifyGameRoom"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取资料
		ReadGameServerInfo(PlatformDBAide,GameServerResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//插入广场赛
bool CServerInfoManager::InsertSquareMatch(tagGameSquareOption * pSquareOptionCreate, tagGameSquareOption & SquareOptionResult)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//插入参数
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

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_InsertSquareMatch"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取资料
		ReadSquareMatchInfo(PlatformDBAide,SquareOptionResult);

		//插入参数
		PlatformDBAide.ResetParameter();
// 		PlatformDBAide.AddParameter(TEXT("@wServerID"),SquareOptionResult.wServerID);
		PlatformDBAide.AddParameter(TEXT("@wMatchID"),SquareOptionResult.wMatchID);

		//输出信息
		ZeroMemory(szDescribeString,sizeof(szDescribeString));
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadSquareMatchRankBonus"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		tagRANKBONUS RankBonus[20];
		ZeroMemory(RankBonus,sizeof(RankBonus));
		//读取列表
		BYTE cbRankIndex=0;
		BYTE cbPropIDIndex=0;
		while (PlatformDBModule->IsRecordsetEnd()==false)
		{
			//出界
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
			{//首次填充
				SquareOptionResult.RankBonus[cbRankIndex].wFirst=wFirstRank;
				SquareOptionResult.RankBonus[cbRankIndex].wLast=PlatformDBAide.GetValue_WORD(TEXT("EndRank"));
				SquareOptionResult.RankBonus[cbRankIndex].wPropertyID[cbPropIDIndex++]=PlatformDBAide.GetValue_WORD(TEXT("AwardID"));
			}
			else
			{
				if(wFirstRank==SquareOptionResult.RankBonus[cbRankIndex].wFirst)
				{//同排名范围内的道具
					SquareOptionResult.RankBonus[cbRankIndex].wFirst=wFirstRank;
					SquareOptionResult.RankBonus[cbRankIndex].wLast=PlatformDBAide.GetValue_WORD(TEXT("EndRank"));
					SquareOptionResult.RankBonus[cbRankIndex].wPropertyID[cbPropIDIndex++]=PlatformDBAide.GetValue_WORD(TEXT("AwardID"));
				}
				else
				{//下一个排名范围内的道具
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
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//加载广场赛
bool CServerInfoManager::LoadSquareMatch(WORD wServerID, tagGameSquareOption SquareOptionResultArray[], BYTE & cbGameSquareOpitonArrayCount)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;
	cbGameSquareOpitonArrayCount=0;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadSquareMatch"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		while (PlatformDBModule->IsRecordsetEnd()==false)
		{
			if(cbGameSquareOpitonArrayCount>=20)break;

			tagGameSquareOption* pSquareOption=&SquareOptionResultArray[cbGameSquareOpitonArrayCount++];
			//读取资料
			ReadSquareMatchInfo(PlatformDBAide,*pSquareOption);

			//加载单个广场赛配置
			LoadSquareMatchOption(/*pSquareOption->wServerID,*/pSquareOption->wMatchID,*pSquareOption);
			PlatformDBModule->MoveToNext();
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//加载广场赛配置
bool CServerInfoManager::LoadSquareMatchOption(/*WORD wServerID, */WORD wMatchID, tagGameSquareOption & SquareOptionResult)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		/*
		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);
		PlatformDBAide.AddParameter(TEXT("@wMatchID"),wMatchID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadSquareMatchOption"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取资料
		ReadSquareMatchInfo(PlatformDBAide,SquareOptionResult);
		*/

		//插入参数
		PlatformDBAide.ResetParameter();
// 		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);
		PlatformDBAide.AddParameter(TEXT("@wMatchID"),wMatchID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadSquareMatchRankBonus"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		tagRANKBONUS RankBonus[20];
		ZeroMemory(RankBonus,sizeof(RankBonus));
		//读取列表
		BYTE cbRankIndex=0;
		BYTE cbPropIDIndex=0;
		while (PlatformDBModule->IsRecordsetEnd()==false)
		{
			//出界
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
			{//首次填充
				SquareOptionResult.RankBonus[cbRankIndex].wFirst=wFirstRank;
				SquareOptionResult.RankBonus[cbRankIndex].wLast=PlatformDBAide.GetValue_WORD(TEXT("EndRank"));
				SquareOptionResult.RankBonus[cbRankIndex].wPropertyID[cbPropIDIndex++]=PlatformDBAide.GetValue_WORD(TEXT("AwardID"));
				SquareOptionResult.RankBonus[cbRankIndex].cbPropertyCount++;
				SquareOptionResult.cbRankBonus++;
			}
			else
			{
				if(wFirstRank==SquareOptionResult.RankBonus[cbRankIndex].wFirst)
				{//同排名范围内的道具
					SquareOptionResult.RankBonus[cbRankIndex].wFirst=wFirstRank;
					SquareOptionResult.RankBonus[cbRankIndex].wLast=PlatformDBAide.GetValue_WORD(TEXT("EndRank"));
					SquareOptionResult.RankBonus[cbRankIndex].wPropertyID[cbPropIDIndex++]=PlatformDBAide.GetValue_WORD(TEXT("AwardID"));
					SquareOptionResult.RankBonus[cbRankIndex].cbPropertyCount++;
				}
				else
				{//下一个排名范围内的道具
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
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//插入桌子配置列表
//bool CServerInfoManager::InsertRoomTable(WORD wServerID, tagGameTableInfo* pGameTableInfo)
bool CServerInfoManager::InsertRoomTable(WORD wServerID, vector<tagDBGameTableInfo>* pGameTableInfo)
{
	//采用全部更新的方式

	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);

		//输出信息
		TCHAR szDescribeString[128] = TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_DeleteGameRoomTable"), true) != DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"), szDescribeString, CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		//插入参数
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

			//输出信息
			TCHAR szDescribeString[128] = TEXT("");
			PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

			//执行命令
			if (PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_InsertGameRoomTable"), true) != DB_SUCCESS)
			{
				//获取信息
				PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"), szDescribeString, CountArray(szDescribeString));

				//错误提示
				CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

				//错误提示
				AfxMessageBox(szDescribeString, MB_ICONERROR);

				return false;
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe, TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe, MB_ICONERROR);

		return false;
	}

	return false;
};

//加载桌子配置列表
bool CServerInfoManager::LoadRoomTable()
{
	return false;
};

//连接数据
bool CServerInfoManager::ConnectPlatformDB(CDataBaseHelper & PlatformDBModule)
{
	//获取参数
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

	//创建对象
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("CreateGameServer 创建 PlatformDBModule 对象失败"),MB_ICONERROR);
		return false;
	}

	//设置连接
	PlatformDBModule->SetConnectionInfo(pDataBaseParameter->szDataBaseAddr,pDataBaseParameter->wDataBasePort,
		pDataBaseParameter->szDataBaseName,pDataBaseParameter->szDataBaseUser,pDataBaseParameter->szDataBasePass);

	//发起连接
	PlatformDBModule->OpenConnection();

	return true;
}

bool CServerInfoManager::ConnectUserCustomDB(CDataBaseHelper & UserCustomDBModule)
{
	//获取参数
	CModuleDBParameter * pModuleDBParameter = CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter = pModuleDBParameter->GetUserCustomDBParameter();

	//创建对象
	if ((UserCustomDBModule.GetInterface() == NULL) && (UserCustomDBModule.CreateInstance() == false))
	{
		AfxMessageBox(TEXT("CreateGameServer 创建 UserCustomDBModule 对象失败"), MB_ICONERROR);
		return false;
	}

	//设置连接
	UserCustomDBModule->SetConnectionInfo(pDataBaseParameter->szDataBaseAddr, pDataBaseParameter->wDataBasePort,
		pDataBaseParameter->szDataBaseName, pDataBaseParameter->szDataBaseUser, pDataBaseParameter->szDataBasePass);

	//发起连接
	UserCustomDBModule->OpenConnection();

	return true;
}

//读取房间配置
bool CServerInfoManager::ReadGameTableInfo(CDataBaseAide & PlatformDBAide, tagDBGameTableInfo & TableInfo)
{
	//设置变量
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
	//扩展配置
	if (szCustomRule[0] != 0)
	{
		//获取长度
		ASSERT(lstrlen(szCustomRule) % 2 == 0);
		INT nCustomRuleSize = lstrlen(szCustomRule) / 2;

		// 转换字符
		for (INT i = 0; i < nCustomRuleSize; i++)
		{
			//获取字符
			TCHAR cbChar1 = szCustomRule[i * 2];
			TCHAR cbChar2 = szCustomRule[i * 2 + 1];

			//效验字符
			ASSERT((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9')) || (cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F')));
			ASSERT((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9')) || (cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F')));

			//生成结果
			if ((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9'))) TableInfo.TableOption.cbCustomRule[i] += (cbChar2 - TEXT('0'));
			if ((cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F'))) TableInfo.TableOption.cbCustomRule[i] += (cbChar2 - TEXT('A') + 0x0A);

			//生成结果
			if ((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9'))) TableInfo.TableOption.cbCustomRule[i] += (cbChar1 - TEXT('0')) * 0x10;
			if ((cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F'))) TableInfo.TableOption.cbCustomRule[i] += (cbChar1 - TEXT('A') + 0x0A) * 0x10;
		}
	}

	return true;
}

//读取房间
bool CServerInfoManager::ReadGameServerInfo(CDataBaseAide & PlatformDBAide, tagGameServerInfo & GameServerResult)
{
	//设置变量
	ZeroMemory(&GameServerResult,sizeof(GameServerResult));

	//索引变量
	GameServerResult.wGameID=PlatformDBAide.GetValue_WORD(TEXT("GameID"));
	GameServerResult.wServerID=PlatformDBAide.GetValue_WORD(TEXT("ServerID"));

	//挂接属性
	GameServerResult.wKindID=PlatformDBAide.GetValue_WORD(TEXT("KindID"));
	GameServerResult.wNodeID=PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
	GameServerResult.wSortID=PlatformDBAide.GetValue_WORD(TEXT("SortID"));

	//版本信息
	GameServerResult.dwClientVersion=PlatformDBAide.GetValue_DWORD(TEXT("ClientVersion"));
//	GameServerResult.dwAppClientVersion = PlatformDBAide.GetValue_DWORD(TEXT("AppClientVersion"));
	GameServerResult.dwServerVersion=PlatformDBAide.GetValue_DWORD(TEXT("ServerVersion"));

	//模块属性
	PlatformDBAide.GetValue_String(TEXT("GameName"),GameServerResult.szGameName,CountArray(GameServerResult.szGameName));
	PlatformDBAide.GetValue_String(TEXT("ServerDLLName"),GameServerResult.szServerDLLName,CountArray(GameServerResult.szServerDLLName));
	PlatformDBAide.GetValue_String(TEXT("ClientEXEName"),GameServerResult.szClientEXEName,CountArray(GameServerResult.szClientEXEName));

	//税收配置
	GameServerResult.lCellScore=PlatformDBAide.GetValue_LONG(TEXT("CellScore"));
	GameServerResult.wRevenueRatio=PlatformDBAide.GetValue_WORD(TEXT("RevenueRatio"));
	GameServerResult.lServiceScore=PlatformDBAide.GetValue_LONGLONG(TEXT("ServiceScore"));

	//房间配置
	GameServerResult.lRestrictScore=PlatformDBAide.GetValue_LONGLONG(TEXT("RestrictScore"));
	GameServerResult.lMinTableScore=PlatformDBAide.GetValue_LONGLONG(TEXT("MinTableScore"));
	GameServerResult.lMinEnterScore=PlatformDBAide.GetValue_LONGLONG(TEXT("MinEnterScore"));
	GameServerResult.lMaxEnterScore=PlatformDBAide.GetValue_LONGLONG(TEXT("MaxEnterScore"));

	//会员限制
	GameServerResult.cbMinEnterMember=PlatformDBAide.GetValue_BYTE(TEXT("MinEnterMember"));
	GameServerResult.cbMaxEnterMember=PlatformDBAide.GetValue_BYTE(TEXT("MaxEnterMember"));

	//房间配置
	GameServerResult.dwServerRule=PlatformDBAide.GetValue_DWORD(TEXT("ServerRule"));
	GameServerResult.dwAttachUserRight=PlatformDBAide.GetValue_DWORD(TEXT("AttachUserRight"));

	//房间属性
	GameServerResult.wMaxPlayer=PlatformDBAide.GetValue_WORD(TEXT("MaxPlayer"));
	GameServerResult.wTableCount=PlatformDBAide.GetValue_WORD(TEXT("TableCount"));
	GameServerResult.wServerPort=PlatformDBAide.GetValue_WORD(TEXT("ServerPort"));
	GameServerResult.wServerType=PlatformDBAide.GetValue_WORD(TEXT("ServerType"));
	PlatformDBAide.GetValue_String(TEXT("ServerName"),GameServerResult.szServerName,CountArray(GameServerResult.szServerName));

	//分组属性
	GameServerResult.cbDistributeRule=PlatformDBAide.GetValue_BYTE(TEXT("DistributeRule"));
	GameServerResult.wMinDistributeUser=PlatformDBAide.GetValue_WORD(TEXT("MinDistributeUser"));
	GameServerResult.wMaxDistributeUser=PlatformDBAide.GetValue_WORD(TEXT("MaxDistributeUser"));
	GameServerResult.wDistributeTimeSpace=PlatformDBAide.GetValue_WORD(TEXT("DistributeTimeSpace"));
	GameServerResult.wDistributeDrawCount=PlatformDBAide.GetValue_WORD(TEXT("DistributeDrawCount"));
	GameServerResult.wDistributeStartDelay=PlatformDBAide.GetValue_WORD(TEXT("DistributeStartDelay"));

	//数据属性
	PlatformDBAide.GetValue_String(TEXT("DataBaseName"),GameServerResult.szDataBaseName,CountArray(GameServerResult.szDataBaseName));
	PlatformDBAide.GetValue_String(TEXT("DataBaseAddr"),GameServerResult.szDataBaseAddr,CountArray(GameServerResult.szDataBaseAddr));

	//扩展配置
//	TCHAR szCustomRule[CountArray(GameServerResult.cbCustomRule)*2+1];
//	PlatformDBAide.GetValue_String(TEXT("CustomRule"),szCustomRule,CountArray(szCustomRule));
	PlatformDBAide.GetValue_String(TEXT("ServiceMachine"),GameServerResult.szServiceMachine,CountArray(GameServerResult.szServiceMachine));


	//本地版本
	LPCTSTR pszServerDLLName=GameServerResult.szServerDLLName;
	CWHService::GetModuleVersion(pszServerDLLName,GameServerResult.dwNativeVersion);

	return true;
}

//读取广场赛
bool CServerInfoManager::ReadSquareMatchInfo(CDataBaseAide & PlatformDBAide, tagGameSquareOption & GameSquareResult)
{
	//设置变量
	ZeroMemory(&GameSquareResult,sizeof(GameSquareResult));

	//索引变量
	GameSquareResult.wGameID=PlatformDBAide.GetValue_WORD(TEXT("GameID"));
	GameSquareResult.wMatchID=PlatformDBAide.GetValue_WORD(TEXT("MatchID"));

	//房间属性
	GameSquareResult.wServerID=PlatformDBAide.GetValue_WORD(TEXT("ServerID"));
	PlatformDBAide.GetValue_String(TEXT("ServerName"),GameSquareResult.szServerName,CountArray(GameSquareResult.szServerName));

	//时间配置
	PlatformDBAide.GetValue_SystemTime(TEXT("StartAt"),GameSquareResult.StartTime);
	PlatformDBAide.GetValue_SystemTime(TEXT("EndAt"),GameSquareResult.EndTime);
	GameSquareResult.wInterval=PlatformDBAide.GetValue_WORD(TEXT("Interval"));

	//排名与描述
	GameSquareResult.wRankNumber=PlatformDBAide.GetValue_WORD(TEXT("RankNumber"));
	PlatformDBAide.GetValue_String(TEXT("SquareName"),GameSquareResult.szSquareName,CountArray(GameSquareResult.szSquareName));
	PlatformDBAide.GetValue_String(TEXT("SquareDesc"),GameSquareResult.szSquareDesc,CountArray(GameSquareResult.szSquareDesc));

	return true;
}

// 读取自建桌子配置
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

	// 字符串转规则内存
	if (szCustomRule[0] != 0)
	{
		//获取长度
		ASSERT(lstrlen(szCustomRule) % 2 == 0);
		INT nCustomRuleSize = lstrlen(szCustomRule) / 2;

		//转换字符
		for (INT i = 0; i < nCustomRuleSize; i++)
		{
			//获取字符
			TCHAR cbChar1 = szCustomRule[i * 2];
			TCHAR cbChar2 = szCustomRule[i * 2 + 1];

			//效验字符
			ASSERT((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9')) || (cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F')));
			ASSERT((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9')) || (cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F')));

			//生成结果
			if ((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9'))) DBUserCustomTableInfo.cbCustomRule[i] += (cbChar2 - TEXT('0'));
			if ((cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F'))) DBUserCustomTableInfo.cbCustomRule[i] += (cbChar2 - TEXT('A') + 0x0A);

			//生成结果
			if ((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9'))) DBUserCustomTableInfo.cbCustomRule[i] += (cbChar1 - TEXT('0')) * 0x10;
			if ((cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F'))) DBUserCustomTableInfo.cbCustomRule[i] += (cbChar1 - TEXT('A') + 0x0A) * 0x10;
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
