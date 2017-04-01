
----------------------------------------------------------------------------------------------------

USE QPGameScoreDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ResetGameScoreLocker]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ResetGameScoreLocker]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 离开房间
CREATE PROC GSP_GR_ResetGameScoreLocker
	-- 属性信息
	@wKindID INT,								-- 游戏 I D
	@wServerID INT								-- 房间 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

Declare @UserID INT
DECLARE @BalanceScore INT

DECLARE @AndroidUserID INT

DECLARE @PlayerDefaultBalanceScore INT
DECLARE	@AndroidDefaultBalanceScore INT

-- 执行逻辑
BEGIN
	-- 得到配置的默认的运气值
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

	-- 重置所有在房间的玩家的BalanceScore
	--声明一个游标  
	Declare curUser Cursor for 
	
	Select UserID From GameScoreLocker WHERE KindID=@wKindID AND ServerID=@wServerID
	--打开游标  
	OPEN curUser
	
	--循环并提取记录  
	FETCH NEXT FROM curUser Into @UserID--取第一条记录存入@result中  
	WHILE ( @@Fetch_Status=0 )     
    BEGIN  
		-- 查询是否是机器人玩家
		SELECT @AndroidUserID=UserID FROM AndroidManager WHERE UserID=@UserID AND ServerID=@wServerID
		-- 插入玩家平衡分信息
		IF @AndroidUserID IS NULL 
		BEGIN
			UPDATE GameBalanceScore SET BalanceScore = @PlayerDefaultBalanceScore WHERE UserID=@UserID
			IF @@ROWCOUNT=0
			BEGIN
				INSERT INTO GameBalanceScore (UserID, BalanceScore, BuffTime, BuffGames, BuffAmount)
				VALUES (@UserID, @PlayerDefaultBalanceScore, 0, 0, 0)
			END
		END
		ELSE
		BEGIN
			UPDATE GameBalanceScore SET BalanceScore = @AndroidDefaultBalanceScore WHERE UserID=@UserID
			IF @@ROWCOUNT=0
			BEGIN
				INSERT INTO GameBalanceScore (UserID, BalanceScore, BuffTime, BuffGames, BuffAmount)
				VALUES (@UserID, @AndroidDefaultBalanceScore, 0, 0, 0)
			END
		END
		
		FETCH NEXT FROM curUser into @UserID----下一条  
    END   

	--关闭游标     
	Close curUser  
	--释放游标  
	Deallocate curUser 
	
	-- 锁定解除
	DELETE GameScoreLocker WHERE KindID=@wKindID AND ServerID=@wServerID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------