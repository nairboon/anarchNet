#include <gtest/gtest.h>
#include <glog/logging.h>

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
	
  // setting output to be stderr
  bool FLAGS_logtostderr;
	std::string FLAGS_log_dir;
	
	FLAGS_logtostderr = true;
	FLAGS_log_dir=".";
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
