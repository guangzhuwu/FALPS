#ifndef __ARGS_H__
#define __ARGS_H__


#include<hash_map>
using namespace std;
using namespace stdext;
#ifdef _WIN32
#define strtoull _strtoui64
#endif
// arguments
class Args:public stdext::hash_map<string, string>
{
	static vector<string> split(const string& line, const string& string delims=" \t")
	{
		string::size_type bi, ei;
		vector<string> words;

		bi=line.find_first_not_of(delims);
		while (bi!=string::npos)
		{
			ei=line.find_first_of(delims,bi);
			if (ei==string::npos)
				ei=line.length();
			words.push_back(line.substr(bi,ei-bi));
			bi=line.find_first_not_of(delims,ei);
		}

		return words;
	}
public:
	Args()
	{
	}

	Args(vector<string>* v, int offset=0)
	{
		for (unsigned int i=offset; i<v->size(); i++)
		{
			vector<string> arg=split((*v)[i],"=");
			insert(make_pair(arg[0],arg[1]));
		}
	}

	template <class T>
	T nget(const string& s, T defaultv=(T)0, unsigned base=16)
	{
		if (this->find(s)==this->end())
			return defaultv;
		return (T)strtoull((*this)[s].c_str(),NULL,base);
	}
	double fget(const string& s, double defaultv=0.0)
	{
		if (this->find(s)==this->end())
			return defaultv;
		return atof((*this)[s].c_str());
	}
	string sget(const string&, string defaultv="?")
	{
		if (this->find(s)==this->end())
			return defaultv;
		return ((*this)[s]);
	}
};

#endif // __ARGS_H
