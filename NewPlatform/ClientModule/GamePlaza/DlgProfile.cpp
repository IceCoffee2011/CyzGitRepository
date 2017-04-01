#include "StdAfx.h"
#include "GamePlaza.h"
//#include "DlgRegistxer.h"
#include "DlgProfile.h"
#include "MissionLogon.h"

//////////////////////////////////////////////////////////////////////////////////

//Բ�Ǵ�С
#define ROUND_CX					7									//Բ�ǿ��
#define ROUND_CY					7									//Բ�Ǹ߶�

//��Ļλ��
#define CAPTION_SIZE				35									//�����С

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgProfile, CDialog)

	//ϵͳ��Ϣ
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()

	//��ť��Ϣ
	ON_BN_CLICKED(IDC_QUIT, OnBnClickedQuit)
	ON_BN_CLICKED(IDC_SELECT_FACE, OnBnClickedSelectFace)
	ON_BN_CLICKED(IDC_BTN_TEST, OnBnClickedCheckNick)

	//������Ϣ
	ON_EN_CHANGE(IDC_LOGON_PASS1, OnEnChangeLogonPass)
	ON_EN_CHANGE(IDC_INSURE_PASS1, OnEnChangeInsurePass)
	ON_STN_CLICKED(IDC_CHANGE_VALIDATE, OnBnClickedChangeValidate)

	//�ؼ���Ϣ
//	ON_CBN_SELCHANGE(IDC_ACCOUNTS, OnSelchangeQuestions)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgProfile::CDlgProfile() : CDialog(IDD_DLG_REGISTER)
{
	//���ñ���
	m_wFaceID=0;
	m_cbGender=0;
	m_szAccounts[0]=0;
	m_szNickName[0]=0;
	//m_szSpreader[0]=0;
	//m_szLogonPass[0]=0;
	//m_szInsurePass[0]=0;
	//m_szPassPortID[0]=0;
	//m_szCompellation[0]=0;
	m_szMobilePhone[0] = 0;
	m_szUnderWrite[0] = 0;
	m_szQQ[0] = 0;
	
	//���ñ���
	m_cbRemPassword=FALSE;
	m_cbLogonPassLevel=PASSWORD_LEVEL_0;
	m_cbInsurePassLevel=PASSWORD_LEVEL_0;

	//������ˢ
	m_brBrush.CreateSolidBrush(RGB(255,255,255));

	return;
}

//��������
CDlgProfile::~CDlgProfile()
{
}

//�ؼ���
VOID CDlgProfile::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//��ť�ؼ�
	DDX_Control(pDX, IDOK, m_btEnter);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_BTN_TEST, m_btTestNickname);
	DDX_Control(pDX, IDC_EDIT_NICKNAME, m_edNickName);
//	DDX_Control(pDX, IDC_EDIT_MOBILE, m_edMobilePhone);
//	DDX_Control(pDX, IDC_EDIT_QQ, m_edQQ);
//	DDX_Control(pDX, IDC_EDIT_SIGN, m_edUnderWrite);
	DDX_Control(pDX, IDC_PASSPORT_ID, m_edPassPortID);
	DDX_Control(pDX, IDC_EDIT_COMPELLATION, m_edCompellation);

	DDX_Control(pDX, IDC_SECURITY_QUESTIONS, m_cmSecurityQuestions);
	DDX_Control(pDX, IDC_EDIT_SECURITY_ANSWER, m_edSecurityAnswer);
	
	//�༭�ؼ�
//	DDX_Control(pDX, IDC_ACCOUNTS, m_edAccounts);
//	DDX_Control(pDX, IDC_LOGON_PASS1, m_edLogonPass1);
//	DDX_Control(pDX, IDC_LOGON_PASS2, m_edLogonPass2);
//	DDX_Control(pDX, IDC_VALIDATE_CODE, m_edValidateCode);

	//�����ؼ�
//	DDX_Control(pDX, IDC_CHANGE_VALIDATE, m_ChangeValidate);
//	DDX_Control(pDX, IDC_VALIDATE_CODE_WND, m_WndValidateCode);
}

