#include "StdAfx.h"
#include "LastGameServerCtrl.h"
#include "PlatformFrame.h"

BEGIN_MESSAGE_MAP(CLastGameServerCtrl, CWnd)

	//ϵͳ��Ϣ
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()

	// �����Ϣ
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()

END_MESSAGE_MAP()

CLastGameServerCtrl::CLastGameServerCtrl(void)
{
	m_ImageTypeBkgnd.LoadFromResource(AfxGetInstanceHandle(),IDB_SERVERITEM_BKGND);
	m_ImageArrow1.LoadImage(AfxGetInstanceHandle(),TEXT("KIND_TYPE_ARROW"));
//	m_ImageArrow2.LoadImage(AfxGetInstanceHandle(),TEXT("SERVER_LIST_ARROW"));

	m_FontBold.CreateFont(-12,0,0,0,600,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("����"));
	m_FontUnderLine.CreateFont(-12,0,0,0,300,0,1,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("����"));

	m_rcTypeBkgnd = CRect(CPoint(1,1), CSize(m_ImageTypeBkgnd.GetWidth()/3,m_ImageTypeBkgnd.GetHeight()));
	m_wIndexBkgnd = 0;

	m_bShowLastGame = true;

	m_vectRectLastGame.push_back(CRect(CPoint(12,m_ImageTypeBkgnd.GetHeight()+5), CSize(66+6,89)));
	m_vectRectLastGame.push_back(CRect(CPoint(12+12+72,m_ImageTypeBkgnd.GetHeight()+5), CSize(66+6,89)));
	m_vectRectLastGame.push_back(CRect(CPoint(12+12*2+72*2,m_ImageTypeBkgnd.GetHeight()+5), CSize(66+6,89)));

	m_vectGameServerItem.clear();

	m_wHoverRectIndex = -1;
}

CLastGameServerCtrl::~CLastGameServerCtrl(void)
{
//	for (map<WORD, CPngImage*>::iterator iter = m_mapLastGameIcon.begin(); iter != m_mapLastGameIcon.end(); iter++)
	for (map<WORD, CImage*>::iterator iter = m_mapLastGameIcon.begin(); iter != m_mapLastGameIcon.end(); iter++)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
			iter->second = NULL;
		}
	}
	m_mapLastGameIcon.clear();
}

VOID CLastGameServerCtrl::OnPaint()
{
	CPaintDC dc(this);

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CImage ImageBuffer;
	if(ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32) == FALSE)
		return;

	//���� DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(BufferDC);

	pBufferDC->FillSolidRect(&rcClient,RGB(234,242,247));

	//���Ƶװ�
	m_ImageTypeBkgnd.StretchBlt(pBufferDC->GetSafeHdc(), m_rcTypeBkgnd.left, m_rcTypeBkgnd.top, m_rcTypeBkgnd.Width(), m_rcTypeBkgnd.Height(), 
		m_ImageTypeBkgnd.GetWidth() / 3 * m_wIndexBkgnd, 0, m_ImageTypeBkgnd.GetWidth() / 3, m_ImageTypeBkgnd.GetHeight()); 

	//���Ƽ�ͷ
	CSize SizeArrow(m_ImageArrow1.GetWidth(), m_ImageArrow1.GetHeight());
	m_ImageArrow1.DrawImage(pBufferDC, rcClient.left + 6, rcClient.top + 8, SizeArrow.cx / 2, SizeArrow.cy, m_ImageArrow1.GetWidth() / 2 * (m_bShowLastGame?1:0), 0, SizeArrow.cx / 2, SizeArrow.cy);

	//���û���
	pBufferDC->SelectObject(m_FontBold);
	pBufferDC->SetBkMode(TRANSPARENT);
	pBufferDC->SetTextColor(RGB(13,61,88));
	pBufferDC->DrawText(_T("�����Ϸ"), CRect(CPoint(19,3), CSize(rcClient.Width(),m_ImageTypeBkgnd.GetHeight())), DT_LEFT|DT_VCENTER|DT_SINGLELINE);

	// ������Ϸͼ��
	if (m_bShowLastGame)
	{
		DWORD wIndex = 0;
//		for (map<WORD, CGameServerItem*>::iterator iter = m_mapLastGameServer.begin(); iter != m_mapLastGameServer.end(); iter++)
		for (vector<CGameServerItem*>::iterator iter = m_vectGameServerItem.begin(); iter != m_vectGameServerItem.end(); iter++)
		{
			if (wIndex >= m_vectRectLastGame.size())
			{
				continue;
			}
			CRect rcLastGame = m_vectRectLastGame.at(wIndex);
			// �������ڵ�ǰ������HOVER�������Բ�Ǿ��ο�
			if (m_wHoverRectIndex == wIndex)
			{
				CBrush brush;
				brush.CreateSolidBrush(RGB(165,225,255));
				pBufferDC->SelectObject(&brush);
				CPen pen;
				pen.CreatePen(PS_SOLID, 0, RGB(165,225,255));
				CPen* pOldPen = pBufferDC->SelectObject(&pen);
				pBufferDC->RoundRect(rcLastGame.left,rcLastGame.top,rcLastGame.right,rcLastGame.bottom,5,5);
				pBufferDC->SelectObject(GetStockObject(NULL_BRUSH)); 
				pBufferDC->SelectObject(pOldPen);
			}
			// ���������ͼƬ��ʾ
			CImage* ImageIcon=NULL;
//			map<WORD, CImage*>::iterator iter_img = m_mapLastGameIcon.find(iter->second->m_GameServer.wKindID);
			map<WORD, CImage*>::iterator iter_img = m_mapLastGameIcon.find((*iter)->m_GameServer.wKindID);
			if (iter_img != m_mapLastGameIcon.end())
			{
				ImageIcon = iter_img->second;
			}
			if (ImageIcon && ImageIcon->IsNull()==false)
			{	
				// ����ͼ��
				ImageIcon->BitBlt(pBufferDC->GetSafeHdc(), rcLastGame.left+(rcLastGame.Width()-ImageIcon->GetWidth())/2, rcLastGame.top+3, ImageIcon->GetWidth(), ImageIcon->GetHeight(), 0, 0);
//				ImageIcon->DrawImage(pBufferDC, rcLastGame.left+(rcLastGame.Width()-ImageIcon->GetWidth())/2, rcLastGame.top+3, ImageIcon->GetWidth(), ImageIcon->GetHeight(), 0, 0, ImageIcon->GetWidth(), ImageIcon->GetHeight());
			}
			//���Ʒ�������
			pBufferDC->SelectObject(m_FontUnderLine);
			pBufferDC->SetBkMode(TRANSPARENT);
			pBufferDC->SetTextColor(RGB(13,61,88));

			CString strName, strDesc;
//			CString strServerName(iter->second->m_GameServer.szServerName);
			CString strServerName( (*iter)->m_GameServer.szServerName);
			int nIndex=strServerName.Find(TEXT("|"));
			if(nIndex!=-1)
			{
				strName=strServerName.Left(nIndex);
				strDesc=strServerName.Right(strServerName.GetLength()-nIndex-1);
			}
			else
			{
				strName=strServerName;
			}

			CRect rcText = CRect(rcLastGame.left, rcLastGame.bottom-21, rcLastGame.right, rcLastGame.bottom);
			pBufferDC->DrawText(strName,rcText,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			wIndex++;
		}
	}

	//�滭����
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);
}

