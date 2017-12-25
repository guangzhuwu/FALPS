/********************************************************************
  文件:   d:\Simulator\FEPeerSim\FEPeerSim\random.h
  日期: 18:1:2007   13:58
  作者: 武广柱
  说明: 随机数

  Copyright (C) 2007 - All Rights Reserved
*********************************************************************/
#ifndef _RANDOM_H_
#define _RANDOM_H_
#include "Mt19937ar.h"

#ifndef uint32
typedef unsigned int uint32;
#endif

inline void Srand(uint32 seed)
{
	init_genrand(seed);
}

inline double Rand01()
{
	return genrand_real1();
}

inline double Rand(double a, double b)
{
	double c = (b - a) * Rand01();
	return c > 0 ? c + a : c + b;
}

inline unsigned char RandUint8()
{
	unsigned char intMax = static_cast<unsigned char>(~0);
	unsigned char d = static_cast<unsigned char>(genrand_int32()%
		((unsigned short) intMax + 1));
	return d;
}

inline unsigned short RandUint16()
{
	unsigned short intMax = static_cast<unsigned short>(~0);
	unsigned short d = static_cast<unsigned short>(genrand_int32()%
		((uint32) intMax + 1));
	return d;
}

inline unsigned int RandUint32()
{
	return genrand_int32();
}

#endif//_RANDOM_H_