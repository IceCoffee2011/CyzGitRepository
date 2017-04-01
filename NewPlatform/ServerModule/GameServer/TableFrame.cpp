#include "StdAfx.h"
#include "TableFrame.h"
#include "StockManager.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////////

//���߶���
#define IDI_OFF_LINE							(TIME_TABLE_SINK_RANGE+1)			//���߱�ʶ
#define MAX_OFF_LINE							3									//���ߴ���
#define TIME_OFF_LINE							90000L								//����ʱ��

#define IDI_GAME_TIME							(TIME_TABLE_SINK_RANGE+2)			//һ��ʱ�䶨ʱ��
#define MAX_GAME_TIME							360000L								//һ���ʱ��	

#define IDI_CUSTOMTABLE_OVERDUE					(TIME_TABLE_SINK_RANGE+3)			// �Խ����ӹ��ڼ�鶨ʱ��
#define CUSTOMTABLE_OVERDUE_FREQUENCY			60000L								// �Խ����ӹ��ڼ��Ƶ��	

#define IDI_CHECK_DISMISS_TABLE					(TIME_TABLE_SINK_RANGE+4)			// ��ɢ��ϷͶƱ��ʱʱ��
#define CHECK_DISMISS_TABLE_TIME				300000L								// ��ʱ����Чʱ��

//////////////////////////////////////////////////////////////////////////////////

//�������
CStockManager						g_StockManager;						//������

//��Ϸ��¼
CGameScoreRecordArray				CTableFrame::m_GameScoreRecordBuffer;

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrame::CTableFrame()
{
	//��������
	m_wTableID=0;
	m_wChairCount=0;
	m_cbStartMode=START_MODE_ALL_READY;
	m_wUserCount=0;

	//��־����
	m_bGameStarted=false;
	m_bDrawStarted=false;
	m_bTableStarted=false;
	m_bReset = true;
	m_bLockedForCustom = false;
	m_bLockedForDismiss = false;
	ZeroMemory(m_bAllowLookon,sizeof(m_bAllowLookon));
	ZeroMemory(m_lFrozenedScore,sizeof(m_lFrozenedScore));

	//��Ϸ����
	m_lCellScore=0L;
	m_cbGameStatus=GAME_STATUS_FREE;
	m_wDrawCount=0;

	//ʱ�����
	m_dwDrawStartTime=0L;
	ZeroMemory(&m_SystemTimeStart,sizeof(m_SystemTimeStart));

	//��̬����
	m_dwTableOwnerID=0L;
	ZeroMemory(m_szEnterPassword,sizeof(m_szEnterPassword));

	//���߱���
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
	ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));

	//������Ϣ
	m_pGameParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
//	m_pGameServiceTableOption = NULL;

	//����ӿ�
	m_pITimerEngine=NULL;
	m_pITableFrameSink=NULL;
	m_pIMainServiceFrame=NULL;
	m_pIAndroidUserManager=NULL;

	//���Žӿ�
	m_pITableUserAction=NULL;
	m_pITableUserRequest=NULL;
	m_pIMatchTableAction=NULL;

	//���ݽӿ�
	m_pIKernelDataBaseEngine=NULL;
	m_pIRecordDataBaseEngine=NULL;

	//�����ӿ�
	m_pIGameMatchSink=NULL;

	//�û�����
	ZeroMemory(m_TableUserItemArray,sizeof(m_TableUserItemArray));

	return;
}

//��������
CTableFrame::~CTableFrame()
{
	//�ͷŶ���
	SafeRelease(m_pITableFrameSink);
	SafeRelease(m_pIMatchTableAction);

	if (m_pIGameMatchSink!=NULL)
	{
		SafeDelete(m_pIGameMatchSink);
	}

	m_mapDismissVote.clear();

	return;
}

// ������������
void CTableFrame::Reset()
{
	LOGI("CTableFrame::Reset");
	m_wDrawCount = 0;
	m_bLockedForCustom = false;
	m_bLockedForDismiss = false;
	m_wConcludeType = INVALID_WORD;

	m_pGameTableOption->dwCreatUserID = INVALID_DWORD;
	memset(m_pGameTableOption->szSecret, 0, sizeof(m_pGameTableOption->szSecret));
	m_pGameTableOption->dwRoundCount = 0;						// �����ľ���
	m_pGameTableOption->dwPlayRoundCount = 0;
	::GetSystemTime(&(m_pGameTableOption->TableCreateTime));

	m_mapDismissVote.clear();

	KillGameTimer(IDI_CHECK_DISMISS_TABLE);
	KillGameTimer(IDI_CUSTOMTABLE_OVERDUE);
}

//�ӿڲ�ѯ
VOID * CTableFrame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrame,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrame,Guid,dwQueryVer);
	return NULL;
}

//��������
tagGameServiceOption*  CTableFrame::GetGameServiceOption()
{
	return m_pGameServiceOption; 
}

//��������
tagGameTableOption*	CTableFrame::GetGameTableOption()
{
//	return &(m_pGameServiceTableOption->TableOption[m_wTableID]);
	return m_pGameTableOption;
}

//��ʼ��Ϸ
bool CTableFrame::StartGame()
{
	if (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM)
	{
		// ׼����ɢ��Ͳ��ܿ�ʼ��Ϸ
		if (m_bLockedForDismiss == true)
		{
			LOGI("CTableFrame::StartGame ��Ϸ׼����ɢ,���ܿ�ʼ��Ϸ");
			return true;
		}
	}
	//��Ϸ״̬
	ASSERT(m_bDrawStarted==false);
	if (m_bDrawStarted==true) return false;

	//�������
	bool bGameStarted=m_bGameStarted;
	bool bTableStarted=m_bTableStarted;

	//����״̬
	m_bGameStarted=true;
	m_bDrawStarted=true;
	m_bTableStarted=true;

	//��ʼʱ��
	GetLocalTime(&m_SystemTimeStart);
	m_dwDrawStartTime=(DWORD)time(NULL);

	//��ʼ����
	if (bGameStarted==false)
	{
		//״̬����
		ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
		ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));

		//�����û�
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//�����û�
			if (pIServerUserItem!=NULL)
			{
				//������Ϸ��
				if (m_pGameTableOption->lServiceScore > 0L)
				{
					m_lFrozenedScore[i] = m_pGameTableOption->lServiceScore;
					pIServerUserItem->FrozenedUserScore(m_pGameTableOption->lServiceScore);
				}

				//����״̬
				BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
				if ((cbUserStatus!=US_OFFLINE)&&(cbUserStatus!=US_PLAYING)) pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,i);
			}
		}

		//����״̬
		if (bTableStarted!=m_bTableStarted) SendTableStatus();
	}

	//����֪ͨ
	bool bStart=true;
	if(m_pIGameMatchSink!=NULL) bStart=m_pIGameMatchSink->OnEventGameStart(this, m_wChairCount);

	//֪ͨ�¼�
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink!=NULL&&bStart) m_pITableFrameSink->OnEventGameStart();

	// ������Ϸ��ʱ��ʱ��,��ʱ�Զ���ɢ
	if (m_pGameTableOption->nMaxGameTime == 0)
	{
		// �����г�ʱ��ɢ
	}
	else
	{
		SetGameTimer(IDI_GAME_TIME, m_pGameTableOption->nMaxGameTime * 1000, 1, NULL);
	}

	// �Զ���ģʽ,��¼��ʼ��Ϸʱ�������Ŀ
	if (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM)
	{
		for (WORD i = 0; i < GetChairCount(); i++)
		{
			IServerUserItem* pIServerUserItem = GetTableUserItem(i);
			if (pIServerUserItem == nullptr)
			{
				continue;
			}
			DBR_GR_UserStartCustomGame UserStartCustomGame;
			UserStartCustomGame.wServerID = m_pGameServiceOption->wServerID;
			UserStartCustomGame.dwUserID = pIServerUserItem->GetUserID();
			UserStartCustomGame.wTableID = m_wTableID;
			UserStartCustomGame.wChairID = i;
			UserStartCustomGame.dwRoundCount = m_wDrawCount + 1;
			if (m_pIKernelDataBaseEngine)
			{
				m_pIKernelDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_START_CUSTOMGAME, NULL, &UserStartCustomGame, sizeof(DBR_GR_UserStartCustomGame));
			}
		}
	}

	
	return true;
}

//��ɢ��Ϸ
bool CTableFrame::DismissGame()
{
	//״̬�ж�
	ASSERT(m_bTableStarted==true);
	if (m_bTableStarted==false) return false;

	//������Ϸ
	if ((m_bGameStarted==true)&&(m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//����״̬
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//���ñ���
		m_bTableStarted=false;

		//����״̬
		SendTableStatus();
	}

	return false;
}

//������Ϸ
bool CTableFrame::ConcludeGame(BYTE cbGameStatus)
{
	//Ч��״̬
	ASSERT(m_bGameStarted==true);
	if (m_bGameStarted == false)
	{
		LOGI("CTableFrame::ConcludeGame m_bGameStarted == false");
		return false;
	}

	//�������
	bool bDrawStarted=m_bDrawStarted;

	//����״̬
	m_bDrawStarted=false;
	m_cbGameStatus=cbGameStatus;
	m_bGameStarted=(cbGameStatus>=GAME_STATUS_PLAY)?true:false;
	m_wDrawCount++;

	// �رճ�ʱ��ʱ��
	KillGameTimer(IDI_GAME_TIME);

	//��Ϸ��¼
	RecordGameScore(bDrawStarted);
	
	//��������
	if (m_bGameStarted==false)
	{
		//��������
		bool bOffLineWait=false;

		//�����û�
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//�û�����
			if (pIServerUserItem!=NULL)
			{
				tagTimeInfo* TimeInfo=pIServerUserItem->GetTimeInfo();
				//��Ϸʱ��
				DWORD dwCurrentTime=(DWORD)time(NULL);
				TimeInfo->dwEndGameTimer=dwCurrentTime;
				//������Ϸ��
				if (m_lFrozenedScore[i]!=0L)
				{
					pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[i]);
					m_lFrozenedScore[i]=0L;
				}

				//����״̬
				if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
				{
					//���ߴ���
					bOffLineWait=true;
					LOGI("CTableFrame::ConcludeGame PerformStandUpAction, (pIServerUserItem->GetUserStatus()==US_OFFLINE), NickName="<<pIServerUserItem->GetNickName());
					if ( (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM) == 0)
					{
						PerformStandUpAction(pIServerUserItem);
					}
				}
				else if (pIServerUserItem->GetUserStatus()==US_TRUSTEE)
				{
					LOGI("CTableFrame::ConcludeGame PerformStandUpAction, (pIServerUserItem->GetUserStatus()==US_TRUSTEE), NickName="<<pIServerUserItem->GetNickName());
					if ((m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM) == 0)
					{
						PerformStandUpAction(pIServerUserItem);
						// �ж�����Ƿ��뿪�����ˣ�����뿪�ˣ���Ҫɾ�����
						if (pIServerUserItem->GetBindIndex() == INVALID_WORD)
						{
							pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
						}
					}
				}
				else
				{
					//����״̬
					pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);

					//��������ͺ��Ե�
					if(m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)continue;
					//��������
					if (pIServerUserItem->IsAndroidUser()==true)
					{
						//���һ���
						CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;
						tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());
						IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(),
							pBindParameter->dwSocketID);

						//��������
						if (pIAndroidUserItem!=NULL)
						{
							//��ȡʱ��
							SYSTEMTIME SystemTime;
							GetLocalTime(&SystemTime);
							DWORD dwTimeMask=(1L<<SystemTime.wHour);

							//��ȡ����
							tagAndroidService * pAndroidService=pIAndroidUserItem->GetAndroidService();
							tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

							//������Ϣ
							if(pAndroidService->dwResidualPlayDraw>0)
								--pAndroidService->dwResidualPlayDraw;

							//����ʱ��
							if (((pIServerUserItem->GetTableID()==GetTableID())&&(pAndroidParameter->dwServiceTime&dwTimeMask)==0L))
							{
								LOGI("CTableFrame::ConcludeGame PerformStandUpAction, (pAndroidParameter->dwServiceTime&dwTimeMask == 0L), NickName="<<pIServerUserItem->GetNickName());
								PerformStandUpAction(pIServerUserItem);
								continue;
							}

	
							DWORD dwLogonTime=pIServerUserItem->GetLogonTime();
							DWORD dwReposeTime=pAndroidService->dwReposeTime;
							if ((dwLogonTime+dwReposeTime)<dwCurrentTime)
							{
								LOGI("CTableFrame::ConcludeGame PerformStandUpAction, ((dwLogonTime+dwReposeTime)<dwCurrentTime), NickName="<<pIServerUserItem->GetNickName());
								PerformStandUpAction(pIServerUserItem);
								continue;
							}

							//�����ж�
							if ((pIServerUserItem->GetTableID()==GetTableID())&&(pAndroidService->dwResidualPlayDraw==0))
							{
								LOGI("CTableFrame::ConcludeGame PerformStandUpAction, (pAndroidService->dwResidualPlayDraw==0), NickName="<<pIServerUserItem->GetNickName());
								PerformStandUpAction(pIServerUserItem);
								continue;
							}
						}
					}
				}
			}
		}

		//ɾ��ʱ��
		if (bOffLineWait==true)
		{
			LOGI("CTableFrame::ConcludeGame, KillGameTimer(IDI_OFF_LINE)");
			KillGameTimer(IDI_OFF_LINE);
		}
	}

	//֪ͨ����
	if(m_pIGameMatchSink!=NULL) m_pIGameMatchSink->OnEventGameEnd(this,0, NULL, cbGameStatus);

	//��������
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink!=NULL) m_pITableFrameSink->RepositionSink();

	//�߳����
	if (m_bGameStarted==false)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			if (m_TableUserItemArray[i]==NULL) continue;
			IServerUserItem * pIServerUserItem=m_TableUserItemArray[i];

			//��������
			if ( (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM) == 0)
			{
				if ((m_pGameTableOption->lMinTableScore != 0L) && (pIServerUserItem->GetUserScore() < m_pGameTableOption->lMinTableScore))
				{
					//������ʾ
					TCHAR szDescribe[128] = TEXT("");
					if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
					{
						_sntprintf(szDescribe, CountArray(szDescribe), TEXT("������Ϸ������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), m_pGameTableOption->lMinTableScore);
					}
					else
					{
						_sntprintf(szDescribe, CountArray(szDescribe), TEXT("������Ϸ�������� ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), m_pGameTableOption->lMinTableScore);
					}

					//������Ϣ(�����˷���С�����Ҫ���Ҳ���˳����䣬��Ϊ����ȡǮ������Ϸ)
					if (pIServerUserItem->IsAndroidUser() == true)
						SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);
					else
						SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT);

					//�û�����
					LOGI("CTableFrame::ConcludeGame PerformStandUpAction, " << szDescribe << ", NickName=" << pIServerUserItem->GetNickName());
					PerformStandUpAction(pIServerUserItem);

					continue;
				}
			}
			//�ر��ж�
			if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
			{
				//������Ϣ
				LPCTSTR pszMessage=TEXT("����ϵͳά������ǰ��Ϸ���ӽ�ֹ�û�������Ϸ��");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

				//�û�����
				LOGI("CTableFrame::ConcludeGame PerformStandUpAction "<<pszMessage<<", NickName="<<pIServerUserItem->GetNickName());
				PerformStandUpAction(pIServerUserItem);

				continue;
			}
		}
	}

	//��������
	if (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM)
	{
		// ���еľ�������������Ϸ��ǰ����
		if (m_pGameTableOption->dwRoundCount <= m_wDrawCount)
		{
			// ��������
			// ֪ͨ��Ϸ��������ɢ����
			DismissCustomTable(CONCLUDE_TYPE_NORMAL);
		}
		if (m_bLockedForDismiss && (m_wConcludeType == CONCLUDE_TYPE_PLAYER_DISMISS || m_wConcludeType == CONCLUDE_TYPE_TIMEOUT_START))
		{
			ConcludeTable();
		}
	}
	else
	{
		ConcludeTable();
	}

	//����״̬
	SendTableStatus();

	return true;
}

