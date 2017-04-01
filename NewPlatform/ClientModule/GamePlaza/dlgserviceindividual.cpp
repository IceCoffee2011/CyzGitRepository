#include "StdAfx.h"
#include "Resource.h"
#include "GamePlaza.h"
#include "PlatformEvent.h"
#include "PlatformFrame.h"
#include "DlgCustomFace.h"
#include "DlgServiceIndividual.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServiceIndividual, CDlgServiceItem)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_SELECT_FACE, OnBnClickedSelectFace)
	ON_BN_CLICKED(IDC_BT_UNDERWRITE, OnBnClickedUnderWrite)
	ON_BN_CLICKED(IDC_BT_SUBMIT, OnBnClickedSubmit)
	ON_BN_CLICKED(IDC_BT_EXCHANGE_LOVELINESS, OnBnClickedExchangeLoveliness)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgServiceIndividual::CDlgServiceIndividual() : CDlgServiceItem(IDD_DLG_SERVICE_INDIVIDUAL)
{
	//����ͼ��
	m_cbImageIndex=0;

	//�������
	m_bQueryMission=false;
	m_bModifyMission=false;

	//��������
	m_MissionManager.InsertMissionItem(this);

	//�ʺ�����
	m_cbGender=0;
	ZeroMemory(m_szNickName,sizeof(m_szNickName));
	ZeroMemory(m_szPassword,sizeof(m_szPassword));
	ZeroMemory(m_szUnderWrite,sizeof(m_szUnderWrite));

	//��ϸ����
	ZeroMemory(m_szQQ,sizeof(m_szQQ));
	ZeroMemory(m_szEMail,sizeof(m_szEMail));
	ZeroMemory(m_szUserNote,sizeof(m_szUserNote));
	ZeroMemory(m_szSeatPhone,sizeof(m_szSeatPhone));
	ZeroMemory(m_szMobilePhone,sizeof(m_szMobilePhone));
	ZeroMemory(m_szCompellation,sizeof(m_szCompellation));
	ZeroMemory(m_szDwellingPlace,sizeof(m_szDwellingPlace));

	//������ˢ
	m_brBrush.CreateSolidBrush(RGB(255,255,255));

	return;
}

//��������
CDlgServiceIndividual::~CDlgServiceIndividual()
{
}

//�ؼ���
VOID CDlgServiceIndividual::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//�����ؼ�
//	DDX_Control(pDX, IDOK, m_btOk);
//	DDX_Control(pDX, IDC_FACE_ITEM, m_FaceItemView);
//	DDX_Control(pDX, IDC_SELECT_FACE, m_btSelectFace);

	//�༭�ؼ�
	DDX_Control(pDX, IDC_ACCOUNTS, m_edAccounts);
	DDX_Control(pDX, IDC_NICKNAME, m_edNickName);
//	DDX_Control(pDX, IDC_PASSWORD, m_edPassword);
	DDX_Control(pDX, IDC_UNDER_WRITE, m_edUnderWrite);

	//�༭�ؼ�
	DDX_Control(pDX, IDC_QQ, m_edQQ);
//	DDX_Control(pDX, IDC_EMAIL, m_edEMail);
//	DDX_Control(pDX, IDC_USER_NOTE, m_edUserNote);
//	DDX_Control(pDX, IDC_SEAT_PHONE, m_edSeatPhone);
	DDX_Control(pDX, IDC_MOBILE_PHONE, m_edMobilePhone);
//	DDX_Control(pDX, IDC_COMPELLATION, m_edCompellation);
//	DDX_Control(pDX, IDC_DWELLING_PLACE, m_edDwellingPlace);
	DDX_Control(pDX, IDC_PASSPORTID, m_edPassPortID);

	DDX_Control(pDX, IDC_GAMEID, m_edGameID);
	DDX_Control(pDX, IDC_GENDER, m_edGender);
	DDX_Control(pDX, IDC_MEMBER, m_edMember);
	DDX_Control(pDX, IDC_USERMEDAL, m_edUserMedal);

	DDX_Control(pDX, IDC_BT_UNDERWRITE, m_btUnderWrite);
	DDX_Control(pDX, IDC_BT_SUBMIT, m_btOk);

	DDX_Control(pDX, IDC_BT_EXCHANGE_LOVELINESS, m_btExchangeLoveLiness);

