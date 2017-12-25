#ifndef _P2PSTREAM_VOD_H_
#define _P2PSTREAM_VOD_H_

#include "falps.h"
#include "Player.h"
#include "StrmBuffer.h"
#include "SpeedMeter.h"
#include "ParamDef.h"
#include "VodMsg.h"
#include "PendingQue.h"
#include "VoDTimer.h"


//邻居节点集
//节点和其邻居节点间进行“随机”“稀有先取”等策略下的基于拉的数据调度（Pull）
struct NeightborInfo
{
	vector<piece_id_t>normalRequestsCurrCycle;//本轮请求向其请求的片段
	vector<piece_id_t>urgentRequestsCurrCycle;//本轮请求向其请求的片段

	time_s_t beNbTime;//
	int pieceCntRecvdCurrCycle;//本轮请求收到的片段
	double ability;//带宽能力，每轮可以成功发送的数据片的多少
	size_t allRequestPieces;//所有请求数
	size_t allResponsePieces;//所有回应数
	id_t currSessionID;//当前请求的session号
	size_t requestCycle;//请求过的轮数
	size_t remainAbility;//剩余能力
	ContributeMeter contribute;//邻居的贡献能力 
	ContributeMeter consumption;//邻居的消费能力

	NeightborInfo():contribute(20)
	{
		beNbTime=NOW();
		requestCycle=0;
		pieceCntRecvdCurrCycle=0;
		allRequestPieces=0;
		allResponsePieces=0;
		currSessionID=-1;
		ability=(int)(0.33*(MEDIA_BPS/PIECE_LEN)*PIECE_REQUEST_CYCLE_TIME);
	}
};

//父节点集
//一节点的父节点集的节点向该节点进行数据推送（Push）
struct FatherInfo 
{
	ContributeMeter contribute;//父节点贡献
	FatherInfo():contribute(10){}
};

//子节点集
//节点向其子节点内的节点进行数据推送(Push)
struct ChildInfo
{
	size_t ability;//子点带宽能力
	pair<piece_id_t,piece_id_t> range;//一个周期内要推送的数据所在的区间
	size_t pusedCntCurrCycle;
	ContributeMeter contribute;//推送给子节点的数据带宽
	ChildInfo():contribute(10){range.first=-1;range.second=-1;pusedCntCurrCycle=0;}
};



typedef vector< vector<list<addr_t> > > bucket_map_t;
typedef map<addr_t,NeightborInfo>       intreasted_nb_map_t;//对local感兴趣的节点
typedef map<addr_t,NeightborInfo>       choking_nb_map_t;//被local阻塞的节点
typedef map<addr_t,NeightborInfo>       choked_nb_map_t;//阻塞local的节点
typedef map<addr_t,NeightborInfo>       intreasting_nb_map_t;//local感兴趣的节点
typedef map<addr_t,NeightborInfo>       unintreasing_nb_map_t;//local暂不感兴趣的节点
typedef map<addr_t,FatherInfo>          father_map_t;
typedef map<addr_t,ChildInfo>           child_map_t;



class PendingRequestQue
{	
public:
	size_t size()
	{
		return que_.size();
	}
	void insert(piece_id_t pieceID,time_s_t timeOut)
	{
		que_[pieceID]=timeOut;
	}
	void erase(piece_id_t pieceID)
	{
		que_.erase(pieceID);
	}
	time_s_t timeOut(piece_id_t pieceID)
	{
		map<piece_id_t,time_s_t>::iterator itr(que_.find(pieceID));
		if (itr!=que_.end())
			return itr->second;
		else
			return -1;
	}
private:
	map<piece_id_t,time_s_t> que_;
};

class VodApp:public Application
{
public:
	VodApp();
	virtual void onRecvd(Packet* pkt);
	virtual void onSended(pkt_type_t t);
	virtual void start();
	virtual void stop();
public:
	static void init();
public:
	//动态时间坐标系
	bucket_id_t playingPoint2DynamicBucketID(time_s_t playingPoint);
	//静态时间坐标系
	bucket_id_t playingPoint2StaticBucketID(time_s_t playingPoint);
	//退出当前所在的桶
	void        leaveFromCurrentDynamicBucket();


public:
	//////////////////////////////////////////////////////////////////////////
	//Overlay组织部分
	void        neightborGossip();