//��������
bool CTableFrame::ConcludeTable()
{
	//��������
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//�����ж�
		WORD wTableUserCount=GetSitUserCount();
		if (wTableUserCount==0) m_bTableStarted=false;
		if (m_pGameServiceAttrib->wChairCount==MAX_CHAIR) m_bTableStarted=false;

		//ģʽ�ж�
		if (m_cbStartMode==START_MODE_FULL_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_PAIR_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_ALL_READY) m_bTableStarted=false;

		if (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM)
		{
			m_pITableFrameSink->OnEventTableConclude();
		}
	}

	return true;
}

//д�����
bool CTableFrame::WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo, DWORD dwGameMemal, DWORD dwPlayGameTime)
{
	//Ч�����
	ASSERT((wChairID<m_wChairCount)&&(ScoreInfo.cbType!=SCORE_TYPE_NULL));
	if ((wChairID>=m_wChairCount)&&(ScoreInfo.cbType==SCORE_TYPE_NULL)) return false;

	//��ȡ�û�
	ASSERT(GetTableUserItem(wChairID)!=NULL);
	IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
	TCHAR szMessage[128]=TEXT("");

	//д�����
	if (pIServerUserItem!=NULL)
	{
		//��������
		DWORD dwUserMemal=0L;
#ifndef _READ_TABLE_OPTION_
		SCORE lRevenueScore=__min(m_lFrozenedScore[wChairID],m_pGameServiceOption->lServiceScore);
#else
//		SCORE lRevenueScore = __min(m_lFrozenedScore[wChairID], m_pGameServiceOption->TableOption[m_wTableID].lServiceScore);
		SCORE lRevenueScore = __min(m_lFrozenedScore[wChairID], m_pGameTableOption->lServiceScore);
#endif

		//�۷����
#ifndef _READ_TABLE_OPTION_
		if (m_pGameServiceOption->lServiceScore>0L 
			&& m_pGameServiceOption->wServerType == GAME_GENRE_GOLD
			&& m_pITableFrameSink->QueryBuckleServiceCharge(wChairID))
#else
// 		if (m_pGameServiceOption->TableOption[m_wTableID].lServiceScore > 0L
// 			&& m_pGameServiceOption->wServerType == GAME_GENRE_GOLD
// 			&& m_pITableFrameSink->QueryBuckleServiceCharge(wChairID))
		if (m_pGameTableOption->lServiceScore > 0L
			&& m_pGameServiceOption->wServerType == GAME_GENRE_GOLD
			&& m_pITableFrameSink->QueryBuckleServiceCharge(wChairID))
#endif
		{
			//�۷����
			ScoreInfo.lScore-=lRevenueScore;
			ScoreInfo.lRevenue+=lRevenueScore;

			//������Ϸ��
			pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
			m_lFrozenedScore[wChairID]=0L;
		}

		//�㳡���Ʒ�
		m_pIMainServiceFrame->ResultSquareMatchScore(this,wChairID,ScoreInfo);

		//���Ƽ���
		if(dwGameMemal != INVALID_DWORD)
		{
			dwUserMemal = dwGameMemal;
		}
		else if (ScoreInfo.lRevenue>0L)
		{
			WORD wMedalRate=m_pGameParameter->wMedalRate;
			dwUserMemal=(DWORD)(ScoreInfo.lRevenue*wMedalRate/1000L);
		}

		//��Ϸʱ��
		DWORD dwCurrentTime=(DWORD)time(NULL);
		DWORD dwPlayTimeCount=((m_bDrawStarted==true)?(dwCurrentTime-m_dwDrawStartTime):0L);
		if(dwPlayGameTime!=INVALID_DWORD) dwPlayTimeCount=dwPlayGameTime;

		//��������
		tagUserProperty * pUserProperty=pIServerUserItem->GetUserProperty();

		//�����ж�
		if(m_pGameServiceOption->wServerType == GAME_GENRE_SCORE)
		{
			if (ScoreInfo.lScore>0L)
			{
				//�ı�����
				if ((pUserProperty->wPropertyUseMark&PT_USE_MARK_FOURE_SCORE)!=0)
				{
					//��������
					DWORD dwValidTime=pUserProperty->PropertyInfo[1].wPropertyCount*pUserProperty->PropertyInfo[1].dwValidNum;
					if(pUserProperty->PropertyInfo[1].dwEffectTime+dwValidTime>dwCurrentTime)
					{
						//���ַ���
						ScoreInfo.lScore *= 4;
						_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ] ʹ����[ �ı����ֿ� ]���÷ַ��ı���)"),pIServerUserItem->GetNickName());
					}
					else
					{
						pUserProperty->wPropertyUseMark&=~PT_USE_MARK_FOURE_SCORE;
					}
				} //˫������
				else if ((pUserProperty->wPropertyUseMark&PT_USE_MARK_DOUBLE_SCORE)!=0)
				{
					//��������
					DWORD dwValidTime=pUserProperty->PropertyInfo[0].wPropertyCount*pUserProperty->PropertyInfo[0].dwValidNum;
					if (pUserProperty->PropertyInfo[0].dwEffectTime+dwValidTime>dwCurrentTime)
					{
						//���ַ���
						ScoreInfo.lScore*=2L;
						_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ] ʹ����[ ˫�����ֿ� ]���÷ַ�����"), pIServerUserItem->GetNickName());
					}
					else
					{
						pUserProperty->wPropertyUseMark&=~PT_USE_MARK_DOUBLE_SCORE;
					}
				}
			}
			else
			{
				//�����
				if ((pUserProperty->wPropertyUseMark&PT_USE_MARK_POSSESS)!=0)
				{
					//��������
					DWORD dwValidTime=pUserProperty->PropertyInfo[3].wPropertyCount*pUserProperty->PropertyInfo[3].dwValidNum;
					if(pUserProperty->PropertyInfo[3].dwEffectTime+dwValidTime>dwCurrentTime)
					{
						//���ַ���
						ScoreInfo.lScore = 0;
						_sntprintf(szMessage,CountArray(szMessage),TEXT("[ %s ] ʹ����[ ������� ]�����ֲ��䣡"),pIServerUserItem->GetNickName());
					}
					else
					{
						pUserProperty->wPropertyUseMark &= ~PT_USE_MARK_POSSESS;
					}
				}
			}
		}

		
		// ��ȡ����Ǯ����Ӯ����������
		SCORE lTotalWin = pIServerUserItem->GetTotalWin();
		SCORE lTotalLose = pIServerUserItem->GetTotalLose();

		//д�����
		pIServerUserItem->WriteUserScore(ScoreInfo.lScore,ScoreInfo.lGrade,ScoreInfo.lRevenue,dwUserMemal,ScoreInfo.cbType,dwPlayTimeCount);

		//��Ϸ��¼
		if (pIServerUserItem->IsAndroidUser()==false && CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			LOGI("CTableFrame::WriteUserScore ���������Ϸ��¼");
			//��������
			tagGameScoreRecord * pGameScoreRecord=NULL;

			//��ѯ���
			if (m_GameScoreRecordBuffer.GetCount()>0L)
			{
				//��ȡ����
				INT_PTR nCount=m_GameScoreRecordBuffer.GetCount();
				pGameScoreRecord=m_GameScoreRecordBuffer[nCount-1];

				//ɾ������
				m_GameScoreRecordBuffer.RemoveAt(nCount-1);
			}

			//��������
			if (pGameScoreRecord==NULL)
			{
				try
				{
					//��������
					pGameScoreRecord=new tagGameScoreRecord;
					if (pGameScoreRecord==NULL) throw TEXT("��Ϸ��¼���󴴽�ʧ��");
				}
				catch (...)
				{
					ASSERT(FALSE);
				}
			}

			//��¼����
			if (pGameScoreRecord!=NULL)
			{
				//�û���Ϣ
				pGameScoreRecord->wChairID=wChairID;
				pGameScoreRecord->dwUserID=pIServerUserItem->GetUserID();
				pGameScoreRecord->cbAndroid=(pIServerUserItem->IsAndroidUser()?TRUE:FALSE);

				//�û���Ϣ
				pGameScoreRecord->dwDBQuestID=pIServerUserItem->GetDBQuestID();
				pGameScoreRecord->dwInoutIndex=pIServerUserItem->GetInoutIndex();

				//��Ϸ����Ϣ
				pGameScoreRecord->lScore=ScoreInfo.lScore;
				pGameScoreRecord->lGrade=ScoreInfo.lGrade;
				pGameScoreRecord->lRevenue=ScoreInfo.lRevenue;

				//������Ϣ
				pGameScoreRecord->dwUserMemal=dwUserMemal;
				pGameScoreRecord->dwPlayTimeCount=dwPlayTimeCount;

				//��������˰
				if(pIServerUserItem->IsAndroidUser())
				{
					pGameScoreRecord->lScore += pGameScoreRecord->lRevenue;
					pGameScoreRecord->lRevenue = 0L;
				}

				//��������
				m_GameScoreRecordActive.Add(pGameScoreRecord);
			}
		}

		//��Ϸ��¼
//		LOGI("CTableFrame::WriteUserScore dwGameMemal="<<dwGameMemal<<", dwPlayTimeCount="<<dwPlayTimeCount);
		if(dwGameMemal != INVALID_DWORD || dwPlayTimeCount!=INVALID_DWORD)
		{
			LOGI("CTableFrame::WriteUserScore ��¼��Ϸ��¼");
			RecordGameScore(true);
		}
	}
	else
	{
		//�뿪�û�
		CTraceService::TraceString(TEXT("ϵͳ��ʱδ֧���뿪�û��Ĳ��ֲ�������"),TraceLevel_Exception);

		return false;
	}

	//�㲥��Ϣ
	if (szMessage[0]!=0)
	{
		//��������
		IServerUserItem * pISendUserItem = NULL;
		WORD wEnumIndex=0;

		//��Ϸ���
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			pISendUserItem=GetTableUserItem(i);
			if(pISendUserItem==NULL) continue;

			//������Ϣ
			SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
		}

		//�Թ��û�
		do
		{
			pISendUserItem=EnumLookonUserItem(wEnumIndex++);
			if(pISendUserItem!=NULL) 
			{
				//������Ϣ
				SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
			}
		} while (pISendUserItem!=NULL);
	}

	return true;
}

