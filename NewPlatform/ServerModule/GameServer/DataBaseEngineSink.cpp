#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"

// 打印内存
#ifdef _DEBUG

#include <psapi.h>  
#pragma comment(lib,"psapi.lib")  

#endif // _DEBUG

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//配置变量
	m_pGameParameter=NULL;
	m_pInitParameter=NULL;
	m_pDataBaseParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件变量
	m_pIDataBaseEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_pIDataBaseEngineEvent=NULL;
	m_pIGameDataBaseEngineSink=NULL;
	m_pIDBCorrespondManager=NULL;

	//辅助变量
	ZeroMemory(&m_LogonFailure,sizeof(m_LogonFailure));
	ZeroMemory(&m_LogonSuccess,sizeof(m_LogonSuccess));

	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
	//释放对象
	SafeRelease(m_pIGameDataBaseEngineSink);

	return;
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
//	QUERYINTERFACE(IGameDataBaseEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//获取对象
VOID * CDataBaseEngineSink::GetDataBase(REFGUID Guid, DWORD dwQueryVer)
{
	//效验状态
	ASSERT(m_GameDBModule.GetInterface()!=NULL);
	if (m_GameDBModule.GetInterface()==NULL) return NULL;

	//查询对象
	IDataBase * pIDataBase=m_GameDBModule.GetInterface();
	VOID * pIQueryObject=pIDataBase->QueryInterface(Guid,dwQueryVer);

	return pIQueryObject;
}

//获取对象
VOID * CDataBaseEngineSink::GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer)
{
	//效验状态
	ASSERT(m_pIDataBaseEngine!=NULL);
	if (m_pIDataBaseEngine==NULL) return NULL;

	//查询对象
	VOID * pIQueryObject=m_pIDataBaseEngine->QueryInterface(Guid,dwQueryVer);

	return pIQueryObject;
}

//投递结果
bool CDataBaseEngineSink::PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//查询对象
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine)!=NULL);
	m_pIDataBaseEngine=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine);

	//创建对象
	if ((m_GameDBModule.GetInterface()==NULL)&&(m_GameDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_TreasureDBModule.GetInterface()==NULL)&&(m_TreasureDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_UserCustomDBModule.GetInterface() == NULL) && (m_UserCustomDBModule.CreateInstance() == false))
	{
		ASSERT(FALSE);
		return false;
	}
	

	//连接游戏
	try
	{
		//连接信息
		tagDataBaseParameter * pTreasureDBParameter=&m_pInitParameter->m_TreasureDBParameter;
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;
		tagDataBaseParameter * pUserCustomDBParameter = &m_pInitParameter->m_UserCustomDBParameter;

		//设置连接
		m_GameDBModule->SetConnectionInfo(m_pDataBaseParameter->szDataBaseAddr,m_pDataBaseParameter->wDataBasePort,
			m_pDataBaseParameter->szDataBaseName,m_pDataBaseParameter->szDataBaseUser,m_pDataBaseParameter->szDataBasePass);

		//设置连接
		m_TreasureDBModule->SetConnectionInfo(pTreasureDBParameter->szDataBaseAddr,pTreasureDBParameter->wDataBasePort,
			pTreasureDBParameter->szDataBaseName,pTreasureDBParameter->szDataBaseUser,pTreasureDBParameter->szDataBasePass);

		//设置连接
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
			pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

		// 设置连接
		m_UserCustomDBModule->SetConnectionInfo(pUserCustomDBParameter->szDataBaseAddr, pUserCustomDBParameter->wDataBasePort,
			pUserCustomDBParameter->szDataBaseName, pUserCustomDBParameter->szDataBaseUser, pUserCustomDBParameter->szDataBasePass);


		//发起连接
		m_GameDBModule->OpenConnection();
		m_GameDBAide.SetDataBase(m_GameDBModule.GetInterface());

		//发起连接
		m_TreasureDBModule->OpenConnection();
		m_TreasureDBAide.SetDataBase(m_TreasureDBModule.GetInterface());

		//发起连接
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		//发起连接
		m_UserCustomDBModule->OpenConnection();
		m_UserCustomDBAide.SetDataBase(m_UserCustomDBModule.GetInterface());

		//数据钩子
		ASSERT(m_pIGameServiceManager!=NULL);
		m_pIGameDataBaseEngineSink=(IGameDataBaseEngineSink *)m_pIGameServiceManager->CreateGameDataBaseEngineSink(IID_IGameDataBaseEngineSink,VER_IGameDataBaseEngineSink);

		//配置对象
		if ((m_pIGameDataBaseEngineSink!=NULL)&&(m_pIGameDataBaseEngineSink->InitializeSink(QUERY_ME_INTERFACE(IUnknownEx))==false))
		{
			//错误断言
			ASSERT(FALSE);

			//输出信息
			CTraceService::TraceString(TEXT("游戏数据库扩展钩子引擎对象配置失败"),TraceLevel_Exception);

			return false;
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGE("CDataBaseEngineSink::OnDataBaseEngineStart, "<<pIException->GetExceptionDescribe());

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
//	LOGI(_T("CDataBaseEngineSink::OnDataBaseEngineConclude"));
	//配置变量
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件变量
	m_pIGameServiceManager=NULL;
	m_pIDataBaseEngineEvent=NULL;

	//设置对象
	m_GameDBAide.SetDataBase(NULL);

	//释放对象
	SafeRelease(m_pIGameDataBaseEngineSink);

	//关闭连接
	if (m_GameDBModule.GetInterface()!=NULL)
	{
		m_GameDBModule->CloseConnection();
		m_GameDBModule.CloseInstance();
	}

	//关闭连接
	if (m_TreasureDBModule.GetInterface()!=NULL)
	{
		m_TreasureDBModule->CloseConnection();
		m_TreasureDBModule.CloseInstance();
	}

	//关闭连接
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	//关闭连接
	if (m_UserCustomDBModule.GetInterface() != NULL)
	{
		m_UserCustomDBModule->CloseConnection();
		m_UserCustomDBModule.CloseInstance();
	}

	return true;
}

//时间事件
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//控制事件
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
//	LOGI(_T("CDataBaseEngineSink::OnDataBaseEngineRequest, wRequestID = "<< wRequestID));
	//变量定义
	bool bSucceed = false;
	DWORD dwUserID = 0L;

	//请求处理
	switch (wRequestID)
	{
	case DBR_GR_LOGON_USERID:				//I D 登录
		{
			bSucceed = OnRequestLogonUserID(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOGON_MOBILE:				//手机登录
		{
			bSucceed = OnRequestLogonMobile(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOGON_ACCOUNTS:				//帐号登录
		{
			bSucceed = OnRequestLogonAccounts(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOGON_MATCHTIMER:			//定时賽比赛进入
		{
			bSucceed = OnRequestLogonUserID(dwContextID,pData,wDataSize,dwUserID,TRUE);
		}
		break;
	case DBR_GR_WRITE_GAME_SCORE:			//游戏写分
		{
			bSucceed = OnRequestWriteGameScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LEAVE_GAME_SERVER:			//离开房间
		{
			bSucceed = OnRequestLeaveGameServer(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_GAME_SCORE_RECORD:			//游戏记录
		{
			bSucceed = OnRequestGameScoreRecord(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_PARAMETER:				//加载参数
		{
			bSucceed = OnRequestLoadParameter(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_GAME_COLUMN:			//加载列表
		{
			bSucceed = OnRequestLoadGameColumn(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_ANDROID_USER:			//加载机器
		{
			bSucceed = OnRequestLoadAndroidUser(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_GAME_PROPERTY:			//加载道具
		{
			bSucceed = OnRequestLoadGameProperty(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_USER_SAVE_SCORE:			//存入游戏币
		{
			bSucceed = OnRequestUserSaveScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_USER_TAKE_SCORE:			//提取游戏币
		{
			bSucceed = OnRequestUserTakeScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_USER_TRANSFER_SCORE:		//转账游戏币
		{
			bSucceed = OnRequestUserTransferScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_QUERY_INSURE_INFO:			//查询银行
		{
			bSucceed = OnRequestQueryInsureInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
    case DBR_GR_QUERY_TRANSRECORD:			// 查询转账记录
        {
            bSucceed = OnRequestQueryTransRecord(dwContextID,pData,wDataSize,dwUserID);
        }
        break;
	case DBR_GR_QUERY_TRANSFER_USER_INFO:	//查询用户
		{
			bSucceed = OnRequestQueryTransferUserInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_PROPERTY_REQUEST:			//道具请求
		{
			bSucceed = OnRequestPropertyRequest(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MANAGE_USER_RIGHT:			//用户权限
		{
			bSucceed = OnRequestManageUserRight(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_SYSTEM_MESSAGE:		//系统消息
		{
			bSucceed = OnRequestLoadSystemMessage(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_SENSITIVE_WORDS:		//加载敏感词
		{
			return OnRequestLoadSensitiveWords(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_FEE:					//比赛费用
		{
			bSucceed = OnRequestMatchFee(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_START:				//比赛开始
		{
			bSucceed = OnRequestMatchStart(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_OVER:					//比赛结束
		{
			bSucceed = OnRequestMatchOver(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_REWARD:				//比赛奖励
		{
			bSucceed = OnRequestMatchReward(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_QUIT:					//退出比赛
		{
			bSucceed = OnRequestMatchQuit(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_QUERY_NICKNAME_BY_GAMEID:	//根据GameID查询昵称
		{
			bSucceed = OnRequestQueryNickNameByGameID(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_VERIFY_INSURE_PASSWORD:		//验证银行密码
		{
			bSucceed = OnRequestVerifyInsurePassword(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MODIFY_INSURE_PASSWORD:		//更改银行密码
		{
			bSucceed = OnRequestModifyInsurePassword(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
// 	case DBR_GR_LOAD_BALANCE_SCORE_CURVE:
// 		{
// 			bSucceed = OnRequestLoadBalanceScoreCurve(dwContextID, pData, wDataSize, dwUserID);
// 		}
// 		break;
	case DBR_GR_RESET_GAME_SCORE_LOCKER:	//重置游戏锁
		{
			bSucceed = OnRequestResetGameScoreLocker(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_SQUARE_REWARD:				//广场赛奖励
		{
			bSucceed = OnRequestSquareReward(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_LOAD_SQUARE_REWARD:			//加载广场赛奖励配置
		{
			bSucceed = OnRequestLoadSquareReward(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_USER_CREATE_CUSTOMTABLE:		// 用户创建桌子
		{
			bSucceed = OnRequestUserCreateCustomTable(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_USER_JOIN_CUSTOMTABLE:			// 用户加入自定义桌子
		{
			bSucceed = OnRequestUserJoinCustomTable(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_CONCLUDE_CUSTOMTABLE:			// 解散自定义桌子
		{
			bSucceed = OnRequestConcludeCustomTable(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_USER_START_CUSTOMGAME:			// 用户开始游戏,记录到数据库
		{
			bSucceed = OnRequestUserStartCustomGame(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	}

	if (bSucceed == false)
	{
		int j = 0;
	}

	//协调通知
	if(m_pIDBCorrespondManager)
		m_pIDBCorrespondManager->OnPostRequestComplete(dwUserID, bSucceed);

	return bSucceed;
}

//I D 登录
bool CDataBaseEngineSink::OnRequestLogonUserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID,BOOL bMatch)
{
	//执行查询
	DBR_GR_LogonUserID * pLogonUserID=(DBR_GR_LogonUserID *)pData;
	dwUserID = pLogonUserID->dwUserID;

	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_LogonUserID));
	if (wDataSize!=sizeof(DBR_GR_LogonUserID)) return false;

	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonUserID->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLogonUserID->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@strPassword"),pLogonUserID->szPassword);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pLogonUserID->szMachineID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH))
			m_GameDBAide.AddParameter(TEXT("@cbTimeMatch"),bMatch);
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyUserID"),true);

		//用户信息
		lstrcpyn(m_LogonSuccess.szPassword,pLogonUserID->szPassword,CountArray(m_LogonSuccess.szPassword));
		lstrcpyn(m_LogonSuccess.szMachineID,pLogonUserID->szMachineID,CountArray(m_LogonSuccess.szMachineID));
	
		//结果处理
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		// 登录成功，才去读取
		if (lResultCode == DB_SUCCESS)
		{
			// 取消平衡分功能
//			OnUserBalanceScoreDisposeResult(dwContextID, pLogonUserID->dwUserID, true);
			if (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM)
			{
				// 载入玩家之前的比分
				OnUserInCustomTableScoreDisposeResult(dwContextID, pLogonUserID->dwUserID);
			}
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestLogonUserID, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),false);

		return false;
	}

	return true;
}

//I D 登录
bool CDataBaseEngineSink::OnRequestLogonMobile(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//执行查询
	DBR_GR_LogonMobile * pLogonMobile=(DBR_GR_LogonMobile *)pData;
	dwUserID = pLogonMobile->dwUserID;

	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_LogonMobile));
		if (wDataSize!=sizeof(DBR_GR_LogonMobile)) return false;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonMobile->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLogonMobile->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@strPassword"),pLogonMobile->szPassword);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pLogonMobile->szMachineID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyMobile"),true);

		//用户信息
		lstrcpyn(m_LogonSuccess.szPassword,pLogonMobile->szPassword,CountArray(m_LogonSuccess.szPassword));
		lstrcpyn(m_LogonSuccess.szMachineID,pLogonMobile->szMachineID,CountArray(m_LogonSuccess.szMachineID));
		m_LogonSuccess.cbDeviceType=pLogonMobile->cbDeviceType;
		m_LogonSuccess.wBehaviorFlags=pLogonMobile->wBehaviorFlags;
		m_LogonSuccess.wPageTableCount=pLogonMobile->wPageTableCount;
	
		//结果处理
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),true,pLogonMobile->cbDeviceType,pLogonMobile->wBehaviorFlags,pLogonMobile->wPageTableCount);

		// 登录成功，才去读取
		if (lResultCode == DB_SUCCESS)
		{
			if (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM)
			{
				// 载入玩家之前的比分
				OnUserInCustomTableScoreDisposeResult(dwContextID, pLogonMobile->dwUserID);
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestLogonMobile, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),true);

		return false;
	}

	return true;
}

//帐号登录
bool CDataBaseEngineSink::OnRequestLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_LogonAccounts));
		if (wDataSize!=sizeof(DBR_GR_LogonAccounts)) return false;

		//请求处理
		DBR_GR_LogonAccounts * pLogonAccounts=(DBR_GR_LogonAccounts *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@strAccounts"),pLogonAccounts->szAccounts);
		m_GameDBAide.AddParameter(TEXT("@strPassword"),pLogonAccounts->szPassword);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pLogonAccounts->szMachineID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyAccounts"),true);

		//用户信息
		lstrcpyn(m_LogonSuccess.szPassword,pLogonAccounts->szPassword,CountArray(m_LogonSuccess.szPassword));
		lstrcpyn(m_LogonSuccess.szMachineID,pLogonAccounts->szMachineID,CountArray(m_LogonSuccess.szMachineID));

		//结果处理
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestLogonAccounts, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),false);

		return false;
	}

	return true;
}

//游戏写分
bool CDataBaseEngineSink::OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//请求处理
	DBR_GR_WriteGameScore * pWriteGameScore=(DBR_GR_WriteGameScore *)pData;
	dwUserID=pWriteGameScore->dwUserID;

	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_WriteGameScore));
		if (wDataSize!=sizeof(DBR_GR_WriteGameScore)) return false;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pWriteGameScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();

		//用户信息
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pWriteGameScore->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwDBQuestID"),pWriteGameScore->dwDBQuestID);
		m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"),pWriteGameScore->dwInoutIndex);

		//变更游戏币
		m_GameDBAide.AddParameter(TEXT("@lVariationScore"),pWriteGameScore->VariationInfo.lScore);
		m_GameDBAide.AddParameter(TEXT("@lVariationGrade"),pWriteGameScore->VariationInfo.lGrade);
		m_GameDBAide.AddParameter(TEXT("@lVariationInsure"),pWriteGameScore->VariationInfo.lInsure);
		m_GameDBAide.AddParameter(TEXT("@lVariationRevenue"),pWriteGameScore->VariationInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lVariationWinCount"),pWriteGameScore->VariationInfo.dwWinCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationLostCount"),pWriteGameScore->VariationInfo.dwLostCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationDrawCount"),pWriteGameScore->VariationInfo.dwDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationFleeCount"),pWriteGameScore->VariationInfo.dwFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationUserMedal"),pWriteGameScore->VariationInfo.dwUserMedal);
		m_GameDBAide.AddParameter(TEXT("@lVariationExperience"),pWriteGameScore->VariationInfo.dwExperience);
		m_GameDBAide.AddParameter(TEXT("@lVariationLoveLiness"),pWriteGameScore->VariationInfo.lLoveLiness);
		m_GameDBAide.AddParameter(TEXT("@dwVariationPlayTimeCount"),pWriteGameScore->VariationInfo.dwPlayTimeCount);

		// 平衡分
// 		m_GameDBAide.AddParameter(TEXT("@lBalanceScore"),pWriteGameScore->VariationInfo.lBalanceScore);
// 		m_GameDBAide.AddParameter(TEXT("@lBuffTime"),pWriteGameScore->VariationInfo.lBuffTime);
// 		m_GameDBAide.AddParameter(TEXT("@lBuffGames"),pWriteGameScore->VariationInfo.lBuffGames);
// 		m_GameDBAide.AddParameter(TEXT("@lBuffAmount"),pWriteGameScore->VariationInfo.lBuffAmount);
// 		LOGI("CDataBaseEngineSink::OnRequestWriteGameScore, dwUserID="<<dwUserID<<", lBalanceScore="<<pWriteGameScore->VariationInfo.lBalanceScore<< \
// 			", lBuffTime="<<pWriteGameScore->VariationInfo.lBuffTime<<", lBuffGames="<<pWriteGameScore->VariationInfo.lBuffGames<<\
// 			", lBuffAmount="<<pWriteGameScore->VariationInfo.lBuffAmount);

		//属性信息
//		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
//		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
//		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		// 记录桌号，座位号
//		m_GameDBAide.AddParameter(TEXT("@wTableID"), pWriteGameScore->wTableID);
//		m_GameDBAide.AddParameter(TEXT("@wChairID"), pWriteGameScore->wChairID);

		//执行查询
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_WriteGameScore"),false);
		
		// 登录成功，才去读取
//		if (lResultCode == DB_SUCCESS)
//		{
//			OnUserBalanceScoreDisposeResult(dwContextID, pWriteGameScore->dwUserID, false);
//		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestWriteGameScore, "<<pIException->GetExceptionDescribe());

		return false;
	}

	return true;
}

//离开房间
bool CDataBaseEngineSink::OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//请求处理
	DBR_GR_LeaveGameServer * pLeaveGameServer=(DBR_GR_LeaveGameServer *)pData;
	dwUserID=pLeaveGameServer->dwUserID;

	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_LeaveGameServer));
		if (wDataSize!=sizeof(DBR_GR_LeaveGameServer)) return false;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLeaveGameServer->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();

		//用户信息
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLeaveGameServer->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwOnLineTimeCount"),pLeaveGameServer->dwOnLineTimeCount);

		//系统信息
		m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"),pLeaveGameServer->dwInoutIndex);
		m_GameDBAide.AddParameter(TEXT("@dwLeaveReason"),pLeaveGameServer->dwLeaveReason);

		//记录游戏币
		m_GameDBAide.AddParameter(TEXT("@lRecordScore"),pLeaveGameServer->RecordInfo.lScore);
		m_GameDBAide.AddParameter(TEXT("@lRecordGrade"),pLeaveGameServer->RecordInfo.lGrade);
		m_GameDBAide.AddParameter(TEXT("@lRecordInsure"),pLeaveGameServer->RecordInfo.lInsure);
		m_GameDBAide.AddParameter(TEXT("@lRecordRevenue"),pLeaveGameServer->RecordInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lRecordWinCount"),pLeaveGameServer->RecordInfo.dwWinCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordLostCount"),pLeaveGameServer->RecordInfo.dwLostCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordDrawCount"),pLeaveGameServer->RecordInfo.dwDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordFleeCount"),pLeaveGameServer->RecordInfo.dwFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordUserMedal"),pLeaveGameServer->RecordInfo.dwUserMedal);
		m_GameDBAide.AddParameter(TEXT("@lRecordExperience"),pLeaveGameServer->RecordInfo.dwExperience);
		m_GameDBAide.AddParameter(TEXT("@lRecordLoveLiness"),pLeaveGameServer->RecordInfo.lLoveLiness);
		m_GameDBAide.AddParameter(TEXT("@dwRecordPlayTimeCount"),pLeaveGameServer->RecordInfo.dwPlayTimeCount);

		//变更游戏币
		m_GameDBAide.AddParameter(TEXT("@lVariationScore"),pLeaveGameServer->VariationInfo.lScore);
		m_GameDBAide.AddParameter(TEXT("@lVariationGrade"),pLeaveGameServer->VariationInfo.lGrade);
		m_GameDBAide.AddParameter(TEXT("@lVariationInsure"),pLeaveGameServer->VariationInfo.lInsure);
		m_GameDBAide.AddParameter(TEXT("@lVariationRevenue"),pLeaveGameServer->VariationInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lVariationWinCount"),pLeaveGameServer->VariationInfo.dwWinCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationLostCount"),pLeaveGameServer->VariationInfo.dwLostCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationDrawCount"),pLeaveGameServer->VariationInfo.dwDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationFleeCount"),pLeaveGameServer->VariationInfo.dwFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationUserMedal"),pLeaveGameServer->VariationInfo.dwUserMedal);
		m_GameDBAide.AddParameter(TEXT("@lVariationExperience"),pLeaveGameServer->VariationInfo.dwExperience);
		m_GameDBAide.AddParameter(TEXT("@lVariationLoveLiness"),pLeaveGameServer->VariationInfo.lLoveLiness);
		m_GameDBAide.AddParameter(TEXT("@dwVariationPlayTimeCount"),pLeaveGameServer->VariationInfo.dwPlayTimeCount);

		//其他参数
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pLeaveGameServer->szMachineID);

		//执行查询
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LeaveGameServer"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestLeaveGameServer, "<<pIException->GetExceptionDescribe());

		return false;
	}

	return true;
}

//游戏记录
bool CDataBaseEngineSink::OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//变量定义
		DBR_GR_GameScoreRecord * pGameScoreRecord=(DBR_GR_GameScoreRecord *)pData;
		dwUserID=INVALID_DWORD;

		//效验参数
		ASSERT(wDataSize<=sizeof(DBR_GR_GameScoreRecord));
		ASSERT(wDataSize>=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)));
		ASSERT(wDataSize==(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0])));

		//效验参数
		if (wDataSize>sizeof(DBR_GR_GameScoreRecord)) return false;
		if (wDataSize<(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord))) return false;
		if (wDataSize!=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0]))) return false;

		//房间信息
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//桌子信息
		m_GameDBAide.AddParameter(TEXT("@wTableID"),pGameScoreRecord->wTableID);
		m_GameDBAide.AddParameter(TEXT("@wUserCount"),pGameScoreRecord->wUserCount);
		m_GameDBAide.AddParameter(TEXT("@wAndroidCount"),pGameScoreRecord->wAndroidCount);

		//税收损耗
		m_GameDBAide.AddParameter(TEXT("@lWasteCount"),pGameScoreRecord->lWasteCount);
		m_GameDBAide.AddParameter(TEXT("@lRevenueCount"),pGameScoreRecord->lRevenueCount);

		//统计信息
		m_GameDBAide.AddParameter(TEXT("@dwUserMemal"),pGameScoreRecord->dwUserMemal);
		m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pGameScoreRecord->dwPlayTimeCount);

		//时间信息
		m_GameDBAide.AddParameter(TEXT("@SystemTimeStart"),pGameScoreRecord->SystemTimeStart);
		m_GameDBAide.AddParameter(TEXT("@SystemTimeConclude"),pGameScoreRecord->SystemTimeConclude);

		//执行查询
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordDrawInfo"),true);

		//写入记录
		DWORD dwDrawID = INVALID_DWORD;
		if (lResultCode==DB_SUCCESS)
		{
			//获取标识
			dwDrawID = m_GameDBAide.GetValue_DWORD(TEXT("DrawID"));

			//写入记录
			for (WORD i=0;i<pGameScoreRecord->wRecordCount;i++)
			{
				//重置参数
				m_GameDBAide.ResetParameter();
				
				//房间信息
				m_GameDBAide.AddParameter(TEXT("@dwDrawID"),dwDrawID);
				m_GameDBAide.AddParameter(TEXT("@dwUserID"),pGameScoreRecord->GameScoreRecord[i].dwUserID);
				m_GameDBAide.AddParameter(TEXT("@wChairID"),pGameScoreRecord->GameScoreRecord[i].wChairID);

				//用户信息
				m_GameDBAide.AddParameter(TEXT("@dwDBQuestID"),pGameScoreRecord->GameScoreRecord[i].dwDBQuestID);
				m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"),pGameScoreRecord->GameScoreRecord[i].dwInoutIndex);

				//游戏币信息
				m_GameDBAide.AddParameter(TEXT("@lScore"),pGameScoreRecord->GameScoreRecord[i].lScore);
				m_GameDBAide.AddParameter(TEXT("@lGrade"),pGameScoreRecord->GameScoreRecord[i].lGrade);
				m_GameDBAide.AddParameter(TEXT("@lRevenue"),pGameScoreRecord->GameScoreRecord[i].lRevenue);
				m_GameDBAide.AddParameter(TEXT("@dwUserMedal"),pGameScoreRecord->GameScoreRecord[i].dwUserMemal);
				m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pGameScoreRecord->GameScoreRecord[i].dwPlayTimeCount);

				//执行查询
				m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordDrawScore"),false);
			}
		}

		//记录捕鱼游戏操作
		if (m_pGameServiceOption->wKindID == 7009)
		{
			//房间信息
			m_TreasureDBAide.ResetParameter();
			m_TreasureDBAide.AddParameter(TEXT("@dwDrawID"), dwDrawID);
			m_TreasureDBAide.AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
			m_TreasureDBAide.AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);

			//桌子信息
			m_TreasureDBAide.AddParameter(TEXT("@wTableID"), pGameScoreRecord->wTableID);
			m_TreasureDBAide.AddParameter(TEXT("@wUserID"), pGameScoreRecord->GameScoreRecord[0].dwUserID);
			m_TreasureDBAide.AddParameter(TEXT("@wChairID"), pGameScoreRecord->GameScoreRecord[0].wChairID);

			//游戏信息
			m_TreasureDBAide.AddParameter(TEXT("@lScore"), pGameScoreRecord->GameScoreRecord[0].lScore);

			//执行查询
			m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_RecordFishDrawInfo"), false);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestGameScoreRecord, "<<pIException->GetExceptionDescribe());

		return false;
	}

	return true;
}

//加载参数
bool CDataBaseEngineSink::OnRequestLoadParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//变量定义
		DBO_GR_GameParameter GameParameter;
		ZeroMemory(&GameParameter,sizeof(GameParameter));

		//执行查询
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_PlatformDBAide.AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);

		//执行查询
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadParameter"), true);

		//读取信息
		if (lResultCode==DB_SUCCESS)
		{
			//汇率信息
			GameParameter.wMedalRate = m_PlatformDBAide.GetValue_WORD(TEXT("MedalRate"));
			GameParameter.wRevenueRate = m_PlatformDBAide.GetValue_WORD(TEXT("RevenueRate"));

			//版本信息
			GameParameter.dwClientVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("ClientVersion"));
//			GameParameter.dwAppClientVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("AppClientVersion"));
			GameParameter.dwServerVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("ServerVersion"));
		
			//发送信息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PARAMETER,dwContextID,&GameParameter,sizeof(GameParameter));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestLoadParameter, "<<pIException->GetExceptionDescribe());

		return false;
	}

	return true;
}

//加载列表
bool CDataBaseEngineSink::OnRequestLoadGameColumn(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//变量定义
		DBO_GR_GameColumnInfo GameColumnInfo;
		ZeroMemory(&GameColumnInfo,sizeof(GameColumnInfo));

		//执行查询
		m_GameDBAide.ResetParameter();
		GameColumnInfo.lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_GameColumnItem"),true);

		//读取信息
		for (WORD i=0;i<CountArray(GameColumnInfo.ColumnItemInfo);i++)
		{
			//结束判断
			if (m_GameDBModule->IsRecordsetEnd()==true) break;

			//溢出效验
			ASSERT(GameColumnInfo.cbColumnCount<CountArray(GameColumnInfo.ColumnItemInfo));
			if (GameColumnInfo.cbColumnCount>=CountArray(GameColumnInfo.ColumnItemInfo)) break;

			//读取数据
			GameColumnInfo.cbColumnCount++;
			GameColumnInfo.ColumnItemInfo[i].cbColumnWidth=m_GameDBAide.GetValue_BYTE(TEXT("ColumnWidth"));
			GameColumnInfo.ColumnItemInfo[i].cbDataDescribe=m_GameDBAide.GetValue_BYTE(TEXT("DataDescribe"));
			m_GameDBAide.GetValue_String(TEXT("ColumnName"),GameColumnInfo.ColumnItemInfo[i].szColumnName,CountArray(GameColumnInfo.ColumnItemInfo[i].szColumnName));

			//移动记录
			m_GameDBModule->MoveToNext();
		}

		//发送信息
		WORD wHeadSize=sizeof(GameColumnInfo)-sizeof(GameColumnInfo.ColumnItemInfo);
		WORD wPacketSize=wHeadSize+GameColumnInfo.cbColumnCount*sizeof(GameColumnInfo.ColumnItemInfo[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_COLUMN_INFO,dwContextID,&GameColumnInfo,wPacketSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestLoadGameColumn, "<<pIException->GetExceptionDescribe());

		//变量定义
		DBO_GR_GameColumnInfo GameColumnInfo;
		ZeroMemory(&GameColumnInfo,sizeof(GameColumnInfo));

		//构造变量
		GameColumnInfo.cbColumnCount=0L;
		GameColumnInfo.lResultCode=pIException->GetExceptionResult();

		//发送信息
		WORD wPacketSize=sizeof(GameColumnInfo)-sizeof(GameColumnInfo.ColumnItemInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_COLUMN_INFO,dwContextID,&GameColumnInfo,wPacketSize);

		return false;
	}

	return true;
}

//加载机器
bool CDataBaseEngineSink::OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//变量定义
		DBO_GR_GameAndroidInfo GameAndroidInfo;
		ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

		//用户帐户
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceAttrib->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//执行查询
		GameAndroidInfo.lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadAndroidUser"),true);

		//读取信息
		for (WORD i=0;i<MAX_ANDROID;i++)
		{
			//结束判断
			if (m_GameDBModule->IsRecordsetEnd()==true)
				break;

			//溢出效验
//			ASSERT(GameAndroidInfo.wAndroidCount<CountArray(GameAndroidInfo.AndroidParameter));
//			if (GameAndroidInfo.wAndroidCount>=CountArray(GameAndroidInfo.AndroidParameter)) break;

			//读取数据
//			GameAndroidInfo.wAndroidCount++;
// 			GameAndroidInfo.AndroidParameter[i].dwUserID=m_GameDBAide.GetValue_DWORD(TEXT("UserID"));
// 			GameAndroidInfo.AndroidParameter[i].dwServiceTime=m_GameDBAide.GetValue_DWORD(TEXT("ServiceTime"));
// 			GameAndroidInfo.AndroidParameter[i].dwMinPlayDraw=m_GameDBAide.GetValue_DWORD(TEXT("MinPlayDraw"));
// 			GameAndroidInfo.AndroidParameter[i].dwMaxPlayDraw=m_GameDBAide.GetValue_DWORD(TEXT("MaxPlayDraw"));
// 			GameAndroidInfo.AndroidParameter[i].dwMinReposeTime=m_GameDBAide.GetValue_DWORD(TEXT("MinReposeTime"));
// 			GameAndroidInfo.AndroidParameter[i].dwMaxReposeTime=m_GameDBAide.GetValue_DWORD(TEXT("MaxReposeTime"));
// 			GameAndroidInfo.AndroidParameter[i].dwServiceGender=m_GameDBAide.GetValue_DWORD(TEXT("ServiceGender"));
// 			GameAndroidInfo.AndroidParameter[i].lMinTakeScore=m_GameDBAide.GetValue_LONGLONG(TEXT("MinTakeScore"));
// 			GameAndroidInfo.AndroidParameter[i].lMaxTakeScore=m_GameDBAide.GetValue_LONGLONG(TEXT("MaxTakeScore"));
// 
// 			GameAndroidInfo.AndroidParameter[i].dwMinSitRate=m_GameDBAide.GetValue_DWORD(TEXT("MinSitRate"));
// 			GameAndroidInfo.AndroidParameter[i].dwMaxSitRate=m_GameDBAide.GetValue_DWORD(TEXT("MaxSitRate"));
// 			GameAndroidInfo.AndroidParameter[i].dwSitRate=m_GameDBAide.GetValue_DWORD(TEXT("SitRate"));
// 
// 			LOGI("CDataBaseEngineSink::OnRequestLoadAndroidUser, dwUserID="<<GameAndroidInfo.AndroidParameter[i].dwUserID<<
// 				", dwServiceGender="<<GameAndroidInfo.AndroidParameter[i].dwServiceGender);

			GameAndroidInfo.AndroidParameter.dwUserID=m_GameDBAide.GetValue_DWORD(TEXT("UserID"));
			GameAndroidInfo.AndroidParameter.dwServiceTime=m_GameDBAide.GetValue_DWORD(TEXT("ServiceTime"));
			GameAndroidInfo.AndroidParameter.dwMinPlayDraw=m_GameDBAide.GetValue_DWORD(TEXT("MinPlayDraw"));
			GameAndroidInfo.AndroidParameter.dwMaxPlayDraw=m_GameDBAide.GetValue_DWORD(TEXT("MaxPlayDraw"));
			GameAndroidInfo.AndroidParameter.dwMinReposeTime=m_GameDBAide.GetValue_DWORD(TEXT("MinReposeTime"));
			GameAndroidInfo.AndroidParameter.dwMaxReposeTime=m_GameDBAide.GetValue_DWORD(TEXT("MaxReposeTime"));
			GameAndroidInfo.AndroidParameter.dwServiceGender=m_GameDBAide.GetValue_DWORD(TEXT("ServiceGender"));
			GameAndroidInfo.AndroidParameter.lMinTakeScore=m_GameDBAide.GetValue_LONGLONG(TEXT("MinTakeScore"));
			GameAndroidInfo.AndroidParameter.lMaxTakeScore=m_GameDBAide.GetValue_LONGLONG(TEXT("MaxTakeScore"));

			GameAndroidInfo.AndroidParameter.dwMinSitRate=m_GameDBAide.GetValue_DWORD(TEXT("MinSitRate"));
			GameAndroidInfo.AndroidParameter.dwMaxSitRate=m_GameDBAide.GetValue_DWORD(TEXT("MaxSitRate"));
			GameAndroidInfo.AndroidParameter.dwSitRate=m_GameDBAide.GetValue_DWORD(TEXT("SitRate"));

			LOGI("CDataBaseEngineSink::OnRequestLoadAndroidUser, dwUserID="<<GameAndroidInfo.AndroidParameter.dwUserID<<
				", dwServiceGender="<<GameAndroidInfo.AndroidParameter.dwServiceGender);

			// 发送消息
			if (i == 0)
			{
				// 第一次发送先重置数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_GAME_ANDROID_INFO,0xFFFF,&GameAndroidInfo,sizeof(GameAndroidInfo));
			}
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_GAME_ANDROID_INFO,dwContextID,&GameAndroidInfo,sizeof(GameAndroidInfo));

			//移动记录
			m_GameDBModule->MoveToNext();
		}

		//发送信息
//		WORD wHeadSize=sizeof(GameAndroidInfo)-sizeof(GameAndroidInfo.AndroidParameter);
//		WORD wDataSize=GameAndroidInfo.wAndroidCount*sizeof(GameAndroidInfo.AndroidParameter[0]);

#ifdef _DEBUG
		HANDLE handle = GetCurrentProcess();
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
		LOGD("CDataBaseEngineSink::OnRequestLoadAndroidUser, WorkingSetSize = " << (pmc.WorkingSetSize / 1000));
#endif

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestLoadAndroidUser, "<<pIException->GetExceptionDescribe());

		//变量定义
		DBO_GR_GameAndroidInfo GameAndroidInfo;
		ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

		//构造变量
//		GameAndroidInfo.wAndroidCount=0L;
		GameAndroidInfo.lResultCode=DB_ERROR;

		//发送信息
		WORD wHeadSize=sizeof(GameAndroidInfo)-sizeof(GameAndroidInfo.AndroidParameter);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_GAME_ANDROID_INFO,dwContextID,&GameAndroidInfo,wHeadSize);
	}

	return false;
}

//加载道具
bool CDataBaseEngineSink::OnRequestLoadGameProperty(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//变量定义
		DBO_GR_GamePropertyInfo GamePropertyInfo;
		ZeroMemory(&GamePropertyInfo,sizeof(GamePropertyInfo));

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//执行查询
		GamePropertyInfo.lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadGameProperty"),true);

		//读取信息
		for (WORD i=0;i<CountArray(GamePropertyInfo.PropertyInfo);i++)
		{
			//结束判断
			if (m_GameDBModule->IsRecordsetEnd()==true) break;

			//溢出效验
			ASSERT(GamePropertyInfo.cbPropertyCount<CountArray(GamePropertyInfo.PropertyInfo));
			if (GamePropertyInfo.cbPropertyCount>=CountArray(GamePropertyInfo.PropertyInfo)) break;

			//读取数据
			GamePropertyInfo.cbPropertyCount++;
			GamePropertyInfo.PropertyInfo[i].wIndex=m_GameDBAide.GetValue_WORD(TEXT("ID"));
			GamePropertyInfo.PropertyInfo[i].wDiscount=m_GameDBAide.GetValue_WORD(TEXT("Discount"));
			GamePropertyInfo.PropertyInfo[i].wIssueArea=m_GameDBAide.GetValue_WORD(TEXT("IssueArea"));
			GamePropertyInfo.PropertyInfo[i].dPropertyCash=m_GameDBAide.GetValue_DOUBLE(TEXT("Cash"));
			GamePropertyInfo.PropertyInfo[i].lPropertyGold=m_GameDBAide.GetValue_LONGLONG(TEXT("Gold"));
			GamePropertyInfo.PropertyInfo[i].lSendLoveLiness=m_GameDBAide.GetValue_LONGLONG(TEXT("SendLoveLiness"));
			GamePropertyInfo.PropertyInfo[i].lRecvLoveLiness=m_GameDBAide.GetValue_LONGLONG(TEXT("RecvLoveLiness"));

			//移动记录
			m_GameDBModule->MoveToNext();
		}

		//发送信息
		WORD wHeadSize=sizeof(GamePropertyInfo)-sizeof(GamePropertyInfo.PropertyInfo);
		WORD wDataSize=GamePropertyInfo.cbPropertyCount*sizeof(GamePropertyInfo.PropertyInfo[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PROPERTY_INFO,dwContextID,&GamePropertyInfo,wHeadSize+wDataSize);
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestLoadGameProperty, "<<pIException->GetExceptionDescribe());

		//变量定义
		DBO_GR_GamePropertyInfo GamePropertyInfo;
		ZeroMemory(&GamePropertyInfo,sizeof(GamePropertyInfo));

		//构造变量
		GamePropertyInfo.cbPropertyCount=0L;
		GamePropertyInfo.lResultCode=DB_ERROR;

		//发送信息
		WORD wHeadSize=sizeof(GamePropertyInfo)-sizeof(GamePropertyInfo.PropertyInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PROPERTY_INFO,dwContextID,&GamePropertyInfo,wHeadSize);

		return false;
	}

	return true;
}

//存入游戏币
bool CDataBaseEngineSink::OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
//	LOGI(_T("CDataBaseEngineSink::OnRequestUserSaveScore"));
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_UserSaveScore));
	if (wDataSize!=sizeof(DBR_GR_UserSaveScore)) return false;

	//变量定义
	DBR_GR_UserSaveScore * pUserSaveScore=(DBR_GR_UserSaveScore *)pData;
	dwUserID=pUserSaveScore->dwUserID;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserSaveScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserSaveScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lSaveScore"),pUserSaveScore->lSaveScore);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserSaveScore->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserSaveScore"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,pUserSaveScore->lSaveScore,CW2CT(DBVarValue.bstrVal),false,pUserSaveScore->cbActivityGame);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestUserSaveScore, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,pUserSaveScore->lSaveScore,TEXT("由于数据库操作异常，请您稍后重试！"),false,pUserSaveScore->cbActivityGame);

		return false;
	}

	return true;
}

//提取游戏币
bool CDataBaseEngineSink::OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_UserTakeScore));
	if (wDataSize!=sizeof(DBR_GR_UserTakeScore)) return false;

	//变量定义
	DBR_GR_UserTakeScore * pUserTakeScore=(DBR_GR_UserTakeScore *)pData;
	dwUserID=pUserTakeScore->dwUserID;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTakeScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserTakeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lTakeScore"),pUserTakeScore->lTakeScore);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pUserTakeScore->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserTakeScore->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserTakeScore"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,0L,CW2CT(DBVarValue.bstrVal),false,pUserTakeScore->cbActivityGame);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestUserTakeScore, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false,pUserTakeScore->cbActivityGame);

		return false;
	}

	return true;
}

