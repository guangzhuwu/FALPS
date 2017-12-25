#define _CRT_SECURE_NO_WARNINGS
#include <ctime>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Log.h"
#include "Falps.h"

using namespace std;

const char* Logger::LogHead[]=
{
	"",
	"[致命错误]",
	"[严重错误]",
	"[一般错误]",

	"[警    告]",
	"[注    意]",
	"[消    息]",

	"[调试信息]",
	"[调试跟踪]"
};
//const char* Logger::LogHead[]=
//{
//	"[FALTAL]",
//	"[CRITICAL]",
//	"[ERROR]",
//	"[WARRING]",
//	"[NOTICE]",
//	"[INFORMATION]",
//
//	"[DEBUG]",
//	"[TRACK]"
//};
void Logger::log(enum_LogLevel level,char* fname,long line,const char* msg)
{
	static ofstream outfile;
	if (!outfile.is_open()||!outfile.good())
	{
		time_t t = time(NULL);
		struct tm *tp = localtime(&t);
		char buf[128];
		sprintf(buf,"[%d_%02d_%02d-%02d_%02d_%02d].log",
			tp -> tm_year + 1900,
			tp -> tm_mon + 1,
			tp -> tm_mday,
			tp -> tm_hour,tp -> tm_min,tp -> tm_sec);
		outfile.open(buf);
	}
	char buf[8192];
	pathToName(fname,buf);
	stringstream rst;
	//rst<<LogHead[level]<<" ["<<NOW()<<"]"<<" ["<<buf<<"("<<line<<")"<<"]\n"<<msg<<"\n";
	rst<<msg<<"\n";
	outfile<<rst.str().c_str();
	outfile.flush();
	cout<<rst.str();
}

void Logger::pathToName(char* path,char* name)
{
	char* start=path;
	char* end=path;
	for (;;)
	{
		if (*end=='\\'||*end=='/')
			start=end;		
		else if(*end==0)
			break;
		++end;
	}
	strcpy(name,++start);
}