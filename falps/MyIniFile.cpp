#include "MyIniFile.h"

namespace
{
void trimStr(string& str)
{
	string s = str;
	size_t j = 0;
	for (size_t i = 0; i < s.length(); i++)
	{
		if (s[i] != ' ' && s[i] != '\n' && s[i] != '\r')
			str[j++] = str[i];
	}
	str.resize(j);
};
}

TMyIniFile::TMyIniFile(const char* fname)
{
	if (fname)
		readFile(fname);
}

string TMyIniFile::getOrSetValue(const string& sectionName,
	const string& itermName, const string& defaultvalue)
{
	TIniInfo::iterator itr1;
	TIniInfoPair::iterator itr2;
	if ((itr1 = Sections.find(sectionName)) == Sections.end())
	{
		TIniInfoPair tmp;
		tmp.insert(TIniInfoPair::value_type(itermName, defaultvalue));
		Sections.insert(TIniInfo::value_type(sectionName, tmp));
		changed = true;
		return defaultvalue;
	}
	if ((itr2 = itr1->second.find(itermName)) == itr1->second.end())
	{
		itr1->second.insert(TIniInfoPair::value_type(itermName, defaultvalue));
		changed = true;
		return defaultvalue;
	}
	return itr2->second;
}
void TMyIniFile::readFile(const char* fname)
{
	string strLine;
	fileName = fname;
	changed = false;
	ifstream fIni;
	fIni.open(fname, ios_base::in);
	if (!fIni.good())
		return;
	string sectionnamestr;
	TIniInfo::iterator itr;
	while (!fIni.eof())
	{
		getline(fIni, strLine);
		if (strLine.length() == 0)  //空白行
			continue;
		trimStr(strLine);
		if (strLine[0] == '#')//注释行
			continue;
		if (strLine[0] == '[')  //此行是一个section
		{
			TIniInfoPair tmpPair;
			size_t pos = strLine.find(']');
			string::iterator itr = (pos == -1) ?
				strLine.end() :
				strLine.end() -
				1;
			sectionnamestr = string(strLine.begin() + 1, itr);
			Sections.insert(TIniInfo::value_type(sectionnamestr, tmpPair));
		}
		else if ((itr = Sections.find(sectionnamestr)) != Sections.end())
		{
			size_t pos = strLine.find('=');
			if (pos == -1)
				continue;
			string str1(strLine.begin(), strLine.begin() + pos);
			string str2(strLine.begin() + pos + 1, strLine.end());
			itr->second.insert(TIniInfoPair::value_type(str1, str2));
		}
	}
	fIni.close();
}
void TMyIniFile::writeFile(const char* fname)
{
	string file = fname ? fname : fileName;
	if (!changed && file == fileName)
		return;
	ofstream fp(file.c_str());
	if (!fp.good())
	{
		cout << "Error ,can not open " << fileName << endl;
		return;
	}
	changed = false;
	TIniInfo::iterator itr1;
	TIniInfoPair::iterator itr2;
	for (itr1 = Sections.begin(); itr1 != Sections.end(); itr1++)
	{
		fp << "[" << itr1->first << "]" << endl;
		for (itr2 = itr1->second.begin(); itr2 != itr1->second.end(); itr2++)
			fp << itr2->first << "=" << itr2->second << endl;
		fp << endl;
	}
}
/*
void main()
{
TMyIniFile f("src_file.ini");
f.getOrSetValue("做实验","武广柱","good");
system("pause");

}*/