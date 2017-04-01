#ifndef SQUARE_MATCH_MANAGER_HEAD_FILE
#define SQUARE_MATCH_MANAGER_HEAD_FILE
#pragma once

#include "DataBasePacket.h"
#include <map>
#include <vector>
using namespace std;

struct tagSCORERANK
{
	DWORD							dwKindID;							//�û���ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	LONGLONG						lScore;								//����
};

struct tagSQUARERANK
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	WORD							wResult;							//��Ч���
	LONGLONG						lScore;								//����
	DWORD							dwEnterTime;						//����ʱ��
};

struct tagRANKBONUSSTRING
{
	WORD							wFirst;								//��ʼ����
	WORD							wLast;								//ĩβ����

	TCHAR							szBonus[100];
	TCHAR							szDesc[100];
};

// �㳡��
class CSquare
{
public:
	CSquare();
	virtual ~CSquare();

public:
	//״̬����
	BOOL							m_bStartSquare;						//�㳡���Ƿ�ʼ
	BOOL							m_bEndSquare;						//�㳡���Ƿ��Ѿ�����

	DWORD							m_dwSquareRemain;					//�㳡��ʣ��ʱ�䣨�룩
	WORD							m_dwAnnounceInterval;				//�㳡��ʱ����
	map<DWORD, tagSQUARERANK>		m_SquareScoreMap;					//�㳡��������

	map<DWORD, CString>				m_StrSelfRankBonusMap;				//����������Ϣ��
	map<DWORD, CString>				m_StrSelfResultBonusMap;			//���ֽ�����Ϣ��


	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IMainServiceFrame *				m_pIMainServiceFrame;				//������
	IServerUserManager *			m_pIServerUserManager;				//�û�����

	//���ݽӿ�
protected:
	IDataBaseEngine *				m_pIKernelDataBaseEngine;			//�ں�����
	IDataBaseEngine *				m_pIRecordDataBaseEngine;			//��¼����

	//������Ϣ
public:
	tagGameParameter *				m_pGameParameter;					//���ò���
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������
	tagGameSquareOption *			m_pGameSquareOption;				//�㳡������

	//���ܺ���
public:
	//�������
	bool InitSquareMatch(tagSquareMatchParameter SquareMatchParameter);
	//��ʼ�㳡��
	void StartSquare();
	//�������������
	BYTE  ResultToRank(ITableFrame* pITableFrame, tagScoreInfo ScoreInfoArray[], WORD wScoreCount);
	//�������������
	BYTE  ResultToRank(ITableFrame* pITableFrame, WORD wChairID, tagScoreInfo ScoreInfo);

	CString GetPlayerRankString(map<DWORD, int> UserRankMap, DWORD dwRankUserID);

	void SendTopPlayers(DWORD dwSpecUserID, WORD wSplit=20);
	void SendSquareEnter(DWORD dwRankUserID);
	void SquareEnd();
	void SendSquareEnd(WORD wSplit=20);

	void AddRank(DWORD dwUserID, tagSQUARERANK SquareRank);

public:
	//��ʱ��
	void OnTimer();
};

class CSquareManager : public map<WORD,CSquare>
{
public:
	CSquareManager(void);
	virtual ~CSquareManager(void);

public:
	//ֹͣ����
	bool ConcludeService();
public:
	//��ʱ��
	void OnTimer();
};

#endif