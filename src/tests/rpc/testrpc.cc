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
  std::string json = "{\"jsonrpc\": \"2.0\",  \"params\": { \"testtype\": \"abc\"}, \"id\": 1}";
  boost::json::read_or_throw(json,v);
  
  //LOG(INFO) << boost::json::write(v);
  rpc::RPC_Request r(v);
  rpc::RPC_Request::Parameters rules;

  rules["testtype"] = boost::json::str_type;
  // complete rule set but invalid rpc request
  ASSERT_TRUE(r.valid(rules));
 
  rpc::RPC_Response res = r.createResponse();
  ASSERT_EQ(res.json()["id"].get_int(),1);
 }


TEST(util,base64)
{
  std::string con = "ABCDEF01234,.-¨?$!*#{}[";
  
  boost::json::Value v;
  std::string json = "{\"jsonrpc\": \"2.0\",  \"params\": { \"content\": \""+ con +"\"}, \"id\": 1}";
  boost::json::read_or_throw(json,v);
  
 rpc::RPC_Request req(v); 
 rpc::RPC_Response res = req.createResponse();
 std::string b64 = res.encode_base64(con);
 LOG(INFO) << "b64: " << b64;
   req.params()["content"] = b64;

 std::string ress = req.decode_base64("content");
 
 ASSERT_EQ(con,ress);
  
}