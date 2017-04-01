#pragma once

#include "resource.h"
#include "ModuleManagerHead.h"

#include "DlgServerWizard.h"

class CDlgRoomTableConfig : public CDialog
{
	DECLARE_DYNAMIC(CDlgRoomTableConfig)

public:
	CDlgRoomTableConfig(CDlgServerWizard* pDlgServerWizard, tagDBGameTableInfo* pTableInfo, int mode, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgRoomTableConfig();

// �Ի�������
	enum { IDD = IDD_DIALOG_TABLE_CONFIG };

public:
	CDlgServerWizard*	m_pDlgServerWizard;
	tagDBGameTableInfo*	m_pTableInfo;
	int					m_mode;  // ����ģʽ

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditTablename();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonCustomRule();
};