//转账游戏币
bool CDataBaseEngineSink::OnRequestUserTransferScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
//	LOGI(_T("CDataBaseEngineSink::OnRequestUserTransferScore"));
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_UserTransferScore));
	if (wDataSize!=sizeof(DBR_GR_UserTransferScore)) return false;

	//变量定义
	DBR_GR_UserTransferScore * pUserTransferScore=(DBR_GR_UserTransferScore *)pData;
	dwUserID=pUserTransferScore->dwUserID;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTransferScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserTransferScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@cbByNickName"),pUserTransferScore->cbByNickName);
		m_TreasureDBAide.AddParameter(TEXT("@lTransferScore"),pUserTransferScore->lTransferScore);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pUserTransferScore->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@strNickName"),pUserTransferScore->szNickName);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserTransferScore->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserTransferScore"),true);

		if (DB_SUCCESS == lResultCode)
		{
			CMD_GR_UserTransferReceipt UserTransferReceipt;
			// 转账成功，发送回执
			UserTransferReceipt.dwRecordID = m_TreasureDBAide.GetValue_DWORD(TEXT("RecordID"));		
			UserTransferReceipt.dwSourceUserID = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));				// 发起转账的玩家
			UserTransferReceipt.dwTargetUserID = m_TreasureDBAide.GetValue_DWORD(TEXT("TargetUserID"));			// 接受转账的玩家
			UserTransferReceipt.dwSourceGameID = m_TreasureDBAide.GetValue_DWORD(TEXT("SourceGameID"));			// 发起转账的GameID
			UserTransferReceipt.dwTargetGameID = m_TreasureDBAide.GetValue_DWORD(TEXT("TargetGameID"));			// 接受转账的GameID

			m_TreasureDBAide.GetValue_String(TEXT("SourceNickName"), UserTransferReceipt.szSourceNickName, CountArray(UserTransferReceipt.szSourceNickName) );
			m_TreasureDBAide.GetValue_String(TEXT("TargetNickName"), UserTransferReceipt.szTargetNickName, CountArray(UserTransferReceipt.szTargetNickName));

			UserTransferReceipt.lTransferScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("lTransferScore"));					// 转账金额
			UserTransferReceipt.lRevenueScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("InsureRevenue"));						// 税收金额

			UserTransferReceipt.lSourceUserScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("SourceScore"))+ m_TreasureDBAide.GetValue_LONGLONG(TEXT("VariationScore"));
			UserTransferReceipt.lSourceUserInsure = m_TreasureDBAide.GetValue_LONGLONG(TEXT("SourceInsure"))+ m_TreasureDBAide.GetValue_LONGLONG(TEXT("VariationInsure"));

			m_TreasureDBAide.GetValue_SystemTime(TEXT("stTransferTime"), UserTransferReceipt.stTransferTime);					// 转账时间

			LOGI("CDataBaseEngineSink::OnRequestUserTransferScore, dwRecordID="<<UserTransferReceipt.dwRecordID<<", dwSourceUserID="<<UserTransferReceipt.dwSourceUserID<<\
				", dwTargetUserID="<<UserTransferReceipt.dwTargetUserID<<", dwSourceGameID="<<UserTransferReceipt.dwSourceGameID<<", dwTargetGameID="<<UserTransferReceipt.dwTargetGameID<<\
				", szSourceNickName="<<UserTransferReceipt.szSourceNickName<<", szTargetNickName="<<UserTransferReceipt.szTargetNickName<<\
				", lTransferScore="<<UserTransferReceipt.lTransferScore<<", lRevenueScore="<<UserTransferReceipt.lRevenueScore);

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INSURE_TRANS_RECEIPT, dwContextID, &UserTransferReceipt, sizeof(UserTransferReceipt) );
		}
		else
		{
			//结果处理
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			OnInsureDisposeResult(dwContextID,lResultCode,0L,CW2CT(DBVarValue.bstrVal),false,pUserTransferScore->cbActivityGame);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestUserTransferScore, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false,pUserTransferScore->cbActivityGame);

		return false;
	}

	return true;
}

