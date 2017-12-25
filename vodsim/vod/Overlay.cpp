#include "Vod.h"

//////////////////////////////////////////////////////////////////////////
//�ھӽڵ���֯
addr_t VodApp::eraseNeightbors()
{
	if (m_intreastingNB.size()<0.9*NEIGHTBOR_COUNT)
		return INVALID_ADDR;
	intreasting_nb_map_t::iterator nbItr(m_intreastingNB.begin());
	rate_Bps_t minContribute=INT_MAX;
	intreasting_nb_map_t::iterator minNbItr(m_intreastingNB.end());
	for(;nbItr!=m_intreastingNB.end();++nbItr)
	{
		NeightborInfo& info=nbItr->second;
		rate_Bps_t contribute=info.contribute.getSpeed();
		if (contribute<minContribute)
		{
			minContribute=contribute;
			minNbItr=nbItr;
		}
	}
	if (minContribute<INT_MAX
		&&minContribute<0.06*MEDIA_BPS
		&&minNbItr->second.beNbTime+15<NOW()
		)
	{
		addr_t ip=minNbItr->first;
		m_intreastingNB.erase(minNbItr);
		return ip;
	}
	
	return INVALID_ADDR;
}

void VodApp::updataNeightbors()
{
	//��Ӧ������һ�ڵ����IS���ͺ����ڵ�������Ϣ
	//����������ݵ��Ȳ��ԣ������㷨����

	time_s_t targetPlayingPoint=m_player.playingPoint();
	if (m_player.playingSeqNum()<0)
		targetPlayingPoint=0;
	bucket_id_t bucket=playingPoint2DynamicBucketID(targetPlayingPoint);
	if(bucket!=m_dynamicBucket)
		m_intreastingNB.clear();

	//1.�Ƿ񽫷���������Ϊ�ھ�?
	m_supperNeightbor.ability=(int)(1.2*(MEDIA_BPS/PIECE_LEN)*PIECE_REQUEST_CYCLE_TIME);
	if (addr()<8)
		m_supperBeNeightbor=true;
	else
		m_supperBeNeightbor=false;

	//2.ɾ��һЧ�ʵ͵Ľڵ�
	addr_t erasedNbr=eraseNeightbors();

	//3.���Һ����ڵ�
	//�ӱ�������Ͱ��ýڵ�
	list<addr_t>& lst=
		s_dynamicBucketMap[m_playingChannelID][bucket];
	if (!lst.empty()&&m_intreastingNB.size()<NEIGHTBOR_COUNT)
	{
		//������ڵ㷢���ھ�����
		list<addr_t>::iterator itr(lst.begin());
		for (size_t i=0;i<lst.size()&&m_intreastingNB.size()<NEIGHTBOR_COUNT;++i,++itr)
		{
			VodApp* thatApp=(VodApp*)(Node::get(*itr)->app());
			time_s_t hisPlayingPoint=thatApp->m_player.playingPoint();
			if (m_intreastingNB.find(*itr)==m_intreastingNB.end()
				&&(abs(hisPlayingPoint-targetPlayingPoint)<16
				||hisPlayingPoint-targetPlayingPoint<20
				&&hisPlayingPoint-targetPlayingPoint>0)
				&&*itr!=erasedNbr
				&&Rand01()<1.0/(1+m_intreastingNB.size())
				&&*itr!=addr()
				)
			{
				m_intreastingNB[*itr]=NeightborInfo();
				sendNeightborRequest(*itr);
			}
		}
	}
	//������뱾Ͱ���൱��IS�����ڵ�������������ݿ⣩
	if(bucket!=m_dynamicBucket)
	{
		leaveFromCurrentDynamicBucket();
		lst.push_back(addr());
		m_dynamicBucket=bucket;
	}

	//����Ͱ��ýڵ�
	if (m_intreastingNB.size()<NEIGHTBOR_COUNT)
	{
		if (targetPlayingPoint+BUCKET_TIME+1<MEDIA_TIME)
		{
			list<addr_t>& lst=s_dynamicBucketMap[m_playingChannelID][playingPoint2DynamicBucketID(targetPlayingPoint+BUCKET_TIME+1)];
			if (!lst.empty()&&m_intreastingNB.size()<NEIGHTBOR_COUNT)
			{
				//����������ڵ㷢������Ļ���map���
				list<addr_t>::iterator itr(lst.begin());
				for (size_t i=0;i<lst.size()&&m_intreastingNB.size()<NEIGHTBOR_COUNT;++i,++itr)
				{
					VodApp* thatApp=(VodApp*)(Node::get(*itr)->app());
					time_s_t hisPlayingPoint=thatApp->m_player.playingPoint();
					if (m_intreastingNB.find(*itr)==m_intreastingNB.end()
						&&(abs(hisPlayingPoint-targetPlayingPoint)<16
						||hisPlayingPoint-targetPlayingPoint<20
						&&hisPlayingPoint-targetPlayingPoint>0)
						&&*itr!=erasedNbr
						&&Rand01()<1.0/(1+m_intreastingNB.size())
						&&*itr!=addr()
						)
					{
						m_intreastingNB[*itr]=NeightborInfo();
						sendNeightborRequest(*itr);
					}
				}
			}
		}
	}

	//����Ͱ��ýڵ�
	if (m_intreastingNB.size()<NEIGHTBOR_COUNT)
	{
		if (targetPlayingPoint-BUCKET_TIME-1>0)
		{
			list<addr_t>& lst=s_dynamicBucketMap[m_playingChannelID][playingPoint2DynamicBucketID(targetPlayingPoint-BUCKET_TIME-1)];
			if (!lst.empty()&&m_intreastingNB.size()<NEIGHTBOR_COUNT)
			{
				//����������ڵ㷢������Ļ���map���
				list<addr_t>::reverse_iterator itr(lst.rbegin());
				for (size_t i=0;i<lst.size()&&m_intreastingNB.size()<NEIGHTBOR_COUNT;++i,++itr)
				{
					VodApp* thatApp=(VodApp*)(Node::get(*itr)->app());
					time_s_t hisPlayingPoint=thatApp->m_player.playingPoint();
					if (m_intreastingNB.find(*itr)==m_intreastingNB.end()
						&&(abs(hisPlayingPoint-targetPlayingPoint)<16
						||hisPlayingPoint-targetPlayingPoint<20
						&&hisPlayingPoint-targetPlayingPoint>0)
						&&*itr!=erasedNbr
						&&Rand01()<1.0/(1+m_intreastingNB.size())
						&&*itr!=addr()
						)
					{
						m_intreastingNB[*itr]=NeightborInfo();
						sendNeightborRequest(*itr);
					}
				}
			}
		}
	}

	
}

