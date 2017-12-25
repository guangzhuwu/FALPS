#include "Vod.h"
#include "VodMsg.h"
#include <sstream>
using namespace std;

REGIST_OUT_CLASS(VodApp);
bucket_map_t VodApp::s_dynamicBucketMap;
bucket_map_t VodApp::s_staticBucketMap;
vector<double> VodApp::s_channelPopulation;
size_t VodApp::s_onlineNodes;
StatisticTimer VodApp::s_statistic;

ContributeMeter VodApp::s_uplinkPushRate(10);
ContributeMeter VodApp::s_uplinkPulledRate(10);
ContributeMeter VodApp::s_uplinkAllRate(10);//近段时间上行带宽
ContributeMeter VodApp::s_downlinkAllRate(10);//近段时间下行带宽
ContributeMeter VodApp::s_neightborsContribute(10);//合作节点对我的贡献
ContributeMeter VodApp::s_uselessPushedRate(10);
ContributeMeter VodApp::s_usefulPushedRate(10);


size_B_f_t XXX=0;
size_B_f_t YYY=0;
time_s_t TTT=0;
double SSS=0;

ContributeMeter MMM(10);

vector<int> ZZZ(8192);

void VodApp::init()
{
	//设置索引服务器IS的索引数据库
	if (s_dynamicBucketMap.size()<CHANNEL_COUNT)
	{
		s_dynamicBucketMap.resize(CHANNEL_COUNT);
		size_t bucketCount=2*(size_t)((MEDIA_TIME+BUCKET_TIME-1)/BUCKET_TIME)+1;
		for (size_t i=0;i<s_dynamicBucketMap.size();++i)
			s_dynamicBucketMap[i].resize(bucketCount);
	}
	//根据Zipf分布计算出每个影片的受欢迎程度
	if (s_channelPopulation.size()<CHANNEL_COUNT)
	{
		ostringstream info;
		info<<string("影片受欢迎程度:");

		s_channelPopulation.resize(CHANNEL_COUNT);
		double skewfactor=0.271;
		double sum=0;
		for (size_t i=0;i<CHANNEL_COUNT;++i)
		{
			s_channelPopulation[i]=1.0/pow(double(i+1),1-0.271);
			sum+=s_channelPopulation[i];
		}
		for (size_t i=0;i<CHANNEL_COUNT;++i)
		{
			s_channelPopulation[i]/=sum;
			info<<s_channelPopulation[i]<<" ";
		}
		//LogInfo(info.str().c_str());
	}
	Node::registerSendedCallback(MSG_PIECE);
	s_statistic.resched(0);
}

VodApp::VodApp()
{
	OutputAssert(s_dynamicBucketMap.size()>=CHANNEL_COUNT);

	m_bServer=false;
	m_joinTime=-1;
	m_playingChannelID=-1;
	m_dynamicBucket=-1;
	m_playingBuffer.bindApp(*this);
	m_player.bindBuffer(m_playingBuffer);
	m_player.bindApp(*this);
	m_player.setUrgentTime(1.8*PIECE_REQUEST_CYCLE_TIME);
	for (size_t i=0;i<sizeof(m_responsePendingQue)/sizeof(m_responsePendingQue[0]);++i)
		m_responsePendingQue[i].bindApp(*this);
	m_urgentResponsePendingQue.bindApp(*this);
	m_maxDownloadRate=1.5*MEDIA_BPS;
	m_maxUpRate=0;
	m_sendingPiecePkt=false;
	m_downloadNumAfterPlay=0;
	m_allDownPieceNum=0;//所有下载的片段数


	m_joinTimer.bindApp(*this);
	m_leaveTimer.bindApp(*this);
	m_updataNeightborsTimer.bindApp(*this);
	m_updataFathersTimer.bindApp(*this);
	m_updataChildrenTimer.bindApp(*this);
	m_sendBufferMapTimer.bindApp(*this);
	m_requestPieceTimer.bindApp(*this);

	m_maxknownPiece=0;


	//根据频道的受欢迎程度设置改节点的点播频道号
	double randNum=Rand01();
	double d=0;
	for (int i=CHANNEL_COUNT-1;i>=0;--i)
	{	
		d+=s_channelPopulation[i];
		if (randNum<=d)
		{
			m_playingChannelID=i;
			break;
		}
	}

	//设置一个随机加入时间
	static time_s_t  T=0;
	time_s_t t=Rand(0.8,1.2)*(1.0/JOIN_NODES_PER_SECOND);
	m_joinTime=T;
	T+=t;
}

