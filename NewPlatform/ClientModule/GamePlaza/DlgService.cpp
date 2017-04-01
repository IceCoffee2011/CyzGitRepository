#include "StdAfx.h"
#include "DlgService.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////////////
//�궨��

//�ߴ綨��
#define ITEM_IMAGE_CX				20									//ͼ����
#define ITEM_IMAGE_CY				20									//ͼ��߶� 

//�ߴ綨��
#define SERVICE_BAR_CX				150									//���ƿ��
#define SERVICE_BAR_CY				375									//���Ƹ߶� 

//�ߴ綨��
#define BAR_BT_SPACE				3									//��ť���
#define BAR_BT_EXCURSION			5									//��ťƫ��

//�ؼ���ʶ
#define IDC_SERVICE_BAR				100									//�������

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



//���캯��
CDlgServiceBar::CDlgServiceBar()
{
	//���Ϣ
	m_bHovering=false;
	m_wItemDown=INVALID_WORD;
	m_wItemHover=INVALID_WORD;
	m_wItemActive=INVALID_WORD;

	//������Ϣ
	m_SizeButton.SetSize(0,0);
	m_rcItemCreate.SetRect(0,0,0,0);

	//������Դ
	CPngImage ImageButton;
	ImageButton.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_BAR_BT"));
	m_SizeButton.SetSize(ImageButton.GetWidth()/3,ImageButton.GetHeight());

	return;
}

//��������
CDlgServiceBar::~CDlgServiceBar()
{
}

