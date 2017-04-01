// DlgExchangeLoveliness.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "resource.h"
#include "DlgExchangeLoveliness.h"


// CDlgExchangeLoveliness �Ի���

//IMPLEMENT_DYNAMIC(CDlgExchangeLoveliness, CDialog)
CDlgExchangeLoveliness::CDlgExchangeLoveliness()
	: CSkinDialog(IDD_DLG_EXCHANGE_LOVELINESS)
{
	m_bQueryMission = false;

	//��������
	m_MissionManager.InsertMissionItem(this);

	ZeroMemory(m_szExchangeInfo, sizeof(m_szExchangeInfo));
	ZeroMemory(m_szTips, sizeof(m_szTips));
}

CDlgExchangeLoveliness::~CDlgExchangeLoveliness()
{
}

void CDlgExchangeLoveliness::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, m_btExchange);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

// CDlgExchangeLoveliness ��Ϣ�������
BEGIN_MESSAGE_MAP(CDlgExchangeLoveliness, CSkinDialog)

END_MESSAGE_MAP()

//��������
BOOL CDlgExchangeLoveliness::OnInitDialog()
{
	__super::OnInitDialog();

	// ���ñ���
	SetWindowText(TEXT("�����һ�"));

	HINSTANCE hInstance=AfxGetInstanceHandle();

	// ������ť
	m_btExchange.SetButtonImage(IDB_BT_OK,  hInstance, false, false);
	m_btCancel.SetButtonImage(IDB_BT_CANCEL, hInstance, false, false);

	CRect rcExchange;
	m_btExchange.GetClientRect(rcExchange);
	m_btExchange.MoveWindow(125, 100, rcExchange.Width(), rcExchange.Height());

	CRect rcCancel;
	m_btCancel.GetClientRect(rcCancel);
	m_btCancel.MoveWindow(240, 100, rcCancel.Width(), rcCancel.Height());

	// ���öһ���ť
	m_btExchange.EnableWindow(FALSE);
	// ��ȡ������Ϣ
	QueryExchangeLovelinessInfo();

	return TRUE;
}

//ȷ������
VOID CDlgExchangeLoveliness::OnOK()
{
	LOGI("CDlgExchangeLoveliness::OnOK");
	//���ñ���
	m_bQueryMission=false;
	m_bExchangeMisssion=true;

	//�¼�����
	OnMissionStart();

	//��������
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}
}
// ȡ��
VOID CDlgExchangeLoveliness::OnCancel()
{
	__super::OnCancel();
}

//��ѯ����
VOID CDlgExchangeLoveliness::QueryExchangeLovelinessInfo()
{
	LOGI("CDlgExchangeLoveliness::QueryExchangeLovelinessInfo");
	//���ñ���
	m_bQueryMission=true;
	m_bExchangeMisssion=false;

	//�¼�����
	OnMissionStart();

	//��������
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}
}

//��ʼ����
VOID CDlgExchangeLoveliness::OnMissionStart()
{
	// ����˫��ť
	m_btExchange.EnableWindow(FALSE);
	m_btCancel.EnableWindow(FALSE);
}

//��ֹ����
VOID CDlgExchangeLoveliness::OnMissionConclude()
{
	m_bQueryMission=false;
	m_bExchangeMisssion=false;

	// ��ť����
	m_btExchange.EnableWindow(TRUE);
	m_btCancel.EnableWindow(TRUE);
}

//�����¼�
bool CDlgExchangeLoveliness::OnEventMissionLink(INT nErrorCode)
{
	LOGI("CDlgExchangeLoveliness::OnEventMissionLink, nErrorCode="<<nErrorCode);
	// ���ӽ��
	if (nErrorCode!=0)
	{
		//��������
		if (m_MissionManager.AvtiveMissionItem(this,true)==true)
		{
			return true;
		}

		//�¼�����
//		OnMissionConclude();

		//������ʾ
 		if (m_bQueryMission==true)
 		{
			m_bQueryMission = false;
 			//��ʾ��Ϣ
 			CInformation Information(this);
 			Information.ShowMessageBox(TEXT("���ӷ�������ʱ����ȡ�����һ�������Ϣʧ�ܣ�"),MB_OK|MB_ICONSTOP,30);
 		}

		//������ʾ
 		if (m_bExchangeMisssion==true)
 		{
			m_bExchangeMisssion = false;
 			//��ʾ��Ϣ
 			CInformation Information(this);
 			Information.ShowMessageBox(TEXT("���ӷ�������ʱ���һ�����ʧ�ܣ�"),MB_OK|MB_ICONSTOP,30);
 		}

		return true;
	}

	if (m_bQueryMission == true)
	{
//		LOGI("m_bQueryMission == true");
		CMD_GP_QueryExchangeLoveliness QueryExchangeLoveliness;
		ZeroMemory(&QueryExchangeLoveliness, sizeof(QueryExchangeLoveliness));

		//��������
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//���ñ���
		QueryExchangeLoveliness.dwUserID=pGlobalUserData->dwUserID;

		//��������
		m_MissionManager.SendData(MDM_GP_USER_SERVICE, SUB_GP_QUERY_EXCHANGE_LOVELINESS, &QueryExchangeLoveliness,sizeof(QueryExchangeLoveliness));
	}

	if (m_bExchangeMisssion == true)
	{
		CMD_GP_ExchangeLoveliness ExchangeLoveliness;

		//��������
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//���ñ���
		ExchangeLoveliness.dwUserID=pGlobalUserData->dwUserID;

		//��������
		m_MissionManager.SendData(MDM_GP_USER_SERVICE, SUB_GP_EXCHANGE_LOVELINESS, &ExchangeLoveliness,sizeof(ExchangeLoveliness));
	}
	return true;
}