//	DDX_Control(pDX, IDC_STATIC_GAMEID, m_hlGameID);
}

//��Ϣ����
BOOL CDlgServiceIndividual::PreTranslateMessage(MSG * pMsg)
{
	//��ʾ��Ϣ
	if (m_ToolTipCtrl.m_hWnd!=NULL)
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//��������
BOOL CDlgServiceIndividual::OnInitDialog()
{
	__super::OnInitDialog();
	HINSTANCE hInstance=AfxGetInstanceHandle();

	//�ʺ���Ϣ
	m_edAccounts.LimitText(LEN_ACCOUNTS-1);
	m_edNickName.LimitText(LEN_NICKNAME-1);
//	m_edPassword.LimitText(LEN_PASSWORD-1);
	m_edUnderWrite.LimitText(LEN_UNDER_WRITE-1);

	m_btUnderWrite.SetButtonImage(IDB_BT_UNDERWRITE,  hInstance, false, false);
	m_btOk.SetButtonImage(IDB_BT_SUBMIT_UNDERWRITE, hInstance, false, false);
	m_btExchangeLoveLiness.SetButtonImage(IDB_BT_EXCHANGE_LOVELINESS, hInstance, false, false);

	//��������
//	m_edQQ.LimitText(LEN_QQ-1);
//	m_edEMail.LimitText(LEN_EMAIL-1);
//	m_edUserNote.LimitText(LEN_USER_NOTE-1);
//	m_edSeatPhone.LimitText(LEN_SEAT_PHONE-1);
//	m_edMobilePhone.LimitText(LEN_MOBILE_PHONE-1);
//	m_edCompellation.LimitText(LEN_COMPELLATION-1);
//	m_edDwellingPlace.LimitText(LEN_DWELLING_PLACE-1);

	// �����ռ���
//	GetDlgItem(IDC_STATIC_NICKNAME)->MoveWindow(27, 40+3, 60, 18);
//	GetDlgItem(IDC_STATIC_MEMBER)->MoveWindow(27, 60+3, 60, 18);
//	GetDlgItem(IDC_STATIC_ACCOUNTS)->MoveWindow(27, 80+3, 60, 18);
//	GetDlgItem(IDC_STATIC_GENDER)->MoveWindow(27, 100+3, 60, 18);

	m_edGameID.MoveWindow(100, 17-5,100, 22);
	m_edNickName.MoveWindow(100, 36-5, 100, 22);
	m_edMember.MoveWindow(100, 55-4, 100,22);
	m_edAccounts.MoveWindow(100, 74-4, 100, 22);
	m_edGender.MoveWindow(100, 93-3,100, 22);
	m_edUserMedal.MoveWindow(100, 112-3, 100, 22);

	m_edQQ.MoveWindow(100,147-5,100,22);
	m_edMobilePhone.MoveWindow(290,147-5,100,22);
	m_edPassPortID.MoveWindow(100,174-5,300,22);

	CRect rcUnderWrite;
	m_btUnderWrite.GetClientRect(rcUnderWrite);
	m_btUnderWrite.MoveWindow(90, 213, rcUnderWrite.Width(), rcUnderWrite.Height());

	CRect rcExchangeLoveLiness;
	m_btExchangeLoveLiness.GetClientRect(rcExchangeLoveLiness);
	m_btExchangeLoveLiness.MoveWindow(210, 112-3, rcExchangeLoveLiness.Width(), rcExchangeLoveLiness.Height());

	CRect rcOk;
	m_btOk.GetClientRect(rcOk);
	m_btOk.MoveWindow(420, 225, rcOk.Width(), rcOk.Height());
	
	// �����ؼ���ɫ
	m_edAccounts.SetEnableColor( RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255) );
	m_edAccounts.SetDisableColor( RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255) );

	m_edNickName.SetEnableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255) );
	m_edNickName.SetDisableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));

	m_edGameID.SetEnableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255) );
	m_edGameID.SetDisableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));

	m_edGender.SetEnableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));
	m_edGender.SetDisableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));

	m_edMember.SetEnableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));
	m_edMember.SetDisableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));

	m_edUserMedal.SetEnableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));
	m_edUserMedal.SetDisableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));

	m_edQQ.SetEnableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));
	m_edQQ.SetDisableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));

	m_edMobilePhone.SetEnableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));
	m_edMobilePhone.SetDisableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));

	m_edPassPortID.SetEnableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));
	m_edPassPortID.SetDisableColor(RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255));
	
	m_edUnderWrite.SetEnableColor(RGB(61, 61, 61), RGB(255,255,255), RGB(131,192,219));
	m_edUnderWrite.SetDisableColor(RGB(172, 168, 153), RGB(255,255,255), RGB(255,255,255));

	m_btOk.ShowWindow(SW_HIDE);

	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_btUnderWrite,TEXT("�޸ĸ���ǩ��"));

	//������Ϣ
	LoadAccountsInfo();

	return TRUE;
}

