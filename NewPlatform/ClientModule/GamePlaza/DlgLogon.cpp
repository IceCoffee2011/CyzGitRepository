#include "Stdafx.h"
#include "DlgLogon.h"
#include "GamePlaza.h"
#include "MissionLogon.h"
#include "DlgProxyConfig.h"
#include ".\dlglogon.h"

//////////////////////////////////////////////////////////////////////////////////

//圆角大小
#define ROUND_CX					7									//圆角宽度
#define ROUND_CY					7									//圆角高度

//屏幕位置
#define LAYERED_SIZE				1									//6603分层大小
#define CAPTION_SIZE				35									//标题大小

//控件标识
#define IDC_WEB_PUBLICIZE			300									//浏览控件


//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgLogon, CDialog)

	//系统消息
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()

	//按钮消息
	ON_BN_CLICKED(IDC_REGISTER, OnBnClickedRegister)
	ON_BN_CLICKED(IDC_PROXY_CONFIG, OnBnClickedProxyConfig)
	ON_BN_CLICKED(IDC_CLEAR_ACCOUNT, OnBnClearAccount)
	ON_BN_CLICKED(IDC_FIND_PASS, OnBnFindPass)


	//控件消息
	ON_CBN_SELCHANGE(IDC_ACCOUNTS, OnSelchangeAccounts)
	ON_CBN_EDITCHANGE(IDC_ACCOUNTS, OnCbnEditchangeAccounts)
	ON_EN_CHANGE(IDC_PASSWORD, OnCbnEditchangeAccounts) 

	ON_BN_CLICKED(IDC_CHECK_REMPASS, OnBnClickedCheckRempass)

	ON_STN_CLICKED(IDC_STATIC_AGREE_AGREEMENT, OnBnClickedAgreeAgreement)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgLogon::CDlgLogon() : CDialog(IDD_DLG_LOGON)
{
	//登录信息
	m_szAccounts[0]=0;
	m_szPassword[0]=0;
	m_szLogonServer[0]=0;

	//配置变量
	m_cbRemPassword=FALSE;
	m_cbLogonMode=LOGON_BY_ACCOUNTS;

	//创建画刷
	m_brBrush.CreateSolidBrush(RGB(215,223,228));

	return;
}

//析构函数
CDlgLogon::~CDlgLogon()
{
	//销毁资源
	if (m_brBrush.GetSafeHandle()!=NULL)
	{
		m_brBrush.DeleteObject();
	}

	//删除对象
	for (INT_PTR i=0;i<m_AccountsInfoArray.GetCount();i++)
	{
		SafeDelete(m_AccountsInfoArray[i]);
	}

	return;
}

//控件绑定
VOID CDlgLogon::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//绑定控件
	DDX_Control(pDX, IDOK, m_btEnter);
	DDX_Control(pDX, IDC_REGISTER, m_btRegister);

	//绑定控件
	DDX_Control(pDX, IDCANCEL, m_btQuit);
	DDX_Control(pDX, IDC_PROXY_CONFIG, m_btOption);
	DDX_Control(pDX, IDC_CLEAR_ACCOUNT, m_btClear);
	DDX_Control(pDX, IDC_FIND_PASS, m_btFindPass);

	//其他控件
	DDX_Control(pDX, IDC_ACCOUNTS, m_AccountsControl);
	DDX_Control(pDX, IDC_PASSWORD, m_PasswordControl);
	DDX_Check(pDX, IDC_CHECK_REMPASS, m_cbRemPassword);

	//辅助控件
	DDX_Control(pDX, IDC_LOGON_SERVER, m_ServerControl);

	DDX_Control(pDX, IDC_STATIC_AGREE_AGREEMENT, m_AgreeAgreement);
}

