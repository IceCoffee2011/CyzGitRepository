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

Date: 2016-12-27 12:13:56
*/


-- ----------------------------
-- Table structure for AgentCoin
-- ----------------------------
DROP TABLE [dbo].[AgentCoin]
GO
CREATE TABLE [dbo].[AgentCoin] (
[ID] int NOT NULL IDENTITY(1,1) ,
[AgentID] int NOT NULL ,
[CoinCount] int NOT NULL DEFAULT ((0)) 
)


GO

-- ----------------------------
-- Indexes structure for table AgentCoin
-- ----------------------------

-- ----------------------------
-- Primary Key structure for table AgentCoin
-- ----------------------------
ALTER TABLE [dbo].[AgentCoin] ADD PRIMARY KEY ([ID])
GO
