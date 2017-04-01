#pragma once

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
//Á´½Ó´úÂë

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../Libs/Ansi/ServiceCore.lib")
#pragma comment (lib,"../../Libs/Ansi/boost_system-vc120-mt-1_60.lib")
#else
#pragma comment (lib,"../../Libs/Unicode/ServiceCore.lib")
#pragma comment (lib,"../../Libs/Unicode/boost_system-vc120-mt-1_60.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../Libs/Ansi/ServiceCoreD.lib")
#pragma comment (lib,"../../Libs/Ansi/boost_system-vc120-mt-gd-1_60.lib")
#else
#pragma comment (lib,"../../Libs/Unicode/ServiceCoreD.lib")
#pragma comment (lib,"../../Libs/Unicode/boost_system-vc120-mt-gd-1_60.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////
#define BOOST_REGEX_NO_LIB
#define BOOST_DATE_TIME_SOURCE
#define BOOST_SYSTEM_NO_LIB
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::asio;
