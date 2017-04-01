#include "Stdafx.h"
#include "GamePlaza.h"
#include "DlgService.h"
#include "DlgCustomFace.h"
#include "PlatformFrame.h"
#include "DlgLockMachine.h"
#include "WndUserInfoCtrl.h"
#include "DlgJyMx.h"

//////////////////////////////////////////////////////////////////////////////////

//���ư�ť
#define IDC_USER_INFO_1				100									//�û���ť
#define IDC_USER_INFO_2				101									//�û���ť
#define IDC_USER_INFO_3				102									//�û���ť
#define IDC_USER_INFO_4				103									//�û���ť
#define IDC_USER_INFO_5				104									//�û���ť
#define IDC_USER_INFO_6				105									//�û���ť

#define IDC_USER_INFO_SET			106									//�û���ť
#define IDC_USER_INFO_EDIT			107									//�༭�ؼ�
#define IDC_USER_INFO_COPY			108
#define IDC_SYSTEM_SET				109									// ϵͳ����

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CWndUserInfoCtrl, CWnd)

	//ϵͳ��Ϣ
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()

	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()

	//�Զ���Ϣ
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_PLATFORM_EVENT, OnMessagePlatformEvent)

END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CEditUnderWrite, CSkinEditEx)
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CEditUnderWrite::CEditUnderWrite()
{
	//���ñ���
	m_bNeedSend=false;
	m_pParentWnd=NULL;
	ZeroMemory(m_szUnderWrite,sizeof(m_szUnderWrite));

	//���ÿؼ�
	m_MissionManager.InsertMissionItem(this);

	return;
}

//��������
CEditUnderWrite::~CEditUnderWrite()
{
}

//��Ϣ����
BOOL CEditUnderWrite::PreTranslateMessage(MSG * pMsg)
{
	//ȡ����Ϣ
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		//�رտؼ�
		CloseUnderWrite();

		return TRUE;
	}

	//ȷ����Ϣ
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_RETURN))
	{
		//���ý���
		ASSERT(m_pParentWnd!=NULL);
		if (m_pParentWnd!=NULL) m_pParentWnd->SetFocus();

		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//�����¼�
bool CEditUnderWrite::OnEventMissionLink(INT nErrorCode)
{
	//�����ж�
	if (nErrorCode!=0L)
	{
		//������ʾ
		CInformation Information(m_pParentWnd);
		Information.ShowMessageBox(TEXT("����������ʧ�ܣ�����ǩ������ʧ�ܣ�"),MB_ICONERROR,30L);

		return true;
	}

	//���ñ���
	m_bNeedSend=false;

	//��������
	ASSERT(GetMissionManager()!=NULL);
	CMissionManager * pMissionManager=GetMissionManager();

	//�û���Ϣ
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//��������
	CMD_GP_ModifyUnderWrite ModifyUnderWrite;
	ZeroMemory(&ModifyUnderWrite,sizeof(ModifyUnderWrite));

	//��������
	ModifyUnderWrite.dwUserID=pGlobalUserData->dwUserID;
	lstrcpyn(ModifyUnderWrite.szUnderWrite,m_szUnderWrite,CountArray(ModifyUnderWrite.szUnderWrite));
//	lstrcpyn(ModifyUnderWrite.szPassword,pGlobalUserData->szPassword,CountArray(ModifyUnderWrite.szPassword));

	//��������
	WORD wHeadSize=sizeof(ModifyUnderWrite)-sizeof(ModifyUnderWrite.szUnderWrite);
	pMissionManager->SendData(MDM_GP_USER_SERVICE,SUB_GP_MODIFY_UNDER_WRITE,&ModifyUnderWrite,wHeadSize+CountStringBuffer(ModifyUnderWrite.szUnderWrite));

	return true;
}

//�ر��¼�
bool CEditUnderWrite::OnEventMissionShut(BYTE cbShutReason)
{
	//�رմ���
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//��������
		CMissionManager * pMissionManager=GetMissionManager();
		if (pMissionManager->AvtiveMissionItem(this,true)==true) return true;

		//���ÿؼ�
		CloseUnderWrite();

		//��ʾ��ʾ
		CInformation Information(m_pParentWnd);
		Information.ShowMessageBox(TEXT("���ڵ�ǰ����������æ������ǩ���޸�ʧ�ܣ����Ժ������ԣ�"),MB_ICONERROR);
	}

	return true;
}

