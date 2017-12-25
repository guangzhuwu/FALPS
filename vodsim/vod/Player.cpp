#include "Player.h"
#include "Vod.h"

time_s_t Player::playingPoint()
{
	OutputAssert(m_buffer);
	if (m_playingSeqNum<0)
		return -1;
	time_s_t point=m_playingSeqNum*(PLAY_TIME_PER_PIECE)+(NOW()-m_currSeqStartPlayingTime);
	return min(point,MEDIA_TIME);
}

time_s_t Player::continuousTime()
{
	OutputAssert(m_buffer);
	int n=m_buffer->continuousNum(m_playingSeqNum);
	if (m_state==PLAYING)
		return (PLAY_TIME_PER_PIECE-(NOW()-m_currSeqStartPlayingTime))+PLAY_TIME_PER_PIECE*n;
	else
		return PLAY_TIME_PER_PIECE*n;

}

void Player::onCellAbsent()
{
	if(m_app)
	{
		/*LOG(	
			ostringstream s;
		s<<"Node("<<m_app->addr()<<")'Player stops because of BlockAbsent";
		LogInfo(s.str().c_str());
		);*/
	}
}

void Player::onStop()
{
	if(m_app)
	{
		/*LOG(	
			ostringstream s;
		s<<"Node("<<m_app->addr()<<")'Player stops because of end";
		LogInfo(s.str().c_str());
		);*/
	}
}

double Player::getQuality()
{
	if (state()==NOT_START)
		return -1;
	else if (m_badpaly==false)
		return 1.0;
	else
	{
		time_s_t t=NOW()-m_currSeqStartPlayingTime;
		if (t>PLAY_TIME_PER_PIECE)
			t=PLAY_TIME_PER_PIECE;
		return (PLAY_TIME_PER_PIECE*m_playingSeqNum+t)/(NOW()-m_startPlayTime);
	}
}

//开始播放
void Player::startPlay(int startID,int endID)
{
	OutputAssert(NULL!=m_buffer);
	OutputAssert(m_state==NOT_START);

	if (endID<0)
		endID=(int)(MEDIA_TIME/PLAY_TIME_PER_PIECE);
	if (m_buffer->exist(startID))
	{
		m_playingSeqNum=startID;
		m_currSeqStartPlayingTime=NOW();
		m_timer.resched(PLAY_TIME_PER_PIECE);
		m_state=PLAYING;
	}
	else
	{
		onCellAbsent();
		m_badpaly=true;
		m_state=BLOCK_ABSENT;
	}
	m_startPlayBlock=startID;
	m_endPlaySeq=endID;
	m_startPlayTime=NOW();
	/*LOG(	
		ostringstream s;
	s<<"Node("<<m_app->addr()<<")'Player starts";
	LogInfo(s.str().c_str());
	);*/
}

void Player::onNextCell()
{
	OutputAssert(NULL!=m_buffer);

	//紧急状态回调
	
	for (int i=m_playingSeqNum;i<m_playingSeqNum+m_urgentTime/PLAY_TIME_PER_PIECE;++i)
	{
		VodApp* vodApp=(VodApp*)m_app;

		if (!m_buffer->exist(i))
		{
			time_s_t t=vodApp->requestPendingTimeOut(i);
			{
				if (t>NOW())
					continue;
			}
			vodApp->onPlayerUrgent(i);
			break;
		}
	}
	//播放完毕？
	if (m_playingSeqNum>m_endPlaySeq)
	{
		m_state=STOPED;
		m_stopPlayTime=NOW();
		onStop();
		return;
	}

	//往下播放
	if (m_buffer->exist(m_playingSeqNum+1))
	{
		m_playingSeqNum++;
		m_currSeqStartPlayingTime=NOW();
		m_timer.resched(PLAY_TIME_PER_PIECE);
		if (m_state==BLOCK_ABSENT)
		{
		/*	LOG(	
				ostringstream s;
			s<<"Node("<<m_app->addr()<<")'Player restarts";
			LogInfo(s.str().c_str());
			);*/
		}
		m_state=PLAYING;
	}
	else
	{
		m_badpaly=true;
		m_state=BLOCK_ABSENT;
		onCellAbsent();
	}
}