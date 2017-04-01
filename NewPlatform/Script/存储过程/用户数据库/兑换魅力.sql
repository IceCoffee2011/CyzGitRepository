
----------------------------------------------------------------------------------------------------

USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryExchangeLoveliness]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryExchangeLoveliness]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ExchangeLoveliness]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ExchangeLoveliness]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 查询魅力兑换的相关消息
CREATE PROC GSP_GP_QueryExchangeLoveliness
	@dwUserID INT,								-- 用户 I D
	@strTips NVARCHAR(1024) OUTPUT,				-- 提示
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN 
	-- 辅助变量
	DECLARE @LoveLiness INT
--	DECLARE @strTips NVARCHAR(1024) 
--	DECLARE @EnjoinLogon INT
--	DECLARE @EnjoinInsure INT

	-- 系统暂停
--	SELECT @EnjoinInsure=StatusValue FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
--	IF @EnjoinInsure IS NOT NULL AND @EnjoinInsure<>0
--	BEGIN
--		SELECT @strErrorDescribe=StatusString FROM QPAccountsDBLink.QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'EnjoinInsure'
--		RETURN 2
--	END

	-- 效验地址
--	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
--	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
--	BEGIN
--		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的游戏服务权限，请联系客户服务中心了解详细情况！'
--		RETURN 4
--	END
	
	-- 效验机器
--	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
--	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
--	BEGIN
--		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的游戏服务权限，请联系客户服务中心了解详细情况！'
--		RETURN 7
--	END

	-- 载入魅力兑换配置
	DECLARE @ExchangeRate SMALLINT
	DECLARE @MinExchangeAllow INT
	DECLARE @ExchangeUnit INT
	SELECT @ExchangeRate=ExchangeRate, @MinExchangeAllow=MinExchangeAllow, @ExchangeUnit=ExchangeUnit, @strTips=ExchangeTips
	FROM QPPlatformDB.dbo.ExchangeLovelinessInfo
	
--	SELECT @strTips As Tips
	
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine,@LoveLiness=LoveLiness
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
	
	-- 没有魅力,无法兑换
	IF @LoveLiness=0
	BEGIN
		SET @strErrorDescribe=N'您的帐号没有魅力值,无法兑换'
		return 3
	END
	
	IF @ExchangeRate IS NULL OR @MinExchangeAllow IS NULL OR @ExchangeUnit IS NULL OR @ExchangeUnit=0
	BEGIN
		SET @strErrorDescribe=N'读取魅力兑换配置失败,无法兑换魅力'
		return 6
	END
	
	IF @LoveLiness < @MinExchangeAllow 
	BEGIN
		SET @strErrorDescribe=N'您的魅力值 [ '+LTRIM(STR(@LoveLiness))+N' ] 小于起兑的最小值 [ '+LTRIM(STR(@MinExchangeAllow))+N' ] ，不能进行兑换！'
		return 7
	END
	
	-- 计算实际能兑的数目
	DECLARE @UnitAmount INT
	SET @UnitAmount= @LoveLiness / @ExchangeUnit
	
	-- 计算最终能得到的货币值
	DECLARE @AvaibleAmount BIGINT
	DECLARE @FinalAmount BIGINT
	SET @AvaibleAmount = @UnitAmount * @ExchangeUnit
	SET @FinalAmount = (@AvaibleAmount * @ExchangeRate) / 1000
	
	SELECT @dwUserID As UserID, @LoveLiness As LoveLiness, @AvaibleAmount As AvaibleAmount,
	 	@FinalAmount As FinalAmount
	
	return 0
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 兑换魅力
-- 修改密码
CREATE PROC GSP_GP_ExchangeLoveliness
	@dwUserID INT,								-- 用户 I D
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @LoveLiness INT
DECLARE	@InsureScore BIGINT
DECLARE @Score BIGINT

