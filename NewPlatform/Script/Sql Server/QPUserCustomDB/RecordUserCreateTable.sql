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

Date: 2016-12-21 09:57:28
*/


-- ----------------------------
-- Table structure for RecordUserCreateTable
-- ----------------------------
DROP TABLE [dbo].[RecordUserCreateTable]
GO
CREATE TABLE [dbo].[RecordUserCreateTable] (
[UserID] int NOT NULL ,
[KindID] int NOT NULL ,
[ServerID] int NOT NULL ,
[TableID] int NOT NULL ,
[CreateDateTime] datetime NOT NULL ,
[Secret] nvarchar(9) NOT NULL ,
[CustomRule] nvarchar(2048) NOT NULL ,
[RoundCount] int NOT NULL ,
[ConludeDateTime] datetime NOT NULL DEFAULT (getdate()) ,
[ConludeType] int NOT NULL 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserCreateTable', 
'COLUMN', N'ConludeDateTime')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'桌子结算时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserCreateTable'
, @level2type = 'COLUMN', @level2name = N'ConludeDateTime'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'桌子结算时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserCreateTable'
, @level2type = 'COLUMN', @level2name = N'ConludeDateTime'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'RecordUserCreateTable', 
'COLUMN', N'ConludeType')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'结算类型(1:打满局数,正常结算; 2:未开始游戏,创建者解散; 3:未开始游戏,到时间自动解散; 4:游戏中玩家申请解散;)'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserCreateTable'
, @level2type = 'COLUMN', @level2name = N'ConludeType'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'结算类型(1:打满局数,正常结算; 2:未开始游戏,创建者解散; 3:未开始游戏,到时间自动解散; 4:游戏中玩家申请解散;)'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'RecordUserCreateTable'
, @level2type = 'COLUMN', @level2name = N'ConludeType'
GO
