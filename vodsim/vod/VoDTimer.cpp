#include "VodTimer.h"
#include "Vod.h"



//////////////////////////////////////////////////////////////////////////
//�ڵ㶯̬���

//��½��ʱ��
void JoinTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	//FIXME:���÷ŵ�����ȷ��Ű�
	VodApp::s_onlineNodes++;
	p->onJoinTimer();
}

//��½��ʱ��
void LeaveTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	//FIXME:���÷ŵ�����ȷ��Ű�
	VodApp::s_onlineNodes++;
	p->onLeaveTimer();
}

//////////////////////////////////////////////////////////////////////////
//������֯���

//�����Ը��º����ڵ㶨ʱ��
void UpdataNeightborsTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	p->onUpdataNeightborsTimer();
}

//�����Ը��¸��ڵ㶨ʱ��
void UpdataFathersTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	p->onUpdataFathersTimer();
}

//�����Ը����ӽڵ㶨ʱ��
void UpdataChildrenTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	p->onUpdataChildrenTimer();
}

//////////////////////////////////////////////////////////////////////////
//���ݵ������

//������buffermapͨ�涨ʱ��
void SendBufferMapTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	p->onSendBufferMapTimer();
}

//��������������ʱ��
void RequestPieceTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	p->onRequestPieceTimer();
}

//////////////////////////////////////////////////////////////////////////
//��������ͳ�����
void StatisticTimer::expire(Event*)
{
	VodApp::outPutStatistic();
	this->resched(1);
}