void VodApp::onNeightborRequestMsg(Packet* pkt)
{
	VodApp* thatApp=(VodApp*)(Node::get(pkt->srcAddr())->app());
	time_s_t hisPlayingPoint=thatApp->m_player.playingPoint();
	time_s_t myPlayingPoint=m_player.playingPoint();
	time_s_t playintPointDisstense=abs(myPlayingPoint-hisPlayingPoint);

//	���[(�����ڵ㼯��С)����(����ɾ��ĳ��Ч�ʽڵ�)]����[���ŵ����]
	if ((m_intreastingNB.size()<NEIGHTBOR_COUNT||eraseNeightbors()!=INVALID_ADDR)
		&&playintPointDisstense<BUFFER_TIME)
	{
		map<addr_t,NeightborInfo>::iterator itr=m_intreastingNB.find(pkt->srcAddr());
		if (itr==m_intreastingNB.end())
			m_intreastingNB[pkt->srcAddr()]=NeightborInfo();
	}
}

//////////////////////////////////////////////////////////////////////////
//���ڵ���֯
addr_t VodApp::eraseFathers(bool must)
{
	//��������ڵ�ĸ�������������һ�����ʣ������ش��������Լ�����������0.9
	if (m_fathers.size()>=FATHER_COUNT)
	{
		father_map_t::iterator faItr(m_fathers.begin());
		father_map_t::iterator eraseItr(m_fathers.end());
		rate_Bps_t minContribute=INT_MAX;
		addr_t     minFa=INVALID_ADDR;
		//rate_Bps_t allContribte=0;
		for(;faItr!=m_fathers.end();++faItr)
		{
			FatherInfo& info=faItr->second;
			rate_Bps_t contribute=info.contribute.getSpeed();
			//allContribte+=contribute;
			if (contribute<minContribute)
			{
				minContribute=contribute;
				minFa=faItr->first;
				eraseItr=faItr;
			}
		}
		if (must
			||(eraseItr!=m_fathers.end()&&minContribute<0.1*MEDIA_BPS&&Rand01()<0.8*(1-1.0/(1+m_fathers.size()))))
		{
			m_fathers.erase(eraseItr);
			//���͸����ڵ�Ҫ��Ͽ�
			sendTo(CONTROL_MSG_LEN,minFa,Packet::newPacket(MSG_NOT_BE_MY_FATHER));
		//	m_uselessPush=0;
		//	m_usefulPush=0;
			return minFa;
		}
	}
	return INVALID_ADDR;
}