bucket_id_t VodApp::playingPoint2DynamicBucketID(time_s_t playingPoint)
{
	//详见“基于动态时间坐标系的拓扑组织方法”
	int now=(int)NOW();
	int x=(int)playingPoint-now;
	x%=(int)(2*MEDIA_TIME);
	return static_cast<bucket_id_t>(x/BUCKET_TIME+2*MEDIA_TIME/BUCKET_TIME);
}

void VodApp::start()
{
	m_stoped=false;
	m_maxDownloadRate=0.85*getDownlinkBandwidth();

	//启动加入网络的自消息方式定时
	if (addr()!=SERVER_ADDRESS)
	{
		m_joinTimer.resched(m_joinTime);
		//if(Rand01()<LEAVE_RATE)
		//	m_leaveTimer.resched(Rand(m_joinTime,MEDIA_TIME));
	}
	else
	{
		//设置为服务器
		m_bServer=true;
		//服务器拥有所有片段
		m_playingBuffer.setAll();
	}
}

void VodApp::stop()
{
}


void VodApp::onRecvd(Packet* pkt)
{
	if(m_stoped)
	{
		delete pkt;
		return;
	}
	int pktType=pkt->type();
	switch (pktType)
	{
	case MSG_NEIGHTBOR_GOSSIP:
		onNeightborRequestMsg(pkt);
		break;
		/*case MSG_NEIGHTBOR_ACCEPT:
		onNeightborResponseAcceptMsg(pkt);
		break;
		case MSG_NEIGHTBOR_REJECT:
		onNeightborResponseRejectMsg(pkt);
		break;*/
	case MSG_PIECE_REQUEST:
		onPieceRequestMsg(pkt);
		break;
	case MSG_PIECE:
		onPieceMsg(pkt);
		break;
	case MSG_NOT_BE_MY_FATHER:
		onChildToFatherDisconnectMsg(pkt);
		break;
	case MSG_NOT_BE_MY_CHILD:
		onFatherToChildDisconnectMsg(pkt);
		break;
	case MSG_BE_MY_CHILD:
		onFatherToChildConnectMsg(pkt);
		break;
	case MSG_BE_MY_FATHER:
		onChildToFatherConnectMsg(pkt);
		break;
	case MSG_RANGE:
		onRangeMsg(pkt);
		break;
	case MSG_BUFFERMAP:
		break;

	default:
		OutputAssert(0);
	}
	delete pkt;
}

void VodApp::leaveFromCurrentDynamicBucket()
{
	//从原来所在桶退出
	if (m_dynamicBucket>=0)
	{
		list<addr_t>& lst=
			s_dynamicBucketMap[m_playingChannelID][m_dynamicBucket];
		list<addr_t>::iterator itr(lst.begin());
		for (;itr!=lst.end();++itr)
		{
			if (*itr==addr())
			{
				lst.erase(itr);
				break;
			}
		}
		m_dynamicBucket=-1;
	}
}

bucket_id_t VodApp::playingPoint2StaticBucketID(time_s_t playingPoint)
{
	return static_cast<bucket_id_t>(playingPoint/BUCKET_TIME);
}


void VodApp:: sendNeightborRequest(addr_t dst)
{
	sendTo(CONTROL_MSG_LEN,dst,Packet::newPacket(MSG_NEIGHTBOR_GOSSIP));
}


