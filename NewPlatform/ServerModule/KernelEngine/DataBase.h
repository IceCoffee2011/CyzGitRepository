#ifndef DATA_BASE_ENGINE_HEAD_FILE
#define DATA_BASE_ENGINE_HEAD_FILE

#pragma once

//���ͷ�ļ�
#include "KernelEngineHead.h"
#include "QueueService.h"
#include "TraceService.h"
#include "QueueServiceEvent.h"

#ifdef _USE_OTL_

// �������
struct tag_OtlParameter
{
public:
	CDBVarValue			Value;
	CDBVarValue			Name;
	LONG				Size;
	TCHAR				SqlParameter[MAX_PATH];
};

#include <vector>
#include <map>
using namespace std;

#endif

//////////////////////////////////////////////////////////////////////////

//ADO ������
class CADOError : public IDataBaseException
{
	//��������
protected:
	enSQLException					m_enErrorType;						//�������
	CString							m_strErrorDescribe;					//������Ϣ

	//��������
public:
	//���캯��
	CADOError();
	//��������
	virtual ~CADOError();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release()
	{
		return /*true*/;
	}
	//�Ƿ���Ч
	virtual bool IsValid()
	{
		return AfxIsValidAddress(this, sizeof(CADOError)) ? true : false;
	}
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//���ܽӿ�
public:
	//��������
	virtual enSQLException GetExceptionType()
	{
		return m_enErrorType;
	}
	//��������
	virtual LPCTSTR GetExceptionDescribe()
	{
		return m_strErrorDescribe;
	}
	//�쳣����
	virtual HRESULT GetExceptionResult()
	{
		return (HRESULT)0;    //
	}

	//���ܺ���
public:
	//���ô���
	void SetErrorInfo(enSQLException enErrorType, LPCTSTR pszDescribe);
#ifdef _USE_OTL_
	//���ô���
	void SetErrorInfo(const otl_exception& p);
#endif // _USE_OTL_
};

//////////////////////////////////////////////////////////////////////////

//���ݿ����
class CDataBase : public IDataBase
{
	//��Ϣ����
protected:
	CADOError						m_ADOError;							//�������
	CString							m_strConnect;						//�����ַ���
	CString							m_strErrorDescribe;					//������Ϣ

	//״̬����
protected:
	DWORD							m_dwConnectCount;					//���Դ���
	DWORD							m_dwConnectErrorTime;				//����ʱ��
	const DWORD						m_dwResumeConnectCount;				//�ָ�����
	const DWORD						m_dwResumeConnectTime;				//�ָ�ʱ��

	//�ں˱���
protected:
	_CommandPtr						m_DBCommand;						//�������
	_RecordsetPtr					m_DBRecordset;						//��¼������
	_ConnectionPtr					m_DBConnection;						//���ݿ����

#ifdef _USE_OTL_
	// ���ݿ��������
protected:
	otl_connect						m_DBConnect;			
	otl_stream						m_DBStream;
	// �������
protected:
	vector<tag_OtlParameter>		m_vectParameterIn;
	map<string, CDBVarValue>		m_mapParameterOut;
#endif // _USE_OTL_

	//��������
public:
	//���캯��
	CDataBase();
	//��������
	virtual ~CDataBase();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release()
	{
		if (IsValid()) delete this;
		return /*true*/;
	}
	//�Ƿ���Ч
	virtual bool IsValid()
	{
		return AfxIsValidAddress(this, sizeof(CDataBase)) ? true : false;
	}
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//������
	virtual VOID OpenConnection();
	//�رռ�¼
	virtual VOID CloseRecordset();
	//�ر�����
	virtual VOID CloseConnection();
	//��������
	virtual bool TryConnectAgain(bool bFocusConnect, CComError * pComError);
	//������Ϣ
	virtual bool SetConnectionInfo(DWORD dwDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword);
	//������Ϣ
	virtual bool SetConnectionInfo(LPCTSTR szDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword);
	//�л���¼
	virtual VOID NextRecordset();

	//״̬�ӿ�
public:
	//�Ƿ����Ӵ���
	virtual bool IsConnectError();
	//�Ƿ��
	virtual bool IsRecordsetOpened();

