/********************************************************************
  �ļ�:   d:\MyProjects\FEPeerSim\FEPeerSim\Factory.h 
  ����: 2006/11/01
  ����: �����
  
  ˵��: �����������������ɶ���
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
		OutputMessage("Factory registe��%d", id);
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

//��ȫ���,��Factory������һЩ���͵Ķ���,�û����ܵ���new delet������
//�����Щ���͵ĵļ̳��࣬��Ҫ��������дREGIST_IN_CLASS��
//���������� Node* node=(Node*)new_class("Node");
//��Ҫ����������º꣡������
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
//��Ҫ����������Ϻ꣡������


#endif