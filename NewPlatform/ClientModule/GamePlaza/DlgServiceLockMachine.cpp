#include "StdAfx.h"
#include "resource.h"
#include "GamePlaza.h"
#include "DlgServiceLockMachine.h"

BEGIN_MESSAGE_MAP(CDlgServiceLockMachine, CDlgServiceItem)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CDlgServiceLockMachine::CDlgServiceLockMachine() : CDlgServiceItem(IDD_DLG_LOCK_MACHINE)
{
	m_cbMachine = FALSE;
	ZeroMemory(m_szPassword,sizeof(m_szPassword));

	//任务组件
	m_MissionManager.InsertMissionItem(this);
}

CDlgServiceLockMachine::~CDlgServiceLockMachine()
{
}

VOID CDlgServiceLockMachine::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PASSWORD, m_edInsurePassword);

	DDX_Control(pDX, IDC_LOCKMACHINE, m_btLockMachine);
	DDX_Control(pDX, IDC_UNLOCKMACHINE, m_btUnLockMachine);
}

//创建函数
BOOL CDlgServiceLockMachine::OnInitDialog()
{
	__super::OnInitDialog();
	HINSTANCE hInstance=AfxGetInstanceHandle();
	// 移动控件位置
	m_edInsurePassword.SetDrawBorad(false);
	m_edInsurePassword.MoveWindow(92, 70, 180, 18);

	m_btLockMachine.SetButtonImage(IDB_BT_LOCK_MACHINE, hInstance, false, false);
	m_btUnLockMachine.SetButtonImage(IDB_BT_UNLOCK_MACHINE, hInstance, false, false);

	CRect rcLockMachine;
	m_btLockMachine.GetWindowRect(rcLockMachine);
	m_btLockMachine.MoveWindow(290,68,rcLockMachine.Width(), rcLockMachine.Height());
	CRect rcUnLockMachine;
	m_btLockMachine.GetWindowRect(rcUnLockMachine);
	m_btUnLockMachine.MoveWindow(290,68,rcUnLockMachine.Width(), rcUnLockMachine.Height());

	//设置按钮
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	m_cbMachine=(pGlobalUserData->cbMoorMachine==0)?TRUE:FALSE;

	if (pGlobalUserData->cbMoorMachine == 0)
	{
		m_btLockMachine.ShowWindow(TRUE);
		m_btUnLockMachine.ShowWindow(FALSE);
	}
	else
	{
		m_btUnLockMachine.ShowWindow(TRUE);
		m_btLockMachine.ShowWindow(FALSE);
	}

	return TRUE;
}

BOOL CDlgServiceLockMachine::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	UINT nCommandID=LOWORD(wParam);

	//功能按钮
	switch (nCommandID)
	{
	case IDC_LOCKMACHINE:
	case IDC_UNLOCKMACHINE:
		{
			//获取信息
			m_edInsurePassword.GetUserPassword(m_szPassword);

			//密码判断
			if (m_szPassword[0]==0)
			{
				//错误提示
				CInformation Information(this);
				Information.ShowMessageBox(TEXT("密码不能为空，请输入保险柜密码进行绑定！"),MB_ICONERROR);

				//设置焦点
				m_edInsurePassword.SetFocus();

				return TRUE;
			}

			//变量定义
			ASSERT(GetMissionManager()!=NULL);
			CMissionManager * pMissionManager=GetMissionManager();

			//发起连接
			if (pMissionManager->AvtiveMissionItem(this,false)==false)
			{
				//错误提示
				CInformation Information(this);
				Information.ShowMessageBox(TEXT("服务器连接失败，请检查网络配置以及网络连接状况！"),MB_ICONERROR);

				return TRUE;
			}

			//设置控件
//			m_btOk.EnableWindow(FALSE);
		}
		break;
// 	case IDC_UNLOCKMACHINE:
// 		{
// 			MessageBox(_T("11111"));
// 		}
// 		break;
	}

	return __super::OnCommand(wParam,lParam);
}

//连接事件
bool CDlgServiceLockMachine::OnEventMissionLink(INT nErrorCode)
{
//	MessageBox(_T("CDlgServiceLockMachine::OnEventMissionLink"));

	//错误处理
	if (nErrorCode!=0L)
	{
		//设置控件
		OnMissionConclude();

		//显示提示
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("尝试了所有的服务器都无法成功连接服务器，请留意网站维护公告！"),MB_ICONERROR);
	}
	else
	{
		//变量定义
		CMD_GP_ModifyMachine ModifyMachine;
		ZeroMemory(&ModifyMachine,sizeof(ModifyMachine));

		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//设置变量
		ModifyMachine.cbBind=m_cbMachine;
		ModifyMachine.dwUserID=pGlobalUserData->dwUserID;

		//加密密码
		CWHService::GetMachineIDEx(ModifyMachine.szMachineID);
		CWHEncrypt::MD5Encrypt(m_szPassword,ModifyMachine.szPassword);

		//发送数据
		ASSERT(GetMissionManager()!=NULL);
		GetMissionManager()->SendData(MDM_GP_USER_SERVICE,SUB_GP_MODIFY_MACHINE,&ModifyMachine,sizeof(ModifyMachine));

		OnMissionStart();
	}

	return true;
}

