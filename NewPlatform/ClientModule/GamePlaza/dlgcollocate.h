#ifndef DLG_COLLOCATE_HEAD_FILE
#define DLG_COLLOCATE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603类说明
class CCollocateItem;

//6603数组说明
typedef CWHArray<CCollocateItem *> CCollocateItemArray;

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603选择控制
class CCollocateBar : public CWnd
{
	//6603活动信息
protected:
	bool							m_bHovering;						//6603盘旋标志
	WORD							m_wItemDown;						//6603按下子项
	WORD							m_wItemHover;						//6603盘旋子项
	WORD							m_wItemActive;						//6603激活子项

	//6603子项信息
protected:
	CSize							m_SizeButton;						//6603按钮大小
	CRect							m_rcItemCreate;						//6603创建位置
	CCollocateItemArray				m_CollocateItemArray;				//6603控制数组

	//6603函数定义
public:
	//6603构造函数
	CCollocateBar();
	//6603析构函数
	virtual ~CCollocateBar();

	//6603重载函数
public:
	//6603消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//6603参数控制
public:
	//6603保存参数
	bool SaveParameter();
	//6603默认参数
	bool DefaultParameter();

	//6603功能函数
public:
	//6603设置区域
	bool SetItemCreateRect(CRect rcItemCreate);
	//6603插入子项
	bool InsertCollocateItem(CCollocateItem * pCollocateItem);
	//6603激活设置
	bool SetActiveCollocateItem(CCollocateItem * pCollocateItem);

	//6603内部函数
private:
	//6603索引切换
	WORD SwitchToButtonIndex(CPoint MousePoint);

	//6603消息函数
protected:
	//6603重画消息
	VOID OnPaint();
	//6603创建消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//6603鼠标移动
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//6603鼠标消息
	VOID OnLButtonUp(UINT nFlags, CPoint Point);
	//6603鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//6603鼠标离开
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//6603光标消息
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603配置子项
class CCollocateItem : public CDialog
{
	//6603友元定义
	friend class CCollocateBar;
	friend class CDlgCollocate;

	//6603变量定义
protected:
	BYTE							m_cbImageIndex;						//6603图像索要

	//6603函数定义
public:
	//6603构造函数
	CCollocateItem(UINT uIDTemplate);
	//6603析构函数
	virtual ~CCollocateItem();

	//6603重载函数
protected:
	//6603确定消息
	virtual VOID OnOK() { GetParent()->PostMessage(WM_COMMAND,IDOK,0); }
	//6603取消消息
	virtual VOID OnCancel() { GetParent()->PostMessage(WM_COMMAND,IDCANCEL,0); }

	//6603重载函数
public:
	//6603保存参数
	virtual bool SaveParameter()=NULL;
	//6603默认参数
	virtual bool DefaultParameter()=NULL;
	//6603更新控制
	virtual bool UpdateControlStatus()=NULL;

	//6603功能函数
public:
	//6603创建窗口
	bool CreateCollocateItem(CWnd * pParentWnd, CRect rcCreate);

	//6603消息函数
protected:
	//6603背景消息
	BOOL OnEraseBkgnd(CDC * pDC);
	//6603颜色消息
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603同桌规则
class CCollocateServer : public CCollocateItem
{
	//6603胜率限制
public:
	WORD							m_wMinWinRate;						//6603最低胜率
	bool							m_bLimitWinRate;					//6603限制胜率

	//6603逃率限制
public:
	WORD							m_wMaxFleeRate;						//6603最高逃跑
	bool							m_bLimitFleeRate;					//6603限制断线

	//6603积分限制
public:
	LONG							m_lMaxGameScore;					//6603最高分数 
	LONG							m_lMinGameScore;					//6603最低分数
	bool							m_bLimitGameScore;					//6603限制分数

	//6603其他配置
public:
	bool							m_bLimitDetest;						//6603限制玩家
	bool							m_bLimitSameIP;						//6603效验地址

	//6603携带密码
public:
	bool							m_bTakePassword;					//6603携带密码
	TCHAR							m_szPassword[LEN_PASSWORD];			//6603桌子密码

	//6603编辑控件
protected:
	CSkinEdit						m_edWinRate;						//6603用户胜率
	CSkinEdit						m_edFleeRate;						//6603用户逃率
	CSkinEdit						m_edScoreMin;						//6603最低积分
	CSkinEdit						m_edScoreMax;						//6603最高积分
	CSkinEdit						m_edPassword;						//6603桌子密码

	//6603函数定义
public:
	//6603构造函数
	CCollocateServer();
	//6603析构函数
	virtual ~CCollocateServer();

	//6603重载函数
public:
	//6603控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//6603配置函数
	virtual BOOL OnInitDialog();

	//6603重载函数
public:
	//6603保存参数
	virtual bool SaveParameter();
	//6603默认参数
	virtual bool DefaultParameter();
	//6603更新控制
	virtual bool UpdateControlStatus();
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603代理配置
class CCollocateProxy : public CCollocateItem
{
	//6603配置变量
public:
	BYTE							m_cbProxyType;						//6603代理类型
	tagProxyServer					m_ProxyServer;						//6603代理信息

