#include "StdAfx.h"
#include ".\squaremanager.h"
#include <algorithm>
//////////////////////////////////////////////////////////////////////////////////

bool sortbyscore(tagSQUARERANK e1, tagSQUARERANK e2)
{
	if(e1.lScore!=e2.lScore)
		return e1.lScore > e2.lScore;

	if(e1.wResult!=e2.wResult)
		return e1.wResult > e2.wResult;

	if(e1.dwEnterTime!=e2.dwEnterTime)
		return e1.dwEnterTime < e2.dwEnterTime;

	return 0;
}

//��ȡ�ַ�����
int GetCharLength(TCHAR chInfo)
{
	if( chInfo >= 0xA0 )//ȫ�� ���� 
	{ 
		return 2;
	} 
	else					//��� Ӣ�� 
	{ 
		return 1;
	} 
}

//��ȡ�ַ�������
int GetStringLength(LPCTSTR str)
{
	CString strInfo(str);
	int	nLengthX = 0;
	for ( int i = 0 ; i < lstrlen(strInfo); ++i )
	{
		nLengthX += GetCharLength(strInfo[i]);
	}
	return nLengthX;
}

//nMaxLength �ַ���󳤶�, nDrawLength ��ʾ��󳤶�
void TransformString(LPCTSTR str, CString& strInfo, int nMaxLength, int nDrawLength)
{
	//ת���ַ���
	strInfo=str;
	int	nLengthX = 0;
	bool bComplete = false;
	for ( int i = 0 ; i < lstrlen(strInfo); ++i )
	{
		nLengthX += GetCharLength(strInfo[i]);
		if(nLengthX >nDrawLength)
		{
			//�ȵ�����ʾ���ޣ�����"..."���ȡ�ַ���
			strInfo.Insert(i-3,TEXT("..."));
			strInfo=strInfo.Left(i);
			nLengthX=GetStringLength(strInfo);
			int nRightLength=nDrawLength-nLengthX;
			if(nRightLength>0)
			{
				//����֮�䳤�����ո�
				for(int j=0; j<nRightLength; j++)
				{
					strInfo.Insert(i,TEXT(" "));
				}
				strInfo=strInfo.Left(i+nRightLength);
				break;
			}

			break;
		}

		if( nLengthX > nMaxLength)
		{				
			
			//�����ַ�������(˵��nMaxLength<=nDrawLength)������"..."���ȡ�ַ���
			strInfo.Insert(i-3,TEXT("..."));
			strInfo=strInfo.Left(i);

			nLengthX=GetStringLength(strInfo);
			if(nLengthX>nDrawLength)
			{
				//������ʾ���ޣ�����"..."���ȡ�ַ���
				strInfo.Insert(i-3,TEXT("..."));
				strInfo=strInfo.Left(i);
				break;
			}

			nLengthX=GetStringLength(strInfo);
			int nRightLength=nDrawLength-nLengthX;
			if(nRightLength>0)
			{
				//����֮�䳤�����ո�
				for(int j=0; j<nRightLength; j++)
				{
					strInfo.Insert(i,TEXT(" "));
				}
				strInfo=strInfo.Left(i+nRightLength);
				break;
			}

			break;
		}

		if(i+1>=lstrlen(strInfo))
		{
			nLengthX=GetStringLength(strInfo);
			int nRightLength=nDrawLength-nLengthX;
			if(nRightLength>0)
			{
				//����֮�䳤�����ո�
				for(int j=0; j<nRightLength; j++)
				{
					strInfo.Insert(i+1,TEXT(" "));
				}
				strInfo=strInfo.Left(i+1+nRightLength);
				break;
			}
		}


	}
}

//////////////////////////////////////////////////////////////////////////////////

CSquare::CSquare()
{
	//���ñ���
	m_bStartSquare=FALSE;
	m_bEndSquare=FALSE;
	m_dwSquareRemain=0;
	m_dwAnnounceInterval=0;
	m_SquareScoreMap.clear();
	m_StrSelfRankBonusMap.clear();
	m_StrSelfResultBonusMap.clear();

	//ϵͳ����
	m_pGameParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
	m_pGameSquareOption=NULL;

	//����ӿ�
	m_pITimerEngine=NULL;
	m_pIMainServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pIKernelDataBaseEngine=NULL;
	m_pIRecordDataBaseEngine=NULL;
}

CSquare::~CSquare()
{
}

//�������
bool CSquare::InitSquareMatch(tagSquareMatchParameter SquareMatchParameter)
{
	//���ñ���
	m_pGameParameter=SquareMatchParameter.pGameParameter;
	m_pGameServiceAttrib=SquareMatchParameter.pGameServiceAttrib;
	m_pGameServiceOption=SquareMatchParameter.pGameServiceOption;
	m_pGameSquareOption=SquareMatchParameter.pGameSquareOption;

	//����ӿ�
	m_pITimerEngine=SquareMatchParameter.pITimerEngine;
	m_pIMainServiceFrame=SquareMatchParameter.pIMainServiceFrame;
	m_pIServerUserManager=SquareMatchParameter.pIServerUserManager;
	m_pIKernelDataBaseEngine=SquareMatchParameter.pIKernelDataBaseEngine;
	m_pIRecordDataBaseEngine=SquareMatchParameter.pIRecordDataBaseEngine;

	return true;
}

void CSquare::StartSquare()
{
	//�㳡����ʼ
	m_bStartSquare=TRUE;
	m_bEndSquare=FALSE;

	//ʣ��ʱ��
	CTime timeEndTime(m_pGameSquareOption->EndTime);
	m_dwSquareRemain=(DWORD)(timeEndTime.GetTime()-(CTime::GetCurrentTime()).GetTime());

	//��������б�
	m_SquareScoreMap.clear();
	//ö���û�
	WORD wEnumIndex=0;
	while(wEnumIndex<m_pIServerUserManager->GetUserItemCount())
	{
		//�����û�
		IServerUserItem *pIServerUserItem=m_pIServerUserManager->EnumUserItem(wEnumIndex++);
		if(pIServerUserItem==NULL) continue;

		tagSQUARERANK SquareRank;
		SquareRank.lScore=0;
		SquareRank.wResult=0;
		SquareRank.dwUserID=pIServerUserItem->GetUserID();
		SquareRank.dwEnterTime=(DWORD)(CTime::GetCurrentTime().GetTime());
		lstrcpyn(SquareRank.szNickName,pIServerUserItem->GetNickName(),CountArray(SquareRank.szNickName));
		m_SquareScoreMap[SquareRank.dwUserID]=SquareRank;
	}

	//���͹㳡�����ơ�����
	TCHAR szSquareBuffer[1024]=TEXT("");
	_sntprintf(szSquareBuffer,CountArray(szSquareBuffer),\
		TEXT("%s��ʼ��,���Ѿ���ò����ʸ�,����ʱ��Ϊ%d-%02d-%02d %02d:%02d:%02d - %d-%02d-%02d %02d:%02d:%02d,ף������."),\
		m_pGameSquareOption->szSquareName,\
		m_pGameSquareOption->StartTime.wYear,m_pGameSquareOption->StartTime.wMonth,m_pGameSquareOption->StartTime.wDay,\
		m_pGameSquareOption->StartTime.wHour,m_pGameSquareOption->StartTime.wMinute,m_pGameSquareOption->StartTime.wSecond,\
		m_pGameSquareOption->EndTime.wYear,m_pGameSquareOption->EndTime.wMonth,m_pGameSquareOption->EndTime.wDay,\
		m_pGameSquareOption->EndTime.wHour,m_pGameSquareOption->EndTime.wMinute,m_pGameSquareOption->EndTime.wSecond);
	m_pIMainServiceFrame->SendRoomMessage(szSquareBuffer,SMT_CHAT);
	if(lstrlen(m_pGameSquareOption->szSquareDesc)!=0)m_pIMainServiceFrame->SendRoomMessage(m_pGameSquareOption->szSquareDesc,SMT_CHAT);
	LOGI("<"<< m_pGameSquareOption->wMatchID << "|" << m_pGameSquareOption->szSquareName << "> " << szSquareBuffer << " ʣ��ʱ��SquareRemain= " << m_dwSquareRemain);
}

