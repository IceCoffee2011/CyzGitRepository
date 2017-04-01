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

Date: 2016-12-21 09:57:03
*/


-- ----------------------------
-- Table structure for GameScoreInfo
-- ----------------------------
DROP TABLE [dbo].[GameScoreInfo]
GO
CREATE TABLE [dbo].[GameScoreInfo] (
[UserID] int NOT NULL ,
[Score] bigint NOT NULL DEFAULT ((1000000)) ,
[Revenue] bigint NULL DEFAULT ((0)) ,
[InsureScore] bigint NULL DEFAULT ((0)) ,
[WinCount] int NULL DEFAULT ((0)) ,
[LostCount] int NULL DEFAULT ((0)) ,
[DrawCount] int NULL DEFAULT ((0)) ,
[FleeCount] int NULL DEFAULT ((0)) ,
[UserRight] int NULL DEFAULT ((0)) ,
[MasterRight] int NULL DEFAULT ((0)) ,
[MasterOrder] tinyint NULL DEFAULT ((0)) ,
[AllLogonTimes] int NULL DEFAULT ((0)) ,
[PlayTimeCount] int NULL DEFAULT ((0)) ,
[OnLineTimeCount] int NULL DEFAULT ((0)) ,
[LastLogonIP] nvarchar(15) NULL DEFAULT (N'') ,
[LastLogonDate] datetime NULL DEFAULT (getdate()) ,
[LastLogonMachine] nvarchar(32) NULL DEFAULT ('------------') ,
[RegisterIP] nvarchar(15) NULL DEFAULT (N'') ,
[RegisterDate] datetime NULL DEFAULT (getdate()) ,
[RegisterMachine] nvarchar(32) NULL DEFAULT (N'------------') ,
[TotalWin] bigint NULL DEFAULT ((0)) ,
[TotalLose] bigint NULL DEFAULT ((0)) 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'UserID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户 ID'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'UserID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户 ID'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'UserID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'Score')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户积分（货币）'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'Score'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户积分（货币）'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'Score'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'Revenue')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'游戏税收'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'Revenue'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'游戏税收'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'Revenue'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'InsureScore')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'银行金币'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'InsureScore'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'银行金币'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'InsureScore'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'WinCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'胜局数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'WinCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'胜局数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'WinCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'LostCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'输局数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'LostCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'输局数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'LostCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'DrawCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'和局数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'DrawCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'和局数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'DrawCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'FleeCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'逃局数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'FleeCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'逃局数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'FleeCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'UserRight')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户权限'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'UserRight'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户权限'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'UserRight'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'MasterRight')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'管理权限'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'MasterRight'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'管理权限'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'MasterRight'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'MasterOrder')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'管理等级'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'MasterOrder'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'管理等级'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'MasterOrder'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'AllLogonTimes')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'总登陆次数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'AllLogonTimes'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'总登陆次数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'AllLogonTimes'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'PlayTimeCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'游戏时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'PlayTimeCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'游戏时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'PlayTimeCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'OnLineTimeCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'在线时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'OnLineTimeCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'在线时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'OnLineTimeCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'LastLogonIP')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'上次登陆 IP'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'LastLogonIP'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'上次登陆 IP'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'LastLogonIP'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'LastLogonDate')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'上次登陆时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'LastLogonDate'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'上次登陆时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'LastLogonDate'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'LastLogonMachine')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'登录机器'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'LastLogonMachine'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'登录机器'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'LastLogonMachine'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'RegisterIP')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'注册 IP'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'RegisterIP'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'注册 IP'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'RegisterIP'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'RegisterDate')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'注册时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'RegisterDate'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'注册时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'RegisterDate'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'RegisterMachine')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'注册机器'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'RegisterMachine'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'注册机器'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'RegisterMachine'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'TotalWin')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'玩家赢量总计'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'TotalWin'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'玩家赢量总计'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'TotalWin'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreInfo', 
'COLUMN', N'TotalLose')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'玩家输量总计'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'TotalLose'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'玩家输量总计'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreInfo'
, @level2type = 'COLUMN', @level2name = N'TotalLose'
GO

-- ----------------------------
-- Indexes structure for table GameScoreInfo
-- ----------------------------

-- ----------------------------
-- Primary Key structure for table GameScoreInfo
-- ----------------------------
ALTER TABLE [dbo].[GameScoreInfo] ADD PRIMARY KEY ([UserID])
GO
