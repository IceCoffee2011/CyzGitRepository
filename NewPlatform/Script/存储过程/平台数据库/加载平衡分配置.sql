
----------------------------------------------------------------------------------------------------

USE QPPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadBalanceScoreCurve]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadBalanceScoreCurve]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

--读取系统消息
CREATE  PROCEDURE dbo.GSP_GR_LoadBalanceScoreCurve 
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

SELECT Score AS Score, BalanceScore As BalanceScore FROM BalanceScoreCurve

RETURN 0

GO

----------------------------------------------------------------------------------------------------