//д�����
bool CTableFrame::WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount)
{
	//Ч�����
	ASSERT(wScoreCount==m_wChairCount);
	if (wScoreCount!=m_wChairCount) return false;

	//д�����
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (ScoreInfoArray[i].cbType!=SCORE_TYPE_NULL)
		{
			WriteUserScore(i,ScoreInfoArray[i]);
		}
	}

	////�㳡���Ʒ�
	//m_pIMainServiceFrame->ResultSquareMatchScore(this,ScoreInfoArray,wScoreCount);

	return true;
}

//����˰��
SCORE CTableFrame::CalculateRevenue(WORD wChairID, SCORE lScore)
{
	//Ч�����
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return 0L;

	//����˰��
#ifndef _READ_TABLE_OPTION_
	if ((m_pGameServiceOption->wRevenueRatio>0)&&(lScore>=REVENUE_BENCHMARK))
	{
		//��ȡ�û�
		ASSERT(GetTableUserItem(wChairID)!=NULL);
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);

		//����˰��
		SCORE lRevenue=lScore*m_pGameServiceOption->wRevenueRatio/REVENUE_DENOMINATOR;

		return lRevenue;
	}
#else
// 	if ((m_pGameServiceOption->TableOption[m_wTableID].wRevenueRatio>0) && (lScore >= REVENUE_BENCHMARK))
// 	{
// 		//��ȡ�û�
// 		ASSERT(GetTableUserItem(wChairID) != NULL);
// 		IServerUserItem * pIServerUserItem = GetTableUserItem(wChairID);
// 
// 		//����˰��
// 		SCORE lRevenue = lScore*m_pGameServiceOption->TableOption[m_wTableID].wRevenueRatio / REVENUE_DENOMINATOR;
// 
// 		return lRevenue;
// 	}
	if ((m_pGameTableOption->wRevenueRatio > 0) && (lScore >= REVENUE_BENCHMARK))
	{
		//��ȡ�û�
		ASSERT(GetTableUserItem(wChairID) != NULL);
		IServerUserItem * pIServerUserItem = GetTableUserItem(wChairID);

		//����˰��
		SCORE lRevenue = lScore*m_pGameTableOption->wRevenueRatio / REVENUE_DENOMINATOR;

		return lRevenue;
	}
#endif

	return 0L;
}

//�����޶�
SCORE CTableFrame::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	//�û�Ч��
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	if (pIServerUserItem->GetTableID()!=m_wTableID) return 0L;

	//��ѯ���
	SCORE lTrusteeScore=pIServerUserItem->GetTrusteeScore();
#ifndef _READ_TABLE_OPTION_
	SCORE lMinEnterScore=m_pGameServiceOption->lMinTableScore;
#else
//	SCORE lMinEnterScore = m_pGameServiceOption->TableOption[m_wTableID].lMinTableScore;
	SCORE lMinEnterScore = m_pGameTableOption->lMinTableScore;
#endif
	SCORE lUserConsumeQuota=m_pITableFrameSink->QueryConsumeQuota(pIServerUserItem);

	//Ч����
	ASSERT((lUserConsumeQuota>=0L)&&(lUserConsumeQuota<=pIServerUserItem->GetUserScore()-lMinEnterScore));
	if ((lUserConsumeQuota<0L)||(lUserConsumeQuota>pIServerUserItem->GetUserScore()-lMinEnterScore)) return 0L;

	return lUserConsumeQuota+lTrusteeScore;
}

//Ѱ���û�
IServerUserItem * CTableFrame::SearchUserItem(DWORD dwUserID)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//�����û�
	for (WORD i=0;i<m_wChairCount;i++)
	{
		pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	}

	//�Թ��û�
	do
	{
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	} while (pIServerUserItem!=NULL);

	return NULL;
}

//��Ϸ�û�
IServerUserItem * CTableFrame::GetTableUserItem(WORD wChairID)
{
	//Ч�����
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return nullptr;

	//��ȡ�û�
	return m_TableUserItemArray[wChairID];
}

//�Թ��û�
IServerUserItem * CTableFrame::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex >= m_LookonUserItemArray.GetCount()) return nullptr;
	return m_LookonUserItemArray[wEnumIndex];
}

//����ʱ��
bool CTableFrame::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	LOGI("CTableFrame::SetGameTimer, dwTimerID=" << dwTimerID << ", dwElapse=" << dwElapse << ", dwRepeat=" << dwRepeat);
	//Ч�����
	ASSERT((dwTimerID>0)&&(dwTimerID<TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID <= 0) || (dwTimerID >= TIME_TABLE_MODULE_RANGE))
	{
		return false;
	}

	//����ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine != NULL)
	{
		LOGI("CTableFrame::SetGameTimer, m_pITimerEngine->SetTimer dwTimerID=" << (dwEngineTimerID + dwTimerID) << "dwElapse=" << dwElapse << ", dwRepeat=" << dwRepeat);
		m_pITimerEngine->SetTimer(dwEngineTimerID + dwTimerID, dwElapse, dwRepeat, dwBindParameter);
	}

	return true;
}

//ɾ��ʱ��
bool CTableFrame::KillGameTimer(DWORD dwTimerID)
{
	LOGI("CTableFrame::KillGameTimer, dwTimerID=" << dwTimerID);
	//Ч�����
	ASSERT((dwTimerID>0)&&(dwTimerID<=TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//ɾ��ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine != NULL)
	{
		LOGI("CTableFrame::KillGameTimer, m_pITimerEngine->KillTimer dwTimerID=" << (dwEngineTimerID + dwTimerID) );
		m_pITimerEngine->KillTimer(dwEngineTimerID + dwTimerID);
	}

	return true;
}

//��������
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	//״̬Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//��������
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//��������
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//��������
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//��������
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID)
{
	//�û�Ⱥ��
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;

			//Ч��״̬
			ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) continue;

			//��������
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

		return true;
	}
	else
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) return false;

		//Ч��״̬
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//��������
		m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

		return true;
	}

	return false;
}

//��������
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID)
{
	//�û�Ⱥ��
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

			//��������
			m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}
	else
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

		//��������
		m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);

		return true;
	}

	return false;
}

//��������
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//��������
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

	} while (true);

	return true;
}

//��������
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		//ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//��������
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
		}

	} while (true);

	return true;
}

//������Ϣ
bool CTableFrame::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	WORD wEnumIndex=0;

	//������Ϣ
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		//������Ϣ
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
	}

	//ö���û�
	do
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//������Ϣ
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);

	} while (true);

	return true;
}

//������Ϣ
bool CTableFrame::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//������Ϣ
	m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,lpszMessage,wType);

	return true;
}

//��Ϸ��Ϣ
bool CTableFrame::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//������Ϣ
	return m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
}

//����ϵͳ��Ϣ
bool CTableFrame::SendSystemMessage(LPCTSTR lpszMessage, WORD wType)
{
	return m_pIMainServiceFrame->SendSystemMessage(lpszMessage, wType);
}

//���ͳ���
bool CTableFrame::SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

	//���ͳ���
	ASSERT(m_pIMainServiceFrame!=NULL);
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_SCENE,pData,wDataSize);

	return true;
}

//�����¼�
bool CTableFrame::OnEventUserOffLine(IServerUserItem * pIServerUserItem)
{
	LOGI("CTableFrame::OnEventUserOffLine PerformStandUpAction1, UserID="<<pIServerUserItem->GetUserID()<<" NickName="<<pIServerUserItem->GetNickName());
	//����Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�û�����
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	LOGI("CTableFrame::OnEventUserOffLine, UserStatus=" << cbUserStatus << ", m_wOffLineCount=" << m_wOffLineCount[wChairID]);
	//��Ϸ�û�
	if (cbUserStatus!=US_LOOKON)
	{
		//Ч���û�
		ASSERT(pIServerUserItem==GetTableUserItem(wChairID));
		if (pIServerUserItem!=GetTableUserItem(wChairID)) return false;
		if(m_pIGameMatchSink)m_pIGameMatchSink->SetUserOffline(wChairID,true);

		//���ߴ���
		if ((cbUserStatus==US_PLAYING))
		{
			// �Խ�����ģʽû���йܴ�������
			if (m_wOffLineCount[wChairID]<MAX_OFF_LINE || (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM))
			{
				//�û�����
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);

				//���ߴ���
				if ((m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM) != 0)
				{
					// ���Խ�ģʽ�Ž��ж��߳�ʱ���,����һֱ���ߴ���
					if (m_dwOffLineTime[wChairID] == 0L)
					{
						//���ñ���
						m_wOffLineCount[wChairID]++;
						m_dwOffLineTime[wChairID] = (DWORD)time(NULL);

						//ʱ������
						WORD wOffLineCount = GetOffLineUserCount();
						if (wOffLineCount == 1)
						{
							LOGI("CTableFrame::OnEventUserOffLine, wOffLineCount=" << wOffLineCount);
							SetGameTimer(IDI_OFF_LINE, TIME_OFF_LINE, 1, wChairID);
						}
					}
				}
			}

			return true;
		}
	}

	if ((m_pGameServiceOption->wServerType&GAME_GENRE_USERCUSTOM) && m_bTableStarted == true)
	{
	}
	else
	{
		//�û�����
		LOGI("CTableFrame::OnEventUserOffLine PerformStandUpAction2, UserID="<<pIServerUserItem->GetUserID()<<", NickName="<<pIServerUserItem->GetNickName());
		PerformStandUpAction(pIServerUserItem);

		//ɾ���û�
		if ( CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule) )
		{
			if (pIServerUserItem->GetUserStatus() != US_TRUSTEE)
			{
				ASSERT(pIServerUserItem->GetUserStatus()==US_FREE);
				pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);
			}
		}
		else
		{
			ASSERT(pIServerUserItem->GetUserStatus()==US_FREE);
			pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);
		}
	}
	return true;
}

//�����¼�
bool CTableFrame::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//֪ͨ��Ϸ
	return m_pITableFrameSink->OnUserScroeNotify(wChairID,pIServerUserItem,cbReason);
}

