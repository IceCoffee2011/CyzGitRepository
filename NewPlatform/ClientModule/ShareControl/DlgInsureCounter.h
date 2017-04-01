#ifndef DLG_INSURE_COUNTER_HEAD_FILE
#define DLG_INSURE_COUNTER_HEAD_FILE

#pragma once

#include "MissionManager.h"
#include "PasswordControl.h"
#include "ShareControlHead.h"

//////////////////////////////////////////////////////////////////////////////////

//类说明
class CDlgInsureItem;
class CDlgInsureMain;
class CDlgInsureSave;

//////////////////////////////////////////////////////////////////////////////////

//操作方式
#define INSURE_SAVE					0									// 存取方式
#define INSURE_TRANSFER				1									// 转账方式
#define TRANS_RECORD				2									// 转账历史记录	
#define INSURE_MODIFY_PASSWORD		3									// 修改银行密码
#define INSURE_PASSWORD				4									// 验证银行密码标签


#define EVENT_QUERYNICKNAME			100									// 根据GameID查询昵称					

struct RealTransRecord
{
	DWORD                               dwGameID;
	TCHAR                               szSourceAccount[LEN_ACCOUNTS];
	TCHAR                               szTargetAccounts[LEN_ACCOUNTS];
	LONGLONG                            lSwapScore;
	LONGLONG                            lRevenue;
	DWORD								dwSourceID;
	DWORD								dwTargetID;
	TCHAR                               szTime[LEN_ACCOUNTS];
};

typedef CWHArray<RealTransRecord *>	VecTransRecord;					//转账记录数据

//////////////////////////////////////////////////////////////////////////////////

//银行信息
struct tagUserInsureInfo
{
	WORD							wRevenueTake;						//税收比例
	WORD							wRevenueTransfer;					//税收比例
	WORD							wServerID;							//房间标识
	SCORE							lUserScore;							//用户游戏币
	SCORE							lUserInsure;						//银行游戏币
	SCORE							lTransferPrerequisite;				//转账条件
};

//////////////////////////////////////////////////////////////////////////////////

//银行接口
interface IInsureCounterAction
{
	//查询事件
	virtual VOID PerformQueryInfo()=NULL;
	//存入事件
	virtual VOID PerformSaveScore(SCORE lSaveScore)=NULL;
	//取出事件
	virtual VOID PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass)=NULL;
	//转账事件
	virtual VOID PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass)=NULL;
    //转账记录
    virtual VOID PerformQueryTransRecord()=NULL;
	// 查询玩家昵称
	virtual VOID PerformQueryUserNickName(DWORD dwGameID) = NULL;
	// 处理银行密码
	virtual VOID PerformInsurePassword(LPCTSTR pszInsurePass) = NULL;
	// 修改银行密码
	virtual VOID PerformModiyInsurePassword(LPCTSTR pszSrcPassword, LPCTSTR pszDesPassword) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//页面子类
class CDlgInsureItem
{
	//函数定义
public:
	//构造函数
	CDlgInsureItem();
	//析构函数
	virtual ~CDlgInsureItem();

	//功能函数
public:
	//获取游戏币
	SCORE GetUserControlScore(CWnd * pWndScore);

	//功能函数
public:
	//绘画数字
	VOID DrawNumberString(CDC * pDC, SCORE lScore, INT nXPos, INT nYPos);
	//转换字符
	VOID SwitchScoreString(SCORE lScore, LPTSTR pszBuffer, WORD wBufferSize);
	//转换字符
	VOID SwitchScoreFormat(SCORE lScore, UINT uSpace, LPTSTR pszBuffer, WORD wBufferSize);
};
//////////////////////////////////////////////////////////////////////////
// 起始输入银行密码对话框
class CDlgInsurePassword : public CDialog
{
	//友元定义
	friend class CDlgInsureMain;
	//对象指针
protected:
	CDlgInsureMain *				m_pDlgInsureMain;					//窗口指针
public:
	CSkinButton						m_btOk;								// 确定按钮
	CSkinButton						m_btCancel;							// 取消按钮
public:
	CPasswordControl				m_PasswordControl;					// 银行密码窗口

public:
	CDlgInsurePassword();
	virtual ~CDlgInsurePassword();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//创建函数
	virtual BOOL OnInitDialog();
	// 确定按钮
	virtual VOID OnOK();
	// 取消按钮
	virtual VOID OnCancel();
protected:
	// 确定密码按钮
	VOID OnBnClickedOk();
	// 取消按钮
	VOID OnBnClickedCancel();

protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);


	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
class CTransRecordList:public CSkinListCtrl
{
    
    //函数定义
public:
    //构造函数
    CTransRecordList(){};
    //析构函数
    virtual ~CTransRecordList(){};
    //重载函数
public:
    //绘画函数
    virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

    DECLARE_MESSAGE_MAP()
};


//存取游戏币(记录)
class CDlgTransRecord : public CDialog
{
	//友元定义
	friend class CDlgInsureMain;
	//对象指针
protected:
	CDlgInsureMain *				m_pDlgInsureMain;					//窗口指针
	tagUserInsureInfo *				m_pUserInsureInfo;					//用户信息

	//控件变量
public:
	CSkinListCtrl					m_ListView;							
	CSkinButton						m_btQuery;						//查询按钮
	CSkinButton						m_btFirst;						//首页按钮
	CSkinButton						m_btPre;						//上一页按钮
	CSkinButton						m_btNext;						//下一页按钮
	CSkinButton						m_btLast;						//尾页按钮
	CDlgInsureItem					m_DlgInsureItem;					//银行子项
	
    bool m_bQueryRecord;
	VecTransRecord					m_vecTransRecord;
	BYTE							m_cbCurPage;
	BYTE							m_cbPageCell;
    BYTE                            m_cbPageCount;


	//函数定义
public:
	//构造函数
	CDlgTransRecord();
	//析构函数
	virtual ~CDlgTransRecord();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定按钮
	virtual VOID OnOK(){return;}
	//取消按钮
	virtual VOID OnCancel(){return;};
private:
	void ShowTransRecord();
public:
    void SetRecordList(CMD_GR_RealTransRecord* pRealTransRecord);
	void SetRecordList(CMD_GP_RealTransRecord* pRealTransRecord);
	//消息函数
protected:
	afx_msg void OnTimer(UINT nIDEvent);
    //命令函数
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};
//////////////////////////////////////////////////////////////////////////////////

//存取游戏币
class CDlgInsureSave : public CDialog
{
	//友元定义
	friend class CDlgInsureMain;

	//辅助变量
protected:
	bool							m_bSwitchIng;						//转换标志

	//对象指针
protected:
	CDlgInsureMain *				m_pDlgInsureMain;					//窗口指针
	tagUserInsureInfo *				m_pUserInsureInfo;					//用户信息

	//控件变量
public:
	CSkinEditEx						m_edScore;							//操作游戏币
	CSkinButton						m_btTakeScore;						//取款按钮
	CSkinButton						m_btSaveScore;						//存款按钮

	//其他控件
public:
//	CSkinHyperLink					m_ForgetInsure;						//忘记密码
	CDlgInsureItem					m_DlgInsureItem;					//银行子项
//	CPasswordControl				m_PasswordControl;					//用户密码 (不用输入用户密码，在对话框打开时已经输入过)
//	CDlgTransRecord					m_DlgTransRecord;

	// 快捷操作
protected:
	CSkinHyperLink					m_hlOneMillion;					// 100万快捷
	CSkinHyperLink					m_hlFiveMillion;				// 500万快捷
	CSkinHyperLink					m_hlTenMillion;					// 1000万快捷
	CSkinHyperLink					m_hlFiftyMillion;				// 5000万快捷
	CSkinHyperLink					m_hlOneHundredMillion;			// 1亿
	CSkinHyperLink					m_hlFiveHundredMillion;			// 5亿
	CSkinHyperLink					m_hlAllScore;					// 全部游戏
	CSkinHyperLink					m_hlAllInsureScore;				// 全部保险箱	
	
	//函数定义
public:
	//构造函数
	CDlgInsureSave();
	//析构函数
	virtual ~CDlgInsureSave();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定按钮
	virtual VOID OnOK();
	//取消按钮
	virtual VOID OnCancel();

	//控件函数
protected:
	//输入信息
	VOID OnEnChangeScore();
	//存款按钮
	VOID OnBnClickedSaveScore();
	//取款按钮
	VOID OnBnClickedTakeScore();
	//忘记密码
	VOID OnBnClickedForgetInsure();

	// 各种金额超链接
	VOID OnBnClickedHyperLink1();
	VOID OnBnClickedHyperLink2();
	VOID OnBnClickedHyperLink3();
	VOID OnBnClickedHyperLink4();
	VOID OnBnClickedHyperLink5();
	VOID OnBnClickedHyperLink6();
	VOID OnBnClickedHyperLink7();
	VOID OnBnClickedHyperLink8();
	//消息函数
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//转账游戏币
class CDlgInsureTransfer : public CDialog
{
	//友元定义
	friend class CDlgInsureMain;

	//辅助变量
protected:
	bool							m_bSwitchIng;						//转换标志

	//对象指针
protected:
	CDlgInsureMain *				m_pDlgInsureMain;					//窗口指针
	tagUserInsureInfo *				m_pUserInsureInfo;					//用户信息

	//控件变量
public:
	CSkinEditEx						m_edScore;							//操作游戏币
	CSkinEditEx						m_edNickName;						//用户昵称
	CSkinButton						m_btTransferScore;					//转账按钮

