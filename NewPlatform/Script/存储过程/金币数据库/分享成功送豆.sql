
----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_ShareSuccess]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_ShareSuccess]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON
GO

----------------------------------------------------------------------------------------------------

-- 存入金币
CREATE PROC GSP_MB_ShareSuccess
	@dwUserID INT,								-- 用户 I D
	@strMachineID NCHAR(32),					-- 机器标识
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 辅助变量
	DECLARE @EnjoinLogon INT
	DECLARE @EnjoinInsure INT
	DECLARE @ShareLimitCount INT
	DECLARE @ShareIPLimitCount INT
	DECLARE @ShareDeviceLimitCount INT
	DECLARE	@ShareRewardCount INT

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 1
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的游戏服务权限，请联系客户服务中心了解详细情况！'
		RETURN 2
	END
	
	-- 查询用户
	DECLARE @UserID INT
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE	@MachineID NVARCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE @AreaID	INT
	SELECT @UserID=UserID, @Nullity=Nullity, @StunDown=StunDown, @MoorMachine=MoorMachine, @MachineID=LastLogonMachine, @AreaID=AreaID
	FROM QPAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 4
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 5
	END	

	-- 帐号关闭
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'您的帐号使用了安全关闭功能，必须重新开通后才能继续使用！'
		RETURN 6
	END	
	
	-- 获取分享次数限制
	SELECT @ShareLimitCount=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'ShareLimitCount' AND AreaID=@AreaID
	SELECT @ShareIPLimitCount=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'ShareIPLimitCount' AND AreaID=@AreaID
	SELECT @ShareDeviceLimitCount=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'ShareDeviceLimitCount' AND AreaID=@AreaID
	IF @ShareLimitCount IS NULL OR @ShareLimitCount=0 OR @ShareIPLimitCount IS NULL OR @ShareIPLimitCount=0 OR @ShareDeviceLimitCount IS NULL OR @ShareDeviceLimitCount=0
	BEGIN
		SET @strErrorDescribe=N'获取分享奖励配置失败,请联系客户服务中心了解详细情况!'
		RETURN 3
	END
	
	-- 获取分享奖励数量
	-- 带上区域ID
	SELECT @ShareRewardCount=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'ShareRewardCount' AND AreaID=@AreaID
	IF @ShareRewardCount IS NULL OR @ShareRewardCount < 0
	BEGIN
		SET @ShareRewardCount=0
	END
	
	-- 判断机器ID参数是否正确
	IF @strMachineID=''
	BEGIN
		SET @strErrorDescribe=N'获取当前设备配置失败,请联系客户服务中心了解详细情况！'
		RETURN 7
	END
	
	-- 固定机器
--	IF @MoorMachine=1
--	BEGIN
--		IF @MachineID<>@strMachineID
--		BEGIN
--			SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
--			RETURN 1
--		END
--	END
	
	-- 记录日志
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- 统计24小时内分享次数
	DECLARE @ShareCount INT
--	SELECT @ShareCount=Count(*) FROM RecordAppShare WHERE UserID=@dwUserID AND CollectDate >= DATEADD(hh,-24,GETDATE()) AND ShareRewardCount > 0
	SELECT @ShareCount=Count(*) FROM RecordAppShare WHERE UserID=@dwUserID AND DateID = @DateID AND ShareRewardCount > 0
	DECLARE @ShareIPCount INT
--	SELECT @ShareIPCount=Count(*) FROM RecordAppShare WHERE ClientIP=@strClientIP AND CollectDate >= DATEADD(hh,-24,GETDATE()) AND ShareRewardCount > 0
	SELECT @ShareIPCount=Count(*) FROM RecordAppShare WHERE ClientIP=@strClientIP AND DateID = @DateID AND ShareRewardCount > 0
	DECLARE @ShareDeviceCount INT
	SELECT @ShareDeviceCount=Count(*) FROM RecordAppShare WHERE Machine=@strMachineID AND DateID = @DateID AND ShareRewardCount > 0

	-- 金币判断
	IF @ShareCount IS NULL OR @ShareCount>=@ShareLimitCount OR @ShareIPCount IS NULL OR @ShareIPCount>=@ShareIPLimitCount OR @ShareDeviceCount IS NULL OR @ShareDeviceCount > @ShareDeviceLimitCount
	BEGIN
		-- 不返回错误信息,依然继续
		SET @ShareRewardCount=0
--		SET @strErrorDescribe=N'您最近24小时内的分享次数超过了' + LTRIM(STR(@ShareLimitCount)) + '次,无法获得奖励,请稍后再重新尝试.'
	END
	
	-- 判断是否是微信登录(10002用户)
	IF (NOT EXISTS (SELECT * FROM QPAccountsDB.dbo.AccountsThirdParty(NOLOCK) WHERE UserID=@dwUserID AND ThirdPartyID=10002) )
	AND (NOT EXISTS (SELECT * FROM QPAccountsDB.dbo.IndividualDatum(NOLOCK) WHERE UserID=@dwUserID AND MobilePhone IS NOT NULL AND MobilePhone<>N'') )
	BEGIN
		SET @ShareRewardCount=0
	END
	
	-- 开始事务
	SET TRANSACTION ISOLATION LEVEL REPEATABLE READ
	BEGIN TRAN

		-- 更新数据
		IF @ShareRewardCount=0
		BEGIN
			SET @strErrorDescribe=N'分享成功!(绑定手机或微信登录账号分享可获银子)'
			-- 依然记录数据库
--			INSERT INTO RecordAppShare(UserID,ClientIP,CollectDate,ShareRewardCount) VALUES(@UserID,@strClientIP,GetDate(),@ShareRewardCount)
			INSERT INTO RecordAppShare(UserID,DateID,ClientIP,CollectDate,ShareRewardCount,Machine) VALUES(@UserID,@DateID,@strClientIP,GetDate(),@ShareRewardCount,@strMachineID)
		END
		ELSE
		BEGIN
			UPDATE GameScoreInfo SET InsureScore=InsureScore+@ShareRewardCount WHERE UserID=@dwUserID
			
			IF @@ERROR=0
			BEGIN
				SET @strErrorDescribe=N'分享成功,恭喜您获得了' + LTRIM(STR(@ShareRewardCount)) + '银子,奖励已发放至您的保险箱'
				-- 记录日志
				INSERT INTO RecordAppShare(UserID,DateID,ClientIP,CollectDate,ShareRewardCount,Machine) VALUES(@UserID,@DateID,@strClientIP,GetDate(),@ShareRewardCount,@strMachineID)
			END
			ELSE
			BEGIN
				-- 结束事务
				ROLLBACK TRAN
				SET TRANSACTION ISOLATION LEVEL READ COMMITTED
				SET @strErrorDescribe=N'更新玩家分享奖励失败.'
				return 5
			END
		END
				
	-- 结束事务
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED

	-- 输出结果
--	SELECT @dwUserID AS UserID, @SourceScore AS SourceScore, @SourceInsure AS SourceInsure, @VariationScore AS VariationScore,
--		@VariationInsure AS VariationInsure, @InsureRevenue AS InsureRevenue, @LockKindID AS KindID, @LockServerID AS ServerID

END

RETURN 0

GO
