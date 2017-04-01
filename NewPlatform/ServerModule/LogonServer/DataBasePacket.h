#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//�������ݰ�

//��¼����
#define	DBR_GP_LOGON_GAMEID			1									//I D ��¼
#define	DBR_GP_LOGON_ACCOUNTS		2									//�ʺŵ�¼
#define DBR_GP_REGISTER_ACCOUNTS	3									//ע���ʺ�
#define DBR_GP_REGISTER_PROFILE		4									//��������
#define DBR_GP_CHECKNICK			5									//����ǳ�

//�˺ŷ���
#define DBR_GP_MODIFY_MACHINE		10									//�޸Ļ���
#define DBR_GP_MODIFY_LOGON_PASS	11									//�޸�����
#define DBR_GP_MODIFY_INSURE_PASS	12									//�޸�����
#define DBR_GP_MODIFY_UNDER_WRITE	13									//�޸�ǩ��
#define DBR_GP_MODIFY_INDIVIDUAL	14									//�޸�����

//ͷ������
#define DBR_GP_MODIFY_SYSTEM_FACE	20									//�޸�ͷ��
#define DBR_GP_MODIFY_CUSTOM_FACE	21									//�޸�ͷ��

//��������
#define DBR_GP_USER_SAVE_SCORE		30									//������Ϸ��
#define DBR_GP_USER_TAKE_SCORE		31									//��ȡ��Ϸ��
#define DBR_GP_USER_TRANSFER_SCORE	32									//ת����Ϸ��

//��ѯ����
#define DBR_GP_QUERY_INDIVIDUAL				40									//��ѯ����
#define DBR_GP_QUERY_INSURE_INFO			41									//��ѯ����
#define DBR_GP_QUERY_USER_INFO				42									//��ѯ�û�
#define DBR_GP_QUERY_TRANSRECORD			43
#define DBR_GP_QUERY_NICKNAME_BY_GAMEID		44									// ͨ��GameID��ѯ�ǳ�
#define DBR_GP_VERIFY_INSURE_PASSWORD		45									// ��֤��������
#define DBR_GP_QUERY_EXCHANGE_LOVELINESS	46							// ��ѯ�����һ���Ϣ
#define DBR_GP_EXCHANGE_LOVELINESS			47							// �һ�����

//ϵͳ����
#define DBR_GP_LOAD_GAME_LIST		50									//�����б�
#define DBR_GP_ONLINE_COUNT_INFO	51									//������Ϣ


//////////////////////////////////////////////////////////////////////////////////

//ID ��¼
struct DBR_GP_LogonGameID
{
	//��¼��Ϣ
	DWORD							dwGameID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	BYTE							cbNeeValidateMBCard;				//�ܱ�У��

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//�ʺŵ�¼
struct DBR_GP_LogonAccounts
{
	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	BYTE							cbNeeValidateMBCard;				//�ܱ�У��

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//�ʺ�ע��
struct DBR_GP_RegisterAccounts
{
	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szSpreader[LEN_ACCOUNTS];			//�Ƽ��ʺ�
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//֤������
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szMobile[LEN_MOBILE_PHONE];

	//�������
	TCHAR							szLogonPass[LEN_MD5];				//��¼����
	TCHAR							szInsurePass[LEN_MD5];				//��������

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

struct DBR_GP_RegisterProfile
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szQQ[LEN_QQ];						//Q Q ����
	TCHAR							szMobile[LEN_MOBILE_PHONE];			//�ֻ�����
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		// ���֤����
	TCHAR							szCompellation[LEN_COMPELLATION];	// ��ʵ����
	TCHAR							szSecurityQuestion[LEN_UNDER_WRITE];		// �ܱ�����
	TCHAR							szSecurityAnswer[LEN_UNDER_WRITE];		// �ܱ��ش�
	WORD							wFaceID;							// ͷ��ID	
};

struct DBR_GP_CheckNick
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
};//�޸Ļ���
struct DBR_GP_ModifyMachine
{
	BYTE							cbBind;								//�󶨱�־
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�޸�����
struct DBR_GP_ModifyLogonPass
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����
};

//�޸�����
struct DBR_GP_ModifyInsurePass
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����
};

//�޸�ǩ��
struct DBR_GP_ModifyUnderWrite
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
};

//�޸�ͷ��
struct DBR_GP_ModifySystemFace
{
	//�û���Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�޸�ͷ��
struct DBR_GP_ModifyCustomFace
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//ͼƬ��Ϣ

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�޸�����
struct DBR_GP_ModifyIndividual
{
	//��֤����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//�ʺ�����
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��

	//�û���Ϣ
	TCHAR							szUserNote[LEN_USER_NOTE];			//�û�˵��
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����

	//�绰����
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//�̶��绰
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ƶ��绰

	//��ϵ����
	TCHAR							szQQ[LEN_QQ];						//Q Q ����
	TCHAR							szEMail[LEN_EMAIL];					//�����ʼ�
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//��ϵ��ַ
};

//��ѯ����
struct DBR_GP_QueryIndividual
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
};

// ��ѯ�ǳ�
struct DBR_GP_QueryNickNameByGameID
{
	DWORD							dwGameID;							// �û�GameID
};

//������Ϸ��
struct DBR_GP_UserSaveScore
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lSaveScore;							//������Ϸ��
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ȡ����Ϸ��
struct DBR_GP_UserTakeScore
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lTakeScore;							//��ȡ��Ϸ��
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ת����Ϸ��
struct DBR_GP_UserTransferScore
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
	BYTE                            cbByNickName;                       //�ǳ�����
	SCORE							lTransferScore;						//ת����Ϸ��
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szPassword[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ѯ����
struct DBR_GP_QueryInsureInfo
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						//���ӵ�ַ
};

//��ѯ�û�
struct DBR_GP_QueryInsureUserInfo
{
	BYTE                            cbByNickName;                       //�ǳ�����
	TCHAR							szNickName[LEN_NICKNAME];			//Ŀ���û�
};

//�û�����
struct DBO_GP_UserTransferUserInfo
{
	DWORD							dwGameID;							//�û� I D
	TCHAR							szNickName[LEN_NICKNAME];			//�ʺ��ǳ�
};

//������Ϣ
struct DBR_GP_OnLineCountInfo
{
	WORD							wKindCount;							//������Ŀ
	DWORD							dwOnLineCountSum;					//��������
	tagOnLineInfoKind				OnLineCountKind[MAX_KIND];			//��������
};

// ��֤�û���������
struct DBR_GP_VerifyInsurePassword
{
	DWORD							dwUserID;						//�û� I D
	TCHAR							szInsurePassword[LEN_MD5];		//�ʺ��ǳ�
};

// ��ѯ�����һ�����Ϣ
struct DBR_GP_QueryExchangeLoveliness
{
	DWORD							dwUserID;							//�û� I D
};

// ��ѯ�����һ���Ϣ�ظ�
struct DBO_GP_ExchangeLovelinessInfo
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwLoveliness;						// �û�����
	DWORD							dwAvailbleAmount;					// �ɶһ�����
	DWORD							dwFinalAmount;						// ʵ���ܵõ��Ļ���(�ɶһ����� * �һ���)
	TCHAR							szTips[1024];						// �һ���ʾ
	TCHAR							szErr[128];							// ������ʾ
};

// ���������һ�
struct DBR_GP_ExchangeLoveliness
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientAddr;						// �û�IP
};

