
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

-- ������Ϸ��¼
CREATE PROC GSP_GR_RecordFishDrawInfo

	-- ������Ϣ
	@wDrawID INT,								-- ���� I D
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D

	-- ������Ϣ
	@wTableID INT,								-- ���Ӻ���
	@wUserID INT,								-- �û� I D
	@wChairID INT,								-- ���Ӻ���

	-- ��Ϸ��Ϣ
	@lScore BIGINT								-- ���·ֻ�����Ŀ
	
	-- ������Ϣ
	--@lSourceGold BIGINT,							-- ����ǰ���ϻ���
	--@lSourceBank BIGINT,							-- ����ǰ����ʣ�����
	--@lTargetGold BIGINT,						-- ���������ϻ���
	--@lTargetBank BIGINT							-- ����������ʣ�����

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ұ���
	DECLARE @lTargetGold BIGINT
	DECLARE @lTargetBank BIGINT

	-- ��ѯ����
	SELECT @lTargetGold=Score, @lTargetBank=InsureScore FROM QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@wUserID

	-- �����¼
	INSERT QPRecordDB.dbo.RecordFishDrawInfo(DrawID,KindID,ServerID,TableID,UserID,ChairID,Score,TargetGold,TargetBank)
	VALUES (@wDrawID,@wKindID,@wServerID,@wTableID,@wUserID,@wChairID,@lScore,@lTargetGold,@lTargetBank)
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
