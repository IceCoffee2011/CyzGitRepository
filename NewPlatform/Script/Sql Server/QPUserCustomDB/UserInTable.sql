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

Date: 2016-12-21 09:58:02
*/


-- ----------------------------
-- Table structure for UserInTable
-- ----------------------------
DROP TABLE [dbo].[UserInTable]
GO
CREATE TABLE [dbo].[UserInTable] (
[UserID] int NOT NULL ,
[TableID] int NOT NULL ,
[RoundCount] int NOT NULL ,
[ServerID] int NOT NULL ,
[ChairID] int NOT NULL ,
[RoundScore] bigint NOT NULL DEFAULT ((0)) ,
[TableBuilder] int NOT NULL DEFAULT ((0)) 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserInTable', 
'COLUMN', N'UserID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserInTable'
, @level2type = 'COLUMN', @level2name = N'UserID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserInTable'
, @level2type = 'COLUMN', @level2name = N'UserID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserInTable', 
'COLUMN', N'TableID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'桌号标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserInTable'
, @level2type = 'COLUMN', @level2name = N'TableID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'桌号标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserInTable'
, @level2type = 'COLUMN', @level2name = N'TableID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserInTable', 
'COLUMN', N'RoundCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'游戏局数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserInTable'
, @level2type = 'COLUMN', @level2name = N'RoundCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'游戏局数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserInTable'
, @level2type = 'COLUMN', @level2name = N'RoundCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserInTable', 
'COLUMN', N'ServerID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'房间标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserInTable'
, @level2type = 'COLUMN', @level2name = N'ServerID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'房间标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserInTable'
, @level2type = 'COLUMN', @level2name = N'ServerID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserInTable', 
'COLUMN', N'RoundScore')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'玩家在自建桌子的局数内的分数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserInTable'
, @level2type = 'COLUMN', @level2name = N'RoundScore'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'玩家在自建桌子的局数内的分数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserInTable'
, @level2type = 'COLUMN', @level2name = N'RoundScore'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserInTable', 
'COLUMN', N'TableBuilder')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'是否是桌子建造者'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserInTable'
, @level2type = 'COLUMN', @level2name = N'TableBuilder'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'是否是桌子建造者'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserInTable'
, @level2type = 'COLUMN', @level2name = N'TableBuilder'
GO

-- ----------------------------
-- Indexes structure for table UserInTable
-- ----------------------------

-- ----------------------------
-- Primary Key structure for table UserInTable
-- ----------------------------
ALTER TABLE [dbo].[UserInTable] ADD PRIMARY KEY ([UserID])
GO
