----------------------------------------------------------------------------------------------------

USE QPUserCustomDB
GO

-- 用户创建桌子
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadUserRoundScoreInCustomTable]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadUserRoundScoreInCustomTable]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 创建桌子
CREATE PROC GSP_GR_LoadUserRoundScoreInCustomTable
	@dwUserID INT
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 辅助变量
DECLARE @lRoundScore AS BIGINT

-- 执行逻辑
BEGIN

	SELECT @lRoundScore = RoundScore FROM UserInTable WHERE UserID=@dwUserID
	IF @lRoundScore IS NULL
	BEGIN
		SET @lRoundScore = 0
	END

	SELECT @lRoundScore AS Score
END

RETURN 0

GO