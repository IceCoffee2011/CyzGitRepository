#ifndef CMD_LONGON_HEAD_FILE
#define CMD_LONGON_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//��¼����

#define MDM_GP_LOGON				1									//�㳡��¼

//��¼ģʽ
#define SUB_GP_LOGON_GAMEID			1									//I D ��¼
#define SUB_GP_LOGON_ACCOUNTS		2									//�ʺŵ�¼
#define SUB_GP_REGISTER_ACCOUNTS	3									//ע���ʺ�
#define SUB_GP_REGISTER_PROFILE		4									//��������	add by chenj
#define SUB_GP_CHECKNICK			5									//����ǳ�

//��¼���
#define SUB_GP_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_GP_LOGON_FAILURE		101									//��¼ʧ��
#define SUB_GP_LOGON_FINISH			102									//��¼���
#define SUB_GP_VALIDATE_MBCARD      103                                 //��¼ʧ��
#define SUB_GP_REGISTER_SUCCESS		105									//ע��ɹ�
#define SUB_GP_PROFILE_SUCCESS		106									//�������ϳɹ�
#define SUB_GP_PROFILE_FAILURE		107									//��������ʧ��
#define SUB_GP_CHECKNICK_RES		108									//����ǳ�

//������ʾ
#define SUB_GP_UPDATE_NOTIFY		200									//������ʾ

//////////////////////////////////////////////////////////////////////////////////
//

#define MB_VALIDATE_FLAGS           0x01                                //Ч���ܱ�
#define LOW_VER_VALIDATE_FLAGS      0x02                                //Ч��Ͱ汾

//I D ��¼
struct CMD_GP_LogonGameID
{
	//ϵͳ��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//��¼��Ϣ
	DWORD							dwGameID;							//��Ϸ I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
	BYTE							cbValidateFlags;			        //У���ʶ
};

//�ʺŵ�¼
struct CMD_GP_LogonAccounts
{
	//ϵͳ��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	BYTE							cbValidateFlags;			        //У���ʶ
};

//ע���ʺ�
struct CMD_GP_RegisterAccounts
{
	//ϵͳ��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//�������
	TCHAR							szLogonPass[LEN_MD5];				//��¼����
	TCHAR							szInsurePass[LEN_MD5];				//��������

	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szSpreader[LEN_ACCOUNTS];			//�Ƽ��ʺ�
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//֤������
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szMobile[LEN_MOBILE_PHONE];
	BYTE							cbValidateFlags;			        //У���ʶ
};

//��¼�ɹ�
struct CMD_GP_LogonSuccess
{
	//��������
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwGroupID;							//���ű�ʶ
	DWORD							dwCustomID;							//�Զ���ʶ
	DWORD							dwUserMedal;						//�û�����
	DWORD							dwExperience;						//������ֵ
	DWORD							dwLoveLiness;						//�û�����

	//�û��ɼ�
	SCORE							lUserScore;							//�û����
	SCORE							lUserInsure;						//�û�����

	//�û���Ϣ
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMoorMachine;						//��������
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������

	//������Ϣ
	BYTE                            cbShowServerStatus;                 //��ʾ������״̬
	WORD							wFillIn;							//�Ƿ񲹳����� by chenj 20140421
};

