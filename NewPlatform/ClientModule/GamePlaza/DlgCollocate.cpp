#include "StdAfx.h"
#include "GlobalUnits.h"
#include "DlgCollocate.h"
#include "PlatformFrame.h"

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603��ť��Ŀ
#define BUTTON_COUNT_CX				3									//6603������Ŀ
#define BUTTON_COUNT_CY				5									//6603������Ŀ

//6603�ߴ綨��
#define COLLOCATE_BAR_CX			150									//6603���ƿ��
#define COLLOCATE_BAR_CY			330									//6603���Ƹ߶� 

//6603�ߴ綨��
#define BAR_BT_SPACE				3									//6603��ť���
#define BAR_BT_EXCURSION			5									//6603��ťƫ��

//6603�ؼ���ʶ
#define IDC_COLLOCATE_BAR			100									//6603�������

//6603��ťλ��
#define IMAGE_INDEX_NOTIFY			0									//6603��Ϣ����
#define IMAGE_INDEX_CUSTOM			1									//6603��������
#define IMAGE_INDEX_SYSTEM			2									//6603ϵͳ����
#define IMAGE_INDEX_PROXY			3									//6603��������
#define IMAGE_INDEX_SERVER			4									//6603��������

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

BEGIN_MESSAGE_MAP(CCollocateBar, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CCollocateItem, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CCollocateProxy, CCollocateItem)
	ON_BN_CLICKED(IDC_PROXY_TEST, OnBnClickedProxyTest)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgCollocate, CSkinDialog)
	ON_BN_CLICKED(IDC_DEFAULT, OnBnClickedDefault)
END_MESSAGE_MAP()

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603���캯��
CCollocateBar::CCollocateBar()
{
	//6603���Ϣ
	m_bHovering=false;
	m_wItemDown=INVALID_WORD;
	m_wItemHover=INVALID_WORD;
	m_wItemActive=INVALID_WORD;

	//6603������Ϣ
	m_SizeButton.SetSize(0,0);
	m_rcItemCreate.SetRect(0,0,0,0);

	//6603������Դ
	CPngImage ImageButton;
	ImageButton.LoadImage(AfxGetInstanceHandle(),TEXT("COLLOCATE_BAR_BT"));
	m_SizeButton.SetSize(ImageButton.GetWidth()/BUTTON_COUNT_CX,ImageButton.GetHeight()/BUTTON_COUNT_CY);

	return;
}

//6603��������
CCollocateBar::~CCollocateBar()
{
}

//6603��Ϣ����
BOOL CCollocateBar::PreTranslateMessage(MSG * pMsg)
{
	//6603��Ϣ����
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
        GetParent()->PostMessage(WM_CLOSE,0,0);
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//6603�������
bool CCollocateBar::SaveParameter()
{
	//6603�������
	if (m_wItemActive!=INVALID_WORD)
	{
		CCollocateItem * pCollocateItem=m_CollocateItemArray[m_wItemActive];
		if (pCollocateItem->SaveParameter()==false) return false;
	}

	return true;
}

//6603Ĭ�ϲ���
bool CCollocateBar::DefaultParameter()
{
	//6603��������
	for (INT_PTR i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		//6603��ȡ����
		CCollocateItem * pCollocateItem=m_CollocateItemArray[i];

		//6603Ĭ�ϲ���
		if (pCollocateItem->m_hWnd!=NULL) 
		{
			pCollocateItem->DefaultParameter();
			pCollocateItem->UpdateControlStatus();
		}
	}

	return true;
}

//6603��������
bool CCollocateBar::SetItemCreateRect(CRect rcItemCreate)
{
	//6603���ñ���
	m_rcItemCreate=rcItemCreate;

	//6603�����ؼ�
	for (INT_PTR i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		CWnd * pWnd=m_CollocateItemArray[i];
		if (pWnd->m_hWnd!=NULL) pWnd->SetWindowPos(NULL,rcItemCreate.left,rcItemCreate.top,rcItemCreate.Width(),rcItemCreate.Height(),SWP_NOZORDER);
	}

	return true;
}

//6603��������
bool CCollocateBar::InsertCollocateItem(CCollocateItem * pCollocateItem)
{
	//6603���ñ���
	m_CollocateItemArray.Add(pCollocateItem);

	//6603���½���
	if (m_hWnd!=NULL)
	{
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return true;
}

//6603��������
bool CCollocateBar::SetActiveCollocateItem(CCollocateItem * pCollocateItem)
{
	//6603Ч�����
	ASSERT(pCollocateItem!=NULL);
	if (pCollocateItem==NULL) return false;

	//6603��������
	CCollocateItem * pCollocateActive=NULL;

	//6603�л��ж�
	if (m_wItemActive!=INVALID_WORD)
	{
		//6603��ȡ����
		ASSERT(m_wItemActive<m_CollocateItemArray.GetCount());
		pCollocateActive=m_CollocateItemArray[m_wItemActive];

		//6603�л��ж�
		if (pCollocateActive==pCollocateItem) return true;
		if (pCollocateActive->SaveParameter()==false) return false;
	}

	//6603��������
	for (INT_PTR i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		if (m_CollocateItemArray[i]==pCollocateItem)
		{
			//6603��������
			if (pCollocateItem->m_hWnd==NULL)
			{
				pCollocateItem->CreateCollocateItem(GetParent(),m_rcItemCreate);
				pCollocateItem->UpdateControlStatus();
			}

			//6603���ñ���
			m_wItemActive=(WORD)i;

			//6603��ʾ����
			pCollocateItem->ShowWindow(SW_SHOW);
			pCollocateItem->SetFocus();

			//6603���ؾ���
			if (pCollocateActive->GetSafeHwnd()!=NULL)
			{
				pCollocateActive->ShowWindow(SW_HIDE);
			}

			//6603���½���
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);

			return true;
		}
	}

	return false;
}

//6603�����л�
WORD CCollocateBar::SwitchToButtonIndex(CPoint MousePoint)
{
	//6603����λ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//6603�߽��ж�
	if (MousePoint.x<((rcClient.Width()-m_SizeButton.cx)/2)) return INVALID_WORD;
	if (MousePoint.x>((rcClient.Width()+m_SizeButton.cx)/2)) return INVALID_WORD;

	//6603�����ж�
	for (WORD i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		//6603λ�ü���
		CRect rcButton;
		rcButton.left=(rcClient.Width()-m_SizeButton.cx)/2;
		rcButton.right=(rcClient.Width()+m_SizeButton.cx)/2;
		rcButton.top=(m_SizeButton.cy+BAR_BT_SPACE)*i+BAR_BT_EXCURSION;
		rcButton.bottom=(m_SizeButton.cy+BAR_BT_SPACE)*i+m_SizeButton.cy+BAR_BT_EXCURSION;

		//6603�����ж�
		if (rcButton.PtInRect(MousePoint)) return i;
	}

	return INVALID_WORD;
}

//6603�ػ���Ϣ
VOID CCollocateBar::OnPaint()
{
	CPaintDC dc(this);

	//6603��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//6603��������
	CBitImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//6603�����豸
	CImageDC BufferDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(BufferDC);

	//6603�����豸
	pBufferDC->SetBkMode(TRANSPARENT);
	pBufferDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//6603������Դ
	CPngImage ImageButton;
	CPngImage ImageBarLogo;
	ImageButton.LoadImage(AfxGetInstanceHandle(),TEXT("COLLOCATE_BAR_BT"));
	ImageBarLogo.LoadImage(AfxGetInstanceHandle(),TEXT("COLLOCATE_BAR_LOGO"));

	//6603�滭����
	pBufferDC->FillSolidRect(&rcClient,RGB(223,236,246));
	ImageBarLogo.DrawImage(pBufferDC,rcClient.Width()-ImageBarLogo.GetWidth(),rcClient.Height()-ImageBarLogo.GetHeight());

	//6603�滭��ť
	for (INT i=0;i<m_CollocateItemArray.GetCount();i++)
	{
		//6603��ȡ����
		ASSERT(m_CollocateItemArray[i]!=NULL);
		CCollocateItem * pCollocateItem=m_CollocateItemArray[i];

		//6603��������
		INT nXButtomPos=0;
		INT nYButtomPos=pCollocateItem->m_cbImageIndex*m_SizeButton.cy;

		//6603���м���
		if ((m_wItemHover==i)&&(m_wItemDown!=i)) nXButtomPos=m_SizeButton.cx*2;
		if ((m_wItemActive==i)||(m_wItemDown==i)) nXButtomPos=m_SizeButton.cx;

		//6603�滭��ť
		INT nXDrawPos=(rcClient.Width()-m_SizeButton.cx)/2;
		INT nYDrawPos=(m_SizeButton.cy+BAR_BT_SPACE)*i+BAR_BT_EXCURSION;
		ImageButton.DrawImage(pBufferDC,nXDrawPos,nYDrawPos,m_SizeButton.cx,m_SizeButton.cy,nXButtomPos,nYButtomPos);
	}

	//6603�滭����
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	return;
}

//6603������Ϣ
INT CCollocateBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//6603���ñ���
	m_bHovering=false;
	m_wItemDown=INVALID_WORD;
	m_wItemHover=INVALID_WORD;
	m_wItemActive=INVALID_WORD;

	//6603������Ϣ
	m_rcItemCreate.SetRect(0,0,0,0);
	m_CollocateItemArray.RemoveAll();

	return 0;
}