//��Ϣ����
BOOL CDlgProfile::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if (pMsg->message==WM_CHAR)
	{
		//�������
		//if ((m_edLogonPass1.m_hWnd==pMsg->hwnd)&&(pMsg->wParam>=0x80)) return TRUE;
		//if ((m_edLogonPass2.m_hWnd==pMsg->hwnd)&&(pMsg->wParam>=0x80)) return TRUE;
		//if ((m_edInsurePass1.m_hWnd==pMsg->hwnd)&&(pMsg->wParam>=0x80)) return TRUE;
		//if ((m_edInsurePass2.m_hWnd==pMsg->hwnd)&&(pMsg->wParam>=0x80)) return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//���ú���
BOOL CDlgProfile::OnInitDialog()
{
	__super::OnInitDialog();

	//���ô���
	ModifyStyle(0, WS_MINIMIZEBOX);
	ModifyStyle(0, WS_MAXIMIZEBOX);

	// ��������������
	SetWindowText(TEXT("����������Ϣ"));

	//������Դ
	CPngImage ImageBack;
	ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("DLG_PROFILE_BACK"));

	//���ô�С
	CSize SizeWindow(ImageBack.GetWidth(),ImageBack.GetHeight());
	SetWindowPos(NULL,0,0,SizeWindow.cx,SizeWindow.cy,SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);

	//�߿�����
	m_cmSecurityQuestions.SetDrawBorad(false);

	//��Ⱦ����
	m_cmSecurityQuestions.SetRenderImage(false);

	//��������
//	m_edAccounts.LimitText(LEN_ACCOUNTS-1);
	m_edNickName.LimitText(LEN_NICKNAME-1);
//	m_edSpreader.LimitText(LEN_ACCOUNTS-1);
//	m_edLogonPass1.LimitText(LEN_PASSWORD-1);
//	m_edLogonPass2.LimitText(LEN_PASSWORD-1);
//	m_edInsurePass1.LimitText(LEN_PASSWORD-1);
//	m_edInsurePass2.LimitText(LEN_PASSWORD-1);
//	m_edPassPortID.LimitText(LEN_PASS_PORT_ID-1);
//	m_edCompellation.LimitText(LEN_COMPELLATION-1)
	
//	m_edMobilePhone.LimitText(LEN_MOBILE_PHONE-1);
//	m_edQQ.LimitText(LEN_QQ-1);	
//	m_edUnderWrite.LimitText(LEN_UNDER_WRITE-1);


	//���ð�ť
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btCancel.SetButtonImage(IDB_BT_PROFILE_CLOSE,hInstance,false,false);
//	m_btQuit.SetButtonImage(IDB_BT_REGISTER,TEXT("BT_REGISTER_QUIT"),hInstance,false,false);
	m_btEnter.SetButtonImage(IDB_BT_PROFILE_ENTER,hInstance,false,false);
	m_btTestNickname.SetButtonImage(IDB_BT_PROFILE_TEST,hInstance,false,false);

	CRect rcBtEnter;
	m_btTestNickname.GetClientRect(rcBtEnter);
	m_btTestNickname.MoveWindow(350, 79, rcBtEnter.Width(), rcBtEnter.Height());

	//��ȡͷ��
//	m_wFaceID=rand()%pIFaceItemControl->GetFaceCount();

	//����ͷ��
//	m_FaceItemView.SetSystemFace(m_wFaceID);

	//�����Ա�
	m_cbGender = GENDER_MALE;
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(BST_CHECKED);

	// �ƶ��ؼ�
	m_edNickName.MoveWindow(109, 81+2, 180, 18);
	m_edCompellation.MoveWindow(109, 140+2, 180, 18);
	m_edPassPortID.MoveWindow(109, 171+2, 180, 18);
	m_cmSecurityQuestions.MoveWindow(109, 202, 180, 18);
	m_edSecurityAnswer.MoveWindow(109, 233+2, 180, 18);		

//	m_edMobilePhone.MoveWindow(109, 140+2, 180, 18);
//	m_edCompellation.MoveWindow(109, 171+2, 180, 18);
//	m_edPassPortID.MoveWindow(109, 202+2, 180, 18);
//	m_edUnderWrite.MoveWindow(109, 233+2, 180, 18);

	CRect rcRadio1,rcRadio2;
	GetDlgItem(IDC_RADIO1)->GetClientRect(rcRadio1);
	GetDlgItem(IDC_RADIO2)->GetClientRect(rcRadio2);
	GetDlgItem(IDC_RADIO1)->MoveWindow(109, 110, rcRadio1.Width(), rcRadio1.Height());
	GetDlgItem(IDC_RADIO2)->MoveWindow(160, 110, rcRadio2.Width(), rcRadio2.Height());

	//���д���
	CenterWindow(this);

	//��ȡĿ¼
//	TCHAR szPath[MAX_PATH]=TEXT("");
//	CWHService::GetWorkDirectory(szPath, sizeof(szPath));

	//��ȡ����
/*	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\Spreader.ini"),szPath);

	//��ȡ�Ƽ���
	TCHAR szSpreader[LEN_ACCOUNTS]=TEXT("");
	GetPrivateProfileString(TEXT("SpreaderInfo"),TEXT("SpreaderName"),TEXT(""),szSpreader,CountArray(szSpreader),szFileName);

	//��������
	if (szSpreader[0]!=0)
	{
		SetDlgItemText(IDC_SPREADER,szSpreader);
		((CEdit *)GetDlgItem(IDC_SPREADER))->SetReadOnly();
	}*/

	//����Ŀ¼
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//����·��
	BYTE cbIndex=1;
	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\SecurityQuestions.ini"),szDirectory);
	TCHAR szItemName[16]=TEXT("");
	TCHAR szQuestion[LEN_PASSWORD]=TEXT("");
	// ��ȡ�ܱ���������
	while (true)
	{
		//��ȡ��Ϣ
		_sntprintf(szItemName,CountArray(szItemName),TEXT("Question%d"),cbIndex);
		GetPrivateProfileString(TEXT("SecurityQuestions"),szItemName,TEXT(""),szQuestion,CountArray(szQuestion),szFileName);

		//����ж�
		if (szQuestion[0]!=0)
		{
			//���ñ���
			cbIndex++;
//			bSuccess=true;

			//����˵�
//			ChatShortMenu.AppendMenu(IDM_SELECT_CHAT_SHORT+cbIndex-1,szShortcut);
			m_cmSecurityQuestions.AddString(szQuestion);
		}

		//�����ж�
		if (szQuestion[0]==0)
		{
			break;
		}
	}
	m_cmSecurityQuestions.SetCurSel(0);

	m_bCheckNickFlag = FALSE;
	return FALSE;
}

