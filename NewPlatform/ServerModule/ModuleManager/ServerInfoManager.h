#ifndef SERVER_INFO_MANAGER_HEAD_FILE
#define SERVER_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CWHArray<tagGameServerInfo *> CGameServerInfoArray;

//��������
typedef CMap<WORD,WORD,tagGameServerInfo *,tagGameServerInfo *> CGameServerInfoMap;

//////////////////////////////////////////////////////////////////////////////////

//ģ������
class MODULE_MANAGER_CLASS CServerInfoBuffer
{
	//��������
public:
	CGameServerInfoMap				m_GameServerInfoMap;				//ģ������
	CGameServerInfoArray			m_GameServerInfoArray;				//ģ������

	//��������
public:
	//���캯��
	CServerInfoBuffer();
	//��������
	virtual ~CServerInfoBuffer();

	//������
public:
	//��������
	bool ResetServerInfo();
	//ɾ������
	bool DeleteServerInfo(WORD wServerID);
	//��������
	bool InsertServerInfo(tagGameServerInfo * pGameServerInfo);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	DWORD GetServerInfoCount();
	//��������
	tagGameServerInfo * SearchServerInfo(WORD wServerID);

	//�ڲ�����
private:
	//��������
	tagGameServerInfo * CreateServerInfo();
};

//////////////////////////////////////////////////////////////////////////////////

//ģ����Ϣ
class MODULE_MANAGER_CLASS CServerInfoManager
{
	//��������
public:
	//���캯��
	CServerInfoManager();
	//��������
	virtual ~CServerInfoManager();

	//���ط���
public:
	//���ط���
	bool LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], CServerInfoBuffer & ServerInfoBuffer);
	//���ط���
	bool LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], WORD wServerID, tagGameServerInfo & GameServerResult);

	//ע�����
public:
	//ɾ������
	bool DeleteGameServer(WORD wServerID);
	//���뷿��
	bool InsertGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult);
	//�޸ķ���
	bool ModifyGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult);
	//����㳡��
	bool InsertSquareMatch(tagGameSquareOption * pSquareOptionCreate, tagGameSquareOption & SquareOptionResult);
	//���ع㳡��
	bool LoadSquareMatch(WORD wServerID, tagGameSquareOption SquareOptionResultArray[], BYTE & cbGameSquareOpitonArrayCount);
	//���ع㳡������
	bool LoadSquareMatchOption(/*WORD wServerID, */WORD wMatchID, tagGameSquareOption & SquareOptionResult);
	//�������������б�
//	bool InsertRoomTable(WORD wServerID, tagGameTableInfo* pGameTableInfo);
	bool InsertRoomTable(WORD wServerID, vector<tagDBGameTableInfo>* pGameTableInfo);
	//�������������б�
	bool LoadRoomTable();

	//�ڲ�����
private:
	//��������
	bool ConnectPlatformDB(CDataBaseHelper & PlatformDBModule);
	bool ConnectUserCustomDB(CDataBaseHelper & UserCustomDBModule);
	//��ȡ����
	bool ReadGameServerInfo(CDataBaseAide & PlatformDBAide, tagGameServerInfo & GameServerResult);
	//��ȡ��������
	bool ReadGameTableInfo(CDataBaseAide & PlatformDBAide, tagDBGameTableInfo & TableInfo);
	//��ȡ�㳡��
	bool ReadSquareMatchInfo(CDataBaseAide & PlatformDBAide, tagGameSquareOption & GameSquareResult);
	// ��ȡ�Խ���������
	bool ReadUserCustomTableInfo(CDataBaseAide & UserCustomDBAide, tagDBUserCustomTableInfo & TableInfo);
};

//////////////////////////////////////////////////////////////////////////////////

#endif