//6603�����Ϣ
VOID CCollocateBar::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	//6603����λ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//6603λ�ü���
	WORD wCollocateHover=SwitchToButtonIndex(Point);

	//6603��������
	if (wCollocateHover!=m_wItemHover)
	{
		//6603���ñ���
		m_wItemHover=wCollocateHover;

		//6603���½���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	//6603�����ж�
	if (m_bHovering==false)
	{
		//6603���ñ���
		m_bHovering=true;

		//6603��������
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//6603ע����Ϣ
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//6603ע���¼�
		_TrackMouseEvent(&TrackMouseEvent);
	}

	return;
}

//6603�����Ϣ
VOID CCollocateBar::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags,Point);

	//6603��������
	if ((m_wItemHover!=INVALID_WORD)&&(m_wItemHover==m_wItemDown))
	{
		SetActiveCollocateItem(m_CollocateItemArray[m_wItemHover]);
	}

	//6603������
	if (m_wItemDown!=INVALID_WORD)
	{
		//6603�ͷ����
		ReleaseCapture();

		//6603���ñ���
		m_wItemDown=INVALID_WORD;

		//6603���½���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//6603�����Ϣ
VOID CCollocateBar::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//6603���ý���
	SetFocus();

	//6603���´���
	if ((m_wItemHover!=INVALID_WORD)&&(m_wItemHover!=m_wItemActive))
	{
		//6603����˻�
		SetCapture();

		//6603���ñ���
		m_wItemDown=m_wItemHover;

		//6603���½���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//6603�����Ϣ
LRESULT CCollocateBar::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	if (m_bHovering==true)
	{
		//6603���ñ���
		m_bHovering=false;
		m_wItemHover=INVALID_WORD;

		//6603���½���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return 0;
}

//6603�����Ϣ
BOOL CCollocateBar::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//6603���ù��
	if (m_wItemHover!=INVALID_WORD)
	{
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
		return TRUE;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603���캯��
CCollocateItem::CCollocateItem(UINT uIDTemplate) : CDialog(uIDTemplate)
{
	//6603������Դ
	m_cbImageIndex=0;

	return;
}

//6603��������
CCollocateItem::~CCollocateItem()
{
}

//6603��������
bool CCollocateItem::CreateCollocateItem(CWnd * pParentWnd, CRect rcCreate)
{
	//6603��������
	Create(m_lpszTemplateName,pParentWnd);

	//6603�ƶ�����
	SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER);

	return true;
}

//6603������Ϣ
BOOL CCollocateItem::OnEraseBkgnd(CDC * pDC)
{
	//6603��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//6603��䱳��
	pDC->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

	return TRUE;
}

//6603��ɫ��Ϣ
HBRUSH CCollocateItem::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(RGB(255,255,255));
			pDC->SetTextColor(RGB(0,0,0));
			return CSkinDialog::m_SkinAttribute.m_brBackGround;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603���캯��
CCollocateServer::CCollocateServer() : CCollocateItem(IDD_COLLOCATE_SERVER)
{
	//6603Ĭ�ϲ���
	DefaultParameter();

	//6603���ñ���
	m_cbImageIndex=IMAGE_INDEX_SERVER;

	return;
}

//6603��������
CCollocateServer::~CCollocateServer()
{
}

//6603�ؼ���
VOID CCollocateServer::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WIN_RATE, m_edWinRate);
	DDX_Control(pDX, IDC_FLEE_RATE, m_edFleeRate);
	DDX_Control(pDX, IDC_GAME_SCORE_MIN, m_edScoreMin);
	DDX_Control(pDX, IDC_GAME_SCORE_MAX, m_edScoreMax);
	DDX_Control(pDX, IDC_TABLE_PASSWORD, m_edPassword);
}

