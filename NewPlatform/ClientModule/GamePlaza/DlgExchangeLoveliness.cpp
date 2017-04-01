// DlgExchangeLoveliness.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgExchangeLoveliness.h"


// CDlgExchangeLoveliness 对话框

//IMPLEMENT_DYNAMIC(CDlgExchangeLoveliness, CDialog)
CDlgExchangeLoveliness::CDlgExchangeLoveliness()
	: CSkinDialog(IDD_DLG_EXCHANGE_LOVELINESS)
{
	m_bQueryMission = false;

	//设置任务
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

// CDlgExchangeLoveliness 消息处理程序
BEGIN_MESSAGE_MAP(CDlgExchangeLoveliness, CSkinDialog)

END_MESSAGE_MAP()

//创建函数
BOOL CDlgExchangeLoveliness::OnInitDialog()
{
	__super::OnInitDialog();

	// 设置标题
	SetWindowText(TEXT("魅力兑换"));

	HINSTANCE hInstance=AfxGetInstanceHandle();

	// 创建按钮
	m_btExchange.SetButtonImage(IDB_BT_OK,  hInstance, false, false);
	m_btCancel.SetButtonImage(IDB_BT_CANCEL, hInstance, false, false);

	CRect rcExchange;
	m_btExchange.GetClientRect(rcExchange);
	m_btExchange.MoveWindow(125, 100, rcExchange.Width(), rcExchange.Height());

	CRect rcCancel;
	m_btCancel.GetClientRect(rcCancel);
	m_btCancel.MoveWindow(240, 100, rcCancel.Width(), rcCancel.Height());

	// 禁用兑换按钮
	m_btExchange.EnableWindow(FALSE);
	// 获取配置信息
	QueryExchangeLovelinessInfo();

	return TRUE;
}

//确定函数
VOID CDlgExchangeLoveliness::OnOK()
{
	LOGI("CDlgExchangeLoveliness::OnOK");
	//设置变量
	m_bQueryMission=false;
	m_bExchangeMisssion=true;

	//事件处理
	OnMissionStart();

	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}
}
// 取消
VOID CDlgExchangeLoveliness::OnCancel()
{
	__super::OnCancel();
}

//查询资料
VOID CDlgExchangeLoveliness::QueryExchangeLovelinessInfo()
{
	LOGI("CDlgExchangeLoveliness::QueryExchangeLovelinessInfo");
	//设置变量
	m_bQueryMission=true;
	m_bExchangeMisssion=false;

	//事件处理
	OnMissionStart();

	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}
}

//开始任务
VOID CDlgExchangeLoveliness::OnMissionStart()
{
	// 禁用双按钮
	m_btExchange.EnableWindow(FALSE);
	m_btCancel.EnableWindow(FALSE);
}

//终止任务
VOID CDlgExchangeLoveliness::OnMissionConclude()
{
	m_bQueryMission=false;
	m_bExchangeMisssion=false;

	// 按钮可用
	m_btExchange.EnableWindow(TRUE);
	m_btCancel.EnableWindow(TRUE);
}

