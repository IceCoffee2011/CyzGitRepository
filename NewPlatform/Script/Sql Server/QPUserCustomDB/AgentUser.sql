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

Date: 2016-12-27 12:14:04
*/


-- ----------------------------
-- Table structure for AgentUser
-- ----------------------------
DROP TABLE [dbo].[AgentUser]
GO
CREATE TABLE [dbo].[AgentUser] (
[AgentID] int NOT NULL IDENTITY(1,1) ,
[AgentName] varchar(50) NOT NULL ,
[AgentPass] varchar(50) NOT NULL ,
[CreateDate] datetime NOT NULL ,
[RealName] nvarchar(50) NOT NULL ,
[PhoneNumber] varchar(50) NOT NULL ,
[Area] nvarchar(50) NOT NULL ,
[City] nvarchar(50) NOT NULL ,
[AgentAddress] nvarchar(100) NOT NULL ,
[IsCancel] bit NOT NULL DEFAULT ((0)) ,
[WeiXinID] nvarchar(64) NOT NULL 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AgentUser', 
'COLUMN', N'WeiXinID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'代理的微信账号'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AgentUser'
, @level2type = 'COLUMN', @level2name = N'WeiXinID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'代理的微信账号'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AgentUser'
, @level2type = 'COLUMN', @level2name = N'WeiXinID'
GO

-- ----------------------------
-- Indexes structure for table AgentUser
-- ----------------------------

-- ----------------------------
-- Primary Key structure for table AgentUser
-- ----------------------------
ALTER TABLE [dbo].[AgentUser] ADD PRIMARY KEY ([AgentID])
GO
