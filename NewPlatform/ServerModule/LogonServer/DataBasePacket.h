#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//请求数据包

//登录命令
#define	DBR_GP_LOGON_GAMEID			1									//I D 登录
#define	DBR_GP_LOGON_ACCOUNTS		2									//帐号登录
#define DBR_GP_REGISTER_ACCOUNTS	3									//注册帐号
#define DBR_GP_REGISTER_PROFILE		4									//补充资料
#define DBR_GP_CHECKNICK			5									//检测昵称

//账号服务
#define DBR_GP_MODIFY_MACHINE		10									//修改机器
#define DBR_GP_MODIFY_LOGON_PASS	11									//修改密码
#define DBR_GP_MODIFY_INSURE_PASS	12									//修改密码
#define DBR_GP_MODIFY_UNDER_WRITE	13									//修改签名
#define DBR_GP_MODIFY_INDIVIDUAL	14									//修改资料

//头像命令
#define DBR_GP_MODIFY_SYSTEM_FACE	20									//修改头像
#define DBR_GP_MODIFY_CUSTOM_FACE	21									//修改头像

//银行命令
#define DBR_GP_USER_SAVE_SCORE		30									//存入游戏币
#define DBR_GP_USER_TAKE_SCORE		31									//提取游戏币
#define DBR_GP_USER_TRANSFER_SCORE	32									//转账游戏币

//查询命令
#define DBR_GP_QUERY_INDIVIDUAL				40									//查询资料
#define DBR_GP_QUERY_INSURE_INFO			41									//查询银行
#define DBR_GP_QUERY_USER_INFO				42									//查询用户
#define DBR_GP_QUERY_TRANSRECORD			43
#define DBR_GP_QUERY_NICKNAME_BY_GAMEID		44									// 通过GameID查询昵称
#define DBR_GP_VERIFY_INSURE_PASSWORD		45									// 验证银行密码
#define DBR_GP_QUERY_EXCHANGE_LOVELINESS	46							// 查询魅力兑换信息
#define DBR_GP_EXCHANGE_LOVELINESS			47							// 兑换魅力

//系统命令
#define DBR_GP_LOAD_GAME_LIST		50									//加载列表
#define DBR_GP_ONLINE_COUNT_INFO	51									//在线信息


//////////////////////////////////////////////////////////////////////////////////

//ID 登录
struct DBR_GP_LogonGameID
{
	//登录信息
	DWORD							dwGameID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	BYTE							cbNeeValidateMBCard;				//密保校验

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//帐号登录
struct DBR_GP_LogonAccounts
{
	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	BYTE							cbNeeValidateMBCard;				//密保校验

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//帐号注册
struct DBR_GP_RegisterAccounts
{
	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szSpreader[LEN_ACCOUNTS];			//推荐帐号
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//证件号码
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字
	TCHAR							szMobile[LEN_MOBILE_PHONE];

	//密码变量
	TCHAR							szLogonPass[LEN_MD5];				//登录密码
	TCHAR							szInsurePass[LEN_MD5];				//银行密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

struct DBR_GP_RegisterProfile
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	BYTE							cbGender;							//用户性别
	TCHAR							szQQ[LEN_QQ];						//Q Q 号码
	TCHAR							szMobile[LEN_MOBILE_PHONE];			//手机号码
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		// 身份证号码
	TCHAR							szCompellation[LEN_COMPELLATION];	// 真实姓名
	TCHAR							szSecurityQuestion[LEN_UNDER_WRITE];		// 密保问题
	TCHAR							szSecurityAnswer[LEN_UNDER_WRITE];		// 密保回答
	WORD							wFaceID;							// 头像ID	
};

struct DBR_GP_CheckNick
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
};//修改机器
struct DBR_GP_ModifyMachine
{
	BYTE							cbBind;								//绑定标志
	DWORD							dwUserID;							//用户标识
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改密码
struct DBR_GP_ModifyLogonPass
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改密码
struct DBR_GP_ModifyInsurePass
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改签名
struct DBR_GP_ModifyUnderWrite
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
};

//修改头像
struct DBR_GP_ModifySystemFace
{
	//用户信息
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//机器信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改头像
struct DBR_GP_ModifyCustomFace
{
	//用户信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//图片信息

