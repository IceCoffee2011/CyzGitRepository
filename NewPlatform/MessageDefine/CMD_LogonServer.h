#ifndef CMD_LONGON_HEAD_FILE
#define CMD_LONGON_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//登录命令

#define MDM_GP_LOGON				1									//广场登录

//登录模式
#define SUB_GP_LOGON_GAMEID			1									//I D 登录
#define SUB_GP_LOGON_ACCOUNTS		2									//帐号登录
#define SUB_GP_REGISTER_ACCOUNTS	3									//注册帐号
#define SUB_GP_REGISTER_PROFILE		4									//补充资料	add by chenj
#define SUB_GP_CHECKNICK			5									//检测昵称

//登录结果
#define SUB_GP_LOGON_SUCCESS		100									//登录成功
#define SUB_GP_LOGON_FAILURE		101									//登录失败
#define SUB_GP_LOGON_FINISH			102									//登录完成
#define SUB_GP_VALIDATE_MBCARD      103                                 //登录失败
#define SUB_GP_REGISTER_SUCCESS		105									//注册成功
#define SUB_GP_PROFILE_SUCCESS		106									//补充资料成功
#define SUB_GP_PROFILE_FAILURE		107									//补充资料失败
#define SUB_GP_CHECKNICK_RES		108									//检测昵称

//升级提示
#define SUB_GP_UPDATE_NOTIFY		200									//升级提示

//////////////////////////////////////////////////////////////////////////////////
//

#define MB_VALIDATE_FLAGS           0x01                                //效验密保
#define LOW_VER_VALIDATE_FLAGS      0x02                                //效验低版本

//I D 登录
struct CMD_GP_LogonGameID
{
	//系统信息
	DWORD							dwPlazaVersion;						//广场版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//登录信息
	DWORD							dwGameID;							//游戏 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
	BYTE							cbValidateFlags;			        //校验标识
};

//帐号登录
struct CMD_GP_LogonAccounts
{
	//系统信息
	DWORD							dwPlazaVersion;						//广场版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	BYTE							cbValidateFlags;			        //校验标识
};

//注册帐号
struct CMD_GP_RegisterAccounts
{
	//系统信息
	DWORD							dwPlazaVersion;						//广场版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//密码变量
	TCHAR							szLogonPass[LEN_MD5];				//登录密码
	TCHAR							szInsurePass[LEN_MD5];				//银行密码

	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szSpreader[LEN_ACCOUNTS];			//推荐帐号
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//证件号码
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字
	TCHAR							szMobile[LEN_MOBILE_PHONE];
	BYTE							cbValidateFlags;			        //校验标识
};

//登录成功
struct CMD_GP_LogonSuccess
{
	//属性资料
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwGroupID;							//社团标识
	DWORD							dwCustomID;							//自定标识
	DWORD							dwUserMedal;						//用户奖牌
	DWORD							dwExperience;						//经验数值
	DWORD							dwLoveLiness;						//用户魅力

	//用户成绩
	SCORE							lUserScore;							//用户金币
	SCORE							lUserInsure;						//用户银行

	//用户信息
	BYTE							cbGender;							//用户性别
	BYTE							cbMoorMachine;						//锁定机器
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字

	//配置信息
	BYTE                            cbShowServerStatus;                 //显示服务器状态
	WORD							wFillIn;							//是否补充资料 by chenj 20140421
};

