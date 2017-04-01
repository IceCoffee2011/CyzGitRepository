USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_SignInCheck]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_SignInCheck]
GO

----------------------------------------------------------------------------------------------------

-- 插入头像
CREATE PROC GSP_MB_SignInCheck
	@dwUserID INT,								-- 用户标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON


DECLARE @LogonCount INT

-- 执行逻辑
BEGIN
	-- 选出登录的天数
	SELECT COUNT(*) As Times, CONVERT(datetime, CONVERT(VARCHAR(10),EnterTime,111), 111) As DayTime FROM RecordUserLogon where UserID=@dwUserID AND CONVERT(VARCHAR(7),GETDATE(),111)=CONVERT(VARCHAR(7),EnterTime,111) GROUP BY ( CONVERT(VARCHAR(10),EnterTime,111) ); 

	-- 查询当天是否成功登录过一次
	SELECT @LogonCount=COUNT(*) FROM [dbo].[RecordUserLogon] where UserID=@dwUserID AND CONVERT(VARCHAR(10),GETDATE(),111)=CONVERT(VARCHAR(10),EnterTime,111)
	IF @LogonCount=0
	BEGIN
		SET @strErrorDescribe=N'账号未签到'
		RETURN 1
	END
	
	IF @LogonCount>1
	BEGIN
		SET @strErrorDescribe=N'账号已签到'
		RETURN 1
	END
	
	RETURN 0
END	

RETURN 0

GO