//��Ϣ����
BOOL CDlgServiceBar::PreTranslateMessage(MSG * pMsg)
{
	//��Ϣ����
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//��������
bool CDlgServiceBar::SetItemCreateRect(CRect rcItemCreate)
{
	//���ñ���
	m_rcItemCreate=rcItemCreate;

	//�����ؼ�
	for (INT_PTR i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		CWnd * pWnd=m_DlgServiceItemArray[i];
		if (pWnd->m_hWnd!=NULL) pWnd->SetWindowPos(NULL,rcItemCreate.left,rcItemCreate.top,rcItemCreate.Width(),rcItemCreate.Height(),SWP_NOZORDER);
	}

	return true;
}

//��������
bool CDlgServiceBar::InsertServiceItem(CDlgServiceItem * pDlgServiceItem)
{
	//���ñ���
	m_DlgServiceItemArray.Add(pDlgServiceItem);

	//���½���
	if (m_hWnd!=NULL)
	{
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return true;
}

//��������
bool CDlgServiceBar::SetActiveServiceItem(CDlgServiceItem * pDlgServiceItem)
{
	//Ч�����
	ASSERT(pDlgServiceItem!=NULL);
	if (pDlgServiceItem==NULL) return false;

	//��������
	CDlgServiceItem * pServiceActive=NULL;

	//�л��ж�
	if (m_wItemActive!=INVALID_WORD)
	{
		//��ȡ����
		ASSERT(m_wItemActive<m_DlgServiceItemArray.GetCount());
		pServiceActive=m_DlgServiceItemArray[m_wItemActive];

		//�л��ж�
		if (pServiceActive==pDlgServiceItem) return true;
	}

	//��������
	for (INT_PTR i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		if (m_DlgServiceItemArray[i]==pDlgServiceItem)
		{
			//��������
			if (pDlgServiceItem->m_hWnd==NULL)
			{
				pDlgServiceItem->CreateServiceItem(GetParent(),m_rcItemCreate);
			}

			//���ñ���
			m_wItemActive=(WORD)i;

			//��ʾ����
			pDlgServiceItem->ShowWindow(SW_SHOW);
			pDlgServiceItem->SetFocus();

			//���ؾ���
			if (pServiceActive->GetSafeHwnd()!=NULL)
			{
				pServiceActive->ShowWindow(SW_HIDE);
			}

			//���½���
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);

			return true;
		}
	}

	return false;
}

//�����л�
WORD CDlgServiceBar::SwitchToButtonIndex(CPoint MousePoint)
{
	//����λ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//�߽��ж�
	if (MousePoint.x<((rcClient.Width()-m_SizeButton.cx)/2)) return INVALID_WORD;
	if (MousePoint.x>((rcClient.Width()+m_SizeButton.cx)/2)) return INVALID_WORD;

	//�����ж�
	for (WORD i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		//λ�ü���
		CRect rcButton;
		rcButton.left=(rcClient.Width()-m_SizeButton.cx)/2;
		rcButton.right=(rcClient.Width()+m_SizeButton.cx)/2;
		rcButton.top=(m_SizeButton.cy+BAR_BT_SPACE)*i+BAR_BT_EXCURSION;
		rcButton.bottom=(m_SizeButton.cy+BAR_BT_SPACE)*i+m_SizeButton.cy+BAR_BT_EXCURSION;

		//�����ж�
		if (rcButton.PtInRect(MousePoint)) return i;
	}

	return INVALID_WORD;
}

//�ػ���Ϣ
VOID CDlgServiceBar::OnPaint()
{
	CPaintDC dc(this);

	//��ȡλ��
/*	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CBitImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//�����豸
	CImageDC BufferDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(BufferDC);

	//�����豸
	pBufferDC->SetBkMode(TRANSPARENT);
	pBufferDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//������Դ
	CPngImage ImageButton;
	CPngImage ImageBarLogo;
	CPngImage ImageBarImage;
	ImageButton.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_BAR_BT"));
	ImageBarLogo.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_BAR_LOGO"));
	ImageBarImage.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_BAR_IMAGE"));

	//�滭����
	pBufferDC->FillSolidRect(&rcClient,RGB(223,236,246));
	ImageBarLogo.DrawImage(pBufferDC,rcClient.Width()-ImageBarLogo.GetWidth(),rcClient.Height()-ImageBarLogo.GetHeight());

	//�滭��ť
	for (INT i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		//��ȡ����
		CDlgServiceItem * pDlgServiceItem=m_DlgServiceItemArray[i];
		LPCTSTR pszServiceName=pDlgServiceItem->GetServiceItemName();

		//���м���
		INT nXButtomPos=0;
		if ((m_wItemHover==i)&&(m_wItemDown!=i)) nXButtomPos=m_SizeButton.cx;
		if ((m_wItemActive==i)||(m_wItemDown==i)) nXButtomPos=m_SizeButton.cx*2;

		//�滭��ť
		INT nXDrawPos=(rcClient.Width()-m_SizeButton.cx)/2;
		INT nYDrawPos=(m_SizeButton.cy+BAR_BT_SPACE)*i+BAR_BT_EXCURSION;
		ImageButton.DrawImage(pBufferDC,nXDrawPos,nYDrawPos,m_SizeButton.cx,m_SizeButton.cy,nXButtomPos,0L);

		//�滭ͼ��
		INT nXImagePos=pDlgServiceItem->m_cbImageIndex*ITEM_IMAGE_CX;
		ImageBarImage.DrawImage(pBufferDC,nXDrawPos+32,nYDrawPos+2,ITEM_IMAGE_CX,ITEM_IMAGE_CY,nXImagePos,0);

		//������ɫ
		if ((m_wItemHover!=i)&&(m_wItemDown!=i)) pBufferDC->SetTextColor(RGB(0,0,0));
		if ((m_wItemHover==i)&&(m_wItemDown!=i)) pBufferDC->SetTextColor(RGB(3,112,218));
		if ((m_wItemActive==i)||(m_wItemDown==i)) pBufferDC->SetTextColor(RGB(255,255,255));

		//�滭�ַ�
		CRect rcItem(nXDrawPos+25,nYDrawPos+1,m_SizeButton.cx+nXDrawPos,m_SizeButton.cy+nYDrawPos);
		pBufferDC->DrawText(pszServiceName,lstrlen(pszServiceName),rcItem,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	}

	//�滭����
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);*/

	return;
}

//������Ϣ
INT CDlgServiceBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//���ñ���
	m_bHovering=false;
	m_wItemDown=INVALID_WORD;
	m_wItemHover=INVALID_WORD;
	m_wItemActive=INVALID_WORD;

	//������Ϣ
	m_rcItemCreate.SetRect(0,0,0,0);
	m_DlgServiceItemArray.RemoveAll();

	return 0;
}

