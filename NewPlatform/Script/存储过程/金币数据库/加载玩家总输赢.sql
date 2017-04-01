USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadUserTotalWinAndLose]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadUserTotalWinAndLose]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载玩家运气值
CREATE PROC GSP_GR_LoadUserTotalWinAndLose
	@dwUserID INT							-- 用户 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @UserID INT
DECLARE @TotalWin 	BIGINT
DECLARE @TotalLose	BIGINT

-- 执行逻辑
BEGIN	
	-- 获取总输赢
	SELECT @TotalWin = TotalWin, @TotalLose=TotalLose From GameScoreInfo WHERE UserID=@dwUserID

	SELECT @UserID As UserID, @TotalWin As TotalWin, @TotalLose As TotalLose
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------