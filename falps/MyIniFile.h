/********************************************************************
�ļ�:   d:\MyProjects\FEPeerSim\FEPeerSim\MyIniFile.h
����: 2006/11/02
����: ����� wuguangzhu@gmail.com

˵��: ��ȡ����д�������ļ�
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
	FILE* fIni;    //ini�ļ�
	bool changed;  //�Ƿ���д��Ķ���,��ʱû�и����ã��Լ��������书�ܺ���
	TIniInfo Sections;//��������
	string fileName;  //ini�ļ���
};

#endif

