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

Date: 2016-12-21 09:56:57
*/


-- ----------------------------
-- Table structure for GameProperty
-- ----------------------------
DROP TABLE [dbo].[GameProperty]
GO
CREATE TABLE [dbo].[GameProperty] (
[ID] int NOT NULL ,
[Name] nvarchar(31) NOT NULL ,
[Cash] decimal(18,2) NOT NULL ,
[Gold] bigint NOT NULL ,
[Discount] smallint NOT NULL DEFAULT ((90)) ,
[IssueArea] smallint NOT NULL DEFAULT ((3)) ,
[ServiceArea] smallint NOT NULL ,
[SendLoveLiness] bigint NOT NULL ,
[RecvLoveLiness] bigint NOT NULL ,
[RegulationsInfo] nvarchar(255) NOT NULL ,
[Nullity] tinyint NOT NULL DEFAULT ((0)) 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameProperty', 
'COLUMN', N'ID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'道具标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'ID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'道具标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'ID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameProperty', 
'COLUMN', N'Name')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'道具名字'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'Name'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'道具名字'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'Name'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameProperty', 
'COLUMN', N'Cash')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'道具价格'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'Cash'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'道具价格'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'Cash'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameProperty', 
'COLUMN', N'Gold')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'道具金币'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'Gold'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'道具金币'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'Gold'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameProperty', 
'COLUMN', N'Discount')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'会员折扣'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'Discount'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'会员折扣'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'Discount'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameProperty', 
'COLUMN', N'IssueArea')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'发行范围'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'IssueArea'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'发行范围'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'IssueArea'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameProperty', 
'COLUMN', N'ServiceArea')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'使用范围'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'ServiceArea'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'使用范围'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'ServiceArea'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameProperty', 
'COLUMN', N'SendLoveLiness')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'增加魅力'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'SendLoveLiness'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'增加魅力'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'SendLoveLiness'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameProperty', 
'COLUMN', N'RecvLoveLiness')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'增加魅力'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'RecvLoveLiness'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'增加魅力'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'RecvLoveLiness'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameProperty', 
'COLUMN', N'RegulationsInfo')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'使用说明'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'RegulationsInfo'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'使用说明'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'RegulationsInfo'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'GameProperty', 
'COLUMN', N'Nullity')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'禁止标志'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'Nullity'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'禁止标志'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'GameProperty'
, @level2type = 'COLUMN', @level2name = N'Nullity'
GO

-- ----------------------------
-- Indexes structure for table GameProperty
-- ----------------------------

-- ----------------------------
-- Primary Key structure for table GameProperty
-- ----------------------------
ALTER TABLE [dbo].[GameProperty] ADD PRIMARY KEY ([ID])
GO
