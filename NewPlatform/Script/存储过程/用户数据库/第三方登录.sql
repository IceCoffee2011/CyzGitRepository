----------------------------------------------------------------------------------------------------
USE QPAccountsDB
GO

-- 第三方登录
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyAccountsThirdParty]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyAccountsThirdParty]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO
----------------------------------------------------------------------------------------------------

-- 帐号登录
CREATE PROC GSP_MB_EfficacyAccountsThirdParty
	@strSessionID NVARCHAR(64),					-- 第三方SessionID
	@strNickName NVARCHAR(31),					-- 用户昵称
	@dwThirdPartyID INT,						-- 第三方平台ID
	@wFaceID SMALLINT,							-- 头像标识
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strClientIP NVARCHAR(15),					-- 连接地址
	@wPlatformID INT,						-- 登录设备类型
	@dwAreaID	INT,							-- 区域ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
-- 昵称长度
DECLARE @NickNameLen TINYINT
-- SessionID长度
DECLARE @SessionIDLen TINYINT
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @PlayTimeCount INT

-- 积分变量
DECLARE @Score BIGINT
DECLARE @Insure BIGINT

-- 扩展信息
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME
DECLARE @FillIn	TINYINT
DECLARE @Note NVARCHAR(512)

-- 记录日志
DECLARE @DateID INT

-- 辅助变量
DECLARE @EnjoinLogon AS INT

-- 执行逻辑
BEGIN
	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的登录功能，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的登录功能，请联系客户服务中心了解详细情况！'
		RETURN 7
	END
	
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @MoorMachine AS TINYINT

	-- 查询用户
	SELECT @UserID=UserID FROM AccountsThirdParty(NOLOCK) WHERE SessionID=@strSessionID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		-- 账号不存在,生成新的账号
		
		-- 先检查以SessionID做账号的,如果已经被注册就返回,说明程序可能出现了问题
		IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE Accounts=@strSessionID)
		BEGIN
			SET @strErrorDescribe=N'此SessionID帐号已被注册，请换另一帐号尝试再次注册！'
			RETURN 1
		END
		
		-- 创建账号
		-- 注册用户
		INSERT AccountsInfo (Accounts,RegAccounts,AreaID,LogonPass,InsurePass,Gender,FaceID,GameLogonTimes,LastLogonIP,LastLogonMobile,LastLogonMachine,RegisterIP,RegisterMobile,RegisterMachine)
		VALUES (@strSessionID,@strSessionID,@dwAreaID,N'',N'E10ADC3949BA59ABBE56E057F20F883E',1,@wFaceID,1,@strClientIP,N'',@strMachineID,@strClientIP,N'',@strMachineID)
		
		-- 错误判断
		IF @@ERROR<>0
		BEGIN
			SET @strErrorDescribe=N'由于意外的原因，帐号注册失败，请尝试再次注册！'
			RETURN 2
		END

		-- 查询用户
		SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @UnderWrite=UnderWrite, @FaceID=FaceID,
			@CustomID=CustomID, @Gender=Gender, @UserMedal=UserMedal, @Experience=Experience, @LoveLiness=LoveLiness, @MemberOrder=MemberOrder,
			@MemberOverDate=MemberOverDate, @MoorMachine=MoorMachine, @FillIn = FillIn
		FROM AccountsInfo(NOLOCK) WHERE Accounts=@strSessionID

		-- 分配标识
		SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
		IF @GameID IS NULL 
		BEGIN
			SET @GameID=0
			SET @strErrorDescribe=N'用户注册成功，但未成功获取游戏 ID 号码，系统稍后将给您分配！'
		END
		ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID
	
--		-- 分配昵称
--		SET @NickName = N'手机用户' + LTRIM(STR(@GameID))
--		-- 更新昵称,先检查要分配的昵称是否被占用,没被占用可以更新
--		IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@NickName)
--		BEGIN
--			-- 昵称被占用
--			SET @NickName = N'手机用户' + LTRIM(STR(@GameID)) + N'&' + LTRIM(STR(@UserID))
--		END
--		-- 更新昵称
--		UPDATE AccountsInfo SET NickName=@NickName, FillIn=1 WHERE UserID=@UserID
		
		-- 昵称为原昵称+SessionID截取的字符串
		DECLARE @TempNickName NVARCHAR(128)
		SET @TempNickName = RTRIM(@strNickName) + RTRIM(@GameID)
		-- 截取字符串
		SET @TempNickName = RTRIM(LEFT(@TempNickName, 15))
		-- 更新昵称,先检查要分配的昵称是否被占用,没被占用可以更新-		
		IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@TempNickName)
		BEGIN
			SET @TempNickName = N'手机用户' + LTRIM(STR(@GameID)) + N'&' + LTRIM(STR(@UserID))
		END
		-- 更新昵称
		UPDATE AccountsInfo SET NickName=@TempNickName, FillIn=1 WHERE UserID=@UserID
		
		-- 插入映射表
		INSERT AccountsThirdParty (SessionID,UserID,ThirdPartyID) VALUES (@strSessionID,@UserID,@dwThirdPartyID)
	
		-- 推广提成
		IF @SpreaderID<>0
		BEGIN
			DECLARE @RegisterGrantScore INT
