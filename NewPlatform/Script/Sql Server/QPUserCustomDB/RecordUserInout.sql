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

Date: 2016-12-21 09:57:34
*/


-- ----------------------------
-- Table structure for RecordUserInout
-- ----------------------------
DROP TABLE [dbo].[RecordUserInout]
GO
CREATE TABLE [dbo].[RecordUserInout] (
[ID] int NOT NULL IDENTITY(1,1) ,
[UserID] int NOT NULL ,
[KindID] int NOT NULL ,
[ServerID] int NOT NULL ,
[EnterTime] datetime NOT NULL DEFAULT (getdate()) ,
[EnterScore] bigint NOT NULL ,
[EnterGrade] bigint NOT NULL DEFAULT ((0)) ,
[EnterInsure] bigint NOT NULL DEFAULT ((0)) ,
[EnterUserMedal] int NOT NULL DEFAULT ((0)) ,
[EnterLoveliness] int NOT NULL DEFAULT ((0)) ,
[EnterMachine] nvarchar(33) NOT NULL DEFAULT (N'') ,
[EnterClientIP] nvarchar(15) NOT NULL ,
[LeaveTime] datetime NULL ,
[LeaveReason] int NULL ,
[LeaveMachine] nvarchar(32) NULL ,
[LeaveClientIP] nvarchar(15) NULL ,
[Score] bigint NOT NULL DEFAULT ((0)) ,
[Grade] bigint NOT NULL DEFAULT ((0)) ,
[Insure] bigint NOT NULL DEFAULT ((0)) ,
[Revenue] bigint NOT NULL DEFAULT ((0)) ,
[WinCount] int NOT NULL DEFAULT ((0)) ,
[LostCount] int NOT NULL DEFAULT ((0)) ,
[DrawCount] int NOT NULL DEFAULT ((0)) ,
[FleeCount] int NOT NULL DEFAULT ((0)) ,
[UserMedal] int NOT NULL DEFAULT ((0)) ,
[LoveLiness] int NOT NULL DEFAULT ((0)) ,
[Experience] int NOT NULL DEFAULT ((0)) ,
[PlayTimeCount] int NOT NULL DEFAULT ((0)) ,
[OnLineTimeCount] int NOT NULL DEFAULT ((0)) 
)


GO
DBCC CHECKIDENT(N'[dbo].[RecordUserInout]', RESEED, 28376)
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'ID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'索引标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'ID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'索引标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'ID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'UserID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'UserID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'UserID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'KindID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'类型标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'KindID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'类型标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'KindID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'ServerID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'房间标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'ServerID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'房间标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'ServerID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'EnterTime')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'进入时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterTime'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'进入时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterTime'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'EnterScore')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'进入积分'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterScore'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'进入积分'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterScore'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'EnterGrade')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'进入成绩'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterGrade'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'进入成绩'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterGrade'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'EnterInsure')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'进入银行'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterInsure'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'进入银行'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterInsure'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'EnterUserMedal')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'进入奖牌'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterUserMedal'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'进入奖牌'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterUserMedal'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'EnterLoveliness')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'进入魅力'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterLoveliness'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'进入魅力'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterLoveliness'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'EnterMachine')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'进入机器'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterMachine'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'进入机器'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterMachine'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'EnterClientIP')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'登录地址'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterClientIP'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'登录地址'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'EnterClientIP'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'LeaveTime')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'离开时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'LeaveTime'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'离开时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'LeaveTime'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'LeaveReason')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'离开原因'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'LeaveReason'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'离开原因'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'LeaveReason'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'LeaveMachine')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'进入机器'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'LeaveMachine'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'进入机器'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'LeaveMachine'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'LeaveClientIP')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'登录地址'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'LeaveClientIP'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'登录地址'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'LeaveClientIP'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'Score')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'变更积分'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'Score'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'变更积分'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'Score'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'Grade')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'成绩变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'Grade'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'成绩变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'Grade'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'Insure')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'银行变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'Insure'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'银行变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'Insure'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'Revenue')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'变更税收'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'Revenue'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'变更税收'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'Revenue'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'WinCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'胜局变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'WinCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'胜局变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'WinCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'LostCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'输局变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'LostCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'输局变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'LostCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'DrawCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'和局变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'DrawCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'和局变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'DrawCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'FleeCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'逃局变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'FleeCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'逃局变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'FleeCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'UserMedal')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'奖牌数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'UserMedal'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'奖牌数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'UserMedal'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'LoveLiness')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'魅力变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'LoveLiness'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'魅力变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'LoveLiness'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'Experience')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'经验变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'Experience'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'经验变更'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'Experience'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'PlayTimeCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'游戏时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'PlayTimeCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'游戏时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'PlayTimeCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserInout', 
'COLUMN', N'OnLineTimeCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'在线时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'OnLineTimeCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'在线时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserInout'
, @level2type = 'COLUMN', @level2name = N'OnLineTimeCount'
GO

-- ----------------------------
-- Indexes structure for table RecordUserInout
-- ----------------------------
CREATE INDEX [IX_RecordUserInout_EnterTime] ON [dbo].[RecordUserInout]
([EnterTime] ASC) 
GO
CREATE INDEX [IX_RecordUserInout_KindID] ON [dbo].[RecordUserInout]
([KindID] ASC) 
GO
CREATE INDEX [IX_RecordUserInout_ServerID] ON [dbo].[RecordUserInout]
([ServerID] ASC) 
GO
CREATE INDEX [IX_RecordUserInout_UserID] ON [dbo].[RecordUserInout]
([UserID] ASC) 
GO

-- ----------------------------
-- Primary Key structure for table RecordUserInout
-- ----------------------------
ALTER TABLE [dbo].[RecordUserInout] ADD PRIMARY KEY ([ID])
GO
