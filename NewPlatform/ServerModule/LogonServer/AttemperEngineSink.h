#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "ServerListManager.h"

//////////////////////////////////////////////////////////////////////////////////
//���ݶ���

//��������
// #define CLIENT_KIND_FALSH			1									//��ҳ����
// #define CLIENT_KIND_MOBILE			2									//�ֻ�����
// #define CLIENT_KIND_COMPUTER		3									//��������

//�󶨲���
struct tagBindParameter
{
	//�������
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwActiveTime;						//����ʱ��

	//������Ϣ
	WORD							wModuleID;							//ģ���ʶ
//	BYTE							cbClientKind;						//��������
	DWORD							dwPlazaVersion;						//�����汾

	// ��¼ϵͳ
	WORD							wPlatformID;						// ϵͳ���
};

//////////////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool							m_bNeekCorrespond;					//Э����־
	bool                            m_bShowServerStatus;                //��ʾ������״̬

	//��������
protected:
	CInitParameter *				m_pInitParameter;					//���ò���
	tagBindParameter *				m_pBindParameter;					//��������

	//�������
protected:
	CServerListManager							m_ServerListManager;				//�б����
	map<DWORD, vector<tagAreaGameKind> >		m_mapAreaGameKind;					//AreaID��KindID��ӳ��
	map<DWORD, vector<tagAreaGameServer> >		m_mapAreaGameServer;				//AreaID��ServerID��ӳ��
	map<WORD, tagGameKindEx>					m_mapGameKindEx;					// GameKind��չ��Ϣ
	map<WORD, tagCustomTablePropertyConfig>		m_mapCustomTablePropertyConfig;		// �Զ�����Ϸ������������

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IDataBaseEngine *				m_pIDataBaseEngine;					//��������
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	ITCPSocketService *				m_pITCPSocketService;				//Э������

	//��Ƶ����
protected:
	WORD							m_wAVServerPort;					//��Ƶ�˿�
	DWORD							m_dwAVServerAddr;					//��Ƶ��ַ

	//��������
public:
	//���캯��
	CAttemperEngineSink();
	//��������
	virtual ~CAttemperEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//�¼��ӿ�
public:
	//�����¼�
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);
	//�Զ��¼�
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//��ȡ�¼�
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//���Ӵ���
protected:
	//ע���¼�
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//Զ�̷���
	bool OnTCPSocketMainRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�����¼�
protected:
	//��¼����
	bool OnTCPNetworkMainPCLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�б���
	bool OnTCPNetworkMainPCServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������
	bool OnTCPNetworkMainPCUserService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//Զ�̴���
	bool OnTCPNetworkMainPCRemoteService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�ֻ��¼�
protected:
	//��¼����
	bool OnTCPNetworkMainMBLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�б���
	bool OnTCPNetworkMainMBServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// ����ϵͳ������Ϣ
	bool OnTCPNetworkMainMBSystemMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�����¼�
protected:
	//I D ��¼
	bool OnTCPNetworkSubPCLogonGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�ʺŵ�¼
	bool OnTCPNetworkSubPCLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�ʺ�ע��
	bool OnTCPNetworkSubPCRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�ֻ��¼�
protected:
	//I D ��¼
	bool OnTCPNetworkSubMBLogonGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�ʺŵ�¼
	bool OnTCPNetworkSubMBLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�ʺ�ע��
	bool OnTCPNetworkSubMBRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// �����ֵ����
	bool OnTCPNetworkSubMBRechargeMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// ����ͷ�����
	bool OnTCPNetworkSubMBServiceMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// ��������¼
	bool OnTCPNetworkSubLogonThirdParty(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//������
protected:
	//������Ϸ��
	bool OnTCPNetworkUserSaveScore(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
 	//ȡ����Ϸ��
	bool OnTCPNetworkUserTakeScore(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// ת����Ϸ��
	bool OnTCPNetworkUserTransferScore(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯ����
	bool OnTCPNetworkQueryInsureInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// ͨ��GameID��ѯ�ǳ�
	bool OnTCPNetworkQueryNicknameByGameID(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// ��֤��������
	bool OnTCPNetworkVerifyInsurePassword(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// ��ѯ�����һ���Ϣ
	bool OnTCPNetworkQueryExchangeLoveliness(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// �һ�����ֵ
	bool OnTCPNetworkExchangeLoveliness(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// ����ɹ�
	bool OnTCPNetworkMBShareSuccess(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// �����Խ�����
	bool OnTCPNetworkUserCreateCustomTable(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// �����Խ�����
	bool OnTCPNetworkUserJoinCustomTable(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// ��ѯ������Խ�����
	bool OnTCPNetworkUserQueryCustomTable(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	// �û�ת����ʯ
	bool OnTCPNetworkUserTransferDiamond(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//��Ϸ�¼�
protected:
	//��¼�ɹ�
	bool OnDBPCLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnDBPCLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnDBPCLogonValidateMBCard(DWORD dwContextID, VOID * pData, WORD wDataSize);
//�������ϳɹ�
	bool OnDBPCProfileSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);	// add by chenj
	//��������ʧ��
	bool OnDBPCProfileFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);	// add by chenj
	//����ǳ�
	bool OnDBPCCheckNick(DWORD dwContextID, VOID * pData, WORD wDataSize);		// add by chenj
	//�û�ͷ��
	bool OnDBPCUserFaceInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�û���Ϣ
	bool OnDBPCUserIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBPCUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
    //ת�˼�¼
    bool OnDBPCUserTransRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���гɹ�
	bool OnDBPCUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����ʧ��
	bool OnDBPCUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�û���Ϣ
	bool OnDBPCUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����ɹ�
	bool OnDBPCOperateSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����ʧ��
	bool OnDBPCOperateFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// ת�˻�ִ
	bool OnDBPCUserTransReceipt(DWORD dwContextID,VOID * pData, WORD wDataSize);
	// ��ȡ�����һ����óɹ�
	bool OnDBPExchangeLovelinessInfo(DWORD dwContextID,VOID * pData, WORD wDataSize);
	// �����һ���������
	bool OnDBPExchangeLoveliness(DWORD dwContextID,VOID * pData, WORD wDataSize);
	// ǩ���ɹ�
	bool OnDBMBSignInCheckSuccess(DWORD dwContextID,VOID* pData,WORD wDataSize);
	// ��ȡ�Ĺ��ڿͷ���������Ϣ
	bool OnDBMBCustomServiceMessage(DWORD dwContextID,VOID* pData,WORD wDataSize);
	// ��ȡ���ڳ�ֵ��������Ϣ
	bool OnDBMBRechargeMessage(DWORD dwContextID,VOID* pData,WORD wDataSize);
	// �����Զ�������
	bool OnDBUserCustomTableSuccess(DWORD dwContextID, VOID* pData, WORD wDataSize);
	// �����Զ�������ʧ��
	bool OnDBUserCustomTableFailed(DWORD dwContextID, VOID* pData, WORD wDataSize);
	// ��ʯ�����ɹ�
	bool OnDBUserDiamondSuccess(DWORD dwContextID, VOID* pData, WORD wDataSize);
	// ��ʯ����ʧ��
	bool OnDBUserDiamondFialed(DWORD dwContextID, VOID* pData, WORD wDataSize);

	//�ֻ��¼�
protected:
	//��¼�ɹ�
	bool OnDBMBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnDBMBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�б��¼�
protected:
	//��Ϸ����
	bool OnDBPCGameTypeItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnDBPCGameKindItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ϸ�ڵ�
	bool OnDBPCGameNodeItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnDBPCGamePageItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����ID��ص�GameKindItem
	bool OnDBMBAreaGameKindItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����ID��ص�GameServerItem
	bool OnDBMBAreaGameServerItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// APP��ص���Դ�б�
	bool OnDBMBGameKindItemEx(DWORD dwContextID, VOID * pData, WORD wDataSize);
	// �Խ�����,���ĵ��ߺ;�������
	bool OnDBMBCustomTablePropertyConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ϸ�б�
	bool OnDBPCGameListResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��������
protected:
	//�汾���
	bool CheckPlazaVersion(BYTE cbDeviceType, DWORD dwPlazaVersion, DWORD dwSocketID, bool bCheckLowVer=true);
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
	// DeviceTypeתƽ̨ID
	WORD DeviceTypeToPlatformID(BYTE cbDeviceType);

	//�����б�
protected:
	//��������
	VOID SendGameTypeInfo(DWORD dwSocketID);
	//��������
	VOID SendGameKindInfo(DWORD dwSocketID);
	//���ͽڵ�
	VOID SendGameNodeInfo(DWORD dwSocketID, WORD wKindID);
	//���Ͷ���
	VOID SendGamePageInfo(DWORD dwSocketID, WORD wKindID);
	//���ͷ���
	VOID SendGameServerInfo(DWORD dwSocketID, WORD wKindID);

	//�ֻ��б�
protected:
	//��������
	VOID SendMobileTypeInfo(DWORD dwSocketID);
	//��������
	// wModuleID==INVALID_WORD ������ɸѡ,����ȫ��
	// dwAreaID==INVALID_DWORD ������ɸѡ,����ȫ��
	VOID SendMobileKindInfo(DWORD dwSocketID, WORD wModuleID, DWORD dwAreaID = INVALID_DWORD, WORD wPlatformID = CC_PLATFORM_UNKNOWN);
	//���ͷ���
	VOID SendMobileServerInfo(DWORD dwSocketID, WORD wModuleID, DWORD dwAreaID = INVALID_DWORD, WORD wPlatformID = CC_PLATFORM_UNKNOWN);
	// ����GameKind��չ��Ϣ
	VOID SendMobileKindInfoEx(DWORD dwSocketID, WORD wModuleID, DWORD dwAreaID = INVALID_DWORD, WORD wPlatformID = CC_PLATFORM_UNKNOWN);
	// ���ͼ����Զ������ӵ���Ϣ
	VOID SendUserCustomTableInfo(DWORD dwSocketID, WORD wTableID, TCHAR szSecret[LEN_CUSTOMTABLEPASSWORD], bool );
	// �����Խ����ӷ���ĵ���������Ϣ
	VOID SendCustomTablePropertyConfig(DWORD dwSocketID, WORD wModuleID, DWORD dwAreaID = INVALID_DWORD, WORD wPlatformID = CC_PLATFORM_UNKNOWN);
};

//////////////////////////////////////////////////////////////////////////////////

#endif