void VodApp::onPieceMsg(Packet* pkt)
{
	PieceMsg* data=(PieceMsg*)(pkt->data());
	m_downlinkAllRate+=PIECE_LEN;
	s_downlinkAllRate+=PIECE_LEN;
	m_maxDownloadRate=max(m_maxDownloadRate,m_downlinkAllRate.getSpeed());

	//从m_requestPendingQue中删除
	m_requestPendingQue.erase(data->pieceID);

	bool useful=m_playingBuffer.insert(data->pieceID);
	if (!useful&&data->sessionID!=INVALID_ID)
	{
		m_uselessPulledRate+=PIECE_LEN;
		cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
			<<pkt->srcAddr()<<"----->"<<addr()<<" playing:"<<m_player.playingSeqNum()
			<<" badpiece:"<<data->pieceID
			<<endl;
		//system("pause");
	}
	if (useful&&m_player.state()!=Player::NOT_START)
	{
		m_downloadNumAfterPlay++;
	}
	if(useful&&data->sessionID!=INVALID_ID)
		m_allDownPieceNum++;

	//if (addr()==15)
	//{
	//	static map<int ,pair<int ,double> > ma;

	//	if (!useful)
	//	{
	//		cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
	//		cout<<pkt->srcAddr()<<"  "<<NOW()
	//			<<" ....... "<<ma[data->pieceID].first<<"  "<<ma[data->pieceID].second<<endl;
	//		cout<<"NOW_PLAY: "<<m_player.playingSeqNum()<<endl;
	//		cout<<"BADPIECE: "<<data->pieceID<<endl;
	//		cout<<addr();
	//		system("pause");
	//	}
	//	else
	//		ma[data->pieceID]=make_pair(pkt->srcAddr(),NOW());

	//}

	if (data->sessionID==INVALID_ID)
	{
		if (useful)
		{
			m_usefulPushedRate+=PIECE_LEN;
			s_usefulPushedRate+=PIECE_LEN;
			father_map_t::iterator itr(m_fathers.find(pkt->srcAddr()));
			if (itr!=m_fathers.end())
				itr->second.contribute+=PIECE_LEN;
		}
		else
		{
			m_uselessPushedRate+=PIECE_LEN;
			s_uselessPushedRate+=PIECE_LEN;
			cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
		}
	}
	else
	{
		if (pkt->srcAddr()==SERVER_ADDRESS)
		{
			if(m_supperNeightbor.currSessionID==data->sessionID)
				m_supperNeightbor.pieceCntRecvdCurrCycle++;
			m_supperNeightbor.allResponsePieces++;
		}
		else
		{
			m_neightborsContribute+=PIECE_LEN;
			s_neightborsContribute+=PIECE_LEN;
			intreasting_nb_map_t::iterator itr(m_intreastingNB.find(pkt->srcAddr()));
			if (itr!=m_intreastingNB.end())
			{
				NeightborInfo& info=itr->second;
				if (useful)
				{
					info.contribute+=PIECE_LEN;
				}
				if (info.currSessionID==data->sessionID)
				{
					info.pieceCntRecvdCurrCycle++;
				}
				info.allResponsePieces++;
			}
		}
	}
	//播放器操作
	if (m_player.state()==Player::NOT_START)
	{
		if (m_player.continuousTime()>START_PLAY_CACH_TIME)
		{
			m_player.startPlay();
			cout<<"------------------------------------"<<NOW()-m_joinTime<<endl;
		}
	}
	else if (m_player.state()==Player::BLOCK_ABSENT)
	{
		if (m_player.continuousTime()>0)
			m_player.continuePlay();
	}


	//////////////////////////////////////////////////////////////////////////
	if (pkt->srcAddr()==SERVER_ADDRESS)
	{
		MMM+=PIECE_LEN;
	//	cout<<MMM.getSpeed()*8/1024<<endl;
	}
	//////////////////////////////////////////////////////////////////////////
}

void VodApp::onPieceRequestMsg(Packet* pkt)
{
	responseScheduling_FIFO(pkt);
}

void VodApp::onSended(pkt_type_t t)
{
	if (t==MSG_PIECE)
	{
		m_sendingPiecePkt=false;
		onPieceRequestMsg(NULL);
	}
}

void VodApp::scheduleRequest()
{
	requestScheduling_RF();
}

int VodApp::getMaxDownPieceCntNextCycle()
{
	double b=min(4*MEDIA_BPS,m_maxDownloadRate)-MEDIA_BPS;
	double a=(2*START_PLAY_CACH_TIME)/PLAY_TIME_PER_PIECE;
	double x=m_allDownPieceNum-m_player.playingSeqNum();
	double y=max((-b*x/a+b)+MEDIA_BPS,MEDIA_BPS)/PIECE_LEN*PIECE_REQUEST_CYCLE_TIME;
	if (m_maxUpRate>MEDIA_BPS)
		y*=1.05;
	if (m_player.continuousTime()<0.35*START_PLAY_CACH_TIME)
		y*=1.05;
	return ceil(y);

	rate_Bps_t B_maxrequest=max(2.0*MEDIA_BPS,m_maxDownloadRate);
	rate_Bps_t B_prefetch_init=1.5*START_PLAY_CACH_TIME*MEDIA_BPS;
	rate_Bps_t B_prefetch_old=(m_allDownPieceNum-m_player.playingSeqNum())*PIECE_LEN;

	rate_Bps_t B_request=max(-B_prefetch_old*(B_maxrequest-MEDIA_BPS)/B_prefetch_init+B_maxrequest,MEDIA_BPS);

	if (m_maxUpRate>MEDIA_BPS)
		B_request*=1.1;
	if (m_player.state()!=Player::NOT_START&&m_player.continuousTime()<3.5)
		B_request*=1.1;

	return (int)ceil(B_request*PIECE_REQUEST_CYCLE_TIME/PIECE_LEN);
}

