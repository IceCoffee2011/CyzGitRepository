#include "Stdafx.h"
#include "Resource.h"
#include "DlgServerItem.h"

//////////////////////////////////////////////////////////////////////////////////

//列表属性
#define LIST_STYTE LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_FLATSB

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerItem, CDialog)

	//按钮消息
	ON_BN_CLICKED(IDC_LOAD_SERVER, OnBnClickedLoadServer)
	ON_BN_CLICKED(IDC_RELOAD_LIST, OnBnClickedReLoadList)
	ON_BN_CLICKED(IDC_DELETE_SERVER, OnBnClickedDeleteServer)
	ON_BN_CLICKED(IDC_SERVICE_ATTRIB, OnBnClickedServiceAttrib)

	//控件消息
	ON_NOTIFY(NM_DBLCLK, IDC_SERVER_LIST, OnNMDblclkServerList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SERVER_LIST, OnLvnItemChangedServerList)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerItem::CDlgServerItem() : CDialog(IDD_SERVER_ITEM)
{
	//模块参数
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//析构函数
CDlgServerItem::~CDlgServerItem()
{
}

//控件绑定
VOID CDlgServerItem::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_LIST, m_ServerListControl);
}

//消息解释
BOOL CDlgServerItem::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//初始化函数
BOOL CDlgServerItem::OnInitDialog()
{
	__super::OnInitDialog();

	//设置资源
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//加载房间
	LoadDBServerItem();

	return TRUE;
}

//确定消息
VOID CDlgServerItem::OnOK()
{
	return;
}

//打开房间
bool CDlgServerItem::OpenGameServer()
{
	//设置资源
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//配置房间
	if (DoModal()==IDC_LOAD_SERVER)
	{
		return true;
	}

	return false;
}

//打开房间(通过命令行方式启动,直接传入ServerID)
bool CDlgServerItem::OpenGameServer(WORD wServerID)
{
	//变量定义
	tagGameServerInfo GameServerResult;
	ZeroMemory(&GameServerResult,sizeof(GameServerResult));

	//机器标识
	TCHAR szMachineID[LEN_MACHINE_ID];
	CWHService::GetMachineID(szMachineID);

	//加载房间
	CServerInfoManager ServerInfoManager;
	if (ServerInfoManager.LoadGameServerInfo(szMachineID,wServerID,GameServerResult)==false)
	{
		return false;
	}

	//获取参数
	if (GetModuleInitParameter(&GameServerResult,true)==false)
	{
		return false;
	}

	// 检查桌子配置是否载入
	if (CheckModuleInitParameter(&GameServerResult, true) == false)
	{
		return false;
	}

	return true;
}

//加载列表
bool CDlgServerItem::LoadDBServerItem()
{
	//重置列表
	m_ServerListControl.DeleteAllItems();

	//设置按钮
	GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_SERVICE_ATTRIB)->EnableWindow(FALSE);

	//机器标识
	TCHAR szMachineID[LEN_MACHINE_ID];
	CWHService::GetMachineID(szMachineID);

	//加载信息
	CServerInfoManager ServerInfoManager;
	if (ServerInfoManager.LoadGameServerInfo(szMachineID,m_ServerInfoBuffer)==true)
	{
		//变量定义
		POSITION Position=m_ServerInfoBuffer.m_GameServerInfoMap.GetStartPosition();

		//枚举模块
		while (Position!=NULL)
		{
			//获取对象
			WORD wServerID=0L;
			tagGameServerInfo * pGameServerInfo=NULL;
			m_ServerInfoBuffer.m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);

			//插入列表
			ASSERT(pGameServerInfo!=NULL);
			if (pGameServerInfo!=NULL) m_ServerListControl.InsertServerInfo(pGameServerInfo);
		}

		return true;
	}

	return false;
}

//加载广场赛
bool CDlgServerItem::LoadDBSquareMatch(WORD wServerID)
{
	//加载广场赛
	ZeroMemory(m_ModuleInitParameter.GameSquareOptionArray,sizeof(m_ModuleInitParameter.GameSquareOptionArray));
	CServerInfoManager ServerInfoManager;
	ServerInfoManager.LoadSquareMatch(wServerID,m_ModuleInitParameter.GameSquareOptionArray,\
		m_ModuleInitParameter.cbGameSquareOptionArrayCount);

	return true;
}

