----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryTransferUserInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryTransferUserInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryUserNickNameByGameID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryUserNickNameByGameID]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON
GO

----------------------------------------------------------------------------------------------------
-- 查询用户
CREATE PROC GSP_GR_QueryTransferUserInfo
	@cbByNickName INT,                          -- 依据用户昵称
	@strNickName NVARCHAR(31),					-- 用户昵称
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

	-- 目标用户
	DECLARE @TargetGameID INT
	DECLARE @NickName NVARCHAR(31)
	IF @cbByNickName=1
		SELECT @TargetGameID=GameID, @NickName=NickName FROM QPAccountsDB.dbo.AccountsInfo WHERE NickName=@strNickName
	ELSE
		SELECT @TargetGameID=GameID, @NickName=NickName FROM QPAccountsDB.dbo.AccountsInfo WHERE GameID=@strNickName

	-- 查询用户
	IF @TargetGameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您所要转账的用户“'+@strNickName+'”不存在或者输入有误，请查证后再次尝试！'
		RETURN 5
	END

	-- 输出结果
	SELECT @TargetGameID AS GameID, @NickName AS NickName

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------

-- 根据GameID查询昵称
CREATE PROC GSP_GR_QueryUserNickNameByGameID
	@dwGameID INT								-- 用户 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 变量定义
	DECLARE @UserID INT
	DECLARE @NickName NVARCHAR(31)

	-- 查询用户
	SELECT @UserID=UserID, @NickName=NickName FROM QPAccountsDB.dbo.AccountsInfo WHERE GameID=@dwGameID

	-- 查询用户
	IF @UserID IS NULL 
	BEGIN
		SET @NickName=N'该用户不存在'
	END

	-- 输出信息
	SELECT @NickName As NickName, @UserID As UserID
	RETURN 0
END

RETURN 0

GO
----------------------------------------------------------------------------------------
