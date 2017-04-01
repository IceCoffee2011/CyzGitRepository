#include "Stdafx.h"
#include "GamePlaza.h"
#include "DlgService.h"
#include "DlgCustomFace.h"
#include "PlatformFrame.h"
#include "DlgLockMachine.h"
#include "WndUserInfoCtrl.h"
#include "DlgJyMx.h"

//////////////////////////////////////////////////////////////////////////////////

//控制按钮
#define IDC_USER_INFO_1				100									//用户按钮
#define IDC_USER_INFO_2				101									//用户按钮
#define IDC_USER_INFO_3				102									//用户按钮
#define IDC_USER_INFO_4				103									//用户按钮
#define IDC_USER_INFO_5				104									//用户按钮
#define IDC_USER_INFO_6				105									//用户按钮

#define IDC_USER_INFO_SET			106									//用户按钮
#define IDC_USER_INFO_EDIT			107									//编辑控件
#define IDC_USER_INFO_COPY			108
#define IDC_SYSTEM_SET				109									// 系统设置

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CWndUserInfoCtrl, CWnd)

	//系统消息
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()

	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()

	//自定消息
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_PLATFORM_EVENT, OnMessagePlatformEvent)

END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CEditUnderWrite, CSkinEditEx)
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CEditUnderWrite::CEditUnderWrite()
{
	//设置变量
	m_bNeedSend=false;
	m_pParentWnd=NULL;
	ZeroMemory(m_szUnderWrite,sizeof(m_szUnderWrite));

	//设置控件
	m_MissionManager.InsertMissionItem(this);

	return;
}

//析构函数
CEditUnderWrite::~CEditUnderWrite()
{
}

//消息解释
BOOL CEditUnderWrite::PreTranslateMessage(MSG * pMsg)
{
	//取消消息
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		//关闭控件
		CloseUnderWrite();

		return TRUE;
	}

	//确定消息
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_RETURN))
	{
		//设置焦点
		ASSERT(m_pParentWnd!=NULL);
		if (m_pParentWnd!=NULL) m_pParentWnd->SetFocus();

		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//连接事件
bool CEditUnderWrite::OnEventMissionLink(INT nErrorCode)
{
	//错误判断
	if (nErrorCode!=0L)
	{
		//错误提示
		CInformation Information(m_pParentWnd);
		Information.ShowMessageBox(TEXT("服务器连接失败，个性签名更新失败！"),MB_ICONERROR,30L);

		return true;
	}

	//设置变量
	m_bNeedSend=false;

	//变量定义
	ASSERT(GetMissionManager()!=NULL);
	CMissionManager * pMissionManager=GetMissionManager();

	//用户信息
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//变量定义
	CMD_GP_ModifyUnderWrite ModifyUnderWrite;
	ZeroMemory(&ModifyUnderWrite,sizeof(ModifyUnderWrite));

	//设置数据
	ModifyUnderWrite.dwUserID=pGlobalUserData->dwUserID;
	lstrcpyn(ModifyUnderWrite.szUnderWrite,m_szUnderWrite,CountArray(ModifyUnderWrite.szUnderWrite));
//	lstrcpyn(ModifyUnderWrite.szPassword,pGlobalUserData->szPassword,CountArray(ModifyUnderWrite.szPassword));

	//发送数据
	WORD wHeadSize=sizeof(ModifyUnderWrite)-sizeof(ModifyUnderWrite.szUnderWrite);
	pMissionManager->SendData(MDM_GP_USER_SERVICE,SUB_GP_MODIFY_UNDER_WRITE,&ModifyUnderWrite,wHeadSize+CountStringBuffer(ModifyUnderWrite.szUnderWrite));

	return true;
}

//关闭事件
bool CEditUnderWrite::OnEventMissionShut(BYTE cbShutReason)
{
	//关闭处理
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//重试任务
		CMissionManager * pMissionManager=GetMissionManager();
		if (pMissionManager->AvtiveMissionItem(this,true)==true) return true;

		//设置控件
		CloseUnderWrite();

		//显示提示
		CInformation Information(m_pParentWnd);
		Information.ShowMessageBox(TEXT("由于当前服务器处理繁忙，个性签名修改失败，请稍后再重试！"),MB_ICONERROR);
	}

	return true;
}