//��ȡ�¼�
bool CEditUnderWrite::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//�����
	if (Command.wMainCmdID==MDM_GP_USER_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_OPERATE_FAILURE:	//����ʧ��
			{
				//Ч�����
				CMD_GP_OperateFailure * pOperateFailure=(CMD_GP_OperateFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString))) return false;

				//�ر�����
				CMissionManager * pMissionManager=GetMissionManager();
				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

				//���ÿؼ�
				CloseUnderWrite();

				//��ʾ��Ϣ
				if (pOperateFailure->szDescribeString[0]!=0)
				{
					CInformation Information(m_pParentWnd);
					Information.ShowMessageBox(pOperateFailure->szDescribeString,MB_ICONERROR,60);
				}

				return true;
			}
		case SUB_GP_OPERATE_SUCCESS:	//�����ɹ�
			{
				//��������
				CMD_GP_OperateSuccess * pOperateSuccess=(CMD_GP_OperateSuccess *)pData;

				//Ч������
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString))) return false;

				//�ر�����
				CMissionManager * pMissionManager=GetMissionManager();
				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

				//������Ϣ
				CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
				tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
				lstrcpyn(pGlobalUserData->szUnderWrite,m_szUnderWrite,CountArray(pGlobalUserData->szUnderWrite));

				//���ÿؼ�
				CloseUnderWrite();

				//�����¼�
				CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
				if (pPlatformEvent!=NULL) pPlatformEvent->SendPlatformEvent(EVENT_USER_INFO_UPDATE,0L);

				//��ʾ��Ϣ
				if (pOperateSuccess->szDescribeString[0]!=0)
				{
					CInformation Information(m_pParentWnd);
					Information.ShowMessageBox(pOperateSuccess->szDescribeString,MB_ICONINFORMATION,60);
				}

				return true;
			}
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//�޸��ж�
bool CEditUnderWrite::IsModifyStatus()
{
	//״̬�ж�
	if (m_bNeedSend==true) return true;
	if (GetActiveStatus()==true) return true;

	return false;
}

//���ô���
VOID CEditUnderWrite::SetParentWindow(CWnd * pParentWnd)
{
	//���ñ���
	m_pParentWnd=pParentWnd;

	return;
}

//�رտؼ�
VOID CEditUnderWrite::CloseUnderWrite()
{
	//�ر��ж�
	if ((m_bNeedSend==false)&&(GetActiveStatus()==false))
	{
		//���ٴ���
		DestroyWindow();
	}
	else
	{
		//���ش���
		ShowWindow(SW_HIDE);
	}

	//���½���
	ASSERT(m_pParentWnd!=NULL);
	m_pParentWnd->RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);

	return;
}

