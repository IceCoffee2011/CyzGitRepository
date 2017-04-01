#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//创建对象
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
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


	try
	{
		//连接信息
		tagDataBaseParameter * pAccountsDBParameter=&m_pInitParameter->m_AccountsDBParameter;
		tagDataBaseParameter * pTreasureDBParameter=&m_pInitParameter->m_TreasureDBParameter;
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;
		tagDataBaseParameter * pUserCustomDBParameter = &m_pInitParameter->m_UserCustomDBParameter;

		//设置连接
		m_AccountsDBModule->SetConnectionInfo(pAccountsDBParameter->szDataBaseAddr,pAccountsDBParameter->wDataBasePort,
			pAccountsDBParameter->szDataBaseName,pAccountsDBParameter->szDataBaseUser,pAccountsDBParameter->szDataBasePass);
		m_TreasureDBModule->SetConnectionInfo(pTreasureDBParameter->szDataBaseAddr,pTreasureDBParameter->wDataBasePort,
			pTreasureDBParameter->szDataBaseName,pTreasureDBParameter->szDataBaseUser,pTreasureDBParameter->szDataBasePass);
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
			pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);
		m_UserCustomDBModule->SetConnectionInfo(pUserCustomDBParameter->szDataBaseAddr, pUserCustomDBParameter->wDataBasePort,
			pUserCustomDBParameter->szDataBaseName, pUserCustomDBParameter->szDataBaseUser, pUserCustomDBParameter->szDataBasePass);

		//发起连接
		m_TreasureDBModule->OpenConnection();
		m_TreasureDBAide.SetDataBase(m_TreasureDBModule.GetInterface());

		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//发起连接
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		m_UserCustomDBModule->OpenConnection();
		m_UserCustomDBAide.SetDataBase(m_UserCustomDBModule.GetInterface());

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//设置对象
	m_AccountsDBAide.SetDataBase(NULL);
	m_TreasureDBAide.SetDataBase(NULL);
	m_PlatformDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
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

	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

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
	switch (wRequestID)
	{
	case DBR_GP_LOGON_GAMEID:			//标识登录
		{
			return OnRequestLogonGameID(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOGON_ACCOUNTS:			//帐号登录
		{
			return OnRequestLogonAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_GP_REGISTER_ACCOUNTS:		//注册帐号
		{
			return OnRequestRegisterAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_GP_REGISTER_PROFILE:		//补充资料 add by chenj
		{
			return OnRequestUserProfile(dwContextID,pData,wDataSize);
		}
	case DBR_GP_CHECKNICK:		//检测昵称 add by chenj
		{
			return OnRequestCheckNick(dwContextID,pData,wDataSize);;
		}	
	case DBR_GP_MODIFY_MACHINE:			//修改机器
		{
			return OnRequestModifyMachine(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_LOGON_PASS:		//修改密码
		{
			return OnRequestModifyLogonPass(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_INSURE_PASS:		//修改密码
		{
			return OnRequestModifyInsurePass(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_UNDER_WRITE:		//修改签名
		{
			return OnRequestModifyUnderWrite(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_SYSTEM_FACE:		//修改头像
		{
			return OnRequestModifySystemFace(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_CUSTOM_FACE:		//修改头像
		{
			return OnRequestModifyCustomFace(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_INDIVIDUAL:		//修改资料
		{
			return OnRequestModifyIndividual(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_SAVE_SCORE:		//存入游戏币
		{
			return OnRequestUserSaveScore(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_TAKE_SCORE:		//提取游戏币
		{
			return OnRequestUserTakeScore(dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_TRANSFER_SCORE:	//转账游戏币
		{
			return OnRequestUserTransferScore(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_INDIVIDUAL:		//查询资料
		{
			return OnRequestQueryIndividual(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_INSURE_INFO:		//查询银行
		{
			return OnRequestQueryInsureInfo(dwContextID,pData,wDataSize);
		}
    case DBR_GP_QUERY_TRANSRECORD:
        {
            return OnRequestQueryTransRecord(dwContextID,pData,wDataSize);
        }
	case DBR_GP_QUERY_USER_INFO:		//查询用户
		{
			return OnRequestQueryTransferUserInfo(dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOAD_GAME_LIST:			//加载列表
		{
			return OnRequestLoadGameList(dwContextID,pData,wDataSize);
		}
	case DBR_GP_ONLINE_COUNT_INFO:		//在线信息
		{
			return OnRequestOnLineCountInfo(dwContextID,pData,wDataSize);
		}
	case DBR_MB_LOGON_GAMEID:			//标识登录
		{
			return OnMobileLogonGameID(dwContextID,pData,wDataSize);
		}
	case DBR_MB_LOGON_ACCOUNTS:			//帐号登录
		{
			return OnMobileLogonAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_MB_REGISTER_ACCOUNTS:		//注册帐号
		{
			return OnMobileRegisterAccounts(dwContextID,pData,wDataSize);
		}
	case DBR_GP_QUERY_NICKNAME_BY_GAMEID:	// 根据GameID查询昵称
		{
			return OnRequestQueryNickNameByGameID(dwContextID, pData, wDataSize);
		}
	case DBR_GP_VERIFY_INSURE_PASSWORD:		// 验证银行密码
		{
			return OnRequestVerifyInsurePassword(dwContextID, pData, wDataSize);
		}
	case DBR_GP_QUERY_EXCHANGE_LOVELINESS:	// 获取魅力兑换信息
		{
			return OnRequestQueryExchangeLoveliness(dwContextID, pData, wDataSize);
		}
	case DBR_GP_EXCHANGE_LOVELINESS:		// 兑换魅力
		{
			return OnRequestExchangeLoveliness(dwContextID, pData, wDataSize);
		}
	case DBR_MB_SIGN_IN_CHECK:				// 签到
		{
			return OnRequestSignInCheck(dwContextID, pData, wDataSize);
		}
	case DBR_MB_RECHARGE_MESSAGE:			// 请求充值相关信息
		{
			return OnMobileReqRechargeMessage(dwContextID, pData, wDataSize);
		}
	case DBR_MB_CUSTOM_SERVICE_MESSAGE:		// 请求客服相关信息
		{
			return OnMobileReqCustomServiceMessage(dwContextID, pData, wDataSize);
		}
	case DBR_TP_LOGON_ACCOUNTS:				// 第三方登录
		{
			return OnThirdPartyLogonAccounts(dwContextID, pData, wDataSize);
		}
	case DBR_MB_SHARE_SUCCESS:				// 分享成功
		{
			return OnShareSuccess(dwContextID, pData, wDataSize);
		}
	case DBR_MB_USER_JOIN_CUSTOMTABLE:		// 加入桌子
		{
			return OnUserJoinCustomTable(dwContextID, pData, wDataSize);
		}
	case DBR_MB_USER_QUERY_CUSTOMTABLE:		// 查询加入的桌子信息
		{
			return OnUserQueryCustomTable(dwContextID, pData, wDataSize);
		}
	case DBR_MB_USER_TRANSFER_DIAMOND:		// 用户转账钻石
		{
			return OnRequestUserTransferDiamond(dwContextID, pData, wDataSize);
		}	
	}

	return false;
}

//I D 登录
bool CDataBaseEngineSink::OnRequestLogonGameID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_LogonGameID));
		if (wDataSize!=sizeof(DBR_GP_LogonGameID)) return false;

		//执行查询
		DBR_GP_LogonGameID * pLogonGameID=(DBR_GP_LogonGameID *)pData;

		//变量定义
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonGameID->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonGameID->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwGameID"),pLogonGameID->dwGameID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonGameID->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonGameID->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@nNeeValidateMBCard"),pLogonGameID->cbNeeValidateMBCard);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyGameID"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),false);

		return false;
	}

	return true;
}

//帐号登录
bool CDataBaseEngineSink::OnRequestLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	LOGI("CDataBaseEngineSink::OnRequestLogonAccounts, Begin="<<GetTickCount());
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_LogonAccounts));
		if (wDataSize!=sizeof(DBR_GP_LogonAccounts)) return false;

		//请求处理
		DBR_GP_LogonAccounts * pLogonAccounts=(DBR_GP_LogonAccounts *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonAccounts->pBindParameter;
		if (pBindParameter!=NULL&&pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@nNeeValidateMBCard"),pLogonAccounts->cbNeeValidateMBCard);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		LOGI("CDataBaseEngineSink::OnRequestLogonAccounts, ExecuteProcess Begin="<<GetTickCount());
		//执行查询
		LONG lResultCode=0;
#ifdef TEST_KERNELENGINE
		lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("QPAccountsDB.dbo.GSP_GP_EfficacyAccounts"),true);
#else
		lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyAccounts"),true);
#endif
		

		LOGI("CDataBaseEngineSink::OnRequestLogonAccounts, ExecuteProcess End="<<GetTickCount());

		//结果处理
		LOGI("CDataBaseEngineSink::OnRequestLogonAccounts, GetParameter Begin="<<GetTickCount());
		CDBVarValue DBVarValue;
#ifdef TEST_KERNELENGINE
		m_AccountsDBModule->GetParameter(TEXT("strErrorDescribe"),DBVarValue);
#else
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
#endif
		

		LOGI("CDataBaseEngineSink::OnRequestLogonAccounts, GetParameter End="<<GetTickCount());

		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),false);

		return false;
	}

	return true;
}

//注册处理
bool CDataBaseEngineSink::OnRequestRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_RegisterAccounts));
		if (wDataSize!=sizeof(DBR_GP_RegisterAccounts)) return false;

		//请求处理
		DBR_GP_RegisterAccounts * pRegisterAccounts=(DBR_GP_RegisterAccounts *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pRegisterAccounts->pBindParameter;
		if (pBindParameter!=NULL&&pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pRegisterAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pRegisterAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strSpreader"),pRegisterAccounts->szSpreader);
		m_AccountsDBAide.AddParameter(TEXT("@strLogonPass"),pRegisterAccounts->szLogonPass);
		m_AccountsDBAide.AddParameter(TEXT("@strInsurePass"),pRegisterAccounts->szInsurePass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strPassPortID"),pRegisterAccounts->szPassPortID);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pRegisterAccounts->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pRegisterAccounts->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=0;
#ifdef TEST_KERNELENGINE
		lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("QPAccountsDB.dbo.GSP_GP_RegisterAccounts"),true);
#else
		lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RegisterAccounts"),true);
#endif
		
		//结果处理
		CDBVarValue DBVarValue;
#ifdef TEST_KERNELENGINE
		m_AccountsDBModule->GetParameter(TEXT("strErrorDescribe"),DBVarValue);
#else
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
#endif
		
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),false);

		return false;
	}

	return true;
}

//补充资料
bool CDataBaseEngineSink::OnRequestUserProfile(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_RegisterProfile));
		if (wDataSize!=sizeof(DBR_GP_RegisterProfile)) return false;

		//请求处理
		DBR_GP_RegisterProfile * pRegisterProfile=(DBR_GP_RegisterProfile *)pData;

		// 检查身份证号是否满了18岁，不在客户端做判断
		TCHAR szDate[9] = {0};
		int nYear = 0;
		int nMonth = 0;
		int nDay = 0;
		TCHAR szYear[10]={0};
		TCHAR szMonth[10]={0};
		TCHAR szDay[10]={0};
		CString cstrPassPortID(pRegisterProfile->szPassPortID);
		if ( cstrPassPortID.GetLength() == 15 )
		{
			for(BYTE i = 0; i < 4; i++)
			{
				if (i == 0)
				{
					szYear[i] = TEXT('1');
				}
				else if ( i == 1)
				{
					szYear[i] = TEXT('9');
				}
				else
				{
					szYear[i] = pRegisterProfile->szPassPortID[i+4];
				}
			}
			for (BYTE i = 0; i < 2; i++)
			{
				szMonth[i] = pRegisterProfile->szPassPortID[i+10];	
			}
			for (BYTE i = 0; i < 2; i++)
			{
				szDay[i] = pRegisterProfile->szPassPortID[i+12];
			}
		}
		else if ( cstrPassPortID.GetLength() == 18 )
		{
			for(BYTE i = 0; i < 4; i++)
			{
				szYear[i] = pRegisterProfile->szPassPortID[i+6];
			}
			for (BYTE i = 0; i < 2; i++)
			{
				szMonth[i] = pRegisterProfile->szPassPortID[i+10];	
			}
			for (BYTE i = 0; i < 2; i++)
			{
				szDay[i] = pRegisterProfile->szPassPortID[i+12];
			}
		}
		// 和本地时间比对
		nYear = StrToInt(szYear);
		nMonth = StrToInt(szMonth);
		nDay = StrToInt(szDay);
		SYSTEMTIME st;
		GetLocalTime(&st);
		if ( (st.wYear<nYear+18) || (st.wYear==nYear+18 && st.wMonth < nMonth) || (st.wYear==nYear+18 && st.wMonth==nMonth && st.wDay<nDay) )
		{
			//变量定义
			DBO_GP_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//构造数据
			LogonFailure.lResultCode=1;
			lstrcpyn(LogonFailure.szDescribeString, _T("您未满十八周岁，不能注册账号！"), CountArray(LogonFailure.szDescribeString));

			//发送结果
			WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
			WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PROFILE_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
			return true;
		}

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pRegisterProfile->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pRegisterProfile->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pRegisterProfile->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pRegisterProfile->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pRegisterProfile->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pRegisterProfile->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strQQ"),pRegisterProfile->szQQ);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pRegisterProfile->szMobile);
		m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"),pRegisterProfile->szUnderWrite);
		m_AccountsDBAide.AddParameter(TEXT("@strPassPortID"),pRegisterProfile->szPassPortID);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pRegisterProfile->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@strSecurityQuestion"),pRegisterProfile->szSecurityQuestion);
		m_AccountsDBAide.AddParameter(TEXT("@strSecurityAnswer"),pRegisterProfile->szSecurityAnswer);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RegisterProfile"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		if(lResultCode==DB_SUCCESS)
		{
			DBO_GP_UserProfile UserProfile;
			UserProfile.dwUserID = pRegisterProfile->dwUserID;
			UserProfile.cbGender = pRegisterProfile->cbGender;
			UserProfile.wFaceID  = pRegisterProfile->wFaceID;

			lstrcpyn(UserProfile.szNickName, pRegisterProfile->szNickName, CountArray(UserProfile.szNickName));
			lstrcpyn(UserProfile.szQQ, pRegisterProfile->szQQ, CountArray(UserProfile.szQQ));
			lstrcpyn(UserProfile.szMobile, pRegisterProfile->szMobile, CountArray(UserProfile.szMobile));
			lstrcpyn(UserProfile.szUnderWrite, pRegisterProfile->szUnderWrite, CountArray(UserProfile.szUnderWrite));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PROFILE_SUCCESS,dwContextID,&UserProfile,sizeof(UserProfile));
		}
		else
		{
			//变量定义
			DBO_GP_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//构造数据
			LogonFailure.lResultCode=lResultCode;
			lstrcpyn(LogonFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(LogonFailure.szDescribeString));

			//发送结果
			WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
			WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_PROFILE_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}


// 检测昵称
bool CDataBaseEngineSink::OnRequestCheckNick(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_CheckNick));
		if (wDataSize!=sizeof(DBR_GP_CheckNick)) return false;

		//请求处理
		DBR_GP_CheckNick * pCheckNick =(DBR_GP_CheckNick *)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pCheckNick->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pCheckNick->szNickName);
	
		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CheckNick"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//构造数据
		DBO_GP_CheckNick CheckNick;
		CheckNick.lResultCode=lResultCode;
		lstrcpyn(CheckNick.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(CheckNick.szDescribeString));		
	
		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CHECKNICK,dwContextID,&CheckNick,sizeof(CheckNick));
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理	
		DBO_GP_CheckNick CheckNick;
		CheckNick.lResultCode=DB_ERROR;
		lstrcpyn(CheckNick.szDescribeString,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(CheckNick.szDescribeString));		

		return false;
	}
	
	return true;
}

//I D 登录
bool CDataBaseEngineSink::OnMobileLogonGameID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_MB_LogonGameID));
		if (wDataSize!=sizeof(DBR_MB_LogonGameID)) return false;

		//执行查询
		DBR_MB_LogonGameID * pLogonGameID=(DBR_MB_LogonGameID *)pData;

		//变量定义
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonGameID->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonGameID->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwGameID"),pLogonGameID->dwGameID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonGameID->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonGameID->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pLogonGameID->szMobilePhone);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyGameID"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),true);

		return false;
	}

	return true;
}

//帐号登录
bool CDataBaseEngineSink::OnMobileLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_MB_LogonAccounts));
		if (wDataSize!=sizeof(DBR_MB_LogonAccounts)) return false;

		//请求处理
		DBR_MB_LogonAccounts * pLogonAccounts=(DBR_MB_LogonAccounts *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pLogonAccounts->szMobilePhone);
//		m_AccountsDBAide.AddParameter(TEXT("@cbClientKind"),pBindParameter->cbClientKind);
		m_AccountsDBAide.AddParameter(TEXT("@wPlatformID"), pBindParameter->wPlatformID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyAccounts"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),true);

		return false;
	}

	return true;
}

//帐号注册
bool CDataBaseEngineSink::OnMobileRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_MB_RegisterAccounts));
		if (wDataSize!=sizeof(DBR_MB_RegisterAccounts)) return false;

		//请求处理
		DBR_MB_RegisterAccounts * pRegisterAccounts=(DBR_MB_RegisterAccounts *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pRegisterAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pRegisterAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pRegisterAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strLogonPass"),pRegisterAccounts->szLogonPass);
		m_AccountsDBAide.AddParameter(TEXT("@strInsurePass"),pRegisterAccounts->szInsurePass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pRegisterAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pRegisterAccounts->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@wPlatformID"), pBindParameter->wPlatformID);
		m_AccountsDBAide.AddParameter(TEXT("@dwAreaID"), pRegisterAccounts->dwAreaID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_RegisterAccounts"),true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),true);

		return false;
	}

	return true;
}

// 请求充值公告
bool CDataBaseEngineSink::OnMobileReqRechargeMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();
		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_ReChargeMessage"),true);
		if (lResultCode == DB_SUCCESS)
		{
			CMD_MB_SystemMessage SystemMessage;
			memset(&SystemMessage, 0, sizeof(SystemMessage));

			m_AccountsDBAide.GetValue_String(TEXT("StatusString"), SystemMessage.szString, CountArray(SystemMessage.szString));

			SystemMessage.wLength=lstrlen(SystemMessage.szString)+1;

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_RECHARGE_MESSAGE, dwContextID, &SystemMessage, sizeof(SystemMessage) );
		}
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}
	return true;
}
// 请求
bool CDataBaseEngineSink::OnMobileReqCustomServiceMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();
		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_CustomServiceMessage"),true);
		if (lResultCode == DB_SUCCESS)
		{
			CMD_MB_SystemMessage SystemMessage;
			memset(&SystemMessage, 0, sizeof(SystemMessage));

			m_AccountsDBAide.GetValue_String(TEXT("StatusString"), SystemMessage.szString, CountArray(SystemMessage.szString));

			SystemMessage.wLength=lstrlen(SystemMessage.szString)+1;

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_CUSTOM_SERVICE_MESSAGE, dwContextID, &SystemMessage, sizeof(SystemMessage) );
		}
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}
	return true;
}

