#ifndef _P2PSTREAM_PENDING_QUE_
#define _P2PSTREAM_PENDING_QUE_

#include "ParamDef.h"
#include "Falps.h"
#include <list>
using namespace std;

class ResponseFifoQue
{	
	struct elem_t 
	{
		elem_t(id_t sessionID,channel_id_t channelID,piece_id_t pieceID,addr_t ip,time_s_t timeOut)
			:pieceID(pieceID),address(ip),timeOut(timeOut)
			,channelID(channelID),sessionID(sessionID)
		{}
		id_t          sessionID;
		channel_id_t channelID;
		piece_id_t pieceID;
		addr_t     address;
		time_s_t   timeOut;
	};
public:
	typedef elem_t elem_type;
public:
	ResponseFifoQue()
	{
		m_app=NULL;
		popWhich_=-1;
		lastSession_=INVALID_ADDR;
		inserverWhich_=-1;
		que_.resize(8);
		priority_=-1;

		urgentpopWhich_=-1;
		urgentlastSession_=INVALID_ADDR;
		urgentinserverWhich_=-1;
		urgentque_.resize(8);
		urgentPriority_=-1;

	}
	void bindApp(Application& app)
	{
		m_app=&app;
	}
	void push(id_t sessionID,channel_id_t channelID,piece_id_t pieceID,addr_t ip,time_s_t timeOut)
	{
		OutputAssert(m_app);
		if (popWhich_==-1)
		{
			popWhich_=0;
			inserverWhich_=0;
			lastSession_=sessionID;
		}
		if (lastSession_==sessionID)
			que_[inserverWhich_].push_back(elem_t(sessionID,channelID,pieceID,ip,timeOut));
		else
		{
			inserverWhich_++;
			inserverWhich_%=que_.size();
			que_[inserverWhich_].push_back(elem_t(sessionID,channelID,pieceID,ip,timeOut));
			lastSession_=sessionID;
		}
		
	}
	void pop()
	{
		OutputAssert(m_app);
		que_[popWhich_].pop_front();
		popWhich_++;
		popWhich_%=que_.size();
	}
	elem_t front()
	{
		OutputAssert(m_app);
		for (size_t i=0;i<que_.size();i++)
		{
			if(!que_[popWhich_].empty())
				return que_[popWhich_].front();
			else
			{
				popWhich_++;
				popWhich_%=que_.size();
			}
		}
	}
	bool empty()
	{
		OutputAssert(m_app);
		for (size_t i=0;i<que_.size();i++)
		{
			if(!que_[i].empty())
				return false;
		}
		return true;
	}
/*

	void urgentPush(id_t sessionID,channel_id_t channelID,piece_id_t pieceID,addr_t ip,time_s_t timeOut)
	{
		OutputAssert(m_app);
		urgentque_.push_back(elem_t(sessionID,channelID,pieceID,ip,timeOut));
	}
	
	void urgentPop()
	{
		OutputAssert(m_app);
		urgentque_.pop_front();
	}
	
	elem_t urgentFront()
	{
		OutputAssert(m_app);
		return urgentque_.front();
	}

	bool urgentEmpty()
	{
		OutputAssert(m_app);
		return urgentque_.empty();
	}
*/

	void urgentPush(id_t sessionID,channel_id_t channelID,piece_id_t pieceID,addr_t ip,time_s_t timeOut)
	{
		OutputAssert(m_app);
		if (urgentpopWhich_==-1)
		{
			urgentpopWhich_=0;
			urgentinserverWhich_=0;
			urgentlastSession_=sessionID;
		}
		if (urgentlastSession_==sessionID)
			urgentque_[urgentinserverWhich_].push_back(elem_t(sessionID,channelID,pieceID,ip,timeOut));
		else
		{
			urgentinserverWhich_++;
			urgentinserverWhich_%=que_.size();
			urgentque_[urgentinserverWhich_].push_back(elem_t(sessionID,channelID,pieceID,ip,timeOut));
			urgentlastSession_=sessionID;
		}

	}
	void urgentPop()
	{
		OutputAssert(m_app);
		urgentque_[urgentpopWhich_].pop_front();
		urgentpopWhich_++;
		urgentpopWhich_%=urgentque_.size();
	}
	elem_t urgentFront()
	{
		OutputAssert(m_app);
		for (size_t i=0;i<que_.size();i++)
		{
			if(!urgentque_[urgentpopWhich_].empty())
				return urgentque_[urgentpopWhich_].front();
			else
			{
				urgentpopWhich_++;
				urgentpopWhich_%=urgentque_.size();
			}
		}
	}
	bool urgentEmpty()
	{
		OutputAssert(m_app);
		for (size_t i=0;i<urgentque_.size();i++)
		{
			if(!urgentque_[i].empty())
				return false;
		}
		return true;
	}

private:
	vector<list<elem_t> >que_;
	id_t  lastSession_;
	int popWhich_;
	int inserverWhich_;
	int priority_;

	vector<list<elem_t> > urgentque_;
	id_t  urgentlastSession_;
	int urgentpopWhich_;
	int urgentinserverWhich_;
	int urgentPriority_;


	Application* m_app;
};


#endif//_P2PSTREAM_PENDING_QUE_
