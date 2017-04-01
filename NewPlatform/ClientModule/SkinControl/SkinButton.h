#ifndef SKIN_BUTTON_HEAD_FILE
#define SKIN_BUTTON_HEAD_FILE

#pragma once

#include "SkinControlHead.h"

//////////////////////////////////////////////////////////////////////////////////

//按钮类
class SKIN_CONTROL_CLASS CSkinButton : public CButton
{
	//状态变量
protected:
	bool							m_bHovering;						//盘旋标志
	bool							m_bTransparent;						//是否透明
	bool							m_bRenderImage;						//渲染背景
	bool							m_bRenderTextFrame;					//是否绘制字体边框

	//配置变量
protected:
	WORD							m_wImageIndex;						//过渡索引
	COLORREF						m_crButtonText;						//字体颜色
	COLORREF						m_crButtonFrameText;				//字体边框颜色

	//背景资源
protected:
	LPCTSTR							m_pszSurface;						//按钮资源
	LPCTSTR							m_pszResource;						//资源信息
	HINSTANCE						m_hResInstance;						//资源句柄
	CFont*							m_pTextFont;						//字体颜色

	//函数定义
public:
	//构造函数
	CSkinButton();
	//析构函数
	virtual ~CSkinButton();

	//重载函数
protected:
	//绑定函数
	virtual VOID PreSubclassWindow();
	//绘画函数
	virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//配置函数
public:
	//设置渲染
	VOID SetRenderImage(bool bRenderImage);
	//设置颜色
	VOID SetButtonColor(COLORREF crButtonText);
	//设置颜色
	VOID SetButtonFrameColor(COLORREF crButtonFrameText);

	//背景函数
public:
	//加载位图
	bool SetButtonImage(UINT uBitmapID, HINSTANCE hResInstance, bool bRenderImage, bool bTransparent);
	//加载位图
	bool SetButtonImage(LPCTSTR pszResource, HINSTANCE hResInstance, bool bRenderImage, bool bTransparent);
	//加载位图
	bool SetButtonImage(UINT uBitmapID, LPCTSTR pszSurface, HINSTANCE hResInstance, bool bRenderImage, bool bTransparent);
	//加载位图
	bool SetButtonImage(LPCTSTR pszResource, LPCTSTR pszSurface, HINSTANCE hResInstance, bool bRenderImage, bool bTransparent);
	//设置字体
	void SetTextFont(CFont* FontText) {  m_pTextFont = FontText; };

	//辅助函数
private:
	//调整控件
	VOID RectifyControl();
	//绘画文字
	VOID DrawButtonText(CDC * pDC, UINT uItemState);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect);

	//消息函数
protected:
	//销毁消息
	VOID OnNcDestroy();
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//时间消息
	VOID OnTimer(UINT_PTR nIDEvent);
	//建立消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标移动
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//鼠标离开
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//光标消息
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif