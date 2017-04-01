#include "StdAfx.h"
#include "LastGameServerCtrl.h"
#include "PlatformFrame.h"

BEGIN_MESSAGE_MAP(CLastGameServerCtrl, CWnd)

	//系统消息
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()

	// 鼠标消息
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()

END_MESSAGE_MAP()

CLastGameServerCtrl::CLastGameServerCtrl(void)
{
	m_ImageTypeBkgnd.LoadFromResource(AfxGetInstanceHandle(),IDB_SERVERITEM_BKGND);
	m_ImageArrow1.LoadImage(AfxGetInstanceHandle(),TEXT("KIND_TYPE_ARROW"));
//	m_ImageArrow2.LoadImage(AfxGetInstanceHandle(),TEXT("SERVER_LIST_ARROW"));

	m_FontBold.CreateFont(-12,0,0,0,600,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
	m_FontUnderLine.CreateFont(-12,0,0,0,300,0,1,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));

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

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CImage ImageBuffer;
	if(ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32) == FALSE)
		return;

	//创建 DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(BufferDC);

	pBufferDC->FillSolidRect(&rcClient,RGB(234,242,247));

	//复制底板
	m_ImageTypeBkgnd.StretchBlt(pBufferDC->GetSafeHdc(), m_rcTypeBkgnd.left, m_rcTypeBkgnd.top, m_rcTypeBkgnd.Width(), m_rcTypeBkgnd.Height(), 
		m_ImageTypeBkgnd.GetWidth() / 3 * m_wIndexBkgnd, 0, m_ImageTypeBkgnd.GetWidth() / 3, m_ImageTypeBkgnd.GetHeight()); 

	//绘制箭头
	CSize SizeArrow(m_ImageArrow1.GetWidth(), m_ImageArrow1.GetHeight());
	m_ImageArrow1.DrawImage(pBufferDC, rcClient.left + 6, rcClient.top + 8, SizeArrow.cx / 2, SizeArrow.cy, m_ImageArrow1.GetWidth() / 2 * (m_bShowLastGame?1:0), 0, SizeArrow.cx / 2, SizeArrow.cy);

	//设置环境
	pBufferDC->SelectObject(m_FontBold);
	pBufferDC->SetBkMode(TRANSPARENT);
	pBufferDC->SetTextColor(RGB(13,61,88));
	pBufferDC->DrawText(_T("最近游戏"), CRect(CPoint(19,3), CSize(rcClient.Width(),m_ImageTypeBkgnd.GetHeight())), DT_LEFT|DT_VCENTER|DT_SINGLELINE);

	// 绘制游戏图标
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
			// 如果鼠标在当前区域内HOVER，则绘制圆角矩形框
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
			// 根据载入的图片显示
			CImage* ImageIcon=NULL;
//			map<WORD, CImage*>::iterator iter_img = m_mapLastGameIcon.find(iter->second->m_GameServer.wKindID);
			map<WORD, CImage*>::iterator iter_img = m_mapLastGameIcon.find((*iter)->m_GameServer.wKindID);
			if (iter_img != m_mapLastGameIcon.end())
			{
				ImageIcon = iter_img->second;
			}
			if (ImageIcon && ImageIcon->IsNull()==false)
			{	
				// 绘制图标
				ImageIcon->BitBlt(pBufferDC->GetSafeHdc(), rcLastGame.left+(rcLastGame.Width()-ImageIcon->GetWidth())/2, rcLastGame.top+3, ImageIcon->GetWidth(), ImageIcon->GetHeight(), 0, 0);
//				ImageIcon->DrawImage(pBufferDC, rcLastGame.left+(rcLastGame.Width()-ImageIcon->GetWidth())/2, rcLastGame.top+3, ImageIcon->GetWidth(), ImageIcon->GetHeight(), 0, 0, ImageIcon->GetWidth(), ImageIcon->GetHeight());
			}
			//绘制房间名字
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

	//绘画背景
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);
}

//绘画背景
BOOL CLastGameServerCtrl::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}

//位置消息
VOID CLastGameServerCtrl::OnSize(UINT nType, INT cx, INT cy)
{

}

//创建消息
INT CLastGameServerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
//	LOGI("CLastGameServerCtrl::OnCreate, size="<<m_mapLastGameServer.size());
	return 0;
}

//鼠标移动
VOID CLastGameServerCtrl::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	// 判断在哪个区域内
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
			// 绘制背景框底色
			InvalidateRect(m_vectRectLastGame.at(i));
			break;
		}
		else
		{
			m_wHoverRectIndex = -1;
			// 绘制背景框底色
			InvalidateRect(m_vectRectLastGame.at(i));
		}
	}

	
}

//鼠标消息
VOID CLastGameServerCtrl::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags, Point);
	// 判断在哪个区域内
	if (m_rcTypeBkgnd.PtInRect(Point))
	{

	}
}
//鼠标消息
VOID CLastGameServerCtrl::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);
	// 判断在哪个区域内
	if (m_rcTypeBkgnd.PtInRect(Point))
	{
		m_wIndexBkgnd = 1;
		InvalidateRect(m_rcTypeBkgnd);

		// 同时隐藏下面的图标栏
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
			// 查询是否可以登录房间
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
			// 已经存在，先删除，之后在头插入
			m_vectGameServerItem.erase(iter);
			break;
		}
	}

	for (size_t i = 0; i < m_vectGameServerItem.size(); i++)
	{
		CGameServerItem* pGameServerItemold = m_vectGameServerItem.at(i);
		int j = 0;
	}
	// 在头插入最新房间
	m_vectGameServerItem.insert(m_vectGameServerItem.begin(), pGameServerItem);

	for (size_t i = 0; i < m_vectGameServerItem.size(); i++)
	{
		CGameServerItem* pGameServerItemold = m_vectGameServerItem.at(i);
		int j = 0;
	}

	// 在游戏目录下载入图片
	map<WORD, CImage*>::iterator iter_img = m_mapLastGameIcon.find(pGameServerItem->m_GameServer.wKindID);
	if (iter_img == m_mapLastGameIcon.end())
	{
		// 得到资源路径
		TCHAR szGameResDirectory[LEN_PROCESS] = _T("");
		CWHService::GetGameResDirectory( pGameServerItem->m_pGameKindItem->m_GameKind.szProcessName, szGameResDirectory, CountArray(szGameResDirectory) );
		TCHAR szWorkDirectory[LEN_PROCESS] = _T("");
		CWHService::GetWorkDirectory( szWorkDirectory, sizeof(szWorkDirectory) );
		TCHAR szImagePath[MAX_PATH] = _T("");
		_sntprintf(szImagePath, sizeof(szImagePath), _T("%s\\%s\\icon.png"), szWorkDirectory, szGameResDirectory);
		// 载入图片
		CImage* pImageIcon = new CImage;
		pImageIcon->Load(szImagePath);
		if (!pImageIcon->IsNull())
		{
			m_mapLastGameIcon.insert(make_pair(pGameServerItem->m_GameServer.wKindID, pImageIcon));
		}
	}
}

// 删除最近玩的游戏
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

// 更新最近玩的游戏数据
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