//ʱ���¼�
bool CTableFrame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//�ص��¼�
	if ((dwTimerID>=0)&&(dwTimerID<TIME_TABLE_SINK_RANGE))
	{
		ASSERT(m_pITableFrameSink!=NULL);
		return m_pITableFrameSink->OnTimerMessage(dwTimerID,dwBindParameter);
	}

	//�¼�����
	switch (dwTimerID)
	{
	case IDI_OFF_LINE:	//�����¼�
		{
			//Ч��״̬
			ASSERT(m_bGameStarted==true);
			if (m_bGameStarted == false)
			{
				LOGI("CTableFrame::OnEventTimer, m_bGameStarted == false");
				return false;
			}

			//��������
			DWORD dwOffLineTime=0L;
			WORD wOffLineChair=INVALID_CHAIR;

			//Ѱ���û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
				{
					wOffLineChair=i;
					dwOffLineTime=m_dwOffLineTime[i];
				}
			}

			//λ���ж�
			ASSERT(wOffLineChair!=INVALID_CHAIR);
			if (wOffLineChair == INVALID_CHAIR)
			{
				LOGI("CTableFrame::OnEventTimer, wOffLineChair == INVALID_CHAIR");
				return false;
			}

			//�û��ж�
			ASSERT(dwBindParameter<m_wChairCount);
			if (wOffLineChair!=(WORD)dwBindParameter)
			{
				//ʱ�����
				DWORD dwCurrentTime=(DWORD)time(NULL);
				DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

				//����ʱ��
				dwLapseTime=__min(dwLapseTime,TIME_OFF_LINE-2000L);
				LOGI("CTableFrame::OnEventTimer wOffLineChair!=(WORD)dwBindParameter SetGameTimer time="<<TIME_OFF_LINE-dwLapseTime);
				SetGameTimer(IDI_OFF_LINE,TIME_OFF_LINE-dwLapseTime,1,wOffLineChair);

				return true;
			}

			//��ȡ�û�
			ASSERT(GetTableUserItem(wOffLineChair)!=NULL);
			IServerUserItem * pIServerUserItem=GetTableUserItem(wOffLineChair);

			//������Ϸ
			if (pIServerUserItem!=NULL)
			{
				//���ñ���
				m_dwOffLineTime[wOffLineChair]=0L;

				//�û�����
				LOGI("CTableFrame::OnEventTimer PerformStandUpAction IDI_OFF_LINE, NickName="<<pIServerUserItem->GetNickName());
				PerformStandUpAction(pIServerUserItem);
			}

			//����ʱ��
			if (m_bGameStarted==true)
			{
				//��������
				DWORD dwOffLineTime=0L;
				WORD wOffLineChair=INVALID_CHAIR;

				//Ѱ���û�
				for (WORD i=0;i<m_wChairCount;i++)
				{
					if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
					{
						wOffLineChair=i;
						dwOffLineTime=m_dwOffLineTime[i];
					}
				}

				//����ʱ��
				if (wOffLineChair!=INVALID_CHAIR)
				{
					//ʱ�����
					DWORD dwCurrentTime=(DWORD)time(NULL);
					DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

					//����ʱ��
					dwLapseTime=__min(dwLapseTime,TIME_OFF_LINE-2000L);
					LOGI("CTableFrame::OnEventTimer wOffLineChair!=INVALID_CHAIR SetGameTimer time="<<TIME_OFF_LINE-dwLapseTime);
					SetGameTimer(IDI_OFF_LINE,TIME_OFF_LINE-dwLapseTime,1,wOffLineChair);
				}
			}

			return true;
		}
	case IDI_GAME_TIME:
		{
			DismissGame();
			KillGameTimer(IDI_GAME_TIME);
			return true;
		}
	case IDI_CUSTOMTABLE_OVERDUE:
		{
			CheckCustomTableOverdue();

			return true;
		}
	case IDI_CHECK_DISMISS_TABLE:
		{
			for (WORD i = 0; i < GetChairCount(); i++)
			{
				IServerUserItem* pUserItem = GetTableUserItem(i);
				if (pUserItem == nullptr)
				{
					continue;
				}

				auto iter = m_mapDismissVote.find(i);
				if (iter != m_mapDismissVote.end())
				{
					continue;
				}

				// δͶƱ��Ĭ��Ͷ��ͬƱ
				m_mapDismissVote.insert(make_pair(i, true));
			}

			// ���ͶƱ
			CheckVoteForDismiss();
			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//��Ϸ�¼�
bool CTableFrame::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//��Ϣ����
	return m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//����¼�
bool CTableFrame::OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//��Ϸ����
	if (m_pITableFrameSink->OnFrameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem)==true) return true;

	//��������
	if(m_pIGameMatchSink!=NULL && m_pIGameMatchSink->OnFrameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem)==true) return true;

	//Ĭ�ϴ���
	switch (wSubCmdID)
	{
	case SUB_GF_GAME_OPTION:	//��Ϸ����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GF_GameOption));
			if (wDataSize!=sizeof(CMD_GF_GameOption)) return false;

			//��������
			CMD_GF_GameOption * pGameOption=(CMD_GF_GameOption *)pData;

			//��ȡ����
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//��������
			if ((cbUserStatus!=US_LOOKON)&&((m_dwOffLineTime[wChairID]!=0L)))
			{
				//���ñ���
				m_dwOffLineTime[wChairID]=0L;

				//ɾ��ʱ��
				WORD wOffLineCount=GetOffLineUserCount();
				if (wOffLineCount==0)
				{
					LOGI("CTableFrame::OnEventSocketFrame, KillGameTimer(IDI_OFF_LINE)");
					KillGameTimer(IDI_OFF_LINE);
				}
			}

			//����״̬
			pIServerUserItem->SetClientReady(true);
			if (cbUserStatus!=US_LOOKON) m_bAllowLookon[wChairID]=pGameOption->cbAllowLookon?true:false;

			//���Ӵ�����
			if (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM)
			{
				CMD_GF_CustomTableInfo CustomTableInfo;
				CustomTableInfo.dwUserID = m_pGameTableOption->dwCreatUserID;
				CustomTableInfo.bTableStarted = m_bTableStarted;
				CustomTableInfo.dwRoundCount = m_pGameTableOption->dwRoundCount;
				CustomTableInfo.dwPlayRoundCount = m_wDrawCount;
				lstrcpyn(CustomTableInfo.szSecret, m_pGameTableOption->szSecret, sizeof(CustomTableInfo));
				m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GF_CUSTOMTABLE_INFO, &CustomTableInfo, sizeof(CustomTableInfo));

				// ����Ƿ���ͬIP�������������
				if (cbUserStatus != US_LOOKON)
				{
					EfficacyIPAddress();
				}
			}

			//����״̬
			CMD_GF_GameStatus GameStatus;
			GameStatus.cbGameStatus=m_cbGameStatus;
			GameStatus.cbAllowLookon=m_bAllowLookon[wChairID]?TRUE:FALSE;
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_STATUS,&GameStatus,sizeof(GameStatus));

			//������Ϣ
			TCHAR szMessage[128]=TEXT("");
			_sntprintf(szMessage,CountArray(szMessage),TEXT("��ӭ�����롰%s-%s����Ϸ��ף����Ϸ��죡"),m_pGameServiceAttrib->szGameName,m_pGameServiceOption->szServerName);
			m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT);

			//���ͳ���
			bool bSendSecret=((cbUserStatus!=US_LOOKON)||(m_bAllowLookon[wChairID]==true));
			m_pITableFrameSink->OnEventSendGameScene(wChairID,pIServerUserItem,m_cbGameStatus,bSendSecret);

			//��ʼ�ж�
			if ((cbUserStatus==US_READY)&&(EfficacyStartGame(wChairID)==true))
			{
				StartGame(); 
			}

			return true;
		}
	case SUB_GF_USER_READY:		//�û�׼��
		{
			//��ȡ����
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//Ч��״̬
			ASSERT(GetTableUserItem(wChairID)==pIServerUserItem);
			if (GetTableUserItem(wChairID)!=pIServerUserItem) return false;

			//Ч��״̬
			//ASSERT(cbUserStatus==US_SIT);
			if (cbUserStatus!=US_SIT) return true;

			//�����׷����ж�
			if((CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
				&& (m_wDrawCount >= m_pGameServiceOption->wDistributeDrawCount || CheckDistribute()))
			{
				//������Ϣ
				LPCTSTR pszMessage=TEXT("ϵͳ���·������ӣ����Ժ�");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_CHAT);

				//������Ϣ
				m_pIMainServiceFrame->InsertWaitDistribute(pIServerUserItem);

				//�û�����
				LOGI("CTableFrame::OnEventTimer PerformStandUpAction SUB_GF_USER_READY, NickName="<<pIServerUserItem->GetNickName());
				PerformStandUpAction(pIServerUserItem);

				return true;
			}

			//�¼�֪ͨ
			if (m_pITableUserAction!=NULL)
			{
				m_pITableUserAction->OnActionUserOnReady(wChairID,pIServerUserItem,pData,wDataSize);
			}

			//�¼�֪ͨ
			if(m_pIMatchTableAction!=NULL && !m_pIMatchTableAction->OnActionUserOnReady(wChairID,pIServerUserItem, pData,wDataSize))
				return true;

			//��ʼ�ж�
			if (EfficacyStartGame(wChairID)==false)
			{
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
			else
			{
				StartGame(); 
			}

			return true;
		}
	case SUB_GF_USER_CHAT:		//�û�����
		{
			//��������
			CMD_GF_C_UserChat * pUserChat=(CMD_GF_C_UserChat *)pData;

			//Ч�����
			ASSERT(wDataSize<=sizeof(CMD_GF_C_UserChat));
			ASSERT(wDataSize>=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)));
			ASSERT(wDataSize==(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0])));

			//Ч�����
			if (wDataSize>sizeof(CMD_GF_C_UserChat)) return false;
			if (wDataSize<(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString))) return false;
			if (wDataSize!=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0]))) return false;

			//Ŀ���û�
			if ((pUserChat->dwTargetUserID!=0)&&(SearchUserItem(pUserChat->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//״̬�ж�
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ��Ϸ���죡"),SMT_CHAT);
				return true;
			}

			//Ȩ���ж�
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//������Ϣ
			CMD_GF_S_UserChat UserChat;
			ZeroMemory(&UserChat,sizeof(UserChat));

			//�ַ�����
			m_pIMainServiceFrame->SensitiveWordFilter(pUserChat->szChatString,UserChat.szChatString,CountArray(UserChat.szChatString));

			//��������
			UserChat.dwChatColor=pUserChat->dwChatColor;
			UserChat.wChatLength=pUserChat->wChatLength;
			UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
			UserChat.dwSendUserID=pIServerUserItem->GetUserID();
			UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

			//��������
			WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
			WORD wSendSize=wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]);

			//��Ϸ�û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
			}

			//�Թ��û�
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//ö���û�
			do
			{
				//��ȡ�û�
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//��������
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);
				}
			} while (true);

			return true;
		}
	case SUB_GF_USER_EXPRESSION:	//�û�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GF_C_UserExpression));
			if (wDataSize!=sizeof(CMD_GF_C_UserExpression)) return false;

			//��������
			CMD_GF_C_UserExpression * pUserExpression=(CMD_GF_C_UserExpression *)pData;

			//Ŀ���û�
			if ((pUserExpression->dwTargetUserID!=0)&&(SearchUserItem(pUserExpression->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//״̬�ж�
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ��Ϸ���죡"),SMT_CHAT);
				return true;
			}

			//Ȩ���ж�
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//������Ϣ
			CMD_GR_S_UserExpression UserExpression;
			ZeroMemory(&UserExpression,sizeof(UserExpression));

			//��������
			UserExpression.wItemIndex=pUserExpression->wItemIndex;
			UserExpression.dwSendUserID=pIServerUserItem->GetUserID();
			UserExpression.dwTargetUserID=pUserExpression->dwTargetUserID;

			//��Ϸ�û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				//��������
				m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));
			}

			//�Թ��û�
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem=NULL;

			//ö���û�
			do
			{
				//��ȡ�û�
				pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//��������
				if (pIServerUserItem->IsClientReady()==true)
				{
					m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));
				}
			} while (true);

			return true;
		}
	case SUB_GF_LOOKON_CONFIG:		//�Թ�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GF_LookonConfig));
			if (wDataSize<sizeof(CMD_GF_LookonConfig)) return false;

			//��������
			CMD_GF_LookonConfig * pLookonConfig=(CMD_GF_LookonConfig *)pData;

			//Ŀ���û�
			if ((pLookonConfig->dwUserID!=0)&&(SearchUserItem(pLookonConfig->dwUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()!=US_LOOKON);
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return false;

			//�Թ۴���
			if (pLookonConfig->dwUserID!=0L)
			{

				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//��ȡ�û�
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetUserID()!=pLookonConfig->dwUserID) continue;
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//������Ϣ
					CMD_GF_LookonStatus LookonStatus;
					LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

					//������Ϣ
					ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));

					break;
				}

				//�¼�֪ͨ
				bool bAllowLookon=(pLookonConfig->cbAllowLookon==TRUE)?true:false;
				if (m_pITableUserAction!=NULL)
				{
					m_pITableUserAction->OnActionUserAllowLookon(pIServerUserItem->GetChairID(), pIServerUserItem, bAllowLookon);
				}
			}
			else
			{
				//�����ж�
				bool bAllowLookon=(pLookonConfig->cbAllowLookon==TRUE)?true:false;
				if (bAllowLookon==m_bAllowLookon[pIServerUserItem->GetChairID()]) return true;

				//���ñ���
				m_bAllowLookon[pIServerUserItem->GetChairID()]=bAllowLookon;

				//������Ϣ
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

				//������Ϣ
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//��ȡ�û�
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//������Ϣ
					ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
				}

				//�¼�֪ͨ
				if (m_pITableUserAction!=NULL)
				{
					m_pITableUserAction->OnActionUserAllowLookon(pIServerUserItem->GetChairID(), pIServerUserItem, bAllowLookon);
				}
			}

			return true;
		}
	case SUB_GF_APPLY_FOR_DISMISS:
		{
			// ��������ɢ
			return OnEventSocketFrameApplyForDismiss(pData, wDataSize, pIServerUserItem);
		}
		break;
	case SUB_GF_CHOICE_FOR_DISMISS:
		{
			// ��ҵĽ�ɢѡ��
			return OnEventSocketFrameChoiceForDismiss(pData, wDataSize, pIServerUserItem);
		}
		break;
	}

	return false;
}

