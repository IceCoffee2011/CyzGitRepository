----------------------------------------------------------------------------------------------------

USE QPPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SquareReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SquareReward]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 广场赛奖励道具
CREATE PROC GSP_GR_SquareReward
	@wUserID INT,							-- 用户 I D
	@wRank INT,								-- 比赛名次
	@wMatchID INT,							-- 比赛 I D 
	@wGameID INT,							-- 游戏 I D
	@wServerID INT							-- 房间 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	DECLARE @AwardID INT
	DECLARE @AwardInfoIndex INT
	DECLARE @AwardInfoCount	INT
	DECLARE @strAwardInfo NVARCHAR(256)
	
	SET @AwardInfoIndex=1				--奖励道具中起始的索引(从1开始)
	SET @strAwardInfo=''
	
	DECLARE @MaxMemberOrder INT
	DECLARE @MaxUserRight INT

	SET @MaxMemberOrder=0
	SET @MaxUserRight=0

	-- 加载道具
	--声明一个游标  
	Declare curUser Cursor for 
	Select AwardID From SquareAward WHERE MatchID=@wMatchID AND FirstRank<=@wRank AND EndRank>=@wRank
	Select @AwardInfoCount=COUNT(*) From SquareAward WHERE MatchID=@wMatchID AND FirstRank<=@wRank AND EndRank>=@wRank
	--SELECT @AwardInfoCount AS AwardInfoCount

	--打开游标  
	OPEN curUser

	--循环并提取记录  
	FETCH NEXT FROM curUser Into @AwardID--取第一条记录
	WHILE ( @@Fetch_Status=0 )     
    BEGIN  
		DECLARE @wType 				INT				--道具类型
		DECLARE @wCount 			BIGINT			--属性数额
		DECLARE @strName			NVARCHAR(256)	--道具名称

		DECLARE @MemberOrder 		INT
		DECLARE @MemberRight		INT
		DECLARE @MemberValidMonth	INT			

		DECLARE @MemberOverDate DATETIME
		DECLARE @MemberSwitchDate DATETIME

		DECLARE @wAwardType			INT				--奖励类型,0 货币奖励,1 其他

		-- 金币变量
		DECLARE @SourceScore BIGINT
		DECLARE @SourceInsure BIGINT
		DECLARE @TargetScore BIGINT
		DECLARE @TargetInsure BIGINT

		-- 设置变量
		SET @MemberOrder=1
		SET @MemberRight=512
		SET	@wAwardType=0
	
		--逐个道具进行奖励
		Select @wType=Type, @wCount=Count, @strName=Name From QPTreasureDB.dbo.SquarePropertyInfo WHERE ID=@AwardID
		
		-- 游戏信息
		SELECT @SourceScore=Score, @SourceInsure=InsureScore FROM QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@wUserID

		--IF @wType > 10000		--服装形象
		IF @wType = 3			--欢乐豆
		BEGIN
			UPDATE QPTreasureDB.dbo.GameScoreInfo SET InsureScore=InsureScore+@wCount WHERE UserID=@wUserID
		END
		ELSE IF @wType = 4			--会员天数
		BEGIN
			-- 删除过期
			DELETE FROM QPAccountsDB.dbo.AccountsMember
			WHERE UserID=@wUserID AND MemberOrder=@MemberOrder AND MemberOverDate<=GETDATE()

			-- 有效期限
			SELECT @MemberValidMonth= @wCount * 1

			-- 更新会员
			SELECT @MemberOverDate=MemberOverDate FROM QPAccountsDB.dbo.AccountsMember
			WHERE UserID=@wUserID AND MemberOrder=@MemberOrder
			UPDATE QPAccountsDB.dbo.AccountsMember SET MemberOverDate=DATEADD(mm, @MemberValidMonth, @MemberOverDate)
			WHERE UserID=@wUserID AND MemberOrder=@MemberOrder
			IF @@ROWCOUNT=0
			BEGIN
				INSERT QPAccountsDB.dbo.AccountsMember(UserID,MemberOrder,UserRight,MemberOverDate)
				VALUES (@wUserID,@MemberOrder,@MemberRight,DATEADD(mm,@MemberValidMonth, GETDATE()))
			END

			-- 绑定会员,(会员期限与切换时间)
			SELECT @MaxMemberOrder=MAX(MemberOrder),@MemberOverDate=MAX(MemberOverDate),@MemberSwitchDate=MIN(MemberOverDate)
			FROM QPAccountsDB.dbo.AccountsMember WHERE UserID=@wUserID

			-- 会员权限
			SELECT @MaxUserRight=UserRight FROM QPAccountsDB.dbo.AccountsMember
			WHERE UserID=@wUserID AND MemberOrder=@MaxMemberOrder
			
			-- 附加会员卡信息
			UPDATE QPAccountsDB.dbo.AccountsInfo
			SET MemberOrder=@MaxMemberOrder,UserRight=@MaxUserRight,MemberOverDate=@MemberOverDate,MemberSwitchDate=@MemberSwitchDate
			WHERE UserID=@wUserID

		END
		
		-- 记录奖励
		-- 游戏信息
		IF @wType = 3			--欢乐豆
		BEGIN
			SET @wAwardType=0
			SELECT @TargetScore=Score, @TargetInsure=InsureScore FROM QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@wUserID
			INSERT QPTreasureDB.dbo.RecordSquareAward(MatchID,ServerID,GameID,UserID,ProperID,ProperName,AwardDate,Rank,AwardType,SourceGold,SourceBank,TargetGold,TargetBank,SwapScore)
			VALUES (@wMatchID,@wServerID,@wGameID,@wUserID,@AwardID,@strName,GETDATE(),@wRank,@wAwardType,@SourceScore,@SourceInsure,@TargetScore,@TargetInsure,@wCount)
		END
		ELSE					--其他
		BEGIN
			SET @wAwardType=1
			INSERT QPTreasureDB.dbo.RecordSquareAward(MatchID,ServerID,GameID,UserID,ProperID,ProperName,AwardDate,Rank,AwardType)
			VALUES (@wMatchID,@wServerID,@wGameID,@wUserID,@AwardID,@strName,GETDATE(),@wRank,@wAwardType)
		END
		
		--拼接道具名称
		SET @strAwardInfo=@strAwardInfo+@strName
		IF (@AwardInfoIndex<@AwardInfoCount) AND (@AwardInfoCount>1)
		BEGIN
			IF @AwardInfoIndex=@AwardInfoCount-1
			BEGIN
				SET @strAwardInfo=@strAwardInfo+N'和'
			END
			ELSE
			BEGIN
				SET @strAwardInfo=@strAwardInfo+N'、'
			END
		END

		/*--拼接道具名称
		SELECT @strName AS Name
		SELECT @strAwardInfo=@strAwardInfo+@strName
		SELECT @strAwardInfo AS strAwardInfo
		IF (@AwardInfoIndex<@AwardInfoCount) AND (@AwardInfoCount>1)
		BEGIN
			SET @strAwardInfo=@strAwardInfo+N'和'
		END*/

		SET @AwardInfoIndex=@AwardInfoIndex+1
		FETCH NEXT FROM curUser into @AwardID----下一条 
    END   

	--关闭游标     
	Close curUser  
	--释放游标  
	Deallocate curUser
	
	SELECT @strAwardInfo AS AwardInfo, @MaxMemberOrder AS MemberOrder, @MaxUserRight AS UserRight
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
