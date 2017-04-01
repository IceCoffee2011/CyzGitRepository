#include "StdAfx.h"
#include "GlobalUnits.h"
#include "MissionLogon.h"
#include "PlatformEvent.h"
#include "PlatformFrame.h"
#include "DLGMBCard.h"

//////////////////////////////////////////////////////////////////////////////////

//��̬����
CMissionLogon * CMissionLogon::m_pMissionLogon=NULL;					//����ָ��

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMissionLogon, CDlgStatus)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CMissionLogon::CMissionLogon()
{
	//���ñ���
	m_bRegister=false;
	m_bProfile=false;
	m_bRemPassword=false;
	m_bLowVerValidate=true;

	//�ؼ�ָ��
	m_pDlgLogon=NULL;
	m_pDlgRegister=NULL;
	m_pDlgProfile = NULL;

	//�������
	SetStatusViewSink(this);

	//���ö���
	ASSERT(m_pMissionLogon==NULL);
	if (m_pMissionLogon==NULL) m_pMissionLogon=this;

	return;
}

//��������
CMissionLogon::~CMissionLogon()
{
	//ɾ������
	SafeDelete(m_pDlgLogon);
	SafeDelete(m_pDlgRegister);
	SafeDelete(m_pDlgProfile);

	//�ͷŶ���
	ASSERT(m_pMissionLogon==this);
	if (m_pMissionLogon==this) m_pMissionLogon=NULL;



	return;
}

//ȡ������
VOID CMissionLogon::OnStatusCancel()
{
	//���ؽ���
	HideStatusWindow();

	//��ֹ����
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,true);

	//���µ�¼
	if (m_bRegister==false)
	{
		ShowLogon();
	}
	else
	{
		ShowRegister();
	}

	return;
}

//�����¼�
bool CMissionLogon::OnEventMissionLink(INT nErrorCode)
{
	LOGI("CDlgLogon::OnEventMissionLink, Step 1="<<GetTickCount());
	//������
	if (nErrorCode!=0L)
	{
		//�ر���ʾ
		HideStatusWindow();
		LOGI("CDlgLogon::OnEventMissionLink, Step 2="<<GetTickCount());

		//��ʾ��ʾ
		CInformation Information(CPlatformFrame::GetInstance());
		Information.ShowMessageBox(TEXT("���������еķ��������޷��ɹ����ӷ���������������վά�����棡"),MB_ICONERROR);

		//���µ�¼
		if (m_bRegister==false)
		{
			ShowLogon();
		}
		else
		{
			ShowRegister();
		}
	}
	else
	{
		LOGI("CDlgLogon::OnEventMissionLink, Step 3="<<GetTickCount());
		//��������
		if(m_bLowVerValidate)
			SendLogonPacket(MB_VALIDATE_FLAGS|LOW_VER_VALIDATE_FLAGS);
		else
			SendLogonPacket(MB_VALIDATE_FLAGS);

		LOGI("CDlgLogon::OnEventMissionLink, Step 4="<<GetTickCount());
	}

	LOGI("CDlgLogon::OnEventMissionLink, Step 5="<<GetTickCount());

	return true;
}

//�ر��¼�
bool CMissionLogon::OnEventMissionShut(BYTE cbShutReason)
{
	//�رմ���
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//��������
		CMissionManager * pMissionManager=GetMissionManager();
		if (pMissionManager->AvtiveMissionItem(this,true)==true) return true;

		//�ر���ʾ
		HideStatusWindow();

		//��ʾ��ʾ
		CInformation Information(CPlatformFrame::GetInstance());
		Information.ShowMessageBox(TEXT("���������еķ��������޷��ɹ����ӷ���������������վά��������Ժ������ԣ�"),MB_ICONERROR);

		//���µ�¼
		if (m_bRegister==false)
		{
			ShowLogon();
		}
		else
		{
			ShowRegister();
		}
	}

	return true;
}

