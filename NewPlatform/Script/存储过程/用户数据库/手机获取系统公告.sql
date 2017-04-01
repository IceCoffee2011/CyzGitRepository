
----------------------------------------------------------------------------------------------------

USE QPAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_ReChargeMessage]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_ReChargeMessage]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_CustomServiceMessage]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_CustomServiceMessage]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

--获取充值公告
CREATE  PROCEDURE GSP_MB_ReChargeMessage
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

-- 基本信息
DECLARE @StatusString NVARCHAR(512)

--插入信息
SELECT @StatusString=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'ReChargeMessage'

-- 输出变量
SELECT @StatusString AS StatusString

RETURN 0

GO

----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------

--获取客服公告
CREATE  PROCEDURE GSP_MB_CustomServiceMessage
WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

-- 基本信息
DECLARE @StatusString NVARCHAR(512)

--插入信息
SELECT @StatusString=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'CustomServiceMessage'

-- 输出变量
SELECT @StatusString AS StatusString

RETURN 0

GO

----------------------------------------------------------------------------------------------------
