#include "Stdafx.h"
#include "DlgLogon.h"
#include "GamePlaza.h"
#include "MissionLogon.h"
#include "DlgProxyConfig.h"
#include ".\dlglogon.h"

//////////////////////////////////////////////////////////////////////////////////

//Բ�Ǵ�С
#define ROUND_CX					7									//Բ�ǿ��
#define ROUND_CY					7									//Բ�Ǹ߶�

//��Ļλ��
#define LAYERED_SIZE				1									//6603�ֲ��С
#define CAPTION_SIZE				35									//�����С

//�ؼ���ʶ
#define IDC_WEB_PUBLICIZE			300									//����ؼ�


//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgLogon, CDialog)

	//ϵͳ��Ϣ
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()

	//��ť��Ϣ
	ON_BN_CLICKED(IDC_REGISTER, OnBnClickedRegister)
	ON_BN_CLICKED(IDC_PROXY_CONFIG, OnBnClickedProxyConfig)
	ON_BN_CLICKED(IDC_CLEAR_ACCOUNT, OnBnClearAccount)
	ON_BN_CLICKED(IDC_FIND_PASS, OnBnFindPass)


	//�ؼ���Ϣ
	ON_CBN_SELCHANGE(IDC_ACCOUNTS, OnSelchangeAccounts)
	ON_CBN_EDITCHANGE(IDC_ACCOUNTS, OnCbnEditchangeAccounts)
	ON_EN_CHANGE(IDC_PASSWORD, OnCbnEditchangeAccounts) 

	ON_BN_CLICKED(IDC_CHECK_REMPASS, OnBnClickedCheckRempass)

	ON_STN_CLICKED(IDC_STATIC_AGREE_AGREEMENT, OnBnClickedAgreeAgreement)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgLogon::CDlgLogon() : CDialog(IDD_DLG_LOGON)
{
	//��¼��Ϣ
	m_szAccounts[0]=0;
	m_szPassword[0]=0;
	m_szLogonServer[0]=0;

	//���ñ���
	m_cbRemPassword=FALSE;
	m_cbLogonMode=LOGON_BY_ACCOUNTS;

	//������ˢ
	m_brBrush.CreateSolidBrush(RGB(215,223,228));

	return;
}

//��������
CDlgLogon::~CDlgLogon()
{
	//������Դ
	if (m_brBrush.GetSafeHandle()!=NULL)
	{
		m_brBrush.DeleteObject();
	}

	//ɾ������
	for (INT_PTR i=0;i<m_AccountsInfoArray.GetCount();i++)
	{
		SafeDelete(m_AccountsInfoArray[i]);
	}

	return;
}

//�ؼ���
VOID CDlgLogon::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//�󶨿ؼ�
	DDX_Control(pDX, IDOK, m_btEnter);
	DDX_Control(pDX, IDC_REGISTER, m_btRegister);

	//�󶨿ؼ�
	DDX_Control(pDX, IDCANCEL, m_btQuit);
	DDX_Control(pDX, IDC_PROXY_CONFIG, m_btOption);
	DDX_Control(pDX, IDC_CLEAR_ACCOUNT, m_btClear);
	DDX_Control(pDX, IDC_FIND_PASS, m_btFindPass);

	//�����ؼ�
	DDX_Control(pDX, IDC_ACCOUNTS, m_AccountsControl);
	DDX_Control(pDX, IDC_PASSWORD, m_PasswordControl);
	DDX_Check(pDX, IDC_CHECK_REMPASS, m_cbRemPassword);

	//�����ؼ�
	DDX_Control(pDX, IDC_LOGON_SERVER, m_ServerControl);

	DDX_Control(pDX, IDC_STATIC_AGREE_AGREEMENT, m_AgreeAgreement);
}

//��������
BOOL CDlgLogon::OnInitDialog()
{
	__super::OnInitDialog();

	//���ô���
	ModifyStyle(0, WS_MINIMIZEBOX);
	ModifyStyle(0, WS_MAXIMIZEBOX);

	//���ñ���
	SetWindowText(TEXT("�û���¼"));
	
	//������Դ
	CPngImage ImageBack;
	ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("DLG_LOGON_BACK"));

	//���ô�С
	CSize SizeWindow(ImageBack.GetWidth(),ImageBack.GetHeight());
	SetWindowPos(NULL,0,0,SizeWindow.cx,SizeWindow.cy,SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);

	//�߿�����
	m_ServerControl.SetDrawBorad(false);
	m_AccountsControl.SetDrawBorad(false);
	m_PasswordControl.SetDrawBorad(false);

	//��Ⱦ����
	m_ServerControl.SetRenderImage(false);
	m_AccountsControl.SetRenderImage(false);

	//���ð�ť
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


	// iii ���β���ʹ�õİ�ť