struct CMD_GP_UserProfile
{
	DWORD							dwUserID;							//用户ID
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

struct CMD_GP_CheckNick
{
	DWORD							dwUserID;
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
};

struct CMD_GP_CheckNick_Res
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//登录失败
struct CMD_GP_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//登陆完成
struct CMD_GP_LogonFinish
{
	WORD							wIntermitTime;						//中断时间
	WORD							wOnLineCountTime;					//更新时间
};

//登录失败
struct CMD_GP_ValidateMBCard
{
	UINT								uMBCardID;						//机器序列
};

//升级提示
struct CMD_GP_UpdateNotify
{
	BYTE							cbMustUpdate;						//强行升级
	BYTE							cbAdviceUpdate;						//建议升级
	DWORD							dwCurrentVersion;					//当前版本
};

//////////////////////////////////////////////////////////////////////////////////
//携带信息 CMD_GP_LogonSuccess

#define DTP_GP_GROUP_INFO			1									//社团信息
#define DTP_GP_MEMBER_INFO			2									//会员信息
#define	DTP_GP_UNDER_WRITE			3									//个性签名
#define DTP_GP_STATION_URL			4									//主页信息

//社团信息
struct DTP_GP_GroupInfo
{
	DWORD							dwGroupID;							//社团索引
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字
};

//会员信息
struct DTP_GP_MemberInfo
{
	BYTE							cbMemberOrder;						//会员等级
	SYSTEMTIME						MemberOverDate;						//到期时间
};

//////////////////////////////////////////////////////////////////////////////////
//列表命令

#define MDM_GP_SERVER_LIST			2									//列表信息

//获取命令
#define SUB_GP_GET_LIST				1									//获取列表
#define SUB_GP_GET_SERVER			2									//获取房间
#define SUB_GP_GET_ONLINE			3									//获取在线
#define SUB_GP_GET_COLLECTION		4									//获取收藏

//列表信息
#define SUB_GP_LIST_TYPE			100									//类型列表
#define SUB_GP_LIST_KIND			101									//种类列表
#define SUB_GP_LIST_NODE			102									//节点列表
#define SUB_GP_LIST_PAGE			103									//定制列表
#define SUB_GP_LIST_SERVER			104									//房间列表
#define SUB_GP_VIDEO_OPTION			105									//视频配置

//完成信息
#define SUB_GP_LIST_FINISH			200									//发送完成
#define SUB_GP_SERVER_FINISH		201									//房间完成

//在线信息
#define SUB_GR_KINE_ONLINE			300									//类型在线
#define SUB_GR_SERVER_ONLINE		301									//房间在线

//////////////////////////////////////////////////////////////////////////////////

//获取在线
struct CMD_GP_GetOnline
{
	WORD							wServerCount;						//房间数目
	WORD							wOnLineServerID[MAX_SERVER];		//房间标识
};

//类型在线
struct CMD_GP_KindOnline
{
	WORD							wKindCount;							//类型数目
	tagOnLineInfoKind				OnLineInfoKind[MAX_KIND];			//类型在线
};

//房间在线
struct CMD_GP_ServerOnline
{
	WORD							wServerCount;						//房间数目
	tagOnLineInfoServer				OnLineInfoServer[MAX_SERVER];		//房间在线
};

//////////////////////////////////////////////////////////////////////////////////
//服务命令

#define MDM_GP_USER_SERVICE			3									//用户服务

//账号服务
#define SUB_GP_MODIFY_MACHINE		100									//修改机器
#define SUB_GP_MODIFY_LOGON_PASS	101									//修改密码
#define SUB_GP_MODIFY_INSURE_PASS	102									//修改密码
#define SUB_GP_MODIFY_UNDER_WRITE	103									//修改签名

//修改头像
#define SUB_GP_USER_FACE_INFO		200									//头像信息
#define SUB_GP_SYSTEM_FACE_INFO		201									//系统头像
#define SUB_GP_CUSTOM_FACE_INFO		202									//自定头像

//个人资料
#define SUB_GP_USER_INDIVIDUAL				301							//个人资料
#define	SUB_GP_QUERY_INDIVIDUAL				302							//查询信息
#define SUB_GP_MODIFY_INDIVIDUAL			303							//修改资料
#define SUB_GP_QUERY_NICKNAME_BY_GAMEID		304							// 通过GameID查询昵称
#define SUB_GP_QUERY_EXCHANGE_LOVELINESS	305							// 查询用户的可转魅力信息
#define SUB_GP_EXCHANGE_LOVELINESS_INFO		306							// 返回克兑魅力信息
#define SUB_GP_EXCHANGE_LOVELINESS			307							// 请求兑换魅力操作	
#define SUB_GP_EXCHANGE_LOVELINESS_RESULT	308							// 兑换魅力结果

//银行服务
#define SUB_GP_USER_SAVE_SCORE				400								//存款操作
#define SUB_GP_USER_TAKE_SCORE				401								//取款操作
#define SUB_GP_USER_TRANSFER_SCORE			402								//转账操作
#define SUB_GP_USER_INSURE_INFO				403								//银行资料
#define SUB_GP_QUERY_INSURE_INFO			404								//查询银行
#define SUB_GP_USER_INSURE_SUCCESS			405								//银行成功
#define SUB_GP_USER_INSURE_FAILURE			406								//银行失败
#define SUB_GP_QUERY_USER_INFO_REQUEST		407								//查询用户
#define SUB_GP_QUERY_USER_INFO_RESULT		408								//用户信息
#define SUB_GP_QUERY_TRANSRECORD			409                             //转账记录
#define SUB_GP_TRANS_RECORD					410
#define SUB_GP_VERIFY_INSURE_PASSWORD		411								// 验证银行密码
#define SUB_GP_USER_TRANSFER_RECEIPT		412								// 银行转账回执	
#define SUB_MB_USER_CUSTOM_TABLE			600								// 用户创建自定义桌子
#define SUB_MB_USER_JOIN_CUSTOMTABLE		601								// 用户进入自定义桌子
#define SUB_MB_USER_QUERY_CUSTOMTABLE		602								// 用户查询自己在自定义桌子房间的信息	
#define SUB_MB_USER_TRANSFER_DIAMOND		603								// 用户转账钻石

#define SUB_MB_SHARE_SUCCESS			500								// app分享成功,领豆

//操作结果
#define SUB_GP_OPERATE_SUCCESS			900								//操作成功
#define SUB_GP_OPERATE_FAILURE			901								//操作失败

#define SUB_MB_USER_CUSTOM_TABLE_SUCCESS	600							// 自建桌子成功
#define SUB_MB_USER_CUSTOM_TABLE_FAILED		601							// 自建桌子失败
#define SUB_MB_USER_DIAMOND_SUCCESS			602							// 钻石操作失败
#define SUB_MB_USER_DIAMOND_FAILURE			603							// 钻石操作失败

//////////////////////////////////////////////////////////////////////////////////

//修改密码
struct CMD_GP_ModifyLogonPass
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改密码
struct CMD_GP_ModifyInsurePass
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改签名
struct CMD_GP_ModifyUnderWrite
{
	DWORD							dwUserID;							//用户 I D
//	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
};

//////////////////////////////////////////////////////////////////////////////////

//用户头像
struct CMD_GP_UserFaceInfo
{
	WORD							wFaceID;							//头像标识
	DWORD							dwCustomID;							//自定标识
};

//修改头像
struct CMD_GP_SystemFaceInfo
{
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改头像
struct CMD_GP_CustomFaceInfo
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//图片信息
};

//////////////////////////////////////////////////////////////////////////////////

//绑定机器
struct CMD_GP_ModifyMachine
{
	BYTE							cbBind;								//绑定标志
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};


//////////////////////////////////////////////////////////////////////////////////

//个人资料
struct CMD_GP_UserIndividual
{
	DWORD							dwUserID;							//用户 I D
};

//查询信息
struct CMD_GP_QueryIndividual
{
	DWORD							dwUserID;							//用户 I D
};

//修改资料
struct CMD_GP_ModifyIndividual
{
	BYTE							cbGender;							//用户性别
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
};

// 查询昵称
struct CMD_GP_QueryNickNameByGameID
{
	DWORD							dwGameID;							// 用户GameID
};

// 查询魅力转换信息
struct CMD_GP_QueryExchangeLoveliness
{
	DWORD							dwUserID;							//用户标识
};

// 回复查询到的魅力转换信息
struct CMD_GP_ExchangeLovelinessInfo
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwLoveliness;						// 用户魅力
	DWORD							dwAvailbleAmount;					// 可兑换魅力
	DWORD							dwFinalAmount;						// 实际能得到的货币(可兑换魅力 * 兑换率)
	TCHAR							szErr[128];							// 错误提示
	TCHAR							szTips[1024];						// 兑换提示
};

