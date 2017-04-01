#ifndef TABLE_VIEW_FRAME_HEAD_FILE
#define TABLE_VIEW_FRAME_HEAD_FILE

// 防作弊
// 普通游戏桌
// m_ImageBitGround 先读取资源目录下的ground.bmp这张图
// 比赛模式

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "GamePlazaHead.h"
#include "PlatformEvent.h"
#include "PlatformPublicize.h"
#include "DlgTipUserInfo.h"
#include "UserInfoDlgEx.h"

//////////////////////////////////////////////////////////////////////////////////

//进入索引
#define INDEX_ENTER_CHAIR			MAX_CHAIR							//进入区域

//按钮标识
#define BT_ENTER_GAME				1									//进入游戏
#define BT_LEAVE_MATCH				2									//关闭房间

//桌子属性
struct tagTableAttribute
{
	//桌子标志
	bool							bLocker;							//密码标志
	bool							bPlaying;							//游戏标志
	bool							bFocusFrame;						//框架标志

	//桌子状态
	WORD							wWatchCount;						//旁观数目
	DWORD							dwTableOwnerID;						//桌主索引

	//属性变量
	WORD							wTableID;							//桌子号码
	WORD							wChairCount;						//椅子数目
	TCHAR							szTableName[LEN_TABLENAME];			//桌子名称
	IClientUserItem *				pIClientUserItem[MAX_CHAIR];		//用户信息
};

//////////////////////////////////////////////////////////////////////////////////

//桌子资源
class CTableResource
{
	//桌子属性
public:
	CSize							m_SizeChair;						//椅子尺寸
	CSize							m_SizeTable;						//桌子尺寸
	CSize							m_SizeEnter;						//进入尺寸
	CSize                           m_SizeNumberBK;                     //桌号尺寸
	CSize                           m_SizeNumberNB;                     //桌号尺寸

	//属性变量
public:
	bool							m_bShowUser;						//显示用户
	bool							m_bShowEnter;						//显示进入
	bool							m_bShowChair;						//显示椅子
	bool							m_bShowTable;						//显示桌子
	bool							m_bShowFocus;						//显示焦点
	bool							m_bCustomUser;						//自定用户
	bool							m_bRenderImage;						//渲染标志
	bool							m_b3DChair;							//3D椅子
	LONG							m_lDefaultFace;						//缺省的头像
	LONG							m_lShowTableID;						//桌号显示模式

	//数目变量
public:
	WORD							m_wTableItemCount;					//子项数目
	WORD							m_wChairItemCount;					//子项数目

	WORD							m_wChairCellCount;					//椅子图片单元格数
	LONG							m_lChairCellIndex[MAX_CHAIR*2];		//椅子图片索引

	//类型数目
public:
	WORD							m_wUserGenreCount;					//类型数目
	WORD							m_wTableGenreCount;					//类型数目
	WORD							m_wChairGenreCount;					//类型数目

	//位置属性
public:
	CPoint							m_ptLock;							//桌锁位置
	CPoint							m_ptEnter;							//进入位置
	CPoint							m_ptTableID;						//桌号位置
	CPoint							m_ptReadyArray[MAX_CHAIR];			//准备位置
	CPoint							m_ptAvatarArray[MAX_CHAIR];			//头像位置

	//头像属性
public:
	int								m_ptAvatarIndex[MAX_CHAIR];			//头像属性 

	//框架位置
public:
	CRect							m_rcNameArray[MAX_CHAIR];			//名字位置
	CRect							m_rcChairArray[MAX_CHAIR];			//椅子位置

	//颜色属性
public:
	COLORREF						m_crName;							//名字颜色
	COLORREF						m_crMember;							//会员颜色
	COLORREF						m_crMaster;							//管理颜色

	//辅助变量
public:
	INT								m_nDrawStyle[MAX_CHAIR];			//输出格式
	TCHAR							m_szDirectory[MAX_PATH];			//资源路径

