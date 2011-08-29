#include <gtest/gtest.h>
#include "daemon.h"
#include "module_manager.h"
#include "crypto.h"
#include "logger.h"

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
	std::string content = "ABCDEFGH";
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
	std::string content = "ABCDEFGH";
	std::string sid = "ABCDEFGH";
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
	std::string content = "ABCDEFGH";
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
	std::string filehash = "2a8a0a3a7ddfbe9bf92113725eece0c5d9b450e1907eb496717a67bc755dc8798672c82b49ef448dba022c9f3db3d96e4b20ed0e20e62187bfc8f6a8eda8a8c4";
	std::string filepath = "2a8a0a3a7ddfbe9bf92113725eece0c5d9b450e1907eb496717a67bc755dc8798672c82b49ef448dba022c9f3db3d96e4b20ed0e20e62187bfc8f6a8eda8a8c4";
	std::string res;
	ASSERT_TRUE(ModuleManager::instance().store_file("TESTFILE",res));
	ASSERT_EQ(res,filehash);
	ASSERT_TRUE(ModuleManager::instance().get_file_path(filehash,res));
	ASSERT_EQ(res,filepath);
	ASSERT_TRUE(ModuleManager::instance().remove_file(filehash));
}