//�������������
BYTE CSquare::ResultToRank(ITableFrame* pITableFrame, tagScoreInfo ScoreInfoArray[], WORD wScoreCount)
{
	if(pITableFrame==NULL)return 0;
	LOGI("<"<< m_pGameServiceOption->wServerID << "|" << m_pGameServiceOption->szServerName << "> CSquare::ResultToRank");

	if (!m_bStartSquare)return 0;

	for(WORD i=0; i<wScoreCount; i++)
	{
		IServerUserItem* pIServerUserItem=pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem == NULL)
		{
			//LOGE("<"<<m_pGameServiceOption->szServerName<<"> CSquare::ResultToRank()... pIServerUserItem=NULL... dwTableID="<< pITableFrame->GetTableID()\
			//	<<", wChairID ="<< i);
			continue;
		}

		if(ScoreInfoArray[i].cbType == SCORE_TYPE_NULL || ScoreInfoArray[i].cbType == SCORE_TYPE_DRAW)
		{
			LOGI("<"<< m_pGameServiceOption->wServerID << "|" << m_pGameServiceOption->szServerName << "> CSquare::ResultToRank() ��ͳ�ƹ㳡������ wChairID ="\
				<< i << " wUserID= " << pIServerUserItem->GetUserID() << " ScoreType= " << ScoreInfoArray[i].cbType << " lScore= " << ScoreInfoArray[i].lScore);
			continue;
		}

		map<DWORD, tagSQUARERANK>::iterator iterRank = m_SquareScoreMap.find(pIServerUserItem->GetUserID());
		if (iterRank == m_SquareScoreMap.end())
		{
			tagSQUARERANK SquareRank;
			SquareRank.lScore=ScoreInfoArray[i].lScore;
			SquareRank.wResult=1;
			SquareRank.dwUserID=pIServerUserItem->GetUserID();
			SquareRank.dwEnterTime=(DWORD)(CTime::GetCurrentTime().GetTime());
			lstrcpyn(SquareRank.szNickName,pIServerUserItem->GetNickName(),CountArray(SquareRank.szNickName));
			m_SquareScoreMap[SquareRank.dwUserID]=SquareRank;
		}
		else
		{
			iterRank->second.lScore+=ScoreInfoArray[i].lScore;
			iterRank->second.wResult++;
		}
	}

	//��ÿһ����ҷ��ͼ�ʱ����
	vector<tagSQUARERANK> vSquareRank;
	for (map<DWORD, tagSQUARERANK>::iterator iterRank=m_SquareScoreMap.begin(); iterRank != m_SquareScoreMap.end(); iterRank++)
	{
		vSquareRank.push_back(iterRank->second); 
	}
	sort(vSquareRank.begin(), vSquareRank.end(), sortbyscore); 

	map<DWORD, int> PlayerRankMap;
	for (size_t i=0; i<vSquareRank.size(); i++)
	{
		PlayerRankMap.insert(make_pair(vSquareRank[i].dwUserID, (int)(i+1)));
	}

	for(WORD i=0; i<wScoreCount; i++)
	{
		IServerUserItem* pIServerUserItem=pITableFrame->GetTableUserItem(i);
		if(pIServerUserItem == NULL)
		{
			//LOGE("<"<<m_pGameServiceOption->szServerName<<"> CSquare::ResultToRank()... pIServerUserItem=NULL... dwTableID="<< pITableFrame->GetTableID()\
			//	<<", wChairID ="<< i);
			continue;
		}

		if(ScoreInfoArray[i].cbType == SCORE_TYPE_NULL || ScoreInfoArray[i].cbType == SCORE_TYPE_DRAW)
		{
			LOGI("<"<< m_pGameServiceOption->wServerID << "|" << m_pGameServiceOption->szServerName << "> CSquare::ResultToRank() ���·�����ҵ�������Ϣ wChairID ="<<\
				i << " wUserID= " << pIServerUserItem->GetUserID() << " ScoreType= " << ScoreInfoArray[i].cbType << " lScore= " << ScoreInfoArray[i].lScore);
			continue;
		}

		map<DWORD, int>::iterator iterRank1 = PlayerRankMap.find(pIServerUserItem->GetUserID());
		if (iterRank1 == PlayerRankMap.end())
		{
			LOGE("<"<<m_pGameServiceOption->szServerName<<"> CSquare::ResultToRank()... has not rank1");
			continue;
		}
		DWORD Rank1ID=iterRank1->first;
		int result=iterRank1->second;

		map<DWORD, tagSQUARERANK>::iterator iterRank2 = m_SquareScoreMap.find(pIServerUserItem->GetUserID());
		if (iterRank2 == m_SquareScoreMap.end())
		{
			LOGE("<"<<m_pGameServiceOption->szServerName<<"> CSquare::ResultToRank()... has not rank2");
			continue;
		}
		DWORD Rank2ID=iterRank2->first;
		tagSQUARERANK SquareRank=iterRank2->second;

		CString strString=TEXT("");
		TCHAR szBuf[1024]=TEXT("");
		if (PlayerRankMap.size() != 0)
		{
			CString strWantResultBonus=TEXT("");
			for (BYTE k=0; k<m_pGameSquareOption->cbRankBonusByResult; k++)
			{
				if (m_pGameSquareOption->RankBonusByResult[k].wFirst== 0||m_pGameSquareOption->RankBonusByResult[k].wLast==0)
					break;

				if (iterRank2->second.wResult < m_pGameSquareOption->RankBonusByResult[k].wFirst)
				{
					TCHAR szWantResultBonusBuf[255];
					_sntprintf(szWantResultBonusBuf,sizeof(szWantResultBonusBuf)-1,m_pGameSquareOption->RankBonusByResult[k].szDesc,\
						iterRank2->second.wResult,m_pGameSquareOption->RankBonusByResult[k].wLast-iterRank2->second.wResult);
					strWantResultBonus = szWantResultBonusBuf;
				}
			}

			TCHAR szBuf1[512]=TEXT("");
			_sntprintf(szBuf1,sizeof(szBuf1)-1,TEXT("���ڹ㳡������������%ld������:%lld����"), iterRank1->second, iterRank2->second.lScore);
			TCHAR szBuf2[512]=TEXT("");
			_sntprintf(szBuf2,sizeof(szBuf2)-1,TEXT("����%ld%%����� %s"),\
				(int)(((PlayerRankMap.size() - (iterRank1->second - 1)) * 100.0) / PlayerRankMap.size()), strWantResultBonus.IsEmpty()?TEXT(""):strWantResultBonus.GetBuffer());

			//_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("���ڹ㳡������������%ld������:%lld��������%ld%%����� %s"), iterRank1->second, iterRank2->second.lScore,
			//	(int)(((PlayerRankMap.size() - (iterRank1->second - 1)) * 100.0) / PlayerRankMap.size()), strWantResultBonus.IsEmpty()?TEXT(""):strWantResultBonus.GetBuffer());
			strWantResultBonus.ReleaseBuffer();
			
			_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("%s%s"), szBuf1, szBuf2);
		}
		else
		{
			_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("���ڹ㳡������������%ld������:%lld��"), iterRank1->second, iterRank2->second.lScore);
		}
		strString += szBuf;

		m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,strString.GetBuffer(),SMT_CHAT);
		strString.ReleaseBuffer();
	}

	return 0;
}

