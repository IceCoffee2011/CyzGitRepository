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

Date: 2016-12-21 09:57:16
*/


-- ----------------------------
-- Table structure for RecordDrawInfo
-- ----------------------------
DROP TABLE [dbo].[RecordDrawInfo]
GO
CREATE TABLE [dbo].[RecordDrawInfo] (
[DrawID] int NOT NULL IDENTITY(1,1) ,
[KindID] int NOT NULL ,
[ServerID] int NOT NULL ,
[TableID] int NOT NULL ,
[UserCount] int NOT NULL ,
[AndroidCount] int NOT NULL ,
[Waste] bigint NOT NULL ,
[Revenue] bigint NOT NULL ,
[UserMedal] int NOT NULL DEFAULT ((0)) ,
[StartTime] datetime NOT NULL ,
[ConcludeTime] datetime NOT NULL ,
[InsertTime] datetime NOT NULL DEFAULT (getdate()) ,
[DrawCourse] image NULL 
)


GO
DBCC CHECKIDENT(N'[dbo].[RecordDrawInfo]', RESEED, 1067)
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawInfo', 
'COLUMN', N'DrawID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'局数标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'DrawID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'局数标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'DrawID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawInfo', 
'COLUMN', N'KindID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'类型标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'KindID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'类型标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'KindID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawInfo', 
'COLUMN', N'ServerID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'房间标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'ServerID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'房间标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'ServerID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawInfo', 
'COLUMN', N'TableID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'桌子号码'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'TableID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'桌子号码'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'TableID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawInfo', 
'COLUMN', N'UserCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'UserCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'UserCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawInfo', 
'COLUMN', N'AndroidCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'机器数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'AndroidCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'机器数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'AndroidCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawInfo', 
'COLUMN', N'Waste')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'损耗数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'Waste'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'损耗数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'Waste'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawInfo', 
'COLUMN', N'Revenue')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'税收数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'Revenue'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'税收数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'Revenue'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawInfo', 
'COLUMN', N'UserMedal')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户奖牌'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'UserMedal'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户奖牌'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'UserMedal'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawInfo', 
'COLUMN', N'StartTime')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'开始时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'StartTime'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'开始时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'StartTime'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawInfo', 
'COLUMN', N'ConcludeTime')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'结束时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'ConcludeTime'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'结束时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'ConcludeTime'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawInfo', 
'COLUMN', N'InsertTime')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'插入时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'InsertTime'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'插入时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'InsertTime'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawInfo', 
'COLUMN', N'DrawCourse')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'游戏过程'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'DrawCourse'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'游戏过程'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawInfo'
, @level2type = 'COLUMN', @level2name = N'DrawCourse'
GO

-- ----------------------------
-- Indexes structure for table RecordDrawInfo
-- ----------------------------
CREATE INDEX [IX_RecordDrawInfo_ConcludeTime] ON [dbo].[RecordDrawInfo]
([ConcludeTime] ASC) 
GO
CREATE INDEX [IX_RecordDrawInfo_InsertTime] ON [dbo].[RecordDrawInfo]
([InsertTime] ASC) 
GO
CREATE INDEX [IX_RecordDrawInfo_StartTime] ON [dbo].[RecordDrawInfo]
([StartTime] ASC) 
GO

-- ----------------------------
-- Primary Key structure for table RecordDrawInfo
-- ----------------------------
ALTER TABLE [dbo].[RecordDrawInfo] ADD PRIMARY KEY ([DrawID])
GO
