
----------------------------------------------------------------------------------------------------

USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RegisterAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RegisterAccounts]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_CheckNick]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_CheckNick]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RegisterProfile]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RegisterProfile]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_RegisterAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_RegisterAccounts]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 检查昵称
CREATE PROC [dbo].[GSP_GP_CheckNick]
	@dwUserID INT,								-- 用户 I D	
	@strNickName NVARCHAR(31),					-- 用户昵称
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 效验昵称
	IF (SELECT COUNT(*) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strNickName)>0)>0
	BEGIN
		SET @strErrorDescribe=N'您所输入的游戏昵称名含有限制字符串，请更换昵称！'
		RETURN 4
	END

	-- 存在判断
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName)
	BEGIN
		SET @strErrorDescribe=N'该昵称已被注册，请换另一用户名尝试再次注册！'
		RETURN 4
	END

	SET @strErrorDescribe=N'此昵称可用'
	RETURN 0

END

return 0

GO
----------------------------------------------------------------------------------------------------

-- 帐号注册
CREATE PROC [dbo].[GSP_GP_RegisterProfile]
	@dwUserID INT,									-- 用户 I D
	@wFaceID SMALLINT,								-- 头像标识
	@strPassword NCHAR(32),							-- 用户密码
	@strNickName NVARCHAR(31),						-- 用户昵称
	@cbGender TINYINT,								-- 用户性别
	@strMobilePhone NVARCHAR(16),					-- 移动电话
	@strQQ NVARCHAR(16),							-- Q Q 号码
	@strUnderWrite NVARCHAR(31),					-- 个性签名
	@strPassPortID NVARCHAR(18),					-- 身份证号
	@strCompellation NVARCHAR(16),					-- 真实名字
	@strSecurityQuestion NVARCHAR(32),				-- 密保问题
	@strSecurityAnswer NVARCHAR(32),				-- 密保回答
	@strClientIP NVARCHAR(15),						-- 连接地址
	@strErrorDescribe	NVARCHAR(127)	OUTPUT		-- 输出信息
WITH ENCRYPTION AS
--AS

-- 属性设置
SET NOCOUNT ON

-- 变量定义
DECLARE @UserID INT
DECLARE @LogonPass AS NCHAR(32)
DECLARE @NickName NVARCHAR(31)

-- 执行逻辑
BEGIN
	-- 查询用户
	SELECT @UserID=UserID, @NickName=NickName, @LogonPass=LogonPass FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- 效验昵称
	IF (SELECT COUNT(*) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strNickName)>0)>0
	BEGIN
		SET @strErrorDescribe=N'您所输入的游戏昵称名含有限制字符串，请更换昵称名后再次修改！'
		RETURN 4
	END

	-- 查询昵称
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName)
	BEGIN
		SET @strErrorDescribe=N'此昵称已被注册，请换另一昵称尝试再次注册！'
		RETURN 7
	END
	
	-- 修改资料
	UPDATE AccountsInfo SET NickName=@strNickName, UnderWrite=@strUnderWrite, Gender=@cbGender, FillIn = 1,
	PassPortID=@strPassPortID, Compellation=@strCompellation, FaceID=@wFaceID
	WHERE UserID=@dwUserID
		
	-- 修改资料 
	UPDATE IndividualDatum SET QQ=@strQQ, MobilePhone=@strMobilePhone WHERE UserID=@dwUserID
	
	-- 修改昵称记录
	IF @NickName<>@strNickName
	BEGIN
		INSERT INTO QPRecordDB.dbo.RecordAccountsExpend(UserID,ReAccounts)
		VALUES(@dwUserID,@strNickName)
	END

	-- 修改资料
	UPDATE IndividualDatum SET QQ=@strQQ, MobilePhone=@strMobilePhone WHERE UserID=@dwUserID
		
	-- 修改资料
	IF @@ROWCOUNT=0
	INSERT IndividualDatum (UserID, QQ, MobilePhone) VALUES (@dwUserID, @strQQ, @strMobilePhone)
	
	-- 插入账号保护表中
	INSERT INTO AccountsProtect(UserID,Question1,Response1,PassportID,CreateIP)
	VALUES(@dwUserID,@strSecurityQuestion,@strSecurityAnswer,@strPassPortID,@strClientIP)
	IF @@ERROR<>0
	BEGIN
		SET @strErrorDescribe=N'更新玩家密保信息失败'
		RETURN 8
	END
	
	-- 设置信息
	IF @@ERROR=0 SET @strErrorDescribe=N'服务器已经接受了您的新资料！'
	
	return 0