//ȷ������
VOID CDlgProfile::OnOK()
{
	m_bCheckNickFlag = FALSE;

	//��ȡ��Ϣ
	if (GetInformation()==false) return;

	//���ش���
	ShowWindow(SW_HIDE);

	//ִ��ע��
	CMissionLogon * pMissionLogon=CMissionLogon::GetInstance();

	//ֱ���Բ���ס���뷽ʽ��¼
	if (pMissionLogon!=NULL) pMissionLogon->PerformLogonMission(FALSE);

	return;
}

//ȡ������
VOID CDlgProfile::OnCancel()
{
	//�˳��ж�
	CInformation Information(this);
	if (Information.ShowMessageBox(TEXT("��ȷ��Ҫ�ر���Ϸ��"),MB_ICONQUESTION|MB_YESNO,0)!=IDYES)
	{
		return;
	}

	//ȫ������
	CGlobalUnits::GetInstance()->DeleteUserCookie();
	CGlobalUserInfo::GetInstance()->ResetUserInfoData();

	DestroyWindow();
	AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);

	return;
}


//���͵�¼
WORD CDlgProfile::ConstructProfilePacket(BYTE cbBuffer[], WORD wBufferSize, WORD & wSubCmdID, BYTE cbValidateFlags)
{
	////��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
	
	if(m_bCheckNickFlag)
	{
		wSubCmdID = SUB_GP_CHECKNICK;

		CMD_GP_CheckNick * pCheckNick = (CMD_GP_CheckNick *)cbBuffer;
		pCheckNick->dwUserID = pGlobalUserData->dwUserID;
		lstrcpyn(pCheckNick->szNickName,m_szNickName,CountArray(pCheckNick->szNickName));

		return sizeof(CMD_GP_CheckNick);
	}
	else
	{
		wSubCmdID = SUB_GP_REGISTER_PROFILE;

		CMD_GP_UserProfile * pRegisterProfile = (CMD_GP_UserProfile *)cbBuffer;
		pRegisterProfile->dwUserID = pGlobalUserData->dwUserID;
		pRegisterProfile->cbGender = m_cbGender;
		lstrcpyn(pRegisterProfile->szNickName,m_szNickName,CountArray(pRegisterProfile->szNickName));
		lstrcpyn(pRegisterProfile->szPassword,pGlobalUserData->szPassword,CountArray(pGlobalUserData->szPassword));

		lstrcpyn(pRegisterProfile->szMobile,m_szMobilePhone,CountArray(pRegisterProfile->szMobile));
		lstrcpyn(pRegisterProfile->szUnderWrite,m_szUnderWrite,CountArray(pRegisterProfile->szUnderWrite));
		lstrcpyn(pRegisterProfile->szQQ,m_szQQ,CountArray(pRegisterProfile->szQQ));
		lstrcpyn(pRegisterProfile->szPassPortID,m_szPassPortID,CountArray(pRegisterProfile->szPassPortID));
		lstrcpyn(pRegisterProfile->szCompellation,m_szCompellation,CountArray(pRegisterProfile->szCompellation));

		lstrcpyn(pRegisterProfile->szSecurityQuestion,m_szSecurityQuestion,CountArray(pRegisterProfile->szSecurityQuestion));
		lstrcpyn(pRegisterProfile->szSecurityAnswer,m_szSecurityAnswer,CountArray(pRegisterProfile->szSecurityAnswer));

		//��������
		return sizeof(CMD_GP_UserProfile);	
	}
}

