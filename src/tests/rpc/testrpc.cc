

#include <gtest/gtest.h>
#include "rpc_manager.h"
/*#include "library/client.h"
#include "library/info.h"
#include "daemon/version.h" */
using namespace an;


TEST(rpc,request)
{
  std::string req = "{jsonrpc:\"2.0\" }";
  boost::json::Value v(req);
  rpc::RPC_Request(v);
  
}
