#include "StdAfx.h"
#include "Resource.h"
#include "SecurityQuestionsControl.h"

//////////////////////////////////////////////////////////////////////////////////

//常量定义
#define ITEM_CY						22									//子项大小

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSecurityQuestionsCtrl, CSkinComboBox)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CSecurityQuestionsCtrl::CSecurityQuestionsCtrl()
{
}

//析构函数
CSecurityQuestionsCtrl::~CSecurityQuestionsCtrl()
{
}

//绑定函数
VOID CSecurityQuestionsCtrl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//设置控件
//	m_SkinComboBoxEdit.LimitText(LEN_ACCOUNTS-1);

	//设置控件
	SetButtonImage(AfxGetInstanceHandle(),IDB_BT_COMBOBOX,CSize(20,15));

	return;
}

//测量子项
VOID CSecurityQuestionsCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	//设置变量
	lpMeasureItemStruct->itemWidth=0;
	lpMeasureItemStruct->itemHeight=ITEM_CY;

	return;
}

//////////////////////////////////////////////////////////////////////////////////
