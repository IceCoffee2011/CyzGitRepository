#include "StdAfx.h"
#include "DlgService.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////////////
//宏定义

//尺寸定义
#define ITEM_IMAGE_CX				20									//图标宽度
#define ITEM_IMAGE_CY				20									//图标高度 

//尺寸定义
#define SERVICE_BAR_CX				150									//控制宽度
#define SERVICE_BAR_CY				375									//控制高度 

//尺寸定义
#define BAR_BT_SPACE				3									//按钮间距
#define BAR_BT_EXCURSION			5									//按钮偏移

//控件标识
#define IDC_SERVICE_BAR				100									//子项控制

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServiceBar, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgService, CSkinDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONTROL, OnTcnSelchangeTabControl)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////



//构造函数
CDlgServiceBar::CDlgServiceBar()
{
	//活动信息
	m_bHovering=false;
	m_wItemDown=INVALID_WORD;
	m_wItemHover=INVALID_WORD;
	m_wItemActive=INVALID_WORD;

	//子项信息
	m_SizeButton.SetSize(0,0);
	m_rcItemCreate.SetRect(0,0,0,0);

	//加载资源
	CPngImage ImageButton;
	ImageButton.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_BAR_BT"));
	m_SizeButton.SetSize(ImageButton.GetWidth()/3,ImageButton.GetHeight());

	return;
}

//析构函数
CDlgServiceBar::~CDlgServiceBar()
{
}

