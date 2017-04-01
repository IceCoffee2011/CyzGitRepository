
----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserSaveScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserSaveScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserTakeScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserTakeScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserTransferScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserTransferScore]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryUserInsureInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryUserInsureInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_VerifyInsurePassword]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_VerifyInsurePassword]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserTransferRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserTransferRecord]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ModifyInsurePassword]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ModifyInsurePassword]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryTransferRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryTransferRecord]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON
GO

----------------------------------------------------------------------------------------------------

-- 存入金币
CREATE PROC GSP_GR_UserSaveScore
	@dwUserID INT,								-- 用户 I D
	@lSaveScore BIGINT,							-- 金币数目
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT,						-- 房间 I D
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 金币变量
DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT
DECLARE @InsureRevenue BIGINT
DECLARE @VariationScore BIGINT
DECLARE @VariationInsure BIGINT

-- 执行逻辑
BEGIN

	-- 辅助变量
	DECLARE @EnjoinLogon INT
	DECLARE @EnjoinInsure INT

	-- 系统暂停
	SELECT @EnjoinInsure=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
		RETURN 2
	END

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 7
	END
 
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine
	FROM QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
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
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
			RETURN 1
		END
	END
	
	-- 金币判断
	DECLARE @BankPrerequisite AS INT
	SELECT @BankPrerequisite=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankPrerequisite'
	IF @BankPrerequisite IS NULL SET @BankPrerequisite=0
	IF @lSaveScore<@BankPrerequisite
	BEGIN
		SET @strErrorDescribe=N'存入保险柜的游戏币数目不能少于 '+LTRIM(STR(@BankPrerequisite))+'，游戏币存入失败！'
		RETURN 4
	END

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 游戏信息
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 金币判断
	IF @SourceScore IS NULL OR @SourceScore<@lSaveScore
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'您当前游戏币的可用余额不足，游戏币存入失败！'
		RETURN 4
	END

	-- 查询锁定
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID
	
	IF @@ROWCOUNT>=2
	BEGIN
		-- 错误信息
		SET @strErrorDescribe=N'抱歉地通知你，游戏币锁定操作失败，请联系客户服务中心了解详细情况！'
		RETURN 7
	END

	-- 锁定调整
	IF @LockKindID IS NULL SET @LockKindID=0
	IF @LockServerID IS NULL SET @LockServerID=0

	-- 锁定判断
	IF @LockKindID<>@wKindID OR @LockServerID<>@wServerID
	BEGIN

		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 查询类型
		IF @LockKindID<>0 AND @LockServerID<>0
		BEGIN
			-- 查询信息
			DECLARE @KindName NVARCHAR(31)
			DECLARE @ServerName NVARCHAR(31)
			SELECT @KindName=KindName FROM QPPlatformDB.dbo.GameKindItem WHERE KindID=@LockKindID
			SELECT @ServerName=ServerName FROM QPPlatformDB.dbo.GameRoomInfo WHERE ServerID=@LockServerID

			-- 错误信息
			IF @KindName IS NULL SET @KindName=N'未知游戏'
			IF @ServerName IS NULL SET @ServerName=N'未知房间'
			SET @strErrorDescribe=N'您正在 [ '+@KindName+N' ] 的 [ '+@ServerName+N' ] 游戏房间中，不能进行当前的保险柜操作！'
			RETURN 4

		END
		ELSE
		BEGIN
			-- 提示消息
			SELECT [ErrorDescribe]=N'您正在使用网站页面进行保险柜处理过程中，不能进行当前的保险柜操作！'
			SET @strErrorDescribe=N'您正在使用网站页面进行保险柜处理过程中，不能进行当前的保险柜操作！'
			RETURN 4
		END
	END

	-- 计算变量
	SET @InsureRevenue=0
	SET @VariationScore=-@lSaveScore
	SET @VariationInsure=@lSaveScore

	-- 设置信息
	SET @strErrorDescribe=N'游戏币存入保险柜操作成功，请查验您的账户信息！'

	-- 更新数据
	UPDATE GameScoreInfo SET Score=Score+@VariationScore, InsureScore=InsureScore+@VariationInsure, Revenue=Revenue+@InsureRevenue	
	WHERE UserID=@dwUserID

	-- 记录日志
	INSERT INTO RecordInsure(KindID,ServerID,SourceUserID,SourceGold,SourceBank,
		SwapScore,Revenue,IsGamePlaza,TradeType,ClientIP)
	VALUES(@wKindID,@wServerID,@UserID,@SourceScore,@SourceInsure,@lSaveScore,@InsureRevenue,0,1,@strClientIP)		

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 输出结果
	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, @VariationScore AS VariationScore,
		@VariationInsure AS VariationInsure, @InsureRevenue AS InsureRevenue, @LockKindID AS KindID, @LockServerID AS ServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 提取金币