// 发起魅力兑换
struct CMD_GP_ExchangeLoveliness
{
	DWORD							dwUserID;							//用户标识
};

struct CMD_GP_ExchangeLovelinessResult
{
	DWORD							dwUserID;							// 用户I D
	DWORD							dwLoveliness;						// 兑换后魅力
	SCORE							lScore;								// 兑换后游戏货币
	SCORE							lInsureScore;						// 兑换后银行货币
	LONG							lResultCode;						// 操作结果
	TCHAR							szDescribeString[128];				// 提示消息
};

//////////////////////////////////////////////////////////////////////////////////
//携带信息 CMD_GP_UserIndividual

#define DTP_GP_UI_NICKNAME			1									//用户昵称
#define DTP_GP_UI_USER_NOTE			2									//用户说明
#define DTP_GP_UI_UNDER_WRITE		3									//个性签名
#define DTP_GP_UI_QQ				4									//Q Q 号码
#define DTP_GP_UI_EMAIL				5									//电子邮件
#define DTP_GP_UI_SEAT_PHONE		6									//固定电话
#define DTP_GP_UI_MOBILE_PHONE		7									//移动电话
#define DTP_GP_UI_COMPELLATION		8									//真实名字
#define DTP_GP_UI_DWELLING_PLACE	9									//联系地址

