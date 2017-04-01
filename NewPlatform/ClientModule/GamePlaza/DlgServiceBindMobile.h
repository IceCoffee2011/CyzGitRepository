#ifndef SERVICE_BIND_MOBILE_HEAD_FILE
#define SERVICE_BIND_MOBILE_HEAD_FILE

#pragma once

#include "DlgServiceItem.h"

// ���ֻ�ҳ��
class CDlgServiceBindMobile : public CDlgServiceItem
{
public:
	CDlgServiceBindMobile(void);
	~CDlgServiceBindMobile(void);

	//���غ���
public:
	//��ȡ����
	virtual LPCTSTR GetServiceItemName() { return TEXT("���ֻ�"); }

	//�ؼ�����
protected:
	CWebBrowser *					m_pHtmlBrowser;						//����ؼ�

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//��������
	virtual BOOL OnInitDialog();
	// ���ڴ�С����
	VOID OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

#endif