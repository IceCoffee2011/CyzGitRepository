
----------------------------------------------------------------------------------------------------

USE QPUserCustomDB
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
	@dwVariationPlayTimeCount INT				-- 游戏时间
	
--	@lBalanceScore INT,							-- 平衡分
--	@lBuffTime INT,								-- Buff时间
--	@lBuffGames INT,							-- Buff持续盘数
--	@lBuffAmount INT,							-- Buff数量

	-- 属性信息
--	@wKindID INT,								-- 游戏 I D
--	@wServerID INT,								-- 房间 I D
--	@strClientIP NVARCHAR(15)					-- 连接地址

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
	
	-- 更新玩家在桌子的信息
	Update UserInTable SET RoundCount = RoundCount + 1, RoundScore = RoundScore + @lVariationScore 
	WHERE UserID=@dwUserID
	
--	DECLARE @dwRoundScore AS BIGINT
--	DECLARE @dwRoundCount AS INT						-- 游戏目前第几轮
--	DECLARE @dwCreateRoundCount AS INT					-- 房卡创建了多少轮
--	DECLARE @dwServerID AS INT
--	DECLARE @dwTableID AS INT
--	DECLARE @dwChairID AS INT
--	DECLARE @strSecret NVARCHAR(9)						-- 定制规则
--	DECLARE @dwTableBuilder AS INT

--	SELECT @dwRoundCount = RoundCount, @dwServerID=ServerID, @dwTableID=TableID, @dwChairID=ChairID, @dwRoundScore = RoundScore, @dwTableBuilder = TableBuilder
--	FROM UserInTable WHERE UserID=@dwUserID
--	SELECT @dwCreateRoundCount = RoundCount, @strSecret = Secret FROM UserCreateTable 
--	WHERE UserID IN(SELECT UserID FROM UserInTable WHERE ServerID=@dwServerID AND TableID=@dwTableID AND TableBuilder != 0)
--	-- 判断是否结束了游戏,记录总日志
--	IF @dwRoundCount = @dwCreateRoundCount
--	BEGIN
--
--	END
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------