//��ȡ�¼�
bool CMissionLogon::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
	if (Command.wMainCmdID==MDM_GP_LOGON)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_LOGON_SUCCESS:		//��¼�ɹ�
			{
				return OnSocketSubLogonSuccess(pData,wDataSize);
			}
		case SUB_GP_LOGON_FAILURE:		//��¼ʧ��
			{
				return OnSocketSubLogonFailure(pData,wDataSize);
			}
		case SUB_GP_LOGON_FINISH:		//��¼���
			{
				return OnSocketSubLogonFinish(pData,wDataSize);
			}
		case SUB_GP_VALIDATE_MBCARD:	//��¼ʧ��
			{
				return OnSocketSubLogonValidateMBCard(pData,wDataSize);
			}
		case SUB_GP_UPDATE_NOTIFY:		//������ʾ
			{
				return OnSocketSubUpdateNotify(pData,wDataSize);
			}
		case SUB_GP_PROFILE_SUCCESS:
			{
				return OnSocketSubProfileSuccess(pData,wDataSize);
			}
		case SUB_GP_PROFILE_FAILURE:
			{
				return OnSocketSubProfileFailure(pData,wDataSize);
			}
		case SUB_GP_CHECKNICK_RES:
			{
				return OnSocketSubCheckNick(pData,wDataSize);
			}
		}
	}

	return false;
}

//��ʾ��¼
VOID CMissionLogon::ShowLogon()
{
	//Ч��״̬
	ASSERT(GetActiveStatus()==false);
	if (GetActiveStatus()==true) return;

	//���ñ���
	m_bRegister=false;

	//��������
	if (m_pDlgLogon==NULL)
	{
		m_pDlgLogon=new CDlgLogon;
	}

	//��������
	if (m_pDlgLogon->m_hWnd==NULL)
	{
		m_pDlgLogon->Create(IDD_DLG_LOGON,GetDesktopWindow());
	}

	//��ʾ����
	m_pDlgLogon->ShowWindow(SW_SHOW);
	m_pDlgLogon->SetForegroundWindow();

	//���ش���
	if ((m_pDlgRegister!=NULL)&&(m_pDlgRegister->m_hWnd!=NULL))
	{
		m_pDlgRegister->ShowWindow(SW_HIDE);
	}

	m_pDlgLogon->ShowWindow(SW_SHOW);
	m_pDlgLogon->SetForegroundWindow();
	return;
}

//��ʾע��
VOID CMissionLogon::ShowRegister()
{
	//Ч��״̬
	ASSERT(GetActiveStatus()==false);
	if (GetActiveStatus()==true) return;

	//���ñ���
	m_bRegister=true;

	//��������
	if (m_pDlgRegister==NULL)
	{
		m_pDlgRegister=new CDlgRegister;
	}
	
	//��������
	if (m_pDlgRegister->m_hWnd==NULL)
	{
		m_pDlgRegister->Create(IDD_DLG_REGISTER,GetDesktopWindow());
	}

	//��ʾ����
	m_pDlgRegister->ShowWindow(SW_SHOW);
	m_pDlgRegister->SetForegroundWindow();

	//���ش���
	if ((m_pDlgLogon!=NULL)&&(m_pDlgLogon->m_hWnd!=NULL))
	{
		m_pDlgLogon->ShowWindow(SW_HIDE);
	}

	return;
}

//��ʾ������Ϣ
VOID CMissionLogon::ShowProfile()
{
	//Ч��״̬
	ASSERT(GetActiveStatus()==false);
	if (GetActiveStatus()==true) return;

	//���ñ���
	m_bProfile=true;

	if(m_pDlgProfile == NULL)
	{
		m_pDlgProfile = new CDlgProfile;
	}

	//��������
	if (m_pDlgProfile->m_hWnd==NULL)
	{
		m_pDlgProfile->Create(IDD_DLG_PROFILE,GetDesktopWindow());
	}

	//��ʾ����
	m_pDlgProfile->ShowWindow(SW_SHOW);
	m_pDlgProfile->SetForegroundWindow();

	return;
}