	//��¼���ӿ�
public:
	//�����ƶ�
	virtual void MoveToNext();
	//�Ƶ���ͷ
	virtual void MoveToFirst();
	//�Ƿ����
	virtual bool IsRecordsetEnd();
	//��ȡ��Ŀ
	virtual long GetRecordCount();
	//��ȡ��С
	virtual long GetActualSize(LPCTSTR pszParamName);
	//�󶨶���
	virtual VOID BindToRecordset(CADORecordBinding * pBind);
	//��ȡ����
#ifndef _USE_OTL_
	virtual VOID GetRecordsetValue(LPCTSTR pszItem, CDBVarValue & DBVarValue);
#else
	virtual VOID GetRecordsetValue(LPCTSTR pszItem, CDBVarValue & DBVarValue);
#endif
	//�ֶνӿ�
public:
	//��ȡ����
	virtual bool GetFieldValue(LPCTSTR lpFieldName, BYTE & bValue);
	//��ȡ����
	virtual bool GetFieldValue(LPCTSTR lpFieldName, WORD & wValue);
	//��ȡ����
	virtual bool GetFieldValue(LPCTSTR lpFieldName, INT & nValue);
	//��ȡ����
	virtual bool GetFieldValue(LPCTSTR lpFieldName, LONG & lValue);
	//��ȡ����
	virtual bool GetFieldValue(LPCTSTR lpFieldName, DWORD & ulValue);
	//��ȡ����
	virtual bool GetFieldValue(LPCTSTR lpFieldName, UINT & ulValue);
	//��ȡ����
	virtual bool GetFieldValue(LPCTSTR lpFieldName, DOUBLE & dbValue);
	//��ȡ����
	virtual bool GetFieldValue(LPCTSTR lpFieldName, __int64 & llValue);
	//��ȡ����
	virtual bool GetFieldValue(LPCTSTR lpFieldName, LPSTR szBuffer, UINT uSize);
	//��ȡ����
	virtual bool GetFieldValue(LPCTSTR lpFieldName, LPWSTR szBuffer, UINT uSize);
	//��ȡ����
	virtual bool GetFieldValue(LPCTSTR lpFieldName, COleDateTime & Time);
	//��ȡ����
	virtual bool GetFieldValue(LPCTSTR lpFieldName, bool & bValue);
	//��ȡ����
#ifndef _USE_OTL_
	virtual bool GetFieldValue(LPCTSTR lpFieldName, CDBVarValue & DBVarValue);
#else
	virtual bool GetFieldValue(LPCTSTR lpFieldName, CDBVarValue & DBVarValue);
#endif

	//�������ӿ�
public:
#ifndef _USE_OTL_
	//��ȡ����
	virtual VOID GetParameter(LPCTSTR pszParamName, CDBVarValue & DBVarValue);
	//�������
	virtual VOID AddParameter(LPCTSTR pszName, DataTypeEnum Type, ParameterDirectionEnum Direction, LONG lSize, CDBVarValue & DBVarValue);
#else
	//��ȡ����
	virtual VOID GetParameter(LPCTSTR pszParamName, CDBVarValue & DBVarValue);
	//�������
	virtual VOID AddParameter(LPCTSTR pszName, DataTypeEnum Type, ParameterDirectionEnum Direction, LONG lSize, CDBVarValue & DBVarValue);
#endif // !_USE_OTL_
	//ɾ������
	virtual void ClearParameters();
	//��ȡ������ֵ
	virtual long GetReturnValue();

	//ִ�нӿ�
public:
	//ִ�����
	virtual VOID ExecuteProcess(LPCTSTR pszSPName, bool bRecordset);
	//ִ������
	virtual VOID ExecuteSentence(LPCTSTR pszCommand, bool bRecordset);

	//�ڲ�����
private:
	//��ȡ����
	LPCTSTR GetComErrorDescribe(CComError & ComError);
	//���ô���
	void SetErrorInfo(enSQLException enErrorType, LPCTSTR pszDescribe);
#ifdef _USE_OTL_
	void SetErrorInfo(const otl_exception &p);
	void GetValueFromStream();
#endif
};

//////////////////////////////////////////////////////////////////////////

//���ݿ������
class CDataBaseEngine : public IDataBaseEngine, public IQueueServiceSink
{
	//�ں˱���
protected:
	bool							m_bService;							//���б�־
	CQueueServiceEvent				m_QueueServiceEvent;				//���ж���
	//IDataBaseEngineSink				* m_pIDataBaseEngineSink;			//֪ͨ����

	vector<IDataBaseEngineSink *>   m_pIDataBaseEngineSinks;			//֪ͨ����
	//��������
public:
	//���캯��
	CDataBaseEngine(void);
	//��������
	virtual ~CDataBaseEngine(void);

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release()
	{
		if (IsValid()) delete this;
		return /*true*/;
	}
	//�Ƿ���Ч
	virtual bool IsValid()
	{
		return AfxIsValidAddress(this, sizeof(CDataBaseEngine)) ? true : false;
	}
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();

public:
	//���渺��
	virtual bool GetBurthenInfo(tagBurthenInfo & BurthenInfo);

	//���ýӿ�
public:
	//����ģ��
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx);
	//����ģ��
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx[], WORD wSinkCount);

	//���нӿ�
public:
	//���нӿ�
	virtual void OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize);

	//�����¼�
public:
	//�����¼�
	virtual bool PostDataBaseControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool DeferDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif