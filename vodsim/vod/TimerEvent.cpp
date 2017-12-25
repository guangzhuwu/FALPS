#include "Vod.h"
/************************************************************************/
/* 定时事件                                                           */
/************************************************************************/

//加入
void VodApp::onJoinTimer()
{
	m_stoped=false;
	m_updataNeightborsTimer.resched(0);
	m_requestPieceTimer.resched(1);
	m_sendBufferMapTimer.resched(2);
	m_updataChildrenTimer.resched((1+Rand01())*UPDATA_OVERLAY_TIME);
	m_updataFathersTimer.resched((1+Rand01())*UPDATA_OVERLAY_TIME);
}

void VodApp::onLeaveTimer()
{
	leaveFromCurrentDynamicBucket();
	m_playingBuffer.clearAll();
	m_player.stopPlay();
	m_stoped=true;
	m_updataNeightborsTimer.cancel();
	m_requestPieceTimer.cancel();
	m_sendBufferMapTimer.cancel();
	m_updataChildrenTimer.cancel();
	m_updataFathersTimer.cancel();
	s_onlineNodes--;
}

//周期性更新合作节点定时器
void VodApp::onUpdataNeightborsTimer()
{
	updataNeightbors();
	m_updataNeightborsTimer.resched(UPDATA_OVERLAY_TIME*Rand(0.8,1.2));
}

//周期性更新父节点定时器
void VodApp::onUpdataFathersTimer()
{
	updataFathers();
	m_updataFathersTimer.resched(UPDATA_OVERLAY_TIME*Rand(0.8,1.2));
}

//周期性更新子节点定时器
void VodApp::onUpdataChildrenTimer()
{
	findNewChildren();
	m_updataChildrenTimer.resched(UPDATA_OVERLAY_TIME*Rand(0.8,1.2));
}

//周期性buffermap通告定时器
void VodApp::onSendBufferMapTimer()
{
	//向父节点发送自己Rang信息
	father_map_t::iterator faItr(m_fathers.begin());
//	piece_id_t s=m_player.playingSeqNum()+1+(m_player.continuousTime()*2+2*PIECE_REQUEST_CYCLE_TIME)/PLAY_TIME_PER_PIECE;
	//piece_id_t e=m_player.playingSeqNum()+1+BUFFER_TIME/PLAY_TIME_PER_PIECE;
	piece_id_t s=m_maxknownPiece+20.0/PLAY_TIME_PER_PIECE;
	piece_id_t e=m_player.playingSeqNum()+1+300/PLAY_TIME_PER_PIECE;
	for (;faItr!=m_fathers.end();++faItr)
	{
		RangeMsg* data=new RangeMsg;
		data->range.first=s;
		data->range.second=e;
		sendTo(CONTROL_MSG_LEN,faItr->first,Packet::newPacket(MSG_RANGE,data));
	}

	//向所有合作节点发送buffermap
	intreasting_nb_map_t::iterator nbItr(m_intreastingNB.begin());
	for(;nbItr!=m_intreastingNB.end();++nbItr)
		sendTo(200,nbItr->first,Packet::newPacket(MSG_BUFFERMAP));
	//这里是使所有节点同时收到这一消息，与实际有一定误差
	//这里的“立刻收到”和实际不会有太大出入：应该是send后+t时间才能收到
	//但是，这里的buffermap周期发送，则每个周期后+t和不+t不会有问题。
	//这里关键仿真了这个buffermap交换对网络的负载情况。
	m_playingBuffer.updataGloableMap();
	m_sendBufferMapTimer.resched(BUFFERMAP_CYCLE_TIME*Rand(0.5,1.5));
}

//周期性数据请求定时器
void VodApp::onRequestPieceTimer()
{
	//运行数据调度策略
	scheduleRequest();
	//定时进行下一轮调度
	m_requestPieceTimer.resched(PIECE_REQUEST_CYCLE_TIME);
}