//��¼�û�
VOID CMissionLogon::UpdateUserInfo()
{
	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//���ñ�־
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
	if (pParameterGlobal!=NULL) pParameterGlobal->m_bRemberPassword=m_bRemPassword;

	//������Ϣ
	TCHAR szUserID[16];
	_sntprintf(szUserID,CountArray(szUserID),TEXT("%ld"),pGlobalUserData->dwUserID);

	//�ϴ��û�
	CWHRegKey RegUserInfo;
	RegUserInfo.OpenRegKey(REG_USER_INFO,true);
	RegUserInfo.WriteValue(TEXT("LastUserID"),pGlobalUserData->dwUserID);

	//��������
	TCHAR szGameID[16]=TEXT("");
	TCHAR szPassBuffer[MAX_ENCRYPT_LEN]=TEXT("");

	//�û�����
	CWHRegKey RegUserItem;
	RegUserItem.Attach(RegUserInfo.CreateItemKey(szUserID));

	//��Ϸ��ʶ
	if (pGlobalUserData->dwGameID!=0L)
	{
		_sntprintf(szGameID,CountArray(szGameID),TEXT("%ld"),pGlobalUserData->dwGameID);
	}

	//��������
	if (m_bRemPassword==true)
	{
		if (m_bRegister==false)
		{
			LPCTSTR pszPassword=m_pDlgLogon->m_szPassword;
			CWHEncrypt::XorEncrypt(pszPassword,szPassBuffer,CountArray(szPassBuffer));
		}
		else
		{
			LPCTSTR pszPassword=m_pDlgRegister->m_szLogonPass;
			CWHEncrypt::XorEncrypt(pszPassword,szPassBuffer,CountArray(szPassBuffer));
		}
	}

	//������Ϣ
	RegUserItem.WriteValue(TEXT("FaceID"),pGlobalUserData->wFaceID);
	RegUserItem.WriteValue(TEXT("AcitveTimeToken"),(DWORD)time(NULL));

	//�ʺ���Ϣ
	RegUserItem.WriteString(TEXT("GameID"),szGameID);
	RegUserItem.WriteString(TEXT("UserPassword"),szPassBuffer);
	RegUserItem.WriteString(TEXT("UserAccount"),pGlobalUserData->szAccounts);
	RegUserItem.WriteString(TEXT("UserPassToken"),pGlobalUserData->szPassword);

	return;
}

//ִ�е�¼
bool CMissionLogon::PerformLogonMission(bool bRemPassword)
{
	LOGI("CDlgLogon::PerformLogonMission, Step 1="<<GetTickCount());
	//Ч��״̬
	ASSERT(GetActiveStatus()==false);
	if (GetActiveStatus()==true) return false;

	//��ȡ��ַ
	TCHAR szCustomServer[LEN_SERVER]=TEXT("");
	if (m_pDlgLogon!=NULL) lstrcpyn(szCustomServer,m_pDlgLogon->m_szLogonServer,CountArray(szCustomServer));

	//���õ�ַ
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->SetCustomServer(szCustomServer);

	//��������
	LOGI("CDlgLogon::PerformLogonMission, Step 2="<<GetTickCount());
	if (pMissionManager->AvtiveMissionItem(this,false)==false)
	{
		LOGI("CDlgLogon::PerformLogonMission, Step 3="<<GetTickCount());
		//������ʾ
		CInformation Information(CPlatformFrame::GetInstance());
		Information.ShowMessageBox(TEXT("����������ʧ�ܣ��������������Լ���������״����"),MB_ICONERROR);

		//���µ�¼
		if (m_bRegister==false)
		{
			ShowLogon();
		}
		else
		{
			ShowRegister();
		}

		return false;
	}
	LOGI("CDlgLogon::PerformLogonMission, Step 4="<<GetTickCount());

	//���ñ���
	m_bRemPassword=bRemPassword;

	//д���ַ
	CGlobalServer GlobalServer;
	GlobalServer.SetLastServerName(szCustomServer);
	LOGI("CDlgLogon::PerformLogonMission, Step 5="<<GetTickCount());

	//��ʾ����
	ShowStatusWindow(TEXT("�������ӷ��������������Ժ�Ƭ��..."));

	LOGI("CDlgLogon::PerformLogonMission, Step 6="<<GetTickCount());

	return true;
}

