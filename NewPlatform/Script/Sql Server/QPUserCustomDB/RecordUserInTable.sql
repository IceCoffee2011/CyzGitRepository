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

Date: 2016-12-21 09:57:40
*/


-- ----------------------------
-- Table structure for RecordUserInTable
-- ----------------------------
DROP TABLE [dbo].[RecordUserInTable]
GO
CREATE TABLE [dbo].[RecordUserInTable] (
[UserID] int NOT NULL ,
[ServerID] int NULL ,
[TableID] int NULL ,
[ChairID] int NULL ,
[RoundCount] int NULL ,
[RoundScore] bigint NULL ,
[TableBuilder] int NULL ,
[Secret] nvarchar(9) NULL 
)


GO