CREATE PROC GSP_GR_UserTakeScore
	@dwUserID INT,								-- 用户 I D
	@lTakeScore BIGINT,							-- 金币数目
	@strPassword NCHAR(32),						-- 用户密码
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT,						-- 房间 I D
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 金币变量
DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT
DECLARE @InsureRevenue BIGINT
DECLARE @VariationScore BIGINT
DECLARE @VariationInsure BIGINT

-- 执行逻辑
BEGIN

	-- 辅助变量
	DECLARE @EnjoinLogon INT
	DECLARE @EnjoinInsure INT

	-- 系统暂停
	SELECT @EnjoinInsure=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
		RETURN 2
	END

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 7
	END
 
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @InsurePass=InsurePass, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine
	FROM QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
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
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
			RETURN 1
		END
	END

	-- 密码判断
	IF @InsurePass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的保险柜密码不正确或者输入有误，请查证后再次尝试！'
		RETURN 3
	END

	-- 金币判断
	DECLARE @BankPrerequisite AS INT
	SELECT @BankPrerequisite=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'BankPrerequisite'
	IF @BankPrerequisite IS NULL SET @BankPrerequisite=0
	IF @lTakeScore<@BankPrerequisite
	BEGIN
		SET @strErrorDescribe=N'从保险柜取出的游戏币数目不能少于 '+LTRIM(STR(@BankPrerequisite))+'，游戏币提取失败！'
		RETURN 4
	END

	-- 银行税收
	DECLARE @RevenueRate INT
	SELECT @RevenueRate=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTake'

	-- 税收调整
	IF @RevenueRate>300 SET @RevenueRate=300
	IF @RevenueRate IS NULL SET @RevenueRate=1

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 游戏信息
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 金币判断
	IF @SourceInsure IS NULL OR @SourceInsure<@lTakeScore
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'您当前保险柜的游戏币余额不足，游戏币提取失败！'
		RETURN 4
	END

	-- 查询锁定
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID
	
	IF @@ROWCOUNT>=2
	BEGIN
		-- 错误信息
		SET @strErrorDescribe=N'抱歉地通知你，游戏币锁定操作失败，请联系客户服务中心了解详细情况！'
		RETURN 7
	END

	-- 计算变量
	SET @InsureRevenue=@lTakeScore*@RevenueRate/1000
	SET @VariationScore=@lTakeScore-@InsureRevenue
	SET @VariationInsure=-@lTakeScore

	-- 设置信息
	SET @strErrorDescribe=N'保险柜提取游戏币操作成功，请查验您的账户信息！'

	-- 更新数据
	UPDATE GameScoreInfo SET Score=Score+@VariationScore, InsureScore=InsureScore+@VariationInsure, Revenue=Revenue+@InsureRevenue
	WHERE UserID=@dwUserID

	-- 记录日志
	INSERT INTO RecordInsure(KindID,ServerID,SourceUserID,SourceGold,SourceBank,
		SwapScore,Revenue,IsGamePlaza,TradeType,ClientIP)
	VALUES(@wKindID,@wServerID,@UserID,@SourceScore,@SourceInsure,@lTakeScore,@InsureRevenue,0,2,@strClientIP)	

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 输出结果
	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, @VariationScore AS VariationScore,
		@VariationInsure AS VariationInsure, @InsureRevenue AS InsureRevenue, @LockKindID AS KindID, @LockServerID AS ServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 转账金币
CREATE PROC GSP_GR_UserTransferScore
	@dwUserID INT,								-- 用户 I D
	@cbByNickName INT,                          -- 依据用户昵称
	@lTransferScore BIGINT,						-- 金币数目
	@strPassword NCHAR(32),						-- 用户密码
	@strNickName NVARCHAR(31),					-- 用户昵称
	@wKindID SMALLINT,							-- 游戏 I D
	@wServerID SMALLINT,						-- 房间 I D
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 金币变量
DECLARE @SourceScore BIGINT
DECLARE @SourceInsure BIGINT

DECLARE @InsureRevenue BIGINT
DECLARE @VariationInsure BIGINT

DECLARE @SourceUserMemberOrder tinyint
DECLARE @TargetUserMemberOrder tinyint

DECLARE @LoveLiness INT

DECLARE @ReceiveScore BIGINT

-- 执行逻辑
BEGIN

	-- 辅助变量
	DECLARE @EnjoinLogon INT
	DECLARE @EnjoinInsure INT

	-- 系统暂停
	SELECT @EnjoinInsure=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
		RETURN 2
	END

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 7
	END
 
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE	@SourceNickName NVARCHAR(31)
	DECLARE @SourceGameID INT
	DECLARE @SourceAreaID INT
	SELECT @UserID=UserID, @SourceGameID=GameID, @SourceNickName=NickName, @InsurePass=InsurePass,
	@Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine,
	@LoveLiness=LoveLiness, @SourceAreaID=AreaID
	FROM QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试！'
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
	IF @MoorMachine=1
	BEGIN
		IF @MachineID<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
			RETURN 1
		END
	END

	-- 密码判断
	IF @InsurePass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的保险柜密码不正确或者输入有误，请查证后再次尝试！'
		RETURN 3
	END

	-- 金币判断
	DECLARE @TransferPrerequisite AS BIGINT
	SELECT @TransferPrerequisite=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferPrerequisite'
	IF @TransferPrerequisite IS NULL SET @TransferPrerequisite=0
	IF @lTransferScore<@TransferPrerequisite
	BEGIN
		SET @strErrorDescribe=N'从保险柜转账的游戏币数目不能少于 '+LTRIM(STR(@TransferPrerequisite))+'，游戏币转账失败！'
		RETURN 4
	END

	-- 目标用户
	DECLARE @TargetUserID INT
	DECLARE	@TargetNickName NVARCHAR(31)
	DECLARE @TargetGameID INT
	DECLARE @TargetAreaID INT
	IF @cbByNickName=1
		SELECT @TargetUserID=UserID, @TargetNickName=NickName, @TargetGameID=GameID, @TargetAreaID=AreaID
		FROM QPAccountsDB.dbo.AccountsInfo WHERE NickName=@strNickName
	ELSE
		SELECT @TargetUserID=UserID, @TargetNickName=NickName, @TargetGameID=GameID, @TargetAreaID=AreaID
		FROM QPAccountsDB.dbo.AccountsInfo WHERE GameID=@strNickName

	-- 查询用户
	IF @TargetUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您所要转账的用户“'+@strNickName+'”不存在或者输入有误，请查证后再次尝试！'
		RETURN 5
	END

	-- 相同判断
	IF @TargetUserID=@dwUserID
	BEGIN
		SET @strErrorDescribe=N'不能使用自己的帐号为自己转账游戏币，请查证后再次尝试！'
		RETURN 6
	END
	
	-- 判断是否允许给当前区域ID的玩家转账
	IF NOT EXISTS (SELECT * FROM QPPlatformDB.dbo.TransferScoreLimit WHERE SourceAreaID=@SourceAreaID AND TargetAreaID=@TargetAreaID)
	AND @SourceAreaID<>@TargetAreaID
	BEGIN
		SET @strErrorDescribe=N'要转账账号的区域不在允许范围内,请查证后再次尝试！如有疑问请联系客服.'
		RETURN 6	
	END

	-- 金币查询
	DECLARE @TargetScore BIGINT
	DECLARE @TargetInsure BIGINT
	SELECT @TargetScore=Score, @TargetInsure=InsureScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@TargetUserID

	-- 插入判断
	IF @TargetScore IS NULL
	BEGIN
		-- 插入资料
		INSERT INTO GameScoreInfo (UserID,LastLogonIP,RegisterIP) VALUES (@TargetUserID,@strClientIP,@strClientIP)

		-- 游戏信息
		SELECT @TargetScore=Score, @TargetInsure=InsureScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@TargetUserID
	END
	
	-- 查询玩家级别
	SELECT @SourceUserMemberOrder=MemberOrder
	FROM QPAccountsDB.dbo.AccountsMember WHERE UserID=@dwUserID
	IF @@ROWCOUNT=0 
	BEGIN
		SET @SourceUserMemberOrder=0
	END
	
	SELECT @TargetUserMemberOrder=MemberOrder
	FROM QPAccountsDB.dbo.AccountsMember WHERE UserID=@TargetUserID
	IF @@ROWCOUNT=0 
	BEGIN
		SET @TargetUserMemberOrder=0
	END
	
	-- 银行税收
	DECLARE @RevenueRate INT
	DECLARE @KickBackRate INT		
	
	-- 根据玩家等级来选择税收方案
	IF @SourceUserMemberOrder=0
	BEGIN
		-- 发起玩家是普通玩家
		IF @TargetUserMemberOrder=0
		BEGIN
			-- 如果收取的是普通玩家,正常扣税
			SELECT @RevenueRate=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransfer'
		END
		ELSE
		BEGIN
			-- 如果收取的是蓝钻玩家,根据配置扣税
			SELECT @RevenueRate=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransferNormalToVip'
		END
	END
	ELSE
	BEGIN
		-- 发起玩家不是普通玩家,再判断收取玩家
		IF @TargetUserMemberOrder=0
		BEGIN
			-- 收款玩家是普通玩家
			SELECT @RevenueRate=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransferVipToNormal'
			-- 还要考虑到回扣
			SELECT @KickBackRate=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransferVipToNormalKickback'
		END
		ELSE
		BEGIN
			-- 收款玩家是蓝钻玩家
			SELECT @RevenueRate=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransferVipToVip'
		END
	END
	
	--SELECT @RevenueRate=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransfer'
	-- 税收调整
	--IF @RevenueRate>300 SET @RevenueRate=300
	IF @RevenueRate IS NULL SET @RevenueRate=0
	-- 设置回扣
	IF @KickBackRate IS NULL SET @KickBackRate=0
	-- 回扣不能比税还多
	IF @KickBackRate > @RevenueRate
	BEGIN
		SET @KickBackRate=0
	END

	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

	-- 游戏信息
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 金币判断
	IF @SourceInsure IS NULL OR @SourceInsure<@lTransferScore
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'您当前保险柜的游戏币余额不足，游戏币转账失败！'
		RETURN 4
	END
	
	-- 计算变量
	SET @InsureRevenue=@lTransferScore*@RevenueRate/1000
	SET @VariationInsure=-@lTransferScore
	
	-- 判断转账方的剩余金币是否够扣税
	IF @SourceInsure-@lTransferScore-@InsureRevenue < 0
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
		
		SET @strErrorDescribe=N'转账后,您保险柜的游戏币余额不足以支付手续费，游戏币转账失败！'
		RETURN 5
	END
	

	-- 查询锁定
	DECLARE @LockKindID INT
	DECLARE @LockServerID INT
	SELECT @LockKindID=KindID, @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

	-- 设置信息
	SET @strErrorDescribe=N'转账 '+LTRIM(STR(@lTransferScore))+' 游戏币到“'+@strNickName+'”的保险柜操作成功，请查验您的账户信息！'


	-- 扣除金币
	UPDATE GameScoreInfo SET InsureScore=InsureScore+@VariationInsure-@InsureRevenue,Revenue=Revenue+@InsureRevenue WHERE UserID=@dwUserID
	SET @SourceInsure = @SourceInsure+@VariationInsure-@InsureRevenue

	-- 增加金币
	UPDATE GameScoreInfo SET InsureScore=InsureScore+@lTransferScore WHERE UserID=@TargetUserID
	SET @ReceiveScore=@lTransferScore
	
	-- 记录日志
	INSERT INTO RecordInsure(KindID,ServerID,SourceUserID,SourceGold,SourceBank,
		TargetUserID,TargetGold,TargetBank,SwapScore,Revenue,IsGamePlaza,TradeType,ClientIP)
	VALUES(@wKindID,@wServerID,@UserID,@SourceScore,@SourceInsure,@TargetUserID,@TargetScore,@TargetInsure,
		@lTransferScore,@InsureRevenue,0,3,@strClientIP) 

	-- 考虑到是否有回扣(蓝钻玩家→普通玩家且回扣配置不为0)
	DECLARE @KickbackRevenue BIGINT
	IF @SourceUserMemberOrder<>0 AND @TargetUserMemberOrder=0 AND @KickBackRate<>0
	BEGIN
		-- 计算回扣的数量
		SET @KickbackRevenue = (@lTransferScore*@KickBackRate)/1000
		-- 把回扣增加的魅力记录日志
		INSERT INTO RecordLoveLiness(SourceUserID,SourceGold,SourceBank,SwapScore,LoveLiness,IsGamePlaza,TradeType,ClientIP,CollectNote)
		VALUES(@dwUserID,@SourceScore,@SourceInsure,@lTransferScore,@KickbackRevenue,0,1,@strClientIP,N'转账回扣')	
		
		-- 更新玩家魅力
		Update QPAccountsDB.dbo.AccountsInfo
		SET QPAccountsDB.dbo.AccountsInfo.LoveLiness = QPAccountsDB.dbo.AccountsInfo.LoveLiness + @KickbackRevenue 
		WHERE QPAccountsDB.dbo.AccountsInfo.UserID=@UserID
		
		SET @LoveLiness=@LoveLiness + @KickbackRevenue 
	END