	//机器信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改资料
struct DBR_GP_ModifyIndividual
{
	//验证资料
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//帐号资料
	BYTE							cbGender;							//用户性别
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//用户信息
	TCHAR							szUserNote[LEN_USER_NOTE];			//用户说明
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字

	//电话号码
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//固定电话
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//移动电话

	//联系资料
	TCHAR							szQQ[LEN_QQ];						//Q Q 号码
	TCHAR							szEMail[LEN_EMAIL];					//电子邮件
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
};

//查询资料
struct DBR_GP_QueryIndividual
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
};

// 查询昵称
struct DBR_GP_QueryNickNameByGameID
{
	DWORD							dwGameID;							// 用户GameID
};

//存入游戏币
struct DBR_GP_UserSaveScore
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lSaveScore;							//存入游戏币
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//取出游戏币
struct DBR_GP_UserTakeScore
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lTakeScore;							//提取游戏币
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//转账游戏币
struct DBR_GP_UserTransferScore
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
	BYTE                            cbByNickName;                       //昵称赠送
	SCORE							lTransferScore;						//转账游戏币
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//查询银行
struct DBR_GP_QueryInsureInfo
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						//连接地址
};

//查询用户
struct DBR_GP_QueryInsureUserInfo
{
	BYTE                            cbByNickName;                       //昵称赠送
	TCHAR							szNickName[LEN_NICKNAME];			//目标用户
};

//用户资料
struct DBO_GP_UserTransferUserInfo
{
	DWORD							dwGameID;							//用户 I D
	TCHAR							szNickName[LEN_NICKNAME];			//帐号昵称
};

//在线信息
struct DBR_GP_OnLineCountInfo
{
	WORD							wKindCount;							//类型数目
	DWORD							dwOnLineCountSum;					//总在线数
	tagOnLineInfoKind				OnLineCountKind[MAX_KIND];			//类型在线
};

// 验证用户银行密码
struct DBR_GP_VerifyInsurePassword
{
	DWORD							dwUserID;						//用户 I D
	TCHAR							szInsurePassword[LEN_MD5];		//帐号昵称
};

// 查询魅力兑换等消息
struct DBR_GP_QueryExchangeLoveliness
{
	DWORD							dwUserID;							//用户 I D
};

// 查询魅力兑换消息回复
struct DBO_GP_ExchangeLovelinessInfo
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwLoveliness;						// 用户魅力
	DWORD							dwAvailbleAmount;					// 可兑换魅力
	DWORD							dwFinalAmount;						// 实际能得到的货币(可兑换魅力 * 兑换率)
	TCHAR							szTips[1024];						// 兑换提示
	TCHAR							szErr[128];							// 错误提示
};

// 进行魅力兑换
struct DBR_GP_ExchangeLoveliness
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientAddr;						// 用户IP
};

// 返回魅力兑换的结果
struct DBO_GP_ExchangeLoveliness
{
	DWORD							dwUserID;							// 用户I D
	DWORD							dwLoveliness;						// 兑换后魅力
	SCORE							lScore;								// 兑换后游戏货币
	SCORE							lInsureScore;						// 兑换后银行货币
	LONG							lResultCode;						// 操作结果
	TCHAR							szDescribeString[128];				// 提示消息
};

//////////////////////////////////////////////////////////////////////////////////
//输出信息

//登录结果
#define DBO_GP_LOGON_SUCCESS		100									//登录成功
#define DBO_GP_LOGON_FAILURE		101									//登录失败
#define DBR_GP_VALIDATE_MBCARD		102									//登录失败
#define DBO_GP_PROFILE_SUCCESS		103									//补充资料成功
#define DBO_GP_PROFILE_FAILURE		104									//补充资料失败
#define DBO_GP_CHECKNICK			105									//

//操作结果
#define DBO_GP_USER_FACE_INFO		110									//用户头像
#define DBO_GP_USER_INDIVIDUAL		111									//用户资料

//银行命令
#define DBO_GP_USER_INSURE_INFO			120									//银行资料
#define DBO_GP_USER_INSURE_SUCCESS		121									//银行成功
#define DBO_GP_USER_INSURE_FAILURE		122									//银行失败
#define DBO_GP_USER_INSURE_USER_INFO	123									//用户资料
#define DBR_GP_TRANS_RECORD				124		                            //转账记录
#define DBO_GP_INSURE_TRANS_RECEIPT		125									// 转账回执	
#define DBO_GP_QUERY_EXCHANGE_LOVELINESS_INFO 126								// 魅力兑换配置信息
#define DBO_GP_EXCHANGE_LOVELINESS		127									// 魅力兑换

