#ifndef MODULE_MANAGER_HEAD_HEAD_FILE
#define MODULE_MANAGER_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//平台定义
#include "..\..\GlobalDefine\Platform.h"

//组件定义
#include "..\..\PublicModule\ServiceCore\ServiceCoreHead.h"
#include "..\GameService\GameServiceHead.h"
#include "..\KernelEngine\KernelEngineHead.h"

#include <map>
using namespace std;

//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef MODULE_MANAGER_CLASS
	#ifdef  MODULE_MANAGER_DLL
		#define MODULE_MANAGER_CLASS _declspec(dllexport)
	#else
		#define MODULE_MANAGER_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManager.dll")			//组件 DLL 名字
#else
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManagerD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//结构定义

//模块信息
struct tagGameModuleInfo
{
	//版本信息
	DWORD							dwClientVersion;					//游戏版本
	DWORD							dwServerVersion;					//服务版本
	DWORD							dwNativeVersion;					//本地版本

	//数据属性
	WORD							wGameID;							//模块标识
	TCHAR							szDataBaseAddr[64];					//数据库名
	TCHAR							szDataBaseName[64];					//数据库名

	//模块属性
	TCHAR							szGameName[LEN_KIND];				//游戏名字
	TCHAR							szServerDLLName[LEN_PROCESS];		//进程名字
	TCHAR							szClientEXEName[LEN_PROCESS];		//进程名字
};

//桌子配置信息(从数据库中中读出的相关信息)
struct tagDBGameTableInfo
{
	//索引变量
	WORD							wServerID;							// 房间号码
	WORD							wStartTableID;						//桌号开始ID
	WORD							wEndTableID;						//桌号结束ID

	tagGameTableOption				TableOption;						// 房间配置
};

// 从数据库中读取的玩家创建桌子信息
struct tagDBUserCustomTableInfo
{
	DWORD			dwUserID;											// 创建者用户ID
	WORD			wTableID;											// 占用的桌号
	TCHAR			szSecret[LEN_CUSTOMTABLEPASSWORD];					// 自建桌子密码
	BYTE			cbCustomRule[1024];									// 自定规则
	DWORD			dwRoundCount;										// 创建的局数
	SYSTEMTIME		TableCreateTime;									// 创建的时间
	DWORD			dwPlayRoundCount;									// 已经游戏的局数

	tagDBUserCustomTableInfo()
	{
		memset(this, 0, sizeof(tagDBUserCustomTableInfo));
	}
};

//房间信息(从数据库中读出的相关信息)
struct tagGameServerInfo
{
	//索引变量
	WORD							wGameID;							//模块标识
	WORD							wServerID;							//房间号码

	//挂接属性
	WORD							wKindID;							//挂接类型
	WORD							wNodeID;							//挂接节点
	WORD							wSortID;							//排列标识

	//版本信息
	DWORD							dwClientVersion;					//游戏版本
	DWORD							dwServerVersion;					//服务版本
	DWORD							dwNativeVersion;					//本地版本

	//模块属性
	TCHAR							szGameName[LEN_KIND];				//游戏名字
	TCHAR							szServerDLLName[LEN_PROCESS];		//进程名字
	TCHAR							szClientEXEName[LEN_PROCESS];		//进程名字

	//税收配置
	LONG							lCellScore;							//单位积分
	WORD							wRevenueRatio;						//税收比例
	SCORE							lServiceScore;						//服务费用

	//房间配置
	SCORE							lRestrictScore;						//限制积分
	SCORE							lMinTableScore;						//最低积分
	SCORE							lMinEnterScore;						//最低积分
	SCORE							lMaxEnterScore;						//最高积分

	//会员限制
	BYTE							cbMinEnterMember;					//最低会员
	BYTE							cbMaxEnterMember;					//最高会员

	//房间配置
	DWORD							dwServerRule;						//房间规则
	DWORD							dwAttachUserRight;					//附加权限

	//房间属性
	WORD							wMaxPlayer;							//最大数目
	WORD							wTableCount;						//桌子数目
	WORD							wServerPort;						//服务端口
	WORD							wServerType;						//房间类型
	TCHAR							szServerName[LEN_SERVER];			//房间名称