//////////////////////////////////////////////////////////////////////////////////

//银行资料
struct CMD_GP_UserInsureInfo
{
	WORD							wRevenueTake;						//税收比例
	WORD							wRevenueTransfer;					//税收比例
	WORD							wServerID;							//房间标识
	SCORE							lUserScore;							//用户金币
	SCORE							lUserInsure;						//银行金币
	SCORE							lTransferPrerequisite;				//转账条件
	DWORD							dwDiamondCount;						// 钻石道具数量
};
//转账记录
struct CMD_GP_RealTransRecord
{
    DWORD                               dwGameID;
    TCHAR                               szSourceAccount[LEN_ACCOUNTS];
    TCHAR                               szTargetAccounts[LEN_ACCOUNTS];
    LONGLONG                            lSwapScore;
    LONGLONG                            lRevenue;
    TCHAR                               szTime[LEN_ACCOUNTS];
	DWORD								dwSourceID;
	DWORD								dwTargetID;
    bool                                bOver;
};

//存入金币
struct CMD_GP_UserSaveScore
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lSaveScore;							//存入金币
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//提取金币
struct CMD_GP_UserTakeScore
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lTakeScore;							//提取金币
	TCHAR							szPassword[LEN_MD5];				//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//转账金币
struct CMD_GP_UserTransferScore
{
	DWORD							dwUserID;							//用户 I D
	BYTE                            cbByNickName;                       //昵称赠送
	SCORE							lTransferScore;						//转账金币
	TCHAR							szPassword[LEN_MD5];				//银行密码
	TCHAR							szNickName[LEN_NICKNAME];			//目标用户
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

// 银行转账回执
struct CMD_GR_UserTransferReceipt
{
	DWORD							dwRecordID;							// 回执编号
	DWORD							dwSourceUserID;						// 发起转账的玩家
	DWORD							dwTargetUserID;						// 接受转账的玩家
	DWORD							dwSourceGameID;						// 发起转账的GameID
	DWORD							dwTargetGameID;						// 接受转账的GameID

	TCHAR							szSourceNickName[LEN_NICKNAME];		// 发起转账玩家的昵称
	TCHAR							szTargetNickName[LEN_NICKNAME];		// 接受转账玩家的昵称

	SCORE							lTransferScore;						// 转账金额
	SCORE							lRevenueScore;						// 税收金额

	SCORE							lSourceUserScore;					// 转账后自己的货币
	SCORE							lSourceUserInsure;					// 转账后自己的银行货币

	SCORE							lReceviceScore;						// 转账后对方实收多少货币

	SYSTEMTIME						stTransferTime;						// 转账时间

	DWORD							dwLoveliness;						// 转账后魅力值
};

//银行成功
struct CMD_GP_UserInsureSuccess
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lUserScore;							//用户金币
	SCORE							lUserInsure;						//银行金币
	TCHAR							szDescribeString[128];				//描述消息
};

//银行失败
struct CMD_GP_UserInsureFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//提取结果
struct CMD_GP_UserTakeResult
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lUserScore;							//用户金币
	SCORE							lUserInsure;						//银行金币
};

//查询银行
struct CMD_GP_QueryInsureInfo
{
	DWORD							dwUserID;							//用户 I D
};

//查询用户
struct CMD_GP_QueryUserInfoRequest
{
	BYTE                            cbByNickName;                       //昵称赠送
	TCHAR							szNickName[LEN_NICKNAME];			//目标用户
};