//消息解释
BOOL CDlgServiceBar::PreTranslateMessage(MSG * pMsg)
{
	//消息过虑
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//设置区域
bool CDlgServiceBar::SetItemCreateRect(CRect rcItemCreate)
{
	//设置变量
	m_rcItemCreate=rcItemCreate;

	//调整控件
	for (INT_PTR i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		CWnd * pWnd=m_DlgServiceItemArray[i];
		if (pWnd->m_hWnd!=NULL) pWnd->SetWindowPos(NULL,rcItemCreate.left,rcItemCreate.top,rcItemCreate.Width(),rcItemCreate.Height(),SWP_NOZORDER);
	}

	return true;
}

//插入子项
bool CDlgServiceBar::InsertServiceItem(CDlgServiceItem * pDlgServiceItem)
{
	//设置变量
	m_DlgServiceItemArray.Add(pDlgServiceItem);

	//更新界面
	if (m_hWnd!=NULL)
	{
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return true;
}

//激活设置
bool CDlgServiceBar::SetActiveServiceItem(CDlgServiceItem * pDlgServiceItem)
{
	//效验参数
	ASSERT(pDlgServiceItem!=NULL);
	if (pDlgServiceItem==NULL) return false;

	//变量定义
	CDlgServiceItem * pServiceActive=NULL;

	//切换判断
	if (m_wItemActive!=INVALID_WORD)
	{
		//获取子项
		ASSERT(m_wItemActive<m_DlgServiceItemArray.GetCount());
		pServiceActive=m_DlgServiceItemArray[m_wItemActive];

		//切换判断
		if (pServiceActive==pDlgServiceItem) return true;
	}

	//激活子项
	for (INT_PTR i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		if (m_DlgServiceItemArray[i]==pDlgServiceItem)
		{
			//创建子项
			if (pDlgServiceItem->m_hWnd==NULL)
			{
				pDlgServiceItem->CreateServiceItem(GetParent(),m_rcItemCreate);
			}

			//设置变量
			m_wItemActive=(WORD)i;

			//显示窗口
			pDlgServiceItem->ShowWindow(SW_SHOW);
			pDlgServiceItem->SetFocus();

			//隐藏旧项
			if (pServiceActive->GetSafeHwnd()!=NULL)
			{
				pServiceActive->ShowWindow(SW_HIDE);
			}

			//更新界面
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);

			return true;
		}
	}

	return false;
}

//索引切换
WORD CDlgServiceBar::SwitchToButtonIndex(CPoint MousePoint)
{
	//窗口位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//边界判断
	if (MousePoint.x<((rcClient.Width()-m_SizeButton.cx)/2)) return INVALID_WORD;
	if (MousePoint.x>((rcClient.Width()+m_SizeButton.cx)/2)) return INVALID_WORD;

	//子项判断
	for (WORD i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		//位置计算
		CRect rcButton;
		rcButton.left=(rcClient.Width()-m_SizeButton.cx)/2;
		rcButton.right=(rcClient.Width()+m_SizeButton.cx)/2;
		rcButton.top=(m_SizeButton.cy+BAR_BT_SPACE)*i+BAR_BT_EXCURSION;
		rcButton.bottom=(m_SizeButton.cy+BAR_BT_SPACE)*i+m_SizeButton.cy+BAR_BT_EXCURSION;

		//区域判断
		if (rcButton.PtInRect(MousePoint)) return i;
	}

	return INVALID_WORD;
}

//重画消息
VOID CDlgServiceBar::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
/*	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CBitImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//创建设备
	CImageDC BufferDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(BufferDC);

	//设置设备
	pBufferDC->SetBkMode(TRANSPARENT);
	pBufferDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//加载资源
	CPngImage ImageButton;
	CPngImage ImageBarLogo;
	CPngImage ImageBarImage;
	ImageButton.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_BAR_BT"));
	ImageBarLogo.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_BAR_LOGO"));
	ImageBarImage.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_BAR_IMAGE"));

	//绘画背景
	pBufferDC->FillSolidRect(&rcClient,RGB(223,236,246));
	ImageBarLogo.DrawImage(pBufferDC,rcClient.Width()-ImageBarLogo.GetWidth(),rcClient.Height()-ImageBarLogo.GetHeight());

	//绘画按钮
	for (INT i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		//获取标题
		CDlgServiceItem * pDlgServiceItem=m_DlgServiceItemArray[i];
		LPCTSTR pszServiceName=pDlgServiceItem->GetServiceItemName();

		//横行计算
		INT nXButtomPos=0;
		if ((m_wItemHover==i)&&(m_wItemDown!=i)) nXButtomPos=m_SizeButton.cx;
		if ((m_wItemActive==i)||(m_wItemDown==i)) nXButtomPos=m_SizeButton.cx*2;

		//绘画按钮
		INT nXDrawPos=(rcClient.Width()-m_SizeButton.cx)/2;
		INT nYDrawPos=(m_SizeButton.cy+BAR_BT_SPACE)*i+BAR_BT_EXCURSION;
		ImageButton.DrawImage(pBufferDC,nXDrawPos,nYDrawPos,m_SizeButton.cx,m_SizeButton.cy,nXButtomPos,0L);

		//绘画图标
		INT nXImagePos=pDlgServiceItem->m_cbImageIndex*ITEM_IMAGE_CX;
		ImageBarImage.DrawImage(pBufferDC,nXDrawPos+32,nYDrawPos+2,ITEM_IMAGE_CX,ITEM_IMAGE_CY,nXImagePos,0);

		//设置颜色
		if ((m_wItemHover!=i)&&(m_wItemDown!=i)) pBufferDC->SetTextColor(RGB(0,0,0));
		if ((m_wItemHover==i)&&(m_wItemDown!=i)) pBufferDC->SetTextColor(RGB(3,112,218));
		if ((m_wItemActive==i)||(m_wItemDown==i)) pBufferDC->SetTextColor(RGB(255,255,255));

		//绘画字符
		CRect rcItem(nXDrawPos+25,nYDrawPos+1,m_SizeButton.cx+nXDrawPos,m_SizeButton.cy+nYDrawPos);
		pBufferDC->DrawText(pszServiceName,lstrlen(pszServiceName),rcItem,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);*/

	return;
}

//建立消息
INT CDlgServiceBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置变量
	m_bHovering=false;
	m_wItemDown=INVALID_WORD;
	m_wItemHover=INVALID_WORD;
	m_wItemActive=INVALID_WORD;

	//子项信息
	m_rcItemCreate.SetRect(0,0,0,0);
	m_DlgServiceItemArray.RemoveAll();

	return 0;
}