//6603���ú���
BOOL CCollocateServer::OnInitDialog()
{
	__super::OnInitDialog();

	//6603��������
	((CEdit *)GetDlgItem(IDC_WIN_RATE))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_FLEE_RATE))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_GAME_SCORE_MAX))->LimitText(10);
	((CEdit *)GetDlgItem(IDC_GAME_SCORE_MIN))->LimitText(11);
	((CEdit *)GetDlgItem(IDC_TABLE_PASSWORD))->LimitText(LEN_PASSWORD-1);

	return TRUE;
}

//6603�������
bool CCollocateServer::SaveParameter()
{
	//6603��ȡ����
	m_bLimitDetest=(IsDlgButtonChecked(IDC_LIMIT_DETEST)==BST_CHECKED)?true:false;
	m_bLimitSameIP=(IsDlgButtonChecked(IDC_LIMIT_SAME_IP)==BST_CHECKED)?true:false;
	m_bTakePassword=(IsDlgButtonChecked(IDC_TAKE_PASSWORD)==BST_CHECKED)?true:false;
	m_bLimitWinRate=(IsDlgButtonChecked(IDC_LIMIT_WIN_RATE)==BST_CHECKED)?true:false;
	m_bLimitFleeRate=(IsDlgButtonChecked(IDC_LIMIT_FLEE_RATE)==BST_CHECKED)?true:false;
	m_bLimitGameScore=(IsDlgButtonChecked(IDC_LIMIT_GAME_SCORE)==BST_CHECKED)?true:false;

	//6603���ʤ��
	TCHAR szBuffer[128]=TEXT("");
	GetDlgItemText(IDC_WIN_RATE,szBuffer,CountArray(szBuffer));
	m_wMinWinRate=(WORD)(_tstof(szBuffer)*100.0)%10000;

	//6603�������
	GetDlgItemText(IDC_FLEE_RATE,szBuffer,CountArray(szBuffer));
	m_wMaxFleeRate=(WORD)(_tstof(szBuffer)*100.0)%10000;

	//6603��������
	m_lMaxGameScore=GetDlgItemInt(IDC_GAME_SCORE_MAX,NULL,TRUE);
	m_lMinGameScore=GetDlgItemInt(IDC_GAME_SCORE_MIN,NULL,TRUE);
	if ((m_bLimitGameScore)&&(m_lMinGameScore>=m_lMaxGameScore))
	{
		//6603��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("�������Ʒ�Χ��������ȷ������ȷ���û������Ʒ�Χ��"),MB_ICONINFORMATION);

		//6603���ý���
		GetDlgItem(IDC_GAME_SCORE_MAX)->SetFocus();

		return false;
	}

	//6603Я������
	GetDlgItemText(IDC_TABLE_PASSWORD,m_szPassword,CountArray(m_szPassword));
	if ((m_bTakePassword==true)&&(m_szPassword[0]==0))
	{
		//6603��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("����Я������û�����ã���������������Я�����룡"),MB_ICONINFORMATION);

		//6603���ý���
		GetDlgItem(IDC_TABLE_PASSWORD)->SetFocus();

		return false;
	}

	return true;
}

//6603Ĭ�ϲ���
bool CCollocateServer::DefaultParameter()
{
	//6603ʤ������
	m_wMinWinRate=0;
	m_bLimitWinRate=false;

	//6603��������
	m_wMaxFleeRate=5000;
	m_bLimitFleeRate=false;

	//6603��������
	m_bLimitGameScore=false;
	m_lMaxGameScore=2147483647L;
	m_lMinGameScore=-2147483646L;

	//6603��������
	m_bLimitDetest=false;
	m_bLimitSameIP=false;

	//6603Я������
	m_bTakePassword=false;
	ZeroMemory(m_szPassword,sizeof(m_szPassword));

	return true;
}