//�滭����
BOOL CLastGameServerCtrl::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}

//λ����Ϣ
VOID CLastGameServerCtrl::OnSize(UINT nType, INT cx, INT cy)
{

}

//������Ϣ
INT CLastGameServerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
//	LOGI("CLastGameServerCtrl::OnCreate, size="<<m_mapLastGameServer.size());
	return 0;
}

//����ƶ�
VOID CLastGameServerCtrl::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	// �ж����ĸ�������
 	if (m_rcTypeBkgnd.PtInRect(Point))
 	{
		m_wIndexBkgnd = 2;
		InvalidateRect(m_rcTypeBkgnd);
		return;
 	}
	else
	{
		m_wIndexBkgnd = 0;
	}

	for (size_t i = 0; i < m_vectRectLastGame.size(); i++)
	{
		if (m_vectRectLastGame.at(i).PtInRect(Point))
		{
			if (m_wHoverRectIndex >=0 && m_wHoverRectIndex < m_vectRectLastGame.size())
			{
				InvalidateRect(m_vectRectLastGame.at(m_wHoverRectIndex));
			}
			m_wHoverRectIndex = (WORD)i;
			// ���Ʊ������ɫ
			InvalidateRect(m_vectRectLastGame.at(i));
			break;
		}
		else
		{
			m_wHoverRectIndex = -1;
			// ���Ʊ������ɫ
			InvalidateRect(m_vectRectLastGame.at(i));
		}
	}

	
}

