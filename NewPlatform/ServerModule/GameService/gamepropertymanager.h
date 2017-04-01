#ifndef GAME_PROPERTY_MANAGER_HEAD_FILE
#define GAME_PROPERTY_MANAGER_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

#include <vector>
using namespace std;
//////////////////////////////////////////////////////////////////////////////////

//类型定义
#ifndef _NO_WHARRAY_
typedef CWHArray<tagPropertyInfo>	CPropertyInfoArray;					//道具数组
#else
typedef vector<tagPropertyInfo>		CPropertyInfoArray;					//道具数组
#endif

//////////////////////////////////////////////////////////////////////////////////

//道具管理
class GAME_SERVICE_CLASS CGamePropertyManager
{
	//变量定义
protected:
	CPropertyInfoArray				m_PropertyInfoArray;				//道具数组

	//函数定义
public:
	//构造函数
	CGamePropertyManager();
	//析构函数
	virtual ~CGamePropertyManager();

	//配置函数
public:
	//设置道具
	bool SetGamePropertyInfo(tagPropertyInfo PropertyInfo[], WORD wPropertyCount);

	//功能函数
public:
	//查找道具
	tagPropertyInfo * SearchPropertyItem(WORD wPropertyIndex);
};

//////////////////////////////////////////////////////////////////////////////////

#endif