	//外部资源
public:
	CPngImage						m_ImageEnter;						//进入位图
	CPngImage						m_ImageChair;						//椅子位图
	CPngImage						m_ImageChairBOX;					//椅子框位图
	CPngImage						m_ImageTable;						//桌子位图
	CPngImage						m_ImageTableBOX;					//桌子框位图
	CPngImage						m_ImageGround;						//背景位图
	CBitImage						m_ImageBitGround;					//背景位图
	CPngImage						m_ImageCustomUser;					//自定位图
	CBitImage						m_ImageWaitDistribute;				//桌子位图

	//内部资源
public:
	CPngImage						m_ImageReady;						//准备位图
	CPngImage						m_ImageLocker;						//桌锁位图
	CPngImage						m_ImageNumberNB;					//号码位图
	CPngImage						m_ImageNumberBK;					//号码背景
	CPngImage						m_ImageUserFace_2D;					//456的头像（缺省）
	CPngImage						m_ImageUserFace_3D;					//3D头像（缺省）

	//函数定义
public:
	//构造函数
	CTableResource();
	//析构函数
	virtual ~CTableResource();

	//功能函数
public:
	//释放资源
	VOID DestroyResource();
	//加载资源
	bool LoadResource(LPCTSTR pszResDirectory, WORD wChairCount, bool bAvertCheatMode);
	//更新资源
	bool UpdateResource(CSkinRenderManager * pSkinRenderManager);

	//功能函数
public:
	//获得桌子图片的索引
	LONG GetTableCellIndex(WORD wChairID, bool bHasUser);

	//辅助函数
protected:
	//读取数值
	LONG ReadStringValue(LPCTSTR & pszSring);
	//读取矩形
	bool ReadRect(RECT & ValueRect, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile);
	//读取坐标
	bool ReadPoint(POINT & ValuePoint, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile);
	//读取颜色
	bool ReadColor(COLORREF & ValueColor, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile);
};

//////////////////////////////////////////////////////////////////////////////////

//桌子视图
class CTableView : public ITableView
{
	//状态变量
protected:
	bool							m_bMouseDown;						//是否按下
	WORD							m_wHoverChairID;					//盘旋椅子
	tagTableAttribute				m_TableAttribute;					//桌子属性

	//字体接口
protected:
	CDFontEx*						m_pDrawFont;						//字体接口

	//组件接口
protected:
	ITableViewFrame *				m_pITableViewFrame;					//桌子接口

	//函数定义
public:
	//构造函数
	CTableView();
	//析构函数
	virtual ~CTableView();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//功能接口
public:
	//空椅子数
	virtual WORD GetNullChairCount(WORD & wNullChairID);
	//配置函数
	virtual VOID InitTableView(WORD wTableID, WORD wChairCount, TCHAR szTableName[LEN_TABLENAME], ITableViewFrame * pITableViewFrame);

	//用户接口
public:
	//获取用户
	virtual IClientUserItem * GetClientUserItem(WORD wChairID);
	//设置信息
	virtual bool SetClientUserItem(WORD wChairID, IClientUserItem * pIClientUserItem);

	//状态接口
public:
	//鼠标按下
	virtual VOID SetMouseDown(bool bMouseDown);
	//焦点框架
	virtual VOID SetFocusFrame(bool bFocusFrame);
	//盘旋位置
	virtual VOID SetHoverStatus(WORD wHoverChairID);
	//桌子状态 
	virtual VOID SetTableStatus(bool bPlaying, bool bLocker);

	//查询接口
public:
	//游戏标志
	virtual bool GetPlayFlag() { return m_TableAttribute.bPlaying; }
	//密码标志
	virtual bool GetLockerFlag() { return m_TableAttribute.bLocker; }

	//设置函数
public:
	//设置字体
	void SetFont( CDFontEx* pFont ) { m_pDrawFont = pFont; }

