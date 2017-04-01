#ifndef PLATFORM_HEAD_FILE
#define PLATFORM_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//包含文件

//定义文件
#include "Macro.h"
#include "Define.h"

//结构文件
#include "Struct.h"
#include "Packet.h"
#include "Property.h"

//模板文件
#include "Array.h"
#include "Module.h"
#include "PacketAide.h"
#include "ServerRule.h"
#include "RightDefine.h"

//////////////////////////////////////////////////////////////////////////////////

//程序版本
#define VERSION_FRAME				PROCESS_VERSION(6,0,3)				//框架版本
#define VERSION_PLAZA				PROCESS_VERSION(6,0,3)				//大厅版本
#define VERSION_MOBILE_ANDROID		PROCESS_VERSION(6,0,3)				//手机版本
#define VERSION_MOBILE_IOS			PROCESS_VERSION(6,0,3)				//手机版本

//版本定义
#define VERSION_EFFICACY			0									//效验版本
#define VERSION_FRAME_SDK			INTERFACE_VERSION(6,3)				//框架版本

//#define VERSION_69

//////////////////////////////////////////////////////////////////////////////////
//发布版本

#ifndef _DEBUG

//平台常量
const TCHAR szProduct[]=TEXT("浙东山庄");					//产品名字
const TCHAR szPlazaClass[]=TEXT("zdszLobby");						//广场类名
const TCHAR szProductKey[]=TEXT("zdszLobby");					    //产品主键

//地址定义

//const TCHAR szCookieUrl[]=TEXT("http://www.dk78.com");				//记录地址
//const TCHAR szLogonServer[]=TEXT("172.168.1.119");				//内测公网机
//const TCHAR szPlatformLink[]=TEXT("http://www.dk78.com");				//平台网站
//const TCHAR szUpdateLink[]=TEXT("us.gamexingji.com");					//下载地址

const TCHAR szCookieUrl[]=TEXT("http://www.1378.com");					//记录地址
extern TCHAR szLogonServer[MAX_PATH];									//内测公网机
//const TCHAR szLogonServer[]=TEXT("127.0.0.1");						//内测公网机
extern TCHAR szPlatformLink[MAX_PATH];									//平台网站
static TCHAR szUpdateLink[MAX_PATH]=TEXT("");							//下载地址

#else

//////////////////////////////////////////////////////////////////////////////////
//内测版本

//平台常量
const TCHAR szProduct[]=TEXT("浙东山庄");					//产品名字
const TCHAR szPlazaClass[]=TEXT("zdszLobby");						//广场类名
const TCHAR szProductKey[]=TEXT("zdszLobby");					    //产品主键

//地址定义

const TCHAR szCookieUrl[]=TEXT("http://www.1378.com");					//记录地址
extern TCHAR szLogonServer[MAX_PATH];							//内测公网机
extern TCHAR szPlatformLink[MAX_PATH];									//平台网站
static TCHAR szUpdateLink[]=TEXT("");									//下载地址

#endif

//////////////////////////////////////////////////////////////////////////////////

//数据库名
const TCHAR szPlatformDB[]=TEXT("QPPlatformDB");						//平台数据库
const TCHAR szAccountsDB[]=TEXT("QPAccountsDB");						//用户数据库
const TCHAR szTreasureDB[]=TEXT("QPTreasureDB");						//财富数据库
const TCHAR szExerciseDB[]=TEXT("QPExerciseDB");						//练习数据库
const TCHAR szUserCustomDB[] = TEXT("QPUserCustomDB");					//用户自建桌子数据库

//////////////////////////////////////////////////////////////////////////////////

//授权信息
const TCHAR szCompilation[]=TEXT("1A998D81-AC7C-432F-A709-9F6D8AA760C2");

//////////////////////////////////////////////////////////////////////////////////

#endif