END

return 0

GO
----------------------------------------------------------------------------------------------------

-- 帐号注册
CREATE PROC GSP_GP_RegisterAccounts
	@strAccounts NVARCHAR(31),					-- 用户帐号
	@strNickName NVARCHAR(31),					-- 用户昵称
	@strSpreader NVARCHAR(31),					-- 推荐帐号
	@strLogonPass NCHAR(32),					-- 登录密码
	@strInsurePass NCHAR(32),					-- 银行密码
	@wFaceID SMALLINT,							-- 头像标识
	@cbGender TINYINT,							-- 用户性别
	@strPassPortID NVARCHAR(18),				-- 身份证号
	@strCompellation NVARCHAR(16),				-- 真实名字
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NCHAR(32),					-- 机器标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @Gender TINYINT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @MoorMachine TINYINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)

-- 积分变量
DECLARE @Score BIGINT
DECLARE @Insure BIGINT

-- 附加信息
DECLARE @GameID INT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @SpreaderID INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @FillIn	TINYINT

-- 辅助变量
DECLARE @EnjoinLogon AS INT
DECLARE @EnjoinRegister AS INT

-- 执行逻辑
BEGIN
	SET @strErrorDescribe=N'抱歉地通知您，PC版暂停注册功能,请使用手机版登录!'
	RETURN 3

	-- 注册暂停
	SELECT @EnjoinRegister=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinRegister'
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinRegister'
		RETURN 1
	END

	-- 登录暂停
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- 效验名字
	IF EXISTS (SELECT [String] FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strAccounts)>0 AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL))
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，您所输入的登录帐号名含有限制字符串，请更换帐号名后再次申请帐号！'
		RETURN 4
	END

	-- 效验昵称
	IF @strNickname IS NOT NULL AND @strNickname<>''
	BEGIN
		IF EXISTS (SELECT [String] FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strNickname)>0 AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL))
		BEGIN
			SET @strErrorDescribe=N'抱歉地通知您，您所输入的游戏昵称名含有限制字符串，请更换昵称名后再次申请帐号！'
			RETURN 4
		END
	END

	-- 效验地址
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的注册功能，请联系客户服务中心了解详细情况！'
		RETURN 5
	END
	
	-- 效验机器
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的注册功能，请联系客户服务中心了解详细情况！'
		RETURN 6
	END
 
	-- 查询帐号
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts)
	BEGIN
		SET @strErrorDescribe=N'此帐号已被注册，请换另一帐号尝试再次注册！'
		RETURN 7
	END

	-- 查询昵称
	IF @strNickname IS NOT NULL AND @strNickname<>''
	BEGIN
		IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName)
		BEGIN
			SET @strErrorDescribe=N'此昵称已被注册，请换另一昵称尝试再次注册！'
			RETURN 7
		END
	END

	-- 查推广员
	IF @strSpreader<>''
	BEGIN
		-- 查推广员
		SELECT @SpreaderID=UserID FROM AccountsInfo(NOLOCK) WHERE Accounts=@strSpreader

		-- 结果处理
		IF @SpreaderID IS NULL
		BEGIN
			SET @strErrorDescribe=N'您所填写的推荐人不存在或者填写错误，请检查后再次注册！'
			RETURN 8
		END
	END
	ELSE SET @SpreaderID=0

	-- 注册用户
	INSERT AccountsInfo (Accounts,NickName,RegAccounts,LogonPass,InsurePass,SpreaderID,PassPortID,Compellation,Gender,FaceID,
		GameLogonTimes,LastLogonIP,LastLogonMachine,RegisterIP,RegisterMachine)
	VALUES (@strAccounts,@strNickName,@strAccounts,@strLogonPass,@strInsurePass,@SpreaderID,@strPassPortID,@strCompellation,@cbGender,@wFaceID,
		1,@strClientIP,@strMachineID,@strClientIP,@strMachineID)

	-- 错误判断
	IF @@ERROR<>0
	BEGIN
		SET @strErrorDescribe=N'帐号已存在，请换另一帐号名字尝试再次注册！'
		RETURN 8
	END

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @UnderWrite=UnderWrite, @FaceID=FaceID,
		@CustomID=CustomID, @Gender=Gender, @UserMedal=UserMedal, @Experience=Experience, @LoveLiness=LoveLiness, @MemberOrder=MemberOrder,
		@MemberOverDate=MemberOverDate, @MoorMachine=MoorMachine,@FillIn=FillIn
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

	-- 分配标识
	SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
	IF @GameID IS NULL 
	BEGIN
		SET @GameID=0
		SET @strErrorDescribe=N'用户注册成功，但未成功获取游戏 ID 号码，系统稍后将给您分配！'
	END
	ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID

	-- 推广提成
	IF @SpreaderID<>0
	BEGIN
		DECLARE @RegisterGrantScore INT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'注册'
--		SELECT @RegisterGrantScore = RegisterGrantScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GlobalSpreadInfo
		SELECT @RegisterGrantScore = RegisterGrantScore FROM QPTreasureDB.dbo.GlobalSpreadInfo
		IF @RegisterGrantScore IS NULL
		BEGIN
			SET @RegisterGrantScore=5000
		END
--		INSERT INTO QPTreasureDBLink.QPTreasureDB.dbo.RecordSpreadInfo(
--			UserID,Score,TypeID,ChildrenID,CollectNote)
		INSERT INTO QPTreasureDB.dbo.RecordSpreadInfo(
			UserID,Score,TypeID,ChildrenID,CollectNote)
		VALUES(@SpreaderID,@RegisterGrantScore,1,@UserID,@Note)		
	END

	-- 记录日志
	DECLARE @DateID INT
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
	SELECT @GrantIPCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantIPCount'
	SELECT @GrantScoreCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantScoreCount'
	SELECT @GrantMachineCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantMachineCount'
	
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

	-- 赠送金币(只要赠送数目不为空,就在GameScoreInfo表中插入数据)
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
--		INSERT QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo (UserID, Score, RegisterIP, LastLogonIP) VALUES (@UserID, @GrantScoreCount, @strClientIP, @strClientIP) 
		INSERT QPTreasureDB.dbo.GameScoreInfo (UserID, Score, RegisterIP, LastLogonIP) VALUES (@UserID, @GrantScoreCount, @strClientIP, @strClientIP)
	END

	----------------------------------------------------------------------------------------------------
	----------------------------------------------------------------------------------------------------

	-- 查询金币
--	SELECT @Score=Score, @Insure=InsureScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID
	SELECT @Score=Score, @Insure=InsureScore FROM QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID

	-- 数据调整
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0

	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName, @UnderWrite AS UnderWrite,
		@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, @UserMedal AS UserMedal, @Experience AS Experience,
		@Score AS Score, @Insure AS Insure, @LoveLiness AS LoveLiness, @MemberOrder AS MemberOrder, @MemberOverDate AS MemberOverDate,
		@MoorMachine AS MoorMachine, @FillIn AS FillIn

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 帐号注册
CREATE PROC GSP_MB_RegisterAccounts
	@strAccounts NVARCHAR(31),					-- 用户帐号
	@strNickName NVARCHAR(31),					-- 用户昵称
	@strLogonPass NCHAR(32),					-- 登录密码
	@strInsurePass NCHAR(32),					-- 银行密码
	@wFaceID SMALLINT,							-- 头像标识
	@cbGender TINYINT,							-- 用户性别
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineID NCHAR(32),					-- 机器标识
	@strMobilePhone NVARCHAR(11),				-- 手机号码
	@wPlatformID INT,							-- 平台编号
	@dwAreaID INT,								-- 区域ID
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @Gender TINYINT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @MoorMachine TINYINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)

-- 积分变量
DECLARE @Score BIGINT
DECLARE @Insure BIGINT

-- 扩展信息
DECLARE @GameID INT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @SpreaderID INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @FillIn	TINYINT

-- 辅助变量
DECLARE @EnjoinLogon AS INT
DECLARE @EnjoinRegister AS INT

-- 执行逻辑
BEGIN
	-- 注册暂停
	SELECT @EnjoinRegister=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinRegister'
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinRegister'
		RETURN 1
	END

	-- 登录暂停
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- 效验名字
	IF (SELECT COUNT(*) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strAccounts)>0)>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，您所输入的登录帐号名含有限制字符串，请更换帐号名后再次申请帐号！'
		RETURN 4
	END

	-- 效验昵称
	IF (SELECT COUNT(*) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strNickName)>0)>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，您所输入的游戏昵称名含有限制字符串，请更换昵称名后再次申请帐号！'
		RETURN 4
	END

	-- 效验地址
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您所在的 IP 地址的注册功能，请联系客户服务中心了解详细情况！'
		RETURN 5
	END
	
	-- 效验机器
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'抱歉地通知您，系统禁止了您的机器的注册功能，请联系客户服务中心了解详细情况！'
		RETURN 6
	END

	-- 查询帐号
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts)
	BEGIN
		SET @strErrorDescribe=N'此帐号已被注册，请换另一帐号尝试再次注册！'
		RETURN 7
	END

	-- 查询昵称
	--IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName)
	--BEGIN
	--	SET @strErrorDescribe=N'此昵称已被注册，请换另一昵称尝试再次注册！'
	--	RETURN 7
	--END

	-- 注册用户
	INSERT AccountsInfo (Accounts,RegAccounts,AreaID,LogonPass,InsurePass,Gender,FaceID,GameLogonTimes,LastLogonIP,LastLogonMobile,LastLogonMachine,RegisterIP,RegisterMobile,RegisterMachine)
	VALUES (@strAccounts,@strAccounts,@dwAreaID,@strLogonPass,'E10ADC3949BA59ABBE56E057F20F883E',@cbGender,@wFaceID,1,@strClientIP,@strMobilePhone,@strMachineID,@strClientIP,@strMobilePhone,@strMachineID)

	-- 错误判断
	IF @@ERROR<>0
	BEGIN
		SET @strErrorDescribe=N'由于意外的原因，帐号注册失败，请尝试再次注册！'
		RETURN 8
	END

	-- 查询用户
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @UnderWrite=UnderWrite, @FaceID=FaceID,
		@CustomID=CustomID, @Gender=Gender, @UserMedal=UserMedal, @Experience=Experience, @LoveLiness=LoveLiness, @MemberOrder=MemberOrder,
		@MemberOverDate=MemberOverDate, @MoorMachine=MoorMachine, @FillIn = FillIn
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

	-- 分配标识
	SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
	IF @GameID IS NULL 
	BEGIN
		SET @GameID=0
		SET @strErrorDescribe=N'用户注册成功，但未成功获取游戏 ID 号码，系统稍后将给您分配！'
	END
	ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID
	
	-- 分配昵称
	SET @NickName = N'手机用户' + LTRIM(STR(@GameID))
	-- 更新昵称,先检查要分配的昵称是否被占用,没被占用可以更新
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@NickName)
	BEGIN
		-- 昵称被占用
		SET @NickName = N'手机用户' + LTRIM(STR(@GameID)) + N'&' + LTRIM(STR(@UserID))
	END
	-- 更新昵称
	UPDATE AccountsInfo SET NickName=@NickName, FillIn=1 WHERE UserID=@UserID
	
	-- 推广提成
	IF @SpreaderID<>0
	BEGIN
		DECLARE @RegisterGrantScore INT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'注册'
--		SELECT @RegisterGrantScore = RegisterGrantScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GlobalSpreadInfo
		SELECT @RegisterGrantScore = RegisterGrantScore FROM QPTreasureDB.dbo.GlobalSpreadInfo
		IF @RegisterGrantScore IS NULL
		BEGIN
			SET @RegisterGrantScore=5000
		END
--		INSERT INTO QPTreasureDBLink.QPTreasureDB.dbo.RecordSpreadInfo(
--			UserID,Score,TypeID,ChildrenID,CollectNote)
		INSERT INTO QPTreasureDB.dbo.RecordSpreadInfo(
			UserID,Score,TypeID,ChildrenID,CollectNote)
		VALUES(@SpreaderID,@RegisterGrantScore,1,@UserID,@Note)		
	END

	-- 记录日志
	DECLARE @DateID INT
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
--		INSERT QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo (UserID, Score, RegisterIP, LastLogonIP) VALUES (@UserID, @GrantScoreCount, @strClientIP, @strClientIP)
		INSERT QPTreasureDB.dbo.GameScoreInfo (UserID, Score, RegisterIP, LastLogonIP) VALUES (@UserID, @GrantScoreCount, @strClientIP, @strClientIP) 		
	END

	----------------------------------------------------------------------------------------------------
	----------------------------------------------------------------------------------------------------

	-- 查询金币
--	SELECT @Score=Score, @Insure=InsureScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID
	SELECT @Score=Score, @Insure=InsureScore FROM QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID

	-- 数据调整
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	
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
		@MoorMachine AS MoorMachine, @FillIn AS FillIn, @dwAreaID As AreaID, @wPlatformID As PlatformID,
		@wServerID AS ServerID, @strVerifyCode AS Secret, @wLockedInTable AS LockedInTable, @wUserDiamondCount As UserDiamondCount, @strAgentInfo As AgentInfo

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