//鼠标消息
VOID CDlgServiceBar::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	//窗口位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//位置计算
	WORD wServiceHover=SwitchToButtonIndex(Point);

	//盘旋变量
	if (wServiceHover!=m_wItemHover)
	{
		//设置变量
		m_wItemHover=wServiceHover;

		//更新界面
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	//进入判断
	if (m_bHovering==false)
	{
		//设置变量
		m_bHovering=true;

		//变量定义
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//注册消息
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//注册事件
		_TrackMouseEvent(&TrackMouseEvent);
	}

	return;
}

//鼠标消息
VOID CDlgServiceBar::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags,Point);

	//激活子项
	if ((m_wItemHover!=INVALID_WORD)&&(m_wItemHover==m_wItemDown))
	{
		SetActiveServiceItem(m_DlgServiceItemArray[m_wItemHover]);
	}

	//按起处理
	if (m_wItemDown!=INVALID_WORD)
	{
		//释放鼠标
		ReleaseCapture();

		//设置变量
		m_wItemDown=INVALID_WORD;

		//更新界面
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//鼠标消息
VOID CDlgServiceBar::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//设置焦点
	SetFocus();

	//按下处理
	if ((m_wItemHover!=INVALID_WORD)&&(m_wItemHover!=m_wItemActive))
	{
		//鼠标扑获
		SetCapture();

		//设置变量
		m_wItemDown=m_wItemHover;

		//更新界面
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//鼠标消息
LRESULT CDlgServiceBar::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_bHovering==true)
	{
		//设置变量
		m_bHovering=false;
		m_wItemHover=INVALID_WORD;

		//更新界面
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return 0;
}

//光标消息
BOOL CDlgServiceBar::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//设置光标
	if (m_wItemHover!=INVALID_WORD)
	{
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
		return TRUE;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//////////////////////////////////////////////////////////////////////////////////

//控件绑定
VOID CDlgService::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//控件变量
	DDX_Control(pDX, IDC_TAB_CONTROL, m_TabControl);
}


//构造函数
CDlgService::CDlgService(BYTE cbPageID /* = SERVICE_INDIVIDUAL */ ) : CSkinDialog(IDD_DLG_SERVICE)
{
	//设置变量
	m_cbServiceID=0;

	m_SkinAttribute.m_crBackGround=RGB(255,255,255);

	m_cbInitPageID = cbPageID;

	return;
}

//析构函数
CDlgService::~CDlgService()
{
}

//消息解释
BOOL CDlgService::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//创建函数
BOOL CDlgService::OnInitDialog()
{
	__super::OnInitDialog();

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//设置标题
	SetWindowText(TEXT("用户服务"));

	//变量定义
// 	INT nTViewBorder=m_SkinAttribute.m_EncircleInfoView.nTBorder;
//  	INT nBViewBorder=m_SkinAttribute.m_EncircleInfoView.nBBorder;
//  	INT nLViewBorder=m_SkinAttribute.m_EncircleInfoView.nLBorder;
//  	INT nRViewBorder=m_SkinAttribute.m_EncircleInfoView.nRBorder;
//  	INT nBFrameBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;
// 	
// 	//创建控制
// 	m_DlgServiceBar.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,IDC_SERVICE_BAR);
// 	m_DlgServiceBar.SetWindowPos(NULL,nLViewBorder,nTViewBorder,SERVICE_BAR_CX,SERVICE_BAR_CY,SWP_NOZORDER);
// 
// 	//子项位置
// 	CRect rcItemCreate;
// 	rcItemCreate.top=nTViewBorder;
// 	rcItemCreate.left=nLViewBorder+SERVICE_BAR_CX;
// 	rcItemCreate.right=rcClient.Width()-nRViewBorder;
// 	rcItemCreate.bottom=nTViewBorder+SERVICE_BAR_CY;
// 	m_DlgServiceBar.SetItemCreateRect(rcItemCreate);
// 
// 	//程序选项
// //	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceIndividual);
// //	m_DlgServiceBar.InsertServiceItem(&m_DlgServicePassword);
// 
// 	//网页选项
// 	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceBrowser1);
// 	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceBrowser2);
// 	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceBrowser3);
// 	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceBrowser4);
// 	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceBrowser5);
// 	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceBrowser6);
// 
// 	//设置地址
// 	m_DlgServiceBrowser1.InitService(5,TEXT("会员充值"),TEXT("UserService/UserService1.aspx"));
// 	m_DlgServiceBrowser2.InitService(6,TEXT("游戏记录"),TEXT("UserService/UserService2.aspx"));
// 	m_DlgServiceBrowser3.InitService(7,TEXT("充值记录"),TEXT("UserService/UserService3.aspx"));
// 	m_DlgServiceBrowser4.InitService(8,TEXT("负分清理"),TEXT("UserService/UserService4.aspx"));
// 	m_DlgServiceBrowser5.InitService(9,TEXT("清逃跑率"),TEXT("UserService/UserService5.aspx"));
// 	m_DlgServiceBrowser6.InitService(10,TEXT("游戏会员"),TEXT("UserService/UserService6.aspx"));
// 
// 	//移动窗口
// 	rcClient.bottom=nTViewBorder+nBFrameBorder+SERVICE_BAR_CY;
// 	SetWindowPos(NULL,0,0,rcClient.Width(),rcClient.Height(),SWP_NOZORDER|SWP_NOMOVE);
// 
// 	//激活选项
// 	CDlgServiceItem * pDlgServiceItem=GetServiceItem(m_cbServiceID);
// 	if (pDlgServiceItem==NULL) pDlgServiceItem=&m_DlgServiceIndividual;
// 
// 	//激活选项
// 	m_DlgServiceBar.SetActiveServiceItem(pDlgServiceItem);


	//移动窗口
	//SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

	//SetWindowPos(NULL, 100, 100, 472, 328 , SWP_NOZORDER|SWP_SHOWWINDOW);


	INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
	INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
	INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
	INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;

	//设置控件
	m_TabControl.MoveWindow( nLBorder-4, 36, rcClient.Width() - nLBorder - nRBorder + 8, rcClient.Height()- nTBorder - nTBorder + 36); 
	m_TabControl.SetItemSize(CSize(rcClient.Width() - nLBorder - nRBorder - 3, 25));

	m_TabControl.InsertItem(SERVICE_INDIVIDUAL,TEXT(" 个人信息 "));
	m_TabControl.InsertItem(SERVICE_LOCK_MACHINE,TEXT(" 绑定机器 "));
	m_TabControl.InsertItem(SERVICE_PASSWORD,TEXT(" 修改登录密码 "));
	m_TabControl.InsertItem(SERVICE_BIND_MOBILE,TEXT(" 绑定手机 "));

	//创建窗口
	m_DlgServiceIndividual.Create(MAKEINTRESOURCE(IDD_DLG_SERVICE_INDIVIDUAL), &m_TabControl);
//	m_DlgServiceIndividual.MoveWindow(CRect(CPoint(4, 30), CSize(440,250)));

	m_DlgServiceLockMachine.Create(MAKEINTRESOURCE(IDD_DLG_SERVICE_LOCK_MACHINE), &m_TabControl);
//	m_DlgServiceLockMachine.MoveWindow(CRect(CPoint(4, 30), CSize(440,250)));
	
	m_DlgServicePassword.Create(MAKEINTRESOURCE(IDD_DLG_SERVICE_PASSWORD), &m_TabControl);
//	m_DlgServicePassword.MoveWindow(CRect(CPoint(4, 30), CSize(440,250)));

	m_DlgServiceBindMobile.Create(MAKEINTRESOURCE(IDD_DLG_SERVICE_BIND_MOBILE), &m_TabControl);
//	m_DlgServiceBindMobile.MoveWindow(CRect(CPoint(4, 30), CSize(440,250)));

	m_DlgServiceItemArray.Add(&m_DlgServiceIndividual);
	m_DlgServiceItemArray.Add(&m_DlgServiceLockMachine);
	m_DlgServiceItemArray.Add(&m_DlgServicePassword);
	m_DlgServiceItemArray.Add(&m_DlgServiceBindMobile);

	CRect rcTabControl;
	m_TabControl.GetClientRect(rcTabControl);
// 	ScreenToClient(&rcTabControl);
// 	LOGI("CDlgService::OnInitDialog rcTabControl, left="<<rcTabControl.left<<", top="<<rcTabControl.top<<
// 		", width="<<rcTabControl.Width()<<", height="<<rcTabControl.Height());
	// 调整窗口区域
	for (INT_PTR i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		CWnd * pWnd=m_DlgServiceItemArray[i];
		if (pWnd->m_hWnd!=NULL)
		{
//			LOGI("m_DlgServiceItemArray.MoveWindow(), width="<<rcTabControl.Width()-2*3<<", height="<<rcTabControl.Height()-25-2*3);
			pWnd->MoveWindow( CRect( CPoint(3,25+3), CSize(rcTabControl.Width()-2*3,rcTabControl.Height()-25-2*3) ) );
		}
	}

	// 设置选中
//	LOGI("CDlgService::OnInitDialog， cbPageID="<<m_cbInitPageID);
	m_TabControl.SetCurSel(m_cbInitPageID);
	SwitchPage(m_cbInitPageID);
 
	return TRUE;
}

//确定消息
VOID CDlgService::OnOK()
{
	return;
}

// 窗口大小调整
VOID CDlgService::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);
}