//�ر��¼�
bool CDlgExchangeLoveliness::OnEventMissionShut(BYTE cbShutReason)
{
	LOGI("CDlgExchangeLoveliness::OnEventMissionShut, cbShutReason="<<cbShutReason);
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
			m_bQueryMission = false;
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("��������������쳣�Ͽ��ˣ���ȡ�����һ�������Ϣʧ�ܣ�"),MB_ICONERROR,30);
		}

		//�޸�����
		if (m_bExchangeMisssion==true)
		{
			m_bExchangeMisssion = false;
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("��������������쳣�Ͽ��ˣ��һ�����ʧ�ܣ�"),MB_ICONERROR,30);
		}
	}

	OnMissionConclude();
	return true;
}

//��ȡ�¼�
bool CDlgExchangeLoveliness::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
	LOGI("CDlgExchangeLoveliness::OnEventMissionRead, wMainCmdID="<<Command.wMainCmdID<<", wSubCmdID="<<Command.wSubCmdID<<", wDataSize="<<wDataSize);
	//�����
	if (Command.wMainCmdID==MDM_GP_USER_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_EXCHANGE_LOVELINESS_INFO:
			{
				// ��Ϣ����
				CMD_GP_ExchangeLovelinessInfo * pExchangeLovelinessInfo=(CMD_GP_ExchangeLovelinessInfo *)pData;
				LOGI("a = "<< sizeof(CMD_GP_ExchangeLovelinessInfo) << ", b="<<CountStringBuffer(pExchangeLovelinessInfo->szTips)<<", c="<<CountStringBuffer(pExchangeLovelinessInfo->szErr));
				//Ч������
				ASSERT(wDataSize>=(sizeof(CMD_GP_ExchangeLovelinessInfo)-sizeof(pExchangeLovelinessInfo->szTips)-sizeof(pExchangeLovelinessInfo->szErr)));
				if (wDataSize<(sizeof(CMD_GP_ExchangeLovelinessInfo)-sizeof(pExchangeLovelinessInfo->szTips)-sizeof(pExchangeLovelinessInfo->szErr))) return false;

//				LOGI("CDlgExchangeLoveliness::OnEventMissionRead 2");
				//�ر�����
				m_MissionManager.ConcludeMissionItem(this,false);
				OnMissionConclude();

				// �ж��ܷ����ת��
				LOGI("pExchangeLovelinessInfo->szErr="<<pExchangeLovelinessInfo->szErr<<", szTips="<<pExchangeLovelinessInfo->szTips);
				if (pExchangeLovelinessInfo->dwFinalAmount == 0)
				{
					m_btExchange.EnableWindow(FALSE);
					
					lstrcpyn(m_szExchangeInfo,pExchangeLovelinessInfo->szErr,CountArray(pExchangeLovelinessInfo->szErr));
				}
				else
				{
					m_btExchange.EnableWindow(TRUE);

					TCHAR szAvailbleAmount[128]=TEXT("");
					SwitchScoreFormat(pExchangeLovelinessInfo->dwAvailbleAmount, 3, szAvailbleAmount, CountArray(szAvailbleAmount));
					TCHAR szFinalAmount[128]=TEXT("");
					SwitchScoreFormat(pExchangeLovelinessInfo->dwFinalAmount, 3, szFinalAmount, CountArray(szFinalAmount));

					_sntprintf(m_szExchangeInfo, CountArray(m_szExchangeInfo),  TEXT("��ȷ���� %s ������ֵ�һ�Ϊ %s ������"), szAvailbleAmount, szFinalAmount);
				}
				LOGI("CDlgExchangeLoveliness::OnEventMissionRead 3, m_szExchangeInfo="<<m_szExchangeInfo);

				lstrcpyn(m_szTips,pExchangeLovelinessInfo->szTips,CountArray(pExchangeLovelinessInfo->szTips));

				// ˢ�µ�ǰ����
				//���½���
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);
				LOGI("CDlgExchangeLoveliness::OnEventMissionRead 4");
			}
			break;
		case SUB_GP_OPERATE_FAILURE:		//����ʧ��
			{
				if (m_bExchangeMisssion == true)
				{
					LOGI("m_bExchangeMisssion == true");
					// ������ʾ��
					//��������
					CMD_GP_OperateSuccess * pOperateSuccess=(CMD_GP_OperateSuccess *)pData;

					//Ч������
					ASSERT(wDataSize>=(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString)));
					if (wDataSize<(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString))) return false;

					LOGI("m_bExchangeMisssion == 1");

					//�ر�����
					m_MissionManager.ConcludeMissionItem(this,false);

					//�ɹ�����
					OnMissionConclude();

					//��ʾ��Ϣ
					if (pOperateSuccess->szDescribeString[0]!=0)
					{
						CInformation Information(this);
						Information.ShowMessageBox(pOperateSuccess->szDescribeString,MB_ICONINFORMATION,60);
					}

					__super::OnCancel();
				}
			}
			break;
		case SUB_GP_EXCHANGE_LOVELINESS_RESULT:	// �һ��������
			{
				//��������
				CMD_GP_ExchangeLovelinessResult * pExchangeLovelinessResult=(CMD_GP_ExchangeLovelinessResult *)pData;

				//Ч������
				ASSERT(wDataSize>=(sizeof(CMD_GP_ExchangeLovelinessResult)-sizeof(pExchangeLovelinessResult->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_ExchangeLovelinessResult)-sizeof(pExchangeLovelinessResult->szDescribeString))) return false;

				if (pExchangeLovelinessResult->lResultCode==DB_SUCCESS)
				{
					// ������Ϣ
					CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
					tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
					tagIndividualUserData * pIndividualUserData=pGlobalUserInfo->GetIndividualUserData();

					if (pGlobalUserData->dwUserID == pExchangeLovelinessResult->dwUserID)
					{
						pGlobalUserData->dwLoveLiness = pExchangeLovelinessResult->dwLoveliness;
					}
				}
				else
				{

				}

				//�ر�����
				m_MissionManager.ConcludeMissionItem(this,false);

				//�ɹ�����
				OnMissionConclude();

				//��ʾ��Ϣ
				if (pExchangeLovelinessResult->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pExchangeLovelinessResult->szDescribeString,MB_ICONINFORMATION,60);
				}

				__super::OnCancel();
			}
			break;
