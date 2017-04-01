#pragma once
#include "afxcmn.h"
#include "resource.h"

#include "DlgRoomTableConfig.h"

#include "DlgServerWizard.h"

#include <vector>
using namespace std;

class CDlgRoomTable : public CDialog
{
	DECLARE_DYNAMIC(CDlgRoomTable)

public:
	CDlgRoomTable(CDlgServerWizard* pDlgServerWizard, vector<tagDBGameTableInfo>* vectGameTableInfo, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRoomTable();

// 对话框数据
	enum { IDD = IDD_DIALOG_TABLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnDelete();
	CListCtrl m_list;

public:
	void InitColumns();
	void SetListRow(int item, tagDBGameTableInfo* info);

	afx_msg void OnDestroy();
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);

	bool CheckTableID();

	afx_msg void OnBnClickedOk();

	//房间配置值
	CDlgServerWizard*				m_pDlgServerWizard;
	vector<tagDBGameTableInfo>*		m_pVectGameTableInfo;
	afx_msg void OnBnClickedCancel();
};