//ȷ������
VOID CDlgServiceIndividual::OnOK()
{
//	MessageBox(_T("CDlgServiceIndividual::OnOK"));
	//��������
 	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
 	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
 
 	//��ȡ����
// 	TCHAR szPassword[LEN_PASSWORD]=TEXT("");
 //	GetDlgItemText(IDC_PASSWORD,szPassword,CountArray(szPassword));
 
 	//�ʺ���Ϣ
// 	GetControlItemText(IDC_NICKNAME,m_szNickName,CountArray(m_szNickName));
 	GetControlItemText(IDC_UNDER_WRITE,m_szUnderWrite,CountArray(m_szUnderWrite));
 
 	//�û�����
// 	GetDlgItemText(IDC_USER_NOTE,m_szUserNote,CountArray(m_szUserNote));
// 	GetDlgItemText(IDC_COMPELLATION,m_szCompellation,CountArray(m_szCompellation));
 
 	//�绰����
// 	GetDlgItemText(IDC_SEAT_PHONE,m_szSeatPhone,CountArray(m_szSeatPhone));
// 	GetDlgItemText(IDC_MOBILE_PHONE,m_szMobilePhone,CountArray(m_szMobilePhone));
 
 	//��ϵ����
// 	GetDlgItemText(IDC_QQ,m_szQQ,CountArray(m_szQQ));
// 	GetDlgItemText(IDC_EMAIL,m_szEMail,CountArray(m_szEMail));
// 	GetDlgItemText(IDC_DWELLING_PLACE,m_szDwellingPlace,CountArray(m_szDwellingPlace));
 
 	//�û��Ա�
// 	if (((CButton *)GetDlgItem(IDC_GENDER_FEMALE))->GetCheck()==BST_CHECKED) m_cbGender=GENDER_FEMALE;
// 	if (((CButton *)GetDlgItem(IDC_GENDER_MANKIND))->GetCheck()==BST_CHECKED) m_cbGender=GENDER_MANKIND;
 
 	//�ǳ��ж�
//  	if (lstrcmp(m_szNickName,pGlobalUserData->szNickName)!=0)
//  	{
//  		//��������
//  		TCHAR szDescribe[128]=TEXT("");
//  		CUserItemElement * pUserItemElement=CUserItemElement::GetInstance();
//  
//  		//�ǳ��ж�
//  		if (pUserItemElement->EfficacyNickName(m_szNickName,szDescribe,CountArray(szDescribe))==false)
//  		{
//  			//��ʾ��Ϣ
//  			CInformation Information(this);
//  			Information.ShowMessageBox(szDescribe,MB_ICONERROR,0);
//  
//  			//���ý���
//  			m_edNickName.SetFocus();
//  
//  			return;
//  		}
//  	}
 
 	//��������
// 	CWHEncrypt::MD5Encrypt(szPassword,m_szPassword);
 
 	//�����ж�
// 	if (lstrcmp(m_szPassword,pGlobalUserData->szPassword)!=0)
// 	{
// 		//��ʾ��Ϣ
// 		CInformation Information(this);
// 		Information.ShowMessageBox(TEXT("�û��ʺ�����������������룡"),MB_ICONERROR,0);
// 
// 		//���ý���
// //		m_edPassword.SetFocus();
// 
// 		return;
// 	}
 
 	//���ñ���
 	m_bQueryMission=false;
 	m_bModifyMission=true;
 
 	//�¼�����
 	OnMissionStart();
 
 	//��������
 	if (m_MissionManager.AvtiveMissionItem(this,false)==false) 
 	{
 		OnMissionConclude();
 		return;
 	}

	return;
}