//更新按钮
bool CDlgServerItem::UpdateControlStatus()
{
	//获取选择
	POSITION Position=m_ServerListControl.GetFirstSelectedItemPosition();

	//设置列表
	if (Position!=NULL)
	{
		//获取房间
		INT nListItem=m_ServerListControl.GetNextSelectedItem(Position);
		tagGameServerInfo * pGameServerInfo=(tagGameServerInfo *)m_ServerListControl.GetItemData(nListItem);

		//设置按钮
		if (pGameServerInfo->dwNativeVersion!=0L)
		{
			GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_SERVICE_ATTRIB)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_SERVICE_ATTRIB)->EnableWindow(FALSE);
		}
	}
	else
	{
		//设置按钮
		GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_SERVICE_ATTRIB)->EnableWindow(FALSE);
	}

	return true;
}

//获取参数
bool CDlgServerItem::GetModuleInitParameter(tagGameServerInfo * pGameServerInfo, bool bAutoMode)
{
	//效验参数
	ASSERT(pGameServerInfo!=NULL);
	if (pGameServerInfo==NULL) return false;

	//游戏模块
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(pGameServerInfo->szServerDLLName,GAME_SERVICE_CREATE_NAME);

	//创建判断
	if (pGameServerInfo->dwNativeVersion==0)
	{
		//构造提示
		TCHAR szString[128]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("[ %s ] 游戏服务器组件还没有安装，请先安装对应的游戏服务器"),pGameServerInfo->szGameName);

		//提示消息
		if (bAutoMode==true) CTraceService::TraceString(szString,TraceLevel_Exception);
		else AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//更新判断
	if (pGameServerInfo->dwNativeVersion!=pGameServerInfo->dwServerVersion)
	{
		//构造提示
		TCHAR szString[128]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("[ %s ] 游戏服务器组件已经更新了，不能继续用于启动房间"),pGameServerInfo->szGameName);

		//提示消息
		if (bAutoMode==true) CTraceService::TraceString(szString,TraceLevel_Exception);
		else AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//加载模块
	if (GameServiceManager.CreateInstance()==false)
	{
		//构造提示
		TCHAR szString[128]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("[ %s ] 服务组件[%s]，请重新安装服务组件"),pGameServerInfo->szGameName,GameServiceManager.GetErrorDescribe());

		//提示消息
		if (bAutoMode==true) CTraceService::TraceString(szString,TraceLevel_Exception);
		else AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//模块属性
	GameServiceManager->GetServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);

	//挂接属性
	m_ModuleInitParameter.GameServiceOption.wKindID=pGameServerInfo->wKindID;
	m_ModuleInitParameter.GameServiceOption.wNodeID=pGameServerInfo->wNodeID;
	m_ModuleInitParameter.GameServiceOption.wSortID=pGameServerInfo->wSortID;
	m_ModuleInitParameter.GameServiceOption.wServerID=pGameServerInfo->wServerID;

	//税收配置
	// 判断桌子是否又有了对应的配置
	for (size_t i = 0; i < pGameServerInfo->TableConfig.size(); i++)
	{
		WORD wStartTableID = pGameServerInfo->TableConfig[i].wStartTableID;
		WORD wEndTableID = pGameServerInfo->TableConfig[i].wEndTableID;

		m_ModuleInitParameter.TableConfig.push_back(pGameServerInfo->TableConfig.at(i));

		// 桌号从1开始
		for (int j = wStartTableID; j <= wEndTableID; j++)
		{
			m_ModuleInitParameter.GameServiceTableOption.TableOption.push_back(pGameServerInfo->TableConfig[i].TableOption);
			m_ModuleInitParameter.GameServiceTableOption.TableOption[j - 1].wTableID = j - 1;
			LOGI("CDlgServerItem::GetModuleInitParameter, TableName=" << m_ModuleInitParameter.GameServiceTableOption.TableOption[j - 1].szTableName);

			// 查找是否有创建好的桌子,有的话拷贝用户自定义规则
			if (pGameServerInfo->wServerType & GAME_GENRE_USERCUSTOM)
			{
				for (size_t k = 0; k < pGameServerInfo->vectUserCustomTableInfo.size(); k++)
				{
					if (pGameServerInfo->vectUserCustomTableInfo.at(k).wTableID == (j - 1))
					{
						memcpy(m_ModuleInitParameter.GameServiceTableOption.TableOption[j - 1].cbCustomRule, pGameServerInfo->vectUserCustomTableInfo.at(k).cbCustomRule, \
					 							sizeof(m_ModuleInitParameter.GameServiceTableOption.TableOption[j - 1].cbCustomRule));

						m_ModuleInitParameter.GameServiceTableOption.TableOption.at(j - 1).dwCreatUserID = pGameServerInfo->vectUserCustomTableInfo.at(k).dwUserID;
						m_ModuleInitParameter.GameServiceTableOption.TableOption.at(j - 1).dwRoundCount = pGameServerInfo->vectUserCustomTableInfo.at(k).dwRoundCount;
						m_ModuleInitParameter.GameServiceTableOption.TableOption.at(j - 1).dwPlayRoundCount = pGameServerInfo->vectUserCustomTableInfo.at(k).dwPlayRoundCount;
						m_ModuleInitParameter.GameServiceTableOption.TableOption.at(j - 1).TableCreateTime = pGameServerInfo->vectUserCustomTableInfo.at(k).TableCreateTime;
						lstrcpyn(m_ModuleInitParameter.GameServiceTableOption.TableOption.at(j - 1).szSecret, pGameServerInfo->vectUserCustomTableInfo.at(k).szSecret,
							sizeof(m_ModuleInitParameter.GameServiceTableOption.TableOption.at(j - 1).szSecret));
					}
				}
			}
		}
	}

	m_ModuleInitParameter.GameServiceOption.lMinEnterScore=pGameServerInfo->lMinEnterScore;
	m_ModuleInitParameter.GameServiceOption.lMaxEnterScore=pGameServerInfo->lMaxEnterScore;

	//会员限制
	m_ModuleInitParameter.GameServiceOption.cbMinEnterMember=pGameServerInfo->cbMinEnterMember;
	m_ModuleInitParameter.GameServiceOption.cbMaxEnterMember=pGameServerInfo->cbMaxEnterMember;

	//房间配置
	m_ModuleInitParameter.GameServiceOption.dwServerRule=pGameServerInfo->dwServerRule;
	m_ModuleInitParameter.GameServiceOption.dwAttachUserRight=pGameServerInfo->dwAttachUserRight;

	//房间属性
	m_ModuleInitParameter.GameServiceOption.wMaxPlayer=pGameServerInfo->wMaxPlayer;
	m_ModuleInitParameter.GameServiceOption.wTableCount=pGameServerInfo->wTableCount;
	m_ModuleInitParameter.GameServiceOption.wServerPort=pGameServerInfo->wServerPort;
	m_ModuleInitParameter.GameServiceOption.wServerType=pGameServerInfo->wServerType;
	lstrcpyn(m_ModuleInitParameter.GameServiceOption.szServerName,pGameServerInfo->szServerName,LEN_SERVER);

	//分组属性
	m_ModuleInitParameter.GameServiceOption.cbDistributeRule=pGameServerInfo->cbDistributeRule;
	m_ModuleInitParameter.GameServiceOption.wMinDistributeUser=pGameServerInfo->wMinDistributeUser;
	m_ModuleInitParameter.GameServiceOption.wMaxDistributeUser=pGameServerInfo->wMaxDistributeUser;
	m_ModuleInitParameter.GameServiceOption.wDistributeTimeSpace=pGameServerInfo->wDistributeTimeSpace;
	m_ModuleInitParameter.GameServiceOption.wDistributeDrawCount=pGameServerInfo->wDistributeDrawCount;
	m_ModuleInitParameter.GameServiceOption.wDistributeStartDelay=pGameServerInfo->wDistributeStartDelay;

	//连接信息
	lstrcpyn(m_ModuleInitParameter.GameServiceOption.szDataBaseName,pGameServerInfo->szDataBaseName,CountArray(m_ModuleInitParameter.GameServiceOption.szDataBaseName));
	lstrcpyn(m_ModuleInitParameter.GameServiceOption.szDataBaseAddr,pGameServerInfo->szDataBaseAddr,CountArray(m_ModuleInitParameter.GameServiceOption.szDataBaseAddr));

	//数据设置
