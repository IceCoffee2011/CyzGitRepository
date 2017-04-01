#pragma once


#include "DlgServiceItem.h"

class CDlgServiceLockMachine : public CDlgServiceItem, public CMissionItem
{

protected:
	CPasswordControl			m_edInsurePassword;			// 银行密码(绑定机器时需要输入)
	CSkinButton					m_btLockMachine;			// 锁定机器
	CSkinButton					m_btUnLockMachine;			// 解锁机器

protected:
	TCHAR						m_szPassword[LEN_PASSWORD];	//用户密码
	CMissionManager				m_MissionManager;			//任务管理
	BYTE						m_cbMachine;				//绑定标志(与当前实际状态相反)

public:
	CDlgServiceLockMachine();
	~CDlgServiceLockMachine();

	//重载函数
public:
	//获取标题
	virtual LPCTSTR GetServiceItemName() { return TEXT("绑定机器"); }

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//创建函数
	virtual BOOL OnInitDialog();
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//网络事件
private:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);

	//事件处理
protected:
	//开始任务
	VOID OnMissionStart();
	//终止任务
	VOID OnMissionConclude();

	//消息映射
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
};
