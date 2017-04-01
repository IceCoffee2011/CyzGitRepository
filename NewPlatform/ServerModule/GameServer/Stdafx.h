#pragma once

//////////////////////////////////////////////////////////////////////////////////
//MFC 文件

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x501
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x501
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x501
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#define _AFX_ALL_WARNINGS

#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxDisp.h>
#include <AfxDtctl.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <AfxCmn.h>
#endif

//////////////////////////////////////////////////////////////////////////////////
//包含文件

//平台定义
#include "..\..\GlobalDefine\Platform.h"
#include "..\..\MessageDefine\CMD_Correspond.h"
#include "..\..\MessageDefine\CMD_GameServer.h"
#include "..\..\MessageDefine\CMD_LogonServer.h"

//组件定义
#include "..\..\PublicModule\ServiceCore\ServiceCoreHead.h"
#include "..\GameService\GameServiceHead.h"
#include "..\MatchService\MatchServiceHead.h"
#include "..\KernelEngine\KernelEngineHead.h"
#include "..\ModuleManager\ModuleManagerHead.h"

#include "..\..\CrashRpt\CrashRpt.h"

//////////////////////////////////////////////////////////////////////////////////
//链接代码

#ifndef _DEBUG
	#ifndef _UNICODE
		#pragma comment (lib,"../../Libs/Ansi/ServiceCore.lib")
		#pragma comment (lib,"../../Libs/Ansi/GameService.lib")
		#pragma comment (lib,"../../Libs/Ansi/KernelEngine.lib")
		#pragma comment (lib,"../../Libs/Ansi/ModuleManager.lib")
		#pragma comment (lib,"../../Libs/Ansi/CrashRpt1403.lib")
		#pragma comment (lib,"../../Libs/Ansi/CrashRptProbe1403.lib")
	#else
		#pragma comment (lib,"../../Libs/Unicode/ServiceCore.lib")
		#pragma comment (lib,"../../Libs/Unicode/GameService.lib")
		#pragma comment (lib,"../../Libs/Unicode/KernelEngine.lib")
		#pragma comment (lib,"../../Libs/Unicode/ModuleManager.lib")
		#pragma comment (lib,"../../Libs/Unicode/CrashRpt1403.lib")
		#pragma comment (lib,"../../Libs/Unicode/CrashRptProbe1403.lib")
	#endif
#else
	#ifndef _UNICODE
		#pragma comment (lib,"../../Libs/Ansi/ServiceCoreD.lib")
		#pragma comment (lib,"../../Libs/Ansi/GameServiceD.lib")
		#pragma comment (lib,"../../Libs/Ansi/KernelEngineD.lib")
		#pragma comment (lib,"../../Libs/Ansi/ModuleManagerD.lib")
		#pragma comment (lib,"../../Libs/Ansi/CrashRpt1403.lib")
		#pragma comment (lib,"../../Libs/Ansi/CrashRptProbe1403.lib")
	#else
		#pragma comment (lib,"../../Libs/Unicode/ServiceCoreD.lib")
		#pragma comment (lib,"../../Libs/Unicode/GameServiceD.lib")
		#pragma comment (lib,"../../Libs/Unicode/KernelEngineD.lib")
		#pragma comment (lib,"../../Libs/Unicode/ModuleManagerD.lib")
		#pragma comment (lib,"../../Libs/Unicode/CrashRpt1403.lib")
		#pragma comment (lib,"../../Libs/Unicode/CrashRptProbe1403.lib")
	#endif
#endif

//////////////////////////////////////////////////////////////////////////////////
