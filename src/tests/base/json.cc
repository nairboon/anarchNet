#include <gtest/gtest.h>
#include "module_manager.h"
#include "crypto.h"

TEST(crypto,sha512)
{
	ASSERT_EQ(an::crypto::sha512(""),"cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
	ASSERT_EQ(an::crypto::sha512("anarchNet"),"2a8a0a3a7ddfbe9bf92113725eece0c5d9b450e1907eb496717a67bc755dc8798672c82b49ef448dba022c9f3db3d96e4b20ed0e20e62187bfc8f6a8eda8a8c4");
}
/*#include "pb_json.h"
#include "rpc_service.pb.h"

TEST(json,write)
{
	InfoResponse ir;
	Extension *e1 = ir.add_extensions();
	
	ir.set_anversion(5);
	ir.set_uptime(23);
	e1->set_id("ABCDERD");
	e1->set_extension_version(100);
	
	EXPECT_EQ( an::JSONInterface::write(ir), "{\"anVersion\": 5,\"uptime\": 23,\"connectedNodes\": 0,\"operatingsystem\": \"osBSD\",\"extensions\": [{\"id\": \"ABCDERD\",\"algorithm_version\": 0,\"extension_version\": 100,\"description\": \"\"}]}");

}*/
