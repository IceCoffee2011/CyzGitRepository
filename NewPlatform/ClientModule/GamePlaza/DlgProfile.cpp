#include "StdAfx.h"
#include "GamePlaza.h"
//#include "DlgRegistxer.h"
#include "DlgProfile.h"
#include "MissionLogon.h"

//////////////////////////////////////////////////////////////////////////////////

//圆角大小
#define ROUND_CX					7									//圆角宽度
#define ROUND_CY					7									//圆角高度

//屏幕位置
#define CAPTION_SIZE				35									//标题大小

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgProfile, CDialog)

	//系统消息
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()

	//按钮消息
	ON_BN_CLICKED(IDC_QUIT, OnBnClickedQuit)
	ON_BN_CLICKED(IDC_SELECT_FACE, OnBnClickedSelectFace)
	ON_BN_CLICKED(IDC_BTN_TEST, OnBnClickedCheckNick)

	//特殊消息
	ON_EN_CHANGE(IDC_LOGON_PASS1, OnEnChangeLogonPass)
	ON_EN_CHANGE(IDC_INSURE_PASS1, OnEnChangeInsurePass)
	ON_STN_CLICKED(IDC_CHANGE_VALIDATE, OnBnClickedChangeValidate)

	//控件消息
//	ON_CBN_SELCHANGE(IDC_ACCOUNTS, OnSelchangeQuestions)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgProfile::CDlgProfile() : CDialog(IDD_DLG_REGISTER)
{
	//设置变量
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
	
	//设置变量
	m_cbRemPassword=FALSE;
	m_cbLogonPassLevel=PASSWORD_LEVEL_0;
	m_cbInsurePassLevel=PASSWORD_LEVEL_0;

	//创建画刷
	m_brBrush.CreateSolidBrush(RGB(255,255,255));

	return;
}

//析构函数
CDlgProfile::~CDlgProfile()
{
}

//控件绑定
VOID CDlgProfile::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//按钮控件
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
	
	//编辑控件
//	DDX_Control(pDX, IDC_ACCOUNTS, m_edAccounts);
//	DDX_Control(pDX, IDC_LOGON_PASS1, m_edLogonPass1);
//	DDX_Control(pDX, IDC_LOGON_PASS2, m_edLogonPass2);
//	DDX_Control(pDX, IDC_VALIDATE_CODE, m_edValidateCode);

	//其他控件
//	DDX_Control(pDX, IDC_CHANGE_VALIDATE, m_ChangeValidate);
//	DDX_Control(pDX, IDC_VALIDATE_CODE_WND, m_WndValidateCode);
}