// ��������ɢ
bool CTableFrame::OnEventSocketFrameApplyForDismiss(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	// �����ɢ����
	// Ч�����
	ASSERT(wDataSize == sizeof(CMD_GF_C_UserApplyForDismiss));
	if (wDataSize != sizeof(CMD_GF_C_UserApplyForDismiss)) return false;

	if (m_bLockedForDismiss == true)
	{
		return true;
	}

	CMD_GF_C_UserApplyForDismiss* pUserApplyForDismiss = (CMD_GF_C_UserApplyForDismiss*)pData;

	// �����ʼ����Ϸ,��ɢ����Ҫͬ�����ͬ��
	if (m_bTableStarted == false)
	{
		// ���û�п�ʼ����Ϸ,�����������Լ������ɢ����,ֱ�ӽ�ɢ
		if (pUserApplyForDismiss->dwUserID == m_pGameTableOption->dwCreatUserID)
		{
			DismissCustomTable(CONCLUDE_TYPE_CREATER_DISMISS);
		}
	}
	else
	{
		// �������Ѿ�Ͷ��Ʊ��,��ʾ��Ҫ�ظ�����
		IServerUserItem* pIServerUserItem = SearchUserItem(pUserApplyForDismiss->dwUserID);
		if (pIServerUserItem == nullptr)
		{
			return true;
		}
		//��Ϸ�û�
		IServerUserItem* pITableUserItem = GetTableUserItem(pIServerUserItem->GetChairID());
		if (pITableUserItem != pIServerUserItem)
		{
			return true;
		}

		// ͳ��ͶƱ���
		auto iter = m_mapDismissVote.find(pITableUserItem->GetChairID());
		if (iter != m_mapDismissVote.end())
		{
			SendGameMessage(pITableUserItem, TEXT("ͶƱ������,��ȴ��������ͶƱ."), SMT_CHAT | SMT_EJECT);
			return true;
		}
		bool bfirstvote = false;
		if (m_mapDismissVote.empty())
		{
			// ֮ǰû��ͶƱ���,�ǵ�һ�η�������
			bfirstvote = true;

			// ������ʱ��
			SetGameTimer(IDI_CHECK_DISMISS_TABLE, CHECK_DISMISS_TABLE_TIME, 1, NULL);
		}
		m_mapDismissVote.insert(make_pair(pITableUserItem->GetChairID(), true));

		// ת����Ϣ���������з��Թ����
		if (bfirstvote )
		{
			CMD_GF_S_UserApplyForDismiss UserApplyForDismiss;
			UserApplyForDismiss.dwUserID = pUserApplyForDismiss->dwUserID;
			for (WORD i = 0; i < GetChairCount(); i++)
			{
				IServerUserItem* pServerUserItem = GetTableUserItem(i);
				if (pServerUserItem == nullptr)
				{
					continue;
				}
				if (pServerUserItem->GetUserID() == pUserApplyForDismiss->dwUserID)
				{
					// ֪ͨ������,��ɢ�����ѷ���
					SendGameMessage(pServerUserItem, TEXT("��ɢ�����ѷ��͸�ͬ�����������,��ȴ�ͶƱ���."), SMT_CHAT | SMT_EJECT);
				}
				else
				{
					SendTableData(i, SUB_GF_APPLY_FOR_DISMISS, &UserApplyForDismiss, sizeof(UserApplyForDismiss), MDM_GF_FRAME);
				}
			}
		}
		else
		{
			// ת����Ϣ���������з��Թ����
			CMD_GF_S_UserChoiceForDismiss UserChoiceForDismiss;
			UserChoiceForDismiss.dwUserID = pITableUserItem->GetUserID();
			UserChoiceForDismiss.bDismiss = true;
			for (WORD i = 0; i < GetChairCount(); i++)
			{
				IServerUserItem* pServerUserItem = GetTableUserItem(i);
				if (pServerUserItem == nullptr || pServerUserItem->GetUserID() == pITableUserItem->GetUserID())
				{
					continue;
				}
				SendTableData(i, SUB_GF_CHOICE_FOR_DISMISS, &UserChoiceForDismiss, sizeof(UserChoiceForDismiss), MDM_GF_FRAME);
			}

			CheckVoteForDismiss();
		}
	}

	return true;
}

// ��ҽ�ɢ�¼���ͶƱ���
bool CTableFrame::OnEventSocketFrameChoiceForDismiss(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GF_C_UserChoiceForDismiss));
	if (wDataSize != sizeof(CMD_GF_C_UserChoiceForDismiss)) return false;

	if (m_mapDismissVote.empty())
	{
		// ���ͶƱ���Ϊ��,˵��û���������ɢ.��Ϊ�����ɢ��,Ĭ�ϻ���������ߵ�ѡ��
		return true;
	}

	CMD_GF_C_UserChoiceForDismiss* pUserChoiceForDismiss = (CMD_GF_C_UserChoiceForDismiss*)pData;

	WORD wVoteSite = 0xFF;
	for (WORD i = 0; i < GetChairCount(); i++)
	{
		IServerUserItem* pUserItem = GetTableUserItem(i);
		if (GetTableUserItem(i) == pIServerUserItem)
		{
			wVoteSite = i;
			break;
		}
	}
	auto iter = m_mapDismissVote.find(wVoteSite);
	if (iter != m_mapDismissVote.end())
	{
		SendGameMessage(pIServerUserItem, TEXT("���Ѿ�Ͷ��Ʊ��,�����ٴ�ͶƱ."), SMT_CHAT | SMT_EJECT);
		return true;
	}

	if (pUserChoiceForDismiss->bDismiss == false)
	{
		// �رն�ʱ��
		KillGameTimer(IDI_CHECK_DISMISS_TABLE);
		// ����ͶƱ���
		m_bLockedForDismiss = false;
		m_wConcludeType = INVALID_WORD;
		m_mapDismissVote.clear();
		// ֪ͨ���
		TCHAR szMessage[128] = { 0 };
		_snprintf(szMessage, sizeof(szMessage), TEXT("���[%s]��ͬ���ɢ��Ϸ,��Ϸ����."), pIServerUserItem->GetNickName());
		SendGameMessage(szMessage, SMT_CHAT | SMT_EJECT);
	}
	else
	{
		// ͳ��ͶƱ���
		for (WORD i = 0; i < GetChairCount(); i++)
		{
			IServerUserItem* pUserItem = GetTableUserItem(i);
			if (pUserItem == nullptr || pUserItem->GetUserID() != pUserChoiceForDismiss->dwUserID)
			{
				continue;
			}

			m_mapDismissVote.insert(make_pair(i, pUserChoiceForDismiss->bDismiss));
		}

		// ת����Ϣ���������з��Թ����
		CMD_GF_S_UserChoiceForDismiss UserChoiceForDismiss;
		UserChoiceForDismiss.dwUserID = pUserChoiceForDismiss->dwUserID;
		UserChoiceForDismiss.bDismiss = pUserChoiceForDismiss->bDismiss;
		for (WORD i = 0; i < GetChairCount(); i++)
		{
			IServerUserItem* pServerUserItem = GetTableUserItem(i);
			if (pServerUserItem == nullptr || pServerUserItem->GetUserID() == pUserChoiceForDismiss->dwUserID)
			{
				continue;
			}
			SendTableData(i, SUB_GF_CHOICE_FOR_DISMISS, &UserChoiceForDismiss, sizeof(UserChoiceForDismiss), MDM_GF_FRAME);
		}

		CheckVoteForDismiss();
	}

	return true;
}

// ����ɢ�������ͶƱ
void CTableFrame::CheckVoteForDismiss()
{
	// ���ͶƱ���
	WORD wTotalCount = 0;
	WORD wAgreeCount = 0;
	WORD wDisagreeCount = 0;
	for (WORD i = 0; i < GetChairCount(); i++)
	{
		IServerUserItem* pUserItem = GetTableUserItem(i);
		if (pUserItem == nullptr || pUserItem->GetUserStatus() < US_PLAYING)
		{
			continue;
		}
		wTotalCount++;
		auto iter = m_mapDismissVote.find(i);
		if (iter != m_mapDismissVote.end())
		{
			if (iter->second == true)
			{
				wAgreeCount++;
			}
			else
			{
				wDisagreeCount++;
			}
		}
	}

	if (wAgreeCount + wDisagreeCount >= wTotalCount - 1)
	{
		// �رն�ʱ��
		KillGameTimer(IDI_CHECK_DISMISS_TABLE);
		// ����ͶƱ���
		m_wConcludeType = INVALID_WORD;
		m_mapDismissVote.clear();
		// ��ɢ����
		DismissCustomTable(CONCLUDE_TYPE_PLAYER_DISMISS);
	}
}

// ��ɢ�Խ�����
void CTableFrame::DismissCustomTable(WORD wConcludeType)
{
	// ��������,���ܿ�ʼ��Ϸ
	m_bLockedForDismiss = true;
	m_wConcludeType = wConcludeType;

	// ֪ͨ��Ϸ��������ɢ����
	DBR_GR_ConcludeCustomTable DismissCustomTable;
	DismissCustomTable.wTableID = m_wTableID;
	DismissCustomTable.wServerID = m_pGameServiceOption->wServerID;
	DismissCustomTable.wConcludeType = wConcludeType;
	if (m_pIKernelDataBaseEngine)
	{
		m_pIKernelDataBaseEngine->PostDataBaseRequest(DBR_GR_CONCLUDE_CUSTOMTABLE, NULL, &DismissCustomTable, sizeof(DismissCustomTable));
	}
}

// ����Ƿ�ʱ,��ɢ����
bool CTableFrame::CheckCustomTableOverdue()
{
	if (m_bTableStarted == true)
	{
		// �Ѿ���ʼ����Ϸ,24Сʱδ�����Ļ���ǿ�ƽ���
		// ����Խ������Ƿ����
		CTime tmLocalTime = CTime::GetCurrentTime();

		if (CTime(m_pGameTableOption->TableCreateTime) + CTimeSpan(0, 24, 45, 0) <= tmLocalTime)
		{
			TCHAR szMessage[2048] = { 0 };
			_snprintf(szMessage, sizeof(szMessage), TEXT("CTableFrame::CheckCustomTableOverdue, Table Is Start. TableCreateTime(%d-%d-%d %d:%d:%d), LocalTime(%d-%d-%d %d:%d:%d), \
							CreateUser=%d, Secret=%s, RoundCount=%d"), \
							m_pGameTableOption->TableCreateTime.wYear, m_pGameTableOption->TableCreateTime.wMonth, m_pGameTableOption->TableCreateTime.wDay, \
							m_pGameTableOption->TableCreateTime.wHour, m_pGameTableOption->TableCreateTime.wMinute, m_pGameTableOption->TableCreateTime.wMilliseconds, \
							tmLocalTime.GetYear(), tmLocalTime.GetMonth(), tmLocalTime.GetDay(), tmLocalTime.GetHour(), tmLocalTime.GetMinute(), tmLocalTime.GetSecond(), \
							m_pGameTableOption->dwCreatUserID, m_pGameTableOption->szSecret, m_pGameTableOption->dwRoundCount);

			LOGI(szMessage);
			// ׼����ɢ
			DismissCustomTable(CONCLUDE_TYPE_TIMEOUT_START);

			KillGameTimer(IDI_CUSTOMTABLE_OVERDUE);

			return true;
		}
		else if (CTime(m_pGameTableOption->TableCreateTime) + CTimeSpan(0, 23, 44, 0) <= tmLocalTime)
		{
			// ��ʾ����1���Ӻ��ɢ
			SendGameMessage(TEXT("�Խ����䳤ʱ��δ����,����һ���Ӻ����!"), SMT_CHAT | SMT_TABLE_ROLL);
			return true;
		}
		else if (CTime(m_pGameTableOption->TableCreateTime) + CTimeSpan(0, 23, 40, 0) <= tmLocalTime)
		{
			// ��ʾ����5���Ӻ��ɢ
			SendGameMessage(TEXT("�Խ����䳤ʱ��δ����,��������Ӻ����!"), SMT_CHAT | SMT_TABLE_ROLL);
			return true;
		}
	}
	else
	{
// 	}
// 	if (m_bGameStarted == false && m_bDrawStarted == false && m_bTableStarted == false)
//	{
		// ����Խ������Ƿ����
		CTime tmLocalTime = CTime::GetCurrentTime();

		if (CTime(m_pGameTableOption->TableCreateTime) + CTimeSpan(0, 0, 15, 0) <= tmLocalTime)
		{
			TCHAR szMessage[2048] = { 0 };
			_snprintf(szMessage, sizeof(szMessage), TEXT("CTableFrame::CheckCustomTableOverdue, Table Is Not Start TableCreateTime(%d-%d-%d %d:%d:%d), LocalTime(%d-%d-%d %d:%d:%d), \
					CreateUser=%d, Secret=%s, RoundCount=%d"), \
				m_pGameTableOption->TableCreateTime.wYear, m_pGameTableOption->TableCreateTime.wMonth, m_pGameTableOption->TableCreateTime.wDay,\
				m_pGameTableOption->TableCreateTime.wHour, m_pGameTableOption->TableCreateTime.wMinute, m_pGameTableOption->TableCreateTime.wMilliseconds, \
				tmLocalTime.GetYear(), tmLocalTime.GetMonth(), tmLocalTime.GetDay(), tmLocalTime.GetHour(), tmLocalTime.GetMinute(), tmLocalTime.GetSecond(),  \
				m_pGameTableOption->dwCreatUserID, m_pGameTableOption->szSecret, m_pGameTableOption->dwRoundCount);

			LOGI(szMessage);
			// ׼����ɢ
			DismissCustomTable(CONCLUDE_TYPE_TIMEOUT_NOSTART);

			KillGameTimer(IDI_CUSTOMTABLE_OVERDUE);

			return true;
		}
		else if (CTime(m_pGameTableOption->TableCreateTime) + CTimeSpan(0, 0, 14, 0) <= tmLocalTime)
		{
			// ��ʾ����1���Ӻ��ɢ
			SendGameMessage(TEXT("��Ϸ��ʱ��δ��ʼ,����һ���Ӻ��ɢ!����Ϸδ��ʼ,��ɢ���䲻��۳���ʯ"), SMT_CHAT | SMT_TABLE_ROLL);
			return true;
		}
		else if (CTime(m_pGameTableOption->TableCreateTime) + CTimeSpan(0, 0, 10, 0) <= tmLocalTime)
		{
			// ��ʾ����5���Ӻ��ɢ
			SendGameMessage(TEXT("��Ϸ��ʱ��δ��ʼ,��������Ӻ��ɢ!����Ϸδ��ʼ,��ɢ���䲻��۳���ʯ"), SMT_CHAT | SMT_TABLE_ROLL);
			return true;
		}
	}

	return true;
}

