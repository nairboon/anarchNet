#include <gtest/gtest.h>
#include "daemon.h"
#include "logger.h"
#include "module_manager.h"
#include "config_manager.h"
#include "crypto.h"
#include "../modules/localstorage/blockstore/lru_cache.h"

using namespace an;


TEST(DaemonTest,initDaemon) 
{
	an::anDaemon *_daemon = new an::anDaemon();
	LOG(INFO) << "initializing daemon";
	
	if (!_daemon->init(".")) {
		LOG(FATAL) << "init failed";
		exit(EXIT_FAILURE);
	}
}


TEST(DaemonTest,db_snapshot)
{
	std::string content = "ABCDEFGHsnapshot";
	std::string based = "ABC";
	db::SnapshotPtr ss(new db::Snapshot(based,content));
	db::SnapshotPtr nss(new db::Snapshot());
	
	ASSERT_TRUE(ModuleManager::instance().db_store_snapshot(ss));
	ASSERT_TRUE(ModuleManager::instance().db_get_snapshot(ss->id,nss));
	
	ASSERT_EQ(ss->id,nss->id);
	ASSERT_EQ(ss->content,nss->content);
	ASSERT_EQ(ss->based,nss->based);
	ASSERT_EQ(ss->time,nss->time);
	
	std::string newcontent = "abcdefg";
	nss->content = newcontent;
	ASSERT_TRUE(ModuleManager::instance().db_store_snapshot(nss));
	ASSERT_TRUE(ModuleManager::instance().db_get_snapshot(nss->id,ss));
	ASSERT_EQ(newcontent,ss->content);


	ASSERT_TRUE(ModuleManager::instance().db_remove(ss->id));
	ASSERT_FALSE(ModuleManager::instance().db_get_snapshot(ss->id,nss));
}

TEST(DaemonTest,db_diff)
{
	std::string content = "ABCDEFGHdiff";
	std::string sid = "ABCDEFGHdiff";
	db::DiffPtr diff(new db::Diff(sid,content));
	db::DiffPtr ndiff(new db::Diff());

	ASSERT_TRUE(ModuleManager::instance().db_store_diff(diff));
	ASSERT_TRUE(ModuleManager::instance().db_get_diff(diff->id,ndiff));

	ASSERT_EQ(diff->id,ndiff->id);
	ASSERT_EQ(diff->snapshot,ndiff->snapshot);
	ASSERT_EQ(diff->content,ndiff->content);
	ASSERT_EQ(diff->time,ndiff->time);
	ASSERT_TRUE(ModuleManager::instance().db_remove(diff->id));
	ASSERT_FALSE(ModuleManager::instance().db_get_diff(diff->id,ndiff));
}

TEST(DaemonTest,db_obj)
{
	std::string content = "ABCDEFGHobj";
	db::ObjPtr obj(new db::Object());
	obj->create(content);
	db::ObjPtr nobj(new db::Object());
	
	ASSERT_TRUE(ModuleManager::instance().db_get_obj(obj->id,nobj));
	
	ASSERT_EQ(obj->id,nobj->id);
	ASSERT_EQ(obj->snapshots.size(),nobj->snapshots.size());
	ASSERT_EQ(nobj->snapshots[0]->content,content);
	ASSERT_TRUE(ModuleManager::instance().db_remove(obj->id));
	ASSERT_FALSE(ModuleManager::instance().db_get_obj(obj->id,nobj));
}

TEST(LocalStore,FileStore)
{
	std::string filehash = "a02a9f00615b9a9b2564ddff8bcad1f0ef9e0b9efc40e300f62b013f63e6dc6327428cf38b9b6f9769eb4da8075b7bfc39e940fc8aad35daba746121d977ee6c";
	std::string filepath = "a0/2a/9f00615b9a9b2564ddff8bcad1f0ef9e0b9efc40e300f62b013f63e6dc6327428cf38b9b6f9769eb4da8075b7bfc39e940fc8aad35daba746121d977ee6c";
	std::string res;
	ASSERT_TRUE(ModuleManager::instance().store_file("TESTFILE",res));
	ASSERT_EQ(res,filehash);
	ASSERT_TRUE(ModuleManager::instance().get_file_path(filehash,res));
	ASSERT_EQ(res,an::ConfigManager::instance().datadir() + "/filestore/"+filepath);
	ASSERT_TRUE(ModuleManager::instance().remove_file(filehash));
}

TEST(LocalStore,HT)
{
	std::string key = "mykey";
	std::string value = "myvalue";
	std::string res;
	ASSERT_TRUE(ModuleManager::instance().kv_put(key,value));
	ASSERT_TRUE(ModuleManager::instance().kv_get(key,res));
	ASSERT_EQ(res,value);
	ASSERT_TRUE(ModuleManager::instance().kv_remove(key));
	ASSERT_FALSE(ModuleManager::instance().kv_get(key,res));
}

 size_t count_evaluations=0;
// Dummy function we want to cache 
std::string fn(const std::string& s)
{
  ++count_evaluations;
  std::string r;
  std::copy(s.rbegin(),s.rend(),std::back_inserter(r));
  return r;
}


typedef lru_cache_using_boost_set<std::string,std::string>::type  cache_type;

TEST(Blockstore, lru_cache)
{
cache_type lru(fn,5); 
 
  // Some initial accesses to prime state 
  ASSERT_EQ(lru("first"),"tsrif"); 
  ASSERT_EQ(lru("second"),"dnoces"); 
  ASSERT_EQ(lru("third"),"driht"); 
  ASSERT_EQ(lru("fourth"),"htruof"); 
  ASSERT_EQ(lru("fifth"),"htfif"); 
  ASSERT_EQ(count_evaluations,5); 
  ASSERT_EQ(lru("sixth"),"htxis"); 
  ASSERT_EQ(count_evaluations,6); 
 
  // This should be retrieved from cache 
  ASSERT_EQ(lru("second"),"dnoces"); 
  ASSERT_EQ(count_evaluations,6); 
 
  // This will have been evicted 
  ASSERT_EQ(lru("first"),"tsrif"); 
  ASSERT_EQ(count_evaluations,7); 
 
  // So check fourth is retrieved 
  ASSERT_EQ(lru("fourth"),"htruof"); 
  ASSERT_EQ(count_evaluations,7); 
 
  // That will have moved up "fourth" to the head 
  // so this will evict fifth 
  ASSERT_EQ(lru("seventh"),"htneves"); 
  ASSERT_EQ(count_evaluations,8); 
 
  // Check fifth was evicted as expected 
  ASSERT_EQ(lru("fifth"),"htfif"); 
  ASSERT_EQ(count_evaluations,9); 
 
}