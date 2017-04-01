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

Date: 2016-12-21 09:58:08
*/


-- ----------------------------
-- Table structure for UserProperty
-- ----------------------------
DROP TABLE [dbo].[UserProperty]
GO
CREATE TABLE [dbo].[UserProperty] (
[UserID] int NOT NULL ,
[PropertyID] int NOT NULL DEFAULT ((27)) ,
[PropertyCount] int NOT NULL DEFAULT ((0)) 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserProperty', 
'COLUMN', N'UserID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserProperty'
, @level2type = 'COLUMN', @level2name = N'UserID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserProperty'
, @level2type = 'COLUMN', @level2name = N'UserID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserProperty', 
'COLUMN', N'PropertyID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'道具标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserProperty'
, @level2type = 'COLUMN', @level2name = N'PropertyID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'道具标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserProperty'
, @level2type = 'COLUMN', @level2name = N'PropertyID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'UserProperty', 
'COLUMN', N'PropertyCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户道具数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserProperty'
, @level2type = 'COLUMN', @level2name = N'PropertyCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户道具数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'UserProperty'
, @level2type = 'COLUMN', @level2name = N'PropertyCount'
GO

-- ----------------------------
-- Indexes structure for table UserProperty
-- ----------------------------

-- ----------------------------
-- Primary Key structure for table UserProperty
-- ----------------------------
ALTER TABLE [dbo].[UserProperty] ADD PRIMARY KEY ([UserID])
GO
