/********************************************************************
文件: CycleTimer.h
日期: 2008.3.16
作者: 武广柱
说明: P2PVoD仿真中使用的一些定时器

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
//节点动态相关

//登陆定时器
class JoinTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};

//登陆定时器
class LeaveTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};

//////////////////////////////////////////////////////////////////////////
//拓扑组织相关

//周期性更新合作节点定时器
class UpdataNeightborsTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};

//周期性更新父节点定时器
class UpdataFathersTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};

//周期性更新子节点定时器
class UpdataChildrenTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};


//////////////////////////////////////////////////////////////////////////
//数据调度相关

//周期性buffermap通告定时器
class SendBufferMapTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};

//周期性数据请求定时器
class RequestPieceTimer:public VodTimer
{
protected:
	virtual void expire(Event*);
};


//////////////////////////////////////////////////////////////////////////
//仿真数据统计相关
class StatisticTimer:public TimerHandler
{
public:
	virtual void expire(Event*);
};

#endif//_P2PSTREAM_CYCLETIMER_H_