#ifndef _READ_TABLE_OPTION_
	UINT uCustomRuleSize=sizeof(m_ModuleInitParameter.GameServiceOption.cbCustomRule);
	CopyMemory(m_ModuleInitParameter.GameServiceOption.cbCustomRule,pGameServerInfo->cbCustomRule,uCustomRuleSize);
#else
	// anjay todo
#endif

	//加载广场赛
	ZeroMemory(m_ModuleInitParameter.GameSquareOptionArray,sizeof(m_ModuleInitParameter.GameSquareOptionArray));
	CServerInfoManager ServerInfoManager;
	ServerInfoManager.LoadSquareMatch(m_ModuleInitParameter.GameServiceOption.wServerID,m_ModuleInitParameter.GameSquareOptionArray,\
		m_ModuleInitParameter.cbGameSquareOptionArrayCount);

	return true;
}

// 检查是否有载入对应的桌子配置
bool CDlgServerItem::CheckModuleInitParameter(tagGameServerInfo * pGameServerInfo, bool bAutoMode)
{
	//效验参数
	ASSERT(pGameServerInfo != NULL);
	if (pGameServerInfo == NULL) return false;

	vector<int> m;
	for (int i = 0; i < pGameServerInfo->wTableCount; i++)
	{
		m.push_back(0);
	}

	//构造提示
	TCHAR szString[128] = TEXT("");

	//提示消息

	for (size_t i = 0; i < pGameServerInfo->TableConfig.size(); i++)
	{
		WORD wStartTableID = pGameServerInfo->TableConfig[i].wStartTableID;
		WORD wEndTableID = pGameServerInfo->TableConfig[i].wEndTableID;

		if (wStartTableID < 1 || wEndTableID < 1)
		{
			_sntprintf(szString, CountArray(szString), TEXT("桌子配置桌号有误,请注意检查"));
			if (bAutoMode == true) CTraceService::TraceString(szString, TraceLevel_Exception);
			else AfxMessageBox(szString, MB_ICONERROR);

			return false;
		}

		// 桌号从1开始
		for (int j = wStartTableID; j <= wEndTableID; j++)
		{
			if (j <= m.size())
			{
				if (m[j-1] == 0)
				{
					m[j-1] = 1;
				}
				else
				{
					// 之前已经配置过了,出现了重复配置,返回
					_sntprintf(szString, CountArray(szString), TEXT("有桌子出现了重复配置,请注意检查"));
					if (bAutoMode == true) CTraceService::TraceString(szString, TraceLevel_Exception);
					else AfxMessageBox(szString, MB_ICONERROR);
					return false;
				}
			}
		}
	}

	for (size_t i = 0; i < m.size(); i++)
	{
		if (m.at(i) == 0)
		{
			//构造提示
			_sntprintf(szString, CountArray(szString), TEXT("还有桌子没有配置,请检查配置后再启动服务") );

			//提示消息
			if (bAutoMode == true) CTraceService::TraceString(szString, TraceLevel_Exception);
			else AfxMessageBox(szString, MB_ICONERROR);

			return false;
		}
	}

	return true;
}

