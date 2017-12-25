/********************************************************************
文件:   d:\MyProjects\FEPeerSim\FEPeerSim\MyIniFile.h
日期: 2006/11/02
作者: 武广柱 wuguangzhu@gmail.com

说明: 读取或者写入配置文件
*********************************************************************/
#ifndef _MYINIFILE_H_
#define _MYINIFILE_H_

#include <cstdio>
#include <string>
#include <cstring>
#include <map>
#include <iostream>
#include <fstream>
using namespace std;

typedef map<string, string> TIniInfoPair;
typedef map<string, TIniInfoPair> TIniInfo;

class TMyIniFile
{
public:
	TMyIniFile(const char* filename = NULL);
	~TMyIniFile()
	{
		writeFile();
	};  
	void readFile(const char* fname);
	void writeFile(const char* fname = NULL);
	string getOrSetValue(const string& sectionName, const string& itermName,
		const string& defaultvalue);

protected:
	FILE* fIni;    //ini文件
	bool changed;  //是否有写入的动作,暂时没有给你用，自己看着扩充功能好了
	TIniInfo Sections;//所有区块
	string fileName;  //ini文件名
};

#endif

