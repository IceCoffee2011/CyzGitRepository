/*
Navicat SQL Server Data Transfer

Source Server         : 172.168.1.3
Source Server Version : 105000
Source Host           : 172.168.1.3:1433
Source Database       : QPUserCustomDB
Source Schema         : dbo

Target Server Type    : SQL Server
Target Server Version : 105000
File Encoding         : 65001

Date: 2016-12-28 15:35:57
*/


-- ----------------------------
-- Table structure for RecordUserProperty
-- ----------------------------
DROP TABLE [dbo].[RecordUserProperty]
GO
CREATE TABLE [dbo].[RecordUserProperty] (
[SourceUserID] int NOT NULL DEFAULT ((0)) ,
[OperateType] int NOT NULL DEFAULT ((0)) ,
[PropertyCount] int NOT NULL ,
[RecordTime] datetime NOT NULL DEFAULT (getdate()) ,
[TargetUserID] int NOT NULL DEFAULT ((0)) ,
[SourceDiamond] int NOT NULL DEFAULT ((0)) ,
[TargetDiamond] int NOT NULL DEFAULT ((0)) ,
[ClientIP] nvarchar(129) NOT NULL DEFAULT ('000.000.000.000') ,
[CollectNote] nvarchar(63) NOT NULL DEFAULT '' ,
[RecordID] bigint NOT NULL IDENTITY(1,1) 
)


GO
DBCC CHECKIDENT(N'[dbo].[RecordUserProperty]', RESEED, 6)
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserProperty', 
'COLUMN', N'SourceUserID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'源玩家标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'SourceUserID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'源玩家标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'SourceUserID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserProperty', 
'COLUMN', N'OperateType')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'玩家操作类型(1:使用, 2:退回, 3:重置加入 4:转账)'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'OperateType'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'玩家操作类型(1:使用, 2:退回, 3:重置加入 4:转账)'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'OperateType'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserProperty', 
'COLUMN', N'PropertyCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'道具数量'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'PropertyCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'道具数量'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'PropertyCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserProperty', 
'COLUMN', N'RecordTime')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'记录时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'RecordTime'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'记录时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'RecordTime'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserProperty', 
'COLUMN', N'TargetUserID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'目标用户ID'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'TargetUserID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'目标用户ID'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'TargetUserID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserProperty', 
'COLUMN', N'SourceDiamond')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'源玩家钻石数量'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'SourceDiamond'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'源玩家钻石数量'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'SourceDiamond'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserProperty', 
'COLUMN', N'TargetDiamond')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'目标玩家钻石数量'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'TargetDiamond'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'目标玩家钻石数量'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'TargetDiamond'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserProperty', 
'COLUMN', N'ClientIP')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'玩家IP地址'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'ClientIP'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'玩家IP地址'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserProperty'
, @level2type = 'COLUMN', @level2name = N'ClientIP'
GO

-- ----------------------------
-- Indexes structure for table RecordUserProperty
-- ----------------------------
CREATE INDEX [IX_RecordUserProperty_UserID] ON [dbo].[RecordUserProperty]
([SourceUserID] ASC) 
GO

-- ----------------------------
-- Primary Key structure for table RecordUserProperty
-- ----------------------------
ALTER TABLE [dbo].[RecordUserProperty] ADD PRIMARY KEY ([RecordID])
GO