	//6603按钮控件
protected:
	CSkinButton						m_btProxyTest;						//6603测试按钮
	CSkinComboBox					m_cmProxyType;						//6603代理类型

	//6603编辑控件
protected:
	CSkinEdit						m_edProxyPort;						//6603代理端口
	CSkinEdit						m_edProxyServer;					//6603代理地址
	CSkinEdit						m_edProxyUserName;					//6603代理用户
	CSkinEdit						m_edProxyPassword;					//6603代理密码

	//6603函数定义
public:
	//6603构造函数
	CCollocateProxy();
	//6603析构函数
	virtual ~CCollocateProxy();

	//6603重载函数
public:
	//6603控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//6603创建函数
	virtual BOOL OnInitDialog();

	//6603重载函数
public:
	//6603保存参数
	virtual bool SaveParameter();
	//6603默认参数
	virtual bool DefaultParameter();
	//6603更新控制
	virtual bool UpdateControlStatus();

	//6603消息函数
public:
	//6603代理测试
	VOID OnBnClickedProxyTest();

	DECLARE_MESSAGE_MAP()
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603消息设置
class CCollocateNotify : public CCollocateItem
{
	//6603配置变量
public:
	WORD							m_wWhisperHotKey;					//6603私聊热键
	bool							m_bAutoShowWhisper;					//6603显示消息

	//6603枚举变量
public:
	BYTE							m_cbInviteMode;						//6603邀请模式
	BYTE							m_cbMessageMode;					//6603消息模式

	//6603函数定义
public:
	//6603构造函数
	CCollocateNotify();
	//6603析构函数
	virtual ~CCollocateNotify();

	//6603重载函数
public:
	//6603保存参数
	virtual bool SaveParameter();
	//6603默认参数
	virtual bool DefaultParameter();
	//6603更新控制
	virtual bool UpdateControlStatus();
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603习惯配置
class CCollocateCustom : public CCollocateItem
{
	//6603配置变量
public:
	bool							m_bSendWhisperByEnter;				//6603回车发送

	//6603动作定义
public:
	BYTE							m_cbActionHitAutoJoin;				//6603自动加入
	BYTE							m_cbActionLeftDoubleList;			//6603双击列表

	//6603函数定义
public:
	//6603构造函数
	CCollocateCustom();
	//6603析构函数
	virtual ~CCollocateCustom();

	//6603重载函数
public:
	//6603保存参数
	virtual bool SaveParameter();
	//6603默认参数
	virtual bool DefaultParameter();
	//6603更新控制
	virtual bool UpdateControlStatus();
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603系统配置
class CCollocateSystem : public CCollocateItem
{
	//6603配置变量
public:
	WORD							m_wBossHotKey;						//6603老板热键

	//6603配置变量
public:
	bool							m_bAllowSound;						//6603允许声音
	bool							m_bAllowBackMusic;					//6603允许背景音乐
	bool							m_bAutoSitDown;						//6603自动坐下
	bool							m_bSalienceTable;					//6603突出桌子
	bool							m_bSaveWhisperChat;					//6603保存私聊
	bool							m_bNotifyUserInOut;					//6603进出消息
	bool							m_bNotifyFriendCome;				//6603好友提示
	bool							m_bFullScreenBrowse;				//6603全屏浏览

	//6603函数定义
public:
	//6603构造函数
	CCollocateSystem();
	//6603析构函数
	virtual ~CCollocateSystem();

	//6603重载函数
public:
	//6603保存参数
	virtual bool SaveParameter();
	//6603默认参数
	virtual bool DefaultParameter();
	//6603更新控制
	virtual bool UpdateControlStatus();
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603配置窗口
class CDlgCollocate : public CSkinDialog
{
	//6603配置变量
protected:
	CParameterGame *				m_pParameterGame;					//6603游戏配置
	CParameterServer *				m_pParameterServer;					//6603房间配置

	//6603控件变量
protected:
	CSkinButton						m_btOk;								//6603确定按钮
	CSkinButton						m_btCancel;							//6603取消按钮
	CSkinButton						m_btDefault;						//6603默认按钮
	CCollocateBar					m_CollocateBar;						//6603子项按钮

	//6603配置子项
public:
	CCollocateProxy					m_CollocateProxy;					//6603代理配置
	CCollocateSystem				m_CollocateSystem;					//6603系统配置
	CCollocateCustom				m_CollocateCustom;					//6603习惯配置
	CCollocateNotify				m_CollocateNotify;					//6603消息配置
	CCollocateServer				m_CollocateServer;					//6603房间配置

	//6603函数定义
public:
	//6603构造函数
	CDlgCollocate();
	//6603析构函数
	virtual ~CDlgCollocate();

	//6603重载函数
protected:
	//6603控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//6603创建函数
	virtual BOOL OnInitDialog();
	//6603确定函数
	virtual VOID OnOK();

	//6603功能函数
public:
	//6603配置参数
	bool InitCollocate(CParameterGame * pParameterGame, CParameterServer * pParameterServer);

	//6603按钮消息
protected:
	//6603默认按钮
	VOID OnBnClickedDefault();

	DECLARE_MESSAGE_MAP()
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

#endif