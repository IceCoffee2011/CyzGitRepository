----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_GetVerificationCode]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_GetVerificationCode]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_VerifyMobile]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_VerifyMobile]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 获取验证码
CREATE PROC GSP_MB_GetVerificationCode
	@dwUserID 		 INT,						-- 验证方的数字ID
	@dwAreaID 		 INT,						-- 验证方的区域ID
	@strMobileNumber NVARCHAR(17)				-- 手机号
WITH ENCRYPTION AS


DECLARE @strErrorDescribe 	NVARCHAR(127) 		-- 输出信息
DECLARE @RETURN_VALUE		INT					-- 返回值
DECLARE @strVerifyCode		NVARCHAR(7)			-- 查询到的验证码


-- 属性设置
SET NOCOUNT ON

BEGIN
	SET @strErrorDescribe = N''
	SET @RETURN_VALUE = 0
	SET @strVerifyCode = N''

	-- 检查传入的手机号是否合法
	IF LEN(@strMobileNumber) <> 11
	BEGIN
		SET @RETURN_VALUE = 1
		SET @strErrorDescribe=N'手机号长度不合法, 请检查您输入的手机号！'
		SELECT @strErrorDescribe As strErrorDescribe, @RETURN_VALUE As RETURN_VALUE
		RETURN @RETURN_VALUE
	END
	
	-- 检验玩家是否存在
	IF NOT EXISTS (SELECT * FROM QPAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID AND AreaID=@dwAreaID)
	BEGIN
		SET @RETURN_VALUE = 2
		SET @strErrorDescribe = N'验证的玩家不存在,为确保您的利益,请及时与客服联系！'
		SELECT @strErrorDescribe As strErrorDescribe, @RETURN_VALUE As RETURN_VALUE
		RETURN @RETURN_VALUE
	END
	
	-- 验证码生成时间间隔为1分钟
	IF EXISTS (SELECT * FROM QPTreasureDB.dbo.MobileVerification WHERE MobileNumber=@strMobileNumber AND GETDATE() <= DATEADD(MINUTE, 1, GenerateTime) AND GETDATE() >= GenerateTime)
	BEGIN
		SET @RETURN_VALUE = 3
		SET @strErrorDescribe=N'一分钟内只能获取一次验证码, 请稍后再试！'
		SELECT @strErrorDescribe As strErrorDescribe, @RETURN_VALUE As RETURN_VALUE
		RETURN @RETURN_VALUE
	END
	
	-- 近一小时获取验证码不能超过8条
	DECLARE @nCount INT
	SELECT @nCount=COUNT(*) FROM QPTreasureDB.dbo.MobileVerification WHERE MobileNumber=@strMobileNumber AND GenerateTime >= DATEADD(HOUR, -1, GETDATE()) AND GenerateTime <= GETDATE()
	IF @nCount > 8
	BEGIN
		SET @RETURN_VALUE = 4
		SET @strErrorDescribe=N'您最近一小时获取验证码太频繁, 请稍后再试！'
		SELECT @strErrorDescribe As strErrorDescribe, @RETURN_VALUE As RETURN_VALUE
		RETURN @RETURN_VALUE
	END
	
	-- 随机生成验证码
	DECLARE @nSuccess INT
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
			IF NOT EXISTS (SELECT * FROM QPTreasureDB.dbo.MobileVerification WHERE MobileNumber=@strMobileNumber AND VerificationCode=@strVerifyCode)
			BEGIN
				-- 插入新生成验证码
				INSERT QPTreasureDB.dbo.MobileVerification (MobileNumber, VerificationCode, GenerateTime) VALUES (@strMobileNumber, @strVerifyCode, GETDATE())
				SET @nSuccess = 1
			END
		END
	END
	
	SET @strErrorDescribe = N''
	SELECT @strErrorDescribe As strErrorDescribe, @RETURN_VALUE As RETURN_VALUE, @strVerifyCode As strVerifyCode		
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 验证手机号
CREATE PROC GSP_MB_VerifyMobile
	@dwUserID 		 INT,						-- 验证方的数字ID
	@dwAreaID 		 INT,						-- 验证方的区域ID
	@nVerifyType	 INT,						-- 验证类型
	@strMobileNumber NVARCHAR(17),				-- 手机号
	@strVerifyCode	 NVARCHAR(7)				-- 查询到的验证码
WITH ENCRYPTION AS


DECLARE @strErrorDescribe 	NVARCHAR(127) 		-- 输出信息
DECLARE @RETURN_VALUE		INT					-- 返回值


-- 属性设置
SET NOCOUNT ON

BEGIN
	-- 检查传入的手机号是否合法
	IF LEN(@strMobileNumber) <> 11
	BEGIN
		SET @RETURN_VALUE = 1
		SET @strErrorDescribe=N'手机号长度不合法, 请检查您输入的手机号！'
		SELECT @strErrorDescribe As strErrorDescribe, @RETURN_VALUE As RETURN_VALUE
		RETURN @RETURN_VALUE
	END

	-- 验证码为6位数字
	IF LEN(@strVerifyCode) <> 6
	BEGIN
		SET @RETURN_VALUE = 2
		SET @strErrorDescribe=N'验证码长度不合法, 请检查您输入的验证码！'
		SELECT @strErrorDescribe As strErrorDescribe, @RETURN_VALUE As RETURN_VALUE
		RETURN @RETURN_VALUE
	END
	
	-- 检验玩家是否存在
	IF NOT EXISTS (SELECT * FROM QPAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID AND AreaID=@dwAreaID)
	BEGIN
		SET @RETURN_VALUE = 3
		SET @strErrorDescribe = N'验证的玩家不存在,为确保您的利益,请及时与客服联系！'
		SELECT @strErrorDescribe As strErrorDescribe, @RETURN_VALUE As RETURN_VALUE
		RETURN @RETURN_VALUE
	END
	
	-- 验证码有效时间为10分钟
	IF NOT EXISTS (SELECT * FROM QPTreasureDB.dbo.MobileVerification WHERE MobileNumber=@strMobileNumber AND VerificationCode=@strVerifyCode AND GETDATE() <= DATEADD(MINUTE, 10, GenerateTime) AND GETDATE() >= GenerateTime)
	BEGIN
		SET @RETURN_VALUE = 4
		SET @strErrorDescribe=N'您的验证码无效, 请重新获取验证码！'
		SELECT @strErrorDescribe As strErrorDescribe, @RETURN_VALUE As RETURN_VALUE
		RETURN @RETURN_VALUE
	END
	
	-- 检查手机号是否已被绑定
	IF EXISTS (SELECT * FROM QPAccountsDB.dbo.IndividualDatum WHERE MobilePhone=@strMobileNumber)
	BEGIN
		SET @RETURN_VALUE = 5
		SET @strErrorDescribe=N'您的手机号已被绑定，请更换绑定手机号！'
		SELECT @strErrorDescribe As strErrorDescribe, @RETURN_VALUE As RETURN_VALUE
		RETURN @RETURN_VALUE
	END
	
	-- 验证成功
	DECLARE @strSourceMobile NVARCHAR(17) 			-- 修改前手机号码
	DECLARE @strTargetMobile NVARCHAR(17) 			-- 修改后手机号码
	DECLARE @strSourceLogonPass NVARCHAR(33) 		-- 修改前登录密码
	DECLARE @strSourceInsurePass NVARCHAR(33) 		-- 修改前安全密码
	DECLARE @strTargetLogonPass NVARCHAR(33) 		-- 修改后登录密码
	DECLARE @strTargetInsurePass NVARCHAR(33) 		-- 修改后安全密码
	SET @strSourceMobile=N''
	SET @strTargetMobile=N''
	SET @strSourceLogonPass=N''
	SET @strSourceInsurePass=N''
	SET @strTargetLogonPass=N''
	SET @strTargetInsurePass=N''
	
	IF @nVerifyType = 1 OR @nVerifyType = 2				-- 绑定手机 和 解绑手机(仅仅解除绑定)
	BEGIN
		-- 验证成功，绑定手机号
		DECLARE @strRegisterMobileNumber NVARCHAR(17) 	-- 注册手机号
		-- 先查询AccountsInfo中是否已记录注册手机号，如果没有，记录当前绑定手机号
		SELECT @strRegisterMobileNumber=RegisterMobile FROM QPAccountsDB.dbo.AccountsInfo(NOLOCK) WHERE UserID=@dwUserID AND AreaID=@dwAreaID
		IF LEN(@strRegisterMobileNumber) = 0
		BEGIN		
			UPDATE QPAccountsDB.dbo.AccountsInfo SET RegisterMobile=@strMobileNumber WHERE UserID=@dwUserID AND AreaID=@dwAreaID
		END

		-- 在表IndividualDatum中记录绑定手机号
		IF NOT EXISTS (SELECT * FROM QPAccountsDB.dbo.IndividualDatum WHERE UserID=@dwUserID)
		BEGIN
			INSERT QPAccountsDB.dbo.IndividualDatum(UserID, MobilePhone) VALUES(@dwUserID, @strMobileNumber)
		END
		ELSE
		BEGIN
			SELECT @strSourceMobile=MobilePhone FROM QPAccountsDB.dbo.IndividualDatum WHERE UserID=@dwUserID
			UPDATE QPAccountsDB.dbo.IndividualDatum SET MobilePhone=@strMobileNumber, CollectDate=GETDATE() WHERE UserID=@dwUserID
		END
		
		SET @strTargetMobile=@strMobileNumber
		
		SET @strErrorDescribe = N'绑定手机号成功'
		IF @nVerifyType = 2
		BEGIN
			SET @strErrorDescribe = N'解绑手机号成功'
		END
	END

	-- 记录验证信息
	INSERT QPRecordDB.dbo.RecordMobileVerification(UserID, VerificationType, VerificationCode, SourceMobile, TargetMobile, InsertTime, SourceLogonPass, SourceInsurePass, TargetLogonPass, TargetInsurePass)
		VALUES(@dwUserID, @nVerifyType, @strVerifyCode, @strSourceMobile, @strTargetMobile, GETDATE(), @strSourceLogonPass, @strSourceInsurePass, @strTargetLogonPass, @strTargetInsurePass)
	
	-- 清除当前存在的所有验证码
	DELETE FROM QPTreasureDB.dbo.MobileVerification WHERE MobileNumber=@strMobileNumber
	
	SELECT @strErrorDescribe As strErrorDescribe, @RETURN_VALUE As RETURN_VALUE
END

RETURN 0

GO