//��ȡ��λ
WORD CTableFrame::GetNullChairID()
{
	//��������
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_TableUserItemArray[i]==NULL)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//�����λ
WORD CTableFrame::GetRandNullChairID()
{
	//��������
	WORD wIndex = rand()%m_wChairCount;
	for (WORD i=wIndex;i<m_wChairCount+wIndex;i++)
	{
		if (m_TableUserItemArray[i%m_wChairCount]==NULL)
		{
			return i%m_wChairCount;
		}
	}

	return INVALID_CHAIR;
}

//�û���Ŀ
WORD CTableFrame::GetSitUserCount()
{
	//��������
	WORD wUserCount=0;

	//��Ŀͳ��
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (GetTableUserItem(i)!=NULL)
		{
			wUserCount++;
		}
	}

	return wUserCount;
}

//�Թ���Ŀ
WORD CTableFrame::GetLookonUserCount()
{
	//��ȡ��Ŀ
	INT_PTR nLookonCount=m_LookonUserItemArray.GetCount();

	return (WORD)(nLookonCount);
}

//������Ŀ
WORD CTableFrame::GetOffLineUserCount()
{
	//��������
	WORD wOffLineCount=0;

	//��������
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_dwOffLineTime[i]!=0L)
		{
			wOffLineCount++;
		}
	}

	return wOffLineCount;
}

//����״��
WORD CTableFrame::GetTableUserInfo(tagTableUserInfo & TableUserInfo)
{
	//���ñ���
	ZeroMemory(&TableUserInfo,sizeof(TableUserInfo));

	//�û�����
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//�û�����
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			TableUserInfo.wTableUserCount++;
		}
		else
		{
			TableUserInfo.wTableAndroidCount++;
		}

		//׼���ж�
		if (pIServerUserItem->GetUserStatus()==US_READY)
		{
			TableUserInfo.wTableReadyCount++;
		}
	}

	//������Ŀ
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:		//����׼��
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_PAIR_READY:		//��Կ�ʼ
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_TIME_CONTROL:	//ʱ�����
		{
			TableUserInfo.wMinUserCount=1;
			break;
		}
	default:						//Ĭ��ģʽ
		{
			TableUserInfo.wMinUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserInfo.wTableAndroidCount+TableUserInfo.wTableUserCount;
}

//��������
bool CTableFrame::InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter)
{
	//���ñ���
	m_wTableID=wTableID;
	m_wChairCount=TableFrameParameter.pGameServiceAttrib->wChairCount;

	//���ò���
	m_pGameParameter=TableFrameParameter.pGameParameter;
	m_pGameServiceAttrib=TableFrameParameter.pGameServiceAttrib;
	m_pGameServiceOption=TableFrameParameter.pGameServiceOption;
//	m_pGameServiceTableOption = TableFrameParameter.pGameServiceTableOption;
//	m_GameTableOption = TableFrameParameter.pGameServiceTableOption->TableOption[wTableID];
	m_pGameTableOption = TableFrameParameter.pTableOption;

	//����ӿ�
	m_pITimerEngine=TableFrameParameter.pITimerEngine;
	m_pIMainServiceFrame=TableFrameParameter.pIMainServiceFrame;
	m_pIAndroidUserManager=TableFrameParameter.pIAndroidUserManager;
	m_pIKernelDataBaseEngine=TableFrameParameter.pIKernelDataBaseEngine;
	m_pIRecordDataBaseEngine=TableFrameParameter.pIRecordDataBaseEngine;

	//��������
	IGameServiceManager * pIGameServiceManager=TableFrameParameter.pIGameServiceManager;
	if (m_pITableFrameSink == NULL)
	{
		m_pITableFrameSink = (ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink, VER_ITableFrameSink);

		//�����ж�
		if (m_pITableFrameSink == NULL)
		{
			ASSERT(FALSE);
			return false;
		}

		//��������
		IUnknownEx * pITableFrame = QUERY_ME_INTERFACE(IUnknownEx);
		if (m_pITableFrameSink->Initialization(pITableFrame) == false) return false;
	}
	else
	{
		// ��������
		m_pITableFrameSink->RepositionSink();

		//��������
		IUnknownEx * pITableFrame = QUERY_ME_INTERFACE(IUnknownEx);
		if (m_pITableFrameSink->ReInitialization(pITableFrame) == false) return false;
	}

	//���ñ���
	m_lCellScore = m_pGameTableOption->lCellScore;
	if (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM)
	{
		if (m_pGameTableOption->dwPlayRoundCount > 0)
		{
			// ��ʼ����Ϸ,��������״̬
			m_wDrawCount = m_pGameTableOption->dwPlayRoundCount - 1;
			m_bTableStarted = true;
		}
	}

	if (m_pGameTableOption && m_pGameTableOption->szSecret[0] != 0)
	{
		m_bLockedForCustom = true;

		// ������ʱ��
		SetGameTimer(IDI_CUSTOMTABLE_OVERDUE, CUSTOMTABLE_OVERDUE_FREQUENCY, TIMES_INFINITY, NULL);

		// ��������Ƿ�ʱ
		CheckCustomTableOverdue();
	}

	//��չ�ӿ�
	m_pITableUserAction=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserAction);
	m_pITableUserRequest=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserRequest);

	//��������ģʽ
	if((TableFrameParameter.pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0 && TableFrameParameter.pIGameMatchServiceManager!=NULL)
	{
		IUnknownEx * pIUnknownEx=QUERY_ME_INTERFACE(IUnknownEx);
		IGameMatchServiceManager * pIGameMatchServiceManager=QUERY_OBJECT_PTR_INTERFACE(TableFrameParameter.pIGameMatchServiceManager,IGameMatchServiceManager);
		if (pIGameMatchServiceManager==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
		if (m_pIGameMatchSink == NULL)
		{
			m_pIGameMatchSink = (IGameMatchSink *)pIGameMatchServiceManager->CreateGameMatchSink(IID_IGameMatchSink, VER_IGameMatchSink);
		}

		//�����ж�
		if (m_pIGameMatchSink==NULL)
		{
			ASSERT(FALSE);
			return false;
		}

		//��չ�ӿ�
		m_pIMatchTableAction=QUERY_OBJECT_PTR_INTERFACE(m_pIGameMatchSink,ITableUserAction);
		if (m_pIGameMatchSink->InitTableFrameSink(pIUnknownEx)==false) 
		{
			return false;
		}
	}

	return true;
}

//��������
bool CTableFrame::PerformStandUpAction(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	ASSERT(pIServerUserItem->GetChairID()<=m_wChairCount);

	//�û�����
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	// ��ӡ��־
	LOGI("CTableFrame::PerformStandUpAction, UserID="<< pIServerUserItem->GetUserID()<<", NickName=" << pIServerUserItem->GetNickName() << ", cbUserStatus=" << cbUserStatus);

	if (pIServerUserItem == pITableUserItem)
	{
		if (CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule))
		{
			// ���������й��˳�״̬,��ֱ�ӷ���,������������ж�
			// (����йܳ���������,��ֱ�ӵ��õ�ǰ����,�����п���֮ǰ�Ķ�ʱ����û��ִ��,�ͻ��ظ����ú���,�������µ��ж�,����bug)
			if ((m_bGameStarted == true) && (cbUserStatus == US_TRUSTEE))
			{
				return true;
			}
		}
	}

	//��Ϸ�û�
	if ((m_bGameStarted==true)&&((cbUserStatus==US_PLAYING)||(cbUserStatus==US_OFFLINE)))
	{
		if (CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule))
		{
			// �������״̬���й�״̬
			pIServerUserItem->SetUserStatus(US_TRUSTEE, m_wTableID, wChairID);
		}
		//������Ϸ
		BYTE cbConcludeReason=(cbUserStatus==US_OFFLINE)?GER_NETWORK_ERROR:GER_USER_LEAVE;
		m_pITableFrameSink->OnEventGameConclude(wChairID,pIServerUserItem,cbConcludeReason);

		//�뿪�ж�
		if (m_TableUserItemArray[wChairID] != pIServerUserItem)
		{
			return true;
		}
	}

	//���ñ���
	if (pIServerUserItem==pITableUserItem)
	{
		if (CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule))
		{
			// �����йܴ���
			if ( (m_bGameStarted==true) && pIServerUserItem->GetUserStatus() == US_TRUSTEE)
			{
				// ������ϵ����ȫ��������״̬����ǿ�ƽ���������Ϸ
				bool bAllTrustee = true;
				for (WORD i = 0; i < m_wChairCount; i++)
				{
					if (GetTableUserItem(i) != NULL)
					{
						if (GetTableUserItem(i)->GetUserStatus() == US_PLAYING || GetTableUserItem(i)->GetUserStatus() == US_OFFLINE)
						{
							bAllTrustee = false;
						}
					}
				}
				if (bAllTrustee && (m_pGameServiceOption->wServerType&GAME_GENRE_USERCUSTOM == 0))
				{
					DismissGame();
				}

				return true;
			}
		}

		// ����û�п�ʼ,���Ҳ����Խ�ģʽ,�������뿪
		if ( (m_pGameServiceOption->wServerType&GAME_GENRE_USERCUSTOM) && m_bTableStarted== true)
		{
		}
		else
		{
			//������Ϸ��
			if (m_lFrozenedScore[wChairID] != 0L)
			{
				pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
				m_lFrozenedScore[wChairID] = 0L;
			}

			//�¼�֪ͨ
			if (m_pITableUserAction != NULL)
			{
				m_pITableUserAction->OnActionUserStandUp(wChairID, pIServerUserItem, false);
			}

			//�¼�֪ͨ
			if (m_pIMatchTableAction != NULL) m_pIMatchTableAction->OnActionUserStandUp(wChairID, pIServerUserItem, false);

			//���ñ���
			m_TableUserItemArray[wChairID] = NULL;

			//�û�״̬
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus((cbUserStatus == US_OFFLINE) ? US_NULL : US_FREE, INVALID_TABLE, INVALID_CHAIR);

			//��������
			bool bTableLocked = IsTableLocked();
			bool bTableStarted = IsTableStarted();
			WORD wTableUserCount = GetSitUserCount();

			//���ñ���
			m_wUserCount = wTableUserCount;

			//������Ϣ
			if (wTableUserCount == 0)
			{
				m_dwTableOwnerID = 0L;
				m_szEnterPassword[0] = 0;
			}

			//�����Թ�
			if (wTableUserCount == 0)
			{
				for (INT_PTR i = 0; i < m_LookonUserItemArray.GetCount(); i++)
				{
					SendGameMessage(m_LookonUserItemArray[i], TEXT("����Ϸ������������Ѿ��뿪�ˣ�"), SMT_CLOSE_GAME | SMT_EJECT);
				}
			}

			//��������
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM) == 0)
			{
				ConcludeTable();
			}

			//��ʼ�ж�
			if (EfficacyStartGame(INVALID_CHAIR) == true)
			{
				StartGame();
			}

			//����״̬
			if ((bTableLocked != IsTableLocked()) || (bTableStarted != IsTableStarted()))
			{
				SendTableStatus();
			}
		}

		return true;
	}
	else
	{
		//��������
		for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
		{
			if (pIServerUserItem==m_LookonUserItemArray[i])
			{
				//ɾ������
				m_LookonUserItemArray.RemoveAt(i);

				//�¼�֪ͨ
				if (m_pITableUserAction!=NULL)
				{
					m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,true);
				}

				//�¼�֪ͨ
				if(m_pIMatchTableAction!=NULL) m_pIMatchTableAction->OnActionUserStandUp(wChairID,pIServerUserItem,true);

				//�û�״̬
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);

				return true;
			}
		}

		//�������
		ASSERT(FALSE);
	}

	return true;
}

// �й��˳�ģʽ��վ��
bool CTableFrame::PerformStandUpActionInTrusteeMode(IServerUserItem * pIServerUserItem)
{
	if (pIServerUserItem == NULL)
	{
		return false;
	}
	if (CServerRule::IsAllowOffLineTrustee(m_pGameServiceOption->dwServerRule) && CServerRule::IsAllowTrusteeWriteScore(m_pGameServiceOption->dwServerRule))
	{
		// ���������й��˳�״̬,�������������ͨ״̬
		// �ж�����Ƿ��뿪�����ˣ�����뿪�ˣ���Ҫɾ�����
		if (pIServerUserItem->GetUserStatus() == US_TRUSTEE)
		{
			pIServerUserItem->SetUserStatus(US_SIT, m_wTableID, pIServerUserItem->GetChairID());
			PerformStandUpAction(pIServerUserItem);

			// �ж�����Ƿ��뿪�����ˣ�����뿪�ˣ���Ҫɾ�����
			if (pIServerUserItem->GetBindIndex() == INVALID_WORD)
			{
				pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
			}

			return true;
		}
	}

	return false;
}

