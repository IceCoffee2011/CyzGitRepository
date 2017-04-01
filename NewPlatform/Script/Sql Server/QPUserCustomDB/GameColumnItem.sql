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

Date: 2016-12-21 09:56:44
*/


-- ----------------------------
-- Table structure for GameColumnItem
-- ----------------------------
DROP TABLE [dbo].[GameColumnItem]
GO
CREATE TABLE [dbo].[GameColumnItem] (
[SortID] int NOT NULL ,
[ColumnName] nvarchar(15) NOT NULL ,
[ColumnWidth] tinyint NOT NULL ,
[DataDescribe] tinyint NOT NULL 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameColumnItem', 
'COLUMN', N'SortID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'排列标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameColumnItem'
, @level2type = 'COLUMN', @level2name = N'SortID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'排列标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameColumnItem'
, @level2type = 'COLUMN', @level2name = N'SortID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameColumnItem', 
'COLUMN', N'ColumnName')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'列头名字'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameColumnItem'
, @level2type = 'COLUMN', @level2name = N'ColumnName'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'列头名字'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameColumnItem'
, @level2type = 'COLUMN', @level2name = N'ColumnName'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameColumnItem', 
'COLUMN', N'ColumnWidth')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'列表宽度'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameColumnItem'
, @level2type = 'COLUMN', @level2name = N'ColumnWidth'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'列表宽度'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameColumnItem'
, @level2type = 'COLUMN', @level2name = N'ColumnWidth'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameColumnItem', 
'COLUMN', N'DataDescribe')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'数据描述'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameColumnItem'
, @level2type = 'COLUMN', @level2name = N'DataDescribe'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'数据描述'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameColumnItem'
, @level2type = 'COLUMN', @level2name = N'DataDescribe'
GO

-- ----------------------------
-- Indexes structure for table GameColumnItem
-- ----------------------------

-- ----------------------------
-- Primary Key structure for table GameColumnItem
-- ----------------------------
ALTER TABLE [dbo].[GameColumnItem] ADD PRIMARY KEY ([SortID])
GO
