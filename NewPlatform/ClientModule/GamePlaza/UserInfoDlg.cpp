#include "Stdafx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "UserInfoDlg.h"
#include "ServerViewItem.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CUserInfoDlg, CDialog)

BEGIN_MESSAGE_MAP(CUserInfoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS() 
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CUserInfoDlg::CUserInfoDlg() : CDialog(IDD_USER_INFO)
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
CUserInfoDlg::~CUserInfoDlg()
{
}

//初始化函数
BOOL CUserInfoDlg::OnInitDialog()
{
	__super::OnInitDialog();
    
	HINSTANCE hInstance=AfxGetInstanceHandle();
	LoadRgnImage(hInstance,IDB_USER_INFO_BACK,RGB(255,0,255));

    SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
	SetLayeredWindowAttributes(0,200,LWA_ALPHA); 
     

    
	return TRUE;
}
//加载位图
bool CUserInfoDlg::LoadRgnImage(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTrans)
{
	m_ImageBack.LoadFromResource(hInstance,IDB_USER_INFO_BACK);
	return CreateControlRgn(crTrans);
}
//创建区域
bool CUserInfoDlg::CreateControlRgn(COLORREF crTrans)
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
VOID CUserInfoDlg::UpdateUserInfo(IClientUserItem* pUserData)
{
	m_pUserData=pUserData;
    if(m_pUserData!=NULL)
    {
        Invalidate(TRUE);
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
void CUserInfoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);

	//模拟按标题
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
}
void CUserInfoDlg::OnPaint()
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
	BufferDC.SetTextColor(RGB(6,52,86));
	BufferDC.SetBkMode(TRANSPARENT);

    CFont newFont;
    newFont.CreateFont(14,0,0,0,FW_NORMAL,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("宋体"));
    CFont * pOldFont=BufferDC.SelectObject(&newFont);
	
	TCHAR szText[256]={0};
	if(m_pUserData!=NULL)
	{
		tagUserInfo* pUserInfo = m_pUserData->GetUserInfo();
        int nXPos=100;
        int nYPos=18;
        int nYSpace=20;
        _sntprintf(szText,sizeof(szText),TEXT("昵称:%s"),pUserInfo->szNickName);
        BufferDC.TextOut(nXPos,nYPos,szText);
        nYPos+=nYSpace;
        _sntprintf(szText,sizeof(szText),TEXT("I  D:%ld"),pUserInfo->dwGameID);
        BufferDC.TextOut(nXPos,nYPos,szText);
        nYPos+=nYSpace;
        LPCTSTR pszGameLevel=m_pIGameLevelParser->GetLevelDescribe(m_pUserData);
        _sntprintf(szText,sizeof(szText),TEXT("级别:%s"),pszGameLevel);
        BufferDC.TextOut(nXPos,nYPos,szText);
        nYPos+=nYSpace;
        if(m_wServerType==GAME_GENRE_GOLD)
        {
            _sntprintf(szText,sizeof(szText),TEXT("银子:%I64d"),pUserInfo->lScore);
        }
        else if(m_wServerType==GAME_GENRE_MATCH)
        {
            _sntprintf(szText,sizeof(szText),TEXT("比赛币:%I64d"),pUserInfo->lScore);
        }
        else
        {
            _sntprintf(szText,sizeof(szText),TEXT("积分:%I64d"),pUserInfo->lScore);
        }
        BufferDC.TextOut(nXPos,nYPos,szText);
        nYPos+=nYSpace;
        if(pUserInfo->cbMemberOrder<1 || pUserInfo->cbMemberOrder>5)
        {
            pUserInfo->cbMemberOrder=0;
        }
        _sntprintf(szText,sizeof(szText),TEXT("会员:%s"),m_MapMemberDescribe[pUserInfo->cbMemberOrder]);
        BufferDC.TextOut(nXPos,nYPos,szText);
        BufferDC.SetTextColor(RGB(255,0,0));
        nYPos+=nYSpace;
        _sntprintf(szText,sizeof(szText),TEXT("魅力:%ld"),pUserInfo->lLoveLiness);
        BufferDC.TextOut(nXPos,nYPos,szText);
        BufferDC.SetTextColor(RGB(20,78,38));
        nYPos+=nYSpace;
        switch(pUserInfo->cbUserStatus)
        {
        case US_FREE:
            {
                _sntprintf(szText,sizeof(szText),TEXT("状态:%s"),TEXT("空闲"));
                break;
            }
        case US_SIT:
            {
                _sntprintf(szText,sizeof(szText),TEXT("状态:在第%d桌坐着"),pUserInfo->wTableID+1);
                break;
            }
        case US_READY:
            {
                _sntprintf(szText,sizeof(szText),TEXT("状态:在第%d桌准备"),pUserInfo->wTableID+1);
                break;
            }
        case US_PLAYING:
            {
                _sntprintf(szText,sizeof(szText),TEXT("状态:在第%d桌游戏中"),pUserInfo->wTableID+1);
                break;
            }
        case US_OFFLINE:
            {
                _sntprintf(szText,sizeof(szText),TEXT("状态:%s"),TEXT("断线"));
                break;
            }
        case US_LOOKON:
            {
                _sntprintf(szText,sizeof(szText),TEXT("状态:在第%d桌旁观"),pUserInfo->wTableID+1);
                break;
            }
        default:
            {
                _sntprintf(szText,sizeof(szText),TEXT("状态:%s"),TEXT("空闲"));
            }
        }
        BufferDC.TextOut(nXPos,nYPos,szText);
        
        BufferDC.SetTextColor(RGB(35,83,145));
        CRect rcPhotoFram(12,160,12+333,160+35);
        _sntprintf(szText,sizeof(szText),TEXT("签名:%s"),pUserInfo->szUnderWrite);
        BufferDC.DrawText(szText,rcPhotoFram,DT_END_ELLIPSIS | DT_LEFT | DT_WORDBREAK);

        
        BufferDC.SetTextColor(RGB(6,52,86));
        //游戏信息
		LONG lPlayCount=m_pUserData->GetUserPlayCount();
		float fUserWinRate=m_pUserData->GetUserWinRate();
        nXPos=12;
        nYPos=210;
        _sntprintf(szText,sizeof(szText),TEXT("总局:%ld"),lPlayCount);
        BufferDC.TextOut(nXPos,nYPos,szText);
        nXPos=125;
        nYPos=210;
        _sntprintf(szText,sizeof(szText),TEXT("胜率:%.2f%%"),fUserWinRate);
        BufferDC.TextOut(nXPos,nYPos,szText);
        
        nXPos=12;
        nYPos=230;
        _sntprintf(szText,sizeof(szText),TEXT("胜局:%ld"),pUserInfo->dwWinCount);
        BufferDC.TextOut(nXPos,nYPos,szText);

        nXPos=125;
        nYPos=230;
        _sntprintf(szText,sizeof(szText),TEXT("输局:%ld"),pUserInfo->dwLostCount);
        BufferDC.TextOut(nXPos,nYPos,szText);
        nXPos=246;
        nYPos=230;
        _sntprintf(szText,sizeof(szText),TEXT("和局:%ld"),pUserInfo->dwDrawCount);
        
        BufferDC.TextOut(nXPos,nYPos,szText);
        CFaceItemControl * pFaceItemControl=CFaceItemControl::GetInstance();
        tagCustomFaceInfo * pCustomFaceInfo=m_pUserData->GetCustomFaceInfo();
        
		//绘画头像
		if ((pCustomFaceInfo->dwDataSize!=0L)&&(m_pUserData->GetCustomID()!=0L))
		{
			pFaceItemControl->DrawFaceNormal(&BufferDC,24,30,pCustomFaceInfo->dwCustomFace);
		}
		else
		{
			pFaceItemControl->DrawFaceNormal(&BufferDC,24,30,m_pUserData->GetFaceID());
		}
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
BOOL CUserInfoDlg::OnEraseBkgnd(CDC * pDC)
{
	return __super::OnEraseBkgnd(pDC);
}
void CUserInfoDlg::OnKillFocus( CWnd* pNewWnd )
{
	if (m_hWnd!=NULL && IsWindowVisible()==TRUE)
	{
		m_pUserData=NULL;
		SetTimer(1,100,NULL);
	}
	return __super::OnKillFocus(pNewWnd);
}
//时间消息
void CUserInfoDlg::OnTimer(UINT nIDEvent)
{
	TRACE("CUserInfoDlg::OnTimer\n");

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