// ���������һ��Ľ��
struct DBO_GP_ExchangeLoveliness
{
	DWORD							dwUserID;							// �û�I D
	DWORD							dwLoveliness;						// �һ�������
	SCORE							lScore;								// �һ�����Ϸ����
	SCORE							lInsureScore;						// �һ������л���
	LONG							lResultCode;						// �������
	TCHAR							szDescribeString[128];				// ��ʾ��Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
//�����Ϣ

//��¼���
#define DBO_GP_LOGON_SUCCESS		100									//��¼�ɹ�
#define DBO_GP_LOGON_FAILURE		101									//��¼ʧ��
#define DBR_GP_VALIDATE_MBCARD		102									//��¼ʧ��
#define DBO_GP_PROFILE_SUCCESS		103									//�������ϳɹ�
#define DBO_GP_PROFILE_FAILURE		104									//��������ʧ��
#define DBO_GP_CHECKNICK			105									//

//�������
#define DBO_GP_USER_FACE_INFO		110									//�û�ͷ��
#define DBO_GP_USER_INDIVIDUAL		111									//�û�����

//��������
#define DBO_GP_USER_INSURE_INFO			120									//��������
#define DBO_GP_USER_INSURE_SUCCESS		121									//���гɹ�
#define DBO_GP_USER_INSURE_FAILURE		122									//����ʧ��
#define DBO_GP_USER_INSURE_USER_INFO	123									//�û�����
#define DBR_GP_TRANS_RECORD				124		                            //ת�˼�¼
#define DBO_GP_INSURE_TRANS_RECEIPT		125									// ת�˻�ִ	
#define DBO_GP_QUERY_EXCHANGE_LOVELINESS_INFO 126								// �����һ�������Ϣ
#define DBO_GP_EXCHANGE_LOVELINESS		127									// �����һ�

//�б���
#define DBO_GP_GAME_TYPE_ITEM				130									//������Ϣ
#define DBO_GP_GAME_KIND_ITEM				131									//������Ϣ
#define DBO_GP_GAME_NODE_ITEM				132									//�ڵ���Ϣ
#define DBO_GP_GAME_PAGE_ITEM				133									//������Ϣ
#define DBO_MB_AREA_GAME_KIND_ITEM			134									//����ID��ص�GameKindItem
#define DBO_MB_AREA_GAME_SERVER_ITEM		135									//����ID��ص�GameKindItem
#define DBO_MB_GAME_KIND_ITEM_EX			136									//GameKind��չ��Ϣ,����App
#define DBO_MB_CUSTOMTABLE_PROPERTYCONFIG	137									//�Խ����������������
#define DBO_GP_GAME_LIST_RESULT				140									//���ؽ��

//������
#define DBO_GP_OPERATE_SUCCESS		500									//�����ɹ�
#define DBO_GP_OPERATE_FAILURE		501									//����ʧ��

#define DBO_GR_USER_CUSTOMTABLE_SUCCESS		900							// �û��Զ�������/�����Զ������ӳɹ�
#define DBO_GR_USER_CUSTOMTABLE_FAILURE		901							// �û��Զ�������/�����Զ�������ʧ��
#define DBO_MB_USER_DIAMOND_SUCCESS			902							// �û�������ʯ�ɹ�
#define DBO_MB_USER_DIAMOND_FAILURE			903							// �û�������ʯʧ��

//��¼�ɹ�
struct DBO_GP_LogonSuccess
{
	//��������
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	DWORD							dwGroupID;							//���ű�ʶ
	DWORD							dwCustomID;							//�Զ�����
	DWORD							dwUserMedal;						//�û�����
	DWORD							dwExperience;						//������ֵ
	DWORD							dwLoveLiness;						//�û�����
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������

	//�û���Ϸ��
	SCORE							lUserScore;							//�û���Ϸ��
	SCORE							lUserInsure;						//�û�����

	//�û�����
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMoorMachine;						//��������
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��

	//��Ա����
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	SYSTEMTIME						MemberOverDate;						//����ʱ��

	WORD							wFillIn;							//�Ƿ�����ע������ by chenj

	//������Ϣ
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��¼ʧ��
struct DBO_GP_LogonFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��¼ʧ��
struct DBR_GP_ValidateMBCard
{
	UINT							uMBCardID;						//��������
};

//��������
struct DBO_GP_UserProfile
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szQQ[LEN_QQ];						//Q Q ����
	TCHAR							szMobile[LEN_MOBILE_PHONE];			//�ֻ�����
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
	WORD							wFaceID;							//ͷ���ʶ
};

struct DBO_GP_CheckNick
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//������Ϣ
};
//ͷ����Ϣ
struct DBO_GP_UserFaceInfo
{
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwCustomID;							//�Զ�����
};

//��������
struct DBO_GP_UserIndividual
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szUserNote[LEN_USER_NOTE];			//�û�˵��
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����

	//�绰����
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//�̶��绰
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ƶ��绰

	//��ϵ����
	TCHAR							szQQ[LEN_QQ];						//Q Q ����
	TCHAR							szEMail[LEN_EMAIL];					//�����ʼ�
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//��ϵ��ַ
};

//��������
struct DBO_GP_UserInsureInfo
{
	WORD							wRevenueTake;						//˰�ձ���
	WORD							wRevenueTransfer;					//˰�ձ���
	WORD							wServerID;							//�����ʶ
	SCORE							lUserScore;							//�û���Ϸ��
	SCORE							lUserInsure;						//������Ϸ��
	SCORE							lTransferPrerequisite;				//ת������
	DWORD							dwDiamondCount;						// ��ʯ����
};
//ת�˼�¼
struct DBR_GP_UserTransRecord
{
    DWORD                               dwGameID;
    TCHAR                               szSourceAccount[LEN_ACCOUNTS];
    TCHAR                               szTargetAccounts[LEN_ACCOUNTS];
    LONGLONG                            lSwapScore;
    LONGLONG                            lRevenue;
    TCHAR                               szTime[32];
	DWORD								dwSourceID;
	DWORD								dwTargetID;
    bool                                bOver;
};
//���гɹ�
struct DBO_GP_UserInsureSuccess
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lSourceScore;						//ԭ����Ϸ��
	SCORE							lSourceInsure;						//ԭ����Ϸ��
	SCORE							lInsureRevenue;						//����˰��
	SCORE							lVariationScore;					//��Ϸ�ұ仯
	SCORE							lVariationInsure;					//���б仯
	TCHAR							szDescribeString[128];				//������Ϣ
};