/*
		case SUB_GP_OPERATE_SUCCESS:		// �����ɹ�
			{
				if (m_bExchangeMisssion == true)
				{
					LOGI("m_bExchangeMisssion == true");
					// ������ʾ��
					//��������
					CMD_GP_OperateSuccess * pOperateSuccess=(CMD_GP_OperateSuccess *)pData;

					//Ч������
					ASSERT(wDataSize>=(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString)));
					if (wDataSize<(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString))) return false;

					LOGI("12312321");
				}
			}
			break;
*/
		default:
			break;
		}
	}

	return true;
}

//�滭��Ϣ
VOID CDlgExchangeLoveliness::OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight)
{
	pDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());
	// ����ת������
	pDC->SetTextColor(RGB(0,0,0));
	pDC->DrawText(m_szExchangeInfo, CRect( CPoint(40, 60), CSize(400, 40) ),DT_EXPANDTABS|DT_NOCLIP|DT_CENTER|DT_WORDBREAK);

	// ������ʾ
	pDC->SetTextColor(RGB(55, 125, 158));
//	pDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());
	pDC->DrawText(m_szTips, CRect( CPoint(40, 160), CSize(400,100) ),DT_EXPANDTABS|DT_NOCLIP|DT_WORD_ELLIPSIS|DT_WORDBREAK);
}

// ���ּӶ���
VOID CDlgExchangeLoveliness::SwitchScoreFormat(SCORE lScore, UINT uSpace, LPTSTR pszBuffer, WORD wBufferSize)
{
	//ת����ֵ
	TCHAR szSwitchScore[21]=TEXT("");
	_sntprintf(szSwitchScore,CountArray(szSwitchScore),TEXT("%I64d"),lScore);

	//��������
	WORD wTargetIndex=0;
	WORD wSourceIndex=0;
	UINT uSwitchLength=lstrlen(szSwitchScore);

	//ת���ַ�
	while (szSwitchScore[wSourceIndex]!=0)
	{
		//�����ַ�
		pszBuffer[wTargetIndex++]=szSwitchScore[wSourceIndex++];

		//���붺��
		if ((uSwitchLength!=wSourceIndex)&&(((uSwitchLength-wSourceIndex)%uSpace)==0L))
		{
			pszBuffer[wTargetIndex++]=TEXT(',');
		}
	}

	//�����ַ�
	pszBuffer[wTargetIndex++]=0;

	return;
}
