#pragma once

#include "resource.h"
#include "ModuleManagerHead.h"

#include "DlgServerWizard.h"

class CDlgRoomTableConfig : public CDialog
{
	DECLARE_DYNAMIC(CDlgRoomTableConfig)

public:
	CDlgRoomTableConfig(CDlgServerWizard* pDlgServerWizard, tagDBGameTableInfo* pTableInfo, int mode, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRoomTableConfig();

// 对话框数据
	enum { IDD = IDD_DIALOG_TABLE_CONFIG };

public:
	CDlgServerWizard*	m_pDlgServerWizard;
	tagDBGameTableInfo*	m_pTableInfo;
	int					m_mode;  // 启动模式

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditTablename();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonCustomRule();
};