struct CMD_GP_UserProfile
{
	DWORD							dwUserID;							//�û�ID
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

struct CMD_GP_CheckNick
{
	DWORD							dwUserID;
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
};

struct CMD_GP_CheckNick_Res
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��¼ʧ��
struct CMD_GP_LogonFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��½���
struct CMD_GP_LogonFinish
{
	WORD							wIntermitTime;						//�ж�ʱ��
	WORD							wOnLineCountTime;					//����ʱ��
};

//��¼ʧ��
struct CMD_GP_ValidateMBCard
{
	UINT								uMBCardID;						//��������
};

//������ʾ
struct CMD_GP_UpdateNotify
{
	BYTE							cbMustUpdate;						//ǿ������
	BYTE							cbAdviceUpdate;						//��������
	DWORD							dwCurrentVersion;					//��ǰ�汾
};

//////////////////////////////////////////////////////////////////////////////////
//Я����Ϣ CMD_GP_LogonSuccess

#define DTP_GP_GROUP_INFO			1									//������Ϣ
#define DTP_GP_MEMBER_INFO			2									//��Ա��Ϣ
#define	DTP_GP_UNDER_WRITE			3									//����ǩ��
#define DTP_GP_STATION_URL			4									//��ҳ��Ϣ

//������Ϣ
struct DTP_GP_GroupInfo
{
	DWORD							dwGroupID;							//��������
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������
};

//��Ա��Ϣ
struct DTP_GP_MemberInfo
{
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	SYSTEMTIME						MemberOverDate;						//����ʱ��
};

//////////////////////////////////////////////////////////////////////////////////
//�б�����

#define MDM_GP_SERVER_LIST			2									//�б���Ϣ

//��ȡ����
#define SUB_GP_GET_LIST				1									//��ȡ�б�
#define SUB_GP_GET_SERVER			2									//��ȡ����
#define SUB_GP_GET_ONLINE			3									//��ȡ����
#define SUB_GP_GET_COLLECTION		4									//��ȡ�ղ�

//�б���Ϣ
#define SUB_GP_LIST_TYPE			100									//�����б�
#define SUB_GP_LIST_KIND			101									//�����б�
#define SUB_GP_LIST_NODE			102									//�ڵ��б�
#define SUB_GP_LIST_PAGE			103									//�����б�
#define SUB_GP_LIST_SERVER			104									//�����б�
#define SUB_GP_VIDEO_OPTION			105									//��Ƶ����

//�����Ϣ
#define SUB_GP_LIST_FINISH			200									//�������
#define SUB_GP_SERVER_FINISH		201									//�������

//������Ϣ
#define SUB_GR_KINE_ONLINE			300									//��������
#define SUB_GR_SERVER_ONLINE		301									//��������

//////////////////////////////////////////////////////////////////////////////////

//��ȡ����
struct CMD_GP_GetOnline
{
	WORD							wServerCount;						//������Ŀ
	WORD							wOnLineServerID[MAX_SERVER];		//�����ʶ
};

//��������
struct CMD_GP_KindOnline
{
	WORD							wKindCount;							//������Ŀ
	tagOnLineInfoKind				OnLineInfoKind[MAX_KIND];			//��������
};

//��������
struct CMD_GP_ServerOnline
{
	WORD							wServerCount;						//������Ŀ
	tagOnLineInfoServer				OnLineInfoServer[MAX_SERVER];		//��������
};

//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GP_USER_SERVICE			3									//�û�����

//�˺ŷ���
#define SUB_GP_MODIFY_MACHINE		100									//�޸Ļ���
#define SUB_GP_MODIFY_LOGON_PASS	101									//�޸�����
#define SUB_GP_MODIFY_INSURE_PASS	102									//�޸�����
#define SUB_GP_MODIFY_UNDER_WRITE	103									//�޸�ǩ��

//�޸�ͷ��
#define SUB_GP_USER_FACE_INFO		200									//ͷ����Ϣ
#define SUB_GP_SYSTEM_FACE_INFO		201									//ϵͳͷ��
#define SUB_GP_CUSTOM_FACE_INFO		202									//�Զ�ͷ��

//��������
#define SUB_GP_USER_INDIVIDUAL				301							//��������
#define	SUB_GP_QUERY_INDIVIDUAL				302							//��ѯ��Ϣ
#define SUB_GP_MODIFY_INDIVIDUAL			303							//�޸�����
#define SUB_GP_QUERY_NICKNAME_BY_GAMEID		304							// ͨ��GameID��ѯ�ǳ�
#define SUB_GP_QUERY_EXCHANGE_LOVELINESS	305							// ��ѯ�û��Ŀ�ת������Ϣ
#define SUB_GP_EXCHANGE_LOVELINESS_INFO		306							// ���ؿ˶�������Ϣ
#define SUB_GP_EXCHANGE_LOVELINESS			307							// ����һ���������	
#define SUB_GP_EXCHANGE_LOVELINESS_RESULT	308							// �һ��������

//���з���
#define SUB_GP_USER_SAVE_SCORE				400								//������
#define SUB_GP_USER_TAKE_SCORE				401								//ȡ�����
#define SUB_GP_USER_TRANSFER_SCORE			402								//ת�˲���
#define SUB_GP_USER_INSURE_INFO				403								//��������
#define SUB_GP_QUERY_INSURE_INFO			404								//��ѯ����
#define SUB_GP_USER_INSURE_SUCCESS			405								//���гɹ�
#define SUB_GP_USER_INSURE_FAILURE			406								//����ʧ��
#define SUB_GP_QUERY_USER_INFO_REQUEST		407								//��ѯ�û�
#define SUB_GP_QUERY_USER_INFO_RESULT		408								//�û���Ϣ
#define SUB_GP_QUERY_TRANSRECORD			409                             //ת�˼�¼
#define SUB_GP_TRANS_RECORD					410
#define SUB_GP_VERIFY_INSURE_PASSWORD		411								// ��֤��������
#define SUB_GP_USER_TRANSFER_RECEIPT		412								// ����ת�˻�ִ	
#define SUB_MB_USER_CUSTOM_TABLE			600								// �û������Զ�������
#define SUB_MB_USER_JOIN_CUSTOMTABLE		601								// �û������Զ�������
#define SUB_MB_USER_QUERY_CUSTOMTABLE		602								// �û���ѯ�Լ����Զ������ӷ������Ϣ	
#define SUB_MB_USER_TRANSFER_DIAMOND		603								// �û�ת����ʯ

#define SUB_MB_SHARE_SUCCESS			500								// app����ɹ�,�춹

//�������
#define SUB_GP_OPERATE_SUCCESS			900								//�����ɹ�
#define SUB_GP_OPERATE_FAILURE			901								//����ʧ��

#define SUB_MB_USER_CUSTOM_TABLE_SUCCESS	600							// �Խ����ӳɹ�
#define SUB_MB_USER_CUSTOM_TABLE_FAILED		601							// �Խ�����ʧ��
#define SUB_MB_USER_DIAMOND_SUCCESS			602							// ��ʯ����ʧ��
#define SUB_MB_USER_DIAMOND_FAILURE			603							// ��ʯ����ʧ��

//////////////////////////////////////////////////////////////////////////////////

//�޸�����
struct CMD_GP_ModifyLogonPass
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����
};

