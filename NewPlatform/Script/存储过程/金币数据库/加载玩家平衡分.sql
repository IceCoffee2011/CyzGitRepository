USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadUserBalanceScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadUserBalanceScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载玩家运气值
CREATE PROC GSP_GR_LoadUserBalanceScore
	@dwUserID INT,							-- 用户 I D
	@wServerID SMALLINT						-- 房间 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @UserID INT
DECLARE @BalanceScore INT
DECLARE @BuffTime INT
DECLARE @BuffGames INT
DECLARE @BuffAmount INT

DECLARE @AndroidUserID INT

-- 执行逻辑
BEGIN

	-- 查询玩家平衡分信息
	SELECT @UserID=UserID, @BalanceScore=BalanceScore, @BuffTime=BuffTime, @BuffGames=BuffGames, @BuffAmount=BuffAmount   
	FROM GameBalanceScore WHERE UserID=@dwUserID
	
	-- 查询是否是机器人玩家
	SELECT @AndroidUserID=UserID FROM AndroidManager WHERE UserID=@dwUserID AND ServerID=@wServerID
	
	-- 信息判断,玩家不存在
	IF @UserID IS NULL
	BEGIN
		-- 得到玩家初始幸运值
		DECLARE @PlayerDefaultBalanceScore INT
		DECLARE	@AndroidDefaultBalanceScore INT
		SELECT @PlayerDefaultBalanceScore=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'PlayerDefaultBalanceScore'
		SELECT @AndroidDefaultBalanceScore=StatusValue FROM QPAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'AndroidDefaultBalanceScore'
		IF @PlayerDefaultBalanceScore IS NULL
		BEGIN
			SET @PlayerDefaultBalanceScore=500
		END
		IF @AndroidDefaultBalanceScore IS NULL
		BEGIN
			SET @AndroidDefaultBalanceScore=500
		END
		
		-- 插入玩家平衡分信息
		IF @AndroidUserID IS NULL 
		BEGIN
			INSERT INTO GameBalanceScore (UserID, BalanceScore, BuffTime, BuffGames, BuffAmount)
			VALUES (@dwUserID, @PlayerDefaultBalanceScore, 0, 0, 0)
		END
		ELSE
		BEGIN
			INSERT INTO GameBalanceScore (UserID, BalanceScore, BuffTime, BuffGames, BuffAmount)
			VALUES (@dwUserID, @AndroidDefaultBalanceScore, 0, 0, 0)
		END
		
		SELECT @UserID=UserID, @BalanceScore=BalanceScore, @BuffTime=BuffTime, @BuffGames=BuffGames, @BuffAmount=BuffAmount   
		FROM GameBalanceScore WHERE UserID=@dwUserID
	END
	
	SELECT @UserID As UserID, @BalanceScore As BalanceScore, @BuffTime As BuffTime, @BuffGames As BuffGames, @BuffAmount As BuffAmount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------