//�ؼ���ɫ
HBRUSH CDlgServiceIndividual::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_BTN:
	case CTLCOLOR_STATIC:
	case CTLCOLOR_EDIT:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(61,61,61));
			return m_brBrush;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//�����¼�
bool CDlgServiceIndividual::OnEventMissionLink(INT nErrorCode)
{
//	MessageBox(_T("CDlgServiceIndividual::OnEventMissionLink"));
	//���ӽ��
	if (nErrorCode!=0)
	{
		//��������
		if (m_MissionManager.AvtiveMissionItem(this,true)==true)
		{
			return true;
		}

		//�¼�����
		OnMissionConclude();

		//������ʾ
		if (m_bQueryMission==true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("���ӷ�������ʱ���û����ϻ�ȡʧ�ܣ�"),MB_OK|MB_ICONSTOP,30);
		}

		//������ʾ
		if (m_bModifyMission==true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("���ӷ�������ʱ���û������޸�ʧ�ܣ�"),MB_OK|MB_ICONSTOP,30);
		}

		return true;
	}

	//�޸�����
	if (m_bModifyMission==true)
	{
		//��������
		BYTE cbBuffer[SOCKET_TCP_BUFFER];
		ZeroMemory(cbBuffer,sizeof(cbBuffer));

		//��������
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		// ��������
		CMD_GP_ModifyUnderWrite ModifyUnderWrite;

		ModifyUnderWrite.dwUserID = pGlobalUserData->dwUserID;
		lstrcpyn(ModifyUnderWrite.szUnderWrite, m_szUnderWrite, CountArray(m_szUnderWrite) );

		//��������
//		CMD_GP_ModifyIndividual * pModifyIndividual=(CMD_GP_ModifyIndividual *)cbBuffer;
//		CSendPacketHelper SendPacket(cbBuffer+sizeof(CMD_GP_ModifyIndividual),sizeof(cbBuffer)-sizeof(CMD_GP_ModifyIndividual));

		//���ñ���
//		pModifyIndividual->cbGender=m_cbGender;
//		pModifyIndividual->dwUserID=pGlobalUserData->dwUserID;
//		lstrcpyn(pModifyIndividual->szPassword,m_szPassword,CountArray(pModifyIndividual->szPassword));

		//�û��ǳ�
// 		if (m_szNickName[0]!=0)
// 		{
// 			SendPacket.AddPacket(m_szNickName,DTP_GP_UI_NICKNAME);
// 		}

		//����ǩ��
//		if (m_szUnderWrite[0]!=0)
//		{
//			SendPacket.AddPacket(m_szUnderWrite,DTP_GP_UI_UNDER_WRITE);
//		}

		//�û���ע
// 		if (m_szUserNote[0]!=0)
// 		{
// 			SendPacket.AddPacket(m_szUserNote,DTP_GP_UI_USER_NOTE);
// 		}

		//��ʵ����
// 		if (m_szCompellation[0]!=0) 
// 		{
// 			SendPacket.AddPacket(m_szCompellation,DTP_GP_UI_COMPELLATION);
// 		}

		//�̶�����
// 		if (m_szSeatPhone[0]!=0) 
// 		{
// 			SendPacket.AddPacket(m_szSeatPhone,DTP_GP_UI_SEAT_PHONE);
// 		}

		//�ֻ�����
// 		if (m_szMobilePhone[0]!=0)
// 		{
// 			SendPacket.AddPacket(m_szMobilePhone,DTP_GP_UI_MOBILE_PHONE);
// 		}

		//Q Q ����
// 		if (m_szQQ[0]!=0) 
// 		{
// 			SendPacket.AddPacket(m_szQQ,DTP_GP_UI_QQ);
// 		}

		//�����ʼ�
// 		if (m_szEMail[0]!=0) 
// 		{
// 			SendPacket.AddPacket(m_szEMail,DTP_GP_UI_EMAIL);
// 		}

		//��ϸ��ַ
// 		if (m_szDwellingPlace[0]!=0) 
// 		{
// 			SendPacket.AddPacket(m_szDwellingPlace,DTP_GP_UI_DWELLING_PLACE);
// 		}

		//��������
//		WORD wSendSize=sizeof(CMD_GP_ModifyIndividual)+SendPacket.GetDataSize();
		WORD wHeadSize=sizeof(ModifyUnderWrite)-sizeof(ModifyUnderWrite.szUnderWrite);
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_MODIFY_UNDER_WRITE,&ModifyUnderWrite,wHeadSize+CountStringBuffer(ModifyUnderWrite.szUnderWrite));
	}

	//��ѯ����
	if (m_bQueryMission==true)
	{
		//��������
		CMD_GP_QueryIndividual QueryIndividual;
		ZeroMemory(&QueryIndividual,sizeof(QueryIndividual));

		//��������
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//���ñ���
		QueryIndividual.dwUserID=pGlobalUserData->dwUserID;

		//��������
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_QUERY_INDIVIDUAL,&QueryIndividual,sizeof(QueryIndividual));
	}

	return true;
}