--	ELSE
--	BEGIN
--		SET @strErrorDescribe=N'SourceUserMemberOrder='+LTRIM(STR(@SourceUserMemberOrder))+N',TargetUserMemberOrder='+LTRIM(STR(@TargetUserMemberOrder))+N',KickBackRate='+LTRIM(STR(@KickBackRate))
--	END
	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 输出结果
	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, 0 AS VariationScore,
		@VariationInsure AS VariationInsure, @InsureRevenue AS InsureRevenue, @LockKindID AS KindID, @LockServerID AS ServerID,
		@TargetUserID As TargetUserID, @TargetNickName As TargetNickName, @TargetGameID As TargetGameID,
		@SourceGameID As SourceGameID, @SourceNickName As SourceNickName, @@IDENTITY As RecordID,
		@lTransferScore As lTransferScore, @InsureRevenue As InsureRevenue, getdate() As stTransferTime,
		@LoveLiness As LoveLiness, @ReceiveScore As ReceiveScore
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询银行
CREATE PROC GSP_GR_QueryUserInsureInfo
	@dwUserID INT,								-- 用户 I D
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @ServerID SMALLINT

-- 执行逻辑
BEGIN

	-- 银行税率
	DECLARE @RevenueRateTake AS INT
	DECLARE @RevenueRateTransfer AS INT
	DECLARE @TransferPrerequisite AS BIGINT
	SELECT @RevenueRateTake=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTake'
	SELECT @RevenueRateTransfer=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'RevenueRateTransfer'
	SELECT @TransferPrerequisite=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TransferPrerequisite'

	-- 参数调整
	IF @RevenueRateTake IS NULL SET @RevenueRateTake=1
	IF @RevenueRateTransfer IS NULL SET @RevenueRateTransfer=1
	IF @TransferPrerequisite IS NULL SET @TransferPrerequisite=0

	-- 查询房间
	SELECT @ServerID=ServerID FROM GameScoreLocker(NOLOCK) WHERE UserID=@dwUserID
	-- 如果至少有两条记录,说明锁定房间的时候出现了问题
	IF @@ROWCOUNT>=2
	BEGIN
		-- 错误信息
		SET @strErrorDescribe=N'抱歉地通知你，游戏币锁定操作失败，请联系客户服务中心了解详细情况！'
		RETURN 7
	END

	-- 查询积分
	SELECT @Score=Score, @Insure=InsureScore FROM GameScoreInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- 查询区域
	DECLARE @dwAreaID INT 
	SELECT @dwAreaID = AreaID FROM qpaccountsdb.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 数据调整
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @ServerID IS NULL SET @ServerID=0
	
	-- 查询钻石
	DECLARE @dwPropertyCount As INT
	SELECT @dwPropertyCount=PropertyCount FROM QPUserCustomDB.dbo.UserProperty(NOLOCK) WHERE UserID=@dwUserID
	IF @dwPropertyCount IS NULL
	BEGIN
		DECLARE @GrantDiamondCount AS INT
		SELECT @GrantDiamondCount=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'GrantDiamondCount' AND AreaID=@dwAreaID
		SET @dwPropertyCount=@GrantDiamondCount
		IF @dwPropertyCount IS NULL
		BEGIN
			SET @dwPropertyCount = 0
		END
	END

	-- 输出结果
	SELECT @dwUserID AS UserID, @Score AS Score, @Insure AS Insure, @ServerID AS ServerID, @RevenueRateTake AS RevenueTake,
	@RevenueRateTransfer AS RevenueTransfer, @TransferPrerequisite AS TransferPrerequisite, @dwPropertyCount As PropertyCount

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 查询银行
CREATE PROC GSP_GR_VerifyInsurePassword
	@dwUserID INT,								-- 用户 I D
	@strInsurePassword NVARCHAR(32),			-- 用户银行密码
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询用户
	DECLARE @UserID INT
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @SourceScore BIGINT
	DECLARE @SourceInsure BIGINT
	DECLARE @InsureRevenue BIGINT
	DECLARE @VariationScore BIGINT
	DECLARE @VariationInsure BIGINT
	SELECT @UserID=UserID, @InsurePass=InsurePass FROM QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在，请查证后再次尝试登录！'
		RETURN 1
	END	
	
	-- 密码判断
	IF @InsurePass<>@strInsurePassword
	BEGIN
		SET @strErrorDescribe=N'您的银行密码不正确或者输入有误，请查证后再次尝试！'
		RETURN 2
	END
	
	-- 游戏信息
	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		-- GameScoreInfo表中没有数据,则插入数据
		INSERT GameScoreInfo (UserID, Score) VALUES (@UserID, 0) 
		SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID
	END
	
	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, 0 AS VariationScore,
		0 AS VariationInsure, 0 AS InsureRevenue
	
	return 0
