#include "Stdafx.h"
#include "Resource.h"
#include "LogonServerDlg.h"

//////////////////////////////////////////////////////////////////////////////////


//消息定义
#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//处理命令

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CLogonServerDlg, CDialog)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)

	//自定消息
	ON_MESSAGE(WM_PROCESS_CMD_LINE, OnMessageProcessCmdLine)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CLogonServerDlg::CLogonServerDlg() : CDialog(IDD_DLG_LOGON_SERVER)
{
}

//析构函数
CLogonServerDlg::~CLogonServerDlg()
{
}

//控件绑定
VOID CLogonServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}

//初始化函数
BOOL CLogonServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("登录服务器 -- [ 停止 ]"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//设置组件
	m_ServiceUnits.SetServiceUnitsSink(this);

	//命令处理
	LPCTSTR pszCmdLine = AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0] != 0) PostMessage(WM_PROCESS_CMD_LINE, 0, (LPARAM)pszCmdLine);

	return TRUE;
}

//确定消息
VOID CLogonServerDlg::OnOK()
{
	return;
}

//取消函数
VOID CLogonServerDlg::OnCancel()
{
	//关闭询问
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		LPCTSTR pszQuestion=TEXT("登录服务器正在运行中，您确实要关闭服务器吗？");
		if (AfxMessageBox(pszQuestion,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES) return;
	}

	//停止服务
	m_ServiceUnits.ConcludeService();

	__super::OnCancel();
}

//消息解释
BOOL CLogonServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//服务状态
VOID CLogonServerDlg::OnServiceUnitsStatus(enServiceStatus ServiceStatus)
{
	//状态设置
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//停止状态
		{
			//设置标题
			SetWindowText(TEXT("登录服务器 -- [ 停止 ]"));

			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

			//提示信息
			LPCTSTR pszDescribe=TEXT("服务停止成功");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Config:	//配置状态
		{
			//设置标题
			SetWindowText(TEXT("登录服务器 -- [ 初始化 ]"));

			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//提示信息
			LPCTSTR pszDescribe=TEXT("正在初始化组件...");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Service:	//服务状态
		{
			//设置标题
			SetWindowText(TEXT("登录服务器 -- [ 运行 ]"));

			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//提示信息
			LPCTSTR pszDescribe=TEXT("服务启动成功");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	}

	return;
}

//启动服务
VOID CLogonServerDlg::OnBnClickedStartService()
{
	//启动服务
	try
	{
		m_ServiceUnits.StartService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//停止服务
VOID CLogonServerDlg::OnBnClickedStopService()
{
	//停止服务
	try
	{
		m_ServiceUnits.ConcludeService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//关闭询问
BOOL CLogonServerDlg::OnQueryEndSession()
{
	//提示消息
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		CTraceService::TraceString(TEXT("服务正在运行中，系统要求注销回话请求失败"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}

// 命令处理
LRESULT CLogonServerDlg::OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	CWHCommandLine CommandLine;
	LPCTSTR pszCommandLine = (LPCTSTR)(lParam);

	//房间标识
	TCHAR szAutoStart[32] = TEXT("");
	if (CommandLine.SearchCommandItem(pszCommandLine, TEXT("/AutoStart:"), szAutoStart, CountArray(szAutoStart)) == true)
	{
		//获取房间
		WORD wAutoStart = (WORD)(_tstol(szAutoStart));

		//启动房间
		if (wAutoStart != 0)
		{
			//启动房间
			OnBnClickedStartService();
		}
	}

	return 0L;
}

//////////////////////////////////////////////////////////////////////////////////