//�޸�����
struct CMD_GP_ModifyInsurePass
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����
};

//�޸�ǩ��
struct CMD_GP_ModifyUnderWrite
{
	DWORD							dwUserID;							//�û� I D
//	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
};

//////////////////////////////////////////////////////////////////////////////////

//�û�ͷ��
struct CMD_GP_UserFaceInfo
{
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwCustomID;							//�Զ���ʶ
};

//�޸�ͷ��
struct CMD_GP_SystemFaceInfo
{
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�޸�ͷ��
struct CMD_GP_CustomFaceInfo
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//ͼƬ��Ϣ
};

//////////////////////////////////////////////////////////////////////////////////

//�󶨻���
struct CMD_GP_ModifyMachine
{
	BYTE							cbBind;								//�󶨱�־
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};


//////////////////////////////////////////////////////////////////////////////////

//��������
struct CMD_GP_UserIndividual
{
	DWORD							dwUserID;							//�û� I D
};

//��ѯ��Ϣ
struct CMD_GP_QueryIndividual
{
	DWORD							dwUserID;							//�û� I D
};

//�޸�����
struct CMD_GP_ModifyIndividual
{
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};

// ��ѯ�ǳ�
struct CMD_GP_QueryNickNameByGameID
{
	DWORD							dwGameID;							// �û�GameID
};

// ��ѯ����ת����Ϣ
struct CMD_GP_QueryExchangeLoveliness
{
	DWORD							dwUserID;							//�û���ʶ
};

// �ظ���ѯ��������ת����Ϣ
struct CMD_GP_ExchangeLovelinessInfo
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwLoveliness;						// �û�����
	DWORD							dwAvailbleAmount;					// �ɶһ�����
	DWORD							dwFinalAmount;						// ʵ���ܵõ��Ļ���(�ɶһ����� * �һ���)
	TCHAR							szErr[128];							// ������ʾ
	TCHAR							szTips[1024];						// �һ���ʾ
};

// ���������һ�
struct CMD_GP_ExchangeLoveliness
{
	DWORD							dwUserID;							//�û���ʶ
};

