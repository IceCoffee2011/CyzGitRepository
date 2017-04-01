----------------------------------------------------------------------------------------------------

USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_TestProcedure]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_TestProcedure]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

--读取系统消息
CREATE  PROCEDURE dbo.GSP_GP_TestProcedure 
	@strAccounts NVARCHAR(31)					-- 用户帐号
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON
BEGIN
	DECLARE @UserID INT
	DECLARE @CustomID INT
	DECLARE @FaceID SMALLINT
	DECLARE @NickName NVARCHAR(31)
	
	SELECT @UserID=UserID, @FaceID=FaceID, @CustomID=CustomID, @NickName=NickName
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts
	
	SELECT @UserID AS UserID, @FaceID AS FaceID, @CustomID AS CustomID, @NickName As NickNameTest
	RETURN 0
END
	RETURN 0
GO
