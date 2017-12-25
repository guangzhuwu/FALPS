#include "Vod.h"
#include <sstream>
using namespace std;

//void main()
//{
//	vector<vector<double> > data(400,vector<double>(9,0) );
//	for(int i=0;i<5;i++)
//	{
//		char x='1'+i;
//		string fname;
//		fname+=x;
//		fname+=".log";
//		ifstream ifs(fname.c_str());
//		char buf[1024];
//		ifs.getline(buf,1023);
//		ifs.getline(buf,1023);
//		ifs.getline(buf,1023);
//		for(int k=0;k<data.size();k++)
//		{
//			for(int j=0;j<8;j++)
//			{
//				ifs.getline(buf,1023,' ');
//				data[k][j]+=atof(buf);
//			}
//			ifs.getline(buf,1023);
//			data[k][8]+=atof(buf);
//		}
//	}
//	ofstream ofs("rst.txt");
//	for(int k=0;k<data.size();k++)
//	{
//		for(int j=0;j<9;j++)
//			ofs<<data[k][j]/5.0<<" ";
//		ofs<<endl;
//	}
//}
//
void main()
{
	InitSim::initSimulator();//初始化仿真器
	
	VodApp::init();
	ostringstream info;
	info<<" ADSL_512: "<<ADSL_512
		<<" ADSL_1500: "<<ADSL_1500
		<<" SDSL_512: "<<SDSL_512
		<<" DS1: "<<DS1<<endl;
	LogInfo(info.str().c_str());

	BackboneNet::instance().setBand(SERVER_ADDRESS,100*1024*1024/8.0);
	Node* node=Node::get(0);
	VodApp* app=(VodApp*)new_class("VodApp");
	node->attachApp(app);
	app->start();
	for (size_t i=1;i<=100;++i)
	{
		Node* node=Node::get(i);
		VodApp* app=(VodApp*)new_class("VodApp");
		node->attachApp(app);
		app->start();
	}

	BackboneNet::instance().printNetInfo(50);
	system("pause");

	Scheduler::instance().run();	//启动仿真器

	system("pause");
	return ;
}
//int T=100;
//int TL=10;
//bucket_id_t playingPoint2DynamicBucketID(int now,time_s_t playingPoint)
//{
//	//详见“基于动态时间坐标系的拓扑组织方法”
//	int t=now;
//	int N=t/(int)T;
//	int O=t%(int)T;
//	int p;
//	if (N%2==0)
//	{
//		p=(int)(playingPoint-O);
//	}
//	else
//	{
//		if (playingPoint<O)
//			p=(int)playingPoint+(int)T-O;
//		else
//			p=((int)playingPoint-(int)T-O);
//	}
//	return (p/TL)+2*T/TL;
//}
//void main()
//{
//
//	for (int now=2*T;now<1000;now+=1)
//	{
//		for (int i=0;i<T;i+=1)
//		{
//			int x=i-now;
//			x%=2*T;
//			cout<<(x/TL)<<" ";
//			//cout<<playingPoint2DynamicBucketID(now,i)<<" ";
//		}
//		cout<<endl;
//		system("pause");
//	}
//}