END

RETURN 0

GO
-----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------

-- 修改密码
CREATE PROC GSP_GR_ModifyInsurePassword
	@dwUserID INT,								-- 用户 I D
	@strSrcPassword NCHAR(32),					-- 用户密码
	@strNewPassword NCHAR(32),					-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 变量定义
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @SourceScore BIGINT
	DECLARE @SourceInsure BIGINT
	DECLARE @InsurePass AS NCHAR(32)

	-- 查询用户
	SELECT @UserID=UserID, @InsurePass=InsurePass, @Nullity=Nullity, @StunDown=StunDown
	FROM QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试！'
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
	
	-- 密码判断
	IF @InsurePass IS NOT NULL AND @InsurePass<>@strSrcPassword
	BEGIN
		SET @strErrorDescribe=N'您的密码输入有误，请查证后再次尝试！'
		RETURN 3
	END

	-- 密码判断
	IF @strNewPassword=N''
	BEGIN
		SET @strErrorDescribe=N'新帐号密码为空，不允许设置密码为空，请查证后再次尝试！'
		RETURN 3
	END

	-- 修改密码
	UPDATE QPAccountsDB.dbo.AccountsInfo SET InsurePass=@strNewPassword WHERE UserID=@dwUserID

	-- 修改密码记录
	INSERT INTO QPRecordDB.dbo.RecordPasswdExpend(OperMasterID,UserID,ReLogonPasswd,ReInsurePasswd,ClientIP)
	VALUES(0,@dwUserID,N'',@strNewPassword,@strClientIP)

	-- 设置信息
	IF @@ERROR=0 
	BEGIN
		SET @strErrorDescribe=N'保险柜密码修改成功，请牢记您的新保险柜密码！'
	END
	
	return 0
	-- 游戏信息