//�������������
BYTE CSquare::ResultToRank(ITableFrame* pITableFrame, WORD wChairID, tagScoreInfo ScoreInfo)
{
	if(pITableFrame==NULL)return 0;
	LOGI("<"<< m_pGameSquareOption->wMatchID << "|" << m_pGameSquareOption->szSquareName << "> CSquare::ResultToRank, wTableID= " << pITableFrame->GetTableID() << " wChairID= " << wChairID);

	if (!m_bStartSquare)return 0;

	IServerUserItem* pIServerUserItem=pITableFrame->GetTableUserItem(wChairID);
	if(pIServerUserItem == NULL)
	{
		LOGE("<"<< m_pGameServiceOption->wServerID << "|" << m_pGameServiceOption->szServerName << "> CSquare::ResultToRank()... pIServerUserItem=NULL... "\
			<<", wChairID ="<< wChairID);
		return 0;
	}

	if(ScoreInfo.cbType == SCORE_TYPE_NULL || ScoreInfo.cbType == SCORE_TYPE_DRAW)
	{
		LOGI("<"<< m_pGameServiceOption->wServerID << "|" << m_pGameServiceOption->szServerName << "> CSquare::ResultToRank() ��ͳ�ƹ㳡������ wChairID ="\
			<< wChairID << " wUserID= " << pIServerUserItem->GetUserID() << " ScoreType= " << ScoreInfo.cbType << " lScore= " << ScoreInfo.lScore);
		return 0;
	}

	map<DWORD, tagSQUARERANK>::iterator iterRank = m_SquareScoreMap.find(pIServerUserItem->GetUserID());
	if (iterRank == m_SquareScoreMap.end())
	{
		tagSQUARERANK SquareRank;
		SquareRank.lScore=ScoreInfo.lScore;
		SquareRank.wResult=1;
		SquareRank.dwUserID=pIServerUserItem->GetUserID();
		SquareRank.dwEnterTime=(DWORD)(CTime::GetCurrentTime().GetTime());
		lstrcpyn(SquareRank.szNickName,pIServerUserItem->GetNickName(),CountArray(SquareRank.szNickName));
		m_SquareScoreMap[SquareRank.dwUserID]=SquareRank;
	}
	else
	{
		iterRank->second.lScore+=ScoreInfo.lScore;
		iterRank->second.wResult++;
	}

	//��ÿһ����ҷ��ͼ�ʱ����
	vector<tagSQUARERANK> vSquareRank;
	for (map<DWORD, tagSQUARERANK>::iterator iterRank=m_SquareScoreMap.begin(); iterRank != m_SquareScoreMap.end(); iterRank++)
	{
		vSquareRank.push_back(iterRank->second); 
	}
	sort(vSquareRank.begin(), vSquareRank.end(), sortbyscore); 

	map<DWORD, int> PlayerRankMap;
	for (size_t i=0; i<vSquareRank.size(); i++)
	{
		PlayerRankMap.insert(make_pair(vSquareRank[i].dwUserID, (int)(i+1)));
	}

	if(ScoreInfo.cbType == SCORE_TYPE_NULL || ScoreInfo.cbType == SCORE_TYPE_DRAW)
	{
		LOGI("<"<< m_pGameServiceOption->wServerID << "|" << m_pGameServiceOption->szServerName << "> CSquare::ResultToRank() ���·�����ҵ�������Ϣ wChairID ="<<\
			wChairID << " wUserID= " << pIServerUserItem->GetUserID() << " ScoreType= " << ScoreInfo.cbType << " lScore= " << ScoreInfo.lScore);
		return 0;
	}

	map<DWORD, int>::iterator iterRank1 = PlayerRankMap.find(pIServerUserItem->GetUserID());
	if (iterRank1 == PlayerRankMap.end())
	{
		LOGE("<"<<m_pGameServiceOption->szServerName<<"> CSquare::ResultToRank()... has not rank1! PlayerRankMap.size= " << PlayerRankMap.size() << " wUserID= " << pIServerUserItem->GetUserID());
		return 0;
	}
	DWORD Rank1ID=iterRank1->first;
	int result=iterRank1->second;

	map<DWORD, tagSQUARERANK>::iterator iterRank2 = m_SquareScoreMap.find(pIServerUserItem->GetUserID());
	if (iterRank2 == m_SquareScoreMap.end())
	{
		LOGE("<"<<m_pGameServiceOption->szServerName<<"> CSquare::ResultToRank()... has not rank2! m_SquareScoreMap.size= " << m_SquareScoreMap.size() << " wUserID= " << pIServerUserItem->GetUserID());
		return 0;
	}
	DWORD Rank2ID=iterRank2->first;
	tagSQUARERANK SquareRank=iterRank2->second;

	CString strString=TEXT("");
	TCHAR szBuf[1024]=TEXT("");
	if (PlayerRankMap.size() != 0)
	{
		CString strWantResultBonus=TEXT("");
		for (BYTE k=0; k<m_pGameSquareOption->cbRankBonusByResult; k++)
		{
			if (m_pGameSquareOption->RankBonusByResult[k].wFirst== 0||m_pGameSquareOption->RankBonusByResult[k].wLast==0)
				break;

			if (iterRank2->second.wResult < m_pGameSquareOption->RankBonusByResult[k].wFirst)
			{
				TCHAR szWantResultBonusBuf[255];
				_sntprintf(szWantResultBonusBuf,sizeof(szWantResultBonusBuf)-1,m_pGameSquareOption->RankBonusByResult[k].szDesc,\
					iterRank2->second.wResult,m_pGameSquareOption->RankBonusByResult[k].wLast-iterRank2->second.wResult);
				strWantResultBonus = szWantResultBonusBuf;
			}
		}

		TCHAR szBuf1[512]=TEXT("");
		_sntprintf(szBuf1,sizeof(szBuf1)-1,TEXT("���ڹ㳡������������%ld������:%lld����"), iterRank1->second, iterRank2->second.lScore);
		TCHAR szBuf2[512]=TEXT("");
		_sntprintf(szBuf2,sizeof(szBuf2)-1,TEXT("����%ld%%����� %s"),\
			(int)(((PlayerRankMap.size() - (iterRank1->second - 1)) * 100.0) / PlayerRankMap.size()), strWantResultBonus.IsEmpty()?TEXT(""):strWantResultBonus.GetBuffer());

		//_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("���ڹ㳡������������%ld������:%lld��������%ld%%����� %s"), iterRank1->second, iterRank2->second.lScore,
		//	(int)(((PlayerRankMap.size() - (iterRank1->second - 1)) * 100.0) / PlayerRankMap.size()), strWantResultBonus.IsEmpty()?TEXT(""):strWantResultBonus.GetBuffer());
		strWantResultBonus.ReleaseBuffer();

		_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("%s%s"), szBuf1, szBuf2);
	}
	else
	{
		_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("���ڹ㳡������������%ld������:%lld��"), iterRank1->second, iterRank2->second.lScore);
	}
	strString += szBuf;

	m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,strString.GetBuffer(),SMT_CHAT);
	strString.ReleaseBuffer();

	return 0;
}

