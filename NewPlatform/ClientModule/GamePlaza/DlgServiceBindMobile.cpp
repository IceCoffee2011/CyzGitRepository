#include "StdAfx.h"
#include ".\dlgservicebindmobile.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CDlgServiceBindMobile, CDlgServiceItem)
	ON_WM_SIZE()
END_MESSAGE_MAP()

CDlgServiceBindMobile::CDlgServiceBindMobile(void):CDlgServiceItem(IDD_DLG_SERVICE_BIND_MOBILE)
{
	m_pHtmlBrowser = NULL;
}

CDlgServiceBindMobile::~CDlgServiceBindMobile(void)
{
	// 删除cookie
//	CInternetSession::SetCookie(_T("http://1378.com/bind/index.php"),TEXT("GameID"),_T(""));

	//删除控件
	SafeDelete(m_pHtmlBrowser);
}

//控件绑定
VOID CDlgServiceBindMobile::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//消息解释
BOOL CDlgServiceBindMobile::PreTranslateMessage(MSG * pMsg)
{
	return __super::PreTranslateMessage(pMsg);
}

//创建函数
BOOL CDlgServiceBindMobile::OnInitDialog()
{
	__super::OnInitDialog();


	CRect rcBrowse;
	GetClientRect(rcBrowse);

	//创建控件
	m_pHtmlBrowser=new CWebBrowser;
	m_pHtmlBrowser->Create(NULL,NULL,WS_CHILD|WS_VISIBLE,rcBrowse,this,101);

	CGlobalUserInfo* pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	DWORD dwGameID = 0;
	if (pGlobalUserInfo)
	{
		dwGameID = pGlobalUserInfo->GetGlobalUserData()->dwGameID;
	}

	//构造地址
	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szPath, sizeof(szPath));
	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\local.dat"), szPath);
	//获取地址
	TCHAR szBindMobileUrl[256]=TEXT("");
	GetPrivateProfileString(TEXT("HyperLink"),TEXT("BindMobile"),TEXT(""), szBindMobileUrl, sizeof(szBindMobileUrl), szFileName);
//	_sntprintf(szBindMobileUrl,CountArray(szBindMobileUrl),TEXT("%s/bind/index.php?GameID=%d"), szPlatformLink, dwGameID);

	//浏览规则
	m_pHtmlBrowser->Navigate(szBindMobileUrl,NULL,NULL,NULL,NULL);

	return TRUE;
}

// 窗口大小调整
VOID CDlgServiceBindMobile::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	CRect rcBrowse;
	GetClientRect(rcBrowse);

	if (m_pHtmlBrowser)
	{
		m_pHtmlBrowser->MoveWindow(rcBrowse);
	}
}
