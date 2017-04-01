#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

//#include "Random.h"

//////////////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_LOAD_GAME_LIST			1									//�����б�
#define IDI_CONNECT_CORRESPOND		2									//������ʶ
#define IDI_COLLECT_ONLINE_INFO		3									//ͳ������

//////////////////////////////////////////////////////////////////////////////////

typedef unsigned int    __u16;
     typedef unsigned long   __u32;

__u16 checksum(__u16 *dp, __u16 length)
     {
	__u16 sum = 0;
	__u16	len	=	length	>>	1;

while(len-- > 0)
             sum += *dp++;
         if(length & 1)
             sum += (*dp & 0xFF00);

sum = (sum >> 16) + (sum & 0xFFFF);
         sum += (sum >> 16);
            
         len = sum & 0xFFFF;
         return(~len);
	}

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_bNeekCorrespond=true;
	m_bShowServerStatus=false;

	//״̬����
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketService=NULL;

	//��Ƶ����
	m_wAVServerPort=0;
	m_dwAVServerAddr=0;

	BYTE buf11[4];
	buf11[0] = 0x70;
	buf11[1] = 0x70;
	buf11[2] = 0x2f;
	buf11[3] = 0x70;

	int xx = checksum((__u16*)buf11, 4);

	int xxx = xx;
	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
}

//�ӿڲ�ѯ
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//�󶨲���
	m_pBindParameter=new tagBindParameter[m_pInitParameter->m_wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pInitParameter->m_wMaxConnect);

	//����ʱ��
	ASSERT(m_pITimerEngine!=NULL);
	m_pITimerEngine->SetTimer(IDI_COLLECT_ONLINE_INFO,m_pInitParameter->m_wCollectTime*1000L,TIMES_INFINITY,0);

	//��ȡĿ¼
	TCHAR szPath[MAX_PATH]=TEXT("");
	CString szFileName;
	GetModuleFileName(AfxGetInstanceHandle(),szPath,sizeof(szPath));
	szFileName=szPath;
	int nIndex = szFileName.ReverseFind(TEXT('\\'));
	szFileName = szFileName.Left(nIndex);
	szFileName += TEXT("\\PlazaOptionConfig.ini");

	//��ȡ����
	m_bShowServerStatus = (GetPrivateProfileInt(TEXT("ServerStatus"),TEXT("ShowServerStatus"),0,szFileName) != 0);

	//��ȡĿ¼
	TCHAR szServerAddr[LEN_SERVER]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//��ȡ����
	TCHAR szVideoFileName[MAX_PATH]=TEXT("");
	_sntprintf(szVideoFileName,CountArray(szVideoFileName),TEXT("%s\\VideoOption.ini"),szPath);
	m_wAVServerPort=GetPrivateProfileInt(TEXT("VideoOption"),TEXT("ServerPort"), 0,szVideoFileName);
	DWORD dwAddrLen=GetPrivateProfileString(TEXT("VideoOption"),TEXT("ServerAddr"), TEXT(""), szServerAddr,LEN_SERVER,szVideoFileName);
	if(dwAddrLen>0)
	{
		CT2CA strServerDomain(szServerAddr);
		m_dwAVServerAddr=inet_addr(strServerDomain);
	}
	else
	{
		m_dwAVServerAddr=0;
	}

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//״̬����
	m_bNeekCorrespond=true;

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketService=NULL;

	//ɾ������
	SafeDeleteArray(m_pBindParameter);

	//�б����
	m_ServerListManager.ResetServerList();

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_LOAD_DB_GAME_LIST:		//�����б�
		{
			//�����б�
			m_ServerListManager.DisuseKernelItem();
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_GAME_LIST,0,NULL,0);

			return true;
		}
	case CT_CONNECT_CORRESPOND:		//����Э��
		{
			//��������
			tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
			m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//������ʾ
			TCHAR szString[512]=TEXT("");
			_sntprintf(szString,CountArray(szString),TEXT("��������Э�������� [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString,TraceLevel_Normal);

			return true;
		}
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//ʱ���¼�
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
	case IDI_LOAD_GAME_LIST:		//�����б�
		{
			//�����б�
			m_ServerListManager.DisuseKernelItem();
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_GAME_LIST,0,NULL,0);

			return true;
		}
	case IDI_CONNECT_CORRESPOND:	//����Э��
		{
			//��������
			tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
			m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//������ʾ
			TCHAR szString[512]=TEXT("");
			_sntprintf(szString,CountArray(szString),TEXT("��������Э�������� [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString,TraceLevel_Normal);

			return true;
		}
	case IDI_COLLECT_ONLINE_INFO:	//ͳ������
		{
			//��������
			DBR_GP_OnLineCountInfo OnLineCountInfo;
			ZeroMemory(&OnLineCountInfo,sizeof(OnLineCountInfo));

			//��ȡ����
			OnLineCountInfo.dwOnLineCountSum=m_ServerListManager.CollectOnlineInfo();

			//��ȡ����
			POSITION KindPosition=NULL;
			do
			{
				//��ȡ����
				CGameKindItem * pGameKindItem=m_ServerListManager.EmunGameKindItem(KindPosition);

				//���ñ���
				if (pGameKindItem!=NULL)
				{
					WORD wKindIndex=OnLineCountInfo.wKindCount++;
					OnLineCountInfo.OnLineCountKind[wKindIndex].wKindID=pGameKindItem->m_GameKind.wKindID;
					OnLineCountInfo.OnLineCountKind[wKindIndex].dwOnLineCount=pGameKindItem->m_GameKind.dwOnLineCount;
				}

				//����ж�
				if (OnLineCountInfo.wKindCount>=CountArray(OnLineCountInfo.OnLineCountKind))
				{
					ASSERT(FALSE);
					break;
				}

			} while (KindPosition!=NULL);

			//��������
			WORD wHeadSize=sizeof(OnLineCountInfo)-sizeof(OnLineCountInfo.OnLineCountKind);
			WORD wSendSize=wHeadSize+OnLineCountInfo.wKindCount*sizeof(OnLineCountInfo.OnLineCountKind[0]);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_ONLINE_COUNT_INFO,0,&OnLineCountInfo,wSendSize);

			return true;
		}
	}

	return false;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//��ȡ����
	ASSERT(LOWORD(dwSocketID)<m_pInitParameter->m_wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pInitParameter->m_wMaxConnect) return false;

//	CString str;
//	str.Format(TEXT("%ld"), dwClientAddr);

//	CTraceService::TraceString(str,TraceLevel_Normal);

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//���ñ���
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientAddr=dwClientAddr;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//�����Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	ZeroMemory((m_pBindParameter+wBindIndex),sizeof(tagBindParameter));

	// ��¼�뿪��Ϣ
	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	LOGI("CAttemperEngineSink::OnEventTCPNetworkRead, wMainCmdID=" << Command.wMainCmdID << ", wSubCmdID="<< Command.wSubCmdID);
	switch (Command.wMainCmdID)
	{
	case MDM_GP_LOGON:			//��¼����
		{
			return OnTCPNetworkMainPCLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GP_SERVER_LIST:	//�б�����
		{
			return OnTCPNetworkMainPCServerList(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GP_USER_SERVICE:	//��������
		{
			return OnTCPNetworkMainPCUserService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GP_REMOTE_SERVICE:	//Զ�̷���
		{
			return OnTCPNetworkMainPCRemoteService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_MB_LOGON:			//MB��¼����
		{
			return OnTCPNetworkMainMBLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_MB_SERVER_LIST:	//�б�����
		{
			return OnTCPNetworkMainMBServerList(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_MB_SYSTEM_MESSAGE:	// ����ϵͳ������Ϣ
		{
			return OnTCPNetworkMainMBSystemMessage(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}
	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GP_LOGON_SUCCESS:			//��¼�ɹ�
		{
			return OnDBPCLogonSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GP_LOGON_FAILURE:			//��¼ʧ��
		{
			return OnDBPCLogonFailure(dwContextID,pData,wDataSize);
		}
	case DBR_GP_VALIDATE_MBCARD:
		{
			return OnDBPCLogonValidateMBCard(dwContextID,pData,wDataSize);
		}
	case DBO_GP_PROFILE_SUCCESS:
		{
			return OnDBPCProfileSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GP_PROFILE_FAILURE:
		{
			return OnDBPCProfileFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GP_CHECKNICK:
		{
			return OnDBPCCheckNick(dwContextID,pData,wDataSize);
		}
	case DBO_GP_USER_FACE_INFO:			//�û�ͷ��
		{
			return OnDBPCUserFaceInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GP_USER_INDIVIDUAL:		//�û���Ϣ
		{
			return OnDBPCUserIndividual(dwContextID,pData,wDataSize);
		}
	case DBO_GP_USER_INSURE_INFO:		//��������
		{
			return OnDBPCUserInsureInfo(dwContextID,pData,wDataSize);
		}
    case DBR_GP_TRANS_RECORD:
        {
            return OnDBPCUserTransRecord(dwContextID,pData,wDataSize);
        }
	case DBO_GP_USER_INSURE_SUCCESS:	//���гɹ�
		{
			return OnDBPCUserInsureSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GP_USER_INSURE_FAILURE:	//����ʧ��
		{
			return OnDBPCUserInsureFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GP_USER_INSURE_USER_INFO:  //�û���Ϣ
		{
			return OnDBPCUserInsureUserInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GP_OPERATE_SUCCESS:		//�����ɹ�
		{
			return OnDBPCOperateSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GP_OPERATE_FAILURE:		//����ʧ��
		{
			return OnDBPCOperateFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_TYPE_ITEM:			//��������
		{
			return OnDBPCGameTypeItem(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_KIND_ITEM:			//��������
		{
			return OnDBPCGameKindItem(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_NODE_ITEM:			//�ڵ�����
		{
			return OnDBPCGameNodeItem(dwContextID,pData,wDataSize);
		}
	case DBO_GP_GAME_PAGE_ITEM:			//��������
		{
			return OnDBPCGamePageItem(dwContextID,pData,wDataSize);
		}
	case DBO_MB_AREA_GAME_KIND_ITEM:	//����ID��ص�GameKindItem
		{
			return OnDBMBAreaGameKindItem(dwContextID, pData, wDataSize);
		}
	case DBO_MB_AREA_GAME_SERVER_ITEM:	//����ID��ص�GameServerItem
		{
			return OnDBMBAreaGameServerItem(dwContextID, pData, wDataSize);
		}
	case DBO_MB_GAME_KIND_ITEM_EX:		// GameKind��չ��Ϣ,����App
		{
			return OnDBMBGameKindItemEx(dwContextID, pData, wDataSize);
		}
	case DBO_MB_CUSTOMTABLE_PROPERTYCONFIG:	// �Խ�����,���ĵ��ߺ;�������
		{
			return OnDBMBCustomTablePropertyConfig(dwContextID, pData, wDataSize);
		}
	case DBO_GP_GAME_LIST_RESULT:		//���ؽ��
		{
			return OnDBPCGameListResult(dwContextID,pData,wDataSize);
		}
	case DBO_MB_LOGON_SUCCESS:			//��¼�ɹ�
		{
			return OnDBMBLogonSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_MB_LOGON_FAILURE:			//��¼ʧ��
		{
			return OnDBMBLogonFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GP_INSURE_TRANS_RECEIPT:	// ת�˳ɹ���ִ
		{
			return OnDBPCUserTransReceipt(dwContextID,pData,wDataSize);
		}
	case DBO_GP_QUERY_EXCHANGE_LOVELINESS_INFO:	// �����һ����ö�ȡ�ɹ�
		{
			return OnDBPExchangeLovelinessInfo(dwContextID, pData, wDataSize);
		}
	case DBO_GP_EXCHANGE_LOVELINESS:	// �����һ�
		{
			return 	OnDBPExchangeLoveliness(dwContextID, pData, wDataSize);
		}
	case DBO_MB_SIGN_IN_CHECK_SUCCESS:
		{
			// ǩ���ɹ�
			return OnDBMBSignInCheckSuccess(dwContextID, pData, wDataSize);
		}
	case DBO_MB_RECHARGE_MESSAGE:
		{
			// ��ֵϵͳ��Ϣ
			return OnDBMBRechargeMessage(dwContextID, pData, wDataSize);
		}
		break;
	case DBO_MB_CUSTOM_SERVICE_MESSAGE:
		{
			// �ͷ���Ϣ
			return OnDBMBCustomServiceMessage(dwContextID, pData, wDataSize);
		}
		break;
	case DBO_GR_USER_CUSTOMTABLE_SUCCESS:
		{
			return OnDBUserCustomTableSuccess(dwContextID, pData, wDataSize);
		}
		break;
	case DBO_GR_USER_CUSTOMTABLE_FAILURE:
		{
			return OnDBUserCustomTableFailed(dwContextID, pData, wDataSize);
		}
		break;
	case DBO_MB_USER_DIAMOND_SUCCESS:		// ��ʯ�����ɹ�
		{
			return OnDBUserDiamondSuccess(dwContextID, pData, wDataSize);
		}
		break;
	case DBO_MB_USER_DIAMOND_FAILURE:		// ��ʯ����ʧ��
		{
			return OnDBUserDiamondFialed(dwContextID, pData, wDataSize);
		}
		break;
	}

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	try
	{
		//Э������
		if (wServiceID==NETWORK_CORRESPOND)
		{
			//�����ж�
			if (m_bNeekCorrespond==true)
			{
				//������ʾ
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf(szDescribe,CountArray(szDescribe),TEXT("��Э�������������ӹر��ˣ�%ld �����������"),m_pInitParameter->m_wConnectTime);

				//��ʾ��Ϣ
				CTraceService::TraceString(szDescribe,TraceLevel_Warning);

				//����ʱ��
				ASSERT(m_pITimerEngine!=NULL);
				m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);

				return true;
			}
		}
	}
	catch(...)
	{
		LOGE("CAttemperEngineSink::OnEventTCPSocketShut catch");
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf(szDescribe,CountArray(szDescribe),TEXT("Э������������ʧ�� [ %ld ]��%ld �����������"),
				nErrorCode,m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("����ע����Ϸ��¼������..."),TraceLevel_Normal);

		//��������
		CMD_CS_C_RegisterPlaza RegisterPlaza;
		ZeroMemory(&RegisterPlaza,sizeof(RegisterPlaza));

		//���ñ���
		lstrcpyn(RegisterPlaza.szServerName,m_pInitParameter->m_szServerName,CountArray(RegisterPlaza.szServerName));
		lstrcpyn(RegisterPlaza.szServerAddr,m_pInitParameter->m_ServiceAddress.szAddress,CountArray(RegisterPlaza.szServerAddr));

		//��������
		m_pITCPSocketService->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_PLAZA,&RegisterPlaza,sizeof(RegisterPlaza));

		return true;
	}

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:		//ע�����
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_SERVICE_INFO:	//������Ϣ
			{
				return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_REMOTE_SERVICE:	//Զ�̷���
			{
				return OnTCPSocketMainRemoteService(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_MANAGER_SERVICE: //�������
			{
				return true;
			}
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//ע���¼�
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//��������
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;

			//Ч�����
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���
			m_bNeekCorrespond=false;
			m_pITCPSocketService->CloseSocket();

			//��ʾ��Ϣ
			LPCTSTR pszDescribeString=pRegisterFailure->szDescribeString;
			if (lstrlen(pszDescribeString)>0) CTraceService::TraceString(pszDescribeString,TraceLevel_Exception);

			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//�б��¼�
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVER_INFO:		//������Ϣ
		{
			//�����б�
			m_ServerListManager.DisuseServerItem();

			return true;
		}
	case SUB_CS_S_SERVER_ONLINE:	//��������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerOnLine));
			if (wDataSize!=sizeof(CMD_CS_S_ServerOnLine)) return false;

			//��������
			CMD_CS_S_ServerOnLine * pServerOnLine=(CMD_CS_S_ServerOnLine *)pData;

			//���ҷ���
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerOnLine->wServerID);
			if(pGameServerItem == NULL) return true;

			//��������
			DWORD dwOldOnlineCount=0;
			dwOldOnlineCount = pGameServerItem->m_GameServer.dwOnLineCount;
			pGameServerItem->m_GameServer.dwOnLineCount=pServerOnLine->dwOnLineCount;

			//Ŀ¼����
			CGameKindItem * pGameKindItem=m_ServerListManager.SearchGameKind(pGameServerItem->m_GameServer.wKindID);
			if (pGameKindItem!=NULL)
			{
				tagGameServer * pGameServer=&pGameServerItem->m_GameServer;
				pGameKindItem->m_GameKind.dwOnLineCount -= dwOldOnlineCount;
				pGameKindItem->m_GameKind.dwOnLineCount += pGameServer->dwOnLineCount;
			}

			return true;
		}
	case SUB_CS_S_SERVER_INSERT:	//�������
		{
			//Ч�����
			
			ASSERT(wDataSize%sizeof(tagGameServer)==0);
			if (wDataSize%sizeof(tagGameServer)!=0) return false;

			//��������
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;

			//��������
			for (WORD i=0;i<wItemCount;i++)
			{
				m_ServerListManager.InsertGameServer(pGameServer++);
			}

			return true;
		}
	case SUB_CS_S_SERVER_MODIFY:	//�����޸�
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerModify));
			if (wDataSize!=sizeof(CMD_CS_S_ServerModify)) return false;

			//��������
			CMD_CS_S_ServerModify * pServerModify=(CMD_CS_S_ServerModify *)pData;

			//���ҷ���
			ASSERT(m_ServerListManager.SearchGameServer(pServerModify->wServerID));
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerModify->wServerID);

			//���÷���
			if (pGameServerItem!=NULL)
			{
				//��������
				DWORD dwOldOnlineCount=0, dwOldFullCount=0;
				dwOldOnlineCount = pGameServerItem->m_GameServer.dwOnLineCount;
				dwOldFullCount   = pGameServerItem->m_GameServer.dwFullCount;

				//�޸ķ�����Ϣ
				pGameServerItem->m_GameServer.wKindID=pServerModify->wKindID;
				pGameServerItem->m_GameServer.wNodeID=pServerModify->wNodeID;
				pGameServerItem->m_GameServer.wSortID=pServerModify->wSortID;
				pGameServerItem->m_GameServer.wServerPort=pServerModify->wServerPort;
				pGameServerItem->m_GameServer.dwOnLineCount=pServerModify->dwOnLineCount;
				pGameServerItem->m_GameServer.dwFullCount=pServerModify->dwFullCount;
				lstrcpyn(pGameServerItem->m_GameServer.szServerName,pServerModify->szServerName,CountArray(pGameServerItem->m_GameServer.szServerName));
				lstrcpyn(pGameServerItem->m_GameServer.szServerAddr,pServerModify->szServerAddr,CountArray(pGameServerItem->m_GameServer.szServerAddr));

				//Ŀ¼����
				CGameKindItem * pGameKindItem=m_ServerListManager.SearchGameKind(pGameServerItem->m_GameServer.wKindID);
				if (pGameKindItem!=NULL)
				{
					tagGameServer * pGameServer=&pGameServerItem->m_GameServer;
					pGameKindItem->m_GameKind.dwOnLineCount -= dwOldOnlineCount;
					pGameKindItem->m_GameKind.dwOnLineCount += pGameServer->dwOnLineCount;

					pGameKindItem->m_GameKind.dwFullCount -= dwOldFullCount;
					pGameKindItem->m_GameKind.dwFullCount += pGameServer->dwFullCount;
				}
			}

			return true;
		}
	case SUB_CS_S_SERVER_REMOVE:	//����ɾ��
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerRemove));
			if (wDataSize!=sizeof(CMD_CS_S_ServerRemove)) return false;

			//��������
			CMD_CS_S_ServerRemove * pServerRemove=(CMD_CS_S_ServerRemove *)pData;

			//��������
			m_ServerListManager.DeleteGameServer(pServerRemove->wServerID);

			return true;
		}
	case SUB_CS_S_SERVER_FINISH:	//�������
		{
			//�����б�
			m_ServerListManager.CleanServerItem();

			//�¼�����
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//Զ�̷���
bool CAttemperEngineSink::OnTCPSocketMainRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SEARCH_CORRESPOND:	//Э������
		{
			//��������
			CMD_CS_S_SearchCorrespond * pSearchCorrespond=(CMD_CS_S_SearchCorrespond *)pData;

			//Ч�����
			ASSERT(wDataSize<=sizeof(CMD_CS_S_SearchCorrespond));
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_SearchCorrespond)-sizeof(pSearchCorrespond->UserRemoteInfo)));
			ASSERT(wDataSize==(sizeof(CMD_CS_S_SearchCorrespond)-sizeof(pSearchCorrespond->UserRemoteInfo)+pSearchCorrespond->wUserCount*sizeof(pSearchCorrespond->UserRemoteInfo[0])));

			//Ч�����
			if (wDataSize>sizeof(CMD_CS_S_SearchCorrespond)) return false;
			if (wDataSize<(sizeof(CMD_CS_S_SearchCorrespond)-sizeof(pSearchCorrespond->UserRemoteInfo))) return false;
			if (wDataSize!=(sizeof(CMD_CS_S_SearchCorrespond)-sizeof(pSearchCorrespond->UserRemoteInfo)+pSearchCorrespond->wUserCount*sizeof(pSearchCorrespond->UserRemoteInfo[0]))) return false;

			//�ж�����
			ASSERT(LOWORD(pSearchCorrespond->dwSocketID)<m_pInitParameter->m_wMaxConnect);
			if ((m_pBindParameter+LOWORD(pSearchCorrespond->dwSocketID))->dwSocketID!=pSearchCorrespond->dwSocketID) return true;

			//��������
			CMD_GP_S_SearchCorrespond SearchCorrespond;
			ZeroMemory(&SearchCorrespond,sizeof(SearchCorrespond));

			//���ñ���
			for (WORD i=0;i<pSearchCorrespond->wUserCount;i++)
			{
				//����Ч��
				ASSERT(SearchCorrespond.wUserCount<CountArray(SearchCorrespond.UserRemoteInfo));
				if (SearchCorrespond.wUserCount>=CountArray(SearchCorrespond.UserRemoteInfo)) break;

				//���ñ���
				WORD wIndex=SearchCorrespond.wUserCount++;
				CopyMemory(&SearchCorrespond.UserRemoteInfo[wIndex],&pSearchCorrespond->UserRemoteInfo[i],sizeof(SearchCorrespond.UserRemoteInfo[wIndex]));
			}

			//��������
			WORD wHeadSize=sizeof(SearchCorrespond)-sizeof(SearchCorrespond.UserRemoteInfo);
			WORD wItemSize=sizeof(SearchCorrespond.UserRemoteInfo[0])*SearchCorrespond.wUserCount;
			m_pITCPNetworkEngine->SendData(pSearchCorrespond->dwSocketID,MDM_GP_REMOTE_SERVICE,SUB_GP_S_SEARCH_CORRESPOND,&SearchCorrespond,wHeadSize+wItemSize);

			return true;
		}
		case SUB_CS_S_USER_CUSTOM_TABLE_SUCCESS:
		{
			// �Խ����ӳɹ�
			//Ч�����
			ASSERT(wDataSize == sizeof(CMD_CS_C_UserCustomTableSuccess));
			if (wDataSize != sizeof(CMD_CS_C_UserCustomTableSuccess)) return false;

			//��������
			CMD_CS_C_UserCustomTableSuccess * pUserCustomTableSuccess = (CMD_CS_C_UserCustomTableSuccess *)pData;

			//�û��Զ��巿��(Э������������¼������)
			CMD_MB_UserCustomTableSuccess UserCustomTableSuccess;
//			UserCustomTableSuccess.wKindID = pUserCustomTableSuccess->wKindID;
			UserCustomTableSuccess.wServerID = pUserCustomTableSuccess->wServerID;
			lstrcpyn(UserCustomTableSuccess.szSecret, pUserCustomTableSuccess->szSecret, CountArray(UserCustomTableSuccess.szSecret));
			UserCustomTableSuccess.bLockedInTable = true;
			UserCustomTableSuccess.dwDiamondCount = pUserCustomTableSuccess->dwPropertyCount;
			m_pITCPNetworkEngine->SendData(pUserCustomTableSuccess->dwSocketID, MDM_GP_USER_SERVICE, SUB_MB_USER_CUSTOM_TABLE_SUCCESS, &UserCustomTableSuccess, sizeof(UserCustomTableSuccess));

			return true;
		}
		case SUB_CS_S_USER_CUSTOM_TABLE_FAILED:
		{	
			//Ч�����
			ASSERT(wDataSize == sizeof(CMD_CS_C_UserCustomTableFailed));
			if (wDataSize != sizeof(CMD_CS_C_UserCustomTableFailed)) return false;

			//��������
			CMD_CS_C_UserCustomTableFailed * pUserCustomTableFailed = (CMD_CS_C_UserCustomTableFailed *)pData;
			
			CMD_MB_UserCustomTableFailed UserCustomTableFailed;
			memset(&UserCustomTableFailed, 0, sizeof(UserCustomTableFailed));
			UserCustomTableFailed.lErrorCode = UserCustomTableFailed.lErrorCode;
//			UserCustomTableFailed.bLockedInTable = false;
			lstrcpyn(UserCustomTableFailed.szDescribeString, pUserCustomTableFailed->szDescribeString, CountArray(UserCustomTableFailed.szDescribeString));

			//��������
			WORD wStringSize = CountStringBuffer(UserCustomTableFailed.szDescribeString);
			WORD wSendSize = sizeof(UserCustomTableFailed) - sizeof(UserCustomTableFailed.szDescribeString) + wStringSize;
			//�û��Զ��巿��(Э������������¼������)
			m_pITCPNetworkEngine->SendData(pUserCustomTableFailed->dwSocketID, MDM_GP_USER_SERVICE, SUB_MB_USER_CUSTOM_TABLE_FAILED, &UserCustomTableFailed, wSendSize);

			return true;
		}
	}

	return true;
}

//��¼����
bool CAttemperEngineSink::OnTCPNetworkMainPCLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_LOGON_GAMEID:		//I D ��¼
		{
			return OnTCPNetworkSubPCLogonGameID(pData,wDataSize,dwSocketID);
		}
	case SUB_GP_LOGON_ACCOUNTS:		//�ʺŵ�¼
		{
//			LOGI("CAttemperEngineSink::OnTCPNetworkMainPCLogon, OnTCPNetworkSubPCLogonAccounts");
			return OnTCPNetworkSubPCLogonAccounts(pData,wDataSize,dwSocketID);
		}
	case SUB_GP_REGISTER_ACCOUNTS:	//�ʺ�ע��
		{
			return OnTCPNetworkSubPCRegisterAccounts(pData,wDataSize,dwSocketID);
		}
	case SUB_GP_REGISTER_PROFILE:	//�������� by chenj
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(CMD_GP_UserProfile));
			if (wDataSize<sizeof(CMD_GP_UserProfile)) return false;

			//������Ϣ
			CMD_GP_UserProfile * pRegisterProfile=(CMD_GP_UserProfile *)pData;
			pRegisterProfile->szPassword[CountArray(pRegisterProfile->szPassword)-1]=0;

			//��������
			DBR_GP_RegisterProfile RegisterProfile;
			ZeroMemory(&RegisterProfile,sizeof(RegisterProfile));

			RegisterProfile.dwUserID = pRegisterProfile->dwUserID;
			RegisterProfile.dwClientAddr = (m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			RegisterProfile.cbGender = pRegisterProfile->cbGender;
			lstrcpyn(RegisterProfile.szPassword,pRegisterProfile->szPassword,CountArray(RegisterProfile.szPassword));
			lstrcpyn(RegisterProfile.szNickName,pRegisterProfile->szNickName,CountArray(RegisterProfile.szNickName));
			lstrcpyn(RegisterProfile.szQQ,pRegisterProfile->szQQ,CountArray(RegisterProfile.szQQ));
			lstrcpyn(RegisterProfile.szMobile,pRegisterProfile->szMobile,CountArray(RegisterProfile.szMobile));
			lstrcpyn(RegisterProfile.szUnderWrite,pRegisterProfile->szUnderWrite,CountArray(RegisterProfile.szUnderWrite));
			lstrcpyn(RegisterProfile.szPassPortID,pRegisterProfile->szPassPortID,CountArray(RegisterProfile.szPassPortID));
			lstrcpyn(RegisterProfile.szCompellation,pRegisterProfile->szCompellation,CountArray(RegisterProfile.szCompellation));
			lstrcpyn(RegisterProfile.szSecurityQuestion,pRegisterProfile->szSecurityQuestion,CountArray(RegisterProfile.szSecurityQuestion));
			lstrcpyn(RegisterProfile.szSecurityAnswer,pRegisterProfile->szSecurityAnswer,CountArray(RegisterProfile.szSecurityAnswer));

			// �����Ա�������ͷ��
			CRandom random;
			do 
			{
				RegisterProfile.wFaceID = random.Random(8);
			} while ( (RegisterProfile.wFaceID/4)!=RegisterProfile.cbGender);

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_REGISTER_PROFILE,dwSocketID,&RegisterProfile,sizeof(RegisterProfile));

			return true;
		}
	case SUB_GP_CHECKNICK:
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(CMD_GP_CheckNick));
			if (wDataSize<sizeof(CMD_GP_CheckNick)) return false;

			//������Ϣ
			CMD_GP_CheckNick * pCheckNick=(CMD_GP_CheckNick *)pData;

			//��������
			DBR_GP_CheckNick CheckNick;
			ZeroMemory(&CheckNick,sizeof(CheckNick));

			CheckNick.dwUserID = pCheckNick->dwUserID;
			lstrcpyn(CheckNick.szNickName,pCheckNick->szNickName,CountArray(CheckNick.szNickName));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_CHECKNICK,dwSocketID,&CheckNick,sizeof(CheckNick));
			return true;
		}
	}

	return false;
}

//�б���
bool CAttemperEngineSink::OnTCPNetworkMainPCServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_GET_LIST:			//��ȡ�б�
		{
			//�����б�
			SendGameTypeInfo(dwSocketID);
			SendGameKindInfo(dwSocketID);

			//�����б�
			if (m_pInitParameter->m_cbDelayList==TRUE)
			{
				//�����б�
				SendGamePageInfo(dwSocketID,INVALID_WORD);
				SendGameNodeInfo(dwSocketID,INVALID_WORD);
				SendGameServerInfo(dwSocketID,INVALID_WORD);
			}
			else
			{
				//����ҳ��
				SendGamePageInfo(dwSocketID,0);
			}

			//�������
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH);

			return true;
		}
	case SUB_GP_GET_SERVER:			//��ȡ����
		{
			//Ч������
			ASSERT(wDataSize%sizeof(WORD)==0);
			if (wDataSize%sizeof(WORD)!=0) return false;

			//�����б�
			UINT nKindCount=wDataSize/sizeof(WORD);
			for (UINT i=0;i<nKindCount;i++)
			{
				SendGameNodeInfo(dwSocketID,((WORD *)pData)[i]);
				SendGamePageInfo(dwSocketID,((WORD *)pData)[i]);
				SendGameServerInfo(dwSocketID,((WORD *)pData)[i]);
			}

			//�������
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_SERVER_FINISH,pData,wDataSize);

			return true;
		}
	case SUB_GP_GET_ONLINE:			//��ȡ����
		{
			//��������
			CMD_GP_GetOnline * pGetOnline=(CMD_GP_GetOnline *)pData;
			WORD wHeadSize=(sizeof(CMD_GP_GetOnline)-sizeof(pGetOnline->wOnLineServerID));

			//Ч������
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGetOnline->wServerCount*sizeof(WORD))));
			if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGetOnline->wServerCount*sizeof(WORD)))) return false;

			//��������
			CMD_GP_KindOnline KindOnline;
			CMD_GP_ServerOnline ServerOnline;
			ZeroMemory(&KindOnline,sizeof(KindOnline));
			ZeroMemory(&ServerOnline,sizeof(ServerOnline));

			//��ȡ����
			POSITION KindPosition=NULL;
			do
			{
				//��ȡ����
				CGameKindItem * pGameKindItem=m_ServerListManager.EmunGameKindItem(KindPosition);

				//���ñ���
				if (pGameKindItem!=NULL)
				{
					WORD wKindIndex=KindOnline.wKindCount++;
					KindOnline.OnLineInfoKind[wKindIndex].wKindID=pGameKindItem->m_GameKind.wKindID;
					KindOnline.OnLineInfoKind[wKindIndex].dwOnLineCount=pGameKindItem->m_GameKind.dwOnLineCount;
				}

				//����ж�
				if (KindOnline.wKindCount>=CountArray(KindOnline.OnLineInfoKind))
				{
					ASSERT(FALSE);
					break;
				}

			} while (KindPosition!=NULL);

			//��ȡ����
			for (WORD i=0;i<pGetOnline->wServerCount;i++)
			{
				//��ȡ����
				WORD wServerID=pGetOnline->wOnLineServerID[i];
				CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(wServerID);

				//���ñ���
				if (pGameServerItem!=NULL)
				{
					WORD wServerIndex=ServerOnline.wServerCount++;
					ServerOnline.OnLineInfoServer[wServerIndex].wServerID=wServerID;
					ServerOnline.OnLineInfoServer[wServerIndex].dwOnLineCount=pGameServerItem->m_GameServer.dwOnLineCount;
				}

				//����ж�
				if (ServerOnline.wServerCount>=CountArray(ServerOnline.OnLineInfoServer))
				{
					ASSERT(FALSE);
					break;
				}
			}

			//��������
			if (KindOnline.wKindCount>0)
			{
				WORD wHeadSize=sizeof(KindOnline)-sizeof(KindOnline.OnLineInfoKind);
				WORD wSendSize=wHeadSize+KindOnline.wKindCount*sizeof(KindOnline.OnLineInfoKind[0]);
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GR_KINE_ONLINE,&KindOnline,wSendSize);
			}

			//��������
			if (ServerOnline.wServerCount>0)
			{
				WORD wHeadSize=sizeof(ServerOnline)-sizeof(ServerOnline.OnLineInfoServer);
				WORD wSendSize=wHeadSize+ServerOnline.wServerCount*sizeof(ServerOnline.OnLineInfoServer[0]);
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GR_SERVER_ONLINE,&ServerOnline,wSendSize);
			}

			return true;
		}
	case SUB_GP_GET_COLLECTION:		//��ȡ�ղ�
		{
			return true;
		}
	}

	return false;
}

