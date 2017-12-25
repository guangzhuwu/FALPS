/********************************************************************
文件: Buffer.h
日期: 18:1:2007   13:53
作者: 武广柱
说明: 模拟片段缓存区

Copyright (C) 2007 - All Rights Reserved
*********************************************************************/
#ifndef _P2PSTREAM_BUFFER_H_
#define _P2PSTREAM_BUFFER_H_

#include "Falps.h"
#include "ParamDef.h"
#include <bitset>
using namespace std;

enum piece_state_t
{
	PIECE_VALID,
	PIECE_INVALID,
	PIECE_REQUESTING,
};

class StrmBuffer:public Data
{

public:
	StrmBuffer();
	
	void bindApp(Application& app)
	{
		m_app=&app;
	}

	//下载完一个单元，插入到buffer中
	bool insert(int id);

	//是否有这个id块
	//拥有这个buffer的节点自己调用
	bool exist(sequence_id_t id);

	//是否有这个id块
	//其它节点调用，以模拟buffer map gossip
	static bool exist(addr_t ip,sequence_id_t id);

	//从这个id开始，还有多少个连续的片段可以播放
	sequence_id_t continuousNum(sequence_id_t id);

	//发送给了邻居节点buffermap，更新全局map
	//这近似模拟了buffer map 通告过程
	void updataGloableMap();

	//设置成具有所有片段（服务器用）
	void setAll();
	void clearAll();

protected:
	//为了节省内存，并提高仿真速度，这里用一个全局的buffermap。
	//但是，节点在计算拥有某片段的节点的个数时候，仅仅取部分区域计算，这不会给
	//仿真带来任何误差。
	//误差存在于：则实际情况下邻居节点不会同时收到buffermap消息，更新不同步，
	//而这里邻居节点将被同时告知buffermap情况。

	//<节点,片段>,s_gloableBufferMap[3][5]==true表示节点3拥有片段5
	enum{
		PIECE_MAP_SIZE=3600*60//==MEDIA_TIME/PLAY_TIME_PER_PIECE
	};

	static vector<bitset<PIECE_MAP_SIZE> > s_gloableBufferMap;

	//在一个buffermap发送周期内，收到的片段
	set<piece_id_t> m_recvdThisCycle;
	//buffer已经满了，下载一个，就要删除一个
	piece_id_t         m_erasedPieceBecauseOfBufferFull;
	//总共下载了多少piece
	size_t             m_allDownPieceCnt;
	Application*       m_app;
};

#endif//_P2PSTREAM_BUFFER_H_
