#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "ServerListManager.h"

//////////////////////////////////////////////////////////////////////////////////
//数据定义

//连接类型
// #define CLIENT_KIND_FALSH			1									//网页类型
// #define CLIENT_KIND_MOBILE			2									//手机类型
// #define CLIENT_KIND_COMPUTER		3									//电脑类型

//绑定参数
struct tagBindParameter
{
	//网络参数
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwActiveTime;						//激活时间

	//连接信息
	WORD							wModuleID;							//模块标识
//	BYTE							cbClientKind;						//连接类型
	DWORD							dwPlazaVersion;						//大厅版本

	// 登录系统
	WORD							wPlatformID;						// 系统编号
};

//////////////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	bool							m_bNeekCorrespond;					//协调标志
	bool                            m_bShowServerStatus;                //显示服务器状态

	//变量定义
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//辅助数组

	//组件变量
protected:
	CServerListManager							m_ServerListManager;				//列表管理
	map<DWORD, vector<tagAreaGameKind> >		m_mapAreaGameKind;					//AreaID和KindID的映射
	map<DWORD, vector<tagAreaGameServer> >		m_mapAreaGameServer;				//AreaID和ServerID的映射
	map<WORD, tagGameKindEx>					m_mapGameKindEx;					// GameKind扩展信息
	map<WORD, tagCustomTablePropertyConfig>		m_mapCustomTablePropertyConfig;		// 自定义游戏开房道具消耗

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	ITCPSocketService *				m_pITCPSocketService;				//协调服务

	//视频配置
protected:
	WORD							m_wAVServerPort;					//视频端口
	DWORD							m_dwAVServerAddr;					//视频地址

	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//异步接口
public:
	//启动事件
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//事件接口
public:
	//控制事件
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);
	//自定事件
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//内核事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//连接事件
public:
	//连接事件
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//网络事件
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//连接处理
protected:
	//注册事件
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//服务信息
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//远程服务
	bool OnTCPSocketMainRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//网络事件
protected:
	//登录处理
	bool OnTCPNetworkMainPCLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//列表处理
	bool OnTCPNetworkMainPCServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//服务处理
	bool OnTCPNetworkMainPCUserService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//远程处理
	bool OnTCPNetworkMainPCRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//手机事件
protected:
	//登录处理
	bool OnTCPNetworkMainMBLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//列表处理
	bool OnTCPNetworkMainMBServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 请求系统文字信息
	bool OnTCPNetworkMainMBSystemMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//网络事件
protected:
	//I D 登录
	bool OnTCPNetworkSubPCLogonGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//帐号登录
	bool OnTCPNetworkSubPCLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//帐号注册
	bool OnTCPNetworkSubPCRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//手机事件
