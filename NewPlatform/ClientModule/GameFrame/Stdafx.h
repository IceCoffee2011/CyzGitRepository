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

//MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>

//////////////////////////////////////////////////////////////////////////////////
//链接代码

//多媒体库
#pragma comment (lib,"Winmm.lib")

//链接代码
#ifndef _DEBUG
	#ifndef _UNICODE
		#pragma comment (lib,"../../Libs/Ansi/WHImage.lib")
//		#pragma comment (lib,"../../链接库/Ansi/GameEngine.lib")
		#pragma comment (lib,"../../Libs/Ansi/ServiceCore.lib")
		#pragma comment (lib,"../../Libs/Ansi/SkinControl.lib")
		#pragma comment (lib,"../../Libs/Ansi/ShareControl.lib")
		#pragma comment (lib,"../../Libs/Ansi/PlatformData.lib")
		#pragma comment (lib,"../../Libs/Ansi/GameProperty.lib")
		#pragma comment (lib,"../../Libs/Ansi/AvatarControl.lib")
		#pragma comment (lib,"../../Libs/Ansi/VideoService.lib")
		#pragma comment (lib,"../../Libs/Ansi/bass.lib")
	#else
		#pragma comment (lib,"../../Libs/Unicode/WHImage.lib")
//		#pragma comment (lib,"../../链接库/Unicode/GameEngine.lib")
		#pragma comment (lib,"../../Libs/Unicode/ServiceCore.lib")
		#pragma comment (lib,"../../Libs/Unicode/SkinControl.lib")
		#pragma comment (lib,"../../Libs/Unicode/ShareControl.lib")
		#pragma comment (lib,"../../Libs/Unicode/PlatformData.lib")
		#pragma comment (lib,"../../Libs/Unicode/GameProperty.lib")
		#pragma comment (lib,"../../Libs/Unicode/AvatarControl.lib")
		#pragma comment (lib,"../../Libs/Unicode/VideoService.lib")
		#pragma comment (lib,"../../Libs/Unicode/bass.lib")
	#endif
#else
	#ifndef _UNICODE
		#pragma comment (lib,"../../Libs/Ansi/WHImageD.lib")
//		#pragma comment (lib,"../../链接库/Ansi/GameEngine.lib")
		#pragma comment (lib,"../../Libs/Ansi/ServiceCoreD.lib")
		#pragma comment (lib,"../../Libs/Ansi/SkinControlD.lib")
		#pragma comment (lib,"../../Libs/Ansi/ShareControlD.lib")
		#pragma comment (lib,"../../Libs/Ansi/PlatformDataD.lib")
		#pragma comment (lib,"../../Libs/Ansi/GamePropertyD.lib")
		#pragma comment (lib,"../../Libs/Ansi/AvatarControlD.lib")
		#pragma comment (lib,"../../Libs/Ansi/VideoServiceD.lib")
		#pragma comment (lib,"../../Libs/Ansi/bass.lib")
	#else
		#pragma comment (lib,"../../Libs/Unicode/WHImageD.lib")
//		#pragma comment (lib,"../../链接库/Unicode/GameEngine.lib")
		#pragma comment (lib,"../../Libs/Unicode/ServiceCoreD.lib")
		#pragma comment (lib,"../../Libs/Unicode/SkinControlD.lib")
		#pragma comment (lib,"../../Libs/Unicode/ShareControlD.lib")
		#pragma comment (lib,"../../Libs/Unicode/PlatformDataD.lib")
		#pragma comment (lib,"../../Libs/Unicode/GamePropertyD.lib")
		#pragma comment (lib,"../../Libs/Unicode/AvatarControlD.lib")
		#pragma comment (lib,"../../Libs/Unicode/VideoServiceD.lib")
		#pragma comment (lib,"../../Libs/Unicode/bass.lib")
	#endif
#endif

//////////////////////////////////////////////////////////////////////////////////