	//其他控件
public:
//	CSkinHyperLink					m_ForgetInsure;						//忘记密码
	CDlgInsureItem					m_DlgInsureItem;					//银行子项
//	CPasswordControl				m_PasswordControl;					//用户密码

	CSkinHyperLink					m_hlOneMillion;						// 一百万超链接
	CSkinHyperLink					m_hlFiveMillion;					// 5百万超链接
	CSkinHyperLink					m_hlTenMillion;						// 1千万超链接
	CSkinHyperLink					m_hlFiftyMillion;					// 5千万超链接
	CSkinHyperLink					m_hlOneHundredMillion;				// 1亿超链接
	CSkinHyperLink					m_hlFiveHundredMillion;				// 5亿超链接


	//函数定义
public:
	//构造函数
	CDlgInsureTransfer();
	//析构函数
	virtual ~CDlgInsureTransfer();

public:
	// 设置赠送玩家昵称
	VOID SetUserNickName(LPCTSTR lpszNickName);

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定按钮
	virtual VOID OnOK();
	//取消按钮
	virtual VOID OnCancel();

	//控件函数
protected:
	//输入信息
	VOID OnEnChangeScore();
	//忘记密码
	VOID OnBnClickedForgetInsure();
	//转账按钮
	VOID OnBnClickedTransferScore();
	// GameID输入窗口失去焦点
	VOID OnKillFocusNickName();
	// 各种金额超链接
	VOID OnBnClickedHyperLink1();
	VOID OnBnClickedHyperLink2();
	VOID OnBnClickedHyperLink3();
	VOID OnBnClickedHyperLink4();
	VOID OnBnClickedHyperLink5();
	VOID OnBnClickedHyperLink6();

	//消息函数
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);
	//时间消息
	VOID OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedById();
	afx_msg void OnBnClickedByName();
};

//////////////////////////////////////////////////////////////////////////////////

// 修改银行密码
class CDlgInsureModifyPassword : public CDialog
{
	//友元定义
	friend class CDlgInsureMain;
public:
	CSkinButton					m_btModify;				// 修改按钮
	CPasswordControl			m_edInsurePassword1;	// 银行密码1
	CPasswordControl			m_edInsurePassword2;	// 银行密码2
	CPasswordControl			m_edInsurePassword3;	// 银行密码2

protected:
	CDlgInsureMain *			m_pDlgInsureMain;					//窗口指针

public:
	CDlgInsureModifyPassword();
	~CDlgInsureModifyPassword();
	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//创建函数
	virtual BOOL OnInitDialog();
	// 确定按钮
	virtual VOID OnOK();
	// 取消按钮
	virtual VOID OnCancel();
protected:
	// 修改密码按钮
	VOID OnBnClickedModifyPassword();
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//保险柜类
class SHARE_CONTROL_CLASS CDlgInsureMain : public CSkinDialog, public IInsureCounterAction
{
	//变量定义
public:
	static TCHAR					m_szInsurePass[LEN_MD5];			// MD5加密后的银行密码
	static bool						m_bAutoLogOff;						// 关闭对话框时自动注销
protected:
	bool							m_bSaveMission;						//存款任务
	bool							m_bTakeMission;						//取款任务
	bool							m_bQueryMission;					//查询任务
	bool							m_bTransferMission;					//转账任务
	bool                            m_bTransRecordMission;              //转账记录
	bool							m_bQueryNickNameMission;			// 查询昵称
	bool							m_bVerifyInsurePassword;			// 输入银行密码，进行检测
	bool							m_bModifyInsurePassword;			// 修改银行密码

	//查询变量
protected:
	bool							m_bInitInfo;						//银行信息
	BYTE							m_cbInsureMode;						//银行模式
	tagUserInsureInfo				m_UserInsureInfo;					//银行信息
	HBRUSH							m_hBrush;

	//其他控件
protected:
	CSkinTabCtrl					m_TabControl;						//操作选择
	CSkinButton						m_btQueryInfo;						//查询按钮

	//子项窗口
protected:
	CDlgInsureItem					m_DlgInsureItem;					//银行子项
	CDlgInsurePassword				m_DlgInsurePassword;				// 输入银行密码框
	CDlgInsureSave					m_DlgInsureSave;					//存取子项
	CDlgInsureTransfer				m_DlgInsureTransfer;				//转账子项
	CDlgTransRecord					m_DlgTransRecord;					// 转账记录
	CDlgInsureModifyPassword		m_DlgInsureModifyPassword;			// 修改密码

	//函数定义
public:
	//构造函数
	CDlgInsureMain();
	//析构函数
	virtual ~CDlgInsureMain();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定按钮
	virtual VOID OnOK();

	//窗口函数
protected:
	//绘画消息
	virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight);

