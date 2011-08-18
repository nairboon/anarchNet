#include <gtest/gtest.h>
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
