#include "StdAfx.h"
#include "SkinTabCtrl.h"
#include "SkinRenderManager.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinTabCtrl, CTabCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CSkinTabCtrl::CSkinTabCtrl()
{
	//״̬����
	m_bRenderImage=false;

	//���ñ���
	m_crBackGround=RGB(194,218,230);
	m_crControlBorder=RGB(131,192,219);

	m_FontBold.CreateFont(-12,0,0,0,600,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("����"));
	m_FontNormal.CreateFont(-12,0,0,0,300,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("����"));

	return;
}

//��������
CSkinTabCtrl::~CSkinTabCtrl()
{
	m_FontBold.DeleteObject(); 
}

//������Ⱦ
VOID CSkinTabCtrl::SetRenderImage(bool bRenderImage)
{
	//���ñ���
	m_bRenderImage=bRenderImage;

	//���´���
	if (m_hWnd!=NULL)
	{
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//������ɫ
VOID CSkinTabCtrl::SetTabCtrlColor(COLORREF crBackGround, COLORREF crControlBorder)
{
	//���ñ���
	m_crBackGround=crBackGround;
	m_crControlBorder=crControlBorder;

	//���½���
	if (m_hWnd!=NULL)
	{
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//�ػ���Ϣ
VOID CSkinTabCtrl::OnPaint()
{
	//��������
	CPaintDC dc(this);

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//�滭����
	CRect rcClip;
	dc.GetClipBox(&rcClip);
	dc.FillSolidRect(&rcClip, RGB(255, 255, 255));

	//���� DC
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());
	
	//�滭�߿�
	CRect rcFrame = rcClient;
	rcFrame.top+=26;
	dc.Draw3dRect(&rcFrame,m_crControlBorder,m_crControlBorder);

	dc.GetClipBox(&rcClip);
	CRect rcHead = rcClip;
	rcHead.bottom = 26;
	dc.FillSolidRect(&rcHead, m_crBackGround);

	//�滭��ť
	CPen LinePen(PS_SOLID,1,m_crControlBorder);
	CPen * pOldPen=dc.SelectObject(&LinePen);

	for (INT i=0;i<GetItemCount();i++)
	{
		//��������
		TCITEM ItemInfo;
		ZeroMemory(&ItemInfo,sizeof(ItemInfo));

		//��������
		TCHAR szBuffer[100];
		ItemInfo.mask=TCIF_TEXT|TCIF_IMAGE;
		ItemInfo.pszText=szBuffer;
		ItemInfo.cchTextMax=sizeof(szBuffer);

		//��ȡ��Ϣ
		CRect rcItem;
		GetItem(i,&ItemInfo);
		GetItemRect(i,&rcItem);

		CRect rcText = rcItem;
		rcText.top += 3;
		rcText.left += 1;
		rcText.right -= 1;

		//�滭�߿�
		if (i!=GetCurSel())
		{
			//��䱳��
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
		//�滭�߿�
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

	//������Դ
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
