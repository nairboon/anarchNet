#!/bin/sh

glog() {
wget http://google-glog.googlecode.com/files/glog-0.3.1.tar.gz
tar -xzf glog-0.3.1.tar.gz
cd glog-0.3.1
sh configure
make
sudo make install
cd ..
}

gtest() {
wget http://googletest.googlecode.com/files/gtest-1.5.0.tar.gz
tar -xzf gtest-1.5.0.tar.gz
cd gtest-1.5.0
sh configure
make
sudo make install
cd ..
}

protobuf() {
wget http://protobuf.googlecode.com/files/protobuf-2.3.0.tar.gz
tar -xzf protobuf-2.3.0.tar.gz
cd protobuf-2.3.0
sh configure
make
sudo make install
cd ..
}

boost() {
wget http://downloads.sourceforge.net/project/boost/boost/1.43.0/boost_1_43_0.tar.gz?use_mirror=puzzle
tar -xzf boost_1_43_0.tar.gz
cd boost_1_43_0
sh bootstrap --without-libraries=graph,python,mpi,graph_parallel,wave
./bjam
./bjam install
cd ..
}

maidsafe() {
svn checkout http://maidsafe-dht.googlecode.com/svn/trunk/ maidsafe-dht-read-only
cd maidsafe-dht-read-only
cmake .
make
sudo make install
cd ..
}

glog
gtest
protobuf
boost

ldconfig

maidsafe
