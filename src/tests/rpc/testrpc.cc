

#include <gtest/gtest.h>
#include <boost/json/value.hpp>
#include "rpc_server.h"
#include "rpc_manager.h"
/*#include "library/client.h"
#include "library/info.h"
#include "daemon/version.h" */
using namespace an;


TEST(rpc,request)
{
  std::string req = "{jsonrpc:\"2.0\" }";
  boost::json::Value v(req);
  rpc::RPC_Request r(v);
  rpc::RPC_Request::Parameters rules;
  rules["test"] = boost::json::str_type;
  ASSERT_FALSE(r.valid(rules) );
}
