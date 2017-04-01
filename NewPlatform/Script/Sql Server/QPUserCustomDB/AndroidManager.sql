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

Date: 2016-12-21 09:56:36
*/


-- ----------------------------
-- Table structure for AndroidManager
-- ----------------------------
DROP TABLE [dbo].[AndroidManager]
GO
CREATE TABLE [dbo].[AndroidManager] (
[UserID] int NOT NULL ,
[ServerID] int NOT NULL ,
[MinPlayDraw] int NOT NULL DEFAULT ((0)) ,
[MaxPlayDraw] int NOT NULL DEFAULT ((0)) ,
[MinTakeScore] bigint NOT NULL DEFAULT ((0)) ,
[MaxTakeScore] bigint NOT NULL DEFAULT ((0)) ,
[MinReposeTime] int NOT NULL DEFAULT ((0)) ,
[MaxReposeTime] int NOT NULL DEFAULT ((0)) ,
[ServiceTime] int NOT NULL DEFAULT ((0)) ,
[ServiceGender] int NOT NULL DEFAULT ((0)) ,
[Nullity] tinyint NOT NULL DEFAULT ((0)) ,
[CreateDate] datetime NOT NULL DEFAULT (getdate()) ,
[AndroidNote] nvarchar(128) NOT NULL DEFAULT (N'') ,
[MinSitRate] int NOT NULL DEFAULT ((1)) ,
[MaxSitRate] int NOT NULL DEFAULT ((1000)) ,
[SitRate] int NOT NULL DEFAULT ((1000)) 
)


GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AndroidManager', 
'COLUMN', N'UserID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'用户标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'UserID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'用户标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'UserID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AndroidManager', 
'COLUMN', N'ServerID')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'房间标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'ServerID'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'房间标识'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'ServerID'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AndroidManager', 
'COLUMN', N'MinPlayDraw')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'最少局数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'MinPlayDraw'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'最少局数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'MinPlayDraw'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AndroidManager', 
'COLUMN', N'MaxPlayDraw')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'最大局数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'MaxPlayDraw'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'最大局数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'MaxPlayDraw'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AndroidManager', 
'COLUMN', N'MinTakeScore')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'最少分数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'MinTakeScore'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'最少分数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'MinTakeScore'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AndroidManager', 
'COLUMN', N'MaxTakeScore')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'最高分数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'MaxTakeScore'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'最高分数'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'MaxTakeScore'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AndroidManager', 
'COLUMN', N'MinReposeTime')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'最少休息'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'MinReposeTime'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'最少休息'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'MinReposeTime'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AndroidManager', 
'COLUMN', N'MaxReposeTime')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'最大休息'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'MaxReposeTime'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'最大休息'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'MaxReposeTime'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AndroidManager', 
'COLUMN', N'ServiceGender')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'服务类型'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'ServiceGender'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'服务类型'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'ServiceGender'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AndroidManager', 
'COLUMN', N'Nullity')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'禁止标志'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'Nullity'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'禁止标志'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'Nullity'
GO
IF ((SELECT COUNT(*) from fn_listextendedproperty('MS_Description', 
'SCHEMA', N'dbo', 
'TABLE', N'AndroidManager', 
'COLUMN', N'AndroidNote')) > 0) 
EXEC sp_updateextendedproperty @name = N'MS_Description', @value = N'备注信息'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'AndroidNote'
ELSE
EXEC sp_addextendedproperty @name = N'MS_Description', @value = N'备注信息'
, @level0type = 'SCHEMA', @level0name = N'dbo'
, @level1type = 'TABLE', @level1name = N'AndroidManager'
, @level2type = 'COLUMN', @level2name = N'AndroidNote'
GO

-- ----------------------------
-- Indexes structure for table AndroidManager
-- ----------------------------
CREATE INDEX [IX_AndroidManager_Nullity] ON [dbo].[AndroidManager]
([Nullity] ASC) 
GO

-- ----------------------------
-- Primary Key structure for table AndroidManager
-- ----------------------------
ALTER TABLE [dbo].[AndroidManager] ADD PRIMARY KEY ([UserID], [ServerID])
GO