//查询银行
bool CDataBaseEngineSink::OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_QueryInsureInfo));
	if (wDataSize!=sizeof(DBR_GR_QueryInsureInfo)) return false;

	//请求处理
	DBR_GR_QueryInsureInfo * pQueryInsureInfo=(DBR_GR_QueryInsureInfo *)pData;
	dwUserID=pQueryInsureInfo->dwUserID;

	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryInsureInfo->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pQueryInsureInfo->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryUserInsureInfo"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GR_UserInsureInfo UserInsureInfo;
			ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

			//银行信息
			UserInsureInfo.cbActivityGame=pQueryInsureInfo->cbActivityGame;
			UserInsureInfo.wRevenueTake=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTake"));
			UserInsureInfo.wRevenueTransfer=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTransfer"));
			UserInsureInfo.wServerID=m_TreasureDBAide.GetValue_WORD(TEXT("ServerID"));
			UserInsureInfo.lUserScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Score"));
			UserInsureInfo.lUserInsure=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Insure"));
			UserInsureInfo.lTransferPrerequisite=m_TreasureDBAide.GetValue_LONGLONG(TEXT("TransferPrerequisite"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_INFO,dwContextID,&UserInsureInfo,sizeof(UserInsureInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//错误处理
			OnInsureDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),0L,CW2CT(DBVarValue.bstrVal),false,pQueryInsureInfo->cbActivityGame);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestQueryInsureInfo, "<<pIException->GetExceptionDescribe());

		//结果处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false,pQueryInsureInfo->cbActivityGame);

		return false;
	}

	return true;
}
//转账记录
bool CDataBaseEngineSink::OnRequestQueryTransRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
    //效验参数
    ASSERT(wDataSize==sizeof(DBR_GR_QueryInsureInfo));
    if (wDataSize!=sizeof(DBR_GR_QueryInsureInfo)) return false;

    //请求处理
    DBR_GR_QueryInsureInfo * pQueryInsureInfo=(DBR_GR_QueryInsureInfo *)pData;
    dwUserID=pQueryInsureInfo->dwUserID;

    try
    {
        //转化地址
		TCHAR szClientAddr[16]=TEXT("");
//		BYTE * pClientAddr=(BYTE *)&pQueryInsureInfo->dwClientAddr;
//		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

        //构造参数
        m_TreasureDBAide.ResetParameter();
        m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pQueryInsureInfo->dwUserID);
        //m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

        //输出参数
        TCHAR szDescribeString[128]=TEXT("");
