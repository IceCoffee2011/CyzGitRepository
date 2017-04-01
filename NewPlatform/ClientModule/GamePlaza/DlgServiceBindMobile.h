#ifndef SERVICE_BIND_MOBILE_HEAD_FILE
#define SERVICE_BIND_MOBILE_HEAD_FILE

#pragma once

#include "DlgServiceItem.h"

// 绑定手机页面
class CDlgServiceBindMobile : public CDlgServiceItem
{
public:
	CDlgServiceBindMobile(void);
	~CDlgServiceBindMobile(void);

	//重载函数
public:
	//获取标题
	virtual LPCTSTR GetServiceItemName() { return TEXT("绑定手机"); }

	//控件变量
protected:
	CWebBrowser *					m_pHtmlBrowser;						//浏览控件

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//创建函数
	virtual BOOL OnInitDialog();
	// 窗口大小调整
	VOID OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

#endif