//6603���¿���
bool CCollocateServer::UpdateControlStatus()
{
	//6603���ÿؼ�
	CheckDlgButton(IDC_LIMIT_DETEST,(m_bLimitDetest==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_LIMIT_SAME_IP,(m_bLimitSameIP==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_TAKE_PASSWORD,(m_bTakePassword==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_LIMIT_WIN_RATE,(m_bLimitWinRate==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_LIMIT_FLEE_RATE,(m_bLimitFleeRate==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_LIMIT_GAME_SCORE,(m_bLimitGameScore==true)?BST_CHECKED:BST_UNCHECKED);

	//6603���ʤ��
	TCHAR szBuffer[128]=TEXT("");
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("%.2f"),((DOUBLE)(m_wMinWinRate))/100.0);
	SetDlgItemText(IDC_WIN_RATE,szBuffer);

	//6603�������
	_sntprintf(szBuffer,CountArray(szBuffer),TEXT("%.2f"),((DOUBLE)(m_wMaxFleeRate))/100.0);
	SetDlgItemText(IDC_FLEE_RATE,szBuffer);

	//6603��������
	SetDlgItemInt(IDC_GAME_SCORE_MAX,m_lMaxGameScore,TRUE);
	SetDlgItemInt(IDC_GAME_SCORE_MIN,m_lMinGameScore,TRUE);

	//6603Я������
	SetDlgItemText(IDC_TABLE_PASSWORD,m_szPassword);

	return true;
}

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603���캯��
CCollocateProxy::CCollocateProxy() : CCollocateItem(IDD_COLLOCATE_PROXY)
{
	//6603Ĭ�ϲ���
	DefaultParameter();

	//6603���ñ���
	m_cbImageIndex=IMAGE_INDEX_PROXY;

	return;
}

//6603��������
CCollocateProxy::~CCollocateProxy()
{
}

//6603�ؼ���
VOID CCollocateProxy::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROXY_TYPE, m_cmProxyType);
	DDX_Control(pDX, IDC_PROXY_TEST, m_btProxyTest);
	DDX_Control(pDX, IDC_PROXY_PORT, m_edProxyPort);
	DDX_Control(pDX, IDC_PROXY_USER, m_edProxyUserName);
	DDX_Control(pDX, IDC_PROXY_PASS, m_edProxyPassword);
	DDX_Control(pDX, IDC_PROXY_SERVER, m_edProxyServer);
}

//6603��������
BOOL CCollocateProxy::OnInitDialog()
{
	__super::OnInitDialog();

	//6603��������
	((CEdit *)(GetDlgItem(IDC_PROXY_PORT)))->LimitText(5);
	((CEdit *)(GetDlgItem(IDC_PROXY_USER)))->LimitText(31);
	((CEdit *)(GetDlgItem(IDC_PROXY_PASS)))->LimitText(31);
	((CEdit *)(GetDlgItem(IDC_PROXY_SERVER)))->LimitText(31);

	//6603��������
	BYTE cbProxyType[]={PROXY_NONE,PROXY_HTTP,PROXY_SOCKS4,PROXY_SOCKS5};
	LPCTSTR szProxyType[]={TEXT("��ʹ�ô���"),TEXT("HTTP ����"),TEXT("SOCKS 4"),TEXT("SOCKS 5")};

	//6603��������
	for (INT i=0;i<CountArray(szProxyType);i++)
	{
		INT nItem=m_cmProxyType.AddString(szProxyType[i]);
		m_cmProxyType.SetItemData(nItem,cbProxyType[i]);
		if (m_cbProxyType==cbProxyType[i]) m_cmProxyType.SetCurSel(nItem);
	}

	//6603������Ϣ
	SetDlgItemText(IDC_PROXY_USER,m_ProxyServer.szUserName);
	SetDlgItemText(IDC_PROXY_PASS,m_ProxyServer.szPassword);
	SetDlgItemText(IDC_PROXY_SERVER,m_ProxyServer.szProxyServer);
	if (m_ProxyServer.wProxyPort!=0) SetDlgItemInt(IDC_PROXY_PORT,m_ProxyServer.wProxyPort);

	//6603����ѡ��
	if (m_cmProxyType.GetCurSel()==CB_ERR) m_cmProxyType.SetCurSel(0);


	// ��ȡ��ɫ
	LOGI("CCollocateProxy::OnInitDialog, m_edProxyPort.m_crEnableBorad="<<m_edProxyPort.m_SkinAttribute.m_crEnableBorad);

	return FALSE;
}

//6603�������
bool CCollocateProxy::SaveParameter()
{
	//6603��������
	m_cbProxyType=(BYTE)m_cmProxyType.GetItemData(m_cmProxyType.GetCurSel());

	//6603������Ϣ
	m_ProxyServer.wProxyPort=GetDlgItemInt(IDC_PROXY_PORT);
	GetDlgItemText(IDC_PROXY_USER,m_ProxyServer.szUserName,CountArray(m_ProxyServer.szUserName));
	GetDlgItemText(IDC_PROXY_PASS,m_ProxyServer.szPassword,CountArray(m_ProxyServer.szPassword));
	GetDlgItemText(IDC_PROXY_SERVER,m_ProxyServer.szProxyServer,CountArray(m_ProxyServer.szProxyServer));

	//6603Ч�����
	if (m_cbProxyType!=PROXY_NONE)
	{
		//6603�����ַ
		if (m_ProxyServer.szProxyServer[0]==0)
		{
			//6603������ʾ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("�����������ַ����Ϊ�գ����������룡"),MB_ICONERROR,30);

			//6603���ý���
			m_edProxyServer.SetFocus();

			return false;
		}

		//6603����˿�
		if (m_ProxyServer.wProxyPort==0)
		{
			//6603������ʾ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("���������������˿ںţ����磺1080��"),MB_ICONERROR,30);

			//6603���ý���
			m_edProxyPort.SetFocus();

			return false;
		}
	}

	return true;
}

//6603Ĭ�ϲ���
bool CCollocateProxy::DefaultParameter()
{
	//6603���ñ���
	m_cbProxyType=PROXY_NONE;
	ZeroMemory(&m_ProxyServer,sizeof(m_ProxyServer));

	return true;
}

//6603���¿���
bool CCollocateProxy::UpdateControlStatus()
{
	//6603������Ϣ
	SetDlgItemText(IDC_PROXY_USER,m_ProxyServer.szUserName);
	SetDlgItemText(IDC_PROXY_PASS,m_ProxyServer.szPassword);
	SetDlgItemText(IDC_PROXY_SERVER,m_ProxyServer.szProxyServer);

	//6603����˿�
	if (m_ProxyServer.wProxyPort==0)
	{
		SetDlgItemText(IDC_PROXY_PORT,TEXT(""));
	}
	else
	{
		SetDlgItemInt(IDC_PROXY_PORT,m_ProxyServer.wProxyPort);
	}

	//6603����ѡ��
	INT i = 0;
	for (i=0;i<m_cmProxyType.GetCount();i++)
	{
		if (m_cmProxyType.GetItemData(i)==m_cbProxyType)
		{
			m_cmProxyType.SetCurSel(i);
		}
	}

	//6603Ĭ��ѡ��
	if (i==m_cmProxyType.GetCount())
	{
		m_cmProxyType.SetCurSel(0);
	}

	return true;
}

//6603�������
VOID CCollocateProxy::OnBnClickedProxyTest()
{
	//6603��������
	BYTE cbProxyType=(BYTE)m_cmProxyType.GetItemData(m_cmProxyType.GetCurSel());

	//6603������Ϣ
	tagProxyServer ProxyServer;
	ZeroMemory(&ProxyServer,sizeof(ProxyServer));
	ProxyServer.wProxyPort=GetDlgItemInt(IDC_PROXY_PORT);
	GetDlgItemText(IDC_PROXY_USER,ProxyServer.szUserName,CountArray(ProxyServer.szUserName));
	GetDlgItemText(IDC_PROXY_PASS,ProxyServer.szPassword,CountArray(ProxyServer.szPassword));
	GetDlgItemText(IDC_PROXY_SERVER,ProxyServer.szProxyServer,CountArray(ProxyServer.szProxyServer));

	//6603Ч�����
	if (cbProxyType!=PROXY_NONE)
	{
		//6603�����ַ
		if (ProxyServer.szProxyServer[0]==0)
		{
			//6603������ʾ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("�����������ַ����Ϊ�գ����������룡"),MB_ICONERROR,30);

			//6603���ý���
			m_edProxyServer.SetFocus();

			return;
		}

		//6603����˿�
		if (ProxyServer.wProxyPort==0)
		{
			//6603������ʾ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("���������������˿ںţ����磺1080��"),MB_ICONERROR,30);

			//6603���ý���
			m_edProxyPort.SetFocus();

			return;
		}
	}
	else 
	{
		//6603������ʾ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("����ѡ������������������������������Ϣ��"),MB_ICONERROR,30);

		//6603���ý���
		m_cmProxyType.SetFocus();

		return;
	}

	//6603�������
	CWHNetworkHelper WHNetworkModule;
	if (WHNetworkModule.CreateInstance()==false)
	{
		ShowInformation(TEXT("�����������������ʧ�ܣ�����ʧ�ܣ�"),MB_ICONERROR,30);
		return;
	}

	//6603�������
	switch (WHNetworkModule->ProxyServerTesting(cbProxyType,ProxyServer))
	{
	case CONNECT_SUCCESS:				//6603���ӳɹ�
		{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("�������������������"),MB_ICONINFORMATION);
			return;
		}
	case CONNECT_PROXY_USER_INVALID:	//6603�û�����
		{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("����������û��������������"),MB_ICONERROR);
			return;
		}
	default:							//6603Ĭ�ϴ���
		{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("��������������ڻ�������ʧ�ܣ�"),MB_ICONERROR);
			return;
		}
	}

	return;
}

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603���캯��
CCollocateNotify::CCollocateNotify() : CCollocateItem(IDD_COLLOCATE_NOTIFY)
{
	//6603Ĭ�ϲ���
	DefaultParameter();

	//6603���ñ���
	m_cbImageIndex=IMAGE_INDEX_NOTIFY;

	return;
}