//������
bool CAttemperEngineSink::OnTCPNetworkMainPCUserService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_MODIFY_MACHINE:		//�󶨻���
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_ModifyMachine));
			if (wDataSize!=sizeof(CMD_GP_ModifyMachine)) return false;

			//������Ϣ
			CMD_GP_ModifyMachine * pModifyMachine=(CMD_GP_ModifyMachine *)pData;
			pModifyMachine->szPassword[CountArray(pModifyMachine->szPassword)-1]=0;

			//��������
			DBR_GP_ModifyMachine ModifyMachine;
			ZeroMemory(&ModifyMachine,sizeof(ModifyMachine));

			//��������
			ModifyMachine.cbBind=pModifyMachine->cbBind;
			ModifyMachine.dwUserID=pModifyMachine->dwUserID;
			ModifyMachine.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ModifyMachine.szPassword,pModifyMachine->szPassword,CountArray(ModifyMachine.szPassword));
			lstrcpyn(ModifyMachine.szMachineID,pModifyMachine->szMachineID,CountArray(ModifyMachine.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_MACHINE,dwSocketID,&ModifyMachine,sizeof(ModifyMachine));

			return true;
		}
	case SUB_GP_MODIFY_LOGON_PASS:	//�޸�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_ModifyLogonPass));
			if (wDataSize!=sizeof(CMD_GP_ModifyLogonPass)) return false;

			//������Ϣ
			CMD_GP_ModifyLogonPass * pModifyLogonPass=(CMD_GP_ModifyLogonPass *)pData;
			pModifyLogonPass->szDesPassword[CountArray(pModifyLogonPass->szDesPassword)-1]=0;
			pModifyLogonPass->szScrPassword[CountArray(pModifyLogonPass->szScrPassword)-1]=0;

			//��������
			DBR_GP_ModifyLogonPass ModifyLogonPass;
			ZeroMemory(&ModifyLogonPass,sizeof(ModifyLogonPass));

			//��������
			ModifyLogonPass.dwUserID=pModifyLogonPass->dwUserID;
			ModifyLogonPass.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ModifyLogonPass.szDesPassword,pModifyLogonPass->szDesPassword,CountArray(ModifyLogonPass.szDesPassword));
			lstrcpyn(ModifyLogonPass.szScrPassword,pModifyLogonPass->szScrPassword,CountArray(ModifyLogonPass.szScrPassword));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_LOGON_PASS,dwSocketID,&ModifyLogonPass,sizeof(ModifyLogonPass));

			return true;
		}
	case SUB_GP_MODIFY_INSURE_PASS:	//�޸�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_ModifyInsurePass));
			if (wDataSize!=sizeof(CMD_GP_ModifyInsurePass)) return false;

			//������Ϣ
			CMD_GP_ModifyInsurePass * pModifyInsurePass=(CMD_GP_ModifyInsurePass *)pData;
			pModifyInsurePass->szDesPassword[CountArray(pModifyInsurePass->szDesPassword)-1]=0;
			pModifyInsurePass->szScrPassword[CountArray(pModifyInsurePass->szScrPassword)-1]=0;

			//��������
			DBR_GP_ModifyInsurePass ModifyInsurePass;
			ZeroMemory(&ModifyInsurePass,sizeof(ModifyInsurePass));

			//��������
			ModifyInsurePass.dwUserID=pModifyInsurePass->dwUserID;
			ModifyInsurePass.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ModifyInsurePass.szDesPassword,pModifyInsurePass->szDesPassword,CountArray(ModifyInsurePass.szDesPassword));
			lstrcpyn(ModifyInsurePass.szScrPassword,pModifyInsurePass->szScrPassword,CountArray(ModifyInsurePass.szScrPassword));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_INSURE_PASS,dwSocketID,&ModifyInsurePass,sizeof(ModifyInsurePass));

			return true;
		}
	case SUB_GP_MODIFY_UNDER_WRITE:	//�޸�ǩ��
		{
			//��������
			CMD_GP_ModifyUnderWrite * pModifyUnderWrite=(CMD_GP_ModifyUnderWrite *)pData;

			//Ч�����
			ASSERT(wDataSize<=sizeof(CMD_GP_ModifyUnderWrite));
			ASSERT(wDataSize>=(sizeof(CMD_GP_ModifyUnderWrite)-sizeof(pModifyUnderWrite->szUnderWrite)));
			ASSERT(wDataSize==(sizeof(CMD_GP_ModifyUnderWrite)-sizeof(pModifyUnderWrite->szUnderWrite)+CountStringBuffer(pModifyUnderWrite->szUnderWrite)));

			//Ч�����
			if (wDataSize>sizeof(CMD_GP_ModifyUnderWrite)) return false;
			if (wDataSize<(sizeof(CMD_GP_ModifyUnderWrite)-sizeof(pModifyUnderWrite->szUnderWrite))) return false;
			if (wDataSize!=(sizeof(CMD_GP_ModifyUnderWrite)-sizeof(pModifyUnderWrite->szUnderWrite)+CountStringBuffer(pModifyUnderWrite->szUnderWrite))) return false;

			//������Ϣ
//			pModifyUnderWrite->szPassword[CountArray(pModifyUnderWrite->szPassword)-1]=0;
			pModifyUnderWrite->szUnderWrite[CountArray(pModifyUnderWrite->szUnderWrite)-1]=0;

			//��������
			DBR_GP_ModifyUnderWrite ModifyUnderWrite;
			ZeroMemory(&ModifyUnderWrite,sizeof(ModifyUnderWrite));

			//��������
			ModifyUnderWrite.dwUserID=pModifyUnderWrite->dwUserID;
			ModifyUnderWrite.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
//			lstrcpyn(ModifyUnderWrite.szPassword,pModifyUnderWrite->szPassword,CountArray(ModifyUnderWrite.szPassword));
			lstrcpyn(ModifyUnderWrite.szUnderWrite,pModifyUnderWrite->szUnderWrite,CountArray(ModifyUnderWrite.szUnderWrite));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_UNDER_WRITE,dwSocketID,&ModifyUnderWrite,sizeof(ModifyUnderWrite));

			return true;
		}
	case SUB_GP_SYSTEM_FACE_INFO:	//�޸�ͷ��
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_SystemFaceInfo));
			if (wDataSize!=sizeof(CMD_GP_SystemFaceInfo)) return false;

			//������Ϣ
			CMD_GP_SystemFaceInfo * pSystemFaceInfo=(CMD_GP_SystemFaceInfo *)pData;

			//��������
			DBR_GP_ModifySystemFace ModifySystemFace;
			ZeroMemory(&ModifySystemFace,sizeof(ModifySystemFace));

			//��������
			ModifySystemFace.wFaceID=pSystemFaceInfo->wFaceID;
			ModifySystemFace.dwUserID=pSystemFaceInfo->dwUserID;
			ModifySystemFace.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ModifySystemFace.szPassword,pSystemFaceInfo->szPassword,CountArray(ModifySystemFace.szPassword));
			lstrcpyn(ModifySystemFace.szMachineID,pSystemFaceInfo->szMachineID,CountArray(ModifySystemFace.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_SYSTEM_FACE,dwSocketID,&ModifySystemFace,sizeof(ModifySystemFace));

			return true;
		}
	case SUB_GP_CUSTOM_FACE_INFO:	//�޸�ͷ��
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_CustomFaceInfo));
			if (wDataSize!=sizeof(CMD_GP_CustomFaceInfo)) return false;

			//������Ϣ
			CMD_GP_CustomFaceInfo * pCustomFaceInfo=(CMD_GP_CustomFaceInfo *)pData;

			//��������
			DBR_GP_ModifyCustomFace ModifyCustomFace;
			ZeroMemory(&ModifyCustomFace,sizeof(ModifyCustomFace));

			//��������
			ModifyCustomFace.dwUserID=pCustomFaceInfo->dwUserID;
			ModifyCustomFace.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ModifyCustomFace.szPassword,pCustomFaceInfo->szPassword,CountArray(ModifyCustomFace.szPassword));
			lstrcpyn(ModifyCustomFace.szMachineID,pCustomFaceInfo->szMachineID,CountArray(ModifyCustomFace.szMachineID));
			CopyMemory(ModifyCustomFace.dwCustomFace,pCustomFaceInfo->dwCustomFace,sizeof(ModifyCustomFace.dwCustomFace));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_CUSTOM_FACE,dwSocketID,&ModifyCustomFace,sizeof(ModifyCustomFace));

			return true;
		}
	case SUB_GP_QUERY_INDIVIDUAL:	//��ѯ��Ϣ
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_QueryIndividual));
			if (wDataSize!=sizeof(CMD_GP_QueryIndividual)) return false;

			//������Ϣ
			CMD_GP_QueryIndividual * pQueryIndividual=(CMD_GP_QueryIndividual *)pData;

			//��������
			DBR_GP_QueryIndividual QueryIndividual;
			ZeroMemory(&QueryIndividual,sizeof(QueryIndividual));

			//��������
			QueryIndividual.dwUserID=pQueryIndividual->dwUserID;
			QueryIndividual.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_INDIVIDUAL,dwSocketID,&QueryIndividual,sizeof(QueryIndividual));

			return true;
		}
	case SUB_GP_MODIFY_INDIVIDUAL:	//�޸�����
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(CMD_GP_ModifyIndividual));
			if (wDataSize<sizeof(CMD_GP_ModifyIndividual)) return false;

			//������Ϣ
			CMD_GP_ModifyIndividual * pModifyIndividual=(CMD_GP_ModifyIndividual *)pData;
			pModifyIndividual->szPassword[CountArray(pModifyIndividual->szPassword)-1]=0;

			//��������
			DBR_GP_ModifyIndividual ModifyIndividual;
			ZeroMemory(&ModifyIndividual,sizeof(ModifyIndividual));

			//���ñ���
			ModifyIndividual.dwUserID=pModifyIndividual->dwUserID;
			ModifyIndividual.cbGender=pModifyIndividual->cbGender;
			ModifyIndividual.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
			lstrcpyn(ModifyIndividual.szPassword,pModifyIndividual->szPassword,CountArray(ModifyIndividual.szPassword));

			//��������
			VOID * pDataBuffer=NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pModifyIndividual+1,wDataSize-sizeof(CMD_GP_ModifyIndividual));

			//��չ��Ϣ
			while (true)
			{
				pDataBuffer=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL) break;
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_GP_UI_NICKNAME:		//�û��ǳ�
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szNickName));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szNickName))
						{
							CopyMemory(&ModifyIndividual.szNickName,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szNickName[CountArray(ModifyIndividual.szNickName)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_UNDER_WRITE:		//����ǩ��
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szUnderWrite));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szUnderWrite))
						{
							CopyMemory(&ModifyIndividual.szUnderWrite,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szUnderWrite[CountArray(ModifyIndividual.szUnderWrite)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_USER_NOTE:		//�û���ע
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szUserNote));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szUserNote))
						{
							CopyMemory(&ModifyIndividual.szUserNote,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szUserNote[CountArray(ModifyIndividual.szUserNote)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_COMPELLATION:	//��ʵ����
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szCompellation));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szCompellation))
						{
							CopyMemory(&ModifyIndividual.szCompellation,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szCompellation[CountArray(ModifyIndividual.szCompellation)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_SEAT_PHONE:		//�̶��绰
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szSeatPhone));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szSeatPhone))
						{
							CopyMemory(ModifyIndividual.szSeatPhone,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szSeatPhone[CountArray(ModifyIndividual.szSeatPhone)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_MOBILE_PHONE:	//�ƶ��绰
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szMobilePhone));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szMobilePhone))
						{
							CopyMemory(ModifyIndividual.szMobilePhone,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szMobilePhone[CountArray(ModifyIndividual.szMobilePhone)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_QQ:				//Q Q ����
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szQQ));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szQQ))
						{
							CopyMemory(ModifyIndividual.szQQ,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szQQ[CountArray(ModifyIndividual.szQQ)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_EMAIL:			//�����ʼ�
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szEMail));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szEMail))
						{
							CopyMemory(ModifyIndividual.szEMail,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szEMail[CountArray(ModifyIndividual.szEMail)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_DWELLING_PLACE:	//��ϵ��ַ
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szDwellingPlace));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szDwellingPlace))
						{
							CopyMemory(ModifyIndividual.szDwellingPlace,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szDwellingPlace[CountArray(ModifyIndividual.szDwellingPlace)-1]=0;
						}
						break;
					}
				}
			}

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_INDIVIDUAL,dwSocketID,&ModifyIndividual,sizeof(ModifyIndividual));

			return true;
		}
	case SUB_GP_USER_SAVE_SCORE:	//������Ϸ��
		{
			//ȡ����Ϸ��
			return OnTCPNetworkUserSaveScore(wSubCmdID, pData, wDataSize, dwSocketID);
		}
	case SUB_GP_USER_TAKE_SCORE:	//ȡ����Ϸ��
		{
			//ȡ����Ϸ��
			return OnTCPNetworkUserTakeScore(wSubCmdID, pData, wDataSize, dwSocketID);
		}
	case SUB_GP_USER_TRANSFER_SCORE:
		{
			// ת����Ϸ��
			return OnTCPNetworkUserTransferScore(wSubCmdID, pData, wDataSize, dwSocketID);
		}
	case SUB_GP_QUERY_INSURE_INFO:	
		{
			//��ѯ����
			return OnTCPNetworkQueryInsureInfo(wSubCmdID, pData, wDataSize, dwSocketID);
		}
    case SUB_GP_QUERY_TRANSRECORD:
        {
            //Ч�����
            ASSERT(wDataSize==sizeof(CMD_GP_QueryInsureInfo));
            if (wDataSize!=sizeof(CMD_GP_QueryInsureInfo)) return false;

            //������Ϣ
            CMD_GP_QueryInsureInfo * pQueryInsureInfo=(CMD_GP_QueryInsureInfo *)pData;

            //��������
            DBR_GP_QueryInsureInfo QueryInsureInfo;
            ZeroMemory(&QueryInsureInfo,sizeof(QueryInsureInfo));

            //��������
            QueryInsureInfo.dwUserID=pQueryInsureInfo->dwUserID;
            QueryInsureInfo.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;

            //Ͷ������
            m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_TRANSRECORD,dwSocketID,&QueryInsureInfo,sizeof(QueryInsureInfo));
            return true;
        }
	case SUB_GP_QUERY_USER_INFO_REQUEST:  //��ѯ�û�
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_QueryUserInfoRequest));
			if (wDataSize!=sizeof(CMD_GP_QueryUserInfoRequest)) return false;

			//������Ϣ
			CMD_GP_QueryUserInfoRequest * pQueryUserInfoRequest=(CMD_GP_QueryUserInfoRequest *)pData;

			//��������
			DBR_GP_QueryInsureUserInfo QueryInsureUserInfo;
			ZeroMemory(&QueryInsureUserInfo,sizeof(QueryInsureUserInfo));

			//��������
			QueryInsureUserInfo.cbByNickName=pQueryUserInfoRequest->cbByNickName;
			lstrcpyn(QueryInsureUserInfo.szNickName,pQueryUserInfoRequest->szNickName,CountArray(QueryInsureUserInfo.szNickName));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_USER_INFO,dwSocketID,&QueryInsureUserInfo,sizeof(QueryInsureUserInfo));

			return true;
		}
	case SUB_GP_QUERY_NICKNAME_BY_GAMEID:	
		{
			// ͨ��GameID��ѯ�ǳ�
			return OnTCPNetworkQueryNicknameByGameID(wSubCmdID, pData, wDataSize, dwSocketID);
		}
		break;
	case SUB_GP_VERIFY_INSURE_PASSWORD:		
		{
			// ��֤��������
			return OnTCPNetworkVerifyInsurePassword(wSubCmdID, pData, wDataSize, dwSocketID);
		}
	case SUB_GP_QUERY_EXCHANGE_LOVELINESS:
		{
			// ��ѯ�����һ���Ϣ
			return OnTCPNetworkQueryExchangeLoveliness(wSubCmdID, pData, wDataSize, dwSocketID);
		}
	case SUB_GP_EXCHANGE_LOVELINESS:
		{
			// ����һ�
			return OnTCPNetworkExchangeLoveliness(wSubCmdID, pData, wDataSize, dwSocketID);
		}
		break;
	case SUB_MB_SHARE_SUCCESS:
		{
			// ����ɹ�
			return OnTCPNetworkMBShareSuccess(wSubCmdID, pData, wDataSize, dwSocketID);
		}
	case SUB_MB_USER_CUSTOM_TABLE:
		{
			// �����Խ�����
			return OnTCPNetworkUserCreateCustomTable(wSubCmdID, pData, wDataSize, dwSocketID);
		}
	case SUB_MB_USER_JOIN_CUSTOMTABLE:
		{
			// �����Խ�����
			return OnTCPNetworkUserJoinCustomTable(wSubCmdID, pData, wDataSize, dwSocketID);
		}
	case SUB_MB_USER_QUERY_CUSTOMTABLE:
		{
			// ��ѯ������Խ�������Ϣ
			return OnTCPNetworkUserQueryCustomTable(wSubCmdID, pData, wDataSize, dwSocketID);
		}
	case SUB_MB_USER_TRANSFER_DIAMOND:
		{
			// ��ѯ������Խ�������Ϣ
			return OnTCPNetworkUserTransferDiamond(wSubCmdID, pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//Զ�̴���
bool CAttemperEngineSink::OnTCPNetworkMainPCRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_C_SEARCH_CORRESPOND:	//Э������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_C_SearchCorrespond));
			if (wDataSize!=sizeof(CMD_GP_C_SearchCorrespond)) return false;

			//������Ϣ
			CMD_GP_C_SearchCorrespond * pSearchCorrespond=(CMD_GP_C_SearchCorrespond *)pData;
			pSearchCorrespond->szNickName[CountArray(pSearchCorrespond->szNickName)-1]=0;

			//��������
			CMD_CS_C_SearchCorrespond SearchCorrespond;
			ZeroMemory(&SearchCorrespond,sizeof(SearchCorrespond));

			//���ӱ���
			SearchCorrespond.dwSocketID=dwSocketID;
			SearchCorrespond.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;

			//���ұ���
			SearchCorrespond.dwGameID=pSearchCorrespond->dwGameID;
			lstrcpyn(SearchCorrespond.szNickName,pSearchCorrespond->szNickName,CountArray(SearchCorrespond.szNickName));

			//��������
			m_pITCPSocketService->SendData(MDM_CS_REMOTE_SERVICE,SUB_CS_C_SEARCH_CORRESPOND,&SearchCorrespond,sizeof(SearchCorrespond));

			return true;
		}
	}

	return false;
}