void VodApp::updataFathers()
{
	
	//����������д���С�������ڴ����0.9�����ӽ��������٣��������ӽڵ�
	rate_Bps_t downAllRate=m_downlinkAllRate.getSpeed();
	rate_Bps_t neightborContrib=m_neightborsContribute.getSpeed();
	rate_Bps_t usefulPushedRate=m_usefulPushedRate.getSpeed();
	if (downAllRate>0.98*m_maxDownloadRate
		&&downAllRate>MEDIA_BPS
		&&Rand01()<0.2)
	{
		eraseFathers(true);
		return;
	}
	if (m_fathers.size()<=FATHER_COUNT
		&&downAllRate<1.2*MEDIA_BPS
		&&neightborContrib<0.92*MEDIA_BPS
		&&usefulPushedRate<0.10*MEDIA_BPS
		)
	{
		addr_t erasedNbr=eraseFathers();

		time_s_t myPlayingPoint=m_player.playingPoint();
		if (m_player.playingSeqNum()<0)
			myPlayingPoint=0;
		bucket_id_t bucket=playingPoint2DynamicBucketID(myPlayingPoint);
		if(bucket!=m_dynamicBucket)
			m_intreastingNB.clear();

		//�ӱ�������Ͱ��ýڵ�
		list<addr_t>& lst=
			s_dynamicBucketMap[m_playingChannelID][bucket];
		if (!lst.empty()&&m_fathers.size()<FATHER_COUNT)
		{
			//������ڵ㷢���ھ�����
			list<addr_t>::iterator itr(lst.begin());
			for (size_t i=0;i<lst.size()&&m_fathers.size()<FATHER_COUNT;++i,++itr)
			{
				VodApp* thatApp=(VodApp*)(Node::get(*itr)->app());
				time_s_t hisPlayingPoint=thatApp->m_player.playingPoint();
				if (m_intreastingNB.find(*itr)==m_intreastingNB.end()
					&&m_fathers.find(*itr)==m_fathers.end()
					&&*itr!=erasedNbr
					&&(hisPlayingPoint-myPlayingPoint)>16
					&&(hisPlayingPoint-myPlayingPoint)<BUFFER_TIME
					&&Rand01()<0.8*(1.0/(1+m_fathers.size()))
					&&*itr!=addr()
					)
				{
					m_fathers[*itr]=FatherInfo();
					//���͸����ڵ�
					sendTo(CONTROL_MSG_LEN,*itr,Packet::newPacket(MSG_BE_MY_FATHER));
					//�򸸽ڵ㷢��PUSH����
					piece_id_t s=m_maxknownPiece+20.0/PLAY_TIME_PER_PIECE;
					piece_id_t e=m_player.playingSeqNum()+1+300/PLAY_TIME_PER_PIECE;
					RangeMsg* data=new RangeMsg;
					data->range.first=s;
					data->range.second=e;
					sendTo(CONTROL_MSG_LEN,*itr,Packet::newPacket(MSG_RANGE,data));
					
				}
			}
		}
		//����Ͱ��ýڵ�
		if (m_fathers.size()<FATHER_COUNT)
		{
			if (myPlayingPoint+BUCKET_TIME+1<MEDIA_TIME)
			{
				list<addr_t>& lst=s_dynamicBucketMap[m_playingChannelID][playingPoint2DynamicBucketID(myPlayingPoint+BUCKET_TIME+1)];
				if (!lst.empty()&&m_fathers.size()<FATHER_COUNT)
				{
					//����������ڵ㷢������Ļ���map���
					list<addr_t>::reverse_iterator itr(lst.rbegin());
					for (size_t i=0;i<lst.size()&&m_fathers.size()<FATHER_COUNT;++i,++itr)
					{
						VodApp* thatApp=(VodApp*)(Node::get(*itr)->app());
						time_s_t hisPlayingPoint=thatApp->m_player.playingPoint();
						if (m_intreastingNB.find(*itr)==m_intreastingNB.end()
							&&m_fathers.find(*itr)==m_fathers.end()
							&&*itr!=erasedNbr
							&&(hisPlayingPoint-myPlayingPoint)>16
							&&(hisPlayingPoint-myPlayingPoint)<BUFFER_TIME
							&&Rand01()<0.8*(1.0/(1+m_fathers.size()))
							&&*itr!=addr()
							)
						{
							m_fathers[*itr]=FatherInfo();
							sendTo(CONTROL_MSG_LEN,*itr,Packet::newPacket(MSG_BE_MY_FATHER));
							//�򸸽ڵ㷢��PUSH����
							piece_id_t s=m_maxknownPiece+20.0/PLAY_TIME_PER_PIECE;
							piece_id_t e=m_player.playingSeqNum()+1+BUFFER_TIME/PLAY_TIME_PER_PIECE;
							RangeMsg* data=new RangeMsg;
							data->range.first=s;
							data->range.second=e;
							sendTo(CONTROL_MSG_LEN,*itr,Packet::newPacket(MSG_RANGE,data));
						}
					}
				}
			}
		}
	}
}

