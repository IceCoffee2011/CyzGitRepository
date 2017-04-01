// DlgRoomTable.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgRoomTable.h"
#include "afxdialogex.h"

int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl* pListCtrl = (CListCtrl*)lParamSort;
	CString strItem1 = pListCtrl->GetItemText(lParam1, 0);
	CString strItem2 = pListCtrl->GetItemText(lParam2, 1);
	return 0;
}


// CDlgRoomTable 对话框

IMPLEMENT_DYNAMIC(CDlgRoomTable, CDialog)

CDlgRoomTable::CDlgRoomTable(CDlgServerWizard*	pDlgServerWizard, vector<tagDBGameTableInfo>* vectGameTableInfo, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRoomTable::IDD, pParent)
{
//	m_pTableConfig = pTableConfig;
	m_pVectGameTableInfo = vectGameTableInfo;
	m_pDlgServerWizard = pDlgServerWizard;
}


CDlgRoomTable::~CDlgRoomTable()
{
}

void CDlgRoomTable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CDlgRoomTable, CDialog)
	ON_BN_CLICKED(IDC_BTN_ADD, &CDlgRoomTable::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CDlgRoomTable::OnBnClickedBtnDelete)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CDlgRoomTable::OnNMDblclkList)
	ON_BN_CLICKED(IDOK, &CDlgRoomTable::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgRoomTable::OnBnClickedCancel)
END_MESSAGE_MAP()

// CDlgRoomTable 消息处理程序
void CDlgRoomTable::InitColumns()
{
	m_list.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 62);
	m_list.InsertColumn(1, _T("StartID"), LVCFMT_LEFT, 62);
	m_list.InsertColumn(2, _T("EndID"), LVCFMT_LEFT, 62);

	m_list.InsertColumn(3, _T("底分"), LVCFMT_LEFT, 62);
	m_list.InsertColumn(4, _T("封顶"), LVCFMT_LEFT, 62);
	m_list.InsertColumn(5, _T("税收比例"), LVCFMT_LEFT, 62);
	m_list.InsertColumn(6, _T("服务费"), LVCFMT_LEFT, 62);

	m_list.InsertColumn(7, _T("最小准坐"), LVCFMT_LEFT, 62);
	m_list.InsertColumn(8, _T("最大准坐"), LVCFMT_LEFT, 62);
	m_list.InsertColumn(9, _T("最小会员"), LVCFMT_LEFT, 62);
	m_list.InsertColumn(10, _T("最大会员"), LVCFMT_LEFT, 62);
	m_list.InsertColumn(11, _T("最长游戏时间"), LVCFMT_LEFT, 62);
	m_list.InsertColumn(12, _T("桌子普通图标"), LVCFMT_LEFT, 62);
	m_list.InsertColumn(13, _T("桌子游戏图标"), LVCFMT_LEFT, 62);
// 	m_list.InsertColumn(11, _T("显示规则"), LVCFMT_LEFT, 62);
// 	m_list.InsertColumn(12, _T("定制规则"), LVCFMT_LEFT, 62);

	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
};