struct CMD_GP_ExchangeLovelinessResult
{
	DWORD							dwUserID;							// �û�I D
	DWORD							dwLoveliness;						// �һ�������
	SCORE							lScore;								// �һ�����Ϸ����
	SCORE							lInsureScore;						// �һ������л���
	LONG							lResultCode;						// �������
	TCHAR							szDescribeString[128];				// ��ʾ��Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
//Я����Ϣ CMD_GP_UserIndividual

#define DTP_GP_UI_NICKNAME			1									//�û��ǳ�
#define DTP_GP_UI_USER_NOTE			2									//�û�˵��
#define DTP_GP_UI_UNDER_WRITE		3									//����ǩ��
#define DTP_GP_UI_QQ				4									//Q Q ����
#define DTP_GP_UI_EMAIL				5									//�����ʼ�
#define DTP_GP_UI_SEAT_PHONE		6									//�̶��绰
#define DTP_GP_UI_MOBILE_PHONE		7									//�ƶ��绰
#define DTP_GP_UI_COMPELLATION		8									//��ʵ����
#define DTP_GP_UI_DWELLING_PLACE	9									//��ϵ��ַ

//////////////////////////////////////////////////////////////////////////////////

//��������
struct CMD_GP_UserInsureInfo
{
	WORD							wRevenueTake;						//˰�ձ���
	WORD							wRevenueTransfer;					//˰�ձ���
	WORD							wServerID;							//�����ʶ
	SCORE							lUserScore;							//�û����
	SCORE							lUserInsure;						//���н��
	SCORE							lTransferPrerequisite;				//ת������
	DWORD							dwDiamondCount;						// ��ʯ��������
};
//ת�˼�¼
struct CMD_GP_RealTransRecord
{
    DWORD                               dwGameID;
    TCHAR                               szSourceAccount[LEN_ACCOUNTS];
    TCHAR                               szTargetAccounts[LEN_ACCOUNTS];
    LONGLONG                            lSwapScore;
    LONGLONG                            lRevenue;
    TCHAR                               szTime[LEN_ACCOUNTS];
	DWORD								dwSourceID;
	DWORD								dwTargetID;
    bool                                bOver;
};

//������
struct CMD_GP_UserSaveScore
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lSaveScore;							//������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ���
struct CMD_GP_UserTakeScore
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lTakeScore;							//��ȡ���
	TCHAR							szPassword[LEN_MD5];				//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ת�˽��
struct CMD_GP_UserTransferScore
{
	DWORD							dwUserID;							//�û� I D
	BYTE                            cbByNickName;                       //�ǳ�����
	SCORE							lTransferScore;						//ת�˽��
	TCHAR							szPassword[LEN_MD5];				//��������
	TCHAR							szNickName[LEN_NICKNAME];			//Ŀ���û�
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

// ����ת�˻�ִ
struct CMD_GR_UserTransferReceipt
{
	DWORD							dwRecordID;							// ��ִ���
	DWORD							dwSourceUserID;						// ����ת�˵����
	DWORD							dwTargetUserID;						// ����ת�˵����
	DWORD							dwSourceGameID;						// ����ת�˵�GameID
	DWORD							dwTargetGameID;						// ����ת�˵�GameID

	TCHAR							szSourceNickName[LEN_NICKNAME];		// ����ת����ҵ��ǳ�
	TCHAR							szTargetNickName[LEN_NICKNAME];		// ����ת����ҵ��ǳ�

	SCORE							lTransferScore;						// ת�˽��
	SCORE							lRevenueScore;						// ˰�ս��

	SCORE							lSourceUserScore;					// ת�˺��Լ��Ļ���
	SCORE							lSourceUserInsure;					// ת�˺��Լ������л���

	SCORE							lReceviceScore;						// ת�˺�Է�ʵ�ն��ٻ���

	SYSTEMTIME						stTransferTime;						// ת��ʱ��

	DWORD							dwLoveliness;						// ת�˺�����ֵ
};

//���гɹ�
struct CMD_GP_UserInsureSuccess
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lUserScore;							//�û����
	SCORE							lUserInsure;						//���н��
	TCHAR							szDescribeString[128];				//������Ϣ
};

//����ʧ��
struct CMD_GP_UserInsureFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��ȡ���
struct CMD_GP_UserTakeResult
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lUserScore;							//�û����
	SCORE							lUserInsure;						//���н��
};

//��ѯ����
struct CMD_GP_QueryInsureInfo
{
	DWORD							dwUserID;							//�û� I D
};

