
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

--��ȡ��ֵ����
CREATE  PROCEDURE GSP_MB_ReChargeMessage
WITH ENCRYPTION AS

--��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @StatusString NVARCHAR(512)

--������Ϣ
SELECT @StatusString=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'ReChargeMessage'

-- �������
SELECT @StatusString AS StatusString

RETURN 0

GO

----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------

--��ȡ�ͷ�����
CREATE  PROCEDURE GSP_MB_CustomServiceMessage
WITH ENCRYPTION AS

--��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @StatusString NVARCHAR(512)

--������Ϣ
SELECT @StatusString=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'CustomServiceMessage'

-- �������
SELECT @StatusString AS StatusString

RETURN 0

GO

----------------------------------------------------------------------------------------------------