//�ر��¼�
bool CDlgServiceIndividual::OnEventMissionShut(BYTE cbShutReason)
{
	//��ʾ��Ϣ
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//��������
		if (m_MissionManager.AvtiveMissionItem(this,true)==true)
		{
			return true;
		}

		//��ѯ����
		if (m_bQueryMission==true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("��������������쳣�Ͽ��ˣ��û����ϻ�ȡʧ�ܣ�"),MB_ICONERROR,30);
		}

		//�޸�����
		if (m_bModifyMission==true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("��������������쳣�Ͽ��ˣ��û������޸�ʧ�ܣ�"),MB_ICONERROR,30);
		}
	}

	//�¼�����
	OnMissionConclude();

	return true;
}

//��ȡ�¼�
bool CDlgServiceIndividual::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//�����
	if (Command.wMainCmdID==MDM_GP_USER_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_OPERATE_FAILURE:		//����ʧ��
			{
				//Ч�����
				CMD_GP_OperateFailure * pOperateFailure=(CMD_GP_OperateFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString))) return false;

				//ʧ�ܴ���
				OnMissionConclude();

				//�ر�����
				m_MissionManager.ConcludeMissionItem(this,false);

				//��ʾ��Ϣ
				if (pOperateFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
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
				m_MissionManager.ConcludeMissionItem(this,false);

				//��������
				CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
				tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
				tagIndividualUserData * pIndividualUserData=pGlobalUserInfo->GetIndividualUserData();

//				LOGI("CDlgServiceIndividual::OnEventMissionRead, m_szNickName="<<m_szNickName);

				//�ʺ�����
				pGlobalUserData->cbGender=m_cbGender;
//				lstrcpyn(pGlobalUserData->szNickName,m_szNickName,CountArray(pGlobalUserData->szNickName));
				lstrcpyn(pGlobalUserData->szUnderWrite,m_szUnderWrite,CountArray(pGlobalUserData->szUnderWrite));

				//��ϸ����
//				lstrcpyn(pIndividualUserData->szQQ,m_szQQ,CountArray(pIndividualUserData->szQQ));
//				lstrcpyn(pIndividualUserData->szEMail,m_szEMail,CountArray(pIndividualUserData->szEMail));
//				lstrcpyn(pIndividualUserData->szUserNote,m_szUserNote,CountArray(pIndividualUserData->szUserNote));
//				lstrcpyn(pIndividualUserData->szSeatPhone,m_szSeatPhone,CountArray(pIndividualUserData->szSeatPhone));
//				lstrcpyn(pIndividualUserData->szMobilePhone,m_szMobilePhone,CountArray(pIndividualUserData->szMobilePhone));
//				lstrcpyn(pIndividualUserData->szCompellation,m_szCompellation,CountArray(pIndividualUserData->szCompellation));
//				lstrcpyn(pIndividualUserData->szDwellingPlace,m_szDwellingPlace,CountArray(pIndividualUserData->szDwellingPlace));

				//�����¼�
				CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
				if (pPlatformEvent!=NULL) pPlatformEvent->SendPlatformEvent(EVENT_USER_INFO_UPDATE,0L);

				//��ʾ��Ϣ
				if (pOperateSuccess->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pOperateSuccess->szDescribeString,MB_ICONINFORMATION,60);
				}

				//�ɹ�����
				OnMissionConclude();

				return true;
			}
		case SUB_GP_USER_INDIVIDUAL:	//������Ϣ
			{
//				MessageBox(_T("SUB_GP_USER_INDIVIDUAL"));
				//Ч�����
				ASSERT(wDataSize>=sizeof(CMD_GP_UserIndividual));
				if (wDataSize<sizeof(CMD_GP_UserIndividual)) return false;

				//��������
				CMD_GP_UserIndividual * pUserIndividual=(CMD_GP_UserIndividual *)pData;

				//��������
				CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
				tagIndividualUserData * pIndividualUserData=pGlobalUserInfo->GetIndividualUserData();

				//���ñ���
				pIndividualUserData->dwUserID=pUserIndividual->dwUserID;
				
				//��������
				VOID * pDataBuffer=NULL;
				tagDataDescribe DataDescribe;
				CRecvPacketHelper RecvPacket(pUserIndividual+1,wDataSize-sizeof(CMD_GP_UserIndividual));

				//��չ��Ϣ
				while (true)
				{
					pDataBuffer=RecvPacket.GetData(DataDescribe);
					if (DataDescribe.wDataDescribe==DTP_NULL) break;
					switch (DataDescribe.wDataDescribe)
					{
					case DTP_GP_UI_USER_NOTE:		//�û���ע
						{
							lstrcpyn(pIndividualUserData->szUserNote,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szUserNote));
							break;
						}
					case DTP_GP_UI_COMPELLATION:	//��ʵ����
						{
							lstrcpyn(pIndividualUserData->szCompellation,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szCompellation));
							break;
						}
					case DTP_GP_UI_SEAT_PHONE:		//�̶��绰
						{
							lstrcpyn(pIndividualUserData->szSeatPhone,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szSeatPhone));
							break;
						}
					case DTP_GP_UI_MOBILE_PHONE:	//�ƶ��绰
						{
							lstrcpyn(pIndividualUserData->szMobilePhone,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szMobilePhone));
							break;
						}
					case DTP_GP_UI_QQ:				//Q Q ����
						{
							lstrcpyn(pIndividualUserData->szQQ,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szQQ));
							break;
						}
					case DTP_GP_UI_EMAIL:			//�����ʼ�
						{
							lstrcpyn(pIndividualUserData->szEMail,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szEMail));
							break;
						}
					case DTP_GP_UI_DWELLING_PLACE:	//��ϵ��ַ
						{
							lstrcpyn(pIndividualUserData->szDwellingPlace,(LPCTSTR)pDataBuffer,CountArray(pIndividualUserData->szDwellingPlace));
							break;
						}
					}
				}

				//�ɹ�����
				OnMissionConclude();

				//��������
				UpdateIndividualInfo();

				//�ر�����
				m_MissionManager.ConcludeMissionItem(this,false);

				return true;
			}
		}
	}

	return true;
}