//		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

        //结果处理
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTransferRecord"),true)==DB_SUCCESS)
		{
			DBR_GR_UserTransRecord UserTransRecord;
			ZeroMemory(&UserTransRecord, sizeof(UserTransRecord));
            INT nRecordCount=m_TreasureDBModule->GetRecordCount();
			//读取记录
			if(nRecordCount > 0)
			{
				m_TreasureDBModule->MoveToFirst();
                DWORD dw=1;
				while ( m_TreasureDBModule->IsRecordsetEnd() == false )
				{
//					UserTransRecord.dwGameID=dw++;
					UserTransRecord.bOver=false;
					m_TreasureDBAide.GetValue_String(TEXT("SourceNickName"),UserTransRecord.szSourceAccount,CountArray(UserTransRecord.szSourceAccount));
					m_TreasureDBAide.GetValue_String(TEXT("TargetNickName"),UserTransRecord.szTargetAccounts,CountArray(UserTransRecord.szTargetAccounts));
					UserTransRecord.dwGameID = m_TreasureDBAide.GetValue_DWORD(TEXT("RecordID"));
					UserTransRecord.lSwapScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("SwapScore"));
					UserTransRecord.lRevenue= m_TreasureDBAide.GetValue_LONGLONG(TEXT("Revenue"));
					UserTransRecord.dwTargetID=m_TreasureDBAide.GetValue_DWORD(TEXT("TargetGameID"));
					CDBVarValue varvalue;
					m_TreasureDBAide.GetValue_VarValue(TEXT("CollectDate"),varvalue);

					COleDateTime time(varvalue);

					CString GetTime;
					GetTime.Format(_T("%d年%d月%d日%d时%d分%d秒"),time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(), time.GetSecond());
					lstrcpyn(UserTransRecord.szTime,GetTime,sizeof(UserTransRecord.szTime));

					//投递调度通知
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_TRANS_RECORD,dwContextID, &UserTransRecord, sizeof(UserTransRecord));

					//移动记录
					m_TreasureDBModule->MoveToNext();
				}
				UserTransRecord.bOver=true;
				//发送结果
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_TRANS_RECORD,dwContextID, &UserTransRecord, sizeof(UserTransRecord));
			}
			else
			{
                UserTransRecord.bOver=true;
				//发送结果
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_TRANS_RECORD,dwContextID, &UserTransRecord, sizeof(UserTransRecord));
			}
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//结果处理
			OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false,pQueryInsureInfo->cbActivityGame);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestQueryTransRecord, "<<pIException->GetExceptionDescribe());

		//结果处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false,pQueryInsureInfo->cbActivityGame);


		return false;
	}
    return true;
}
//查询用户
bool CDataBaseEngineSink::OnRequestQueryTransferUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_QueryTransferUserInfo));
	if (wDataSize!=sizeof(DBR_GR_QueryTransferUserInfo)) return false;

	//请求处理
	DBR_GR_QueryTransferUserInfo * pQueryTransferUserInfo=(DBR_GR_QueryTransferUserInfo *)pData;
	dwUserID=pQueryTransferUserInfo->dwUserID;

	try
	{
		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@cbByNickName"),pQueryTransferUserInfo->cbByNickName);
		m_TreasureDBAide.AddParameter(TEXT("@strNickName"),pQueryTransferUserInfo->szNickName);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTransferUserInfo"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GR_UserTransferUserInfo TransferUserInfo;
			ZeroMemory(&TransferUserInfo,sizeof(TransferUserInfo));

			//银行信息
			TransferUserInfo.cbActivityGame=pQueryTransferUserInfo->cbActivityGame;
			TransferUserInfo.dwGameID=m_TreasureDBAide.GetValue_DWORD(TEXT("GameID"));
			m_TreasureDBAide.GetValue_String(TEXT("NickName"), TransferUserInfo.szNickName, CountArray(TransferUserInfo.szNickName));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_USER_INFO,dwContextID,&TransferUserInfo,sizeof(TransferUserInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//错误处理
			OnInsureDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),0L,CW2CT(DBVarValue.bstrVal),false,pQueryTransferUserInfo->cbActivityGame);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestQueryTransferUserInfo, "<<pIException->GetExceptionDescribe());

		//结果处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false,pQueryTransferUserInfo->cbActivityGame);

		return false;
	}

	return true;
}

// 根据GameID查询昵称
bool CDataBaseEngineSink::OnRequestQueryNickNameByGameID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
//	LOGI("CDataBaseEngineSink::OnRequestQueryNickNameByGameID");
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_QueryNickNameByGameID));
	if (wDataSize!=sizeof(DBR_GR_QueryNickNameByGameID)) return false;

	//请求处理
	DBR_GR_QueryNickNameByGameID* pQueryNickNameByGameID=(DBR_GR_QueryNickNameByGameID *)pData;
	dwUserID = pQueryNickNameByGameID->dwUserID;
	try
	{
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwGameID"),pQueryNickNameByGameID->dwGameID);

		m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryUserNickNameByGameID"),true);

		//变量定义
		DBO_GR_UserTransferUserInfo TransferUserInfo;
		ZeroMemory(&TransferUserInfo,sizeof(TransferUserInfo));

		//银行信息
		TransferUserInfo.dwGameID=pQueryNickNameByGameID->dwGameID;
		m_TreasureDBAide.GetValue_String(TEXT("NickName"), TransferUserInfo.szNickName, CountArray(TransferUserInfo.szNickName));

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_USER_INFO,dwContextID,&TransferUserInfo,sizeof(TransferUserInfo));

//		LOGI(_T("CDataBaseEngineSink::OnRequestQueryNickNameByGameID return 1"));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		CString cstr;
		cstr.Format(_T("CDataBaseEngineSink::OnRequestQueryNickNameByGameID, dwGameID = %d, exception = %s"), pQueryNickNameByGameID->dwGameID, pIException->GetExceptionDescribe());
		//输出错误
		CTraceService::TraceString(cstr, TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestQueryNickNameByGameID, "<<cstr);

		//结果处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);
//		LOGI(_T("CDataBaseEngineSink::OnRequestQueryNickNameByGameID return 2"));
		return false;
	}

//	LOGI(_T("CDataBaseEngineSink::OnRequestQueryNickNameByGameID return 3"));
	return true;
}

// 验证银行密码
bool CDataBaseEngineSink::OnRequestVerifyInsurePassword(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
//	LOGI(_T("CDataBaseEngineSink::OnRequestVerifyInsurePassword"));
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_VerifyInsurePassword));
	if (wDataSize!=sizeof(DBR_GR_VerifyInsurePassword)) return false;

	//请求处理
	DBR_GR_VerifyInsurePassword * pUserTransferScore=(DBR_GR_VerifyInsurePassword *)pData;
	dwUserID = pUserTransferScore->dwUserID;
	try
	{
		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserTransferScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strInsurePassword"),pUserTransferScore->szInsurePassword);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_VerifyInsurePassword"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		OnInsureDisposeResult(dwContextID, lResultCode, 0L, CW2CT(DBVarValue.bstrVal), false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestVerifyInsurePassword, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

// 更改银行密码
bool CDataBaseEngineSink::OnRequestModifyInsurePassword(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_ModifyInsurePass));
	if (wDataSize!=sizeof(DBR_GR_ModifyInsurePass)) return false;

	//请求处理
	DBR_GR_ModifyInsurePass * pModifyInsurePass=(DBR_GR_ModifyInsurePass *)pData;
	dwUserID = pModifyInsurePass->dwUserID;

	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyInsurePass->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pModifyInsurePass->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strScrPassword"),pModifyInsurePass->szScrPassword);
		m_TreasureDBAide.AddParameter(TEXT("@strNewPassword"),pModifyInsurePass->szDesPassword);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//结果处理
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_ModifyInsurePassword"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		OnOperateDisposeResult(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal), false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestModifyInsurePassword, "<<pIException->GetExceptionDescribe());

		//结果处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//道具请求
bool CDataBaseEngineSink::OnRequestPropertyRequest(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_PropertyRequest));
		if (wDataSize!=sizeof(DBR_GR_PropertyRequest)) return false;

		//请求处理
		DBR_GR_PropertyRequest * pPropertyRequest=(DBR_GR_PropertyRequest *)pData;
		dwUserID=pPropertyRequest->dwSourceUserID;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pPropertyRequest->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();

		//消费信息
		m_GameDBAide.AddParameter(TEXT("@dwSourceUserID"),pPropertyRequest->dwSourceUserID);
		m_GameDBAide.AddParameter(TEXT("@dwTargetUserID"),pPropertyRequest->dwTargetUserID);
		m_GameDBAide.AddParameter(TEXT("@wPropertyID"),pPropertyRequest->wPropertyIndex);
		m_GameDBAide.AddParameter(TEXT("@wPropertyCount"),pPropertyRequest->wItemCount);

		//消费区域
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@wTableID"),pPropertyRequest->wTableID);

		//购买方式
		m_GameDBAide.AddParameter(TEXT("@cbConsumeScore"),pPropertyRequest->cbConsumeScore);
		m_GameDBAide.AddParameter(TEXT("@lFrozenedScore"),pPropertyRequest->lFrozenedScore);

		//系统信息
		m_GameDBAide.AddParameter(TEXT("@dwEnterID"),pPropertyRequest->dwInoutIndex);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pPropertyRequest->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_ConsumeProperty"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GR_S_PropertySuccess PresentSuccess;
			ZeroMemory(&PresentSuccess,sizeof(PresentSuccess));

			//道具信息
			PresentSuccess.cbRequestArea=pPropertyRequest->cbRequestArea;
			PresentSuccess.wItemCount=pPropertyRequest->wItemCount;
			PresentSuccess.wPropertyIndex=pPropertyRequest->wPropertyIndex;
			PresentSuccess.dwSourceUserID=pPropertyRequest->dwSourceUserID;
			PresentSuccess.dwTargetUserID=pPropertyRequest->dwTargetUserID;

			//消费模式
			PresentSuccess.cbConsumeScore=pPropertyRequest->cbConsumeScore;
			PresentSuccess.lFrozenedScore=pPropertyRequest->lFrozenedScore;

			//用户权限
			PresentSuccess.dwUserRight=pPropertyRequest->dwUserRight;

			//结果信息
			PresentSuccess.lConsumeGold=m_GameDBAide.GetValue_LONGLONG(TEXT("ConsumeGold"));
			PresentSuccess.lSendLoveLiness=m_GameDBAide.GetValue_LONG(TEXT("SendLoveLiness"));
			PresentSuccess.lRecvLoveLiness=m_GameDBAide.GetValue_LONG(TEXT("RecvLoveLiness"));
			PresentSuccess.cbMemberOrder=m_GameDBAide.GetValue_BYTE(TEXT("MemberOrder"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PROPERTY_SUCCESS,dwContextID,&PresentSuccess,sizeof(PresentSuccess));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//变量定义
			DBO_GR_PropertyFailure PropertyFailure;
			ZeroMemory(&PropertyFailure,sizeof(PropertyFailure));

			//设置变量
			PropertyFailure.lResultCode=m_GameDBAide.GetReturnValue();
			PropertyFailure.lFrozenedScore=pPropertyRequest->lFrozenedScore;
			PropertyFailure.cbRequestArea=pPropertyRequest->cbRequestArea;
			lstrcpyn(PropertyFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(PropertyFailure.szDescribeString));

			//发送结果
			WORD wDataSize=CountStringBuffer(PropertyFailure.szDescribeString);
			WORD wHeadSize=sizeof(PropertyFailure)-sizeof(PropertyFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PROPERTY_FAILURE,dwContextID,&PropertyFailure,wHeadSize+wDataSize);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestPropertyRequest, "<<pIException->GetExceptionDescribe());

		//变量定义
		DBO_GR_PropertyFailure PropertyFailure;
		ZeroMemory(&PropertyFailure,sizeof(PropertyFailure));

		//变量定义
		DBR_GR_PropertyRequest * pPropertyRequest=(DBR_GR_PropertyRequest *)pData;

		//设置变量
		PropertyFailure.lResultCode=DB_ERROR;
		PropertyFailure.lFrozenedScore=pPropertyRequest->lFrozenedScore;
		PropertyFailure.cbRequestArea=pPropertyRequest->cbRequestArea;
		lstrcpyn(PropertyFailure.szDescribeString,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(PropertyFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(PropertyFailure.szDescribeString);
		WORD wHeadSize=sizeof(PropertyFailure)-sizeof(PropertyFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PROPERTY_FAILURE,dwContextID,&PropertyFailure,wHeadSize+wDataSize);

		return false;
	}

	return true;
}

//比赛报名
bool CDataBaseEngineSink::OnRequestMatchFee(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchFee));
	if (wDataSize!=sizeof(DBR_GR_MatchFee)) return false;

	//变量定义
	DBR_GR_MatchFee * pMatchFee=(DBR_GR_MatchFee *)pData;
	dwUserID=INVALID_DWORD;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchFee->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchFee->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchFee"),pMatchFee->dwMatchFee);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchFee->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchNO"),pMatchFee->dwMatchNO);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pMatchFee->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserMatchFee"),true);

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_FEE_RESULT,dwContextID,&lReturnValue,sizeof(lReturnValue));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestMatchFee, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//比赛开始
bool CDataBaseEngineSink::OnRequestMatchStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchStart));
	if (wDataSize!=sizeof(DBR_GR_MatchStart)) return false;

	//变量定义
	DBR_GR_MatchStart * pMatchStart=(DBR_GR_MatchStart *)pData;
	dwUserID=INVALID_DWORD;

	//请求处理
	try
	{
		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wMatchID"),pMatchStart->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@wMatchNo"),pMatchStart->dwMatchNo);
		m_GameDBAide.AddParameter(TEXT("@wMatchCount"),pMatchStart->wMatchCount);	
		m_GameDBAide.AddParameter(TEXT("@lInitScore"),pMatchStart->lInitScore);	

// 		//输出参数
// 		TCHAR szDescribeString[128]=TEXT("");
// 		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchStart"),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestMatchStart, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//比赛结束
bool CDataBaseEngineSink::OnRequestMatchOver(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchOver));
	if (wDataSize!=sizeof(DBR_GR_MatchOver)) return false;

	//变量定义
	DBR_GR_MatchOver * pMatchOver=(DBR_GR_MatchOver *)pData;
	dwUserID=INVALID_DWORD;

	//请求处理
	try
	{
		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wMatchID"),pMatchOver->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@wMatchNo"),pMatchOver->dwMatchNo);
		m_GameDBAide.AddParameter(TEXT("@wMatchCount"),pMatchOver->wMatchCount);		

// 		//输出参数
// 		TCHAR szDescribeString[128]=TEXT("");
// 		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchOver"),true);

		if(lReturnValue==DB_SUCCESS)
		{
			LONG lCount=0;
			if(m_GameDBModule.GetInterface()!=NULL)
				lCount=m_GameDBModule->GetRecordCount();
			DBO_GR_MatchRank *pMatchRank=new DBO_GR_MatchRank[lCount];
			for(LONG i=0; i<lCount; i++)
			{
				pMatchRank[i].cbRank=(BYTE)i;
				m_GameDBAide.GetValue_String(TEXT("NickName"),pMatchRank[i].szNickName,CountArray(pMatchRank[i].szNickName));
				pMatchRank[i].lMatchScore=m_GameDBAide.GetValue_LONG(TEXT("HScore"));
				m_GameDBModule->MoveToNext();
				if(m_GameDBModule->IsRecordsetEnd()) break;
			}
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_RANK,dwContextID,pMatchRank,(WORD)(sizeof(DBO_GR_MatchRank)*lCount));
			if(pMatchRank!=NULL)
				SafeDeleteArray(pMatchRank);
		}
	
		
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestMatchOver, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//比赛奖励
bool CDataBaseEngineSink::OnRequestMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchReward));
	if (wDataSize!=sizeof(DBR_GR_MatchReward)) return false;

	//变量定义
	DBR_GR_MatchReward * pMatchReward=(DBR_GR_MatchReward *)pData;
	dwUserID=INVALID_DWORD;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchReward->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchReward->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchReward->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchNO"),pMatchReward->dwMatchNO);
		m_GameDBAide.AddParameter(TEXT("@wRank"),pMatchReward->wRank);
		m_GameDBAide.AddParameter(TEXT("@lMatchScore"),pMatchReward->lMatchScore);
		m_GameDBAide.AddParameter(TEXT("@dwExperience"),pMatchReward->dwExperience);
		m_GameDBAide.AddParameter(TEXT("@dwGold"),pMatchReward->dwGold);
		m_GameDBAide.AddParameter(TEXT("@dwMedal"),pMatchReward->dwMedal);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//结果处理
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchReward"),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestMatchReward, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//退出比赛
bool CDataBaseEngineSink::OnRequestMatchQuit(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchFee));
	if (wDataSize!=sizeof(DBR_GR_MatchFee)) return false;

	//变量定义
	DBR_GR_MatchFee * pMatchFee=(DBR_GR_MatchFee *)pData;
	dwUserID=INVALID_DWORD;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchFee->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchFee->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchFee"),pMatchFee->dwMatchFee);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchFee->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchNO"),pMatchFee->dwMatchNO);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pMatchFee->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserMatchQuit"),true);

		//发送结果
		if(INVALID_WORD!=dwContextID)
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_QUIT_RESULT,dwContextID,&lReturnValue,sizeof(lReturnValue));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestMatchQuit, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}
}


// 加载平衡分曲线(added by anjay)
// bool CDataBaseEngineSink::OnRequestLoadBalanceScoreCurve(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
// {
// 	try
// 	{
// 		//构造参数
// 		m_PlatformDBAide.ResetParameter();
// 
// 		//执行查询
// 		LONG lReturnValue=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadBalanceScoreCurve"),true);
// 
// 		//读取信息
// 		if (lReturnValue==DB_SUCCESS)
// 		{
// 			//起始消息
// 			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BALANCE_SCORE_CURVE, 0xfffe, NULL, 0);
// 
// 			//读取消息
// 			while (m_PlatformDBModule->IsRecordsetEnd()==false)
// 			{
// 				DBO_GR_BalanceScoreCurve BalanceScoreCurve;
// 				ZeroMemory(&BalanceScoreCurve, sizeof(BalanceScoreCurve));
// 
// 				//读取消息
// 				BalanceScoreCurve.lScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Score"));	
// 				BalanceScoreCurve.lBalanceScore = m_PlatformDBAide.GetValue_DWORD(TEXT("BalanceScore"));
// 
// 				//发送消息
// 				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BALANCE_SCORE_CURVE, 0, &BalanceScoreCurve, sizeof(DBO_GR_BalanceScoreCurve));
// 
// 				//移动记录
// 				m_PlatformDBModule->MoveToNext();
// 			};
// 		}
// 
// 		return true;
// 	}
// 	catch (IDataBaseException * pIException)
// 	{
// 		TCHAR szBuffer[256] = TEXT("");
// 		_sntprintf(szBuffer,CountArray(szBuffer), TEXT("CDataBaseEngineSink::OnRequestLoadBalanceScoreCurve %s"), pIException->GetExceptionDescribe() );
// 		CTraceService::TraceString(szBuffer,TraceLevel_Exception);
// 		LOGI("CDataBaseEngineSink::OnRequestLoadBalanceScoreCurve, "<<szBuffer);
// 
// 		return false;
// 	}
// 
// 	return true;
// }

// 重置游戏锁
bool CDataBaseEngineSink::OnRequestResetGameScoreLocker(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//执行查询
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_ResetGameScoreLocker"),true);

		return true;
	}
	catch(IDataBaseException * pIException)
	{
		//错误信息
		TCHAR szBuffer[256] = TEXT("");
		_sntprintf(szBuffer,CountArray(szBuffer), TEXT("CDataBaseEngineSink::OnRequestResetGameScoreLocker %s"), pIException->GetExceptionDescribe() );
		CTraceService::TraceString(szBuffer, TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestResetGameScoreLocker, "<<szBuffer);

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}
}

//广场赛奖励
bool CDataBaseEngineSink::OnRequestSquareReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_SuqareReward));
		if (wDataSize!=sizeof(DBR_GR_SuqareReward)) return false;

		//变量定义
		DBR_GR_SuqareReward * pSquareReward=(DBR_GR_SuqareReward *)pData;
		dwUserID=pSquareReward->dwUserID;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pSquareReward->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@wRank"),pSquareReward->wRank);
		m_PlatformDBAide.AddParameter(TEXT("@wMatchID"),pSquareReward->wMatchID);
		m_PlatformDBAide.AddParameter(TEXT("@wGameID"),pSquareReward->wGameID);
		m_PlatformDBAide.AddParameter(TEXT("@wServerID"),pSquareReward->wServerID);

		//执行查询
		if (m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_SquareReward"),true)==DB_SUCCESS)
		{
			//读取范围
			DBO_GR_Square_Reward SquareReward;
			SquareReward.dwUserID=pSquareReward->dwUserID;
			SquareReward.wRank=pSquareReward->wRank;
			SquareReward.wMatchID=pSquareReward->wMatchID;
			SquareReward.wGameID=pSquareReward->wGameID;
			SquareReward.wServerID=pSquareReward->wServerID;
			SquareReward.cbMemberOrder=m_PlatformDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			SquareReward.dwUserRight=m_PlatformDBAide.GetValue_WORD(TEXT("UserRight"));
			m_PlatformDBAide.GetValue_String(TEXT("AwardInfo"), SquareReward.szAwardInfo, CountArray(SquareReward.szAwardInfo));
			LOGI("CDataBaseEngineSink::OnRequestSquareReward 广场赛wMatchID= " << SquareReward.wMatchID << " dwUserID= " << SquareReward.dwUserID << " wRank= " << SquareReward.wRank << " 奖励描述= " << SquareReward.szAwardInfo);

			//发送消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SQUARE_REWARD, dwContextID, &SquareReward, sizeof(DBO_GR_Square_Reward));
		}

		//效验参数
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestSquareReward, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//加载广场赛奖励配置
bool CDataBaseEngineSink::OnRequestLoadSquareReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_LoadSuqareReward));
		if (wDataSize!=sizeof(DBR_GR_LoadSuqareReward)) return false;

		//变量定义
		DBR_GR_LoadSuqareReward * pLoadSquareReward=(DBR_GR_LoadSuqareReward *)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wMatchID"),pLoadSquareReward->wMatchID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		if (m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadSquareMatchRankBonus"),true)==DB_SUCCESS)
		{
			DBO_GR_Load_Square_Reward LoadSquareReward;
			memset(&LoadSquareReward,0,sizeof(DBO_GR_Load_Square_Reward));

			LoadSquareReward.wMatchID=pLoadSquareReward->wMatchID;
			LoadSquareReward.wGameID=pLoadSquareReward->wGameID;
			LoadSquareReward.wServerID=pLoadSquareReward->wServerID;
			//读取列表
			BYTE cbRankIndex=0;
			BYTE cbPropIDIndex=0;
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//出界
				if(cbRankIndex>=20)break;
				if(cbPropIDIndex>=5)
				{
					cbRankIndex++;
					cbPropIDIndex=0;
					m_PlatformDBModule->MoveToNext();
					continue;
				}

				WORD wFirstRank=m_PlatformDBAide.GetValue_WORD(TEXT("FirstRank"));
				if(cbRankIndex==0&&cbPropIDIndex==0)
				{//首次填充
					LoadSquareReward.RankBonus[cbRankIndex].wFirst=wFirstRank;
					LoadSquareReward.RankBonus[cbRankIndex].wLast=m_PlatformDBAide.GetValue_WORD(TEXT("EndRank"));
					LoadSquareReward.RankBonus[cbRankIndex].wPropertyID[cbPropIDIndex++]=m_PlatformDBAide.GetValue_WORD(TEXT("AwardID"));
					LoadSquareReward.RankBonus[cbRankIndex].cbPropertyCount++;
					LoadSquareReward.cbRankBonus++;
				}
				else
				{
					if(wFirstRank==LoadSquareReward.RankBonus[cbRankIndex].wFirst)
					{//同排名范围内的道具
						LoadSquareReward.RankBonus[cbRankIndex].wFirst=wFirstRank;
						LoadSquareReward.RankBonus[cbRankIndex].wLast=m_PlatformDBAide.GetValue_WORD(TEXT("EndRank"));
						LoadSquareReward.RankBonus[cbRankIndex].wPropertyID[cbPropIDIndex++]=m_PlatformDBAide.GetValue_WORD(TEXT("AwardID"));
						LoadSquareReward.RankBonus[cbRankIndex].cbPropertyCount++;
					}
					else
					{//下一个排名范围内的道具
						cbRankIndex++;
						cbPropIDIndex=0;
						LoadSquareReward.RankBonus[cbRankIndex].wFirst=wFirstRank;
						LoadSquareReward.RankBonus[cbRankIndex].wLast=m_PlatformDBAide.GetValue_WORD(TEXT("EndRank"));
						LoadSquareReward.RankBonus[cbRankIndex].wPropertyID[cbPropIDIndex++]=m_PlatformDBAide.GetValue_WORD(TEXT("AwardID"));
						LoadSquareReward.RankBonus[cbRankIndex].cbPropertyCount++;
						LoadSquareReward.cbRankBonus++;
					}
				}

				m_PlatformDBModule->MoveToNext();
			}

			LOGI("CDataBaseEngineSink::OnRequestLoadSquareReward 加载广场赛配置信息 MatchID= " << LoadSquareReward.wMatchID << " ServerID= " << LoadSquareReward.wServerID\
				<< " GameID= " << LoadSquareReward.wGameID << " RankBonusCount= " << LoadSquareReward.cbRankBonus);

			//发送消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOAD_SQUARE_REWARD, dwContextID, &LoadSquareReward, sizeof(DBO_GR_Load_Square_Reward));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestLoadSquareReward, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

// 用户自建桌子
bool CDataBaseEngineSink::OnRequestUserCreateCustomTable(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBR_GR_UserCreateCustomTable));
	if (wDataSize != sizeof(DBR_GR_UserCreateCustomTable)) return false;

	//变量定义
	DBR_GR_UserCreateCustomTable * pCutsomTable = (DBR_GR_UserCreateCustomTable *)pData;

	try
	{
		//构造参数
		m_UserCustomDBAide.ResetParameter();
		m_UserCustomDBAide.AddParameter(TEXT("@dwUserID"), pCutsomTable->dwUserID);
		m_UserCustomDBAide.AddParameter(TEXT("@wKindID"), pCutsomTable->wKindID);
		m_UserCustomDBAide.AddParameter(TEXT("@wServerID"), pCutsomTable->wServerID);
		m_UserCustomDBAide.AddParameter(TEXT("@wTableID"), pCutsomTable->wTableID);
		m_UserCustomDBAide.AddParameter(TEXT("@wConsumePropertyCount"), pCutsomTable->wCardCousumeCount);

		// 规则转文本
		TCHAR szCustomRule[CountArray(pCutsomTable->cbCustomRule) * 2 + 1] = TEXT("");
		for (INT i = 0; i < CountArray(pCutsomTable->cbCustomRule); i++)
		{
			_stprintf(&szCustomRule[i * 2], TEXT("%02X"), pCutsomTable->cbCustomRule[i]);
		}
		m_UserCustomDBAide.AddParameter(TEXT("@strCustomRule"), szCustomRule);

		//输出信息
		TCHAR szDescribeString[128] = TEXT("");
		m_UserCustomDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_UserCustomDBAide.ExecuteProcess(TEXT("GSP_GR_UserCustomCreateTable"), true);

		CDBVarValue DBVarValue;
		m_UserCustomDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

		OnUserCustomTableDisposeResult(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal), pCutsomTable->cbCustomRule, pCutsomTable->wTableID);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		LOGE("CDataBaseEngineSink::OnRequestUserCreateCustomTable, " << pIException->GetExceptionDescribe());

		//错误处理
		OnUserCustomTableDisposeResult(dwContextID, DB_ERROR, TEXT("由于数据库操作异常，请您稍后重试！"), nullptr, pCutsomTable->wTableID);

		return false;
	}

	return true;
}