//关闭事件
bool CDlgServiceLockMachine::OnEventMissionShut(BYTE cbShutReason)
{
//	MessageBox(_T("CDlgServiceLockMachine::OnEventMissionShut"));

	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//重试任务
		CMissionManager * pMissionManager=GetMissionManager();
		if (pMissionManager->AvtiveMissionItem(this,true)==true) return true;

		//设置控件
		OnMissionConclude();

		//显示提示
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("由于当前服务器处理繁忙，帐号绑定操作处理失败，请稍后再重试！"),MB_ICONERROR);
	}
	return true;
}
//读取事件
bool CDlgServiceLockMachine::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
//	MessageBox(_T("CDlgServiceLockMachine::OnEventMissionRead"));
	if (Command.wMainCmdID==MDM_GP_USER_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_OPERATE_FAILURE:	//操作失败
			{
				//效验参数
				CMD_GP_OperateFailure * pOperateFailure=(CMD_GP_OperateFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString))) return false;

				//关闭连接
				CMissionManager * pMissionManager=GetMissionManager();
				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

				//显示消息
				if (pOperateFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pOperateFailure->szDescribeString,MB_ICONERROR,60);
				}

				//设置控件
				OnMissionConclude();
				m_edInsurePassword.SetFocus();

				return true;
			}
		case SUB_GP_OPERATE_SUCCESS:	//操作成功
			{
				//变量定义
				CMD_GP_OperateSuccess * pOperateSuccess=(CMD_GP_OperateSuccess *)pData;

				//效验数据
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString))) return false;

				//关闭连接
				CMissionManager * pMissionManager=GetMissionManager();
				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

				//变量定义
				CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
				tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();


				//设置变量
				pGlobalUserData->cbMoorMachine=m_cbMachine;
				m_cbMachine = (pGlobalUserData->cbMoorMachine==0)?TRUE:FALSE;

				//发送事件
 				CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
 				if (pPlatformEvent!=NULL) pPlatformEvent->SendPlatformEvent(EVENT_USER_MOOR_MACHINE,0L);
 
 				//显示消息
 				if (pOperateSuccess->szDescribeString[0]!=0)
 				{
 					CInformation Information(this);
 					Information.ShowMessageBox(pOperateSuccess->szDescribeString,MB_ICONINFORMATION,60);
 				}
				
				m_edInsurePassword.SetUserPassword(_T(""));
				
				OnMissionConclude();
				return true;
			}
		}
	}
	return true;
}


//开始任务
VOID CDlgServiceLockMachine::OnMissionStart()
{
//	MessageBox(_T("CDlgServiceLockMachine::OnMissionStart"));
	if (m_btLockMachine.IsWindowVisible())
	{
		m_btLockMachine.EnableWindow(FALSE);
	}

	if (m_btUnLockMachine.IsWindowVisible())
	{
		m_btUnLockMachine.EnableWindow(FALSE);
	}

}

//终止任务
VOID CDlgServiceLockMachine::OnMissionConclude()
{
	if (m_cbMachine == TRUE)
	{
		m_btLockMachine.ShowWindow(SW_SHOW);
		m_btUnLockMachine.ShowWindow(SW_HIDE);
	}
	else if (m_cbMachine == FALSE)
	{
		m_btUnLockMachine.ShowWindow(SW_SHOW);
		m_btLockMachine.ShowWindow(SW_HIDE);
	}

	if (m_btLockMachine.IsWindowVisible())
	{
		m_btLockMachine.EnableWindow(TRUE);
	}

	if (m_btUnLockMachine.IsWindowVisible())
	{
		m_btUnLockMachine.EnableWindow(TRUE);
	}
}

BOOL CDlgServiceLockMachine::OnEraseBkgnd(CDC * pDC)
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

	//加载资源
	CPngImage ImageServiceLockMachine;
	ImageServiceLockMachine.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_LOCKMACHINE"));

	//填充背景
	pBufferDC->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

	// 贴底图
	ImageServiceLockMachine.DrawImage(pBufferDC,0,0,ImageServiceLockMachine.GetWidth(),ImageServiceLockMachine.GetHeight(),0,0);


	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	return TRUE;
}
