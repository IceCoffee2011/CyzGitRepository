/*
Navicat SQL Server Data Transfer

Source Server         : 松滋App
Source Server Version : 105000
Source Host           : rds8nrwkdc7sb7uobv0fo.sqlserver.rds.aliyuncs.com,3433:1433
Source Database       : qpplatformdb
Source Schema         : dbo

Target Server Type    : SQL Server
Target Server Version : 105000
File Encoding         : 65001

Date: 2017-01-03 09:55:49
*/


-- ----------------------------
-- Table structure for AppShareInfo
-- ----------------------------
DROP TABLE [dbo].[AppShareInfo]
GO
CREATE TABLE [dbo].[AppShareInfo] (
[AreaID] int NOT NULL ,
[ShareTitle] nvarchar(1024) NULL DEFAULT '' ,
[ShareContent] nvarchar(1024) NULL DEFAULT '' ,
[ShareURL] nvarchar(1024) NULL DEFAULT '' 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AppShareInfo', 
'COLUMN', N'AreaID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'区域标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AppShareInfo'
, @level2type = 'COLUMN', @level2name = N'AreaID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'区域标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AppShareInfo'
, @level2type = 'COLUMN', @level2name = N'AreaID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AppShareInfo', 
'COLUMN', N'ShareTitle')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'社交分享标题'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AppShareInfo'
, @level2type = 'COLUMN', @level2name = N'ShareTitle'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'社交分享标题'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AppShareInfo'
, @level2type = 'COLUMN', @level2name = N'ShareTitle'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AppShareInfo', 
'COLUMN', N'ShareContent')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'社交分享文字'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AppShareInfo'
, @level2type = 'COLUMN', @level2name = N'ShareContent'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'社交分享文字'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AppShareInfo'
, @level2type = 'COLUMN', @level2name = N'ShareContent'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AppShareInfo', 
'COLUMN', N'ShareURL')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'社交分享网址'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AppShareInfo'
, @level2type = 'COLUMN', @level2name = N'ShareURL'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'社交分享网址'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AppShareInfo'
, @level2type = 'COLUMN', @level2name = N'ShareURL'
GO

-- ----------------------------
-- Indexes structure for table AppShareInfo
-- ----------------------------

-- ----------------------------
-- Primary Key structure for table AppShareInfo
-- ----------------------------
ALTER TABLE [dbo].[AppShareInfo] ADD PRIMARY KEY ([AreaID])
GO