// 用户加入桌子
bool CDataBaseEngineSink::OnRequestUserJoinCustomTable(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBR_GR_UserJoinCustomTable));
	if (wDataSize != sizeof(DBR_GR_UserJoinCustomTable)) return false;

	//变量定义
	DBR_GR_UserJoinCustomTable * pUserJoinCustomTable = (DBR_GR_UserJoinCustomTable *)pData;
	dwUserID = pUserJoinCustomTable->dwUserID;
	try
	{
		//构造参数
		m_UserCustomDBAide.ResetParameter();
		m_UserCustomDBAide.AddParameter(TEXT("@dwUserID"), pUserJoinCustomTable->dwUserID);
		m_UserCustomDBAide.AddParameter(TEXT("@strSecret"), pUserJoinCustomTable->szSecret);
		//输出信息
		TCHAR szDescribeString[128] = TEXT("");
		m_UserCustomDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_UserCustomDBAide.ExecuteProcess(TEXT("GSP_GS_UserJoinCustomTable"), true);

		CDBVarValue DBVarValue;
		m_UserCustomDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

		OnUserJoinCustomTableDisposeResult(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));
		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		LOGE("CDataBaseEngineSink::OnRequestUserJoinCustomTable, " << pIException->GetExceptionDescribe());

		//错误处理
		OnUserJoinCustomTableDisposeResult(dwContextID, DB_ERROR, TEXT("由于数据库操作异常，请您稍后重试！"));

		return false;
	}
	return true;
}

// 解散自定义桌子
bool CDataBaseEngineSink::OnRequestConcludeCustomTable(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBR_GR_ConcludeCustomTable));
	if (wDataSize != sizeof(DBR_GR_ConcludeCustomTable)) return false;

	//变量定义
	DBR_GR_ConcludeCustomTable * pConcludeCutsomTable = (DBR_GR_ConcludeCustomTable *)pData;

	try
	{
		//构造参数
		m_UserCustomDBAide.ResetParameter();
		m_UserCustomDBAide.AddParameter(TEXT("@wServerID"), pConcludeCutsomTable->wServerID);
		m_UserCustomDBAide.AddParameter(TEXT("@wTableID"), pConcludeCutsomTable->wTableID);
		m_UserCustomDBAide.AddParameter(TEXT("@wConludeType"), pConcludeCutsomTable->wConcludeType);

		//输出信息
		TCHAR szDescribeString[128] = TEXT("");
		m_UserCustomDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_UserCustomDBAide.ExecuteProcess(TEXT("GSP_GR_ConcludeTable"), true);

		CDBVarValue DBVarValue;
		m_UserCustomDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

		OnDismissCustomTableDisposeResult(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal), pConcludeCutsomTable->wTableID, pConcludeCutsomTable->wConcludeType);
		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestUserCreateCustomTable, " << pIException->GetExceptionDescribe());

		//错误处理
		OnDismissCustomTableDisposeResult(dwContextID, DB_ERROR, TEXT("由于数据库操作异常，请您稍后重试！"), pConcludeCutsomTable->wTableID, pConcludeCutsomTable->wConcludeType);
		return false;
	}

	return true;
}

// 在自定义桌子开始游戏
bool CDataBaseEngineSink::OnRequestUserStartCustomGame(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBR_GR_UserStartCustomGame));
	if (wDataSize != sizeof(DBR_GR_UserStartCustomGame)) return false;

	//变量定义
	DBR_GR_UserStartCustomGame * pUserStartCustomGame = (DBR_GR_UserStartCustomGame *)pData;

	try
	{
		//构造参数
		m_UserCustomDBAide.ResetParameter();
		m_UserCustomDBAide.AddParameter(TEXT("@wServerID"), pUserStartCustomGame->wServerID);
		m_UserCustomDBAide.AddParameter(TEXT("@dwUserID"), pUserStartCustomGame->dwUserID);
		m_UserCustomDBAide.AddParameter(TEXT("@wTableID"), pUserStartCustomGame->wTableID);
		m_UserCustomDBAide.AddParameter(TEXT("@wChairID"), pUserStartCustomGame->wChairID);
		m_UserCustomDBAide.AddParameter(TEXT("@dwRoundCount"), pUserStartCustomGame->dwRoundCount);

		//输出信息
		TCHAR szDescribeString[128] = TEXT("");
		m_UserCustomDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_UserCustomDBAide.ExecuteProcess(TEXT("GSP_GR_UserStartCustomGame"), true);

		CDBVarValue DBVarValue;
		m_UserCustomDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestManageUserRight, " << pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID, DB_ERROR, 0L, TEXT("由于数据库操作异常，请您稍后重试！"), false);

		return false;
	}
	return true;
}

//用户权限
bool CDataBaseEngineSink::OnRequestManageUserRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_ManageUserRight));
		if (wDataSize!=sizeof(DBR_GR_ManageUserRight)) return false;

		//请求处理
		DBR_GR_ManageUserRight * pManageUserRight=(DBR_GR_ManageUserRight *)pData;
		dwUserID=pManageUserRight->dwUserID;

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pManageUserRight->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwAddRight"),pManageUserRight->dwAddRight);
		m_GameDBAide.AddParameter(TEXT("@dwRemoveRight"),pManageUserRight->dwRemoveRight);

		//执行查询
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_ManageUserRight"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestManageUserRight, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//系统消息
bool CDataBaseEngineSink::OnRequestLoadSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//执行查询
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadSystemMessage"),true);

		//结果处理
		if(lReturnValue==0)
		{
			TCHAR szServerID[32]={0};
			_sntprintf(szServerID, CountArray(szServerID), TEXT("%d"), m_pGameServiceOption->wServerID);

			while(true)
			{
				//结束判断
				if (m_GameDBModule->IsRecordsetEnd()==true) break;

				//定义变量
				TCHAR szServerRange[1024]={0};
				CString strServerRange;
				bool bSendMessage=false;
				bool bAllRoom=false;

				//读取范围
				m_GameDBAide.GetValue_String(TEXT("ServerRange"), szServerRange, CountArray(szServerRange));
				szServerRange[1023]=0;
				strServerRange.Format(TEXT("%s"), szServerRange);

				//范围判断
				while(true)
				{
					int nfind=strServerRange.Find(TEXT(','));
					if(nfind!=-1 && nfind>0)
					{
						CString strID=strServerRange.Left(nfind);
						WORD wServerID=StrToInt(strID);
						bSendMessage=(wServerID==0 || wServerID==m_pGameServiceOption->wServerID);
						if(wServerID==0)bAllRoom=true;

						if(bSendMessage) break;

						strServerRange=strServerRange.Right(strServerRange.GetLength()-nfind-1);
					}
					else
					{
						WORD wServerID=StrToInt(szServerRange);
						bSendMessage=(wServerID==0 || wServerID==m_pGameServiceOption->wServerID);
						if(wServerID==0)bAllRoom=true;

						break;
					}
				}

				//发送消息
				if(bSendMessage)
				{
					//定义变量
					DBR_GR_SystemMessage SystemMessage;
					ZeroMemory(&SystemMessage, sizeof(SystemMessage));

					//读取消息
					SystemMessage.cbMessageID=m_GameDBAide.GetValue_BYTE(TEXT("ID"));
					SystemMessage.cbMessageType=m_GameDBAide.GetValue_BYTE(TEXT("MessageType"));
					SystemMessage.dwTimeRate=m_GameDBAide.GetValue_DWORD(TEXT("TimeRate"));
					SystemMessage.cbAllRoom=bAllRoom?TRUE:FALSE;
					m_GameDBAide.GetValue_String(TEXT("MessageString"),SystemMessage.szSystemMessage,CountArray(SystemMessage.szSystemMessage));

					//读取时间
					SYSTEMTIME systime;
					ZeroMemory(&systime, sizeof(systime));
					m_GameDBAide.GetValue_SystemTime(TEXT("ConcludeTime"),systime);

					//转换时间
					CTime time(systime);
					SystemMessage.tConcludeTime=time.GetTime();

					//发送结果
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SYSTEM_MESSAGE_RESULT,dwContextID,&SystemMessage,sizeof(SystemMessage));
				}

				//下一条
				m_GameDBModule->MoveToNext();
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestLoadSystemMessage, "<<pIException->GetExceptionDescribe());

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//加载敏感词
bool CDataBaseEngineSink::OnRequestLoadSensitiveWords(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();

		//执行查询
		LONG lReturnValue=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadSensitiveWords"),true);

		//读取信息
		if (lReturnValue==DB_SUCCESS)
		{
			//起始消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SENSITIVE_WORDS,0xfffe,NULL,0);

			//读取消息
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//变量定义
				TCHAR szSensitiveWords[32]=TEXT("");

				//读取消息
				m_PlatformDBAide.GetValue_String(TEXT("SensitiveWords"),szSensitiveWords,CountArray(szSensitiveWords));				

				//发送消息
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SENSITIVE_WORDS,0,szSensitiveWords,sizeof(szSensitiveWords));

				//移动记录
				m_PlatformDBModule->MoveToNext();
			};

			//结束消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SENSITIVE_WORDS,0xffff,NULL,0);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		LOGI("CDataBaseEngineSink::OnRequestLoadSensitiveWords, "<<pIException->GetExceptionDescribe());

		return false;
	}

	return true;
}

