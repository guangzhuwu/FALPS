/********************************************************************
�ļ�: Buffer.h
����: 18:1:2007   13:53
����: �����
˵��: ģ��Ƭ�λ�����

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

	//������һ����Ԫ�����뵽buffer��
	bool insert(int id);

	//�Ƿ������id��
	//ӵ�����buffer�Ľڵ��Լ�����
	bool exist(sequence_id_t id);

	//�Ƿ������id��
	//�����ڵ���ã���ģ��buffer map gossip
	static bool exist(addr_t ip,sequence_id_t id);

	//�����id��ʼ�����ж��ٸ�������Ƭ�ο��Բ���
	sequence_id_t continuousNum(sequence_id_t id);

	//���͸����ھӽڵ�buffermap������ȫ��map
	//�����ģ����buffer map ͨ�����
	void updataGloableMap();

	//���óɾ�������Ƭ�Σ��������ã�
	void setAll();
	void clearAll();

protected:
	//Ϊ�˽�ʡ�ڴ棬����߷����ٶȣ�������һ��ȫ�ֵ�buffermap��
	//���ǣ��ڵ��ڼ���ӵ��ĳƬ�εĽڵ�ĸ���ʱ�򣬽���ȡ����������㣬�ⲻ���
	//��������κ���
	//�������ڣ���ʵ��������ھӽڵ㲻��ͬʱ�յ�buffermap��Ϣ�����²�ͬ����
	//�������ھӽڵ㽫��ͬʱ��֪buffermap�����

	//<�ڵ�,Ƭ��>,s_gloableBufferMap[3][5]==true��ʾ�ڵ�3ӵ��Ƭ��5
	enum{
		PIECE_MAP_SIZE=3600*60//==MEDIA_TIME/PLAY_TIME_PER_PIECE
	};

	static vector<bitset<PIECE_MAP_SIZE> > s_gloableBufferMap;

	//��һ��buffermap���������ڣ��յ���Ƭ��
	set<piece_id_t> m_recvdThisCycle;
	//buffer�Ѿ����ˣ�����һ������Ҫɾ��һ��
	piece_id_t         m_erasedPieceBecauseOfBufferFull;
	//�ܹ������˶���piece
	size_t             m_allDownPieceCnt;
	Application*       m_app;
};

#endif//_P2PSTREAM_BUFFER_H_