//�Թ۶���
bool CTableFrame::PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword /*=NULL*/)
{
	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//��������
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//��Ϸ״̬
	if ((m_bGameStarted==false)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ϸ��û�п�ʼ�������Թ۴���Ϸ����"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//ģ�⴦��
	if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR && pIServerUserItem->IsAndroidUser()==false)
	{
		//�������
		CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

		//���һ���
		for (WORD i=0; i<m_pGameServiceAttrib->wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem *pIUserItem=m_TableUserItemArray[i];
			if(pIUserItem==NULL) continue;
			if(pIUserItem->IsAndroidUser()==false)break;

			//��ȡ����
			tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIUserItem->GetBindIndex());
			IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIUserItem->GetUserID(),pBindParameter->dwSocketID);
			tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

			//ģ���ж�
			if((pAndroidParameter->dwServiceGender&ANDROID_SIMULATE)!=0
				&& (pAndroidParameter->dwServiceGender&ANDROID_PASSIVITY)==0
				&& (pAndroidParameter->dwServiceGender&ANDROID_INITIATIVE)==0)
			{
				SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ���ӽ�ֹ�û��Թۣ�"),REQUEST_FAILURE_NORMAL);
				return false;
			}

			break;
		}
	}


	//�Թ��ж�
	if (CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule)==true
		&& (CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule)==false))
	{
		if ((pITableUserItem!=NULL)&&(pITableUserItem->IsAndroidUser()==true))
		{
			SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ�û��Թۣ�"),REQUEST_FAILURE_NORMAL);
			return false;
		}
	}

	//״̬�ж�
	if ((CServerRule::IsForfendGameLookon(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ�û��Թۣ�"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//�����ж�
	if ((pITableUserItem==NULL)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		SendRequestFailure(pIServerUserItem,TEXT("���������λ��û����Ϸ��ң��޷��Թ۴���Ϸ��"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//����Ч��
// 	if ((IsTableLocked()==true)&&(pIServerUserItem->GetMasterOrder()==0L)&&(lstrcmp(pUserRule->szPassword,m_szEnterPassword)!=0))
// 	{
// 		SendRequestFailure(pIServerUserItem,TEXT("��Ϸ���������벻��ȷ�������Թ���Ϸ��"),REQUEST_FAILURE_PASSWORD);
// 		return false;
// 	}
	//����Ч��
	if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && ((IsTableLocked()==true)&&(pIServerUserItem->GetMasterOrder()==0L))
		&&((lpszPassword==NULL)||(lstrcmp(lpszPassword,m_szEnterPassword)!=0)))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ϸ���������벻��ȷ�������Թ���Ϸ��"),REQUEST_FAILURE_PASSWORD);
		return false;
	}

	//��չЧ��
	if (m_pITableUserRequest!=NULL)
	{
		//��������
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//����Ч��
		if (m_pITableUserRequest->OnUserRequestLookon(wChairID,pIServerUserItem,RequestResult)==false)
		{
			//������Ϣ
			SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);

			return false;
		}
	}

	//�����û�
	m_LookonUserItemArray.Add(pIServerUserItem);

	//�û�״̬
	pIServerUserItem->SetClientReady(false);
	pIServerUserItem->SetUserStatus(US_LOOKON,m_wTableID,wChairID);

	//�¼�֪ͨ
	if (m_pITableUserAction!=NULL)
	{
		m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,true);
	}

	//�¼�֪ͨ
	if(m_pIMatchTableAction!=NULL) m_pIMatchTableAction->OnActionUserSitDown(wChairID,pIServerUserItem,true);
	return true;
}

//���¶���
bool CTableFrame::PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword)
{
	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//��������
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//���ֱ���
	SCORE lUserScore=pIServerUserItem->GetUserScore();
	SCORE lMinTableScore = m_pGameTableOption->lMinTableScore;
	SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);

	//״̬�ж�
	if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	{
		LOGI("CTableFrame::PerformSitDownAction, dwServerRule="<<m_pGameServiceOption->dwServerRule<<", GetMasterOrder="<<pIServerUserItem->GetMasterOrder())
		SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ���ӽ�ֹ�û����룡"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	// �йܴ���״̬
	if (pIServerUserItem->GetUserStatus() == US_TRUSTEE)
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ǹ������ǰ�����йܴ���״̬�����ܽ�����Ϸ����"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//ģ�⴦��
	if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR && pIServerUserItem->IsAndroidUser()==false)
	{
		//�������
		CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

		//���һ���
		for (WORD i=0; i<m_pGameServiceAttrib->wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem *pIUserItem=m_TableUserItemArray[i];
			if(pIUserItem==NULL) continue;
			if(pIUserItem->IsAndroidUser()==false)break;

			//��ȡ����
			tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIUserItem->GetBindIndex());
			IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIUserItem->GetUserID(),pBindParameter->dwSocketID);
			tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

			//ģ���ж�
			if((pAndroidParameter->dwServiceGender&ANDROID_SIMULATE)!=0
				&& (pAndroidParameter->dwServiceGender&ANDROID_PASSIVITY)==0
				&& (pAndroidParameter->dwServiceGender&ANDROID_INITIATIVE)==0)
			{
				LOGI("CTableFrame::PerformSitDownAction, dwServiceGender="<<pAndroidParameter->dwServiceGender);
				SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ���ӽ�ֹ�û����룡"),REQUEST_FAILURE_NORMAL);
				return false;
			}

			break;
		}
	}

	//��̬����
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

	//��Ϸ״̬
	if ((m_bGameStarted==true)&&(bDynamicJoin==false))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ϸ�Ѿ���ʼ�ˣ����ڲ��ܽ�����Ϸ����"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//�����ж�
	if (pITableUserItem!=NULL)
	{
		//������
		if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return false;

		//������Ϣ
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("�����Ѿ��� [ %s ] �����ȵ��ˣ��´ζ���Ҫ����ˣ�"),pITableUserItem->GetNickName());

		//������Ϣ
		SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

		return false;
	}

	//����Ч��
	if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && ((IsTableLocked()==true)&&(pIServerUserItem->GetMasterOrder()==0L))
		&&((lpszPassword==NULL)||(lstrcmp(lpszPassword,m_szEnterPassword)!=0)))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ϸ���������벻��ȷ�����ܼ�����Ϸ��"),REQUEST_FAILURE_PASSWORD);
		return false;
	}

	//����Ч��
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) == 0 && (m_pGameServiceOption->wServerType&GAME_GENRE_USERCUSTOM) == 0)
	{
		if (EfficacyEnterTableScoreRule(wChairID, pIServerUserItem) == false)
		{
			return false;
		}
		if (EfficacyEnterTableMemberRule(pIServerUserItem) == false)
		{
			return false;
		}
		if (EfficacyIPAddress(pIServerUserItem) == false)
		{
			return false;
		}
		if (EfficacyScoreRule(pIServerUserItem) == false)
		{
			return false;
		}
	}

	//��չЧ��
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && m_pITableUserRequest!=NULL)
	{
		//��������
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//����Ч��
		if (m_pITableUserRequest->OnUserRequestSitDown(wChairID,pIServerUserItem,RequestResult)==false)
		{
			//������Ϣ
			SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);

			return false;
		}
	}

	//���ñ���
	m_TableUserItemArray[wChairID]=pIServerUserItem;
	if ( (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM) == 0)
	{
		// ���Խ�ģʽ,���������ʱ,���þ���
		m_wDrawCount = 0;
	}

	//�û�״̬
	if ((IsGameStarted()==false)||(m_cbStartMode!=START_MODE_TIME_CONTROL))
	{
		if (CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)==false && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
		{
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,wChairID);
		}
		else
		{
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
		}
	}
	else
	{
		//���ñ���
		m_wOffLineCount[wChairID]=0L;
		m_dwOffLineTime[wChairID]=0L;

		//������Ϸ��
		if (m_pGameTableOption->lServiceScore > 0L)
		{
			m_lFrozenedScore[wChairID] = m_pGameTableOption->lServiceScore;
			pIServerUserItem->FrozenedUserScore(m_pGameTableOption->lServiceScore);
		}

		//����״̬
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,wChairID);
	}

	// �趨������ƻ���
	pIServerUserItem->SetRestrictScore(m_pGameTableOption->lRestrictScore);
	
	//���ñ���
	m_wUserCount=GetSitUserCount();

	//������Ϣ
	if (GetSitUserCount()==1)
	{
		//״̬����
		bool bTableLocked=IsTableLocked();

		//���ñ���
		m_dwTableOwnerID=pIServerUserItem->GetUserID();
		lstrcpyn(m_szEnterPassword,pUserRule->szPassword,CountArray(m_szEnterPassword));

		//����״̬
		if (bTableLocked!=IsTableLocked()) SendTableStatus();
	}

	//�¼�֪ͨ
	if (m_pITableUserAction!=NULL)
	{
		m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);
	}

	//�¼�֪ͨ
	if(m_pIMatchTableAction!=NULL) m_pIMatchTableAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);


	return true;
}

//����״̬
bool CTableFrame::SendTableStatus()
{
	//��������
	CMD_GR_TableStatus TableStatus;
	ZeroMemory(&TableStatus,sizeof(TableStatus));

	//��������
	TableStatus.wTableID=m_wTableID;
	TableStatus.TableStatus.cbTableLock=IsTableLocked()?TRUE:FALSE;
	TableStatus.TableStatus.cbPlayStatus=IsTableStarted()?TRUE:FALSE;

	//��������
	m_pIMainServiceFrame->SendData(BG_COMPUTER,MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	//�ֻ�����

	// �����Խ����������Ϣ
	if (m_pGameServiceOption->wServerType & GAME_GENRE_USERCUSTOM)
	{
		CMD_GF_CustomTableInfo CustomTableInfo;
		CustomTableInfo.dwUserID = m_pGameTableOption->dwCreatUserID;
		CustomTableInfo.bTableStarted = m_bTableStarted;
		CustomTableInfo.dwRoundCount = m_pGameTableOption->dwRoundCount;
		CustomTableInfo.dwPlayRoundCount = m_wDrawCount;
		lstrcpyn(CustomTableInfo.szSecret, m_pGameTableOption->szSecret, sizeof(CustomTableInfo));
		for (WORD i = 0; i < GetChairCount(); i++)
		{
			SendTableData(i, SUB_GF_CUSTOMTABLE_INFO, &CustomTableInfo, sizeof(CustomTableInfo), MDM_GF_FRAME);
		}
	}

	return true;
}

//����ʧ��
bool CTableFrame::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//��������
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//��������
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(RequestFailure.szDescribeString);
	WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wHeadSize+wDataSize);

	return true;
}

//��ʼЧ��
bool CTableFrame::EfficacyStartGame(WORD wReadyChairID)
{
	//״̬�ж�
	if (m_bGameStarted==true) return false;

	//ģʽ����
	if (m_cbStartMode==START_MODE_TIME_CONTROL) return false;
	if (m_cbStartMode==START_MODE_MASTER_CONTROL) return false;

	//׼������
	WORD wReadyUserCount=0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem==NULL) continue;

		//�û�ͳ��
		if (pITableUserItem!=NULL)
		{
			//״̬�ж�
			if (pITableUserItem->IsClientReady()==false) return false;
			if ((wReadyChairID!=i)&&(pITableUserItem->GetUserStatus()!=US_READY)) return false;

			//�û�����
			wReadyUserCount++;
		}
	}

	//��ʼ����
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:			//����׼��
		{
			//��Ŀ�ж�
			if (wReadyUserCount>=2L) return true;

			return false;
		}
	case START_MODE_FULL_READY:			//���˿�ʼ
		{
			//�����ж�
			if (wReadyUserCount==m_wChairCount) return true;

			return false;
		}
	case START_MODE_PAIR_READY:			//��Կ�ʼ
		{
			//��Ŀ�ж�
			if (wReadyUserCount==m_wChairCount) return true;
			if ((wReadyUserCount<2L)||(wReadyUserCount%2)!=0) return false;

			//λ���ж�
			for (WORD i=0;i<m_wChairCount/2;i++)
			{
				//��ȡ�û�
				IServerUserItem * pICurrentUserItem=GetTableUserItem(i);
				IServerUserItem * pITowardsUserItem=GetTableUserItem(i+m_wChairCount/2);

				//λ�ù���
				if ((pICurrentUserItem==NULL)&&(pITowardsUserItem!=NULL)) return false;
				if ((pICurrentUserItem!=NULL)&&(pITowardsUserItem==NULL)) return false;
			}

			return true;
		}
	default:
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return false;
}