//期望标识
VOID CDlgService::SetExpectationID(BYTE cbServiceID)
{
	//设置变量
	m_cbServiceID=cbServiceID;

	return;
}

//获取子项
CDlgServiceItem * CDlgService::GetServiceItem(BYTE cbServiceID)
{
	//子项设置
/*	switch (cbServiceID)
	{
	case SERVICE_ACCOUNTS:			//基本资料
		{
			return &m_ServiceModifyAccouts;
		}
	case SERVICE_INDIVIDUAL:		//更多资料
		{
			return &m_ServiceModifyIndividual;
		}
	case SERVICE_PASSWORD:			//修改密码
		{
			return &m_ServiceModifyPassword;
		}
	case SERVICE_PASSPROTECT:		//密码保护
		{
			return &m_ServiceModifyProtect;
		}
	case SERVICE_SECURITY:			//安全设置
		{
			return &m_ServiceModifySecurity;
		}
	case SERVICE_MEMBER_PAYMENT:	//会员充值
		{
			return &m_ServiceMemberPayment;
		}
	case SERVICE_BANK_ACCESS:		//银行存取
		{
			return &m_ServiceBankAccess;
		}
// 	case SERVICE_BANK_TRANSFER:		//银行转帐
// 		{
// 			return &m_ServiceBankTransfer;
// 		}
	case SERVICE_GAME_RECORD:		//游戏记录
		{
			return &m_ServiceGameRecord;
		}
	case SERVICE_PAY_RECORD:		//充值记录
		{
			return &m_ServicePayRecord;
		}
	case SERVICE_CLEAN_SCORE:		//负分清零
		{
			return &m_ServiceCleanScore;
		}
	case SERVICE_CLEAN_FLEE:		//清逃跑率
		{
			return &m_ServiceCleanFlee;
		}
	case SERVICE_GAME_MEMBER:		//游戏会员
		{
			return &m_ServiceGameMember;
		}
	}*/

	return NULL;
}