//列表结果
#define DBO_GP_GAME_TYPE_ITEM				130									//种类信息
#define DBO_GP_GAME_KIND_ITEM				131									//类型信息
#define DBO_GP_GAME_NODE_ITEM				132									//节点信息
#define DBO_GP_GAME_PAGE_ITEM				133									//定制信息
#define DBO_MB_AREA_GAME_KIND_ITEM			134									//区域ID相关的GameKindItem
#define DBO_MB_AREA_GAME_SERVER_ITEM		135									//区域ID相关的GameKindItem
#define DBO_MB_GAME_KIND_ITEM_EX			136									//GameKind扩展信息,用于App
#define DBO_MB_CUSTOMTABLE_PROPERTYCONFIG	137									//自建房间道具数量配置
#define DBO_GP_GAME_LIST_RESULT				140									//加载结果

//服务结果
#define DBO_GP_OPERATE_SUCCESS		500									//操作成功
#define DBO_GP_OPERATE_FAILURE		501									//操作失败

#define DBO_GR_USER_CUSTOMTABLE_SUCCESS		900							// 用户自定义桌子/加入自定义桌子成功
#define DBO_GR_USER_CUSTOMTABLE_FAILURE		901							// 用户自定义桌子/加入自定义桌子失败
#define DBO_MB_USER_DIAMOND_SUCCESS			902							// 用户操作钻石成功
#define DBO_MB_USER_DIAMOND_FAILURE			903							// 用户操作钻石失败

//登录成功
struct DBO_GP_LogonSuccess
{
	//属性资料
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	DWORD							dwGroupID;							//社团标识
	DWORD							dwCustomID;							//自定索引
	DWORD							dwUserMedal;						//用户奖牌
	DWORD							dwExperience;						//经验数值
	DWORD							dwLoveLiness;						//用户魅力
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字

	//用户游戏币
	SCORE							lUserScore;							//用户游戏币
	SCORE							lUserInsure;						//用户银行

	//用户资料
	BYTE							cbGender;							//用户性别
	BYTE							cbMoorMachine;						//锁定机器
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//会员资料
	BYTE							cbMemberOrder;						//会员等级
	SYSTEMTIME						MemberOverDate;						//到期时间

	WORD							wFillIn;							//是否完善注册资料 by chenj

	//描述信息
	TCHAR							szDescribeString[128];				//描述消息
};

//登录失败
struct DBO_GP_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//登录失败
struct DBR_GP_ValidateMBCard
{
	UINT							uMBCardID;						//机器序列
};

//补充资料
struct DBO_GP_UserProfile
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	BYTE							cbGender;							//用户性别
	TCHAR							szQQ[LEN_QQ];						//Q Q 号码
	TCHAR							szMobile[LEN_MOBILE_PHONE];			//手机号码
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
	WORD							wFaceID;							//头像标识
};

struct DBO_GP_CheckNick
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//描述消息
};
//头像信息
struct DBO_GP_UserFaceInfo
{
	WORD							wFaceID;							//头像标识
	DWORD							dwCustomID;							//自定索引
};

//个人资料
struct DBO_GP_UserIndividual
{
	//用户信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szUserNote[LEN_USER_NOTE];			//用户说明
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字

	//电话号码
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//固定电话
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//移动电话

	//联系资料
	TCHAR							szQQ[LEN_QQ];						//Q Q 号码
	TCHAR							szEMail[LEN_EMAIL];					//电子邮件
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
};

//银行资料
struct DBO_GP_UserInsureInfo
{
	WORD							wRevenueTake;						//税收比例
	WORD							wRevenueTransfer;					//税收比例
	WORD							wServerID;							//房间标识
	SCORE							lUserScore;							//用户游戏币
	SCORE							lUserInsure;						//银行游戏币
	SCORE							lTransferPrerequisite;				//转账条件
	DWORD							dwDiamondCount;						// 钻石数量
};
//转账记录
struct DBR_GP_UserTransRecord
{
    DWORD                               dwGameID;
    TCHAR                               szSourceAccount[LEN_ACCOUNTS];
    TCHAR                               szTargetAccounts[LEN_ACCOUNTS];
    LONGLONG                            lSwapScore;
    LONGLONG                            lRevenue;
    TCHAR                               szTime[32];
	DWORD								dwSourceID;
	DWORD								dwTargetID;
    bool                                bOver;
};
//银行成功
struct DBO_GP_UserInsureSuccess
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lSourceScore;						//原来游戏币
	SCORE							lSourceInsure;						//原来游戏币
	SCORE							lInsureRevenue;						//银行税收
	SCORE							lVariationScore;					//游戏币变化
	SCORE							lVariationInsure;					//银行变化
	TCHAR							szDescribeString[128];				//描述消息
};