//��ַЧ��
bool CTableFrame::EfficacyIPAddress(IServerUserItem * pIServerUserItem)
{
	//����Ա��������
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//�����ж�
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return true;

	//��ַЧ��
	const tagUserRule * pUserRule=pIServerUserItem->GetUserRule(),*pTableUserRule=NULL;
	bool bCheckSameIP=pUserRule->bLimitSameIP;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
		{
			pTableUserRule=pITableUserItem->GetUserRule();
			if (pTableUserRule->bLimitSameIP==true) 
			{
				bCheckSameIP=true;
				break;
			}
		}
	}

	//��ַЧ��
	if (bCheckSameIP==true)
	{
		DWORD dwUserIP=pIServerUserItem->GetClientAddr();
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if ((pITableUserItem!=NULL)&&(pITableUserItem != pIServerUserItem)&&(!pITableUserItem->IsAndroidUser())&&(pITableUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==dwUserIP))
			{
				if (!pUserRule->bLimitSameIP)
				{
					//������Ϣ
					LPCTSTR pszDescribe=TEXT("����Ϸ����������˲�����ͬ IP ��ַ�������Ϸ���� IP ��ַ�����ҵ� IP ��ַ��ͬ�����ܼ�����Ϸ��");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					return false;
				}
				else
				{
					//������Ϣ
					LPCTSTR pszDescribe=TEXT("�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ���������� IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					return false;
				}
			}
		}
		for (WORD i=0;i<m_wChairCount-1;i++)
		{
			//��ȡ�û�
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
			{
				for (WORD j=i+1;j<m_wChairCount;j++)
				{
					//��ȡ�û�
					IServerUserItem * pITableNextUserItem=GetTableUserItem(j);
					if ((pITableNextUserItem!=NULL) && (!pITableNextUserItem->IsAndroidUser()) && (pITableNextUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==pITableNextUserItem->GetClientAddr()))
					{
						LPCTSTR pszDescribe=TEXT("�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ������ IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
						SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
						return false;
					}
				}
			}
		}
	}

	return true;
}

// ��ַУ��(�����Խ�����У��)
bool CTableFrame::EfficacyIPAddress()
{
	// ������������Ƿ�����ϵ����IP��ͬ,��ͬ�Ļ������������
	map<DWORD, vector<DWORD>> mapUserIPInfo;		// ���IP��Ϣ(map<IP, vector<User>>)
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem = GetTableUserItem(i);
		if (pITableUserItem == nullptr || pITableUserItem->IsAndroidUser() || pITableUserItem->GetMasterOrder() != 0)
		{
			continue;
		}

		auto iter = mapUserIPInfo.find(pITableUserItem->GetClientAddr());
		if (iter != mapUserIPInfo.end())
		{
			iter->second.push_back(pITableUserItem->GetUserID());
		}
		else
		{
			vector<DWORD> vectUserID;
			vectUserID.push_back(pITableUserItem->GetUserID());
			mapUserIPInfo.insert(make_pair(pITableUserItem->GetClientAddr(), vectUserID));
		}
	}

	for (auto &iter : mapUserIPInfo)
	{
		if (iter.second.size() > 1)
		{
			// �����������IP��ͬ
			TCHAR szMessage[256] = { 0 };
			_sntprintf(szMessage, sizeof(szMessage), TEXT("���"));
			for (size_t i = 0; i < iter.second.size(); i++)
			{
				IServerUserItem* pITableUserItem = SearchUserItem(iter.second.at(i));
				if (pITableUserItem)
				{
					if (i == iter.second.size() - 1)
					{
						_sntprintf(szMessage, sizeof(szMessage), TEXT("%s[%s]"), szMessage, pITableUserItem->GetNickName());
					}
					else
					{
						_sntprintf(szMessage, sizeof(szMessage), TEXT("%s[%s],"), szMessage, pITableUserItem->GetNickName());
					}
				}
			}
			_sntprintf(szMessage, sizeof(szMessage), TEXT("%sΪͬһIP��ַ"), szMessage);

			SendGameMessage(szMessage, SMT_CHAT | SMT_EJECT);
		}
	}
	return true;
}

//����Ч��
bool CTableFrame::EfficacyScoreRule(IServerUserItem * pIServerUserItem)
{
	//����Ա��������
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//�����ж�
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return true;

	//��������
	WORD wWinRate=pIServerUserItem->GetUserWinRate();
	WORD wFleeRate=pIServerUserItem->GetUserFleeRate();

	//���ַ�Χ
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem=GetTableUserItem(i);

		//����Ч��
		if (pITableUserItem!=NULL)
		{
			//��ȡ����
			tagUserRule * pTableUserRule=pITableUserItem->GetUserRule();

			//����Ч��
			if ((pTableUserRule->bLimitFleeRate)&&(wFleeRate>pTableUserRule->wMaxFleeRate))
			{
				//������Ϣ
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("����������̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

				//������Ϣ
				SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

				return false;
			}

			//ʤ��Ч��
			if ((pTableUserRule->bLimitWinRate)&&(wWinRate<pTableUserRule->wMinWinRate))
			{
				//������Ϣ
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("����ʤ��̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

				//������Ϣ
				SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

				return false;
			}

			//����Ч��
			if (pTableUserRule->bLimitGameScore==true)
			{
				//��߻���
				if (pIServerUserItem->GetUserScore()>pTableUserRule->lMaxGameScore)
				{
					//������Ϣ
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf(szDescribe,CountArray(szDescribe),TEXT("���Ļ���̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

					//������Ϣ
					SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

					return false;
				}

				//��ͻ���
				if (pIServerUserItem->GetUserScore()<pTableUserRule->lMinGameScore)
				{
					//������Ϣ
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf(szDescribe,CountArray(szDescribe),TEXT("���Ļ���̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

					//������Ϣ
					SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

					return false;
				}
			}
		}
	}

	return true;
}

//����Ч��
bool CTableFrame::EfficacyEnterTableScoreRule(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//���ֱ���
	SCORE lUserScore=pIServerUserItem->GetUserScore();
	SCORE lMinTableScore = m_pGameTableOption->lMinTableScore;
	SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);

	if (((lMinTableScore!=0L)&&(lUserScore<lMinTableScore))||((lLessEnterScore!=0L)&&(lUserScore<lLessEnterScore)))
	{
		//������Ϣ
		TCHAR szDescribe[128]=TEXT("");
		if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("������Ϸ������Ҫ ") SCORE_STRING TEXT(" ����Ϸ�ң�������Ϸ�Ҳ��������ܼ��룡"),__max(lMinTableScore,lLessEnterScore));
		else if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("������Ϸ������Ҫ ") SCORE_STRING TEXT(" �ı����ң����ı����Ҳ��������ܼ��룡"),__max(lMinTableScore,lLessEnterScore));
		else
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("������Ϸ������Ҫ ") SCORE_STRING TEXT(" ����Ϸ���֣����Ļ��ֲ��������ܼ��룡"),__max(lMinTableScore,lLessEnterScore));

		//������Ϣ
		SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NOSCORE);

		return false;
	}

//	SCORE lMaxTableScore = m_pGameServiceOption->TableOption[m_wTableID].lMaxTableScore;
	SCORE lMaxTableScore = m_pGameTableOption->lMaxTableScore;
	if ((lMaxTableScore != 0) && lUserScore > lMaxTableScore)
	{
		//������Ϣ
		TCHAR szDescribe[128] = TEXT("");
		if (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
			_sntprintf(szDescribe, CountArray(szDescribe), TEXT("������Ϸ���಻�ܳ��� ") SCORE_STRING TEXT(" ����Ϸ�ң�������Ϸ�Ҳ��������ã����ܼ��룡"), lMaxTableScore);
		else if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
			_sntprintf(szDescribe, CountArray(szDescribe), TEXT("������Ϸ���಻�ܳ��� ") SCORE_STRING TEXT(" �ı����ң����ı����Ҳ��������ã����ܼ��룡"), lMaxTableScore);
		else
			_sntprintf(szDescribe, CountArray(szDescribe), TEXT("������Ϸ���಻�ܳ��� ") SCORE_STRING TEXT(" ����Ϸ���֣����Ļ��ֲ��������ã����ܼ��룡"), lMaxTableScore);

		//������Ϣ
		SendRequestFailure(pIServerUserItem, szDescribe, REQUEST_FAILURE_NOSCORE);
		return false;
	}

	return true;
}

// Ȩ��У��
bool CTableFrame::EfficacyEnterTableMemberRule(IServerUserItem * pIServerUserItem)
{
	BYTE cbMemberOrder = pIServerUserItem->GetMemberOrder();
	BYTE cbMasterOrder = pIServerUserItem->GetMasterOrder();
	//��Ա�ж�
	if (m_pGameTableOption->cbMinTableEnterMember != 0 && cbMemberOrder < m_pGameTableOption->cbMinTableEnterMember && (cbMemberOrder == 0))
	{
		//����ʧ��
		TCHAR szDescribe[128] = TEXT("");
		_sntprintf(szDescribe, CountArray(szDescribe), TEXT("��Ǹ�����Ļ�Ա������ڵ�ǰ����������»�Ա�������������£�"));

		//�������
		SendRequestFailure(pIServerUserItem, szDescribe, REQUEST_FAILURE_NOSCORE);
	
		return false;
	}

	//��Ա�ж�
	if (m_pGameTableOption->cbMinTableEnterMember != 0 && cbMemberOrder > m_pGameTableOption->cbMinTableEnterMember && (cbMasterOrder == 0))
	{
		//����ʧ��
		TCHAR szDescribe[128] = TEXT("");
		_sntprintf(szDescribe, CountArray(szDescribe), TEXT("��Ǹ�����Ļ�Ա������ڵ�ǰ����������߻�Ա�������������£�"));

		//�������
		SendRequestFailure(pIServerUserItem, szDescribe, REQUEST_FAILURE_NOSCORE);

		return false;
	}
	return true;
}

bool CTableFrame::SetMatchInterface(IUnknownEx * pIUnknownEx)
{
	return NULL;
}

//������
bool CTableFrame::CheckDistribute()
{
	//������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule))
	{
		//����״��
		tagTableUserInfo TableUserInfo;
		WORD wUserSitCount=GetTableUserInfo(TableUserInfo);

		//�û�����
		if(wUserSitCount < TableUserInfo.wMinUserCount)
		{
			return true;
		}
	}

	return false;
}

//��Ϸ��¼
void CTableFrame::RecordGameScore(bool bDrawStarted)
{
	if (bDrawStarted==true)
	{
		//д���¼
		if (CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			LOGI("CTableFrame::RecordGameScore");
			//��������
			DBR_GR_GameScoreRecord GameScoreRecord;
			ZeroMemory(&GameScoreRecord,sizeof(GameScoreRecord));

			//���ñ���
			GameScoreRecord.wTableID=m_wTableID;
			GameScoreRecord.dwPlayTimeCount=(bDrawStarted==true)?(DWORD)time(NULL)-m_dwDrawStartTime:0;

			//��Ϸʱ��
			GameScoreRecord.SystemTimeStart=m_SystemTimeStart;
			GetLocalTime(&GameScoreRecord.SystemTimeConclude);

			//�û�����
			for (INT_PTR i=0;i<m_GameScoreRecordActive.GetCount();i++)
			{
				//��ȡ����
				ASSERT(m_GameScoreRecordActive[i]!=NULL);
				tagGameScoreRecord * pGameScoreRecord=m_GameScoreRecordActive[i];

				//�û���Ŀ
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.wUserCount++;
				}
				else
				{
					GameScoreRecord.wAndroidCount++;
				}

				//����ͳ��
				GameScoreRecord.dwUserMemal+=pGameScoreRecord->dwUserMemal;

				//ͳ����Ϣ
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.lWasteCount-=(pGameScoreRecord->lScore+pGameScoreRecord->lRevenue);
					GameScoreRecord.lRevenueCount+=pGameScoreRecord->lRevenue;
				}

				//��Ϸ����Ϣ
				if (GameScoreRecord.wRecordCount<CountArray(GameScoreRecord.GameScoreRecord))
				{
					WORD wIndex=GameScoreRecord.wRecordCount++;
					CopyMemory(&GameScoreRecord.GameScoreRecord[wIndex],pGameScoreRecord,sizeof(tagGameScoreRecord));
				}
			}

			//Ͷ������
			if(GameScoreRecord.wUserCount > 0)
			{
				WORD wHeadSize=sizeof(GameScoreRecord)-sizeof(GameScoreRecord.GameScoreRecord);
				WORD wDataSize=sizeof(GameScoreRecord.GameScoreRecord[0])*GameScoreRecord.wRecordCount;
				m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_GAME_SCORE_RECORD,0,&GameScoreRecord,wHeadSize+wDataSize);
			}
		}

		//�����¼
		if (m_GameScoreRecordActive.GetCount()>0L)
		{
			m_GameScoreRecordBuffer.Append(m_GameScoreRecordActive);
			m_GameScoreRecordActive.RemoveAll();
		}
	}

}
//////////////////////////////////////////////////////////////////////////////////
