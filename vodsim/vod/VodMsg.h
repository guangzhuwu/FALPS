#ifndef _P2PSTREAM_VODMSG_H_ 
#define _P2PSTREAM_VODMSG_H_

#include "Falps.h"
#include "ParamDef.h"
#include "StrmBuffer.h"
#include <vector>
#include <map>
#include <set>
using namespace std;

#define  MSG_PEER_REQUEST			000//节点向IS请求在某频道中的节点
#define  MSG_PEER_RESPONSE			001//IS回复请求

#define  MSG_BUFFERMAP				101//buffer map消息
#define  MSG_NEIGHTBOR_GOSSIP		102//用Gossip协议搜索neightbor
#define  MSG_NEIGHTBOR_ACCEPT		103//
#define  MSG_NEIGHTBOR_REJECT		104
#define  MSG_PIECE_REQUEST			105
#define  MSG_PIECE					106
#define  MSG_CHOKED_TRUE			107//阻塞对方
#define  MSG_CHOKED_FALSE			108//解除阻塞对方
#define  MSG_INTREASTED_TRUE		109//通知对方本节点对其感兴趣，要求定时发送buffer map
#define  MSG_INTREASTED_FALSE		110//通知对方本节点对其失去兴趣，要求停止发送buffer map
#define  MSG_NOT_BE_MY_FATHER		111
#define  MSG_BE_MY_FATHER			112
#define  MSG_NOT_BE_MY_CHILD		113
#define  MSG_BE_MY_CHILD			114
#define  MSG_RANGE					115





struct FindNeightborMsg:public Data
{
	bool isContact;
	addr_t srcAddress;
	time_s_t playingPoint;
	FindNeightborMsg(addr_t srcIP,time_s_t p,bool isContact)
		:srcAddress(srcIP),playingPoint(p),isContact(isContact)
	{
	}
};

struct FindNeightborResponseMsg:public Data
{
	list<addr_t> addressList;//IS回复用
	addr_t       address;
	time_s_t     playingPoint;
	FindNeightborResponseMsg(const list<addr_t>&addressList)
		:addressList(addressList),address(INVALID_ADDR),playingPoint(-1)
	{
	}
	FindNeightborResponseMsg(addr_t srcIP,time_s_t p)
		:address(srcIP),playingPoint(p)
	{
	}
};

struct PieceRequestMsg :public Data
{
	channel_id_t channelID;
	id_t sessionID;
	vector<piece_id_t> normalPieces;
	vector<piece_id_t> urgentPieces;
	PieceRequestMsg()
	{
		static id_t id=0;
		sessionID=id++;
	}
};

struct PieceMsg : public Data
{
	piece_id_t pieceID;
	id_t sessionID;
	channel_id_t channelID;
};

struct RangeMsg:public Data
{
	pair<piece_id_t,piece_id_t> range;
};

#endif//_P2PSTREAM_VODMSG_H_

//PIECE_DATA_spp，暂未实现