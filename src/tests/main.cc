#include <gtest/gtest.h>
#include "logger.h"
#include "daemon.h"


int main(int argc, char **argv) {
	
	//an::log::Logger::instance().init("TESTbase.log");
	
  testing::InitGoogleTest(&argc, argv);
  

  an::anDaemon *_daemon = new an::anDaemon();
  LOG(INFO) << "initializing daemon";
  
  if (!_daemon->init(".")) {
    LOG(FATAL) << "init failed";
    exit(EXIT_FAILURE);
  }

  int state =  RUN_ALL_TESTS();
  _daemon->stop();
  return state;
}
