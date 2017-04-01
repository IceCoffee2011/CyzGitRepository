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

Date: 2016-12-21 09:56:50
*/


-- ----------------------------
-- Table structure for GameKindProperty
-- ----------------------------
DROP TABLE [dbo].[GameKindProperty]
GO
CREATE TABLE [dbo].[GameKindProperty] (
[KindID] int NOT NULL ,
[PropertyConsumeCount] int NOT NULL ,
[RoundCount] int NOT NULL 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameKindProperty', 
'COLUMN', N'PropertyConsumeCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'消耗的道具数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameKindProperty'
, @level2type = 'COLUMN', @level2name = N'PropertyConsumeCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'消耗的道具数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameKindProperty'
, @level2type = 'COLUMN', @level2name = N'PropertyConsumeCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameKindProperty', 
'COLUMN', N'RoundCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'可以创建的游戏局数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameKindProperty'
, @level2type = 'COLUMN', @level2name = N'RoundCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'可以创建的游戏局数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameKindProperty'
, @level2type = 'COLUMN', @level2name = N'RoundCount'
GO

-- ----------------------------
-- Indexes structure for table GameKindProperty
-- ----------------------------

-- ----------------------------
-- Primary Key structure for table GameKindProperty
-- ----------------------------
ALTER TABLE [dbo].[GameKindProperty] ADD PRIMARY KEY ([KindID], [PropertyConsumeCount])
GO