CString CSquare::GetPlayerRankString(map<DWORD, int> UserRankMap, DWORD dwRankUserID)
{
	TCHAR szBuf[512];
	map<DWORD, int>::iterator iterRank1 = UserRankMap.find(dwRankUserID);
	map<DWORD, tagSQUARERANK>::iterator iterRank2 = m_SquareScoreMap.find(dwRankUserID);
	if (iterRank1 == UserRankMap.end())
	{
		LOGE("<"<<m_pGameServiceOption->szServerName<<"> CSquare::SquareAnnounce()... has not rank1");
		return TEXT("");
	}
	if (iterRank2 == m_SquareScoreMap.end())
	{
		LOGE("<"<<m_pGameServiceOption->szServerName<<"> CSquare::SquareAnnounce()... has not rank2");
		return TEXT("");
	}

	if (UserRankMap.size() != 0)
	{
		TCHAR szBuf1[512];
		TCHAR szBuf2[512];
		_sntprintf(szBuf1,sizeof(szBuf1)-1,TEXT("��%ld������:%ld "),\
			iterRank1->second, iterRank2->second.lScore);
		_sntprintf(szBuf2,sizeof(szBuf2)-1,TEXT("��Ч��:%ld��������%ld%%�����"),\
			iterRank2->second.wResult, (int)(((UserRankMap.size() - (iterRank1->second - 1)) * 100.0) / UserRankMap.size()));
		//_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("��%ld������:%ld ��Ч��:%ld��������%ld%%�����"),\
		//	iterRank1->second, iterRank2->second.lScore, iterRank2->second.wResult,
		//	(int)(((UserRankMap.size() - (iterRank1->second - 1)) * 100.0) / UserRankMap.size()));
		_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("%s%s"),szBuf1,szBuf2);
	}
	else
	{
		_sntprintf(szBuf,sizeof(szBuf),TEXT("��%ld������:%ld ��Ч��:%ld��"), iterRank1->second, iterRank2->second.lScore, iterRank2->second.wResult);
	}

	return (CString)szBuf;
};

void CSquare::SendTopPlayers(DWORD dwSpecUserID, WORD wSplit/*=20*/)
{
	vector<tagSQUARERANK> vSquareRank;

	for (map<DWORD, tagSQUARERANK>::iterator iter=m_SquareScoreMap.begin(); iter!=m_SquareScoreMap.end(); iter++)
	{
		vSquareRank.push_back(iter->second); 
	}

	sort(vSquareRank.begin(), vSquareRank.end(), sortbyscore); 

	map<DWORD, int> PlayerRankMap;
	for (size_t i = 0; i < vSquareRank.size(); i++)
	{
		PlayerRankMap.insert(make_pair(vSquareRank[i].dwUserID, (int)(i + 1)));
	}

	bool bFirst=true;
	WORD wRemainRank=min((int)vSquareRank.size(), m_pGameSquareOption->wRankNumber);
	WORD wStartRank=0;
	while (1)
	{
		CString strTopPlayers;
		if (bFirst)
		{
			strTopPlayers = TEXT("����������\n");
			//һ���ܹ�43������ַ���
			TCHAR szBuf[255]=TEXT("");
			//_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("%-6s%-14s%-9s%-2s\n"),TEXT("����"),TEXT("�ǳ�"),TEXT("��Ӯ��"),TEXT("����"));			
			_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("%-8s%-16s%-12s%-4s\n"),TEXT("����"),TEXT("�ǳ�"),TEXT("��Ӯ��"),TEXT("����"));
// 			CString strRank;
// 			CString strNickName;
// 			CString strScore;
// 			CString strResult;
// 			TransformString(TEXT("����"),strRank,8,8);
// 			TransformString(TEXT("�ǳ�"),strNickName,16,16);
// 			TransformString(TEXT("��Ӯ��"),strScore,12,12);
// 			TransformString(TEXT("����"),strResult,4,4);
// 			_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("%-s%-s%-s%-s\n"),strRank,strNickName,strScore,strResult);
			strTopPlayers += szBuf;
		}

		WORD wItems = min(wRemainRank, wSplit);
		//WORD wItems = wRemainRank;
		for (int i = 0; i < wItems; i++)
		{
// 			CString strNickName;
// 			TransformString(TEXT("��׼���ʺٺٹ������"), strNickName, 16, 19);
// // 			TransformString(TEXT("����ȴ���������"), strNickName, 16, 19);
// 			CString strNickName1;
// 			TransformString(TEXT("��׼��abcde�ٹ�a���"), strNickName1, 16, 19);

// 			TCHAR szBuf[255];
// 			TCHAR szBuf1[127]=TEXT("");
// 			//_sntprintf(szBuf1,sizeof(szBuf1)-1,TEXT("<%ld>   %s      %ld	"), wStartRank + i + 1, vSquareRank[wStartRank + i].szNickName,\
// 			//vSquareRank[wStartRank + i].lScore);
// 			//8---19---12---4 = 43
// 			_sntprintf(szBuf1,sizeof(szBuf1)-1,TEXT("%-8ld%-s%-12ld"), 45, strNickName, -175882010);
// 			TCHAR szBuf2[127]=TEXT("");
// 			//_sntprintf(szBuf2,sizeof(szBuf2)-1,TEXT("      %ld"),vSquareRank[wStartRank + i].wResult);
// 			_sntprintf(szBuf2,sizeof(szBuf2)-1,TEXT("%-4ld"),274);
// // 			_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("%s%s"), szBuf1, szBuf2);


			TCHAR szBuf[255];
			TCHAR szBuf1[127]=TEXT("");
			CString strNickName;
			TransformString(vSquareRank[wStartRank + i].szNickName, strNickName, 15, 16);
			_sntprintf(szBuf1,sizeof(szBuf1)-1,TEXT("%-8ld%-s%-12ld"), wStartRank + i + 1, strNickName,\
			vSquareRank[wStartRank + i].lScore);
			//8---16---12---4 = 40
			TCHAR szBuf2[127]=TEXT("");
			_sntprintf(szBuf2,sizeof(szBuf2)-1,TEXT("%-4ld"),vSquareRank[wStartRank + i].wResult);
			_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("%s%s"), szBuf1, szBuf2);

			strTopPlayers += szBuf;

			if (i != wItems - 1)
				strTopPlayers += TEXT("\n");
		}

		if(!bFirst)
		{
			strTopPlayers = TEXT("\n") + strTopPlayers;
		}

		wRemainRank -= wItems;

		if(wRemainRank<=0)
		{
			CString strBroadCast =  m_pGameSquareOption->szSquareBroadcast;
			if(strBroadCast.GetLength() > 0)
			{
				strTopPlayers += TEXT("\n");
				strTopPlayers += strBroadCast;
			}
		}

		//ö���û�
		WORD wEnumIndex=0;
		while(wEnumIndex<m_pIServerUserManager->GetUserItemCount())
		{
			//�����û�
			IServerUserItem *pIServerUserItem=m_pIServerUserManager->EnumUserItem(wEnumIndex++);
			if(pIServerUserItem==NULL) continue;

			//ָ���˴�������
			if (dwSpecUserID != 0 && dwSpecUserID != pIServerUserItem->GetUserID())
				continue;

			if (bFirst)
			{
				LOGI("SendTopPlayers first" << strTopPlayers.GetBuffer());
				m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,strTopPlayers.GetBuffer(),SMT_CHAT);
			}
			else
			{
				LOGI("SendTopPlayers " << strTopPlayers.GetBuffer())
				m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,strTopPlayers.GetBuffer(),SMT_CHAT);
			}
			strTopPlayers.ReleaseBuffer();
		}

		bFirst = false;
		wStartRank += wSplit;
		if (wRemainRank <= 0)
			break;
	}

}