//读取事件
bool CEditUnderWrite::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//命令处理
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

				//设置控件
				CloseUnderWrite();

				//显示消息
				if (pOperateFailure->szDescribeString[0]!=0)
				{
					CInformation Information(m_pParentWnd);
					Information.ShowMessageBox(pOperateFailure->szDescribeString,MB_ICONERROR,60);
				}

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

				//设置信息
				CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
				tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
				lstrcpyn(pGlobalUserData->szUnderWrite,m_szUnderWrite,CountArray(pGlobalUserData->szUnderWrite));

				//设置控件
				CloseUnderWrite();

				//发送事件
				CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
				if (pPlatformEvent!=NULL) pPlatformEvent->SendPlatformEvent(EVENT_USER_INFO_UPDATE,0L);

				//显示消息
				if (pOperateSuccess->szDescribeString[0]!=0)
				{
					CInformation Information(m_pParentWnd);
					Information.ShowMessageBox(pOperateSuccess->szDescribeString,MB_ICONINFORMATION,60);
				}

				return true;
			}
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//修改判断
bool CEditUnderWrite::IsModifyStatus()
{
	//状态判断
	if (m_bNeedSend==true) return true;
	if (GetActiveStatus()==true) return true;

	return false;
}

//设置窗口
VOID CEditUnderWrite::SetParentWindow(CWnd * pParentWnd)
{
	//设置变量
	m_pParentWnd=pParentWnd;

	return;
}

//关闭控件
VOID CEditUnderWrite::CloseUnderWrite()
{
	//关闭判断
	if ((m_bNeedSend==false)&&(GetActiveStatus()==false))
	{
		//销毁窗口
		DestroyWindow();
	}
	else
	{
		//隐藏窗口
		ShowWindow(SW_HIDE);
	}

	//更新界面
	ASSERT(m_pParentWnd!=NULL);
	m_pParentWnd->RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);

	return;
}

//更新签名
VOID CEditUnderWrite::UpdateUnderWrite()
{
	if (IsWindowVisible()==TRUE)
	{
		//获取信息
		TCHAR szUnderWrite[LEN_UNDER_WRITE]=TEXT("");
		GetWindowText(szUnderWrite,CountArray(szUnderWrite));

		//用户信息
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//更新判断
		bool bActiveMission=false;
		if ((IsModifyStatus()==true)&&(lstrcmp(szUnderWrite,m_szUnderWrite)!=0L)) bActiveMission=true;
		if ((IsModifyStatus()==false)&&(lstrcmp(szUnderWrite,pGlobalUserData->szUnderWrite)!=0L)) bActiveMission=true;

		//更新判断
		if (bActiveMission==true)
		{
			//设置信息
			m_bNeedSend=true;
			lstrcpyn(m_szUnderWrite,szUnderWrite,CountArray(m_szUnderWrite));

			//终止任务
			CMissionManager * pMissionManager=GetMissionManager();
			if (GetActiveStatus()==true) pMissionManager->ConcludeMissionItem(this,false);

			//设置地址
			LPCTSTR pszCurrentServer=pMissionManager->GetCurrentServer();
			if ((pszCurrentServer!=NULL)&&(pszCurrentServer[0]!=0)) pMissionManager->SetCustomServer(pszCurrentServer);

			//激活任务
			pMissionManager->AvtiveMissionItem(this,false);

			//隐藏控件
			ShowWindow(SW_HIDE);

			//更新界面
			ASSERT(m_pParentWnd!=NULL);
			m_pParentWnd->RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
		}
		else
		{
			//关闭控件
			CloseUnderWrite();
		}
	}

	return;
}

//消耗消息
VOID CEditUnderWrite::OnDestroy()
{
	__super::OnDestroy();

	//终止任务
	if (GetActiveStatus()==true)
	{
		//变量定义
		CMissionManager * pMissionManager=GetMissionManager();
		if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

		//更新界面
		ASSERT(m_pParentWnd!=NULL);
		m_pParentWnd->RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//失去焦点
VOID CEditUnderWrite::OnKillFocus(CWnd * pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	//关闭控件
	UpdateUnderWrite();

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CWndUserInfoCtrl::CWndUserInfoCtrl()
{
	//6603状态标志
	m_bClickFace=false;
	m_bMouseEvent=false;

	//6603盘旋标志
	m_bHoverFace=false;

	//6603设置区域
	m_rcFaceArea.SetRect(18,3,78,63);
}

//析构函数
CWndUserInfoCtrl::~CWndUserInfoCtrl()
{
}

//消息过虑
BOOL CWndUserInfoCtrl::PreTranslateMessage(MSG * pMsg)
{
	//提示消息
	if (m_ToolTipCtrl.m_hWnd!=NULL)
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//命令函数
BOOL CWndUserInfoCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	UINT nCommandID=LOWORD(wParam);

	//功能按钮
	switch (nCommandID)
	{
	case IDC_USER_INFO_1:		//自定头像
		{
			//个人设置
			CDlgService DlgService;
			DlgService.DoModal();

			//显示窗口  //小三角图标修改点
		/*	CDlgCustomFace DlgCustomFace;
			DlgCustomFace.SetCustomFaceEvent(QUERY_OBJECT_PTR_INTERFACE(CPlatformFrame::GetInstance(),IUnknownEx));

			//显示窗口
			DlgCustomFace.DoModal();*/
						
			return TRUE;
		}
	case IDC_USER_INFO_2:		//锁定机器
		{
			//显示窗口
			/*CDlgLockMachine DlgLockMachine;
			DlgLockMachine.DoModal();*/
			//变量定义
			ASSERT(CParameterGlobal::GetInstance()!=NULL);
			CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

			//构造地址
			TCHAR szNavigation[256]=TEXT("");
			_sntprintf(szNavigation,CountArray(szNavigation),TEXT("%s/Member/ApplyPasswordCard.aspx"),szPlatformLink,5);

			CPlatformFrame * pPlatformFrame=CPlatformFrame::GetInstance();
			pPlatformFrame->WebBrowse(szNavigation,true);

			return TRUE;
		}
	case IDC_USER_INFO_3:		//魅力兑换
		{
			return TRUE;
			//构造地址
			TCHAR szLogonLink[256]=TEXT("");
			_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("LoveLiness.aspx"));

			//打开页面
			CDlgBrowser DlgBrowser;
			DlgBrowser.CreateBrowserControl(TEXT("魅力兑换"),szLogonLink,CSize(730,450));

			return TRUE;
		}
	case IDC_USER_INFO_4:      //查找好友
		{
			return TRUE;
			CPlatformFrame * pPlatformFrame=CPlatformFrame::GetInstance();
			pPlatformFrame->OnBnClickedPlatform1();
			return TRUE;
		}
	case IDC_USER_INFO_5:    //转账记录
		{
			ASSERT(CParameterGlobal::GetInstance()!=NULL);
			CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

			//构造地址
			TCHAR szNavigation[256]=TEXT("");
			_sntprintf(szNavigation,CountArray(szNavigation),TEXT("%s/Member/InsureRecord.aspx"),szPlatformLink,5);

			CPlatformFrame * pPlatformFrame=CPlatformFrame::GetInstance();
			pPlatformFrame->WebBrowse(szNavigation,true);
			return TRUE;
		}
	case IDC_USER_INFO_6:
		{
			CPlatformFrame * pPlatformFrame=CPlatformFrame::GetInstance();
			pPlatformFrame->OnBnClickedPlatform3();
			return TRUE;
		}
		break;
	case IDC_USER_INFO_SET:
		{
			//个人设置
			CDlgService DlgService;
			DlgService.DoModal();

			return TRUE;
		}
		break;
	case IDC_SYSTEM_SET:		//个人设置
		{
#ifdef VERSION_69
			//个人设置
			CDlgService DlgService;
			DlgService.DoModal();
#else
			CPlatformFrame * pPlatformFrame=CPlatformFrame::GetInstance();
			pPlatformFrame->OnBnClickedPlatform3();
#endif
			return TRUE;
		}
	case IDC_USER_INFO_COPY:   //复制个人信息
		{
			CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();

			CString str, str1;
			str.Format(_T("昵称："));
			str += pGlobalUserInfo->GetGlobalUserData()->szNickName;

			str1.Format(_T(" ID号：%ld"), pGlobalUserInfo->GetGlobalUserData()->dwGameID);
			CWHService::SetClipboardString(str + str1);

			CInformation Information(this);
			Information.ShowMessageBox(TEXT("系统提示"), TEXT("已经成功复制用户昵称和ID号"),MB_OK,10);
			return TRUE;
		}
	default:
		break;
	}

	return __super::OnCommand(wParam,lParam);
}

//位置消息
VOID CWndUserInfoCtrl::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);

	return;
}

