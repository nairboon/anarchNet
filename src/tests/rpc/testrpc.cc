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
  ASSERT_FALSE(r.valid(rules));

  rules2["testtype"] = boost::json::str_type;
  ASSERT_TRUE(r.valid(rules2));

  rpc::RPC_Response res = r.createResponse();
}