//连接事件
bool CDlgExchangeLoveliness::OnEventMissionLink(INT nErrorCode)
{
	LOGI("CDlgExchangeLoveliness::OnEventMissionLink, nErrorCode="<<nErrorCode);
	// 连接结果
	if (nErrorCode!=0)
	{
		//重试任务
		if (m_MissionManager.AvtiveMissionItem(this,true)==true)
		{
			return true;
		}

		//事件处理
//		OnMissionConclude();

		//错误提示
 		if (m_bQueryMission==true)
 		{
			m_bQueryMission = false;
 			//提示消息
 			CInformation Information(this);
 			Information.ShowMessageBox(TEXT("连接服务器超时，获取魅力兑换配置信息失败！"),MB_OK|MB_ICONSTOP,30);
 		}

		//错误提示
 		if (m_bExchangeMisssion==true)
 		{
			m_bExchangeMisssion = false;
 			//提示消息
 			CInformation Information(this);
 			Information.ShowMessageBox(TEXT("连接服务器超时，兑换魅力失败！"),MB_OK|MB_ICONSTOP,30);
 		}

		return true;
	}

	if (m_bQueryMission == true)
	{
//		LOGI("m_bQueryMission == true");
		CMD_GP_QueryExchangeLoveliness QueryExchangeLoveliness;
		ZeroMemory(&QueryExchangeLoveliness, sizeof(QueryExchangeLoveliness));

		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//设置变量
		QueryExchangeLoveliness.dwUserID=pGlobalUserData->dwUserID;

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE, SUB_GP_QUERY_EXCHANGE_LOVELINESS, &QueryExchangeLoveliness,sizeof(QueryExchangeLoveliness));
	}

	if (m_bExchangeMisssion == true)
	{
		CMD_GP_ExchangeLoveliness ExchangeLoveliness;

		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//设置变量
		ExchangeLoveliness.dwUserID=pGlobalUserData->dwUserID;

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE, SUB_GP_EXCHANGE_LOVELINESS, &ExchangeLoveliness,sizeof(ExchangeLoveliness));
	}
	return true;
}

//关闭事件
bool CDlgExchangeLoveliness::OnEventMissionShut(BYTE cbShutReason)
{
	LOGI("CDlgExchangeLoveliness::OnEventMissionShut, cbShutReason="<<cbShutReason);
	//提示消息
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//重试任务
		if (m_MissionManager.AvtiveMissionItem(this,true)==true)
		{
			return true;
		}

		//查询任务
		if (m_bQueryMission==true)
		{
			m_bQueryMission = false;
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("与服务器的连接异常断开了，获取魅力兑换配置信息失败！"),MB_ICONERROR,30);
		}

		//修改任务
		if (m_bExchangeMisssion==true)
		{
			m_bExchangeMisssion = false;
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("与服务器的连接异常断开了，兑换魅力失败！"),MB_ICONERROR,30);
		}
	}

	OnMissionConclude();
	return true;
}

//读取事件
bool CDlgExchangeLoveliness::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
	LOGI("CDlgExchangeLoveliness::OnEventMissionRead, wMainCmdID="<<Command.wMainCmdID<<", wSubCmdID="<<Command.wSubCmdID<<", wDataSize="<<wDataSize);
	//命令处理
	if (Command.wMainCmdID==MDM_GP_USER_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_EXCHANGE_LOVELINESS_INFO:
			{
				// 消息定义
				CMD_GP_ExchangeLovelinessInfo * pExchangeLovelinessInfo=(CMD_GP_ExchangeLovelinessInfo *)pData;
				LOGI("a = "<< sizeof(CMD_GP_ExchangeLovelinessInfo) << ", b="<<CountStringBuffer(pExchangeLovelinessInfo->szTips)<<", c="<<CountStringBuffer(pExchangeLovelinessInfo->szErr));
				//效验数据
				ASSERT(wDataSize>=(sizeof(CMD_GP_ExchangeLovelinessInfo)-sizeof(pExchangeLovelinessInfo->szTips)-sizeof(pExchangeLovelinessInfo->szErr)));
				if (wDataSize<(sizeof(CMD_GP_ExchangeLovelinessInfo)-sizeof(pExchangeLovelinessInfo->szTips)-sizeof(pExchangeLovelinessInfo->szErr))) return false;

//				LOGI("CDlgExchangeLoveliness::OnEventMissionRead 2");
				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);
				OnMissionConclude();

				// 判断能否进行转账
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

					_sntprintf(m_szExchangeInfo, CountArray(m_szExchangeInfo),  TEXT("您确定将 %s 点魅力值兑换为 %s 银子吗？"), szAvailbleAmount, szFinalAmount);
				}
				LOGI("CDlgExchangeLoveliness::OnEventMissionRead 3, m_szExchangeInfo="<<m_szExchangeInfo);

				lstrcpyn(m_szTips,pExchangeLovelinessInfo->szTips,CountArray(pExchangeLovelinessInfo->szTips));

				// 刷新当前窗口
				//更新界面
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);
				LOGI("CDlgExchangeLoveliness::OnEventMissionRead 4");
			}
			break;
		case SUB_GP_OPERATE_FAILURE:		//操作失败
			{
				if (m_bExchangeMisssion == true)
				{
					LOGI("m_bExchangeMisssion == true");
					// 弹出提示框
					//变量定义
					CMD_GP_OperateSuccess * pOperateSuccess=(CMD_GP_OperateSuccess *)pData;

					//效验数据
					ASSERT(wDataSize>=(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString)));
					if (wDataSize<(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString))) return false;

					LOGI("m_bExchangeMisssion == 1");

					//关闭连接
					m_MissionManager.ConcludeMissionItem(this,false);

					//成功处理
					OnMissionConclude();

					//显示消息
					if (pOperateSuccess->szDescribeString[0]!=0)
					{
						CInformation Information(this);
						Information.ShowMessageBox(pOperateSuccess->szDescribeString,MB_ICONINFORMATION,60);
					}

					__super::OnCancel();
				}
			}
			break;
		case SUB_GP_EXCHANGE_LOVELINESS_RESULT:	// 兑换操作结果
			{
				//变量定义
				CMD_GP_ExchangeLovelinessResult * pExchangeLovelinessResult=(CMD_GP_ExchangeLovelinessResult *)pData;

				//效验数据
				ASSERT(wDataSize>=(sizeof(CMD_GP_ExchangeLovelinessResult)-sizeof(pExchangeLovelinessResult->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_ExchangeLovelinessResult)-sizeof(pExchangeLovelinessResult->szDescribeString))) return false;

				if (pExchangeLovelinessResult->lResultCode==DB_SUCCESS)
				{
					// 更新消息
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

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				//成功处理
				OnMissionConclude();

				//显示消息
				if (pExchangeLovelinessResult->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pExchangeLovelinessResult->szDescribeString,MB_ICONINFORMATION,60);
				}

				__super::OnCancel();
			}
			break;