//	m_btDeleteRecord.ShowWindow(SW_HIDE);
//	m_btClear.ShowWindow(SW_HIDE);
	m_btOption.ShowWindow(SW_HIDE);



//	m_btLogonLink2.ShowWindow(SW_HIDE);
//	m_btLogonLink3.ShowWindow(SW_HIDE);
//	m_btLogonLink4.ShowWindow(SW_HIDE);


	//���ؼ�
	//CRect rcCreate(0,0,0,0);
	//m_PlatformPublicize.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,rcCreate,this,IDC_WEB_PUBLICIZE);

	//�����ַ
	//TCHAR szBillUrl[256]=TEXT("");
	//_sntprintf(szBillUrl,CountArray(szBillUrl),TEXT("http://www.aoyuevip.com/aa/gamelogin_lkgame_com.htm"),szPlatformLink);//%s/aa/RevealTrans.htm

	//���ù��
	//m_PlatformPublicize.Navigate(szBillUrl);
	//m_PlatformPublicize.SetWindowPos(NULL,LAYERED_SIZE+4,93,SizeWindow.cx-12-LAYERED_SIZE,212,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE);

    //���ؼ�
	CRect rcCreate(0,0,0,0);
	m_PlatformPublicize.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,rcCreate,this,IDC_WEB_PUBLICIZE);


	//��ȡĿ¼
	TCHAR szPath[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szPath, sizeof(szPath));
	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\local.dat"), szPath);
	//��ȡ��ַ
	TCHAR szBillUrl[256]=TEXT("");
	GetPrivateProfileString(TEXT("HyperLink"),TEXT("LogonAD"),TEXT(""), szBillUrl, sizeof(szBillUrl), szFileName);

	//���ù��
//	m_PlatformPublicize.SetBoradColor(RGB(51,25,3));
	m_PlatformPublicize.Navigate(szBillUrl);
	//m_PlatformPublicize.SetWindowPos(NULL,LAYERED_SIZE,42,SizeWindow.cx-2*LAYERED_SIZE,140,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE);
    m_PlatformPublicize.SetWindowPos(NULL,LAYERED_SIZE,30,SizeWindow.cx-2*LAYERED_SIZE,82,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE);
	//m_PlatformPublicize.ShowWindow(SW_HIDE);

	//������Ϣ
	LoadAccountsInfo();
	LoadLogonServerInfo();

	//���ý���
	if (m_AccountsControl.GetCurSel()!=LB_ERR)
	{
		//��Ϣ����
		OnSelchangeAccounts();

		//���ý���
		//m_PasswordControl.SetFocus();
	}
	else
	{
		//���ý���
		//m_AccountsControl.SetFocus();
	}

	//������ť
	HDWP hDwp=BeginDeferWindowPos(32);
	UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER|SWP_NOSIZE;

	DeferWindowPos(hDwp,m_btClear,NULL,358,229,0,0,uFlags);

	EndDeferWindowPos(hDwp);

	//���д���
	CenterWindow(this);

	return FALSE;
}

//��Ϣ����
BOOL CDlgLogon::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//ȷ������
VOID CDlgLogon::OnOK()
{
	LOGI("CDlgLogon::OnOK, Step 1="<<GetTickCount());
	//��ȡ��Ϣ
	if (GetInformation()==false) return;
	LOGI("CDlgLogon::OnOK, Step 2="<<GetTickCount());

	//���ش���
	ShowWindow(SW_HIDE);
	LOGI("CDlgLogon::OnOK, Step 3="<<GetTickCount());

	//ִ�е�¼
	CMissionLogon * pMissionLogon=CMissionLogon::GetInstance();
	if (pMissionLogon!=NULL) pMissionLogon->PerformLogonMission(m_cbRemPassword==TRUE);

	LOGI("CDlgLogon::OnOK, Step 4="<<GetTickCount());

	return;
}

//ȡ����Ϣ
VOID CDlgLogon::OnCancel()
{
	//�رմ���
	DestroyWindow();
	AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);

	return;
}