void VodApp::onPlayerUrgent(piece_id_t piece)
{

	PieceRequestMsg* data=new PieceRequestMsg;
	data->urgentPieces.push_back(piece);
	data->channelID=m_playingChannelID;
	m_supperNeightbor.currSessionID=data->sessionID;
	sendTo(CONTROL_MSG_LEN,SERVER_ADDRESS,Packet::newPacket(MSG_PIECE_REQUEST,data));
	//估计超时时间
	time_s_t delay=1.3*PIECE_REQUEST_CYCLE_TIME;
	m_requestPendingQue.insert(piece,NOW()+delay);
	m_supperNeightbor.allRequestPieces+=1;
}

void VodApp::outPutStatistic()
{
	rate_Bps_t uplinkPushRate=0;
	rate_Bps_t uplinkPulledRate=0;
	rate_Bps_t uplinkAllRate=0;
	rate_Bps_t downlinkAllRate=0;
	rate_Bps_t neightborsContribute=0;
	rate_Bps_t uselessPushedRate=0;
	rate_Bps_t usefulPushedRate=0;
	double playQuality=0;
	double playingCnt=0;

	for (size_t i=SERVER_ADDRESS+1;i<s_onlineNodes;++i)
	{
		VodApp*p=(VodApp*)(Node::get(i)->app());
		if(p->m_stoped)
			continue;
		double playQualityTmp=p->m_player.getQuality();
		if (playQualityTmp>0)
		{
			playQuality+=playQualityTmp;
			playingCnt++;
		}
	}


	rate_Bps_t uplinkPushRateTmp=s_uplinkPushRate.getSpeed();
	rate_Bps_t uplinkPulledRateTmp=s_uplinkPulledRate.getSpeed();
	rate_Bps_t uplinkAllRateTmp=s_uplinkAllRate.getSpeed();
	rate_Bps_t downlinkAllRateTmp=s_downlinkAllRate.getSpeed();
	rate_Bps_t neightborsContributeTmp=s_neightborsContribute.getSpeed();
	rate_Bps_t uselessPushedRateTmp=s_uselessPushedRate.getSpeed();
	rate_Bps_t usefulPushedRateTmp=s_usefulPushedRate.getSpeed();

	uplinkPushRate+=max(0.0,uplinkPushRateTmp);
	uplinkPulledRate+=max(0.0,uplinkPulledRateTmp);
	uplinkAllRate+=max(0.0,uplinkAllRateTmp);
	downlinkAllRate+=max(0.0,downlinkAllRateTmp);
	neightborsContribute+=max(0.0,neightborsContributeTmp);
	uselessPushedRate+=max(0.0,uselessPushedRateTmp);
	usefulPushedRate+=max(0.0,usefulPushedRateTmp);

	if(s_onlineNodes==0)
	{
		uplinkPushRate=0;
		uplinkPulledRate=0;
		uplinkAllRate=0;
		downlinkAllRate=0;
		neightborsContribute=0;
		uselessPushedRate=0;
		usefulPushedRate=0;
		if (playingCnt>0)
			playQuality=0;
		else
			playQuality=0;
	}
	else
	{
		uplinkPushRate/=s_onlineNodes;
		uplinkPulledRate/=s_onlineNodes;
		uplinkAllRate/=s_onlineNodes;
		downlinkAllRate/=s_onlineNodes;
		neightborsContribute/=s_onlineNodes;
		uselessPushedRate/=s_onlineNodes;
		usefulPushedRate/=s_onlineNodes;
		playQuality=1;
	}

	static bool first=true;

	ostringstream info;
	if (first)
	{
		info<<string("*serverUpRate: ")
			<<string(" *uplinkPushRate: ") 
			<<string(" *uplinkPulledRate: ") 
			<<string(" *uplinkAllRate: ") 
			<<string(" *downlinkAllRate: ") 
			<<string(" *neightborsContribute: ") 
			<<string(" *uselessPushedRate: ") 
			<<string(" *usefulPushedRate: ") 
			<<string(" *playQuality: ")
			<<endl;
		first=false;
	}
	info<<MMM.getSpeed()*8/1024
		<<string(" ")<<uplinkPushRate*8/1024
		<<string(" ")<<uplinkPulledRate*8/1024
		<<string(" ")<<uplinkAllRate*8/1024
		<<string(" ")<<downlinkAllRate*8/1024
		<<string(" ")<<neightborsContribute*8/1024
		<<string(" ")<<uselessPushedRate*8/1024
		<<string(" ")<<usefulPushedRate*8/1024
		<<string(" ")<<playQuality;
	LogInfo(info.str().c_str());
}