--			DECLARE @Note NVARCHAR(512)
			SET @Note = N'注册'
			SELECT @RegisterGrantScore = RegisterGrantScore FROM QPTreasureDB.dbo.GlobalSpreadInfo
			IF @RegisterGrantScore IS NULL
			BEGIN
				SET @RegisterGrantScore=5000
			END
			INSERT INTO QPTreasureDB.dbo.RecordSpreadInfo(
				UserID,Score,TypeID,ChildrenID,CollectNote)
			VALUES(@SpreaderID,@RegisterGrantScore,1,@UserID,@Note)		
		END

		-- 记录日志
--		DECLARE @DateID INT
		SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
		UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameRegisterSuccess) VALUES (@DateID, 1)

		----------------------------------------------------------------------------------------------------
		----------------------------------------------------------------------------------------------------
		-- 注册赠送

		-- 读取变量
		DECLARE @GrantIPCount AS BIGINT
		DECLARE @GrantScoreCount AS BIGINT
		DECLARE @GrantMachineCount AS BIGINT
		SELECT @GrantIPCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantIPCount' AND AreaID=@dwAreaID
		SELECT @GrantScoreCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantScoreCount' AND AreaID=@dwAreaID
		SELECT @GrantMachineCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantMachineCount' AND AreaID=@dwAreaID
		
		-- QQ注册
		DECLARE @QQGrantScoreCount AS BIGINT
		IF @dwThirdPartyID=10001
		BEGIN
			SELECT @QQGrantScoreCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'QQGrantScoreCount' AND AreaID=@dwAreaID
			IF @QQGrantScoreCount IS NULL SET @QQGrantScoreCount=0
			SET @GrantScoreCount = @QQGrantScoreCount
		END
		
		-- 微信注册
		DECLARE @WXGrantScoreCount AS BIGINT
		IF @dwThirdPartyID=10002
		BEGIN
			SELECT @WXGrantScoreCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'WXGrantScoreCount' AND AreaID=@dwAreaID
			IF @WXGrantScoreCount IS NULL SET @WXGrantScoreCount=0
			SET @GrantScoreCount = @WXGrantScoreCount
		END
		
		IF @GrantScoreCount IS NULL OR @GrantScoreCount < 0
		BEGIN
			SET @GrantScoreCount = 0
		END
		IF @GrantIPCount IS NULL OR @GrantIPCount < 0
		BEGIN
			SET @GrantIPCount = 1
		END
		IF @GrantMachineCount IS NULL OR @GrantMachineCount < 0
		BEGIN
			SET @GrantMachineCount = 1
		END

		-- 赠送限制
		IF @GrantScoreCount IS NOT NULL AND @GrantScoreCount>0 AND @GrantIPCount IS NOT NULL AND @GrantIPCount>0 AND @GrantMachineCount IS NOT NULL AND @GrantMachineCount>0
		BEGIN
			-- 赠送次数
			DECLARE @IPCount AS BIGINT
			DECLARE @MachineCount AS BIGINT
			SELECT @IPCount=SUM(GrantCount) FROM SystemGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterIP=@strClientIP
			SELECT @MachineCount=SUM(GrantCount) FROM SystemGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterMachine=@strMachineID
		
			-- 次数判断
			IF (@IPCount IS NOT NULL AND @IPCount>=@GrantIPCount) OR ( @MachineCount IS NOT NULL AND @MachineCount>=@GrantMachineCount )
			BEGIN
				SET @GrantScoreCount=0
			END
		END

		-- 赠送金币
		IF @GrantScoreCount IS NOT NULL
		BEGIN
			-- 更新记录
			UPDATE SystemGrantCount SET GrantScore=GrantScore+@GrantScoreCount, GrantCount=GrantCount+1 WHERE DateID=@DateID AND RegisterIP=@strClientIP AND RegisterMachine=@strMachineID

			-- 插入记录
			IF @@ROWCOUNT=0
			BEGIN
				INSERT SystemGrantCount (DateID, RegisterIP, RegisterMachine, GrantScore, GrantCount) VALUES (@DateID, @strClientIP, @strMachineID, @GrantScoreCount, 1)
			END

			-- 赠送金币
			INSERT QPTreasureDB.dbo.GameScoreInfo (UserID, Score, RegisterIP, LastLogonIP) VALUES (@UserID, @GrantScoreCount, @strClientIP, @strClientIP) 		
		END
	END	

	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @UserMedal=UserMedal, @Experience=Experience,
		@LoveLiness=LoveLiness, @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount, @FillIn = FillIn
	FROM AccountsInfo(NOLOCK) WHERE UserID=@UserID
	
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
	IF @MoorMachine <> 0
	BEGIN
		SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您不能使用手机终端进行登录！'
		RETURN 1
	END

	-- 密码判断
--	IF @LogonPass<>@strPassword
--	BEGIN
--		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
--		RETURN 3
--	END

	-- 推广员提成
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
--		DECLARE @Note NVARCHAR(512)
		SET @Note = N'游戏时长达标一次性奖励'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM QPTreasureDB.dbo.GlobalSpreadInfo
		WHERE ID=1
		IF @GrantTime IS NULL OR @GrantTime=0
		BEGIN
			SET @GrantTime = 108000 -- 30小时
			SET @GrantScore = 200000
		END			
		IF @PlayTimeCount>=@GrantTime
		BEGIN
			-- 获取提成信息
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM QPTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO QPTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- 查询金币
	SELECT @Score=Score, @Insure=InsureScore FROM QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID

	-- 数据调整
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0

	-- 会员等级
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- 删除会员
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- 搜索会员
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- 数据调整
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- 更新数据
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	-- 更新信息
	UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,
		LastLogonMachine=@strMachineID, LastLogonMobile=N'' WHERE UserID=@UserID

	-- 记录日志
--	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)
	
	INSERT INTO RecordUserLogon(UserID,EnterTime,EnterMachine,EnterClientIP,ClientKind) VALUES(@UserID,GETDATE(),@strMachineID,@strClientIP,@wPlatformID)	
	
	-- 查询玩家的钻石道具数量
	DECLARE @wUserDiamondCount INT
	SELECT @wUserDiamondCount=PropertyCount FROM QPUserCustomDB.dbo.UserProperty WHERE UserID=@UserID
	IF @wUserDiamondCount IS NULL
	BEGIN
		DECLARE @GrantDiamondCount AS INT
		SELECT @GrantDiamondCount=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'GrantDiamondCount' AND AreaID=@dwAreaID
		SET @wUserDiamondCount = @GrantDiamondCount
		IF @wUserDiamondCount IS NULL
		BEGIN
			SET @wUserDiamondCount = 0
		END
		INSERT QPUserCustomDB.dbo.UserProperty(UserID, PropertyCount) VALUES(@UserID,@wUserDiamondCount)
	END
	
	-- 查找玩家是否创建/加入过自定义桌子
	DECLARE @wServerID INT
	DECLARE @wTableID INT
	DECLARE @strVerifyCode		NVARCHAR(8)			-- 查询到的验证码
	DECLARE @wLockedInTable TINYINT
	SET @wServerID = 0
	SET @strVerifyCode = N''
	SET @wLockedInTable = 0
	
	IF EXISTS (SELECT * FROM QPUserCustomDB.dbo.UserCreateTable WHERE UserID=@UserID)
	BEGIN
		SELECT @wServerID=ServerID, @strVerifyCode=Secret FROM QPUserCustomDB.dbo.UserCreateTable WHERE UserID=@UserID
		SET @wLockedInTable = 1
	END
	ELSE IF EXISTS (SELECT * FROM QPUserCustomDB.dbo.UserInTable WHERE UserID=@UserID)
	BEGIN
		SELECT @wServerID=ServerID, @wTableID=TableID FROM QPUserCustomDB.dbo.UserInTable WHERE UserID=@UserID
		SELECT @strVerifyCode=Secret FROM QPUserCustomDB.dbo.UserCreateTable WHERE ServerID=@wServerID AND TableID=@wTableID
		SET @wLockedInTable = 1
	END
	
	-- 查询代理信息
	DECLARE @strAgentWeiXin NVARCHAR(64)
	select @strAgentWeiXin = (select top 1 WeiXinID FROM QPUserCustomDB.dbo.AgentUser ORDER BY NEWID())
	-- 字符串拼接
	DECLARE @strAgentInfo NVARCHAR(128)
	SET @strAgentInfo = N'购买钻石请联系所在群的群主或以下微信号: '+ char(10) +LTRIM(@strAgentWeiXin)

	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName, @UnderWrite AS UnderWrite,
		@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, @UserMedal AS UserMedal, @Experience AS Experience,
		@Score AS Score, @Insure AS Insure, @LoveLiness AS LoveLiness, @MemberOrder AS MemberOrder, @MemberOverDate AS MemberOverDate,
		@MoorMachine AS MoorMachine, @FillIn as FillIn, @dwAreaID As AreaID, @wPlatformID As PlatformID, @wServerID AS ServerID,
		@strVerifyCode AS Secret, @wLockedInTable AS LockedInTable, @wUserDiamondCount As UserDiamondCount, @strAgentInfo As AgentInfo
END

RETURN 0

GO
