#ifndef SQUARE_MATCH_MANAGER_HEAD_FILE
#define SQUARE_MATCH_MANAGER_HEAD_FILE
#pragma once

#include "DataBasePacket.h"
#include <map>
#include <vector>
using namespace std;

struct tagSCORERANK
{
	DWORD							dwKindID;							//用户标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	LONGLONG						lScore;								//分数
};

struct tagSQUARERANK
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	WORD							wResult;							//有效结果
	LONGLONG						lScore;								//分数
	DWORD							dwEnterTime;						//进入时间
};

struct tagRANKBONUSSTRING
{
	WORD							wFirst;								//起始排名
	WORD							wLast;								//末尾排名

	TCHAR							szBonus[100];
	TCHAR							szDesc[100];
};

// 广场赛
class CSquare
{
public:
	CSquare();
	virtual ~CSquare();

public:
	//状态变量
	BOOL							m_bStartSquare;						//广场赛是否开始
	BOOL							m_bEndSquare;						//广场赛是否已经结束

	DWORD							m_dwSquareRemain;					//广场赛剩余时间（秒）
	WORD							m_dwAnnounceInterval;				//广场赛时间间隔
	map<DWORD, tagSQUARERANK>		m_SquareScoreMap;					//广场赛排名表

	map<DWORD, CString>				m_StrSelfRankBonusMap;				//排名奖励信息表
	map<DWORD, CString>				m_StrSelfResultBonusMap;			//满局建立信息表


	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IMainServiceFrame *				m_pIMainServiceFrame;				//服务框架
	IServerUserManager *			m_pIServerUserManager;				//用户管理

	//数据接口
protected:
	IDataBaseEngine *				m_pIKernelDataBaseEngine;			//内核数据
	IDataBaseEngine *				m_pIRecordDataBaseEngine;			//记录数据

	//配置信息
public:
	tagGameParameter *				m_pGameParameter;					//配置参数
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置
	tagGameSquareOption *			m_pGameSquareOption;				//广场赛配置

	//功能函数
public:
	//配置组件
	bool InitSquareMatch(tagSquareMatchParameter SquareMatchParameter);
	//开始广场赛
	void StartSquare();
	//结果到排名分数
	BYTE  ResultToRank(ITableFrame* pITableFrame, tagScoreInfo ScoreInfoArray[], WORD wScoreCount);
	//结果到排名分数
	BYTE  ResultToRank(ITableFrame* pITableFrame, WORD wChairID, tagScoreInfo ScoreInfo);

	CString GetPlayerRankString(map<DWORD, int> UserRankMap, DWORD dwRankUserID);

	void SendTopPlayers(DWORD dwSpecUserID, WORD wSplit=20);
	void SendSquareEnter(DWORD dwRankUserID);
	void SquareEnd();
	void SendSquareEnd(WORD wSplit=20);

	void AddRank(DWORD dwUserID, tagSQUARERANK SquareRank);

public:
	//定时器
	void OnTimer();
};

class CSquareManager : public map<WORD,CSquare>
{
public:
	CSquareManager(void);
	virtual ~CSquareManager(void);

public:
	//停止比赛
	bool ConcludeService();
public:
	//定时器
	void OnTimer();
};

#endif