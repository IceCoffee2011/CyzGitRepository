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

Date: 2016-12-21 09:57:51
*/


-- ----------------------------
-- Table structure for SystemStreamInfo
-- ----------------------------
DROP TABLE [dbo].[SystemStreamInfo]
GO
CREATE TABLE [dbo].[SystemStreamInfo] (
[DateID] int NOT NULL ,
[KindID] int NOT NULL ,
[ServerID] int NOT NULL ,
[LogonCount] int NOT NULL DEFAULT ((0)) ,
[RegisterCount] int NOT NULL DEFAULT ((0)) ,
[CollectDate] datetime NOT NULL DEFAULT (getdate()) 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'SystemStreamInfo', 
'COLUMN', N'DateID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'日期标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'SystemStreamInfo'
, @level2type = 'COLUMN', @level2name = N'DateID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'日期标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'SystemStreamInfo'
, @level2type = 'COLUMN', @level2name = N'DateID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'SystemStreamInfo', 
'COLUMN', N'KindID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'类型标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'SystemStreamInfo'
, @level2type = 'COLUMN', @level2name = N'KindID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'类型标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'SystemStreamInfo'
, @level2type = 'COLUMN', @level2name = N'KindID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'SystemStreamInfo', 
'COLUMN', N'ServerID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'房间标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'SystemStreamInfo'
, @level2type = 'COLUMN', @level2name = N'ServerID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'房间标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'SystemStreamInfo'
, @level2type = 'COLUMN', @level2name = N'ServerID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'SystemStreamInfo', 
'COLUMN', N'LogonCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'进入数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'SystemStreamInfo'
, @level2type = 'COLUMN', @level2name = N'LogonCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'进入数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'SystemStreamInfo'
, @level2type = 'COLUMN', @level2name = N'LogonCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'SystemStreamInfo', 
'COLUMN', N'RegisterCount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'注册数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'SystemStreamInfo'
, @level2type = 'COLUMN', @level2name = N'RegisterCount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'注册数目'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'SystemStreamInfo'
, @level2type = 'COLUMN', @level2name = N'RegisterCount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'SystemStreamInfo', 
'COLUMN', N'CollectDate')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'录入时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'SystemStreamInfo'
, @level2type = 'COLUMN', @level2name = N'CollectDate'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'录入时间'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'SystemStreamInfo'
, @level2type = 'COLUMN', @level2name = N'CollectDate'
GO

-- ----------------------------
-- Indexes structure for table SystemStreamInfo
-- ----------------------------

-- ----------------------------
-- Primary Key structure for table SystemStreamInfo
-- ----------------------------
ALTER TABLE [dbo].[SystemStreamInfo] ADD PRIMARY KEY ([DateID], [KindID], [ServerID])
GO