//银行失败
struct  DBO_GP_UserInsureFailure
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//描述消息
};

//操作失败
struct DBO_GP_OperateFailure
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//描述消息
};

//操作成功
struct DBO_GP_OperateSuccess
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//成功消息
};

//////////////////////////////////////////////////////////////////////////////////

//游戏类型
struct DBO_GP_GameType
{
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wTypeID;							//类型索引
	TCHAR							szTypeName[LEN_TYPE];				//种类名字
};

//游戏种类
struct DBO_GP_GameKind
{
	WORD							wTypeID;							//类型索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//类型索引
	WORD							wGameID;							//模块索引
	TCHAR							szKindName[LEN_KIND];				//游戏名字
	TCHAR							szProcessName[LEN_PROCESS];			//进程名字
};

//游戏节点
struct DBO_GP_GameNode
{
	WORD							wKindID;							//名称索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wNodeID;							//节点索引
	TCHAR							szNodeName[LEN_NODE];				//节点名称
};

//定制类型
struct DBO_GP_GamePage
{
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wPageID;							//定制索引
	WORD							wOperateType;						//控制类型
	TCHAR							szDisplayName[LEN_PAGE];			//显示名称
};

// 区域ID和KindID映射(手机用到)
struct DBO_MB_AreaGameKind
{
	DWORD							dwAreaID;							//区域索引
	WORD							wKindID;							//类型索引
	WORD							wSortID;							//排序索引
	WORD							wPlatformID;						//系统编号
};

// 区域ID和KindID映射(手机用到)
struct DBO_MB_AreaGameServer
{
	DWORD							dwAreaID;							//区域索引
	WORD							wServerID;							//排序索引
	WORD							wSortID;							//类型索引
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	WORD							wPlatformID;						//系统编号
};

// App资源更新用到
struct DBO_MB_GameKindEx
{
	WORD		wKindID;
	DWORD		dwAppVersion;
	TCHAR		szAppDownloadURL[LEN_URL];			// 地址长度
};

// 自建桌子,消耗道具和局数的配置
struct DBO_MB_CustomTablePropertyConfig
{
	WORD		wKindID;							// 游戏类型标识
	WORD		wPropertyConsumeCount;				// 道具消耗数量
	WORD		wRoundCount;						// 对应的局数
};

//加载结果
struct DBO_GP_GameListResult
{
	BYTE							cbSuccess;							//成功标志
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//手机数据包

//登录命令
#define	DBR_MB_LOGON_GAMEID			500									//I D 登录
#define	DBR_MB_LOGON_ACCOUNTS		501									//帐号登录
#define DBR_MB_REGISTER_ACCOUNTS	502									//注册帐号
#define DBR_TP_LOGON_ACCOUNTS		503									//第三方登录账号

//登录结果
#define DBO_MB_LOGON_SUCCESS		600									//登录成功
#define DBO_MB_LOGON_FAILURE		601									//登录失败

//////////////////////////////////////////////////////////////////////////////////

//ID 登录
struct DBR_MB_LogonGameID
{
	//登录信息
	DWORD							dwGameID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//帐号登录
struct DBR_MB_LogonAccounts
{
	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

// 第三方账号登录
struct DBR_TP_LogonAccounts
{
	WORD							wFaceID;							//头像标识

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
//	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码

	unsigned int					nThirdPartyID;						// 第三方ID
	TCHAR							szSessionID[LEN_SESSION_ID];		// SessionID
	TCHAR							szNickName[LEN_NICKNAME];			// 玩家昵称

	// 区域ID
	DWORD							dwAreaID;							//区域ID

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//帐号注册
struct DBR_MB_RegisterAccounts
{
	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称

	//密码变量
	TCHAR							szLogonPass[LEN_MD5];				//登录密码
	TCHAR							szInsurePass[LEN_MD5];				//银行密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
	DWORD							dwAreaID;							//区域ID

