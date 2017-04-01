#ifndef MODULE_MANAGER_HEAD_HEAD_FILE
#define MODULE_MANAGER_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//ƽ̨����
#include "..\..\GlobalDefine\Platform.h"

//�������
#include "..\..\PublicModule\ServiceCore\ServiceCoreHead.h"
#include "..\GameService\GameServiceHead.h"
#include "..\KernelEngine\KernelEngineHead.h"

#include <map>
using namespace std;

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef MODULE_MANAGER_CLASS
	#ifdef  MODULE_MANAGER_DLL
		#define MODULE_MANAGER_CLASS _declspec(dllexport)
	#else
		#define MODULE_MANAGER_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManager.dll")			//��� DLL ����
#else
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManagerD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//ģ����Ϣ
struct tagGameModuleInfo
{
	//�汾��Ϣ
	DWORD							dwClientVersion;					//��Ϸ�汾
	DWORD							dwServerVersion;					//����汾
	DWORD							dwNativeVersion;					//���ذ汾

	//��������
	WORD							wGameID;							//ģ���ʶ
	TCHAR							szDataBaseAddr[64];					//���ݿ���
	TCHAR							szDataBaseName[64];					//���ݿ���

	//ģ������
	TCHAR							szGameName[LEN_KIND];				//��Ϸ����
	TCHAR							szServerDLLName[LEN_PROCESS];		//��������
	TCHAR							szClientEXEName[LEN_PROCESS];		//��������
};

//����������Ϣ(�����ݿ����ж����������Ϣ)
struct tagDBGameTableInfo
{
	//��������
	WORD							wServerID;							// �������
	WORD							wStartTableID;						//���ſ�ʼID
	WORD							wEndTableID;						//���Ž���ID

	tagGameTableOption				TableOption;						// ��������
};

// �����ݿ��ж�ȡ����Ҵ���������Ϣ
struct tagDBUserCustomTableInfo
{
	DWORD			dwUserID;											// �������û�ID
	WORD			wTableID;											// ռ�õ�����
	TCHAR			szSecret[LEN_CUSTOMTABLEPASSWORD];					// �Խ���������
	BYTE			cbCustomRule[1024];									// �Զ�����
	DWORD			dwRoundCount;										// �����ľ���
	SYSTEMTIME		TableCreateTime;									// ������ʱ��
	DWORD			dwPlayRoundCount;									// �Ѿ���Ϸ�ľ���

	tagDBUserCustomTableInfo()
	{
		memset(this, 0, sizeof(tagDBUserCustomTableInfo));
	}
};

//������Ϣ(�����ݿ��ж����������Ϣ)
struct tagGameServerInfo
{
	//��������
	WORD							wGameID;							//ģ���ʶ
	WORD							wServerID;							//�������

	//�ҽ�����
	WORD							wKindID;							//�ҽ�����
	WORD							wNodeID;							//�ҽӽڵ�
	WORD							wSortID;							//���б�ʶ

	//�汾��Ϣ
	DWORD							dwClientVersion;					//��Ϸ�汾
	DWORD							dwServerVersion;					//����汾
	DWORD							dwNativeVersion;					//���ذ汾

	//ģ������
	TCHAR							szGameName[LEN_KIND];				//��Ϸ����
	TCHAR							szServerDLLName[LEN_PROCESS];		//��������
	TCHAR							szClientEXEName[LEN_PROCESS];		//��������

	//˰������
	LONG							lCellScore;							//��λ����
	WORD							wRevenueRatio;						//˰�ձ���
	SCORE							lServiceScore;						//�������

	//��������
	SCORE							lRestrictScore;						//���ƻ���
	SCORE							lMinTableScore;						//��ͻ���
	SCORE							lMinEnterScore;						//��ͻ���
	SCORE							lMaxEnterScore;						//��߻���

	//��Ա����
	BYTE							cbMinEnterMember;					//��ͻ�Ա
	BYTE							cbMaxEnterMember;					//��߻�Ա

	//��������
	DWORD							dwServerRule;						//�������
	DWORD							dwAttachUserRight;					//����Ȩ��

	//��������
	WORD							wMaxPlayer;							//�����Ŀ
	WORD							wTableCount;						//������Ŀ
	WORD							wServerPort;						//����˿�
	WORD							wServerType;						//��������
	TCHAR							szServerName[LEN_SERVER];			//��������

	//��������
	BYTE							cbDistributeRule;					//�������
	WORD							wMinDistributeUser;					//��������
	WORD							wMaxDistributeUser;					//�������
	WORD							wDistributeTimeSpace;				//������
	WORD							wDistributeDrawCount;				//�������
	WORD							wDistributeStartDelay;				//��ʼ��ʱ

	//��������
	TCHAR							szDataBaseName[64];					//���ݿ���
	TCHAR							szDataBaseAddr[64];					//��ַ��Ϣ

	//��������
//	BYTE							cbCustomRule[1024];					//�Զ�����
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//��������

	vector<tagDBGameTableInfo>			TableConfig;					// ��������
	vector<tagDBUserCustomTableInfo>	vectUserCustomTableInfo;		// ����Խ���������
};


//������Ϣ
struct tagGameServerCreate
{
	//��������
	WORD							wGameID;							//ģ���ʶ
	WORD							wServerID;							//�������

	//�ҽ�����
	WORD							wKindID;							//�ҽ�����
	WORD							wNodeID;							//�ҽӽڵ�
	WORD							wSortID;							//���б�ʶ

	//˰������
	LONG							lCellScore;							//��λ����
	WORD							wRevenueRatio;						//˰�ձ���
	SCORE							lServiceScore;						//�������

	//��������
	SCORE							lRestrictScore;						//���ƻ���
	SCORE							lMinTableScore;						//��ͻ���
	SCORE							lMinEnterScore;						//��ͻ���
	SCORE							lMaxEnterScore;						//��߻���

	//��Ա����
	BYTE							cbMinEnterMember;					//��ͻ�Ա
	BYTE							cbMaxEnterMember;					//��߻�Ա

	//��������
	DWORD							dwServerRule;						//�������
	DWORD							dwAttachUserRight;					//����Ȩ��

	//��������
	WORD							wMaxPlayer;							//�����Ŀ
	WORD							wTableCount;						//������Ŀ
	WORD							wServerPort;						//����˿�
	WORD							wServerType;						//��������
	TCHAR							szServerName[LEN_SERVER];			//��������

	//��������
	BYTE							cbDistributeRule;					//�������
	WORD							wMinDistributeUser;					//��������
	WORD							wMaxDistributeUser;					//�������
	WORD							wDistributeTimeSpace;				//������
	WORD							wDistributeDrawCount;				//�������
	WORD							wDistributeStartDelay;				//��ʼ��ʱ

	//��������
	TCHAR							szDataBaseName[32];					//���ݿ���
	TCHAR							szDataBaseAddr[64];					//��ַ��Ϣ

	//��������
	BYTE							cbCustomRule[1024];					//�Զ�����
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//��������
};



//////////////////////////////////////////////////////////////////////////////////
//���ò���
struct tagModuleInitParameter
{
	tagGameServiceAttrib			GameServiceAttrib;					//��������
	tagGameServiceOption			GameServiceOption;					//��������
	tagGameSquareOption				GameSquareOptionArray[20];			//��������
	BYTE							cbGameSquareOptionArrayCount;		//��������
//	tagGameTableInfo				TableConfig[MAX_TABLE_OPTION];		//��������(�����ݿ���ص�ӳ������)
	vector<tagDBGameTableInfo>		TableConfig;						// ��������
	tagGameServiceTableOption		GameServiceTableOption;				//������������
};

//////////////////////////////////////////////////////////////////////////////////

//�����ļ�
#ifndef MODULE_MANAGER_DLL 
	
	#include "ListControl.h"
	#include "DlgServerItem.h"
	#include "DlgServerMatch.h"
	#include "DlgServerWizard.h"
	
	#include "ModuleListControl.h"
	#include "ModuleDBParameter.h"
	#include "ModuleInfoManager.h"

	#include "ServerCustomRule.h"
	#include "ServerListControl.h"
	#include "ServerInfoManager.h"

#endif

//////////////////////////////////////////////////////////////////////////////////

#endif