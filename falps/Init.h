/********************************************************************
  文件:   d:\Simulator\FEPeerSim\FEPeerSim\Init.h
  日期: 18:1:2007   13:58
  作者: 武广柱
  说明: 初始化，读取参数

  Copyright (C) 2007 - All Rights Reserved
*********************************************************************/
#ifndef __INIT_H_
#define __INIT_H_
#include "App.h"
#include "BackboneNet.h"
#include "Packet.h"
#include "Utility.h"
#include "MyIniFile.h"
#include <cstdlib>

class InitSim
{
public:
	static void initSimulator()
	{
		static bool flag = false;
		static InitSim sim;
		if (flag)
			return;
		flag = true;
		sim.init();
	}
private:
	InitSim()
	{
	};
	void init();
private:
	size_B_f_t parse_size(const string& s);
	band_Bps_t parse_bw(const string& s)
	{
		return parse_size(s);
	}
	time_s_t parse_time(const string& s);
};
#endif//__INIT_H_