	//功能函数
public:
	//绘画函数
	VOID DrawTableView(CDC * pDC, INT nXPos, INT nYPos, DWORD dwServerRule, CTableResource * pTableResource);
	//绘画名字
	VOID DrawTableName(CDC * pDC, INT nXPos, INT nYPos, WORD wChairID, LPCTSTR pszNickName, CTableResource * pTableResource, COLORREF crTextColor);
	//绘画头像
	VOID DrawUserAvatar(CDC * pDC, INT nXPos, INT nYPos, WORD wChairID, IClientUserItem * pIClientUserItem, CTableResource * pTableResource);
	//绘画头像
	VOID DrawUserAvatar(CDC * pDC, INT nXPos, INT nYPos, WORD wFaceID, WORD wChairID, CTableResource * pTableResource);
};

//////////////////////////////////////////////////////////////////////////////////

//数组定义
typedef CWHArray<CTableView *>		CTableViewArray;					//桌子数组

//桌子框架
class CTableViewFrame : public CWnd, public ITableViewFrame
{
	//友元定义
	friend class CTableView;

	//状态信息
protected:
	bool							m_bHovering;						//盘旋标志
	bool							m_bLMouseDown;						//右键标志
	bool							m_bRMouseDown;						//左键标志
	bool                            m_bWaitDistribute;                  //等待分配
	BYTE							m_cbButtonDown;						//按钮掩码
	BYTE							m_cbButtonHover;					//按钮掩码

	//索引变量
protected:
	WORD							m_wDownTableID;						//桌子号码
	WORD							m_wDownChairID;						//椅子号码

	//属性变量
protected:
	WORD							m_wTableCount;						//游戏桌数
	WORD							m_wChairCount;						//椅子数目
	DWORD							m_dwServerRule;						//房间规则
	WORD							m_wServerType;						//房间类型

	WORD							m_wTipTableID;						//游戏桌数
	WORD							m_wTipChairID;						//椅子数目
	WORD							m_wLastTipChairID;					//椅子数目

	//滚动信息
protected:
	INT								m_nXExcursion;						//横行偏移
	INT								m_nYExcursion;						//竖向偏移
	INT								m_nXTableCount;						//桌子列数
	INT								m_nYTableCount;						//桌子行数

	//滚动信息
protected:
	INT								m_nScrollPos;						//滚动位置
	INT								m_nScrollPosMax;					//最大位置
	INT								m_nVisibleLineCount;				//可见行数

	//控件变量
public:
	CSkinScrollBar					m_SkinScrollBar;					//滚动条类
	CTableResource					m_TableResource;					//桌子资源
	CTableViewArray					m_TableViewArray;					//桌子数组
	//CDlgTipUserInfo                 m_DlgTipUserInfo;                   //提示组件
    
	CUserInfoDlgEx                  m_UserInfoDlgEx;
	CString							m_strName;							//房间名称
	//比赛变量
public:
	DWORD							m_dwWaitting;						//等待人数
	DWORD							m_dwTotalUser;						//参赛人数
	DWORD							m_dwBestRank;						//最好游戏币
	DWORD							m_dwMatchCount;						//参数次数
	CTime							m_tStartTime;						//开始时间
	BYTE							m_cbMatchStatus;					//比赛状态
	CBitImage						m_ImageEnterMatch;					//加入比赛
	CBitImage						m_ImageLeaveMatch;					//退出比赛
	CBitImage						m_ImageTableTL;						//图片资源
	CBitImage						m_ImageTableTM;						//图片资源
	CBitImage						m_ImageTableTR;						//图片资源
	CPngImage						m_ImageNumberGreen;					//绿色数字
	CPngImage						m_ImageNumberYellow;				//黄色数字
	CPngImage						m_ImageLine;						//图片资源
	CPlatformPublicize				m_BrowseMatchInfo;					//比赛页面
	CMD_GR_MatchDesc				m_pMatchDesc;						//信息描述

	//字体信息
public:
	CDFontEx						m_DrawFont;							//一般字体

	//组件接口
public:
	ITableViewFrameSink *			m_pITableViewFrameSink;				//通知接口
	IGameLevelParser *				m_pIGameLevelParser;				//等级接口