//��¼����
bool CAttemperEngineSink::OnTCPNetworkMainMBLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_LOGON_GAMEID:		//I D ��¼
		{
			return OnTCPNetworkSubMBLogonGameID(pData,wDataSize,dwSocketID);
		}
	case SUB_GP_LOGON_ACCOUNTS:		//�ʺŵ�¼
		{
			return OnTCPNetworkSubMBLogonAccounts(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_REGISTER_ACCOUNTS:	//�ʺ�ע��
		{
			return OnTCPNetworkSubMBRegisterAccounts(pData,wDataSize,dwSocketID);
		}
	case SUB_LOGON_THIRDPARTY:		//��������¼
		{
			return OnTCPNetworkSubLogonThirdParty(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//�б���
bool CAttemperEngineSink::OnTCPNetworkMainMBServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	return false;
}

// ����ϵͳ������Ϣ
bool CAttemperEngineSink::OnTCPNetworkMainMBSystemMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_MB_RECHARGE_MESSAGE:			//��ֵ����
		{
			return OnTCPNetworkSubMBRechargeMessage(pData,wDataSize,dwSocketID);
		}
	case SUB_MB_CUSTOM_SERVICE_MESSAGE:		//�ͷ�����
		{
			return OnTCPNetworkSubMBServiceMessage(pData,wDataSize,dwSocketID);
		}
	}
	return false;
}

//I D ��¼
bool CAttemperEngineSink::OnTCPNetworkSubPCLogonGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonGameID));
	if (wDataSize<sizeof(CMD_GP_LogonGameID))
	{
		if (wDataSize<sizeof(CMD_GP_LogonGameID)-sizeof(BYTE))
			return false;
	}

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_GP_LogonGameID * pLogonGameID=(CMD_GP_LogonGameID *)pData;
	pLogonGameID->szPassword[CountArray(pLogonGameID->szPassword)-1]=0;
	pLogonGameID->szMachineID[CountArray(pLogonGameID->szMachineID)-1]=0;

	//��������
//	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonGameID->dwPlazaVersion;
	pBindParameter->wPlatformID = CC_PLATFORM_WIN32;

	//Ч��汾
	if (CheckPlazaVersion(DEVICE_TYPE_PC,pLogonGameID->dwPlazaVersion,dwSocketID,((pLogonGameID->cbValidateFlags&LOW_VER_VALIDATE_FLAGS)!=0))==false)
	{
		return true;
	}

	//��������
	DBR_GP_LogonGameID LogonGameID;
	ZeroMemory(&LogonGameID,sizeof(LogonGameID));

	//������Ϣ
	LogonGameID.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	LogonGameID.dwGameID=pLogonGameID->dwGameID;
	LogonGameID.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(LogonGameID.szPassword,pLogonGameID->szPassword,CountArray(LogonGameID.szPassword));
	lstrcpyn(LogonGameID.szMachineID,pLogonGameID->szMachineID,CountArray(LogonGameID.szMachineID));
	LogonGameID.cbNeeValidateMBCard=(pLogonGameID->cbValidateFlags&MB_VALIDATE_FLAGS);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_GAMEID,dwSocketID,&LogonGameID,sizeof(LogonGameID));

	return true;
}

//�ʺŵ�¼
bool CAttemperEngineSink::OnTCPNetworkSubPCLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	LOGI("CAttemperEngineSink::OnTCPNetworkSubPCLogonAccounts, Begin="<<GetTickCount());
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonAccounts));
	if (wDataSize<sizeof(CMD_GP_LogonAccounts))
	{
		if (wDataSize<sizeof(CMD_GP_LogonAccounts)-sizeof(BYTE))
			return false;
	}

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_GP_LogonAccounts * pLogonAccounts=(CMD_GP_LogonAccounts *)pData;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szPassword[CountArray(pLogonAccounts->szPassword)-1]=0;
	pLogonAccounts->szMachineID[CountArray(pLogonAccounts->szMachineID)-1]=0;

	//��������
//	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonAccounts->dwPlazaVersion;
	pBindParameter->wPlatformID = CC_PLATFORM_WIN32;

	//�汾�ж�
	if (CheckPlazaVersion(DEVICE_TYPE_PC,pLogonAccounts->dwPlazaVersion,dwSocketID,((pLogonAccounts->cbValidateFlags&LOW_VER_VALIDATE_FLAGS)!=0))==false)
	{
		return true;
	}

	//��������
	DBR_GP_LogonAccounts LogonAccounts;
	ZeroMemory(&LogonAccounts,sizeof(LogonAccounts));

	//������Ϣ
	LogonAccounts.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	LogonAccounts.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(LogonAccounts.szAccounts,pLogonAccounts->szAccounts,CountArray(LogonAccounts.szAccounts));
	lstrcpyn(LogonAccounts.szPassword,pLogonAccounts->szPassword,CountArray(LogonAccounts.szPassword));
	lstrcpyn(LogonAccounts.szMachineID,pLogonAccounts->szMachineID,CountArray(LogonAccounts.szMachineID));
	LogonAccounts.cbNeeValidateMBCard=(pLogonAccounts->cbValidateFlags&MB_VALIDATE_FLAGS);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_ACCOUNTS,dwSocketID,&LogonAccounts,sizeof(LogonAccounts));
	LOGI("CAttemperEngineSink::OnTCPNetworkSubPCLogonAccounts, End="<<GetTickCount());

	return true;
}