// 切换显示页
VOID CDlgService::SwitchPage(BYTE cbPageID)
{
	if (cbPageID < 0 || cbPageID >= m_DlgServiceItemArray.GetCount())
	{
		return;
	}

	CDlgServiceItem* pDlgServiceItem=m_DlgServiceItemArray[cbPageID];
	CDlgServiceItem* pServiceItemActive=NULL;
	if ( (m_cbInitPageID>=0&&m_cbInitPageID<m_DlgServiceItemArray.GetCount()) && cbPageID != m_cbInitPageID)
	{
		pServiceItemActive = m_DlgServiceItemArray[m_cbInitPageID];
	}
	for (INT_PTR i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		if (m_DlgServiceItemArray[i]==pDlgServiceItem)
		{
			//6603创建子项

			//6603设置变量
//			m_wItemActive=(WORD)i;
			m_cbInitPageID=(BYTE)i;

			//6603显示窗口
			pDlgServiceItem->ShowWindow(SW_SHOW);
			pDlgServiceItem->SetFocus();
			pDlgServiceItem->SetActiveWindow();

			//6603隐藏旧项
			if (pServiceItemActive->GetSafeHwnd()!=NULL)
			{
				pServiceItemActive->ShowWindow(SW_HIDE);
			}

			//6603更新界面
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
			return;
		}
	}
// 	switch(cbPageID)
// 	{
// 	case SERVICE_INDIVIDUAL:		// 个人资料
// 		{
// 			m_DlgServiceIndividual.ShowWindow(SW_SHOW);
// 			m_DlgServiceLockMachine.ShowWindow(SW_HIDE);
// 			m_DlgServicePassword.ShowWindow(SW_HIDE);
// 	
// 			m_DlgServiceIndividual.SetFocus();
// 			m_DlgServiceIndividual.SetActiveWindow();
// 		}
// 		break;
// 	case SERVICE_LOCK_MACHINE:		// 锁定机器
// 		{
// 			m_DlgServiceLockMachine.ShowWindow(SW_SHOW);
// 			m_DlgServiceIndividual.ShowWindow(SW_HIDE);
// 			m_DlgServicePassword.ShowWindow(SW_HIDE);
// 	
// 			m_DlgServiceLockMachine.SetFocus();
// 			m_DlgServiceLockMachine.SetActiveWindow();
// 		}
// 		break;
// 	case SERVICE_PASSWORD:			// 更改登录密码
// 		{
// 			m_DlgServicePassword.ShowWindow(SW_SHOW);
// 			m_DlgServiceIndividual.ShowWindow(SW_HIDE);
// 			m_DlgServiceLockMachine.ShowWindow(SW_HIDE);
// 	
// 			m_DlgServicePassword.SetFocus();
// 			m_DlgServicePassword.SetActiveWindow();
// 		}
// 		break;
// 	case SERVICE_BIND_MOBILE:
// 		{
// 			m_DlgServicePassword.ShowWindow(SW_HIDE);
// 			m_DlgServiceIndividual.ShowWindow(SW_HIDE);
// 			m_DlgServiceLockMachine.ShowWindow(SW_HIDE);
// 			m_DlgServiceBindMobile.ShowWindow(SW_SHOW);
// 
// 			m_DlgServiceBindMobile.SetFocus();
// 			m_DlgServiceBindMobile.SetActiveWindow();
// 		}
// 		break;
// 	}
}

