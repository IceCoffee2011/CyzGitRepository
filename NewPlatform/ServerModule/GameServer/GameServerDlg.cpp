#include "Stdafx.h"
#include "Resource.h"
#include "GameServerDlg.h"
//////////////////////////////////////////////////////////////////////////////////

//��Ϣ����
#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//��������

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameServerDlg, CDialog)

	//ϵͳ��Ϣ
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_OPEN_SERVER, OnBnClickedOpenServer)
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_CREATE_SERVER, OnBnClickedCreateServer)
	ON_BN_CLICKED(IDC_OPTION_SERVER, OnBnClickedOptionServer)
	ON_BN_CLICKED(IDC_OPTION_MATCH, OnBnClickedOptionMatch)
	ON_BN_CLICKED(IDC_LOAD_SQUAREMATCH, OnBnClickedLoadSquareMatch)

	//�Զ���Ϣ
	ON_MESSAGE(WM_PROCESS_CMD_LINE,OnMessageProcessCmdLine)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameServerDlg::CGameServerDlg() : CDialog(IDD_DLG_GAME_SERVER)
{
	//���ò���
	m_bAutoControl=false;
	m_bOptionSuccess=false;
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//��������
CGameServerDlg::~CGameServerDlg()
{
}

//�ؼ���
VOID CGameServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}

//��ʼ������
BOOL CGameServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("��Ϸ������ -- [ ֹͣ ]"));

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//�������
	m_ServiceUnits.SetServiceUnitsSink(this);

	//�����
	LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0]!=0) PostMessage(WM_PROCESS_CMD_LINE,0,(LPARAM)pszCmdLine);

	return TRUE;
}

//ȷ����Ϣ
VOID CGameServerDlg::OnOK()
{
	return;
}

//ȡ������
VOID CGameServerDlg::OnCancel()
{
	//�ر�ѯ��
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		if (m_ModuleInitParameter.GameServiceOption.wServerType & GAME_GENRE_USERCUSTOM == 0)
		{
			LPCTSTR pszQuestion = TEXT("��Ϸ���������������У���ȷʵҪ�رշ�������");
			if (AfxMessageBox(pszQuestion, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES) return;
		}
	}

	//ֹͣ����
	m_ServiceUnits.ConcludeService();

	__super::OnCancel();
}