void CDlgRoomTable::SetListRow(int item, tagDBGameTableInfo* info)
{
	m_list.SetItemText(item, 0, info->TableOption.szTableName);
	CString str; str.Format(_T("%ld"), info->wStartTableID);
	m_list.SetItemText(item, 1, str);
	str.Format(_T("%ld"), info->wEndTableID);
	m_list.SetItemText(item, 2, str);

	str.Format(_T("%ld"), info->TableOption.lCellScore);
	m_list.SetItemText(item, 3, str);

	str.Format(_T("%ld"), info->TableOption.lRestrictScore);
	m_list.SetItemText(item, 4, str);

	str.Format(_T("%ld"), info->TableOption.wRevenueRatio);
	m_list.SetItemText(item, 5, str);

	str.Format(_T("%ld"), info->TableOption.lServiceScore);
	m_list.SetItemText(item, 6, str);

	str.Format(_T("%ld"), info->TableOption.lMinTableScore);
	m_list.SetItemText(item, 7, str);

	str.Format(_T("%ld"), info->TableOption.lMaxTableScore);
	m_list.SetItemText(item, 8, str);

	str.Format(_T("%ld"), info->TableOption.cbMinTableEnterMember);
	m_list.SetItemText(item, 9, str);

	str.Format(_T("%ld"), info->TableOption.cbMaxTableEnterMember);
	m_list.SetItemText(item, 10, str);

	str.Format(_T("%ld"), info->TableOption.nMaxGameTime);
	m_list.SetItemText(item, 11, str);

	str.Format(_T("%ld"), info->TableOption.nNormalPicID);
	m_list.SetItemText(item, 12, str);

	str.Format(_T("%ld"), info->TableOption.nPlayingPicID);
	m_list.SetItemText(item, 13, str);

	m_list.SetItemData(item, (DWORD_PTR)info);

//	m_list.SortItems(ListCompare, (LPARAM)&m_list);

	if (!CheckTableID())
	{
		SetDlgItemText(IDC_STATIC_HINT, _T("有部分桌号没有配置或者出现了重叠的配置，请注意!!!"));
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	else
	{
		SetDlgItemText(IDC_STATIC_HINT, _T(""));
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
};

BOOL CDlgRoomTable::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitColumns();
	GetDlgItem(IDOK)->EnableWindow(FALSE);

// 	for (int i = 0; i < MAX_TABLE_OPTION; i++)
// 	{
// 		tagGameTableInfo* pInfo = m_pTableConfig + i;
// 		if (pInfo == NULL || pInfo->wStartTableID == 0 || pInfo->wEndTableID == 0)
// 		{
// 			continue;
// 		}
// 
// 		tagGameTableInfo* pNewinfo = new tagGameTableInfo;
// 		memcpy(pNewinfo, pInfo, sizeof(tagGameTableInfo));
// 
// 		int item = m_list.InsertItem(m_list.GetItemCount(), _T(""));
// 		SetListRow(item, pNewinfo);
// 	}

	for (size_t i = 0; i < m_pVectGameTableInfo->size(); i++)
	{
		tagDBGameTableInfo* pNewInfo = new tagDBGameTableInfo;
		memcpy(pNewInfo, &(m_pVectGameTableInfo->at(i)), sizeof(tagDBGameTableInfo));

		int item = m_list.InsertItem(m_list.GetItemCount(), _T(""));
		SetListRow(item, pNewInfo);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CDlgRoomTable::OnDestroy()
{
	CDialog::OnDestroy();

	for (int i = 0; i < m_list.GetItemCount(); i++)
	{
		tagDBGameTableInfo* info = (tagDBGameTableInfo*)m_list.GetItemData(i);
		if (info != NULL)
		{
			delete info;
			info = NULL;
		}
	}
}


void CDlgRoomTable::OnBnClickedBtnAdd()
{
	tagDBGameTableInfo* pInfo = new tagDBGameTableInfo;
	memset(pInfo, 0, sizeof(tagDBGameTableInfo));

	CDlgRoomTableConfig dlg(m_pDlgServerWizard, pInfo, 0);
	if (dlg.DoModal() == IDOK)
	{
		int item = m_list.InsertItem(m_list.GetItemCount(), _T(""));
		SetListRow(item, pInfo);
	}
	else
	{
		delete pInfo;
		pInfo = nullptr;
	};
}


void CDlgRoomTable::OnBnClickedBtnDelete()
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos == NULL)
		TRACE0("No items were selected!\n");
	else
	{
		while (pos)
		{
			int nItem = m_list.GetNextSelectedItem(pos);
			if (AfxMessageBox(_T("你确定删除此项配置吗？"), MB_YESNO) == IDYES)
			{
				tagDBGameTableInfo* info = (tagDBGameTableInfo*)m_list.GetItemData(nItem);
				delete info;
				info = NULL;
				m_list.DeleteItem(nItem);
				break;;
			}
			else
			{
				return;
			}
		}
		if (!CheckTableID())
		{
			SetDlgItemText(IDC_STATIC_HINT, _T("有部分桌号没有配置或者出现了重叠的配置，请注意!!!"));
			GetDlgItem(IDOK)->EnableWindow(FALSE);
		}
		else
		{
			SetDlgItemText(IDC_STATIC_HINT, _T(""));
			GetDlgItem(IDOK)->EnableWindow(TRUE);
		}
	}
}

void CDlgRoomTable::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	tagDBGameTableInfo* info = (tagDBGameTableInfo*)m_list.GetItemData(pNMItemActivate->iItem);
	if (info != NULL)
	{
		CDlgRoomTableConfig dlg(m_pDlgServerWizard, info, 1);
		if (dlg.DoModal() == IDOK)
		{
			SetListRow(pNMItemActivate->iItem, info);
		}
// 		else
// 		{
// 			delete info;
// 			info = NULL;
// 		};
	}
	
	*pResult = 0;
}