--	SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	
--	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, 0 AS VariationScore,
--		0 AS VariationInsure, 0 AS InsureRevenue
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 修改密码
CREATE PROC GSP_GR_QueryTransferRecord
	@dwUserID INT								-- 用户 I D
--	@strClientIP NVARCHAR(15),					-- 连接地址
--	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

Declare @RecordID		INT
Declare @SourceUserID 	INT
Declare @TargetUserID 	INT
Declare @SwapScore		BIGINT
Declare @Revenue		BIGINT
Declare @CollectDate	datetime
Declare @SourceNickName NVARCHAR(31)
Declare @TargetNickName NVARCHAR(31)
Declare @SourceGameID 	INT
Declare @TargetGameID 	INT


-- 执行逻辑
BEGIN
	
	--声明一个游标  
	Declare CurRecord Cursor FOR 

	SELECT
	dbo.RecordInsure.RecordID,
	dbo.RecordInsure.SourceUserID,
	dbo.RecordInsure.TargetUserID,
	dbo.RecordInsure.SwapScore,
	dbo.RecordInsure.Revenue,
	dbo.RecordInsure.CollectDate
	FROM dbo.RecordInsure WHERE 
	(dbo.RecordInsure.SourceUserID = @dwUserID OR dbo.RecordInsure.TargetUserID = @dwUserID) AND
	dbo.RecordInsure.TradeType = 3
	AND DATEDIFF(DAY,CollectDate,GETDATE()) <= 31
	ORDER BY
	dbo.RecordInsure.CollectDate DESC
--	SELECT
--	dbo.RecordInsure.RecordID,
--	dbo.RecordInsure.SourceUserID,
--	dbo.RecordInsure.TargetUserID,
--	dbo.RecordInsure.SwapScore,
--	dbo.RecordInsure.Revenue,
--	dbo.RecordInsure.CollectDate
--	FROM dbo.RecordInsure WHERE 
--	dbo.RecordInsure.TargetUserID = @dwUserID AND
--	dbo.RecordInsure.TradeType = 3
--	AND DATEDIFF(DAY,CollectDate,GETDATE()) <= 31
--	ORDER BY
--	dbo.RecordInsure.CollectDate DESC
	-- 创建临时表
	DECLARE @TempTable 
	TABLE(RecordID INT, SourceUserID INT, TargetUserID INT, SwapScore BIGINT, Revenue BIGINT, CollectDate datetime, SourceNickName NVARCHAR(31), TargetNickName NVARCHAR(31), SourceGameID INT, TargetGameID INT)

	--打开游标  
	OPEN CurRecord
	--循环并提取记录  
	--取第一条记录存入变量中  
	FETCH NEXT FROM CurRecord Into @RecordID,@SourceUserID,@TargetUserID,@SwapScore,@Revenue,@CollectDate 
	WHILE ( @@Fetch_Status=0 )     
	BEGIN 
		SELECT @SourceGameID=GameID, @SourceNickName=NickName From QPAccountsDb.dbo.AccountsInfo WHERE 
		QPAccountsDb.dbo.AccountsInfo.UserID=@SourceUserID
		SELECT @TargetGameID=GameID, @TargetNickName=NickName From QPAccountsDb.dbo.AccountsInfo WHERE 
		QPAccountsDb.dbo.AccountsInfo.UserID=@TargetUserID
		
		-- 选中
--		SELECT @RecordID As RecordID, @SourceUserID As SourceUserID, @TargetUserID As TargetUserID,
--		@SwapScore As SwapScore, @Revenue As Revenue, @CollectDate As CollectDate, @SourceNickName As SourceNickName,
--		@TargetNickName As TargetNickName, @SourceGameID As SourceGameID, @TargetGameID As TargetGameID
		
		INSERT INTO @TempTable 
		VALUES(@RecordID,@SourceUserID,@TargetUserID,@SwapScore,@Revenue,@CollectDate,@SourceNickName,@TargetNickName,@SourceGameID,@TargetGameID)
		FETCH NEXT FROM CurRecord Into @RecordID,@SourceUserID,@TargetUserID,@SwapScore,@Revenue,@CollectDate 
	END
	
	--关闭游标     
	Close CurRecord  
	--释放游标  
	Deallocate CurRecord 
	
	SELECT * FROM @TempTable;
	return 0
END

RETURN 0

GO