//��������
WORD CDlgLogon::ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize, BYTE cbValidateFlags)
{
	//��������
	TCHAR szPassword[LEN_MD5];
	CWHEncrypt::MD5Encrypt(m_szPassword,szPassword);

	//��������
	switch (m_cbLogonMode)
	{
	case LOGON_BY_ACCOUNTS:			//�ʺŵ�¼
		{
			//��������
			CMD_GP_LogonAccounts * pLogonAccounts=(CMD_GP_LogonAccounts *)cbBuffer;

			//ϵͳ��Ϣ
			pLogonAccounts->dwPlazaVersion=VERSION_PLAZA;

			//������ʶ
			CWHService::GetMachineIDEx(pLogonAccounts->szMachineID);

			//��¼��Ϣ
			lstrcpyn(pLogonAccounts->szPassword,szPassword,CountArray(pLogonAccounts->szPassword));
			lstrcpyn(pLogonAccounts->szAccounts,m_szAccounts,CountArray(pLogonAccounts->szAccounts));
			pLogonAccounts->cbValidateFlags=cbValidateFlags;

			//��������
			CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
			tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
			lstrcpyn(pGlobalUserData->szPassword,pLogonAccounts->szPassword,CountArray(pGlobalUserData->szPassword));

			return sizeof(CMD_GP_LogonAccounts);
		}
	}

	return 0;
}

//�����˺�
VOID CDlgLogon::LoadAccountsInfo()
{
	//��ȡ��Ϣ
	CWHRegKey RegUserInfo;
	if (RegUserInfo.OpenRegKey(REG_USER_INFO,false)==false) return;

	//��������
	DWORD dwRegIndex=0L;
	DWORD dwLastUserID=RegUserInfo.GetValue(TEXT("LastUserID"),0L);

	//��ȡ��Ϣ
	do
	{
		//��������
		tagAccountsInfo AccountsInfo;
		ZeroMemory(&AccountsInfo,sizeof(AccountsInfo));

		//��ȡ����
		TCHAR szKeyItemName[16]=TEXT("");
		if (RegUserInfo.EnumItemString(dwRegIndex++,szKeyItemName,CountArray(szKeyItemName))==false) break;

		//���Ӽ�
		CWHRegKey RegUserItem;
		if (RegUserItem.Attach(RegUserInfo.OpenItemKey(szKeyItemName))==NULL) continue;

		//�ʻ���Ϣ
		AccountsInfo.dwUserID=_tstol(szKeyItemName);
		RegUserItem.GetString(TEXT("GameID"),AccountsInfo.szGameID,CountArray(AccountsInfo.szGameID));
		RegUserItem.GetString(TEXT("UserAccount"),AccountsInfo.szAccounts,CountArray(AccountsInfo.szAccounts));

		//��ȡ����
		TCHAR szPassword[MAX_ENCRYPT_LEN]=TEXT("");
		RegUserItem.GetString(TEXT("UserPassword"),szPassword,CountArray(szPassword));

		//�⿪����
		if (szPassword[0]!=0)
		{
			CWHEncrypt::XorCrevasse(szPassword,AccountsInfo.szPassword,CountArray(AccountsInfo.szPassword));
		}

		//��������
		tagAccountsInfo * pAccountsInfo=new tagAccountsInfo;
		CopyMemory(pAccountsInfo,&AccountsInfo,sizeof(AccountsInfo));

		//��������
		m_AccountsInfoArray.InsertAt(m_AccountsInfoArray.GetCount(),pAccountsInfo);

	} while (true);

	//������Ϣ
	for (INT_PTR i=0;i<m_AccountsInfoArray.GetCount();i++)
	{
		//������Ϣ
		INT nGameIDItem=CB_ERR;
		INT nAccountsItem=CB_ERR;
		tagAccountsInfo * pAccountsInfo=m_AccountsInfoArray[i];
		
		//������Ϣ
		if (pAccountsInfo->szAccounts[0]!=0) nAccountsItem=m_AccountsControl.AddString(pAccountsInfo->szAccounts);

		//��������
		if (nAccountsItem!=CB_ERR) m_AccountsControl.SetItemDataPtr(nAccountsItem,pAccountsInfo);

		//����ѡ��
		if ((dwLastUserID!=0L)&&(pAccountsInfo->dwUserID==dwLastUserID))
		{
			if (nAccountsItem!=CB_ERR) m_AccountsControl.SetCurSel(nAccountsItem);
		}
	}

	//����ѡ��
	if ((m_AccountsControl.GetCount()>0)&&(m_AccountsControl.GetCurSel()==LB_ERR)) m_AccountsControl.SetCurSel(0);

	return;
}

