#ifndef LAST_GAME_SERVER_CTRL_HEAD_FILE
#define LAST_GAME_SERVER_CTRL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServerListData.h"

#define  WM_LASTGAMESERVERCTRL_MOVE	 WM_USER+777

// 列表通知最近游戏控件接口
interface ILastGameServerSink
{
	//状态通知
public:
	// 添加最近玩的游戏
	virtual VOID OnAddLastPlayGame(WORD wServerID, CGameServerItem *pGameServerItem)=NULL;
	// 删除最近玩的游戏
	virtual VOID OnRemoveLastPlayGame(WORD wServerID)=NULL;
	// 更新最近玩的游戏数据
	virtual VOID OnUpdateLastPlayGame(WORD wServerID, CGameServerItem *pGameServerItem)=NULL;
};

class CLastGameServerCtrl : public CWnd, public ILastGameServerSink
{
public:
	CLastGameServerCtrl(void);
	virtual ~CLastGameServerCtrl(void);
protected:
	CBitImage						m_ImageTypeBkgnd;					//类型的底图
	CPngImage						m_ImageArrow1;						//箭头图标（大的类型）
//	CPngImage						m_ImageArrow2;						//箭头图标（游戏）

	CFont							m_FontBold;							//粗体字体
	CFont							m_FontUnderLine;					// 下划线字体

	WORD							m_wIndexBkgnd;						// 背景图片Index
//	map<WORD, CGameServerItem*>		m_mapLastGameServer;				// 最近游戏	
	vector<CGameServerItem*>		m_vectGameServerItem;				// 最近游戏
	bool							m_bShowLastGame;					// 是否显示最近在玩游戏
//	map<WORD, CPngImage*>			m_mapLastGameIcon;					// 游戏的Icon	
	map<WORD, CImage*>				m_mapLastGameIcon;
protected:
	CRect							m_rcTypeBkgnd;						// 标题区域
	vector<CRect>					m_vectRectLastGame;					// 最近游戏区域
	WORD							m_wHoverRectIndex;

public:
	// 添加最近玩的游戏
	virtual VOID OnAddLastPlayGame(WORD wServerID, CGameServerItem *pGameServerItem);
	// 删除最近玩的游戏
	virtual VOID OnRemoveLastPlayGame(WORD wServerID);
	// 更新最近玩的游戏数据
	virtual VOID OnUpdateLastPlayGame(WORD wServerID, CGameServerItem *pGameServerItem);

	//消息函数
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);
	//创建消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标移动
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonUp(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	// 重绘消息
	afx_msg VOID OnPaint();

	DECLARE_MESSAGE_MAP()
};

#endif