// 第三方登录
bool CDataBaseEngineSink::OnThirdPartyLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_TP_LogonAccounts));
		if (wDataSize!=sizeof(DBR_TP_LogonAccounts)) return false;

		//请求处理
		DBR_TP_LogonAccounts * pLogonAccounts=(DBR_TP_LogonAccounts *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pLogonAccounts->pBindParameter;
		if (pBindParameter!=NULL&&pBindParameter->dwSocketID!=dwContextID) return true;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strSessionID"),pLogonAccounts->szSessionID);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pLogonAccounts->szNickName);
 		m_AccountsDBAide.AddParameter(TEXT("@dwThirdPartyID"),pLogonAccounts->nThirdPartyID);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pLogonAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonAccounts->szMachineID);
 		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
// 		m_AccountsDBAide.AddParameter(TEXT("@cbClientKind"),pBindParameter->cbClientKind);
		m_AccountsDBAide.AddParameter(TEXT("@wPlatformID"), pBindParameter->wPlatformID);
		m_AccountsDBAide.AddParameter(TEXT("@dwAreaID"), pLogonAccounts->dwAreaID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		LOGI("CDataBaseEngineSink::OnThirdPartyLogonAccounts, ExecuteProcess Begin="<<GetTickCount());
		//执行查询
		LONG lResultCode=0;
#ifdef TEST_KERNELENGINE
		lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("QPAccountsDB.dbo.GSP_GP_EfficacyAccounts"),true);
#else
		lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyAccountsThirdParty"),true);
#endif

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),true);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),true);
	}

	return true;
}

// 分享成功
bool CDataBaseEngineSink::OnShareSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_MB_ShareSuccess));
		if (wDataSize!=sizeof(DBR_MB_ShareSuccess)) return false;

		//请求处理
		DBR_MB_ShareSuccess * pShareSuccess=(DBR_MB_ShareSuccess *)pData;

		//执行判断
		tagBindParameter * pBindParameter=(tagBindParameter *)pShareSuccess->pBindParameter;
		if (pBindParameter!=NULL&&pBindParameter->dwSocketID!=dwContextID)
		{
			return true;
		}

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pShareSuccess->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
 		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pShareSuccess->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"), pShareSuccess->szMachineID);
 		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=0;
#ifdef TEST_KERNELENGINE
		lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("QPTreasureDB.dbo.GSP_MB_ShareSuccess"),true);
#else
		lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_MB_ShareSuccess"), true);
#endif

		//结果处理
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),true);

	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnOperateDisposeResult(dwContextID, DB_ERROR, TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);
	}

	return true;
}

// 用户加入自定义桌子
bool CDataBaseEngineSink::OnUserJoinCustomTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_UserJoinCustomTable));
		if (wDataSize != sizeof(DBR_MB_UserJoinCustomTable)) return false;

		//请求处理
		DBR_MB_UserJoinCustomTable * pUserJoinCustomTable = (DBR_MB_UserJoinCustomTable *)pData;

		//构造参数
		m_UserCustomDBAide.ResetParameter();
		m_UserCustomDBAide.AddParameter(TEXT("@dwUserID"), pUserJoinCustomTable->dwUserID);
		m_UserCustomDBAide.AddParameter(TEXT("@strSecret"), pUserJoinCustomTable->szSecret);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_UserCustomDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = 0;
		lResultCode = m_UserCustomDBAide.ExecuteProcess(TEXT("GSP_LS_UserJoinCustomTable"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_UserCustomDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnUserCustomTableDisposeResult(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal), true);

	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//错误处理
		OnUserCustomTableDisposeResult(dwContextID, DB_ERROR, TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);
	}

	return true;
}

// 用户查询加入自定义桌子的信息
bool CDataBaseEngineSink::OnUserQueryCustomTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_UserQueryCustomTable));
		if (wDataSize != sizeof(DBR_MB_UserQueryCustomTable)) return false;

		//请求处理
		DBR_MB_UserQueryCustomTable * pUserQueryCustomTable = (DBR_MB_UserQueryCustomTable *)pData;

		//构造参数
		m_UserCustomDBAide.ResetParameter();
		m_UserCustomDBAide.AddParameter(TEXT("@dwUserID"), pUserQueryCustomTable->dwUserID);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_UserCustomDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = 0;
		lResultCode = m_UserCustomDBAide.ExecuteProcess(TEXT("GSP_GR_QueryUserJoinCustomTable"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_UserCustomDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnUserCustomTableDisposeResult(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal), true);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//错误处理
		OnUserCustomTableDisposeResult(dwContextID, DB_ERROR, TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);
	}
	return true;
}

// 用户转账钻石
bool CDataBaseEngineSink::OnRequestUserTransferDiamond(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_UserTransferDiamond));
		if (wDataSize != sizeof(DBR_MB_UserTransferDiamond)) return false;

		//请求处理
		DBR_MB_UserTransferDiamond * pUserTransferDiamond = (DBR_MB_UserTransferDiamond *)pData;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE * pClientAddr = (BYTE *)&pUserTransferDiamond->dwClientAddr;
		_sntprintf(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_UserCustomDBAide.ResetParameter();
		m_UserCustomDBAide.AddParameter(TEXT("@dwUserID"), pUserTransferDiamond->dwUserID);
		m_UserCustomDBAide.AddParameter(TEXT("@dwGameID"), pUserTransferDiamond->dwGameID);
		m_UserCustomDBAide.AddParameter(TEXT("@lTransferDiamond"), pUserTransferDiamond->dwTransferDiamond);
		m_UserCustomDBAide.AddParameter(TEXT("@strPassword"), pUserTransferDiamond->szPassword);
		m_UserCustomDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_UserCustomDBAide.AddParameter(TEXT("@strMachineID"), pUserTransferDiamond->szMachineID);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_UserCustomDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = 0;
		lResultCode = m_UserCustomDBAide.ExecuteProcess(TEXT("GSP_GR_UserTransferDiamond"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_UserCustomDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnDiamondDisposeResult(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//错误处理
		OnDiamondDisposeResult(dwContextID, DB_ERROR, TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"));
	}

	return true;
}

//修改机器
bool CDataBaseEngineSink::OnRequestModifyMachine(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyMachine));
		if (wDataSize!=sizeof(DBR_GP_ModifyMachine)) return false;

		//请求处理
		DBR_GP_ModifyMachine * pModifyMachine=(DBR_GP_ModifyMachine *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyMachine->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyMachine->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyMachine->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifyMachine->szMachineID);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//绑定操作
		if (pModifyMachine->cbBind==TRUE)
		{
			m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_MoorMachine"),false);
		}
		else
		{
			m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UnMoorMachine"),false);
		}

		//结果处理
		if (m_AccountsDBAide.GetReturnValue()==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateSuccess.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateSuccess.szDescribeString));

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wSendSize);
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.lResultCode=m_AccountsDBModule->GetReturnValue();
			lstrcpyn(OperateFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(OperateFailure.szDescribeString));

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改头像
bool CDataBaseEngineSink::OnRequestModifySystemFace(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifySystemFace));
		if (wDataSize!=sizeof(DBR_GP_ModifySystemFace)) return false;

		//请求处理
		DBR_GP_ModifySystemFace * pModifySystemFace=(DBR_GP_ModifySystemFace *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifySystemFace->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifySystemFace->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifySystemFace->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pModifySystemFace->wFaceID);

		//机器信息
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifySystemFace->szMachineID);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_SystemFaceInsert"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserFaceInfo UserFaceInfo;
			ZeroMemory(&UserFaceInfo,sizeof(UserFaceInfo));

			//读取信息
			UserFaceInfo.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_FACE_INFO,dwContextID,&UserFaceInfo,sizeof(UserFaceInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//结果处理
			OnOperateDisposeResult(dwContextID,m_AccountsDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改头像
bool CDataBaseEngineSink::OnRequestModifyCustomFace(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyCustomFace));
		if (wDataSize!=sizeof(DBR_GP_ModifyCustomFace)) return false;

		//请求处理
		DBR_GP_ModifyCustomFace * pModifyCustomFace=(DBR_GP_ModifyCustomFace *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyCustomFace->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//变量定义
		SAFEARRAYBOUND SafeArrayBound;
		ZeroMemory(&SafeArrayBound,sizeof(SafeArrayBound));

		//设置变量
		SafeArrayBound.lLbound=0L;
		SafeArrayBound.cElements=sizeof(pModifyCustomFace->dwCustomFace);

		//变量定义
		BYTE * pcbCustomFace=(BYTE *)pModifyCustomFace->dwCustomFace;
		SAFEARRAY * pSafeArray=SafeArrayCreate(VT_UI1,1,&SafeArrayBound);

		//创建数组
		for (LONG nIndex=0;nIndex<sizeof(pModifyCustomFace->dwCustomFace);nIndex++)
		{
			SafeArrayPutElement(pSafeArray,&nIndex,pcbCustomFace+nIndex);
		}

		//设置变量
		VARIANT VarChunk;
		VariantClear(&VarChunk);
		VarChunk.parray=pSafeArray;
		VarChunk.vt=VT_ARRAY|VT_UI1;

		//获取对象
		ASSERT(m_AccountsDBModule.GetInterface()!=NULL);
		IDataBase * pIDataBase=m_AccountsDBModule.GetInterface();

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyCustomFace->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyCustomFace->szPassword);

		//头像信息
		pIDataBase->AddParameter(TEXT("@dwCustomFace"),adLongVarBinary,adParamInput,sizeof(pModifyCustomFace->dwCustomFace),CDBVarValue(VarChunk));

		//机器信息
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pModifyCustomFace->szMachineID);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//释放资源
		SafeArrayDestroy(pSafeArray);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CustomFaceInsert"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserFaceInfo UserFaceInfo;
			ZeroMemory(&UserFaceInfo,sizeof(UserFaceInfo));

			//读取信息
			UserFaceInfo.wFaceID=INVALID_WORD;
			UserFaceInfo.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_FACE_INFO,dwContextID,&UserFaceInfo,sizeof(UserFaceInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//结果处理
			OnOperateDisposeResult(dwContextID,m_AccountsDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改密码
bool CDataBaseEngineSink::OnRequestModifyLogonPass(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyLogonPass));
		if (wDataSize!=sizeof(DBR_GP_ModifyLogonPass)) return false;

		//请求处理
		DBR_GP_ModifyLogonPass * pModifyLogonPass=(DBR_GP_ModifyLogonPass *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyLogonPass->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyLogonPass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"),pModifyLogonPass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"),pModifyLogonPass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyLogonPassword"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改密码
bool CDataBaseEngineSink::OnRequestModifyInsurePass(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyInsurePass));
		if (wDataSize!=sizeof(DBR_GP_ModifyInsurePass)) return false;

		//请求处理
		DBR_GP_ModifyInsurePass * pModifyInsurePass=(DBR_GP_ModifyInsurePass *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyInsurePass->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyInsurePass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"),pModifyInsurePass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"),pModifyInsurePass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//结果处理
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyInsurePassword"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改签名
bool CDataBaseEngineSink::OnRequestModifyUnderWrite(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyUnderWrite));
		if (wDataSize!=sizeof(DBR_GP_ModifyUnderWrite)) return false;

		//请求处理
		DBR_GP_ModifyUnderWrite * pModifyUnderWrite=(DBR_GP_ModifyUnderWrite *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyUnderWrite->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyUnderWrite->dwUserID);
