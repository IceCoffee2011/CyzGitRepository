#ifndef EXCHANGE_LOVELINESS_HEAD_FILE
#define EXCHANGE_LOVELINESS_HEAD_FILE

#pragma once


// CDlgExchangeLoveliness 对话框

class CDlgExchangeLoveliness : public CSkinDialog, public CMissionItem
{
//	DECLARE_DYNAMIC(CDlgExchangeLoveliness)

public:
	CDlgExchangeLoveliness();   // 标准构造函数
	virtual ~CDlgExchangeLoveliness();
public:
	CMissionManager					m_MissionManager;						// 任务管理
	bool							m_bQueryMission;						// 获取配置操作
	bool							m_bExchangeMisssion;					// 兑换操作
protected:
	CSkinButton						m_btExchange;							// 兑换按钮
	CSkinButton						m_btCancel;								// 取消兑换

	TCHAR							m_szTips[1024];							// 提示			
	TCHAR							m_szExchangeInfo[1024];					// 绘制转账信息
// 对话框数据
//	enum { IDD = IDD_DLG_EXCHANGE_LOVELINESS };
	//网络事件
protected:
	//查询资料
	VOID QueryExchangeLovelinessInfo();
	//开始任务
	VOID OnMissionStart();
	//终止任务
	VOID OnMissionConclude();

private:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);
protected:
	//创建函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();
	// 取消
	virtual VOID OnCancel();
	// 数字加逗号处理
	VOID SwitchScoreFormat(SCORE lScore, UINT uSpace, LPTSTR pszBuffer, WORD wBufferSize);
	
	//窗口函数
protected:
	//绘画消息
	virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};

#endif