void VodApp::onChildToFatherConnectMsg(Packet* pkt)
{
	if (m_children.find(pkt->srcAddr())!=m_children.end())
		return;
	eraseChildren();
	//������д���ʹ���ʺܸߣ��ܾ�
	if (
		m_children.size()>=CHILDREN_COUNT
		||m_maxUpRate<MEDIA_BPS
		||m_maxUpRate/m_children.size()<0.3*MEDIA_TIME
		)
	{
		//���͸��ڵ�Ҫ��Ͽ�
		sendTo(CONTROL_MSG_LEN,pkt->srcAddr(),Packet::newPacket(MSG_NOT_BE_MY_CHILD));
	}
	else
	{
		m_children[pkt->srcAddr()]=ChildInfo();
	}
}


void VodApp::onChildToFatherDisconnectMsg(Packet* pkt)
{
	m_children.erase(pkt->srcAddr());
}

//////////////////////////////////////////////////////////////////////////
//�ӵ���֯
addr_t VodApp::eraseChildren()
{
	//������ӽڵ�ĸ�������������һ�����ʣ������ش������Լ����д���������0.9
	if (m_children.size()>=CHILDREN_COUNT
		//m_outgoingRate.getSpeed()>0.9*m_maxUpRate
		)
	{
		child_map_t::iterator chItr(m_children.begin());
		child_map_t::iterator eraseItr(m_children.end());
		rate_Bps_t minContribute=INT_MAX;
		addr_t     minCh=INVALID_ADDR;
		//rate_Bps_t allContribte=0;
		for(;chItr!=m_children.end();++chItr)
		{
			ChildInfo& info=chItr->second;
			rate_Bps_t contribute=info.contribute.getSpeed();
			//allContribte+=contribute;
			if (contribute<minContribute)
			{
				minContribute=contribute;
				minCh=chItr->first;
				eraseItr=chItr;
			}
		}
		if (eraseItr!=m_children.end()&&minContribute<0.1*MEDIA_BPS&&Rand01()<0.4)
		{
			m_children.erase(eraseItr);
			//���͸��ӽڵ�Ҫ��Ͽ�
			sendTo(CONTROL_MSG_LEN,minCh,Packet::newPacket(MSG_NOT_BE_MY_CHILD));
			return minCh;
		}
	}
	return INVALID_ADDR;
}

void VodApp::onFatherToChildDisconnectMsg(Packet* pkt)
{
	m_fathers.erase(pkt->srcAddr());
}

