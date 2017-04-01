#include "StdAfx.h"
#include "SkinTabCtrl.h"
#include "SkinRenderManager.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinTabCtrl, CTabCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CSkinTabCtrl::CSkinTabCtrl()
{
	//状态变量
	m_bRenderImage=false;

	//设置变量
	m_crBackGround=RGB(194,218,230);
	m_crControlBorder=RGB(131,192,219);

	m_FontBold.CreateFont(-12,0,0,0,600,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
	m_FontNormal.CreateFont(-12,0,0,0,300,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));

	return;
}

//析够函数
CSkinTabCtrl::~CSkinTabCtrl()
{
	m_FontBold.DeleteObject(); 
}

//设置渲染
VOID CSkinTabCtrl::SetRenderImage(bool bRenderImage)
{
	//设置变量
	m_bRenderImage=bRenderImage;

	//更新窗口
	if (m_hWnd!=NULL)
	{
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//设置颜色
VOID CSkinTabCtrl::SetTabCtrlColor(COLORREF crBackGround, COLORREF crControlBorder)
{
	//设置变量
	m_crBackGround=crBackGround;
	m_crControlBorder=crControlBorder;

	//更新界面
	if (m_hWnd!=NULL)
	{
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//重画消息
VOID CSkinTabCtrl::OnPaint()
{
	//变量定义
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//绘画背景
	CRect rcClip;
	dc.GetClipBox(&rcClip);
	dc.FillSolidRect(&rcClip, RGB(255, 255, 255));

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());
	
	//绘画边框
	CRect rcFrame = rcClient;
	rcFrame.top+=26;
	dc.Draw3dRect(&rcFrame,m_crControlBorder,m_crControlBorder);

	dc.GetClipBox(&rcClip);
	CRect rcHead = rcClip;
	rcHead.bottom = 26;
	dc.FillSolidRect(&rcHead, m_crBackGround);

	//绘画按钮
	CPen LinePen(PS_SOLID,1,m_crControlBorder);
	CPen * pOldPen=dc.SelectObject(&LinePen);

	for (INT i=0;i<GetItemCount();i++)
	{
		//变量定义
		TCITEM ItemInfo;
		ZeroMemory(&ItemInfo,sizeof(ItemInfo));

		//变量定义
		TCHAR szBuffer[100];
		ItemInfo.mask=TCIF_TEXT|TCIF_IMAGE;
		ItemInfo.pszText=szBuffer;
		ItemInfo.cchTextMax=sizeof(szBuffer);

		//获取信息
		CRect rcItem;
		GetItem(i,&ItemInfo);
		GetItemRect(i,&rcItem);

		CRect rcText = rcItem;
		rcText.top += 3;
		rcText.left += 1;
		rcText.right -= 1;

		//绘画边框
		if (i!=GetCurSel())
		{
			//填充背景
			dc.SetTextColor(RGB(63,64,129));
			//dc.MoveTo(rcItem.left,rcItem.bottom-1);
			//dc.LineTo(rcItem.right,rcItem.bottom-1);
			dc.SelectObject(&m_FontNormal);
			dc.FillSolidRect(&rcText,RGB(221,244,255));
			DrawText(dc,szBuffer,lstrlen(szBuffer),&rcText,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
		}
		else 
		{
			dc.SetTextColor(RGB(0,0,0));
			dc.SelectObject(&m_FontBold);
			dc.FillSolidRect(&rcText,RGB(255,255,255));
			DrawText(dc,szBuffer,lstrlen(szBuffer),&rcText,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
		}

		rcItem.top += 2;
		//绘画边框
		dc.MoveTo(rcItem.left,rcItem.bottom-1);
		dc.LineTo(rcItem.left,rcItem.top+2);
		dc.LineTo(rcItem.left+2,rcItem.top);
		dc.LineTo(rcItem.right-2,rcItem.top);
		dc.LineTo(rcItem.right,rcItem.top+2);
		dc.LineTo(rcItem.right,rcItem.bottom-1);

		if (i!=GetCurSel())
		{
			dc.LineTo(rcItem.left,rcItem.bottom-1);
		}
	}

	//清理资源
	dc.SelectObject(pOldPen);
	LinePen.DeleteObject();

	CPen WhiteLinePen(PS_SOLID,1,RGB(250,250,250));
	dc.SelectObject(&WhiteLinePen);

	dc.MoveTo(rcClient.left + 1, rcClient.bottom - 1);
	dc.LineTo(rcClient.right - 1, rcClient.bottom - 1);

	WhiteLinePen.DeleteObject();

	return;
}

//////////////////////////////////////////////////////////////////////////////////