//用户信息
struct CMD_GP_UserTransferUserInfo
{
	DWORD							dwTargetGameID;						//目标用户
	TCHAR							szNickName[LEN_NICKNAME];			//目标用户
};

// 验证用户密码信息
struct CMD_GP_VerifyUserInsurePassword
{
	DWORD							dwUserID;							// 用户I D
	TCHAR							szInsurePassword[LEN_PASSWORD];		// 用户银行密码
};

// app分享成功
struct CMD_MB_ShareSuccess
{
	DWORD							dwUserID;							// 发起分享的用户UserID
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

// 用户创建桌子
struct CMD_MB_UserCustomTable
{
	DWORD							dwUserID;							// 发起分享的用户UserID
	WORD							wKindID;							// 类型索引
	WORD							wCardCousumeCount;					// 消耗的房卡数目
	BYTE							cbCustomRule[LEN_CUSTOMRULE];		// 自定规则
};

// 用户加入自定义桌子
struct CMD_MB_UserJoinCustomTable
{
	DWORD							dwUserID;								// 发起分享的用户UserID
	TCHAR							szSecret[LEN_CUSTOMTABLEPASSWORD];		// 桌子密码
};

// 用户查询加入的桌子信息
struct CMD_MB_UserQueryCustomTable
{
	DWORD							dwUserID;								// 发起分享的用户UserID
};

struct CMD_MB_UserTransferDiamond
{
	DWORD							dwUserID;							//转账用户的I D
	DWORD							dwGameID;							//接收用户的I D
	DWORD							dwTransferDiamond;					//转账钻石
	TCHAR							szPassword[LEN_MD5];				//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//////////////////////////////////////////////////////////////////////////////////

//操作失败
struct CMD_GP_OperateFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//操作成功
struct CMD_GP_OperateSuccess
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//成功消息
};

//////////////////////////////////////////////////////////////////////////////////

// 用户自建桌子成功
struct CMD_MB_UserCustomTableSuccess
{
	WORD							wServerID;							// 房间ID
	TCHAR							szSecret[LEN_CUSTOMTABLEPASSWORD];	// 密码
	bool							bLockedInTable;						// 是否被锁定在桌子上,已经锁定的话,只能返回房间
	DWORD							dwDiamondCount;						// 剩余道具数量
};

// 用户自建桌子失败
struct CMD_MB_UserCustomTableFailed
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

// 用户钻石成功
struct CMD_MB_UserDiamondSuccess
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwDiamondCount;						//原来的钻石
	TCHAR							szDescribeString[128];				//描述消息
};

// 用户钻石失败
struct CMD_MB_UserDiamondFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//////////////////////////////////////////////////////////////////////////////////
//远程服务

#define MDM_GP_REMOTE_SERVICE		4									//远程服务

//查找服务
#define SUB_GP_C_SEARCH_DATABASE	100									//数据查找
#define SUB_GP_C_SEARCH_CORRESPOND	101									//协调查找

//查找服务
#define SUB_GP_S_SEARCH_DATABASE	200									//数据查找
#define SUB_GP_S_SEARCH_CORRESPOND	201									//协调查找

//////////////////////////////////////////////////////////////////////////////////

//协调查找
struct CMD_GP_C_SearchCorrespond
{
	DWORD							dwGameID;							//游戏标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
};

//协调查找
struct CMD_GP_S_SearchCorrespond
{
	WORD							wUserCount;							//用户数目
	tagUserRemoteInfo				UserRemoteInfo[16];					//用户信息
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//登录命令
#define MDM_MB_LOGON				100									//广场登录

//登录模式
#define SUB_MB_LOGON_GAMEID			1									//I D 登录
#define SUB_MB_LOGON_ACCOUNTS		2									//帐号登录
#define SUB_MB_REGISTER_ACCOUNTS	3									//注册帐号
#define SUB_MB_SIGN_IN_CHECK		4									//签到
#define SUB_LOGON_THIRDPARTY		5									// 第三方登录

//登录结果
#define SUB_MB_LOGON_SUCCESS		100									//登录成功
#define SUB_MB_LOGON_FAILURE		101									//登录失败

//升级提示
#define SUB_MB_UPDATE_NOTIFY		200									//升级提示

//////////////////////////////////////////////////////////////////////////////////

//I D 登录
struct CMD_MB_LogonGameID
{
	//系统信息
	WORD							wModuleID;							//模块标识
	DWORD							dwPlazaVersion;						//广场版本
	BYTE                            cbDeviceType;                       //设备类型

