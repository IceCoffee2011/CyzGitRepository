#ifndef DLG_DLGJYMX_HEAD_FILE
#define DLG_DLGJYMX_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "PlatformPublicize.h"
//////////////////////////////////////////////////////////////////////////

//�������
class CDlgJyMx : public CSkinDialog
{
	//��������
public:
	//CString							m_strKickMessage;					//�߳���ʾ
	CPlatformPublicize				m_PlatformPublicize;				//����ؼ�

	//���Ʊ���
protected:
	CSkinButton						m_btOK;								//ȷ����ť
	CSkinButton						m_btCancel;							//ȡ����ť

	//��������
public:
	//���캯��
	CDlgJyMx();
	//��������
	virtual ~CDlgJyMx();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////


#endif