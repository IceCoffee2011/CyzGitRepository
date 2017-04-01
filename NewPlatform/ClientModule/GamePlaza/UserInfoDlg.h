#ifndef PLAZA_USER_INFO_DLG_HEAD_FILE
#define PLAZA_USER_INFO_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////
typedef CMap<BYTE,BYTE,CString,CString &> CMapMemberDescribe;			//描述索引

//用户信息
class CUserInfoDlg : public CDialog
{
	//状态变量
protected:
	IClientUserItem*			    m_pUserData;
	CRgn							m_DialogRgn;						//窗口区域
	CBitImage						m_ImageBack;						//背景位图
	IGameLevelParser *				m_pIGameLevelParser;				//等级接口
	CMapMemberDescribe				m_MapMemberDescribe;				//会员描述
public:
     WORD							m_wServerType;						//房间类型
	//函数定义
public:
	//构造函数
	CUserInfoDlg();
	//析构函数
	virtual ~CUserInfoDlg();
	//功能函数
public:
	//加载位图
	bool LoadRgnImage(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTrans);

	//内部函数 
private:
	//创建区域
	bool CreateControlRgn(COLORREF crTrans);
	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK() { return; };
	//取消函数
	virtual VOID OnCancel() { return; };

	//功能函数
public:
	//更新信息
	VOID UpdateUserInfo(IClientUserItem* pUserData);
    //设置接口
	VOID SetGameLevelParser(IGameLevelParser * pIGameLevelParser) { m_pIGameLevelParser=pIGameLevelParser; }
	//消息函数
public:
	//重画函数
	afx_msg void OnPaint();
	//鼠标右键按下消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//背景消息
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);

	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CUserInfoDlg)
};

//////////////////////////////////////////////////////////////////////////

#endif