#include "Stdafx.h"
#include "LogonServer.h"
#include "LogonServerDlg.h"

//////////////////////////////////////////////////////////////////////////////////

//�������
CLogonServerApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CLogonServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CLogonServerApp::CLogonServerApp()
{
}

//��������
BOOL CLogonServerApp::InitInstance()
{
	__super::InitInstance();

	ILog4zManager::GetInstance()->Start();
	ILog4zManager::GetInstance()->SetLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_INFO);

	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//����ע���
	SetRegistryKey(szProduct);

	// ��ʼ���쳣����
	CR_INSTALL_INFO info;
	memset(&info, 0, sizeof(CR_INSTALL_INFO));
	info.cb = sizeof(CR_INSTALL_INFO);
	info.pszAppName = TEXT("LogonServerCrashRpt");
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;
	crInstall(&info);

	//��ʾ����
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