void VodApp::findNewChildren()
{
	child_map_t::iterator chItr(m_children.begin());
	for(;chItr!=m_children.end();++chItr)
		chItr->second.pusedCntCurrCycle;


	//addr_t erasedNbr=eraseChildren();
	////����������д���С�������ڴ����0.9�����ӽ��������٣��������ӽڵ�
	//if (m_children.size()<CHILDREN_COUNT
	//	&&m_outgoingRate.getSpeed()<0.9*m_maxUpRate
	//	)
	//{
	//	time_s_t myPlayingPoint=m_player.playingPoint();
	//	if (m_player.playingSeqNum()<0)
	//		myPlayingPoint=0;
	//	bucket_id_t bucket=playingPoint2DynamicBucketID(myPlayingPoint);
	//	if(bucket!=m_dynamicBucket)
	//		m_neighbors.clear();

	//	//�ӱ�������Ͱ��ýڵ�
	//	list<addr_t>& lst=
	//		s_dynamicBucketMap[m_playingChannelID][bucket];
	//	if (!lst.empty()&&m_children.size()<CHILDREN_COUNT)
	//	{
	//		//������ڵ㷢���ھ�����
	//		list<addr_t>::reverse_iterator itr(lst.rbegin());
	//		for (size_t i=0;i<lst.size()&&m_children.size()<CHILDREN_COUNT;++i,++itr)
	//		{
	//			VodApp* thatApp=(VodApp*)(Node::get(*itr)->app());
	//			time_s_t hisPlayingPoint=thatApp->m_player.playingPoint();
	//			if (m_neighbors.find(*itr)==m_neighbors.end()
	//				&&m_children.find(*itr)==m_children.end()
	//				&&*itr!=erasedNbr
	//				&&(myPlayingPoint-hisPlayingPoint)>URGENT_1_REQUEST_TIME
	//				&&(myPlayingPoint-hisPlayingPoint)<BUFFER_TIME
	//				)
	//			{
	//				m_children[*itr]=ChildInfo();
	//				//���͸��ӽڵ�
	//				sendTo(CONTROL_MSG_LEN,*itr,Packet::newPacket(MSG_BE_MY_CHILD));
	//			}
	//		}
	//	}
	//	//����Ͱ��ýڵ�
	//	if (m_children.size()<CHILDREN_COUNT)
	//	{
	//		if (myPlayingPoint-BUCKET_TIME-1>0)
	//		{
	//			list<addr_t>& lst=s_dynamicBucketMap[m_playingChannelID][playingPoint2DynamicBucketID(myPlayingPoint-BUCKET_TIME-1)];
	//			if (!lst.empty()&&m_children.size()<CHILDREN_COUNT)
	//			{
	//				//������ڵ㷢���ھ�����
	//				list<addr_t>::iterator itr(lst.begin());
	//				for (size_t i=0;i<lst.size()&&m_children.size()<CHILDREN_COUNT;++i,++itr)
	//				{
	//					VodApp* thatApp=(VodApp*)(Node::get(*itr)->app());
	//					time_s_t hisPlayingPoint=thatApp->m_player.playingPoint();
	//					if (m_neighbors.find(*itr)==m_neighbors.end()
	//						&&m_children.find(*itr)==m_children.end()
	//						&&*itr!=erasedNbr
	//						&&(myPlayingPoint-hisPlayingPoint)>URGENT_1_REQUEST_TIME
	//						&&(myPlayingPoint-hisPlayingPoint)<BUFFER_TIME
	//						)
	//					{
	//						m_children[*itr]=ChildInfo();
	//						//���͸��ӽڵ�
	//						sendTo(CONTROL_MSG_LEN,*itr,Packet::newPacket(MSG_BE_MY_CHILD));
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}

void VodApp::onFatherToChildConnectMsg(Packet* pkt)
{
	if (m_fathers.find(pkt->srcAddr())!=m_fathers.end())
		return;
	//������д���ʹ���ʺܸߣ��ܾ�
	if (m_downlinkAllRate.getSpeed()>0.7*m_maxDownloadRate
		//||m_neighbors.find(pkt->srcAddr())!=m_neighbors.end()
		||m_player.state()==Player::PLAYING&&m_player.continuousTime()<2.0
		)
	{
		//���͸����ڵ�Ҫ��Ͽ�
		sendTo(CONTROL_MSG_LEN,pkt->srcAddr(),Packet::newPacket(MSG_NOT_BE_MY_FATHER));
	}
	else
	{
		if(m_fathers.size()<FATHER_COUNT
			||eraseChildren()!=INVALID_ADDR)
		{
			m_fathers[pkt->srcAddr()]=FatherInfo();
			//�򸸽ڵ㷢��PUSH����
			piece_id_t s=m_maxknownPiece+10.0/PLAY_TIME_PER_PIECE;
			piece_id_t e=m_player.playingSeqNum()+1+BUFFER_TIME/PLAY_TIME_PER_PIECE;
			RangeMsg* data=new RangeMsg;
			data->range.first=s;
			data->range.second=e;
			sendTo(CONTROL_MSG_LEN,pkt->srcAddr(),Packet::newPacket(MSG_RANGE,data));
		}
		else
		{
			//���͸����ڵ�Ҫ��Ͽ�
			sendTo(CONTROL_MSG_LEN,pkt->srcAddr(),Packet::newPacket(MSG_NOT_BE_MY_FATHER));
		}
	}
}

void VodApp::onRangeMsg(Packet* pkt)
{
	RangeMsg* data=(RangeMsg*)(pkt->data());
	child_map_t::iterator itr(m_children.find(pkt->srcAddr()));
	if (itr!=m_children.end())
		itr->second.range=data->range;
}