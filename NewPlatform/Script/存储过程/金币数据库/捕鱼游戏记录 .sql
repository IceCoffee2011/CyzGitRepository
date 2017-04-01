
----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordFishDrawInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordFishDrawInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 捕鱼游戏记录
CREATE PROC GSP_GR_RecordFishDrawInfo

	-- 房间信息
	@wDrawID INT,								-- 局数 I D
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D

	-- 桌子信息
	@wTableID INT,								-- 桌子号码
	@wUserID INT,								-- 用户 I D
	@wChairID INT,								-- 椅子号码

	-- 游戏信息
	@lScore BIGINT								-- 上下分货币数目
	
	-- 货币信息
	--@lSourceGold BIGINT,							-- 操作前身上货币
	--@lSourceBank BIGINT,							-- 操作前银行剩余货币
	--@lTargetGold BIGINT,						-- 操作后身上货币
	--@lTargetBank BIGINT							-- 操作后银行剩余货币

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 金币变量
	DECLARE @lTargetGold BIGINT
	DECLARE @lTargetBank BIGINT

	-- 查询货币
	SELECT @lTargetGold=Score, @lTargetBank=InsureScore FROM QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@wUserID

	-- 插入记录
	INSERT QPRecordDB.dbo.RecordFishDrawInfo(DrawID,KindID,ServerID,TableID,UserID,ChairID,Score,TargetGold,TargetBank)
	VALUES (@wDrawID,@wKindID,@wServerID,@wTableID,@wUserID,@wChairID,@lScore,@lTargetGold,@lTargetBank)
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