	//登录信息
	DWORD							dwGameID;							//游戏 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
};

//帐号登录
struct CMD_MB_LogonAccounts
{
	//系统信息
	WORD							wModuleID;							//模块标识
	DWORD							dwPlazaVersion;						//广场版本
	BYTE                            cbDeviceType;                       //设备类型

	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
};

//账号登录(第三方)
struct CMD_LogonAccounts_ThirdParty
{
	//系统信息
	DWORD							dwPlazaVersion;						//广场版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	
	BYTE                            cbDeviceType;                       // 设备类型
	unsigned int					nThirdPartyID;						// 第三方ID
	TCHAR							szSessionID[LEN_SESSION_ID];		// SessionID
	TCHAR							szNickName[LEN_NICKNAME];			// 昵称

	// 区域ID
	DWORD							dwAreaID;							//区域ID
};

//注册帐号
struct CMD_MB_RegisterAccounts
{
	//系统信息
	WORD							wModuleID;							//模块标识
	DWORD							dwPlazaVersion;						//广场版本
	BYTE                            cbDeviceType;                       //设备类型

	//密码变量
	TCHAR							szLogonPass[LEN_MD5];				//登录密码
	TCHAR							szInsurePass[LEN_MD5];				//银行密码

	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码

	// 区域ID
	DWORD							dwAreaID;							//区域ID
};

//登录成功
struct CMD_MB_LogonSuccess
{
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwExperience;						//经验数值
	DWORD							dwLoveLiness;						//用户魅力

	//用户成绩
	SCORE							lUserScore;							//用户金币
	SCORE							lUserInsure;						//用户银行	

	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户账号
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称

	// 用户道具钻石
	DWORD							dwDiamondCount;						// 开房钻石数量
};

//登录失败
struct CMD_MB_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//升级提示
struct CMD_MB_UpdateNotify
{
	BYTE							cbMustUpdate;						//强行升级
	BYTE							cbAdviceUpdate;						//建议升级
	DWORD							dwCurrentVersion;					//当前版本
};

// 签到
struct CMD_MB_SignInCheck
{
	DWORD							dwErrorCode;						// 标识码
	DWORD							dwCheckDay;							// 签到的日期
};

//////////////////////////////////////////////////////////////////////////////////
//携带信息 CMD_MB_LogonSuccess

#define DTP_MB_GROUP_INFO			1									//社团信息
#define DTP_MB_MEMBER_INFO			2									//会员信息
#define	DTP_MB_UNDER_WRITE			3									//个性签名
#define DTP_MB_STATION_URL			4									//主页信息
#define DTP_MB_AGENT_INFO			5									// 钻石代理信息

//会员信息
struct DTP_MB_MemberInfo
{
	BYTE							cbMemberOrder;						//会员等级
};

struct DTP_MB_AgentInfo
{
	TCHAR							szAgentInfo[LEN_URL];				//钻石代理信息
};

//////////////////////////////////////////////////////////////////////////////////
//列表命令

#define MDM_MB_SERVER_LIST			101									//列表信息

//列表信息
#define SUB_MB_LIST_TYPE			100									//类型列表
#define SUB_MB_LIST_KIND			101									//种类列表
#define SUB_MB_LIST_SERVER			102									//房间列表
#define SUB_MB_LIST_KIND_EX			103									//房间扩展信息
#define SUB_MB_LIST_PROP_CONFIG		104									// 自建桌子道具消耗情况
#define SUB_MB_LIST_FINISH			200									//列表完成

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//常用显示信息
#define MDM_MB_SYSTEM_MESSAGE			102								// 官方公告

#define SUB_MB_RECHARGE_MESSAGE			100								// 充值公告
#define SUB_MB_CUSTOM_SERVICE_MESSAGE 	101								// 客服公告	

struct CMD_MB_SystemMessage
{
	WORD							wLength;							//消息长度
	TCHAR							szString[1024];						//消息内容
};

#pragma pack()

#endif