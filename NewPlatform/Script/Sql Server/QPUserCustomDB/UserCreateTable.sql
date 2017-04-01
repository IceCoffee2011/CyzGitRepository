/*
Navicat SQL Server Data Transfer

Source Server         : 120.26.86.241
Source Server Version : 105000
Source Host           : 120.26.86.241:1433
Source Database       : QPUserCustomDB
Source Schema         : dbo

Target Server Type    : SQL Server
Target Server Version : 105000
File Encoding         : 65001

Date: 2016-12-28 18:56:57
*/


-- ----------------------------
-- Table structure for UserCreateTable
-- ----------------------------
DROP TABLE [dbo].[UserCreateTable]
GO
CREATE TABLE [dbo].[UserCreateTable] (
[UserID] int NOT NULL ,
[KindID] int NOT NULL ,
[ServerID] int NOT NULL ,
[TableID] int NOT NULL ,
[CreateDateTime] datetime NOT NULL DEFAULT (getdate()) ,
[Secret] nvarchar(9) NOT NULL DEFAULT '' ,
[CustomRule] nvarchar(2048) NOT NULL DEFAULT '' ,
[RoundCount] int NOT NULL 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserCreateTable', 
'COLUMN', N'UserID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'UserID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'UserID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserCreateTable', 
'COLUMN', N'KindID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'游戏类型标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'KindID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'游戏类型标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'KindID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserCreateTable', 
'COLUMN', N'ServerID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'房间号码'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'ServerID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'房间号码'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'ServerID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserCreateTable', 
'COLUMN', N'TableID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'桌子标识(从0开始)'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'TableID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'桌子标识(从0开始)'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'TableID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserCreateTable', 
'COLUMN', N'Secret')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'桌子进入密钥'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'Secret'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'桌子进入密钥'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'Secret'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserCreateTable', 
'COLUMN', N'CustomRule')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'自定义规则'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'CustomRule'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'自定义规则'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'CustomRule'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserCreateTable', 
'COLUMN', N'RoundCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'创建桌子的局数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'RoundCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'创建桌子的局数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserCreateTable'
, @level2type = 'COLUMN', @level2name = N'RoundCount'
GO

-- ----------------------------
-- Indexes structure for table UserCreateTable
-- ----------------------------

-- ----------------------------
-- Primary Key structure for table UserCreateTable
-- ----------------------------
ALTER TABLE [dbo].[UserCreateTable] ADD PRIMARY KEY ([Secret], [ServerID], [TableID])
GO