-- 执行逻辑
BEGIN 
	
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine,@LoveLiness=LoveLiness
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
	
	-- 没有魅力,无法兑换
	IF @LoveLiness=0
	BEGIN
		SET @strErrorDescribe=N'您的帐号没有魅力值,无法兑换'
		return 3
	END

	-- 载入魅力兑换配置
	DECLARE @ExchangeRate INT
	DECLARE @MinExchangeAllow INT
	DECLARE @ExchangeUnit INT
	SELECT @ExchangeRate=ExchangeRate, @MinExchangeAllow=MinExchangeAllow, @ExchangeUnit=ExchangeUnit
	FROM QPPlatformDB.dbo.ExchangeLovelinessInfo
	
	IF @ExchangeRate IS NULL OR @MinExchangeAllow IS NULL OR @ExchangeUnit IS NULL OR @ExchangeUnit=0
	BEGIN
		SET @strErrorDescribe=N'读取魅力兑换配置失败,无法兑换魅力'
		return 6
	END
	
	IF @LoveLiness < @MinExchangeAllow 
	BEGIN
		SET @strErrorDescribe=N'您的魅力值 [ '+LTRIM(STR(@LoveLiness))+N' ] 小于起兑的最小值 [ '+LTRIM(STR(@MinExchangeAllow))+N' ] ，不能进行兑换！'
		return 7
	END
	
	-- 计算实际能兑的数目
	DECLARE @UnitAmount BIGINT
	SET @UnitAmount= @LoveLiness / @ExchangeUnit
	
	-- 计算最终能得到的货币值
	DECLARE @AvaibleAmount BIGINT
	DECLARE @FinalAmount BIGINT
	SET @AvaibleAmount = @UnitAmount * @ExchangeUnit
	SET @FinalAmount = (@UnitAmount * @ExchangeUnit * @ExchangeRate) / 1000
	
	-- 获取魅力,银行货币最新值
	DECLARE @SourceScore BIGINT
	DECLARE	@SourceInsureScore BIGINT
	SELECT @LoveLiness=QPAccountsDB.dbo.AccountsInfo.LoveLiness, @SourceScore=QPTreasureDB.dbo.GameScoreInfo.Score,
	@SourceInsureScore=QPTreasureDB.dbo.GameScoreInfo.InsureScore
	FROM QPAccountsDB.dbo.AccountsInfo,QPTreasureDB.dbo.GameScoreInfo
	WHERE QPAccountsDB.dbo.AccountsInfo.UserID=@dwUserID AND QPTreasureDB.dbo.GameScoreInfo.UserID=@dwUserID
	
	SET @Score=@SourceScore
	
	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN
	
	-- 扣除魅力
	Update QPAccountsDB.dbo.AccountsInfo 
	SET LoveLiness=LoveLiness-@AvaibleAmount
	WHERE UserID=@dwUserID
	IF @@ROWCOUNT=0
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
		SET @strErrorDescribe=N'扣除魅力失败！' 
		RETURN 1
	END
	SET @LoveLiness=@LoveLiness-@AvaibleAmount
	
	-- 加入欢乐豆
	Update QPTreasureDB.dbo.GameScoreInfo
	SET InsureScore=InsureScore+@FinalAmount
	WHERE UserID=@dwUserID
	
	IF @@ROWCOUNT=0
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
		SET @strErrorDescribe=N'增加游戏币失败！' 
		RETURN 1
	END
	SET @InsureScore=@SourceInsureScore+@FinalAmount
	
	-- 写入日志
	INSERT INTO
	QPTreasureDB.dbo.RecordLoveLiness(SourceUserID,SourceGold,SourceBank,TargetUserID,TargetGold,TargetBank,SwapScore,LoveLiness,IsGamePlaza,TradeType,ClientIP,CollectNote)
	VALUES(@dwUserID,@SourceScore,@SourceInsureScore, @dwUserID,@Score,@InsureScore,@FinalAmount,-@AvaibleAmount,0,2,@strClientIP,N'魅力兑换')
	IF @@ROWCOUNT=0
	BEGIN
		-- 结束事务
		ROLLBACK TRAN
		SET TRANSACTION ISOLATION LEVEL READ COMMITTED
		SET @strErrorDescribe=N'记录兑换日志失败！' 
		RETURN 1
	END
	
	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	
	
	SET @strErrorDescribe=N'魅力兑换游戏币成功,请在银行中查收!'
	
	SELECT @dwUserID As UserID, @LoveLiness As LoveLiness, @Score As Score, @InsureScore As InsureScore
	return 0
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
