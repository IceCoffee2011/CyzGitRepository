
----------------------------------------------------------------------------------------------------

USE QPGameScoreDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteGameScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteGameScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 游戏写分
CREATE PROC GSP_GR_WriteGameScore

	-- 用户信息
	@dwUserID INT,								-- 用户 I D
	@dwDBQuestID INT,							-- 请求标识
	@dwInoutIndex INT,							-- 进出索引

	-- 变更成绩
	@lVariationScore BIGINT,					-- 用户分数
	@lVariationGrade BIGINT,					-- 用户成绩
	@lVariationInsure BIGINT,					-- 用户银行
	@lVariationRevenue BIGINT,					-- 游戏税收
	@lVariationWinCount INT,					-- 胜利盘数
	@lVariationLostCount INT,					-- 失败盘数
	@lVariationDrawCount INT,					-- 和局盘数
	@lVariationFleeCount INT,					-- 断线数目
	@lVariationUserMedal INT,					-- 用户奖牌
	@lVariationExperience INT,					-- 用户经验
	@lVariationLoveLiness INT,					-- 用户魅力
	@dwVariationPlayTimeCount INT,				-- 游戏时间
	
	@lBalanceScore INT,							-- 平衡分
	@lBuffTime INT,								-- Buff时间
	@lBuffGames INT,							-- Buff持续盘数
	@lBuffAmount INT,							-- Buff数量

	-- 属性信息
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
	@strClientIP NVARCHAR(15)					-- 连接地址

WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 用户积分
	IF @lVariationScore >= 0
	BEGIN
		UPDATE GameScoreInfo SET Score=Score+@lVariationScore, Revenue=Revenue+@lVariationRevenue, 
			InsureScore=InsureScore+@lVariationInsure,
			WinCount=WinCount+@lVariationWinCount, LostCount=LostCount+@lVariationLostCount, DrawCount=DrawCount+@lVariationDrawCount,
			FleeCount=FleeCount+@lVariationFleeCount, PlayTimeCount=PlayTimeCount+@dwVariationPlayTimeCount,
			TotalWin=TotalWin+@lVariationScore
		WHERE UserID=@dwUserID
	END
	
	IF @lVariationScore < 0
	BEGIN
		UPDATE GameScoreInfo SET Score=Score+@lVariationScore, Revenue=Revenue+@lVariationRevenue, 
			InsureScore=InsureScore+@lVariationInsure,
			WinCount=WinCount+@lVariationWinCount, LostCount=LostCount+@lVariationLostCount, DrawCount=DrawCount+@lVariationDrawCount,
			FleeCount=FleeCount+@lVariationFleeCount, PlayTimeCount=PlayTimeCount+@dwVariationPlayTimeCount,
			TotalLose=TotalLose+@lVariationScore
		WHERE UserID=@dwUserID
	END

	-- 全局信息
	IF @lVariationExperience>0 OR @lVariationLoveLiness>0 OR @lVariationUserMedal>0
	BEGIN
		UPDATE QPAccountsDB.dbo.AccountsInfo SET Experience=Experience+@lVariationExperience, LoveLiness=LoveLiness+@lVariationLoveLiness,
			UserMedal=UserMedal+@lVariationUserMedal
		WHERE UserID=@dwUserID
	END
	
	IF @lBalanceScore<1
	BEGIN
		SET @lBalanceScore = 1
	END
	
	-- 先去获取用户平衡分
	DECLARE @DBBuffTime INT
	DECLARE @DBBuffGames INT
	DECLARE @DBBuffAmount INT
	SELECT @DBBuffTime=BuffTime, @DBBuffGames=BuffGames, @DBBuffAmount=BuffAmount FROM GameBalanceScore
	WHERE UserID=@dwUserID
	
	IF @@ROWCOUNT=0
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
		
		-- 插入默认值
		IF @strClientIP=N'0.0.0.0'
		BEGIN
			INSERT INTO GameBalanceScore (UserID, BalanceScore, BuffTime, BuffGames, BuffAmount)
			VALUES (@dwUserID, @AndroidDefaultBalanceScore, 0, 0, 0)
		END
		ELSE
		BEGIN
			INSERT INTO GameBalanceScore (UserID, BalanceScore, BuffTime, BuffGames, BuffAmount)
			VALUES (@dwUserID, @PlayerDefaultBalanceScore, 0, 0, 0)
		END
	END
	ELSE
	BEGIN
		-- 判断传入的数据与数据库读取的数据是否正常
		IF @DBBuffAmount+@lBuffAmount <> 0 AND @lBuffAmount<>0
		BEGIN
			SET @lBuffAmount=@DBBuffAmount
		END
		IF @DBBuffTime+@lBuffTime<0
		BEGIN
			SET @lBuffTime = -@DBBuffTime
		END
		IF @DBBuffGames+@lBuffGames<0
		BEGIN
			SET @lBuffGames = -@DBBuffGames
		END
	END

	-- 更新用户平衡分
	Update GameBalanceScore SET BalanceScore=@lBalanceScore, BuffTime=BuffTime+@lBuffTime, BuffGames=BuffGames+@lBuffGames, BuffAmount=BuffAmount+@lBuffAmount
	WHERE UserID=@dwUserID
	
	-- 得到更新的数据
	SELECT @dwUserID = UserID, @lBalanceScore=BalanceScore, @lBuffTime=BuffTime, @lBuffGames=BuffGames, @lBuffAmount=BuffAmount
	From GameBalanceScore
	WHERE UserID=@dwUserID

	SELECT @dwUserID As UserID, @lBalanceScore As BalanceScore, @lBuffTime As BuffTime, @lBuffGames As BuffGames, @lBuffAmount As BuffAmount 
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------