#ifndef MODULE_DB_PARAMETER_HEAD_FILE
#define MODULE_DB_PARAMETER_HEAD_FILE

#pragma once

#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////

//ģ�����
class MODULE_MANAGER_CLASS CModuleDBParameter
{
	//�������
protected:
	tagDataBaseParameter			m_PlatformDBParameter;				//������Ϣ
	tagDataBaseParameter			m_UserCustomDBParameter;			//������Ϣ

	//��̬����
protected:
	static CModuleDBParameter *		m_pModuleDBParameter;				//����ָ��

	//��������
public:
	//���캯��
	CModuleDBParameter();
	//��������
	virtual ~CModuleDBParameter();

	//������Ϣ
public:
	//������Ϣ
	tagDataBaseParameter * GetPlatformDBParameter() { return &m_PlatformDBParameter; }
	//������Ϣ
	VOID SetPlatformDBParameter(tagDataBaseParameter & PlatformDBParameter) { m_PlatformDBParameter=PlatformDBParameter; }
	//������Ϣ
	tagDataBaseParameter * GetUserCustomDBParameter() { return &m_UserCustomDBParameter; }
	//������Ϣ
	VOID SetUserCustomDBParameter(tagDataBaseParameter & UserCustomDBParameter) { m_UserCustomDBParameter = UserCustomDBParameter; }

	//��̬����
public:
	//��ȡ����
	static CModuleDBParameter * GetModuleDBParameter() { return m_pModuleDBParameter; }
};

//////////////////////////////////////////////////////////////////////////////////

#endif