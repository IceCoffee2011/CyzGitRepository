#include "StdAfx.h"
#include "Resource.h"
#include "DlgJyMx.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgJyMx, CSkinDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgJyMx::CDlgJyMx() : CSkinDialog(IDD_JYMX)
{
}

//析构函数 
CDlgJyMx::~CDlgJyMx()
{
}

//控件绑定
VOID CDlgJyMx::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control( pDX,IDOK,m_btOK ); 
	DDX_Control( pDX,IDCANCEL,m_btCancel );
	//DDX_Text(pDX,IDC_EDT_MESSAGE,m_strKickMessage);
} 

//初始化函数
BOOL CDlgJyMx::OnInitDialog()
{
	__super::OnInitDialog();
   
	//设置标题 
	//SetWindowText(TEXT("提示"));
	
	//默认提示
	//m_strKickMessage = TEXT("抱歉，您被管理员请出房间，若有任何疑问，请联系游戏客服！");
	//广告控件
	CRect rcCreate(0,0,0,0);
	m_PlatformPublicize.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,rcCreate,this,1000);

	//构造地址
	TCHAR szBillUrl[256]=TEXT("");
	_sntprintf(szBillUrl,CountArray(szBillUrl),TEXT("%s/Member/InsureRecord_game.aspx"),szPlatformLink);

	//设置广告
	m_PlatformPublicize.Navigate(szBillUrl);
	m_PlatformPublicize.SetWindowPos(NULL,7,37,750,490,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE);


	UpdateData(FALSE);

	return TRUE;
}

//确定函数
VOID CDlgJyMx::OnOK()
{
	UpdateData();
	
	return __super::OnOK();
}

//////////////////////////////////////////////////////////////////////////
