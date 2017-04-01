#ifndef TIMERENGINE_HEAD_FILE
#define TIMERENGINE_HEAD_FILE

#pragma once

//ϵͳͷ�ļ�

#include "KernelEngineHead.h"
#include "QueueServiceEvent.h"

#define TIMER_SPACE								25				//ʱ����

#include <vector>
using namespace std;

//��˵��
class CTimerEngine;

//////////////////////////////////////////////////////////////////////////

//��ʱ���߳�
class CTimerThread : public CWHThread
{
	//��������
protected:
	CTimerEngine						* m_pTimerEngine;				//��ʱ������
	//��������
public:
	//���캯��
	CTimerThread(void);
	//��������
	virtual ~CTimerThread(void);

	//���ܺ���
public:
	//���ú���
	bool InitThread(CTimerEngine * pTimerEngine);

	//���غ���
private:
	//���к���
	virtual bool OnEventThreadRun();
};

//////////////////////////////////////////////////////////////////////////

//��ʱ������
struct tagTimerItem
{
	DWORD								wTimerID;						//��ʱ�� ID
	DWORD								dwElapse;						//��ʱʱ��
	DWORD								dwTimeLeave;					//����ʱ��
	DWORD								dwRepeatTimes;					//�ظ�����
	WPARAM								wBindParam;						//�󶨲���
	deadline_timer*						pTimer;							//��ʱ��

	tagTimerItem()
	{
		pTimer = nullptr;
	}
};

//��ʱ���������鶨��
#ifdef _NO_WHARRAY_
typedef vector<tagTimerItem*>	 CTimerItemPtr;
#else
typedef CWHArray<tagTimerItem *> CTimerItemPtr;
#endif



//////////////////////////////////////////////////////////////////////////

//��ʱ������
class CTimerEngine : public ITimerEngine
{
	friend class CTimerThread;

	//״̬����
protected:
	bool								m_bService;						//���б�־
	CTimerItemPtr						m_TimerItemFree;				//��������
	CTimerItemPtr						m_TimerItemActive;				//�����

	//�������
protected:
	CCriticalSection					m_CriticalSection;				//�߳���
	CTimerThread						m_TimerThread;					//��ʱ���߳�
	IQueueServiceSink *					m_pIQueueServiceSink;			//֪ͨ�ص�

protected:
	io_service							m_IOService;
	boost::asio::io_service::work*		m_pWork;
	//��������
public:
	//���캯��
	CTimerEngine(void);
	//��������
	virtual ~CTimerEngine(void);

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { if (IsValid()) delete this; }
	//�Ƿ���Ч
	virtual bool IsValid() { return AfxIsValidAddress(this, sizeof(CTimerEngine)) ? true : false; }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//�ӿں���
public:
	//���ö�ʱ��
	virtual bool SetTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
	//ɾ����ʱ��
	virtual bool KillTimer(DWORD dwTimerID);
	//ɾ����ʱ��
	virtual bool KillAllTimer();
	//���ýӿ�
	virtual bool SetTimerEngineEvent(IUnknownEx * pIUnknownEx);

	//����ӿ�
public:
	//��ʼ����
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();

	//�ڲ�����
private:
	//��ʱ��֪ͨ
	void OnTimerThreadSink();

	// ��ʱ����ʱ�ص�
protected:
	void OnTimerCallback(boost::system::error_code ecode, DWORD dwTimerID);
};

//////////////////////////////////////////////////////////////////////////

#endif