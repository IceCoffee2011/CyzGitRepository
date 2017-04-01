
----------------------------------------------------------------------------------------------------

USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_EfficacyAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_EfficacyAccounts]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyAccounts]
GO

--IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QuerySalt]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
--DROP PROCEDURE [dbo].[GSP_GP_QuerySalt]
--GO

--IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QuerySaltByUserID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
--DROP PROCEDURE [dbo].[GSP_GP_QuerySaltByUserID]
--GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 查询账号Salt

--CREATE PROC GSP_GP_QuerySalt
--	@strAccounts NVARCHAR(31),					-- 用户帐号
--	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
--WITH ENCRYPTION AS

-- 基本信息

-- 扩展信息
--DECLARE @Salt NVARCHAR(6)

--BEGIN
	-- 查询用户
--	SELECT @Salt = Salt
--	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts
	
--	SELECT @Salt As Salt
--END

--RETURN 0
--GO
----------------------------------------------------------------------------------------------------

-- 查询账号Salt

--CREATE PROC GSP_GP_QuerySaltByUserID
--	@strUserID NVARCHAR(31),					-- 用户帐号
--	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
--WITH ENCRYPTION AS

-- 基本信息

-- 扩展信息
--DECLARE @Salt NVARCHAR(6)

--BEGIN
	-- 查询用户
--	SELECT @Salt = Salt
--	FROM AccountsInfo(NOLOCK) WHERE UserID=@strUserID
	
--	SELECT @Salt As Salt
--END

--RETURN 0
--GO

----------------------------------------------------------------------------------------------------

-- 帐号登录
CREATE PROC GSP_GP_EfficacyAccounts
	@strAccounts NVARCHAR(31),					-- 用户帐号
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@nNeeValidateMBCard BIT,					-- 密保校验
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
DECLARE @ProtectID INT
DECLARE @PasswordID INT
DECLARE @FillIn	TINYINT

-- 辅助变量
DECLARE @EnjoinLogon AS INT

-- 输出信息
--DECLARE	@strErrorDescribe NVARCHAR(127)
--DECLARE @RETURN_VALUE	INT

-- 执行逻辑
BEGIN
	-- 系统暂停
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
--		SET @RETURN_VALUE = 2
--		SELECT @strErrorDescribe As strErrorDescribe, @RETURN_VALUE As RETURN_VALUE
		RETURN 2
	END

	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的登录功能，请联系客户服务中心了解详细情况！'
--		SELECT @strErrorDescribe As strErrorDescribe
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的登录功能，请联系客户服务中心了解详细情况！'
		RETURN 7
	END
 
	-- 查询用户
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE	@MachineSerial NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @UserMedal=UserMedal, @Experience=Experience,
		@LoveLiness=LoveLiness, @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine, @MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@PasswordID=PasswordID,@ProtectID=ProtectID,
		@FillIn = FillIn
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

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
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
			RETURN 1
		END
	END
	
	-- 密码不能为空
	IF @strPassword=N''
	BEGIN
		SET @strErrorDescribe=N'密码不能为空！'
		RETURN 3
	END

	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 3
	END

	-- 密保校验
	IF @nNeeValidateMBCard=1 AND @PasswordID<>0
	BEGIN
		SELECT @PasswordID AS PasswordID
		RETURN 18
	END

	-- 固定机器
	IF @MoorMachine=2
	BEGIN
		SET @MoorMachine=1
		SET @strErrorDescribe=N'您的帐号成功使用了固定机器登录功能！'
		UPDATE AccountsInfo SET MoorMachine=@MoorMachine, LastLogonMachine=@strMachineID WHERE UserID=@UserID
	END

	-- 推广员提成
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
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
	UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1,LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,
		LastLogonMachine=@strMachineID WHERE UserID=@UserID

	-- 记录日志
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName, @UnderWrite AS UnderWrite,
		@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, @UserMedal AS UserMedal, @Experience AS Experience,
		@Score AS Score, @Insure AS Insure, @LoveLiness AS LoveLiness, @MemberOrder AS MemberOrder, @MemberOverDate AS MemberOverDate,
		@MoorMachine AS MoorMachine,@PasswordID as PasswordID, @FillIn as FillIn
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 帐号登录
CREATE PROC GSP_MB_EfficacyAccounts
	@strAccounts NVARCHAR(31),					-- 用户帐号
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NVARCHAR(32),					-- 机器标识
	@strMobilePhone NVARCHAR(11),				-- 手机号码
	@wPlatformID INT,							-- 登录设备类型
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
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @PlayTimeCount INT
DECLARE	@AreaID	INT

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

	-- 查询用户
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @UserMedal=UserMedal, @Experience=Experience,
		@LoveLiness=LoveLiness, @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount, @FillIn = FillIn, @AreaID = AreaID
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

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
	IF @MoorMachine <> 0
	BEGIN
		SET @strErrorDescribe=N'您的帐号使用固定机器登录功能，您不能使用手机终端进行登录！'
		RETURN 1
	END
	
	-- 密码不能为空
	IF @strPassword=N''
	BEGIN
		SET @strErrorDescribe=N'密码不能为空！'
		RETURN 3
	END

	-- 密码判断
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 3
	END

	-- 推广员提成
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
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
		LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone WHERE UserID=@UserID

	-- 记录日志
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)
	
	INSERT INTO RecordUserLogon(UserID,EnterTime,EnterMachine,EnterClientIP,ClientKind) VALUES(@UserID,GETDATE(),@strMachineID,@strClientIP,@wPlatformID)

	-- 查询玩家的钻石道具数量
	DECLARE @wUserDiamondCount INT
	SELECT @wUserDiamondCount=PropertyCount FROM QPUserCustomDB.dbo.UserProperty WHERE UserID=@UserID
	IF @wUserDiamondCount IS NULL
	BEGIN
		SET @wUserDiamondCount = 0
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
		@MoorMachine AS MoorMachine, @FillIn as FillIn, @AreaID As AreaID, @wPlatformID As PlatformID, 
		@wServerID AS ServerID, @strVerifyCode AS Secret, @wLockedInTable AS LockedInTable, @wUserDiamondCount As UserDiamondCount, @strAgentInfo As AgentInfo

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
