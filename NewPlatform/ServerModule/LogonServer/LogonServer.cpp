#include "Stdafx.h"
#include "LogonServer.h"
#include "LogonServerDlg.h"

//////////////////////////////////////////////////////////////////////////////////

//程序对象
CLogonServerApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CLogonServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CLogonServerApp::CLogonServerApp()
{
}

//启动函数
BOOL CLogonServerApp::InitInstance()
{
	__super::InitInstance();

	ILog4zManager::GetInstance()->Start();
	ILog4zManager::GetInstance()->SetLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_INFO);

	//设置组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册表
	SetRegistryKey(szProduct);

	// 初始化异常捕获
	CR_INSTALL_INFO info;
	memset(&info, 0, sizeof(CR_INSTALL_INFO));
	info.cb = sizeof(CR_INSTALL_INFO);
	info.pszAppName = TEXT("LogonServerCrashRpt");
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;
	crInstall(&info);

	//显示窗口
	CLogonServerDlg LogonServerDlg;
	m_pMainWnd=&LogonServerDlg;
	LogonServerDlg.DoModal();

	return FALSE;
}

int CLogonServerApp::ExitInstance()
{
	return __super::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////////////
