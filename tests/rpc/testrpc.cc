

#include <gtest/gtest.h>
#include "library/client.h"
#include "library/info.h"
#include "daemon/version.h"

TEST(rpc,fgetInfo)
{
	InfoResponse* r = getInfo();
	EXPECT_EQ(r->anversion(),ANARCHNET_VERSION);
	EXPECT_EQ(r->anprotocolversion(),ANARCHNET_PROTOCOL_VERSION);

	delete r;
}
