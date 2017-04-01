
----------------------------------------------------------------------------------------------------

USE QPUserCustomDB
GO

-- 用户创建桌子
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserCustomCreateTable]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserCustomCreateTable]
GO

-- 用户加入桌子(登录服务器)
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LS_UserJoinCustomTable]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LS_UserJoinCustomTable]
GO

-- 用户加入桌子(游戏服务器)
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_UserJoinCustomTable]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_UserJoinCustomTable]
GO

-- 查询玩家加入的桌子信息
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryUserJoinCustomTable]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryUserJoinCustomTable]
GO

-- 加载玩家创建的桌子信息
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadUserCustomTableInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadUserCustomTableInfo]
GO

-- 开始游戏
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserStartCustomGame]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserStartCustomGame]
GO

-- 桌子结算
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ConcludeTable]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ConcludeTable]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 创建桌子
CREATE PROC GSP_GR_UserCustomCreateTable
	@dwUserID INT,	
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@wTableID INT,								-- 桌子 I D
	@wConsumePropertyCount INT,					-- 消耗道具数
	@strCustomRule NVARCHAR(2048),				-- 定制规则
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 辅助变量
DECLARE @EnjoinLogon AS INT
DECLARE @RoundCount AS INT						-- 游戏局数

-- 执行逻辑
BEGIN

	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinCreateCustomTable'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM QPAccountsDB.dbo.SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinCreateCustomTable'
		RETURN 2
	END

	-- 效验地址
	--SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	--IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	--BEGIN
	--	SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的登录功能，请联系客户服务中心了解详细情况！'
	--	RETURN 4
	--END
	
	-- 效验机器
	--SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	--IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	--BEGIN
	--	SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的登录功能，请联系客户服务中心了解详细情况！'
	--	RETURN 7
	--END
	
	-- 查询用户
	--DECLARE @dwUserID INT
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	--DECLARE @LogonPass AS NCHAR(32)
	DECLARE @MachineSerial NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE @dwAreaID AS INT
	
	SELECT @dwUserID=UserID, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MachineSerial=LastLogonMachine, @dwAreaID=AreaID
	FROM QPAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 查询用户
	IF @dwUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 2
	END	

	-- 帐号关闭
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！'
		RETURN 2
	END	
	
	-- 固定机器
	--IF @MoorMachine=1
	--BEGIN
	--	IF @MachineSerial<>@strMachineID
	--	BEGIN
	--		SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
	--		RETURN 1
	--	END
	--END.
	
	-- 检查用户状态是否符合情况(是否已经创建了房间之类的)
	IF EXISTS (SELECT * FROM UserCreateTable(NOLOCK) WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe=N'您已经创建过房间,不能再次创建房间.赶快进入房间,邀请朋友一起游戏吧!'
		RETURN 6
	END
	
	-- 判断是否已经参加了其它人创建的房间
	IF EXISTS (SELECT * FROM UserInTable(NOLOCK) WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe=N'您已经加入过其它房间,不能再次创建房间.赶快进入房间和朋友一起游戏吧!'
		RETURN 6
	END
	
	-- 检查房间对应的桌号是否被创建了
	IF EXISTS (SELECT * FROM UserInTable(NOLOCK) WHERE ServerID=@wServerID AND TableID=@wTableID)
	BEGIN
		SET @strErrorDescribe=N'要创建的房间被占用,不能创建房间,请稍后再试!'
		RETURN 6
	END

	
	-- 检查道具是否符合情况
	-- 变量定义
	DECLARE @UserPropertyCount AS INT

	-- 道具判断
	SELECT @UserPropertyCount=PropertyCount FROM UserProperty(NOLOCK) WHERE UserID=@dwUserID AND PropertyID=27

	-- 存在判断
	IF @UserPropertyCount IS NULL
	BEGIN
		-- 给玩家赠送钻石
		DECLARE @GrantDiamondCount AS INT
		SELECT @GrantDiamondCount=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'GrantDiamondCount' AND AreaID=@dwAreaID
		SET @UserPropertyCount = @GrantDiamondCount
		IF @UserPropertyCount IS NULL
		BEGIN
			SET @UserPropertyCount = 10
		END
		INSERT QPUserCustomDB.dbo.UserProperty(UserID, PropertyCount) VALUES(@dwUserID,@UserPropertyCount)
	END
	
	IF @UserPropertyCount < @wConsumePropertyCount
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您,您的房卡数量不足,不能创建房间!'
		RETURN 4
	END

	
	-- 判断可以创建多少局
	SELECT @RoundCount = RoundCount  FROM GameKindProperty(NOLOCK) WHERE KindID=@wKindID AND PropertyConsumeCount = @wConsumePropertyCount
	IF @RoundCount IS NULL
	BEGIN
		SET @strErrorDescribe=N'您创建的游戏局数和配置不符合!如有疑问请联系客服'
		RETURN 5
	END
	
	-- 创建房间,生成随机密码
	DECLARE @nSuccess INT
	DECLARE @strVerifyCode		NVARCHAR(8)			-- 查询到的验证码
	SET @nSuccess = 0	
	WHILE @nSuccess = 0
	BEGIN
		-- 重置验证码
		SET @strVerifyCode = N''
		
		-- 随机拼接生成
		DECLARE @nIndex INT
		SET @nIndex = 0
		WHILE @nIndex < 6
		BEGIN
			SET @strVerifyCode = @strVerifyCode + CAST(CAST(RAND()*10 As INT) As NVARCHAR)
			SET @nIndex = @nIndex + 1
		END
		
		-- 检查生成验证码是否合法 长度为6位，并且没有重复
		IF LEN(@strVerifyCode) = 6
		BEGIN
			IF NOT EXISTS (SELECT * FROM UserCreateTable WHERE Secret=@strVerifyCode)
			BEGIN
				-- 
				SET @nSuccess = 1
			END
		END
	END
	
	-- 扣除道具
	UPDATE UserProperty SET PropertyCount = PropertyCount - @wConsumePropertyCount WHERE UserID=@dwUserID
	
	-- 记录道具日志
	INSERT RecordUserProperty(SourceUserID, SourceDiamond, OperateType, PropertyCount, CollectNote)
	VALUES (@dwUserID, @UserPropertyCount, 1, @wConsumePropertyCount, N'创建房间')
	
	SET @UserPropertyCount = @UserPropertyCount - @wConsumePropertyCount
	
	-- 插入记录
	INSERT INTO UserCreateTable(UserID,KindID,ServerID,TableID,Secret,CustomRule,RoundCount)
	VALUES(@dwUserID,@wKindID,@wServerID,@wTableID,@strVerifyCode,@strCustomRule,@RoundCount)
	
	-- 获取结果
	SELECT @dwUserID AS CreateUserID, @wKindID AS KindID, @wServerID AS ServerID, @strVerifyCode AS Secret, @wTableID AS TableID, @RoundCount As RoundCount, getdate() AS CreateDateTime,
	 @UserPropertyCount AS UserPropertyCount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
CREATE PROC GSP_LS_UserJoinCustomTable
	@dwUserID INT,	
	@strSecret NVARCHAR(9),						-- 定制规则
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @wServerID AS INT						-- 密码对应的房间 I D
DECLARE @wTableID AS INT						-- 密码对应的桌子 I D

DECLARE @wUserServerID AS INT					-- 玩家对应的房间 I D
DECLARE @wUserTableID AS INT					-- 玩家对应的房间 I D

DECLARE	@wLockedInTable AS TINYINT				-- 用户是否被锁定在桌子上

DECLARE @dwAreaID AS INT

-- 执行逻辑
BEGIN

	-- 查询用户是否存在
	SELECT @dwAreaID=AreaID FROM QPAccountsDB.dbo.AccountsInfo where UserID=@dwUserID
	IF @dwAreaID IS NULL
	BEGIN
		SET @strErrorDescribe=N'用户不存在!'
		RETURN 1
	END
	
	SELECT @wServerID = ServerID, @wTableID = TableID FROM UserCreateTable WHERE Secret=@strSecret
	-- 检查密码是否存在
	IF @wServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'房间不存在!'
		RETURN 1
	END
	
	SET @wLockedInTable = 0
	-- 检查当前玩家是否是创建者,创建者不能加入其它房间
	SELECT @wUserServerID = ServerID, @wUserTableID = TableID FROM UserCreateTable WHERE UserID=@dwUserID
	IF @wUserServerID IS NOT NULL AND @wUserTableID IS NOT NULL AND @wServerID != @wUserServerID AND @wTableID != @wUserTableID
	BEGIN 
		SET @strErrorDescribe=N'您已经创建过房间,不能加入其它房间.赶快返回房间和朋友一起游戏吧!'
		RETURN 100
	END
	
	
	-- 检查密码对应的房间是否开始过游戏,并且这个玩家没有在开始游戏的玩家里
	IF EXISTS (SELECT * FROM UserInTable(NOLOCK) WHERE ServerID=@wServerID AND TableID=@wTableID)
	BEGIN
		-- 桌子已开始过游戏
		IF NOT EXISTS (SELECT * FROM UserInTable(NOLOCK) WHERE ServerID=@wServerID AND TableID=@wTableID AND UserID=@dwUserID)
		BEGIN
			-- 并且自己没在桌子里
			SET @strErrorDescribe=N'当前房间已开始游戏,不能加入!'
			RETURN 1
		END
	END
	
	-- 检测玩家是否加入过其它桌号的游戏了
	SET @wLockedInTable = 0
	SELECT @wUserServerID=ServerID, @wUserTableID=TableID FROM UserInTable(NOLOCK) WHERE UserID=@dwUserID
	IF @wUserServerID IS NOT NULL AND @wUserTableID IS NOT NULL
	BEGIN
		SET @wLockedInTable = 1
		IF @wUserServerID != @wServerID OR @wUserTableID != @wTableID
		BEGIN
			SET @strErrorDescribe=N'您已经加入过房间,不能再次加入其它房间.赶快返回房间和朋友一起游戏吧!'
			RETURN 100
		END
	END
	
	-- 检查玩家钻石
	DECLARE @UserPropertyCount AS INT					-- 用户的钻石数量
	SELECT @UserPropertyCount=PropertyCount FROM UserProperty(NOLOCK) WHERE UserID=@dwUserID AND PropertyID=27

	-- 存在判断
	IF @UserPropertyCount IS NULL
	BEGIN
		DECLARE @GrantDiamondCount AS INT
		SELECT @GrantDiamondCount=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'GrantDiamondCount' AND @dwAreaID=AreaID
		IF @GrantDiamondCount IS NULL
		BEGIN
			SET @UserPropertyCount = 10
		END
		INSERT QPUserCustomDB.dbo.UserProperty(UserID, PropertyCount) VALUES(@dwUserID,@UserPropertyCount)
	END

	SELECT @dwUserID As UserID,	@strSecret As Secret, @wServerID As ServerID, @wLockedInTable AS LockedInTable, @UserPropertyCount As PropertyCount
END

RETURN 0

GO

-- 加入桌子
CREATE PROC GSP_GS_UserJoinCustomTable
	@dwUserID INT,	
	@strSecret NVARCHAR(9),						-- 定制规则
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 辅助变量
DECLARE @wServerID AS INT						-- 密码对应的房间 I D
DECLARE @wTableID AS INT						-- 密码对应的桌子 I D

DECLARE @wUserServerID AS INT					-- 玩家已经开始过游戏的房间 I D
DECLARE @wUserTableID AS INT					-- 玩家已经开始过游戏的桌号 I D
DECLARE @wUserChairID AS INT					-- 玩家已经开始过游戏的椅子 I D

-- 执行逻辑
BEGIN

	SELECT @wServerID = ServerID, @wTableID = TableID FROM UserCreateTable WHERE Secret=@strSecret
	-- 检查密码是否存在
	IF @wServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'房间不存在!'
		RETURN 1
	END
	
	-- 检测玩家是否加入过其它桌号的游戏了
	SELECT @wUserServerID=ServerID, @wUserTableID=TableID, @wUserChairID = ChairID FROM UserInTable(NOLOCK) WHERE UserID=@dwUserID
	IF @wUserChairID IS NOT NULL
	BEGIN
		IF @wUserServerID != @wServerID OR @wUserTableID != @wTableID
		BEGIN
			SET @strErrorDescribe=N'您已经加入过其它房间,不能再次加入房间.赶快返回房间和朋友一起游戏吧!'
			RETURN 100
		END
	END
	ELSE
	BEGIN
		SET @wUserTableID = @wTableID
		SET @wUserChairID = 65535
	END
	
	-- 传出结果
	SELECT @wUserTableID AS TableID, @wUserChairID AS ChairID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询已经加入的桌子信息
CREATE PROC GSP_GR_QueryUserJoinCustomTable
	@dwUserID INT,	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 辅助变量
DECLARE @dwAreaID AS INT						-- 用户区域ID
DECLARE @wServerID AS INT						-- 房间 I D
DECLARE @wTableID AS INT						-- 房间 I D
DECLARE @strSecret AS NVARCHAR(9)				-- 密码
DECLARE @wLockedInTable TINYINT					-- 是否房间锁定

-- 执行逻辑
BEGIN
	SET @wLockedInTable = 0
	
	IF EXISTS (SELECT * FROM QPUserCustomDB.dbo.UserCreateTable WHERE UserID=@dwUserID)
	BEGIN
		SELECT @wServerID=ServerID, @strSecret=Secret FROM QPUserCustomDB.dbo.UserCreateTable WHERE UserID=@dwUserID
		SET @wLockedInTable = 1
	END
	ELSE IF EXISTS (SELECT * FROM QPUserCustomDB.dbo.UserInTable WHERE UserID=@dwUserID)
	BEGIN
		SELECT @wServerID=ServerID, @wTableID=TableID FROM QPUserCustomDB.dbo.UserInTable WHERE UserID=@dwUserID
		SELECT @strSecret=Secret FROM QPUserCustomDB.dbo.UserCreateTable WHERE ServerID=@wServerID AND TableID=@wTableID
		SET @wLockedInTable = 1
	END
	ELSE
	BEGIN
		SET @strErrorDescribe=N'您加入的房间已经解散,请选择其他房间加入!'
		RETURN 1
	END
	
	-- 检查玩家钻石
	DECLARE @dwUserPropertyCount AS INT					-- 用户的钻石数量
	SELECT @dwUserPropertyCount=PropertyCount FROM UserProperty(NOLOCK) WHERE UserID=@dwUserID AND PropertyID=27

	-- 存在判断
	IF @dwUserPropertyCount IS NULL
	BEGIN
		DECLARE @GrantDiamondCount AS INT
		SELECT @GrantDiamondCount=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'GrantDiamondCount' AND @dwAreaID=AreaID
		SET @dwUserPropertyCount = @GrantDiamondCount
		IF @dwUserPropertyCount IS NULL
		BEGIN
			SET @dwUserPropertyCount = 0
		END
		INSERT QPUserCustomDB.dbo.UserProperty(UserID, PropertyCount) VALUES(@dwUserID,@dwUserPropertyCount)
	END
	
	-- 传出结果
	SELECT @dwUserID As UserID,	@strSecret As Secret, @wServerID As ServerID, @wLockedInTable As LockedInTable, @dwUserPropertyCount As PropertyCount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载玩家创建的桌子信息
CREATE PROC GSP_GR_LoadUserCustomTableInfo
	@wServerID INT,	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON


DECLARE @dwUserID AS INT						-- 创建用户的UserID
DECLARE @wTableID AS INT						-- 创建用户的UserID
DECLARE @strSecret AS NVARCHAR(9)				-- 密码
DECLARE	@strCustomRule NVARCHAR(2048)			-- 定制规则
DECLARE @dwRoundCound AS INT					-- 游戏局数
DECLARE @CreateDateTime DATETIME
DECLARE @PlayRoundCount AS INT					-- 游戏局数

-- 执行逻辑
BEGIN

	SELECT A.UserID  AS UserID, A.TableID AS TableID, A.Secret AS Secret, A.CustomRule AS CustomRule, A.RoundCount AS RoundCount,
		A.CreateDateTime AS CreateDateTime, B.RoundCount AS PlayRoundCount
	FROM UserCreateTable A LEFT JOIN UserInTable B ON B.ServerID=A.ServerID AND B.TableID=A.TableID AND B.UserID = A.UserID WHERE A.ServerID = @wServerID
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 开始游戏
CREATE PROC GSP_GR_UserStartCustomGame
	@wServerID INT,	
	@dwUserID INT,
	@wTableID INT,
	@wChairID INT,
	@dwRoundCount INT,
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	
	IF EXISTS (SELECT * FROM UserInTable(NOLOCK) WHERE UserID=@dwUserID AND (ServerID != @wServerID OR TableID != @wTableID ) ) 
	BEGIN
		-- 已经在游戏中,不能再次开始游戏
		SET @strErrorDescribe=N'已经加入在游戏中,不能再次开始游戏！'
		RETURN 1
	END
	
	UPDATE UserInTable SET ServerID=@wServerID, TableID=@wTableID, ChairID=@wChairID, RoundCount=@dwRoundCount  WHERE UserID = @dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		IF EXISTS (SELECT * FROM UserCreateTable WHERE UserID = @dwUserID)
		BEGIN
			INSERT UserInTable(UserID, ServerID, TableID, ChairID, RoundCount, TableBuilder) VALUES(@dwUserID, @wServerID, @wTableID, @wChairID, @dwRoundCount, 1)
		END
		ELSE
		BEGIN
			INSERT UserInTable(UserID, ServerID, TableID, ChairID, RoundCount, TableBuilder) VALUES(@dwUserID, @wServerID, @wTableID, @wChairID, @dwRoundCount, 0)
		END
	END

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 开始游戏
CREATE PROC GSP_GR_ConcludeTable
	@wServerID INT,								-- 结算的服务器ID
	@wTableID INT,								-- 结算的桌子ID
	@wConludeType INT,							-- 结算类型
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @dwUserID AS INT						-- 创建用户的UserID
DECLARE @dwKindID AS INT
DECLARE @dwRoundCount AS INT					-- 游戏局数
DECLARE @wConsumePropertyCount AS INT			-- 消耗道具数
DECLARE @CreateDateTime AS DATETIME				-- 创建时间
DECLARE @strSecret AS NVARCHAR(9)				-- 桌子密码
DECLARE @strCustomRule AS NVARCHAR(2048)		-- 自定义规则
DECLARE @dwCreaterPropertyCount AS INT				-- 玩家道具数量

-- 执行逻辑
BEGIN

	SELECT @dwUserID=UserID, @dwKindID=KindID, @dwRoundCount=RoundCount, @CreateDateTime=CreateDateTime, @strSecret=Secret, @strCustomRule=CustomRule, @dwRoundCount=RoundCount
	FROM UserCreateTable WHERE ServerID=@wServerID AND TableID=@wTableID
	
	IF @dwUserID IS NULL
	BEGIN
		-- 错误信息
		SET @strErrorDescribe=N'没有可解散的桌子！'
		RETURN 1
	END

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN
	
	IF @wConludeType = 1 OR @wConludeType = 4 OR @wConludeType = 5
	BEGIN
		-- 1:打满局数,正常结算;  
		-- 4:游戏中玩家申请解散
		-- 5:创建房间后,长时间未结束
		--声明一个游标
		Declare @wUserIDInTable INT		
		Declare curUser Cursor for 
		
		Select UserID From UserInTable WHERE ServerID=@wServerID AND TableID=@wTableID
		--打开游标  
		OPEN curUser
		
		--循环并提取记录  
		FETCH NEXT FROM curUser Into @wUserIDInTable--取第一条记录存入@result中  
		WHILE ( @@Fetch_Status=0 )     
		BEGIN  

			DECLARE @wChairID AS INT
			DECLARE @wGameRoundCount AS INT
			DECLARE @lRoundScore AS BIGINT
			DECLARE @dwTableBuilder  AS INT
			-- 查询是否是机器人玩家
			SELECT @wChairID=ChairID, @wGameRoundCount=RoundCount, @lRoundScore=RoundScore, @dwTableBuilder=TableBuilder FROM UserInTable WHERE UserID=@wUserIDInTable
			-- 记录日志
			INSERT RecordUserInTable (UserID, ServerID, TableID, ChairID, RoundCount, RoundScore, TableBuilder, Secret) 
			VALUES (@wUserIDInTable, @wServerID, @wTableID, @wChairID, @wGameRoundCount, @lRoundScore, @dwTableBuilder, @strSecret)
			-- 删除游戏记录
			DELETE FROM UserInTable WHERE UserID=@wUserIDInTable
			-- 重置玩家分数
			Update GameScoreInfo SET Score = 1000000 WHERE UserID=@wUserIDInTable
			
			--下一条 
			FETCH NEXT FROM curUser into @wUserIDInTable
		END   

		--关闭游标     
		Close curUser  
		--释放游标  
		Deallocate curUser 
		
	END
	ELSE IF @wConludeType = 2 OR @wConludeType = 3
	BEGIN
		-- 2:未开始游戏,创建者解散;
		-- 3:未开始游戏,到时间自动解散; 
		
		DECLARE @dwAreaID AS INT
		SELECT @dwAreaID = AreaID FROM QPAccountsDB.dbo.AccountsInfo WHERE UserID = @dwUserID
		DECLARE @GrantDiamondCount AS INT
		SELECT @GrantDiamondCount=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'GrantDiamondCount' AND AreaID=@dwAreaID 
		IF @GrantDiamondCount IS NULL
		BEGIN
			SET @GrantDiamondCount = 0
		END
		
		-- 没有开始过游戏解散,道具退还
		SELECT @wConsumePropertyCount = PropertyConsumeCount FROM GameKindProperty WHERE KindID=@dwKindID AND RoundCount=@dwRoundCount
		IF @wConsumePropertyCount IS NULL
		BEGIN
			SET @wConsumePropertyCount = 0		
		END
		
		SELECT @dwCreaterPropertyCount=PropertyCount FROM UserProperty WHERE UserID=@dwUserID
		IF @dwCreaterPropertyCount IS NULL
		BEGIN
			SET @dwCreaterPropertyCount = @GrantDiamondCount
			-- 插入值
			INSERT UserProperty(UserID, PropertyCount)  VALUES(@dwUserID, @dwCreaterPropertyCount)
		END
		
		-- 更新玩家道具数量
		UPDATE UserProperty SET PropertyCount=PropertyCount+@wConsumePropertyCount WHERE UserID=@dwUserID
		IF @@ROWCOUNT=0
		INSERT UserProperty (UserID, PropertyCount) VALUES (@dwUserID, @wConsumePropertyCount)
		
		-- 记录日志
		INSERT INTO RecordUserProperty(SourceUserID,SourceDiamond,PropertyCount,OperateType,CollectNote)
		VALUES(@dwUserID, @dwCreaterPropertyCount, @wConsumePropertyCount, 2, N'钻石退回')	
	END
	
	INSERT RecordUserCreateTable(UserID, KindID, ServerID, TableID, CreateDateTime, Secret, CustomRule, RoundCount, ConludeType)
	VALUES(@dwUserID, @dwKindID, @wServerID, @wTableID, @CreateDateTime, @strSecret, @strCustomRule, @dwRoundCount, @wConludeType)
	
	-- 删除房创建间信息
	DELETE FROM UserCreateTable WHERE ServerID=@wServerID AND TableID=@wTableID

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	
	-- 获取桌子创建者剩余的钻石数量
	SELECT @dwCreaterPropertyCount=PropertyCount FROM UserProperty WHERE UserID=@dwUserID
	SELECT @dwUserID As CreateUserID, @dwCreaterPropertyCount AS CreaterPropertyCount

END

RETURN 0

GO