//��ַ��Ϣ
VOID CDlgLogon::LoadLogonServerInfo()
{
	//��ȡ��Ŀ
	CGlobalServer GlobalServer;
	WORD wItemCount=GlobalServer.GetItemCount();

	//�ϴε�¼
	TCHAR szLastServer[LEN_SERVER]=TEXT("");
	GlobalServer.GetLastServerName(szLastServer);

	//��ȡ��Ϣ
	for (WORD i=0;i<wItemCount;i++)
	{
		//��ȡ��Ϣ
		TCHAR szServerItem[LEN_SERVER]=TEXT("");
		GlobalServer.GetServerItemName(i,szServerItem);

		//������Ϣ
		if (szServerItem[0]!=0)
		{
			//������Ϣ
			INT nItem=m_ServerControl.AddString(szServerItem);

			//�����¼
			if ((szLastServer[0]!=0)&&(lstrcmp(szLastServer,szServerItem)==0))
			{
				szLastServer[0]=0;
				m_ServerControl.SetCurSel(nItem);
			}
		}
	}

	//�����¼
	if (szLastServer[0]!=0)
	{
		m_ServerControl.SetCurSel(m_ServerControl.AddString(szLastServer));
	}

	//����ѡ�� 
	if (m_ServerControl.GetCurSel()==LB_ERR)
	{
		if (m_ServerControl.GetCount()==0L)
		{
			TCHAR szLogonServer[MAX_PATH];
			CGlobalServer GlobalServer;
			GlobalServer.ReadFileString(_T("LogonServer"), _T("LogonServer"), szLogonServer, CountArray(szLogonServer));
			m_ServerControl.SetCurSel(m_ServerControl.AddString(szLogonServer));  //�������÷������б�ѡ��
			/*m_ServerControl.SetCurSel(m_ServerControl.AddString(TEXT("������·һ")));
			m_ServerControl.SetCurSel(m_ServerControl.AddString(TEXT("������·��")));
			m_ServerControl.SetCurSel(m_ServerControl.AddString(TEXT("������·��")));*/
		}
		else
		{
			m_ServerControl.SetCurSel(0);
		}
	}

	return;
}

//��ȡ��Ϣ
bool CDlgLogon::GetInformation()
{
	//��ȡ��Ϣ
	GetDlgItemText(IDC_ACCOUNTS,m_szAccounts,CountArray(m_szAccounts));

	//��ȡ��Ϣ
	m_PasswordControl.GetUserPassword(m_szPassword);
	GetDlgItemText(IDC_LOGON_SERVER,m_szLogonServer,CountArray(m_szLogonServer)); //���÷������ĵط�

	//��ַ�ж�
	if (m_szLogonServer[0]==0L)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("��ѡ�񡰵�¼�����������ٵ�¼��������������������ָ�ϡ���ȡ������"),MB_ICONERROR,0);

		//��ʾ����
		ShowWindow(SW_SHOW);

		//���ý���
		//m_ServerControl.SetFocus();

		return false;
	}

	//�ʺ��ж�
	if ((m_cbLogonMode==LOGON_BY_ACCOUNTS)&&(m_szAccounts[0]==0L))
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("���������ʺź��ٵ�¼��������������������ָ�ϡ���ȡ������"),MB_ICONERROR,0);

		//��ʾ����
		ShowWindow(SW_SHOW);

		//���ý���
		//m_AccountsControl.SetFocus();

		return false;
	}

	//�����ж�
	if (m_szPassword[0]==0)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("��������������ٵ�¼��������������ϵ���߿ͷ���ȡ������"),MB_ICONERROR,0);

		//��ʾ����
		ShowWindow(SW_SHOW);

		//���ý���
		//m_PasswordControl.SetFocus();

		return false;
	}

	if ( ((CButton*)GetDlgItem(IDC_CHECK_AGREE_AGREEMENT))->GetCheck() == FALSE)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("�װ����û�����л������Ϸ��֧�֣��������������Ϸ�ľ��ʣ������Ķ��û�Э����ڡ�ͬ�����ǰ�򹳣�лл��"),MB_ICONERROR,0);
		
