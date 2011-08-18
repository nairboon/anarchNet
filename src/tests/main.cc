#include <gtest/gtest.h>
#include "logger.h"

int main(int argc, char **argv) {
	
	an::Logger::instance().init("TESTbase.log");
	
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
