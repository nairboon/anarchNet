#include <gtest/gtest.h>
#include <boost/json/value.hpp>
#include "rpc_server.h"
#include "rpc_manager.h"

using namespace an;
using namespace boost;
using namespace std;

TEST(rpc,json)
{
  boost::json::Value v;
  boost::json::Object o;
  o.push_back(json::Pair("a","b"));
  v = o;

  ASSERT_TRUE(v.contains("a"));
  ASSERT_EQ(v.type(), json::obj_type);
  ASSERT_EQ(v["a"].type(), json::str_type);
  ASSERT_EQ(v["a"].get_str(),"b");

  std::string json = boost::json::write(v);
  std::string rjson = "{\"a\":\"b\"}";


  ASSERT_EQ(json,rjson);
  boost::json::Value v2,v3;

  ASSERT_TRUE(boost::json::read(json,v2));
}

TEST(rpc,request)
{
  boost::json::Value v;
  boost::json::Object o;
  o.push_back(json::Pair("testtype","abc"));
  v = o;

  rpc::RPC_Request r(v);
  rpc::RPC_Request::Parameters rules, rules2;
  rules["test"] = boost::json::str_type;
  // incomplete rule set
  ASSERT_FALSE(r.valid(rules));

  rules2["testtype"] = boost::json::str_type;
  // complete rule set but invalid rpc request
  ASSERT_FALSE(r.valid(rules2));
  //o.push_back(json::Pair("id","1"));
  o.push_back(json::Pair("jsonrpc","2.0"));
  v=o;
  boost::json::Config::add(v.get_obj(),"id",1);
  rpc::RPC_Request r2(v);
  // valid rpc
  ASSERT_TRUE(r2.valid(rules2));
  rpc::RPC_Response res = r2.createResponse();
  ASSERT_EQ(res.json()["id"].get_int(),1);
  boost::json::Config::add(res.json().get_obj(),"random","string");
  ASSERT_EQ(res.json()["random"].get_str(),"string");
}