//��Ϣ����
BOOL CGameServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//����״̬
VOID CGameServerDlg::OnServiceUnitsStatus(enServiceStatus ServiceStatus)
{
	//״̬����
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//ֹͣ״̬
		{
			//���±���
			UpdateServerTitle(ServiceStatus);

			//����ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

			//���ð�ť
			GetDlgItem(IDC_OPEN_MATCH)->EnableWindow(TRUE);
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(TRUE);
			if((m_ModuleInitParameter.GameServiceOption.wServerType&GAME_GENRE_MATCH)!=0)
				GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(TRUE);
			GetDlgItem(IDC_LOAD_SQUAREMATCH)->EnableWindow(TRUE);


			//���а�ť
			GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("����ֹͣ�ɹ�");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Config:	//����״̬
		{
			//���±���
			UpdateServerTitle(ServiceStatus);

			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//���ð�ť
			GetDlgItem(IDC_OPEN_MATCH)->EnableWindow(FALSE);
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(FALSE);
			GetDlgItem(IDC_LOAD_SQUAREMATCH)->EnableWindow(FALSE);

			//���а�ť
			GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("���ڳ�ʼ�����...");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Service:	//����״̬
		{
			//���±���
			UpdateServerTitle(ServiceStatus);

			//����ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//���ð�ť
			GetDlgItem(IDC_OPEN_MATCH)->EnableWindow(FALSE);
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_LOAD_SQUAREMATCH)->EnableWindow(TRUE);

			//���а�ť
			GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(TRUE);
			GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(TRUE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("���������ɹ�");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	}

	return;
}

//����ͼ��
VOID CGameServerDlg::UpdateServerLogo(LPCTSTR pszServerDLL)
{
	//������Դ
	HINSTANCE hInstance=AfxLoadLibrary(pszServerDLL);

	//����ͼ��
	if (hInstance!=NULL)
	{
		//������Դ
		AfxSetResourceHandle(hInstance);

		//������Դ
		CStatic * pServerLogo=(CStatic *)GetDlgItem(IDC_SERVER_LOGO);
		pServerLogo->SetIcon(::LoadIcon(hInstance,TEXT("SERVER_ICON")));

		//�ͷ���Դ
		AfxFreeLibrary(hInstance);
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	return;
}

//���±���
VOID CGameServerDlg::UpdateServerTitle(enServiceStatus ServiceStatus)
{
	//��������
	LPCTSTR pszStatusName=NULL;
	LPCTSTR pszServerName=NULL;

	//״̬�ַ�
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//ֹͣ״̬
		{
			pszStatusName=TEXT("ֹͣ");
			break;
		}
	case ServiceStatus_Config:	//����״̬
		{
			pszStatusName=TEXT("��ʼ��");
			break;
		}
	case ServiceStatus_Service:	//����״̬
		{
			pszStatusName=TEXT("����");
			break;
		}
	}

	//��������
	if (m_bOptionSuccess==false) pszServerName=TEXT("��Ϸ������");
	else pszServerName=m_ModuleInitParameter.GameServiceOption.szServerName;

	//�������
	TCHAR szTitle[128]=TEXT("");
	_sntprintf(szTitle,CountArray(szTitle),TEXT("[ %s ] -- [ %s ]"),pszServerName,pszStatusName);

	//���ñ���
	SetWindowText(szTitle);

	return;
}

//����״̬
VOID CGameServerDlg::UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter)
{
	//���ñ���
	m_bOptionSuccess=true;
	m_ModuleInitParameter=ModuleInitParameter;

	//���±���
	UpdateServerTitle(ServiceStatus_Stop);
	UpdateServerLogo(ModuleInitParameter.GameServiceAttrib.szServerDLLName);

	//���ð�ť
	GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
	GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(TRUE);

	if((ModuleInitParameter.GameServiceOption.wServerType&GAME_GENRE_MATCH)!=0)
	{
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(TRUE);
	}

	//���ÿؼ�
	SetDlgItemText(IDC_GAME_NAME,m_ModuleInitParameter.GameServiceAttrib.szGameName);
	SetDlgItemText(IDC_SERVER_NAME,m_ModuleInitParameter.GameServiceOption.szServerName);

	//�����˿�
	if (m_ModuleInitParameter.GameServiceOption.wServerPort==0)
	{
		SetDlgItemText(IDC_SERVER_PORT,TEXT("�Զ�����"));
	}
	else
	{
		SetDlgItemInt(IDC_SERVER_PORT,m_ModuleInitParameter.GameServiceOption.wServerPort);
	}

	//����ģ��
	LPCTSTR pszServerDLLName=m_ModuleInitParameter.GameServiceAttrib.szServerDLLName;
	m_ServiceUnits.CollocateService(pszServerDLLName,m_ModuleInitParameter.GameServiceOption,m_ModuleInitParameter.GameServiceTableOption);
	m_ServiceUnits.CollocateSquareMatch(m_ModuleInitParameter.GameSquareOptionArray,m_ModuleInitParameter.cbGameSquareOptionArrayCount);

	//������ʾ
	TCHAR szString[256]=TEXT("");
	LPCTSTR pszServerName=m_ModuleInitParameter.GameServiceOption.szServerName;
	_sntprintf(szString,CountArray(szString),TEXT("[ %s ] ����������سɹ�"),pszServerName);

	//�����Ϣ
	CTraceService::TraceString(szString,TraceLevel_Normal);

	return;
}

//��������
bool CGameServerDlg::StartServerService(WORD wServerID)
{
	//��������
	tagDataBaseParameter DataBaseParameter;
	ZeroMemory(&DataBaseParameter,sizeof(DataBaseParameter));

	//���ò���
	InitPlatformDBParameter(DataBaseParameter);
	m_ModuleDBParameter.SetPlatformDBParameter(DataBaseParameter);
	ZeroMemory(&DataBaseParameter, sizeof(DataBaseParameter));
	InitUserCustomDBParameter(DataBaseParameter);
	m_ModuleDBParameter.SetUserCustomDBParameter(DataBaseParameter);

	//��ȡ����
	CDlgServerItem DlgServerItem;
	if (DlgServerItem.OpenGameServer(wServerID)==false)
	{
		CTraceService::TraceString(TEXT("�������ò��������ڻ��߼���ʧ��"),TraceLevel_Exception);
		return false;
	}

	//����״̬
	UpdateParameterStatus(DlgServerItem.m_ModuleInitParameter);

	//��������
	try
	{
		m_ServiceUnits.StartService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return true;
}

//��ȡ����
bool CGameServerDlg::InitPlatformDBParameter(tagDataBaseParameter & DataBaseParameter)
{
	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szWorkDir,CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf(szIniFile,CountArray(szIniFile),TEXT("%s\\ServerParameter.ini"),szWorkDir);

	//��ȡ����
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	//������Ϣ
	DataBaseParameter.wDataBasePort=(WORD)IniData.ReadInt(TEXT("PlatformDB"),TEXT("DBPort"),1433);
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBAddr"),NULL,DataBaseParameter.szDataBaseAddr,CountArray(DataBaseParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBUser"),NULL,DataBaseParameter.szDataBaseUser,CountArray(DataBaseParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBPass"),NULL,DataBaseParameter.szDataBasePass,CountArray(DataBaseParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("PlatformDB"),TEXT("DBName"),szPlatformDB,DataBaseParameter.szDataBaseName,CountArray(DataBaseParameter.szDataBaseName));

	return true;
}

//��ȡ����
bool CGameServerDlg::InitUserCustomDBParameter(tagDataBaseParameter & DataBaseParameter)
{
	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szWorkDir, CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH] = TEXT("");
	_sntprintf(szIniFile, CountArray(szIniFile), TEXT("%s\\ServerParameter.ini"), szWorkDir);

	//��ȡ����
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	//������Ϣ
	DataBaseParameter.wDataBasePort = (WORD)IniData.ReadInt(TEXT("UserCustomDB"), TEXT("DBPort"), 1433);
	IniData.ReadEncryptString(TEXT("UserCustomDB"), TEXT("DBAddr"), NULL, DataBaseParameter.szDataBaseAddr, CountArray(DataBaseParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("UserCustomDB"), TEXT("DBUser"), NULL, DataBaseParameter.szDataBaseUser, CountArray(DataBaseParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("UserCustomDB"), TEXT("DBPass"), NULL, DataBaseParameter.szDataBasePass, CountArray(DataBaseParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("UserCustomDB"), TEXT("DBName"), szUserCustomDB, DataBaseParameter.szDataBaseName, CountArray(DataBaseParameter.szDataBaseName));

	return true;
}

//��������
VOID CGameServerDlg::OnBnClickedStartService()
{
	//��������
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

//ֹͣ����
VOID CGameServerDlg::OnBnClickedStopService()
{
	//ֹͣ����
	try
	{
		if (m_ModuleInitParameter.GameServiceOption.wServerType & GAME_GENRE_USERCUSTOM)
		{
			// �Խ�����ģʽ��,ֹͣ�����ֱ�ӹرս���
			// ��Ϊֹͣ�����,�������÷���.���ǹ����Խ����ӵ�������ݲ�û�б�����ModuleManager��,������ɺ������ʱ,�������ݻᱻ����
			LPCTSTR pszQuestion = TEXT("�Խ�����ģʽ��,ֹͣ����󽫻�ֱ�ӹرս��̣���ȷʵҪֹͣ������");
			if (AfxMessageBox(pszQuestion, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
			{
				PostMessage(WM_CLOSE);
				return;
			}
		}
		m_ServiceUnits.ConcludeService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//�򿪷���
VOID CGameServerDlg::OnBnClickedOpenServer()
{
	//��������
	tagDataBaseParameter DataBaseParameter;
	ZeroMemory(&DataBaseParameter,sizeof(DataBaseParameter));

	//���ò���
	InitPlatformDBParameter(DataBaseParameter);
	m_ModuleDBParameter.SetPlatformDBParameter(DataBaseParameter);
	ZeroMemory(&DataBaseParameter, sizeof(DataBaseParameter));
	InitUserCustomDBParameter(DataBaseParameter);
	m_ModuleDBParameter.SetUserCustomDBParameter(DataBaseParameter);

	//���÷���
	CDlgServerItem DlgServerItem;
	if (DlgServerItem.OpenGameServer()==false) return;

	//����״̬
	UpdateParameterStatus(DlgServerItem.m_ModuleInitParameter);

	return;
}

//��������
VOID CGameServerDlg::OnBnClickedCreateServer()
{
	//��������
	tagDataBaseParameter DataBaseParameter;
	ZeroMemory(&DataBaseParameter,sizeof(DataBaseParameter));

	//���ò���
	InitPlatformDBParameter(DataBaseParameter);
	m_ModuleDBParameter.SetPlatformDBParameter(DataBaseParameter);
	ZeroMemory(&DataBaseParameter, sizeof(DataBaseParameter));
	InitUserCustomDBParameter(DataBaseParameter);
	m_ModuleDBParameter.SetUserCustomDBParameter(DataBaseParameter);

	//��������
	CDlgServerWizard DlgServerWizard;
	if (DlgServerWizard.CreateGameServer()==false) return;

	//����״̬
	UpdateParameterStatus(DlgServerWizard.m_ModuleInitParameter);

	return;
}

//���÷���
VOID CGameServerDlg::OnBnClickedOptionServer()
{
	//��Ϸģ��
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(m_ModuleInitParameter.GameServiceAttrib.szServerDLLName,GAME_SERVICE_CREATE_NAME);

	//����ģ��
	if (GameServiceManager.CreateInstance()==false)
	{
		AfxMessageBox(TEXT("������������ڻ��߼���ʧ�ܣ����÷���ʧ��"),MB_ICONERROR);
		return;
	}

	//���÷���
	CDlgServerWizard DlgServerWizard;
	DlgServerWizard.SetWizardParameter(GameServiceManager.GetInterface(),&m_ModuleInitParameter.GameServiceOption, &(m_ModuleInitParameter.TableConfig));

	//��������
	if (DlgServerWizard.CreateGameServer()==false) return;

	//����״̬
	UpdateParameterStatus(DlgServerWizard.m_ModuleInitParameter);

	return;
}

//���ñ���
VOID CGameServerDlg::OnBnClickedOptionMatch()
{
	//���Ǳ����͹���
	if((m_ModuleInitParameter.GameServiceOption.wServerType&GAME_GENRE_MATCH)==0) return;

	//��������
	tagDataBaseParameter DataBaseParameter;
	ZeroMemory(&DataBaseParameter,sizeof(DataBaseParameter));

	//���ò���
	InitPlatformDBParameter(DataBaseParameter);
	m_ModuleDBParameter.SetPlatformDBParameter(DataBaseParameter);
	ZeroMemory(&DataBaseParameter, sizeof(DataBaseParameter));
	InitUserCustomDBParameter(DataBaseParameter);
	m_ModuleDBParameter.SetUserCustomDBParameter(DataBaseParameter);

	//���÷���
	CDlgServerMatch DlgServerMatch;
	if (DlgServerMatch.OpenGameMatch()==false) return;

	//����״̬
	UpdateParameterStatus(DlgServerMatch.m_ModuleInitParameter);


}

VOID CGameServerDlg::OnBnClickedLoadSquareMatch()
{
	//���ع㳡��
	CDlgServerItem DlgServerItem;
	if (DlgServerItem.LoadDBSquareMatch(m_ModuleInitParameter.GameServiceOption.wServerID)==false) return;
	
	//���¹㳡������
	for(int i=0; i<DlgServerItem.m_ModuleInitParameter.cbGameSquareOptionArrayCount; i++)
	{
		if(i>=20)break;

		if(DlgServerItem.m_ModuleInitParameter.GameSquareOptionArray[i].wMatchID<=0)continue;

		CTime tmLocalTime=CTime::GetCurrentTime();
		CTime tmStartTime(DlgServerItem.m_ModuleInitParameter.GameSquareOptionArray[i].StartTime);
		CTime tmEndTime(DlgServerItem.m_ModuleInitParameter.GameSquareOptionArray[i].EndTime);
		if(tmLocalTime>=tmEndTime)
		{
			LOGW("CGameServerDlg::OnBnClickedLoadSquareMatch ���ع㳡������, �˹㳡���ѽ��� wMatch= "\
				<< DlgServerItem.m_ModuleInitParameter.GameSquareOptionArray[i].wMatchID << " SquareName= "\
				<< DlgServerItem.m_ModuleInitParameter.GameSquareOptionArray[i].szSquareName);
			continue;			
		}

		bool bAdd=true;
		for(int j=0; j<m_ModuleInitParameter.cbGameSquareOptionArrayCount;j++)
		{
			if(j>=20)break;
			if(DlgServerItem.m_ModuleInitParameter.GameSquareOptionArray[i].wMatchID==m_ModuleInitParameter.GameSquareOptionArray[j].wMatchID)
			{//�Ѵ��ڹ㳡��
				bAdd=false;

				//ʣ��ʱ��
				CTime tmLocalTime=CTime::GetCurrentTime();
				CTime tmStartTime(m_ModuleInitParameter.GameSquareOptionArray[j].StartTime);
				CTime tmEndTime(m_ModuleInitParameter.GameSquareOptionArray[j].EndTime);

				if(tmLocalTime>=tmStartTime&&tmLocalTime<tmEndTime)
				{
					LOGW("CGameServerDlg::OnBnClickedLoadSquareMatch ���ع㳡��ʧ��, �㳡���ѿ�ʼ wMatch= "\
						<< m_ModuleInitParameter.GameSquareOptionArray[j].wMatchID << " SquareName= "\
						<< m_ModuleInitParameter.GameSquareOptionArray[j].szSquareName);
					break;
				}

				memcpy(&(m_ModuleInitParameter.GameSquareOptionArray[j]), &(DlgServerItem.m_ModuleInitParameter.GameSquareOptionArray[i]),sizeof(tagGameSquareOption));
				break;
			}
		}

		if(bAdd)memcpy(&(m_ModuleInitParameter.GameSquareOptionArray[m_ModuleInitParameter.cbGameSquareOptionArrayCount++]), &(DlgServerItem.m_ModuleInitParameter.GameSquareOptionArray[i]),sizeof(tagGameSquareOption));		
	}

	if(DlgServerItem.m_ModuleInitParameter.cbGameSquareOptionArrayCount)
	{
		m_ServiceUnits.UpdateSquareMatch(m_ModuleInitParameter.GameSquareOptionArray,m_ModuleInitParameter.cbGameSquareOptionArrayCount);
	}
	else
	{
		//������ʾ
		TCHAR szString[256]=TEXT("");
		_sntprintf(szString,CountArray(szString),TEXT("�޹㳡��������Ҫ����"));

		//�����Ϣ
		CTraceService::TraceString(szString,TraceLevel_Normal);

		LOGI("CGameServerDlg::OnBnClickedLoadSquareMatch() �޹㳡��������Ҫ����!");
	}
};
//�ر�ѯ��
BOOL CGameServerDlg::OnQueryEndSession()
{
	//��ʾ��Ϣ
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		CTraceService::TraceString(TEXT("�������������У�ϵͳҪ��ע���ػ�����ʧ��"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}

//�����
LRESULT CGameServerDlg::OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam)
{
	//��������
	CWHCommandLine CommandLine;
	LPCTSTR pszCommandLine=(LPCTSTR)(lParam);

	//�����ʶ
	TCHAR szSrverID[32]=TEXT("");
	if (CommandLine.SearchCommandItem(pszCommandLine,TEXT("/ServerID:"),szSrverID,CountArray(szSrverID))==true)
	{
		//��ȡ����
		WORD wServerID=(WORD)(_tstol(szSrverID));

		//��������
		if (wServerID!=0)
		{
			//���ñ���
			m_bAutoControl=true;

			//��������
			StartServerService(wServerID);
		}
	}

	return 0L;
}

//////////////////////////////////////////////////////////////////////////////////
