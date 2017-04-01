#include "Stdafx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "UserInfoDlgEx.h"
#include "ServerViewItem.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CUserInfoDlgEx, CDialog)

BEGIN_MESSAGE_MAP(CUserInfoDlgEx, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS() 
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CUserInfoDlgEx::CUserInfoDlgEx() : CDialog(IDD_USER_INFO)
{
    m_pUserData=NULL;
    m_wServerType=GAME_GENRE_GOLD;
    //默认设置
	m_MapMemberDescribe[0]=TEXT("普通会员");
	m_MapMemberDescribe[1]=TEXT("蓝钻会员");
	m_MapMemberDescribe[2]=TEXT("黄钻会员");
	m_MapMemberDescribe[3]=TEXT("白钻会员");
	m_MapMemberDescribe[4]=TEXT("红钻会员");
	m_MapMemberDescribe[5]=TEXT("VIP会员");
}

//析构函数
CUserInfoDlgEx::~CUserInfoDlgEx()
{
}

//初始化函数
BOOL CUserInfoDlgEx::OnInitDialog()
{
	__super::OnInitDialog();
    
	HINSTANCE hInstance=AfxGetInstanceHandle();
	LoadRgnImage(hInstance,IDB_USER_INFO_BACK_EX,RGB(255,0,255));

    SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
	SetLayeredWindowAttributes(0,200,LWA_ALPHA); 
     

    
	return TRUE;
}
//加载位图
bool CUserInfoDlgEx::LoadRgnImage(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTrans)
{
	m_ImageBack.LoadFromResource(hInstance,uBitmapID);
	return CreateControlRgn(crTrans);
}
//创建区域
bool CUserInfoDlgEx::CreateControlRgn(COLORREF crTrans)
{
	//创建区域
	CRgn RgnControl;
	m_ImageBack.CreateImageRegion(RgnControl,crTrans);

	//设置窗口
	if (RgnControl.GetSafeHandle()!=NULL)
	{
		//移动窗口
		SetWindowRgn(RgnControl,TRUE);
		SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE);

		//删除区域
		m_DialogRgn.DeleteObject();
		m_DialogRgn.Attach(RgnControl.Detach());

		return true;
	}

	return false;
}

//更新信息
VOID CUserInfoDlgEx::UpdateUserInfo(IClientUserItem* pUserData, CString strName)
{
	m_pUserData=pUserData;
	m_strName=strName;
    if(m_pUserData!=NULL)
    {
        Invalidate(FALSE);
	    ShowWindow(SW_SHOW);
		SetForegroundWindow();
    }
    else
    {
        ShowWindow(SW_HIDE);
    }
	return;
}
//鼠标右键按下消息
void CUserInfoDlgEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);

	//模拟按标题
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
}
void CUserInfoDlgEx::OnPaint()
{
	CPaintDC dc(this);

	//绘画底图
	CDC* pDC=&dc;

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),24);

	//创建 DC
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(pDC);
	BufferDC.SelectObject(ImageBuffer);

	//绘画背景
	m_ImageBack.TransDrawImage(&BufferDC,0,0,RGB(255,0,255));

	//设置 DC