//创建函数
BOOL CDlgLogon::OnInitDialog()
{
	__super::OnInitDialog();

	//设置窗口
	ModifyStyle(0, WS_MINIMIZEBOX);
	ModifyStyle(0, WS_MAXIMIZEBOX);

	//设置标题
	SetWindowText(TEXT("用户登录"));
	
	//加载资源
	CPngImage ImageBack;
	ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("DLG_LOGON_BACK"));

	//设置大小
	CSize SizeWindow(ImageBack.GetWidth(),ImageBack.GetHeight());
	SetWindowPos(NULL,0,0,SizeWindow.cx,SizeWindow.cy,SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);

	//边框设置
	m_ServerControl.SetDrawBorad(false);
	m_AccountsControl.SetDrawBorad(false);
	m_PasswordControl.SetDrawBorad(false);

	//渲染设置
	m_ServerControl.SetRenderImage(false);
	m_AccountsControl.SetRenderImage(false);

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
    m_btQuit.SetButtonImage(IDB_BT_LOGON_QUIT,hInstance,false,false);
	m_btEnter.SetButtonImage(IDB_BT_LOGON, TEXT("BT_LOGON_ENTER"),hInstance,false,false);


	m_btOption.SetButtonImage(IDB_BT_LOGON_ITEM,TEXT("BT_LOGON_ITEM"),hInstance,false,false);

	m_btRegister.SetButtonImage(IDB_BT_LOGON_ITEM, TEXT("BT_LOGON_REGISTER"),hInstance,false,false);
	//m_btEnter.SetButtonImage(TEXT("BT_LOGON_ENTER"),hInstance,true,true);

	m_btClear.SetButtonImage(IDB_BT_LOGON_ITEM,TEXT("BT_LOGON_CLEAR"),hInstance,false,false);
	m_btFindPass.SetButtonImage(IDB_BT_LOGON_ITEM,TEXT("BT_LOGON_FINDPASS"),hInstance,false,false);
	//m_btLogonLink2.SetButtonImage(IDB_BT_LOGON_LINK,TEXT("BT_LOGON_LINK_2"),hInstance,false,false);
	//m_btLogonLink3.SetButtonImage(IDB_BT_LOGON_LINK,TEXT("BT_LOGON_LINK_3"),hInstance,false,false);
	//m_btLogonLink4.SetButtonImage(IDB_BT_LOGON_LINK,TEXT("BT_LOGON_LINK_4"),hInstance,false,false);
	m_AgreeAgreement.SetBackGroundColor(RGB(220,228,230));
	((CButton*)GetDlgItem(IDC_CHECK_AGREE_AGREEMENT))->SetCheck(TRUE);


	// iii 屏蔽不再使用的按钮
//	m_btDeleteRecord.ShowWindow(SW_HIDE);
//	m_btClear.ShowWindow(SW_HIDE);
	m_btOption.ShowWindow(SW_HIDE);



//	m_btLogonLink2.ShowWindow(SW_HIDE);
//	m_btLogonLink3.ShowWindow(SW_HIDE);
//	m_btLogonLink4.ShowWindow(SW_HIDE);


	//广告控件
	//CRect rcCreate(0,0,0,0);
	//m_PlatformPublicize.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,rcCreate,this,IDC_WEB_PUBLICIZE);

	//构造地址
	//TCHAR szBillUrl[256]=TEXT("");
	//_sntprintf(szBillUrl,CountArray(szBillUrl),TEXT("http://www.aoyuevip.com/aa/gamelogin_lkgame_com.htm"),szPlatformLink);//%s/aa/RevealTrans.htm

	//设置广告
	//m_PlatformPublicize.Navigate(szBillUrl);
	//m_PlatformPublicize.SetWindowPos(NULL,LAYERED_SIZE+4,93,SizeWindow.cx-12-LAYERED_SIZE,212,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE);

    //广告控件
	CRect rcCreate(0,0,0,0);
	m_PlatformPublicize.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,rcCreate,this,IDC_WEB_PUBLICIZE);


	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szPath, sizeof(szPath));
	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\local.dat"), szPath);
	//获取地址
	TCHAR szBillUrl[256]=TEXT("");
	GetPrivateProfileString(TEXT("HyperLink"),TEXT("LogonAD"),TEXT(""), szBillUrl, sizeof(szBillUrl), szFileName);

	//设置广告
//	m_PlatformPublicize.SetBoradColor(RGB(51,25,3));
	m_PlatformPublicize.Navigate(szBillUrl);
	//m_PlatformPublicize.SetWindowPos(NULL,LAYERED_SIZE,42,SizeWindow.cx-2*LAYERED_SIZE,140,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE);
    m_PlatformPublicize.SetWindowPos(NULL,LAYERED_SIZE,30,SizeWindow.cx-2*LAYERED_SIZE,82,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE);
	//m_PlatformPublicize.ShowWindow(SW_HIDE);

	//配置信息
	LoadAccountsInfo();
	LoadLogonServerInfo();

	//设置焦点
	if (m_AccountsControl.GetCurSel()!=LB_ERR)
	{
		//消息处理
		OnSelchangeAccounts();

		//设置焦点
		//m_PasswordControl.SetFocus();
	}
	else
	{
		//设置焦点
		//m_AccountsControl.SetFocus();
	}

	//调整按钮
	HDWP hDwp=BeginDeferWindowPos(32);
	UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER|SWP_NOSIZE;

	DeferWindowPos(hDwp,m_btClear,NULL,358,229,0,0,uFlags);

	EndDeferWindowPos(hDwp);

	//居中窗口
	CenterWindow(this);

	return FALSE;
}

//消息解释
BOOL CDlgLogon::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//确定函数
VOID CDlgLogon::OnOK()
{
	LOGI("CDlgLogon::OnOK, Step 1="<<GetTickCount());
	//获取信息
	if (GetInformation()==false) return;
	LOGI("CDlgLogon::OnOK, Step 2="<<GetTickCount());

	//隐藏窗口
	ShowWindow(SW_HIDE);
	LOGI("CDlgLogon::OnOK, Step 3="<<GetTickCount());

	//执行登录
	CMissionLogon * pMissionLogon=CMissionLogon::GetInstance();
	if (pMissionLogon!=NULL) pMissionLogon->PerformLogonMission(m_cbRemPassword==TRUE);

	LOGI("CDlgLogon::OnOK, Step 4="<<GetTickCount());

	return;
}

//取消消息
VOID CDlgLogon::OnCancel()
{
	//关闭窗口
	DestroyWindow();
	AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);

	return;
}

//构造数据
WORD CDlgLogon::ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize, BYTE cbValidateFlags)
{
	//变量定义
	TCHAR szPassword[LEN_MD5];
	CWHEncrypt::MD5Encrypt(m_szPassword,szPassword);

	//发送数据
	switch (m_cbLogonMode)
	{
	case LOGON_BY_ACCOUNTS:			//帐号登录
		{
			//变量定义
			CMD_GP_LogonAccounts * pLogonAccounts=(CMD_GP_LogonAccounts *)cbBuffer;

			//系统信息
			pLogonAccounts->dwPlazaVersion=VERSION_PLAZA;

			//机器标识
			CWHService::GetMachineIDEx(pLogonAccounts->szMachineID);

			//登录信息
			lstrcpyn(pLogonAccounts->szPassword,szPassword,CountArray(pLogonAccounts->szPassword));
			lstrcpyn(pLogonAccounts->szAccounts,m_szAccounts,CountArray(pLogonAccounts->szAccounts));
			pLogonAccounts->cbValidateFlags=cbValidateFlags;

			//保存密码
			CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
			tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
			lstrcpyn(pGlobalUserData->szPassword,pLogonAccounts->szPassword,CountArray(pGlobalUserData->szPassword));

			return sizeof(CMD_GP_LogonAccounts);
		}
	}

	return 0;
}

//加载账号
VOID CDlgLogon::LoadAccountsInfo()
{
	//读取信息
	CWHRegKey RegUserInfo;
	if (RegUserInfo.OpenRegKey(REG_USER_INFO,false)==false) return;

	//变量定义
	DWORD dwRegIndex=0L;
	DWORD dwLastUserID=RegUserInfo.GetValue(TEXT("LastUserID"),0L);

	//读取信息
	do
	{
		//变量定义
		tagAccountsInfo AccountsInfo;
		ZeroMemory(&AccountsInfo,sizeof(AccountsInfo));

		//读取键项
		TCHAR szKeyItemName[16]=TEXT("");
		if (RegUserInfo.EnumItemString(dwRegIndex++,szKeyItemName,CountArray(szKeyItemName))==false) break;

		//打开子键
		CWHRegKey RegUserItem;
		if (RegUserItem.Attach(RegUserInfo.OpenItemKey(szKeyItemName))==NULL) continue;

		//帐户信息
		AccountsInfo.dwUserID=_tstol(szKeyItemName);
		RegUserItem.GetString(TEXT("GameID"),AccountsInfo.szGameID,CountArray(AccountsInfo.szGameID));
		RegUserItem.GetString(TEXT("UserAccount"),AccountsInfo.szAccounts,CountArray(AccountsInfo.szAccounts));

		//读取密码
		TCHAR szPassword[MAX_ENCRYPT_LEN]=TEXT("");
		RegUserItem.GetString(TEXT("UserPassword"),szPassword,CountArray(szPassword));

		//解开密码
		if (szPassword[0]!=0)
		{
			CWHEncrypt::XorCrevasse(szPassword,AccountsInfo.szPassword,CountArray(AccountsInfo.szPassword));
		}

		//构造数据
		tagAccountsInfo * pAccountsInfo=new tagAccountsInfo;
		CopyMemory(pAccountsInfo,&AccountsInfo,sizeof(AccountsInfo));

		//插入数据
		m_AccountsInfoArray.InsertAt(m_AccountsInfoArray.GetCount(),pAccountsInfo);

	} while (true);

	//插入信息
	for (INT_PTR i=0;i<m_AccountsInfoArray.GetCount();i++)
	{
		//插入信息
		INT nGameIDItem=CB_ERR;
		INT nAccountsItem=CB_ERR;
		tagAccountsInfo * pAccountsInfo=m_AccountsInfoArray[i];
		
		//插入信息
		if (pAccountsInfo->szAccounts[0]!=0) nAccountsItem=m_AccountsControl.AddString(pAccountsInfo->szAccounts);

		//设置数据
		if (nAccountsItem!=CB_ERR) m_AccountsControl.SetItemDataPtr(nAccountsItem,pAccountsInfo);

		//设置选择
		if ((dwLastUserID!=0L)&&(pAccountsInfo->dwUserID==dwLastUserID))
		{
			if (nAccountsItem!=CB_ERR) m_AccountsControl.SetCurSel(nAccountsItem);
		}
	}

	//设置选择
	if ((m_AccountsControl.GetCount()>0)&&(m_AccountsControl.GetCurSel()==LB_ERR)) m_AccountsControl.SetCurSel(0);

	return;
}

//地址信息
VOID CDlgLogon::LoadLogonServerInfo()
{
	//读取数目
	CGlobalServer GlobalServer;
	WORD wItemCount=GlobalServer.GetItemCount();

	//上次登录
	TCHAR szLastServer[LEN_SERVER]=TEXT("");
	GlobalServer.GetLastServerName(szLastServer);

	//读取信息
	for (WORD i=0;i<wItemCount;i++)
	{
		//读取信息
		TCHAR szServerItem[LEN_SERVER]=TEXT("");
		GlobalServer.GetServerItemName(i,szServerItem);

		//插入信息
		if (szServerItem[0]!=0)
		{
			//插入信息
			INT nItem=m_ServerControl.AddString(szServerItem);

			//最近登录
			if ((szLastServer[0]!=0)&&(lstrcmp(szLastServer,szServerItem)==0))
			{
				szLastServer[0]=0;
				m_ServerControl.SetCurSel(nItem);
			}
		}
	}

	//最近登录
	if (szLastServer[0]!=0)
	{
		m_ServerControl.SetCurSel(m_ServerControl.AddString(szLastServer));
	}

	//设置选择 
	if (m_ServerControl.GetCurSel()==LB_ERR)
	{
		if (m_ServerControl.GetCount()==0L)
		{
			TCHAR szLogonServer[MAX_PATH];
			CGlobalServer GlobalServer;
			GlobalServer.ReadFileString(_T("LogonServer"), _T("LogonServer"), szLogonServer, CountArray(szLogonServer));
			m_ServerControl.SetCurSel(m_ServerControl.AddString(szLogonServer));  //可以设置服务器列表选择
			/*m_ServerControl.SetCurSel(m_ServerControl.AddString(TEXT("电信线路一")));
			m_ServerControl.SetCurSel(m_ServerControl.AddString(TEXT("电信线路二")));
			m_ServerControl.SetCurSel(m_ServerControl.AddString(TEXT("电信线路三")));*/
		}
		else
		{
			m_ServerControl.SetCurSel(0);
		}
	}

	return;
}

//获取信息
bool CDlgLogon::GetInformation()
{
	//获取信息
	GetDlgItemText(IDC_ACCOUNTS,m_szAccounts,CountArray(m_szAccounts));

	//获取信息
	m_PasswordControl.GetUserPassword(m_szPassword);
	GetDlgItemText(IDC_LOGON_SERVER,m_szLogonServer,CountArray(m_szLogonServer)); //配置服务器的地方

	//地址判断
	if (m_szLogonServer[0]==0L)
	{
		//提示信息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("请选择“登录服务器”后再登录，若有困难请点击“新手指南”获取帮助！"),MB_ICONERROR,0);

		//显示窗口
		ShowWindow(SW_SHOW);

		//设置焦点
		//m_ServerControl.SetFocus();

		return false;
	}

	//帐号判断
	if ((m_cbLogonMode==LOGON_BY_ACCOUNTS)&&(m_szAccounts[0]==0L))
	{
		//提示信息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("请您输入帐号后再登录，若有困难请点击“新手指南”获取帮助！"),MB_ICONERROR,0);

		//显示窗口
		ShowWindow(SW_SHOW);

		//设置焦点
		//m_AccountsControl.SetFocus();

		return false;
	}

	//密码判断
	if (m_szPassword[0]==0)
	{
		//提示信息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("请您输入密码后再登录，若有困难请联系在线客服获取帮助！"),MB_ICONERROR,0);

		//显示窗口
		ShowWindow(SW_SHOW);

		//设置焦点
		//m_PasswordControl.SetFocus();

		return false;
	}

	if ( ((CButton*)GetDlgItem(IDC_CHECK_AGREE_AGREEMENT))->GetCheck() == FALSE)
	{
		//提示信息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("亲爱的用户，感谢您对游戏的支持，如果您想体验游戏的精彩，请您阅读用户协议后，在『同意条款』前打钩，谢谢。"),MB_ICONERROR,0);
		
// 		//构造地址
// 		TCHAR szLogonLink[256]=TEXT("");
// 		_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("%s/news/index?id=48"),szPlatformLink);
// 		//打开页面
// 		ShellExecute(NULL,TEXT("OPEN"),szLogonLink,NULL,NULL,SW_NORMAL);

		//显示窗口
		ShowWindow(SW_SHOW);

		return false;
	}

	//配置信息
	UpdateData(TRUE);
	//m_cbRemPassword = m_btRem_Password.IsWindowVisible()?true:false;

	return true;
}