//		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyUnderWrite->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"),pModifyUnderWrite->szUnderWrite);
//		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUnderWrite"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//修改资料
bool CDataBaseEngineSink::OnRequestModifyIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyIndividual));
		if (wDataSize!=sizeof(DBR_GP_ModifyIndividual)) return false;

		//请求处理
		DBR_GP_ModifyIndividual * pModifyIndividual=(DBR_GP_ModifyIndividual *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pModifyIndividual->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyIndividual->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pModifyIndividual->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pModifyIndividual->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"),pModifyIndividual->szUnderWrite);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pModifyIndividual->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@strQQ"),pModifyIndividual->szQQ);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"),pModifyIndividual->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strSeatPhone"),pModifyIndividual->szSeatPhone);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pModifyIndividual->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strDwellingPlace"),pModifyIndividual->szDwellingPlace);
		m_AccountsDBAide.AddParameter(TEXT("@strUserNote"),pModifyIndividual->szUserNote);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出变量
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//执行查询
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUserIndividual"),false);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnOperateDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//存入游戏币
bool CDataBaseEngineSink::OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_UserSaveScore));
		if (wDataSize!=sizeof(DBR_GP_UserSaveScore)) return false;

		//请求处理
		DBR_GP_UserSaveScore * pUserSaveScore=(DBR_GP_UserSaveScore *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserSaveScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserSaveScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lSaveScore"),pUserSaveScore->lSaveScore);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),0L);
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
		OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//提取游戏币
bool CDataBaseEngineSink::OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_UserTakeScore));
		if (wDataSize!=sizeof(DBR_GP_UserTakeScore)) return false;

		//请求处理
		DBR_GP_UserTakeScore * pUserTakeScore=(DBR_GP_UserTakeScore *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTakeScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserTakeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lTakeScore"),pUserTakeScore->lTakeScore);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pUserTakeScore->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),0L);
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
		OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//转账游戏币
bool CDataBaseEngineSink::OnRequestUserTransferScore(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_UserTransferScore));
		if (wDataSize!=sizeof(DBR_GP_UserTransferScore)) return false;

		//请求处理
		DBR_GP_UserTransferScore * pUserTransferScore=(DBR_GP_UserTransferScore *)pData;

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
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),0L);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserTransferScore->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserTransferScore"),true);

		//结果处理
//		CDBVarValue DBVarValue;
//		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
//		OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
		
		LOGI("CDataBaseEngineSink::OnRequestUserTransferScore, lResultCode="<<lResultCode);
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
			UserTransferReceipt.lReceviceScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("ReceiveScore"));						// 收账方实收金额

			UserTransferReceipt.lSourceUserScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("SourceScore"));
			UserTransferReceipt.lSourceUserInsure = m_TreasureDBAide.GetValue_LONGLONG(TEXT("SourceInsure"));

			UserTransferReceipt.dwLoveliness = m_TreasureDBAide.GetValue_DWORD(TEXT("LoveLiness"));

			m_TreasureDBAide.GetValue_SystemTime(TEXT("stTransferTime"), UserTransferReceipt.stTransferTime);					// 转账时间

//			SCORE lTest = m_TreasureDBAide.GetValue_LONGLONG(TEXT("KickbackRevenue"));

			LOGI("CDataBaseEngineSink::OnRequestUserTransferScore, dwRecordID="<<UserTransferReceipt.dwRecordID<<", dwSourceUserID="<<UserTransferReceipt.dwSourceUserID<<\
				", dwTargetUserID="<<UserTransferReceipt.dwTargetUserID<<", dwSourceGameID="<<UserTransferReceipt.dwSourceGameID<<", dwTargetGameID="<<UserTransferReceipt.dwTargetGameID<<\
				", szSourceNickName="<<UserTransferReceipt.szSourceNickName<<", szTargetNickName="<<UserTransferReceipt.szTargetNickName<<\
				", lTransferScore="<<UserTransferReceipt.lTransferScore<<", lRevenueScore="<<UserTransferReceipt.lRevenueScore<<\
				", dwLoveliness="<<UserTransferReceipt.dwLoveliness<<", lSourceUserScore="<<UserTransferReceipt.lSourceUserScore<<\
				", SourceInsure="<<UserTransferReceipt.lSourceUserInsure);

			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			LOGI("CDataBaseEngineSink::OnRequestUserTransferScore, DBVarValue="<<CW2CT(DBVarValue.bstrVal));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_INSURE_TRANS_RECEIPT, dwContextID, &UserTransferReceipt, sizeof(UserTransferReceipt) );
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			LOGI("CDataBaseEngineSink::OnRequestUserTransferScore, DBVarValue="<<CW2CT(DBVarValue.bstrVal));

			//错误处理
			OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);
		}
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

// 验证银行密码
bool CDataBaseEngineSink::OnRequestVerifyInsurePassword(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_VerifyInsurePassword));
	if (wDataSize!=sizeof(DBR_GP_VerifyInsurePassword)) return false;

	//请求处理
	DBR_GP_VerifyInsurePassword * pUserTransferScore=(DBR_GP_VerifyInsurePassword *)pData;
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
		OnInsureDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//查询资料
bool CDataBaseEngineSink::OnRequestQueryIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_QueryIndividual));
		if (wDataSize!=sizeof(DBR_GP_QueryIndividual)) return false;

		//请求处理
		DBR_GP_QueryIndividual * pQueryIndividual=(DBR_GP_QueryIndividual *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryIndividual->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pQueryIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserIndividual"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_UserIndividual UserIndividual;
			ZeroMemory(&UserIndividual,sizeof(UserIndividual));

			//用户信息
			UserIndividual.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("UserNote"),UserIndividual.szUserNote,CountArray(UserIndividual.szUserNote));
			m_AccountsDBAide.GetValue_String(TEXT("Compellation"),UserIndividual.szCompellation,CountArray(UserIndividual.szCompellation));

			//电话号码
			m_AccountsDBAide.GetValue_String(TEXT("SeatPhone"),UserIndividual.szSeatPhone,CountArray(UserIndividual.szSeatPhone));
			m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),UserIndividual.szMobilePhone,CountArray(UserIndividual.szMobilePhone));

			//联系资料
			m_AccountsDBAide.GetValue_String(TEXT("QQ"),UserIndividual.szQQ,CountArray(UserIndividual.szQQ));
			m_AccountsDBAide.GetValue_String(TEXT("EMail"),UserIndividual.szEMail,CountArray(UserIndividual.szEMail));
			m_AccountsDBAide.GetValue_String(TEXT("DwellingPlace"),UserIndividual.szDwellingPlace,CountArray(UserIndividual.szDwellingPlace));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INDIVIDUAL,dwContextID,&UserIndividual,sizeof(UserIndividual));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//错误处理
			OnOperateDisposeResult(dwContextID,m_AccountsDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//查询银行
bool CDataBaseEngineSink::OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_QueryInsureInfo));
		if (wDataSize!=sizeof(DBR_GP_QueryInsureInfo)) return false;

		//请求处理
		DBR_GP_QueryInsureInfo * pQueryInsureInfo=(DBR_GP_QueryInsureInfo *)pData;

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
			DBO_GP_UserInsureInfo UserInsureInfo;
			ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

			//银行信息
			UserInsureInfo.wRevenueTake=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTake"));
			UserInsureInfo.wRevenueTransfer=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTransfer"));
			UserInsureInfo.wServerID=m_TreasureDBAide.GetValue_WORD(TEXT("ServerID"));
			UserInsureInfo.lUserScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Score"));
			UserInsureInfo.lUserInsure=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Insure"));
			UserInsureInfo.lTransferPrerequisite=m_TreasureDBAide.GetValue_LONGLONG(TEXT("TransferPrerequisite"));
			UserInsureInfo.dwDiamondCount = m_TreasureDBAide.GetValue_DWORD(TEXT("PropertyCount"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_INFO,dwContextID,&UserInsureInfo,sizeof(UserInsureInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//错误处理
			OnOperateDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}
//转账记录
bool CDataBaseEngineSink::OnRequestQueryTransRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_QueryInsureInfo));
		if (wDataSize!=sizeof(DBR_GP_QueryInsureInfo)) return false;

		//请求处理
		DBR_GP_QueryInsureInfo * pQueryInsureInfo=(DBR_GP_QueryInsureInfo *)pData;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryInsureInfo->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pQueryInsureInfo->dwUserID);
//		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
//		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTransferRecord"),true)==DB_SUCCESS)
		{
//			CDBVarValue DBVarValue;
//			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			DBR_GP_UserTransRecord UserTransRecord;
			ZeroMemory(&UserTransRecord, sizeof(UserTransRecord));
			INT nRecordCount=m_TreasureDBModule->GetRecordCount();
			LOGI(TEXT("CDataBaseEngineSink::OnRequestQueryTransRecord, nRecordCount="<<nRecordCount));
			//读取记录
			if(nRecordCount>0)
			{
				m_TreasureDBModule->MoveToFirst();
//				DWORD dw=1;
				while ( m_TreasureDBModule->IsRecordsetEnd() == false )
				{
//					DWORD dwTestUserID = 0;
//					dwTestUserID = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
//					UserTransRecord.dwGameID=dw++;
					UserTransRecord.bOver=false;
					m_TreasureDBAide.GetValue_String(TEXT("SourceNickName"),UserTransRecord.szSourceAccount,CountArray(UserTransRecord.szSourceAccount));
					m_TreasureDBAide.GetValue_String(TEXT("TargetNickName"),UserTransRecord.szTargetAccounts,CountArray(UserTransRecord.szTargetAccounts));
					UserTransRecord.dwGameID = m_TreasureDBAide.GetValue_DWORD(TEXT("RecordID"));
					UserTransRecord.lSwapScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("SwapScore"));
					UserTransRecord.lRevenue= m_TreasureDBAide.GetValue_LONGLONG(TEXT("Revenue"));
					UserTransRecord.dwSourceID=m_TreasureDBAide.GetValue_DWORD(TEXT("SourceGameID"));
					UserTransRecord.dwTargetID=m_TreasureDBAide.GetValue_DWORD(TEXT("TargetGameID"));

					CDBVarValue varvalue;
					m_TreasureDBAide.GetValue_VarValue(TEXT("CollectDate"),varvalue);

					COleDateTime time(varvalue);

					CString GetTime;
					GetTime.Format(_T("%d-%02d-%02d %02d:%02d:%02d"),time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(), time.GetSecond());
 					lstrcpyn(UserTransRecord.szTime,GetTime,sizeof(UserTransRecord.szTime));
 
 					//投递调度通知
 					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_TRANS_RECORD,dwContextID, &UserTransRecord, sizeof(UserTransRecord));
 
 					//移动记录
 					m_TreasureDBModule->MoveToNext();
 				}
 				UserTransRecord.bOver=true;
 				//发送结果
 				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_TRANS_RECORD,dwContextID, &UserTransRecord, sizeof(UserTransRecord));
 			}
 			else
 			{
 				UserTransRecord.bOver=true;
 				//发送结果
 				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_TRANS_RECORD,dwContextID, &UserTransRecord, sizeof(UserTransRecord));
 			}
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//错误处理
			OnOperateDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}
//查询用户
bool CDataBaseEngineSink::OnRequestQueryTransferUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_QueryInsureUserInfo));
	if (wDataSize!=sizeof(DBR_GP_QueryInsureUserInfo)) return false;

	//请求处理
	DBR_GP_QueryInsureUserInfo * pQueryTransferUserInfo=(DBR_GP_QueryInsureUserInfo *)pData;

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
			DBO_GP_UserTransferUserInfo TransferUserInfo;
			ZeroMemory(&TransferUserInfo,sizeof(TransferUserInfo));

			//银行信息
			TransferUserInfo.dwGameID=m_TreasureDBAide.GetValue_DWORD(TEXT("GameID"));
			m_TreasureDBAide.GetValue_String(TEXT("NickName"), TransferUserInfo.szNickName, CountArray(TransferUserInfo.szNickName));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_USER_INFO,dwContextID,&TransferUserInfo,sizeof(TransferUserInfo));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//错误处理
			OnInsureDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),CW2CT(DBVarValue.bstrVal),false);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		OnInsureDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}
}

// 根据GameID查询昵称
bool CDataBaseEngineSink::OnRequestQueryNickNameByGameID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_QueryNickNameByGameID));
	if (wDataSize!=sizeof(DBR_GP_QueryNickNameByGameID)) return false;

	//请求处理
	DBR_GP_QueryNickNameByGameID* pQueryNickNameByGameID=(DBR_GP_QueryNickNameByGameID *)pData;
	try
	{
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwGameID"),pQueryNickNameByGameID->dwGameID);

		//输出参数
//		TCHAR szDescribeString[128]=TEXT("");
//		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserNickNameByGameID"),true);

		//变量定义
		DBO_GP_UserTransferUserInfo TransferUserInfo;
		ZeroMemory(&TransferUserInfo,sizeof(TransferUserInfo));

		//银行信息
		TransferUserInfo.dwGameID=pQueryNickNameByGameID->dwGameID;
		m_AccountsDBAide.GetValue_String(TEXT("NickName"), TransferUserInfo.szNickName, CountArray(TransferUserInfo.szNickName));

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_USER_INFO,dwContextID,&TransferUserInfo,sizeof(TransferUserInfo));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		CString cstr;
		cstr.Format(_T("CDataBaseEngineSink::OnRequestQueryNickNameByGameID, dwGameID = %d, exception = %s"), pQueryNickNameByGameID->dwGameID, pIException->GetExceptionDescribe());
		//输出错误
		CTraceService::TraceString(cstr, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);
		return false;
	}
}

// 获取魅力兑换等信息
bool CDataBaseEngineSink::OnRequestQueryExchangeLoveliness(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_QueryExchangeLoveliness));
	if (wDataSize!=sizeof(DBR_GP_QueryExchangeLoveliness)) return false;

	//请求处理
	DBR_GP_QueryExchangeLoveliness* pQueryExchangeLoveliness=(DBR_GP_QueryExchangeLoveliness *)pData;

	try
	{
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pQueryExchangeLoveliness->dwUserID);

		TCHAR szTips[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strTips"),szTips,sizeof(szTips),adParamOutput);

		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryExchangeLoveliness"),true)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_ExchangeLovelinessInfo ExchangeLovelinessInfo;
			ZeroMemory(&ExchangeLovelinessInfo,sizeof(ExchangeLovelinessInfo));

			//银行信息
			ExchangeLovelinessInfo.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			ExchangeLovelinessInfo.dwLoveliness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
			ExchangeLovelinessInfo.dwAvailbleAmount=m_AccountsDBAide.GetValue_DWORD(TEXT("AvaibleAmount"));
			ExchangeLovelinessInfo.dwFinalAmount=m_AccountsDBAide.GetValue_DWORD(TEXT("FinalAmount"));
//			m_AccountsDBAide.GetValue_String(TEXT("Tips"),ExchangeLovelinessInfo.szTips,CountArray(ExchangeLovelinessInfo.szTips));

			CDBVarValue DBTipsVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strTips"),DBTipsVarValue);
			_sntprintf(ExchangeLovelinessInfo.szTips, sizeof(ExchangeLovelinessInfo.szTips), CW2CT(DBTipsVarValue.bstrVal));

			//获取参数
//			CDBVarValue DBVarValue;
//			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
//			_sntprintf(ExchangeLovelinessInfo.szErr, sizeof(ExchangeLovelinessInfo.szErr), CW2CT(DBVarValue.bstrVal));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_EXCHANGE_LOVELINESS_INFO,dwContextID,&ExchangeLovelinessInfo,sizeof(ExchangeLovelinessInfo));
		}
		else
		{
			//变量定义
			DBO_GP_ExchangeLovelinessInfo ExchangeLovelinessInfo;
			ZeroMemory(&ExchangeLovelinessInfo,sizeof(ExchangeLovelinessInfo));

			CDBVarValue DBTipsVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strTips"),DBTipsVarValue);
			_sntprintf(ExchangeLovelinessInfo.szTips, sizeof(ExchangeLovelinessInfo.szTips), CW2CT(DBTipsVarValue.bstrVal));

			//获取参数
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			_sntprintf(ExchangeLovelinessInfo.szErr, sizeof(ExchangeLovelinessInfo.szErr), CW2CT(DBVarValue.bstrVal));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_QUERY_EXCHANGE_LOVELINESS_INFO,dwContextID,&ExchangeLovelinessInfo,sizeof(ExchangeLovelinessInfo));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TCHAR szErr[2048] = TEXT("");
		_sntprintf(szErr, CountArray(szErr), TEXT("CDataBaseEngineSink::OnRequestQueryExchangeLoveliness, %s"), pIException->GetExceptionDescribe());
		//输出错误
		CTraceService::TraceString(szErr, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);
		return false;
	}
}

// 兑换魅力
bool CDataBaseEngineSink::OnRequestExchangeLoveliness(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GP_ExchangeLoveliness));
	if (wDataSize!=sizeof(DBR_GP_ExchangeLoveliness)) return false;

	//请求处理
	DBR_GP_ExchangeLoveliness* pExchangeLoveliness=(DBR_GP_ExchangeLoveliness *)pData;

	try
	{
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pExchangeLoveliness->dwUserID);

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pExchangeLoveliness->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//结果处理
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ExchangeLoveliness"),true);

		DBO_GP_ExchangeLoveliness ExchangeLoveliness;
		ZeroMemory( &ExchangeLoveliness, sizeof(ExchangeLoveliness) );
		if (lResultCode == DB_SUCCESS)
		{
			ExchangeLoveliness.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			ExchangeLoveliness.dwLoveliness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
			ExchangeLoveliness.lScore=m_AccountsDBAide.GetValue_DWORD(TEXT("Score"));
			ExchangeLoveliness.lInsureScore=m_AccountsDBAide.GetValue_DWORD(TEXT("InsureScore"));
		}
		
		ExchangeLoveliness.lResultCode = lResultCode;
		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);	
		_sntprintf(ExchangeLoveliness.szDescribeString, sizeof(ExchangeLoveliness.szDescribeString), CW2CT(DBVarValue.bstrVal));

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_EXCHANGE_LOVELINESS,dwContextID,&ExchangeLoveliness,sizeof(ExchangeLoveliness));
	}
	catch (IDataBaseException * pIException)
	{
		TCHAR szErr[2048] = TEXT("");
		_sntprintf(szErr, CountArray(szErr), TEXT("CDataBaseEngineSink::OnRequestExchangeLoveliness, %s"), pIException->GetExceptionDescribe());
		//输出错误
		CTraceService::TraceString(szErr, TraceLevel_Exception);

		//结果处理
		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);
		return false;
	}
	return true;
}

// 签到
bool CDataBaseEngineSink::OnRequestSignInCheck(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_MB_SiginInCheck));
	if (wDataSize!=sizeof(DBR_MB_SiginInCheck)) return false;

	//请求处理
	DBR_MB_SiginInCheck* pExchangeLoveliness=(DBR_MB_SiginInCheck *)pData;

	try
	{
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pExchangeLoveliness->dwUserID);

		//转化地址
// 		TCHAR szClientAddr[16]=TEXT("");
// 		BYTE * pClientAddr=(BYTE *)&pExchangeLoveliness->dwClientAddr;
// 		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);
// 		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
 
 		TCHAR szDescribeString[128]=TEXT("");
 		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

 		//结果处理
 		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_SignInCheck"),true);

		DBO_MB_SignInCheck SignInCheck;
		memset(&SignInCheck, 0, sizeof(SignInCheck));
		SignInCheck.dwErrorCode = lResultCode;
		while (m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
// 			if ((wPacketSize+sizeof(DBO_GP_GameKind))>sizeof(cbBuffer))
// 			{
// 				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);
// 				wPacketSize=0;
// 			}

			//读取信息
// 			pGameKind=(DBO_GP_GameKind *)(cbBuffer+wPacketSize);
// 			pGameKind->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
// 			pGameKind->wTypeID=m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
// 			pGameKind->wJoinID=m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
// 			pGameKind->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
// 			pGameKind->wGameID=m_PlatformDBAide.GetValue_WORD(TEXT("GameID"));
			INT times = m_AccountsDBAide.GetValue_INT(TEXT("Times"));
			SYSTEMTIME systemtime;
			m_AccountsDBAide.GetValue_SystemTime(TEXT("DayTime"), systemtime);

			//设置位移
			SignInCheck.dwCheckDay |= 1 << (systemtime.wDay-1);

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_SIGN_IN_CHECK_SUCCESS, dwContextID, &SignInCheck, sizeof(SignInCheck));
	}
	catch (IDataBaseException * pIException)
	{
// 		TCHAR szErr[2048] = TEXT("");
// 		_sntprintf(szErr, CountArray(szErr), TEXT("CDataBaseEngineSink::OnRequestExchangeLoveliness, %s"), pIException->GetExceptionDescribe());
 		//输出错误
// 		CTraceService::TraceString(szErr, TraceLevel_Exception);
// 
// 		//结果处理
// 		OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("由于数据库操作异常，请您稍后重试！"),false);
		return false;
	}

	return true;
}

//加载列表
bool CDataBaseEngineSink::OnRequestLoadGameList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA];

		//加载类型
		m_PlatformDBAide.ResetParameter();
#ifdef TEST_KERNELENGINE
		m_PlatformDBAide.ExecuteProcess(TEXT("QPPlatformDB.GSP_GP_LoadGameTypeItem"),true);
#else
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameTypeItem"),true);
#endif
		

		//发送种类
		wPacketSize=0;
		DBO_GP_GameType * pGameType=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GameType))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGameType=(DBO_GP_GameType *)(cbBuffer+wPacketSize);
			pGameType->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameType->wJoinID=m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameType->wTypeID=m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			m_PlatformDBAide.GetValue_String(TEXT("TypeName"),pGameType->szTypeName,CountArray(pGameType->szTypeName));

			//设置位移
			wPacketSize+=sizeof(DBO_GP_GameType);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//读取类型
		m_PlatformDBAide.ResetParameter();
#ifdef TEST_KERNELENGINE
		m_PlatformDBAide.ExecuteProcess(TEXT("QPPlatformDB.GSP_GP_LoadGameKindItem"),true);
#else
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameKindItem"),true);
#endif

		//发送类型
		wPacketSize=0;
		DBO_GP_GameKind * pGameKind=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GameKind))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGameKind=(DBO_GP_GameKind *)(cbBuffer+wPacketSize);
			pGameKind->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameKind->wTypeID=m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			pGameKind->wJoinID=m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameKind->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameKind->wGameID=m_PlatformDBAide.GetValue_WORD(TEXT("GameID"));
			m_PlatformDBAide.GetValue_String(TEXT("KindName"),pGameKind->szKindName,CountArray(pGameKind->szKindName));
			m_PlatformDBAide.GetValue_String(TEXT("ProcessName"),pGameKind->szProcessName,CountArray(pGameKind->szProcessName));

			//设置位移
			wPacketSize+=sizeof(DBO_GP_GameKind);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);

		//读取节点
		m_PlatformDBAide.ResetParameter();
#ifdef TEST_KERNELENGINE
		m_PlatformDBAide.ExecuteProcess(TEXT("QPPlatformDB.GSP_GP_LoadGameNodeItem"),true);
#else
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameNodeItem"),true);
#endif

		//发送节点
		wPacketSize=0;
		DBO_GP_GameNode * pGameNode=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GameNode))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGameNode=(DBO_GP_GameNode *)(cbBuffer+wPacketSize);
			pGameNode->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameNode->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameNode->wJoinID=m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameNode->wNodeID=m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			m_PlatformDBAide.GetValue_String(TEXT("NodeName"),pGameNode->szNodeName,CountArray(pGameNode->szNodeName));
			
			//设置位移
			wPacketSize+=sizeof(DBO_GP_GameNode);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//读取定制
		m_PlatformDBAide.ResetParameter();
#ifdef TEST_KERNELENGINE
		m_PlatformDBAide.ExecuteProcess(TEXT("QPPlatformDB.GSP_GP_LoadGamePageItem"),true);
#else
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGamePageItem"),true);
#endif

		//发送定制
		wPacketSize=0;
		DBO_GP_GamePage * pGamePage=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//发送信息
			if ((wPacketSize+sizeof(DBO_GP_GamePage))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PAGE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//读取信息
			pGamePage=(DBO_GP_GamePage *)(cbBuffer+wPacketSize);
			pGamePage->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGamePage->wNodeID=m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			pGamePage->wSortID=m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGamePage->wPageID=m_PlatformDBAide.GetValue_WORD(TEXT("PageID"));
			pGamePage->wOperateType=m_PlatformDBAide.GetValue_WORD(TEXT("OperateType"));
			m_PlatformDBAide.GetValue_String(TEXT("DisplayName"),pGamePage->szDisplayName,CountArray(pGamePage->szDisplayName));
			
			//设置位移
			wPacketSize+=sizeof(DBO_GP_GamePage);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PAGE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//读取区域游戏
		m_PlatformDBAide.ResetParameter();
#ifdef TEST_KERNELENGINE
		m_PlatformDBAide.ExecuteProcess(TEXT("QPPlatformDB.GSP_GP_LoadGamePageItem"), true);
#else
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAreaGameKindItem"), true);
#endif
		//发送区域游戏
		wPacketSize = 0;
		DBO_MB_AreaGameKind * pAreaGameKind = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//发送信息
			if ((wPacketSize + sizeof(DBO_MB_AreaGameKind)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_AREA_GAME_KIND_ITEM, dwContextID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//读取信息
			pAreaGameKind = (DBO_MB_AreaGameKind *)(cbBuffer + wPacketSize);
			pAreaGameKind->dwAreaID = m_PlatformDBAide.GetValue_DWORD(TEXT("AreaID"));
			pAreaGameKind->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pAreaGameKind->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pAreaGameKind->wPlatformID = m_PlatformDBAide.GetValue_WORD(TEXT("PlatformID"));

			//设置位移
			wPacketSize += sizeof(DBO_MB_AreaGameKind);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_AREA_GAME_KIND_ITEM, dwContextID, cbBuffer, wPacketSize);

		//读取区域房间
		m_PlatformDBAide.ResetParameter();
#ifdef TEST_KERNELENGINE
		m_PlatformDBAide.ExecuteProcess(TEXT("QPPlatformDB.GSP_GP_LoadGamePageItem"), true);
#else
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadAreaGameServerItem"), true);
#endif
		//发送区域房间
		wPacketSize = 0;
		DBO_MB_AreaGameServer * pAreaGameServer = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//发送信息
			if ((wPacketSize + sizeof(DBO_MB_AreaGameServer)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_AREA_GAME_SERVER_ITEM, dwContextID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//读取信息
			pAreaGameServer = (DBO_MB_AreaGameServer *)(cbBuffer + wPacketSize);
			pAreaGameServer->dwAreaID = m_PlatformDBAide.GetValue_DWORD(TEXT("AreaID"));
			pAreaGameServer->wServerID = m_PlatformDBAide.GetValue_WORD(TEXT("ServerID"));
			pAreaGameServer->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			m_PlatformDBAide.GetValue_String(TEXT("ServerName"), pAreaGameServer->szServerName, CountArray(pAreaGameServer->szServerName));
			pAreaGameServer->wPlatformID = m_PlatformDBAide.GetValue_WORD(TEXT("PlatformID"));

			//设置位移
			wPacketSize += sizeof(DBO_MB_AreaGameServer);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_AREA_GAME_SERVER_ITEM, dwContextID, cbBuffer, wPacketSize);

		//读取游戏扩展信息
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameKindItemEx"), true);
		//发送游戏扩展信息
		wPacketSize = 0;
		DBO_MB_GameKindEx * pGameKindEx = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//发送信息
			if ((wPacketSize + sizeof(DBO_MB_GameKindEx)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_GAME_KIND_ITEM_EX, dwContextID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//读取信息
			pGameKindEx = (DBO_MB_GameKindEx *)(cbBuffer + wPacketSize);
			pGameKindEx->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameKindEx->dwAppVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("AppClientVersion"));
			m_PlatformDBAide.GetValue_String(TEXT("AppDownLoadUrl"), pGameKindEx->szAppDownloadURL, CountArray(pGameKindEx->szAppDownloadURL));

			//设置位移
			wPacketSize += sizeof(DBO_MB_GameKindEx);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_GAME_KIND_ITEM_EX, dwContextID, cbBuffer, wPacketSize);

		// 读取自建桌子道具需求配置
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadCustomTablePropertyConfig"), true);
		//发送游戏扩展信息
		wPacketSize = 0;
		DBO_MB_CustomTablePropertyConfig * pPropertyConfig = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//发送信息
			if ((wPacketSize + sizeof(DBO_MB_CustomTablePropertyConfig)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_CUSTOMTABLE_PROPERTYCONFIG, dwContextID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//读取信息
			pPropertyConfig = (DBO_MB_CustomTablePropertyConfig *)(cbBuffer + wPacketSize);
			pPropertyConfig->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pPropertyConfig->wPropertyConsumeCount = m_PlatformDBAide.GetValue_DWORD(TEXT("PropertyConsumeCount"));
			pPropertyConfig->wRoundCount = m_PlatformDBAide.GetValue_DWORD(TEXT("RoundCount"));

			//设置位移
			wPacketSize += sizeof(DBO_MB_CustomTablePropertyConfig);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_CUSTOMTABLE_PROPERTYCONFIG, dwContextID, cbBuffer, wPacketSize);

		//变量定义
		DBO_GP_GameListResult GameListResult;
		ZeroMemory(&GameListResult,sizeof(GameListResult));

		//设置变量
		GameListResult.cbSuccess=TRUE;

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_LIST_RESULT,dwContextID,&GameListResult,sizeof(GameListResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//变量定义
		DBO_GP_GameListResult GameListResult;
		ZeroMemory(&GameListResult,sizeof(GameListResult));

		//设置变量
		GameListResult.cbSuccess=FALSE;

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_LIST_RESULT,dwContextID,&GameListResult,sizeof(GameListResult));

		return false;
	}

	return true;
}

//在线信息
bool CDataBaseEngineSink::OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		DBR_GP_OnLineCountInfo * pOnLineCountInfo=(DBR_GP_OnLineCountInfo *)pData;
		WORD wHeadSize=(sizeof(DBR_GP_OnLineCountInfo)-sizeof(pOnLineCountInfo->OnLineCountKind));

		//效验数据
		ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOnLineCountInfo->wKindCount*sizeof(tagOnLineInfoKind))));
		if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOnLineCountInfo->wKindCount*sizeof(tagOnLineInfoKind)))) return false;

		//机器标识
		TCHAR szMachineID[LEN_MACHINE_ID];
		CWHService::GetMachineID(szMachineID);

		//构造信息
		TCHAR szOnLineCountKind[2048]=TEXT("");
		for (WORD i=0;i<pOnLineCountInfo->wKindCount;i++)
		{
			INT nLength=lstrlen(szOnLineCountKind);
			_sntprintf(&szOnLineCountKind[nLength],CountArray(szOnLineCountKind)-nLength,TEXT("%d:%ld;"),pOnLineCountInfo->OnLineCountKind[i].wKindID,
				pOnLineCountInfo->OnLineCountKind[i].dwOnLineCount);
		}

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),szMachineID);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineServer"),m_pInitParameter->m_szServerName);
		m_PlatformDBAide.AddParameter(TEXT("@dwOnLineCountSum"),pOnLineCountInfo->dwOnLineCountSum);
		m_PlatformDBAide.AddParameter(TEXT("@strOnLineCountKind"),szOnLineCountKind);

		//执行命令
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_OnLineCountInfo"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//登录成功
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient)
{
	if (bMobileClient==false)
	{
		if (dwErrorCode==DB_SUCCESS)
		{
			LOGI("CDataBaseEngineSink::OnLogonDisposeResult, Begin="<<GetTickCount());
			//变量定义
			DBO_GP_LogonSuccess LogonSuccess;
			ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));

			//属性资料
			LOGI("CDataBaseEngineSink::OnLogonDisposeResult, GetValue Begin="<<GetTickCount());
			LogonSuccess.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			LogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			LogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			LogonSuccess.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
			LogonSuccess.dwUserMedal=m_AccountsDBAide.GetValue_DWORD(TEXT("UserMedal"));
			LogonSuccess.dwExperience=m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));
			LogonSuccess.dwLoveLiness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
			m_AccountsDBAide.GetValue_String(TEXT("Accounts"),LogonSuccess.szAccounts,CountArray(LogonSuccess.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),LogonSuccess.szNickName,CountArray(LogonSuccess.szNickName));

			//用户游戏币
			LogonSuccess.lUserScore=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			LogonSuccess.lUserInsure=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Insure"));

			//用户资料
			LogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			LogonSuccess.cbMoorMachine=m_AccountsDBAide.GetValue_BYTE(TEXT("MoorMachine"));
			
			//是否补充注册资料
			LogonSuccess.wFillIn=m_AccountsDBAide.GetValue_WORD(TEXT("FillIn"));
			m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),LogonSuccess.szUnderWrite,CountArray(LogonSuccess.szUnderWrite));

			//会员资料
			LogonSuccess.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			m_AccountsDBAide.GetValue_SystemTime(TEXT("MemberOverDate"),LogonSuccess.MemberOverDate);

			//获取信息
			lstrcpyn(LogonSuccess.szDescribeString,pszErrorString,CountArray(LogonSuccess.szDescribeString));

			LOGI("CDataBaseEngineSink::OnLogonDisposeResult, GetValue End="<<GetTickCount());
			//发送结果
			WORD wDataSize=CountStringBuffer(LogonSuccess.szDescribeString);
			WORD wHeadSize=sizeof(LogonSuccess)-sizeof(LogonSuccess.szDescribeString);
			LOGI("CDataBaseEngineSink::OnLogonDisposeResult, OnEventDataBaseResult Begin="<<GetTickCount());
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOGON_SUCCESS,dwContextID,&LogonSuccess,wHeadSize+wDataSize);
			LOGI("CDataBaseEngineSink::OnLogonDisposeResult, OnEventDataBaseResult End="<<GetTickCount());
		}
		else if(dwErrorCode == DB_NEEDMB)
		{
			//登录成功
			DBR_GP_ValidateMBCard ValidateMBCard;
			ZeroMemory(&ValidateMBCard,sizeof(ValidateMBCard));

			//读取变量
			ValidateMBCard.uMBCardID=m_AccountsDBAide.GetValue_UINT(TEXT("PasswordID"));

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_VALIDATE_MBCARD,dwContextID,&ValidateMBCard,sizeof(ValidateMBCard));
		}
		else
		{
			//变量定义
			DBO_GP_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//构造数据
			LogonFailure.lResultCode=dwErrorCode;
			lstrcpyn(LogonFailure.szDescribeString,pszErrorString,CountArray(LogonFailure.szDescribeString));

			// 打印日志
			LOGE("CDataBaseEngineSink::OnLogonDisposeResult Failed, dwErrorCode="<<dwErrorCode<<", szDescribeString="<<LogonFailure.szDescribeString);

			//发送结果
			WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
			WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
		}
	}
	else
	{
		if (dwErrorCode==DB_SUCCESS)
		{
			//变量定义
			DBO_MB_LogonSuccess LogonSuccess;
			ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));

			//属性资料
			LogonSuccess.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			LogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			LogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			LogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			LogonSuccess.dwExperience=m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));
			LogonSuccess.dwLoveLiness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
			LogonSuccess.lUserScore=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			LogonSuccess.lUserInsure=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Insure"));
			m_AccountsDBAide.GetValue_String(TEXT("Accounts"),LogonSuccess.szAccounts,CountArray(LogonSuccess.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),LogonSuccess.szNickName,CountArray(LogonSuccess.szNickName));

			//获取信息
			lstrcpyn(LogonSuccess.szDescribeString,pszErrorString,CountArray(LogonSuccess.szDescribeString));

			// 获取区域ID
			LogonSuccess.dwAreaID = m_AccountsDBAide.GetValue_DWORD(TEXT("AreaID"));
			LogonSuccess.wPlatformID = m_AccountsDBAide.GetValue_DWORD(TEXT("PlatformID"));

			LogonSuccess.cbMemberOrder = m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));

			// 自建桌子信息
			LogonSuccess.wServerID = m_AccountsDBAide.GetValue_DWORD(TEXT("ServerID"));
			m_AccountsDBAide.GetValue_String(TEXT("Secret"), LogonSuccess.szSecret, CountArray(LogonSuccess.szSecret));
			if (m_AccountsDBAide.GetValue_WORD(TEXT("LockedInTable")) == 0)
			{
				LogonSuccess.bLockedInTable = false;
			}
			else
			{
				LogonSuccess.bLockedInTable = true;
			}
			LogonSuccess.dwDiamondCount = m_AccountsDBAide.GetValue_WORD(TEXT("UserDiamondCount"));
			m_AccountsDBAide.GetValue_String(TEXT("AgentInfo"), LogonSuccess.szAgentInfo, CountArray(LogonSuccess.szAgentInfo));

			// 打印日志
			LOGI("CDataBaseEngineSink::OnLogonDisposeResult MobileClient Success, UserID="<<LogonSuccess.dwUserID<<", lUserScore="<<LogonSuccess.lUserScore<<
				", UserInsure="<<LogonSuccess.lUserInsure);

			//发送结果
			WORD wDataSize=CountStringBuffer(LogonSuccess.szDescribeString);
			WORD wHeadSize=sizeof(LogonSuccess)-sizeof(LogonSuccess.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGON_SUCCESS,dwContextID,&LogonSuccess,wHeadSize+wDataSize);
		}
		else
		{
			//变量定义
			DBO_MB_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//构造数据
			LogonFailure.lResultCode=dwErrorCode;
			lstrcpyn(LogonFailure.szDescribeString,pszErrorString,CountArray(LogonFailure.szDescribeString));

			// 打印日志
			LOGE("CDataBaseEngineSink::OnLogonDisposeResult MobileClient Failed, dwErrorCode="<<dwErrorCode<<", szDescribeString="<<LogonFailure.szDescribeString);

			//发送结果
			WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
			WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
		}
	}

	return;
}

