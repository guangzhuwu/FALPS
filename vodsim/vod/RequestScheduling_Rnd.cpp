#include "Vod.h"

vector<double> PriorityVec;
//void  zipf()
//{
//	PriorityVec.resize(8192);
//	double skewfactor=2.25;
//	double sum=0;
//	for (size_t i=0;i<PriorityVec.size();++i)
//	{
//		PriorityVec[i]=1.0/pow(double(i+1),1.0-skewfactor);
//		sum+=PriorityVec[i];
//	}
//	for (size_t i=0;i<PriorityVec.size();++i)
//	{
//		PriorityVec[i]/=sum;
//		PriorityVec[i]*=1.5;
//		//PriorityVec[i]=1;
//	}
//}

struct piece_info{
	piece_id_t pieceID;//片段号
	vector<NeightborInfo*>holders;//具有本片段的节
	double priority;
	bool  serverIn;
	bool  isUrgent;
	piece_info()
	{
		serverIn=false;
		isUrgent=false;
	}
};

struct PriorityCmp{
	bool operator() (const piece_info &elem1, const piece_info &elem2) const
	{
		return elem1.priority>elem2.priority;
	}
};
struct PieceIDCmp{
	bool operator() (const piece_info &elem1, const piece_info &elem2) const
	{
		return elem1.pieceID<elem2.pieceID;
	}
};