//��¼�ɹ�
bool CMissionLogon::OnSocketSubLogonSuccess(VOID * pData, WORD wDataSize)
{
	LOGI("CMissionLogon::OnSocketSubLogonSuccess, Step 1="<<GetTickCount());
	//Ч������
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonSuccess));
	if (wDataSize<sizeof(CMD_GP_LogonSuccess)) return false;

	//��Ϣ����
	CMD_GP_LogonSuccess * pLogonSuccess=(CMD_GP_LogonSuccess *)pData;

	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//������Ϣ
	pGlobalUserData->wFaceID=pLogonSuccess->wFaceID;
	pGlobalUserData->cbGender=pLogonSuccess->cbGender;
	pGlobalUserData->dwGameID=pLogonSuccess->dwGameID;
	pGlobalUserData->dwUserID=pLogonSuccess->dwUserID;
	pGlobalUserData->dwCustomID=pLogonSuccess->dwCustomID;
	pGlobalUserData->dwUserMedal=pLogonSuccess->dwUserMedal;
	pGlobalUserData->dwExperience=pLogonSuccess->dwExperience;
	pGlobalUserData->dwLoveLiness=pLogonSuccess->dwLoveLiness;
	pGlobalUserData->cbMoorMachine=pLogonSuccess->cbMoorMachine;
	pGlobalUserData->wFillIn= pLogonSuccess->wFillIn;

	//�û���Ϸ��
	pGlobalUserData->lUserScore=pLogonSuccess->lUserScore;
	pGlobalUserData->lUserInsure=pLogonSuccess->lUserInsure;

	//��ʾ����
	pParameterGlobal->m_bShowServerStatus=(pLogonSuccess->cbShowServerStatus!=0);

	//�ʺ���Ϣ
	lstrcpyn(pGlobalUserData->szAccounts,pLogonSuccess->szAccounts,CountArray(pGlobalUserData->szAccounts));
	lstrcpyn(pGlobalUserData->szNickName,pLogonSuccess->szNickName,CountArray(pGlobalUserData->szNickName));

	//��չ��Ϣ
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pLogonSuccess+1,wDataSize-sizeof(CMD_GP_LogonSuccess));
	while (true)
	{
		//��ȡ����
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		//��ȡ����
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_GP_MEMBER_INFO:	//��Ա��Ϣ
			{
				ASSERT(DataDescribe.wDataSize==sizeof(DTP_GP_MemberInfo));
				if (DataDescribe.wDataSize==sizeof(DTP_GP_MemberInfo))
				{
					DTP_GP_MemberInfo * pMemberInfo=(DTP_GP_MemberInfo *)pDataBuffer;
					pGlobalUserData->cbMemberOrder=pMemberInfo->cbMemberOrder;
					pGlobalUserData->MemberOverDate=pMemberInfo->MemberOverDate;
				}

				break;
			}
		case DTP_GP_UNDER_WRITE:	//����ǩ��
			{
				ASSERT(DataDescribe.wDataSize<=sizeof(pGlobalUserData->szUnderWrite));
				if (DataDescribe.wDataSize<=sizeof(pGlobalUserData->szUnderWrite))
				{
					pGlobalUserData->szUnderWrite[DataDescribe.wDataSize/sizeof(TCHAR)-1L]=0;
					CopyMemory(pGlobalUserData->szUnderWrite,pDataBuffer,DataDescribe.wDataSize);
				}

				break;
			}
		}
	}

    //���ؼ���
	if (pLogonSuccess->dwCustomID!=0)
	{
		CCustomFaceManager * pCustomFaceManager=CCustomFaceManager::GetInstance();
		pCustomFaceManager->LoadUserCustomFace(pLogonSuccess->dwUserID,pLogonSuccess->dwCustomID,pGlobalUserData->CustomFaceInfo);
	}


	if(pLogonSuccess->wFillIn > 0)
	{
		// ������ʾ
		ShowStatusWindow(TEXT("���ڻ�ȡ��Ϸ�б�..."));	
	}
	else
	{

	}
	LOGI("CMissionLogon::OnSocketSubLogonSuccess, Step 2="<<GetTickCount());

	return true;
}