	//函数定义
public:
	//构造函数
	CTableViewFrame();
	//析构函数
	virtual ~CTableViewFrame();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置接口
public:
	//创建桌子
	virtual bool CreateTableFrame(CWnd * pParentWnd, UINT uWndID, IUnknownEx * pIUnknownEx);
	//配置桌子
	virtual bool ConfigTableFrame(WORD wTableCount, WORD wChairCount, TCHAR(* szTableNameArray)[LEN_TABLENAME], DWORD dwServerRule, WORD wServerType, WORD wServerID, LPCTSTR pszResDirectory);

	//信息接口
public:
	//桌子数目
	virtual WORD GetTableCount() { return m_wTableCount; }
	//椅子数目
	virtual WORD GetChairCount() { return m_wChairCount; }
	//获取资源
	virtual CTableResource * GetTableResource() { return &m_TableResource; }

	//用户接口
public:
	//获取用户
	virtual IClientUserItem * GetClientUserItem(WORD wTableID, WORD wChairID);
	//设置信息
	virtual bool SetClientUserItem(WORD wTableID, WORD wChairID, IClientUserItem * pIClientUserItem);

	//状态接口
public:
	//游戏标志
	virtual bool GetPlayFlag(WORD wTableID);
	//密码标志
	virtual bool GetLockerFlag(WORD wTableID);
	//焦点框架
	virtual VOID SetFocusFrame(WORD wTableID, bool bFocusFrame);
	//桌子状态 
	virtual VOID SetTableStatus(WORD wTableID, bool bPlaying, bool bLocker);
	//桌子状态 
	virtual VOID SetTableStatus(bool bWaitDistribute);

	//视图控制
public:
	//桌子可视
	virtual bool VisibleTable(WORD wTableID);
	//闪动桌子
	virtual bool FlashGameTable(WORD wTableID);
	//闪动椅子
	virtual bool FlashGameChair(WORD wTableID, WORD wChairID);

	//功能接口
public:
	//更新桌子
	virtual bool UpdateTableView(WORD wTableID);
	//获取桌子
	virtual ITableView * GetTableViewItem(WORD wTableID);
	//空椅子数
	virtual WORD GetNullChairCount(WORD wTableID, WORD & wNullChairID);
	//获取接口
	virtual IGameLevelParser * GetGameLevelParser() { return m_pIGameLevelParser; }
	//设置接口
	VOID SetGameLevelParser(IGameLevelParser * pIGameLevelParser) { m_pIGameLevelParser=pIGameLevelParser; }

	//重载函数
protected:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//辅助函数
protected:
	//桌子测试
	WORD SwitchTableID(POINT MousePoint);
	//椅子测试
	WORD SwitchChairID(POINT MousePoint);
	//获得矩形框
	CRect GetTableRect(WORD wTableID);
	//调整控件
	VOID RectifyControl(INT nWidth, INT nHeight);
	//按钮测试
	BYTE GetHoverButton(CPoint MousePoint);
	//绘画数字
	VOID DrawNumberString(CDC * pDC, CPngImage &PngNumber, LONG lNumber, INT nXPos, INT nYPos,BYTE cbAlign=TA_LEFT);
	//绘画图片
	VOID SpreadDrawImage(CDC * pDC, CPngImage &PngImage, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight);
	//绘画图片
	VOID SpreadDrawImage(CDC * pDC, CBitImage &BitImage, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight);
	//消息函数
protected:
	//重画消息
	VOID OnPaint();
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);
	//鼠标消息
	VOID OnRButtonUp(UINT nFlags, CPoint MousePoint);
	//鼠标消息
	VOID OnLButtonUp(UINT nFlags, CPoint MousePoint);
	//鼠标离开
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//鼠标消息
	VOID OnRButtonDown(UINT nFlags, CPoint MousePoint);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint MousePoint);
	//鼠标消息
	VOID OnLButtonDblClk(UINT nFlags, CPoint MousePoint);
	//光标消息
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//鼠标消息
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint MousePoint);
	//滚动消息
	VOID OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	//定时消息
	afx_msg void OnTimer(UINT nIDEvent);
	//自定消息
protected:
	//事件消息
	LRESULT OnMessagePlatformEvent(WPARAM wParam, LPARAM lParam);

	//比赛函数
public:
	//比赛状态
	void SetMatchStatus(BYTE cbMatchStatus);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif
