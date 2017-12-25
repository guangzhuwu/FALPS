/********************************************************************
文件:   d:\MyProjects\FEPeerSim\FEPeerSim\Const.h 
日期: 2006/10/31
作者: 武广柱

说明: 一些常量的定义
*********************************************************************/

#ifndef __CONSTS_H_
#define __CONSTS_H_
#include <string>
using namespace std;

//网络类型定义
const double ADSL_256_DOWNLOAD_RATE = 256 * 1024.0 / 8.0;//Bytes 
const double ADSL_256_UPLOAD_RATE = 64 * 1024.0 / 8.0; 

const double ADSL_512_DOWNLOAD_RATE = 1536 * 1024.0 / 8.0; 
const double ADSL_512_UPLOAD_RATE = 128 * 1024.0 / 8.0; 

const double ADSL_1500_DOWNLOAD_RATE = 1536 * 1024.0 / 8.0; 
const double ADSL_1500_UPLOAD_RATE = 256 * 1024.0 / 8.0; 

const double SDSL_512_DOWNLOAD_RATE = 1536 * 1024.0 / 8.0; 
const double SDSL_512_UPLOAD_RATE = 512 * 1024.0 / 8.0; 

const double DS1_DOWNLOAD_RATE = 2048 * 1024.0 / 8.0; 
const double DS1_UPLOAD_RATE = 2048 * 1024.0 / 8.0; 


const double ETHERNET_10_DOWNLOAD_RATE = 1000010000 * 1024.0 / 8.0;
const double ETHERNET_10_UPLOAD_RATE = 1000010000 * 1024.0 / 8.0; 
const double ETHERNET_100_DOWNLOAD_RATE = 10000100000 * 1024.0 / 8.0; 
const double ETHERNET_100_UPLOAD_RATE = 10000100000 * 1024.0 / 8.0; 


const int PROTOCOL_OVERHEAD = 48;

// 报文相关
extern size_t MSS;
extern size_t FRAGMENT_LEN;

extern double ADSL_256;
extern double ADSL_512;
extern double ADSL_1500;
extern double SDSL_512;
extern double DS1;

extern string PROTOCAL;

extern unsigned int RAND_SEED;

#endif