//��¼ʧ��
bool CMissionLogon::OnSocketSubLogonFailure(VOID * pData, WORD wDataSize)
{
	//��������
	CMD_GP_LogonFailure * pLogonFailure=(CMD_GP_LogonFailure *)pData;

	//Ч������
	ASSERT(wDataSize>=(sizeof(CMD_GP_LogonFailure)-sizeof(pLogonFailure->szDescribeString)));
	if (wDataSize<(sizeof(CMD_GP_LogonFailure)-sizeof(pLogonFailure->szDescribeString))) return false;

	//�رմ���
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

	//���Ƚ���
	HideStatusWindow();

	//��ʾ��Ϣ
	LPCTSTR pszDescribeString(pLogonFailure->szDescribeString);
	if (lstrlen(pszDescribeString)>0) ShowInformation(pszDescribeString,MB_ICONERROR,60);

	//���µ�¼
	if (m_bRegister==false)
	{
		ShowLogon();
	}
	else
	{
		ShowRegister();
	}

	return true;
}

//��¼���
bool CMissionLogon::OnSocketSubLogonFinish(VOID * pData, WORD wDataSize)
{
	LOGI("CMissionLogon::OnSocketSubLogonFinish, Step 1="<<GetTickCount());
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GP_LogonFinish));
	if (wDataSize!=sizeof(CMD_GP_LogonFinish)) return false;

	//��������
	CMD_GP_LogonFinish * pLogonFinish=(CMD_GP_LogonFinish *)pData;

	//��������
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//ʱ������
	pParameterGlobal->m_wIntermitTime=pLogonFinish->wIntermitTime;
	pParameterGlobal->m_wOnLineCountTime=pLogonFinish->wOnLineCountTime;

	LOGI("CMissionLogon::OnSocketSubLogonFinish, Step 2="<<GetTickCount());

	//������Ϣ
	UpdateUserInfo();

	//�رմ���
	HideStatusWindow();

	LOGI("CMissionLogon::OnSocketSubLogonFinish, Step 3="<<GetTickCount());

	//ɾ���ؼ�
	//SafeDelete(m_pDlgLogon);
	//SafeDelete(m_pDlgRegister);

	////����ҳ
	//ASSERT(CPlatformFrame::GetInstance()!=NULL);
	//CPlatformFrame::GetInstance()->WebBrowse(szPlatformLink,false);

	//��ֹ����
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	// ��Ҫ��������
	if(pGlobalUserData->wFillIn <= 0)
	{
		ShowProfile();
		return true;
	}

	LOGI("CMissionLogon::OnSocketSubLogonFinish, Step 4="<<GetTickCount());

	SafeDelete(m_pDlgLogon);
	SafeDelete(m_pDlgRegister);
	SafeDelete(m_pDlgProfile);

	//����ҳ
	ASSERT(CPlatformFrame::GetInstance()!=NULL);
	CPlatformFrame::GetInstance()->WebBrowse(szPlatformLink,false);

	//�����¼�
	CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
	if (pPlatformEvent!=NULL) pPlatformEvent->SendPlatformEvent(EVENT_USER_LOGON,0L);

	//��������
	//CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	//tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//�������
	if ((pGlobalUserData->dwCustomID!=0)&&(pGlobalUserData->CustomFaceInfo.dwDataSize==0L))
	{
		CCustomFaceManager * pCustomFaceManager=CCustomFaceManager::GetInstance();
		pCustomFaceManager->LoadUserCustomFace(pGlobalUserData->dwUserID,pGlobalUserData->dwCustomID);
	}
	LOGI("CMissionLogon::OnSocketSubLogonFinish, Step 5="<<GetTickCount());
	return true;
}

