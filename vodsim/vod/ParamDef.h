#ifndef _P2PSTREAM_CONSTDEFINE_H_
#define _P2PSTREAM_CONSTDEFINE_H_

#include "falps.h"

typedef int channel_id_t;
typedef int bucket_id_t;
typedef int block_id_t;
typedef int piece_id_t;
typedef int sequence_id_t;
typedef int offset_t;

static const addr_t SERVER_ADDRESS=0;//服务器地址
static const band_bps_t SERVER_BANDWIDTH=10 _Mb_;//服务器带宽

static const size_t CHANNEL_COUNT=1;
static const double JOIN_NODES_PER_SECOND=0.5;//平均每秒加入几个节点

static const time_s_t MEDIA_TIME=3000;//整个影片播放秒
static const time_s_t BUCKET_TIME=3000;//桶大小(s)
static const time_s_t BUFFER_TIME=3000;//整个缓存大小(s)
static const time_s_t EXCHANGE_BUFFER_TIME=30;//交换缓存区大小(s)

static const size_t NEIGHTBOR_COUNT=9;//合作节点个数
static const size_t CHILDREN_COUNT=10;
static const size_t FATHER_COUNT=5;

static const double LEAVE_RATE=0.1;


static const rate_Bps_t MEDIA_BPS =300 _Kb_;//码率
static const size_B_i_t PIECE_LEN =1 _KB_;//每个小片的大小
static const time_s_t   PLAY_TIME_PER_PIECE=PIECE_LEN/MEDIA_BPS;

static const time_s_t BUFFERMAP_CYCLE_TIME=1.5;//发送buffermap周期
static const time_s_t PIECE_REQUEST_CYCLE_TIME=1;//请求数据周期
static const time_s_t UPDATA_OVERLAY_TIME=5.0;//维护overlay周期

static const size_t   PIECE_COUNT_PER_CYCLE=(size_t)ceil((MEDIA_BPS/PIECE_LEN)*PIECE_REQUEST_CYCLE_TIME);

//static const time_s_t   PLAY_TIME_PER_PIECE=PIECE_LEN/MEDIA_BPS;//每个片的播放时间
static const size_B_i_t CONTROL_MSG_LEN = 20;//各种控制消息长度(bytes)

static const time_s_t START_PLAY_CACH_TIME =10.0;//开始播放前需要缓冲的秒数
static const time_s_t URGENT_1_REQUEST_TIME=2.0*PIECE_REQUEST_CYCLE_TIME;//如果缓冲仅可维持这段时间，则紧急请求
static const time_s_t URGENT_2_REQUEST_TIME=6;//如果缓冲仅可维持这段时间，则尽量顺序取数据
static const size_t   MAX_PARALLEL_REQUEST_PER_BLOCK=6;//一个片段最多可以分成几块来请求 

static const time_s_t HARTBEAT_TIME = 120;
static const size_t MAXNODNUM = 5000;//最大节点数

//for nc
static const size_B_i_t PIECE_COUNT_PER_BLOCK=32;//一个block被分成的块数
static const size_B_i_t BLOCK_LEN =(size_B_i_t)MEDIA_BPS;//每个块的大小
static const size_B_i_t PIECE_LEN_NC =BLOCK_LEN/PIECE_COUNT_PER_BLOCK;//每个小片的大小
static const time_s_t   PLAY_TIME_PER_BLOCK=BLOCK_LEN/MEDIA_BPS;//每个块的播放时间

#endif//_P2PSTREAM_CONSTDEFINE_H_