	//更新邻居节点
	void        updataNeightbors();
	//删除效率低的合作节点
	addr_t      eraseNeightbors();
	//查找父节点
	void        updataFathers();
	//删除效率低的父节点
	addr_t      eraseFathers(bool must=false);
	//查找子节点
	void        findNewChildren();
	//删除效率低的子节点
	addr_t      eraseChildren();

public:
	//向dst请求频道channelID播放点在myPlayingPoint附近的邻居节点
	//void sendNeightborRequest(addr_t dst,channel_id channelID,time_s_t myPlayingPoint);
	//请求向dst请求频道channelID的数据
	id_t sendPieceRequest(addr_t dst,channel_id_t channelID,const vector<piece_id_t>& pieces);
	//发送到dst频道channelID的片段号pieceID长为len的数据，如果没有这片段数据，则ok为false
	void sendPiece(addr_t dst,channel_id_t channelID,piece_id_t pieceID,size_t sessionID);
	//请求作为邻居
	void sendNeightborRequest(addr_t dst);
	void sendChoke(addr_t dst);
	void sendUnChoke(addr_t dst);
	void sendIntreasting(addr_t dst);
	void sendUnIntreasting(addr_t dst);
	void sendBufferMap(addr_t dst);
	void sendPushedBufferRange(addr_t dst);

	//收到邻居请求
	void onNeightborRequestMsg(Packet* pkt);
	void onNeightborGossipMsg(Packet* pkt);
	void onHandshakeMsg(Packet* pkt);
	//收到邻居请求的接受响应
	//void onNeightborResponseAcceptMsg(Packet* pkt);
	//收到邻居请求的拒绝响应
	//void onNeightborResponseRejectMsg(Packet* pkt);
	//收到邻居响应
	void onNeightborsMsg(Packet* pkt);
	//收到片段请求
	void onPieceRequestMsg(Packet* pkt);
	//收到片段响应
	void onPieceMsg(Packet* pkt);
	//子节点通知父节点断开连接
	void onChildToFatherDisconnectMsg(Packet* pkt);
	//父节点通知子节点断开连接
	void onFatherToChildDisconnectMsg(Packet* pkt);
	////子节点通知父节点断开连接
	//void onChildToFatherDisconnectMsg(Packet* pkt);
	//父节点通知子节点连接
	void onFatherToChildConnectMsg(Packet* pkt);
	void onChildToFatherConnectMsg(Packet* pkt);
	//父节点收到子节点的PUSH Rang消息
	void onRangeMsg(Packet* pkt);

public:
	void onRecvdBlock(channel_id_t channelID,block_id_t blockID);
	void onPlayerUrgent(piece_id_t);
	time_s_t requestPendingTimeOut(piece_id_t id)
	{
		return m_requestPendingQue.timeOut(id);
	}
public:
	//////////////////////////////////////////////////////////////////////////
	//周期性或者启动事件

	//加入
	void onJoinTimer();
	void onLeaveTimer();
	//周期性更新合作节点定时器
	void onUpdataNeightborsTimer();
	//周期性更新父节点定时器
	void onUpdataFathersTimer();
	//周期性更新子节点定时器
	void onUpdataChildrenTimer();
	//周期性buffermap通告定时器
	void onSendBufferMapTimer();
	//周期性数据请求定时器
	void onRequestPieceTimer();

private:
	//////////////////////////////////////////////////////////////////////////
	//数据调度策略
	void scheduleRequest();
	//请求方数据调度策略_稀有先取
	void requestScheduling_RF();
	//请求方数据调度策略_随机
	void requestScheduling_Rnd();
	void requestScheduling_RF_Rnd();
	void requestScheduling_RF_Rnd_2();
	//发送方数据调度策略_先到先服务
	void responseScheduling_FIFO(Packet*pkt);

public:
	int getMaxDownPieceCntNextCycle();

public:
	static void outPutStatistic();