/*
		case SUB_GP_OPERATE_SUCCESS:		// 操作成功
			{
				if (m_bExchangeMisssion == true)
				{
					LOGI("m_bExchangeMisssion == true");
					// 弹出提示框
					//变量定义
					CMD_GP_OperateSuccess * pOperateSuccess=(CMD_GP_OperateSuccess *)pData;

					//效验数据
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

//绘画消息
VOID CDlgExchangeLoveliness::OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight)
{
	pDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());
	// 绘制转账信心
	pDC->SetTextColor(RGB(0,0,0));
	pDC->DrawText(m_szExchangeInfo, CRect( CPoint(40, 60), CSize(400, 40) ),DT_EXPANDTABS|DT_NOCLIP|DT_CENTER|DT_WORDBREAK);

	// 绘制提示
	pDC->SetTextColor(RGB(55, 125, 158));
//	pDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());
	pDC->DrawText(m_szTips, CRect( CPoint(40, 160), CSize(400,100) ),DT_EXPANDTABS|DT_NOCLIP|DT_WORD_ELLIPSIS|DT_WORDBREAK);
}

// 数字加逗号
VOID CDlgExchangeLoveliness::SwitchScoreFormat(SCORE lScore, UINT uSpace, LPTSTR pszBuffer, WORD wBufferSize)
{
	//转换数值
	TCHAR szSwitchScore[21]=TEXT("");
	_sntprintf(szSwitchScore,CountArray(szSwitchScore),TEXT("%I64d"),lScore);

	//变量定义
	WORD wTargetIndex=0;
	WORD wSourceIndex=0;
	UINT uSwitchLength=lstrlen(szSwitchScore);

	//转换字符
	while (szSwitchScore[wSourceIndex]!=0)
	{
		//拷贝字符
		pszBuffer[wTargetIndex++]=szSwitchScore[wSourceIndex++];

		//插入逗号
		if ((uSwitchLength!=wSourceIndex)&&(((uSwitchLength-wSourceIndex)%uSpace)==0L))
		{
			pszBuffer[wTargetIndex++]=TEXT(',');
		}
	}

	//结束字符
	pszBuffer[wTargetIndex++]=0;

	return;
}
