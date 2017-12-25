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


//�ھӽڵ㼯
//�ڵ�����ھӽڵ����С��������ϡ����ȡ���Ȳ����µĻ����������ݵ��ȣ�Pull��
struct NeightborInfo
{
	vector<piece_id_t>normalRequestsCurrCycle;//�����������������Ƭ��
	vector<piece_id_t>urgentRequestsCurrCycle;//�����������������Ƭ��

	time_s_t beNbTime;//
	int pieceCntRecvdCurrCycle;//���������յ���Ƭ��
	double ability;//����������ÿ�ֿ��Գɹ����͵�����Ƭ�Ķ���
	size_t allRequestPieces;//����������
	size_t allResponsePieces;//���л�Ӧ��
	id_t currSessionID;//��ǰ�����session��
	size_t requestCycle;//�����������
	size_t remainAbility;//ʣ������
	ContributeMeter contribute;//�ھӵĹ������� 
	ContributeMeter consumption;//�ھӵ���������

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

//���ڵ㼯
//һ�ڵ�ĸ��ڵ㼯�Ľڵ���ýڵ�����������ͣ�Push��
struct FatherInfo 
{
	ContributeMeter contribute;//���ڵ㹱��
	FatherInfo():contribute(10){}
};

//�ӽڵ㼯
//�ڵ������ӽڵ��ڵĽڵ������������(Push)
struct ChildInfo
{
	size_t ability;//�ӵ��������
	pair<piece_id_t,piece_id_t> range;//һ��������Ҫ���͵��������ڵ�����
	size_t pusedCntCurrCycle;
	ContributeMeter contribute;//���͸��ӽڵ�����ݴ���
	ChildInfo():contribute(10){range.first=-1;range.second=-1;pusedCntCurrCycle=0;}
};



typedef vector< vector<list<addr_t> > > bucket_map_t;
typedef map<addr_t,NeightborInfo>       intreasted_nb_map_t;//��local����Ȥ�Ľڵ�
typedef map<addr_t,NeightborInfo>       choking_nb_map_t;//��local�����Ľڵ�
typedef map<addr_t,NeightborInfo>       choked_nb_map_t;//����local�Ľڵ�
typedef map<addr_t,NeightborInfo>       intreasting_nb_map_t;//local����Ȥ�Ľڵ�
typedef map<addr_t,NeightborInfo>       unintreasing_nb_map_t;//local�ݲ�����Ȥ�Ľڵ�
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
	//��̬ʱ������ϵ
	bucket_id_t playingPoint2DynamicBucketID(time_s_t playingPoint);
	//��̬ʱ������ϵ
	bucket_id_t playingPoint2StaticBucketID(time_s_t playingPoint);
	//�˳���ǰ���ڵ�Ͱ
	void        leaveFromCurrentDynamicBucket();


public:
	//////////////////////////////////////////////////////////////////////////
	//Overlay��֯����
	void        neightborGossip();

	//�����ھӽڵ�
	void        updataNeightbors();
	//ɾ��Ч�ʵ͵ĺ����ڵ�
	addr_t      eraseNeightbors();
	//���Ҹ��ڵ�
	void        updataFathers();
	//ɾ��Ч�ʵ͵ĸ��ڵ�
	addr_t      eraseFathers(bool must=false);
	//�����ӽڵ�
	void        findNewChildren();
	//ɾ��Ч�ʵ͵��ӽڵ�
	addr_t      eraseChildren();

public:
	//��dst����Ƶ��channelID���ŵ���myPlayingPoint�������ھӽڵ�
	//void sendNeightborRequest(addr_t dst,channel_id channelID,time_s_t myPlayingPoint);
	//������dst����Ƶ��channelID������
	id_t sendPieceRequest(addr_t dst,channel_id_t channelID,const vector<piece_id_t>& pieces);
	//���͵�dstƵ��channelID��Ƭ�κ�pieceID��Ϊlen�����ݣ����û����Ƭ�����ݣ���okΪfalse
	void sendPiece(addr_t dst,channel_id_t channelID,piece_id_t pieceID,size_t sessionID);
	//������Ϊ�ھ�
	void sendNeightborRequest(addr_t dst);
	void sendChoke(addr_t dst);
	void sendUnChoke(addr_t dst);
	void sendIntreasting(addr_t dst);
	void sendUnIntreasting(addr_t dst);
	void sendBufferMap(addr_t dst);
	void sendPushedBufferRange(addr_t dst);

	//�յ��ھ�����
	void onNeightborRequestMsg(Packet* pkt);
	void onNeightborGossipMsg(Packet* pkt);
	void onHandshakeMsg(Packet* pkt);
	//�յ��ھ�����Ľ�����Ӧ
	//void onNeightborResponseAcceptMsg(Packet* pkt);
	//�յ��ھ�����ľܾ���Ӧ
	//void onNeightborResponseRejectMsg(Packet* pkt);
	//�յ��ھ���Ӧ
	void onNeightborsMsg(Packet* pkt);
	//�յ�Ƭ������
	void onPieceRequestMsg(Packet* pkt);
	//�յ�Ƭ����Ӧ
	void onPieceMsg(Packet* pkt);
	//�ӽڵ�֪ͨ���ڵ�Ͽ�����
	void onChildToFatherDisconnectMsg(Packet* pkt);
	//���ڵ�֪ͨ�ӽڵ�Ͽ�����
	void onFatherToChildDisconnectMsg(Packet* pkt);
	////�ӽڵ�֪ͨ���ڵ�Ͽ�����
	//void onChildToFatherDisconnectMsg(Packet* pkt);
	//���ڵ�֪ͨ�ӽڵ�����
	void onFatherToChildConnectMsg(Packet* pkt);
	void onChildToFatherConnectMsg(Packet* pkt);
	//���ڵ��յ��ӽڵ��PUSH Rang��Ϣ
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
	//�����Ի��������¼�

	//����
	void onJoinTimer();
	void onLeaveTimer();
	//�����Ը��º����ڵ㶨ʱ��
	void onUpdataNeightborsTimer();
	//�����Ը��¸��ڵ㶨ʱ��
	void onUpdataFathersTimer();
	//�����Ը����ӽڵ㶨ʱ��
	void onUpdataChildrenTimer();
	//������buffermapͨ�涨ʱ��
	void onSendBufferMapTimer();
	//��������������ʱ��
	void onRequestPieceTimer();

private:
	//////////////////////////////////////////////////////////////////////////
	//���ݵ��Ȳ���
	void scheduleRequest();
	//�������ݵ��Ȳ���_ϡ����ȡ
	void requestScheduling_RF();
	//�������ݵ��Ȳ���_���
	void requestScheduling_Rnd();
	void requestScheduling_RF_Rnd();
	void requestScheduling_RF_Rnd_2();
	//���ͷ����ݵ��Ȳ���_�ȵ��ȷ���
	void responseScheduling_FIFO(Packet*pkt);

public:
	int getMaxDownPieceCntNextCycle();

public:
	static void outPutStatistic();

	static size_t         s_onlineNodes;

private:
	//ȫ�ֵ�bucketmap����̬ʱ������ϵ��
	//s_dynamicBucketMap[i][j]��ʾ��i��Ƶ����j��Ͱ�еĽڵ㼯
	static bucket_map_t s_dynamicBucketMap;
	//ȫ�ֵ�bucketmap����̬ʱ������ϵ��
	//s_staticBucketMap[i][j]��ʾ��i��Ƶ����j��Ͱ�еĽڵ㼯
	static bucket_map_t s_staticBucketMap;
	//����Zipf�ֲ������ÿ��ӰƬ���ܻ�ӭ�̶�
	static vector<double> s_channelPopulation;
	static StatisticTimer s_statistic;

	PendingRequestQue    m_requestPendingQue;//�������󣬵���û���յ�����
	ResponseFifoQue      m_responsePendingQue[3];//�յ����ھӽڵ�������������
	ResponseFifoQue      m_urgentResponsePendingQue;//�յ����ھӽڵ�������������
	bool                 m_sendingPiecePkt;

	father_map_t         m_fathers;
	child_map_t          m_children;
	intreasting_nb_map_t m_intreastingNB;//�����ڵ㼯

	//intreasted_nb_map_t;//��local����Ȥ�Ľڵ�
	//choking_nb_map_t;//��local�����Ľڵ�
	//choked_nb_map_t;//����local�Ľڵ�
	//intreasting_nb_map_t;//local����Ȥ�Ľڵ�
	//unintreasing_nb_map_t;//local�ݲ�����Ȥ�Ľڵ�


	NeightborInfo   m_supperNeightbor;//��Ϊ������
	bool            m_supperBeNeightbor;//�Ƿ񽫷�����ֱ����Ϊ�ھ�

	time_s_t m_joinTime;//����ʱ��
	time_s_t m_leaveTime;//�뿪ʱ��
	StrmBuffer m_playingBuffer;//����Ƶ���Ļ���
	Player m_player;//������
	int m_playingChannelID;//��ǰ����Ƶ��


	band_Bps_t m_maxDownloadRate;
	band_Bps_t m_maxUpRate;
	bucket_id_t m_dynamicBucket;//�������ڵĶ�̬����ϵ�µ�Ͱ

	bool m_bServer;

	//��½��ʱ��
	JoinTimer m_joinTimer;
	LeaveTimer m_leaveTimer;
	//�����Ը��º����ڵ㶨ʱ��
	UpdataNeightborsTimer m_updataNeightborsTimer;
	//�����Ը��¸��ڵ㶨ʱ��
	UpdataFathersTimer m_updataFathersTimer;
	//�����Ը����ӽڵ㶨ʱ��
	UpdataChildrenTimer m_updataChildrenTimer;
	//������buffermapͨ�涨ʱ��
	SendBufferMapTimer  m_sendBufferMapTimer;
	//��������������ʱ��
	RequestPieceTimer m_requestPieceTimer;


	//һЩͳ����Ϣ
	ContributeMeter m_uplinkPushRate;
	ContributeMeter m_uplinkPulledRate;
	ContributeMeter m_uplinkAllRate;//����ʱ�����д���
	ContributeMeter m_downlinkAllRate;//����ʱ�����д���
	ContributeMeter m_neightborsContribute;//�����ڵ���ҵĹ���
	ContributeMeter m_uselessPushedRate;
	ContributeMeter m_usefulPushedRate;
	ContributeMeter m_uselessPulledRate;

	//һЩͳ����Ϣ
	static ContributeMeter s_uplinkPushRate;
	static ContributeMeter s_uplinkPulledRate;
	static ContributeMeter s_uplinkAllRate;//����ʱ�����д���
	static ContributeMeter s_downlinkAllRate;//����ʱ�����д���
	static ContributeMeter s_neightborsContribute;//�����ڵ���ҵĹ���
	static ContributeMeter s_uselessPushedRate;
	static ContributeMeter s_usefulPushedRate;

	size_t                 m_downloadNumAfterPlay;
	size_t                 m_allDownPieceNum;//�������ص�Ƭ����

	piece_id_t m_maxknownPiece;
	bool m_stoped;

	REGIST_IN_CLASS(VodApp);
};


#endif//_P2PSTREAM_VOD_H_