//��ʼ����
VOID CDlgServiceIndividual::OnMissionStart()
{
	//��Ϣ�ؼ�
//	m_edAccounts.EnableWindow(FALSE);
//	m_edNickName.EnableWindow(FALSE);
//	m_edPassword.EnableWindow(FALSE);
//	m_edUnderWrite.EnableWindow(FALSE);

	//��ϸ����
//	m_edQQ.EnableWindow(FALSE);
//	m_edEMail.EnableWindow(FALSE);
//	m_edUserNote.EnableWindow(FALSE);
//	m_edSeatPhone.EnableWindow(FALSE);
//	m_edMobilePhone.EnableWindow(FALSE);
//	m_edCompellation.EnableWindow(FALSE);
//	m_edDwellingPlace.EnableWindow(FALSE);

	//���ܰ�ť
//	m_btOk.EnableWindow(FALSE);
//	m_btSelectFace.EnableWindow(FALSE);

	return;
}

//��ֹ����
VOID CDlgServiceIndividual::OnMissionConclude()
{
	//��Ϣ�ؼ�
	m_edUnderWrite.EnableWindow(FALSE);
	m_edUnderWrite.MoveWindow(107, 210, 325, 35);
//	m_edAccounts.EnableWindow(TRUE);
//	m_edNickName.EnableWindow(TRUE);
//	m_edPassword.EnableWindow(TRUE);
//	m_edUnderWrite.EnableWindow(TRUE);

	//��ϸ����
//	m_edQQ.EnableWindow(TRUE);
//	m_edEMail.EnableWindow(TRUE);
//	m_edUserNote.EnableWindow(TRUE);
//	m_edSeatPhone.EnableWindow(TRUE);
//	m_edMobilePhone.EnableWindow(TRUE);
//	m_edCompellation.EnableWindow(TRUE);
//	m_edDwellingPlace.EnableWindow(TRUE);

	//���ܰ�ť
//	m_btOk.EnableWindow(TRUE);
//	m_btSelectFace.EnableWindow(TRUE);
	m_btUnderWrite.ShowWindow(SW_SHOW);
	m_btUnderWrite.EnableWindow(TRUE);

	m_btOk.ShowWindow(SW_HIDE);

	//���ÿؼ�
//	m_edPassword.SetWindowText(TEXT(""));

	return;
}