	//连接信息
	LPVOID							pBindParameter;						//绑定参数
};

//登录成功
struct DBO_MB_LogonSuccess
{
	//用户属性
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwExperience;						//经验数值
	DWORD							dwLoveLiness;						//用户魅力
	SCORE							lUserScore;							//用户金币
	SCORE							lUserInsure;						//用户银行	.
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户账号
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称

	//描述信息
	TCHAR							szDescribeString[128];				//描述消息

	// 用户区域ID
	DWORD							dwAreaID;							// 用户区域ID
	WORD							wPlatformID;						// 用户系统ID

	BYTE							cbMemberOrder;						//会员等级

	// 用户加入的自定义房间
	WORD							wServerID;
	TCHAR							szSecret[LEN_CUSTOMTABLEPASSWORD];
	bool							bLockedInTable;						// 用户被锁定桌子
	DWORD							dwDiamondCount;						// 钻石数目
	TCHAR							szAgentInfo[LEN_URL];				// 代理微信信息
};


//登录失败
struct DBO_MB_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//手机数据包

//签到命令
#define	DBR_MB_SIGN_IN_CHECK				600							//登录签到

// 签到结果
#define	DBO_MB_SIGN_IN_CHECK_SUCCESS		300							// 签到成功
#define DBO_MB_SIGN_IN_CHECK_FAILURE		301							// 签到失败

struct DBR_MB_SiginInCheck
{
	DWORD							dwUserID;							//用户 I D
};

struct DBO_MB_SignInCheck
{
	DWORD							dwErrorCode;						// 标识码
	DWORD							dwCheckDay;							// 签到的日期
};


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//请求公告
#define DBR_MB_RECHARGE_MESSAGE				700								// 充值公告
#define DBR_MB_CUSTOM_SERVICE_MESSAGE 		701								// 客服公告

#define DBO_MB_RECHARGE_MESSAGE				710								// 数据库回复充值公告
#define DBO_MB_CUSTOM_SERVICE_MESSAGE		711								// 数据库回复客服公告

//////////////////////////////////////////////////////////////////////////////////

#define DBR_MB_SHARE_SUCCESS				702								// 分享成功
#define DBR_MB_USER_JOIN_CUSTOMTABLE		703								// 用户加入自定义桌子
#define DBR_MB_USER_QUERY_CUSTOMTABLE		704								// 查询自己加入的桌子
#define DBR_MB_USER_TRANSFER_DIAMOND		705								// 用户转账钻石
struct DBR_MB_ShareSuccess
{
	DWORD							dwUserID;								// 用户ID
	TCHAR							szMachineID[LEN_MACHINE_ID];			//机器序列

	//连接信息
	DWORD							dwClientAddr;						//连接地址
	LPVOID							pBindParameter;							//绑定参数
};

// 请求数据库加入房间
struct DBR_MB_UserJoinCustomTable
{
	DWORD							dwUserID;								// 发起分享的用户UserID
	TCHAR							szSecret[LEN_CUSTOMTABLEPASSWORD];		// 桌子密码
};

// 请求数据库查询房间信息
struct DBR_MB_UserQueryCustomTable
{
	DWORD							dwUserID;								// 查询的用户ID
};

//转账游戏币
struct DBR_MB_UserTransferDiamond
{
	DWORD							dwUserID;							//发起用户 I D
	DWORD							dwGameID;							//接收用户 I D 
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwTransferDiamond;					//转账钻石
	TCHAR							szPassword[LEN_PASSWORD];			//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

// 自建桌子成功
struct DBO_GR_UserCustomTableSuccess
{
	DWORD							dwUserID;							// 用户ID
	WORD							wKindID;							// 游戏类型
	WORD							wServerID;							// 房间编号
	TCHAR							szSecret[LEN_CUSTOMTABLEPASSWORD];	// 自建桌子密码
	bool							bLockedInTable;						// 被锁定在桌子里
	DWORD							dwUserPropertyCount;				// 玩家道具数目
};

//自建桌子失败
struct DBO_GR_UserCustomTableFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

// 钻石操作成功
struct DBO_MB_UserDiamondSuccess
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwDiamondCount;						//原来的钻石
	TCHAR							szDescribeString[128];				//描述消息
};

// 钻石操作失败
struct DBO_MB_UserDiamondFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

#endif
