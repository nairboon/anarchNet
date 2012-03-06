#include <gtest/gtest.h>
#include "daemon.h"
#include "logger.h"
#include "module_manager.h"
#include "config_manager.h"
#include "db_manager.h"
#include "crypto.h"
#include "diff_match_patch.h"
#include "../modules/localstorage/blockstore/lru_cache.h"
#include "../modules/localstorage/blockstore/blockstore.h"

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

TEST(Lib, diff_match_patch)
{
  std::string text1 = "ABCEDFGahahahah";
  std::string text2 = "ABxEDFGahaxahyh";
  
  diff_match_patch<std::string> dmp;
  
  /* dmp::Diff::Diffs d = dmp.diff_main(text1,text2);
  std::string delta = dmp.diff_toDelta(dmp.diff_cleanupEfficiency(d));
  
  dmp.Patch::Patches p = dmp.patch_fromText(delta);
  
  std::string res = dmp.patch_apply(text1,p);*/
  
  std::string strPatch = dmp.patch_toText(dmp.patch_make(text1, text2));
  std::pair<std::string, std::vector<bool> > out
  = dmp.patch_apply(dmp.patch_fromText(strPatch), text1);
  std::string res = out.first;
  
  EXPECT_EQ(text2,res);
  
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
  std::string content = "ABCDEFGHobj\nabc";
  std::string content2 = "ABCDXFGHobj\nabc";
  db::ObjPtr obj(new db::Object());
  obj->create(content);
  db::ObjPtr nobj(new db::Object());
  
  ASSERT_TRUE(ModuleManager::instance().db_get_obj(obj->id,nobj));
  
  ASSERT_EQ(obj->id,nobj->id);
  ASSERT_EQ(obj->snapshots.size(),nobj->snapshots.size());
  ASSERT_EQ(nobj->snapshots[0]->content,content);
  ASSERT_EQ(nobj->get(),obj->get());
  
  ASSERT_TRUE(DBManager::instance().update_object(obj,content2));
  ASSERT_EQ(obj->get(),content2);
  ASSERT_TRUE(ModuleManager::instance().db_get_obj(obj->id,nobj));
  ASSERT_EQ(nobj->get(),content2);
  
  ASSERT_TRUE(ModuleManager::instance().db_remove(obj->id));
  ASSERT_FALSE(ModuleManager::instance().db_get_obj(obj->id,nobj));
}

TEST(Object, add_del) {
  std::string content = "abcdefg";
  std::string id = "XXXXX";
   db::ObjPtr obj(new db::Object());
   db::ObjPtr obj2(new db::Object());

  ASSERT_TRUE(obj->create(content,id));
  ASSERT_TRUE(DBManager::instance().get_object_head (id,obj2));
  ASSERT_EQ(obj2->id,id);
  ASSERT_EQ(obj2->head,content);
  ASSERT_TRUE(DBManager::instance().delete_object(id));
}

TEST(Object, update) {
  
}



TEST(LocalStore,BlockStore_file_store_small)
{
  std::string filehash = "a02a9f00615b9a9b2564ddff8bcad1f0ef9e0b9efc40e300f62b013f63e6dc6327428cf38b9b6f9769eb4da8075b7bfc39e940fc8aad35daba746121d977ee6c";
  std::string filepath = "a0/2a/9f00615b9a9b2564ddff8bcad1f0ef9e0b9efc40e300f62b013f63e6dc6327428cf38b9b6f9769eb4da8075b7bfc39e940fc8aad35daba746121d977ee6c";
  std::string res;
  
  Blockstore* bs;
  bs = static_cast<Blockstore*>(ModuleManager::instance().get_ls_plugin("blockstore"));
  assert(bs);
  ASSERT_TRUE(bs->store_file("TESTFILE",res));
  ASSERT_EQ(res,filehash);
  ASSERT_TRUE(bs->get_stored_file_path(filehash,res));
  ASSERT_EQ(res,an::ConfigManager::instance().datadir() + "/blockstore/"+filepath);
  
}

TEST(LocalStore,BlockStore_file_store_big)
{
  std::string filehash = "81ec274c906247df385cf276b80dc618bf5d2a3175f4fdde92049b47da9a89a9e22d5e457d69d286fa4096fae74d44309987b9dc8c678be924044837c8e5e0db";
  std::string filepath = "81/ec/274c906247df385cf276b80dc618bf5d2a3175f4fdde92049b47da9a89a9e22d5e457d69d286fa4096fae74d44309987b9dc8c678be924044837c8e5e0db";
  std::string res;
  
  Blockstore* bs;
  bs = static_cast<Blockstore*>(ModuleManager::instance().get_ls_plugin("blockstore"));
  ASSERT_TRUE(bs->store_file("testfile.jpg",res));
  ASSERT_EQ(res,filehash);
  ASSERT_TRUE(bs->get_stored_file_path(filehash,res));
  ASSERT_EQ(res,an::ConfigManager::instance().datadir() + "/blockstore/"+filepath);
}

TEST(LocalStore,BlockStore_file_get_small)
{
  Blockstore* bs;
  bs = static_cast<Blockstore*>(ModuleManager::instance().get_ls_plugin("blockstore"));
  
  std::string hash = an::crypto::toHex(an::crypto::HashFile("TESTFILE"));
  std::string rpath;
  ASSERT_TRUE(bs->get_file(hash,rpath));
  std::string hash2 = an::crypto::toHex(an::crypto::HashFile(rpath));
  ASSERT_EQ(hash,hash2);
  
  ASSERT_TRUE(bs->remove_file(hash));
}

TEST(LocalStore,BlockStore_file_get_big)
{
  Blockstore* bs;
  bs = static_cast<Blockstore*>(ModuleManager::instance().get_ls_plugin("blockstore"));
  
  std::string hash = an::crypto::toHex(an::crypto::HashFile("testfile.jpg"));
  std::string rpath;
  ASSERT_TRUE(bs->get_file(hash,rpath));
  std::string hash2 = an::crypto::toHex(an::crypto::HashFile(rpath));
  ASSERT_EQ(hash,hash2);
  
  ASSERT_TRUE(bs->remove_file(hash));
}

TEST(LocalStore,BlockStore_blockstore)
{
  Blockstore* bs;
  bs = static_cast<Blockstore*>(ModuleManager::instance().get_ls_plugin("blockstore"));
  
  std::string content = "ABCDEFG12346";
  std::string hash = an::crypto::toHex(an::crypto::Hash(content));
  std::string res;
  an::KV_ResPtr kvres;
  ASSERT_FALSE(bs->kv_get(hash,kvres));
  ASSERT_TRUE(bs->store_block(content));
  ASSERT_TRUE(bs->kv_get_unique(hash,res));
  ASSERT_EQ(content,res);
  ASSERT_TRUE(bs->kv_remove(hash));
}

TEST(LocalStore,HT)
{
  Blockstore* bs;
  bs = static_cast<Blockstore*>(ModuleManager::instance().get_ls_plugin("blockstore"));
  
  std::string key = "mykey";
  std::string value = "myvalue";
  std::string res;
  an::KV_ResPtr kres;
  ASSERT_TRUE(bs->kv_put(key,value));
  ASSERT_TRUE(bs->kv_get(key,kres));
  ASSERT_EQ((*kres)[an::crypto::toHex(an::crypto::Hash(value))],value);
  ASSERT_TRUE(bs->kv_remove(key));
  ASSERT_FALSE(bs->kv_get(key,kres));
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