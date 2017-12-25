/********************************************************************
�ļ�: player.h
����: 18:1:2007   13:53
����: �����
˵��: ģ��һ��������

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
	//��һ��buffer
	void bindBuffer(StrmBuffer& buffer)
	{
		m_buffer=&buffer;
	}
	
	//��һ��application
	void bindApp(Application& app)
	{
		m_app=&app;
	}
	
	//��������ʱ��
	void setUrgentTime(time_s_t t)
	{
		m_urgentTime=t;
	}
	
	//��ǰ���ڲ��ŵĵ�Ԫ��ID
	int playingSeqNum()
	{
		OutputAssert(m_buffer);
		if(m_state==STOPED) 
			return  -1;
		return  m_playingSeqNum;
	}

	//��ȡ��ʼ����ʱ��
	time_s_t startePlayTime()
	{
		return m_startPlayTime;
	}
	
	//��ǰ�Ĳ��ŵ�
	time_s_t playingPoint();

	//��ʼ����
	void startPlay(int startID=0,int endID=-1);

	//��������
	void continuePlay()
	{
		OutputAssert(m_buffer);
		onNextCell();
	}

	//ֹͣ����
	void stopPlay()
	{
		m_stopPlayTime=NOW();
		m_timer.cancel();
		m_state=STOPED;
	}

	//�ӵ��ڲ��ŵ㣬���������������ŵ�ʱ��
	time_s_t continuousTime();

	//״̬
	PlayerState state()
	{
		return m_state;
	}

	//��ȡ����������
	double getQuality();
public:
	void onNextCell();
	void onCellAbsent();
	void onStop();
protected:
	time_s_t m_startPlayTime;
	time_s_t m_stopPlayTime;
	block_id_t m_playingSeqNum;//�����ŵ�block��
	block_id_t m_startPlayBlock;//��ʼ���ŵ�block��
	block_id_t m_endPlaySeq;//��ֹ���ŵ�block��
	time_s_t m_currSeqStartPlayingTime;//��ǰ���ŵĿ�Ĳ���ʱ��
	StrmBuffer* m_buffer;
	Application* m_app;
	PlayerTimer m_timer;
	PlayerState m_state;
	time_s_t m_urgentTime;
	bool m_badpaly;
};


#endif//_FALPS_PLAYER_

