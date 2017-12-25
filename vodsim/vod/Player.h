/********************************************************************
文件: player.h
日期: 18:1:2007   13:53
作者: 武广柱
说明: 模拟一个播放器

Copyright (C) 2007 - All Rights Reserved
*********************************************************************/
#ifndef _FALPS_PLAYER_
#define _FALPS_PLAYER_

#include "Falps.h"
#include "StrmBuffer.h"
#include "ParamDef.h"
#include <sstream>
using namespace std;
class VodApp;

class Player
{
	class PlayerTimer : public TimerHandler
	{
	public :
		PlayerTimer()
			:player_(NULL)
		{
		}
		void bindPlayer(Player* player)
		{
			player_=player;
		}
	protected:
		virtual void expire(Event*)
		{
			OutputAssert(player_!=NULL);
			player_->onNextCell();
		}
		Player* player_;
	};
public:
	enum PlayerState{
		NOT_START,
		PLAYING,
		STOPED,
		BLOCK_ABSENT,
	};

public:
	Player()
		:m_startPlayTime(-1)
		,m_currSeqStartPlayingTime(-1)
		,m_endPlaySeq(-1)
		,m_buffer(NULL)
		,m_app(NULL)
		,m_state(NOT_START)
		,m_playingSeqNum(-1)
		,m_urgentTime(4)
	{
		m_timer.bindPlayer(this);
		m_badpaly=false;
	}
public:
	//绑定一个buffer
	void bindBuffer(StrmBuffer& buffer)
	{
		m_buffer=&buffer;
	}
	
	//绑定一个application
	void bindApp(Application& app)
	{
		m_app=&app;
	}
	
	//紧急调度时间
	void setUrgentTime(time_s_t t)
	{
		m_urgentTime=t;
	}
	
	//当前正在播放的单元的ID
	int playingSeqNum()
	{
		OutputAssert(m_buffer);
		if(m_state==STOPED) 
			return  -1;
		return  m_playingSeqNum;
	}

	//获取开始播放时间
	time_s_t startePlayTime()
	{
		return m_startPlayTime;
	}
	
	//当前的播放点
	time_s_t playingPoint();

	//开始播放
	void startPlay(int startID=0,int endID=-1);

	//继续播放
	void continuePlay()
	{
		OutputAssert(m_buffer);
		onNextCell();
	}

	//停止播放
	void stopPlay()
	{
		m_stopPlayTime=NOW();
		m_timer.cancel();
		m_state=STOPED;
	}

	//从当期播放点，缓存中能连续播放的时间
	time_s_t continuousTime();

	//状态
	PlayerState state()
	{
		return m_state;
	}

	//获取播放流畅度
	double getQuality();
public:
	void onNextCell();
	void onCellAbsent();
	void onStop();
protected:
	time_s_t m_startPlayTime;
	time_s_t m_stopPlayTime;
	block_id_t m_playingSeqNum;//正播放的block号
	block_id_t m_startPlayBlock;//开始播放的block号
	block_id_t m_endPlaySeq;//终止播放的block号
	time_s_t m_currSeqStartPlayingTime;//当前播放的块的播放时间
	StrmBuffer* m_buffer;
	Application* m_app;
	PlayerTimer m_timer;
	PlayerState m_state;
	time_s_t m_urgentTime;
	bool m_badpaly;
};


#endif//_FALPS_PLAYER_

