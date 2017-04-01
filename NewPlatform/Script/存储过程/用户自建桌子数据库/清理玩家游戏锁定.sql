
----------------------------------------------------------------------------------------------------

USE QPUserCustomDB
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
	-- 锁定解除
	DELETE GameScoreLocker WHERE KindID=@wKindID AND ServerID=@wServerID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------