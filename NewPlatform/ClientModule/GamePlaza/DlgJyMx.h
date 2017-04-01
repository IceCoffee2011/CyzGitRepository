#ifndef DLG_DLGJYMX_HEAD_FILE
#define DLG_DLGJYMX_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "PlatformPublicize.h"
//////////////////////////////////////////////////////////////////////////

//房间管理
class CDlgJyMx : public CSkinDialog
{
	//变量定义
public:
	//CString							m_strKickMessage;					//踢出提示
	CPlatformPublicize				m_PlatformPublicize;				//浏览控件

	//控制变量
protected:
	CSkinButton						m_btOK;								//确定按钮
	CSkinButton						m_btCancel;							//取消按钮

	//函数定义
public:
	//构造函数
	CDlgJyMx();
	//析构函数
	virtual ~CDlgJyMx();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////


#endif