//	BufferDC.SetTextColor(RGB(17,85,140));
	BufferDC.SetTextColor(RGB(27,39,51));
	BufferDC.SetBkMode(TRANSPARENT);

    CFont newFont;
    newFont.CreateFont(13,0,0,0,FW_NORMAL,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
    CFont * pOldFont=BufferDC.SelectObject(&newFont);
	
	TCHAR szText[256]={0};
	if(m_pUserData!=NULL)
	{
		tagUserInfo* pUserInfo = m_pUserData->GetUserInfo();
        int nXPos=9;
        int nYPos=7;

        int nYSpace=22;
        _sntprintf(szText,sizeof(szText),TEXT("%s"),pUserInfo->szNickName);
        BufferDC.TextOut(nXPos,nYPos,szText);

         nYPos+=nYSpace;
        //游戏信息
        LPCTSTR pszGameLevel=m_pIGameLevelParser->GetLevelDescribe(m_pUserData);
        _sntprintf(szText,sizeof(szText),TEXT("级  别： %s"),pszGameLevel);
        BufferDC.TextOut(nXPos,nYPos,szText);

         nYPos+=nYSpace;
        if(m_wServerType==GAME_GENRE_GOLD)
        {
            _sntprintf(szText,sizeof(szText),TEXT("银子： %I64d"),pUserInfo->lScore);
        }
        else if(m_wServerType==GAME_GENRE_MATCH)
        {
            _sntprintf(szText,sizeof(szText),TEXT("比赛币： %I64d"),pUserInfo->lScore);
        }
        else
        {
            _sntprintf(szText,sizeof(szText),TEXT("积  分： %I64d"),pUserInfo->lScore);
        }
        BufferDC.TextOut(nXPos,nYPos,szText);

        nYPos+=nYSpace;
        _sntprintf(szText,sizeof(szText),TEXT("成  绩： 0"));
        BufferDC.TextOut(nXPos,nYPos,szText);

        nYPos+=nYSpace;
        _sntprintf(szText,sizeof(szText),TEXT("签  名：%s"),pUserInfo->szUnderWrite);
		CSize sz = pDC->GetTextExtent(szText);
        BufferDC.DrawText(szText,CRect(CPoint(nXPos, nYPos), sz),DT_END_ELLIPSIS | DT_LEFT | DT_WORDBREAK);

        nYPos+= sz.cy+4;
        _sntprintf(szText,sizeof(szText),TEXT("状  态：%s"),pUserInfo->szUnderWrite);
        BufferDC.TextOut(nXPos,nYPos,szText);
        
//			nYPos+=nYSpace-3;
// 			nXPos+=3;
//			_sntprintf(szText,sizeof(szText),TEXT("%s"), m_strName);
//			BufferDC.TextOut(nXPos,nYPos,szText);

 		/* 不再需要绘制头像
     //   BufferDC.TextOut(nXPos,nYPos,szText);
        CFaceItemControl * pFaceItemControl=CFaceItemControl::GetInstance();
        tagCustomFaceInfo * pCustomFaceInfo=m_pUserData->GetCustomFaceInfo();
        
		//绘画头像
		if ((pCustomFaceInfo->dwDataSize!=0L)&&(m_pUserData->GetCustomID()!=0L))
		{
			pFaceItemControl->DrawFaceNormal(&BufferDC,12,14,pCustomFaceInfo->dwCustomFace);
		}
		else
		{
			pFaceItemControl->DrawFaceNormal(&BufferDC,12,14,m_pUserData->GetFaceID());
		}*/
	}
    BufferDC.SelectObject(pOldFont);
    newFont.DeleteObject();
	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	ImageBuffer.Destroy();
	return;
}
//绘画背景
BOOL CUserInfoDlgEx::OnEraseBkgnd(CDC * pDC)
{
	return __super::OnEraseBkgnd(pDC);
}
void CUserInfoDlgEx::OnKillFocus( CWnd* pNewWnd )
{
	if (m_hWnd!=NULL && IsWindowVisible()==TRUE)
	{
		m_pUserData=NULL;
		SetTimer(1,100,NULL);
	}
	return __super::OnKillFocus(pNewWnd);
}
//时间消息
void CUserInfoDlgEx::OnTimer(UINT nIDEvent)
{
	TRACE("CUserInfoDlgEx::OnTimer\n");

	//时间消息
	switch(nIDEvent)
	{
	case 1:		//更新界面
		{
			if (m_hWnd!=NULL)
			{
				KillTimer(1);
				//更新界面
				if (m_pUserData==NULL)
				{
					UpdateUserInfo(NULL);
				}
			}
			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////
