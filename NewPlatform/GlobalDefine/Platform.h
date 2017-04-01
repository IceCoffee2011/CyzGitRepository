#ifndef PLATFORM_HEAD_FILE
#define PLATFORM_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//�����ļ�

//�����ļ�
#include "Macro.h"
#include "Define.h"

//�ṹ�ļ�
#include "Struct.h"
#include "Packet.h"
#include "Property.h"

//ģ���ļ�
#include "Array.h"
#include "Module.h"
#include "PacketAide.h"
#include "ServerRule.h"
#include "RightDefine.h"

//////////////////////////////////////////////////////////////////////////////////

//����汾
#define VERSION_FRAME				PROCESS_VERSION(6,0,3)				//��ܰ汾
#define VERSION_PLAZA				PROCESS_VERSION(6,0,3)				//�����汾
#define VERSION_MOBILE_ANDROID		PROCESS_VERSION(6,0,3)				//�ֻ��汾
#define VERSION_MOBILE_IOS			PROCESS_VERSION(6,0,3)				//�ֻ��汾

//�汾����
#define VERSION_EFFICACY			0									//Ч��汾
#define VERSION_FRAME_SDK			INTERFACE_VERSION(6,3)				//��ܰ汾

//#define VERSION_69

//////////////////////////////////////////////////////////////////////////////////
//�����汾

#ifndef _DEBUG

//ƽ̨����
const TCHAR szProduct[]=TEXT("�㶫ɽׯ");					//��Ʒ����
const TCHAR szPlazaClass[]=TEXT("zdszLobby");						//�㳡����
const TCHAR szProductKey[]=TEXT("zdszLobby");					    //��Ʒ����

//��ַ����

//const TCHAR szCookieUrl[]=TEXT("http://www.dk78.com");				//��¼��ַ
//const TCHAR szLogonServer[]=TEXT("172.168.1.119");				//�ڲ⹫����
//const TCHAR szPlatformLink[]=TEXT("http://www.dk78.com");				//ƽ̨��վ
//const TCHAR szUpdateLink[]=TEXT("us.gamexingji.com");					//���ص�ַ

const TCHAR szCookieUrl[]=TEXT("http://www.1378.com");					//��¼��ַ
extern TCHAR szLogonServer[MAX_PATH];									//�ڲ⹫����
//const TCHAR szLogonServer[]=TEXT("127.0.0.1");						//�ڲ⹫����
extern TCHAR szPlatformLink[MAX_PATH];									//ƽ̨��վ
static TCHAR szUpdateLink[MAX_PATH]=TEXT("");							//���ص�ַ

#else

//////////////////////////////////////////////////////////////////////////////////
//�ڲ�汾

//ƽ̨����
const TCHAR szProduct[]=TEXT("�㶫ɽׯ");					//��Ʒ����
const TCHAR szPlazaClass[]=TEXT("zdszLobby");						//�㳡����
const TCHAR szProductKey[]=TEXT("zdszLobby");					    //��Ʒ����

//��ַ����

const TCHAR szCookieUrl[]=TEXT("http://www.1378.com");					//��¼��ַ
extern TCHAR szLogonServer[MAX_PATH];							//�ڲ⹫����
extern TCHAR szPlatformLink[MAX_PATH];									//ƽ̨��վ
static TCHAR szUpdateLink[]=TEXT("");									//���ص�ַ

#endif

//////////////////////////////////////////////////////////////////////////////////

//���ݿ���
const TCHAR szPlatformDB[]=TEXT("QPPlatformDB");						//ƽ̨���ݿ�
const TCHAR szAccountsDB[]=TEXT("QPAccountsDB");						//�û����ݿ�
const TCHAR szTreasureDB[]=TEXT("QPTreasureDB");						//�Ƹ����ݿ�
const TCHAR szExerciseDB[]=TEXT("QPExerciseDB");						//��ϰ���ݿ�
const TCHAR szUserCustomDB[] = TEXT("QPUserCustomDB");					//�û��Խ��������ݿ�

//////////////////////////////////////////////////////////////////////////////////

//��Ȩ��Ϣ
const TCHAR szCompilation[]=TEXT("1A998D81-AC7C-432F-A709-9F6D8AA760C2");

//////////////////////////////////////////////////////////////////////////////////

#endif
