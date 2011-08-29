see doc/API.txt, TODO.txt

== How to build == 
cd build/<your os>/<Debug|Release>
cmake ../../..

Dependencies:
 * google-glog
 * google-gtest
 * google-protobuf >= 2.3
 * boost >= 1.42
 * maidsafe_dht	! requires gcc >= 4.3

on Mac:
 sudo fink install libcryptopp

cd third-party
sh install-dependencies.sh

This will install all the dependencies


Structure:
	*an			anarchnet packages
		*core 		plugins
		*universe	web*extensions
	*browser		browser app
		*http		http layer
	*daemon			daemon app
		*protocol	protobuf files
	*library		library interface
	*third-party		ext. software, dependencies