//6603��������
CCollocateNotify::~CCollocateNotify()
{
}

//6603�������
bool CCollocateNotify::SaveParameter()
{
	//6603��������
	if (IsDlgButtonChecked(IDC_MESSAGE_SHOW_ALL)==BST_CHECKED) m_cbMessageMode=INVITE_MODE_ALL;
	if (IsDlgButtonChecked(IDC_MESSAGE_HIDE_DETEST)==BST_CHECKED) m_cbMessageMode=MESSAGE_MODE_DETEST;
	if (IsDlgButtonChecked(IDC_MESSAGE_ONLY_FRIEND)==BST_CHECKED) m_cbMessageMode=MESSAGE_MODE_FRIEND;

	//6603��������
	if (IsDlgButtonChecked(IDC_INVITE_ALL)==BST_CHECKED) m_cbInviteMode=INVITE_MODE_ALL;
	if (IsDlgButtonChecked(IDC_INVITE_NONE)==BST_CHECKED) m_cbInviteMode=INVITE_MODE_NONE;
	if (IsDlgButtonChecked(IDC_INVITE_ONLY_FRIEND)==BST_CHECKED) m_cbInviteMode=INVITE_MODE_FRIEND;

	//6603˽������
	m_bAutoShowWhisper=(IsDlgButtonChecked(IDC_AUTO_SHOW_WHISPER)==BST_CHECKED);
	m_wWhisperHotKey=(WORD)((CHotKeyCtrl *)GetDlgItem(IDC_WHISPER_HOTKEY))->GetHotKey();

	return true;
}

//6603Ĭ�ϲ���
bool CCollocateNotify::DefaultParameter()
{
	//6603���ñ���
	m_bAutoShowWhisper=false;
	m_wWhisperHotKey=MAKEWORD(VK_F11,0);

	//6603ö�ٱ���
	m_cbInviteMode=INVITE_MODE_ALL;
	m_cbMessageMode=MESSAGE_MODE_DETEST;

	return true;
}