//��ѯ�û�
struct CMD_GP_QueryUserInfoRequest
{
	BYTE                            cbByNickName;                       //�ǳ�����
	TCHAR							szNickName[LEN_NICKNAME];			//Ŀ���û�
};


//�û���Ϣ
struct CMD_GP_UserTransferUserInfo
{
	DWORD							dwTargetGameID;						//Ŀ���û�
	TCHAR							szNickName[LEN_NICKNAME];			//Ŀ���û�
};

// ��֤�û�������Ϣ
struct CMD_GP_VerifyUserInsurePassword
{
	DWORD							dwUserID;							// �û�I D
	TCHAR							szInsurePassword[LEN_PASSWORD];		// �û���������
};

// app����ɹ�
struct CMD_MB_ShareSuccess
{
	DWORD							dwUserID;							// ���������û�UserID
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

// �û���������
struct CMD_MB_UserCustomTable
{
	DWORD							dwUserID;							// ���������û�UserID
	WORD							wKindID;							// ��������
	WORD							wCardCousumeCount;					// ���ĵķ�����Ŀ
	BYTE							cbCustomRule[LEN_CUSTOMRULE];		// �Զ�����
};

// �û������Զ�������
struct CMD_MB_UserJoinCustomTable
{
	DWORD							dwUserID;								// ���������û�UserID
	TCHAR							szSecret[LEN_CUSTOMTABLEPASSWORD];		// ��������
};

// �û���ѯ�����������Ϣ
struct CMD_MB_UserQueryCustomTable
{
	DWORD							dwUserID;								// ���������û�UserID
};

struct CMD_MB_UserTransferDiamond
{
	DWORD							dwUserID;							//ת���û���I D
	DWORD							dwGameID;							//�����û���I D
	DWORD							dwTransferDiamond;					//ת����ʯ
	TCHAR							szPassword[LEN_MD5];				//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//////////////////////////////////////////////////////////////////////////////////

//����ʧ��
struct CMD_GP_OperateFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�����ɹ�
struct CMD_GP_OperateSuccess
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

//////////////////////////////////////////////////////////////////////////////////

// �û��Խ����ӳɹ�
struct CMD_MB_UserCustomTableSuccess
{
	WORD							wServerID;							// ����ID
	TCHAR							szSecret[LEN_CUSTOMTABLEPASSWORD];	// ����
	bool							bLockedInTable;						// �Ƿ�������������,�Ѿ������Ļ�,ֻ�ܷ��ط���
	DWORD							dwDiamondCount;						// ʣ���������
};

// �û��Խ�����ʧ��
struct CMD_MB_UserCustomTableFailed
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

// �û���ʯ�ɹ�
struct CMD_MB_UserDiamondSuccess
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwDiamondCount;						//ԭ������ʯ
	TCHAR							szDescribeString[128];				//������Ϣ
};

// �û���ʯʧ��
struct CMD_MB_UserDiamondFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
//Զ�̷���

#define MDM_GP_REMOTE_SERVICE		4									//Զ�̷���

//���ҷ���
#define SUB_GP_C_SEARCH_DATABASE	100									//���ݲ���
#define SUB_GP_C_SEARCH_CORRESPOND	101									//Э������

//���ҷ���
#define SUB_GP_S_SEARCH_DATABASE	200									//���ݲ���
#define SUB_GP_S_SEARCH_CORRESPOND	201									//Э������

//////////////////////////////////////////////////////////////////////////////////

//Э������
struct CMD_GP_C_SearchCorrespond
{
	DWORD							dwGameID;							//��Ϸ��ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
};

//Э������
struct CMD_GP_S_SearchCorrespond
{
	WORD							wUserCount;							//�û���Ŀ
	tagUserRemoteInfo				UserRemoteInfo[16];					//�û���Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//��¼����
#define MDM_MB_LOGON				100									//�㳡��¼

//��¼ģʽ
#define SUB_MB_LOGON_GAMEID			1									//I D ��¼
#define SUB_MB_LOGON_ACCOUNTS		2									//�ʺŵ�¼
#define SUB_MB_REGISTER_ACCOUNTS	3									//ע���ʺ�
#define SUB_MB_SIGN_IN_CHECK		4									//ǩ��
#define SUB_LOGON_THIRDPARTY		5									// ��������¼

//��¼���
#define SUB_MB_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_MB_LOGON_FAILURE		101									//��¼ʧ��

//������ʾ
#define SUB_MB_UPDATE_NOTIFY		200									//������ʾ

//////////////////////////////////////////////////////////////////////////////////

//I D ��¼
struct CMD_MB_LogonGameID
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	DWORD							dwPlazaVersion;						//�㳡�汾
	BYTE                            cbDeviceType;                       //�豸����

