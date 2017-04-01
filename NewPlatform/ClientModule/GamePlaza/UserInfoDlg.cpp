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

//���캯��
CUserInfoDlg::CUserInfoDlg() : CDialog(IDD_USER_INFO)
{
    m_pUserData=NULL;
    m_wServerType=GAME_GENRE_GOLD;
    //Ĭ������
	m_MapMemberDescribe[0]=TEXT("��ͨ��Ա");
	m_MapMemberDescribe[1]=TEXT("�����Ա");
	m_MapMemberDescribe[2]=TEXT("�����Ա");
	m_MapMemberDescribe[3]=TEXT("�����Ա");
	m_MapMemberDescribe[4]=TEXT("�����Ա");
	m_MapMemberDescribe[5]=TEXT("VIP��Ա");
}

//��������
CUserInfoDlg::~CUserInfoDlg()
{
}

//��ʼ������
BOOL CUserInfoDlg::OnInitDialog()
{
	__super::OnInitDialog();
    
	HINSTANCE hInstance=AfxGetInstanceHandle();
	LoadRgnImage(hInstance,IDB_USER_INFO_BACK,RGB(255,0,255));

    SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
	SetLayeredWindowAttributes(0,200,LWA_ALPHA); 
     

    
	return TRUE;
}
//����λͼ
bool CUserInfoDlg::LoadRgnImage(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTrans)
{
	m_ImageBack.LoadFromResource(hInstance,IDB_USER_INFO_BACK);
	return CreateControlRgn(crTrans);
}
//��������
bool CUserInfoDlg::CreateControlRgn(COLORREF crTrans)
{
	//��������
	CRgn RgnControl;
	m_ImageBack.CreateImageRegion(RgnControl,crTrans);

	//���ô���
	if (RgnControl.GetSafeHandle()!=NULL)
	{
		//�ƶ�����
		SetWindowRgn(RgnControl,TRUE);
		SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE);

		//ɾ������
		m_DialogRgn.DeleteObject();
		m_DialogRgn.Attach(RgnControl.Detach());

		return true;
	}

	return false;
}