void CSquare::SendSquareEnter(DWORD dwRankUserID)
{
	if (dwRankUserID == 0)return;

	vector<tagSQUARERANK> vSquareRank;

	for (map<DWORD, tagSQUARERANK>::iterator iter=m_SquareScoreMap.begin(); iter!=m_SquareScoreMap.end(); iter++)
	{
		vSquareRank.push_back(iter->second); 
	}

	sort(vSquareRank.begin(), vSquareRank.end(), sortbyscore); 

	map<DWORD, int> UserRankMap;
	for (size_t i=0; i<vSquareRank.size(); i++)
	{
		UserRankMap.insert(make_pair(vSquareRank[i].dwUserID, (int)(i+1)));
	}

	CString strString;
	TCHAR szEnter[256];
	DWORD szLeftMin=m_dwSquareRemain/60;
	if(szLeftMin>0)
		_sntprintf(szEnter,sizeof(szEnter)-1,TEXT("��ӭ���μ�%s���������%ld���ӣ�ף������ Ŀǰ��"),m_pGameSquareOption->szSquareName,szLeftMin);
	else
		_sntprintf(szEnter,sizeof(szEnter)-1,TEXT("��ӭ���μ�%s�������������1���ӣ�ף������ Ŀǰ��"),m_pGameSquareOption->szSquareName);

	strString = (CString)szEnter + GetPlayerRankString(UserRankMap, dwRankUserID);
	LOGI("SendSquareEnter " << strString.GetBuffer())
	strString.ReleaseBuffer();

	//ö���û�
	WORD wEnumIndex=0;
	while(wEnumIndex<m_pIServerUserManager->GetUserItemCount())
	{
		//�����û�
		IServerUserItem *pIServerUserItem=m_pIServerUserManager->EnumUserItem(wEnumIndex++);
		if(pIServerUserItem==NULL) continue;

		if(dwRankUserID==pIServerUserItem->GetUserID())
		{
			m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,strString.GetBuffer(),SMT_CHAT);
			if(lstrlen(m_pGameSquareOption->szSquareDesc)!=0)m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,m_pGameSquareOption->szSquareDesc,SMT_CHAT);
			strString.ReleaseBuffer();
			break;
		}
	}
};

//void CSquare::SendSquareEnd(WORD wSplit/*=2*/)
//{
//	vector<tagSQUARERANK> vSquareRank;
//
//	for (map<DWORD, tagSQUARERANK>::iterator iter=m_SquareScoreMap.begin(); iter!=m_SquareScoreMap.end(); iter++)
//	{
//		vSquareRank.push_back(iter->second); 
//	}
//
//	sort(vSquareRank.begin(), vSquareRank.end(), sortbyscore); 
//
//	map<DWORD, int> UserRankMap;
//	size_t i=0;
//	for (i=0; i<vSquareRank.size(); i++)
//	{
//		tagSQUARERANK rk = vSquareRank[i];
//		UserRankMap.insert(make_pair(vSquareRank[i].dwUserID, (int)(i+1)));
//	}
//
//	// չ����������
//	tagRANKBONUS RankBonus[300];
//	memset(&RankBonus, 0, sizeof(RankBonus));
//
//	WORD wCursor = 0;
//	for (i=0; i<m_pGameSquareOption->cbRankBonus; i++)
//	{
//		if(m_pGameSquareOption->RankBonus[i].wFirst==0||m_pGameSquareOption->RankBonus[i].wLast==0)
//			continue;
//
//		for(WORD j=m_pGameSquareOption->RankBonus[i].wFirst; j<=m_pGameSquareOption->RankBonus[i].wLast; j++)
//		{
//			RankBonus[wCursor].wFirst = RankBonus[wCursor].wLast = j;
//			int a=CountArray(RankBonus[wCursor].wPropertyID);
//			int b=RankBonus[wCursor].cbPropertyCount;
//			//CopyMemory(RankBonus[wCursor].wPropertyID,m_pGameSquareOption->RankBonus[i].wPropertyID,\
//			//	CountArray(RankBonus[wCursor].wPropertyID)*RankBonus[wCursor].cbPropertyCount);
//			CopyMemory(RankBonus[wCursor].wPropertyID,m_pGameSquareOption->RankBonus[i].wPropertyID,\
//				sizeof(RankBonus[wCursor].wPropertyID[0])*m_pGameSquareOption->RankBonus[i].cbPropertyCount);
//			RankBonus[wCursor].cbPropertyCount = m_pGameSquareOption->RankBonus[i].cbPropertyCount;
//			lstrcpyn(RankBonus[wCursor].szDesc,m_pGameSquareOption->RankBonus[i].szDesc,sizeof(RankBonus[wCursor].szDesc));
//			wCursor++;
//		}
//	}
//
//	vector<CString>  vStrTopUser;
//	WORD wCount=0; wstring wstrTopUser;
//
//	map<DWORD, CString> StrSelfRankBonusMap;
//	map<DWORD, CString> StrSelfResultBonusMap;
//
//	for (size_t i=0; i<vSquareRank.size(); i++)
//	{
//		tagSQUARERANK SquareRank = vSquareRank[i];
//
//		CString strResultBonus;
//		for (BYTE k=0; k<m_pGameSquareOption->cbRankBonusByResult; k++)
//		{
//			if (m_pGameSquareOption->RankBonusByResult[k].wFirst==0||m_pGameSquareOption->RankBonusByResult[k].wLast==0)
//				continue;
//
//			if (SquareRank.wResult >= m_pGameSquareOption->RankBonusByResult[k].wFirst)
//			{
//				//if (!m_match->m_callback->OnSquareBonus(m_match, ranks[i].id, m_squareconfig.bonusbyresult[k].bonus))
//				DBR_GR_SuqareReward SquareReward;
//				SquareReward.dwUserID=SquareRank.dwUserID;
//				SquareReward.wRank=WORD(i+1);
//				SquareReward.wMatchID=m_pGameSquareOption->wMatchID;
//				SquareReward.wGameID=m_pGameSquareOption->wGameID;
//				SquareReward.wServerID=m_pGameSquareOption->wServerID;
//				if(!m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_SQUARE_REWARD,0,&SquareReward,sizeof(SquareReward)))
//				{
//					LOGE("SendSquareEnd::PostDataBaseRequest(DBR_GR_SQUARE_REWARD) ���ֽ�����¼�쳣! dwUserID=" << SquareRank.dwUserID\
//						<< " rank=" << (int)(i+1));
//				}
//
//			}
//		} 
//
//		StrSelfResultBonusMap[SquareRank.dwUserID] = strResultBonus;
//
//		if((WORD)i>=wCursor)continue;
//
//		CString strRankBonus;
//		strRankBonus=RankBonus[i].szDesc;
//
//		DBR_GR_SuqareReward SquareReward;
//		SquareReward.dwUserID=SquareRank.dwUserID;
//		SquareReward.wRank=WORD(i+1);
//		SquareReward.wMatchID=m_pGameSquareOption->wMatchID;
//		SquareReward.wGameID=m_pGameSquareOption->wGameID;
//		SquareReward.wServerID=m_pGameSquareOption->wServerID;
//		//SquareReward.dwMatchID=m_pGameServiceOption->;
//		if(!m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_SQUARE_REWARD,0,&SquareReward,sizeof(SquareReward)))
//		{
//			LOGE("SendSquareEnd::PostDataBaseRequest(DBR_GR_SQUARE_REWARD) ���ν�����¼�쳣! dwUserID=" << SquareRank.dwUserID\
//				<< " rank=" << (int)(i+1));
//		}
//
//		// ǰn���Ĵ���
//		if ((i+1) <= (size_t)m_pGameSquareOption->wRankNumber)
//		{
//			if (wCount==wSplit)
//			{
//				if (!wstrTopUser.empty())wstrTopUser=wstrTopUser.substr(0, wstrTopUser.length()-1);
//				vStrTopUser.push_back(CString(wstrTopUser.c_str()));
//				wstrTopUser.clear();
//				wCount = 0;
//			}
//
//			TCHAR szBufTopUser[255];
//			_sntprintf(szBufTopUser,sizeof(szBufTopUser)-1,TEXT("<%ld>%s��%ld(%ld) %s\n"),\
//				i+1,SquareRank.szNickName, SquareRank.lScore, SquareRank.wResult,strRankBonus.GetBuffer());
//			strRankBonus.ReleaseBuffer();
//			wstrTopUser+=szBufTopUser;
//			wCount++;
//		}
//
//		// ��������
//		StrSelfRankBonusMap[SquareRank.dwUserID]=strRankBonus.GetBuffer();
//		strRankBonus.ReleaseBuffer();
//	}
//
//	if (!wstrTopUser.empty())
//	{
//		wstrTopUser=wstrTopUser.substr(0,wstrTopUser.length()-1);
//		vStrTopUser.push_back(CString(wstrTopUser.c_str()));
//		wstrTopUser.clear();
//		wCount=0;			
//	}
//
//	//ö���û�
//	WORD wEnumIndex=0;
//	while(wEnumIndex<m_pIServerUserManager->GetUserItemCount())
//	{
//		//�����û�
//		IServerUserItem *pIServerUserItem=m_pIServerUserManager->EnumUserItem(wEnumIndex++);
//		if(pIServerUserItem==NULL) continue;
//
//		DWORD dwUserID=pIServerUserItem->GetUserID();
//
//		//û�з�����ֱ��continue
//		if(UserRankMap.find(dwUserID)==UserRankMap.end())continue;
//
//		for (size_t k=0; k<vStrTopUser.size(); k++)
//		{
//			if(k==0)
//			{
//				m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("�����������μ��������£�\n")+vStrTopUser[k],SMT_CHAT);
//			}
//			else	
//			{
//				m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,vStrTopUser[k].GetBuffer(),SMT_CHAT);
//				vStrTopUser[k].ReleaseBuffer();
//			}
//		}
//
//		CString strStr = TEXT("����������Ϊ") + GetPlayerRankString(UserRankMap, dwUserID);
//		bool bHasBonus=false;
//
//		map<DWORD, CString>::iterator iter1=StrSelfRankBonusMap.find(dwUserID);
//		if (iter1!=StrSelfRankBonusMap.end())
//		{
//			if (!iter1->second.IsEmpty())
//			{
//				strStr=strStr+TEXT(" ��ã�")+iter1->second;
//				bHasBonus=true;
//			}
//		}
//
//		map<DWORD, CString>::iterator iter2=StrSelfResultBonusMap.find(dwUserID);
//		if (iter2!=StrSelfResultBonusMap.end())
//		{
//			if (!iter2->second.IsEmpty())
//			{
//				strStr=strStr+TEXT(" �����Ч�ֽ���")+iter2->second;
//				bHasBonus=true;
//			}
//		}
//
//		if (!bHasBonus)strStr=strStr+TEXT(" ���ź�����δ��ý�����ף���´κ��ˡ�");
//
//		CString strBroadcast = m_pGameSquareOption->szSquareBroadcast;
//		if(strBroadcast.GetLength()>0)
//		{
//			strStr=strStr+strBroadcast;
//		}
//
//		if (!strStr.IsEmpty())
//		{
//			m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,strStr.GetBuffer(),SMT_CHAT);
//			strStr.ReleaseBuffer();
//		}
//	}
//};

void CSquare::SquareEnd()
{
	LOGI("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName << "> CSquare::SquareEnd, �㳡��������!");

	vector<tagSQUARERANK> vSquareRank;
	for (map<DWORD, tagSQUARERANK>::iterator iter=m_SquareScoreMap.begin(); iter!=m_SquareScoreMap.end(); iter++)
	{
		vSquareRank.push_back(iter->second); 
	}

	sort(vSquareRank.begin(), vSquareRank.end(), sortbyscore); 

	WORD wCursor = 0;
	WORD wCursorResult = 0;
	for (BYTE i=0; i<m_pGameSquareOption->cbRankBonus; i++)
	{
		if(m_pGameSquareOption->RankBonus[i].wFirst==0||m_pGameSquareOption->RankBonus[i].wLast==0)
			continue;

		for(WORD j=m_pGameSquareOption->RankBonus[i].wFirst; j<=m_pGameSquareOption->RankBonus[i].wLast; j++)
			wCursor++;
	}

	for (BYTE i=0; i<m_pGameSquareOption->cbRankBonusByResult; i++)
	{
		if(m_pGameSquareOption->RankBonusByResult[i].wFirst==0||m_pGameSquareOption->RankBonusByResult[i].wLast==0)
			continue;

		for(WORD j=m_pGameSquareOption->RankBonusByResult[i].wFirst; j<=m_pGameSquareOption->RankBonusByResult[i].wLast; j++)
			wCursorResult++;
	}

	LOGI("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName << "> CSquare::SquareEnd, wCursor= " << wCursor\
		<< " wCursorResult= " << wCursorResult);
	for (size_t i=0; i<vSquareRank.size(); i++)
	{
		tagSQUARERANK SquareRank = vSquareRank[i];

		LOGI("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName << "> CSquare::SquareEnd, �㳡������: dwUserID= "\
			<< SquareRank.dwUserID << " szNickName= " << SquareRank.szNickName << " lScore= " << SquareRank.lScore << " wResult= " << SquareRank.wResult);
		for (BYTE k=0; k<m_pGameSquareOption->cbRankBonusByResult; k++)
		{
			if (m_pGameSquareOption->RankBonusByResult[k].wFirst==0||m_pGameSquareOption->RankBonusByResult[k].wLast==0)
				continue;

			if (SquareRank.wResult >= m_pGameSquareOption->RankBonusByResult[k].wFirst)
			{
				//if (!m_match->m_callback->OnSquareBonus(m_match, ranks[i].id, m_squareconfig.bonusbyresult[k].bonus))
				DBR_GR_SuqareReward SquareReward;
				SquareReward.dwUserID=SquareRank.dwUserID;
				SquareReward.wRank=WORD(i+1);
				SquareReward.wMatchID=m_pGameSquareOption->wMatchID;
				SquareReward.wGameID=m_pGameSquareOption->wGameID;
				SquareReward.wServerID=m_pGameSquareOption->wServerID;
				if(!m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_SQUARE_REWARD,0,&SquareReward,sizeof(SquareReward)))
				{
					LOGE("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName\
						<< "> SendSquareEnd::PostDataBaseRequest(DBR_GR_SQUARE_REWARD) ���ֽ�����¼�쳣! dwUserID=" << SquareRank.dwUserID << " rank=" << (int)(i+1));
				}

			}
		} 

		if((WORD)i>=wCursor)continue;

		DBR_GR_SuqareReward SquareReward;
		SquareReward.dwUserID=SquareRank.dwUserID;
		SquareReward.wRank=WORD(i+1);
		SquareReward.wMatchID=m_pGameSquareOption->wMatchID;
		SquareReward.wGameID=m_pGameSquareOption->wGameID;
		SquareReward.wServerID=m_pGameSquareOption->wServerID;
		int nEndIndex=min(wCursor,vSquareRank.size());
		DWORD dwContextID=(i==nEndIndex-1?0xFFFF:0L);
		if(!m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_SQUARE_REWARD,dwContextID,&SquareReward,sizeof(SquareReward)))
		{
			LOGE("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName\
				<< "> SendSquareEnd::PostDataBaseRequest(DBR_GR_SQUARE_REWARD) ���ν�����¼�쳣! dwUserID=" << SquareRank.dwUserID << " rank=" << (int)(i+1));
		}
	}

	//�����޽��� ���� �����޽��� ���� ���˲μ�
	if((wCursor==0 && wCursorResult==0) || vSquareRank.size()==0)
	{
		m_bEndSquare=TRUE;
	}
};

void CSquare::SendSquareEnd(WORD wSplit/*=3*/)
{
	LOGI("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName << "> CSquare::SendSquareEnd ƴ�ӷ��������������ַ��� wSplit= " << wSplit);
	vector<tagSQUARERANK> vSquareRank;

	for (map<DWORD, tagSQUARERANK>::iterator iter=m_SquareScoreMap.begin(); iter!=m_SquareScoreMap.end(); iter++)
	{
		vSquareRank.push_back(iter->second); 
	}

	sort(vSquareRank.begin(), vSquareRank.end(), sortbyscore); 

	map<DWORD, int> UserRankMap;
	size_t i=0;
	for (i=0; i<vSquareRank.size(); i++)
	{
		tagSQUARERANK rk = vSquareRank[i];
		UserRankMap.insert(make_pair(vSquareRank[i].dwUserID, (int)(i+1)));
	}

	WORD wCursor = 0;
	for (i=0; i<m_pGameSquareOption->cbRankBonus; i++)
	{
		if(m_pGameSquareOption->RankBonus[i].wFirst==0||m_pGameSquareOption->RankBonus[i].wLast==0)
			continue;

		for(WORD j=m_pGameSquareOption->RankBonus[i].wFirst; j<=m_pGameSquareOption->RankBonus[i].wLast; j++)
			wCursor++;
	}

	//vector<CString>  vStrTopUser;
	//WORD wCount=0; wstring wstrTopUser;
	WORD wCount1=0, wCount2=0;
#ifdef _UNICODE
	vector<wstring>  vStrTopUser;
	wstring strTopUser;
	vector<wstring>  vStrBonusUser;
	wstring strBonusUser;
#else
	vector<string>  vStrTopUser;
	string strTopUser;
	vector<string>  vStrBonusUser;
	string strBonusUser;
#endif

	LOGI("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName << "> CSquare::SendSquareEnd wCursor= " << wCursor);
	for (size_t i=0; i<vSquareRank.size(); i++)
	{
		tagSQUARERANK SquareRank = vSquareRank[i];

		if((WORD)i>=wCursor)continue;

		CString strRankBonus;
		map<DWORD,CString>::iterator iterRankBonus=m_StrSelfRankBonusMap.find(SquareRank.dwUserID);
		if(iterRankBonus!=m_StrSelfRankBonusMap.end())strRankBonus=iterRankBonus->second;
		LOGI("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName << "> CSquare::SendSquareEnd dwUserID= " << SquareRank.dwUserID\
			<< " szNickName= " << SquareRank.szNickName << " lScore= " << SquareRank.lScore << " wResult= " << SquareRank.wResult << " strRankBonus= " << strRankBonus.GetBuffer());
		strRankBonus.ReleaseBuffer();

		// ǰn���Ĵ���
		if ((i+1) <= (size_t)m_pGameSquareOption->wRankNumber)
		{
			//��������
			if (wCount1==wSplit)
			{
				if(vStrBonusUser.size() > 0)
				{
					strBonusUser = TEXT("\n") + strBonusUser;
				}

				if (!strBonusUser.empty())strBonusUser=strBonusUser.substr(0, strBonusUser.length()-1);
				vStrBonusUser.push_back(strBonusUser);
				strBonusUser.clear();
				wCount1 = 0;
			}

			TCHAR szBuf[255];
			CString strBonus;
			TransformString(strRankBonus.GetBuffer(), strBonus, 32, 32);
			_sntprintf(szBuf,sizeof(szBuf)-1,TEXT("%-8ld%-s\n"), i+1, strBonus);
			strBonusUser+=szBuf;

			//��������
 			if (wCount2==wSplit)
 			{
				if(vStrTopUser.size() > 0)
				{
					strTopUser = TEXT("\n") + strTopUser;
				}

 				if (!strTopUser.empty())strTopUser=strTopUser.substr(0, strTopUser.length()-1);
 				vStrTopUser.push_back(strTopUser);
 				strTopUser.clear();
 				wCount2 = 0;
 			}

			TCHAR szBufTopUser[255];
			TCHAR szBufTopUser1[127];
			TCHAR szBufTopUser2[127];
			CString strNickName;
			TransformString(SquareRank.szNickName, strNickName, 15, 16);
			_sntprintf(szBufTopUser1,sizeof(szBufTopUser1)-1,TEXT("%-8ld%-s%-12ld"), i+1,strNickName, SquareRank.lScore);
			//8---16---12---4 = 40
			_sntprintf(szBufTopUser2,sizeof(szBufTopUser2)-1,TEXT("%-4ld"),SquareRank.wResult);
			_sntprintf(szBufTopUser,sizeof(szBufTopUser)-1,TEXT("%s%s\n"), szBufTopUser1, szBufTopUser2);
			strTopUser+=szBufTopUser;

			wCount1++;
			wCount2++;
		}
	}

	if (!strTopUser.empty())
	{
		strTopUser = TEXT("\n") + strTopUser;
		strTopUser=strTopUser.substr(0,strTopUser.length()-1);
		vStrTopUser.push_back(strTopUser);
		strTopUser.clear();
		wCount1=0;			
	}

	if (!strBonusUser.empty())
	{
		strBonusUser = TEXT("\n") + strBonusUser;
		strBonusUser=strBonusUser.substr(0,strBonusUser.length()-1);
		vStrBonusUser.push_back(strBonusUser);
		strBonusUser.clear();
		wCount2=0;			
	}

	//ö���û�
	WORD wEnumIndex=0;
	while(wEnumIndex<m_pIServerUserManager->GetUserItemCount())
	{
		//�����û�
		IServerUserItem *pIServerUserItem=m_pIServerUserManager->EnumUserItem(wEnumIndex++);
		if(pIServerUserItem==NULL) continue;

		DWORD dwUserID=pIServerUserItem->GetUserID();

		//û�з�����ֱ��continue
		if(UserRankMap.find(dwUserID)==UserRankMap.end())continue;

		//���ͽ���
		for (size_t k=0; k<vStrBonusUser.size(); k++)
		{
			if(k==0)
			{
				TCHAR szBonusBuf[255]=TEXT("");
				_sntprintf(szBonusBuf,sizeof(szBonusBuf)-1,TEXT("%-6s%-30s\n"),TEXT("����"),TEXT("����"));

				TCHAR szResultRank[1024]=TEXT("");
				_sntprintf(szResultRank,CountArray(szResultRank),TEXT("�����������μ��������£�\n%s%s"),\
					szBonusBuf, vStrBonusUser[k].c_str());
				m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,szResultRank,SMT_CHAT);
				LOGI("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName << "> CSquare::SendSquareEnd dwUserID= " << dwUserID\
					<< " szNickName= " << pIServerUserItem->GetNickName() << ": " << szResultRank);
			}
			else	
			{
				m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,vStrBonusUser[k].c_str(),SMT_CHAT);
				LOGI("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName << "> CSquare::SendSquareEnd dwUserID= " << dwUserID\
					<< " szNickName= " << pIServerUserItem->GetNickName() << ": " << vStrBonusUser[k].c_str());
			}
		}

		//��������
		for (size_t k=0; k<vStrTopUser.size(); k++)
		{
			if(k==0)
			{
				//TCHAR szBonusBuf[255]=TEXT("");
				//_sntprintf(szBonusBuf,sizeof(szBonusBuf)-1,TEXT("%-6s%-30s\n"),TEXT("����"),TEXT("����"));

				TCHAR szRankBuf[255]=TEXT("");
				//_sntprintf(szRankBuf,sizeof(szRankBuf)-1,TEXT("%-6s%-14s%-9s%-2s\n"),TEXT("����"),TEXT("�ǳ�"),TEXT("��Ӯ��"),TEXT("����"));
				_sntprintf(szRankBuf,sizeof(szRankBuf)-1,TEXT("%-8s%-16s%-12s%-4s\n"),TEXT("����"),TEXT("�ǳ�"),TEXT("��Ӯ��"),TEXT("����"));

				TCHAR szResultRank[1024]=TEXT("");
				//_sntprintf(szResultRank,CountArray(szResultRank),TEXT("�����������μ��������£�\n%s%s%s%s"),\
				//	szBonusBuf, vStrBonusUser[k].c_str(), szRankBuf, vStrTopUser[k].c_str());
				_sntprintf(szResultRank,CountArray(szResultRank),TEXT("\n%s%s"), szRankBuf, vStrTopUser[k].c_str());
				m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,szResultRank,SMT_CHAT);
				LOGI("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName << "> CSquare::SendSquareEnd dwUserID= " << dwUserID\
					<< " szNickName= " << pIServerUserItem->GetNickName() << ": " << szResultRank);
			}
			else	
			{
				m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,vStrTopUser[k].c_str(),SMT_CHAT);
				LOGI("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName << "> CSquare::SendSquareEnd dwUserID= " << dwUserID\
					<< " szNickName= " << pIServerUserItem->GetNickName() << ": " << vStrTopUser[k].c_str());
			}
		}

		CString strStr = TEXT("����������Ϊ") + GetPlayerRankString(UserRankMap, dwUserID);
		bool bHasBonus=false;

		map<DWORD, CString>::iterator iter1=m_StrSelfRankBonusMap.find(dwUserID);
		if (iter1!=m_StrSelfRankBonusMap.end())
		{
			if (!iter1->second.IsEmpty())
			{
				strStr=strStr+TEXT(" ��ã�")+iter1->second;
				bHasBonus=true;
			}
		}

		map<DWORD, CString>::iterator iter2=m_StrSelfResultBonusMap.find(dwUserID);
		if (iter2!=m_StrSelfResultBonusMap.end())
		{
			if (!iter2->second.IsEmpty())
			{
				strStr=strStr+TEXT(" �����Ч�ֽ���")+iter2->second;
				bHasBonus=true;
			}
		}

		if (!bHasBonus)strStr=strStr+TEXT(" ���ź�����δ��ý�����ף���´κ��ˡ�");

		CString strBroadcast = m_pGameSquareOption->szSquareBroadcast;
		if(strBroadcast.GetLength()>0)
		{
			strStr=strStr+strBroadcast;
		}

		if (!strStr.IsEmpty())
		{
			m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,strStr.GetBuffer(),SMT_CHAT);
			LOGI("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName << "> CSquare::SendSquareEnd dwUserID= " << dwUserID\
				<< " szNickName= " << pIServerUserItem->GetNickName() << ": " << strStr.GetBuffer());
			strStr.ReleaseBuffer();
		}
	}

	m_bEndSquare=TRUE;
};

void CSquare::AddRank(DWORD dwUserID, tagSQUARERANK SquareRank)
{ 
	m_SquareScoreMap[dwUserID]=SquareRank;
}

void CSquare::OnTimer()
{
	//ʣ��ʱ��
	CTime tmLocalTime=CTime::GetCurrentTime();
	CTime tmStartTime(m_pGameSquareOption->StartTime);
	CTime tmEndTime(m_pGameSquareOption->EndTime);

	if(m_bStartSquare)
	{
		if(tmLocalTime>=tmEndTime)
		{//�㳡������
			LOGI("�㳡������ SquareRemain= " << m_dwSquareRemain);
			m_bStartSquare=FALSE;
			m_dwAnnounceInterval=0;
			m_dwSquareRemain=0;
			SquareEnd();
		}
		else
		{
			m_dwAnnounceInterval--;
			m_dwSquareRemain--;
			if (m_dwAnnounceInterval<=0)
			{
				SendTopPlayers(0);
				m_dwAnnounceInterval=m_pGameSquareOption->wInterval*60;
			}

			//ʣ��1����ʱ�����¹㳡����������
			if(m_dwSquareRemain==60)
			{
				DBR_GR_LoadSuqareReward LoadSquareReward;
				LoadSquareReward.wMatchID=m_pGameSquareOption->wMatchID;
				LoadSquareReward.wGameID=m_pGameSquareOption->wGameID;
				LoadSquareReward.wServerID=m_pGameSquareOption->wServerID;
				if(!m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_SQUARE_REWARD,0,&LoadSquareReward,sizeof(LoadSquareReward)))
				{
					LOGE("<" << m_pGameSquareOption->wMatchID <<  "|" << m_pGameSquareOption->szSquareName\
						<< "> CSquare::OnTimer::PostDataBaseRequest(DBR_GR_LOAD_SQUARE_REWARD) ʣ��1����ʱ�����¹㳡�����������쳣! MatchID="\
						<< LoadSquareReward.wMatchID << " ServerID=" << LoadSquareReward.wServerID);
				}
			}
		}
	}
	else
	{
		if(tmLocalTime>=tmStartTime&&tmLocalTime<tmEndTime)
		{//��ʼ�㳡��
			StartSquare();
			m_dwAnnounceInterval=m_pGameSquareOption->wInterval*60;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////

CSquareManager::CSquareManager(void)
{
}

CSquareManager::~CSquareManager(void)
{
}

//ֹͣ����
bool CSquareManager::ConcludeService()
{
	clear();
	return true;
}

void CSquareManager::OnTimer()
{
	for (CSquareManager::iterator iter=begin(); iter!=end();)
	{
		WORD wMatchID=iter->first;
		CSquare Square=iter->second;
		iter->second.OnTimer();

		if(iter->second.m_bEndSquare)
		{
			LOGI("CSquareManager::OnTimer() <" << iter->second.m_pGameSquareOption->wMatchID << "|" << iter->second.m_pGameSquareOption->szSquareName\
				<< ">�㳡���ѽ��� �����޳���");
			memset(iter->second.m_pGameSquareOption,0,sizeof(tagGameSquareOption));
			erase(iter++);
		}
		else
		{
			iter++;
		}
	}
};
