#include "StdAfx.h"
#include "Resource.h"
#include "DlgJyMx.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgJyMx, CSkinDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgJyMx::CDlgJyMx() : CSkinDialog(IDD_JYMX)
{
}

//�������� 
CDlgJyMx::~CDlgJyMx()
{
}

//�ؼ���
VOID CDlgJyMx::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control( pDX,IDOK,m_btOK ); 
	DDX_Control( pDX,IDCANCEL,m_btCancel );
	//DDX_Text(pDX,IDC_EDT_MESSAGE,m_strKickMessage);
} 

//��ʼ������
BOOL CDlgJyMx::OnInitDialog()
{
	__super::OnInitDialog();
   
	//���ñ��� 
	//SetWindowText(TEXT("��ʾ"));
	
	//Ĭ����ʾ
	//m_strKickMessage = TEXT("��Ǹ����������Ա������䣬�����κ����ʣ�����ϵ��Ϸ�ͷ���");
	//���ؼ�
	CRect rcCreate(0,0,0,0);
	m_PlatformPublicize.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,rcCreate,this,1000);

	//�����ַ
	TCHAR szBillUrl[256]=TEXT("");
	_sntprintf(szBillUrl,CountArray(szBillUrl),TEXT("%s/Member/InsureRecord_game.aspx"),szPlatformLink);

	//���ù��
	m_PlatformPublicize.Navigate(szBillUrl);
	m_PlatformPublicize.SetWindowPos(NULL,7,37,750,490,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE);


	UpdateData(FALSE);

	return TRUE;
}

//ȷ������
VOID CDlgJyMx::OnOK()
{
	UpdateData();
	
	return __super::OnOK();
}

//////////////////////////////////////////////////////////////////////////