//�ʺ���Ϣ
VOID CDlgServiceIndividual::LoadAccountsInfo()
{
	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
	tagIndividualUserData * pIndividualUserData=pGlobalUserInfo->GetIndividualUserData();

	//�û�ͷ��
//	if ((pGlobalUserData->dwCustomID!=0L)&&(pGlobalUserData->CustomFaceInfo.dwDataSize!=0))
//	{
//		tagCustomFaceInfo * pCustomFaceInfo=&pGlobalUserData->CustomFaceInfo;
//		m_FaceItemView.SetCustomFace(pGlobalUserData->dwCustomID,pCustomFaceInfo->dwCustomFace);
//	}
//	else
//	{
//		m_FaceItemView.SetSystemFace(pGlobalUserData->wFaceID);
//	}

	//��������
	SetDlgItemText(IDC_ACCOUNTS,pGlobalUserData->szAccounts);
	SetDlgItemText(IDC_NICKNAME,pGlobalUserData->szNickName);
	SetDlgItemText(IDC_UNDER_WRITE,pGlobalUserData->szUnderWrite);
	TCHAR chGameID[MAX_PATH] = {0};
	_sntprintf(chGameID, MAX_PATH-1, _T("%ld"), pGlobalUserData->dwGameID);
	SetDlgItemText(IDC_GAMEID, chGameID);

	//�û��Ա�
	if (pGlobalUserData->cbGender==GENDER_FEMALE)
	{
		SetDlgItemText(IDC_GENDER, _T("Ů"));
	}
	if (pGlobalUserData->cbGender==GENDER_MALE)
	{
		SetDlgItemText(IDC_GENDER, _T("��"));
	}

	// ��Ա��Ϣ
	SetDlgItemText(IDC_MEMBER, theApp.m_UserOrderParserModule.GetInterface()->GetMemberDescribe(pGlobalUserData->cbMemberOrder) );

	// ����ǩ��
	m_edUnderWrite.EnableWindow(FALSE);
	SetDlgItemText(IDC_UNDER_WRITE, pGlobalUserData->szUnderWrite);
	m_edUnderWrite.MoveWindow(107, 210, 325, 35);

	// ������
	TCHAR szUserLoveLiness[MAX_PATH] = {0};
	_sntprintf(szUserLoveLiness, MAX_PATH-1, _T("%ld"), pGlobalUserData->dwLoveLiness);
	SetDlgItemText(IDC_USERMEDAL, szUserLoveLiness);

	//��ϸ����
	if (pIndividualUserData->dwUserID==0L)
	{
		QueryIndividualInfo();
	}
	else
	{
		UpdateIndividualInfo();
	}

	return;
}

//��ѯ����
VOID CDlgServiceIndividual::QueryIndividualInfo()
{
	//���ñ���
	m_bQueryMission=true;
	m_bModifyMission=false;

	//�¼�����
	OnMissionStart();

	//��������
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}

//��������
VOID CDlgServiceIndividual::UpdateIndividualInfo()
{
	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagIndividualUserData * pIndividualUserData=pGlobalUserInfo->GetIndividualUserData();

	//�û�����
//	SetDlgItemText(IDC_USER_NOTE,pIndividualUserData->szUserNote);
//	SetDlgItemText(IDC_COMPELLATION,pIndividualUserData->szCompellation);

	//�绰����
//	SetDlgItemText(IDC_SEAT_PHONE,pIndividualUserData->szSeatPhone);
//	SetDlgItemText(IDC_MOBILE_PHONE,pIndividualUserData->szMobilePhone);

	//��ϵ����
//	SetDlgItemText(IDC_QQ,pIndividualUserData->szQQ);
//	SetDlgItemText(IDC_EMAIL,pIndividualUserData->szEMail);
//	SetDlgItemText(IDC_DWELLING_PLACE,pIndividualUserData->szDwellingPlace);

	return;
}

//�滭����
BOOL CDlgServiceIndividual::OnEraseBkgnd(CDC * pDC)
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
	CPngImage ImageServiceIndividual;
	ImageServiceIndividual.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_INDIVIDUAL"));

	//��䱳��
	pBufferDC->FillSolidRect(&rcClient,RGB(255,255,255));

	//�滭����
	ImageServiceIndividual.DrawImage(pBufferDC,0,0,ImageServiceIndividual.GetWidth(),ImageServiceIndividual.GetHeight(),0,0);

	// ����ͷ��
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
	tagIndividualUserData * pIndividualUserData=pGlobalUserInfo->GetIndividualUserData();
	if (pGlobalUserData->CustomFaceInfo.dwDataSize!=0)
	{
		DWORD * pdwCustomFace=pGlobalUserData->CustomFaceInfo.dwCustomFace;
		theApp.m_FaceItemControlModule->DrawFaceNormal(pBufferDC, 352, 14, 60, 60, pdwCustomFace);
	}
	else
	{
		theApp.m_FaceItemControlModule->DrawFaceNormal(pBufferDC, 352, 14, 60, 60, pGlobalUserData->wFaceID);
	}

	TCHAR szUserLoveLiness[MAX_PATH] = {0};
	_sntprintf(szUserLoveLiness, MAX_PATH-1, _T("%ld"), pGlobalUserData->dwLoveLiness);
	SetDlgItemText(IDC_USERMEDAL, szUserLoveLiness);


	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	return TRUE;
}

//ͷ��ѡ��
VOID CDlgServiceIndividual::OnBnClickedSelectFace()
{
	//��������
	CDlgCustomFace DlgCustomFace;
	DlgCustomFace.SetCustomFaceEvent(QUERY_OBJECT_PTR_INTERFACE(CPlatformFrame::GetInstance(),IUnknownEx));

	//��ʾ����
	if (DlgCustomFace.DoModal()==IDCANCEL)
	{
		return;
	}

	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
	tagIndividualUserData * pIndividualUserData=pGlobalUserInfo->GetIndividualUserData();

	//�û�ͷ��
	if ((pGlobalUserData->dwCustomID!=0L)&&(pGlobalUserData->CustomFaceInfo.dwDataSize!=0))
	{
		tagCustomFaceInfo * pCustomFaceInfo=&pGlobalUserData->CustomFaceInfo;
//		m_FaceItemView.SetCustomFace(pGlobalUserData->dwCustomID,pCustomFaceInfo->dwCustomFace);
	}
	else
	{
//		m_FaceItemView.SetSystemFace(pGlobalUserData->wFaceID);
	}

	return;
}

// �༭����ǩ��
VOID CDlgServiceIndividual::OnBnClickedUnderWrite()
{
	// �༭���Ϊ����״̬(λ���ƶ�)
	m_edUnderWrite.EnableWindow(TRUE);
	m_edUnderWrite.MoveWindow(90, 200, 320, 45);

	// �༭��ť����
	m_btUnderWrite.ShowWindow(SW_HIDE);

	// �ύ��ť��ʾ
	m_btOk.EnableWindow(TRUE);
	m_btOk.ShowWindow(SW_SHOW);
}

// �ύ����ǩ��
VOID CDlgServiceIndividual::OnBnClickedSubmit()
{
//	MessageBox(_T("CDlgServiceIndividual::OnBnClickedSubmit"));
	// �����ύ��ť
	m_btOk.EnableWindow(FALSE);

	// ��ȡ��Ϣ���ύ
	GetControlItemText(IDC_UNDER_WRITE,m_szUnderWrite,CountArray(m_szUnderWrite));

	//���ñ���
	m_bQueryMission=false;
	m_bModifyMission=true;

	//�¼�����
	OnMissionStart();

	//��������
	if (m_MissionManager.AvtiveMissionItem(this,false)==false) 
	{
		OnMissionConclude();
		return;
	}
}

// �һ�����
VOID CDlgServiceIndividual::OnBnClickedExchangeLoveliness()
{
	// �����һ���
	CDlgExchangeLoveliness DlgExchangeLoveliness;
	INT_PTR nResult=DlgExchangeLoveliness.DoModal();
}

//////////////////////////////////////////////////////////////////////////////////
