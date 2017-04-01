// DlgRoomTableConfig.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgRoomTableConfig.h"
#include "afxdialogex.h"


// CDlgRoomTableConfig �Ի���

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


// CDlgRoomTableConfig ��Ϣ�������
BOOL CDlgRoomTableConfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ����ȱʡֵ
	if (m_mode == 0) // ����
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
	else // �޸�
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
	// �쳣:  OCX ����ҳӦ���� FALSE
}

void CDlgRoomTableConfig::OnEnChangeEditTablename()
{
	CString str;
	GetDlgItemText(IDC_EDIT_TABLENAME, str);

	UINT sid = GetDlgItemInt(IDC_EDIT1);
	UINT eid = GetDlgItemInt(IDC_EDIT2);

	if (eid < sid || eid == 0 || sid == 0)
	{
		SetWindowText(_T("��������"));
		return;
	}

	str.Trim();
	if (str.IsEmpty())
	{
		
	}

	CString title;
	title.Format(_T("�������� - %s %ld -> %ld"), str, sid, eid);
	SetWindowText(title);
}

void CDlgRoomTableConfig::OnBnClickedOk()
{
	UINT sid = GetDlgItemInt(IDC_EDIT1);
	UINT eid = GetDlgItemInt(IDC_EDIT2);

	if (eid < sid || sid == 0 || eid == 0 )
	{
		AfxMessageBox(_T("��������ȷ������"));
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

	// ���������˰��,�Ͳ���ȡ�����
	if (m_pTableInfo->TableOption.wRevenueRatio != 0)
	{
		m_pTableInfo->TableOption.lServiceScore = 0;
	}
	// ��������˷����,�Ͳ���ȡ˰��
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
		AfxMessageBox(_T("����ͼƬ���Ϊ��1-255"));
		return;
	}

	if (m_pTableInfo->TableOption.nMaxGameTime < 0)
	{
		AfxMessageBox(_T("���ʱʱ����������"));
		return;
	}

//	GetDlgItemText(IDC_EDIT11, (TCHAR*)m_pTableInfo->TableOption.cbCustomRule, 1024);

	CDialog::OnOK();
}

void CDlgRoomTableConfig::OnBnClickedButtonCustomRule()
{
	// ��� �Զ������ ��ť
	// �����Զ������Ի���
	CDlgServerOptionItemCustom dlg(m_pDlgServerWizard, m_pTableInfo);
	if (dlg.DoModal() == IDOK)
	{
	}
	int i = 0;
}