//������ʾ
bool CMissionLogon::OnSocketSubUpdateNotify(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GP_UpdateNotify));
	if (wDataSize!=sizeof(CMD_GP_UpdateNotify)) return false;

	//���ñ���
	CMD_GP_UpdateNotify * pUpdateNotify=(CMD_GP_UpdateNotify *)pData;

	//��ֹ����
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

	//���ش���
	HideStatusWindow();

	//��ʾ��Ϣ
	if ((pUpdateNotify->cbMustUpdate==TRUE)||(pUpdateNotify->cbAdviceUpdate==TRUE))
	{
		//������ʾ
		if (pUpdateNotify->cbMustUpdate==FALSE)
		{
			//������ʾ
			LPCTSTR pszString=TEXT("��Ϸ�����������������ڵİ汾���ܼ���ʹ�ã��������������ظ��£�");

			//�˳��ж�
			CInformation Information(CPlatformFrame::GetInstance());
			if (Information.ShowMessageBox(pszString,MB_ICONINFORMATION|MB_YESNO,0)!=IDYES)
			{
				m_bLowVerValidate=false;
				PerformLogonMission(m_bRemPassword);
				return true;
			}
		}
	}

	//���ش���
	CGlobalUnits * pGlobalUnits=CGlobalUnits::GetInstance();
	if (pGlobalUnits!=NULL) pGlobalUnits->DownLoadClient(TEXT("��Ϸ����"),0,0);

	return true;
}

//������Ϣ
bool CMissionLogon::SendLogonPacket(BYTE cbValidateFlags)
{
		//��������
	WORD wPacketSize,wSubCmdID;
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	//��������
	if(m_bProfile)	// add by chenj
	{ 
		wPacketSize = m_pDlgProfile->ConstructProfilePacket(cbBuffer,sizeof(cbBuffer),wSubCmdID, cbValidateFlags);		
		GetMissionManager()->SendData(MDM_GP_LOGON,wSubCmdID,cbBuffer,wPacketSize);
	
		return true;
	}
	else if (m_bRegister==false)
	{
		wPacketSize=m_pDlgLogon->ConstructLogonPacket(cbBuffer,sizeof(cbBuffer), cbValidateFlags);
		wSubCmdID=(m_pDlgLogon->m_cbLogonMode==LOGON_BY_GAME_ID)?SUB_GP_LOGON_GAMEID:SUB_GP_LOGON_ACCOUNTS;
	}
	else
	{
		wSubCmdID=SUB_GP_REGISTER_ACCOUNTS;
		wPacketSize=m_pDlgRegister->ConstructRegisterPacket(cbBuffer,sizeof(cbBuffer), cbValidateFlags);
	}

	//��ʾ��Ϣ
	ShowStatusWindow(TEXT("������֤�û���¼��Ϣ..."));

	//��������
	ASSERT(GetMissionManager()!=NULL);
	GetMissionManager()->SendData(MDM_GP_LOGON,wSubCmdID,cbBuffer,wPacketSize);
//	for (int i = 0; i < 100; i++)
//		GetMissionManager()->SendData(34,i);

	return true;
}

//��¼ʧ��
bool CMissionLogon::OnSocketSubLogonValidateMBCard(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GP_ValidateMBCard));
	if (wDataSize!=sizeof(CMD_GP_ValidateMBCard)) return false;

	//���Ƚ���
	HideStatusWindow();

	//��Ϣ����
	CMD_GP_ValidateMBCard * pValidateMBCard=(CMD_GP_ValidateMBCard *)pData;

	//��Ϣ����
	CDLGMBCard DLGMBCard;
	DLGMBCard.SetMBCard(pValidateMBCard->uMBCardID);
	if (IDOK==DLGMBCard.DoModal())
	{
		SendLogonPacket(0);
	}
	else
	{
		//��������
		GetMissionManager()->ConcludeMissionItem(this, true);

		//���µ�¼
		if (m_bRegister==false)
		{
			ShowLogon();
		}
		else
		{
			ShowRegister();
		}
	}

	return true;
}

