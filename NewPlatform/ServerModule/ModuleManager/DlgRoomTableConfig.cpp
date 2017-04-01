// DlgRoomTableConfig.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgRoomTableConfig.h"
#include "afxdialogex.h"


// CDlgRoomTableConfig 对话框

IMPLEMENT_DYNAMIC(CDlgRoomTableConfig, CDialog)

CDlgRoomTableConfig::CDlgRoomTableConfig(CDlgServerWizard* pDlgServerWizard, tagDBGameTableInfo* pTableInfo, int mode, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRoomTableConfig::IDD, pParent)
{
	m_pDlgServerWizard = pDlgServerWizard;
	m_pTableInfo = pTableInfo;
	m_mode = mode;

}

CDlgRoomTableConfig::~CDlgRoomTableConfig()
{
}

void CDlgRoomTableConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRoomTableConfig, CDialog)
	ON_EN_CHANGE(IDC_EDIT_TABLENAME, &CDlgRoomTableConfig::OnEnChangeEditTablename)
	ON_EN_CHANGE(IDC_EDIT1, &CDlgRoomTableConfig::OnEnChangeEditTablename)
	ON_EN_CHANGE(IDC_EDIT2, &CDlgRoomTableConfig::OnEnChangeEditTablename)
	ON_BN_CLICKED(IDOK, &CDlgRoomTableConfig::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_CUSTOM_RULE, &CDlgRoomTableConfig::OnBnClickedButtonCustomRule)
END_MESSAGE_MAP()


