#ifndef MATCH_SERVICE_HEAD_HEAD_FILE
#define MATCH_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//ƽ̨����
#include "..\..\GlobalDefine\Platform.h"

//��Ϣ����
#include "..\..\MessageDefine\CMD_Commom.h"
#include "..\..\MessageDefine\CMD_Correspond.h"
#include "..\..\MessageDefine\CMD_GameServer.h"

//ƽ̨�ļ�
#include "..\..\PublicModule\ServiceCore\ServiceCoreHead.h"
#include "..\GameService\GameServiceHead.h"
#include "..\KernelEngine\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef MATCH_SERVICE_CLASS
	#ifdef  MATCH_SERVICE_DLL
		#define MATCH_SERVICE_CLASS _declspec(dllexport)
	#else
		#define MATCH_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchService.dll")			//�������
#else
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchServiceD.dll")			//�������
#endif

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
DECLARE_MODULE_HELPER(GameMatchServiceManager,MATCH_SERVICE_DLL_NAME,"CreateGameMatchServiceManager")

//////////////////////////////////////////////////////////////////////////////////

#endif