void VodApp::requestScheduling_Rnd()
{
	//if (PriorityVec.empty())
	//{
	//	zipf();
	//}
	/************************************************************************/
	/*         1.清理掉上个循环的请求并更新能力测量                         */
	/************************************************************************/
	m_supperNeightbor.remainAbility=m_supperNeightbor.ability;
	if (m_supperNeightbor.pieceCntRecvdCurrCycle>0
		&&m_supperNeightbor.pieceCntRecvdCurrCycle>=m_supperNeightbor.ability
		)
	{
		//m_supperNeightbor.ability+=0.4;
	}
	else if (m_supperNeightbor.requestCycle!=0
		&&(m_supperNeightbor.normalRequestsCurrCycle.size()+m_supperNeightbor.urgentRequestsCurrCycle.size())>m_supperNeightbor.pieceCntRecvdCurrCycle)
	{
		//m_supperNeightbor.ability-=0.4;
		//if (m_supperNeightbor.ability==0)
		//	m_supperNeightbor.ability=1;
		//m_supperNeightbor.remainAbility=0;
	}
	m_supperNeightbor.pieceCntRecvdCurrCycle=0;
	m_supperNeightbor.normalRequestsCurrCycle.clear();
	m_supperNeightbor.urgentRequestsCurrCycle.clear();
	m_supperNeightbor.currSessionID=-1;


	intreasting_nb_map_t::iterator itr(m_intreastingNB.begin());
	for(;itr != m_intreastingNB.end();++itr)
	{
		NeightborInfo& info=itr->second;
		info.remainAbility=info.ability;
		if (info.pieceCntRecvdCurrCycle>0
			&&info.pieceCntRecvdCurrCycle>=info.ability
			)
		{
			info.ability+=0.5;
		}
		else if ((info.normalRequestsCurrCycle.size()+info.urgentRequestsCurrCycle.size())>info.pieceCntRecvdCurrCycle)
		{
			info.ability=info.pieceCntRecvdCurrCycle+1;
			info.ability=max((double)info.pieceCntRecvdCurrCycle,info.ability);
			if (info.ability<1)
				info.ability=1;
			info.remainAbility=0;
		}

		//////////////////////////////////////////////////////////////////////////
		if (addr()==2)
		{
			cout<<itr->first<<"---------------"
				<<info.contribute.getSpeed()*8/1024<<"     "
				<<info.ability<<"     "
				<<info.normalRequestsCurrCycle.size()<<"     "
				<<info.pieceCntRecvdCurrCycle<<"     "
				<<info.allRequestPieces<<"     "
				<<info.allResponsePieces<<endl;
		}
		//////////////////////////////////////////////////////////////////////////
		info.pieceCntRecvdCurrCycle=0;
		//cout<<itr->first<<" to "<<addr()<<" 'ability-----------------:"<<info.ability<<endl;
		info.normalRequestsCurrCycle.clear();
		info.urgentRequestsCurrCycle.clear();
		info.currSessionID=-1;
	}

	/************************************************************************/
	/*         2.数据调度策略，节点和片段选择                               */
	/************************************************************************/
	vector<piece_info> candidatePieces;
	piece_id_t startPiece=m_player.playingSeqNum()+1;
	int requestCnt=0;

	int minCnt=0;
	//首先，查看有无紧急片段需要紧急调度
	time_s_t continueT=m_player.continuousTime()+0.001;
	//if (false
	//	&&(m_player.state()!=Player::NOT_START)
	//	&&continueT<URGENT_1_REQUEST_TIME
	//	)
	//{
	//	double bigPriority=INT_MAX/4;//设置一个足够大的优先级
	//	time_s_t range=URGENT_1_REQUEST_TIME;
	//	piece_id_t endPice=startPiece+(piece_id_t)(range/PLAY_TIME_PER_PIECE);
	//	for (piece_id_t i=startPiece+continueT/PLAY_TIME_PER_PIECE-1;i<=endPice;++i)
	//	{
	//		if (i<startPiece+PIECE_REQUEST_CYCLE_TIME/PLAY_TIME_PER_PIECE)
	//		{
	//			minCnt++;
	//		}
	//		double ability=0;
	//		if (!m_playingBuffer.exist(i))
	//		{
	//			time_s_t t=m_requestPendingQue.timeOut(i);
	//			double priorityBecauseWait=0;
	//			//if (t>=0&&i>startPiece+1.0*PIECE_REQUEST_CYCLE_TIME/PLAY_TIME_PER_PIECE)
	//				//if (t>0)
	//			{
	//				if (t>NOW())
	//					continue;
	//				else
	//				{
	//					//m_requestPendingQue.erase(i);
	//					priorityBecauseWait=INT_MAX/4;
	//					continue;
	//				}
	//			}
	//			piece_info pieceInfo;
	//			pieceInfo.pieceID=i;
	//			intreasting_nb_map_t::iterator neightborItr(m_intreastingNB.begin());

	//			if (i>startPiece+1.5*PIECE_REQUEST_CYCLE_TIME/PLAY_TIME_PER_PIECE)
	//			{
	//				for (;neightborItr!=m_intreastingNB.end();++neightborItr)
	//				{
	//					addr_t a=neightborItr->first;
	//					VodApp* p=(VodApp*)(Node::get(a)->app());
	//					if (p->m_playingBuffer.exist(i)&&neightborItr->second.ability>8)
	//					{
	//						pieceInfo.holders.push_back(&(neightborItr->second));
	//					}
	//				}
	//			}

	//			if (pieceInfo.holders.empty())
	//			{
	//				if (i<startPiece+0.5*PIECE_REQUEST_CYCLE_TIME/PLAY_TIME_PER_PIECE)
	//					pieceInfo.isUrgent=true;
	//				pieceInfo.holders.push_back(&m_supperNeightbor);
	//				if(pieceInfo.isUrgent)
	//					pieceInfo.priority=bigPriority+Rand(0,0.5);
	//				else
	//					pieceInfo.priority=bigPriority/2+1.0/(NEIGHTBOR_COUNT+priorityBecauseWait)+Rand(0,0.5);
	//			}
	//			else
	//			{
	//				pieceInfo.isUrgent=true;
	//				if(pieceInfo.isUrgent)
	//					pieceInfo.priority=bigPriority+Rand(0,0.5);
	//				else
	//					pieceInfo.priority=bigPriority/2+1.0/(pieceInfo.holders.size()+priorityBecauseWait)+Rand(0,0.5);
	//			}
	//			candidatePieces.push_back(pieceInfo);
	//			--bigPriority;//越晚播放的，优先级逐渐降低
	//		}
	//	}
	//	startPiece=endPice+1;
	//}

	//其次，进行通常的RF调度
	startPiece+=(continueT>PLAY_TIME_PER_PIECE)?((continueT/PLAY_TIME_PER_PIECE)-1):0;
	piece_id_t num=START_PLAY_CACH_TIME/PLAY_TIME_PER_PIECE;//*PIECE_COUNT_PER_CYCLE;
	time_s_t t=2*(continueT)+URGENT_1_REQUEST_TIME;
	piece_id_t endpiece=startPiece+(t)/PLAY_TIME_PER_PIECE;
	if (m_player.state()==Player::NOT_START)
	{
		endpiece=(2*START_PLAY_CACH_TIME)/PLAY_TIME_PER_PIECE;
	}

	bool expanded=false;
	for (piece_id_t i=startPiece;candidatePieces.size()<num&&i<endpiece;++i)
	{
		m_maxknownPiece=i;
		double ability=0;
		double priorityBecauseWait=0;
		if (!m_playingBuffer.exist(i))
		{
			time_s_t t=m_requestPendingQue.timeOut(i);
			if (t>=NOW())
				continue;
			piece_info pieceInfo;
			pieceInfo.pieceID=i;
			intreasting_nb_map_t::iterator neightborItr(m_intreastingNB.begin());
			for (;neightborItr!=m_intreastingNB.end();++neightborItr)
			{
				addr_t a=neightborItr->first;
				VodApp* p=(VodApp*)(Node::get(a)->app());
				//不要用p->m_playingBuffer.exist(i)
				//用StrmBuffer::exist(a,i)表示改节点是从buffermap交换中获得的信息，不一定准确
				//
				if (StrmBuffer::exist(a,i))
				//if (p->m_playingBuffer.exist(i))
				{
					pieceInfo.holders.push_back(&(neightborItr->second));
					ability+=neightborItr->second.ability;
				}
			}
			if (pieceInfo.holders.size()>0)
			{
				//pieceInfo.priority=1.0/(pieceInfo.holders.size()+priorityBecauseWait)+Rand(0,0.05);
				pieceInfo.priority=Rand(0,1);
				candidatePieces.push_back(pieceInfo);
				if (m_player.state()==Player::NOT_START)
				{
					pieceInfo.holders.push_back(&m_supperNeightbor);
					//	pieceInfo.priority=1.0/(NEIGHTBOR_COUNT+priorityBecauseWait)+Rand(0,0.05);
				}
			}
			//else if (m_player.state()==Player::NOT_START
			//	&&(Rand01()<0.5||m_maxUpRate>MEDIA_BPS))
			//{
			//	pieceInfo.holders.push_back(&m_supperNeightbor);
			//	if (pieceInfo.holders.size()>0)
			//	{
			//		//pieceInfo.priority=1.0/2+Rand(0,0.05);
			//		pieceInfo.priority=Rand(0,1);
			//		candidatePieces.push_back(pieceInfo);
			//	}
			//}
			else 
			{
				//if (m_supperBeNeightbor)//如果服务器是邻居节点，将其加入
				pieceInfo.holders.push_back(&m_supperNeightbor);
				//else if(m_maxDownloadRate)//否则以一定的概率将服务器加入
				//	pieceInfo.holders.push_back(&m_supperNeightbor);
				if (pieceInfo.holders.size()>0)
				{
					//pieceInfo.priority=1.0/(NEIGHTBOR_COUNT+priorityBecauseWait)+Rand(0,0.05);
					//pieceInfo.priority=0.5*Rand(0,1);
					pieceInfo.priority=Rand(0,1);
					candidatePieces.push_back(pieceInfo);
				}
			}

		}
		if (i==endpiece-1&&candidatePieces.size()<num)
		{
			if (expanded==false)
			{
				vector<piece_info>::iterator itr(candidatePieces.begin());
				for(;itr!=candidatePieces.end();++itr)
				{
					itr->priority+=1.0;
				}
			}
			endpiece+=(num-candidatePieces.size());
			expanded=true;
		}
	}

	//最后，按照优先级排序，并计算向谁调度以及调度量
	int maxRequestCntPerCycle=getMaxDownPieceCntNextCycle();

	sort(candidatePieces.begin(),candidatePieces.end(),PriorityCmp());
	for (size_t i=0;i<candidatePieces.size();++i)
	{
		vector<NeightborInfo*>&holders=candidatePieces[i].holders;
		int oldRequestCnt=requestCnt;
		for (size_t j=0;j<4*holders.size();j++)
		{
			size_t n=RandUint32()%holders.size();
			NeightborInfo* holderInfo=holders[n];
			double needAbility=holderInfo->normalRequestsCurrCycle.size()+holderInfo->urgentRequestsCurrCycle.size()
				+(holderInfo->allRequestPieces-holderInfo->allResponsePieces)/(holderInfo->requestCycle+0.0001);
			if ((holderInfo->ability+0.5)>(needAbility))
			{
				if (candidatePieces[i].isUrgent)
					holderInfo->urgentRequestsCurrCycle.push_back(candidatePieces[i].pieceID);
				else
					holderInfo->normalRequestsCurrCycle.push_back(candidatePieces[i].pieceID);
				++requestCnt;
				break;
			}
		}
		//if (oldRequestCnt==requestCnt)//说明没有可用节点了,试试服务器
		//{
		//	NeightborInfo* holderInfo=&m_supperNeightbor;
		//	double needAbility=holderInfo->normalRequestsCurrCycle.size()+holderInfo->urgentRequestsCurrCycle.size()
		//		+(holderInfo->allRequestPieces-holderInfo->allResponsePieces)/(holderInfo->requestCycle+0.0001);
		//	if ((holderInfo->ability+0.5)>(needAbility))
		//	{
		//		if (candidatePieces[i].isUrgent)
		//			holderInfo->urgentRequestsCurrCycle.push_back(candidatePieces[i].pieceID);
		//		else
		//			holderInfo->normalRequestsCurrCycle.push_back(candidatePieces[i].pieceID);
		//		++requestCnt;
		//		break;
		//	}
		//}
		if(requestCnt>=maxRequestCntPerCycle)
			break;
	}

	/************************************************************************/
	/*         3.发送请求                                                   */
	/************************************************************************/
	//注意，经过上面的处理，已经选择好了优先级高的片段；同时，各个邻居节点的
	//请求队列中的片段也是按照优先级排队了的。

	//首先查看需不需要向server请求
	if (m_supperNeightbor.normalRequestsCurrCycle.size()>0
		||m_supperNeightbor.urgentRequestsCurrCycle.size()>0)
	{

		PieceRequestMsg* data=new PieceRequestMsg;
		data->normalPieces=m_supperNeightbor.normalRequestsCurrCycle;
		data->urgentPieces=m_supperNeightbor.urgentRequestsCurrCycle;

		//sort(data->urgentPieces.begin(),data->urgentPieces.end());
		//sort(data->normalPieces.begin(),data->normalPieces.end());

		data->channelID=m_playingChannelID;
		sendTo(CONTROL_MSG_LEN,SERVER_ADDRESS,Packet::newPacket(MSG_PIECE_REQUEST,data));

		m_supperNeightbor.requestCycle++;
		m_supperNeightbor.currSessionID=data->sessionID;
		m_supperNeightbor.allRequestPieces+=m_supperNeightbor.normalRequestsCurrCycle.size()
			+m_supperNeightbor.urgentRequestsCurrCycle.size();
		//估计超时时间
		double dlt=(1.8*PIECE_REQUEST_CYCLE_TIME)/(data->urgentPieces.size()+data->normalPieces.size());
		double delay=dlt;
		dlt=1.3*PIECE_REQUEST_CYCLE_TIME;
		for(size_t i=0;i<data->urgentPieces.size();++i)
		{
			time_s_t timeOut=NOW()+dlt;
			m_requestPendingQue.insert(data->urgentPieces[i],timeOut);
			delay+=dlt;
			//if (addr()==3&&data->urgentPieces[i]==631)
			//{
			//	cout<<NOW();
			//	system("pause");
			//}
		}
		for(size_t i=0;i<data->normalPieces.size();++i)
		{
			time_s_t timeOut=NOW()+dlt;
			m_requestPendingQue.insert(data->normalPieces[i],timeOut);
			delay+=dlt;
			/*	if (addr()==3&&data->normalPieces[i]==631)
			{
			cout<<NOW();
			system("pause");
			}*/

		}
	}
	//然后向一般邻居节点发送请求
	for(itr=m_intreastingNB.begin();itr!=m_intreastingNB.end();++itr)
	{
		NeightborInfo& info=itr->second;
		if (info.normalRequestsCurrCycle.size()>0
			||info.urgentRequestsCurrCycle.size()>0)
		{
			PieceRequestMsg* data=new PieceRequestMsg;
			data->normalPieces=info.normalRequestsCurrCycle;
			data->urgentPieces=info.urgentRequestsCurrCycle;
			data->channelID=m_playingChannelID;

		//	sort(data->urgentPieces.begin(),data->urgentPieces.end());
		//	sort(data->normalPieces.begin(),data->normalPieces.end());
			sendTo(CONTROL_MSG_LEN,itr->first,Packet::newPacket(MSG_PIECE_REQUEST,data));

			info.currSessionID=data->sessionID;
			info.allRequestPieces+=info.normalRequestsCurrCycle.size()
				+info.urgentRequestsCurrCycle.size();
			info.requestCycle++;
			double dlt=(1.3*PIECE_REQUEST_CYCLE_TIME)/(data->urgentPieces.size()+data->normalPieces.size());
			double delay=dlt;
			dlt=1.3*PIECE_REQUEST_CYCLE_TIME;
			for(size_t i=0;i<data->urgentPieces.size();++i)
			{
				time_s_t timeOut=NOW()+dlt;
				m_requestPendingQue.insert(data->urgentPieces[i],timeOut);
				delay+=dlt;
				//if (addr()==3&&data->urgentPieces[i]==631)
				//{
				//	cout<<NOW();
				//	system("pause");
				//}
			}
			for(size_t i=0;i<data->normalPieces.size();++i)
			{
				
				time_s_t timeOut=NOW()+dlt;
				m_requestPendingQue.insert(data->normalPieces[i],timeOut);
				delay+=dlt;
				/*		if (addr()==3&&data->normalPieces[i]==631)
				{
				cout<<NOW();
				system("pause");
				}*/
				
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	if (addr()==2)
	{
		cout<<m_downlinkAllRate.getSpeed()*8/1024
			<<"maxRequest:"<<maxRequestCntPerCycle
			<<"  realRequest:"<<requestCnt
			<<"  candidateSize:"<<candidatePieces.size()
			<<"  continueT:"<<continueT<<endl;
		if (NOW()>1000)
		{
			system("pause");
		}
		//system("pause");
	}
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
}