#ifndef TABLE_TIPUSERINFO_HEAD_FILE
#define TABLE_TIPUSERINFO_HEAD_FILE

#pragma once

#include "Resource.h"

//用户信息
#define TIP_USER_INFO_W             260
#define TIP_USER_INFO_H             300

// CDlgTipUserInfo 对话框
//////////////////////////////////////////////////////////////////////////
class CDlgTipUserInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgTipUserInfo)

protected:
	CImage                          m_ImageBuf;

public:
	//构造函数
	CDlgTipUserInfo(CWnd* pParent = NULL);
	//析构函数
	virtual ~CDlgTipUserInfo();

protected:
	//数据交换
	virtual void DoDataExchange(CDataExchange* pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//绘画函数
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()

public:
	//构造信息
	VOID ConstructUserInfo(IClientUserItem *pUserItem,IGameLevelParser *pIGameLevelParser);
};

//////////////////////////////////////////////////////////////////////////
#endif