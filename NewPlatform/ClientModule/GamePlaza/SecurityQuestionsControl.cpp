#include "StdAfx.h"
#include "Resource.h"
#include "SecurityQuestionsControl.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
#define ITEM_CY						22									//�����С

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSecurityQuestionsCtrl, CSkinComboBox)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CSecurityQuestionsCtrl::CSecurityQuestionsCtrl()
{
}

//��������
CSecurityQuestionsCtrl::~CSecurityQuestionsCtrl()
{
}

//�󶨺���
VOID CSecurityQuestionsCtrl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//���ÿؼ�
//	m_SkinComboBoxEdit.LimitText(LEN_ACCOUNTS-1);

	//���ÿؼ�
	SetButtonImage(AfxGetInstanceHandle(),IDB_BT_COMBOBOX,CSize(20,15));

	return;
}

//��������
VOID CSecurityQuestionsCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	//���ñ���
	lpMeasureItemStruct->itemWidth=0;
	lpMeasureItemStruct->itemHeight=ITEM_CY;

	return;
}

//////////////////////////////////////////////////////////////////////////////////