//�����Ϣ
VOID CDlgServiceBar::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	//����λ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//λ�ü���
	WORD wServiceHover=SwitchToButtonIndex(Point);

	//��������
	if (wServiceHover!=m_wItemHover)
	{
		//���ñ���
		m_wItemHover=wServiceHover;

		//���½���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	//�����ж�
	if (m_bHovering==false)
	{
		//���ñ���
		m_bHovering=true;

		//��������
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//ע����Ϣ
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//ע���¼�
		_TrackMouseEvent(&TrackMouseEvent);
	}

	return;
}

//�����Ϣ
VOID CDlgServiceBar::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags,Point);

	//��������
	if ((m_wItemHover!=INVALID_WORD)&&(m_wItemHover==m_wItemDown))
	{
		SetActiveServiceItem(m_DlgServiceItemArray[m_wItemHover]);
	}

	//������
	if (m_wItemDown!=INVALID_WORD)
	{
		//�ͷ����
		ReleaseCapture();

		//���ñ���
		m_wItemDown=INVALID_WORD;

		//���½���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//�����Ϣ
VOID CDlgServiceBar::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//���ý���
	SetFocus();

	//���´���
	if ((m_wItemHover!=INVALID_WORD)&&(m_wItemHover!=m_wItemActive))
	{
		//����˻�
		SetCapture();

		//���ñ���
		m_wItemDown=m_wItemHover;

		//���½���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//�����Ϣ
LRESULT CDlgServiceBar::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_bHovering==true)
	{
		//���ñ���
		m_bHovering=false;
		m_wItemHover=INVALID_WORD;

		//���½���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return 0;
}

//�����Ϣ
BOOL CDlgServiceBar::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//���ù��
	if (m_wItemHover!=INVALID_WORD)
	{
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
		return TRUE;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//////////////////////////////////////////////////////////////////////////////////

//�ؼ���
VOID CDlgService::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//�ؼ�����
	DDX_Control(pDX, IDC_TAB_CONTROL, m_TabControl);
}


//���캯��
CDlgService::CDlgService(BYTE cbPageID /* = SERVICE_INDIVIDUAL */ ) : CSkinDialog(IDD_DLG_SERVICE)
{
	//���ñ���
	m_cbServiceID=0;

	m_SkinAttribute.m_crBackGround=RGB(255,255,255);

	m_cbInitPageID = cbPageID;

	return;
}

//��������
CDlgService::~CDlgService()
{
}

//��Ϣ����
BOOL CDlgService::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//��������
BOOL CDlgService::OnInitDialog()
{
	__super::OnInitDialog();

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//���ñ���
	SetWindowText(TEXT("�û�����"));

	//��������
// 	INT nTViewBorder=m_SkinAttribute.m_EncircleInfoView.nTBorder;
//  	INT nBViewBorder=m_SkinAttribute.m_EncircleInfoView.nBBorder;
//  	INT nLViewBorder=m_SkinAttribute.m_EncircleInfoView.nLBorder;
//  	INT nRViewBorder=m_SkinAttribute.m_EncircleInfoView.nRBorder;
//  	INT nBFrameBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;
// 	
// 	//��������
// 	m_DlgServiceBar.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,IDC_SERVICE_BAR);
// 	m_DlgServiceBar.SetWindowPos(NULL,nLViewBorder,nTViewBorder,SERVICE_BAR_CX,SERVICE_BAR_CY,SWP_NOZORDER);
// 
// 	//����λ��
// 	CRect rcItemCreate;
// 	rcItemCreate.top=nTViewBorder;
// 	rcItemCreate.left=nLViewBorder+SERVICE_BAR_CX;
// 	rcItemCreate.right=rcClient.Width()-nRViewBorder;
// 	rcItemCreate.bottom=nTViewBorder+SERVICE_BAR_CY;
// 	m_DlgServiceBar.SetItemCreateRect(rcItemCreate);
// 
// 	//����ѡ��
// //	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceIndividual);
// //	m_DlgServiceBar.InsertServiceItem(&m_DlgServicePassword);
// 
// 	//��ҳѡ��
// 	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceBrowser1);
// 	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceBrowser2);
// 	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceBrowser3);
// 	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceBrowser4);
// 	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceBrowser5);
// 	m_DlgServiceBar.InsertServiceItem(&m_DlgServiceBrowser6);
// 
// 	//���õ�ַ
// 	m_DlgServiceBrowser1.InitService(5,TEXT("��Ա��ֵ"),TEXT("UserService/UserService1.aspx"));
// 	m_DlgServiceBrowser2.InitService(6,TEXT("��Ϸ��¼"),TEXT("UserService/UserService2.aspx"));
// 	m_DlgServiceBrowser3.InitService(7,TEXT("��ֵ��¼"),TEXT("UserService/UserService3.aspx"));
// 	m_DlgServiceBrowser4.InitService(8,TEXT("��������"),TEXT("UserService/UserService4.aspx"));
// 	m_DlgServiceBrowser5.InitService(9,TEXT("��������"),TEXT("UserService/UserService5.aspx"));
// 	m_DlgServiceBrowser6.InitService(10,TEXT("��Ϸ��Ա"),TEXT("UserService/UserService6.aspx"));
// 
// 	//�ƶ�����
// 	rcClient.bottom=nTViewBorder+nBFrameBorder+SERVICE_BAR_CY;
// 	SetWindowPos(NULL,0,0,rcClient.Width(),rcClient.Height(),SWP_NOZORDER|SWP_NOMOVE);
// 
// 	//����ѡ��
// 	CDlgServiceItem * pDlgServiceItem=GetServiceItem(m_cbServiceID);
// 	if (pDlgServiceItem==NULL) pDlgServiceItem=&m_DlgServiceIndividual;
// 
// 	//����ѡ��
// 	m_DlgServiceBar.SetActiveServiceItem(pDlgServiceItem);


	//�ƶ�����
	//SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

	//SetWindowPos(NULL, 100, 100, 472, 328 , SWP_NOZORDER|SWP_SHOWWINDOW);


	INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
	INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
	INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
	INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;

	//���ÿؼ�
	m_TabControl.MoveWindow( nLBorder-4, 36, rcClient.Width() - nLBorder - nRBorder + 8, rcClient.Height()- nTBorder - nTBorder + 36); 
	m_TabControl.SetItemSize(CSize(rcClient.Width() - nLBorder - nRBorder - 3, 25));

	m_TabControl.InsertItem(SERVICE_INDIVIDUAL,TEXT(" ������Ϣ "));
	m_TabControl.InsertItem(SERVICE_LOCK_MACHINE,TEXT(" �󶨻��� "));
	m_TabControl.InsertItem(SERVICE_PASSWORD,TEXT(" �޸ĵ�¼���� "));
	m_TabControl.InsertItem(SERVICE_BIND_MOBILE,TEXT(" ���ֻ� "));

	//��������
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
	// ������������
	for (INT_PTR i=0;i<m_DlgServiceItemArray.GetCount();i++)
	{
		CWnd * pWnd=m_DlgServiceItemArray[i];
		if (pWnd->m_hWnd!=NULL)
		{
//			LOGI("m_DlgServiceItemArray.MoveWindow(), width="<<rcTabControl.Width()-2*3<<", height="<<rcTabControl.Height()-25-2*3);
			pWnd->MoveWindow( CRect( CPoint(3,25+3), CSize(rcTabControl.Width()-2*3,rcTabControl.Height()-25-2*3) ) );
		}
	}

	// ����ѡ��
//	LOGI("CDlgService::OnInitDialog�� cbPageID="<<m_cbInitPageID);
	m_TabControl.SetCurSel(m_cbInitPageID);
	SwitchPage(m_cbInitPageID);
 
	return TRUE;
}

//ȷ����Ϣ
VOID CDlgService::OnOK()
{
	return;
}

// ���ڴ�С����
VOID CDlgService::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);
}

//������ʶ
VOID CDlgService::SetExpectationID(BYTE cbServiceID)
{
	//���ñ���
	m_cbServiceID=cbServiceID;

	return;
}

//��ȡ����
CDlgServiceItem * CDlgService::GetServiceItem(BYTE cbServiceID)
{
	//��������
/*	switch (cbServiceID)
	{
	case SERVICE_ACCOUNTS:			//��������
		{
			return &m_ServiceModifyAccouts;
		}
	case SERVICE_INDIVIDUAL:		//��������
		{
			return &m_ServiceModifyIndividual;
		}
	case SERVICE_PASSWORD:			//�޸�����
		{
			return &m_ServiceModifyPassword;
		}
	case SERVICE_PASSPROTECT:		//���뱣��
		{
			return &m_ServiceModifyProtect;
		}
	case SERVICE_SECURITY:			//��ȫ����
		{
			return &m_ServiceModifySecurity;
		}
	case SERVICE_MEMBER_PAYMENT:	//��Ա��ֵ
		{
			return &m_ServiceMemberPayment;
		}
	case SERVICE_BANK_ACCESS:		//���д�ȡ
		{
			return &m_ServiceBankAccess;
		}
// 	case SERVICE_BANK_TRANSFER:		//����ת��
// 		{
// 			return &m_ServiceBankTransfer;
// 		}
	case SERVICE_GAME_RECORD:		//��Ϸ��¼
		{
			return &m_ServiceGameRecord;
		}
	case SERVICE_PAY_RECORD:		//��ֵ��¼
		{
			return &m_ServicePayRecord;
		}
	case SERVICE_CLEAN_SCORE:		//��������
		{
			return &m_ServiceCleanScore;
		}
	case SERVICE_CLEAN_FLEE:		//��������
		{
			return &m_ServiceCleanFlee;
		}
	case SERVICE_GAME_MEMBER:		//��Ϸ��Ա
		{
			return &m_ServiceGameMember;
		}
	}*/

	return NULL;
}

