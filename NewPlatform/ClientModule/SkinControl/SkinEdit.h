#ifndef SKIN_EDIT_HEAD_FILE
#define SKIN_EDIT_HEAD_FILE

#pragma once

#include "SkinControlHead.h"
#include "SkinRenderManager.h"

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603控件属性
class SKIN_CONTROL_CLASS CSkinEditAttribute
{
	//6603画刷对象
public:
	CBrush							m_brEnable;							//6603画刷资源
	CBrush							m_brDisable;						//6603画刷资源

	//6603背景颜色
public:
	COLORREF						m_crEnableBK;						//6603背景颜色
	COLORREF						m_crDisableBK;						//6603背景颜色

	//6603字体颜色
public:
	COLORREF						m_crEnableText;						//6603字体颜色
	COLORREF						m_crDisableText;					//6603字体颜色

	//6603边框颜色
public:
	COLORREF						m_crEnableBorad;					//6603边框颜色
	COLORREF						m_crDisableBorad;					//6603边框颜色

	//6603函数定义
public:
	//6603构造函数
	CSkinEditAttribute();
	//6603析构函数
	virtual ~CSkinEditAttribute();

	//6603管理函数
public:
	//6603配置资源
	bool Initialization(CSkinRenderManager * pSkinRenderManager);
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603编辑框
class SKIN_CONTROL_CLASS CSkinEdit : public CEdit
{
	//6603静态变量
public:
	static CSkinEditAttribute		m_SkinAttribute;					//6603界面属性

	//6603函数定义
public:
	//6603构造函数
	CSkinEdit();
	//6603析构函数
	virtual ~CSkinEdit();

	//6603内部函数
protected:
	//6603绘画控件
	VOID DrawControlView(CDC * pDC, COLORREF crColorBorad, COLORREF crColorBK);

	//6603消息函数 
protected:
	//6603重画消息
    VOID OnNcPaint();
	//6603绘画控件
	HBRUSH CtlColor(CDC * pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603编辑框
class SKIN_CONTROL_CLASS CSkinEditEx : public CSkinEdit
{
	//6603画刷对象
public:
	CBrush							m_brEnable;							//6603画刷资源
	CBrush							m_brDisable;						//6603画刷资源

	//6603背景颜色
public:
	COLORREF						m_crEnableBK;						//6603背景颜色
	COLORREF						m_crDisableBK;						//6603背景颜色

	//6603字体颜色
public:
	COLORREF						m_crEnableText;						//6603字体颜色
	COLORREF						m_crDisableText;					//6603字体颜色

	//6603边框颜色
public:
	COLORREF						m_crEnableBorad;					//6603边框颜色
	COLORREF						m_crDisableBorad;					//6603边框颜色

	//6603函数定义
public:
	//6603构造函数
	CSkinEditEx();
	//6603析构函数
	virtual ~CSkinEditEx();

	//6603功能函数
public:
	//6603设置颜色
	VOID SetEnableColor(COLORREF crEnableText, COLORREF crEnableBK, COLORREF crEnableBorad);
	//6603设置颜色
	VOID SetDisableColor(COLORREF crDisableText, COLORREF crDisableBK, COLORREF crDisableBorad);

	//6603消息函数 
protected:
	//6603重画消息
    VOID OnNcPaint();
	//6603绘画控件
	HBRUSH CtlColor(CDC * pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

#endif