//����ǩ��
VOID CEditUnderWrite::UpdateUnderWrite()
{
	if (IsWindowVisible()==TRUE)
	{
		//��ȡ��Ϣ
		TCHAR szUnderWrite[LEN_UNDER_WRITE]=TEXT("");
		GetWindowText(szUnderWrite,CountArray(szUnderWrite));

		//�û���Ϣ
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//�����ж�
		bool bActiveMission=false;
		if ((IsModifyStatus()==true)&&(lstrcmp(szUnderWrite,m_szUnderWrite)!=0L)) bActiveMission=true;
		if ((IsModifyStatus()==false)&&(lstrcmp(szUnderWrite,pGlobalUserData->szUnderWrite)!=0L)) bActiveMission=true;

		//�����ж�
		if (bActiveMission==true)
		{
			//������Ϣ
			m_bNeedSend=true;
			lstrcpyn(m_szUnderWrite,szUnderWrite,CountArray(m_szUnderWrite));

			//��ֹ����
			CMissionManager * pMissionManager=GetMissionManager();
			if (GetActiveStatus()==true) pMissionManager->ConcludeMissionItem(this,false);

			//���õ�ַ
			LPCTSTR pszCurrentServer=pMissionManager->GetCurrentServer();
			if ((pszCurrentServer!=NULL)&&(pszCurrentServer[0]!=0)) pMissionManager->SetCustomServer(pszCurrentServer);

			//��������
			pMissionManager->AvtiveMissionItem(this,false);

			//���ؿؼ�
			ShowWindow(SW_HIDE);

			//���½���
			ASSERT(m_pParentWnd!=NULL);
			m_pParentWnd->RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
		}
		else
		{
			//�رտؼ�
			CloseUnderWrite();
		}
	}

	return;
}

