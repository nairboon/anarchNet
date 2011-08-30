see doc/API.txt, TODO.txt

== How to build == 
cd build/<your os>/<Debug|Release>
cmake ../../..

Dependencies:
 * boost >= 1.47
 * litesql: https://sourceforge.net/apps/trac/litesql/
     * with this patch: https://sourceforge.net/apps/trac/litesql/ticket/43
 * cryptopp >= 5.6
 * boost.log

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
