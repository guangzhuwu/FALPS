#include "VodTimer.h"
#include "Vod.h"



//////////////////////////////////////////////////////////////////////////
//节点动态相关

//登陆定时器
void JoinTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	//FIXME:不该放到这里，先放着吧
	VodApp::s_onlineNodes++;
	p->onJoinTimer();
}

//登陆定时器
void LeaveTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	//FIXME:不该放到这里，先放着吧
	VodApp::s_onlineNodes++;
	p->onLeaveTimer();
}

//////////////////////////////////////////////////////////////////////////
//拓扑组织相关

//周期性更新合作节点定时器
void UpdataNeightborsTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	p->onUpdataNeightborsTimer();
}

//周期性更新父节点定时器
void UpdataFathersTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	p->onUpdataFathersTimer();
}

//周期性更新子节点定时器
void UpdataChildrenTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	p->onUpdataChildrenTimer();
}

//////////////////////////////////////////////////////////////////////////
//数据调度相关

//周期性buffermap通告定时器
void SendBufferMapTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	p->onSendBufferMapTimer();
}

//周期性数据请求定时器
void RequestPieceTimer::expire(Event*)
{
	OutputAssert(m_app);
	VodApp* p=(VodApp*)(m_app);
	p->onRequestPieceTimer();
}

//////////////////////////////////////////////////////////////////////////
//仿真数据统计相关
void StatisticTimer::expire(Event*)
{
	VodApp::outPutStatistic();
	this->resched(1);
}