//建立消息
INT CWndUserInfoCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//注册事件
	CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
	if (pPlatformEvent!=NULL) pPlatformEvent->RegisterEventWnd(m_hWnd);

	//加载资源
	CPngImage ImageBkgnd;
	ImageBkgnd.LoadImage(AfxGetInstanceHandle(),TEXT("USER_INFO_BACK"));

	//创建按钮
	CRect rcCreate(0,0,0,0);
	m_btUserInfoSet.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_USER_INFO_SET);
	m_btUserInfoCopy.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_USER_INFO_COPY);
	m_btSystemSet.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SYSTEM_SET);

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btUserInfoSet.SetButtonImage(IDB_BK_USER_INFO_SET,TEXT("BT_USER_INFO_SET"),hInstance,true,false);
	m_btUserInfoCopy.SetButtonImage(IDB_BK_USER_INFO,TEXT("BT_USER_INFO_COPY"),hInstance,true,false);
	m_btSystemSet.SetButtonImage(IDB_BK_USER_INFO,TEXT("BT_SYSTEM_SET"),hInstance,true,false);

	//移动窗口
	CRect rcUserInfo;
	GetWindowRect(&rcUserInfo);
	SetWindowPos(NULL,0,0,ImageBkgnd.GetWidth(),ImageBkgnd.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//提示控件
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_btSystemSet,TEXT("系统设置"));
	m_ToolTipCtrl.AddTool(&m_btUserInfoCopy,TEXT("复制个人信息"));
	m_ToolTipCtrl.AddTool(&m_btUserInfoSet,TEXT("个人设置"));

	return 0;
}

VOID CWndUserInfoCtrl::OnLButtonUp(UINT nFlags, CPoint MousePoint)
{
	__super::OnLButtonUp(nFlags,MousePoint);

	if ((m_bHoverFace==true)&&(m_bClickFace==true))
	{
		//6603释放捕获
		ReleaseCapture();

		//6603设置变量
		m_bClickFace=false;

		//6603个人信息
		if (m_rcFaceArea.PtInRect(MousePoint)==TRUE)
		{
			CDlgService DlgService;
			DlgService.DoModal();
		}
	}
}
//6603鼠标消息
VOID CWndUserInfoCtrl::OnLButtonDown(UINT nFlags, CPoint MousePoint)
{
	__super::OnLButtonDown(nFlags,MousePoint);

	//6603设置焦点
	SetFocus();

	//6603动作处理
	if ((m_bHoverFace==true)&&(m_bClickFace==false))
	{
		//6603鼠标扑获
		SetCapture();

		//6603设置变量
		m_bClickFace=true;
	}
}
//6603光标消息
BOOL CWndUserInfoCtrl::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//6603获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//6603变量定义
	bool bMouseEvent=false;
	bool bRedrawWindow=false;

	//6603盘旋判断
	if ((m_bHoverFace==false)&&(m_rcFaceArea.PtInRect(MousePoint)==TRUE))
	{
		//6603设置变量
		bMouseEvent=true;
		bRedrawWindow=true;

		//6603设置变量
		m_bHoverFace=true;
	}

	//6603离开判断
	if ((m_bHoverFace==true)&&(m_rcFaceArea.PtInRect(MousePoint)==FALSE))
	{
		//6603设置变量
		bRedrawWindow=true;

		//6603设置变量
		m_bHoverFace=false;
	}

	//6603注册事件
	if ((m_bMouseEvent==false)&&(bMouseEvent==true))
	{
		//6603设置变量
		m_bMouseEvent=true;

		//6603变量定义
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//6603注册消息
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//6603注册事件
		_TrackMouseEvent(&TrackMouseEvent);
	}

	//6603更新界面
	if (bRedrawWindow==true)
	{
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	//6603设置光标
	if (m_bHoverFace==true)
	{
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
		return true;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//调整控件
VOID CWndUserInfoCtrl::RectifyControl(INT nWidth, INT nHeight)
{
	//变量定义
	HDWP hDwp=BeginDeferWindowPos(64);
	UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER|SWP_NOSIZE;

	CRect rcClient;
	GetClientRect(&rcClient);

	//调整按钮
	DeferWindowPos(hDwp,m_btSystemSet,NULL,rcClient.Width()-8-22,22,0,0,uFlags);
	DeferWindowPos(hDwp,m_btUserInfoCopy,NULL,rcClient.Width()-8-22,42,0,0,uFlags);
	DeferWindowPos(hDwp,m_btUserInfoSet,NULL,80,2,0,0,uFlags);

	//结束调整
	EndDeferWindowPos(hDwp);

	return;
}

//绘画背景
BOOL CWndUserInfoCtrl::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CImage ImageBuffer;
	if(ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32) == FALSE) return TRUE;

	//创建 DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(BufferDC);

	//变量定义
//	HINSTANCE hInstance=AfxGetInstanceHandle();
//	CSkinRenderManager * pSkinRenderManager=CSkinRenderManager::GetInstance();

	//绘画背景
	CPngImage ImageBkgnd;
	ImageBkgnd.LoadImage(AfxGetInstanceHandle(),TEXT("USER_INFO_BACK"));

	ImageBkgnd.DrawImage(pBufferDC, 0, 0);

	//用户信息
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//绘画头像
	if (pGlobalUserData->CustomFaceInfo.dwDataSize!=0)
	{
		DWORD * pdwCustomFace=pGlobalUserData->CustomFaceInfo.dwCustomFace;
		theApp.m_FaceItemControlModule->DrawFaceNormal(pBufferDC,18,3,60,60,pdwCustomFace);
	}
	else
	{
		//69根据性别绘制头像
		theApp.m_FaceItemControlModule->DrawFaceNormal(pBufferDC,18,3,60,60,pGlobalUserData->wFaceID);
	}

	//绘画标题
	pBufferDC->SetTextColor(RGB(21,72,124));

//	LOGI("CWndUserInfoCtrl::OnEraseBkgnd, szNickName"<<pGlobalUserData->szNickName);

	//输出帐号
	CRect rcNickName;
	rcNickName.SetRect(86,22,rcClient.Width()-15,40);
	CString str;
	str.Format(_T("昵称："));
	str += pGlobalUserData->szNickName;
	pBufferDC->DrawText(str, &rcNickName,DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	CRect rcGameID;
	rcGameID.SetRect(86,42,rcClient.Width()-15,60);
	str.Format(_T("ID号：%ld"), pGlobalUserData->dwGameID);
	pBufferDC->DrawText(str,&rcGameID,DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);


	// 不显示游戏币
// 	CRect rcScore;
// 	rcScore.SetRect(86,44,rcClient.Width()-15,62);
// 	CString str;
// 	str.Format(TEXT("游戏币：%ld"), pGlobalUserData->lUserScore);
// 	pBufferDC->DrawText(str, &rcScore, DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	return TRUE;
}

//6603鼠标消息
LRESULT CWndUserInfoCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	//6603设置变量
	m_bMouseEvent=false;

	//6603离开处理
	if (m_bHoverFace==true)
	{
		//6603设置变量
		m_bMouseEvent=false;

		//6603更新界面
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return 0;
}

//事件消息
LRESULT CWndUserInfoCtrl::OnMessagePlatformEvent(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case EVENT_USER_LOGON:			//登录成功
		{
			//更新窗口
			Invalidate();

			//设置按钮
			m_btUserInfoSet.EnableWindow(TRUE);
			m_btSystemSet.EnableWindow(TRUE);
			m_btUserInfoCopy.EnableWindow(TRUE);


			return 0L;
		}
	case EVENT_USER_LOGOUT:			//注销成功
		{
			//设置按钮
			m_btUserInfoSet.EnableWindow(FALSE);
			m_btSystemSet.EnableWindow(FALSE);
			m_btUserInfoCopy.EnableWindow(FALSE);

			return 0L;
		}
	case EVENT_USER_INFO_UPDATE:	//资料更新
		{
//			LOGI("更新窗体");
			//更新窗口
			Invalidate();

			return 0L;
		}
		break;
	case EVENT_USER_MOOR_MACHINE:	//绑定机器
		{
			//用户信息
// 			CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
// 			tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
// 
// 			//设置按钮
// 			if (pGlobalUserData->cbMoorMachine==FALSE)
// 			{
// 				HINSTANCE hInstance=AfxGetInstanceHandle();
// 				m_btUserInfo2.SetButtonImage(IDB_BK_USER_INFO,TEXT("BT_USER_INFO_2"),hInstance,true,false);
// 				m_ToolTipCtrl.AddTool(&m_btUserInfo2,TEXT("锁定本机"));
// 			}
// 			else
// 			{
// 				HINSTANCE hInstance=AfxGetInstanceHandle();
// 				m_btUserInfo2.SetButtonImage(IDB_BK_USER_INFO,TEXT("BT_USER_INFO_4"),hInstance,true,false);
// 				m_ToolTipCtrl.AddTool(&m_btUserInfo2,TEXT("解锁本机"));
// 			}

			return 0L;
		}
	}

	return 0L;
}

//////////////////////////////////////////////////////////////////////////////////
