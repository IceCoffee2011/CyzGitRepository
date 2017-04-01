#include "StdAfx.h"
#include "resource.h"
#include "GamePlaza.h"
#include "DlgServiceLockMachine.h"

BEGIN_MESSAGE_MAP(CDlgServiceLockMachine, CDlgServiceItem)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CDlgServiceLockMachine::CDlgServiceLockMachine() : CDlgServiceItem(IDD_DLG_LOCK_MACHINE)
{
	m_cbMachine = FALSE;
	ZeroMemory(m_szPassword,sizeof(m_szPassword));

	//�������
	m_MissionManager.InsertMissionItem(this);
}

CDlgServiceLockMachine::~CDlgServiceLockMachine()
{
}

VOID CDlgServiceLockMachine::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PASSWORD, m_edInsurePassword);

	DDX_Control(pDX, IDC_LOCKMACHINE, m_btLockMachine);
	DDX_Control(pDX, IDC_UNLOCKMACHINE, m_btUnLockMachine);
}

//��������
BOOL CDlgServiceLockMachine::OnInitDialog()
{
	__super::OnInitDialog();
	HINSTANCE hInstance=AfxGetInstanceHandle();
	// �ƶ��ؼ�λ��
	m_edInsurePassword.SetDrawBorad(false);
	m_edInsurePassword.MoveWindow(92, 70, 180, 18);

	m_btLockMachine.SetButtonImage(IDB_BT_LOCK_MACHINE, hInstance, false, false);
	m_btUnLockMachine.SetButtonImage(IDB_BT_UNLOCK_MACHINE, hInstance, false, false);

	CRect rcLockMachine;
	m_btLockMachine.GetWindowRect(rcLockMachine);
	m_btLockMachine.MoveWindow(290,68,rcLockMachine.Width(), rcLockMachine.Height());
	CRect rcUnLockMachine;
	m_btLockMachine.GetWindowRect(rcUnLockMachine);
	m_btUnLockMachine.MoveWindow(290,68,rcUnLockMachine.Width(), rcUnLockMachine.Height());

	//���ð�ť
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	m_cbMachine=(pGlobalUserData->cbMoorMachine==0)?TRUE:FALSE;

	if (pGlobalUserData->cbMoorMachine == 0)
	{
		m_btLockMachine.ShowWindow(TRUE);
		m_btUnLockMachine.ShowWindow(FALSE);
	}
	else
	{
		m_btUnLockMachine.ShowWindow(TRUE);
		m_btLockMachine.ShowWindow(FALSE);
	}

	return TRUE;
}

BOOL CDlgServiceLockMachine::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//��������
	UINT nCommandID=LOWORD(wParam);

	//���ܰ�ť
	switch (nCommandID)
	{
	case IDC_LOCKMACHINE:
	case IDC_UNLOCKMACHINE:
		{
			//��ȡ��Ϣ
			m_edInsurePassword.GetUserPassword(m_szPassword);

			//�����ж�
			if (m_szPassword[0]==0)
			{
				//������ʾ
				CInformation Information(this);
				Information.ShowMessageBox(TEXT("���벻��Ϊ�գ������뱣�չ�������а󶨣�"),MB_ICONERROR);

				//���ý���
				m_edInsurePassword.SetFocus();

				return TRUE;
			}

			//��������
			ASSERT(GetMissionManager()!=NULL);
			CMissionManager * pMissionManager=GetMissionManager();

			//��������
			if (pMissionManager->AvtiveMissionItem(this,false)==false)
			{
				//������ʾ
				CInformation Information(this);
				Information.ShowMessageBox(TEXT("����������ʧ�ܣ��������������Լ���������״����"),MB_ICONERROR);

				return TRUE;
			}

			//���ÿؼ�
//			m_btOk.EnableWindow(FALSE);
		}
		break;
// 	case IDC_UNLOCKMACHINE:
// 		{
// 			MessageBox(_T("11111"));
// 		}
// 		break;
	}

	return __super::OnCommand(wParam,lParam);
}