//选择改变
VOID CDlgLogon::OnSelchangeAccounts()
{
	//获取选择
	INT nCurrentSel=m_AccountsControl.GetCurSel();
	tagAccountsInfo * pAccountsInfo=(tagAccountsInfo *)m_AccountsControl.GetItemDataPtr(nCurrentSel);

	//设置控件
	if ((nCurrentSel!=LB_ERR)&&(pAccountsInfo!=NULL))
	{
		//用户帐号
		m_AccountsControl.SetCurSel(nCurrentSel);

		//设置密码
		m_PasswordControl.SetUserPassword(pAccountsInfo->szPassword);

		//记住密码
		m_cbRemPassword=(pAccountsInfo->szPassword[0]!=0);
		UpdateData(FALSE);
	}
	else
	{
		//设置密码
		m_PasswordControl.SetUserPassword(TEXT(""));

		//记住密码
		//((CButton *)GetDlgItem(IDC_REM_PASSWORD))->SetCheck(BST_UNCHECKED);
		ASSERT(0);
		//m_btRem_Password.ShowWindow(SW_HIDE);
		//m_btRem_Password2.ShowWindow(SW_HIDE);
	}

	return;
}

//网络设置
VOID CDlgLogon::OnBnClickedProxyConfig()
{
	//隐藏窗口
	ShowWindow(SW_HIDE);

	//代理设置
	CDlgProxyConfig DlgProxyConfig;
	DlgProxyConfig.DoModal();

	//显示窗口
	ShowWindow(SW_SHOW);

	return;
}

//编辑改变
VOID CDlgLogon::OnCbnEditchangeAccounts()
{
	//安全设置
	m_cbRemPassword = FALSE;
	UpdateData(FALSE);

	//密码重置
	if (m_PasswordControl.IsFalsityPassword()) m_PasswordControl.SetUserPassword(NULL);

	

	return;
}

//删除记录
VOID CDlgLogon::OnBnClearAccount()
{
	//变量定义
	tagAccountsInfo * pAccountsInfo=NULL;

	//获取信息
	INT nCurrentSel=m_AccountsControl.GetCurSel();

	//删除处理
	if (nCurrentSel!=LB_ERR)
	{
		//获取变量
		ASSERT(m_AccountsControl.GetItemDataPtr(nCurrentSel)!=NULL);
		pAccountsInfo=(tagAccountsInfo *)m_AccountsControl.GetItemDataPtr(nCurrentSel);

		//构造标识
		CString strUserID;
		strUserID.Format(TEXT("%ld"),pAccountsInfo->dwUserID);

		//用户表项
		CWHRegKey RegUserInfo;
		RegUserInfo.OpenRegKey(REG_USER_INFO,false);

		//删除表项
		RegUserInfo.RecurseDeleteKey(strUserID);

		//删除列表
		for (INT i=0;i<m_AccountsControl.GetCount();i++)
		{
			if (m_AccountsControl.GetItemDataPtr(i)==pAccountsInfo)
			{
				//删除字符
				m_AccountsControl.DeleteString(i);
				m_AccountsControl.SetWindowText(TEXT(""));

				//更新选择
				if ((m_AccountsControl.GetCurSel()==LB_ERR)&&(m_AccountsControl.GetCount()>0))
				{
					m_AccountsControl.SetCurSel(0);
				}

				break;
			}
		}
	}

	//设置焦点
	//pAccountsCtrl->SetFocus();

	//获取变量
	nCurrentSel=m_AccountsControl.GetCurSel();
	pAccountsInfo=(nCurrentSel!=LB_ERR)?(tagAccountsInfo *)m_AccountsControl.GetItemDataPtr(nCurrentSel):NULL;

	//设置密码
	m_PasswordControl.SetUserPassword((pAccountsInfo!=NULL)?pAccountsInfo->szPassword:TEXT(""));

	//记住密码
	m_cbRemPassword=(pAccountsInfo!=NULL)?(pAccountsInfo->szPassword[0]!=0):false;
	UpdateData(FALSE);

	return;
}