//������Ϣ
VOID CEditUnderWrite::OnDestroy()
{
	__super::OnDestroy();

	//��ֹ����
	if (GetActiveStatus()==true)
	{
		//��������
		CMissionManager * pMissionManager=GetMissionManager();
		if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

		//���½���
		ASSERT(m_pParentWnd!=NULL);
		m_pParentWnd->RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return;
}

//ʧȥ����
VOID CEditUnderWrite::OnKillFocus(CWnd * pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	//�رտؼ�
	UpdateUnderWrite();

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CWndUserInfoCtrl::CWndUserInfoCtrl()
{
	//6603״̬��־
	m_bClickFace=false;
	m_bMouseEvent=false;

	//6603������־
	m_bHoverFace=false;

	//6603��������
	m_rcFaceArea.SetRect(18,3,78,63);
}

//��������
CWndUserInfoCtrl::~CWndUserInfoCtrl()
{
}

//��Ϣ����
BOOL CWndUserInfoCtrl::PreTranslateMessage(MSG * pMsg)
{
	//��ʾ��Ϣ
	if (m_ToolTipCtrl.m_hWnd!=NULL)
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//�����
BOOL CWndUserInfoCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//��������
	UINT nCommandID=LOWORD(wParam);

	//���ܰ�ť
	switch (nCommandID)
	{
	case IDC_USER_INFO_1:		//�Զ�ͷ��
		{
			//��������
			CDlgService DlgService;
			DlgService.DoModal();

			//��ʾ����  //С����ͼ���޸ĵ�
		/*	CDlgCustomFace DlgCustomFace;
			DlgCustomFace.SetCustomFaceEvent(QUERY_OBJECT_PTR_INTERFACE(CPlatformFrame::GetInstance(),IUnknownEx));

			//��ʾ����
			DlgCustomFace.DoModal();*/
						
			return TRUE;
		}
	case IDC_USER_INFO_2:		//��������
		{
			//��ʾ����
			/*CDlgLockMachine DlgLockMachine;
			DlgLockMachine.DoModal();*/
			//��������
			ASSERT(CParameterGlobal::GetInstance()!=NULL);
			CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

			//�����ַ
			TCHAR szNavigation[256]=TEXT("");
			_sntprintf(szNavigation,CountArray(szNavigation),TEXT("%s/Member/ApplyPasswordCard.aspx"),szPlatformLink,5);

			CPlatformFrame * pPlatformFrame=CPlatformFrame::GetInstance();
			pPlatformFrame->WebBrowse(szNavigation,true);

			return TRUE;
		}
	case IDC_USER_INFO_3:		//�����һ�
		{
			return TRUE;
			//�����ַ
			TCHAR szLogonLink[256]=TEXT("");
			_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("LoveLiness.aspx"));

			//��ҳ��
			CDlgBrowser DlgBrowser;
			DlgBrowser.CreateBrowserControl(TEXT("�����һ�"),szLogonLink,CSize(730,450));

			return TRUE;
		}
	case IDC_USER_INFO_4:      //���Һ���
		{
			return TRUE;
			CPlatformFrame * pPlatformFrame=CPlatformFrame::GetInstance();
			pPlatformFrame->OnBnClickedPlatform1();
			return TRUE;
		}
	case IDC_USER_INFO_5:    //ת�˼�¼
		{
			ASSERT(CParameterGlobal::GetInstance()!=NULL);
			CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

			//�����ַ
			TCHAR szNavigation[256]=TEXT("");
			_sntprintf(szNavigation,CountArray(szNavigation),TEXT("%s/Member/InsureRecord.aspx"),szPlatformLink,5);

			CPlatformFrame * pPlatformFrame=CPlatformFrame::GetInstance();
			pPlatformFrame->WebBrowse(szNavigation,true);
			return TRUE;
		}
	case IDC_USER_INFO_6:
		{
			CPlatformFrame * pPlatformFrame=CPlatformFrame::GetInstance();
			pPlatformFrame->OnBnClickedPlatform3();
			return TRUE;
		}
		break;
	case IDC_USER_INFO_SET:
		{
			//��������
			CDlgService DlgService;
			DlgService.DoModal();

			return TRUE;
		}
		break;
	case IDC_SYSTEM_SET:		//��������
		{
#ifdef VERSION_69
			//��������
			CDlgService DlgService;
			DlgService.DoModal();
#else
			CPlatformFrame * pPlatformFrame=CPlatformFrame::GetInstance();
			pPlatformFrame->OnBnClickedPlatform3();
#endif
			return TRUE;
		}
	case IDC_USER_INFO_COPY:   //���Ƹ�����Ϣ
		{
			CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();

			CString str, str1;
			str.Format(_T("�ǳƣ�"));
			str += pGlobalUserInfo->GetGlobalUserData()->szNickName;

			str1.Format(_T(" ID�ţ�%ld"), pGlobalUserInfo->GetGlobalUserData()->dwGameID);
			CWHService::SetClipboardString(str + str1);

			CInformation Information(this);
			Information.ShowMessageBox(TEXT("ϵͳ��ʾ"), TEXT("�Ѿ��ɹ������û��ǳƺ�ID��"),MB_OK,10);
			return TRUE;
		}
	default:
		break;
	}

	return __super::OnCommand(wParam,lParam);
}

//λ����Ϣ
VOID CWndUserInfoCtrl::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//������Ϣ
INT CWndUserInfoCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//ע���¼�
	CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
	if (pPlatformEvent!=NULL) pPlatformEvent->RegisterEventWnd(m_hWnd);

	//������Դ
	CPngImage ImageBkgnd;
	ImageBkgnd.LoadImage(AfxGetInstanceHandle(),TEXT("USER_INFO_BACK"));

	//������ť
	CRect rcCreate(0,0,0,0);
	m_btUserInfoSet.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_USER_INFO_SET);
	m_btUserInfoCopy.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_USER_INFO_COPY);
	m_btSystemSet.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SYSTEM_SET);

	//���ð�ť
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btUserInfoSet.SetButtonImage(IDB_BK_USER_INFO_SET,TEXT("BT_USER_INFO_SET"),hInstance,true,false);
	m_btUserInfoCopy.SetButtonImage(IDB_BK_USER_INFO,TEXT("BT_USER_INFO_COPY"),hInstance,true,false);
	m_btSystemSet.SetButtonImage(IDB_BK_USER_INFO,TEXT("BT_SYSTEM_SET"),hInstance,true,false);

	//�ƶ�����
	CRect rcUserInfo;
	GetWindowRect(&rcUserInfo);
	SetWindowPos(NULL,0,0,ImageBkgnd.GetWidth(),ImageBkgnd.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//��ʾ�ؼ�
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_btSystemSet,TEXT("ϵͳ����"));
	m_ToolTipCtrl.AddTool(&m_btUserInfoCopy,TEXT("���Ƹ�����Ϣ"));
	m_ToolTipCtrl.AddTool(&m_btUserInfoSet,TEXT("��������"));

	return 0;
}

