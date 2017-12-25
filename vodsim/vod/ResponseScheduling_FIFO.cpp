#include "Vod.h"
void VodApp::responseScheduling_FIFO(Packet*pkt)
{

	if (pkt)
	{
		//将请求放入响应队列
		PieceRequestMsg* data=(PieceRequestMsg*)(pkt->data());
		vector<piece_id_t>::iterator itr(data->normalPieces.begin());
		size_t shouldSendCnt=data->normalPieces.size();
		size_t realSendCnt=0;

		//根据对方的上行带宽决定插入哪个响应队列
		size_t which=0;
		if (BackboneNet::instance().getNodeToBackboneBw(pkt->srcAddr())>=2*MEDIA_BPS)
			which=2;
		else if (BackboneNet::instance().getNodeToBackboneBw(pkt->srcAddr())>=MEDIA_BPS)
			which=1;

		which=0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		ResponseFifoQue& resQue=m_responsePendingQue[which];

		double dlt=(PIECE_REQUEST_CYCLE_TIME+0.1)/(data->urgentPieces.size()+data->normalPieces.size());
		time_s_t delay=dlt;
		vector<piece_id_t>::iterator urgentItr(data->urgentPieces.begin());
		for (int i=0;urgentItr!=data->urgentPieces.end();++urgentItr,++i)
		{
			piece_id_t piece=*urgentItr;
			if (addr()==SERVER_ADDRESS
				||m_playingBuffer.exist(piece)
				)
			{
				time_s_t timeout=NOW()+1*PIECE_REQUEST_CYCLE_TIME;
				m_urgentResponsePendingQue.urgentPush(data->sessionID,data->channelID,piece,pkt->srcAddr(),timeout);
				delay+=dlt;
			}
		}

		for (int i=0;itr!=data->normalPieces.end();++itr,++i)
		{
			piece_id_t piece=*itr;
			if (addr()==SERVER_ADDRESS
				||m_playingBuffer.exist(piece)
				)
			{
				time_s_t timeout=NOW()+PIECE_REQUEST_CYCLE_TIME;
				resQue.push(data->sessionID,data->channelID,piece,pkt->srcAddr(),timeout);
				delay+=dlt;
			}
		}
	}

	//如果当前上行带宽空闲，响应邻居节点的数据请求，发送数据片段
	if (!m_sendingPiecePkt)
	{
		for (;!m_urgentResponsePendingQue.urgentEmpty();)
		{
			ResponseFifoQue::elem_type elem=m_urgentResponsePendingQue.urgentFront();
			if (
				addr()==SERVER_ADDRESS&&elem.timeOut>NOW()
				||(elem.timeOut>NOW()&&m_playingBuffer.exist(elem.pieceID))
				)
			{
				PieceMsg* p=new PieceMsg;
				p->pieceID=elem.pieceID;
				p->channelID=elem.channelID;
				p->sessionID=elem.sessionID;
				sendTo(PIECE_LEN,elem.address,Packet::newPacket(MSG_PIECE,p));
				m_urgentResponsePendingQue.urgentPop();
				m_sendingPiecePkt=true;
				m_uplinkAllRate+=PIECE_LEN;
				m_uplinkPulledRate+=PIECE_LEN;
				s_uplinkAllRate+=PIECE_LEN;
				s_uplinkPulledRate+=PIECE_LEN;
				m_maxUpRate=max(m_uplinkAllRate.getSpeed(),m_maxUpRate);
				return;
			}
			else
			{
				m_urgentResponsePendingQue.urgentPop();
			}
		}

		size_t  which=0;
	/*	if (!m_responsePendingQue[2].empty())
		{
			if (Rand01()<0.4)
				which=2;
			else if (!m_responsePendingQue[1].empty())
			{
				if (Rand01()<0.6)
					which=1;
			}
		}
		else if (!m_responsePendingQue[1].empty())
		{
			if (Rand01()<0.6)
				which=1;
		}*/

		ResponseFifoQue& resQue=m_responsePendingQue[which];

		
		for (;!resQue.empty();)
		{
			ResponseFifoQue::elem_type elem=resQue.front();
			if (elem.timeOut>NOW()
				&&(addr()==SERVER_ADDRESS||m_playingBuffer.exist(elem.pieceID))
				)
			{
				PieceMsg* p=new PieceMsg;
				p->pieceID=elem.pieceID;
				p->channelID=elem.channelID;
				p->sessionID=elem.sessionID;
				sendTo(PIECE_LEN,elem.address,Packet::newPacket(MSG_PIECE,p));
				resQue.pop();
				m_sendingPiecePkt=true;
				m_uplinkAllRate+=PIECE_LEN;
				m_uplinkPulledRate+=PIECE_LEN;
				s_uplinkAllRate+=PIECE_LEN;
				s_uplinkPulledRate+=PIECE_LEN;
				m_maxUpRate=max(m_uplinkAllRate.getSpeed(),m_maxUpRate);
				return;
			}
			else
			{
				resQue.pop();
			}
		}
	}
	//如果没有邻居节点的数据请求当前上行带宽空闲，则向子节点PUSH数据
	if (1&&!m_sendingPiecePkt&&addr()!=SERVER_ADDRESS)
	{
		if (!m_children.empty())
		{
			for (int k=0;k<10;k++)
			{
				size_t n=RandUint32()%m_children.size();
				child_map_t::iterator itr(m_children.begin());
				for (size_t i=0;i<n;++i)
					++itr;
				piece_id_t s=itr->second.range.first;
				piece_id_t e=itr->second.range.second;
				if (s<0||e<0||itr->second.pusedCntCurrCycle>0.1*(BUFFERMAP_CYCLE_TIME/PLAY_TIME_PER_PIECE))
				{
					continue;
				}
				addr_t dst=itr->first;
				for (int i=0;i<15;i++)
				{
					piece_id_t piece=(piece_id_t)Rand(s,e);
					//用StrmBuffer::exist(a,i)表示改节点是从buffermap交换中获得的信息，不一定准确
					VodApp* p=(VodApp*)(Node::get(dst)->app());
					if (m_playingBuffer.exist(piece)
						&&!p->m_playingBuffer.exist(piece)
						&&p->m_requestPendingQue.timeOut(piece)<0
						&&!StrmBuffer::exist(dst,piece)
						)
					{
						PieceMsg* p=new PieceMsg;
						p->pieceID=piece;
						p->channelID=-1;
						p->sessionID=INVALID_ID;
						sendTo(PIECE_LEN,dst,Packet::newPacket(MSG_PIECE,p));
						m_uplinkAllRate+=PIECE_LEN;
						m_uplinkPushRate+=PIECE_LEN;
						s_uplinkAllRate+=PIECE_LEN;
						s_uplinkPushRate+=PIECE_LEN;
						m_maxUpRate=max(m_uplinkAllRate.getSpeed(),m_maxUpRate);
						m_sendingPiecePkt=true;
						itr->second.pusedCntCurrCycle++;
						itr->second.contribute+=PIECE_LEN;
						return;
					}
				}
			}
		}
		//else
		//	cout<<"好闲好闲好闲好闲好闲好闲好闲好闲好闲好闲好闲好闲好闲"<<addr()<<endl;
	}
	else
	{
		for (int i=0;i<15;i++)
		{
			RandUint32();
		}
	}
}