bool CDlgRoomTable::CheckTableID()
{
	// 此处检查是否有漏配,配置覆盖情况
	// 至于是否配置满了,在上层对话框中进行检测
	int maxtableid = 0;
	for (int i = 0; i < m_list.GetItemCount(); i++)
	{
		tagDBGameTableInfo* info = (tagDBGameTableInfo*)m_list.GetItemData(i);
		if (info == NULL)
		{
			continue;
		}

		if (info->wEndTableID > maxtableid)
			maxtableid = info->wEndTableID;
	}

// 	if (maxtableid <= 0)
// 		return false;

	vector<int> m;
	m.push_back(0);  // 桌号是1索引
	for (int i = 0; i < maxtableid; i++)
	{
		m.push_back(0);
	}

	for (int i = 0; i < m_list.GetItemCount(); i++)
	{
		tagDBGameTableInfo* info = (tagDBGameTableInfo*)m_list.GetItemData(i);
		if (info == NULL)
		{
			continue;
		}

		for (int j = info->wStartTableID; j <= info->wEndTableID; j++)
		{
			if (m[j] == 0)
			{
				m[j] = 1;
			}
			else
			{
				// 之前已经配置过了,出现了重复配置,返回
				return false;
			}
		}
	}

	for (int i = 1; i < (int)m.size(); i++)
	{
		if (m[i] == 0)
			return false;
	}

	return true;

};

void CDlgRoomTable::OnBnClickedOk()
{
	if (m_pVectGameTableInfo == nullptr)
	{
		return;
	}
	m_pVectGameTableInfo->clear();
	for (int i = 0; i < m_list.GetItemCount(); i++)
	{
		tagDBGameTableInfo* pInfo = (tagDBGameTableInfo*)m_list.GetItemData(i);
		if (pInfo == nullptr)
		{
			continue;
		}

		tagDBGameTableInfo GameTableInfo;
		memcpy(&GameTableInfo, pInfo, sizeof(tagDBGameTableInfo));

		m_pVectGameTableInfo->push_back(GameTableInfo);

// 		if (i == MAX_TABLE_OPTION)
// 			break;

//		memcpy(m_pTableConfig + i, pInfo, sizeof(tagGameTableInfo));

// 		tagGameTableInfo TableOption;
// 		memcpy(&TableOption, pInfo, sizeof(TableOption));
// 
// 		if (m_pVectTableOption != nullptr)
// 		{
// 			m_pVectTableOption->push_back(TableOption);
// 		}
	}

// 	for (; i < MAX_TABLE_OPTION; i++)
// 	{
// 		// 可能是删除了配置,所以要继续重置剩余的数据
// 		memset(m_pTableConfig + i, 0, sizeof(tagGameTableInfo));
// 	}

	CDialog::OnOK();
}




void CDlgRoomTable::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialog::OnCancel();
}