protected:
	//I D 登录
	bool OnTCPNetworkSubMBLogonGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//帐号登录
	bool OnTCPNetworkSubMBLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//帐号注册
	bool OnTCPNetworkSubMBRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 请求充值公告
	bool OnTCPNetworkSubMBRechargeMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 请求客服公告
	bool OnTCPNetworkSubMBServiceMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 第三方登录
	bool OnTCPNetworkSubLogonThirdParty(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//服务处理
protected:
	//存入游戏币
	bool OnTCPNetworkUserSaveScore(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
 	//取出游戏币
	bool OnTCPNetworkUserTakeScore(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 转账游戏币
	bool OnTCPNetworkUserTransferScore(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询银行
	bool OnTCPNetworkQueryInsureInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 通过GameID查询昵称
	bool OnTCPNetworkQueryNicknameByGameID(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 验证银行密码
	bool OnTCPNetworkVerifyInsurePassword(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 查询魅力兑换信息
	bool OnTCPNetworkQueryExchangeLoveliness(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 兑换魅力值
	bool OnTCPNetworkExchangeLoveliness(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 分享成功
	bool OnTCPNetworkMBShareSuccess(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 创建自建桌子
	bool OnTCPNetworkUserCreateCustomTable(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 加入自建桌子
	bool OnTCPNetworkUserJoinCustomTable(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 查询加入的自建桌子
	bool OnTCPNetworkUserQueryCustomTable(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// 用户转账钻石
	bool OnTCPNetworkUserTransferDiamond(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//游戏事件
protected:
	//登录成功
	bool OnDBPCLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBPCLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBPCLogonValidateMBCard(DWORD dwContextID, VOID * pData, WORD wDataSize);
//补充资料成功
	bool OnDBPCProfileSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);	// add by chenj
	//补充资料失败
	bool OnDBPCProfileFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);	// add by chenj
	//检查昵称
	bool OnDBPCCheckNick(DWORD dwContextID, VOID * pData, WORD wDataSize);		// add by chenj
	//用户头像
	bool OnDBPCUserFaceInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户信息
	bool OnDBPCUserIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//银行信息
	bool OnDBPCUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
    //转账记录
    bool OnDBPCUserTransRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//银行成功
	bool OnDBPCUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//银行失败
	bool OnDBPCUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//用户信息
	bool OnDBPCUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//操作成功
	bool OnDBPCOperateSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//操作失败
	bool OnDBPCOperateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 转账回执
	bool OnDBPCUserTransReceipt(DWORD dwContextID,VOID * pData, WORD wDataSize);
	// 读取魅力兑换配置成功
	bool OnDBPExchangeLovelinessInfo(DWORD dwContextID,VOID * pData, WORD wDataSize);
	// 魅力兑换操作返回
	bool OnDBPExchangeLoveliness(DWORD dwContextID,VOID * pData, WORD wDataSize);
	// 签到成功
	bool OnDBMBSignInCheckSuccess(DWORD dwContextID,VOID* pData,WORD wDataSize);
	// 读取的关于客服的配置消息
	bool OnDBMBCustomServiceMessage(DWORD dwContextID,VOID* pData,WORD wDataSize);
	// 读取关于充值的配置消息
	bool OnDBMBRechargeMessage(DWORD dwContextID,VOID* pData,WORD wDataSize);
	// 加入自定义桌子
	bool OnDBUserCustomTableSuccess(DWORD dwContextID, VOID* pData, WORD wDataSize);
	// 加入自定义桌子失败
	bool OnDBUserCustomTableFailed(DWORD dwContextID, VOID* pData, WORD wDataSize);
	// 钻石操作成功
	bool OnDBUserDiamondSuccess(DWORD dwContextID, VOID* pData, WORD wDataSize);
	// 钻石操作失败
	bool OnDBUserDiamondFialed(DWORD dwContextID, VOID* pData, WORD wDataSize);

	//手机事件
protected:
	//登录成功
	bool OnDBMBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBMBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//列表事件
protected:
	//游戏种类
	bool OnDBPCGameTypeItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏类型
	bool OnDBPCGameKindItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏节点
	bool OnDBPCGameNodeItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏定制
	bool OnDBPCGamePageItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//区域ID相关的GameKindItem
	bool OnDBMBAreaGameKindItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//区域ID相关的GameServerItem
	bool OnDBMBAreaGameServerItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// APP相关的资源列表
	bool OnDBMBGameKindItemEx(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// 自建桌子,消耗道具和局数配置
	bool OnDBMBCustomTablePropertyConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏列表
	bool OnDBPCGameListResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//辅助函数
protected:
	//版本检测
	bool CheckPlazaVersion(BYTE cbDeviceType, DWORD dwPlazaVersion, DWORD dwSocketID, bool bCheckLowVer=true);
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
	// DeviceType转平台ID
	WORD DeviceTypeToPlatformID(BYTE cbDeviceType);

	//发送列表
protected:
	//发送类型
	VOID SendGameTypeInfo(DWORD dwSocketID);
	//发送种类
	VOID SendGameKindInfo(DWORD dwSocketID);
	//发送节点
	VOID SendGameNodeInfo(DWORD dwSocketID, WORD wKindID);
	//发送定制
	VOID SendGamePageInfo(DWORD dwSocketID, WORD wKindID);
	//发送房间
	VOID SendGameServerInfo(DWORD dwSocketID, WORD wKindID);

	//手机列表
protected:
	//发送类型
	VOID SendMobileTypeInfo(DWORD dwSocketID);
	//发送种类
	// wModuleID==INVALID_WORD 表明不筛选,发送全部
	// dwAreaID==INVALID_DWORD 表明不筛选,发送全部
	VOID SendMobileKindInfo(DWORD dwSocketID, WORD wModuleID, DWORD dwAreaID = INVALID_DWORD, WORD wPlatformID = CC_PLATFORM_UNKNOWN);
	//发送房间
	VOID SendMobileServerInfo(DWORD dwSocketID, WORD wModuleID, DWORD dwAreaID = INVALID_DWORD, WORD wPlatformID = CC_PLATFORM_UNKNOWN);
	// 发送GameKind扩展信息
	VOID SendMobileKindInfoEx(DWORD dwSocketID, WORD wModuleID, DWORD dwAreaID = INVALID_DWORD, WORD wPlatformID = CC_PLATFORM_UNKNOWN);
	// 发送加入自定义桌子的信息
	VOID SendUserCustomTableInfo(DWORD dwSocketID, WORD wTableID, TCHAR szSecret[LEN_CUSTOMTABLEPASSWORD], bool );
	// 发送自建桌子房间的道具消耗信息
	VOID SendCustomTablePropertyConfig(DWORD dwSocketID, WORD wModuleID, DWORD dwAreaID = INVALID_DWORD, WORD wPlatformID = CC_PLATFORM_UNKNOWN);
};

//////////////////////////////////////////////////////////////////////////////////

#endif