VOID CWndUserInfoCtrl::OnLButtonUp(UINT nFlags, CPoint MousePoint)
{
	__super::OnLButtonUp(nFlags,MousePoint);

	if ((m_bHoverFace==true)&&(m_bClickFace==true))
	{
		//6603�ͷŲ���
		ReleaseCapture();

		//6603���ñ���
		m_bClickFace=false;

		//6603������Ϣ
		if (m_rcFaceArea.PtInRect(MousePoint)==TRUE)
		{
			CDlgService DlgService;
			DlgService.DoModal();
		}
	}
}
//6603�����Ϣ
VOID CWndUserInfoCtrl::OnLButtonDown(UINT nFlags, CPoint MousePoint)
{
	__super::OnLButtonDown(nFlags,MousePoint);

	//6603���ý���
	SetFocus();

	//6603��������
	if ((m_bHoverFace==true)&&(m_bClickFace==false))
	{
		//6603����˻�
		SetCapture();

		//6603���ñ���
		m_bClickFace=true;
	}
}
//6603�����Ϣ
BOOL CWndUserInfoCtrl::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//6603��ȡ���
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//6603��������
	bool bMouseEvent=false;
	bool bRedrawWindow=false;

	//6603�����ж�
	if ((m_bHoverFace==false)&&(m_rcFaceArea.PtInRect(MousePoint)==TRUE))
	{
		//6603���ñ���
		bMouseEvent=true;
		bRedrawWindow=true;

		//6603���ñ���
		m_bHoverFace=true;
	}

	//6603�뿪�ж�
	if ((m_bHoverFace==true)&&(m_rcFaceArea.PtInRect(MousePoint)==FALSE))
	{
		//6603���ñ���
		bRedrawWindow=true;

		//6603���ñ���
		m_bHoverFace=false;
	}

	//6603ע���¼�
	if ((m_bMouseEvent==false)&&(bMouseEvent==true))
	{
		//6603���ñ���
		m_bMouseEvent=true;

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

	//6603���½���
	if (bRedrawWindow==true)
	{
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	//6603���ù��
	if (m_bHoverFace==true)
	{
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
		return true;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//�����ؼ�
VOID CWndUserInfoCtrl::RectifyControl(INT nWidth, INT nHeight)
{
	//��������
	HDWP hDwp=BeginDeferWindowPos(64);
	UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER|SWP_NOSIZE;

	CRect rcClient;
	GetClientRect(&rcClient);

	//������ť
	DeferWindowPos(hDwp,m_btSystemSet,NULL,rcClient.Width()-8-22,22,0,0,uFlags);
	DeferWindowPos(hDwp,m_btUserInfoCopy,NULL,rcClient.Width()-8-22,42,0,0,uFlags);
	DeferWindowPos(hDwp,m_btUserInfoSet,NULL,80,2,0,0,uFlags);

	//��������
	EndDeferWindowPos(hDwp);

	return;
}

//�滭����
BOOL CWndUserInfoCtrl::OnEraseBkgnd(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CImage ImageBuffer;
	if(ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32) == FALSE) return TRUE;

	//���� DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(BufferDC);

	//��������
//	HINSTANCE hInstance=AfxGetInstanceHandle();
//	CSkinRenderManager * pSkinRenderManager=CSkinRenderManager::GetInstance();

	//�滭����
	CPngImage ImageBkgnd;
	ImageBkgnd.LoadImage(AfxGetInstanceHandle(),TEXT("USER_INFO_BACK"));

	ImageBkgnd.DrawImage(pBufferDC, 0, 0);

	//�û���Ϣ
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//�滭ͷ��
	if (pGlobalUserData->CustomFaceInfo.dwDataSize!=0)
	{
		DWORD * pdwCustomFace=pGlobalUserData->CustomFaceInfo.dwCustomFace;
		theApp.m_FaceItemControlModule->DrawFaceNormal(pBufferDC,18,3,60,60,pdwCustomFace);
	}
	else
	{
		//69�����Ա����ͷ��
		theApp.m_FaceItemControlModule->DrawFaceNormal(pBufferDC,18,3,60,60,pGlobalUserData->wFaceID);
	}

	//�滭����
	pBufferDC->SetTextColor(RGB(21,72,124));

//	LOGI("CWndUserInfoCtrl::OnEraseBkgnd, szNickName"<<pGlobalUserData->szNickName);

	//����ʺ�
	CRect rcNickName;
	rcNickName.SetRect(86,22,rcClient.Width()-15,40);
	CString str;
	str.Format(_T("�ǳƣ�"));
	str += pGlobalUserData->szNickName;
	pBufferDC->DrawText(str, &rcNickName,DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	CRect rcGameID;
	rcGameID.SetRect(86,42,rcClient.Width()-15,60);
	str.Format(_T("ID�ţ�%ld"), pGlobalUserData->dwGameID);
	pBufferDC->DrawText(str,&rcGameID,DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);


	// ����ʾ��Ϸ��
// 	CRect rcScore;
// 	rcScore.SetRect(86,44,rcClient.Width()-15,62);
// 	CString str;
// 	str.Format(TEXT("��Ϸ�ң�%ld"), pGlobalUserData->lUserScore);
// 	pBufferDC->DrawText(str, &rcScore, DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	return TRUE;
}

//6603�����Ϣ
LRESULT CWndUserInfoCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	//6603���ñ���
	m_bMouseEvent=false;

	//6603�뿪����
	if (m_bHoverFace==true)
	{
		//6603���ñ���
		m_bMouseEvent=false;

		//6603���½���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}

	return 0;
}

//�¼���Ϣ
LRESULT CWndUserInfoCtrl::OnMessagePlatformEvent(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case EVENT_USER_LOGON:			//��¼�ɹ�
		{
			//���´���
			Invalidate();

			//���ð�ť
			m_btUserInfoSet.EnableWindow(TRUE);
			m_btSystemSet.EnableWindow(TRUE);
			m_btUserInfoCopy.EnableWindow(TRUE);


			return 0L;
		}
	case EVENT_USER_LOGOUT:			//ע���ɹ�
		{
			//���ð�ť
			m_btUserInfoSet.EnableWindow(FALSE);
			m_btSystemSet.EnableWindow(FALSE);
			m_btUserInfoCopy.EnableWindow(FALSE);

			return 0L;
		}
	case EVENT_USER_INFO_UPDATE:	//���ϸ���
		{
//			LOGI("���´���");
			//���´���
			Invalidate();

			return 0L;
		}
		break;
	case EVENT_USER_MOOR_MACHINE:	//�󶨻���
		{
			//�û���Ϣ
// 			CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
// 			tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
// 
// 			//���ð�ť
// 			if (pGlobalUserData->cbMoorMachine==FALSE)
// 			{
// 				HINSTANCE hInstance=AfxGetInstanceHandle();
// 				m_btUserInfo2.SetButtonImage(IDB_BK_USER_INFO,TEXT("BT_USER_INFO_2"),hInstance,true,false);
// 				m_ToolTipCtrl.AddTool(&m_btUserInfo2,TEXT("��������"));
// 			}
// 			else
// 			{
// 				HINSTANCE hInstance=AfxGetInstanceHandle();
// 				m_btUserInfo2.SetButtonImage(IDB_BK_USER_INFO,TEXT("BT_USER_INFO_4"),hInstance,true,false);
// 				m_ToolTipCtrl.AddTool(&m_btUserInfo2,TEXT("��������"));
// 			}

			return 0L;
		}
	}

	return 0L;
}

//////////////////////////////////////////////////////////////////////////////////