//消息解释
BOOL CDlgProfile::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if (pMsg->message==WM_CHAR)
	{
		//密码过滤
		//if ((m_edLogonPass1.m_hWnd==pMsg->hwnd)&&(pMsg->wParam>=0x80)) return TRUE;
		//if ((m_edLogonPass2.m_hWnd==pMsg->hwnd)&&(pMsg->wParam>=0x80)) return TRUE;
		//if ((m_edInsurePass1.m_hWnd==pMsg->hwnd)&&(pMsg->wParam>=0x80)) return TRUE;
		//if ((m_edInsurePass2.m_hWnd==pMsg->hwnd)&&(pMsg->wParam>=0x80)) return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//配置函数
BOOL CDlgProfile::OnInitDialog()
{
	__super::OnInitDialog();

	//设置窗口
	ModifyStyle(0, WS_MINIMIZEBOX);
	ModifyStyle(0, WS_MAXIMIZEBOX);

	// 设置任务栏文字
	SetWindowText(TEXT("创建个人信息"));

	//加载资源
	CPngImage ImageBack;
	ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("DLG_PROFILE_BACK"));

	//设置大小
	CSize SizeWindow(ImageBack.GetWidth(),ImageBack.GetHeight());
	SetWindowPos(NULL,0,0,SizeWindow.cx,SizeWindow.cy,SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);

	//边框设置
	m_cmSecurityQuestions.SetDrawBorad(false);

	//渲染设置
	m_cmSecurityQuestions.SetRenderImage(false);

	//限制输入
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


	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btCancel.SetButtonImage(IDB_BT_PROFILE_CLOSE,hInstance,false,false);
//	m_btQuit.SetButtonImage(IDB_BT_REGISTER,TEXT("BT_REGISTER_QUIT"),hInstance,false,false);
	m_btEnter.SetButtonImage(IDB_BT_PROFILE_ENTER,hInstance,false,false);
	m_btTestNickname.SetButtonImage(IDB_BT_PROFILE_TEST,hInstance,false,false);

	CRect rcBtEnter;
	m_btTestNickname.GetClientRect(rcBtEnter);
	m_btTestNickname.MoveWindow(350, 79, rcBtEnter.Width(), rcBtEnter.Height());

	//获取头像
//	m_wFaceID=rand()%pIFaceItemControl->GetFaceCount();

	//设置头像
//	m_FaceItemView.SetSystemFace(m_wFaceID);

	//设置性别
	m_cbGender = GENDER_MALE;
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(BST_CHECKED);

	// 移动控件
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

	//居中窗口
	CenterWindow(this);

	//获取目录
//	TCHAR szPath[MAX_PATH]=TEXT("");
//	CWHService::GetWorkDirectory(szPath, sizeof(szPath));

	//读取配置
/*	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\Spreader.ini"),szPath);

	//读取推荐人
	TCHAR szSpreader[LEN_ACCOUNTS]=TEXT("");
	GetPrivateProfileString(TEXT("SpreaderInfo"),TEXT("SpreaderName"),TEXT(""),szSpreader,CountArray(szSpreader),szFileName);

	//设置名字
	if (szSpreader[0]!=0)
	{
		SetDlgItemText(IDC_SPREADER,szSpreader);
		((CEdit *)GetDlgItem(IDC_SPREADER))->SetReadOnly();
	}*/

	//工作目录
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//构造路径
	BYTE cbIndex=1;
	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\SecurityQuestions.ini"),szDirectory);
	TCHAR szItemName[16]=TEXT("");
	TCHAR szQuestion[LEN_PASSWORD]=TEXT("");
	// 读取密保问题配置
	while (true)
	{
		//读取信息
		_sntprintf(szItemName,CountArray(szItemName),TEXT("Question%d"),cbIndex);
		GetPrivateProfileString(TEXT("SecurityQuestions"),szItemName,TEXT(""),szQuestion,CountArray(szQuestion),szFileName);

		//结果判断
		if (szQuestion[0]!=0)
		{
			//设置变量
			cbIndex++;
//			bSuccess=true;

			//加入菜单
//			ChatShortMenu.AppendMenu(IDM_SELECT_CHAT_SHORT+cbIndex-1,szShortcut);
			m_cmSecurityQuestions.AddString(szQuestion);
		}

		//结束判断
		if (szQuestion[0]==0)
		{
			break;
		}
	}
	m_cmSecurityQuestions.SetCurSel(0);

	m_bCheckNickFlag = FALSE;
	return FALSE;
}

//确定函数
VOID CDlgProfile::OnOK()
{
	m_bCheckNickFlag = FALSE;

	//获取信息
	if (GetInformation()==false) return;

	//隐藏窗口
	ShowWindow(SW_HIDE);

	//执行注册
	CMissionLogon * pMissionLogon=CMissionLogon::GetInstance();

	//直接以不记住密码方式登录
	if (pMissionLogon!=NULL) pMissionLogon->PerformLogonMission(FALSE);

	return;
}

//取消函数
VOID CDlgProfile::OnCancel()
{
	//退出判断
	CInformation Information(this);
	if (Information.ShowMessageBox(TEXT("您确定要关闭游戏吗？"),MB_ICONQUESTION|MB_YESNO,0)!=IDYES)
	{
		return;
	}

	//全局设置
	CGlobalUnits::GetInstance()->DeleteUserCookie();
	CGlobalUserInfo::GetInstance()->ResetUserInfoData();

	DestroyWindow();
	AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);

	return;
}


//发送登录
WORD CDlgProfile::ConstructProfilePacket(BYTE cbBuffer[], WORD wBufferSize, WORD & wSubCmdID, BYTE cbValidateFlags)
{
	////变量定义
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

		//发送数据
		return sizeof(CMD_GP_UserProfile);	
	}
}