//��ȡ��Ϣ
bool CDlgProfile::GetInformation()
{
	//��֤��Ϣ
	TCHAR szValidateCode[VALIDATE_COUNT+1]=TEXT("");
	GetDlgItemText(IDC_VALIDATE_CODE,szValidateCode,CountArray(szValidateCode));

	//������Ϣ
	TCHAR szLogonPass2[LEN_PASSWORD]=TEXT("");
	TCHAR szInsurePass2[LEN_PASSWORD]=TEXT("");
	//GetDlgItemText(IDC_LOGON_PASS2,szLogonPass2,CountArray(szLogonPass2));
	//GetDlgItemText(IDC_LOGON_PASS1,m_szLogonPass,CountArray(m_szLogonPass));
	//GetDlgItemText(IDC_INSURE_PASS2,szInsurePass2,CountArray(szInsurePass2));
	//GetDlgItemText(IDC_INSURE_PASS1,m_szInsurePass,CountArray(m_szInsurePass));

	//��ȡ�ַ�
	//GetControlString(IDC_ACCOUNTS,m_szAccounts,CountArray(m_szAccounts));
	GetControlString(IDC_EDIT_NICKNAME,m_szNickName,CountArray(m_szNickName));
	//GetControlString(IDC_SPREADER,m_szSpreader,CountArray(m_szSpreader));
	//GetControlString(IDC_PASSPORT_ID,m_szPassPortID,CountArray(m_szPassPortID));
	//GetControlString(IDC_COMPELLATION,m_szCompellation,CountArray(m_szCompellation));
	//GetControlString(IDC_EDIT_SIGN,m_szNickName,CountArray(m_szNickName));

	GetControlString(IDC_EDIT_QQ,m_szQQ,CountArray(m_szQQ));
//	GetControlString(IDC_EDIT_MOBILE,m_szMobilePhone,CountArray(m_szMobilePhone));
//	GetControlString(IDC_EDIT_SIGN,m_szUnderWrite,CountArray(m_szUnderWrite));
	GetControlString(IDC_PASSPORT_ID, m_szPassPortID, CountArray(m_szPassPortID));
	GetControlString(IDC_EDIT_COMPELLATION, m_szCompellation, CountArray(m_szCompellation));

	// �õ��ܱ�������ܱ��ش�
	GetControlString(IDC_SECURITY_QUESTIONS, m_szSecurityQuestion, CountArray(m_szSecurityQuestion));
	GetControlString(IDC_EDIT_SECURITY_ANSWER, m_szSecurityAnswer, CountArray(m_szSecurityAnswer));

	//�û��Ա�
	if (((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck()==BST_CHECKED) m_cbGender=GENDER_MALE;
	if (((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck()==BST_CHECKED) m_cbGender=GENDER_FEMALE;

	//��������
	TCHAR szDescribe[128]=TEXT("");
	CUserItemElement * pUserItemElement=CUserItemElement::GetInstance();

	////�ʺ��ж�
	//if (pUserItemElement->EfficacyAccounts(m_szAccounts,szDescribe,CountArray(szDescribe))==false)
	//{
	//	//��ʾ��Ϣ
	//	CInformation Information(this);
	//	Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

	//	//���ý���
	//	m_edAccounts.SetFocus();

	//	return false;
	//}

	//�ǳ��ж�
	if (pUserItemElement->EfficacyNickName(m_szNickName,szDescribe,CountArray(szDescribe))==false)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		//���ý���
		m_edNickName.SetFocus();

		return false;
	}

// 	// �ֻ��Ų���Ϊ��
// 	if (_tcsclen(m_szMobilePhone) == 0 || _tcsclen(m_szMobilePhone) != 11)
// 	{
// 		_sntprintf(szDescribe, sizeof(szDescribe), _T("������ 11 λ�ֻ�����"));
// 		CInformation Information(this);
// 		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);
// 
// 		m_edMobilePhone.SetFocus();
// 		return false;
// 	}

	// ��ʵ��������Ϊ��
	if (_tcsclen(m_szCompellation) < 2)
	{
		_sntprintf(szDescribe, sizeof(szDescribe), _T("��������ʵ����"));
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		m_edCompellation.SetFocus();
		return false;
	}

	// ���֤�Ų���Ϊ��
	if (pUserItemElement->EfficacyPassPortID(m_szPassPortID, szDescribe, CountArray(szDescribe))==false)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		//���ý���
		m_edPassPortID.SetFocus();

		return false;
	}

	// ��Ҫѡ���ܱ�����
	if (_tcsclen(m_szCompellation) <= 0)
	{
		//��ʾ��Ϣ
		_sntprintf(szDescribe, sizeof(szDescribe), _T("��ѡ���ܱ�����"));
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		//���ý���
		m_cmSecurityQuestions.SetFocus();

		return false;
	}

	// ��Ҫѡ���ܱ�����
	if (_tcsclen(m_szSecurityQuestion) <= 0)
	{
		//��ʾ��Ϣ
		_sntprintf(szDescribe, sizeof(szDescribe), _T("��ѡ���ܱ�����"));
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		//���ý���
		m_cmSecurityQuestions.SetFocus();

		return false;
	}

	// ��Ҫѡ���ܱ�����
	if (_tcsclen(m_szSecurityAnswer) <= 0)
	{
		//��ʾ��Ϣ
		_sntprintf(szDescribe, sizeof(szDescribe), _T("�ܱ�����ش���Ϊ��"));
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		//���ý���
		m_edSecurityAnswer.SetFocus();

		return false;
	}

	////�����ж�
	//if (pUserItemElement->EfficacyPassword(m_szLogonPass,szDescribe,CountArray(szDescribe))==false)
	//{
	//	//��ʾ��Ϣ
	//	CInformation Information(this);
	//	Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

	//	//���ý���
	//	m_edLogonPass1.SetFocus();

	//	return false;
	//}

	////�����ж�
	//if (lstrcmp(m_szLogonPass,szLogonPass2)!=0)
	//{
	//	//��ʾ��Ϣ
	//	CInformation Information(this);
	//	Information.ShowMessageBox(TEXT("������������ʺ����벻һ�£����������룡"),MB_ICONERROR,0);

	//	//���ý���
	//	m_edLogonPass1.SetFocus();

	//	return false;
	//}

	////Ч���ж�
	//if (m_WndValidateCode.CheckValidateCode(szValidateCode)==false)
	//{
	//	//��ʾ��Ϣ
	//	CInformation Information(this);
	//	Information.ShowMessageBox(TEXT("Ч����Ч����֤ʧ�ܣ����������룡"),MB_ICONERROR,0);

	//	//���ý���
	//	m_edValidateCode.SetFocus();

	//	return false;
	//}

	return true;
}


//����ǳ�
VOID CDlgProfile::OnBnClickedCheckNick()
{

	GetControlString(IDC_EDIT_NICKNAME,m_szNickName,CountArray(m_szNickName));

	//�ǳ��ж�
	TCHAR szDescribe[128]=TEXT("");
	CUserItemElement * pUserItemElement=CUserItemElement::GetInstance();
	if (pUserItemElement->EfficacyNickName(m_szNickName,szDescribe,CountArray(szDescribe))==false)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		//���ý���
		m_edNickName.SetFocus();
	}
	m_bCheckNickFlag = TRUE;

	//ִ��ע��
	CMissionLogon * pMissionLogon=CMissionLogon::GetInstance();

	//ֱ���Բ���ס���뷽ʽ��¼
	if (pMissionLogon!=NULL) pMissionLogon->PerformLogonMission(FALSE);

	return;
}

//��ȡ�ַ�
VOID CDlgProfile::GetControlString(UINT uControlID, TCHAR szString[], WORD wMaxCount)
{
	//��ȡ�ַ�
	CString strString;
	GetDlgItemText(uControlID,strString);

	//ȥ���ո�
	strString.TrimLeft();
	strString.TrimRight();

	//�����ַ�
	lstrcpyn(szString,strString,wMaxCount);

	return;
}

//�رհ�ť
VOID CDlgProfile::OnBnClickedQuit()
{
	//�رմ���
	OnCancel();

	return;
}

//��������
VOID CDlgProfile::OnEnChangeLogonPass()
{
	//��ȡ����
	TCHAR szPassword[LEN_PASSWORD]=TEXT("");
	GetDlgItemText(IDC_LOGON_PASS1,szPassword,CountArray(szPassword));

	//�ȼ��ж�
	CUserItemElement * pUserItemElement=CUserItemElement::GetInstance();
	BYTE cbPasswordLevel=pUserItemElement->GetPasswordLevel(szPassword);

	//���´���
	if (m_cbLogonPassLevel!=cbPasswordLevel)
	{
		//���ñ���
		m_cbLogonPassLevel=cbPasswordLevel;

		//���´���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	}

	return;
}

//��������
VOID CDlgProfile::OnEnChangeInsurePass()
{
	//��ȡ����
	TCHAR szPassword[LEN_PASSWORD]=TEXT("");
	GetDlgItemText(IDC_INSURE_PASS1,szPassword,CountArray(szPassword));

	//�ȼ��ж�
	CUserItemElement * pUserItemElement=CUserItemElement::GetInstance();
	BYTE cbPasswordLevel=pUserItemElement->GetPasswordLevel(szPassword);

	//���´���
	if (m_cbInsurePassLevel!=cbPasswordLevel)
	{
		//���ñ���
		m_cbInsurePassLevel=cbPasswordLevel;

		//���´���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	}

	return;
}

//ͷ��ѡ��
VOID CDlgProfile::OnBnClickedSelectFace()
{
	//�������
	if ((m_FaceSelectControl.GetInterface()==NULL)&&(m_FaceSelectControl.CreateInstance()==false))
	{
		CInformation Information(this);
		Information.ShowMessageBox(m_FaceSelectControl.GetErrorDescribe(),MB_ICONERROR);
		return;
	}

	//���ؽ���
	ShowWindow(SW_HIDE);

	//���ÿؼ�
	m_FaceSelectControl->SetAllowItemFull();

	//ѡ��ͷ��
	if (m_FaceSelectControl->GetSelectFaceID(m_wFaceID)==true)
	{
		m_FaceItemView.SetSystemFace(m_wFaceID);
	}

	//��ʾ����
	ShowWindow(SW_SHOW);

	return;
}

//������֤
VOID CDlgProfile::OnBnClickedChangeValidate()
{
	//������֤
	m_WndValidateCode.RandValidateCode();

	return;
}

//�滭����
BOOL CDlgProfile::OnEraseBkgnd(CDC * pDC)
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
	ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("DLG_PROFILE_BACK"));

	//�滭����
	ImageBack.DrawImage(pBufferDC,0,0);

	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	return TRUE;
}

//�����Ϣ
VOID CDlgProfile::OnLButtonDown(UINT nFlags, CPoint Point)
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
HBRUSH CDlgProfile::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
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

//////////////////////////////////////////////////////////////////////////////////
