#include "Init.h"

size_B_f_t InitSim::parse_size(const string& s)
{
	string::size_type n = s.find_first_not_of("0123456789.");
	if (n != string::npos)
	{
		// Found non-numeric
		string numeric = s.substr(0, n);
		string trailer = s.substr(n, string::npos);
		double r = atof(numeric.c_str());
		if (trailer.find_first_of("b") == 0)
			return r / 8.0; 	// Bits
		if (trailer.find_first_of("B") == 0)
			return r; // Bytes
		if (trailer.find_first_of("kb") == 0)
			return r * 1024.0 / 8.0; // KiloBits
		if (trailer.find_first_of("Kb") == 0)
			return r * 1024.0 / 8.0; // KiloBits
		if (trailer.find_first_of("KB") == 0)
			return r * 1024.0; // KiloBytes
		if (trailer.find_first_of("Mb") == 0)
			return r * 1024.0 * 1024.0 / 8.0; // MegaBits
		if (trailer.find_first_of("MB") == 0)
			return r * 1024.0 * 1024.0; // MegaBytes
		if (trailer.find_first_of("Gb") == 0)
			return r * 1024.0 * 1024.0 * 1024.0 / 8.0; // GigaBits
		if (trailer.find_first_of("GB") == 0)
			return r * 1024.0 * 1024.0 * 1024.0; // GigaBytes
		cout << "Can't parse size " << s << endl;
		abort();
	}
	return atof(s.c_str()); // No special formatting, just convert value
}

time_s_t InitSim::parse_time(const string& s)
{
	string::size_type n = s.find_first_not_of("0123456789.");
	if (n != string::npos)
	{
		// Found non-numeric
		string numeric = s.substr(0, n);
		string trailer = s.substr(n, string::npos);
		double r = atof(numeric.c_str());
		if (trailer.find("s") == 0)
			return r;
		if (trailer.find("ms") == 0)
			return r / 1000.0;
		cout << "Can't parse time " << s << endl;
		abort();
	}
	return atof(s.c_str()); // No special formatting, just convert value
}

void InitSim::init()
{
	TMyIniFile inif("falps.ini");
	//随机数相关
	RAND_SEED = atoi(inif.getOrSetValue("RANDCONFIG", "RAND_SEED",
							"1234567//随机种子").c_str());
	Srand(RAND_SEED);
	//初始化Backbone相关的参数
	int nodeCount = atoi(inif.getOrSetValue("BACKBONE", "nodeCount",
								"10000//最大节点个数").c_str());
	int routerCount = atoi(inif.getOrSetValue("BACKBONE", "routerCount",
									"10//骨干网中路由器个数").c_str());
	string delaystr = inif.getOrSetValue("BACKBONE", "meanDelayBetweenRouter",
							"10ms//骨干网中路由器间的平均延迟");
	time_s_t delayRouter = parse_time(delaystr);
	delaystr = inif.getOrSetValue("BACKBONE", "meanDelayBetweenNodeBackbone",
						"5ms//节点到骨干网的平均延迟");
	PROTOCAL=inif.getOrSetValue("PROTOCAL","Protocal","None//P2P protocal,such as Kademlia");
	time_s_t delayNode = parse_time(delaystr);
	//网络相关
	MSS = atoi(inif.getOrSetValue("MSS", "MSS", "1024//Maximum Segment Size").c_str());
	FRAGMENT_LEN = MSS + PROTOCOL_OVERHEAD;

	ADSL_256 = atof(inif.getOrSetValue("NETCONFIG", "ADSL_256",
							"0.1//ADSL_256类型节点所占比例").c_str());
	ADSL_512 = atof(inif.getOrSetValue("NETCONFIG", "ADSL_512",
							"0.3//ADSL_512类型节点所占比例").c_str());
	ADSL_1500 = atof(inif.getOrSetValue("NETCONFIG", "ADSL_1500",
							"0.3//ADSL_1500类型节点所占比例").c_str());
	SDSL_512 = atof(inif.getOrSetValue("NETCONFIG", "SDSL_512",
							"0.2//SDSL_512类型节点所占比例").c_str());
	DS1 = atof(inif.getOrSetValue("NETCONFIG", "DS1",
						"0.1//DS1类型节点所占比例").c_str());

	BackboneNet::instance().initialize(nodeCount, routerCount, delayRouter,
								delayNode);
}