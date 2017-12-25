#include "Vod.h"
/************************************************************************/
/* ��ʱ�¼�                                                           */
/************************************************************************/

//����
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

//�����Ը��º����ڵ㶨ʱ��
void VodApp::onUpdataNeightborsTimer()
{
	updataNeightbors();
	m_updataNeightborsTimer.resched(UPDATA_OVERLAY_TIME*Rand(0.8,1.2));
}

//�����Ը��¸��ڵ㶨ʱ��
void VodApp::onUpdataFathersTimer()
{
	updataFathers();
	m_updataFathersTimer.resched(UPDATA_OVERLAY_TIME*Rand(0.8,1.2));
}

//�����Ը����ӽڵ㶨ʱ��
void VodApp::onUpdataChildrenTimer()
{
	findNewChildren();
	m_updataChildrenTimer.resched(UPDATA_OVERLAY_TIME*Rand(0.8,1.2));
}

//������buffermapͨ�涨ʱ��
void VodApp::onSendBufferMapTimer()
{
	//�򸸽ڵ㷢���Լ�Rang��Ϣ
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

	//�����к����ڵ㷢��buffermap
	intreasting_nb_map_t::iterator nbItr(m_intreastingNB.begin());
	for(;nbItr!=m_intreastingNB.end();++nbItr)
		sendTo(200,nbItr->first,Packet::newPacket(MSG_BUFFERMAP));
	//������ʹ���нڵ�ͬʱ�յ���һ��Ϣ����ʵ����һ�����
	//����ġ������յ�����ʵ�ʲ�����̫����룺Ӧ����send��+tʱ������յ�
	//���ǣ������buffermap���ڷ��ͣ���ÿ�����ں�+t�Ͳ�+t���������⡣
	//����ؼ����������buffermap����������ĸ��������
	m_playingBuffer.updataGloableMap();
	m_sendBufferMapTimer.resched(BUFFERMAP_CYCLE_TIME*Rand(0.5,1.5));
}

//��������������ʱ��
void VodApp::onRequestPieceTimer()
{
	//�������ݵ��Ȳ���
	scheduleRequest();
	//��ʱ������һ�ֵ���
	m_requestPieceTimer.resched(PIECE_REQUEST_CYCLE_TIME);
}