bool CMissionLogon::OnSocketSubProfileSuccess(VOID * pData, WORD wDataSize)
{
	m_bProfile = false;

	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GP_UserProfile));
	if (wDataSize!=sizeof(CMD_GP_UserProfile)) return false;

	//��������
	CMD_GP_UserProfile * pUserProfile=(CMD_GP_UserProfile *)pData;

	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	pGlobalUserData->cbGender = pUserProfile->cbGender;
	pGlobalUserData->wFaceID = pUserProfile->wFaceID;
	pGlobalUserData->wFillIn = 1;
	lstrcpyn(pGlobalUserData->szNickName,pUserProfile->szNickName,CountArray(pGlobalUserData->szNickName));
	lstrcpyn(pGlobalUserData->szUnderWrite,pUserProfile->szUnderWrite,CountArray(pGlobalUserData->szUnderWrite));	

	tagIndividualUserData * pGlobalIndividualData= pGlobalUserInfo->GetIndividualUserData();
	lstrcpyn(pGlobalIndividualData->szQQ,pUserProfile->szQQ,CountArray(pGlobalIndividualData->szQQ));
	lstrcpyn(pGlobalIndividualData->szMobilePhone,pUserProfile->szMobile,CountArray(pGlobalIndividualData->szMobilePhone));
	
	//�رմ���
	HideStatusWindow();

	SafeDelete(m_pDlgLogon);
	SafeDelete(m_pDlgRegister);
	SafeDelete(m_pDlgProfile);
	
	//����ҳ
	ASSERT(CPlatformFrame::GetInstance()!=NULL);
	CPlatformFrame::GetInstance()->WebBrowse(szPlatformLink,false);

	//�����¼�
	CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
	if (pPlatformEvent!=NULL) pPlatformEvent->SendPlatformEvent(EVENT_USER_LOGON,0L);

	//�������
	if ((pGlobalUserData->dwCustomID!=0)&&(pGlobalUserData->CustomFaceInfo.dwDataSize==0L))
	{
		CCustomFaceManager * pCustomFaceManager=CCustomFaceManager::GetInstance();
		pCustomFaceManager->LoadUserCustomFace(pGlobalUserData->dwUserID,pGlobalUserData->dwCustomID);
	}

	return true;
}

bool CMissionLogon::OnSocketSubProfileFailure(VOID * pData, WORD wDataSize)
{
	//��������
	CMD_GP_LogonFailure * pLogonFailure=(CMD_GP_LogonFailure *)pData;

	//Ч������
	ASSERT(wDataSize>=(sizeof(CMD_GP_LogonFailure)-sizeof(pLogonFailure->szDescribeString)));
	if (wDataSize<(sizeof(CMD_GP_LogonFailure)-sizeof(pLogonFailure->szDescribeString))) return false;

	//�رմ���
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

	//���Ƚ���
	HideStatusWindow();

	//��ʾ��Ϣ
	LPCTSTR pszDescribeString(pLogonFailure->szDescribeString);
	if (lstrlen(pszDescribeString)>0) ShowInformation(pszDescribeString,MB_ICONERROR,60);

	//���µ�¼
	if(m_bProfile)
	{
		ShowProfile();
	}
	else if (m_bRegister==false)
	{
		ShowLogon();
	}
	else
	{
		ShowRegister();
	}

	return true;
}

//����ǳ�
bool CMissionLogon::OnSocketSubCheckNick(VOID * pData, WORD wDataSize)
{
	m_pDlgProfile->m_bCheckNickFlag = FALSE;

	//��������
	CMD_GP_CheckNick_Res * pCheckNick=(CMD_GP_CheckNick_Res *)pData;

	//�رմ���
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

	//���Ƚ���
	HideStatusWindow();

	//��ʾ��Ϣ
	LPCTSTR pszDescribeString(pCheckNick->szDescribeString);

	if(pCheckNick->lResultCode == DB_SUCCESS && lstrlen(pszDescribeString)>0)
		ShowInformation(pszDescribeString,MB_ICONWARNING,60);
	else if (lstrlen(pszDescribeString) > 0)
		ShowInformation(pszDescribeString,MB_ICONERROR,60);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
