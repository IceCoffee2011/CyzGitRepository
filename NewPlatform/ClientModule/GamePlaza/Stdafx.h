#pragma once

//////////////////////////////////////////////////////////////////////////////////

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0501
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0501
#endif

#define _ATL_ATTRIBUTES
#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

//////////////////////////////////////////////////////////////////////////////////

//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>

//MFC �ļ�
#include <Nb30.h>
#include <AfxInet.h>
#include <afxdtctl.h>

//////////////////////////////////////////////////////////////////////////////////
//ƽ̨�ļ�

//ƽ̨�ļ�
#include "..\..\GlobalDefine\Platform.h"

//��������
#include "..\..\MessageDefine\CMD_Commom.h"
#include "..\..\MessageDefine\CMD_GameServer.h"
#include "..\..\MessageDefine\CMD_LogonServer.h"

//��������
#include "..\..\MessageDefine\IPC_GameFrame.h"

//����ļ�
#include "..\..\PublicModule\ServiceCore\ServiceCoreHead.h"
#include "..\..\PublicModule\ProcessControl\ProcessControlHead.h"

//����ļ�
#include "..\WHImage\WHImageHead.h"
#include "..\DownLoad\DownLoadHead.h"
#include "..\SkinControl\SkinControlHead.h"
#include "..\UserService\UserServiceHead.h"
#include "..\PlatformData\PlatformDataHead.h"
#include "..\ShareControl\ShareControlHead.h"
#include "..\GameProperty\GamePropertyHead.h"
#include "..\AvatarControl\AvatarControlHead.h"
#include "..\GameEngine\GameEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//���Ӵ���

//��ý���
#pragma comment (lib,"Winmm.lib")

#ifndef _DEBUG
#ifndef _UNICODE
	#pragma comment (lib,"../../Libs/Ansi/WHImage.lib")
	#pragma comment (lib,"../../Libs/Ansi/DownLoad.lib")
	#pragma comment (lib,"../../Libs/Ansi/ServiceCore.lib")
	#pragma comment (lib,"../../Libs/Ansi/SkinControl.lib")
	#pragma comment (lib,"../../Libs/Ansi/GameProperty.lib")
	#pragma comment (lib,"../../Libs/Ansi/ShareControl.lib")
	#pragma comment (lib,"../../Libs/Ansi/PlatformData.lib")
	#pragma comment (lib,"../../Libs/Ansi/AvatarControl.lib")
//	#pragma comment (lib,"../../���ӿ�/Ansi/GameEngine.lib")
#else
	#pragma comment (lib,"../../Libs/Unicode/WHImage.lib")
	#pragma comment (lib,"../../Libs/Unicode/DownLoad.lib")
	#pragma comment (lib,"../../Libs/Unicode/ServiceCore.lib")
	#pragma comment (lib,"../../Libs/Unicode/SkinControl.lib")
	#pragma comment (lib,"../../Libs/Unicode/GameProperty.lib")
	#pragma comment (lib,"../../Libs/Unicode/ShareControl.lib")
	#pragma comment (lib,"../../Libs/Unicode/PlatformData.lib")
	#pragma comment (lib,"../../Libs/Unicode/AvatarControl.lib")
//	#pragma comment (lib,"../../���ӿ�/Unicode/GameEngine.lib")
#endif
#else
#ifndef _UNICODE
	#pragma comment (lib,"../../Libs/Ansi/WHImageD.lib")
	#pragma comment (lib,"../../Libs/Ansi/DownLoadD.lib")
	#pragma comment (lib,"../../Libs/Ansi/ServiceCoreD.lib")
	#pragma comment (lib,"../../Libs/Ansi/SkinControlD.lib")
	#pragma comment (lib,"../../Libs/Ansi/GamePropertyD.lib")
	#pragma comment (lib,"../../Libs/Ansi/ShareControlD.lib")
	#pragma comment (lib,"../../Libs/Ansi/PlatformDataD.lib")
	#pragma comment (lib,"../../Libs/Ansi/AvatarControlD.lib")
//	#pragma comment (lib,"../../���ӿ�/Ansi/GameEngineD.lib")
#else
	#pragma comment (lib,"../../Libs/Unicode/WHImageD.lib")
	#pragma comment (lib,"../../Libs/Unicode/DownLoadD.lib")
	#pragma comment (lib,"../../Libs/Unicode/ServiceCoreD.lib")
	#pragma comment (lib,"../../Libs/Unicode/SkinControlD.lib")
	#pragma comment (lib,"../../Libs/Unicode/GamePropertyD.lib")
	#pragma comment (lib,"../../Libs/Unicode/ShareControlD.lib")
	#pragma comment (lib,"../../Libs/Unicode/PlatformDataD.lib")
	#pragma comment (lib,"../../Libs/Unicode/AvatarControlD.lib")
//	#pragma comment (lib,"../../���ӿ�/Unicode/GameEngineD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////