// 		//�����ַ
// 		TCHAR szLogonLink[256]=TEXT("");
// 		_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("%s/news/index?id=48"),szPlatformLink);
// 		//��ҳ��
// 		ShellExecute(NULL,TEXT("OPEN"),szLogonLink,NULL,NULL,SW_NORMAL);

		//��ʾ����
		ShowWindow(SW_SHOW);

		return false;
	}

	//������Ϣ
	UpdateData(TRUE);
	//m_cbRemPassword = m_btRem_Password.IsWindowVisible()?true:false;

	return true;
}

//ѡ��ı�
VOID CDlgLogon::OnSelchangeAccounts()
{
	//��ȡѡ��
	INT nCurrentSel=m_AccountsControl.GetCurSel();
	tagAccountsInfo * pAccountsInfo=(tagAccountsInfo *)m_AccountsControl.GetItemDataPtr(nCurrentSel);

	//���ÿؼ�
	if ((nCurrentSel!=LB_ERR)&&(pAccountsInfo!=NULL))
	{
		//�û��ʺ�
		m_AccountsControl.SetCurSel(nCurrentSel);

		//��������
		m_PasswordControl.SetUserPassword(pAccountsInfo->szPassword);

		//��ס����
		m_cbRemPassword=(pAccountsInfo->szPassword[0]!=0);
		UpdateData(FALSE);
	}
	else
	{
		//��������
		m_PasswordControl.SetUserPassword(TEXT(""));

		//��ס����
		//((CButton *)GetDlgItem(IDC_REM_PASSWORD))->SetCheck(BST_UNCHECKED);
		ASSERT(0);
		//m_btRem_Password.ShowWindow(SW_HIDE);
		//m_btRem_Password2.ShowWindow(SW_HIDE);
	}

	return;
}

//��������
VOID CDlgLogon::OnBnClickedProxyConfig()
{
	//���ش���
	ShowWindow(SW_HIDE);

	//��������
	CDlgProxyConfig DlgProxyConfig;
	DlgProxyConfig.DoModal();

	//��ʾ����
	ShowWindow(SW_SHOW);

	return;
}

//�༭�ı�
VOID CDlgLogon::OnCbnEditchangeAccounts()
{
	//��ȫ����
	m_cbRemPassword = FALSE;
	UpdateData(FALSE);

	//��������
	if (m_PasswordControl.IsFalsityPassword()) m_PasswordControl.SetUserPassword(NULL);

	

	return;
}

//ɾ����¼
VOID CDlgLogon::OnBnClearAccount()
{
	//��������
	tagAccountsInfo * pAccountsInfo=NULL;

	//��ȡ��Ϣ
	INT nCurrentSel=m_AccountsControl.GetCurSel();

	//ɾ������
	if (nCurrentSel!=LB_ERR)
	{
		//��ȡ����
		ASSERT(m_AccountsControl.GetItemDataPtr(nCurrentSel)!=NULL);
		pAccountsInfo=(tagAccountsInfo *)m_AccountsControl.GetItemDataPtr(nCurrentSel);

		//�����ʶ
		CString strUserID;
		strUserID.Format(TEXT("%ld"),pAccountsInfo->dwUserID);

		//�û�����
		CWHRegKey RegUserInfo;
		RegUserInfo.OpenRegKey(REG_USER_INFO,false);

		//ɾ������
		RegUserInfo.RecurseDeleteKey(strUserID);

		//ɾ���б�
		for (INT i=0;i<m_AccountsControl.GetCount();i++)
		{
			if (m_AccountsControl.GetItemDataPtr(i)==pAccountsInfo)
			{
				//ɾ���ַ�
				m_AccountsControl.DeleteString(i);
				m_AccountsControl.SetWindowText(TEXT(""));

				//����ѡ��
				if ((m_AccountsControl.GetCurSel()==LB_ERR)&&(m_AccountsControl.GetCount()>0))
				{
					m_AccountsControl.SetCurSel(0);
				}

				break;
			}
		}
	}

	//���ý���
	//pAccountsCtrl->SetFocus();

	//��ȡ����
	nCurrentSel=m_AccountsControl.GetCurSel();
	pAccountsInfo=(nCurrentSel!=LB_ERR)?(tagAccountsInfo *)m_AccountsControl.GetItemDataPtr(nCurrentSel):NULL;

	//��������
	m_PasswordControl.SetUserPassword((pAccountsInfo!=NULL)?pAccountsInfo->szPassword:TEXT(""));

	//��ס����
	m_cbRemPassword=(pAccountsInfo!=NULL)?(pAccountsInfo->szPassword[0]!=0):false;
	UpdateData(FALSE);

	return;
}