// CDlgRoomTableConfig 消息处理程序
BOOL CDlgRoomTableConfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置缺省值
	if (m_mode == 0) // 新增
	{
		SetDlgItemInt(IDC_EDIT3, 1);
		SetDlgItemInt(IDC_EDIT4, 0);
		SetDlgItemInt(IDC_EDIT5, 0);

		SetDlgItemInt(IDC_EDIT7, 1);
		SetDlgItemInt(IDC_EDIT8, 0);
		SetDlgItemInt(IDC_EDIT9, 0);
		SetDlgItemInt(IDC_EDIT10, 0);
		SetDlgItemInt(IDC_EDIT12, 0);

		SetDlgItemInt(IDC_EDIT_NORMAL_PIC, 1);
		SetDlgItemInt(IDC_EDIT_PLAYING_PIC, 1);

		SetDlgItemInt(IDC_MAX_GAME_TIME, 360);
	}
	else // 修改
	{
		if (m_pTableInfo != NULL)
		{
			SetDlgItemText(IDC_EDIT_TABLENAME, m_pTableInfo->TableOption.szTableName);
			SetDlgItemInt(IDC_EDIT1, m_pTableInfo->wStartTableID);
			SetDlgItemInt(IDC_EDIT2, m_pTableInfo->wEndTableID);

			SetDlgItemInt(IDC_EDIT3, (UINT)m_pTableInfo->TableOption.lCellScore);
			SetDlgItemInt(IDC_EDIT4, (UINT)m_pTableInfo->TableOption.lRestrictScore);

			SetDlgItemInt(IDC_EDIT12, m_pTableInfo->TableOption.wRevenueRatio);
			SetDlgItemInt(IDC_EDIT5, (UINT)m_pTableInfo->TableOption.lServiceScore);

			SetDlgItemInt(IDC_EDIT7, (UINT)m_pTableInfo->TableOption.lMinTableScore);
			SetDlgItemInt(IDC_EDIT8, (UINT)m_pTableInfo->TableOption.lMaxTableScore);

			SetDlgItemInt(IDC_EDIT9, m_pTableInfo->TableOption.cbMinTableEnterMember);
			SetDlgItemInt(IDC_EDIT10, m_pTableInfo->TableOption.cbMaxTableEnterMember);

			SetDlgItemInt(IDC_EDIT_NORMAL_PIC, m_pTableInfo->TableOption.nNormalPicID);
			SetDlgItemInt(IDC_EDIT_PLAYING_PIC, m_pTableInfo->TableOption.nPlayingPicID);

			SetDlgItemInt(IDC_MAX_GAME_TIME, m_pTableInfo->TableOption.nMaxGameTime);

//			SetDlgItemText(IDC_EDIT11, (TCHAR*)m_pTableInfo->TableOption.cbCustomRule);
		}
		else
		{
			ASSERT(0);
		}

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CDlgRoomTableConfig::OnEnChangeEditTablename()
{
	CString str;
	GetDlgItemText(IDC_EDIT_TABLENAME, str);

	UINT sid = GetDlgItemInt(IDC_EDIT1);
	UINT eid = GetDlgItemInt(IDC_EDIT2);

	if (eid < sid || eid == 0 || sid == 0)
	{
		SetWindowText(_T("桌子配置"));
		return;
	}

	str.Trim();
	if (str.IsEmpty())
	{
		
	}

	CString title;
	title.Format(_T("桌子配置 - %s %ld -> %ld"), str, sid, eid);
	SetWindowText(title);
}

void CDlgRoomTableConfig::OnBnClickedOk()
{
	UINT sid = GetDlgItemInt(IDC_EDIT1);
	UINT eid = GetDlgItemInt(IDC_EDIT2);

	if (eid < sid || sid == 0 || eid == 0 )
	{
		AfxMessageBox(_T("请设置正确的桌号"));
		return;
	}

	GetDlgItemText(IDC_EDIT_TABLENAME, m_pTableInfo->TableOption.szTableName, LEN_SERVER);
	m_pTableInfo->TableOption.szTableName[LEN_TABLENAME - 1] = 0;
	m_pTableInfo->wStartTableID = GetDlgItemInt(IDC_EDIT1);
	m_pTableInfo->wEndTableID   = GetDlgItemInt(IDC_EDIT2);
	m_pTableInfo->TableOption.lCellScore = GetDlgItemInt(IDC_EDIT3);
	m_pTableInfo->TableOption.lRestrictScore = GetDlgItemInt(IDC_EDIT4);

	m_pTableInfo->TableOption.wRevenueRatio = GetDlgItemInt(IDC_EDIT12);
	m_pTableInfo->TableOption.lServiceScore = GetDlgItemInt(IDC_EDIT5);

	// 如果配置了税收,就不收取服务费
	if (m_pTableInfo->TableOption.wRevenueRatio != 0)
	{
		m_pTableInfo->TableOption.lServiceScore = 0;
	}
	// 如果配置了服务费,就不收取税收
	if (m_pTableInfo->TableOption.lServiceScore != 0)
	{
		m_pTableInfo->TableOption.wRevenueRatio = 0;
	}

	m_pTableInfo->TableOption.lMinTableScore = GetDlgItemInt(IDC_EDIT7);
	m_pTableInfo->TableOption.lMaxTableScore = GetDlgItemInt(IDC_EDIT8);

	m_pTableInfo->TableOption.cbMinTableEnterMember = GetDlgItemInt(IDC_EDIT9);
	m_pTableInfo->TableOption.cbMaxTableEnterMember = GetDlgItemInt(IDC_EDIT10);

	m_pTableInfo->TableOption.nNormalPicID = GetDlgItemInt(IDC_EDIT_NORMAL_PIC);
	m_pTableInfo->TableOption.nPlayingPicID = GetDlgItemInt(IDC_EDIT_PLAYING_PIC);
	m_pTableInfo->TableOption.nMaxGameTime = GetDlgItemInt(IDC_MAX_GAME_TIME);

	if (m_pTableInfo->TableOption.nNormalPicID <= 0 || m_pTableInfo->TableOption.nPlayingPicID <= 0)
	{
		AfxMessageBox(_T("桌子图片编号为从1-255"));
		return;
	}

	if (m_pTableInfo->TableOption.nMaxGameTime < 0)
	{
		AfxMessageBox(_T("最长超时时间设置有误"));
		return;
	}

//	GetDlgItemText(IDC_EDIT11, (TCHAR*)m_pTableInfo->TableOption.cbCustomRule, 1024);

	CDialog::OnOK();
}

void CDlgRoomTableConfig::OnBnClickedButtonCustomRule()
{
	// 点击 自定义规则 按钮
	// 弹出自定义规则对话框
	CDlgServerOptionItemCustom dlg(m_pDlgServerWizard, m_pTableInfo);
	if (dlg.DoModal() == IDOK)
	{
	}
	int i = 0;
}