	static size_t         s_onlineNodes;

private:
	//全局的bucketmap，动态时间坐标系下
	//s_dynamicBucketMap[i][j]表示第i个频道第j个桶中的节点集
	static bucket_map_t s_dynamicBucketMap;
	//全局的bucketmap，静态时间坐标系下
	//s_staticBucketMap[i][j]表示第i个频道第j个桶中的节点集
	static bucket_map_t s_staticBucketMap;
	//根据Zipf分布计算出每个影片的受欢迎程度
	static vector<double> s_channelPopulation;
	static StatisticTimer s_statistic;

	PendingRequestQue    m_requestPendingQue;//发出请求，但还没有收到数据
	ResponseFifoQue      m_responsePendingQue[3];//收到的邻居节点的数据请求队列
	ResponseFifoQue      m_urgentResponsePendingQue;//收到的邻居节点的数据请求队列
	bool                 m_sendingPiecePkt;

	father_map_t         m_fathers;
	child_map_t          m_children;
	intreasting_nb_map_t m_intreastingNB;//合作节点集

	//intreasted_nb_map_t;//对local感兴趣的节点
	//choking_nb_map_t;//被local阻塞的节点
	//choked_nb_map_t;//阻塞local的节点
	//intreasting_nb_map_t;//local感兴趣的节点
	//unintreasing_nb_map_t;//local暂不感兴趣的节点


	NeightborInfo   m_supperNeightbor;//此为服务器
	bool            m_supperBeNeightbor;//是否将服务器直接作为邻居

	time_s_t m_joinTime;//加入时间
	time_s_t m_leaveTime;//离开时间
	StrmBuffer m_playingBuffer;//播放频道的缓存
	Player m_player;//播放器
	int m_playingChannelID;//当前播放频道


	band_Bps_t m_maxDownloadRate;
	band_Bps_t m_maxUpRate;
	bucket_id_t m_dynamicBucket;//自身所在的动态坐标系下的桶

	bool m_bServer;

	//登陆定时器
	JoinTimer m_joinTimer;
	LeaveTimer m_leaveTimer;
	//周期性更新合作节点定时器
	UpdataNeightborsTimer m_updataNeightborsTimer;
	//周期性更新父节点定时器
	UpdataFathersTimer m_updataFathersTimer;
	//周期性更新子节点定时器
	UpdataChildrenTimer m_updataChildrenTimer;
	//周期性buffermap通告定时器
	SendBufferMapTimer  m_sendBufferMapTimer;
	//周期性数据请求定时器
	RequestPieceTimer m_requestPieceTimer;


	//一些统计信息
	ContributeMeter m_uplinkPushRate;
	ContributeMeter m_uplinkPulledRate;
	ContributeMeter m_uplinkAllRate;//近段时间上行带宽
	ContributeMeter m_downlinkAllRate;//近段时间下行带宽
	ContributeMeter m_neightborsContribute;//合作节点对我的贡献
	ContributeMeter m_uselessPushedRate;
	ContributeMeter m_usefulPushedRate;
	ContributeMeter m_uselessPulledRate;

	//一些统计信息
	static ContributeMeter s_uplinkPushRate;
	static ContributeMeter s_uplinkPulledRate;
	static ContributeMeter s_uplinkAllRate;//近段时间上行带宽
	static ContributeMeter s_downlinkAllRate;//近段时间下行带宽
	static ContributeMeter s_neightborsContribute;//合作节点对我的贡献
	static ContributeMeter s_uselessPushedRate;
	static ContributeMeter s_usefulPushedRate;

	size_t                 m_downloadNumAfterPlay;
	size_t                 m_allDownPieceNum;//所有下载的片段数

	piece_id_t m_maxknownPiece;
	bool m_stoped;

	REGIST_IN_CLASS(VodApp);
};


#endif//_P2PSTREAM_VOD_H_