//加载房间
VOID CDlgServerItem::OnBnClickedLoadServer()
{
	//获取选择
	ASSERT(m_ServerListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_ServerListControl.GetFirstSelectedItemPosition();
	
	//获取房间
	INT nListItem=m_ServerListControl.GetNextSelectedItem(Position);
	tagGameServerInfo * pGameServerInfo=(tagGameServerInfo *)m_ServerListControl.GetItemData(nListItem);

	//设置变量
	if ((pGameServerInfo!=NULL)&&(GetModuleInitParameter(pGameServerInfo,false)==true))
	{
		// 检查桌子是否配置了
		CheckModuleInitParameter(pGameServerInfo, false);
		EndDialog(IDC_LOAD_SERVER);
	}

	return;
}

//刷新列表
VOID CDlgServerItem::OnBnClickedReLoadList()
{
	//加载列表
	LoadDBServerItem();

	return;
}

//删除房间
VOID CDlgServerItem::OnBnClickedDeleteServer()
{
	//获取选择
	ASSERT(m_ServerListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_ServerListControl.GetFirstSelectedItemPosition();
	
	//获取房间
	INT nListItem=m_ServerListControl.GetNextSelectedItem(Position);
	tagGameServerInfo * pGameServerInfo=(tagGameServerInfo *)m_ServerListControl.GetItemData(nListItem);

	//设置变量
	if (pGameServerInfo!=NULL)
	{
		//构造提示
		TCHAR szString[128]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("确实要删除 [ %s ] 游戏房间吗？"),pGameServerInfo->szServerName);

		//提示消息
		if (AfxMessageBox(szString,MB_ICONQUESTION|MB_YESNO)!=IDYES) return;

		//删除房间
		CServerInfoManager ServerInfoManager;
		if (ServerInfoManager.DeleteGameServer(pGameServerInfo->wServerID)==false) return;

		//删除列表
		m_ServerListControl.DeleteItem(nListItem);
		m_ServerInfoBuffer.DeleteServerInfo(pGameServerInfo->wServerID);

		//设置列表
		if (m_ServerListControl.GetItemCount()>0)
		{
			//设置变量
			INT nItemCount=m_ServerListControl.GetItemCount();
			INT nNextItem=(nListItem>=nItemCount)?(nItemCount-1):nListItem;

			//设置选择
			m_ServerListControl.SetItemState(nNextItem,LVIS_SELECTED,LVIS_SELECTED);
		}

		//更新控制
		UpdateControlStatus();
	}

	return;
}

//组件属性
VOID CDlgServerItem::OnBnClickedServiceAttrib()
{
	return;
}

//双击列表
VOID CDlgServerItem::OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//加载配置
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		OnBnClickedLoadServer();
	}

	return;
}

//选择改变
VOID CDlgServerItem::OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//更新控制
	UpdateControlStatus();

	return;
}

//////////////////////////////////////////////////////////////////////////////////