//����ʧ��
struct  DBO_GP_UserInsureFailure
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//����ʧ��
struct DBO_GP_OperateFailure
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�����ɹ�
struct DBO_GP_OperateSuccess
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
struct DBO_GP_GameType
{
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wTypeID;							//��������
	TCHAR							szTypeName[LEN_TYPE];				//��������
};

//��Ϸ����
struct DBO_GP_GameKind
{
	WORD							wTypeID;							//��������
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wKindID;							//��������
	WORD							wGameID;							//ģ������
	TCHAR							szKindName[LEN_KIND];				//��Ϸ����
	TCHAR							szProcessName[LEN_PROCESS];			//��������
};

//��Ϸ�ڵ�
struct DBO_GP_GameNode
{
	WORD							wKindID;							//��������
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wNodeID;							//�ڵ�����
	TCHAR							szNodeName[LEN_NODE];				//�ڵ�����
};

//��������
struct DBO_GP_GamePage
{
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	WORD							wPageID;							//��������
	WORD							wOperateType;						//��������
	TCHAR							szDisplayName[LEN_PAGE];			//��ʾ����
};

// ����ID��KindIDӳ��(�ֻ��õ�)
struct DBO_MB_AreaGameKind
{
	DWORD							dwAreaID;							//��������
	WORD							wKindID;							//��������
	WORD							wSortID;							//��������
	WORD							wPlatformID;						//ϵͳ���
};

// ����ID��KindIDӳ��(�ֻ��õ�)
struct DBO_MB_AreaGameServer
{
	DWORD							dwAreaID;							//��������
	WORD							wServerID;							//��������
	WORD							wSortID;							//��������
	TCHAR							szServerName[LEN_SERVER];			//��������
	WORD							wPlatformID;						//ϵͳ���
};

// App��Դ�����õ�
struct DBO_MB_GameKindEx
{
	WORD		wKindID;
	DWORD		dwAppVersion;
	TCHAR		szAppDownloadURL[LEN_URL];			// ��ַ����
};

// �Խ�����,���ĵ��ߺ;���������
struct DBO_MB_CustomTablePropertyConfig
{
	WORD		wKindID;							// ��Ϸ���ͱ�ʶ
	WORD		wPropertyConsumeCount;				// ������������
	WORD		wRoundCount;						// ��Ӧ�ľ���
};

//���ؽ��
struct DBO_GP_GameListResult
{
	BYTE							cbSuccess;							//�ɹ���־
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//�ֻ����ݰ�

//��¼����
#define	DBR_MB_LOGON_GAMEID			500									//I D ��¼
#define	DBR_MB_LOGON_ACCOUNTS		501									//�ʺŵ�¼
#define DBR_MB_REGISTER_ACCOUNTS	502									//ע���ʺ�
#define DBR_TP_LOGON_ACCOUNTS		503									//��������¼�˺�

//��¼���
#define DBO_MB_LOGON_SUCCESS		600									//��¼�ɹ�
#define DBO_MB_LOGON_FAILURE		601									//��¼ʧ��

//////////////////////////////////////////////////////////////////////////////////

//ID ��¼
struct DBR_MB_LogonGameID
{
	//��¼��Ϣ
	DWORD							dwGameID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//�ʺŵ�¼
struct DBR_MB_LogonAccounts
{
	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

// �������˺ŵ�¼
struct DBR_TP_LogonAccounts
{
	WORD							wFaceID;							//ͷ���ʶ

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
//	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����

	unsigned int					nThirdPartyID;						// ������ID
	TCHAR							szSessionID[LEN_SESSION_ID];		// SessionID
	TCHAR							szNickName[LEN_NICKNAME];			// ����ǳ�

	// ����ID
	DWORD							dwAreaID;							//����ID

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//�ʺ�ע��
struct DBR_MB_RegisterAccounts
{
	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�

	//�������
	TCHAR							szLogonPass[LEN_MD5];				//��¼����
	TCHAR							szInsurePass[LEN_MD5];				//��������

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
	DWORD							dwAreaID;							//����ID

	//������Ϣ
	LPVOID							pBindParameter;						//�󶨲���
};

//��¼�ɹ�
struct DBO_MB_LogonSuccess
{
	//�û�����
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwExperience;						//������ֵ
	DWORD							dwLoveLiness;						//�û�����
	SCORE							lUserScore;							//�û����
	SCORE							lUserInsure;						//�û�����	.
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��˺�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�

	//������Ϣ
	TCHAR							szDescribeString[128];				//������Ϣ

	// �û�����ID
	DWORD							dwAreaID;							// �û�����ID
	WORD							wPlatformID;						// �û�ϵͳID

	BYTE							cbMemberOrder;						//��Ա�ȼ�

	// �û�������Զ��巿��
	WORD							wServerID;
	TCHAR							szSecret[LEN_CUSTOMTABLEPASSWORD];
	bool							bLockedInTable;						// �û�����������
	DWORD							dwDiamondCount;						// ��ʯ��Ŀ
	TCHAR							szAgentInfo[LEN_URL];				// ����΢����Ϣ
};


//��¼ʧ��
struct DBO_MB_LogonFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//�ֻ����ݰ�

//ǩ������
#define	DBR_MB_SIGN_IN_CHECK				600							//��¼ǩ��

// ǩ�����
#define	DBO_MB_SIGN_IN_CHECK_SUCCESS		300							// ǩ���ɹ�
#define DBO_MB_SIGN_IN_CHECK_FAILURE		301							// ǩ��ʧ��

struct DBR_MB_SiginInCheck
{
	DWORD							dwUserID;							//�û� I D
};

struct DBO_MB_SignInCheck
{
	DWORD							dwErrorCode;						// ��ʶ��
	DWORD							dwCheckDay;							// ǩ��������
};


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//���󹫸�
#define DBR_MB_RECHARGE_MESSAGE				700								// ��ֵ����
#define DBR_MB_CUSTOM_SERVICE_MESSAGE 		701								// �ͷ�����

#define DBO_MB_RECHARGE_MESSAGE				710								// ���ݿ�ظ���ֵ����
#define DBO_MB_CUSTOM_SERVICE_MESSAGE		711								// ���ݿ�ظ��ͷ�����

//////////////////////////////////////////////////////////////////////////////////

#define DBR_MB_SHARE_SUCCESS				702								// ����ɹ�
#define DBR_MB_USER_JOIN_CUSTOMTABLE		703								// �û������Զ�������
#define DBR_MB_USER_QUERY_CUSTOMTABLE		704								// ��ѯ�Լ����������
#define DBR_MB_USER_TRANSFER_DIAMOND		705								// �û�ת����ʯ
struct DBR_MB_ShareSuccess
{
	DWORD							dwUserID;								// �û�ID
	TCHAR							szMachineID[LEN_MACHINE_ID];			//��������

	//������Ϣ
	DWORD							dwClientAddr;						//���ӵ�ַ
	LPVOID							pBindParameter;							//�󶨲���
};

// �������ݿ���뷿��
struct DBR_MB_UserJoinCustomTable
{
	DWORD							dwUserID;								// ���������û�UserID
	TCHAR							szSecret[LEN_CUSTOMTABLEPASSWORD];		// ��������
};

// �������ݿ��ѯ������Ϣ
struct DBR_MB_UserQueryCustomTable
{
	DWORD							dwUserID;								// ��ѯ���û�ID
};

//ת����Ϸ��
struct DBR_MB_UserTransferDiamond
{
	DWORD							dwUserID;							//�����û� I D
	DWORD							dwGameID;							//�����û� I D 
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwTransferDiamond;					//ת����ʯ
	TCHAR							szPassword[LEN_PASSWORD];			//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

// �Խ����ӳɹ�
struct DBO_GR_UserCustomTableSuccess
{
	DWORD							dwUserID;							// �û�ID
	WORD							wKindID;							// ��Ϸ����
	WORD							wServerID;							// ������
	TCHAR							szSecret[LEN_CUSTOMTABLEPASSWORD];	// �Խ���������
	bool							bLockedInTable;						// ��������������
	DWORD							dwUserPropertyCount;				// ��ҵ�����Ŀ
};

//�Խ�����ʧ��
struct DBO_GR_UserCustomTableFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

// ��ʯ�����ɹ�
struct DBO_MB_UserDiamondSuccess
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwDiamondCount;						//ԭ������ʯ
	TCHAR							szDescribeString[128];				//������Ϣ
};

// ��ʯ����ʧ��
struct DBO_MB_UserDiamondFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

#endif
