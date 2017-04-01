----------------------------------------------------------------------------------------------------

USE QPUserCustomDB
GO

-- 用户创建桌子
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_UserTransferDiamond]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_UserTransferDiamond]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- 创建桌子
CREATE PROC GSP_GR_UserTransferDiamond
	@dwUserID INT,								-- 转账用户UserID
	@dwGameID INT,								-- 接受用户GameID
	@lTransferDiamond INT,						-- 金币数目
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 辅助变量


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
	SELECT @EnjoinLogon=EnjoinLogon FROM QPAccountsDB.dbo.ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM QPAccountsDB.dbo.ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
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
	SELECT @UserID=UserID, @SourceGameID=GameID, @SourceNickName=NickName, @InsurePass=InsurePass,@Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine,@MachineID=LastLogonMachine,@SourceAreaID=AreaID
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
	
	-- 查询玩家级别
	DECLARE @SourceUserMemberOrder TINYINT
	SELECT @SourceUserMemberOrder=MemberOrder FROM QPAccountsDB.dbo.AccountsMember WHERE UserID=@dwUserID
	IF @SourceUserMemberOrder IS NULL OR @SourceUserMemberOrder=0
	BEGIN
		SET @strErrorDescribe=N'您的权限不足,不能转账钻石给其他玩家！'
		RETURN 3
	END
	
	-- 目标用户
	DECLARE @TargetUserID INT
	DECLARE	@TargetNickName NVARCHAR(31)
	DECLARE @TargetGameID INT
	DECLARE @TargetAreaID INT
	DECLARE @TargetDiamond INT
	SELECT @TargetUserID=UserID, @TargetNickName=NickName, @TargetGameID=GameID, @TargetAreaID=AreaID
	FROM QPAccountsDB.dbo.AccountsInfo WHERE GameID=@dwGameID

	-- 查询用户
	IF @TargetUserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您所要转账的用户不存在或者输入有误，请查证后再次尝试！'
		RETURN 5
	END

	-- 相同判断
	IF @TargetUserID=@dwUserID
	BEGIN
		SET @strErrorDescribe=N'不能使用自己的帐号为自己转账钻石，请查证后再次尝试！'
		RETURN 6
	END
	
	-- 接收方金币查询
	SELECT @TargetDiamond=PropertyCount FROM UserProperty(NOLOCK) WHERE UserID=@TargetUserID

	-- 插入判断
	IF @TargetDiamond IS NULL
	BEGIN
		DECLARE @GrantDiamondCount AS INT
		SELECT @GrantDiamondCount=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'GrantDiamondCount' AND AreaID=@TargetAreaID
		IF @GrantDiamondCount IS NULL
		BEGIN
			SET @GrantDiamondCount = 0
		END
		-- 插入资料
		INSERT INTO UserProperty (UserID,PropertyCount) VALUES (@TargetUserID,@GrantDiamondCount)

		-- 游戏信息
		SELECT @TargetDiamond=PropertyCount FROM UserProperty(NOLOCK) WHERE UserID=@TargetUserID
	END
	
	DECLARE @SourceDiamond INT
	
	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN
	
	-- 游戏信息
	SELECT @SourceDiamond=PropertyCount FROM UserProperty WHERE UserID=@dwUserID

	-- 金币判断
	IF @SourceDiamond IS NULL OR @SourceDiamond<@lTransferDiamond
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED

		-- 错误信息
		SET @strErrorDescribe=N'您当前钻石余额不足，钻石转账失败！'
		RETURN 4
	END

	-- 扣除金币
	UPDATE UserProperty SET PropertyCount=PropertyCount-@lTransferDiamond WHERE UserID=@dwUserID

	-- 增加金币
	UPDATE UserProperty SET PropertyCount=PropertyCount+@lTransferDiamond WHERE UserID=@TargetUserID	
	
	SET @strErrorDescribe=N'转账 '+LTRIM(STR(@lTransferDiamond))+' 钻石到“'+@TargetNickName+'”的账号操作成功，请查验您的账户信息！'

	-- 记录日志
	INSERT INTO RecordUserProperty(SourceUserID,SourceDiamond,TargetUserID,TargetDiamond,PropertyCount,OperateType,ClientIP,CollectNote)
	VALUES(@dwUserID, @SourceDiamond, @TargetUserID, @TargetDiamond, @lTransferDiamond, 4, @strClientIP, N'玩家转账')	
	
	SET @SourceDiamond = @SourceDiamond-@lTransferDiamond

	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	SELECT @dwUserID As UserID, @SourceDiamond As SourceDiamond
END

RETURN 0

GO