//�����Ϣ
VOID CLastGameServerCtrl::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags, Point);
	// �ж����ĸ�������
	if (m_rcTypeBkgnd.PtInRect(Point))
	{

	}
}
//�����Ϣ
VOID CLastGameServerCtrl::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);
	// �ж����ĸ�������
	if (m_rcTypeBkgnd.PtInRect(Point))
	{
		m_wIndexBkgnd = 1;
		InvalidateRect(m_rcTypeBkgnd);

		// ͬʱ���������ͼ����
		m_bShowLastGame = !m_bShowLastGame;

		if (m_bShowLastGame)
		{
			CRect rcClient;
			GetWindowRect(rcClient);
			MoveWindow(rcClient.left, rcClient.top, rcClient.Width(), 130);
		}
		else
		{
			CRect rcClient;
			GetWindowRect(rcClient);
			MoveWindow(rcClient.left, rcClient.top, rcClient.Width(), m_ImageTypeBkgnd.GetHeight() + 2);

			m_wHoverRectIndex = -1;
		}
		GetParent()->PostMessage(WM_LASTGAMESERVERCTRL_MOVE, NULL, NULL);
		return;
	}

	for (size_t i = 0; i < m_vectRectLastGame.size(); i++)
	{
		if (m_vectRectLastGame.at(i).PtInRect(Point))
		{
			// ��ѯ�Ƿ���Ե�¼����
// 			map<WORD, CGameServerItem*>::iterator iter = m_mapLastGameServer.begin();
// 			for (size_t j = 0; j < i; j++)
// 			{
// 				iter++;
// 			}
// 			if (iter != m_mapLastGameServer.end())
// 			{
// 				CPlatformFrame * pPlatformFrame=CPlatformFrame::GetInstance();
// 				pPlatformFrame->EntranceServerItem(iter->second);
// 			}
			vector<CGameServerItem*>::iterator iter = m_vectGameServerItem.begin()+i;
			if (iter != m_vectGameServerItem.end())
			{
				CPlatformFrame * pPlatformFrame=CPlatformFrame::GetInstance();
				pPlatformFrame->EntranceServerItem((*iter));
			}
		}
	}
}

VOID CLastGameServerCtrl::OnAddLastPlayGame(WORD wServerID, CGameServerItem* pGameServerItem)
{
// 	map<WORD, CGameServerItem*>::iterator iter = m_mapLastGameServer.find(wServerID);
// 	if (iter != m_mapLastGameServer.end())
// 	{
// 		iter->second = pGameServerItem;
// 	}
// 	else
// 	{
// 		m_mapLastGameServer.insert(make_pair(wServerID, pGameServerItem));
// 	}
	for (vector<CGameServerItem*>::iterator iter=m_vectGameServerItem.begin(); iter!=m_vectGameServerItem.end();iter++)
	{
		if ((*iter)->m_GameServer.wServerID==wServerID)
		{
			// �Ѿ����ڣ���ɾ����֮����ͷ����
			m_vectGameServerItem.erase(iter);
			break;
		}
	}

	for (size_t i = 0; i < m_vectGameServerItem.size(); i++)
	{
		CGameServerItem* pGameServerItemold = m_vectGameServerItem.at(i);
		int j = 0;
	}
	// ��ͷ�������·���
	m_vectGameServerItem.insert(m_vectGameServerItem.begin(), pGameServerItem);

	for (size_t i = 0; i < m_vectGameServerItem.size(); i++)
	{
		CGameServerItem* pGameServerItemold = m_vectGameServerItem.at(i);
		int j = 0;
	}

	// ����ϷĿ¼������ͼƬ
	map<WORD, CImage*>::iterator iter_img = m_mapLastGameIcon.find(pGameServerItem->m_GameServer.wKindID);
	if (iter_img == m_mapLastGameIcon.end())
	{
		// �õ���Դ·��
		TCHAR szGameResDirectory[LEN_PROCESS] = _T("");
		CWHService::GetGameResDirectory( pGameServerItem->m_pGameKindItem->m_GameKind.szProcessName, szGameResDirectory, CountArray(szGameResDirectory) );
		TCHAR szWorkDirectory[LEN_PROCESS] = _T("");
		CWHService::GetWorkDirectory( szWorkDirectory, sizeof(szWorkDirectory) );
		TCHAR szImagePath[MAX_PATH] = _T("");
		_sntprintf(szImagePath, sizeof(szImagePath), _T("%s\\%s\\icon.png"), szWorkDirectory, szGameResDirectory);
		// ����ͼƬ
		CImage* pImageIcon = new CImage;
		pImageIcon->Load(szImagePath);
		if (!pImageIcon->IsNull())
		{
			m_mapLastGameIcon.insert(make_pair(pGameServerItem->m_GameServer.wKindID, pImageIcon));
		}
	}
}

// ɾ����������Ϸ
VOID CLastGameServerCtrl::OnRemoveLastPlayGame(WORD wServerID)
{
// 	map<WORD, CGameServerItem*>::iterator iter = m_mapLastGameServer.find(wServerID);
// 	if (iter != m_mapLastGameServer.end())
// 	{
// 		m_mapLastGameServer.erase(iter);
// 	}
	for (vector<CGameServerItem*>::iterator iter=m_vectGameServerItem.begin(); iter!=m_vectGameServerItem.end(); iter++)
	{
		if ((*iter)->m_GameServer.wServerID == wServerID)
		{
			m_vectGameServerItem.erase(iter);
			break;
		}
	}
}

// ������������Ϸ����
VOID CLastGameServerCtrl::OnUpdateLastPlayGame(WORD wServerID, CGameServerItem *pGameServerItem)
{
	for (size_t i = 0; i < m_vectGameServerItem.size(); i++)
	{
		if (m_vectGameServerItem.at(i)->m_GameServer.wServerID == wServerID)
		{
			m_vectGameServerItem.at(i) = pGameServerItem;
		}
	}
}
