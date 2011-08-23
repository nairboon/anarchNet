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
TEST(DaemonTest,db_store_snapshot)
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

	ASSERT_TRUE(ModuleManager::instance().db_remove(ss->id));
	ASSERT_FALSE(ModuleManager::instance().db_get_snapshot(ss->id,nss));
}

TEST(DaemonTest,db_store_diff)
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

