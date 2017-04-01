#ifndef TABLE_TIPUSERINFO_HEAD_FILE
#define TABLE_TIPUSERINFO_HEAD_FILE

#pragma once

#include "Resource.h"

//�û���Ϣ
#define TIP_USER_INFO_W             260
#define TIP_USER_INFO_H             300

// CDlgTipUserInfo �Ի���
//////////////////////////////////////////////////////////////////////////
class CDlgTipUserInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgTipUserInfo)

protected:
	CImage                          m_ImageBuf;

public:
	//���캯��
	CDlgTipUserInfo(CWnd* pParent = NULL);
	//��������
	virtual ~CDlgTipUserInfo();

protected:
	//���ݽ���
	virtual void DoDataExchange(CDataExchange* pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//�滭����
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()

public:
	//������Ϣ
	VOID ConstructUserInfo(IClientUserItem *pUserItem,IGameLevelParser *pIGameLevelParser);
};

//////////////////////////////////////////////////////////////////////////
#endif