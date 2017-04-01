#include "Stdafx.h"
#include "GameServer.h"
#include "GameServerDlg.h"

//////////////////////////////////////////////////////////////////////////////////

//�������
CGameServerApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameServerApp::CGameServerApp()
{
}

//��������
BOOL CGameServerApp::InitInstance()
{
	__super::InitInstance();

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ILog4zManager::GetInstance()->Start();

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
	info.pszAppName = TEXT("GameServerCrashRpt");
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;
	info.uPriorities[CR_HTTP] = 3;								// Use HTTP the first.
	info.uPriorities[CR_SMTP] = CR_NEGATIVE_PRIORITY;			// Use SMTP the second.
	info.uPriorities[CR_SMAPI] = CR_NEGATIVE_PRIORITY;			// Use Simple MAPI the last.

	CrAutoInstallHelper cr_install_helper(&info);
	if (cr_install_helper.m_nInstallStatus != 0)
	{
	}

	//��ʾ����
	CGameServerDlg GameServerDlg;
	m_pMainWnd=&GameServerDlg;
	GameServerDlg.DoModal();

	ILog4zManager::GetInstance()->Stop();

	return FALSE;
}

int CGameServerApp::ExitInstance()
{
	_CrtDumpMemoryLeaks();
	crUninstall();
	return __super::ExitInstance();
}


//////////////////////////////////////////////////////////////////////////////////