//获取信息
bool CDlgProfile::GetInformation()
{
	//验证信息
	TCHAR szValidateCode[VALIDATE_COUNT+1]=TEXT("");
	GetDlgItemText(IDC_VALIDATE_CODE,szValidateCode,CountArray(szValidateCode));

	//密码信息
	TCHAR szLogonPass2[LEN_PASSWORD]=TEXT("");
	TCHAR szInsurePass2[LEN_PASSWORD]=TEXT("");
	//GetDlgItemText(IDC_LOGON_PASS2,szLogonPass2,CountArray(szLogonPass2));
	//GetDlgItemText(IDC_LOGON_PASS1,m_szLogonPass,CountArray(m_szLogonPass));
	//GetDlgItemText(IDC_INSURE_PASS2,szInsurePass2,CountArray(szInsurePass2));
	//GetDlgItemText(IDC_INSURE_PASS1,m_szInsurePass,CountArray(m_szInsurePass));

	//获取字符
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

	// 得到密保问题和密保回答
	GetControlString(IDC_SECURITY_QUESTIONS, m_szSecurityQuestion, CountArray(m_szSecurityQuestion));
	GetControlString(IDC_EDIT_SECURITY_ANSWER, m_szSecurityAnswer, CountArray(m_szSecurityAnswer));

	//用户性别
	if (((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck()==BST_CHECKED) m_cbGender=GENDER_MALE;
	if (((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck()==BST_CHECKED) m_cbGender=GENDER_FEMALE;

	//变量定义
	TCHAR szDescribe[128]=TEXT("");
	CUserItemElement * pUserItemElement=CUserItemElement::GetInstance();

	////帐号判断
	//if (pUserItemElement->EfficacyAccounts(m_szAccounts,szDescribe,CountArray(szDescribe))==false)
	//{
	//	//提示信息
	//	CInformation Information(this);
	//	Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

	//	//设置焦点
	//	m_edAccounts.SetFocus();

	//	return false;
	//}

	//昵称判断
	if (pUserItemElement->EfficacyNickName(m_szNickName,szDescribe,CountArray(szDescribe))==false)
	{
		//提示信息
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		//设置焦点
		m_edNickName.SetFocus();

		return false;
	}

// 	// 手机号不能为空
// 	if (_tcsclen(m_szMobilePhone) == 0 || _tcsclen(m_szMobilePhone) != 11)
// 	{
// 		_sntprintf(szDescribe, sizeof(szDescribe), _T("请输入 11 位手机号码"));
// 		CInformation Information(this);
// 		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);
// 
// 		m_edMobilePhone.SetFocus();
// 		return false;
// 	}

	// 真实姓名不能为空
	if (_tcsclen(m_szCompellation) < 2)
	{
		_sntprintf(szDescribe, sizeof(szDescribe), _T("请输入真实姓名"));
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		m_edCompellation.SetFocus();
		return false;
	}

	// 身份证号不能为空
	if (pUserItemElement->EfficacyPassPortID(m_szPassPortID, szDescribe, CountArray(szDescribe))==false)
	{
		//提示信息
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		//设置焦点
		m_edPassPortID.SetFocus();

		return false;
	}

	// 需要选择密保问题
	if (_tcsclen(m_szCompellation) <= 0)
	{
		//提示信息
		_sntprintf(szDescribe, sizeof(szDescribe), _T("请选择密保问题"));
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		//设置焦点
		m_cmSecurityQuestions.SetFocus();

		return false;
	}

	// 需要选择密保问题
	if (_tcsclen(m_szSecurityQuestion) <= 0)
	{
		//提示信息
		_sntprintf(szDescribe, sizeof(szDescribe), _T("请选择密保问题"));
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		//设置焦点
		m_cmSecurityQuestions.SetFocus();

		return false;
	}

	// 需要选择密保问题
	if (_tcsclen(m_szSecurityAnswer) <= 0)
	{
		//提示信息
		_sntprintf(szDescribe, sizeof(szDescribe), _T("密保问题回答不能为空"));
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		//设置焦点
		m_edSecurityAnswer.SetFocus();

		return false;
	}

	////密码判断
	//if (pUserItemElement->EfficacyPassword(m_szLogonPass,szDescribe,CountArray(szDescribe))==false)
	//{
	//	//提示信息
	//	CInformation Information(this);
	//	Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

	//	//设置焦点
	//	m_edLogonPass1.SetFocus();

	//	return false;
	//}

	////密码判断
	//if (lstrcmp(m_szLogonPass,szLogonPass2)!=0)
	//{
	//	//提示信息
	//	CInformation Information(this);
	//	Information.ShowMessageBox(TEXT("您两次输入的帐号密码不一致，请重新输入！"),MB_ICONERROR,0);

	//	//设置焦点
	//	m_edLogonPass1.SetFocus();

	//	return false;
	//}

	////效验判断
	//if (m_WndValidateCode.CheckValidateCode(szValidateCode)==false)
	//{
	//	//提示信息
	//	CInformation Information(this);
	//	Information.ShowMessageBox(TEXT("效验码效验验证失败，请重新输入！"),MB_ICONERROR,0);

	//	//设置焦点
	//	m_edValidateCode.SetFocus();

	//	return false;
	//}

	return true;
}


//检测昵称
VOID CDlgProfile::OnBnClickedCheckNick()
{

	GetControlString(IDC_EDIT_NICKNAME,m_szNickName,CountArray(m_szNickName));

	//昵称判断
	TCHAR szDescribe[128]=TEXT("");
	CUserItemElement * pUserItemElement=CUserItemElement::GetInstance();
	if (pUserItemElement->EfficacyNickName(m_szNickName,szDescribe,CountArray(szDescribe))==false)
	{
		//提示信息
		CInformation Information(this);
		Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);

		//设置焦点
		m_edNickName.SetFocus();
	}
	m_bCheckNickFlag = TRUE;

	//执行注册
	CMissionLogon * pMissionLogon=CMissionLogon::GetInstance();

	//直接以不记住密码方式登录
	if (pMissionLogon!=NULL) pMissionLogon->PerformLogonMission(FALSE);

	return;
}

//获取字符
VOID CDlgProfile::GetControlString(UINT uControlID, TCHAR szString[], WORD wMaxCount)
{
	//获取字符
	CString strString;
	GetDlgItemText(uControlID,strString);

	//去掉空格
	strString.TrimLeft();
	strString.TrimRight();

	//拷贝字符
	lstrcpyn(szString,strString,wMaxCount);

	return;
}

//关闭按钮
VOID CDlgProfile::OnBnClickedQuit()
{
	//关闭窗口
	OnCancel();

	return;
}

//密码输入
VOID CDlgProfile::OnEnChangeLogonPass()
{
	//获取密码
	TCHAR szPassword[LEN_PASSWORD]=TEXT("");
	GetDlgItemText(IDC_LOGON_PASS1,szPassword,CountArray(szPassword));

	//等级判断
	CUserItemElement * pUserItemElement=CUserItemElement::GetInstance();
	BYTE cbPasswordLevel=pUserItemElement->GetPasswordLevel(szPassword);

	//更新窗口
	if (m_cbLogonPassLevel!=cbPasswordLevel)
	{
		//设置变量
		m_cbLogonPassLevel=cbPasswordLevel;

		//更新窗口
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	}

	return;
}

//密码输入
VOID CDlgProfile::OnEnChangeInsurePass()
{
	//获取密码
	TCHAR szPassword[LEN_PASSWORD]=TEXT("");
	GetDlgItemText(IDC_INSURE_PASS1,szPassword,CountArray(szPassword));

	//等级判断
	CUserItemElement * pUserItemElement=CUserItemElement::GetInstance();
	BYTE cbPasswordLevel=pUserItemElement->GetPasswordLevel(szPassword);

	//更新窗口
	if (m_cbInsurePassLevel!=cbPasswordLevel)
	{
		//设置变量
		m_cbInsurePassLevel=cbPasswordLevel;

		//更新窗口
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW);
	}

	return;
}

//头像选择
VOID CDlgProfile::OnBnClickedSelectFace()
{
	//创建组件
	if ((m_FaceSelectControl.GetInterface()==NULL)&&(m_FaceSelectControl.CreateInstance()==false))
	{
		CInformation Information(this);
		Information.ShowMessageBox(m_FaceSelectControl.GetErrorDescribe(),MB_ICONERROR);
		return;
	}

	//隐藏界面
	ShowWindow(SW_HIDE);

	//设置控件
	m_FaceSelectControl->SetAllowItemFull();

	//选择头像
	if (m_FaceSelectControl->GetSelectFaceID(m_wFaceID)==true)
	{
		m_FaceItemView.SetSystemFace(m_wFaceID);
	}

	//显示界面
	ShowWindow(SW_SHOW);

	return;
}

//更换验证
VOID CDlgProfile::OnBnClickedChangeValidate()
{
	//更换验证
	m_WndValidateCode.RandValidateCode();

	return;
}

//绘画背景
BOOL CDlgProfile::OnEraseBkgnd(CDC * pDC)
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
	ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("DLG_PROFILE_BACK"));

	//绘画背景
	ImageBack.DrawImage(pBufferDC,0,0);

	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	return TRUE;
}

//鼠标消息
VOID CDlgProfile::OnLButtonDown(UINT nFlags, CPoint Point)
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