// �л���ʾҳ
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
			//6603��������

			//6603���ñ���
//			m_wItemActive=(WORD)i;
			m_cbInitPageID=(BYTE)i;

			//6603��ʾ����
			pDlgServiceItem->ShowWindow(SW_SHOW);
			pDlgServiceItem->SetFocus();
			pDlgServiceItem->SetActiveWindow();

			//6603���ؾ���
			if (pServiceItemActive->GetSafeHwnd()!=NULL)
			{
				pServiceItemActive->ShowWindow(SW_HIDE);
			}

			//6603���½���
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
			return;
		}
	}
// 	switch(cbPageID)
// 	{
// 	case SERVICE_INDIVIDUAL:		// ��������
// 		{
// 			m_DlgServiceIndividual.ShowWindow(SW_SHOW);
// 			m_DlgServiceLockMachine.ShowWindow(SW_HIDE);
// 			m_DlgServicePassword.ShowWindow(SW_HIDE);
// 	
// 			m_DlgServiceIndividual.SetFocus();
// 			m_DlgServiceIndividual.SetActiveWindow();
// 		}
// 		break;
// 	case SERVICE_LOCK_MACHINE:		// ��������
// 		{
// 			m_DlgServiceLockMachine.ShowWindow(SW_SHOW);
// 			m_DlgServiceIndividual.ShowWindow(SW_HIDE);
// 			m_DlgServicePassword.ShowWindow(SW_HIDE);
// 	
// 			m_DlgServiceLockMachine.SetFocus();
// 			m_DlgServiceLockMachine.SetActiveWindow();
// 		}
// 		break;
// 	case SERVICE_PASSWORD:			// ���ĵ�¼����
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
// 	case SERVICE_INDIVIDUAL:		// ��������
// 		{
// 			//��ʾ�ؼ�
// //			m_FaceItemCustomWnd.ShowWindow(SW_SHOW);
// //			m_FaceItemSelectWnd.ShowWindow(SW_HIDE);
// 
// 			//���ý���
// //			m_FaceItemCustomWnd.SetFocus();
// //			m_FaceItemCustomWnd.SetActiveWindow();
// 
// 			SwitchPage(SERVICE_INDIVIDUAL);
// 
// 			break;
// 		}
// 	case SERVICE_LOCK_MACHINE:		//ϵͳͷ��
// 		{
// 			//��ʾ�ؼ�
// //			m_FaceItemCustomWnd.ShowWindow(SW_HIDE);
// //			m_FaceItemSelectWnd.ShowWindow(SW_SHOW);
// 
// 			//���ý���
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

//�滭��Ϣ
VOID CDlgService::OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight)
{
	// �����м�����Ϊ��ɫ

}