//找回密码
VOID CDlgLogon::OnBnFindPass()
{
// 	//构造地址
// 	TCHAR szLogonLink[256]=TEXT("");
// 	_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("%s/yzm/index.php"),szPlatformLink);
	// 获取配置文件
	TCHAR szPath[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szPath, sizeof(szPath));
	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\local.dat"), szPath);
	// 获取地址
	TCHAR szLogonLink[MAX_PATH]=TEXT("");
	GetPrivateProfileString(TEXT("HyperLink"),TEXT("ForgetPassword"),TEXT(""), szLogonLink, sizeof(szLogonLink), szFileName);

	//打开页面
	ShellExecute(NULL,TEXT("OPEN"),szLogonLink,NULL,NULL,SW_NORMAL);

	return;
}

// 同意条款
VOID CDlgLogon::OnBnClickedAgreeAgreement()
{
//	BOOL flag = ((CButton*)GetDlgItem(IDC_CHECK_AGREE_AGREEMENT))->GetCheck();
//	((CButton*)GetDlgItem(IDC_CHECK_AGREE_AGREEMENT))->SetCheck(!flag);

	//构造地址
// 	TCHAR szLogonLink[256]=TEXT("");
// 	_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("%s/news/index?id=48"),szPlatformLink);

	// 获取配置文件
	TCHAR szPath[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szPath, sizeof(szPath));
	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\local.dat"), szPath);
	// 获取地址
	TCHAR szLink[MAX_PATH]=TEXT("");
	GetPrivateProfileString(TEXT("HyperLink"),TEXT("AgreementLink"),TEXT(""), szLink, sizeof(szLink), szFileName);

	//打开页面
	ShellExecute(NULL,TEXT("OPEN"),szLink,NULL,NULL,SW_NORMAL);
}


//注册帐号
VOID CDlgLogon::OnBnClickedRegister()
{
	//隐藏窗口
	ShowWindow(SW_HIDE);

	//获取地址
	CString strLogonServer;
	GetDlgItemText(IDC_LOGON_SERVER,strLogonServer);

	// 获取地址
	TCHAR szLogonServer[MAX_PATH];
	CGlobalServer GlobalServer;
	GlobalServer.ReadFileString(_T("LogonServer"), _T("LogonServer"), szLogonServer, CountArray(szLogonServer));

	//构造地址
	strLogonServer.TrimLeft();
	strLogonServer.TrimRight();
	lstrcpyn(m_szLogonServer,(strLogonServer.IsEmpty()==true)?szLogonServer:strLogonServer,CountArray(m_szLogonServer));

	//显示注册
	ASSERT(CMissionLogon::GetInstance()!=NULL);
	if (CMissionLogon::GetInstance()!=NULL) CMissionLogon::GetInstance()->ShowRegister();

	return;
}

//绘画背景
BOOL CDlgLogon::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//创建 DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(BufferDC);

	//设置缓冲
	pBufferDC->SetBkMode(TRANSPARENT);
	pBufferDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//加载资源
	CPngImage ImageBack;
	CPngImage ImageTitle;
	ImageTitle.LoadImage(AfxGetInstanceHandle(),TEXT("LOGON_TITLE"));
	ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("DLG_LOGON_BACK"));

	//加载图标
//	CPngImage ImageLogo;
//	ImageLogo.LoadImage(GetModuleHandle(SKIN_CONTROL_DLL_NAME),TEXT("SKIN_WND_LOGO"));

	//绘画背景
	ImageBack.DrawImage(pBufferDC,0,0);
//	ImageLogo.DrawImage(pBufferDC,11,4);
//	ImageTitle.DrawImage(pBufferDC,25,2);

	ImageTitle.DrawImage(pBufferDC,10,2);

	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	return TRUE;
}

//鼠标消息
VOID CDlgLogon::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//模拟标题
	if (Point.y<=CAPTION_SIZE)
	{
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));
	}

	return;
}

//控件颜色
HBRUSH CDlgLogon::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_BTN:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(61,61,61));
			return m_brBrush;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

void CDlgLogon::OnBnClickedCheckRempass()
{
	// TODO: 在此添加控件通知处理程序代码
	m_cbRemPassword = !m_cbRemPassword;
}
