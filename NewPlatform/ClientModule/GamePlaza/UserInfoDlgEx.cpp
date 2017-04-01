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

//���캯��
CUserInfoDlgEx::CUserInfoDlgEx() : CDialog(IDD_USER_INFO)
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
CUserInfoDlgEx::~CUserInfoDlgEx()
{
}

//��ʼ������
BOOL CUserInfoDlgEx::OnInitDialog()
{
	__super::OnInitDialog();
    
	HINSTANCE hInstance=AfxGetInstanceHandle();
	LoadRgnImage(hInstance,IDB_USER_INFO_BACK_EX,RGB(255,0,255));

    SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
	SetLayeredWindowAttributes(0,200,LWA_ALPHA); 
     

    
	return TRUE;
}
//����λͼ
bool CUserInfoDlgEx::LoadRgnImage(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTrans)
{
	m_ImageBack.LoadFromResource(hInstance,uBitmapID);
	return CreateControlRgn(crTrans);
}
//��������
bool CUserInfoDlgEx::CreateControlRgn(COLORREF crTrans)
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
//����Ҽ�������Ϣ
void CUserInfoDlgEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);

	//ģ�ⰴ����
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
}
void CUserInfoDlgEx::OnPaint()
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
//	BufferDC.SetTextColor(RGB(17,85,140));
	BufferDC.SetTextColor(RGB(27,39,51));
	BufferDC.SetBkMode(TRANSPARENT);

    CFont newFont;
    newFont.CreateFont(13,0,0,0,FW_NORMAL,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("����"));
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
        //��Ϸ��Ϣ
        LPCTSTR pszGameLevel=m_pIGameLevelParser->GetLevelDescribe(m_pUserData);
        _sntprintf(szText,sizeof(szText),TEXT("��  �� %s"),pszGameLevel);
        BufferDC.TextOut(nXPos,nYPos,szText);

         nYPos+=nYSpace;
        if(m_wServerType==GAME_GENRE_GOLD)
        {
            _sntprintf(szText,sizeof(szText),TEXT("���ӣ� %I64d"),pUserInfo->lScore);
        }
        else if(m_wServerType==GAME_GENRE_MATCH)
        {
            _sntprintf(szText,sizeof(szText),TEXT("�����ң� %I64d"),pUserInfo->lScore);
        }
        else
        {
            _sntprintf(szText,sizeof(szText),TEXT("��  �֣� %I64d"),pUserInfo->lScore);
        }
        BufferDC.TextOut(nXPos,nYPos,szText);

        nYPos+=nYSpace;
        _sntprintf(szText,sizeof(szText),TEXT("��  ���� 0"));
        BufferDC.TextOut(nXPos,nYPos,szText);

        nYPos+=nYSpace;
        _sntprintf(szText,sizeof(szText),TEXT("ǩ  ����%s"),pUserInfo->szUnderWrite);
		CSize sz = pDC->GetTextExtent(szText);
        BufferDC.DrawText(szText,CRect(CPoint(nXPos, nYPos), sz),DT_END_ELLIPSIS | DT_LEFT | DT_WORDBREAK);

        nYPos+= sz.cy+4;
        _sntprintf(szText,sizeof(szText),TEXT("״  ̬��%s"),pUserInfo->szUnderWrite);
        BufferDC.TextOut(nXPos,nYPos,szText);
        
//			nYPos+=nYSpace-3;
// 			nXPos+=3;
//			_sntprintf(szText,sizeof(szText),TEXT("%s"), m_strName);
//			BufferDC.TextOut(nXPos,nYPos,szText);

 		/* ������Ҫ����ͷ��
     //   BufferDC.TextOut(nXPos,nYPos,szText);
        CFaceItemControl * pFaceItemControl=CFaceItemControl::GetInstance();
        tagCustomFaceInfo * pCustomFaceInfo=m_pUserData->GetCustomFaceInfo();
        
		//�滭ͷ��
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
	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//������Դ
	BufferDC.DeleteDC();
	ImageBuffer.Destroy();
	return;
}
//�滭����
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
//ʱ����Ϣ
void CUserInfoDlgEx::OnTimer(UINT nIDEvent)
{
	TRACE("CUserInfoDlgEx::OnTimer\n");

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
