#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////////////

//数据库类
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//用户数据库
protected:
	CDataBaseAide					m_AccountsDBAide;					//用户数据库
	CDataBaseHelper					m_AccountsDBModule;					//用户数据库

	//游戏币数据库
protected:
	CDataBaseAide					m_TreasureDBAide;					//游戏币数据库
	CDataBaseHelper					m_TreasureDBModule;					//游戏币数据库

	//平台数据库
protected:
	CDataBaseAide					m_PlatformDBAide;					//平台数据库
	CDataBaseHelper					m_PlatformDBModule;					//平台数据库

	// 用户自建桌子数据库
protected:
	CDataBaseAide					m_UserCustomDBAide;					//用户自建数据库
	CDataBaseHelper					m_UserCustomDBModule;				//用户自建数据库

	//组件变量
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//数据事件

	//函数定义
public:
	//构造函数
	CDataBaseEngineSink();
	//析构函数
	virtual ~CDataBaseEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//系统事件
public:
	//启动事件
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx);

	//内核事件
public:
	//时间事件
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//登录服务
protected:
	//I D 登录
	bool OnRequestLogonGameID(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//帐号登录
	bool OnRequestLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//帐号注册
	bool OnRequestRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//手机登录
protected:
	//I D 登录
	bool OnMobileLogonGameID(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//帐号登录
	bool OnMobileLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//帐号注册
	bool OnMobileRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 请求充值公告
	bool OnMobileReqRechargeMessage(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 请求
	bool OnMobileReqCustomServiceMessage(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 第三方登录
	bool OnThirdPartyLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 分享成功
	bool OnShareSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 用户加入自定义桌子
	bool OnUserJoinCustomTable(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 用户查询加入自定义桌子的信息
	bool OnUserQueryCustomTable(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 用户转账钻石
	bool OnRequestUserTransferDiamond(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//账号服务
protected:
	//修改机器
	bool OnRequestModifyMachine(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改密码
	bool OnRequestModifyLogonPass(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改密码
	bool OnRequestModifyInsurePass(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改签名
	bool OnRequestModifyUnderWrite(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改头像
	bool OnRequestModifySystemFace(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改头像
	bool OnRequestModifyCustomFace(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//修改资料
	bool OnRequestModifyIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//补充资料
	bool OnRequestUserProfile(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//检测昵称
	bool OnRequestCheckNick(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//银行服务
protected:
	//存入游戏币
	bool OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//提取游戏币
	bool OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//转账游戏币
	bool OnRequestUserTransferScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 验证银行密码
	bool OnRequestVerifyInsurePassword(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//信息查询
protected:
	//查询资料
	bool OnRequestQueryIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询银行
	bool OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询用户
	bool OnRequestQueryTransferUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
    //转账记录
	bool OnRequestQueryTransRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 根据GameID查询昵称
	bool OnRequestQueryNickNameByGameID(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 获取魅力兑换等信息
	bool OnRequestQueryExchangeLoveliness(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 兑换魅力
	bool OnRequestExchangeLoveliness(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 签到
	bool OnRequestSignInCheck(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//系统功能
protected:
	//加载列表
	bool OnRequestLoadGameList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//在线信息
	bool OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//结果处理
protected:
	//登录结果
	VOID OnLogonDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient);
	//银行结果
	VOID OnInsureDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient);
	//操作结果
	VOID OnOperateDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient);
	// 自建桌子结果
	VOID OnUserCustomTableDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient);
	// 钻石结果
	VOID OnDiamondDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

};

//////////////////////////////////////////////////////////////////////////////////

#endif