	//��¼��Ϣ
	DWORD							dwGameID;							//��Ϸ I D
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
};

//�ʺŵ�¼
struct CMD_MB_LogonAccounts
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	DWORD							dwPlazaVersion;						//�㳡�汾
	BYTE                            cbDeviceType;                       //�豸����

	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
};

//�˺ŵ�¼(������)
struct CMD_LogonAccounts_ThirdParty
{
	//ϵͳ��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	
	BYTE                            cbDeviceType;                       // �豸����
	unsigned int					nThirdPartyID;						// ������ID
	TCHAR							szSessionID[LEN_SESSION_ID];		// SessionID
	TCHAR							szNickName[LEN_NICKNAME];			// �ǳ�

	// ����ID
	DWORD							dwAreaID;							//����ID
};

//ע���ʺ�
struct CMD_MB_RegisterAccounts
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	DWORD							dwPlazaVersion;						//�㳡�汾
	BYTE                            cbDeviceType;                       //�豸����

	//�������
	TCHAR							szLogonPass[LEN_MD5];				//��¼����
	TCHAR							szInsurePass[LEN_MD5];				//��������

	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����

	// ����ID
	DWORD							dwAreaID;							//����ID
};

//��¼�ɹ�
struct CMD_MB_LogonSuccess
{
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwExperience;						//������ֵ
	DWORD							dwLoveLiness;						//�û�����

	//�û��ɼ�
	SCORE							lUserScore;							//�û����
	SCORE							lUserInsure;						//�û�����	

	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��˺�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�

	// �û�������ʯ
	DWORD							dwDiamondCount;						// ������ʯ����
};

//��¼ʧ��
struct CMD_MB_LogonFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//������ʾ
struct CMD_MB_UpdateNotify
{
	BYTE							cbMustUpdate;						//ǿ������
	BYTE							cbAdviceUpdate;						//��������
	DWORD							dwCurrentVersion;					//��ǰ�汾
};

// ǩ��
struct CMD_MB_SignInCheck
{
	DWORD							dwErrorCode;						// ��ʶ��
	DWORD							dwCheckDay;							// ǩ��������
};

//////////////////////////////////////////////////////////////////////////////////
//Я����Ϣ CMD_MB_LogonSuccess

#define DTP_MB_GROUP_INFO			1									//������Ϣ
#define DTP_MB_MEMBER_INFO			2									//��Ա��Ϣ
#define	DTP_MB_UNDER_WRITE			3									//����ǩ��
#define DTP_MB_STATION_URL			4									//��ҳ��Ϣ
#define DTP_MB_AGENT_INFO			5									// ��ʯ������Ϣ

//��Ա��Ϣ
struct DTP_MB_MemberInfo
{
	BYTE							cbMemberOrder;						//��Ա�ȼ�
};

struct DTP_MB_AgentInfo
{
	TCHAR							szAgentInfo[LEN_URL];				//��ʯ������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
//�б�����

#define MDM_MB_SERVER_LIST			101									//�б���Ϣ

//�б���Ϣ
#define SUB_MB_LIST_TYPE			100									//�����б�
#define SUB_MB_LIST_KIND			101									//�����б�
#define SUB_MB_LIST_SERVER			102									//�����б�
#define SUB_MB_LIST_KIND_EX			103									//������չ��Ϣ
#define SUB_MB_LIST_PROP_CONFIG		104									// �Խ����ӵ����������
#define SUB_MB_LIST_FINISH			200									//�б����

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//������ʾ��Ϣ
#define MDM_MB_SYSTEM_MESSAGE			102								// �ٷ�����

#define SUB_MB_RECHARGE_MESSAGE			100								// ��ֵ����
#define SUB_MB_CUSTOM_SERVICE_MESSAGE 	101								// �ͷ�����	

struct CMD_MB_SystemMessage
{
	WORD							wLength;							//��Ϣ����
	TCHAR							szString[1024];						//��Ϣ����
};

#pragma pack()

#endif