	//分组设置
	BYTE							cbDistributeRule;					//分组规则
	WORD							wMinDistributeUser;					//最少人数
	WORD							wMaxDistributeUser;					//最多人数
	WORD							wDistributeTimeSpace;				//分组间隔
	WORD							wDistributeDrawCount;				//分组局数
	WORD							wDistributeStartDelay;				//开始延时

	//数据属性
	TCHAR							szDataBaseName[64];					//数据库名
	TCHAR							szDataBaseAddr[64];					//地址信息

	//数据设置
//	BYTE							cbCustomRule[1024];					//自定规则
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//机器序列

	vector<tagDBGameTableInfo>			TableConfig;					// 桌子配置
	vector<tagDBUserCustomTableInfo>	vectUserCustomTableInfo;		// 玩家自建桌子配置
};


//房间信息
struct tagGameServerCreate
{
	//索引变量
	WORD							wGameID;							//模块标识
	WORD							wServerID;							//房间号码

	//挂接属性
	WORD							wKindID;							//挂接类型
	WORD							wNodeID;							//挂接节点
	WORD							wSortID;							//排列标识

	//税收配置
	LONG							lCellScore;							//单位积分
	WORD							wRevenueRatio;						//税收比例
	SCORE							lServiceScore;						//服务费用

	//限制配置
	SCORE							lRestrictScore;						//限制积分
	SCORE							lMinTableScore;						//最低积分
	SCORE							lMinEnterScore;						//最低积分
	SCORE							lMaxEnterScore;						//最高积分

	//会员限制
	BYTE							cbMinEnterMember;					//最低会员
	BYTE							cbMaxEnterMember;					//最高会员

	//房间配置
	DWORD							dwServerRule;						//房间规则
	DWORD							dwAttachUserRight;					//附加权限

	//房间属性
	WORD							wMaxPlayer;							//最大数目
	WORD							wTableCount;						//桌子数目
	WORD							wServerPort;						//服务端口
	WORD							wServerType;						//房间类型
	TCHAR							szServerName[LEN_SERVER];			//房间名称

	//分组设置
	BYTE							cbDistributeRule;					//分组规则
	WORD							wMinDistributeUser;					//最少人数
	WORD							wMaxDistributeUser;					//最多人数
	WORD							wDistributeTimeSpace;				//分组间隔
	WORD							wDistributeDrawCount;				//分组局数
	WORD							wDistributeStartDelay;				//开始延时

	//数据属性
	TCHAR							szDataBaseName[32];					//数据库名
	TCHAR							szDataBaseAddr[64];					//地址信息

	//数据设置
	BYTE							cbCustomRule[1024];					//自定规则
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//机器序列
};



//////////////////////////////////////////////////////////////////////////////////
//配置参数
struct tagModuleInitParameter
{
	tagGameServiceAttrib			GameServiceAttrib;					//服务属性
	tagGameServiceOption			GameServiceOption;					//服务配置
	tagGameSquareOption				GameSquareOptionArray[20];			//比赛配置
	BYTE							cbGameSquareOptionArrayCount;		//比赛场次
//	tagGameTableInfo				TableConfig[MAX_TABLE_OPTION];		//桌子配置(和数据库相关的映射配置)
	vector<tagDBGameTableInfo>		TableConfig;						// 桌子配置
	tagGameServiceTableOption		GameServiceTableOption;				//房间桌子配置
};

//////////////////////////////////////////////////////////////////////////////////

//导出文件
#ifndef MODULE_MANAGER_DLL 
	
	#include "ListControl.h"
	#include "DlgServerItem.h"
	#include "DlgServerMatch.h"
	#include "DlgServerWizard.h"
	
	#include "ModuleListControl.h"
	#include "ModuleDBParameter.h"
	#include "ModuleInfoManager.h"

	#include "ServerCustomRule.h"
	#include "ServerListControl.h"
	#include "ServerInfoManager.h"

#endif

//////////////////////////////////////////////////////////////////////////////////

#endif