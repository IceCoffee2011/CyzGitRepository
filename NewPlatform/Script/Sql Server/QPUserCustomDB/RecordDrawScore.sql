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

Date: 2016-12-21 09:57:23
*/


-- ----------------------------
-- Table structure for RecordDrawScore
-- ----------------------------
DROP TABLE [dbo].[RecordDrawScore]
GO
CREATE TABLE [dbo].[RecordDrawScore] (
[DrawID] int NOT NULL ,
[UserID] int NOT NULL ,
[ChairID] int NOT NULL DEFAULT ((0)) ,
[Score] bigint NOT NULL ,
[Grade] bigint NOT NULL ,
[Revenue] bigint NOT NULL ,
[UserMedal] int NOT NULL DEFAULT ((0)) ,
[PlayTimeCount] int NOT NULL DEFAULT ((0)) ,
[DBQuestID] int NOT NULL ,
[InoutIndex] int NOT NULL ,
[InsertTime] datetime NOT NULL DEFAULT (getdate()) 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawScore', 
'COLUMN', N'DrawID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'局数标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'DrawID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'局数标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'DrawID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawScore', 
'COLUMN', N'UserID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'UserID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'UserID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawScore', 
'COLUMN', N'ChairID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'椅子号码'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'ChairID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'椅子号码'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'ChairID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawScore', 
'COLUMN', N'Score')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户成绩'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'Score'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户成绩'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'Score'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawScore', 
'COLUMN', N'Grade')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户积分'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'Grade'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户积分'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'Grade'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawScore', 
'COLUMN', N'Revenue')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'税收数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'Revenue'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'税收数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'Revenue'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawScore', 
'COLUMN', N'UserMedal')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户奖牌'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'UserMedal'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户奖牌'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'UserMedal'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawScore', 
'COLUMN', N'PlayTimeCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'游戏时长'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'PlayTimeCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'游戏时长'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'PlayTimeCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawScore', 
'COLUMN', N'DBQuestID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'请求标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'DBQuestID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'请求标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'DBQuestID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawScore', 
'COLUMN', N'InoutIndex')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'进出索引'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'InoutIndex'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'进出索引'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'InoutIndex'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordDrawScore', 
'COLUMN', N'InsertTime')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'插入时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'InsertTime'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'插入时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordDrawScore'
, @level2type = 'COLUMN', @level2name = N'InsertTime'
GO

-- ----------------------------
-- Indexes structure for table RecordDrawScore
-- ----------------------------
CREATE INDEX [IX_RecordDrawScore_ChairID] ON [dbo].[RecordDrawScore]
([ChairID] ASC) 
GO
CREATE INDEX [IX_RecordDrawScore_DBQuestID] ON [dbo].[RecordDrawScore]
([DBQuestID] ASC) 
GO
CREATE INDEX [IX_RecordDrawScore_DrawID] ON [dbo].[RecordDrawScore]
([DrawID] ASC) 
GO
CREATE INDEX [IX_RecordDrawScore_InoutIndex] ON [dbo].[RecordDrawScore]
([InoutIndex] ASC) 
GO
CREATE INDEX [IX_RecordDrawScore_InsertTime] ON [dbo].[RecordDrawScore]
([InsertTime] ASC) 
GO
CREATE INDEX [IX_RecordDrawScore_UserID] ON [dbo].[RecordDrawScore]
([UserID] ASC) 
GO
