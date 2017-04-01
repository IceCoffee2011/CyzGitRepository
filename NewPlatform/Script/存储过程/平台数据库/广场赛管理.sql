----------------------------------------------------------------------------------------------------

USE QPPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_InsertSquareMatch]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_InsertSquareMatch]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_ModifySquareMatch]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_ModifySquareMatch]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_DeleteSquareMatch]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_DeleteSquareMatch]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadSquareMatch]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadSquareMatch]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadSquareMatchOption]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadSquareMatchOption]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_LoadSquareMatchRankBonus]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_LoadSquareMatchRankBonus]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 插入广场赛
CREATE PROC GSP_GS_InsertSquareMatch
	--索引变量
	@wGameID INT,							-- 游戏 I D

	--房间属性
	@wServerID INT,							-- 房间 I D
	@strServerName NVARCHAR(32),			-- 房间名称

	--时间配置
	@StartTime DATETIME,					-- 开始时间
	@EndTime DATETIME,						-- 结束时间
	@wInterval INT,							-- 通知间隔

	--排名奖励配置
	@wRankNumber INT,						-- 排名限额
	@strSquareName NVARCHAR(1024),			-- 比赛名称
	@strSquareDesc NVARCHAR(1024),			-- 比赛描述

	-- 输出信息
	@strErrorDescribe NVARCHAR(127) OUTPUT	-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查找模块
	DECLARE @GameID INT
	SELECT @GameID=GameID FROM GameGameItem(NOLOCK) WHERE GameID=@wGameID

	-- 存在判断
	IF @GameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'相应的游戏服务组件信息不存在，广场赛创建失败！'
		RETURN 1
	END

	-- 查找房间
	DECLARE @ServerID INT
	SELECT @ServerID=ServerID FROM GameRoomInfo(NOLOCK) WHERE ServerID=@wServerID

	-- 存在判断
	IF @ServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'相应的游戏房间不存在，广场赛创建失败！'
		RETURN 1
	END

	-- 插入比赛
	INSERT INTO SquareMatch (ServerID, ServerName, GameID, StartAt, EndAt, Interval, RankNumber, SquareName, SquareDesc)
	VALUES (@wServerID, @strServerName, @wGameID, @StartTime, @EndTime, @wInterval, @wRankNumber, @strSquareName, @strSquareDesc)

	-- 加载比赛
	SELECT SquareMatch.* FROM SquareMatch(NOLOCK) WHERE ServerID=@wServerID AND MatchID=SCOPE_IDENTITY()
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 修改广场赛
CREATE PROC GSP_GS_ModifySquareMatch
	-- 索引变量
	@wMatchID INT,							-- 比赛标识
	@wGameID INT,							-- 游戏 I D

	--房间属性
	@wServerID INT,							-- 房间 I D
	@strServerName NVARCHAR(32),			-- 房间名称

	--时间配置
	@StartTime DATETIME,					-- 开始时间
	@EndTime DATETIME,						-- 结束时间
	@wInterval INT,							-- 通知间隔

	--排名奖励配置
	@wRankNumber INT,						-- 排名限额
	@strSquareName NVARCHAR(1024),			-- 比赛名称
	@strSquareDesc NVARCHAR(1024),			-- 比赛描述

	-- 输出信息
	@strErrorDescribe NVARCHAR(127) OUTPUT	-- 输出信息
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查找模块
	DECLARE @GameID INT
	SELECT @GameID=GameID FROM GameGameItem(NOLOCK) WHERE GameID=@wGameID

	-- 存在判断
	IF @GameID IS NULL
	BEGIN
		SET @strErrorDescribe=N'相应的游戏服务组件信息不存在，广场赛修改失败！'
		RETURN 1
	END

	-- 查找房间
	DECLARE @ServerID INT
	SELECT @ServerID=ServerID FROM GameRoomInfo(NOLOCK) WHERE ServerID=@wServerID

	-- 存在判断
	IF @ServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'相应的游戏房间不存在，广场赛修改失败！'
		RETURN 1
	END

	-- 查找比赛
	DECLARE @MatchID INT
	SELECT @MatchID=MatchID FROM SquareMatch(NOLOCK) WHERE MatchID=@wMatchID

	-- 结果判断
	IF @MatchID IS NULL
	BEGIN
		SET @strErrorDescribe=N'游戏广场赛不存在或者已经被删除了，广场赛修改失败！'
		RETURN 2
	END

	-- 插入比赛
	UPDATE SquareMatch SET StartAt=@StartTime, EndAt=@EndTime, Interval=@wInterval, RankNumber=@wRankNumber, SquareName=@strSquareName, SquareDesc=@strSquareDesc
	WHERE ServerID=@wServerID AND MatchID=@wMatchID
	
	-- 加载比赛
	SELECT SquareMatch.* FROM SquareMatch(NOLOCK) WHERE ServerID=@wServerID AND MatchID=@wMatchID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 删除广场赛
CREATE  PROCEDURE dbo.GSP_GS_DeleteSquareMatch
	@wServerID INT,								-- 房间标识
	@wMatchID INT,								-- 比赛标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 查找房间
	DECLARE @ServerID INT
	SELECT @ServerID=ServerID FROM GameRoomInfo(NOLOCK) WHERE ServerID=@wServerID

	-- 存在判断
	IF @ServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'相应的游戏房间不存在，广场赛删除失败！'
		RETURN 1
	END

	-- 查找比赛
	DECLARE @MatchID INT
	SELECT @MatchID=MatchID FROM SquareMatch(NOLOCK) WHERE MatchID=@wMatchID

	-- 结果判断
	IF @MatchID IS NULL
	BEGIN
		SET @strErrorDescribe=N'游戏广场赛不存在或者已经被删除了，广场赛删除失败！'
		RETURN 2
	END

	-- 删除比赛
	DELETE SquareMatch WHERE ServerID=@wServerID AND MatchID=@wMatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载广场赛
CREATE  PROCEDURE dbo.GSP_GS_LoadSquareMatch
	@wServerID INT,								-- 房间标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	-- 查找房间
	DECLARE @ServerID INT
	SELECT @ServerID=ServerID FROM GameRoomInfo(NOLOCK) WHERE ServerID=@wServerID

	-- 存在判断
	IF @ServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'相应的游戏房间不存在，广场赛加载失败！'
		RETURN 1
	END

	-- 加载广场赛
	SELECT SquareMatch.* FROM SquareMatch(NOLOCK) WHERE ServerID=@wServerID AND GETDATE() < SquareMatch.EndAt
	ORDER BY MatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载广场赛配置
CREATE  PROCEDURE dbo.GSP_GS_LoadSquareMatchOption
	@wServerID INT,								-- 房间标识
	@wMatchID INT,								-- 比赛标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 加载广场赛
	SELECT SquareMatch.* FROM SquareMatch(NOLOCK) WHERE ServerID=@wServerID AND ((MatchID=@wMatchID) OR (@wMatchID=0))
	ORDER BY SquareName

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 加载广场赛奖励
CREATE  PROCEDURE dbo.GSP_GS_LoadSquareMatchRankBonus
	@wMatchID INT,								-- 比赛标识
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- 输出信息
WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 加载广场赛
	--SELECT SquareAward.* FROM SquareAward(NOLOCK) WHERE ServerID=@wServerID AND ((MatchID=@wMatchID) OR (@wMatchID=0))
	SELECT SquareAward.* FROM SquareAward(NOLOCK) WHERE MatchID=@wMatchID
	ORDER BY FirstRank

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------