//�ʺ�ע��
bool CAttemperEngineSink::OnTCPNetworkSubPCRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GP_RegisterAccounts));
	if (wDataSize<sizeof(CMD_GP_RegisterAccounts))
	{
		if (wDataSize<sizeof(CMD_GP_RegisterAccounts)-sizeof(BYTE))
			return false;
	}

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_GP_RegisterAccounts * pRegisterAccounts=(CMD_GP_RegisterAccounts *)pData;
	pRegisterAccounts->szAccounts[CountArray(pRegisterAccounts->szAccounts)-1]=0;
	pRegisterAccounts->szNickName[CountArray(pRegisterAccounts->szNickName)-1]=0;
	pRegisterAccounts->szSpreader[CountArray(pRegisterAccounts->szSpreader)-1]=0;
	pRegisterAccounts->szMachineID[CountArray(pRegisterAccounts->szMachineID)-1]=0;
	pRegisterAccounts->szLogonPass[CountArray(pRegisterAccounts->szLogonPass)-1]=0;
	pRegisterAccounts->szInsurePass[CountArray(pRegisterAccounts->szInsurePass)-1]=0;
	pRegisterAccounts->szPassPortID[CountArray(pRegisterAccounts->szPassPortID)-1]=0;
	pRegisterAccounts->szCompellation[CountArray(pRegisterAccounts->szCompellation)-1]=0;
	pRegisterAccounts->szMobile[CountArray(pRegisterAccounts->szMobile)-1]=0;

	//��������
//	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pRegisterAccounts->dwPlazaVersion;
	pBindParameter->wPlatformID = CC_PLATFORM_WIN32;

	//Ч��汾
	if (CheckPlazaVersion(DEVICE_TYPE_PC,pRegisterAccounts->dwPlazaVersion,dwSocketID,((pRegisterAccounts->cbValidateFlags&LOW_VER_VALIDATE_FLAGS)!=0))==false)
	{
		return true;
	}

	//��������
	DBR_GP_RegisterAccounts RegisterAccounts;
	ZeroMemory(&RegisterAccounts,sizeof(RegisterAccounts));

	//������Ϣ
	RegisterAccounts.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	RegisterAccounts.wFaceID=pRegisterAccounts->wFaceID;
	RegisterAccounts.cbGender=pRegisterAccounts->cbGender;
	RegisterAccounts.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(RegisterAccounts.szAccounts,pRegisterAccounts->szAccounts,CountArray(RegisterAccounts.szAccounts));
	lstrcpyn(RegisterAccounts.szNickName,pRegisterAccounts->szNickName,CountArray(RegisterAccounts.szNickName));
	lstrcpyn(RegisterAccounts.szSpreader,pRegisterAccounts->szSpreader,CountArray(RegisterAccounts.szSpreader));
	lstrcpyn(RegisterAccounts.szMachineID,pRegisterAccounts->szMachineID,CountArray(RegisterAccounts.szMachineID));
	lstrcpyn(RegisterAccounts.szLogonPass,pRegisterAccounts->szLogonPass,CountArray(RegisterAccounts.szLogonPass));
	lstrcpyn(RegisterAccounts.szInsurePass,pRegisterAccounts->szInsurePass,CountArray(RegisterAccounts.szInsurePass));
	lstrcpyn(RegisterAccounts.szPassPortID,pRegisterAccounts->szPassPortID,CountArray(RegisterAccounts.szPassPortID));
	lstrcpyn(RegisterAccounts.szCompellation,pRegisterAccounts->szCompellation,CountArray(RegisterAccounts.szCompellation));
	//lstrcpyn(RegisterAccounts.szMobile,pRegisterAccounts->szMobile,CountArray(RegisterAccounts.szMobile));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_REGISTER_ACCOUNTS,dwSocketID,&RegisterAccounts,sizeof(RegisterAccounts));

	return true;
}

//I D ��¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_LogonGameID));
	if (wDataSize<sizeof(CMD_MB_LogonGameID)) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_MB_LogonGameID * pLogonGameID=(CMD_MB_LogonGameID *)pData;
	pLogonGameID->szPassword[CountArray(pLogonGameID->szPassword)-1]=0;
	pLogonGameID->szMachineID[CountArray(pLogonGameID->szMachineID)-1]=0;
	pLogonGameID->szMobilePhone[CountArray(pLogonGameID->szMobilePhone)-1]=0;

	//��������
//	pBindParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pBindParameter->wModuleID=pLogonGameID->wModuleID;
	pBindParameter->dwPlazaVersion=pLogonGameID->dwPlazaVersion;
	pBindParameter->wPlatformID = DeviceTypeToPlatformID(pLogonGameID->cbDeviceType);

	//Ч��汾
	if (CheckPlazaVersion(pLogonGameID->cbDeviceType,pLogonGameID->dwPlazaVersion,dwSocketID)==false) return true;

	//��������
	DBR_MB_LogonGameID LogonGameID;
	ZeroMemory(&LogonGameID,sizeof(LogonGameID));

	//������Ϣ
	LogonGameID.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	LogonGameID.dwGameID=pLogonGameID->dwGameID;
	LogonGameID.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(LogonGameID.szPassword,pLogonGameID->szPassword,CountArray(LogonGameID.szPassword));
	lstrcpyn(LogonGameID.szMachineID,pLogonGameID->szMachineID,CountArray(LogonGameID.szMachineID));
	lstrcpyn(LogonGameID.szMobilePhone,pLogonGameID->szMobilePhone,CountArray(LogonGameID.szMobilePhone));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_GAMEID,dwSocketID,&LogonGameID,sizeof(LogonGameID));

	return true;
}

//�ʺŵ�¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_LogonAccounts));
	if (wDataSize<sizeof(CMD_MB_LogonAccounts)) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_MB_LogonAccounts * pLogonAccounts=(CMD_MB_LogonAccounts *)pData;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szPassword[CountArray(pLogonAccounts->szPassword)-1]=0;
	pLogonAccounts->szMachineID[CountArray(pLogonAccounts->szMachineID)-1]=0;
	pLogonAccounts->szMobilePhone[CountArray(pLogonAccounts->szMobilePhone)-1]=0;

	//��������
//	pBindParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pBindParameter->wModuleID=pLogonAccounts->wModuleID;
	pBindParameter->dwPlazaVersion=pLogonAccounts->dwPlazaVersion;
	pBindParameter->wPlatformID = DeviceTypeToPlatformID(pLogonAccounts->cbDeviceType);

	//�汾�ж�
	if (CheckPlazaVersion(pLogonAccounts->cbDeviceType,pLogonAccounts->dwPlazaVersion,dwSocketID)==false) return true;

	//��������
	DBR_MB_LogonAccounts LogonAccounts;
	ZeroMemory(&LogonAccounts,sizeof(LogonAccounts));

	//������Ϣ
	LogonAccounts.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	LogonAccounts.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(LogonAccounts.szAccounts,pLogonAccounts->szAccounts,CountArray(LogonAccounts.szAccounts));
	lstrcpyn(LogonAccounts.szPassword,pLogonAccounts->szPassword,CountArray(LogonAccounts.szPassword));
	lstrcpyn(LogonAccounts.szMachineID,pLogonAccounts->szMachineID,CountArray(LogonAccounts.szMachineID));
	lstrcpyn(LogonAccounts.szMobilePhone,pLogonAccounts->szMobilePhone,CountArray(LogonAccounts.szMobilePhone));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_ACCOUNTS,dwSocketID,&LogonAccounts,sizeof(LogonAccounts));

	return true;
}

//�ʺ�ע��
bool CAttemperEngineSink::OnTCPNetworkSubMBRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_RegisterAccounts));
	if (wDataSize<sizeof(CMD_MB_RegisterAccounts)) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_MB_RegisterAccounts * pRegisterAccounts=(CMD_MB_RegisterAccounts *)pData;
	pRegisterAccounts->szAccounts[CountArray(pRegisterAccounts->szAccounts)-1]=0;
	pRegisterAccounts->szNickName[CountArray(pRegisterAccounts->szNickName)-1]=0;
	pRegisterAccounts->szMachineID[CountArray(pRegisterAccounts->szMachineID)-1]=0;
	pRegisterAccounts->szLogonPass[CountArray(pRegisterAccounts->szLogonPass)-1]=0;
	pRegisterAccounts->szInsurePass[CountArray(pRegisterAccounts->szInsurePass)-1]=0;
	pRegisterAccounts->szMobilePhone[CountArray(pRegisterAccounts->szMobilePhone)-1]=0;

	//��������
//	pBindParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pBindParameter->wModuleID=pRegisterAccounts->wModuleID;
	pBindParameter->dwPlazaVersion=pRegisterAccounts->dwPlazaVersion;
	pBindParameter->wPlatformID = DeviceTypeToPlatformID(pRegisterAccounts->cbDeviceType);

	//Ч��汾
	if (CheckPlazaVersion(pRegisterAccounts->cbDeviceType,pRegisterAccounts->dwPlazaVersion,dwSocketID)==false) return true;

	//��������
	DBR_MB_RegisterAccounts RegisterAccounts;
	ZeroMemory(&RegisterAccounts,sizeof(RegisterAccounts));

	//������Ϣ
	RegisterAccounts.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));

	//��������
	RegisterAccounts.wFaceID=pRegisterAccounts->wFaceID;
	RegisterAccounts.cbGender=pRegisterAccounts->cbGender;
	RegisterAccounts.dwAreaID = pRegisterAccounts->dwAreaID;
	RegisterAccounts.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(RegisterAccounts.szAccounts,pRegisterAccounts->szAccounts,CountArray(RegisterAccounts.szAccounts));
	lstrcpyn(RegisterAccounts.szNickName,pRegisterAccounts->szNickName,CountArray(RegisterAccounts.szNickName));
	lstrcpyn(RegisterAccounts.szMachineID,pRegisterAccounts->szMachineID,CountArray(RegisterAccounts.szMachineID));
	lstrcpyn(RegisterAccounts.szLogonPass,pRegisterAccounts->szLogonPass,CountArray(RegisterAccounts.szLogonPass));
	lstrcpyn(RegisterAccounts.szInsurePass,pRegisterAccounts->szInsurePass,CountArray(RegisterAccounts.szInsurePass));
	lstrcpyn(RegisterAccounts.szMobilePhone,pRegisterAccounts->szMobilePhone,CountArray(RegisterAccounts.szMobilePhone));

	LOGI("CAttemperEngineSink::OnTCPNetworkSubMBRegisterAccounts szAccounts=" << pRegisterAccounts->szAccounts << ", dwAreaID=" << pRegisterAccounts->dwAreaID);

	// �������ͷ��
	CRandom random;
	RegisterAccounts.wFaceID = random.Random(8);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REGISTER_ACCOUNTS,dwSocketID,&RegisterAccounts,sizeof(RegisterAccounts));

	return true;
}

// �����ֵ����
bool CAttemperEngineSink::OnTCPNetworkSubMBRechargeMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_RECHARGE_MESSAGE,dwSocketID,NULL,NULL);

	return true;
}

// ����ͷ�����
bool CAttemperEngineSink::OnTCPNetworkSubMBServiceMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_CUSTOM_SERVICE_MESSAGE,dwSocketID,NULL,NULL);

	return true;
}

// ��������¼
bool CAttemperEngineSink::OnTCPNetworkSubLogonThirdParty(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	LOGI("CAttemperEngineSink::OnTCPNetworkSubLogonThirdParty");

	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_LogonAccounts_ThirdParty));
	if (wDataSize<sizeof(CMD_LogonAccounts_ThirdParty)) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//������Ϣ
	CMD_LogonAccounts_ThirdParty * pThirdPartyAccounts=(CMD_LogonAccounts_ThirdParty *)pData;
	int i = CountArray(pThirdPartyAccounts->szSessionID);
	pThirdPartyAccounts->szSessionID[CountArray(pThirdPartyAccounts->szSessionID) - 1] = 0;
	pThirdPartyAccounts->szNickName[CountArray(pThirdPartyAccounts->szNickName) - 1] = 0;
	pThirdPartyAccounts->szMachineID[CountArray(pThirdPartyAccounts->szMachineID) - 1] = 0;

	LOGI("CAttemperEngineSink::OnTCPNetworkSubLogonThirdParty, szSessionID=" << pThirdPartyAccounts->szSessionID << ", szNickName=" << pThirdPartyAccounts->szNickName << \
		", dwAreaID=" << pThirdPartyAccounts->dwAreaID);

	//��������
//	pBindParameter->cbClientKind=pThirdPartyAccounts->cbDeviceType;
	pBindParameter->dwPlazaVersion=pThirdPartyAccounts->dwPlazaVersion;
	pBindParameter->wPlatformID = DeviceTypeToPlatformID(pThirdPartyAccounts->cbDeviceType);

	//Ч��汾
	if (CheckPlazaVersion(pThirdPartyAccounts->cbDeviceType,pThirdPartyAccounts->dwPlazaVersion,dwSocketID)==false) return true;

	//��������
 	DBR_TP_LogonAccounts LogonAccounts;
 	ZeroMemory(&LogonAccounts,sizeof(LogonAccounts));
 
 	//������Ϣ
 	LogonAccounts.pBindParameter=(m_pBindParameter+LOWORD(dwSocketID));
 
 	//��������
 	LogonAccounts.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;
	LogonAccounts.nThirdPartyID=pThirdPartyAccounts->nThirdPartyID;
	LogonAccounts.dwAreaID = pThirdPartyAccounts->dwAreaID;
 	lstrcpyn(LogonAccounts.szSessionID,pThirdPartyAccounts->szSessionID,CountArray(LogonAccounts.szSessionID));
	lstrcpyn(LogonAccounts.szNickName,pThirdPartyAccounts->szNickName,CountArray(LogonAccounts.szNickName));
	lstrcpyn(LogonAccounts.szMachineID, pThirdPartyAccounts->szMachineID, CountArray(LogonAccounts.szMachineID));

	// �������ͷ��
	CRandom random;
	LogonAccounts.wFaceID = random.Random(8);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_TP_LOGON_ACCOUNTS,dwSocketID,&LogonAccounts,sizeof(LogonAccounts));

	return true;
}

//������Ϸ��
bool CAttemperEngineSink::OnTCPNetworkUserSaveScore(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GP_UserSaveScore));
	if (wDataSize != sizeof(CMD_GP_UserSaveScore)) return false;

	//������Ϣ
	CMD_GP_UserSaveScore * pUserSaveScore = (CMD_GP_UserSaveScore *)pData;
	pUserSaveScore->szMachineID[CountArray(pUserSaveScore->szMachineID) - 1] = 0;

	//��������
	DBR_GP_UserSaveScore UserSaveScore;
	ZeroMemory(&UserSaveScore, sizeof(UserSaveScore));

	//��������
	UserSaveScore.dwUserID = pUserSaveScore->dwUserID;
	UserSaveScore.lSaveScore = pUserSaveScore->lSaveScore;
	UserSaveScore.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(UserSaveScore.szMachineID, pUserSaveScore->szMachineID, CountArray(UserSaveScore.szMachineID));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_SAVE_SCORE, dwSocketID, &UserSaveScore, sizeof(UserSaveScore));

	return true;
}

//ȡ����Ϸ��
bool CAttemperEngineSink::OnTCPNetworkUserTakeScore(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GP_UserTakeScore));
	if (wDataSize != sizeof(CMD_GP_UserTakeScore)) return false;

	//������Ϣ
	CMD_GP_UserTakeScore * pUserTakeScore = (CMD_GP_UserTakeScore *)pData;
	pUserTakeScore->szPassword[CountArray(pUserTakeScore->szPassword) - 1] = 0;
	pUserTakeScore->szMachineID[CountArray(pUserTakeScore->szMachineID) - 1] = 0;

	//��������
	DBR_GP_UserTakeScore UserTakeScore;
	ZeroMemory(&UserTakeScore, sizeof(UserTakeScore));

	//��������
	UserTakeScore.dwUserID = pUserTakeScore->dwUserID;
	UserTakeScore.lTakeScore = pUserTakeScore->lTakeScore;
	UserTakeScore.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(UserTakeScore.szPassword, pUserTakeScore->szPassword, CountArray(UserTakeScore.szPassword));
	lstrcpyn(UserTakeScore.szMachineID, pUserTakeScore->szMachineID, CountArray(UserTakeScore.szMachineID));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_TAKE_SCORE, dwSocketID, &UserTakeScore, sizeof(UserTakeScore));

	return true;
}

// ת����Ϸ��
bool CAttemperEngineSink::OnTCPNetworkUserTransferScore(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GP_UserTransferScore));
	if (wDataSize != sizeof(CMD_GP_UserTransferScore))
	{
		LOGI("CAttemperEngineSink::OnTCPNetworkMainPCUserService, wDataSize=" << wDataSize);
		return false;
	}

	//������Ϣ
	CMD_GP_UserTransferScore * pUserTransferScore = (CMD_GP_UserTransferScore *)pData;
	pUserTransferScore->szNickName[CountArray(pUserTransferScore->szNickName) - 1] = 0;
	pUserTransferScore->szPassword[CountArray(pUserTransferScore->szPassword) - 1] = 0;
	pUserTransferScore->szMachineID[CountArray(pUserTransferScore->szMachineID) - 1] = 0;

	//��������
	DBR_GP_UserTransferScore UserTransferScore;
	ZeroMemory(&UserTransferScore, sizeof(UserTransferScore));

	//��������
	UserTransferScore.dwUserID = pUserTransferScore->dwUserID;
	UserTransferScore.cbByNickName = pUserTransferScore->cbByNickName;
	UserTransferScore.lTransferScore = pUserTransferScore->lTransferScore;
	UserTransferScore.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(UserTransferScore.szNickName, pUserTransferScore->szNickName, CountArray(UserTransferScore.szNickName));
	lstrcpyn(UserTransferScore.szPassword, pUserTransferScore->szPassword, CountArray(UserTransferScore.szPassword));
	lstrcpyn(UserTransferScore.szMachineID, pUserTransferScore->szMachineID, CountArray(UserTransferScore.szMachineID));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_USER_TRANSFER_SCORE, dwSocketID, &UserTransferScore, sizeof(UserTransferScore));

	return true;
}

//��ѯ����
bool CAttemperEngineSink::OnTCPNetworkQueryInsureInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GP_QueryInsureInfo));
	if (wDataSize != sizeof(CMD_GP_QueryInsureInfo)) return false;

	//������Ϣ
	CMD_GP_QueryInsureInfo * pQueryInsureInfo = (CMD_GP_QueryInsureInfo *)pData;

	//��������
	DBR_GP_QueryInsureInfo QueryInsureInfo;
	ZeroMemory(&QueryInsureInfo, sizeof(QueryInsureInfo));

	//��������
	QueryInsureInfo.dwUserID = pQueryInsureInfo->dwUserID;
	QueryInsureInfo.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_INSURE_INFO, dwSocketID, &QueryInsureInfo, sizeof(QueryInsureInfo));

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkQueryNicknameByGameID(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GP_QueryNickNameByGameID));
	if (wDataSize != sizeof(CMD_GP_QueryNickNameByGameID)) return false;

	//������Ϣ
	CMD_GP_QueryNickNameByGameID* pQueryNickNameByGameID = (CMD_GP_QueryNickNameByGameID *)pData;

	//��������
	DBR_GP_QueryNickNameByGameID QueryNickNameByGameID;
	ZeroMemory(&QueryNickNameByGameID, sizeof(QueryNickNameByGameID));

	//��������
	QueryNickNameByGameID.dwGameID = pQueryNickNameByGameID->dwGameID;
	//			QueryIndividual.dwClientAddr=(m_pBindParameter+LOWORD(dwSocketID))->dwClientAddr;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_NICKNAME_BY_GAMEID, dwSocketID, &QueryNickNameByGameID, sizeof(QueryNickNameByGameID));

	return true;
}

