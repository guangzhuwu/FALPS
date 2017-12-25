/********************************************************************
  文件:   d:\MyProjects\FEPeerSim\FEPeerSim\Factory.h 
  日期: 2006/11/01
  作者: 武广柱
  
  说明: 工厂函数，用来生成对象
*********************************************************************/
#ifndef _FACTORY_H_
#define _FACTORY_H_

#include "Debug.h"
#include <map>
#include <string>
using namespace std;
using std::map;

template <typename IdentifierType, typename ProductCreator = void* (*) ()>
class Factory
{
public:
	bool Register(const IdentifierType& id, ProductCreator creator)
	{
		OutputMessage("Factory registe：%d", id);
		return associations_.insert(typename 
			IdToProductMap::value_type(id,creator)).second;
	}

	bool Unregister(const IdentifierType& id)
	{
		return associations_.erase(id) == 1;
	}

	void* CreateObject(const IdentifierType& id)
	{
		typename IdToProductMap::iterator i = associations_.find(id);
		if (i != associations_.end())
			return (i->second) ();
		OutputError("%d not registered!", id);
		abort();
		return NULL;
	}
	static Factory& instance()
	{
		static Factory instance_;
		return instance_;
	} 
	Factory()
	{
	};
private:
	Factory(const Factory&);
	Factory& operator=(const Factory&);
private:
	typedef   map<IdentifierType, ProductCreator> IdToProductMap;
	IdToProductMap associations_;
};

inline void* new_class(const string& className)
{
	return (void *) (Factory<string>::instance().CreateObject(className));
}

//安全起见,用Factory来生成一些类型的对象,用户不能调用new delet等生成
//如果这些类型的的继承类，需要在其类内写REGIST_IN_CLASS。
//方法举例： Node* node=(Node*)new_class("Node");
//不要随意更改以下宏！！！！
#define REGIST_GLOBLE_OBJECT_LIST_IN_BASECLASS(baseclassname)\
public:\
  static vector<baseclassname*>& getGlobleObjectList(){\
  static vector<baseclassname*> global_##baseclassname##_list_;\
  return global_##baseclassname##_list_;\
}\
public:\
  static baseclassname* get(id_t id){\
  FALPS_ASSERT((id>=0&&id<=getGlobleObjectList().size()),cout<<#baseclassname<<id<<" is not exist!"<<endl;);\
  return getGlobleObjectList()[id];\
}\
  static size_t size(){\
  return getGlobleObjectList().size();\
}\
protected:\
  static void clear()\
{\
  for (size_t i=0;i<size();i++)\
  delete getGlobleObjectList()[i];\
  getGlobleObjectList().clear();\
}

#define REGIST_IN_CLASS(classname) \
	static bool classname##registered__;\
public:\
	static void* __create##classname()\
{   		  \
  classname* ptr=new classname;\
  classname::getGlobleObjectList().push_back(ptr);\
  return (void*)ptr;\
}

#define REGIST_OUT_CLASS(classname) \
	bool classname::classname##registered__=Factory<string>::instance().Register(string(#classname),&classname::__create##classname);
//不要随意更改以上宏！！！！


#endif