//登录结果
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//属性资料
		m_LogonSuccess.wFaceID=m_GameDBAide.GetValue_WORD(TEXT("FaceID"));
		m_LogonSuccess.dwUserID=m_GameDBAide.GetValue_DWORD(TEXT("UserID"));
		m_LogonSuccess.dwGameID=m_GameDBAide.GetValue_DWORD(TEXT("GameID"));
		m_LogonSuccess.dwGroupID=m_GameDBAide.GetValue_DWORD(TEXT("GroupID"));
		m_LogonSuccess.dwCustomID=m_GameDBAide.GetValue_DWORD(TEXT("CustomID"));
		m_GameDBAide.GetValue_String(TEXT("NickName"),m_LogonSuccess.szNickName,CountArray(m_LogonSuccess.szNickName));
		m_GameDBAide.GetValue_String(TEXT("GroupName"),m_LogonSuccess.szGroupName,CountArray(m_LogonSuccess.szGroupName));

		//用户资料
		m_LogonSuccess.cbGender=m_GameDBAide.GetValue_BYTE(TEXT("Gender"));
		m_LogonSuccess.cbMemberOrder=m_GameDBAide.GetValue_BYTE(TEXT("MemberOrder"));
		m_LogonSuccess.cbMasterOrder=m_GameDBAide.GetValue_BYTE(TEXT("MasterOrder"));
		m_GameDBAide.GetValue_String(TEXT("UnderWrite"),m_LogonSuccess.szUnderWrite,CountArray(m_LogonSuccess.szUnderWrite));

		//积分信息
		m_LogonSuccess.lScore=m_GameDBAide.GetValue_LONGLONG(TEXT("Score"));
		m_LogonSuccess.lGrade=m_GameDBAide.GetValue_LONGLONG(TEXT("Grade"));
		m_LogonSuccess.lInsure=m_GameDBAide.GetValue_LONGLONG(TEXT("Insure"));

		//局数信息
		m_LogonSuccess.dwWinCount=m_GameDBAide.GetValue_LONG(TEXT("WinCount"));
		m_LogonSuccess.dwLostCount=m_GameDBAide.GetValue_LONG(TEXT("LostCount"));
		m_LogonSuccess.dwDrawCount=m_GameDBAide.GetValue_LONG(TEXT("DrawCount"));
		m_LogonSuccess.dwFleeCount=m_GameDBAide.GetValue_LONG(TEXT("FleeCount"));
		m_LogonSuccess.dwUserMedal=m_GameDBAide.GetValue_LONG(TEXT("UserMedal"));
		m_LogonSuccess.dwExperience=m_GameDBAide.GetValue_LONG(TEXT("Experience"));
		m_LogonSuccess.lLoveLiness=m_GameDBAide.GetValue_LONG(TEXT("LoveLiness"));

		//附加信息
		m_LogonSuccess.dwUserRight=m_GameDBAide.GetValue_DWORD(TEXT("UserRight"));
		m_LogonSuccess.dwMasterRight=m_GameDBAide.GetValue_DWORD(TEXT("MasterRight"));
		m_LogonSuccess.cbDeviceType=cbDeviceType;
		m_LogonSuccess.wBehaviorFlags=wBehaviorFlags;
		m_LogonSuccess.wPageTableCount=wPageTableCount;

		//索引变量
		m_LogonSuccess.dwInoutIndex=m_GameDBAide.GetValue_DWORD(TEXT("InoutIndex"));

		// 总输赢量
		m_LogonSuccess.lTotalWin = m_GameDBAide.GetValue_LONGLONG(TEXT("TotalWin"));
		m_LogonSuccess.lTotalLose = m_GameDBAide.GetValue_LONGLONG(TEXT("TotalLose"));

		// 区域ID
		m_LogonSuccess.dwAreaID = m_GameDBAide.GetValue_DWORD(TEXT("AreaID"));

		//获取信息
		if(pszErrorString!=NULL)lstrcpyn(m_LogonSuccess.szDescribeString,pszErrorString,CountArray(m_LogonSuccess.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(m_LogonSuccess.szDescribeString);
		WORD wHeadSize=sizeof(m_LogonSuccess)-sizeof(m_LogonSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS,dwContextID,&m_LogonSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//变量定义
		DBO_GR_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure,sizeof(LogonFailure));

		//构造数据
		LogonFailure.lResultCode=dwErrorCode;
		lstrcpyn(LogonFailure.szDescribeString,pszErrorString,CountArray(LogonFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
		WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
	}

	return;
}

//银行结果
VOID CDataBaseEngineSink::OnInsureDisposeResult(DWORD dwContextID, DWORD dwErrorCode, SCORE lFrozenedScore, LPCTSTR pszErrorString, bool bMobileClient,BYTE cbActivityGame)
{
//	LOGI(_T("CDataBaseEngineSink::OnInsureDisposeResult"));
	if (dwErrorCode==DB_SUCCESS)
	{
		//变量定义
		DBO_GR_UserInsureSuccess UserInsureSuccess;
		ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

		//构造变量
		UserInsureSuccess.cbActivityGame=cbActivityGame;
		UserInsureSuccess.lFrozenedScore=lFrozenedScore;
		UserInsureSuccess.dwUserID=m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
		UserInsureSuccess.lSourceScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("SourceScore"));
		UserInsureSuccess.lSourceInsure=m_TreasureDBAide.GetValue_LONGLONG(TEXT("SourceInsure"));
		UserInsureSuccess.lInsureRevenue=m_TreasureDBAide.GetValue_LONGLONG(TEXT("InsureRevenue"));
		UserInsureSuccess.lVariationScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("VariationScore"));
		UserInsureSuccess.lVariationInsure=m_TreasureDBAide.GetValue_LONGLONG(TEXT("VariationInsure"));
		lstrcpyn(UserInsureSuccess.szDescribeString,pszErrorString,CountArray(UserInsureSuccess.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(UserInsureSuccess.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_SUCCESS,dwContextID,&UserInsureSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//变量定义
		DBO_GR_UserInsureFailure UserInsureFailure;
		ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

		//构造变量
		UserInsureFailure.cbActivityGame=cbActivityGame;
		UserInsureFailure.lResultCode=dwErrorCode;
		UserInsureFailure.lFrozenedScore=lFrozenedScore;
		lstrcpyn(UserInsureFailure.szDescribeString,pszErrorString,CountArray(UserInsureFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_FAILURE,dwContextID,&UserInsureFailure,wHeadSize+wDataSize);
	}

	return;
}

//操作结果
VOID CDataBaseEngineSink::OnOperateDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//变量定义
		DBO_GR_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

		//构造变量
		OperateSuccess.lResultCode=dwErrorCode;
		lstrcpyn(OperateSuccess.szDescribeString,pszErrorString,CountArray(OperateSuccess.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(OperateSuccess.szDescribeString);
		WORD wHeadSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//变量定义
		DBO_GR_OperateSuccess OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//构造变量
		OperateFailure.lResultCode=dwErrorCode;
		lstrcpyn(OperateFailure.szDescribeString,pszErrorString,CountArray(OperateFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wHeadSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,&OperateFailure,wHeadSize+wDataSize);
	}

	return;
}

// 平衡分结果
VOID CDataBaseEngineSink::OnUserBalanceScoreDisposeResult(DWORD dwContextID, DWORD dwUserID,  bool bLogon /* = true */)
{
//	LOGI(TEXT("登录成功，读取平衡分数据。"));
	// 读取平衡分配置
	if (dwContextID != 0)
	{
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);
		//执行查询 
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadUserBalanceScore"),true);

		DBO_GR_UserBalanceScore UserBalanceScore;
		ZeroMemory(&UserBalanceScore, sizeof(UserBalanceScore));
		UserBalanceScore.dwUserID = m_GameDBAide.GetValue_DWORD(TEXT("UserID"));
		UserBalanceScore.lBalanceScore = m_GameDBAide.GetValue_LONG(TEXT("BalanceScore"));
		UserBalanceScore.lBuffTime = m_GameDBAide.GetValue_LONG(TEXT("BuffTime"));
		UserBalanceScore.lBuffGames = m_GameDBAide.GetValue_LONG(TEXT("BuffGames"));
		UserBalanceScore.lBuffAmount = m_GameDBAide.GetValue_LONG(TEXT("BuffAmount"));
		UserBalanceScore.bLogon = bLogon;

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_BALANCE_SCORE, dwContextID, &UserBalanceScore, sizeof(UserBalanceScore) );
	}

	return;
}

// 自建桌子结果
VOID CDataBaseEngineSink::OnUserCustomTableDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, LPBYTE pcbCustomRule, WORD wTableID)
{
	if (dwErrorCode == DB_SUCCESS)
	{
		DBO_GR_UserCustomTableSuccess UserCustomTableSuccess;
		memset(&UserCustomTableSuccess, 0, sizeof(DBO_GR_UserCustomTableSuccess));
		memcpy(&UserCustomTableSuccess.cbCustomRule, pcbCustomRule, sizeof(UserCustomTableSuccess.cbCustomRule));

		// 读取结果
		UserCustomTableSuccess.dwCreateUserID = m_UserCustomDBAide.GetValue_WORD(TEXT("CreateUserID"));
		UserCustomTableSuccess.wKindID = m_UserCustomDBAide.GetValue_WORD(TEXT("KindID"));
		UserCustomTableSuccess.wServerID = m_UserCustomDBAide.GetValue_WORD(TEXT("ServerID"));
		UserCustomTableSuccess.wTableID = m_UserCustomDBAide.GetValue_WORD(TEXT("TableID"));
		UserCustomTableSuccess.dwRoundCount = m_UserCustomDBAide.GetValue_DWORD(TEXT("RoundCount"));
		m_UserCustomDBAide.GetValue_String(TEXT("Secret"), UserCustomTableSuccess.szSecret, CountArray(UserCustomTableSuccess.szSecret));
		m_UserCustomDBAide.GetValue_SystemTime(TEXT("CreateDateTime"), UserCustomTableSuccess.tmCreateCustomTable);
		UserCustomTableSuccess.dwPropertyCount = m_UserCustomDBAide.GetValue_DWORD(TEXT("UserPropertyCount"));

		// 发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CREATE_CUSTOMTABLE_SUCCESS, dwContextID, &UserCustomTableSuccess, sizeof(DBO_GR_UserCustomTableSuccess));
	}
	else
	{
		//变量定义
		DBO_GR_UserCustomTableFailure UserCustomTableFailure;
		memset(&UserCustomTableFailure, 0, sizeof(DBO_GR_UserCustomTableFailure));

		//构造变量
		UserCustomTableFailure.lResultCode = dwErrorCode;
		UserCustomTableFailure.wTableID = wTableID;
		lstrcpyn(UserCustomTableFailure.szDescribeString, pszErrorString, CountArray(UserCustomTableFailure.szDescribeString));

		//发送结果
		WORD wDataSize = CountStringBuffer(UserCustomTableFailure.szDescribeString);
		WORD wHeadSize = sizeof(DBO_GR_UserCustomTableFailure) - sizeof(UserCustomTableFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CREATE_CUSTOMTABLE_FAILURE, dwContextID, &UserCustomTableFailure, wHeadSize + wDataSize);
	}
}

// 加入桌子结果
VOID CDataBaseEngineSink::OnUserJoinCustomTableDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	if (dwErrorCode == DB_SUCCESS)
	{
		DBO_GR_UserJoinCustomTableSuccess UserJoinCustomTableSuccess;

		UserJoinCustomTableSuccess.wTableID = m_UserCustomDBAide.GetValue_WORD(TEXT("TableID"));
		UserJoinCustomTableSuccess.wChairID = m_UserCustomDBAide.GetValue_WORD(TEXT("ChairID"));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_JOIN_CUSTOMTABLE_SUCCESS, dwContextID, &UserJoinCustomTableSuccess, sizeof(UserJoinCustomTableSuccess));
	}
	else
	{
		DBO_GR_UserJoinCustomTableFailure UserJoinCustomTableFailure;
		UserJoinCustomTableFailure.lResultCode = dwErrorCode;
		lstrcpyn(UserJoinCustomTableFailure.szDescribeString, pszErrorString, CountArray(UserJoinCustomTableFailure.szDescribeString));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_JOIN_CUSTOMTABLE_FAILURE, dwContextID, &UserJoinCustomTableFailure, sizeof(UserJoinCustomTableFailure));
	}
}

// 解散桌子结果
VOID CDataBaseEngineSink::OnDismissCustomTableDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, WORD wTableID, WORD wConcludeType)
{
	if (dwErrorCode == DB_SUCCESS)
	{
		DBO_GR_DismissCustomTableSuccess DismissCustomTableSuccess;
		DismissCustomTableSuccess.wTableID = wTableID;
		DismissCustomTableSuccess.wConcludeType = wConcludeType;
		DismissCustomTableSuccess.dwTableCreater = m_UserCustomDBAide.GetValue_DWORD(TEXT("CreateUserID"));
		DismissCustomTableSuccess.dwPropertyCount = m_UserCustomDBAide.GetValue_DWORD(TEXT("CreaterPropertyCount"));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DISMISS_CUSTOMTABLE_SUCCESS, dwContextID, &DismissCustomTableSuccess, sizeof(DBO_GR_DismissCustomTableSuccess));
	}
	else
	{
		DBO_GR_DismissCustomTableFailure DismissCustomTableFailure;
		DismissCustomTableFailure.lResultCode = dwErrorCode;
		DismissCustomTableFailure.wTableID = wTableID;
		lstrcpyn(DismissCustomTableFailure.szDescribeString, pszErrorString, CountArray(DismissCustomTableFailure.szDescribeString));
		DismissCustomTableFailure.wConcludeType = wConcludeType;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DISMISS_CUSTOMTABLE_FAILURE, dwContextID, &DismissCustomTableFailure, sizeof(DBO_GR_DismissCustomTableFailure));
	}
}

// 一次房卡内,玩家的累积分数
VOID CDataBaseEngineSink::OnUserInCustomTableScoreDisposeResult(DWORD dwContextID, DWORD dwUserID)
{
	if (dwContextID != 0)
	{
		try
		{
			m_UserCustomDBAide.ResetParameter();
			m_UserCustomDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
			//执行查询 
			LONG lResultCode = m_UserCustomDBAide.ExecuteProcess(TEXT("GSP_GR_LoadUserRoundScoreInCustomTable"), true);

			DBO_GR_UserRoundScoreInCustomTable UserRoundScoreInCustomTable;
			UserRoundScoreInCustomTable.dwUserID = dwUserID;
			UserRoundScoreInCustomTable.lRoundScore = m_UserCustomDBAide.GetValue_LONGLONG(TEXT("Score"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_ROUNDSCORE_IN_CUSTOMTABLE, dwContextID, &UserRoundScoreInCustomTable, sizeof(UserRoundScoreInCustomTable));
		}
		catch (IDataBaseException * pIException)
		{
			CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
			LOGE("CDataBaseEngineSink::OnUserInCustomTableScoreDisposeResult, " << pIException->GetExceptionDescribe());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