//6603���¿���
bool CCollocateNotify::UpdateControlStatus()
{
	//6603��������
	CheckDlgButton(IDC_MESSAGE_SHOW_ALL,(m_cbMessageMode==INVITE_MODE_ALL)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_MESSAGE_HIDE_DETEST,(m_cbMessageMode==MESSAGE_MODE_DETEST)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_MESSAGE_ONLY_FRIEND,(m_cbMessageMode==MESSAGE_MODE_FRIEND)?BST_CHECKED:BST_UNCHECKED);

	//6603��������
	CheckDlgButton(IDC_INVITE_ALL,(m_cbInviteMode==INVITE_MODE_ALL)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_INVITE_NONE,(m_cbInviteMode==INVITE_MODE_NONE)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_INVITE_ONLY_FRIEND,(m_cbInviteMode==INVITE_MODE_FRIEND)?BST_CHECKED:BST_UNCHECKED);

	//6603˽������
	CheckDlgButton(IDC_AUTO_SHOW_WHISPER,(m_bAutoShowWhisper==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_SHOW_WHISPER_BY_HOTKEY,(m_bAutoShowWhisper==false)?BST_CHECKED:BST_UNCHECKED);
	((CHotKeyCtrl *)GetDlgItem(IDC_WHISPER_HOTKEY))->SetHotKey(LOBYTE(m_wWhisperHotKey),HIBYTE(m_wWhisperHotKey));

	return true;
}

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603���캯��
CCollocateCustom::CCollocateCustom() : CCollocateItem(IDD_COLLOCATE_CUSTOM)
{
	//6603Ĭ�ϲ���
	DefaultParameter();

	//6603���ñ���
	m_cbImageIndex=IMAGE_INDEX_CUSTOM;

	return;
}

//6603��������
CCollocateCustom::~CCollocateCustom()
{
}

//6603�������
bool CCollocateCustom::SaveParameter()
{
	//6603˫���б�
	if (IsDlgButtonChecked(IDC_ACTION_ORIENTATION)==BST_CHECKED) m_cbActionLeftDoubleList=ACTION_ORIENTATION;
	if (IsDlgButtonChecked(IDC_ACTION_SEND_WHISPER)==BST_CHECKED) m_cbActionLeftDoubleList=ACTION_SEND_WHISPER;
	if (IsDlgButtonChecked(IDC_ACTION_SHOW_USER_INFO)==BST_CHECKED) m_cbActionLeftDoubleList=ACTION_SHOW_USER_INFO;

	//6603�Զ�����
	if (IsDlgButtonChecked(IDC_ACTION_SEARCH_TABLE)==BST_CHECKED) m_cbActionHitAutoJoin=ACTION_SEARCH_TABLE;
	if (IsDlgButtonChecked(IDC_SHOW_SEARCH_TABLE_DLG)==BST_CHECKED) m_cbActionHitAutoJoin=ACTION_SHOW_SEARCH_DLG;

	//6603����˽��
	if (IsDlgButtonChecked(IDC_SNED_WHISPER_BY_ENTER)==BST_CHECKED) m_bSendWhisperByEnter=true;
	if (IsDlgButtonChecked(IDC_SNED_WHISPER_BY_CTRL_ENTER)==BST_CHECKED) m_bSendWhisperByEnter=false;

	return true;
}

//6603Ĭ�ϲ���
bool CCollocateCustom::DefaultParameter()
{
	//6603���ñ���
	m_bSendWhisperByEnter=true;

	//6603��������
	m_cbActionHitAutoJoin=ACTION_SEARCH_TABLE;
	m_cbActionLeftDoubleList=ACTION_SEND_WHISPER;

	return true;
}

//6603���¿���
bool CCollocateCustom::UpdateControlStatus()
{
	//6603˫���б�
	CheckDlgButton(IDC_ACTION_ORIENTATION,(m_cbActionLeftDoubleList==ACTION_ORIENTATION)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_ACTION_SEND_WHISPER,(m_cbActionLeftDoubleList==ACTION_SEND_WHISPER)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_ACTION_SHOW_USER_INFO,(m_cbActionLeftDoubleList==ACTION_SHOW_USER_INFO)?BST_CHECKED:BST_UNCHECKED);

	//6603�Զ�����
	CheckDlgButton(IDC_ACTION_SEARCH_TABLE,(m_cbActionHitAutoJoin==ACTION_SEARCH_TABLE)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_SHOW_SEARCH_TABLE_DLG,(m_cbActionHitAutoJoin==ACTION_SHOW_SEARCH_DLG)?BST_CHECKED:BST_UNCHECKED);

	//6603����˽��
	CheckDlgButton(IDC_SNED_WHISPER_BY_ENTER,(m_bSendWhisperByEnter==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_SNED_WHISPER_BY_CTRL_ENTER,(m_bSendWhisperByEnter==false)?BST_CHECKED:BST_UNCHECKED);

	return true;
}

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603���캯��
CCollocateSystem::CCollocateSystem() : CCollocateItem(IDD_COLLOCATE_SYSTEM)
{
	//6603Ĭ�ϲ���
	DefaultParameter();

	//6603���ñ���
	m_cbImageIndex=IMAGE_INDEX_SYSTEM;

	return;
}

//6603��������
CCollocateSystem::~CCollocateSystem()
{
}

//6603�������
bool CCollocateSystem::SaveParameter()
{
	//6603��������
	m_bAllowSound=(IsDlgButtonChecked(IDC_ALLOW_SOUND)==BST_CHECKED);
	m_bAllowBackMusic=(IsDlgButtonChecked(IDC_ALLOW_BACKMUSIC)==BST_CHECKED);
	m_bAutoSitDown=(IsDlgButtonChecked(IDC_AUTO_SIT_DOWN)==BST_CHECKED);
	m_bSalienceTable=(IsDlgButtonChecked(IDC_SALIENCE_TABLE)==BST_CHECKED);
	m_bSaveWhisperChat=(IsDlgButtonChecked(IDC_AUTO_SAVE_WHISPER)==BST_CHECKED);
	m_bNotifyUserInOut=(IsDlgButtonChecked(IDC_NOTIFY_USER_INOUT)==BST_CHECKED);
	m_bNotifyFriendCome=(IsDlgButtonChecked(IDC_NOTIFY_FRIEND_COME)==BST_CHECKED);
	m_bFullScreenBrowse=(IsDlgButtonChecked(IDC_FULL_SCREEN_BROWSE)==BST_CHECKED);

	//6603�ϰ��
	m_wBossHotKey=(WORD)((CHotKeyCtrl *)GetDlgItem(IDC_BOSS_HOTKEY))->GetHotKey();

	return true;
}

//6603Ĭ�ϲ���
bool CCollocateSystem::DefaultParameter()
{
	//6603���ñ���
	m_wBossHotKey=MAKEWORD(VK_F12,HOTKEYF_CONTROL);

	//6603���ñ���
	m_bAllowSound=true;
	m_bAllowBackMusic=true;
	m_bAutoSitDown=true;
	m_bSalienceTable=true;
	m_bSaveWhisperChat=true;
	m_bNotifyUserInOut=false;
	m_bNotifyFriendCome=true;
	m_bFullScreenBrowse=false;

	return true;
}

//6603���¿���
bool CCollocateSystem::UpdateControlStatus()
{
	//6603��ʱ����ѡ��
	m_bAllowSound=false;
	m_bAllowBackMusic=false;
	GetDlgItem(IDC_ALLOW_SOUND)->EnableWindow(FALSE);
	GetDlgItem(IDC_ALLOW_BACKMUSIC)->EnableWindow(FALSE);

	//6603���ÿؼ�
	CheckDlgButton(IDC_ALLOW_SOUND,(m_bAllowSound==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_ALLOW_BACKMUSIC,(m_bAllowBackMusic==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_AUTO_SIT_DOWN,(m_bAutoSitDown==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_SALIENCE_TABLE,(m_bSalienceTable==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_AUTO_SAVE_WHISPER,(m_bSaveWhisperChat==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_NOTIFY_USER_INOUT,(m_bNotifyUserInOut==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_NOTIFY_FRIEND_COME,(m_bNotifyFriendCome==true)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_FULL_SCREEN_BROWSE,(m_bFullScreenBrowse==true)?BST_CHECKED:BST_UNCHECKED);
	
	//6603�ϰ��
	((CHotKeyCtrl *)GetDlgItem(IDC_BOSS_HOTKEY))->SetHotKey(LOBYTE(m_wBossHotKey),HIBYTE(m_wBossHotKey));

	return true;
}

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603���캯��
CDlgCollocate::CDlgCollocate() : CSkinDialog(IDD_COLLOCATE)
{
	//6603���ñ���
	m_pParameterGame=NULL;
	m_pParameterServer=NULL;

	return;
}

//6603��������
CDlgCollocate::~CDlgCollocate()
{
}

//6603���ò���
bool CDlgCollocate::InitCollocate(CParameterGame * pParameterGame, CParameterServer * pParameterServer)
{
	//6603���ñ���
	m_pParameterGame=pParameterGame;
	m_pParameterServer=pParameterServer;

	return true;
}

//6603�ؼ���
VOID CDlgCollocate::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_DEFAULT, m_btDefault);
}

//6603��������
BOOL CDlgCollocate::OnInitDialog()
{
	__super::OnInitDialog();

	//6603��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//6603���ñ���
	SetWindowText(TEXT("ϵͳ����"));

	//6603��������
	INT nTViewBorder=m_SkinAttribute.m_EncircleInfoView.nTBorder;
	INT nBViewBorder=m_SkinAttribute.m_EncircleInfoView.nBBorder;
	INT nLViewBorder=m_SkinAttribute.m_EncircleInfoView.nLBorder;
	INT nRViewBorder=m_SkinAttribute.m_EncircleInfoView.nRBorder;
	INT nBFrameBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;
	
	//6603��������
	m_CollocateBar.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,IDC_COLLOCATE_BAR);
	m_CollocateBar.SetWindowPos(NULL,nLViewBorder,nTViewBorder,COLLOCATE_BAR_CX,COLLOCATE_BAR_CY,SWP_NOZORDER);

	//6603����λ��
	CRect rcItemCreate;
	rcItemCreate.top=nTViewBorder;
	rcItemCreate.left=nLViewBorder+COLLOCATE_BAR_CX;
	rcItemCreate.right=rcClient.Width()-nRViewBorder;
	rcItemCreate.bottom=nTViewBorder+COLLOCATE_BAR_CY;
	m_CollocateBar.SetItemCreateRect(rcItemCreate);

	//6603�ƶ���ť
	CRect rcButton;
	m_btOk.GetWindowRect(&rcButton);

	//6603�ƶ�����
	rcClient.bottom=nTViewBorder+nBFrameBorder+COLLOCATE_BAR_CY;
	SetWindowPos(NULL,0,0,rcClient.Width(),rcClient.Height(),SWP_NOZORDER|SWP_NOMOVE);

	//6603�ƶ���ť
	INT nYButtonPos=rcClient.Height()-rcButton.Height()-nBViewBorder-4;
	m_btOk.SetWindowPos(NULL,rcClient.Width()-rcButton.Width()*3-nRViewBorder-18,nYButtonPos,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_btCancel.SetWindowPos(NULL,rcClient.Width()-rcButton.Width()-nRViewBorder-8,nYButtonPos,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_btDefault.SetWindowPos(NULL,rcClient.Width()-rcButton.Width()*2-nRViewBorder-13,nYButtonPos,0,0,SWP_NOZORDER|SWP_NOSIZE);

	//6603��������
	ASSERT(CParameterGlobal::GetInstance()!=NULL);
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//6603����ѡ��
	m_CollocateProxy.m_cbProxyType=pParameterGlobal->m_cbProxyType;
	m_CollocateProxy.m_ProxyServer=pParameterGlobal->m_ProxyServer;

	//6603��Ϣ����
	m_CollocateNotify.m_cbInviteMode=pParameterGlobal->m_cbInviteMode;
	m_CollocateNotify.m_cbMessageMode=pParameterGlobal->m_cbMessageMode;
	m_CollocateNotify.m_wWhisperHotKey=pParameterGlobal->m_wWhisperHotKey;
	m_CollocateNotify.m_bAutoShowWhisper=pParameterGlobal->m_bAutoShowWhisper;

	//6603ϵͳ����
	m_CollocateSystem.m_wBossHotKey=pParameterGlobal->m_wBossHotKey;
	m_CollocateSystem.m_bAllowSound=pParameterGlobal->m_bAllowSound;
	m_CollocateSystem.m_bAllowBackMusic=pParameterGlobal->m_bAllowBackMusic;
	m_CollocateSystem.m_bAutoSitDown=pParameterGlobal->m_bAutoSitDown;
	m_CollocateSystem.m_bSalienceTable=pParameterGlobal->m_bSalienceTable;
	m_CollocateSystem.m_bSaveWhisperChat=pParameterGlobal->m_bSaveWhisperChat;
	m_CollocateSystem.m_bNotifyUserInOut=pParameterGlobal->m_bNotifyUserInOut;
	m_CollocateSystem.m_bNotifyFriendCome=pParameterGlobal->m_bNotifyFriendCome;
	m_CollocateSystem.m_bFullScreenBrowse=pParameterGlobal->m_bFullScreenBrowse;

	//6603��������
	m_CollocateCustom.m_cbActionHitAutoJoin=pParameterGlobal->m_cbActionHitAutoJoin;
	m_CollocateCustom.m_bSendWhisperByEnter=pParameterGlobal->m_bSendWhisperByEnter;
	m_CollocateCustom.m_cbActionLeftDoubleList=pParameterGlobal->m_cbActionLeftDoubleList;

	//6603ȫ������
	m_CollocateBar.InsertCollocateItem(&m_CollocateNotify);
	m_CollocateBar.InsertCollocateItem(&m_CollocateSystem);
	m_CollocateBar.InsertCollocateItem(&m_CollocateCustom);
	m_CollocateBar.InsertCollocateItem(&m_CollocateProxy);

	//6603//6603��������
	//6603if ((m_pParameterServer!=NULL)&&(m_pParameterGame!=NULL))
	//6603{
	//6603	//6603��Ϸ����
	//6603	m_CollocateServer.m_wMinWinRate=m_pParameterGame->m_wMinWinRate;
	//6603	m_CollocateServer.m_wMaxFleeRate=m_pParameterGame->m_wMaxFleeRate;
	//6603	m_CollocateServer.m_bLimitWinRate=m_pParameterGame->m_bLimitWinRate;
	//6603	m_CollocateServer.m_lMaxGameScore=m_pParameterGame->m_lMaxGameScore;
	//6603	m_CollocateServer.m_lMinGameScore=m_pParameterGame->m_lMinGameScore;
	//6603	m_CollocateServer.m_bLimitFleeRate=m_pParameterGame->m_bLimitFleeRate;
	//6603	m_CollocateServer.m_bLimitGameScore=m_pParameterGame->m_bLimitGameScore;

	//6603	//6603��������
	//6603	m_CollocateServer.m_bLimitDetest=pParameterGlobal->m_bLimitDetest;
	//6603	m_CollocateServer.m_bLimitSameIP=pParameterGlobal->m_bLimitSameIP;

	//6603	//6603��������
	//6603	m_CollocateServer.m_bTakePassword=m_pParameterServer->m_bTakePassword;
	//6603	lstrcpyn(m_CollocateServer.m_szPassword,m_pParameterServer->m_szPassword,CountArray(m_CollocateServer.m_szPassword));

	//6603	//6603��������
	//6603	m_CollocateBar.InsertCollocateItem(&m_CollocateServer);
	//6603}

	//6603����ѡ��
	//6603if ((m_pParameterServer!=NULL)&&(m_pParameterGame!=NULL))
	//6603{
	//6603	m_CollocateBar.SetActiveCollocateItem(&m_CollocateServer);
	//6603}
	//6603else
	{
		m_CollocateBar.SetActiveCollocateItem(&m_CollocateNotify);
	}

	return TRUE;
}

//6603ȷ������
VOID CDlgCollocate::OnOK()
{
	//6603�������
	if (m_CollocateBar.SaveParameter()==false) return;

	//6603��������
	ASSERT(CParameterGlobal::GetInstance()!=NULL);
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//6603�ϰ��ȼ�
	UnregisterHotKey(AfxGetMainWnd()->GetSafeHwnd(),IDI_HOT_KEY_BOSS);
	if (CWHService::RegisterHotKey(AfxGetMainWnd()->GetSafeHwnd(),IDI_HOT_KEY_BOSS,m_CollocateSystem.m_wBossHotKey)==false)
	{
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("ϵͳ�ȼ���ͻ���ϰ��ȼ�ע��ʧ�ܣ�"));
	}

	//6603˽���ȼ�
	UnregisterHotKey(AfxGetMainWnd()->GetSafeHwnd(),IDI_HOT_KEY_WHISPER);
	if (CWHService::RegisterHotKey(AfxGetMainWnd()->GetSafeHwnd(),IDI_HOT_KEY_WHISPER,m_CollocateNotify.m_wWhisperHotKey)==false)
	{
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("ϵͳ�ȼ���ͻ����ȡ��Ϣ�ȼ�ע��ʧ�ܣ�"));
	}

	//6603ͬ������
	if ((m_pParameterServer!=NULL)&&(m_pParameterGame!=NULL))
	{
		//6603��������
		pParameterGlobal->m_bLimitDetest=m_CollocateServer.m_bLimitDetest;
		pParameterGlobal->m_bLimitSameIP=m_CollocateServer.m_bLimitSameIP;

		//6603��Ϸ����
		m_pParameterGame->m_wMinWinRate=m_CollocateServer.m_wMinWinRate;
		m_pParameterGame->m_wMaxFleeRate=m_CollocateServer.m_wMaxFleeRate;
		m_pParameterGame->m_lMaxGameScore=m_CollocateServer.m_lMaxGameScore;
		m_pParameterGame->m_lMinGameScore=m_CollocateServer.m_lMinGameScore;
		m_pParameterGame->m_bLimitWinRate=m_CollocateServer.m_bLimitWinRate;
		m_pParameterGame->m_bLimitFleeRate=m_CollocateServer.m_bLimitFleeRate;
		m_pParameterGame->m_bLimitGameScore=m_CollocateServer.m_bLimitGameScore;

		//6603��������
		m_pParameterServer->m_bTakePassword=m_CollocateServer.m_bTakePassword;
		lstrcpyn(m_pParameterServer->m_szPassword,m_CollocateServer.m_szPassword,CountArray(m_pParameterServer->m_szPassword));
	}

	//6603����ѡ��
	pParameterGlobal->m_cbProxyType=m_CollocateProxy.m_cbProxyType;
	pParameterGlobal->m_ProxyServer=m_CollocateProxy.m_ProxyServer;

	//6603��Ϣ����
	pParameterGlobal->m_cbInviteMode=m_CollocateNotify.m_cbInviteMode;
	pParameterGlobal->m_cbMessageMode=m_CollocateNotify.m_cbMessageMode;
	pParameterGlobal->m_wWhisperHotKey=m_CollocateNotify.m_wWhisperHotKey;
	pParameterGlobal->m_bAutoShowWhisper=m_CollocateNotify.m_bAutoShowWhisper;

	//6603ϵͳ����
	pParameterGlobal->m_wBossHotKey=m_CollocateSystem.m_wBossHotKey;
	pParameterGlobal->m_bAllowSound=m_CollocateSystem.m_bAllowSound;
	pParameterGlobal->m_bAllowBackMusic=m_CollocateSystem.m_bAllowBackMusic;
	pParameterGlobal->m_bAutoSitDown=m_CollocateSystem.m_bAutoSitDown;
	pParameterGlobal->m_bSalienceTable=m_CollocateSystem.m_bSalienceTable;
	pParameterGlobal->m_bSaveWhisperChat=m_CollocateSystem.m_bSaveWhisperChat;
	pParameterGlobal->m_bNotifyUserInOut=m_CollocateSystem.m_bNotifyUserInOut;
	pParameterGlobal->m_bNotifyFriendCome=m_CollocateSystem.m_bNotifyFriendCome;
	pParameterGlobal->m_bFullScreenBrowse=m_CollocateSystem.m_bFullScreenBrowse;

	//6603��������
	pParameterGlobal->m_cbActionHitAutoJoin=m_CollocateCustom.m_cbActionHitAutoJoin;
	pParameterGlobal->m_bSendWhisperByEnter=m_CollocateCustom.m_bSendWhisperByEnter;
	pParameterGlobal->m_cbActionLeftDoubleList=m_CollocateCustom.m_cbActionLeftDoubleList;

	__super::OnOK();
}

//6603Ĭ�ϰ�ť
VOID CDlgCollocate::OnBnClickedDefault()
{
	//6603Ĭ�ϲ���
	m_CollocateBar.DefaultParameter();

	return;
}

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603