	//事件处理
public:
	//开始任务
	VOID OnMissionStart();
	//终止任务
	VOID OnMissionConclude();

	//功能函数
protected:
	//查询资料
	VOID QueryUserInsureInfo();
	//设置模式
	VOID SwitchInsureModeView(BYTE cbInsureMode);

	//消息函数
protected:
	//查询按钮
	VOID OnBnClickedQueryInfo();
	//类型改变
	VOID OnTcnSelchangeInsureMode(NMHDR * pNMHDR, LRESULT * pResult);
	// 改变控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);
	// 销毁窗口
//	afx_msg void OnDestory();
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//广场银行
class SHARE_CONTROL_CLASS CDlgInsurePlaza : public CDlgInsureMain, public CMissionItem
{
	//变量定义
protected:
	BYTE                            m_cbByNickName;                      //昵称赠送
	SCORE							m_lScore;							//游戏币数目
	TCHAR							m_szNickName[LEN_NICKNAME];			//用户昵称
//	TCHAR							m_szInsurePass[LEN_PASSWORD];		//银行密码
	DWORD							m_dwGameID;

	TCHAR							m_szSrcInsurePass[LEN_PASSWORD];	// 要修改的原银行密码	
	TCHAR							m_szDesInsurePass[LEN_PASSWORD];	// 要修改的新银行密码	

	//组件变量
protected:
	CMissionManager					m_MissionManager;					//任务管理

	//函数定义
public:
	//构造函数
	CDlgInsurePlaza();
	//析构函数
	virtual ~CDlgInsurePlaza();

	//执行函数
protected:
	//查询事件
	virtual VOID PerformQueryInfo();
	//存入事件
	virtual VOID PerformSaveScore(SCORE lSaveScore);
	//取出事件
	virtual VOID PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass);
	//转账事件
	virtual VOID PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass);
	// 查询转账事件
    virtual VOID PerformQueryTransRecord();
	// 根据玩家GameID查询昵称
	virtual VOID PerformQueryUserNickName(DWORD dwGameID);
	// 处理银行密码
	virtual VOID PerformInsurePassword(LPCTSTR pszInsurePass);
	// 修改银行密码
	virtual VOID PerformModiyInsurePassword(LPCTSTR pszSrcPassword, LPCTSTR pszDesPassword);

	//网络事件
private:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

//房间银行
class SHARE_CONTROL_CLASS CDlgInsureServer : public CDlgInsureMain
{
	//变量定义
protected:
	BYTE                            m_cbByNickName;                     //昵称赠送
	SCORE							m_lScore;							//游戏币数目
	TCHAR							m_szNickName[LEN_NICKNAME];			//用户昵称
//	TCHAR							m_szInsurePass[LEN_PASSWORD];		//银行密码

	//变量定义
protected:
	ITCPSocket *					m_pITCPSocket;						//网络接口
	IClientUserItem *				m_pIMySelfUserItem;					//自己指针

	//函数定义
public:
	//构造函数
	CDlgInsureServer();
	//析构函数
	virtual ~CDlgInsureServer();

	//执行函数
protected:
	//查询事件
	virtual VOID PerformQueryInfo();
	//存入事件
	virtual VOID PerformSaveScore(SCORE lSaveScore);
	//取出事件
	virtual VOID PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass);
	//转账事件
	virtual VOID PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass);
    // 转账记录
	virtual VOID PerformQueryTransRecord();
	// 根据GameID查询玩家昵称
	virtual VOID PerformQueryUserNickName(DWORD dwGameID);
	// 处理银行密码
	virtual VOID PerformInsurePassword(LPCTSTR pszInsurePass);
	// 修改银行密码
	virtual VOID PerformModiyInsurePassword(LPCTSTR pszSrcPassword, LPCTSTR pszDesPassword);
	//功能函数
public:
	//配置银行
	VOID Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem);
	//银行消息
	bool OnServerInsureMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

// 转账成功回执窗口
class CDlgInsureTransferReceipt : public CSkinDialog
{
protected:
	CMD_GR_UserTransferReceipt*	m_pUserTransferReceipt;

	CDlgInsureItem				m_DlgInsureItem;					//银行子项

	CSkinButton					m_btOk;								// 确定按钮
public:
	CDlgInsureTransferReceipt(CWnd * pParentWnd=NULL);
	~CDlgInsureTransferReceipt();

public:
	INT ShowReceipt(CMD_GR_UserTransferReceipt * pUserTransferReceipt);
	//消息映射
protected:
	//初始化消息
	virtual BOOL OnInitDialog();
	//绘画消息
	virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight);
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
protected:
	//绘画背景
//	BOOL OnEraseBkgnd(CDC * pDC);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);
	// 确定按钮
	VOID OnBnClickedOk();
	// 关闭窗口
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////////////

#endif
