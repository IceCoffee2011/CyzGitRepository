
----------------------------------------------------------------------------------------------------

USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyUnderWrite]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyUnderWrite]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 修改签名
CREATE PROC GSP_GP_ModifyUnderWrite
	@dwUserID INT,								-- 用户 I D	
	@strUnderWrite NVARCHAR(31),				-- 用户签名
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查询信息
	-- 变量定义
	DECLARE @UserID INT
	DECLARE @NickName NVARCHAR(31)
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT

	-- 查询用户
	SELECT @UserID=UserID, @NickName=NickName, @Nullity=Nullity, @StunDown=StunDown
	FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- 用户判断
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'您的帐号不存在，请查证后再次尝试！'
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

	-- 设置签名
	UPDATE AccountsInfo SET UnderWrite=@strUnderWrite WHERE UserID=@dwUserID

	-- 设置信息
	IF @@ERROR=0 SET @strErrorDescribe=N'个人签名修改成功！'

	RETURN 0
END

GO

----------------------------------------------------------------------------------------------------
