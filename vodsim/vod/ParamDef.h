#ifndef _P2PSTREAM_CONSTDEFINE_H_
#define _P2PSTREAM_CONSTDEFINE_H_

#include "falps.h"

typedef int channel_id_t;
typedef int bucket_id_t;
typedef int block_id_t;
typedef int piece_id_t;
typedef int sequence_id_t;
typedef int offset_t;

static const addr_t SERVER_ADDRESS=0;//��������ַ
static const band_bps_t SERVER_BANDWIDTH=10 _Mb_;//����������

static const size_t CHANNEL_COUNT=1;
static const double JOIN_NODES_PER_SECOND=0.5;//ƽ��ÿ����뼸���ڵ�

static const time_s_t MEDIA_TIME=3000;//����ӰƬ������
static const time_s_t BUCKET_TIME=3000;//Ͱ��С(s)
static const time_s_t BUFFER_TIME=3000;//���������С(s)
static const time_s_t EXCHANGE_BUFFER_TIME=30;//������������С(s)

static const size_t NEIGHTBOR_COUNT=9;//�����ڵ����
static const size_t CHILDREN_COUNT=10;
static const size_t FATHER_COUNT=5;

static const double LEAVE_RATE=0.1;


static const rate_Bps_t MEDIA_BPS =300 _Kb_;//����
static const size_B_i_t PIECE_LEN =1 _KB_;//ÿ��СƬ�Ĵ�С
static const time_s_t   PLAY_TIME_PER_PIECE=PIECE_LEN/MEDIA_BPS;

static const time_s_t BUFFERMAP_CYCLE_TIME=1.5;//����buffermap����
static const time_s_t PIECE_REQUEST_CYCLE_TIME=1;//������������
static const time_s_t UPDATA_OVERLAY_TIME=5.0;//ά��overlay����

static const size_t   PIECE_COUNT_PER_CYCLE=(size_t)ceil((MEDIA_BPS/PIECE_LEN)*PIECE_REQUEST_CYCLE_TIME);

//static const time_s_t   PLAY_TIME_PER_PIECE=PIECE_LEN/MEDIA_BPS;//ÿ��Ƭ�Ĳ���ʱ��
static const size_B_i_t CONTROL_MSG_LEN = 20;//���ֿ�����Ϣ����(bytes)

static const time_s_t START_PLAY_CACH_TIME =10.0;//��ʼ����ǰ��Ҫ���������
static const time_s_t URGENT_1_REQUEST_TIME=2.0*PIECE_REQUEST_CYCLE_TIME;//����������ά�����ʱ�䣬���������
static const time_s_t URGENT_2_REQUEST_TIME=6;//����������ά�����ʱ�䣬����˳��ȡ����
static const size_t   MAX_PARALLEL_REQUEST_PER_BLOCK=6;//һ��Ƭ�������Էֳɼ��������� 

static const time_s_t HARTBEAT_TIME = 120;
static const size_t MAXNODNUM = 5000;//���ڵ���

//for nc
static const size_B_i_t PIECE_COUNT_PER_BLOCK=32;//һ��block���ֳɵĿ���
static const size_B_i_t BLOCK_LEN =(size_B_i_t)MEDIA_BPS;//ÿ����Ĵ�С
static const size_B_i_t PIECE_LEN_NC =BLOCK_LEN/PIECE_COUNT_PER_BLOCK;//ÿ��СƬ�Ĵ�С
static const time_s_t   PLAY_TIME_PER_BLOCK=BLOCK_LEN/MEDIA_BPS;//ÿ����Ĳ���ʱ��

#endif//_P2PSTREAM_CONSTDEFINE_H_