//////////////////////////////////////////////////////////////////////////

void CDlgService::OnTcnSelchangeTabControl(NMHDR *pNMHDR, LRESULT *pResult)
{
// 	switch (m_TabControl.GetCurSel())
// 	{
// 	case SERVICE_INDIVIDUAL:		// 个人资料
// 		{
// 			//显示控件
// //			m_FaceItemCustomWnd.ShowWindow(SW_SHOW);
// //			m_FaceItemSelectWnd.ShowWindow(SW_HIDE);
// 
// 			//设置焦点
// //			m_FaceItemCustomWnd.SetFocus();
// //			m_FaceItemCustomWnd.SetActiveWindow();
// 
// 			SwitchPage(SERVICE_INDIVIDUAL);
// 
// 			break;
// 		}
// 	case SERVICE_LOCK_MACHINE:		//系统头像
// 		{
// 			//显示控件
// //			m_FaceItemCustomWnd.ShowWindow(SW_HIDE);
// //			m_FaceItemSelectWnd.ShowWindow(SW_SHOW);
// 
// 			//设置焦点
// //			m_FaceItemSelectWnd.SetFocus();
// //			m_FaceItemSelectWnd.SetActiveWindow();
// 			
// 
// 			SwitchPage(SERVICE_LOCK_MACHINE);
// 
// 			break;
// 		}
// 	case SERVICE_PASSWORD:
// 		{
// 
// 			SwitchPage(SERVICE_PASSWORD);
// 		}
// 		break;
// 	}
	SwitchPage(m_TabControl.GetCurSel());
	*pResult = 0;
}

//绘画消息
VOID CDlgService::OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight)
{
	// 绘制中间区域为白色

}
