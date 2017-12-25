/********************************************************************
�ļ�: CycleTimer.h
����: 2008.3.16
����: �����
˵��: P2PVoD������ʹ�õ�һЩ��ʱ��

Copyright (C) 2007 - All Rights Reserved
*********************************************************************/
#ifndef _P2PSTREAM_CYCLETIMER_H_
#define _P2PSTREAM_CYCLETIMER_H_

#include "Falps.h"

class VodTimer:public TimerHandler
{
public:
	VodTimer():m_app(NULL)
	{
	}
	void bindApp(Application&app)
	{
		m_app=&app;
	}
protected:
	Application* m_app;
};

//////////////////////////////////////////////////////////////////////////
//�ڵ㶯̬���

//��½��ʱ��
class JoinTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};

//��½��ʱ��
class LeaveTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};

//////////////////////////////////////////////////////////////////////////
//������֯���

//�����Ը��º����ڵ㶨ʱ��
class UpdataNeightborsTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};

//�����Ը��¸��ڵ㶨ʱ��
class UpdataFathersTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};

//�����Ը����ӽڵ㶨ʱ��
class UpdataChildrenTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};


//////////////////////////////////////////////////////////////////////////
//���ݵ������

//������buffermapͨ�涨ʱ��
class SendBufferMapTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};

//��������������ʱ��
class RequestPieceTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};


//////////////////////////////////////////////////////////////////////////
//��������ͳ�����
class StatisticTimer:public TimerHandler
{
public:
	virtual void expire(Event*);
};

#endif//_P2PSTREAM_CYCLETIMER_H_