// ��֤��������
bool CAttemperEngineSink::OnTCPNetworkVerifyInsurePassword(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	LOGI("CAttemperEngineSink::OnTCPNetworkVerifyInsurePassword SUB_GP_VERIFY_INSURE_PASSWORD");
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GP_VerifyUserInsurePassword));
	if (wDataSize != sizeof(CMD_GP_VerifyUserInsurePassword)) return false;

	//������Ϣ
	CMD_GP_VerifyUserInsurePassword* pVerifyUserInsurePassword = (CMD_GP_VerifyUserInsurePassword *)pData;

	//��������
	DBR_GP_VerifyInsurePassword VerifyInsurePassword;
	ZeroMemory(&VerifyInsurePassword, sizeof(VerifyInsurePassword));

	//��������
	VerifyInsurePassword.dwUserID = pVerifyUserInsurePassword->dwUserID;
	lstrcpyn(VerifyInsurePassword.szInsurePassword, pVerifyUserInsurePassword->szInsurePassword, CountArray(VerifyInsurePassword.szInsurePassword));

	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_VERIFY_INSURE_PASSWORD, dwSocketID, &VerifyInsurePassword, sizeof(VerifyInsurePassword));
	return true;
}

// ��ѯ�����һ���Ϣ
bool CAttemperEngineSink::OnTCPNetworkQueryExchangeLoveliness(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	// ��ѯ�����һ���Ϣ
	LOGI("CAttemperEngineSink::OnTCPNetworkQueryExchangeLoveliness SUB_GP_QUERY_EXCHANGE_LOVELINESS");

	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GP_QueryExchangeLoveliness));
	if (wDataSize != sizeof(CMD_GP_QueryExchangeLoveliness)) return false;

	//������Ϣ
	CMD_GP_QueryExchangeLoveliness* pQueryExchangeLoveliness = (CMD_GP_QueryExchangeLoveliness *)pData;

	//��������
	DBR_GP_QueryExchangeLoveliness QueryExchangeLoveliness;
	ZeroMemory(&QueryExchangeLoveliness, sizeof(QueryExchangeLoveliness));

	//��������
	QueryExchangeLoveliness.dwUserID = pQueryExchangeLoveliness->dwUserID;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_EXCHANGE_LOVELINESS, dwSocketID, &QueryExchangeLoveliness, sizeof(QueryExchangeLoveliness));

	return true;
}

// �һ�����ֵ
bool CAttemperEngineSink::OnTCPNetworkExchangeLoveliness(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	// ����һ�
	LOGI("CAttemperEngineSink::OnTCPNetworkExchangeLoveliness SUB_GP_EXCHANGE_LOVELINESS");

	//������Ϣ
	CMD_GP_ExchangeLoveliness* pExchangeLoveliness = (CMD_GP_ExchangeLoveliness *)pData;

	//��������
	DBR_GP_ExchangeLoveliness ExchangeLoveliness;
	ZeroMemory(&ExchangeLoveliness, sizeof(ExchangeLoveliness));

	//��������
	ExchangeLoveliness.dwUserID = pExchangeLoveliness->dwUserID;
	ExchangeLoveliness.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_EXCHANGE_LOVELINESS, dwSocketID, &ExchangeLoveliness, sizeof(ExchangeLoveliness));

	return true;
}

// ����ɹ�
bool CAttemperEngineSink::OnTCPNetworkMBShareSuccess(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	LOGI("CAttemperEngineSink::OnTCPNetworkMainPCUserService SUB_MB_SHARE_SUCCESS");
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_MB_ShareSuccess));
	if (wDataSize != sizeof(CMD_MB_ShareSuccess))
	{
		return false;
	}

	//������Ϣ
	CMD_MB_ShareSuccess* pShareSuccess = (CMD_MB_ShareSuccess*)pData;

	// ��������
	DBR_MB_ShareSuccess	DBRShareSucces;
	memset(&DBRShareSucces, 0, sizeof(DBRShareSucces));
	DBRShareSucces.dwUserID = pShareSuccess->dwUserID;
	lstrcpyn(DBRShareSucces.szMachineID, pShareSuccess->szMachineID, CountArray(DBRShareSucces.szMachineID));
	DBRShareSucces.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;
	DBRShareSucces.pBindParameter = (m_pBindParameter + LOWORD(dwSocketID));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_SHARE_SUCCESS, dwSocketID, &DBRShareSucces, sizeof(DBRShareSucces));

	return true;
}

// ��������
bool CAttemperEngineSink::OnTCPNetworkUserCreateCustomTable(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	// �û��Զ�������
	LOGI("CAttemperEngineSink::OnTCPNetworkUserCreateCustomTable SUB_MB_USER_CUSTOM_TABLE");

	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_MB_UserCustomTable));
	if (wDataSize != sizeof(CMD_MB_UserCustomTable))
	{
		LOGI("CAttemperEngineSink::OnTCPNetworkUserCreateCustomTable SUB_MB_USER_CUSTOM_TABLE ��Ϣ���Ȳ�ƥ��");
		return false;
	}

	//������Ϣ
	CMD_MB_UserCustomTable* pUserCustomTable = (CMD_MB_UserCustomTable*)pData;

	// ��������
	CMD_CS_C_UserCustomTable	CSUserCustomTable;
	memset(&CSUserCustomTable, 0, sizeof(CMD_CS_C_UserCustomTable));
	CSUserCustomTable.dwUserID = pUserCustomTable->dwUserID;
	CSUserCustomTable.wKindID = pUserCustomTable->wKindID;										// ��������
	CSUserCustomTable.wCardCousumeCount = pUserCustomTable->wCardCousumeCount;					// ���ĵķ�����Ŀ
	memcpy(CSUserCustomTable.cbCustomRule, pUserCustomTable->cbCustomRule, sizeof(CSUserCustomTable.cbCustomRule));	// �Զ�����
	CSUserCustomTable.dwSocketID = dwSocketID;

	// ת��Э��������
	bool flag = m_pITCPSocketService->SendData(MDM_CS_REMOTE_SERVICE, SUB_CS_C_USER_CUSTOM_TABLE, &CSUserCustomTable, sizeof(CSUserCustomTable));
	if (flag == false)
	{
		// ������Ϣ��Э��������ʧ��
		CMD_MB_UserCustomTableFailed UserCustomTableFailed;
		lstrcpyn(UserCustomTableFailed.szDescribeString, TEXT("�޷�����Э��������,������Ϸ����ʧ��!"), CountArray(UserCustomTableFailed.szDescribeString));
//		UserCustomTableFailed.bLockedInTable = false;

		//��������
		WORD wStringSize = CountStringBuffer(UserCustomTableFailed.szDescribeString);
		WORD wSendSize = sizeof(UserCustomTableFailed) - sizeof(UserCustomTableFailed.szDescribeString) + wStringSize;
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GP_USER_SERVICE, SUB_MB_USER_CUSTOM_TABLE_FAILED, &UserCustomTableFailed, wSendSize);
	}

	return true;
}

// �����Խ�����
bool CAttemperEngineSink::OnTCPNetworkUserJoinCustomTable(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	// �û������Զ�������
	LOGI("CAttemperEngineSink::OnTCPNetworkUserJoinCustomTable");

	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_MB_UserJoinCustomTable));
	if (wDataSize != sizeof(CMD_MB_UserJoinCustomTable))
	{
		LOGI("CAttemperEngineSink::OnTCPNetworkUserJoinCustomTable ��Ϣ���Ȳ�ƥ��");
		return false;
	}
	CMD_MB_UserJoinCustomTable* pUserJoinCustomTable = (CMD_MB_UserJoinCustomTable*)pData;

	//Ͷ������
	DBR_MB_UserJoinCustomTable UserJoinCustomTable;
	UserJoinCustomTable.dwUserID = pUserJoinCustomTable->dwUserID;
	lstrcpyn(UserJoinCustomTable.szSecret, pUserJoinCustomTable->szSecret, CountArray(UserJoinCustomTable.szSecret));
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_USER_JOIN_CUSTOMTABLE, dwSocketID, &UserJoinCustomTable, sizeof(UserJoinCustomTable));

	return true;
}

// ��ѯ������Խ�����
bool CAttemperEngineSink::OnTCPNetworkUserQueryCustomTable(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	LOGI("CAttemperEngineSink::OnTCPNetworkUserQueryCustomTable ");

	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_MB_UserQueryCustomTable));
	if (wDataSize != sizeof(CMD_MB_UserQueryCustomTable))
	{
		LOGI("CAttemperEngineSink::OnTCPNetworkUserQueryCustomTable ��Ϣ���Ȳ�ƥ��");
		return false;
	}
	CMD_MB_UserQueryCustomTable* pUserQueryCustomTable = (CMD_MB_UserQueryCustomTable*)pData;
	
	// Ͷ�ݸ����ݿ�
	DBR_MB_UserQueryCustomTable UserQueryCustomTable;
	UserQueryCustomTable.dwUserID = pUserQueryCustomTable->dwUserID;
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_USER_QUERY_CUSTOMTABLE, dwSocketID, &UserQueryCustomTable, sizeof(UserQueryCustomTable));

	return true;
}

// �û�ת����ʯ
bool CAttemperEngineSink::OnTCPNetworkUserTransferDiamond(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	LOGI("CAttemperEngineSink::OnTCPNetworkUserTransferDiamond ");

	//������Ϣ
	CMD_MB_UserTransferDiamond * pUserTransferDiamond = (CMD_MB_UserTransferDiamond *)pData;
	pUserTransferDiamond->szPassword[CountArray(pUserTransferDiamond->szPassword) - 1] = 0;
	pUserTransferDiamond->szMachineID[CountArray(pUserTransferDiamond->szMachineID) - 1] = 0;

	//��������
	DBR_MB_UserTransferDiamond UserTransferDiamond;
	ZeroMemory(&UserTransferDiamond, sizeof(UserTransferDiamond));

	//��������
	UserTransferDiamond.dwUserID = pUserTransferDiamond->dwUserID;
	UserTransferDiamond.dwGameID = pUserTransferDiamond->dwGameID;
	UserTransferDiamond.dwTransferDiamond = pUserTransferDiamond->dwTransferDiamond;
	UserTransferDiamond.dwClientAddr = (m_pBindParameter + LOWORD(dwSocketID))->dwClientAddr;
	lstrcpyn(UserTransferDiamond.szPassword, pUserTransferDiamond->szPassword, CountArray(UserTransferDiamond.szPassword));
	lstrcpyn(UserTransferDiamond.szMachineID, pUserTransferDiamond->szMachineID, CountArray(UserTransferDiamond.szMachineID));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_USER_TRANSFER_DIAMOND, dwSocketID, &UserTransferDiamond, sizeof(UserTransferDiamond));

	return true;
}

//��¼�ɹ�
bool CAttemperEngineSink::OnDBPCLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	LOGI("CAttemperEngineSink::OnDBPCLogonSuccess, Begin="<<GetTickCount());
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	DBO_GP_LogonSuccess * pDBOLogonSuccess=(DBO_GP_LogonSuccess *)pData;
	CMD_GP_LogonSuccess * pCMDLogonSuccess=(CMD_GP_LogonSuccess *)cbDataBuffer;

	//���Ͷ���
	WORD wHeadSize=sizeof(CMD_GP_LogonSuccess);
	CSendPacketHelper SendPacket(cbDataBuffer+wHeadSize,sizeof(cbDataBuffer)-wHeadSize);

	//���ñ���
	ZeroMemory(pCMDLogonSuccess,sizeof(CMD_GP_LogonSuccess));

	//��������
	pCMDLogonSuccess->wFaceID=pDBOLogonSuccess->wFaceID;
	pCMDLogonSuccess->cbGender=pDBOLogonSuccess->cbGender;
	pCMDLogonSuccess->dwGameID=pDBOLogonSuccess->dwGameID;
	pCMDLogonSuccess->dwUserID=pDBOLogonSuccess->dwUserID;
	pCMDLogonSuccess->dwCustomID=pDBOLogonSuccess->dwCustomID;
	pCMDLogonSuccess->dwUserMedal=pDBOLogonSuccess->dwUserMedal;
	pCMDLogonSuccess->dwExperience=pDBOLogonSuccess->dwExperience;
	pCMDLogonSuccess->dwLoveLiness=pDBOLogonSuccess->dwLoveLiness;
	pCMDLogonSuccess->cbMoorMachine=pDBOLogonSuccess->cbMoorMachine;
	lstrcpyn(pCMDLogonSuccess->szAccounts,pDBOLogonSuccess->szAccounts,CountArray(pCMDLogonSuccess->szAccounts));
	lstrcpyn(pCMDLogonSuccess->szNickName,pDBOLogonSuccess->szNickName,CountArray(pCMDLogonSuccess->szNickName));

	//�û���Ϸ��
	pCMDLogonSuccess->lUserScore=pDBOLogonSuccess->lUserScore;
	pCMDLogonSuccess->lUserInsure=pDBOLogonSuccess->lUserInsure;

	//������Ϣ
	pCMDLogonSuccess->cbShowServerStatus=m_bShowServerStatus?1:0;

	//��Ա��Ϣ
	if (pDBOLogonSuccess->cbMemberOrder!=0)
	{
		DTP_GP_MemberInfo MemberInfo;
		ZeroMemory(&MemberInfo,sizeof(MemberInfo));
		MemberInfo.cbMemberOrder=pDBOLogonSuccess->cbMemberOrder;
		MemberInfo.MemberOverDate=pDBOLogonSuccess->MemberOverDate;
		SendPacket.AddPacket(&MemberInfo,sizeof(MemberInfo),DTP_GP_MEMBER_INFO);
	}
	LOGI("CAttemperEngineSink::OnDBPCLogonSuccess, Step 1="<<GetTickCount());

	//����ǩ��
	if (pDBOLogonSuccess->szUnderWrite[0]!=0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szUnderWrite,CountStringBuffer(pDBOLogonSuccess->szUnderWrite),DTP_GP_UNDER_WRITE);
	}
	LOGI("CAttemperEngineSink::OnDBPCLogonSuccess, Step 2="<<GetTickCount());

	pCMDLogonSuccess->wFillIn = pDBOLogonSuccess->wFillIn;
	//��¼�ɹ�
	WORD wSendSize=SendPacket.GetDataSize()+sizeof(CMD_GP_LogonSuccess);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_SUCCESS,cbDataBuffer,wSendSize);

	LOGI("CAttemperEngineSink::OnDBPCLogonSuccess, Step 3="<<GetTickCount());

	//�����б�
	if(pCMDLogonSuccess->wFillIn > 0)	// add by chenj
	{
		if (m_pInitParameter->m_cbDelayList==TRUE)
		{
			//�����б�
			SendGameTypeInfo(dwContextID);
			SendGameKindInfo(dwContextID);
			SendGamePageInfo(dwContextID,INVALID_WORD);
			SendGameNodeInfo(dwContextID,INVALID_WORD);
			SendGameServerInfo(dwContextID,INVALID_WORD);
			m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH);
			LOGI("CAttemperEngineSink::OnDBPCLogonSuccess, Step 4="<<GetTickCount());
		}
		else
		{
			SendGameTypeInfo(dwContextID);
			SendGameKindInfo(dwContextID);
			SendGamePageInfo(dwContextID,0);
			m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH);
			LOGI("CAttemperEngineSink::OnDBPCLogonSuccess, Step 5="<<GetTickCount());
		}
	}

	//��¼���
	CMD_GP_LogonFinish LogonFinish;
	ZeroMemory(&LogonFinish,sizeof(LogonFinish));
	LogonFinish.wIntermitTime=m_pInitParameter->m_wIntermitTime;
	LogonFinish.wOnLineCountTime=m_pInitParameter->m_wOnLineCountTime;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_FINISH,&LogonFinish,sizeof(LogonFinish));

	LOGI("CAttemperEngineSink::OnDBPCLogonSuccess, End="<<GetTickCount());

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBPCLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GP_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));
	DBO_GP_LogonFailure * pLogonFailure=(DBO_GP_LogonFailure *)pData;

	//��������
	LogonFailure.lResultCode=pLogonFailure->lResultCode;
	lstrcpyn(LogonFailure.szDescribeString,pLogonFailure->szDescribeString,CountArray(LogonFailure.szDescribeString));

	//��������
	WORD wStringSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wSendSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString)+wStringSize;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_FAILURE,&LogonFailure,wSendSize);

	//�ر�����(�ײ�error,��ʱ���ر�����)
//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBPCLogonValidateMBCard(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GP_ValidateMBCard));
	if (wDataSize!=sizeof(DBR_GP_ValidateMBCard)) return false;

	DBR_GP_ValidateMBCard *pValidateMBCard=(DBR_GP_ValidateMBCard *)pData;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_VALIDATE_MBCARD,pData,wDataSize);

	return true;
}

// ��������
bool CAttemperEngineSink::OnDBPCProfileSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_UserProfile * pUserProfile=(DBO_GP_UserProfile *)pData;

	//��������
	CMD_GP_UserProfile UserProfile;
	ZeroMemory(&UserProfile,sizeof(UserProfile));

	//��������
	UserProfile.dwUserID = pUserProfile->dwUserID;
	UserProfile.cbGender = pUserProfile->cbGender;
	UserProfile.wFaceID = pUserProfile->wFaceID;

	lstrcpyn(UserProfile.szNickName,pUserProfile->szNickName,CountArray(UserProfile.szNickName));
	lstrcpyn(UserProfile.szQQ,pUserProfile->szQQ,CountArray(UserProfile.szQQ));
	lstrcpyn(UserProfile.szMobile,pUserProfile->szMobile,CountArray(UserProfile.szMobile));
	lstrcpyn(UserProfile.szUnderWrite,pUserProfile->szUnderWrite,CountArray(UserProfile.szUnderWrite));

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_PROFILE_SUCCESS,&UserProfile,sizeof(UserProfile));


	//�����б�
	if (m_pInitParameter->m_cbDelayList==TRUE)
	{
		//�����б�
		SendGameTypeInfo(dwContextID);
		SendGameKindInfo(dwContextID);
		SendGamePageInfo(dwContextID,INVALID_WORD);
		SendGameNodeInfo(dwContextID,INVALID_WORD);
		SendGameServerInfo(dwContextID,INVALID_WORD);
		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH);
	}
	else
	{
		SendGameTypeInfo(dwContextID);
		SendGameKindInfo(dwContextID);
		SendGamePageInfo(dwContextID,0);
		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH);
	}

	return true;
}

//��������ʧ��
bool CAttemperEngineSink::OnDBPCProfileFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GP_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));
	DBO_GP_LogonFailure * pLogonFailure=(DBO_GP_LogonFailure *)pData;

	//��������
	LogonFailure.lResultCode=pLogonFailure->lResultCode;
	lstrcpyn(LogonFailure.szDescribeString,pLogonFailure->szDescribeString,CountArray(LogonFailure.szDescribeString));

	//��������
	WORD wStringSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wSendSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString)+wStringSize;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_PROFILE_FAILURE,&LogonFailure,wSendSize);

	//�ر�����(�ײ�error,��ʱ���ر�����)
//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//����ǳ�
bool CAttemperEngineSink::OnDBPCCheckNick(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_CheckNick * pCheckNick=(DBO_GP_CheckNick *)pData;

	//��������
	CMD_GP_CheckNick_Res CheckNickRes;
	ZeroMemory(&CheckNickRes,sizeof(CheckNickRes));

	//��������
	CheckNickRes.lResultCode=pCheckNick->lResultCode;
	lstrcpyn(CheckNickRes.szDescribeString,pCheckNick->szDescribeString,CountArray(CheckNickRes.szDescribeString));

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_LOGON,SUB_GP_CHECKNICK_RES,&CheckNickRes,sizeof(CheckNickRes));

	return true;
}
//�û�ͷ��
bool CAttemperEngineSink::OnDBPCUserFaceInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GP_UserFaceInfo UserFaceInfo;
	ZeroMemory(&UserFaceInfo,sizeof(UserFaceInfo));
	DBO_GP_UserFaceInfo * pUserFaceInfo=(DBO_GP_UserFaceInfo *)pData;

	//���ñ���
	UserFaceInfo.wFaceID=pUserFaceInfo->wFaceID;
	UserFaceInfo.dwCustomID=pUserFaceInfo->dwCustomID;

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_USER_FACE_INFO,&UserFaceInfo,sizeof(UserFaceInfo));

	return true;
}

//�û���Ϣ
bool CAttemperEngineSink::OnDBPCUserIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	DBO_GP_UserIndividual * pDBOUserIndividual=(DBO_GP_UserIndividual *)pData;
	CMD_GP_UserIndividual * pCMDUserIndividual=(CMD_GP_UserIndividual *)cbDataBuffer;
	CSendPacketHelper SendPacket(cbDataBuffer+sizeof(CMD_GP_UserIndividual),sizeof(cbDataBuffer)-sizeof(CMD_GP_UserIndividual));

	//���ñ���
	ZeroMemory(pCMDUserIndividual,sizeof(CMD_GP_UserIndividual));

	//��������
	pCMDUserIndividual->dwUserID=pDBOUserIndividual->dwUserID;

	//�û���Ϣ
	if (pDBOUserIndividual->szUserNote[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szUserNote);
		SendPacket.AddPacket(pDBOUserIndividual->szUserNote,wBufferSize,DTP_GP_UI_USER_NOTE);
	}

	//��ʵ����
	if (pDBOUserIndividual->szCompellation[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szCompellation);
		SendPacket.AddPacket(pDBOUserIndividual->szCompellation,wBufferSize,DTP_GP_UI_COMPELLATION);
	}

	//�绰����
	if (pDBOUserIndividual->szSeatPhone[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szSeatPhone);
		SendPacket.AddPacket(pDBOUserIndividual->szSeatPhone,wBufferSize,DTP_GP_UI_SEAT_PHONE);
	}

	//�ƶ��绰
	if (pDBOUserIndividual->szMobilePhone[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szMobilePhone);
		SendPacket.AddPacket(pDBOUserIndividual->szMobilePhone,wBufferSize,DTP_GP_UI_MOBILE_PHONE);
	}

	//��ϵ����
	if (pDBOUserIndividual->szQQ[0]!=0) 
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szQQ);
		SendPacket.AddPacket(pDBOUserIndividual->szQQ,wBufferSize,DTP_GP_UI_QQ);
	}

	//�����ʼ�
	if (pDBOUserIndividual->szEMail[0]!=0) 
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szEMail);
		SendPacket.AddPacket(pDBOUserIndividual->szEMail,wBufferSize,DTP_GP_UI_EMAIL);
	}

	//��ϵ��ַ
	if (pDBOUserIndividual->szDwellingPlace[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szDwellingPlace);
		SendPacket.AddPacket(pDBOUserIndividual->szDwellingPlace,wBufferSize,DTP_GP_UI_DWELLING_PLACE);
	}

	//������Ϣ
	WORD wSendSize=sizeof(CMD_GP_UserIndividual)+SendPacket.GetDataSize();
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_USER_INDIVIDUAL,cbDataBuffer,wSendSize);

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBPCUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_UserInsureInfo * pUserInsureInfo=(DBO_GP_UserInsureInfo *)pData;

	//��������
	CMD_GP_UserInsureInfo UserInsureInfo;
	ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

	//��������
	UserInsureInfo.wRevenueTake=pUserInsureInfo->wRevenueTake;
	UserInsureInfo.wRevenueTransfer=pUserInsureInfo->wRevenueTransfer;
	UserInsureInfo.wServerID=pUserInsureInfo->wServerID;
	UserInsureInfo.lUserScore=pUserInsureInfo->lUserScore;
	UserInsureInfo.lUserInsure=pUserInsureInfo->lUserInsure;
	UserInsureInfo.lTransferPrerequisite=pUserInsureInfo->lTransferPrerequisite;
	UserInsureInfo.dwDiamondCount = pUserInsureInfo->dwDiamondCount;

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_USER_INSURE_INFO,&UserInsureInfo,sizeof(UserInsureInfo));

	//�ر�����(�ײ�error,��ʱ��ȥ�ر�)
//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}
//ת�˼�¼
bool CAttemperEngineSink::OnDBPCUserTransRecord(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
    ASSERT(wDataSize==sizeof(DBR_GP_UserTransRecord));
	if (wDataSize!=sizeof(DBR_GP_UserTransRecord)) return false;

	DBR_GP_UserTransRecord *pTransRecord=(DBR_GP_UserTransRecord *)pData;
    
	CMD_GP_RealTransRecord RealTransRecord;
	ZeroMemory(&RealTransRecord,sizeof(RealTransRecord));
	RealTransRecord.dwGameID = pTransRecord->dwGameID;
	_tcscpy(RealTransRecord.szSourceAccount,pTransRecord->szSourceAccount);
	_tcscpy(RealTransRecord.szTargetAccounts,pTransRecord->szTargetAccounts);
	_tcscpy(RealTransRecord.szTime,pTransRecord->szTime);
	RealTransRecord.lSwapScore = pTransRecord->lSwapScore;
    RealTransRecord.lRevenue=pTransRecord->lRevenue;
    RealTransRecord.bOver = pTransRecord->bOver;
	RealTransRecord.dwTargetID = pTransRecord->dwTargetID;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_TRANS_RECORD,&RealTransRecord,sizeof(RealTransRecord));
    if(pTransRecord->bOver)
    {
         //�ر�����(�ײ�error,��ʱ��ȥ�ر�)
//         m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
    }
    return true;
}
//���гɹ�
bool CAttemperEngineSink::OnDBPCUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_UserInsureSuccess * pUserInsureSuccess=(DBO_GP_UserInsureSuccess *)pData;

	//��������
	CMD_GP_UserInsureSuccess UserInsureSuccess;
	ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

	//���ñ���
	UserInsureSuccess.dwUserID=pUserInsureSuccess->dwUserID;
	UserInsureSuccess.lUserScore=pUserInsureSuccess->lSourceScore+pUserInsureSuccess->lVariationScore;
	UserInsureSuccess.lUserInsure=pUserInsureSuccess->lSourceInsure+pUserInsureSuccess->lVariationInsure;
	lstrcpyn(UserInsureSuccess.szDescribeString,pUserInsureSuccess->szDescribeString,CountArray(UserInsureSuccess.szDescribeString));

	//������Ϣ
	WORD wDescribe=CountStringBuffer(UserInsureSuccess.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_USER_INSURE_SUCCESS,&UserInsureSuccess,wHeadSize+wDescribe);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBPCUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GP_UserInsureFailure UserInsureFailure;
	ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

	//��������
	DBO_GP_UserInsureFailure * pUserInsureFailure=(DBO_GP_UserInsureFailure *)pData;

	//��������
	UserInsureFailure.lResultCode=pUserInsureFailure->lResultCode;
	lstrcpyn(UserInsureFailure.szDescribeString,pUserInsureFailure->szDescribeString,CountArray(UserInsureFailure.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(UserInsureFailure.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_USER_INSURE_FAILURE,&UserInsureFailure,wHeadSize+wDescribe);

	//�ر�����(�ײ�error,��ʱ��ȥ�ر�)
//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//�û���Ϣ
bool CAttemperEngineSink::OnDBPCUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_GP_UserTransferUserInfo * pTransferUserInfo=(DBO_GP_UserTransferUserInfo *)pData;

	//��������
	CMD_GP_UserTransferUserInfo UserTransferUserInfo;
	ZeroMemory(&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	//�������
	UserTransferUserInfo.dwTargetGameID=pTransferUserInfo->dwGameID;
	lstrcpyn(UserTransferUserInfo.szNickName,pTransferUserInfo->szNickName,CountArray(UserTransferUserInfo.szNickName));

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_QUERY_USER_INFO_RESULT,&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	return true;
}

//�����ɹ�
bool CAttemperEngineSink::OnDBPCOperateSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GP_OperateSuccess OperateSuccess;
	ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

	//��������
	DBO_GP_OperateSuccess * pOperateSuccess=(DBO_GP_OperateSuccess *)pData;

	//��������
	OperateSuccess.lResultCode=pOperateSuccess->lResultCode;
	lstrcpyn(OperateSuccess.szDescribeString,pOperateSuccess->szDescribeString,CountArray(OperateSuccess.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(OperateSuccess.szDescribeString);
	WORD wHeadSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_OPERATE_SUCCESS,&OperateSuccess,wHeadSize+wDescribe);

	//�ر�����(�ײ�error,��ʱ���ر�����)
//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBPCOperateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GP_OperateFailure OperateFailure;
	ZeroMemory(&OperateFailure,sizeof(OperateFailure));

	//��������
	DBO_GP_OperateFailure * pOperateFailure=(DBO_GP_OperateFailure *)pData;

	//��������
	OperateFailure.lResultCode=pOperateFailure->lResultCode;
	lstrcpyn(OperateFailure.szDescribeString,pOperateFailure->szDescribeString,CountArray(OperateFailure.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(OperateFailure.szDescribeString);
	WORD wHeadSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_OPERATE_FAILURE,&OperateFailure,wHeadSize+wDescribe);

	//�ر�����(�ײ�error,��ʱ���ر�����)
//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

// ת�˻�ִ
bool CAttemperEngineSink::OnDBPCUserTransReceipt(DWORD dwContextID,VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GR_UserTransferReceipt UserTransferReceipt;
	ZeroMemory(&UserTransferReceipt,sizeof(UserTransferReceipt));

	//��������
	CMD_GR_UserTransferReceipt * pUserTransferReceipt=(CMD_GR_UserTransferReceipt *)pData;

	CopyMemory(&UserTransferReceipt, pUserTransferReceipt, wDataSize);

	LOGI("CAttemperEngineSink::OnDBUserTransferReceipt, dwRecordID="<<UserTransferReceipt.dwRecordID<<", dwSourceUserID="<<UserTransferReceipt.dwSourceUserID<<\
		", dwTargetUserID="<<UserTransferReceipt.dwTargetUserID<<", dwSourceGameID="<<UserTransferReceipt.dwSourceGameID<<", dwTargetGameID="<<UserTransferReceipt.dwTargetGameID<<\
		", szSourceNickName="<<UserTransferReceipt.szSourceNickName<<", szTargetNickName="<<UserTransferReceipt.szTargetNickName<<\
		", lTransferScore="<<UserTransferReceipt.lTransferScore<<", lRevenueScore="<<UserTransferReceipt.lRevenueScore);

	//��������
//	OperateSuccess.lResultCode=pOperateSuccess->lResultCode;
//	lstrcpyn(OperateSuccess.szDescribeString,pOperateSuccess->szDescribeString,CountArray(OperateSuccess.szDescribeString));

	//��������
//	WORD wDescribe=CountStringBuffer(OperateSuccess.szDescribeString);
//	WORD wHeadSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE, SUB_GP_USER_TRANSFER_RECEIPT, &UserTransferReceipt,sizeof(UserTransferReceipt));

	//�ر�����(�ײ�error,��ʱ���ر�����)
//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

// ��ȡ�����һ����óɹ�
bool CAttemperEngineSink::OnDBPExchangeLovelinessInfo(DWORD dwContextID,VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GP_ExchangeLovelinessInfo ExchangeLovelinessInfo;
	ZeroMemory(&ExchangeLovelinessInfo,sizeof(ExchangeLovelinessInfo));

	//��������
	DBO_GP_ExchangeLovelinessInfo * pExchangeLovelinessInfo=(DBO_GP_ExchangeLovelinessInfo *)pData;

	// ��������
	ExchangeLovelinessInfo.dwUserID = pExchangeLovelinessInfo->dwUserID;
	ExchangeLovelinessInfo.dwLoveliness = pExchangeLovelinessInfo->dwLoveliness;						// �û�����
	ExchangeLovelinessInfo.dwAvailbleAmount = pExchangeLovelinessInfo->dwAvailbleAmount;					// �ɶһ�����
	ExchangeLovelinessInfo.dwFinalAmount = pExchangeLovelinessInfo->dwFinalAmount;						// ʵ���ܵõ��Ļ���(�ɶһ����� * �һ���)
	lstrcpyn(ExchangeLovelinessInfo.szTips,pExchangeLovelinessInfo->szTips,CountArray(ExchangeLovelinessInfo.szTips));
	lstrcpyn(ExchangeLovelinessInfo.szErr,pExchangeLovelinessInfo->szErr,CountArray(ExchangeLovelinessInfo.szErr));

//	WORD wTips=CountStringBuffer(ExchangeLovelinessInfo.szTips);
//	WORD wErr=CountStringBuffer(ExchangeLovelinessInfo.szErr);
//	WORD wHeadSize=sizeof(ExchangeLovelinessInfo)-sizeof(ExchangeLovelinessInfo.szTips)-sizeof(ExchangeLovelinessInfo.szErr);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE, SUB_GP_EXCHANGE_LOVELINESS_INFO, &ExchangeLovelinessInfo, sizeof(ExchangeLovelinessInfo));

	//�ر�����(�ײ�error,��ʱ���ر�����)
//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

// �����һ���������
bool CAttemperEngineSink::OnDBPExchangeLoveliness(DWORD dwContextID,VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_GP_ExchangeLovelinessResult ExchangeLovelinessResult;
	ZeroMemory(&ExchangeLovelinessResult,sizeof(ExchangeLovelinessResult));

	//��������
	DBO_GP_ExchangeLoveliness * pExchangeLoveliness=(DBO_GP_ExchangeLoveliness *)pData;

	// ��������
	ExchangeLovelinessResult.dwUserID = pExchangeLoveliness->dwUserID;
	ExchangeLovelinessResult.dwLoveliness = pExchangeLoveliness->dwLoveliness;
	ExchangeLovelinessResult.lScore = pExchangeLoveliness->lScore;				
	ExchangeLovelinessResult.lInsureScore = pExchangeLoveliness->lInsureScore;	
	ExchangeLovelinessResult.lResultCode=pExchangeLoveliness->lResultCode;
	lstrcpyn(ExchangeLovelinessResult.szDescribeString,pExchangeLoveliness->szDescribeString,CountArray(ExchangeLovelinessResult.szDescribeString));

	LOGI("CAttemperEngineSink::OnDBPExchangeLoveliness, dwUserID="<<pExchangeLoveliness->dwUserID<<", dwLoveliness="<<pExchangeLoveliness->dwLoveliness<<\
		", lScore="<<pExchangeLoveliness->lScore<<", lInsureScore="<<pExchangeLoveliness->lInsureScore<<", lResultCode="<<pExchangeLoveliness->lResultCode<<\
		", szDescribeString="<<pExchangeLoveliness->szDescribeString);

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GP_USER_SERVICE, SUB_GP_EXCHANGE_LOVELINESS_RESULT, &ExchangeLovelinessResult, sizeof(ExchangeLovelinessResult));

	//�ر�����(�ײ�error,��ʱ���ر�����)
//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
	return true;
}

//��¼�ɹ�
bool CAttemperEngineSink::OnDBMBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	LOGI("CAttemperEngineSink::OnDBMBLogonSuccess");
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	DBO_MB_LogonSuccess * pDBOLogonSuccess=(DBO_MB_LogonSuccess *)pData;

	//��������
// 	CMD_MB_LogonSuccess LogonSuccess;
// 	ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));


	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	CMD_MB_LogonSuccess * pMBLogonSuccess = (CMD_MB_LogonSuccess *)cbDataBuffer;
	memset(pMBLogonSuccess, 0, sizeof(CMD_MB_LogonSuccess));

	WORD wHeadSize = sizeof(CMD_MB_LogonSuccess);
	CSendPacketHelper SendPacket(cbDataBuffer + wHeadSize, sizeof(cbDataBuffer) - wHeadSize);

	//��������
	pMBLogonSuccess->wFaceID = pDBOLogonSuccess->wFaceID;
	pMBLogonSuccess->cbGender = pDBOLogonSuccess->cbGender;
	pMBLogonSuccess->dwGameID = pDBOLogonSuccess->dwGameID;
	pMBLogonSuccess->dwUserID = pDBOLogonSuccess->dwUserID;
	pMBLogonSuccess->dwExperience = pDBOLogonSuccess->dwExperience;
	pMBLogonSuccess->dwLoveLiness = pDBOLogonSuccess->dwLoveLiness;
	pMBLogonSuccess->lUserScore = pDBOLogonSuccess->lUserScore;							//�û����
	pMBLogonSuccess->lUserInsure = pDBOLogonSuccess->lUserInsure;							//�û�����	.
	lstrcpyn(pMBLogonSuccess->szAccounts, pDBOLogonSuccess->szAccounts, CountArray(pMBLogonSuccess->szAccounts));
	lstrcpyn(pMBLogonSuccess->szNickName, pDBOLogonSuccess->szNickName, CountArray(pMBLogonSuccess->szNickName));
	pMBLogonSuccess->dwDiamondCount = pDBOLogonSuccess->dwDiamondCount;

	// ��ӡ��־
	LOGI("CAttemperEngineSink::OnDBMBLogonSuccess UserID=" << pMBLogonSuccess->dwUserID << ", UserScore=" << pMBLogonSuccess->lUserScore <<
		", lUserInsure=" << pMBLogonSuccess->lUserInsure << ", AreaID=" << pDBOLogonSuccess->dwAreaID);

	if (pDBOLogonSuccess->cbMemberOrder != 0)
	{
		DTP_MB_MemberInfo MemberInfo;
		memset(&MemberInfo, 0, sizeof(MemberInfo));
		MemberInfo.cbMemberOrder = pDBOLogonSuccess->cbMemberOrder;
		SendPacket.AddPacket(&MemberInfo, sizeof(MemberInfo), DTP_MB_MEMBER_INFO);
	}

	if (pDBOLogonSuccess->szAgentInfo[0] != 0)
	{
		DTP_MB_AgentInfo AgentInfo;
		memset(&AgentInfo, 0, sizeof(AgentInfo));
		_sntprintf(AgentInfo.szAgentInfo, sizeof(AgentInfo.szAgentInfo), pDBOLogonSuccess->szAgentInfo);
		SendPacket.AddPacket(&AgentInfo, sizeof(AgentInfo), DTP_MB_AGENT_INFO);
	}

	//��¼�ɹ�
	//��¼�ɹ�
	WORD wSendSize = SendPacket.GetDataSize() + wHeadSize;
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_MB_LOGON, SUB_MB_LOGON_SUCCESS, cbDataBuffer, wSendSize);

	//���ͷ���
	WORD wIndex=LOWORD(dwContextID);
	SendMobileTypeInfo(dwContextID);
	SendMobileKindInfo(dwContextID, INVALID_WORD, pDBOLogonSuccess->dwAreaID, pDBOLogonSuccess->wPlatformID);
	SendMobileServerInfo(dwContextID, INVALID_WORD, pDBOLogonSuccess->dwAreaID, pDBOLogonSuccess->wPlatformID);
	SendMobileKindInfoEx(dwContextID, INVALID_WORD, pDBOLogonSuccess->dwAreaID, pDBOLogonSuccess->wPlatformID);
	SendCustomTablePropertyConfig(dwContextID, INVALID_WORD, pDBOLogonSuccess->dwAreaID);
	// �����Ѿ�������Զ���������Ϣ
	if (pDBOLogonSuccess->wServerID != 0 && pDBOLogonSuccess->szSecret[0] != 0)
	{
		SendUserCustomTableInfo(dwContextID, pDBOLogonSuccess->wServerID, pDBOLogonSuccess->szSecret, pDBOLogonSuccess->bLockedInTable);
	}
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_MB_SERVER_LIST,SUB_MB_LIST_FINISH);

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBMBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//��������
	CMD_MB_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));
	DBO_MB_LogonFailure * pLogonFailure=(DBO_MB_LogonFailure *)pData;

	//��������
	LogonFailure.lResultCode=pLogonFailure->lResultCode;
	lstrcpyn(LogonFailure.szDescribeString,pLogonFailure->szDescribeString,CountArray(LogonFailure.szDescribeString));

	//��������
	WORD wStringSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wSendSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString)+wStringSize;
	LOGD("CAttemperEngineSink::OnDBMBLogonFailure, SendData \n");
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_MB_LOGON,SUB_MB_LOGON_FAILURE,&LogonFailure,wSendSize);

	//�ر�����(�ײ�������,�ر�socket���ܵ�����Ϣ��δ���ͳɹ���ر�)
//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

// ǩ���ɹ�
bool CAttemperEngineSink::OnDBMBSignInCheckSuccess(DWORD dwContextID,VOID* pData,WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	DBO_MB_SignInCheck* DBOSignInCheck = (DBO_MB_SignInCheck*)pData;

	CMD_MB_SignInCheck SignInCheck;
	memset(&SignInCheck, 0, sizeof(SignInCheck));
	SignInCheck.dwErrorCode = DBOSignInCheck->dwErrorCode;
	SignInCheck.dwCheckDay = DBOSignInCheck->dwCheckDay;
//	m_pITCPNetworkEngine->SendData(dwContextID,MDM_MB_LOGON,SUB_MB_SIGN_IN_CHECK,&SignInCheck,sizeof(SignInCheck));

	return true;
}

// ��ȡ�Ĺ��ڿͷ���������Ϣ
bool CAttemperEngineSink::OnDBMBCustomServiceMessage(DWORD dwContextID,VOID* pData,WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	CMD_MB_SystemMessage* pDBOSystemMessage = (CMD_MB_SystemMessage*)pData;

	CMD_MB_SystemMessage SystemMessage;
	memcpy(&SystemMessage, pDBOSystemMessage, sizeof(SystemMessage));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	m_pITCPNetworkEngine->SendData(dwContextID, MDM_MB_SYSTEM_MESSAGE, SUB_MB_CUSTOM_SERVICE_MESSAGE, &SystemMessage, wSendSize);
	
	return true;
}

// ��ȡ���ڳ�ֵ��������Ϣ
bool CAttemperEngineSink::OnDBMBRechargeMessage(DWORD dwContextID,VOID* pData,WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID)<m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	CMD_MB_SystemMessage* pDBOSystemMessage = (CMD_MB_SystemMessage*)pData;

	CMD_MB_SystemMessage SystemMessage;
	memcpy(&SystemMessage, pDBOSystemMessage, sizeof(SystemMessage));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_MB_SYSTEM_MESSAGE, SUB_MB_RECHARGE_MESSAGE, &SystemMessage, wSendSize);

	return true;
}

// �����Զ�������
bool CAttemperEngineSink::OnDBUserCustomTableSuccess(DWORD dwContextID, VOID* pData, WORD wDataSize)
{
	LOGI("CAttemperEngineSink::OnDBUserCustomTableSuccess");
	DBO_GR_UserCustomTableSuccess* pUserCustomTableSuccess = (DBO_GR_UserCustomTableSuccess*)pData;

	CMD_MB_UserCustomTableSuccess UserCustomTableSuccess;
	UserCustomTableSuccess.wServerID = pUserCustomTableSuccess->wServerID;
	lstrcpyn(UserCustomTableSuccess.szSecret, pUserCustomTableSuccess->szSecret, CountArray(UserCustomTableSuccess.szSecret));
	UserCustomTableSuccess.bLockedInTable = pUserCustomTableSuccess->bLockedInTable;
	UserCustomTableSuccess.dwDiamondCount = pUserCustomTableSuccess->dwUserPropertyCount;
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER_SERVICE, SUB_MB_USER_CUSTOM_TABLE_SUCCESS, &UserCustomTableSuccess, sizeof(UserCustomTableSuccess));

	return true;
}

// �����Զ�������ʧ��
bool CAttemperEngineSink::OnDBUserCustomTableFailed(DWORD dwContextID, VOID* pData, WORD wDataSize)
{
	LOGI("CAttemperEngineSink::OnDBUserCustomTableFailed");
	DBO_GR_UserCustomTableFailure* pUserCustomTableFailure = (DBO_GR_UserCustomTableFailure*)pData;

	CMD_MB_UserCustomTableFailed UserCustomTableFailed;
	UserCustomTableFailed.lErrorCode = pUserCustomTableFailure->lResultCode;
	lstrcpyn(UserCustomTableFailed.szDescribeString, pUserCustomTableFailure->szDescribeString, CountArray(UserCustomTableFailed.szDescribeString));
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER_SERVICE, SUB_MB_USER_CUSTOM_TABLE_FAILED, &UserCustomTableFailed, sizeof(UserCustomTableFailed));
	return true;
}

// ��ʯ�����ɹ�
bool CAttemperEngineSink::OnDBUserDiamondSuccess(DWORD dwContextID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID) < m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter + LOWORD(dwContextID))->dwSocketID != dwContextID) return true;

	//��������
	DBO_MB_UserDiamondSuccess * pUserUserDiamondSuccess = (DBO_MB_UserDiamondSuccess *)pData;

	//��������
	CMD_MB_UserDiamondSuccess UserDiamondSuccess;
	ZeroMemory(&UserDiamondSuccess, sizeof(UserDiamondSuccess));

	//���ñ���
	UserDiamondSuccess.dwUserID = pUserUserDiamondSuccess->dwUserID;
	UserDiamondSuccess.dwDiamondCount = pUserUserDiamondSuccess->dwDiamondCount;
	lstrcpyn(UserDiamondSuccess.szDescribeString, pUserUserDiamondSuccess->szDescribeString, CountArray(UserDiamondSuccess.szDescribeString));

	//������Ϣ
	WORD wDescribe = CountStringBuffer(UserDiamondSuccess.szDescribeString);
	WORD wHeadSize = sizeof(UserDiamondSuccess) - sizeof(UserDiamondSuccess.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER_SERVICE, SUB_MB_USER_DIAMOND_SUCCESS, &UserDiamondSuccess, wHeadSize + wDescribe);

	return true;
}

// ��ʯ����ʧ��
bool CAttemperEngineSink::OnDBUserDiamondFialed(DWORD dwContextID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(LOWORD(dwContextID) < m_pInitParameter->m_wMaxConnect);
	if ((m_pBindParameter + LOWORD(dwContextID))->dwSocketID != dwContextID) return true;

	//��������
	CMD_MB_UserDiamondFailure UserDiamondFailure;
	ZeroMemory(&UserDiamondFailure, sizeof(UserDiamondFailure));

	//��������
	DBO_MB_UserDiamondFailure * pUserDiamondFailure = (DBO_MB_UserDiamondFailure *)pData;

	//��������
	UserDiamondFailure.lErrorCode = pUserDiamondFailure->lResultCode;
	lstrcpyn(UserDiamondFailure.szDescribeString, pUserDiamondFailure->szDescribeString, CountArray(UserDiamondFailure.szDescribeString));

	//��������
	WORD wDescribe = CountStringBuffer(UserDiamondFailure.szDescribeString);
	WORD wHeadSize = sizeof(UserDiamondFailure) - sizeof(UserDiamondFailure.szDescribeString);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_GP_USER_SERVICE, SUB_MB_USER_DIAMOND_FAILURE, &UserDiamondFailure, wHeadSize + wDescribe);

	//�ر�����(�ײ�error,��ʱ��ȥ�ر�)
	//	m_pITCPNetworkEngine->ShutDownSocket(dwContextID);

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnDBPCGameTypeItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GameType)==0);
	if (wDataSize%sizeof(DBO_GP_GameType)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameType);
	DBO_GP_GameType * pGameType=(DBO_GP_GameType *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameType GameType;
		ZeroMemory(&GameType,sizeof(GameType));

		//��������
		GameType.wTypeID=(pGameType+i)->wTypeID;
		GameType.wJoinID=(pGameType+i)->wJoinID;
		GameType.wSortID=(pGameType+i)->wSortID;
		lstrcpyn(GameType.szTypeName,(pGameType+i)->szTypeName,CountArray(GameType.szTypeName));

		//�����б�
		m_ServerListManager.InsertGameType(&GameType);
	}

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnDBPCGameKindItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GameKind)==0);
	if (wDataSize%sizeof(DBO_GP_GameKind)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameKind);
	DBO_GP_GameKind * pGameKind=(DBO_GP_GameKind *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameKind GameKind;
		ZeroMemory(&GameKind,sizeof(GameKind));

		//��������
		GameKind.wTypeID=(pGameKind+i)->wTypeID;
		GameKind.wJoinID=(pGameKind+i)->wJoinID;
		GameKind.wSortID=(pGameKind+i)->wSortID;
		GameKind.wKindID=(pGameKind+i)->wKindID;
		GameKind.wGameID=(pGameKind+i)->wGameID;
		GameKind.dwOnLineCount=m_ServerListManager.CollectOnlineInfo((pGameKind+i)->wKindID);
		GameKind.dwFullCount=m_ServerListManager.CollectFullCount((pGameKind + i)->wKindID);
		lstrcpyn(GameKind.szKindName,(pGameKind+i)->szKindName,CountArray(GameKind.szKindName));
		lstrcpyn(GameKind.szProcessName,(pGameKind+i)->szProcessName,CountArray(GameKind.szProcessName));

		//�����б�
		m_ServerListManager.InsertGameKind(&GameKind);
	}

	return true;
}

//��Ϸ�ڵ�
bool CAttemperEngineSink::OnDBPCGameNodeItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GameNode)==0);
	if (wDataSize%sizeof(DBO_GP_GameNode)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameNode);
	DBO_GP_GameNode * pGameNode=(DBO_GP_GameNode *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameNode GameNode;
		ZeroMemory(&GameNode,sizeof(GameNode));

		//��������
		GameNode.wKindID=(pGameNode+i)->wKindID;
		GameNode.wJoinID=(pGameNode+i)->wJoinID;
		GameNode.wSortID=(pGameNode+i)->wSortID;
		GameNode.wNodeID=(pGameNode+i)->wNodeID;
		lstrcpyn(GameNode.szNodeName,(pGameNode+i)->szNodeName,CountArray(GameNode.szNodeName));

		//�����б�
		m_ServerListManager.InsertGameNode(&GameNode);
	}

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnDBPCGamePageItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GamePage)==0);
	if (wDataSize%sizeof(DBO_GP_GamePage)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GamePage);
	DBO_GP_GamePage * pGamePage=(DBO_GP_GamePage *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGamePage GamePage;
		ZeroMemory(&GamePage,sizeof(GamePage));

		//��������
		GamePage.wKindID=(pGamePage+i)->wKindID;
		GamePage.wNodeID=(pGamePage+i)->wNodeID;
		GamePage.wSortID=(pGamePage+i)->wSortID;
		GamePage.wPageID=(pGamePage+i)->wPageID;
		GamePage.wOperateType=(pGamePage+i)->wOperateType;
		lstrcpyn(GamePage.szDisplayName,(pGamePage+i)->szDisplayName,CountArray(GamePage.szDisplayName));

		//�����б�
		m_ServerListManager.InsertGamePage(&GamePage);
	}

	return true;
}

//����ID��ص�GameKindItem
bool CAttemperEngineSink::OnDBMBAreaGameKindItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_MB_AreaGameKind) == 0);
	if (wDataSize%sizeof(DBO_MB_AreaGameKind) != 0) return false;

	//��������
	WORD wItemCount = wDataSize / sizeof(DBO_MB_AreaGameKind);
	DBO_MB_AreaGameKind * pAreaGameKind = (DBO_MB_AreaGameKind *)pData;

	// �����������
	m_mapAreaGameKind.clear();

	//��������
	for (WORD i = 0; i < wItemCount; i++)
	{
		//��������
		tagAreaGameKind AreaGameKind;
		ZeroMemory(&AreaGameKind, sizeof(AreaGameKind));

		//��������
		AreaGameKind.dwAreaID = (pAreaGameKind + i)->dwAreaID;
		AreaGameKind.wSortID = (pAreaGameKind + i)->wSortID;
		AreaGameKind.wKindID = (pAreaGameKind + i)->wKindID;
		AreaGameKind.wPlatformID = (pAreaGameKind + i)->wPlatformID;
// 		AreaGameKind.cbPcVisiable = (pAreaGameKind + i)->cbPcVisiable;
// 		AreaGameKind.cbAppVisiable = (pAreaGameKind + i)->cbAppVisiable;

		LOGI("CAttemperEngineSink::OnDBMBAreaGameKindItem, AreaID=" << AreaGameKind.dwAreaID << ", KindID=" << AreaGameKind.wKindID << \
			", SortID=" << AreaGameKind.wSortID << ", PlatformID=" << AreaGameKind.wPlatformID);

		//�����б�
		map<DWORD, vector<tagAreaGameKind> >::iterator iter = m_mapAreaGameKind.find(AreaGameKind.dwAreaID);
		if (iter != m_mapAreaGameKind.end())
		{
			iter->second.push_back(AreaGameKind);
		}
		else
		{
			vector<tagAreaGameKind> vectAreaGameKind;
			vectAreaGameKind.push_back(AreaGameKind);
			m_mapAreaGameKind.insert(make_pair(AreaGameKind.dwAreaID, vectAreaGameKind));
		}
	}

	return true;
}

//����ID��ص�GameServerItem
bool CAttemperEngineSink::OnDBMBAreaGameServerItem(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_MB_AreaGameServer) == 0);
	if (wDataSize%sizeof(DBO_MB_AreaGameServer) != 0) return false;

	//��������
	WORD wItemCount = wDataSize / sizeof(DBO_MB_AreaGameServer);
	DBO_MB_AreaGameServer * pAreaGameKind = (DBO_MB_AreaGameServer *)pData;

	// �����������
	m_mapAreaGameServer.clear();

	//��������
	for (WORD i = 0; i < wItemCount; i++)
	{
		//��������
		tagAreaGameServer AreaGameServer;
		ZeroMemory(&AreaGameServer, sizeof(AreaGameServer));

		//��������
		AreaGameServer.dwAreaID = (pAreaGameKind + i)->dwAreaID;
		AreaGameServer.wSortID = (pAreaGameKind + i)->wSortID;
		AreaGameServer.wServerID = (pAreaGameKind + i)->wServerID;
		AreaGameServer.wPlatformID = (pAreaGameKind + i)->wPlatformID;
		_tcsncpy(AreaGameServer.szServerName, (pAreaGameKind + i)->szServerName, sizeof(AreaGameServer.szServerName));

		//�����б�
		map<DWORD, vector<tagAreaGameServer> >::iterator iter = m_mapAreaGameServer.find(AreaGameServer.dwAreaID);
		if (iter != m_mapAreaGameServer.end())
		{
			iter->second.push_back(AreaGameServer);
		}
		else
		{
			vector<tagAreaGameServer> vectAreaGameServer;
			vectAreaGameServer.push_back(AreaGameServer);
			m_mapAreaGameServer.insert(make_pair(AreaGameServer.dwAreaID, vectAreaGameServer));
		}
	}
	return true;
}

// APP��ص���Դ�б�
bool CAttemperEngineSink::OnDBMBGameKindItemEx(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_MB_GameKindEx) == 0);
	if (wDataSize%sizeof(DBO_MB_GameKindEx) != 0) return false;

	//��������
	WORD wItemCount = wDataSize / sizeof(DBO_MB_GameKindEx);
	DBO_MB_GameKindEx * pGameKindEx = (DBO_MB_GameKindEx *)pData;

	// �����������
	m_mapGameKindEx.clear();

	//��������
	for (WORD i = 0; i < wItemCount; i++)
	{
		//��������
		tagGameKindEx GameKindEx;
		ZeroMemory(&GameKindEx, sizeof(GameKindEx));

		//��������
		GameKindEx.wKindID = (pGameKindEx + i)->wKindID;
		GameKindEx.dwAppVersion = (pGameKindEx + i)->dwAppVersion;
		_tcsncpy(GameKindEx.szAppDownloadURL, (pGameKindEx + i)->szAppDownloadURL, sizeof(GameKindEx.szAppDownloadURL));

		LOGI("CAttemperEngineSink::OnDBMBGameKindItemEx, wKindID=" << GameKindEx.wKindID << ", dwAppVersion=" << GameKindEx.dwAppVersion << ", AppDownloadURL=" << GameKindEx.szAppDownloadURL);

		//�����б�
		map<WORD, tagGameKindEx >::iterator iter = m_mapGameKindEx.find(GameKindEx.wKindID);
		if (iter != m_mapGameKindEx.end())
		{
			iter->second = GameKindEx;;
		}
		else
		{
			m_mapGameKindEx.insert(make_pair(GameKindEx.wKindID, GameKindEx));
		}
	}

	return true;
}

// �Խ�����,���ĵ��ߺ;�������
bool CAttemperEngineSink::OnDBMBCustomTablePropertyConfig(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_MB_CustomTablePropertyConfig) == 0);
	if (wDataSize%sizeof(DBO_MB_CustomTablePropertyConfig) != 0) return false;

	//��������
	WORD wItemCount = wDataSize / sizeof(DBO_MB_CustomTablePropertyConfig);
	DBO_MB_CustomTablePropertyConfig * pPropertyConfig = (DBO_MB_CustomTablePropertyConfig *)pData;

	// �����������
	m_mapCustomTablePropertyConfig.clear();

	//��������
	for (WORD i = 0; i < wItemCount; i++)
	{
		WORD	wKindID = (pPropertyConfig + i)->wKindID;										// ��Ϸ���ͱ�ʶ
		WORD	wPropertyConsumeCount = (pPropertyConfig + i)->wPropertyConsumeCount;			// ������������
		WORD	wRoundCount = (pPropertyConfig + i)->wRoundCount;								// ��Ӧ�ľ���
		//�����б�
		map<WORD, tagCustomTablePropertyConfig>::iterator iter = m_mapCustomTablePropertyConfig.find(wKindID);
		if (iter != m_mapCustomTablePropertyConfig.end())
		{
			iter->second.wKindID = wKindID;
			iter->second.szPropertyConsumeCount[1] = wPropertyConsumeCount;
			iter->second.szRoundCount[1] = wRoundCount;
		}
		else
		{
			//��������
			tagCustomTablePropertyConfig CustomTablePropertyConfig;
			ZeroMemory(&CustomTablePropertyConfig, sizeof(CustomTablePropertyConfig));

			CustomTablePropertyConfig.wKindID = wKindID;
			CustomTablePropertyConfig.szPropertyConsumeCount[0] = wPropertyConsumeCount;
			CustomTablePropertyConfig.szRoundCount[0] = wRoundCount;

			m_mapCustomTablePropertyConfig.insert(make_pair(wKindID, CustomTablePropertyConfig));
		}
	}

	return true;
}

//��Ϸ�б�
bool CAttemperEngineSink::OnDBPCGameListResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBO_GP_GameListResult));
	if (wDataSize!=sizeof(DBO_GP_GameListResult)) return false;

	//��������
	DBO_GP_GameListResult * pGameListResult=(DBO_GP_GameListResult *)pData;

	//��Ϣ����
	if (pGameListResult->cbSuccess==TRUE)
	{
		//�����б�
		m_ServerListManager.CleanKernelItem();

		//�¼�֪ͨ
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess=ER_SUCCESS;
		SendUIControlPacket(UI_LOAD_DB_LIST_RESULT,&ControlResult,sizeof(ControlResult));

		//����ʱ��
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_LOAD_GAME_LIST,m_pInitParameter->m_wLoadListTime*1000L,1,0);
	}
	else
	{
		//������ʾ
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf(szDescribe,CountArray(szDescribe),TEXT("�������б����ʧ�ܣ�%ld ������¼���"),m_pInitParameter->m_wReLoadListTime);

		//��ʾ��Ϣ
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		//����ʱ��
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_LOAD_GAME_LIST,m_pInitParameter->m_wReLoadListTime*1000L,1,0);
	}

	return true;
}

//�汾���
bool CAttemperEngineSink::CheckPlazaVersion(BYTE cbDeviceType, DWORD dwPlazaVersion, DWORD dwSocketID, bool bCheckLowVer)
{
	//��������
	bool bMustUpdate=false;
	bool bAdviceUpdate=false;
	DWORD dwVersion=VERSION_PLAZA;

	//�ֻ��汾
	if(cbDeviceType >= DEVICE_TYPE_IPAD) dwVersion=VERSION_MOBILE_IOS;
	else if(cbDeviceType >= DEVICE_TYPE_IPHONE) dwVersion=VERSION_MOBILE_IOS;
	else if(cbDeviceType >= DEVICE_TYPE_ITOUCH) dwVersion=VERSION_MOBILE_IOS;
	else if(cbDeviceType >= DEVICE_TYPE_ANDROID) dwVersion=VERSION_MOBILE_ANDROID;
	else if(cbDeviceType == DEVICE_TYPE_PC) dwVersion=VERSION_PLAZA;

	//�汾�ж�
	if (bCheckLowVer && GetSubVer(dwPlazaVersion)<GetSubVer(dwVersion)) bAdviceUpdate=true;
	if (GetMainVer(dwPlazaVersion)!=GetMainVer(dwVersion)) bMustUpdate=true;
	if (GetProductVer(dwPlazaVersion)!=GetProductVer(dwVersion)) bMustUpdate=true;

	//�����ж�
	if ((bMustUpdate==true)||(bAdviceUpdate==true))
	{
		//��������
		CMD_GP_UpdateNotify UpdateNotify;
		ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));

		//��������
		UpdateNotify.cbMustUpdate=bMustUpdate;
		UpdateNotify.cbAdviceUpdate=bAdviceUpdate;
		UpdateNotify.dwCurrentVersion=dwVersion;

		//������Ϣ
		if (cbDeviceType == DEVICE_TYPE_PC)
		{
			// PC������ʾ
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_LOGON,SUB_GP_UPDATE_NOTIFY,&UpdateNotify,sizeof(UpdateNotify));
		}
		else
		{
			// �ֻ�������ʾ(�ֻ��е�6103��6603��Ҫ��ʾ����,6703֮��İ汾����վ����)
			if (dwPlazaVersion == PROCESS_VERSION(1,0,3) || dwPlazaVersion == PROCESS_VERSION(2,0,3) || dwPlazaVersion == PROCESS_VERSION(3,0,3) || dwPlazaVersion == PROCESS_VERSION(4,0,3) ||
				dwPlazaVersion == PROCESS_VERSION(5,0,3) || dwPlazaVersion == PROCESS_VERSION(6,0,3))
			{
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_LOGON,SUB_GP_UPDATE_NOTIFY,&UpdateNotify,sizeof(UpdateNotify));
			}
			else
			{
				return true;
			}
		}
		
		//�ж��ж�
		if (bMustUpdate==true) 
		{
			//�ر�����(�ײ�error,��ʱ���ر�����)
//			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
			return false;
		}
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

// DeviceTypeתƽ̨ID
WORD CAttemperEngineSink::DeviceTypeToPlatformID(BYTE cbDeviceType)
{
	if (cbDeviceType == DEVICE_TYPE_PC)
	{
		return CC_PLATFORM_WIN32;
	}
	else if (cbDeviceType == DEVICE_TYPE_ANDROID)
	{
		return CC_PLATFORM_ANDROID;
	}
	else if (cbDeviceType == DEVICE_TYPE_ITOUCH)
	{
		return CC_PLATFORM_IOS;
	}
	else if (cbDeviceType == DEVICE_TYPE_IPHONE)
	{
		return CC_PLATFORM_IOS;
	}
	else if (cbDeviceType == DEVICE_TYPE_IPAD)
	{
		return CC_PLATFORM_IOS;
	}
	else
	{
		return CC_PLATFORM_UNKNOWN;
	}
}


//��������
VOID CAttemperEngineSink::SendGameTypeInfo(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameTypeItem * pGameTypeItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameTypeCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameType))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_TYPE,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameTypeItem=m_ServerListManager.EmunGameTypeItem(Position);
		if (pGameTypeItem==NULL) break;

		//��������
		CopyMemory(cbDataBuffer+wSendSize,&pGameTypeItem->m_GameType,sizeof(tagGameType));
		wSendSize+=sizeof(tagGameType);
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_TYPE,cbDataBuffer,wSendSize);

	return;
}

//��������
VOID CAttemperEngineSink::SendGameKindInfo(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameKindItem * pGameKindItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameKindCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameKind))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_KIND,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameKindItem=m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem==NULL) break;

		//��������
		CopyMemory(cbDataBuffer+wSendSize,&pGameKindItem->m_GameKind,sizeof(tagGameKind));
		wSendSize+=sizeof(tagGameKind);
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_KIND,cbDataBuffer,wSendSize);

	return;
}

//���ͽڵ�
VOID CAttemperEngineSink::SendGameNodeInfo(DWORD dwSocketID, WORD wKindID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameNodeItem * pGameNodeItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameNodeCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameNode))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_NODE,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameNodeItem=m_ServerListManager.EmunGameNodeItem(Position);
		if (pGameNodeItem==NULL) break;

		//��������
		if ((wKindID==INVALID_WORD)||(pGameNodeItem->m_GameNode.wKindID==wKindID))
		{
			CopyMemory(cbDataBuffer+wSendSize,&pGameNodeItem->m_GameNode,sizeof(tagGameNode));
			wSendSize+=sizeof(tagGameNode);
		}
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_NODE,cbDataBuffer,wSendSize);

	return;
}

//���Ͷ���
VOID CAttemperEngineSink::SendGamePageInfo(DWORD dwSocketID, WORD wKindID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGamePageItem * pGamePageItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGamePageCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGamePage))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_PAGE,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGamePageItem=m_ServerListManager.EmunGamePageItem(Position);
		if (pGamePageItem==NULL) break;

		//��������
		if ((wKindID==INVALID_WORD)||(pGamePageItem->m_GamePage.wKindID==wKindID))
		{
			CopyMemory(cbDataBuffer+wSendSize,&pGamePageItem->m_GamePage,sizeof(tagGamePage));
			wSendSize+=sizeof(tagGamePage);
		}
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_PAGE,cbDataBuffer,wSendSize);

	return;
}

//���ͷ���
VOID CAttemperEngineSink::SendGameServerInfo(DWORD dwSocketID, WORD wKindID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameServerItem * pGameServerItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameServerCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameServer))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_SERVER,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameServerItem=m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem==NULL) break;

		//��������
		if ((wKindID==INVALID_WORD)||(pGameServerItem->m_GameServer.wKindID==wKindID))
		{
			CopyMemory(cbDataBuffer+wSendSize,&pGameServerItem->m_GameServer,sizeof(tagGameServer));
			wSendSize+=sizeof(tagGameServer);
		}
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_SERVER,cbDataBuffer,wSendSize);

	if(m_wAVServerPort!=0 && m_dwAVServerAddr!=0)
	{
		//��������
		tagAVServerOption AVServerOption;
		AVServerOption.wAVServerPort=m_wAVServerPort;
		AVServerOption.dwAVServerAddr=m_dwAVServerAddr;

		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_VIDEO_OPTION,&AVServerOption,sizeof(AVServerOption));
	};

	return;
}

//��������
VOID CAttemperEngineSink::SendMobileTypeInfo(DWORD dwSocketID)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameTypeItem * pGameTypeItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameTypeCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameType))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_TYPE,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameTypeItem=m_ServerListManager.EmunGameTypeItem(Position);
		if (pGameTypeItem==NULL) break;

		//��������
		CopyMemory(cbDataBuffer+wSendSize,&pGameTypeItem->m_GameType,sizeof(tagGameType));
		wSendSize+=sizeof(tagGameType);
	}

	//����ʣ��
	if (wSendSize>0)
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_TYPE,cbDataBuffer,wSendSize);
}

//��������
VOID CAttemperEngineSink::SendMobileKindInfo(DWORD dwSocketID, WORD wModuleID, DWORD dwAreaID /* = INVALID_DWORD */, WORD wPlatformID/* = CC_PLATFORM_UNKNOWN*/)
{
	LOGI("CAttemperEngineSink::SendMobileKindInfo, wModuleID=" << wModuleID << ", dwAreaID=" << dwAreaID << ", wPlatformID=" << wPlatformID);
	//��������
	WORD wSendSize = 0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position = NULL;
	CGameKindItem * pGameKindItem = NULL;

	//ö������
	for (DWORD i = 0; i<m_ServerListManager.GetGameKindCount(); i++)
	{
		//��������
		if ((wSendSize + sizeof(tagGameKind))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_KIND, cbDataBuffer, wSendSize);
			wSendSize = 0;
		}

		//��ȡ����
		pGameKindItem = m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//��������
		auto iter = m_mapAreaGameKind.find(dwAreaID);
		if (dwAreaID == INVALID_DWORD || iter == m_mapAreaGameKind.end())
		{
			// ��ɸѡAreaID
			if ((wModuleID == INVALID_WORD) || (pGameKindItem->m_GameKind.wKindID == wModuleID))
			{
				// ��ɸѡModuleID
				CopyMemory(cbDataBuffer + wSendSize, &pGameKindItem->m_GameKind, sizeof(tagGameKind));
				wSendSize += sizeof(tagGameKind);
			}
		}
		else
		{
			// ����Ҫɸѡ��AreaID
			// ���ɸѡ������
			if ((wModuleID == INVALID_WORD) || (pGameKindItem->m_GameKind.wKindID == wModuleID))
			{
				for (auto v : iter->second)
				{
					if ((v.wPlatformID == CC_PLATFORM_UNKNOWN) || (v.wPlatformID == wPlatformID))
					{
						if (v.wKindID == pGameKindItem->m_GameKind.wKindID)
						{
							// SortID��Ҫ�����±��е�����
							tagGameKind gamekind;
							CopyMemory(&gamekind, &pGameKindItem->m_GameKind, sizeof(tagGameKind));
							if (v.wSortID != 0)
							{
								// ��0�Ų����µ�ֵ
								gamekind.wSortID = v.wSortID;
							}
							CopyMemory(cbDataBuffer + wSendSize, &gamekind, sizeof(tagGameKind));
							wSendSize += sizeof(tagGameKind);
							break;
						}
					}
				}
			}
		}
	}

	//����ʣ��
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_KIND,cbDataBuffer,wSendSize);

	return;
}

//���ͷ���
VOID CAttemperEngineSink::SendMobileServerInfo(DWORD dwSocketID, WORD wModuleID, DWORD dwAreaID /* = INVALID_DWORD */, WORD wPlatformID/* = CC_PLATFORM_UNKNOWN*/)
{
	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position=NULL;
	CGameServerItem * pGameServerItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameServerCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameServer))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_SERVER,cbDataBuffer,wSendSize);
			wSendSize=0;
		}

		//��ȡ����
		pGameServerItem=m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem==NULL) break;

		//��������
		auto iter = m_mapAreaGameServer.find(dwAreaID);
		if (dwAreaID == INVALID_DWORD || iter == m_mapAreaGameServer.end())
		{
			// ���ȫ������
			if ((wModuleID == INVALID_WORD) || (pGameServerItem->m_GameServer.wServerID == wModuleID))
			{
				CopyMemory(cbDataBuffer + wSendSize, &pGameServerItem->m_GameServer, sizeof(tagGameServer));
				wSendSize += sizeof(tagGameServer);
			}
		}
		else
		{
			// ����Ҫɸѡ��AreaID
			// ���ɸѡ������
			if ((wModuleID == INVALID_WORD) || (pGameServerItem->m_GameServer.wServerID == wModuleID))
			{
				for (auto v : iter->second)
				{
					if ((v.wPlatformID == CC_PLATFORM_UNKNOWN) || (v.wPlatformID == wPlatformID))
					{
						if (v.wServerID == pGameServerItem->m_GameServer.wServerID)
						{
							tagGameServer gameserver;
							CopyMemory(&gameserver, &pGameServerItem->m_GameServer, sizeof(tagGameServer));
							if (v.wSortID != 0)
							{
								// ��0�Ų����µ�ֵ
								gameserver.wSortID = v.wSortID;
							}
							if (_tcsclen(v.szServerName) != 0)
							{
								// �ǿղŲ����µ�ֵ
								_tcsncpy(gameserver.szServerName, v.szServerName, sizeof(gameserver.szServerName));
							}
							CopyMemory(cbDataBuffer + wSendSize, &gameserver, sizeof(tagGameServer));
							wSendSize += sizeof(tagGameServer);
						}
					}
				}
			}
		}
	}

	//����ʣ��
	if (wSendSize>0)
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_SERVER,cbDataBuffer,wSendSize);

	return;
}

// ����GameKind��չ��Ϣ
VOID CAttemperEngineSink::SendMobileKindInfoEx(DWORD dwSocketID, WORD wModuleID, DWORD dwAreaID/* = INVALID_DWORD*/, WORD wPlatformID/* = CC_PLATFORM_UNKNOWN*/)
{
	LOGI("CAttemperEngineSink::SendMobileKindInfoEx, wModuleID=" << wModuleID << ", dwAreaID=" << dwAreaID);
	//��������
	WORD wSendSize = 0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position = NULL;
	CGameKindItem * pGameKindItem = NULL;

	//ö������
	for (DWORD i = 0; i<m_ServerListManager.GetGameKindCount(); i++)
	{
		//��������
		if ((wSendSize + sizeof(tagGameKindEx))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_KIND_EX, cbDataBuffer, wSendSize);
			wSendSize = 0;
		}

		//��ȡ����
		pGameKindItem = m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//��������
		auto iter = m_mapAreaGameKind.find(dwAreaID);
		if (dwAreaID == INVALID_DWORD || iter == m_mapAreaGameKind.end())
		{
			// ���ȫ������
			if ((wModuleID == INVALID_WORD) || (pGameKindItem->m_GameKind.wKindID == wModuleID))
			{
				auto iterex = m_mapGameKindEx.find(pGameKindItem->m_GameKind.wKindID);
				if (iterex != m_mapGameKindEx.end())
				{
					CopyMemory(cbDataBuffer + wSendSize, &(iterex->second), sizeof(tagGameKindEx));
					wSendSize += sizeof(tagGameKindEx);
				}
			}
		}
		else
		{
			// ����Ҫɸѡ��AreaID
			// ���ɸѡ������
			if ((wModuleID == INVALID_WORD) || (pGameKindItem->m_GameKind.wKindID == wModuleID))
			{
				for (auto v : iter->second)
				{
					if ((v.wPlatformID == CC_PLATFORM_UNKNOWN) || (v.wPlatformID == wPlatformID))
					{
						if (v.wKindID == pGameKindItem->m_GameKind.wKindID)
						{
							auto iterex = m_mapGameKindEx.find(v.wKindID);
							if (iterex != m_mapGameKindEx.end())
							{
								CopyMemory(cbDataBuffer + wSendSize, &(iterex->second), sizeof(tagGameKindEx));
								wSendSize += sizeof(tagGameKindEx);
							}
						}
					}
				}
			}
		}
	}

	//����ʣ��
	if (wSendSize > 0) m_pITCPNetworkEngine->SendData(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_KIND_EX, cbDataBuffer, wSendSize);
}

// ���ͼ����Զ������ӵ���Ϣ
VOID CAttemperEngineSink::SendUserCustomTableInfo(DWORD dwSocketID, WORD wServerID, TCHAR szSecret[LEN_CUSTOMTABLEPASSWORD], bool bLockedInTable)
{
	CMD_MB_UserCustomTableSuccess UserCustomTableSuccess;
	UserCustomTableSuccess.wServerID = wServerID;
	lstrcpyn(UserCustomTableSuccess.szSecret, szSecret, CountArray(UserCustomTableSuccess.szSecret));
	UserCustomTableSuccess.bLockedInTable = bLockedInTable;
	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_GP_USER_SERVICE, SUB_MB_USER_CUSTOM_TABLE_SUCCESS, &UserCustomTableSuccess, sizeof(UserCustomTableSuccess));
}

// �����Խ����ӷ���ĵ���������Ϣ
VOID CAttemperEngineSink::SendCustomTablePropertyConfig(DWORD dwSocketID, WORD wModuleID, DWORD dwAreaID /*= INVALID_DWORD*/, WORD wPlatformID /*= CC_PLATFORM_UNKNOWN*/)
{
	LOGI("CAttemperEngineSink::SendCustomTablePropertyConfig");
	//��������
	WORD wSendSize = 0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	POSITION Position = NULL;
	CGameKindItem * pGameKindItem = NULL;

	//ö������
	for (DWORD i = 0; i<m_ServerListManager.GetGameKindCount(); i++)
	{
		//��������
		if ((wSendSize + sizeof(tagCustomTablePropertyConfig))>sizeof(cbDataBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_PROP_CONFIG, cbDataBuffer, wSendSize);
			wSendSize = 0;
		}

		//��ȡ����
		pGameKindItem = m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//��������
		auto iter = m_mapAreaGameKind.find(dwAreaID);
		if (dwAreaID == INVALID_DWORD || iter == m_mapAreaGameKind.end())
		{
			// ���ȫ������
			if ((wModuleID == INVALID_WORD) || (pGameKindItem->m_GameKind.wKindID == wModuleID))
			{
				auto iterex = m_mapCustomTablePropertyConfig.find(pGameKindItem->m_GameKind.wKindID);
				if (iterex != m_mapCustomTablePropertyConfig.end())
				{
					CopyMemory(cbDataBuffer + wSendSize, &(iterex->second), sizeof(tagCustomTablePropertyConfig));
					wSendSize += sizeof(tagCustomTablePropertyConfig);
				}
			}
		}
		else
		{
			// ����Ҫɸѡ��AreaID
			// ���ɸѡ������
			if ((wModuleID == INVALID_WORD) || (pGameKindItem->m_GameKind.wKindID == wModuleID))
			{
				for (auto v : iter->second)
				{
					if ((v.wPlatformID == CC_PLATFORM_UNKNOWN) || (v.wPlatformID == wPlatformID))
					{
						if (v.wKindID == pGameKindItem->m_GameKind.wKindID)
						{
							auto iterex = m_mapCustomTablePropertyConfig.find(v.wKindID);
							if (iterex != m_mapCustomTablePropertyConfig.end())
							{
								CopyMemory(cbDataBuffer + wSendSize, &(iterex->second), sizeof(tagCustomTablePropertyConfig));
								wSendSize += sizeof(tagCustomTablePropertyConfig);
							}
						}
					}
				}
			}
		}
	}

	//����ʣ��
	if (wSendSize > 0)
	{
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_PROP_CONFIG, cbDataBuffer, wSendSize);
	}
}

//////////////////////////////////////////////////////////////////////////////////