//������Ϣ
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
//����Ҽ�������Ϣ
void CUserInfoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);

	//ģ�ⰴ����
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
}
void CUserInfoDlg::OnPaint()
{
	CPaintDC dc(this);

	//�滭��ͼ
	CDC* pDC=&dc;

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),24);

	//���� DC
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(pDC);
	BufferDC.SelectObject(ImageBuffer);

	//�滭����
	m_ImageBack.TransDrawImage(&BufferDC,0,0,RGB(255,0,255));

	//���� DC
	BufferDC.SetTextColor(RGB(6,52,86));
	BufferDC.SetBkMode(TRANSPARENT);

    CFont newFont;
    newFont.CreateFont(14,0,0,0,FW_NORMAL,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("����"));
    CFont * pOldFont=BufferDC.SelectObject(&newFont);
	
	TCHAR szText[256]={0};
	if(m_pUserData!=NULL)
	{
		tagUserInfo* pUserInfo = m_pUserData->GetUserInfo();
        int nXPos=100;
        int nYPos=18;
        int nYSpace=20;
        _sntprintf(szText,sizeof(szText),TEXT("�ǳ�:%s"),pUserInfo->szNickName);
        BufferDC.TextOut(nXPos,nYPos,szText);
        nYPos+=nYSpace;
        _sntprintf(szText,sizeof(szText),TEXT("I  D:%ld"),pUserInfo->dwGameID);
        BufferDC.TextOut(nXPos,nYPos,szText);
        nYPos+=nYSpace;
        LPCTSTR pszGameLevel=m_pIGameLevelParser->GetLevelDescribe(m_pUserData);
        _sntprintf(szText,sizeof(szText),TEXT("����:%s"),pszGameLevel);
        BufferDC.TextOut(nXPos,nYPos,szText);
        nYPos+=nYSpace;
        if(m_wServerType==GAME_GENRE_GOLD)
        {
            _sntprintf(szText,sizeof(szText),TEXT("����:%I64d"),pUserInfo->lScore);
        }
        else if(m_wServerType==GAME_GENRE_MATCH)
        {
            _sntprintf(szText,sizeof(szText),TEXT("������:%I64d"),pUserInfo->lScore);
        }
        else
        {
            _sntprintf(szText,sizeof(szText),TEXT("����:%I64d"),pUserInfo->lScore);
        }
        BufferDC.TextOut(nXPos,nYPos,szText);
        nYPos+=nYSpace;
        if(pUserInfo->cbMemberOrder<1 || pUserInfo->cbMemberOrder>5)
        {
            pUserInfo->cbMemberOrder=0;
        }
        _sntprintf(szText,sizeof(szText),TEXT("��Ա:%s"),m_MapMemberDescribe[pUserInfo->cbMemberOrder]);
        BufferDC.TextOut(nXPos,nYPos,szText);
        BufferDC.SetTextColor(RGB(255,0,0));
        nYPos+=nYSpace;
        _sntprintf(szText,sizeof(szText),TEXT("����:%ld"),pUserInfo->lLoveLiness);
        BufferDC.TextOut(nXPos,nYPos,szText);
        BufferDC.SetTextColor(RGB(20,78,38));
        nYPos+=nYSpace;
        switch(pUserInfo->cbUserStatus)
        {
        case US_FREE:
            {
                _sntprintf(szText,sizeof(szText),TEXT("״̬:%s"),TEXT("����"));
                break;
            }
        case US_SIT:
            {
                _sntprintf(szText,sizeof(szText),TEXT("״̬:�ڵ�%d������"),pUserInfo->wTableID+1);
                break;
            }
        case US_READY:
            {
                _sntprintf(szText,sizeof(szText),TEXT("״̬:�ڵ�%d��׼��"),pUserInfo->wTableID+1);
                break;
            }
        case US_PLAYING:
            {
                _sntprintf(szText,sizeof(szText),TEXT("״̬:�ڵ�%d����Ϸ��"),pUserInfo->wTableID+1);
                break;
            }
        case US_OFFLINE:
            {
                _sntprintf(szText,sizeof(szText),TEXT("״̬:%s"),TEXT("����"));
                break;
            }
        case US_LOOKON:
            {
                _sntprintf(szText,sizeof(szText),TEXT("״̬:�ڵ�%d���Թ�"),pUserInfo->wTableID+1);
                break;
            }
        default:
            {
                _sntprintf(szText,sizeof(szText),TEXT("״̬:%s"),TEXT("����"));
            }
        }
        BufferDC.TextOut(nXPos,nYPos,szText);
        
        BufferDC.SetTextColor(RGB(35,83,145));
        CRect rcPhotoFram(12,160,12+333,160+35);
        _sntprintf(szText,sizeof(szText),TEXT("ǩ��:%s"),pUserInfo->szUnderWrite);
        BufferDC.DrawText(szText,rcPhotoFram,DT_END_ELLIPSIS | DT_LEFT | DT_WORDBREAK);

        
        BufferDC.SetTextColor(RGB(6,52,86));
        //��Ϸ��Ϣ
		LONG lPlayCount=m_pUserData->GetUserPlayCount();
		float fUserWinRate=m_pUserData->GetUserWinRate();
        nXPos=12;
        nYPos=210;
        _sntprintf(szText,sizeof(szText),TEXT("�ܾ�:%ld"),lPlayCount);
        BufferDC.TextOut(nXPos,nYPos,szText);
        nXPos=125;
        nYPos=210;
        _sntprintf(szText,sizeof(szText),TEXT("ʤ��:%.2f%%"),fUserWinRate);
        BufferDC.TextOut(nXPos,nYPos,szText);
        
        nXPos=12;
        nYPos=230;
        _sntprintf(szText,sizeof(szText),TEXT("ʤ��:%ld"),pUserInfo->dwWinCount);
        BufferDC.TextOut(nXPos,nYPos,szText);

        nXPos=125;
        nYPos=230;
        _sntprintf(szText,sizeof(szText),TEXT("���:%ld"),pUserInfo->dwLostCount);
        BufferDC.TextOut(nXPos,nYPos,szText);
        nXPos=246;
        nYPos=230;
        _sntprintf(szText,sizeof(szText),TEXT("�;�:%ld"),pUserInfo->dwDrawCount);
        
        BufferDC.TextOut(nXPos,nYPos,szText);
        CFaceItemControl * pFaceItemControl=CFaceItemControl::GetInstance();
        tagCustomFaceInfo * pCustomFaceInfo=m_pUserData->GetCustomFaceInfo();
        
		//�滭ͷ��
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
	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//������Դ
	BufferDC.DeleteDC();
	ImageBuffer.Destroy();
	return;
}
//�滭����
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
//ʱ����Ϣ
void CUserInfoDlg::OnTimer(UINT nIDEvent)
{
	TRACE("CUserInfoDlg::OnTimer\n");

	//ʱ����Ϣ
	switch(nIDEvent)
	{
	case 1:		//���½���
		{
			if (m_hWnd!=NULL)
			{
				KillTimer(1);
				//���½���
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