//银行结果
VOID CDataBaseEngineSink::OnInsureDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//变量定义
		DBO_GP_UserInsureSuccess UserInsureSuccess;
		ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

		//构造变量
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
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_SUCCESS,dwContextID,&UserInsureSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//变量定义
		DBO_GP_UserInsureFailure UserInsureFailure;
		ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

		//构造变量
		UserInsureFailure.lResultCode=dwErrorCode;
		lstrcpyn(UserInsureFailure.szDescribeString,pszErrorString,CountArray(UserInsureFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INSURE_FAILURE,dwContextID,&UserInsureFailure,wHeadSize+wDataSize);
	}

	return;
}

//操作结果
VOID CDataBaseEngineSink::OnOperateDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//变量定义
		DBO_GP_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

		//构造变量
		OperateSuccess.lResultCode=dwErrorCode;
		lstrcpyn(OperateSuccess.szDescribeString,pszErrorString,CountArray(OperateSuccess.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(OperateSuccess.szDescribeString);
		WORD wHeadSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,&OperateSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//构造变量
		OperateFailure.lResultCode=dwErrorCode;
		lstrcpyn(OperateFailure.szDescribeString,pszErrorString,CountArray(OperateFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wHeadSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,&OperateFailure,wHeadSize+wDataSize);
	}

	return;
}

// 自建桌子结果
VOID CDataBaseEngineSink::OnUserCustomTableDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient)
{
	if (dwErrorCode == DB_SUCCESS)
	{
		//变量定义
		DBO_GR_UserCustomTableSuccess UserCustomTableSuccess;
		ZeroMemory(&UserCustomTableSuccess, sizeof(UserCustomTableSuccess));

		//构造变量
		UserCustomTableSuccess.dwUserID = m_UserCustomDBAide.GetValue_DWORD(TEXT("UserID"));
		UserCustomTableSuccess.wServerID = m_UserCustomDBAide.GetValue_WORD(TEXT("ServerID"));
		m_UserCustomDBAide.GetValue_String(TEXT("Secret"), UserCustomTableSuccess.szSecret, CountArray(UserCustomTableSuccess.szSecret));
		if (m_UserCustomDBAide.GetValue_WORD(TEXT("LockedInTable")) == 0)
		{
			UserCustomTableSuccess.bLockedInTable = false;
		}
		else
		{
			UserCustomTableSuccess.bLockedInTable = true;
		}
		UserCustomTableSuccess.dwUserPropertyCount = m_UserCustomDBAide.GetValue_DWORD(TEXT("PropertyCount"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_CUSTOMTABLE_SUCCESS, dwContextID, &UserCustomTableSuccess, sizeof(UserCustomTableSuccess));
	}
	else
	{
		//变量定义
		DBO_GR_UserCustomTableFailure UserCustomTableFailure;
		ZeroMemory(&UserCustomTableFailure, sizeof(UserCustomTableFailure));

		//构造变量
		UserCustomTableFailure.lResultCode = dwErrorCode;
		lstrcpyn(UserCustomTableFailure.szDescribeString, pszErrorString, CountArray(UserCustomTableFailure.szDescribeString));

		//发送结果
		WORD wDataSize = CountStringBuffer(UserCustomTableFailure.szDescribeString);
		WORD wHeadSize = sizeof(UserCustomTableFailure) - sizeof(UserCustomTableFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_CUSTOMTABLE_FAILURE, dwContextID, &UserCustomTableFailure, wHeadSize + wDataSize);
	}
	return;
}

//银行结果
VOID CDataBaseEngineSink::OnDiamondDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	if (dwErrorCode == DB_SUCCESS)
	{
		//变量定义
		DBO_MB_UserDiamondSuccess UserDiamondSuccess;
		ZeroMemory(&UserDiamondSuccess, sizeof(UserDiamondSuccess));

		//构造变量
		UserDiamondSuccess.dwUserID = m_UserCustomDBAide.GetValue_DWORD(TEXT("UserID"));
		UserDiamondSuccess.dwDiamondCount = m_UserCustomDBAide.GetValue_DWORD(TEXT("SourceDiamond"));
		lstrcpyn(UserDiamondSuccess.szDescribeString, pszErrorString, CountArray(UserDiamondSuccess.szDescribeString));

		//发送结果
		WORD wDataSize = CountStringBuffer(UserDiamondSuccess.szDescribeString);
		WORD wHeadSize = sizeof(UserDiamondSuccess) - sizeof(UserDiamondSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_DIAMOND_SUCCESS, dwContextID, &UserDiamondSuccess, wHeadSize + wDataSize);
	}
	else
	{
		//变量定义
		DBO_MB_UserDiamondFailure UserDiamondFailure;
		ZeroMemory(&UserDiamondFailure, sizeof(UserDiamondFailure));

		//构造变量
		UserDiamondFailure.lResultCode = dwErrorCode;
		lstrcpyn(UserDiamondFailure.szDescribeString, pszErrorString, CountArray(UserDiamondFailure.szDescribeString));

		//发送结果
		WORD wDataSize = CountStringBuffer(UserDiamondFailure.szDescribeString);
		WORD wHeadSize = sizeof(UserDiamondFailure) - sizeof(UserDiamondFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_DIAMOND_FAILURE, dwContextID, &UserDiamondFailure, wHeadSize + wDataSize);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////