//�����¼�
bool CDlgServiceLockMachine::OnEventMissionLink(INT nErrorCode)
{
//	MessageBox(_T("CDlgServiceLockMachine::OnEventMissionLink"));

	//������
	if (nErrorCode!=0L)
	{
		//���ÿؼ�
		OnMissionConclude();

		//��ʾ��ʾ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("���������еķ��������޷��ɹ����ӷ���������������վά�����棡"),MB_ICONERROR);
	}
	else
	{
		//��������
		CMD_GP_ModifyMachine ModifyMachine;
		ZeroMemory(&ModifyMachine,sizeof(ModifyMachine));

		//��������
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//���ñ���
		ModifyMachine.cbBind=m_cbMachine;
		ModifyMachine.dwUserID=pGlobalUserData->dwUserID;

		//��������
		CWHService::GetMachineIDEx(ModifyMachine.szMachineID);
		CWHEncrypt::MD5Encrypt(m_szPassword,ModifyMachine.szPassword);

		//��������
		ASSERT(GetMissionManager()!=NULL);
		GetMissionManager()->SendData(MDM_GP_USER_SERVICE,SUB_GP_MODIFY_MACHINE,&ModifyMachine,sizeof(ModifyMachine));

		OnMissionStart();
	}

	return true;
}

//�ر��¼�
bool CDlgServiceLockMachine::OnEventMissionShut(BYTE cbShutReason)
{
//	MessageBox(_T("CDlgServiceLockMachine::OnEventMissionShut"));

	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//��������
		CMissionManager * pMissionManager=GetMissionManager();
		if (pMissionManager->AvtiveMissionItem(this,true)==true) return true;

		//���ÿؼ�
		OnMissionConclude();

		//��ʾ��ʾ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("���ڵ�ǰ����������æ���ʺŰ󶨲�������ʧ�ܣ����Ժ������ԣ�"),MB_ICONERROR);
	}
	return true;
}
//��ȡ�¼�
bool CDlgServiceLockMachine::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
//	MessageBox(_T("CDlgServiceLockMachine::OnEventMissionRead"));
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

				//��ʾ��Ϣ
				if (pOperateFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pOperateFailure->szDescribeString,MB_ICONERROR,60);
				}

				//���ÿؼ�
				OnMissionConclude();
				m_edInsurePassword.SetFocus();

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

				//��������
				CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
				tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();


				//���ñ���
				pGlobalUserData->cbMoorMachine=m_cbMachine;
				m_cbMachine = (pGlobalUserData->cbMoorMachine==0)?TRUE:FALSE;

				//�����¼�
 				CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
 				if (pPlatformEvent!=NULL) pPlatformEvent->SendPlatformEvent(EVENT_USER_MOOR_MACHINE,0L);
 
 				//��ʾ��Ϣ
 				if (pOperateSuccess->szDescribeString[0]!=0)
 				{
 					CInformation Information(this);
 					Information.ShowMessageBox(pOperateSuccess->szDescribeString,MB_ICONINFORMATION,60);
 				}
				
				m_edInsurePassword.SetUserPassword(_T(""));
				
				OnMissionConclude();
				return true;
			}
		}
	}
	return true;
}


//��ʼ����
VOID CDlgServiceLockMachine::OnMissionStart()
{
//	MessageBox(_T("CDlgServiceLockMachine::OnMissionStart"));
	if (m_btLockMachine.IsWindowVisible())
	{
		m_btLockMachine.EnableWindow(FALSE);
	}

	if (m_btUnLockMachine.IsWindowVisible())
	{
		m_btUnLockMachine.EnableWindow(FALSE);
	}

}

//��ֹ����
VOID CDlgServiceLockMachine::OnMissionConclude()
{
	if (m_cbMachine == TRUE)
	{
		m_btLockMachine.ShowWindow(SW_SHOW);
		m_btUnLockMachine.ShowWindow(SW_HIDE);
	}
	else if (m_cbMachine == FALSE)
	{
		m_btUnLockMachine.ShowWindow(SW_SHOW);
		m_btLockMachine.ShowWindow(SW_HIDE);
	}

	if (m_btLockMachine.IsWindowVisible())
	{
		m_btLockMachine.EnableWindow(TRUE);
	}

	if (m_btUnLockMachine.IsWindowVisible())
	{
		m_btUnLockMachine.EnableWindow(TRUE);
	}
}

BOOL CDlgServiceLockMachine::OnEraseBkgnd(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//���� DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(BufferDC);

	//������Դ
	CPngImage ImageServiceLockMachine;
	ImageServiceLockMachine.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_LOCKMACHINE"));

	//��䱳��
	pBufferDC->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

	// ����ͼ
	ImageServiceLockMachine.DrawImage(pBufferDC,0,0,ImageServiceLockMachine.GetWidth(),ImageServiceLockMachine.GetHeight(),0,0);


	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	return TRUE;
}
