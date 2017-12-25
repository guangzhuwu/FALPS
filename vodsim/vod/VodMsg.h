#ifndef _P2PSTREAM_VODMSG_H_ 
#define _P2PSTREAM_VODMSG_H_

#include "Falps.h"
#include "ParamDef.h"
#include "StrmBuffer.h"
#include <vector>
#include <map>
#include <set>
using namespace std;

#define  MSG_PEER_REQUEST			000//�ڵ���IS������ĳƵ���еĽڵ�
#define  MSG_PEER_RESPONSE			001//IS�ظ�����

#define  MSG_BUFFERMAP				101//buffer map��Ϣ
#define  MSG_NEIGHTBOR_GOSSIP		102//��GossipЭ������neightbor
#define  MSG_NEIGHTBOR_ACCEPT		103//
#define  MSG_NEIGHTBOR_REJECT		104
#define  MSG_PIECE_REQUEST			105
#define  MSG_PIECE					106
#define  MSG_CHOKED_TRUE			107//�����Է�
#define  MSG_CHOKED_FALSE			108//��������Է�
#define  MSG_INTREASTED_TRUE		109//֪ͨ�Է����ڵ�������Ȥ��Ҫ��ʱ����buffer map
#define  MSG_INTREASTED_FALSE		110//֪ͨ�Է����ڵ����ʧȥ��Ȥ��Ҫ��ֹͣ����buffer map
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
	list<addr_t> addressList;//IS�ظ���
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

//PIECE_DATA_spp����δʵ��