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

Date: 2016-12-21 09:57:10
*/


-- ----------------------------
-- Table structure for GameScoreLocker
-- ----------------------------
DROP TABLE [dbo].[GameScoreLocker]
GO
CREATE TABLE [dbo].[GameScoreLocker] (
[UserID] int NOT NULL ,
[KindID] int NOT NULL ,
[ServerID] int NOT NULL ,
[EnterID] int NOT NULL ,
[EnterIP] nvarchar(15) NOT NULL DEFAULT (N'') ,
[EnterMachine] nvarchar(32) NOT NULL DEFAULT (N'') ,
[CollectDate] datetime NOT NULL DEFAULT (getdate()) 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreLocker', 
'COLUMN', N'UserID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户索引'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'UserID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户索引'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'UserID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreLocker', 
'COLUMN', N'KindID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'房间索引'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'KindID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'房间索引'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'KindID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreLocker', 
'COLUMN', N'ServerID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'游戏标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'ServerID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'游戏标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'ServerID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreLocker', 
'COLUMN', N'EnterID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'进出索引'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'EnterID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'进出索引'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'EnterID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreLocker', 
'COLUMN', N'EnterIP')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'进入地址'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'EnterIP'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'进入地址'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'EnterIP'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreLocker', 
'COLUMN', N'EnterMachine')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'进入机器'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'EnterMachine'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'进入机器'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'EnterMachine'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameScoreLocker', 
'COLUMN', N'CollectDate')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'录入日期'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'CollectDate'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'录入日期'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameScoreLocker'
, @level2type = 'COLUMN', @level2name = N'CollectDate'
GO

-- ----------------------------
-- Indexes structure for table GameScoreLocker
-- ----------------------------
CREATE INDEX [IX_GameScoreLocker_UserID_ServerID] ON [dbo].[GameScoreLocker]
([UserID] ASC, [ServerID] ASC) 
GO

-- ----------------------------
-- Primary Key structure for table GameScoreLocker
-- ----------------------------
ALTER TABLE [dbo].[GameScoreLocker] ADD PRIMARY KEY ([UserID], [KindID], [ServerID], [EnterID])
GO
