#ifndef SECURITYQUESTIONS_CONTROL_HEAD_FILE
#define SECURITYQUESTIONS_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//�˺ſؼ�
class CSecurityQuestionsCtrl : public CSkinComboBox
{
	//��������
public:
	//���캯��
	CSecurityQuestionsCtrl();
	//��������
	virtual ~CSecurityQuestionsCtrl();

	//���غ���
protected:
	//�󶨺���
	virtual VOID PreSubclassWindow();
	//��������
	virtual VOID MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif