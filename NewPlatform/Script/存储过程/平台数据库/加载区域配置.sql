
----------------------------------------------------------------------------------------------------

USE QPPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAreaGameKindItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAreaGameKindItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAreaGameServerItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAreaGameServerItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadGameKindItemEx]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadGameKindItemEx]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadCustomTablePropertyConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadCustomTablePropertyConfig]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

--游戏类型
CREATE  PROCEDURE dbo.GSP_GP_LoadAreaGameKindItem WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

--游戏类型
SELECT AreaID As AreaID, KindID As KindID, SortID As SortID, PlatformID As PlatformID FROM AreaGameKindItem(NOLOCK) ORDER BY AreaID

RETURN 0

GO

----------------------------------------------------------------------------------------------------

--游戏类型
CREATE  PROCEDURE dbo.GSP_GP_LoadAreaGameServerItem WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

--游戏类型
SELECT AreaID As AreaID, ServerID As ServerID, SortID As SortID, ServerName As ServerName, PlatformID As PlatformID FROM AreaGameServerItem(NOLOCK) ORDER BY AreaID

RETURN 0

GO

----------------------------------------------------------------------------------------------------

--游戏类型
CREATE  PROCEDURE dbo.GSP_GP_LoadGameKindItemEx WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

BEGIN
	--游戏类型
	SELECT a.KindID As KindID, b.AppClientVersion As AppClientVersion, a.AppDownLoadUrl As AppDownLoadUrl FROM GameKindItem(NOLOCK) a, GameGameItem(NOLOCK) b 
	WHERE a.GameID = b.GameID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

--游戏类型
CREATE  PROCEDURE dbo.GSP_GP_LoadCustomTablePropertyConfig WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

BEGIN
	--游戏类型
	SELECT KindID As KindID, PropertyConsumeCount As PropertyConsumeCount, RoundCount As RoundCount FROM QPUserCustomDB.dbo.GameKindProperty ORDER BY KindID
END

RETURN 0

GO