//�һ�����
VOID CDlgLogon::OnBnFindPass()
{
// 	//�����ַ
// 	TCHAR szLogonLink[256]=TEXT("");
// 	_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("%s/yzm/index.php"),szPlatformLink);
	// ��ȡ�����ļ�
	TCHAR szPath[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szPath, sizeof(szPath));
	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\local.dat"), szPath);
	// ��ȡ��ַ
	TCHAR szLogonLink[MAX_PATH]=TEXT("");
	GetPrivateProfileString(TEXT("HyperLink"),TEXT("ForgetPassword"),TEXT(""), szLogonLink, sizeof(szLogonLink), szFileName);

	//��ҳ��
	ShellExecute(NULL,TEXT("OPEN"),szLogonLink,NULL,NULL,SW_NORMAL);

	return;
}

// ͬ������
VOID CDlgLogon::OnBnClickedAgreeAgreement()
{
//	BOOL flag = ((CButton*)GetDlgItem(IDC_CHECK_AGREE_AGREEMENT))->GetCheck();
//	((CButton*)GetDlgItem(IDC_CHECK_AGREE_AGREEMENT))->SetCheck(!flag);

	//�����ַ
// 	TCHAR szLogonLink[256]=TEXT("");
// 	_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("%s/news/index?id=48"),szPlatformLink);

	// ��ȡ�����ļ�
	TCHAR szPath[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szPath, sizeof(szPath));
	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\local.dat"), szPath);
	// ��ȡ��ַ
	TCHAR szLink[MAX_PATH]=TEXT("");
	GetPrivateProfileString(TEXT("HyperLink"),TEXT("AgreementLink"),TEXT(""), szLink, sizeof(szLink), szFileName);

	//��ҳ��
	ShellExecute(NULL,TEXT("OPEN"),szLink,NULL,NULL,SW_NORMAL);
}


//ע���ʺ�
VOID CDlgLogon::OnBnClickedRegister()
{
	//���ش���
	ShowWindow(SW_HIDE);

	//��ȡ��ַ
	CString strLogonServer;
	GetDlgItemText(IDC_LOGON_SERVER,strLogonServer);

	// ��ȡ��ַ
	TCHAR szLogonServer[MAX_PATH];
	CGlobalServer GlobalServer;
	GlobalServer.ReadFileString(_T("LogonServer"), _T("LogonServer"), szLogonServer, CountArray(szLogonServer));

	//�����ַ
	strLogonServer.TrimLeft();
	strLogonServer.TrimRight();
	lstrcpyn(m_szLogonServer,(strLogonServer.IsEmpty()==true)?szLogonServer:strLogonServer,CountArray(m_szLogonServer));

	//��ʾע��
	ASSERT(CMissionLogon::GetInstance()!=NULL);
	if (CMissionLogon::GetInstance()!=NULL) CMissionLogon::GetInstance()->ShowRegister();

	return;
}

//�滭����
BOOL CDlgLogon::OnEraseBkgnd(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//���� DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(BufferDC);

	//���û���
	pBufferDC->SetBkMode(TRANSPARENT);
	pBufferDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//������Դ
	CPngImage ImageBack;
	CPngImage ImageTitle;
	ImageTitle.LoadImage(AfxGetInstanceHandle(),TEXT("LOGON_TITLE"));
	ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("DLG_LOGON_BACK"));

	//����ͼ��
//	CPngImage ImageLogo;
//	ImageLogo.LoadImage(GetModuleHandle(SKIN_CONTROL_DLL_NAME),TEXT("SKIN_WND_LOGO"));

	//�滭����
	ImageBack.DrawImage(pBufferDC,0,0);
//	ImageLogo.DrawImage(pBufferDC,11,4);
//	ImageTitle.DrawImage(pBufferDC,25,2);

	ImageTitle.DrawImage(pBufferDC,10,2);

	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	return TRUE;
}

//�����Ϣ
VOID CDlgLogon::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//ģ�����
	if (Point.y<=CAPTION_SIZE)
	{
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));
	}

	return;
}

//�ؼ���ɫ
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_cbRemPassword = !m_cbRemPassword;
}
